#!/usr/bin/env python3
"""Compile C/C++ with mwccps2 and byte-compare each function against retail.

Usage: tools/match.py [--flags "-O4,p"] src/Class/Method.cpp [...]

For every function symbol in the compiled object that also exists in
config/symbol_addrs.txt, the object's .text bytes are compared with the
baserom at that function's address. Fields covered by a relocation
(jal targets, %hi/%lo halves, gp offsets) are masked on both sides, since
the object isn't linked yet; the opcode/register bits must still match.
Exit status is non-zero if any compared function differs.
"""
import argparse
import os
import re
import struct
import subprocess
import sys
import tempfile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
BASEROM = os.path.join(ROOT, "baserom/SLUS_202.23")
SYMBOLS = os.path.join(ROOT, "config/symbol_addrs.txt")
WIBO = os.path.join(ROOT, "toolchain/wibo")
MWCC = os.environ.get("MWCC") or os.path.join(ROOT, "toolchain/mwcps2-2.4-001213/mwccps2.exe")
# A source line `// compiler: mwcps2-3.0-011126` picks toolchain/<id>/mwccps2.exe
# for that file (zlib and MSL weren't built by retail's 2.4 build).
COMPILER_RE = re.compile(r"^// compiler: ([\w.-]+)\s*$", re.M)


def compiler_path(cc_id):
    path = os.path.join(ROOT, "toolchain", cc_id, "mwccps2.exe")
    if not os.path.exists(path):
        sys.exit(f"unknown compiler {cc_id}: no {path}")
    return path
CROSS = os.environ.get("CROSS", "mipsel-linux-gnu-")
DEFAULT_FLAGS = "-O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly"  #
VRAM_TO_FILE = 0x200000 - 0x100  # main LOAD: file 0x100 -> vram 0x200000

# Retail ELF tables (see tools/extract_symbols.py).
STRTAB_OFF = 0x24EB60
SYMTAB_OFF, SYMTAB_COUNT = 0x29EB50, 18162
RELMAIN_OFF, RELMAIN_END = 0x2EE8E8, 0x3921D8
RELOC_TYPES = {2: "R_MIPS_32", 4: "R_MIPS_26", 5: "R_MIPS_HI16", 6: "R_MIPS_LO16",
               7: "R_MIPS_GPREL16"}

# Bits of an instruction word that a relocation of each type rewrites.
RELOC_MASK = {
    "R_MIPS_26": 0x03FFFFFF,
    "R_MIPS_HI16": 0x0000FFFF,
    "R_MIPS_LO16": 0x0000FFFF,
    "R_MIPS_GPREL16": 0x0000FFFF,
    "R_MIPS_32": 0xFFFFFFFF,
}


def load_symbols():
    syms = {}
    for line in open(SYMBOLS):
        m = re.match(r"(\S+) = 0x([0-9A-F]+);.*type:func(?: size:0x([0-9A-F]+))?", line)
        if m:
            syms[m.group(1)] = (int(m.group(2), 16), int(m.group(3) or "0", 16))
    return syms


def load_retail_relocs(rom):
    """{vram: (reloc type, sanitized symbol name, symbol value)} from .relmain."""
    def name_at(off):
        if off >= SYMTAB_OFF - STRTAB_OFF:  # malformed tail entries
            return ""
        start = STRTAB_OFF + off
        return rom[start:rom.index(b"\0", start)].decode("latin-1")
    relocs = {}
    for p in range(RELMAIN_OFF, RELMAIN_END, 8):
        offset, info = struct.unpack_from("<II", rom, p)
        st_name, st_value = struct.unpack_from("<II", rom, SYMTAB_OFF + 16 * (info >> 8))
        relocs[offset] = (RELOC_TYPES.get(info & 0xFF, str(info & 0xFF)),
                          sanitize(name_at(st_name)), st_value)
    return relocs


# .relmain starts at 0x2005F4: retail kept no relocations for the code below
# it (MemPoolReportToFile). There the target is decoded from the retail bytes.
RELMAIN_FIRST = 0x2005F4
RETAIL_GP = 0x456970  # $gp, set in crt0
# The first two static initializers (vu0Functions.c, MemoryPoolManager.cpp:
# TUs whose code sits below RELMAIN_FIRST) have no relocations either.
SINIT_NORELOC = (0x440D80, 0x440E30)


def no_retail_relocs(vaddr):
    return vaddr < RELMAIN_FIRST or SINIT_NORELOC[0] <= vaddr < SINIT_NORELOC[1]


def load_retail_names(rom):
    """{address: [sanitized symbol names]} from the retail .symtab."""
    names = {}
    for k in range(1, SYMTAB_COUNT):
        st_name, value = struct.unpack_from("<II", rom, SYMTAB_OFF + 16 * k)
        if st_name < SYMTAB_OFF - STRTAB_OFF:
            start = STRTAB_OFF + st_name
            names.setdefault(value, []).append(
                sanitize(rom[start:rom.index(b"\0", start)].decode("latin-1")))
    return names


def load_linker_symbols(rom):
    """{name: value} for the linker-defined symbols in the retail .symtab
    (global, no type, size 0: __static_init, __exception_table_start__, _gp,
    ...). Retail references to them carry no relocations: the linker wrote
    their absolute addresses straight into the %hi/%lo pair."""
    out = {}
    for k in range(1, SYMTAB_COUNT):
        st_name, value, size, info = struct.unpack_from("<IIIB", rom, SYMTAB_OFF + 16 * k)
        if info == 0x10 and size == 0 and st_name < SYMTAB_OFF - STRTAB_OFF:
            start = STRTAB_OFF + st_name
            out[rom[start:rom.index(b"\0", start)].decode("latin-1")] = value
    return out


def decode_uncovered(text, size, rom, addr, relocs, retail_names):
    """Stand-in retail relocations, {offset: (type, name, symbol value)}, for
    words below RELMAIN_FIRST. The target is decoded from the retail word
    (a %hi/%lo pair, or $gp + offset) and our addend subtracted, so the name
    is whichever retail symbol sits at that address: our symbol if it is one
    of them, else any (a mismatch), else "" (no symbol there)."""
    def sext16(v):
        v &= 0xFFFF
        return v - 0x10000 if v & 0x8000 else v

    def word(buf, off):
        return struct.unpack_from("<I", buf, off)[0]

    def retail_word(off):
        return word(rom, addr - VRAM_TO_FILE + off)

    out = {}
    offsets = sorted(i for i in relocs if no_retail_relocs(addr + i))
    for i in offsets:
        rtype, rsym = relocs[i]
        if rtype == "R_MIPS_GPREL16":
            target = RETAIL_GP + sext16(retail_word(i))
            addend = sext16(word(text, i))
        elif rtype in ("R_MIPS_HI16", "R_MIPS_LO16"):
            # Pair a LO16 with the nearest preceding HI16 on the same symbol,
            # and a HI16 with the next LO16.
            if rtype == "R_MIPS_HI16":
                pair = [j for j in offsets if j > i and relocs[j] == ("R_MIPS_LO16", rsym)][:1]
                hi, lo = i, pair[0] if pair else None
            else:
                pair = [j for j in offsets if j < i and relocs[j] == ("R_MIPS_HI16", rsym)][-1:]
                hi, lo = pair[0] if pair else None, i
            if hi is None or lo is None:
                continue
            target = ((retail_word(hi) & 0xFFFF) * 0x10000 + sext16(retail_word(lo))) & 0xFFFFFFFF
            addend = (word(text, hi) & 0xFFFF) * 0x10000 + sext16(word(text, lo))
        else:
            continue  # R_MIPS_26 is decoded in compare()
        value = (target - addend) & 0xFFFFFFFF
        cands = retail_names.get(value, [])
        base = sanitize(rsym).split("$")[0]
        name = next((n for n in cands if n.split("$")[0] == base), cands[0] if cands else "")
        out[i] = (rtype, name, value)
    return out


def sanitize(name):
    # Must mirror tools/extract_symbols.py.
    for bad, good in (("<", "_LT_"), (">", "_GT_"), ("@", "_AT_")):
        name = name.replace(bad, good)
    return name


def compile_obj(src, flags, out, src_dir=None, mwcc=None):
    if os.path.exists(out):
        os.remove(out)
    # mwcc doesn't search the including file's directory by default.
    inc = ["-I-", "-i", src_dir or os.path.dirname(src), "-i", os.path.join(ROOT, "src"),
           "-i", os.path.join(ROOT, "include")]
    cmd = [WIBO, mwcc or MWCC, "-c", *flags.split(), "-nostdinc", *inc, "-stderr", src, "-o", out]
    r = subprocess.run(cmd, stdin=subprocess.DEVNULL, capture_output=True, text=True)
    if r.returncode != 0 or not os.path.exists(out):
        sys.exit("compile failed: " + (r.stdout + r.stderr).strip().splitlines()[-1:][0]
                 if (r.stdout + r.stderr).strip() else f"compile failed: {src}")


def obj_text(obj):
    """Parse the object's ELF directly. mwcc emits one .text section per
    function, so offsets and relocations must stay per-section.

    Returns [(name, bytes, {offset: (reloc type, symbol name)}, literals)],
    where literals maps the offset of each relocation against object-local
    data (string/float literals, `@N` symbols or section symbols) to
    (section bytes, offsets of relocations inside that section, symbol
    value, symbol size), so compare() can check the data itself.
    """
    data = open(obj, "rb").read()
    shoff, = struct.unpack_from("<I", data, 0x20)
    shentsize, shnum, shstrndx = struct.unpack_from("<HHH", data, 0x2E)
    secs = [struct.unpack_from("<IIIIIIIIII", data, shoff + i * shentsize)
            for i in range(shnum)]  # name type flags addr offset size link info align entsize

    def cstr(off):
        return data[off:data.index(b"\0", off)].decode("latin-1")

    shstr = secs[shstrndx][4]
    names = [cstr(shstr + s[0]) for s in secs]
    symtab = next(i for i, s in enumerate(secs) if s[1] == 2)  # SHT_SYMTAB
    strtab = secs[secs[symtab][6]][4]
    syms = []
    for k in range(secs[symtab][5] // 16):
        st_name, value, size, info, _other, shndx = struct.unpack_from(
            "<IIIBBH", data, secs[symtab][4] + 16 * k)
        syms.append((cstr(strtab + st_name), value, size, info & 0xF, shndx))

    relocs = {}  # section index -> {offset: (type, symbol)}
    reloc_offs = {}  # section index -> {offsets that carry a relocation}
    lit_syms = {}  # section index -> {offset: symbol tuple}
    for s in secs:
        if s[1] == 9:  # SHT_REL; sh_info = section it applies to
            table = relocs.setdefault(s[7], {})
            for k in range(s[5] // 8):
                offset, info = struct.unpack_from("<II", data, s[4] + 8 * k)
                sym = syms[info >> 8]
                name = sym[0] or names[sym[4]]  # section symbols are unnamed
                table[offset] = (RELOC_TYPES.get(info & 0xFF, str(info & 0xFF)), name)
                reloc_offs.setdefault(s[7], set()).add(offset)
                if (name.startswith((".", "@")) and 0 < sym[4] < len(secs)
                        and secs[sym[4]][1] == 1):  # defined here, PROGBITS
                    lit_syms.setdefault(s[7], {})[offset] = sym

    # Static initializers (__sinit_<File>) go to .init, everything else to .text.
    funcs = []
    for name, value, size, kind, shndx in syms:
        if kind == 2 and 0 < shndx < len(secs) and names[shndx] in (".text", ".init"):
            sec = secs[shndx]
            body = data[sec[4] + value:sec[4] + value + size]
            table = {off - value: r for off, r in relocs.get(shndx, {}).items()
                     if value <= off < value + size}
            lits = {}
            for off, sym in lit_syms.get(shndx, {}).items():
                if value <= off < value + size:
                    tsec = secs[sym[4]]
                    lits[off - value] = (data[tsec[4]:tsec[4] + tsec[5]],
                                         reloc_offs.get(sym[4], set()), sym[1], sym[2])
            funcs.append((name, body, table, lits))
    return funcs


def literal_extent(sec, value, size, off):
    """Bytes of the object-local literal a relocation points at: the whole
    symbol when it has a size, else a C string or one word."""
    end = value + size if size else len(sec)
    if size:
        return off, end
    s = sec[off:sec.find(b"\0", off) if b"\0" in sec[off:] else len(sec)]
    if s and all(32 <= c < 127 or c in (9, 10, 13) for c in s):
        return off, off + len(s) + 1
    return off, min(off + 4, end)


def compare(text, size, rom, addr, relocs, retail_relocs, syms, retail_names, lits=None,
            linker_syms=None):
    """Return (mismatching word offsets, count of relocations whose target
    couldn't be checked because they point at an object-local symbol).

    Relocations against our own literals (strings, float constants) are
    checked by content: the bytes at our %hi/%lo target must equal retail's
    bytes at the address retail's own %hi/%lo pair computes. Literals that
    themselves carry relocations (jump tables, pointer tables) stay
    unchecked."""
    lits = lits or {}
    linker_syms = linker_syms or {}
    bad, local = [], set()
    if no_retail_relocs(addr):
        uncovered = decode_uncovered(text, size, rom, addr, relocs, retail_names)
        retail_relocs = {**retail_relocs, **{addr + i: r for i, r in uncovered.items()}}
    for i in range(0, size, 4):
        mine = struct.unpack_from("<I", text, i)[0]
        theirs = struct.unpack_from("<I", rom, addr - VRAM_TO_FILE + i)[0]
        rtype, rsym = relocs.get(i, ("", ""))
        mask = ~RELOC_MASK.get(rtype, 0) & 0xFFFFFFFF
        if mine & mask != theirs & mask:
            bad.append(i)
            continue
        if not rtype:
            continue
        # Masked bits must still point at the same thing retail points at.
        retail = retail_relocs.get(addr + i)
        if retail is None and rtype in ("R_MIPS_HI16", "R_MIPS_LO16") and rsym in linker_syms:
            # A linker-defined symbol: retail has the resolved address and no
            # relocation. Our addend must be 0 and retail's immediate must be
            # exactly that symbol's %hi/%lo.
            value = linker_syms[rsym]
            want = ((value + 0x8000) >> 16) & 0xFFFF if rtype == "R_MIPS_HI16" else value & 0xFFFF
            if mine & 0xFFFF or theirs & 0xFFFF != want:
                bad.append(i)
            continue
        if retail is None and rtype == "R_MIPS_26" and rsym in syms:
            # Some retail jumps were resolved without leaving a relocation:
            # decode the target and compare addresses instead.
            target = ((addr + i) & 0xF0000000) | ((theirs & 0x03FFFFFF) << 2)
            if target != syms[rsym][0]:
                bad.append(i)
        elif retail is None or retail[0] != rtype:
            bad.append(i)
        elif rsym.startswith((".", "@")) or not rsym:
            local.add(i)  # section/literal symbol: no stable name to compare
        elif re.search(r"\$\d+$", rsym):
            # Function-local statics get compiler-numbered names
            # (allowThisAssertion$2011, init$5): compare the base name only.
            if rsym.split("$")[0] != retail[1].split("$")[0]:
                bad.append(i)
            else:
                local.add(i)
        elif sanitize(rsym) != retail[1]:
            bad.append(i)

    # Addends: a matching symbol name isn't enough (vtable+0x74 vs +0x78).
    # Pair each LO16 with the nearest preceding HI16 on the same symbol, in
    # our object and at the same offsets in retail, and require our addend
    # to equal retail's (address - symbol value). Section-relative relocs
    # (our literals/jump tables) carry offsets into our own sections: skip.
    def sext16(v):
        v &= 0xFFFF
        return v - 0x10000 if v & 0x8000 else v

    def word(buf, off):
        return struct.unpack_from("<I", buf, off)[0]

    his = {}
    verified = set()
    for i in range(0, size, 4):
        rtype, rsym = relocs.get(i, ("", ""))
        if rtype == "R_MIPS_HI16":
            his[rsym] = i
        elif rtype == "R_MIPS_LO16" and rsym in his and i in lits:
            h = his[rsym]
            if i in bad or h in bad:
                continue
            sec, sec_relocs, value, sym_size = lits[i]
            ours = (word(text, h) & 0xFFFF) * 0x10000 + sext16(word(text, i))
            start, end = literal_extent(sec, value, sym_size, value + ours)
            if start < 0 or start >= end or any(start - 3 <= r < end for r in sec_relocs):
                continue  # out of range, or a table of pointers
            rom_h = word(rom, addr - VRAM_TO_FILE + h)
            rom_l = word(rom, addr - VRAM_TO_FILE + i)
            full = ((rom_h & 0xFFFF) * 0x10000 + sext16(rom_l)) & 0xFFFFFFFF
            fo = full - VRAM_TO_FILE
            if not 0 <= fo <= len(rom) - (end - start):
                continue
            if sec[start:end] != rom[fo:fo + end - start]:
                bad.append(i)
            else:
                verified.update((h, i))
        elif rtype == "R_MIPS_LO16" and rsym in his and not rsym.startswith("."):
            h = his[rsym]
            retail = retail_relocs.get(addr + i)
            if retail is None or i in bad or h in bad:
                continue
            ours = (word(text, h) & 0xFFFF) * 0x10000 + sext16(word(text, i))
            rom_h = word(rom, addr - VRAM_TO_FILE + h)
            rom_l = word(rom, addr - VRAM_TO_FILE + i)
            full = ((rom_h & 0xFFFF) * 0x10000 + sext16(rom_l)) & 0xFFFFFFFF
            if (full - retail[2]) & 0xFFFFFFFF != ours & 0xFFFFFFFF:
                bad.append(i)
    return bad, len(local - verified)


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--flags", default=DEFAULT_FLAGS)
    ap.add_argument("--as-c", action="store_true",
                    help="compile as C (drafts that call functions by mangled name)")
    ap.add_argument("--extern-c", action="store_true",
                    help="compile as C++ inside extern \"C\" (drafts using C99-style declarations)")
    ap.add_argument("-q", "--quiet", action="store_true", help="only print the summary line")
    ap.add_argument("-v", "--verbose", action="store_true", help="show differing words")
    ap.add_argument("--cc", metavar="ID",
                    help="compiler toolchain/ID/mwccps2.exe, overriding a file's `// compiler:` line")
    ap.add_argument("sources", nargs="+")
    args = ap.parse_args()

    rom = open(BASEROM, "rb").read()
    syms = load_symbols()
    retail_relocs = load_retail_relocs(rom)
    retail_names = load_retail_names(rom)
    linker_syms = load_linker_symbols(rom)
    failed = 0
    with tempfile.TemporaryDirectory() as tmp:
        for src in args.sources:
            obj = os.path.join(tmp, "out.o")
            # A MATCHED file records how it was built; honour that.
            head = open(src, errors="replace").read(4000)
            marker = re.search(r"MATCHED \(mwccps2 [\d.]+, (.*), mode: ([\w-]+)\)", head)
            cc = COMPILER_RE.search(head)
            cc_id = args.cc or (cc and cc.group(1))
            mwcc = compiler_path(cc_id) if cc_id else None
            flags = marker.group(1) if marker else args.flags
            as_c = args.as_c or (marker and marker.group(2) == "c")
            extern_c = args.extern_c or (marker and marker.group(2) == "extern-c")
            # Copies keep the source's base name: mwcc names a TU's static
            # initializer after its file (__sinit_vu0Functions.c).
            stem = os.path.splitext(os.path.basename(src))[0]
            if as_c:  # mwcc picks the language from the extension
                c_src = os.path.join(tmp, stem + ".c")
                with open(src, "rb") as fi, open(c_src, "wb") as fo:
                    fo.write(fi.read())
                src_to_build = c_src
            elif extern_c:  # C++ parsing, but names kept verbatim
                c_src = os.path.join(tmp, stem + ".cpp")
                with open(src, "rb") as fi, open(c_src, "wb") as fo:
                    fo.write(b'extern "C" {\n' + fi.read() + b'\n}\n')
                src_to_build = c_src
            else:
                src_to_build = src
            try:
                compile_obj(src_to_build, flags, obj, src_dir=os.path.dirname(os.path.abspath(src)), mwcc=mwcc)
            except SystemExit as e:
                print(f"  ERROR {src}: {e}")
                failed += 1
                continue
            funcs = obj_text(obj)
            print(f"{src}:")
            for name, text, relocs, lits in funcs:
                size = len(text)
                key = sanitize(name)
                if key not in syms:
                    print(f"  ?     {name}: not a retail symbol")
                    continue
                addr, rsize = syms[key]
                if size != rsize:
                    print(f"  FAIL  {name} @ 0x{addr:08X}: size 0x{size:X}, retail 0x{rsize:X}")
                    failed += 1
                    continue
                bad, unchecked = compare(text, size, rom, addr, relocs,
                                         retail_relocs, syms, retail_names, lits, linker_syms)
                if bad:
                    failed += 1
                    print(f"  FAIL  {name} @ 0x{addr:08X}: {len(bad)}/{size // 4} words differ")
                    if args.verbose:
                        for i in bad:
                            mine = struct.unpack_from("<I", text, i)[0]
                            theirs = struct.unpack_from("<I", rom, addr - VRAM_TO_FILE + i)[0]
                            print(f"        +0x{i:03X}: built {mine:08X}  retail {theirs:08X}")
                else:
                    note = f", {unchecked} local relocs unchecked" if unchecked else ""
                    print(f"  OK    {name} @ 0x{addr:08X} (0x{size:X} bytes{note})")
    sys.exit(1 if failed else 0)


if __name__ == "__main__":
    main()
