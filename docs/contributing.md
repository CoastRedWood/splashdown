# Contributing

Every game function matches, so work here means changing matched code without breaking it, or extending the ports. The README covers setup.

## How the source is laid out

- **`src/tu/<Name>.cpp`**: one file per retail translation unit (TU), with its functions in retail order followed by its data and static initializer. zlib is in `src/tu/zlib/` and the MSL C++ runtime is in `src/tu/msl/`.
- **The file header** records where the TU sits in retail and how it was built:
  ```cpp
  // Water.cpp  @ 0x002F6760 (text 0x2F6760-0x2F7EF0)
  // MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
  ```
  `tools/match.py` compiles with exactly the flags and mode in the `MATCHED` line. A `// compiler: <id>` line picks another build from `toolchain/`, and the zlib files use one to get `mwcps2-2.3-zlib`.
- **`src/<Class>/<Class>.h`**: shared class headers, named from the retail symbol table. Many TUs include each one, so a layout change can break dozens of files. Run `make match` after any header edit.
- **`include/`**: SDK and compiler headers. `soultree_assert_block.h` holds retail's `ASSERT` forms. `ps2_port.h` and `mwcc_va.h` hold the macros that let the same source build for the PC.
- **`config/`**: `splat.yaml` splits the baserom. `link_order.txt` is the object order `make relink` links from. `sdk_data.txt` lists the SDK's data sections.

## The checks

Run these before every commit. All of them need your baserom and toolchain (see the README).

| Command | Checks |
|---|---|
| `make` | the split asm reassembles to the retail SHA-1 |
| `make match` | every `MATCHED` file still compiles to retail's bytes, relocations and literals included |
| `make link` | the C objects, code and data, link to the retail SHA-1. This catches what per-function matching can't: jump-table order, merged string literals, data placement |
| `make relink` | (after `make link`) the link from `config/link_order.txt` alone gives the same SHA-1, and a link with padding inserted moves every reference correctly |
| `python3 tools/port/build.py -j 8` | the PC port still builds (when you touch `src/`, `include/` or `ports/`) |
| `python3 tools/port/golden.py check` | the port's deterministic session still draws the same pixels. Run `record` once first: the reference lives in `build/`, which is not committed |

`tools/match.py src/tu/Water.cpp` checks one file (`-v` for detail).

## Rules

- **The matching build never changes for a port.** PC-only code goes in `ports/` or behind `#ifdef SPLASHDOWN_PC`. The macros in `include/ps2_port.h` cover pointer-sized fields.
- **Order matters inside a TU.** Functions, literals, vtables and data come out in the order retail's compiler saw them. Moving a definition can reorder sections even when every function still matches, and `make link` will catch it.
- **Line numbers are code.** `ASSERT` compiles `__LINE__` into retail's bytes, so adding or removing a line above an assert breaks the match. Edit comments in place.
- **Renaming a local or reordering two statements can change register allocation.** Check with `make match`.
- **Nothing from the disc gets committed:** no baserom, asm, assets or extracted data.

## Tools

| Tool | Use |
|---|---|
| `match.py FILE [-v]` | compile a file and byte-compare every function against retail |
| `link_c.py`, `link_check.sh` | `make link`: place each TU's code and data at retail's addresses |
| `relink.py [--shift T,D,R,B]` | `make relink`: the address-free link and the shift test (with `reloc_audit.py`) |
| `extract_symbols.py` | recover the retail symbol table |
| `mwcc_zlib/mkcc.py` | build the patched compiler the zlib TUs use |
| `port/build.py` | build the PC port (runs `port/rewrite.py` and `port/vu.py`) |
| `port/golden.py` | check the port's pixels against a recorded run |
