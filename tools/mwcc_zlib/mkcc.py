#!/usr/bin/env python3
"""mkcc.py: build toolchain/mwcps2-2.3-zlib/ from toolchain/mwcps2-2.3-991202/.

The retail zlib objects were built by an mwcc 2.3.x newer than 2.3-991202 and
older than 2.3.3-000906. This script copies
2.3-991202 and ports into its mwccps2.exe the 2.3.3-000906 backend behaviour
that 2.3-991202 lacks and retail zlib shows:

  * label alignment (2.3.3's `alignlabel`, on unless optimising for size):
    a `.p2alignl 3` pcode before every label whose preceding block has loop
    weight >= 8, and after a jump table's `jr`; at layout it becomes `.word 0`
    fill up to an 8-byte boundary;
  * padloop (2.3.3's `padloop`, same default): loops of at most 6
    instructions get nops before the back branch (the EE short-loop erratum).
    INFERRED restriction, not in 2.3.3: only a loop whose back branch targets
    its own block is padded (2.3.3/2.4 accept a target up to 2 blocks away;
    retail's build_tree, gen_bitlen and huft_build show multi-block short
    loops unpadded, while every padded retail loop is single-block);
  * `a < b` compare-and-branch as slt/sltu into a temp + bne (2.3.3), not the
    fused blt/bltu macro (`slt $at`) 2.3 uses;
  * the pcode-flag masks 2.3.3 widened for p2align (flag 0x100000), and the
    delay-slot filler's trailing-directive test.

2.3.3 features not ported (retail zlib does not need them): dividefix, and the
`alignlabel`/`padloop` pragma names (here both follow -O4,s: on unless
optimising for size); 2.3.3's section-alignment bookkeeping for p2align.

The new code lives in a new PE section (.zlib, VA 0x555000); its source is
cave/cave.s, assembled here with GNU as/ld (i386). Every routine in it is a
transcription of the named 2.3.3 routine with addresses mapped to 2.3.
Usage: mkcc.py [--out DIR]
"""
import hashlib, os, shutil, struct, subprocess, sys, tempfile

ROOT = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "../.."))
SRC = os.path.join(ROOT, "toolchain/mwcps2-2.3-991202")
OUT = os.path.join(ROOT, "toolchain/mwcps2-2.3-zlib")
HERE = os.path.dirname(os.path.abspath(__file__))
IN_SHA1 = "b368c01c0d3e306389d5de622a801e6b56f77ba4"
OUT_SHA1 = "8d7d7f617ef3004673cd5640eeac22a18727fde8"         # with binutils 2.47; another assembler may encode differently
CAVE_VA = 0x555000          # new section .zlib (RVA 0x155000)
CAVE_RAW = 0x11F600         # appended at the old end of file


def text_off(va):
    """file offset of a .text VA (section .text: VA 0x401000, raw 0x400)."""
    assert 0x401000 <= va < 0x401000 + 0xE8A00
    return va - 0x401000 + 0x400


def data_off(va):
    """file offset of a .data VA (section .data: VA 0x51b000, raw 0xe9400)."""
    assert 0x51B000 <= va < 0x51B000 + 0x24400
    return va - 0x51B000 + 0xE9400


def assemble():
    with tempfile.TemporaryDirectory() as t:
        o, e = os.path.join(t, "c.o"), os.path.join(t, "c.elf")
        subprocess.run(["as", "--32", "-o", o, os.path.join(HERE, "cave/cave.s")], check=True)
        subprocess.run(["ld", "-m", "elf_i386", "-Ttext=0x%x" % CAVE_VA, "-e", "_start", "-o", e, o], check=True)
        b = os.path.join(t, "c.bin")
        subprocess.run(["objcopy", "-O", "binary", "-j", ".text", e, b], check=True)
        code = open(b, "rb").read()
        syms = {}
        for line in subprocess.run(["nm", e], capture_output=True, text=True, check=True).stdout.splitlines():
            a, _, n = line.split()
            syms[n] = int(a, 16)
    return code, syms


def assemble_at(name, va, syms):
    """assemble cave/<name> as code at VA, resolving cave symbols."""
    with tempfile.TemporaryDirectory() as t:
        o, e, b = (os.path.join(t, x) for x in ("c.o", "c.elf", "c.bin"))
        subprocess.run(["as", "--32", "-o", o, os.path.join(HERE, "cave", name)], check=True)
        defs = sum((["--defsym", "%s=0x%x" % kv] for kv in syms.items() if not kv[0].startswith("_")), [])
        subprocess.run(["ld", "-m", "elf_i386", "-Ttext=0x%x" % va, "-e", "_start", *defs, "-o", e, o], check=True)
        subprocess.run(["objcopy", "-O", "binary", "-j", ".text", e, b], check=True)
        return open(b, "rb").read()


def pad(code, n):
    assert len(code) <= n, (len(code), n)
    return code + b"\x90" * (n - len(code))


def call(at, target, op=0xE8):
    return bytes([op]) + struct.pack("<i", target - (at + 5))


def main():
    out = OUT
    if "--out" in sys.argv:
        out = sys.argv[sys.argv.index("--out") + 1]
    exe = open(os.path.join(SRC, "mwccps2.exe"), "rb").read()
    sha = hashlib.sha1(exe).hexdigest()
    if sha != IN_SHA1:
        sys.exit("input mwccps2.exe SHA-1 %s, expected %s" % (sha, IN_SHA1))
    code, S = assemble()
    b = bytearray(exe)
    pe = struct.unpack_from("<I", b, 0x3C)[0]
    nsec = struct.unpack_from("<H", b, pe + 6)[0]
    shdr = pe + 24 + 0xE0 + 40 * nsec           # first free section header (0x2e0)
    raw = (len(code) + 0x1FF) & ~0x1FF
    vsz = (len(code) + 0xFFF) & ~0xFFF

    patches = [
        # (file offset, old bytes, new bytes, comment)
        (pe + 6, struct.pack("<H", 9), struct.pack("<H", 10),
         "PE: NumberOfSections 9 -> 10 (adds .zlib)"),
        (pe + 24 + 56, struct.pack("<I", 0x155000), struct.pack("<I", 0x155000 + vsz),
         "PE: SizeOfImage grows by the .zlib section"),
        (shdr, bytes(40),
         b".zlib\0\0\0" + struct.pack("<IIIIIIHHI", len(code), CAVE_VA - 0x400000, raw, CAVE_RAW, 0, 0, 0, 0, 0x60000020),
         "PE: section header .zlib (code, RX) at VA 0x555000, file 0x11f600"),
        (CAVE_RAW, b"", code + bytes(raw - len(code)),
         "the .zlib section body: cave/cave.s (ported 2.3.3 routines)"),
        # opcode table: p2alignl
        (text_off(0x490AE3), bytes.fromhex("c300000000"), call(0x490AE3, S["reg_p2alignl"], 0xE9),
         "end of the opcode-table init (0x490ae3 `ret` + padding): tail-jump to reg_p2alignl, "
         "which registers p2alignl/I/flags 0x8100000 as 2.3.3 does at 0x492c96 (2.3.3 opcode 0x45e; here 0x3f8, an unregistered slot)"),
        (data_off(0x537BDC + 4 * (0x3F8 - 0x3B3)), struct.pack("<I", 0x49FD00), struct.pack("<I", 0x4A2039),
         "macro-expansion switch (0x49fae2) entry for opcode 0x3f8: dead handler -> default (no expansion), "
         "as 2.3.3's p2alignl (0x45e) falls outside its switch (0x4a209c)"),
        # label alignment
        (text_off(0x43318D), bytes.fromhex("50e85de5050059"), call(0x43318D, S["label_hook"]) + b"\x90\x90",
         "label codegen (2.3 0x433170, 2.3.3 0x434cf0): call align_emit (2.3.3 0x4957d0) before starting the label's block"),
        (text_off(0x4E332F), bytes.fromhex("8b44241483c40c"), call(0x4E332F, S["switch_hook"]) + b"\x90\x90",
         "jump-table switch codegen (2.3 0x4e332a, 2.3.3 0x4e6a02): align_emit after the `jr`"),
        (text_off(0x4A6F40), bytes.fromhex("56e83aff ffff59".replace(" ", "")), call(0x4A6F40, S["layout_hook"]) + b"\x90\x90",
         "layout (BackEnd_Binary2, 2.3 0x4a6ef0, 2.3.3 0x4a97a0): expand p2align pcodes into .word fill (2.3.3 0x4a9815)"),
        # padloop
        (text_off(0x43410C), call(0x43410C, 0x4A6E20), call(0x43410C, S["padloop_drv"]),
         "backend driver after conditional moves (2.3 0x43410c, 2.3.3 0x435d28): run padloop (2.3.3 0x4a8fc0) first"),
        # unfused `<` compare-and-branch (2.3.3's blt/bltu codegen)
        (text_off(0x494914), bytes.fromhex(
            "807c244c03751c807c242400740eb8d6030000eb218d842000000000b8d2030000eb13807c2424007407b8d4030000eb05"
            "b8d003000066898424940000008d44244c8d4c2428ffb424b000000050510fb78424a000000050e89f36ffff83c410e9c8"
            "0200008d842000000000"),
         pad(assemble_at("at_494914.s", 0x494914, S), 0x6C),
         "compare-and-branch codegen, case `<` (2.3 0x494914, 2.3.3 0x496cc4): blt-immediate helper or "
         "slt/sltu + bne (cave blt_reg_s/u = 2.3.3 0x489290/0x4892e0) instead of the fused blt/bltu macros"),
        (text_off(0x4875E0), bytes.fromhex("b9d4030000eb05b9d00300008d4424045350550fb7c150e8140a000083c410"),
         pad(assemble_at("at_4875e0.s", 0x4875E0, S), 0x1F),
         "blt-immediate helper (2.3 0x487550, 2.3.3 0x489470), constant too big for slti: "
         "blt_reg_u/s (2.3.3 0x489500) instead of the fused bltu/blt macros"),
        (text_off(0x4A77B7), bytes.fromhex("66817f204b04"),
         call(0x4A77B7, S["trailing_directive_cmp"]) + b"\x90",
         "delay-slot filler (2.3 0x4a77b7, 2.3.3 0x4aa1d5): `cmp [edi+0x20],0x44b` (trailing directive?) "
         "-> call trailing_directive_cmp, which also counts p2alignl (2.3.3 tests >= 0x456, p2align included)"),
        # flag 0x100000 (p2align) added to three pcode-flag masks, as in 2.3.3
        (text_off(0x49D879) + 2, struct.pack("<I", 0x4098D), struct.pack("<I", 0x14098D),
         "dead-pcode test (2.3 0x49d870, 2.3.3 0x49fbd0): `and esi,0x4098d` -> 0x14098d, so p2align pcodes are never dead"),
        (text_off(0x4A3ABA) + 2, struct.pack("<I", 0x40D8D), struct.pack("<I", 0x140D8D),
         "pcode-flag mask (2.3 0x4a3aba, 2.3.3 0x4a60ba): `and ebx,0x40d8d` -> 0x140d8d"),
        (text_off(0x4A5517) + 1, struct.pack("<I", 0x108), struct.pack("<I", 0x100108),
         "pcode-flag mask (2.3 0x4a5517, 2.3.3 0x4a82a7): `and eax,0x108` -> 0x100108"),
    ]
    for off, old, new, why in patches:
        cur = bytes(b[off:off + len(old)])
        if cur != old:
            sys.exit("patch at 0x%x (%s): found %s, expected %s" % (off, why, cur.hex(), old.hex()))
        if off == len(b):
            b += new
        else:
            b[off:off + len(new)] = new
    os.makedirs(out, exist_ok=True)
    for f in os.listdir(SRC):
        if f != "mwccps2.exe":
            shutil.copy2(os.path.join(SRC, f), os.path.join(out, f))
    open(os.path.join(out, "mwccps2.exe"), "wb").write(b)
    print("cave %d bytes; symbols %s" % (len(code), " ".join("%s=%x" % kv for kv in sorted(S.items(), key=lambda x: x[1]) if not kv[0].startswith("_"))))
    sha = hashlib.sha1(b).hexdigest()
    print("output %s/mwccps2.exe SHA-1 %s%s" % (out, sha, "" if sha == OUT_SHA1 else "  (expected %s)" % OUT_SHA1))


main()
