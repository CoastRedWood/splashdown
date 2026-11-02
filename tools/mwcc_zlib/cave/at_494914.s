# In place at 2.3 0x494914..0x494980 (compare-and-branch codegen, case `<`;
# 2.3.3 0x496cc4..0x496d3e). 2.3 emitted the fused macros blt/bltu (reg or
# immediate: opcodes 0x3d0/0x3d2/0x3d4/0x3d6); 2.3.3 calls blt-immediate
# (2.3.3 0x489470 = 2.3 0x487550) with the left operand's type, or
# blt_reg_u/blt_reg_s (slt into a temp + bne). 2.3.3 keeps the left operand
# node at [esp+0x20]; 2.3 keeps it at [esp+0x14].
.intel_syntax noprefix
.text
.globl _start
_start:
    cmp BYTE PTR [esp+0x4c], 3
    jne 1f
    mov eax, DWORD PTR [esp+0x14]
    lea edx, [esp+0x4c]
    mov eax, DWORD PTR [eax+0xa]
    add edx, 0xa
    push DWORD PTR [esp+0xb0]
    push eax
    lea eax, [esp+0x30]
    push edx
    push eax
    call 0x487550
    add esp, 0x10
    jmp 0x494c41
1:  lea eax, [esp+0x28]
    lea ecx, [esp+0x4c]
    push DWORD PTR [esp+0xb0]
    push ecx
    push eax
    cmp BYTE PTR [esp+0x30], 0
    je 2f
    call blt_reg_u
    jmp 3f
2:  call blt_reg_s
3:  add esp, 0xc
    jmp 0x494c41
