#!/usr/bin/env python3
"""Build the PC port: rewrite the sources' inline asm
(tools/port/rewrite.py), compile every game TU with host clang++
for x86-64, compile ports/pc/ and link build/port/splashdown.

The MSL runtime (src/tu/msl/) and zlib 1.0.4 (src/tu/zlib/) are left out:
the host's C++ runtime and zlib (-lz) replace them.

usage: tools/port/build.py [-j N] [--keep-going] [--only NAME ...] [--no-link] [--asan]

--asan builds build/port-asan/splashdown with AddressSanitizer, to find
overflows (its heap is above 4 GB, so a truncated heap pointer crashes there).
"""
import argparse
import concurrent.futures
import glob
import os
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
OUT = os.path.join(ROOT, "build/port")
TREE = os.path.join(ROOT, "build/port/tree")

CXX = os.environ.get("CXX", "clang++")
# The game code: C++98 as mwcc took it, warnings off (they are the
# matching build's business), the PC prelude first.
GAME_FLAGS = [
    "-std=gnu++98", "-m64", "-fno-pie", "-g", "-O1", "-w", "-fms-extensions", "-fms-compatibility",
    "-Wno-address-of-temporary", "-Wno-error=return-mismatch", "-fno-exceptions", "-fno-strict-aliasing", "-fwrapv", "-fno-delete-null-pointer-checks",
    # Retail functions that fall off the end without a return value (mwcc
    # leaves $v0 as is): without this clang takes those paths for unreachable
    # and drops the branch (PSXMediaControl::Stop ran its body when stopped).
    "-fno-strict-return",
    "-include", os.path.join(ROOT, "ports/pc/include/port_prelude.h"),
    "-I" + os.path.join(TREE, "src"), "-I" + os.path.join(TREE, "include"),
    "-I" + os.path.join(ROOT, "ports/pc/include"),
]
PORT_FLAGS = [
    "-std=c++20", "-m64", "-fno-pie", "-g", "-O2", "-Wall",
    "-I" + os.path.join(ROOT, "ports/pc/include"), "-I" + os.path.join(ROOT, "ports/pc"),
]
# The VU microcode upload packets (tools/port/vu.py gen), assembled.
VU_PACKETS = os.path.join(OUT, "gen/vupackets.s")
LIBS = ["-lSDL3", "-lGL", "-lz", "-lm"]


def ffmpeg_flags():
    """libavcodec/libavutil (movies, ports/pc/sdk/mpeg.cpp) from pkg-config:
    (cflags, libs), or None if they aren't installed (PORT_NO_FFMPEG=1 forces
    that): the port is then built with movies stubbed (two black frames)."""
    if os.environ.get("PORT_NO_FFMPEG", "0") != "0":
        return None
    try:
        p = subprocess.run(["pkg-config", "--cflags", "--libs", "libavcodec", "libavutil"],
                           capture_output=True, text=True)
    except FileNotFoundError:
        return None
    if p.returncode:
        return None
    cflags = subprocess.run(["pkg-config", "--cflags", "libavcodec", "libavutil"],
                            capture_output=True, text=True).stdout.split()
    libs = subprocess.run(["pkg-config", "--libs", "libavcodec", "libavutil"],
                          capture_output=True, text=True).stdout.split()
    return cflags, libs


def game_sources():
    srcs = sorted(glob.glob(os.path.join(ROOT, "src/tu/*.cpp")))
    return srcs


def port_sources():
    # ports/pc/game/ holds game-side code (C versions of asm, template
    # instantiations): it is built like the game TUs.
    srcs = sorted(glob.glob(os.path.join(ROOT, "ports/pc/**/*.cpp"), recursive=True))
    return [s for s in srcs if "/ports/pc/game/" not in s]


def port_game_sources():
    return sorted(glob.glob(os.path.join(ROOT, "ports/pc/game/*.cpp")))


def compile_one(src, flags, tree_src):
    rel = os.path.relpath(src, ROOT)
    obj = os.path.join(OUT, "obj", rel.replace("build/port/", "") + ".o")
    os.makedirs(os.path.dirname(obj), exist_ok=True)
    inp = os.path.join(TREE, rel) if tree_src else src
    if rel == "src/tu/PSXMain.cpp":
        # The game's main runs on ports/pc/main.cpp's thread.
        flags = flags + ["-Dmain=splashdown_main"]
    deps = [inp]
    # the command line, so a change of flags rebuilds
    stamp = obj + ".cmd"
    cmdline = " ".join([CXX] + flags)
    same_flags = os.path.exists(stamp) and open(stamp).read() == cmdline
    if same_flags and os.path.exists(obj) and all(os.path.getmtime(obj) >= os.path.getmtime(d) for d in deps) \
            and not os.environ.get("PORT_FORCE"):
        dep = obj + ".d"
        if src.endswith(".s") or (os.path.exists(dep) and dep_fresh(dep, obj)):
            return rel, obj, 0, ""
    if src.endswith(".s"):
        cmd = ["nice", "-n", "10", CXX] + flags + ["-c", inp, "-o", obj]
    else:
        cmd = ["nice", "-n", "10", CXX] + flags + ["-MMD", "-MF", obj + ".d", "-c", inp, "-o", obj]
    p = subprocess.run(cmd, capture_output=True, text=True)
    if p.returncode == 0:
        with open(stamp, "w") as f:
            f.write(cmdline)
    return rel, obj, p.returncode, p.stderr


def dep_fresh(dep, obj):
    t = os.path.getmtime(obj)
    text = open(dep).read().replace("\\\n", " ")
    for path in text.split(":", 1)[1].split():
        if os.path.exists(path) and os.path.getmtime(path) > t:
            return False
    return True


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("-j", type=int, default=8)
    ap.add_argument("--keep-going", action="store_true")
    ap.add_argument("--only", nargs="*", help="TU file names (e.g. Debug.cpp)")
    ap.add_argument("--no-link", action="store_true")
    ap.add_argument("--asan", action="store_true")
    args = ap.parse_args()
    global OUT
    extra = []
    if args.asan:
        OUT = os.path.join(ROOT, "build/port-asan")
        extra = ["-fsanitize=address", "-fno-omit-frame-pointer"]
        GAME_FLAGS.extend(extra)
        PORT_FLAGS.extend(extra)

    subprocess.run([sys.executable, os.path.join(ROOT, "tools/port/rewrite.py")], check=True)
    subprocess.run([sys.executable, os.path.join(ROOT, "tools/port/vu.py"), "gen", VU_PACKETS], check=True)

    ff = ffmpeg_flags()
    if ff:
        PORT_FLAGS.extend(ff[0])
        LIBS.extend(ff[1])
    else:
        print("warning: libavcodec/libavutil not found by pkg-config (or PORT_NO_FFMPEG=1): "
              "building with movies stubbed (two black frames each); install ffmpeg's "
              "development files for movie playback", file=sys.stderr)
        PORT_FLAGS.append("-DPORT_NO_FFMPEG")

    jobs = [(s, GAME_FLAGS, True) for s in game_sources()]
    jobs += [(s, GAME_FLAGS, False) for s in port_game_sources()]
    # PORT_O1="a.cpp b.cpp": build those ports/pc/ files at -O1 (bisecting)
    o1 = set(os.environ.get("PORT_O1", "").split())
    for src in port_sources():
        flags = [f if f != "-O2" or os.path.basename(src) not in o1 else "-O1" for f in PORT_FLAGS]
        if os.path.basename(src) == "heapguard.cpp":
            # it defines malloc & co: no builtin assumptions about them
            flags = flags + ["-fno-builtin"]
        jobs.append((src, flags, False))
    jobs += [(VU_PACKETS, ["-m64", "-fno-pie"], False)]
    if args.only:
        jobs = [j for j in jobs if os.path.basename(j[0]) in args.only]

    objs, failed = [], []
    with concurrent.futures.ThreadPoolExecutor(args.j) as ex:
        for rel, obj, rc, err in ex.map(lambda j: compile_one(*j), jobs):
            if rc:
                failed.append(rel)
                nerr = err.count("error:")
                print(f"FAIL {rel} ({nerr} errors)")
                if not args.keep_going or args.only:
                    sys.stderr.write(err)
            else:
                objs.append(obj)
    print(f"compiled {len(objs)} of {len(jobs)}")
    if failed:
        return 1
    if args.no_link or args.only:
        return 0
    exe = os.path.join(OUT, "splashdown")
    p = subprocess.run([CXX, "-m64", "-no-pie", "-g", "-pthread", "-o", exe] + extra + objs + LIBS, capture_output=True, text=True)
    if p.returncode:
        sys.stderr.write(p.stderr)
        return 1
    print(f"linked {os.path.relpath(exe, ROOT)}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
