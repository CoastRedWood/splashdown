# Code cave for toolchain/mwcps2-2.3-zlib/mwccps2.exe (new PE section ".zlib" at 0x555000).
# Every routine here is a transcription of mwccps2 2.3.3-000906 code with its
# callee/global addresses mapped to their 2.3-991202 counterparts:
#   2.3.3      -> 2.3       what
#   0x493740   -> 0x491520  emit pcode at end of current block (varargs)
#   0x4936e0   -> 0x4914c0  make pcode (varargs), not appended
#   0x493aa0   -> 0x491880  insert pcode before another
#   0x493a50   -> 0x491830  unlink pcode
#   0x493910   -> 0x4916f0  start the block of a label
#   0x495ed0   -> 0x493b50  first label operand of a branch pcode
#   0x4a9730   -> 0x4a6e80  layout: give a pcode its address, advance counter
#   0x4ec3a0   -> 0x4e85a0  call fn for every loop (loop list)
#   0x48a400   -> 0x488490  register an opcode in the opcode table
#   0x557594   -> 0x50c604  current pcode block
#   0x541230   -> 0x502178  layout address counter
#   0x557c4c   -> 0x50d14c  optimisation level byte
#   0x557c4d   -> 0x50d14d  "optimise for size" byte (alignlabel/padloop default off when set)
#   opcodes: nop 0xba -> 0xb8, word 0x2c4 -> 0x2ba, p2alignl(I) 0x45e -> 0x3f8 (a free slot)
.intel_syntax noprefix
.text
.globl _start
_start:

# --- register p2alignl ------------------------------------------------------
# 2.3.3 0x492c96: OpTable_Register(0x45e, "p2alignl", "I", 0, 0x8100000, 0, 0, 0).
# Reached by a jmp from the end of 2.3's opcode-table init (0x490ae3).
reg_p2alignl:
    push 0
    push 0
    push 0
    push 0x8100000
    push 0
    push OFFSET s_I
    push OFFSET s_p2alignl
    push 0x3f8
    call 0x488490
    add esp, 0x20
    ret

# --- align_emit: 2.3.3 0x4957d0 -------------------------------------------
# if (!size && alignlabel && curblock->weight >= 8 && alignval > 2) emit(p2alignl, alignval)
# alignlabel's default is !size and 2.3 has no pragma for it; alignval is the
# R5900 value 3 (2.3.3 0x557ba6, set by the processor setup).
align_emit:
    cmp BYTE PTR ds:0x50d14d, 0
    jne 1f
    mov eax, DWORD PTR ds:0x50c604
    cmp DWORD PTR [eax+0x28], 8
    jl 1f
    push 3              # 64-bit immediate {hi, lo} = {0, 3}
    push 0
    push 0x3f8
    call 0x491520
    add esp, 0xc
1:  ret

# --- label_hook: 2.3.3 0x434d0d (label statement codegen) -------------------
# called from 2.3 0x43318d in place of `push eax; call 0x4916f0; pop ecx`
label_hook:
    call align_emit
    mov eax, DWORD PTR [ebx+0x10]
    push eax
    call 0x4916f0
    pop ecx
    ret

# --- switch_hook: 2.3.3 0x4e6a02 (jump-table switch codegen) ----------------
# called from 2.3 0x4e332f in place of `mov eax,[esp+0x14]; add esp,0xc`
switch_hook:
    pop edx
    add esp, 0xc
    push edx
    call align_emit
    mov eax, DWORD PTR [esp+0xc]
    ret

# --- layout_hook: 2.3.3 0x4a9815..0x4a98fb (BackEnd_Binary2 layout) ---------
# called from 2.3 0x4a6f40 in place of `push esi; call 0x4a6e80; pop ecx`;
# esi = pcode. A p2align pcode is replaced by .word fill pcodes up to the
# alignment, then unlinked (its next pointer stays valid for the caller).
# (2.3.3 also records the largest alignment in 0x5576fc for the section;
# 2.3 has no such variable.)
layout_hook:
    test DWORD PTR [esi+0xc], 0x100000
    jnz 2f
    push esi
    call 0x4a6e80
    pop ecx
    ret
2:  push ebx
    push edi
    mov ecx, DWORD PTR [esi+0x3a]   # operand 0 value (low word)
    test ecx, ecx
    je 4f
    mov edi, 1
    shl edi, cl
3:  mov eax, edi
    dec eax
    and eax, DWORD PTR ds:0x502178
    je 4f
    push 0              # fill: p2alignl I -> 0
    push 0
    push 0x2ba          # word I
    call 0x4914c0
    add esp, 0xc
    mov ebx, eax
    push ebx
    push esi
    call 0x491880
    add esp, 8
    push ebx
    call 0x4a6e80
    pop ecx
    jmp 3b
4:  push esi
    call 0x491830
    pop ecx
    pop edi
    pop ebx
    ret

# --- padloop driver: 2.3.3 0x435d28 ----------------------------------------
# if (padloop && optlevel >= 3 && !arg) padloop(); then the original call.
# called from 2.3 0x43410c in place of `call 0x4a6e20`.
padloop_drv:
    cmp BYTE PTR ds:0x50d14d, 0
    jne 1f
    cmp BYTE PTR ds:0x50d14c, 3
    jl 1f
    cmp BYTE PTR [esp+0x34], 0
    jne 1f
    push OFFSET padloop_cb      # 2.3.3 0x4a8fc0
    call 0x4e85a0
    pop ecx
1:  jmp 0x4a6e20

# 2.3.3 0x4a8ea0
padloop_cb:
    mov eax, DWORD PTR [esp+4]
    mov edx, DWORD PTR [eax+0x24]
    test edx, edx
    mov ecx, edx
    jne 1f
    ret
1:  mov eax, DWORD PTR [ecx+4]
    push eax
    call padloop_blk
    pop ecx
    ret

# 2.3.3 0x4a8ec0, instruction for instruction
padloop_blk:
    push ebx
    push esi
    push edi
    push ebp
    sub esp, 8
    mov esi, DWORD PTR [esp+0x1c]
    xor ebx, ebx
    mov ecx, DWORD PTR [esi+0x18]
    mov DWORD PTR [esp+4], ecx
    mov eax, DWORD PTR [ecx+0xc]
    and eax, 0x10000000
    jne 10f
    jmp 99f
10: push DWORD PTR [esp+4]
    call 0x493b50
    mov eax, DWORD PTR [eax+2]
    pop ecx
    mov edx, DWORD PTR [eax+4]
    mov ebp, DWORD PTR [edx+0x1c]
    mov ecx, ebp
    sub ecx, DWORD PTR [esi+0x1c]
    cmp ecx, 2
    mov eax, ecx
    jg 99f
    cmp eax, -2
    jl 99f
    # INFERRED, not in 2.3.3: retail pads only a loop whose back branch
    # targets its own block (2.3.3 and 2.4 accept a target up to 2 blocks away)
    test eax, eax
    jne 99f
    mov ecx, DWORD PTR [esi]
    xor edi, edi
    test ecx, ecx
    mov eax, ecx
    je 30f
20: cmp DWORD PTR [eax+0x1c], ebp
    je 99f
    inc edi
    cmp edi, 2
    jge 30f
    mov eax, DWORD PTR [eax]
    test eax, eax
    jne 20b
30: cmp edx, esi
    mov ecx, esi
    je 50f
40: movsx eax, WORD PTR [ecx+0x2c]
    mov ecx, DWORD PTR [ecx+4]
    add ebx, eax
    test ecx, ecx
    je 99f
    mov ebp, DWORD PTR [ecx+0x18]
    test ebp, ebp
    je 45f
    mov eax, DWORD PTR [ebp+0xc]
    and eax, 0x100000
    je 45f
    dec ebx
45: cmp ebx, 6
    jg 99f
    cmp edx, ecx
    jne 40b
50: movsx eax, WORD PTR [ecx+0x2c]
    add ebx, eax
    cmp ebx, 6
    jg 99f
    jmp 70f
60:
padloop_ins:                    # (named only for gdb tracing)
    push 0xb8
    call 0x4914c0
    pop ecx
    push eax
    push DWORD PTR [esp+8]
    call 0x491880
    inc ebx
    pop ecx
    pop ecx
70: cmp ebx, 6
    jle 60b
99: add esp, 8
    pop ebp
    pop edi
    pop esi
    pop ebx
    ret


# --- blt_reg_s / blt_reg_u: 2.3.3 0x489290 / 0x4892e0 -----------------------
# `if (a < b) goto L` as slt/sltu into a new temp + bne temp,$zero (2.3 emits
# the fused blt/bltu macro instead). Args (operand *a, operand *b, label).
# 2.3.3 slt 0x488770 -> 2.3 0x4868f0 (op 0xd3 -> 0xd1); sltu 0x4887a0 -> 0x486920
# (0xd6 -> 0xd4); branch emitter 0x489eb0 -> 0x487f40 (bne = 0x2a in both).
blt_reg_s:
    push esi
    push edi
    sub esp, 0x48
    mov eax, DWORD PTR [esp+0x58]
    mov esi, DWORD PTR [esp+0x54]
    push eax
    lea eax, [esp+0x28]
    push esi
    push eax
    call 0x4868f0
    jmp 1f
blt_reg_u:
    push esi
    push edi
    sub esp, 0x48
    mov eax, DWORD PTR [esp+0x58]
    mov esi, DWORD PTR [esp+0x54]
    push eax
    lea eax, [esp+0x28]
    push esi
    push eax
    call 0x486920
1:  mov ecx, 8
    lea edi, [esp+0xc]
    lea esi, [esp+0x30]
    add esp, 0xc
    rep movsd
    movsw
    mov edx, DWORD PTR [esp+0x5c]
    lea eax, [esp]
    push edx
    push eax
    push 0x2a
    call 0x487f40
    add esp, 0xc
    add esp, 0x48
    pop edi
    pop esi
    ret

# --- trailing_directive_cmp: 2.3.3 0x4aa1d5 ------------------------------------
# The delay-slot filler skips trailing directives at a block's end before
# looking for the branch: 2.3 tests `op >= 0x44b`, 2.3.3 `op >= 0x456`, which
# takes in p2align (0x459-0x45e there). Our p2alignl is 0x3f8, so it is
# tested for separately. Returns with CF clear for a directive.
trailing_directive_cmp:
    cmp WORD PTR [edi+0x20], 0x3f8
    je 1f
    cmp WORD PTR [edi+0x20], 0x44b
1:  ret

s_p2alignl: .asciz "p2alignl"
s_I:        .asciz "I"
