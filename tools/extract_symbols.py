#!/usr/bin/env python3
"""Recover the retail symbol table from SLUS_202.23 and write splat's symbol_addrs.txt.

The retail ELF ships with .symtab/.strtab/.debug intact, but its section header
table is truncated (the file ends 56 bytes into it), so readelf/objdump can't
find them. Their offsets are fixed for this exact build (SHA-1 checked below),
so we read them directly.

Usage: tools/extract_symbols.py baserom/SLUS_202.23 config/symbol_addrs.txt
"""
import hashlib
import struct
import sys

SHA1 = "496ba6a5bfe9d7e68ea2edb993b493033ccc9bae"
SYMTAB_OFF, SYMTAB_COUNT = 0x29EB50, 18162  # Elf32_Sym[18162] (null + .text first), ends 0x2E5A70
STRTAB_OFF = 0x24EB60

STT_OBJECT, STT_FUNC = 1, 2

# Template args (<>) and Metrowerks thunk prefixes (@640@48@) aren't legal in splat
# filenames or GNU as labels; spell them out reversibly.
SANITIZE = {"<": "_LT_", ">": "_GT_", "@": "_AT_"}


def sanitize(name):
    for bad, good in SANITIZE.items():
        name = name.replace(bad, good)
    return name


def main(rom_path, out_path):
    data = open(rom_path, "rb").read()
    if hashlib.sha1(data).hexdigest() != SHA1:
        sys.exit(f"{rom_path}: SHA-1 mismatch, expected {SHA1}")

    def name_at(off):
        start = STRTAB_OFF + off
        return data[start:data.index(b"\0", start)].decode("latin-1")

    syms = []
    for i in range(SYMTAB_COUNT):
        st_name, value, size, info, _other, _shndx = struct.unpack_from(
            "<IIIBBH", data, SYMTAB_OFF + 16 * i)
        # Binding is ignored: besides LOCAL/GLOBAL/WEAK, Metrowerks marks
        # some locals with processor-specific binding 13 (info 0xD1/0xD2).
        kind = info & 0xF
        if kind not in (STT_OBJECT, STT_FUNC) or not 0x200000 <= value < 0x4F5680:
            continue
        name = name_at(st_name)
        if name:
            syms.append((value, kind, size, sanitize(name)))

    # Static symbols can repeat across TUs; splat needs unique names.
    seen = {}
    lines = []
    for value, kind, size, name in sorted(set(syms)):
        if name in seen and seen[name] != value:
            name = f"{name}_{value:08X}"
        seen.setdefault(name, value)
        # splat has no generic "data" type; untyped means data.
        attrs = " type:func" if kind == STT_FUNC else ""
        if size:
            attrs += f" size:0x{size:X}"
        comment = f" //{attrs}" if attrs else ""
        lines.append(f"{name} = 0x{value:08X};{comment}\n")

    with open(out_path, "w") as f:
        f.writelines(lines)
    funcs = sum(1 for s in syms if s[1] == STT_FUNC)
    print(f"wrote {len(lines)} symbols ({funcs} functions) to {out_path}")


if __name__ == "__main__":
    if len(sys.argv) != 3:
        sys.exit(__doc__)
    main(sys.argv[1], sys.argv[2])
