# In place at 2.3 0x4875e0..0x487602 (blt-immediate helper 0x487550, constant
# too large for slti: 2.3.3 0x489500..0x48951f). 2.3 loads the constant and
# emits the fused macro bltu/blt (0x3d4/0x3d0); 2.3.3 calls blt_reg_u/blt_reg_s.
.intel_syntax noprefix
.text
.globl _start
_start:
    mov ecx, OFFSET blt_reg_u
    jmp 1f
    mov ecx, OFFSET blt_reg_s
1:  lea eax, [esp+0x4]
    push ebx
    push eax
    push ebp
    call ecx
    add esp, 0xc
    nop
    nop
    nop
    nop
    nop
    nop
    nop
