#!/usr/bin/env python3
"""Find addresses the image holds without a relocation.

A link without pins is only shiftable if every pointer is a relocation: ld
fixes those up when things move, and nothing else. `make relink` calls
audit() on its --emit-relocs link, which lists each aligned word of .main whose value is the address of a defined symbol (0x200000 ..
the end of .bss) but which no relocation covers. Each is either a number
that happens to look like an address or a pointer that would go stale.
"""
import bisect
import collections
import os
import struct
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def elf_sections(data):
    shoff, = struct.unpack_from("<I", data, 0x20)
    shentsize, shnum, shstrndx = struct.unpack_from("<HHH", data, 0x2E)
    secs = [struct.unpack_from("<IIIIIIIIII", data, shoff + i * shentsize) for i in range(shnum)]
    names = [data[secs[shstrndx][4] + s[0]:].split(b"\0")[0].decode() for s in secs]
    return secs, names


def audit(path, verbose=False, top=25):
    """Words of .main in the ELF at `path` (linked with --emit-relocs) that
    hold a defined symbol's address (.main's start to .main_bss's end)
    without a relocation. Prints a summary; returns the count."""
    data = open(path, "rb").read()
    secs, names = elf_sections(data)
    main_sec = secs[names.index(".main")]
    base, off, size = main_sec[3], main_sec[4], main_sec[5]
    bss = secs[names.index(".main_bss")]
    lo, hi = base, bss[3] + bss[5]
    rel = secs[names.index(".rel.main")]
    covered = set()
    for p in range(rel[4], rel[4] + rel[5], 8):
        r_off, info = struct.unpack_from("<II", data, p)
        covered.add(r_off if r_off >= base else base + r_off)
    symtab = secs[names.index(".symtab")]
    strtab = secs[names.index(".strtab")]
    addrs = collections.defaultdict(list)
    for p in range(symtab[4], symtab[4] + symtab[5], 16):
        st_name, value, sz, info, other, shndx = struct.unpack_from("<IIIBBH", data, p)
        if lo <= value < hi and shndx != 0 and (info & 0xF) != 3:
            start = strtab[4] + st_name
            addrs[value].append(data[start:data.index(b"\0", start)].decode())
    syms_sorted = sorted(addrs)
    hits = collections.Counter()
    shown = 0
    for a in range(base, base + size, 4):
        v, = struct.unpack_from("<I", data, off + a - base)
        if v in addrs and a not in covered:
            # which object section holds the word: nearest symbol at or below it
            i = bisect.bisect_right(syms_sorted, a) - 1
            where = addrs[syms_sorted[i]][0] if i >= 0 else "?"
            hits[where] += 1
            if verbose and shown < 200:
                print(f"{a:#x}: {v:#010x} = {addrs[v][0][:40]:40} in {where[:50]}")
                shown += 1
    print(f"{sum(hits.values())} words hold a symbol's address without a relocation, in {len(hits)} places")
    for w, n in hits.most_common(top):
        print(f"  {n:5} {w}")
    return sum(hits.values())

