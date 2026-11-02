#!/usr/bin/env python3
"""Link the executable from the link order alone.

After `make link` (which builds the objects): generate a linker script
from config/link_order.txt and nothing else, link it to build/relink/ and
check the image's SHA-1 against retail. No address and no per-section list
goes in: each output section names every object of the order once, with
one wildcard for the object's sections of that kind
(`obj(.tu.rodata.* .rodata .gnu.linkonce.rodata.*)`), so the linker lays
them out in the object's own (mwcc's) section order. What the script
states besides the order is retail's linker's rules:
- output sections, each starting 128-aligned: .text (crt0, the TUs, the
  SDK, the later TUs), the VU microcode, its upload packets, .data,
  .rodata, the static initializers' text, .ctor (16-aligned, bounded by
  __static_init/__static_init_end), .vtables, the exception index and
  overlay table, .sdata (_gp 0x7FF0 into it), then .sbss, .bss (crt0's
  first) and the COMMON symbols; .main ends 128-aligned;
- section alignment: the compiler's (build/link/align.txt: link_c records
  it before lowering it for its pinned scripts), at least 16 for
  functions, 8 for data, rodata and bss, 4 for small data (FLOOR); the
  objects are copied to build/relink/obj/ with that alignment set and
  their static initializer's section renamed `.tu.init.N` (found by its
  `__sinit_` symbol);
- the linker keeps the first copy of each `.gnu.linkonce.*` section in the
  link order (inline functions, template members, thunks, inline-bound
  data), and `--gc-sections` drops what nothing references, as retail's
  linker dead-stripped: the other copies' data, the `Stripped*` stand-ins,
  MSL's exception tables and the shim sections (.mwccshim, .mwccshim.text),
  which go to an output section that must end up empty. Roots: crt0, the
  SDK and VU asm, the .ctor entries;
- Sony's linker symbols (_fbss, _end, _stack, _stack_size, _heap_size) and
  the Metrowerks linker's data (the empty exception index, the overlay
  table).
The numbers splat named in the SDK asm (D_FFFF, D_20000010, ...) are
defined from build/link/undefined_syms.txt, only those the objects
reference, and none may lie inside the image. The ELF header and the
file's tail (retail's .symtab and the like) stay retail's bytes
(build/assets/), outside the loaded image.

--shift TEXT,DATA,RODATA,BSS links again with that many bytes (multiples
of 128) inserted at the start of the game's text, .data, .rodata and
.main_bss, and checks that it moved: every symbol by the padding before
it, every word of the image either unchanged or relocated, word pointers
(R_MIPS_32) and calls (R_MIPS_26) at their targets' new addresses, and no
word holding an address without a relocation (tools/reloc_audit.py).

Usage: tools/relink.py [-v] [--shift 0x80,0x80,0x80,0x80]
"""
import argparse
import collections
import concurrent.futures as cf
import hashlib
import os
import re
import shutil
import struct
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, "tools"))
from link_c import Elf, STT_FUNC  # noqa: E402
import reloc_audit  # noqa: E402

LINK = "build/link"
OUT = "build/relink"
ORDER = os.path.join(ROOT, "config", "link_order.txt")
CROSS = os.environ.get("CROSS", "mipsel-linux-gnu-")
RETAIL_SHA1 = "496ba6a5bfe9d7e68ea2edb993b493033ccc9bae"
VRAM = 0x200000
SHF_ALLOC = 2
# The kinds of the C objects' sections, in output order, and the names each
# can carry: link_c's `.tu.<kind>.N` (`.text.N` for code), the compiler's
# own name (sections link_c names nothing: copies' data, stand-ins), and
# `.gnu.linkonce.<kind>.<name>` (copies the linker picks among).
KINDS = ["text", "data", "rodata", "init", "ctor", "vtables", "sdata", "sbss", "bss"]
FLOOR = {"text": 16, "init": 16, "data": 8, "rodata": 8, "bss": 8, "sdata": 4, "sbss": 4}
# Not program code or data: the shim sections and MSL's exception tables
# (retail kept none) must be dropped by --gc-sections; the rest is metadata.
GC_ONLY = [".mwccshim", ".mwccshim.*", ".exceptix", ".exception"]
METADATA = [".comment", ".mwcats", ".reginfo", ".MIPS.abiflags", ".pdr", ".gnu.attributes", ".mdebug*"]


def patterns(kind):
    if kind == "text":
        return ".text.* .text .gnu.linkonce.text.*"
    if kind == "init":
        return ".tu.init.*"
    return f".tu.{kind}.* .{kind} .gnu.linkonce.{kind}.*"


def kind_of(name):
    """A C object's section name -> its kind (None: not program data)."""
    if name == ".text" or name.startswith(".text."):
        return "text"
    m = re.match(r"\.(?:tu|gnu\.linkonce)\.([a-z]+)(\.|$)", name)
    if m:
        return m.group(1)
    return name[1:] if name[1:] in KINDS else None


def read_order():
    units = []
    for line in open(ORDER):
        line = line.split("#")[0].strip()
        if line:
            units.append(tuple(line.split()) if line.startswith("sdk ") else ("src", line))
    return units


def c_object(src):
    """tools/link_c.py's object for a source file (named by its path's MD5)."""
    return f"{LINK}/obj/{hashlib.md5(src.encode()).hexdigest()[:10]}.o"


def prepare(src, align):
    """Copy a TU's object to build/relink/obj/ with each section's alignment
    set and its static initializer's section renamed. Returns (path, the
    kinds it has)."""
    obj = c_object(src)
    if not os.path.exists(os.path.join(ROOT, obj)):
        sys.exit(f"relink: no object for {src} ({obj}): run `make link` first")
    dst = f"{OUT}/obj/" + re.sub(r"[^A-Za-z0-9_]", "_", os.path.splitext(os.path.relpath(src, "src/tu"))[0]) + ".o"
    shutil.copy(os.path.join(ROOT, obj), os.path.join(ROOT, dst))
    elf = Elf(os.path.join(ROOT, obj))
    sinit = {sh for n, v, sz, k, sh, b in elf.symbols() if k == STT_FUNC and n.startswith("__sinit_")}
    renames, aligns, kinds = [], {}, set()
    for i, (name, sec) in enumerate(zip(elf.names, elf.secs)):
        if not sec[2] & SHF_ALLOC or name.startswith(".mwccshim"):
            continue
        kind = kind_of(name)
        if kind is None:
            continue
        own = align.get((obj, name)) or align.get(("*", name)) or sec[8]
        if i in sinit:
            kind, new = "init", f".tu.init.{len(renames) // 2}"
            renames += ["--rename-section", f"{name}={new}"]
            name = new
        # (a name mwcc repeats, a stand-in's `.rodata`, gets the largest)
        aligns[name] = max(aligns.get(name, 1), own, FLOOR.get(kind, 1))
        kinds.add(kind)
    if renames:
        subprocess.run([CROSS + "objcopy", *renames, dst], cwd=ROOT, check=True)
    subprocess.run([CROSS + "objcopy", *sum((["--set-section-alignment", f"{n}={a}"] for n, a in aligns.items()), []),
                    dst], cwd=ROOT, check=True)
    return dst, kinds


def sdk_files(lib, obj):
    """An SDK object's pieces: {kind: (file, section)}."""
    out = {}
    text = f"{LINK}/crt0.s.o" if (lib, obj) == ("crt0", "crt0") else f"build/asm/sdk/{lib}/{obj}.s.o"
    if os.path.exists(os.path.join(ROOT, text)):
        out["text"] = (text, ".text")
    for kind in ("data", "rodata", "bss", "common"):
        f = f"{LINK}/sdkdata_{lib}_{obj}_{kind}.s.o"
        if os.path.exists(os.path.join(ROOT, f)):
            out[kind] = (f, "." + kind)
    return out


def vu_microcode():
    """link_c's one non-empty game-code asm chunk: the VU microcode (.vutext)."""
    found = []
    for name in sorted(os.listdir(os.path.join(ROOT, LINK))):
        if re.match(r"chunk_\d+\.s\.o$", name):
            elf = Elf(os.path.join(ROOT, LINK, name))
            if any(n == ".text" and s[5] for n, s in zip(elf.names, elf.secs)):
                found.append(f"{LINK}/{name}")
    if len(found) != 1:
        sys.exit(f"relink: expected one non-empty asm chunk (the VU microcode), found {found}")
    return found[0]


def script(units, objs, vu, pads=None):
    """The linker script: the link order, a wildcard per object and kind, and
    the layout rules. `pads` ({text, data, rodata, bss: bytes}) inserts room
    for the shift test."""
    pads = pads or {}
    order, by_kind = [], {k: [] for k in KINDS}
    for u in units:
        if u[0] == "src":
            path, kinds = objs[u[1]]
            order.append(path)
            for k in KINDS:
                if k in kinds:
                    line = f"{path}({patterns(k)})"
                    by_kind[k].append(f"KEEP({line})" if k == "ctor" else line)
        else:
            lib, obj = u[1].split("/")
            for k, (f, sec) in sdk_files(lib, obj).items():
                if f not in order:
                    order.append(f)
                if k == "common":
                    continue   # (newlib's errno: taken by `*(.scommon COMMON)` at the end of .bss)
                if k == "text":
                    by_kind[k].append(f"KEEP({f}({sec}))")
                else:
                    by_kind[k].append(f". = ALIGN(8); KEEP({f}({sec}))")

    def block(kind, pad_first=None):
        lines = by_kind[kind]
        if pad_first is not None and pads.get(pad_first):
            lines = lines[:1] + [f". += {pads[pad_first]:#x};"] + lines[1:] if kind == "text" \
                else [f". += {pads[pad_first]:#x};"] + lines
        return "\n".join("        " + l for l in lines)

    return f"""/* Generated by tools/relink.py from config/link_order.txt (no addresses). */
INPUT(
{chr(10).join("    " + p for p in order + [vu, LINK + "/vupackets.s.o"])}
)
ENTRY(_start)
SECTIONS
{{
    .elf_header 0 : AT(0)
    {{
        KEEP(build/assets/elf_header.bin.o(.data))
    }}
    main_ROM_START = SIZEOF(.elf_header);
    main_VRAM = {VRAM:#x};
    .main {VRAM:#x} : AT(main_ROM_START)
    {{
        /* .text: crt0, the TUs, the SDK, the later TUs (--shift pads after crt0) */
{block("text", "text")}
        /* .vutext: the VU microcode */
        . = ALIGN(128);
        KEEP({vu}(.text))
        /* the VU microcode's upload packets */
        . = ALIGN(8);
        KEEP({LINK}/vupackets.s.o(.rodata))
        /* .data */
{block("data", "data")}
        /* .rodata */
        . = ALIGN(128);
{block("rodata", "rodata")}
        /* the static initializers */
        . = ALIGN(128);
{block("init")}
        /* .ctor: the static initializer table */
        . = ALIGN(16);
        __static_init = .;
{block("ctor")}
        __static_init_end = .;
        /* .vtables */
{block("vtables")}
        /* the Metrowerks linker's exception index (empty: retail kept no
           exception tables) and overlay table (one entry, the image) */
        __exception_table_start__ = .;
        __exception_table_end__ = .;
        . = ALIGN(16);
        _overlay_group_addreses = .;
        LONG(main_VRAM);
        __data_end = .;
        /* .sdata */
        . = ALIGN(128);
        _gp = . + 0x7FF0;
{block("sdata")}
        . = ALIGN(128);
    }}
    .main_bss (NOLOAD) :
    {{
{"        . += %#x;" % pads["bss"] if pads.get("bss") else ""}
        main_BSS_START = .;
        /* .sbss */
{block("sbss")}
        /* .bss: crt0's, the TUs', the SDK's, the later TUs', then COMMON */
        . = ALIGN(128);
{block("bss")}
        *(.scommon COMMON)
        main_BSS_END = .;
    }}
    /* Sony's linker symbols */
    _fbss = main_BSS_START;
    _end = ALIGN(main_BSS_END, 128);
    _stack = -1;
    _stack_size = 0x100000;
    _heap_size = -1;
    .elf_tail : AT(main_ROM_START + SIZEOF(.main))
    {{
        KEEP(build/assets/elf_tail.bin.o(.data))
    }}
    /* must be empty after --gc-sections */
    .gc (NOLOAD) :
    {{
        {" ".join(f"*({p})" for p in GC_ONLY)}
    }}
    /DISCARD/ :
    {{
        {" ".join(f"*({p})" for p in METADATA)}
    }}
}}
"""


def numbers(inputs):
    """The numbers splat named that the inputs use (D_FFFF ...): their
    definitions from build/link/undefined_syms.txt. None may be an address
    inside the image."""
    defined, used = set(), set()
    out = subprocess.run([CROSS + "nm", "-A", *inputs], cwd=ROOT, capture_output=True, text=True).stdout
    for line in out.split("\n"):
        m = re.match(r"\S+:\s*(?:[0-9a-f]+)?\s+([A-Za-z]) (\S+)$", line)
        if m:
            (used if m.group(1) == "U" else defined).add(m.group(2))
    values = {}
    for f in (f"{LINK}/undefined_syms.txt", f"{LINK}/undefined_funcs.txt"):
        for line in open(os.path.join(ROOT, f)):
            m = re.match(r"(\w+) = (0x[0-9A-Fa-f]+);", line.strip())
            if m:
                values[m.group(1)] = int(m.group(2), 16)
    return {n: values[n] for n in sorted(used - defined) if n in values}


def link(ld, elf, mapfile, extra=()):
    r = subprocess.run([CROSS + "ld", "-EL", "-T", ld, "-T", f"{OUT}/numbers.ld", "--no-check-sections",
                        "--gc-sections", "--print-gc-sections", "-Map", mapfile, "-o", elf, *extra],
                       cwd=ROOT, capture_output=True, text=True)
    if r.returncode or not os.path.exists(os.path.join(ROOT, elf)):
        sys.exit("relink: link failed\n" + "\n".join(l for l in r.stderr.split("\n") if "removing unused" not in l)[:3000])
    return r.stderr


def elf_parts(path):
    """(sections {name: header}, names, data) of a linked ELF."""
    data = open(os.path.join(ROOT, path), "rb").read()
    secs, names = reloc_audit.elf_sections(data)
    return {n: s for n, s in zip(names, secs)}, data


def symbols(secs, data):
    st, strt = secs[".symtab"], secs[".strtab"]
    out = []
    for p in range(st[4], st[4] + st[5], 16):
        n, value, size, info, other, shndx = struct.unpack_from("<IIIBBH", data, p)
        out.append((data[strt[4] + n:data.index(b"\0", strt[4] + n)].decode("latin-1"), value, shndx))
    return out


def shift_check(base_elf, units, objs, vu, pads):
    """Link with padding and check that everything after each pad moved with it."""
    ld = f"{OUT}/shift.ld"
    open(os.path.join(ROOT, ld), "w").write(script(units, objs, vu, pads))
    elf = f"{OUT}/shift.elf"
    link(ld, elf, f"{OUT}/shift.map", ["--emit-relocs"])
    bsecs, bdata = elf_parts(base_elf)
    ssecs, sdata = elf_parts(elf)
    # Where each pad went, in base addresses: the first section of its block.
    bsyms, ssyms = symbols(bsecs, bdata), symbols(ssecs, sdata)
    if [n for n, v, s in bsyms] != [n for n, v, s in ssyms]:
        sys.exit("relink: the shifted link's symbol tables differ")
    cuts = []
    for kind in ("text", "data", "rodata", "bss"):
        if pads.get(kind):
            # the pad sits before the block's first section: find it from the map
            cuts.append((block_start(f"{OUT}/SLUS_202.23.map", kind), pads[kind]))

    def moved(a):
        return a + sum(p for c, p in cuts if a >= c)
    end = next(v for n, v, sh in bsyms if n == "_end")   # the image's end, 128-aligned
    # (_gp is .sdata's start plus 0x7FF0, not the address of what lies there)
    bad_syms = [(n, v, sv) for (n, v, sh), (_, sv, _) in zip(bsyms, ssyms)
                if VRAM <= v <= end and sh and n != "_gp" and sv != moved(v) and v not in {c for c, p in cuts}]
    # The image, word by word.
    bm, sm = bsecs[".main"], ssecs[".main"]
    gp_b = next(v for n, v, sh in bsyms if n == "_gp")
    gp_s = next(v for n, v, sh in ssyms if n == "_gp")

    def word(data, sec, a):
        return struct.unpack_from("<I", data, sec[4] + a - sec[3])[0]

    def sext(v):
        return (v & 0xFFFF) - ((v & 0x8000) << 1)

    rels = []   # (address, type, symbol), in the linker's order
    r = bsecs[".rel.main"]
    for p in range(r[4], r[4] + r[5], 8):
        off, info = struct.unpack_from("<II", bdata, p)
        rels.append((off if off >= bm[3] else bm[3] + off, info & 0xFF, info >> 8))
    rel = {a: t for a, t, sym in rels}
    counts = collections.Counter()
    bad_words = []
    # %hi/%lo pairs: the address they build must move with its target.
    hi = collections.defaultdict(list)   # symbol -> HI16 addresses waiting for their LO16
    for a, t, sym in rels:
        if t == 5:
            hi[sym].append(a)
        elif t == 6 and hi.get(sym):
            lo_b, lo_s = word(bdata, bm, a), word(sdata, sm, moved(a))
            for h in hi.pop(sym):
                addr_b = (word(bdata, bm, h) << 16) + sext(lo_b) & 0xFFFFFFFF
                addr_s = (word(sdata, sm, moved(h)) << 16) + sext(lo_s) & 0xFFFFFFFF
                counts["%hi/%lo pairs"] += 1
                want = gp_s if bsyms[sym][0] == "_gp" else moved(addr_b) if VRAM <= addr_b <= end else addr_b
                if addr_s != want:
                    bad_words.append((h, addr_b, addr_s, "%hi/%lo"))
    for a in range(bm[3], bm[3] + bm[5], 4):
        w = word(bdata, bm, a)
        sa = moved(a)
        sw = word(sdata, sm, sa)
        t = rel.get(a)
        if t is None:
            ok = sw == w
            counts["unchanged" if ok else "changed without a relocation"] += 1
        elif t == 2:   # R_MIPS_32: a pointer
            ok = sw == (moved(w) if VRAM <= w <= end else w)
            counts["R_MIPS_32"] += 1
        elif t == 4:   # R_MIPS_26: a call or jump
            tgt = (a & 0xF0000000) | (w & 0x3FFFFFF) << 2
            ok = (sa & 0xF0000000) | (sw & 0x3FFFFFF) << 2 == moved(tgt) and sw >> 26 == w >> 26
            counts["R_MIPS_26"] += 1
        elif t == 7:   # R_MIPS_GPREL16: $gp-relative small data
            tgt = gp_b + sext(w)
            ok = gp_s + sext(sw) == moved(tgt) and sw >> 16 == w >> 16
            counts["R_MIPS_GPREL16"] += 1
        else:          # HI16/LO16: checked in pairs above
            ok = sw >> 16 == w >> 16
        if not ok:
            bad_words.append((a, w, sw, t))
    checked = [1 for n, v, sh in bsyms if VRAM <= v <= end and sh and n != "_gp"]
    print(f"shift {', '.join(f'{k} +{p:#x}' for k, p in pads.items() if p)}: links; "
          f"{len(checked) - len(bad_syms)} of {len(checked)} symbols in the image moved with their padding; words: "
          + ", ".join(f"{n} {k}" for k, n in sorted(counts.items())))
    for a, w, sw, t in bad_words[:10]:
        print(f"  {a:#x}: {w:#010x} -> {sw:#010x} (relocation {t})")
    for n, v, sv in bad_syms[:10]:
        print(f"  symbol {n}: {v:#x} -> {sv:#x}, expected {moved(v):#x}")
    return not bad_words and not bad_syms


def block_start(mapfile, kind):
    """The base link's address of the first section the padding of `kind`
    goes before (from the map)."""
    mp = re.sub(r"\n {16,}(0x[0-9a-f]+\s+0x[0-9a-f]+ \S)", r" \1",
                open(os.path.join(ROOT, mapfile)).read())
    mp = mp[mp.index("Linker script and memory map"):]
    first = {"text": r"\.text\.\S+|\.gnu\.linkonce\.text\.\S+", "data": r"\.tu\.data\.\S+",
             "rodata": r"\.tu\.rodata\.\S+", "bss": r"\.tu\.sbss\.\S+|\.gnu\.linkonce\.sbss\.\S+"}[kind]
    m = re.search(rf"^ (?:{first})\s+0x([0-9a-f]+)\s+0x[0-9a-f]+ {OUT}/obj/", mp, re.M)
    return int(m.group(1), 16)


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--shift", help="TEXT,DATA,RODATA,BSS padding (multiples of 128) for the shift test")
    ap.add_argument("-v", action="store_true", help="list the unreferenced sections --gc-sections dropped")
    args = ap.parse_args()
    os.makedirs(os.path.join(ROOT, OUT, "obj"), exist_ok=True)
    for f in os.listdir(os.path.join(ROOT, OUT, "obj")):
        os.remove(os.path.join(ROOT, OUT, "obj", f))
    align = {}
    for line in open(os.path.join(ROOT, LINK, "align.txt")):
        obj, sec, a = line.split()
        align[(obj, sec)] = int(a)
    units = read_order()
    srcs = [u[1] for u in units if u[0] == "src"]
    with cf.ThreadPoolExecutor(8) as ex:
        objs = dict(zip(srcs, ex.map(lambda s: prepare(s, align), srcs)))
    vu = vu_microcode()
    ld = f"{OUT}/SLUS_202.23.ld"
    open(os.path.join(ROOT, ld), "w").write(script(units, objs, vu))
    inputs = re.findall(r"^    (\S+\.o)$", open(os.path.join(ROOT, ld)).read(), re.M)
    nums = numbers(inputs)
    open(os.path.join(ROOT, OUT, "numbers.ld"), "w").write("".join(f"{n} = {v:#x};\n" for n, v in nums.items()))
    elf = f"{OUT}/SLUS_202.23.elf"
    stderr = link(ld, elf, f"{OUT}/SLUS_202.23.map", ["--emit-relocs"])
    removed = re.findall(r"removing unused section '([^']+)' in file '([^']+)'", stderr)
    secs, data = elf_parts(elf)
    end = secs[".main_bss"][3] + secs[".main_bss"][5]
    pinned = [n for n, v in nums.items() if VRAM <= v < end]
    if pinned:
        sys.exit(f"relink: numbers inside the image (addresses): {' '.join(pinned)}")
    gc = secs.get(".gc")
    if gc and gc[5]:
        sys.exit(f"relink: .gc kept {gc[5]:#x} bytes (a shim or exception table something references)")
    mp = open(os.path.join(ROOT, OUT, "SLUS_202.23.map")).read()
    kept = set(re.findall(r"^ (\.gnu\.linkonce\.\S+)", mp, re.M))
    program = [(s, f) for s, f in removed if f.startswith(OUT + "/obj/") and kind_of(s)]
    dups = sum(1 for s, f in program if s in kept)
    unref = [(s, f) for s, f in program if s not in kept]
    shims = sum(1 for s, f in removed if s.startswith(".mwccshim"))
    exc = sum(1 for s, f in removed if s in (".exceptix", ".exception"))
    image = os.path.join(ROOT, OUT, "SLUS_202.23")
    subprocess.run([CROSS + "objcopy", "-O", "binary", "-R", ".gc", elf, image], cwd=ROOT, check=True)
    sha1 = hashlib.sha1(open(image, "rb").read()).hexdigest()
    print(f"relink: {len(srcs)} objects and {len(units) - len(srcs)} SDK objects in link order; "
          f"script {sum(1 for l in open(os.path.join(ROOT, ld)) if '(' in l)} lines; "
          f"--gc-sections dropped {dups} duplicate copies, {shims} shim sections, "
          f"{exc} exception tables and {len(unref)} unreferenced sections")
    if args.v:
        for sec, f in unref:
            print(f"  gc {f} {sec}")
    ok = sha1 == RETAIL_SHA1
    print(f"{OUT}/SLUS_202.23: SHA-1 {'OK' if ok else 'differs from retail (' + sha1 + ')'}")
    if args.shift:
        pads = dict(zip(("text", "data", "rodata", "bss"), (int(x, 0) for x in args.shift.split(","))))
        if any(p % 128 for p in pads.values()):
            sys.exit("relink: --shift paddings must be multiples of 128")
        ok = shift_check(elf, units, objs, vu, pads) and ok
        reloc_audit.audit(os.path.join(ROOT, elf), top=0)   # (27 known coincidences)
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
