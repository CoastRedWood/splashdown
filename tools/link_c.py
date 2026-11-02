#!/usr/bin/env python3
"""Link matched C/C++ objects into the executable in place of their asm.

Usage: tools/link_c.py [-j N] [--out build/link] [--only FILE ...]

Every `// MATCHED` source under src/ is compiled the way tools/match.py
would compile it. Each object's .text sections (one per function) replace
the function's asm: the game asm files (asm/text.s, asm/text_late.s; the
Sony SDK objects between them live in asm/sdk/) are
cut into chunks around the chosen functions, and for each file a
linker-script fragment lists chunk, object section,
chunk, ... with the location counter pinned to each retail address, so a
wrong size or alignment fails the link instead of shifting the image.

Only code is taken from the objects. Everything else an object carries
(.rodata literals and jump tables, .vtables, .bss/.sbss/.data globals and
local statics, .ctor/.init static initialisers, exception tables) is left
out of the link: retail's linker placed data per TU, which per-function
files can't reproduce. Instead every relocation against such a section is
redirected to the address retail's own bytes compute at that spot (the
literal match.py already verified by content, the global the asm data
defines), through a generated absolute symbol. The code is then
byte-identical with the asm data standing in for the object's own.

What else it takes to link mwcc objects with GNU ld against splat's asm:
- Retail reuses symbol names (file-scope statics like ZeroVector); splat
  suffixes the later ones with _%08X. Each such reference is renamed to
  the symbol retail's own relocation points at.
- Names with `<`, `>`, `@` are spelled _LT_, _GT_, _AT_ in the asm:
  aliases are generated both ways.
- Inline functions (`_D3DVECTOR::_D3DVECTOR()`), template members and
  `@N@` thunks come out as a copy in every object that uses them, with
  mwcc's own symbol binding 13 (BFD reads it as local) or weak, and some
  template dtor copies retail's linker dropped have no retail address.
  The linker picks the copy: every copy goes into a section
  `.gnu.linkonce.text.<name>` (linkonce_name()) with its symbol global
  (references by section symbol are rebound to it), the fragment lists the
  function's slot as `*(.gnu.linkonce.text.<name>)`, and GNU ld keeps the
  first copy in link order and discards the others, a reference to a
  discarded copy's symbol binding to the kept one. The link order is
  input.ld's INPUT list (the objects by their first function, data-only
  TUs last), which link_check.sh puts first in the script; the copy the
  linker keeps is the one retail's address range for it lies in (the
  TU-mode data placement below checks that copy's data), as retail's
  linker also kept the first. A copy nothing lists (no retail address) is
  discarded with the rest of the unlisted sections; a grouped link drops it
  by --gc-sections.
- Jump-table targets inside a cut function are `jlabel`s the asm rodata
  refers to; they are provided at their addresses.
- A function matched in two files is taken from the first (by path); the
  other copy is left out like an inline copy.

TU mode: a file under src/tu/ is a whole retail
translation unit, so its data is retail's too, and its
data is placed instead of redirected:
- Every allocated data section of the object (.rodata literals and RTTI
  records, .vtables, .data, .bss, .ctor) gets the retail address its
  references compute: from the code's relocations (the address retail's
  bytes encode, minus our symbol value and addend), from data-to-data
  relocations once the referring section is placed (a vtable gives its
  RTTI record, the record its name string), and from retail symbol names
  that are unique (`.p__sinit_<File>`). Its bytes, relocated words aside,
  must equal retail's there.
- The static initializer (`__sinit_<File>`, in .init) is placed like a
  function, in asm/text_island.s.
- Each section is pinned at its address. The data asm files
  (asm/data/rodata.rodata.s, asm/data/data.data.s) are cut around the
  TU's ranges like text, a range widened over zero padding to an 8-aligned
  item boundary; .bss sections go into .main_bss. The labels cut out of
  the asm are PROVIDEd at their addresses for whatever asm still uses them.
  Where no 8-aligned boundary is in reach (adjacent .ctor slots, a vtable
  followed by a non-zero item) the next chunk starts 4-aligned and its
  `.align`s become `.org`s at the retail offsets; zeros to the end of a
  file are padded with `. = <end>` after the last placed section (the
  sdata at the end of .data: JetskiScreenText's template). The text_late TUs' .rodata sits at the
  head of asm/text_island.s (before its first function): that part of the
  island is cut the same way.
- Inline-bound data (a symbol with mwcc's inline binding: a vtable whose
  virtuals are all inline, e.g. SearchableGameObject's, emitted by every
  TU whose dtor inlines the base dtor; an inline function's local statics,
  GameText::GetString's) is linkonce like inline functions
  (`.gnu.linkonce.<kind>.<name>`, listed once as `*(...)`). A TU's copy is
  given an address only if its references agree with the rest of the TU's
  data; the first holder in link order always agrees, and that is the copy
  the linker keeps.
- Two TU objects placing a section at the same retail address: inline-bound
  data is listed once (above); otherwise (an RTTI block reached only from
  an inline-bound vtable copy, LightFlare.cpp's of TextureMap.cpp's; a
  literal retail pooled, MotionLibrary.cpp's "") the copy of the object
  whose own sections lie nearest (the first by path on a tie) is placed,
  the other is named like placed data but not listed (`tudrop ... also
  in`) and its references go to retail's address. Data only inline copies
  reach (their ASSERT statics, the RTTI chain of a vtable copy) is named
  the same way, so a grouped link's wildcards take it and the linker drops
  it with the copy (--gc-sections).
- A section with no retail address that nothing placed refers to (a
  `Stripped*` stand-in's literal, a weak copy's static) is left out and
  listed in report.txt; one that something refers to fails the file's TU
  mode, and only its code links (data from the asm).
- Shim sections (.mwccshim, .mwccshim.text: heap images and session shims,
  include/mwcc_shim.h) are not program data or code: never placed, never
  listed, discarded by name.

SDK data: the SDK objects' .data and .rodata (config/sdk_data.txt, from
the retail .symtab's section symbols) are cut out of
the data asm too, one file per object section, sdkdata_<lib>_<obj>_<kind>.s
under its real section name, each pinned at its address.

Their .bss too (and errno, newlib's one COMMON symbol): each section
becomes a file defining the labels the asm uses inside it, pinned in
.main_bss, and those labels leave undefined_syms_auto.txt (the link uses
the copy in --out).

Writes under --out: chunk_NNNN.s (game code), dchunk_NNNN.s (data),
sdkdata_*.s (SDK data; the linker-generated data and alignment padding
between are linker-script statements: EXC_INDEX_LD, DATA_SYMS),
vupackets.s (the VU upload packets, from
0x3E0000: vu_packet_words()), crt0.s (asm/sdk/crt0/crt0.s with Sony's linker
symbols by name: LINKER_SYMS), input.ld (the link order), undefined_syms.txt,
obj/*.o, text.ld, text_late.ld, text_island.ld, rodata.ld, data.ld and
bss.ld (each replaces that asm object's line in splat's script),
aliases.ld (an extra -T file), objs.txt, tudata.txt (every placed TU data
section), report.txt. tools/link_check.sh drives it.
"""
import argparse
import bisect
import collections
import concurrent.futures as cf
import hashlib
import os
import re
import struct
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, "tools"))
import match  # noqa: E402

# Game code, in link order; the SDK objects (asm/sdk/) sit between the
# first two and are linked as they are. The text island holds the static
# initializers (placed only for TU files). Each file gets its own fragment.
GAME_ASM = ["asm/text.s", "asm/text_late.s", "asm/text_island.s"]
# Data asm, cut around TU files' data (see TU mode above).
DATA_ASM = ["asm/data/rodata.rodata.s", "asm/data/data.data.s"]
TU_DIR = os.path.join(ROOT, "src", "tu")  # whole-TU source files: data placed from the object
VRAM = 0x200000
BSS_VRAM = 0x44EA00  # .main_bss (config/splat.yaml: bss_size 0xA6C80 after the loaded image)
CROSS = os.environ.get("CROSS", "mipsel-linux-gnu-")
STB_LOCAL, STB_GLOBAL, STB_WEAK = 0, 1, 2
STB_INLINE = 13  # mwcc's binding for an inline function's per-object copy (BFD reads it as local)
STT_FUNC, STT_SECTION = 2, 3
SHT_PROGBITS, SHT_SYMTAB, SHT_STRTAB, SHT_NOBITS, SHT_REL = 1, 2, 3, 8, 9
R_MIPS_32, R_MIPS_26, R_MIPS_HI16, R_MIPS_LO16, R_MIPS_GPREL16 = 2, 4, 5, 6, 7


def sext16(v):
    v &= 0xFFFF
    return v - 0x10000 if v & 0x8000 else v


SHIM_SECTION = ".mwccshim"  # include/mwcc_shim.h: heap images, session shims; never linked


def is_shim_section(name):
    return name == SHIM_SECTION or name.startswith(SHIM_SECTION + ".")


LINKONCE = ".gnu.linkonce."
LINKONCE_NOTE = "inline copy the linker drops (linkonce)"


def linkonce_name(kind, symbol):
    """`.gnu.linkonce.<kind>.<symbol>`, the symbol spelled for a linker
    script (<, >, @ as _LT_, _GT_, _AT_; any other character but [A-Za-z0-9_.]
    as _XX_). GNU ld keeps the first section of a name in link order and
    discards the rest; a symbol defined in a discarded one binds to the kept
    copy's."""
    return LINKONCE + kind + "." + re.sub(r"[^A-Za-z0-9_.]", lambda m: f"_{ord(m.group()):02X}_",
                                          match.sanitize(symbol))


def copy_data(elf, symbols, placed):
    """Data sections only inline copies (inline or weak functions,
    inline-bound data) reach, directly or through other such data, leaving
    out `placed` sections: what goes with a copy the linker discards."""
    copies = {sh for n, v, sz, k, sh, b in symbols if b in (STB_INLINE, STB_WEAK) and 0 < sh < elf.shnum}
    out, todo = set(), list(copies)
    while todo:
        for pos, off, symidx, rtype in elf.relocs(todo.pop()):
            t = symbols[symidx][4]
            if 0 < t < elf.shnum and t not in copies | out | set(placed) and elf.secs[t][2] & 2 \
                    and not elf.secs[t][2] & 4:
                out.add(t)
                todo.append(t)
    return out


def script_line(o, name):
    """How a fragment names an object section: a linkonce section by name in
    any object (`*(...)`: the linker picks the copy), anything else by file."""
    return f"*({name})" if name.startswith(LINKONCE) else f"{os.path.relpath(o.path, ROOT)}({name})"


def parse_blocks(lines):
    """[(name, addr, first_line, end_line)] for every function block in
    text.s. A block runs from its `nonmatching`/`glabel` line up to (not
    including) the `.align 3` that opens the next block, so it carries the
    padding nops after its own endlabel."""
    starts = []
    for i, l in enumerate(lines):
        if l != ".align 3":
            continue
        j = i + 1
        while j < len(lines) and (lines[j].strip() == "" or lines[j].startswith("/*")):
            j += 1
        m = re.match(r"(?:nonmatching|glabel) (\S+?),?(?:\s|$)", lines[j])
        if m:
            starts.append((i, j, m.group(1)))
    blocks = []
    for k, (ai, li, name) in enumerate(starts):
        end = starts[k + 1][0] if k + 1 < len(starts) else len(lines)
        addr = None
        for l in lines[li:end]:
            m = re.match(r"\s*/\* \w+ ([0-9A-F]{8}) ", l)
            if m:
                addr = int(m.group(1), 16)
                break
        blocks.append((name, addr, li, end))
    return blocks


class Elf:
    """Just enough of a relocatable ELF32 (little-endian) to list sections,
    symbols and relocations, and to edit them in place."""

    def __init__(self, path):
        self.path = path
        self.data = bytearray(open(path, "rb").read())
        d = self.data
        self.shoff, = struct.unpack_from("<I", d, 0x20)
        self.shentsize, self.shnum, self.shstrndx = struct.unpack_from("<HHH", d, 0x2E)
        self.secs = [list(struct.unpack_from("<IIIIIIIIII", d, self.shoff + i * self.shentsize))
                     for i in range(self.shnum)]  # name type flags addr offset size link info align entsize
        shstr = self.secs[self.shstrndx][4]
        self.names = [self.cstr(shstr + s[0]) for s in self.secs]
        self.symtab = next(i for i, s in enumerate(self.secs) if s[1] == SHT_SYMTAB)
        self.strtab = self.secs[self.symtab][6]

    def cstr(self, off):
        return self.data[off:self.data.index(b"\0", off)].decode("latin-1")

    def symbols(self):
        """[(name, value, size, type, shndx, bind)] in symbol-table order."""
        strtab = self.secs[self.strtab][4]
        out = []
        sec = self.secs[self.symtab]
        for k in range(sec[5] // 16):
            st_name, value, size, info, _other, shndx = struct.unpack_from(
                "<IIIBBH", self.data, sec[4] + 16 * k)
            out.append((self.cstr(strtab + st_name), value, size, info & 0xF, shndx, info >> 4))
        return out

    def relocs(self, target):
        """[(file position of the entry, offset, symidx, type)] for the
        relocations applying to section `target`."""
        out = []
        for s in self.secs:
            if s[1] == SHT_REL and s[7] == target:
                for p in range(s[4], s[4] + s[5], 8):
                    off, info = struct.unpack_from("<II", self.data, p)
                    out.append((p, off, info >> 8, info & 0xFF))
        return out

    def word(self, shndx, off):
        return struct.unpack_from("<I", self.data, self.secs[shndx][4] + off)[0]

    def set_word(self, shndx, off, value):
        struct.pack_into("<I", self.data, self.secs[shndx][4] + off, value)

    def set_reloc_sym(self, pos, symidx):
        off, info = struct.unpack_from("<II", self.data, pos)
        struct.pack_into("<II", self.data, pos, off, (symidx << 8) | (info & 0xFF))

    def append_table(self, shndx, extra):
        """Move section shndx to the end of the file with `extra` appended."""
        s = self.secs[shndx]
        body = bytes(self.data[s[4]:s[4] + s[5]]) + extra
        start = (len(self.data) + 3) & ~3
        self.data += b"\0" * (start - len(self.data)) + body
        s[4], s[5] = start, len(body)

    def rename_sections(self, new_names):
        """new_names: {section index: name}."""
        table = bytearray(b"\0")
        offs = {}
        for i in range(self.shnum):
            offs[i] = len(table)
            table += new_names.get(i, self.names[i]).encode("latin-1") + b"\0"
        s = self.secs[self.shstrndx]
        s[5] = 0
        self.append_table(self.shstrndx, bytes(table))
        for i in range(self.shnum):
            self.secs[i][0] = offs[i]
        self.names = [new_names.get(i, n) for i, n in enumerate(self.names)]

    def add_undefined(self, names):
        """Append undefined global symbols; return {name: index}."""
        if not names:
            return {}
        strtab = self.secs[self.strtab]
        name_off = {}
        extra = bytearray()
        for n in names:
            name_off[n] = strtab[5] + len(extra)
            extra += n.encode("latin-1") + b"\0"
        self.append_table(self.strtab, bytes(extra))
        sym = self.secs[self.symtab]
        first = sym[5] // 16
        entries = b"".join(struct.pack("<IIIBBH", name_off[n], 0, 0, STB_GLOBAL << 4, 0, 0) for n in names)
        self.append_table(self.symtab, entries)
        return {n: first + i for i, n in enumerate(names)}

    def rebind(self, globalize, undefine):
        """Make the symbols at `globalize` global, and those at `undefine`
        undefined globals. Locals must precede globals in a symbol table,
        so the table is rebuilt and every relocation's symbol index
        remapped."""
        if not globalize and not undefine:
            return
        sec = self.secs[self.symtab]
        n = sec[5] // 16
        entries = [bytearray(self.data[sec[4] + 16 * k:sec[4] + 16 * k + 16]) for k in range(n)]
        for k in globalize:
            entries[k][12] = (STB_GLOBAL << 4) | (entries[k][12] & 0xF)
        for k in undefine:
            e = entries[k]
            struct.pack_into("<IIBBH", e, 4, 0, 0, (STB_GLOBAL << 4) | (e[12] & 0xF), e[13], 0)
        order = [k for k in range(n) if entries[k][12] >> 4 == STB_LOCAL] + \
                [k for k in range(n) if entries[k][12] >> 4 != STB_LOCAL]
        remap = {old: new for new, old in enumerate(order)}
        self.data[sec[4]:sec[4] + 16 * n] = b"".join(entries[k] for k in order)
        sec[7] = sum(1 for k in range(n) if entries[k][12] >> 4 == STB_LOCAL)  # sh_info
        for s in self.secs:
            if s[1] == SHT_REL and s[6] == self.symtab:
                for p in range(s[4], s[4] + s[5], 8):
                    off, info = struct.unpack_from("<II", self.data, p)
                    struct.pack_into("<II", self.data, p, off, (remap[info >> 8] << 8) | (info & 0xFF))

    def save(self):
        for i, s in enumerate(self.secs):
            struct.pack_into("<IIIIIIIIII", self.data, self.shoff + i * self.shentsize, *s)
        open(self.path, "wb").write(self.data)


def load_ld_names():
    """{address: [names]} as the asm and splat's symbol files define them."""
    out = {}
    for fn in ("config/symbol_addrs.txt", "undefined_syms_auto.txt", "undefined_funcs_auto.txt"):
        for line in open(os.path.join(ROOT, fn)):
            m = re.match(r"(\S+) = 0x([0-9A-Fa-f]+);", line)
            if m:
                out.setdefault(int(m.group(2), 16), []).append(m.group(1))
    return out


class Func:
    def __init__(self, name, key, addr, size, shndx, symidx, bind):
        self.name, self.key, self.addr, self.size = name, key, addr, size
        self.shndx, self.symidx, self.bind = shndx, symidx, bind
        self.relocs = {}     # offset -> symbol name, for undefined symbols only
        self.redirects = []  # (reloc file position, retail target, mask, section, section start) for refs into this object
        self.section = None  # set when placed


class Obj:
    def __init__(self, src, path):
        self.src, self.path = src, path
        self.funcs = []    # retail functions, in symbol-table order
        self.extra = []    # functions with no retail address (dropped)
        self.renames = {}  # ambiguous-name fixes
        self.undefine = []  # symbol indices to turn undefined (globals in dropped sections)
        self.tu = False     # a whole-TU file (src/tu/): its data is placed too
        self.data = {}      # TU mode: data section index -> retail address
        self.data_relocs = []  # TU mode: (section, reloc position, offset, symidx, retail word) in placed data
        self.data_sections = {}  # TU mode: data section index -> name in the link
        self.unplaced = set()    # TU mode: another TU's copy is placed (`also in`); named, not listed


def place_tu_data(elf, o, symbols, rom, retail_unique, funcs):
    """TU mode: find every allocated data section's retail address. Returns
    ({section: address}, [notes on sections left out]) or an error string."""
    alloc = [i for i, n in enumerate(elf.names)
             if elf.secs[i][2] & 2 and not (elf.secs[i][2] & 4)  # SHF_ALLOC, not SHF_EXECINSTR
             and elf.secs[i][1] in (SHT_PROGBITS, SHT_NOBITS)
             and not is_shim_section(n)]
    addr, why = {}, {}
    # Per-object copies of inline data (a vtable whose virtuals are all
    # inline, e.g. SearchableGameObject's in every TU whose dtor inlines its
    # base dtor) carry mwcc's inline binding. Retail kept one copy, the
    # first TU's in link order; this TU's copy is placed only if it is that
    # one (its references agree with the rest of the TU's data), else it is
    # left out and references to it go to retail's copy.
    # (Inline functions' local statics carry the binding too; they are
    # placed from the code like any data.)
    inline_secs = {sh for n, v, sz, k, sh, b in symbols
                   if b == STB_INLINE and n.startswith("__vt__") and sh in alloc}
    phase = [1]

    def put(sec, a, how):
        if sec not in alloc:
            return None
        if phase[0] == 1 and sec in inline_secs:
            return None
        if sec in addr and addr[sec] != a:
            return (f"{elf.names[sec]} (section {sec}) at {addr[sec]:#x} ({why[sec]}) "
                    f"and {a:#x} ({how})")
        addr.setdefault(sec, a)
        why.setdefault(sec, how)
        return None

    # The TU's own functions first. Template and inline copies (not global)
    # can be retail's copies from another TU, which retail's linker dropped
    # (SoundEmitters.cpp's ContainerList<PSXSound *> members, whose ASSERT
    # strings are another TU's): take them nearest the TU's own code first,
    # and leave out any whose references disagree with the data placed so far.
    own = [f for f in funcs if f.bind == STB_GLOBAL]
    rest = [f for f in funcs if f.bind != STB_GLOBAL]
    if own:
        lo, hi = min(f.addr for f in own), max(f.addr for f in own)
        rest.sort(key=lambda f: 0 if lo <= f.addr <= hi else min(abs(f.addr - lo), abs(f.addr - hi)))
    for f in own:
        for _, _, _, sec, base, _ in f.redirects:
            err = put(sec, base, f"referenced by {f.key}")
            if err:
                return err
    skipped = set()
    for f in rest:
        if any(sec in alloc and sec not in inline_secs and sec in addr and addr[sec] != base
               for _, _, _, sec, base, _ in f.redirects):
            skipped.add(f.shndx)
            continue
        for _, _, _, sec, base, _ in f.redirects:
            err = put(sec, base, f"referenced by {f.key}")
            if err:
                return err
    for name, value, size, kind, shndx, bind in symbols:
        # Compiler-numbered names (@126, allowThisAssertion$663) mean
        # nothing across objects; retail's unique real names do.
        if not name or name.startswith("@") or "$" in name:
            continue
        a = retail_unique.get(match.sanitize(name))
        if a is not None and 0 < shndx < elf.shnum:
            err = put(shndx, a - value, f"retail symbol {name}")
            if err:
                return err
    # Data that only data refers to (RTTI records and their name strings).
    def chain(todo):
        while todo:
            sec = todo.pop()
            if elf.secs[sec][1] == SHT_NOBITS:
                continue
            for pos, off, symidx, rtype in elf.relocs(sec):
                tsec = symbols[symidx][4]
                if not 0 < tsec < elf.shnum or tsec not in alloc:
                    continue
                if rtype != R_MIPS_32:
                    return f"{elf.names[sec]}: relocation type {rtype} at +{off:#x}"
                if not 0 <= addr[sec] + off - match.VRAM_TO_FILE <= len(rom) - 4:
                    # A record that disagrees with retail's (UIWater.cpp's RTTI base
                    # list before UIWaterPatch's private base) chains to garbage.
                    return (f"{elf.names[sec]} (section {sec}) at {addr[sec]:#x} ({why[sec]}) "
                            f"is outside the image")
                word = struct.unpack_from("<I", rom, addr[sec] + off - match.VRAM_TO_FILE)[0]
                new = tsec not in addr
                err = put(tsec, (word - symbols[symidx][1] - elf.word(sec, off)) & 0xFFFFFFFF,
                          f"referenced by {elf.names[sec]} at {addr[sec] + off:#x}")
                if err:
                    return err
                if new and tsec in addr:
                    todo.append(tsec)
        return None
    err = chain(list(addr))
    if err:
        return err
    # Inline data copies: try each at its retail name's address; keep it only
    # if everything it refers to lands where the TU's own data already is.
    phase[0] = 2
    dropped_inline = set()
    for sec in sorted(inline_secs):
        a = None
        for name, value, size, kind, shndx, bind in symbols:
            if shndx == sec and name and not name.startswith("@") and "$" not in name:
                r = retail_unique.get(match.sanitize(name))
                if r is not None:
                    a = r - value
        if a is None:
            continue
        saved = (dict(addr), dict(why))
        if put(sec, a, "retail symbol (inline copy)") is None and chain([sec]) is None:
            continue
        addr.clear(); addr.update(saved[0]); why.clear(); why.update(saved[1])
        dropped_inline.add(sec)
    # Unplaced sections are left out, unless something placed refers to them.
    notes = []
    placed_secs = set(addr) | {f.shndx for f in funcs if f.shndx not in skipped}
    for sec in alloc:
        if sec in addr:
            continue
        if sec in dropped_inline:
            # References to it are redirected to retail's copy (rewrite()).
            holder = next((n for n, v, sz, k, sh, b in symbols if sh == sec and n), "")
            notes.append(f"{elf.names[sec]} {holder}: {LINKONCE_NOTE}")
            continue
        users = [elf.names[s] for s in placed_secs
                 if any(symbols[r[2]][4] == sec for r in elf.relocs(s))]
        if users:
            return f"{elf.names[sec]} (section {sec}) has no retail address but {users[0]} refers to it"
        holder = next((n for n, v, sz, k, sh, b in symbols if sh == sec and n), "")
        if any(b == STB_INLINE and sh == sec for n, v, sz, k, sh, b in symbols):
            notes.append(f"{elf.names[sec]} {holder}: {LINKONCE_NOTE}")
            continue
        why = "only inline copies refer to it" if sec in copy_data(elf, symbols, addr) else "no retail counterpart"
        notes.append(f"{elf.names[sec]} {holder} (0x{elf.secs[sec][5]:X} bytes): {why}, left out")
    # Bytes must be retail's (relocated words aside: the link checks those).
    for sec, a in addr.items():
        if elf.secs[sec][1] != SHT_PROGBITS:
            continue
        size, off0 = elf.secs[sec][5], elf.secs[sec][4]
        relocated = {off for _, off, _, _ in elf.relocs(sec)}
        for k in range(size):
            if k - k % 4 in relocated:
                continue
            if elf.data[off0 + k] != rom[a + k - match.VRAM_TO_FILE]:
                return f"{elf.names[sec]} at {a:#x}: byte +{k:#x} differs from retail"
    return addr, notes


def redirect(elf, f, syms, rom):
    """Point every relocation of f against a dropped section at the address
    retail's bytes compute there. Each entry also records where that puts
    the start of the target section (retail's target minus our symbol
    value and addend), which TU mode places data sections by. Returns
    None, or an error string."""
    rels = elf.relocs(f.shndx)
    base = f.addr - match.VRAM_TO_FILE

    def rom_at(o):
        return struct.unpack_from("<I", rom, base + o)[0]

    for i, (pos, off, symidx, rtype) in enumerate(rels):
        shndx = syms[symidx][4]
        if shndx == 0 or shndx >= elf.shnum:
            continue  # undefined: resolved by name at link time
        rom_word, our_word = rom_at(off), elf.word(f.shndx, off)
        if rtype == R_MIPS_26:
            target = ((f.addr + off) & 0xF0000000) | ((rom_word & 0x03FFFFFF) << 2)
            addend = (our_word & 0x03FFFFFF) << 2
        elif rtype in (R_MIPS_HI16, R_MIPS_LO16):
            if rtype == R_MIPS_HI16:
                pair = next((r for r in rels[i + 1:] if r[2] == symidx and r[3] == R_MIPS_LO16), None)
            else:
                pair = next((r for r in reversed(rels[:i]) if r[2] == symidx and r[3] == R_MIPS_HI16), None)
            if pair is None:
                return f"{f.name}: {'HI16' if rtype == R_MIPS_HI16 else 'LO16'} at +{off:#x} without its pair"
            hi_off, lo_off = (off, pair[1]) if rtype == R_MIPS_HI16 else (pair[1], off)
            target = ((rom_at(hi_off) & 0xFFFF) << 16) + sext16(rom_at(lo_off))
            addend = ((elf.word(f.shndx, hi_off) & 0xFFFF) << 16) + sext16(elf.word(f.shndx, lo_off))
        elif rtype == R_MIPS_GPREL16:
            target = match.RETAIL_GP + sext16(rom_word)
            addend = sext16(our_word)
        elif rtype == R_MIPS_32:
            target, addend = rom_word, our_word
        else:
            return f"{f.name}: relocation type {rtype} against {elf.names[shndx]}"
        mask = {R_MIPS_32: 0, R_MIPS_26: 0xFC000000}.get(rtype, 0xFFFF0000)
        sec_base = (target - syms[symidx][1] - addend) & 0xFFFFFFFF
        f.redirects.append((pos, target & 0xFFFFFFFF, mask, shndx, sec_base, symidx))
    return None


def disambiguate(func_addr, relocs, ctx, renames, unresolved):
    """Retail reuses names (file-scope statics such as ZeroVector); splat
    suffixes the later ones with _%08X. Work out, from retail's own
    relocation at each use, which symbol a reference means."""
    for off, name in relocs.items():
        if name not in ctx["multi"] or name in unresolved:
            continue
        retail = ctx["retail_relocs"].get(func_addr + off)
        if retail is None:
            unresolved.add(name)
            continue
        cands = ctx["ld_names"].get(retail[2], [])
        pick = next((c for c in cands if re.sub(r"_[0-9A-F]{8}$", "", c) == name), None)
        if pick is None or renames.get(name, pick) != pick:
            unresolved.add(name)  # unknown, or two functions want different ones
        else:
            renames[name] = pick


def compile_one(src, outdir, syms, ctx):
    """Compile src; return (src, status, detail, Obj or None)."""
    head = open(src, errors="replace").read(4000)
    marker = re.search(r"MATCHED \(mwccps2 [\d.]+, (.*), mode: ([\w-]+)\)", head)
    if not marker:
        return src, "skip", "no MATCHED marker", None
    flags, mode = marker.group(1), marker.group(2)
    cc = match.COMPILER_RE.search(head)
    mwcc = match.compiler_path(cc.group(1)) if cc else None
    # Short names: wibo fails on long paths ("missing import FormatMessageA").
    base = hashlib.md5(os.path.relpath(src, ROOT).encode()).hexdigest()[:10]
    obj = os.path.join(outdir, "obj", base + ".o")
    src_dir = os.path.dirname(os.path.abspath(src))
    tmp_src = None
    if mode == "c":
        # Keep the file's stem: mwcc names a TU's static initializer after
        # its file (__sinit_vu0Functions.c, src/tu/vu0Functions.cpp).
        stem = os.path.splitext(os.path.basename(src))[0]
        os.makedirs(os.path.join(outdir, "tmp", base), exist_ok=True)
        tmp_src = os.path.join(outdir, "tmp", base, stem + ".c")
        with open(src, "rb") as fi, open(tmp_src, "wb") as fo:
            fo.write(fi.read())
    elif mode == "extern-c":
        tmp_src = os.path.join(outdir, "tmp", base + ".cpp")
        with open(src, "rb") as fi, open(tmp_src, "wb") as fo:
            fo.write(b'extern "C" {\n' + fi.read() + b"\n}\n")
    try:
        match.compile_obj(tmp_src or src, flags, obj, src_dir=src_dir, mwcc=mwcc)
    except SystemExit as e:
        return src, "error", str(e), None

    elf = Elf(obj)
    symbols = elf.symbols()
    is_tu = os.path.abspath(src).startswith(TU_DIR + os.sep)
    # A TU file's static initializer is placed too (asm/text_island.s).
    text_secs = {i for i, n in enumerate(elf.names) if n == ".text" or (is_tu and n == ".init")}
    o = Obj(src, obj)
    o.tu = is_tu
    o.symbols = symbols
    for idx, (name, value, size, kind, shndx, bind) in enumerate(symbols):
        if shndx == 0 or shndx >= elf.shnum:
            continue
        if kind == STT_FUNC and shndx in text_secs:
            key = match.sanitize(name)
            f = Func(name, key, None, size, shndx, idx, bind)
            if key not in syms:
                f.key = None
                o.extra.append(f)
                continue
            f.addr, rsize = syms[key]
            if size != rsize or value != 0:
                return src, "error", f"{name}: size 0x{size:X} vs retail 0x{rsize:X}", None
            o.funcs.append(f)
        elif shndx not in text_secs and bind != STB_LOCAL and kind != STT_SECTION:
            o.undefine.append(idx)  # a global in a dropped section: the asm defines it
    if len(o.funcs) + len(o.extra) != len(text_secs):
        return src, "error", f"{len(text_secs)} .text sections, {len(o.funcs) + len(o.extra)} functions", None
    unresolved = set()
    for f in o.funcs:
        err = redirect(elf, f, symbols, ctx["rom"])
        if err:
            return src, "error", err, None
        for pos, off, symidx, rtype in elf.relocs(f.shndx):
            if symbols[symidx][4] == 0:
                f.relocs[off] = symbols[symidx][0]
        disambiguate(f.addr, f.relocs, ctx, o.renames, unresolved)
    if unresolved:
        return src, "ambig", "ambiguous symbol: " + " ".join(sorted(unresolved)), None
    if is_tu:
        res = place_tu_data(elf, o, symbols, ctx["rom"], ctx["retail_unique"], o.funcs)
        if isinstance(res, str):
            # Fall back for this file: code linked, data from the asm.
            o.tu = False
            o.tu_error = res
        else:
            o.data, o.tu_notes = res
            o.data_info = {sec: (elf.secs[sec][5], elf.secs[sec][1] == SHT_NOBITS) for sec in o.data}
            o.undefine = [k for k in o.undefine if symbols[k][4] not in o.data]
            for sec in o.data:
                if elf.secs[sec][1] == SHT_NOBITS:
                    continue
                for pos, off, symidx, rtype in elf.relocs(sec):
                    word = struct.unpack_from("<I", ctx["rom"], o.data[sec] + off - match.VRAM_TO_FILE)[0]
                    o.data_relocs.append((sec, pos, off, symidx, word))
    return src, "ok", "", o


def rewrite(o, placed, dropped, multi=frozenset(), global_names=frozenset()):
    """Give each placed function its own section name, leave every other
    section out of the link, redirect references into dropped sections,
    and let references to dropped copies of inline functions bind to the
    placed copy elsewhere. Returns the absolute symbols it needs."""
    elf = Elf(o.path)
    symbols = elf.symbols()
    # Copies the linker deduplicates: inline and template functions (mwcc's
    # inline binding, or weak) and inline-bound data (vtables of classes
    # whose virtuals are all inline, inline functions' local statics) go
    # into `.gnu.linkonce.<kind>.<name>` sections in every object that has
    # one, placed or not; GNU ld keeps the first in link order (INPUT order,
    # input.ld), as retail's linker kept the first TU's.
    linkonce = {}  # section index -> linkonce name
    for f in placed + dropped:
        if f.bind in (STB_INLINE, STB_WEAK):
            linkonce[f.shndx] = linkonce_name("text", f.name)
    for name, value, size, kind, shndx, bind in symbols:
        if bind == STB_INLINE and 0 < shndx < elf.shnum and not elf.secs[shndx][2] & 4 \
                and elf.secs[shndx][2] & 2:
            if shndx in linkonce:
                sys.exit(f"link_c: {os.path.relpath(o.src, ROOT)}: two inline-bound symbols in section {shndx}")
            linkonce[shndx] = linkonce_name(elf.names[shndx].lstrip("."), name)
    o.linkonce = set(linkonce.values())
    # Data only a copy refers to (the ASSERT statics of SceneManager.cpp's
    # copies of other TUs' template members) is named like placed data, not
    # listed: where the linker discards that copy, a grouped link drops it (gc).
    if o.tu:
        o.unplaced |= copy_data(elf, symbols, o.data)
    names = dict(linkonce)
    n = 0
    for f in placed:
        if f.shndx in linkonce:
            f.section = linkonce[f.shndx]
        else:
            f.section = f".text.{n}"
            n += 1
        names[f.shndx] = f.section
    for n, f in enumerate(f for f in dropped if f.shndx not in linkonce):
        names[f.shndx] = f".text.drop{n}"
    # TU mode: placed data sections get names of their own too (and the
    # copies of shared data another TU's copy stands for, never listed).
    for n, sec in enumerate(sorted(set(o.data) | o.unplaced)):
        names[sec] = linkonce.get(sec, f".tu{elf.names[sec]}.{n}")
        if sec in o.data:
            o.data_sections[sec] = names[sec]
    elf.rename_sections(names)
    # References into sections that stay (placed functions, a TU's placed
    # data, every linkonce copy) keep their symbols; everything else goes to
    # retail's address.
    kept = {f.shndx for f in placed} | set(o.data) | set(linkonce)
    # A reference into a linkonce copy must name its symbol: if this copy is
    # discarded the symbol binds to the kept one, a section symbol would not.
    primary = {}
    for k, (name, value, size, kind, shndx, bind) in enumerate(symbols):
        if shndx in linkonce and kind != STT_SECTION and bind in (STB_INLINE, STB_WEAK, STB_GLOBAL) and value == 0:
            primary[shndx] = k
    for s in elf.secs:
        if s[1] != SHT_REL or not elf.secs[s[7]][2] & 2:
            continue
        for p in range(s[4], s[4] + s[5], 8):
            off, info = struct.unpack_from("<II", elf.data, p)
            name, value, size, kind, shndx, bind = symbols[info >> 8]
            if shndx not in linkonce:
                continue
            if kind == STT_SECTION:
                if shndx not in primary:
                    sys.exit(f"link_c: {os.path.relpath(o.src, ROOT)}: {linkonce[shndx]} has no symbol to bind by")
                elf.set_reloc_sym(p, primary[shndx])
            elif bind == STB_LOCAL:
                sys.exit(f"link_c: {os.path.relpath(o.src, ROOT)}: local {name} in {linkonce[shndx]} is referenced")
    symbols = elf.symbols()
    redirects = [(f.shndx, r[0], r[1], r[2], r[5]) for f in placed for r in f.redirects if r[3] not in kept]
    # Placed data referring to a dropped section (an inline copy linked from
    # another file) or to an ambiguous name: retail's word is the address.
    redirects += [(sec, pos, word, 0, symidx) for sec, pos, off, symidx, word in o.data_relocs
                  if (symbols[symidx][4] == 0 and symbols[symidx][0] in multi)
                  or (0 < symbols[symidx][4] < elf.shnum and symbols[symidx][4] not in kept)]
    # A named symbol another object places globally: leave the reference
    # symbolic and the symbol undefined here, so it binds to that copy.
    by_name = {r for r in redirects
               if symbols[r[4]][3] != STT_SECTION and symbols[r[4]][0] in global_names
               and symbols[r[4]][4] not in kept}
    o.undefine = sorted(set(o.undefine) | {r[4] for r in by_name})
    redirects = [r for r in redirects if r not in by_name]
    targets = sorted({r[2] for r in redirects})
    index = elf.add_undefined([f"R_{t:08X}" for t in targets])
    for sec, pos, target, mask, _ in redirects:
        elf.set_reloc_sym(pos, index[f"R_{target:08X}"])
        off = struct.unpack_from("<I", elf.data, pos)[0]
        elf.set_word(sec, off, elf.word(sec, off) & mask)
    # The symbols of every linkonce copy (inline functions, template members,
    # inline-bound data) are global, so references bind to whichever copy
    # the linker keeps; none of them is made undefined.
    inline = [k for k, (n, v, sz, kd, sh, b) in enumerate(symbols)
              if sh in linkonce and b in (STB_INLINE, STB_WEAK)]
    o.undefine = [k for k in o.undefine if symbols[k][4] not in linkonce]
    elf.rebind(sorted(set([f.symidx for f in placed if f.bind != STB_GLOBAL] + inline)), o.undefine)
    elf.save()
    # mwcc's own alignments, before they are lowered for the pinned link:
    # what a link without pins needs (build/link/align.txt).
    o.align = {f.section: elf.secs[f.shndx][8] for f in placed}
    o.align.update({n: elf.secs[sec][8] for sec, n in o.data_sections.items()})
    o.align.update({n: elf.secs[sec][8] for sec, n in linkonce.items()})
    args = sum((["--set-section-alignment", f"{f.section}=4"] for f in placed), [])
    args += sum((["--set-section-alignment", f"{n}=1"] for n in o.data_sections.values()), [])
    args += sum((["--set-section-alignment", f"{n}={4 if n.startswith(LINKONCE + 'text.') else 1}"]
                 for sec, n in linkonce.items() if n not in o.data_sections.values()
                 and n not in {f.section for f in placed}), [])
    args += sum((["--redefine-sym", f"{a}={b}"] for a, b in o.renames.items()), [])
    subprocess.run([CROSS + "objcopy", *args, o.path], check=True)
    return targets


def chunk_addr(blocks, cur_line, vram, stop=None):
    """Address where the chunk starting at line cur_line lands: the
    address of the first function block at or after that line (and before
    line `stop`, the end of its file)."""
    for name, addr, li, end in blocks:
        if li >= cur_line and (stop is None or li < stop) and addr is not None:
            return addr
    return vram


ADDR_LINE = re.compile(r"\s*/\* [0-9A-F]+ ([0-9A-F]{8})[ *]")


# crt0 and the SDK's data name the values Sony's linker script defines
# (retail's .symtab has them) by splat's guesses: an address's game symbol
# (_fbss is init$localstatic4$timeGetTime__Fv's address), D_xxx, or a
# symbol plus an offset. The link writes them by their real names, and
# aliases.ld defines them as Sony's script did.
LINKER_SYMS = {
    "init$localstatic4$timeGetTime__Fv": "_fbss",
    "D_4F5680": "_end",
    "Compress + 0x4880": "_gp",
    "D_100000": "_stack_size",
}
# crt0's own labels, by their .symtab names (the old names stay as
# aliases: libgcc's exit and newlib's abort jump to func_002000B0).
CRT0_NAMES = {"func_00200008": "_start", "func_002000B0": "_exit", "func_002000B8": "_root",
              "D_0044F800": "_args"}
# Sony's script: .bss starts at _fbss, _end is its end rounded up to 128;
# _stack and _heap_size -1 (the kernel picks), a 1 MB stack.
LINKER_DEFS = """_fbss = main_BSS_START;
_end = ALIGN(main_BSS_END, 128);
_stack = -1;
_stack_size = 0x100000;
_heap_size = -1;
"""


def linker_names(text, names):
    for old, new in names.items():
        text = re.sub(r"(?<![\w$.])" + re.escape(old) + r"(?![\w$.])", new, text)
    return text


def write_crt0(out):
    """asm/sdk/crt0/crt0.s with the linker symbols and its labels named:
    tools/link_check.sh links it in place of splat's."""
    text = linker_names(open(os.path.join(ROOT, "asm/sdk/crt0/crt0.s")).read(), {**LINKER_SYMS, **CRT0_NAMES})
    # `la $a1, _stack` and `la $a1, _heap_size` (both -1) came out as numbers.
    for name in ("_stack", "_heap_size"):
        text, n = re.subn(r"lui        \$a1, \(0x0 >> 16\)(.*?)addiu      \$a1, \$a1, -0x1",
                          rf"lui        $a1, %hi({name})\1addiu      $a1, $a1, %lo({name})", text, count=1, flags=re.S)
        if n != 1:
            sys.exit(f"link_c: crt0: no `la $a1, {name}`")
    text += "".join(f'\n.globl {old}\n.set {old}, {new}\n' for old, new in CRT0_NAMES.items() if old.startswith("func_"))
    with open(os.path.join(out, "crt0.s"), "w") as fh:
        fh.write(text)


# The game's VU microcode upload packets (assembled like .vutext, not mwcc
# data): DMA REF tags, each followed by the MPG data it points at.
VU_PACKETS = 0x3E0000
WORD_LINE = re.compile(r"(\s*/\* [0-9A-F]+ ([0-9A-F]{8}) ([0-9A-F]{8}) \*/ \.word )(.*)")


def vu_packet_words(text):
    """splat read words of the VU packets as symbols (`mapFile + 0xFB3`,
    `Initialize__4GameFPc`): microcode and VIF codes that happen to look like
    addresses. Retail keeps no relocations there; the only pointers are the
    42 REF tags' address words, each 0x20 past its tag, where the data the
    tag sends starts (and qwc*16 later the next tag). Keep those as labels
    and write every other word as the number it is, so the packets can move."""
    lines = text.split("\n")
    words = {}
    for i, l in enumerate(lines):
        m = WORD_LINE.match(l)
        if m:
            words[int(m.group(2), 16)] = (i, int.from_bytes(bytes.fromhex(m.group(3)), "little"))
    for a, (i, v) in words.items():
        m = WORD_LINE.match(lines[i])
        if not re.match(r"[A-Za-z_]", m.group(4)):
            continue
        tag = words.get(a - 4, (None, 0))[1]
        if a % 16 == 4 and (tag >> 28) & 7 == 3 and v == a - 4 + 0x20:
            continue  # a REF tag's address: a real pointer
        lines[i] = f"{m.group(1)}0x{v:08X}"
    return "\n".join(lines)


# The VU microcode (in text) and its upload packets (vupackets.s).
VU_CODE = (0x3DB000, 0x3F8E00)
ADDR_LINE = re.compile(r"\s*/\* [0-9A-F]+ ([0-9A-F]{8}) ")


def code_labels(out, names):
    """Put a global label at each of `names` ({name: address}) on the line of
    the generated asm that holds that address. Returns the names placed."""
    by_addr = collections.defaultdict(list)
    for n, a in names.items():
        by_addr[a].append(n)
    placed = set()
    for fname in sorted(os.listdir(out)):
        if not fname.endswith(".s"):
            continue
        path = os.path.join(out, fname)
        lines = open(path).read().split("\n")
        new, hit = [], False
        for l in lines:
            m = ADDR_LINE.match(l)
            if m and int(m.group(1), 16) in by_addr:
                for n in sorted(by_addr[int(m.group(1), 16)]):
                    if n not in placed:
                        new += [f".global {n}", f"{n}:"]
                        placed.add(n)
                        hit = True
            new.append(l)
        if hit:
            with open(path, "w") as fh:
                fh.write("\n".join(new))
    return placed


def plain_words(out, relocs):
    """splat also read numbers as symbols outside the VU packets: words of the
    VU microcode (`mapFile + 0x8F98`) and of libm's e_rem_pio2 table. Retail's
    .relmain covers the game and the SDK above match.RELMAIN_FIRST, so a
    symbolic .word there with no retail relocation is a number: write it as
    one, so nothing ties it to the symbol's address. vupackets.s keeps its REF
    tags (vu_packet_words) and glue.o its `.word _end`. Returns the count."""
    n = 0
    for name in sorted(os.listdir(out)):
        if not name.endswith(".s") or name == "vupackets.s":
            continue
        path = os.path.join(out, name)
        lines = open(path).read().split("\n")
        changed = False
        for i, l in enumerate(lines):
            m = WORD_LINE.match(l)
            if not m or not re.match(r"[A-Za-z_]", m.group(4)) or m.group(4).split()[0] in LINKER_SYMS.values():
                continue
            a = int(m.group(2), 16)
            if a in relocs or match.no_retail_relocs(a):
                continue
            lines[i] = f"{m.group(1)}0x{m.group(3)[6:8]}{m.group(3)[4:6]}{m.group(3)[2:4]}{m.group(3)[0:2]}"
            changed = True
            n += 1
        if changed:
            with open(path, "w") as fh:
                fh.write("\n".join(lines))
    return n


# What the Metrowerks linker generated between the data and .sdata: the
# exception table index (empty: retail kept no exception tables), padding
# to 16, the overlay group table (one entry, the image's start) and
# __data_end, then zeros to .sdata's 128-aligned start. Written here as
# linker-script statements in place of the asm.
EXC_INDEX = 0x44E904
EXC_INDEX_LD = """    __exception_table_start__ = .;
    __exception_table_end__ = .;
    . = ALIGN(16);
    _overlay_group_addreses = .;
    LONG(main_VRAM);
    __data_end = .;
    . = ALIGN(128);"""
# Linker symbols defined next to the sections they bound, by retail's
# values: (name, `start` (before the section placed there) or `end`
# (after the section ending there), offset from that point). _gp is
# 0x7FF0 into .sdata, whose first section is VehicleAI.cpp's.
DATA_SYMS = [("__static_init", "start", 0), ("__static_init_end", "end", 0), ("_gp", "start", 0x7FF0)]


def load_sdk_data():
    """config/sdk_data.txt: [(address, kind, end,
    object, note)] for every SDK object's .data/.rodata/.bss section and
    COMMON symbol (its note is the symbol's name)."""
    rows = []
    for line in open(os.path.join(ROOT, "config/sdk_data.txt")):
        body, _, note = line.partition("#")
        w = body.split()
        if w:
            rows.append((int(w[0], 16), w[1], int(w[2], 16), w[3], note.strip()))
    return rows


placed_syms = set()  # DATA_SYMS cut_data() assigned in a fragment


def cut_data(lines, secs, rom, out, first, rel, sdk=None, syms=None):
    """TU mode: cut a data asm file (splat's .rodata or .data) around the
    placed TU sections that fall inside it. Returns (chunks written,
    linker-script fragment lines, {label cut out: address}, SDK files
    written). The asm between TU ranges is also cut at every SDK
    section's start (`sdk`, {address: (kind, object)}): each SDK section
    becomes its own file, sdkdata_<lib>_<obj>_<kind>.s, under its real
    section name, so the SDK's data sits in per-object pieces in link
    order like the TUs'. Each range a
    TU's sections cover is widened over zero bytes until both ends are
    item boundaries (outside any dlabel..enddlabel span, so the asm stays
    well-formed), the end an 8-aligned one (it starts the next chunk,
    whose `.align 3`s count from there); the widened bytes come back as
    the link's zero fill. Where a piece of the asm is all zeros up to a
    128-aligned address (an output section's alignment) it becomes
    `. = ALIGN(128)`, and the linker-generated block at EXC_INDEX becomes
    EXC_INDEX_LD. `syms` ({address: [(name, start/end, offset)]})
    are assigned next to the TU section starting or ending there."""
    sec_line = next(i for i, l in enumerate(lines) if l.startswith(".section "))
    out_sec = lines[sec_line].split()[1].rstrip(",")
    header = lines[:sec_line + 1]
    addr_at, boundary = {}, {}  # line -> address; address -> line, for item boundaries
    open_item, seen = None, False
    for i, l in enumerate(lines):
        m = ADDR_LINE.match(l)
        if m:
            a = int(m.group(1), 16)
            addr_at[i] = a
            if open_item is None or not seen:
                boundary.setdefault(a, i)
            seen = True
            continue
        w = l.split()
        if w[:1] == ["dlabel"]:
            open_item, seen = w[1], False
        elif w[:1] == ["enddlabel"]:
            open_item = None
    addrs = sorted(addr_at.values())
    lo_file, hi_file = addrs[0], addrs[-1]
    mine = [t for t in secs if lo_file <= t[0] <= hi_file]
    obj_line = f"    build/{rel}.o({out_sec});"
    if not mine:
        return 0, ["    /* generated by tools/link_c.py */", obj_line], {}, []

    def zero(a, b):
        return not any(rom[a - match.VRAM_TO_FILE:b - match.VRAM_TO_FILE])

    # Group the sections into contiguous ranges.
    ranges = []
    for t in mine:
        a, size = t[0], t[1]
        if ranges and a - ranges[-1][1] < 16 and a >= ranges[-1][1] and zero(ranges[-1][1], a):
            ranges[-1][1] = max(ranges[-1][1], a + size)
            ranges[-1][2].append(t)
        else:
            ranges.append([a, a + size, [t]])
    bounds = sorted(boundary)

    def widen(a, down):
        cands = [b for b in (reversed(bounds) if down else bounds) if (b <= a if down else b >= a)]
        fallback = None
        for b in cands:
            if abs(b - a) > 64 or not zero(min(a, b), max(a, b)):
                break
            if down or b % 8 == 0:  # only a chunk's start must be 8-aligned
                return b
            if fallback is None and not down and b % 4 == 0:
                fallback = b
        if not down and zero(a, hi_file + 4):
            return hi_file + 4  # zeros to the end of the file: padded after the last section
        if fallback is not None:
            # No 8-aligned boundary in reach (two adjacent .ctor slots
            # ending at 4 mod 8, a vtable followed by a non-zero item): the
            # next chunk starts 4-aligned, and flush() turns its `.align`s
            # into `.org`s at the retail offsets.
            return fallback
        sys.exit(f"link_c: {rel}: no 8-aligned item boundary in zero bytes "
                 f"{'below' if down else 'above'} {a:#x}")

    def first_line(i):
        """Walk back from the address line i over the labels and directives
        that belong to its item."""
        while i > sec_line + 1:
            w = lines[i - 1].split()
            if not w or w[0] in (".align", "nonmatching", "dlabel") or lines[i - 1].startswith("/* Automatically"):
                i -= 1
            else:
                break
        return i

    ld, labels, n, cur = ["    /* generated by tools/link_c.py */"], {}, 0, sec_line + 1
    sdk_files = []
    sdk_starts = {}  # first line of an SDK section's item -> (address, kind, object)
    for a, (kind, obj) in (sdk or {}).items():
        if lo_file <= a <= hi_file:
            if a not in boundary:
                sys.exit(f"link_c: {rel}: SDK {obj} {kind} at {a:#x} is not an item boundary")
            sdk_starts[first_line(boundary[a])] = (a, kind, obj)

    def flush(upto):
        lo = cur
        for i in sorted(k for k in sdk_starts if cur < k < upto) + [upto]:
            piece(lo, i)
            lo = i

    def piece(lo, upto):
        nonlocal n
        body = lines[lo:upto]
        a = next((addr_at[i] for i in range(lo, upto) if i in addr_at), None)
        if a is None:
            return
        end = next((addr_at[i] for i in range(upto, len(lines)) if i in addr_at), hi_file + 4)
        if a == EXC_INDEX:
            ld.append(f"    . = {a - VRAM:#x};")
            ld.extend(EXC_INDEX_LD.split("\n"))
            return
        if end % 128 == 0 and not any(rom[a - match.VRAM_TO_FILE:end - match.VRAM_TO_FILE]) \
                and not any(l.split()[:1] == ["dlabel"] for l in body):
            ld.append(f"    . = {a - VRAM:#x};")
            ld.append("    . = ALIGN(128);  /* the next output section's alignment */")
            return
        if a % 8:
            # `.align N` pads relative to the chunk's start, which as
            # takes to be 2^N-aligned; this one isn't, so pad to each
            # item's retail offset instead.
            body = list(body)
            for k, l in enumerate(body):
                if l.split()[:1] == [".align"]:
                    nxt = next((addr_at[i] for i in range(lo + k, upto) if i in addr_at), None)
                    # (a trailing one: the next pinned section pads)
                    body[k] = f".org {nxt - a:#x}" if nxt is not None else ""
        if a == VU_PACKETS:
            path, hdr, sec = os.path.join(out, "vupackets.s"), header, out_sec
        elif lo in sdk_starts:
            sa, kind, obj = sdk_starts[lo]
            if sa != a:
                sys.exit(f"link_c: {rel}: SDK {obj} {kind} starts at {a:#x}, not {sa:#x}")
            path = os.path.join(out, f"sdkdata_{obj.replace('/', '_')}_{kind[1:]}.s")
            flags = '"wa"' if kind == ".data" else '"a"'
            hdr, sec = header[:-1] + [f".section {kind}, {flags}"], kind
            sdk_files.append(path)
        else:
            path, hdr, sec = os.path.join(out, f"dchunk_{first + n:04d}.s"), header, out_sec
            n += 1
        text = "\n".join(hdr + body) + "\n"
        if lo in sdk_starts:
            text = linker_names(text, LINKER_SYMS)  # (glue.o's heap start, &_end)
        elif a == VU_PACKETS:
            text = vu_packet_words(text)
        with open(path, "w") as fh:
            fh.write(text)
        ld.append(f"    . = {a - VRAM:#x};")
        ld.append(f"    {os.path.relpath(path, ROOT)}.o({sec});")

    for lo, hi, ts in ranges:
        lo, hi = widen(lo, True), (widen(hi, False) if hi <= hi_file else hi)
        start = first_line(boundary[lo])
        end = first_line(boundary[hi]) if hi in boundary else len(lines)
        if start < cur:
            sys.exit(f"link_c: {rel}: TU data ranges overlap at {lo:#x}")
        flush(start)
        for i in range(start, end):
            w = lines[i].split()
            if w[:1] == ["dlabel"]:
                j = next(j for j in range(i, len(lines)) if j in addr_at)
                labels[w[1]] = addr_at[j]
        for a, size, nobits, o, name in ts:
            ld.append(f"    . = {a - VRAM:#x};  /* {os.path.relpath(o.src, ROOT)} */")
            for sym, when, off in (syms or {}).get(a, []):
                if when == "start":
                    ld.append(f"    {sym} = . + {off:#x};" if off else f"    {sym} = .;")
                    placed_syms.add(sym)
            ld.append(f"    {script_line(o, name)};")
            for sym, when, off in (syms or {}).get(a + size, []):
                if when == "end":
                    ld.append(f"    {sym} = .;")
                    placed_syms.add(sym)
            if not hasattr(o, "_cut"):
                o._cut = set()
            o._cut.add(name)
        cur = end
    flush(len(lines))
    if cur == len(lines) and ranges and ranges[-1][1] <= hi_file + 4:
        # The last TU range ran over the zeros to the end of the file:
        # nothing after it pads them, so pad here (the bytes are in the
        # image, and the file's size and what follows depend on them).
        ld.append(f"    . = {hi_file + 4 - VRAM:#x};")
    return n, ld, labels, sdk_files


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("-j", type=int, default=os.cpu_count() or 4)
    ap.add_argument("--out", default=os.path.join(ROOT, "build/link"))
    ap.add_argument("--only", nargs="*", help="link only these sources")
    args = ap.parse_args()
    for d in ("obj", "tmp"):
        os.makedirs(os.path.join(args.out, d), exist_ok=True)

    syms = match.load_symbols()
    rom = open(match.BASEROM, "rb").read()
    retail_names = match.load_retail_names(rom)
    count = {}
    for names in retail_names.values():
        for n in set(names):
            count[n] = count.get(n, 0) + 1
    ctx = {"rom": rom, "retail_relocs": match.load_retail_relocs(rom), "ld_names": load_ld_names(),
           "multi": {n for n, c in count.items() if c > 1},
           "retail_unique": {n: a for a, names in retail_names.items() for n in names if count[n] == 1}}
    # All game asm files as one list of lines; files[i] = (path, first
    # line, end line) so blocks keep global line numbers.
    lines, blocks, files = [], [], []
    for rel in GAME_ASM:
        flines = open(os.path.join(ROOT, rel)).read().split("\n")
        base = len(lines)
        blocks += [(name, addr, li + base, end + base) for name, addr, li, end in parse_blocks(flines)]
        lines += flines
        files.append((rel, base, len(lines)))
    by_name = {name: k for k, (name, addr, li, end) in enumerate(blocks)}

    if args.only:
        sources = args.only
    else:
        r = subprocess.run(["grep", "-rl", "^// MATCHED (", os.path.join(ROOT, "src")],
                           capture_output=True, text=True)
        sources = sorted(r.stdout.split())

    with cf.ThreadPoolExecutor(args.j) as ex:
        results = list(ex.map(lambda s: compile_one(s, args.out, syms, ctx), sources))

    counts, report, objs = {}, [], []
    for src, status, detail, o in results:
        if status != "ok":
            counts[status] = counts.get(status, 0) + 1
            report.append(f"{status:6} {os.path.relpath(src, ROOT)}: {detail}")
        else:
            objs.append(o)
            if getattr(o, "tu_error", None):
                counts["tu-fallback"] = counts.get("tu-fallback", 0) + 1
                report.append(f"tudata {os.path.relpath(src, ROOT)}: data not placed, code only: {o.tu_error}")
            for note in getattr(o, "tu_notes", []):
                if not note.endswith(LINKONCE_NOTE):
                    report.append(f"tudrop {os.path.relpath(src, ROOT)}: {note}")

    # Link order (INPUT order, input.ld): the objects by their first own
    # function's address, data-only TUs last. Of the linkonce copies of an
    # inline function or inline-bound data the linker keeps the first in it.
    def link_key(o):
        own = [f.addr for f in o.funcs if f.bind == STB_GLOBAL]
        return (min(own) if own else 1 << 32, o.src)
    link_order = {id(o): i for i, o in enumerate(sorted(objs, key=link_key))}

    # Two TU objects can hold a copy of the same retail data object: an
    # inline function's local static (GameText::GetString's, in every TU
    # that inlines it), an inline-bound vtable or RTTI record, an inline's
    # literal. Retail's linker kept one copy, at the place of the TU it came
    # from: place the copy of the TU whose own (unshared) sections lie
    # nearest (MotionLibrary.cpp's "" after KeyFrameController.cpp's RTTI,
    # TextureMap.cpp's RTTI block that LightFlare.cpp shares), the first by
    # path on a tie, and let the others' references go to retail's address
    # like any unplaced section.
    holders = collections.defaultdict(list)
    for o in objs:
        if o.tu:
            for a in o.data.values():
                holders[a].append(o)
    def distance(o, a):
        own = [x for x in o.data.values() if len(holders[x]) == 1]
        return min((abs(x - a) for x in own), default=1 << 32)
    # Inline-bound data (a symbol with mwcc's inline binding: rewrite() puts
    # every copy in a linkonce section) is listed once, as `*(name)`: the
    # linker keeps the first copy in link order, so that is the one listed.
    def inline_bound(o, sec):
        return any(b == STB_INLINE and sh == sec for n, v, sz, k, sh, b in o.symbols)
    claimed = {}
    for a, os_ in holders.items():
        if len(os_) > 1:
            once = any(inline_bound(o, s) for o in os_ for s, b in o.data.items() if b == a)
            claimed[a] = min(os_, key=lambda o: link_order[id(o)] if once else distance(o, a))
    for o in objs:
        if not o.tu:
            continue
        for sec in sorted(o.data):
            a = o.data[sec]
            if claimed.setdefault(a, o) is o:
                continue
            del o.data[sec]
            o.data_info.pop(sec, None)
            o.data_relocs = [r for r in o.data_relocs if r[0] != sec]
            if inline_bound(o, sec):
                continue
            o.unplaced.add(sec)  # named like placed data, so a grouped link can drop it (gc)
            report.append(f"tudrop {os.path.relpath(o.src, ROOT)}: data section {sec} at {a:#x}: "
                          f"also in {os.path.relpath(claimed[a].src, ROOT)}, placed from there")

    # Choose one object per function. Globals first (a file dedicated to a
    # function beats an inline copy that happened to be emitted elsewhere);
    # then local copies of inline functions fill blocks nobody else claims.
    chosen = {}  # block index -> (Func, Obj)
    plan = {}    # Obj -> (placed, dropped)
    for o in objs:
        ks = [by_name.get(f.key) for f in o.funcs]
        if any(k is None or blocks[k][1] != f.addr for k, f in zip(ks, o.funcs)):
            counts["noblk"] = counts.get("noblk", 0) + 1
            report.append(f"noblk  {os.path.relpath(o.src, ROOT)}: a function has no game asm block at its address")
            continue
        placed, dropped = [], list(o.extra)
        for k, f in zip(ks, o.funcs):
            if f.bind != STB_GLOBAL:
                dropped.append(f)  # an inline copy: decided in the second pass
            elif k in chosen:
                # The same function matched in two files: the first file
                # (sorted by path) supplies it; this copy stays out, and
                # its symbol goes undefined so the names don't collide.
                counts["dup"] = counts.get("dup", 0) + 1
                report.append(f"dup    {os.path.relpath(o.src, ROOT)}: {f.key} already from "
                              f"{os.path.relpath(chosen[k][1].src, ROOT)}")
                dropped.append(f)
                o.undefine.append(f.symidx)
            else:
                chosen[k] = (f, o)
                placed.append(f)
        plan[o] = (placed, dropped)
    absolute = set()
    # An inline (weak) copy goes first to the TU whose span holds its
    # address: from the TU's first global function up to the next object's
    # (a TU's template members follow its last global). Its asserts then
    # point at that TU's strings (JetskiAudio.cpp's ContainerList<PSXSound *>
    # copies sort before PSXAudio.cpp's, which retail kept).
    starts = sorted((f.addr, id(o)) for o, (placed, _) in plan.items() for f in placed)
    for o, (placed, dropped) in plan.items():
        if not placed or not o.tu:
            continue
        lo = min(f.addr for f in placed)
        top = max(f.addr for f in placed)
        hi = next((a for a, oid in starts if a > top and oid != id(o)), 1 << 32)
        for f in list(dropped):
            if f.key is None or not lo <= f.addr < hi:
                continue
            k = by_name[f.key]
            if k not in chosen:
                chosen[k] = (f, o)
                placed.append(f)
                dropped.remove(f)
    # Any other copy: the first in link order, the one the linker keeps
    # (MWRTTI.cpp's std::exception dtor, before its first global function).
    for o, (placed, dropped) in sorted(plan.items(), key=lambda kv: link_order[id(kv[0])]):
        for f in list(dropped):
            if f.key is None:
                continue
            k = by_name[f.key]
            if k not in chosen:
                chosen[k] = (f, o)
                placed.append(f)
                dropped.remove(f)
        placed.sort(key=lambda f: f.addr)
    # Every name some object places: references to another copy of one of
    # them (an inline function, thunk or template member, inline-bound data)
    # bind to it by name instead of by retail's address.
    global_names = set()
    for o, (placed, dropped) in plan.items():
        global_names.update(f.name for f in placed)
        if o.tu and o.data:
            global_names.update(n for n, v, sz, kd, sh, b in Elf(o.path).symbols()
                                if b == STB_INLINE and sh in o.data)
    for o, (placed, dropped) in plan.items():
        if placed or (o.tu and o.data):  # (a data-only TU, src/tu/MapFile.cpp, links for its data)
            counts["ok"] = counts.get("ok", 0) + 1
            absolute.update(rewrite(o, placed, dropped, ctx["multi"], global_names))
        else:
            counts["empty"] = counts.get("empty", 0) + 1
            report.append(f"empty  {os.path.relpath(o.src, ROOT)}: every function already linked from elsewhere")
    linked = {o for o, (placed, dropped) in plan.items() if placed or (o.tu and o.data)}
    # The link order, explicit: link_check.sh puts this first in the script,
    # so the linker loads the objects in this order whatever the script
    # names first.
    with open(os.path.join(args.out, "input.ld"), "w") as fh:
        fh.write("/* tools/link_c.py: link order (objects by first function; data-only TUs last) */\nINPUT(\n")
        for o in sorted(linked, key=lambda o: link_order[id(o)]):
            fh.write(f"    {os.path.relpath(o.path, ROOT)}\n")
        fh.write(")\n")

    tu_secs = []  # (address, size, nobits, Obj, section name in the link)
    for o in linked:
        if o.tu:  # (objcopy renumbered the sections: go by size and name)
            for sec, a in o.data.items():
                tu_secs.append((a, *o.data_info[sec], o, o.data_sections[sec]))
    tu_secs.sort(key=lambda t: t[0])
    listed_once = {n for n in {f.section for f, o in chosen.values()} | {t[4] for t in tu_secs}
                   if n.startswith(LINKONCE)}
    counts["linkonce"] = len(listed_once)  # sections listed as *(name): the linker picks the copy
    dchunks = 0

    # Cut each game asm file into chunks around the chosen blocks.
    chunk_srcs = []
    cut_labels = {}  # jlabel/alabel/ehlabel inside cut blocks: data refers to them
    for rel, fstart, fend in files:
        header = lines[fstart:lines.index('.section .text, "ax"', fstart) + 1]
        ld = ["    /* generated by tools/link_c.py */"]
        cur = fstart + len(header) - 1  # first line after the header
        if rel == "asm/text_island.s":
            # The island opens with data: the .rodata (strings, RTTI) of
            # the text_late TUs, up to the `.align 3` before its first
            # function. Cut it like a data file around TU sections there.
            dend = next(i for i in range(fstart, fend) if re.match(r"(?:alabel|glabel) ", lines[i]))
            while lines[dend - 1] != ".align 3":
                dend -= 1
            sub = lines[fstart:dend]
            secs = [t for t in tu_secs if not t[2]]
            n, dld, labels, _ = cut_data(sub, secs, rom, args.out, dchunks, rel)
            # n == 0 with sections listed: TU sections cover the whole
            # island head (no asm chunk left), which still has to be placed.
            if n or len(dld) > 2:
                dchunks += n
                cut_labels.update(labels)
                ld += dld[1:]
                cur = dend

        def flush_chunk(upto, addr):
            if upto <= cur:
                return
            path = os.path.join(args.out, f"chunk_{len(chunk_srcs):04d}.s")
            with open(path, "w") as fh:
                fh.write("\n".join(header + lines[cur:upto]) + "\n")
            chunk_srcs.append(path)
            ld.append(f"    . = {addr - VRAM:#x};")
            ld.append(f"    {os.path.relpath(path, ROOT)}.o(.text);")

        for k in sorted(chosen):
            f, o = chosen[k]
            _, addr, li, end = blocks[k]
            if not fstart <= li < fend:
                continue
            align_line = li  # the chunk before ends at the `.align 3` opening this block
            while lines[align_line] != ".align 3":
                align_line -= 1
            flush_chunk(align_line + 1, chunk_addr(blocks, cur, VRAM, fend))
            ld.append(f"    . = {addr - VRAM:#x};  /* {f.key} ({os.path.relpath(o.src, ROOT)}) */")
            ld.append(f"    {script_line(o, f.section)};")
            for i in range(li, end):
                m = re.match(r"\s*(?:jlabel|alabel|ehlabel) (\S+)", lines[i])
                if m:
                    j = i + 1
                    while j < end and not re.match(r"\s*/\* \w+ ([0-9A-F]{8}) ", lines[j]):
                        j += 1
                    if j < end:
                        cut_labels[m.group(1)] = int(re.match(r"\s*/\* \w+ ([0-9A-F]{8}) ", lines[j]).group(1), 16)
            cur = end
        flush_chunk(fend, chunk_addr(blocks, cur, VRAM, fend))

        frag = os.path.splitext(os.path.basename(rel))[0] + ".ld"  # text.ld, text_late.ld, text_island.ld
        with open(os.path.join(args.out, frag), "w") as fh:
            fh.write("\n".join(ld) + "\n")

    # TU mode: cut the data asm around each TU's placed data, and put its
    # .bss into .main_bss.
    linker_values = match.load_linker_symbols(rom)
    data_syms = {}
    for name, when, off in DATA_SYMS:
        data_syms.setdefault(linker_values[name] - off, []).append((name, when, off))
    sdk_rows = load_sdk_data()
    sdk = {a: (kind, obj) for a, kind, end, obj, note in sdk_rows if kind in (".data", ".rodata")}
    sdk_files = []
    for rel in DATA_ASM:
        dlines = open(os.path.join(ROOT, rel)).read().split("\n")
        secs = [t for t in tu_secs if not t[2]]
        n, ld, labels, files_ = cut_data(dlines, secs, rom, args.out, dchunks, rel, sdk, data_syms)
        dchunks += n
        sdk_files += files_
        cut_labels.update(labels)
        with open(os.path.join(args.out, os.path.basename(rel).split(".")[0] + ".ld"), "w") as fh:
            fh.write("\n".join(ld) + "\n")  # rodata.ld, data.ld
    missing = [t for t in tu_secs if not t[2] and t[4] not in getattr(t[3], "_cut", set())]
    for a, size, nobits, o, name in missing:
        sys.exit(f"link_c: {os.path.relpath(o.src, ROOT)} {name} at {a:#x} is in no data asm file")
    # The SDK's .bss (and errno, COMMON): one file per object section,
    # defining the labels the asm addresses there, which then leave the
    # absolute definitions (undefined_syms.txt below).
    bss = [(a, f"{script_line(o, name)};", os.path.relpath(o.src, ROOT))
           for a, size, nobits, o, name in tu_secs if nobits]
    und_lines = open(os.path.join(ROOT, "undefined_syms_auto.txt")).read().splitlines()
    und = {}
    for line in und_lines:
        m = re.match(r"(\S+) = 0x([0-9A-Fa-f]+);", line)
        if m:
            und[m.group(1)] = int(m.group(2), 16)
    sdk_defined = set()
    for a, kind, end, obj, note in sdk_rows:
        if kind not in (".bss", "COMMON"):
            continue
        path = os.path.join(args.out, f"sdkdata_{obj.replace('/', '_')}_{kind.lstrip('.').lower()}.s")
        body = [".include \"macro.inc\"", ""]
        if kind == "COMMON":
            body.append(f".comm {note}, {end - a}, 4")
            sdk_defined.add(note)
            sec = "COMMON .scommon"  # (as makes a 4-byte .comm small common)
        else:
            body.append(".section .bss, \"wa\", @nobits")
            off = a
            for name, v in sorted(((n, v) for n, v in und.items() if a <= v < end), key=lambda t: t[1]):
                if v > off:
                    body.append(f"    .space {v - off:#x}")
                body.append(f"dlabel {CRT0_NAMES.get(name, name)}")
                sdk_defined.add(name)
                off = v
            body.append(f"    .space {end - off:#x}")
            sec = ".bss"
        with open(path, "w") as fh:
            fh.write("\n".join(body) + "\n")
        sdk_files.append(path)
        bss.append((a, f"{os.path.relpath(path, ROOT)}.o({sec});", obj))
    write_crt0(args.out)
    # Splat's absolute definitions of .bss names: drop the SDK labels the
    # sdkdata files now define, and every name inside a placed TU section
    # (its object defines it; an assignment in the script would override the
    # object's symbol and pin it to retail's address).
    tu_spans = sorted((a, a + size) for a, size, nobits, o, name in tu_secs)
    tu_starts = [s for s, _ in tu_spans]

    def in_tu(v):
        i = bisect.bisect_right(tu_starts, v) - 1
        return i >= 0 and tu_spans[i][0] <= v < tu_spans[i][1]
    with open(os.path.join(args.out, "undefined_syms.txt"), "w") as fh:
        fh.write("\n".join(l for l in und_lines
                           if not (m := re.match(r"(\S+) = (0x[0-9A-Fa-f]+);", l))
                           or (m.group(1) not in sdk_defined and not in_tu(int(m.group(2), 16)))) + "\n")
    with open(os.path.join(args.out, "bss.ld"), "w") as fh:
        fh.write("    /* generated by tools/link_c.py: TU-mode and SDK .bss */\n")
        for a, line, src in sorted(bss):
            if a < BSS_VRAM:
                sys.exit(f"link_c: {src} {line} at {a:#x} is below .bss")
            fh.write(f"    . = {a - BSS_VRAM:#x};  /* {src} */\n    {line}\n")
    with open(os.path.join(args.out, "tudata.txt"), "w") as fh:
        for a, size, nobits, o, name in tu_secs:
            fh.write(f"{a:#x} {size:#x} {name} {os.path.relpath(o.src, ROOT)}\n")
    with open(os.path.join(args.out, "objs.txt"), "w") as fh:
        for k in sorted(chosen):
            f, o = chosen[k]
            fh.write(f"{f.key} {blocks[k][1]:#x} {os.path.relpath(o.src, ROOT)} "
                     f"{os.path.relpath(o.path, ROOT)} {f.section}\n")

    # The asm spells `<`, `>` and `@` in symbol names as _LT_, _GT_, _AT_
    # (tools/extract_symbols.py); C objects use the real names. Alias each
    # way as needed. Symbols the asm never names (splat's D_xxx labels for
    # retail's microcode symbols, say) get their retail address.
    defined, undefined, all_und = set(), set(), set()
    for o in linked:
        elf = Elf(o.path)
        for name, value, size, kind, shndx, bind in elf.symbols():
            if 0 < shndx < len(elf.names) and elf.names[shndx].startswith(LINKONCE) \
                    and elf.names[shndx] not in listed_once:
                continue  # a copy of nothing the script places (dropped with its symbols)
            if 0 < shndx < len(elf.names) and elf.names[shndx].startswith((".tu.", LINKONCE)):
                if bind == STB_LOCAL:
                    continue  # a TU's placed data: only its globals need aliases
            elif not name or (0 < shndx < len(elf.names) and not elf.names[shndx].startswith(".text.")) \
                    or (0 < shndx < len(elf.names) and elf.names[shndx].startswith(".text.drop")):
                continue  # dropped sections are discarded, symbols included
            if shndx == 0:
                all_und.add(name)
            if re.search(r"[<>@]", name):
                (undefined if shndx == 0 else defined).add(name)
    retail_addr = {}
    for addr, names in retail_names.items():
        for n in names:
            retail_addr.setdefault(n, addr)
    # The VU microprograms' names (StripLitCode ...) that game code loads:
    # global labels in the microcode chunk and vupackets.s, not PROVIDEs.
    vu_names = {n: retail_addr[n] for n in all_und - undefined
                if n not in syms and n in retail_addr and VU_CODE[0] <= retail_addr[n] < VU_CODE[1]}
    labeled = code_labels(args.out, vu_names)
    with open(os.path.join(args.out, "aliases.ld"), "w") as fh:
        # Outside SECTIONS: inside an output section a number is an offset.
        fh.write(LINKER_DEFS)
        for name, when, off in DATA_SYMS:  # (their section isn't linked: --only)
            if name not in placed_syms:
                fh.write(f"{name} = {linker_values[name]:#x};\n")
        for t in sorted(absolute):
            fh.write(f"R_{t:08X} = {t:#x};\n")
        for name, addr in sorted(cut_labels.items()):
            fh.write(f'PROVIDE("{name}" = {addr:#x});\n')
        for name in sorted(defined):
            fh.write(f'{match.sanitize(name)} = "{name}";\n')
        for name in sorted(undefined - defined):
            fh.write(f'"{name}" = {match.sanitize(name)};\n')
        for name in sorted(all_und - undefined):
            if name not in syms and name in retail_addr and not name.startswith("R_") and name not in labeled:
                fh.write(f'PROVIDE("{name}" = {retail_addr[name]:#x});\n')
    counts["plain"] = plain_words(args.out, ctx["retail_relocs"])
    # A name the generated asm defines as a label (the VU microcode's
    # D_003DB0D0 ...) must not also be assigned an address in the script.
    labels = set()
    for fname in os.listdir(args.out):
        if fname.endswith(".s"):
            labels.update(re.findall(r"^(?:dlabel|glabel|jlabel)\s+(\S+)", open(os.path.join(args.out, fname)).read(), re.M))
    for root, _, files in os.walk(os.path.join(ROOT, "asm", "sdk")):
        for fname in files:
            if fname.endswith(".s"):
                labels.update(re.findall(r"^\s*(?:alabel|glabel)\s+(\S+)", open(os.path.join(root, fname)).read(), re.M))
    for src, dst in (("undefined_syms.txt", os.path.join(args.out, "undefined_syms.txt")),
                     ("undefined_funcs_auto.txt", os.path.join(args.out, "undefined_funcs.txt"))):
        text = open(dst if src == "undefined_syms.txt" else os.path.join(ROOT, src)).read()
        keep = [l for l in text.splitlines() if not (m := re.match(r"(\S+) = ", l)) or m.group(1) not in labels]
        with open(dst, "w") as fh:
            fh.write("\n".join(keep) + "\n")
    with open(os.path.join(args.out, "align.txt"), "w") as fh:
        for o in sorted(linked, key=lambda o: o.path):
            for name, a in sorted(getattr(o, "align", {}).items()):
                if not name.startswith(LINKONCE):
                    fh.write(f"{os.path.relpath(o.path, ROOT)} {name} {a}\n")
        once = {}
        for o in linked:
            for name, a in getattr(o, "align", {}).items():
                if name.startswith(LINKONCE):
                    once[name] = max(a, once.get(name, 0))
        for name, a in sorted(once.items()):
            fh.write(f"* {name} {a}\n")
    with open(os.path.join(args.out, "report.txt"), "w") as fh:
        fh.write("\n".join(sorted(report)) + "\n")
    total = sum(syms[chosen[k][0].key][1] for k in chosen)
    tu_files = sum(1 for o in linked if o.tu)
    print(f"linkable: {len(chosen)} functions from {counts.get('ok', 0)} files, {total:#x} bytes, "
          f"{len(chunk_srcs)} asm chunks, {len(absolute)} redirected data addresses; "
          f"TU mode: {tu_files} files, {len(tu_secs)} data sections placed, {dchunks} data chunks; "
          f"SDK data: {len(sdk_files)} sections; "
          + ", ".join(f"{k} {v}" for k, v in sorted(counts.items())))
    print(f"details: {os.path.relpath(args.out, ROOT)}/report.txt")


if __name__ == "__main__":
    main()
