# Makefile — Splashdown (SLUS_202.23) matching decomp
#
# Compiler: Metrowerks CodeWarrior for PS2, mwccps2 2.4 (Engineering Build
# 0017) — its version string is "MW MIPS C Compiler (2.4.1.01)", exactly what
# the retail ELF's .comment says. Get it from decomp.me's compiler releases
# (mwcps2-2.4-001213) into toolchain/, gitignored. It runs under wibo
# (github.com/decompals/wibo), not Wine: under Wine it opens a window and hangs.
#
# Assembler/linker: GNU binutils for MIPS (SN's toolchain ships no linker).
# Arch: `yay -S mipsel-linux-gnu-binutils-minimal`. Override CROSS for
# another prefix, e.g. `make CROSS=mips-linux-gnu-`.
#
# Flags: -O4,p reproduces retail scheduling (delay-slot filling needs >= -O3).

WIBO      ?= toolchain/wibo
MWCC_DIR  ?= toolchain/mwcps2-2.4-001213
# zlib's compiler: 2.3-991202 with 2.3.3 backend behaviour ported in
#; built from the 2.3 bundle.
ZLIB_CC   := toolchain/mwcps2-2.3-zlib/mwccps2.exe
CROSS     ?= mipsel-linux-gnu-
SPLAT     ?= .venv/bin/splat

CC := $(WIBO) $(MWCC_DIR)/mwccps2.exe -c -nostdinc -stderr
# eabi, not o32: retail uses 64-bit sd/ld, which o32 would expand as macros.
AS := $(CROSS)as -EL -march=r5900 -mabi=eabi -G0 -no-pad-sections -Iinclude
LD := $(CROSS)ld -EL
OBJCOPY := $(CROSS)objcopy

CFLAGS := -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly

BASEROM    := baserom/SLUS_202.23
BUILD_DIR  := build
TARGET     := $(BUILD_DIR)/SLUS_202.23
LD_SCRIPT  := SLUS_202.23.ld
SYMBOLS    := config/symbol_addrs.txt
EXPECTED_SHA1 := 496ba6a5bfe9d7e68ea2edb993b493033ccc9bae

# Everything splat emits. Game code: asm/text.s (0x2000C0-0x3A1AD8) and
# asm/text_late.s (0x3CA670-). Sony SDK, libgcc, newlib and crt0: one file
# per retail object under asm/sdk/<library>/, listed in config/splat.yaml
#. The linker script fixes the order.
SDK_ASM  := $(shell sed -n 's|^ *- \[0x[0-9A-Fa-f]*, asm, \(sdk/[^]]*\)\].*|asm/\1.s|p' config/splat.yaml)
ASM_SRCS := $(SDK_ASM) asm/text.s asm/text_late.s asm/data/rodata.rodata.s asm/text_island.s asm/data/data.data.s
BIN_SRCS := assets/elf_header.bin assets/elf_tail.bin
OBJS := $(ASM_SRCS:%=$(BUILD_DIR)/%.o) $(BIN_SRCS:%=$(BUILD_DIR)/%.o)

.PHONY: all splat check match link relink clean distclean setup

all: check

setup:
	@test -f $(BASEROM) || (echo "missing $(BASEROM): copy your dump to baserom/SLUS_202.23" && false)
	@test -f $(MWCC_DIR)/mwccps2.exe || (echo "missing $(MWCC_DIR):" && false)
	@test -x $(WIBO) || (echo "missing $(WIBO): https://github.com/decompals/wibo/releases" && false)
	@command -v $(CROSS)as >/dev/null || (echo "missing $(CROSS)as: install MIPS binutils" && false)
	@test -x $(SPLAT) || (echo "missing $(SPLAT): uv venv .venv && uv pip install --python .venv 'splat64[mips]'" && false)

$(ZLIB_CC): tools/mwcc_zlib/mkcc.py $(wildcard tools/mwcc_zlib/cave/*.s)
	python3 -I tools/mwcc_zlib/mkcc.py

# The retail ELF keeps its symbol table; recover it for splat.
$(SYMBOLS): $(BASEROM) tools/extract_symbols.py
	python3 -I tools/extract_symbols.py $< $@

# Split the baserom into asm + a linker script. The stamp stands in for
# splat's many outputs.
$(BUILD_DIR)/.splat: config/splat.yaml $(SYMBOLS) $(BASEROM)
	$(SPLAT) split config/splat.yaml
	@mkdir -p $(@D) && touch $@

splat: $(BUILD_DIR)/.splat

# Empty recipe: make re-reads the files' times after splat rewrites them.
$(ASM_SRCS) $(BIN_SRCS) $(LD_SCRIPT): $(BUILD_DIR)/.splat ;

$(BUILD_DIR)/%.s.o: %.s
	@mkdir -p $(@D)
	$(AS) $< -o $@

# ee-gcc objects start on 8-byte boundaries; `as` gives .text 16-byte
# alignment, which tools/link_check.sh (it drops splat's SUBALIGN) would honour.
$(BUILD_DIR)/asm/sdk/%.s.o: asm/sdk/%.s
	@mkdir -p $(@D)
	$(AS) $< -o $@
	$(OBJCOPY) --set-section-alignment .text=8 $@

# Assemble binary blobs with `as` (not objcopy -I binary) so every object
# carries the same eabi flags and ld will merge them.
$(BUILD_DIR)/%.bin.o: %.bin
	@mkdir -p $(@D)
	printf '.section .data\n.incbin "%s"\n' $< | $(AS) -o $@ -

$(TARGET).elf: $(OBJS) $(LD_SCRIPT)
	$(LD) -T $(LD_SCRIPT) -T undefined_syms_auto.txt -T undefined_funcs_auto.txt \
		--no-check-sections -Map $(TARGET).map -o $@

$(TARGET): $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

# Byte-identity gate: the whole project diffs against this.
check: $(TARGET)
	@echo "$(EXPECTED_SHA1)  $(TARGET)" | sha1sum -c

# Per-function gate: every src file marked MATCHED must still compile to
# retail's bytes, with every relocation pointing at retail's symbol.
match: $(SYMBOLS) $(ZLIB_CC)
	@mkdir -p $(BUILD_DIR)
	@grep -rl '^// MATCHED (' src | sort | xargs -n 20 -P 8 python3 -I tools/match.py \
		| tee $(BUILD_DIR)/match.log | grep -E '^  (FAIL|ERROR)' && exit 1 || \
		echo "match: $$(grep -c '^  OK' $(BUILD_DIR)/match.log) functions OK"

# Link the C objects, code and data, in place of the asm and check the
# SHA-1 of the result.
link: $(BUILD_DIR)/.splat $(SYMBOLS) $(ZLIB_CC)
	tools/link_check.sh

# After `make link` (its objects): link from config/link_order.txt alone (a
# generated script: one wildcard per object and section kind, alignment
# rules, --gc-sections; no addresses), check the SHA-1, then link again with
# padding in .text, .data, .rodata and .bss and check that everything moved
# with it (tools/relink.py).
relink:
	python3 tools/relink.py --shift 0x80,0x100,0x180,0x200

clean:
	rm -rf $(BUILD_DIR)

distclean: clean
	rm -rf asm/text.s asm/text_late.s asm/text_island.s asm/sdk asm/data assets $(LD_SCRIPT) \
		undefined_funcs_auto.txt undefined_syms_auto.txt $(SYMBOLS)
