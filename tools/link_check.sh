#!/bin/sh
# Link the matched C objects in place of their asm and check the SHA-1.
# Usage: tools/link_check.sh [link_c.py args...]   (run from the repo root)
set -e
CROSS=${CROSS:-mipsel-linux-gnu-}
AS="${CROSS}as -EL -march=r5900 -mabi=eabi -G0 -no-pad-sections -Iinclude"
LD="${CROSS}ld -EL"
OUT=build/link
rm -rf $OUT
python3 tools/link_c.py --out $OUT "$@"
# Chunks may start at 8-aligned addresses: don't let ld round them up.
$AS $OUT/crt0.s -o $OUT/crt0.s.o && ${CROSS}objcopy --set-section-alignment .text=8 $OUT/crt0.s.o
ls $OUT/chunk_*.s | xargs -P "$(nproc)" -I{} sh -c "$AS {} -o {}.o && ${CROSS}objcopy --set-section-alignment .text=4 {}.o"
# Data chunks (cut around TU-mode files' data) are pinned too; as gives
# .data 16-byte alignment, which ld would round a pinned address up to.
# The SDK's data, one file per object section (config/sdk_data.txt), the same
# (its .bss files too: as gives .bss 16-byte alignment).
ls $OUT/dchunk_*.s $OUT/sdkdata_*.s $OUT/vupackets.s 2>/dev/null | xargs -r -P "$(nproc)" -I{} sh -c \
    "$AS {} -o {}.o && ${CROSS}objcopy --set-section-alignment .data=4 --set-section-alignment .rodata=4 --set-section-alignment .text=4 --set-section-alignment .bss=4 {}.o"
# Splice the fragments into splat's linker script, one per asm object
# link_c.py cuts: game code (asm/text.s, asm/text_late.s, the text island)
# and data (rodata, data; .bss goes where the island's empty .bss was).
# The SDK objects (asm/sdk/) stay as splat lists them. input.ld (the C
# objects in link order, INPUT) goes first: the linker keeps the first copy
# of each .gnu.linkonce section in that order.
awk -v out="$OUT" '
  function splice(frag) { while ((getline l < (out "/" frag)) > 0) print l; close(out "/" frag) }
  BEGIN { splice("input.ld") }   # the link order: the C objects, loaded first
  /build\/asm\/text\.s\.o\(\.text\);/             { splice("text.ld"); next }
  /build\/asm\/text_late\.s\.o\(\.text\);/        { splice("text_late.ld"); next }
  /build\/asm\/text_island\.s\.o\(\.text\);/      { splice("text_island.ld"); next }
  /build\/asm\/text_island\.s\.o\(\.bss\);/       { splice("bss.ld"); next }
  /build\/asm\/data\/rodata\.rodata\.s\.o\(/      { splice("rodata.ld"); next }
  /build\/asm\/data\/data\.data\.s\.o\(/          { splice("data.ld"); next }
  /build\/asm\/text(_late|_island)?\.s\.o\(/ { next }   # their other sections are empty
  /^    \.main(_bss)? / { sub(/ SUBALIGN\([0-9]+\)/, "") }   # pinned addresses may be 4-aligned
  { sub(/build\/asm\/sdk\/crt0\/crt0\.s\.o/, out "/crt0.s.o") }   # crt0 with its linker symbols named
  { print }' SLUS_202.23.ld > $OUT/SLUS_202.23.ld
$LD -T $OUT/SLUS_202.23.ld -T $OUT/aliases.ld -T $OUT/undefined_syms.txt -T $OUT/undefined_funcs.txt \
    --no-check-sections -Map $OUT/SLUS_202.23.map -o $OUT/SLUS_202.23.elf
${CROSS}objcopy -O binary $OUT/SLUS_202.23.elf $OUT/SLUS_202.23
echo "496ba6a5bfe9d7e68ea2edb993b493033ccc9bae  $OUT/SLUS_202.23" | sha1sum -c
