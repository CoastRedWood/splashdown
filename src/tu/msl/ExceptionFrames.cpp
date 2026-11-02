// ExceptionFrames.cpp  @ 0x003A1620
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// The MSL C++ runtime's frame helpers for the PS2 (MIPS) port as one
// translation unit, 0x3A1620-0x3A19A0: __TransferControl and __throw
// (hand-written asm), __SkipUnwindInfo, __FindExceptionTable, __SetupFrameInfo
// and __PopStackFrame; no data. Split from ExceptionHandling.cpp: that TU's
// bad_exception vtable copies (the what copy at 0x3A1610, the RTTI and
// "bad_exception" at the end of its .rodata) come only at a TU's end, so these
// functions are another TU. The name is a guess (MSL keeps the platform part in
// its own file, ExceptionPPC.cp on PowerPC). The C++ runtime (MSL) was built
// with exceptions on, unlike game code.

#pragma exceptions on

#include "NMWException.h"

void __ThrowHandler(ThrowContext *context);


// Hand-written assembly: restore the callee-saved registers recorded in the
// ThrowContext (GPRs as quadwords at +0x120..+0x200, $f20-$f31 at +0x288..),
// the handler frame's SP (+0x1C) and $v0 (+0x224), then jump to the landing
// pad `address`.
asm void __TransferControl(ThrowContext *context, ExceptionInfo *info, char *address)
{
    .set noreorder
    lq         s0, 0x120(a0)
    lq         s1, 0x130(a0)
    lq         s2, 0x140(a0)
    lq         s3, 0x150(a0)
    lq         s4, 0x160(a0)
    lq         s5, 0x170(a0)
    lq         s6, 0x180(a0)
    lq         s7, 0x190(a0)
    lq         fp, 0x200(a0)
    lwc1       f20, 0x288(a0)
    lwc1       f21, 0x28C(a0)
    lwc1       f22, 0x290(a0)
    lwc1       f23, 0x294(a0)
    lwc1       f24, 0x298(a0)
    lwc1       f25, 0x29C(a0)
    lwc1       f26, 0x2A0(a0)
    lwc1       f27, 0x2A4(a0)
    lwc1       f28, 0x2A8(a0)
    lwc1       f29, 0x2AC(a0)
    lwc1       f30, 0x2B0(a0)
    lwc1       f31, 0x2B4(a0)
    lw         sp, 0x1C(a0)
    lw         v0, 0x224(a0)
    jr         a2
    sub       sp, sp, v0
}

// Hand-written assembly: builds a ThrowContext on the stack (callee-saved
// GPRs as quadwords at +0x120..+0x200, $f20-$f31 at +0x288.., the throwing SP
// at +0x14/+0x1C, return address at +0x10, and the throw type/location/dtor
// at +0x0/+0x4/+0x8) and hands it to __ThrowHandler, which never returns.
extern "C" asm void __throw(char *throwtype, void *location, void *dtor)
{
    .set noreorder
    paddub     v0, sp, zero
    addiu      sp, sp, -0x2C0
    sq         s0, 0x120(sp)
    sq         s1, 0x130(sp)
    sq         s2, 0x140(sp)
    sq         s3, 0x150(sp)
    sq         s4, 0x160(sp)
    sq         s5, 0x170(sp)
    sq         s6, 0x180(sp)
    sq         s7, 0x190(sp)
    sq         fp, 0x200(sp)
    swc1       f20, 0x288(sp)
    swc1       f21, 0x28C(sp)
    swc1       f22, 0x290(sp)
    swc1       f23, 0x294(sp)
    swc1       f24, 0x298(sp)
    swc1       f25, 0x29C(sp)
    swc1       f26, 0x2A0(sp)
    swc1       f27, 0x2A4(sp)
    swc1       f28, 0x2A8(sp)
    swc1       f29, 0x2AC(sp)
    swc1       f30, 0x2B0(sp)
    swc1       f31, 0x2B4(sp)
    sw         v0, 0x14(sp)
    sw         v0, 0x1C(sp)
    sw         ra, 0x10(sp)
    sw         a0, 0x0(sp)
    sw         a1, 0x4(sp)
    sw         a2, 0x8(sp)
    jal        __ThrowHandler
    paddub    a0, sp, zero
}

// Skip an unwind-info header.
char *__SkipUnwindInfo(char *p)
{
    unsigned int dummy;
    int hasExtra = *p & 0x40;

    p = __DecodeUnsignedNumber(p + 1, &dummy);
    p = __DecodeUnsignedNumber(p, &dummy);
    if (hasExtra)
        p = __DecodeUnsignedNumber(p, &dummy);
    return p;
}

// Report the bounds of the exception table for a return address. This build
// links a single (empty) table, so it stores the linker-defined bounds and
// succeeds. Retail references the symbols without relocations; tools/match.py
// accepts that when the %hi/%lo pair encodes the symbol's address (retail
// .symtab: both are 0x44E904).
extern "C" ExceptionTableIndex __exception_table_start__[];
extern "C" ExceptionTableIndex __exception_table_end__[];

int __FindExceptionTable(ExceptionInfo *info, char *retaddr)
{
    info->exception_table_start = __exception_table_start__;
    info->exception_table_end = __exception_table_end__;
    return 1;
}

// Decode the unwind-info header of the current frame's exception record.
void __SetupFrameInfo(ThrowContext *context, ExceptionInfo *info)
{
    char *p = info->exception_record;
    int hasFPRs = *p & 0x40;

    context->usesFP = *p & 0x20;
    context->returnSaved = *p & 0x10;
    context->savedGPRs = *p & 0xF;
    p = __DecodeUnsignedNumber(p + 1, &context->frameSize);
    p = __DecodeUnsignedNumber(p, &context->savedRegOffset);
    if (context->usesFP)
        context->frameBase = (char *)context->GPR[30];
    else
        context->frameBase = context->throwSP;
    if (hasFPRs)
        __DecodeUnsignedNumber(p, &context->savedFPRs);
    else
        context->savedFPRs = 0;
}

// Restore the callee-saved registers the current frame saved and step the
// context to its caller's frame. Returns the frame's saved return address.
// Frame layout below frameBase + savedRegOffset: the saved FPRs, then the
// saved $s registers (and $fp, as a 9th), then the return address.
char *__PopStackFrame(ThrowContext *context, ExceptionInfo *info)
{
    char *p;
    char *fprs;
    int hasFPRs;
    char *retaddr;
    char *gprs;
    int i;
    unsigned long frameSize;
    unsigned long regOffset;
    unsigned long nGPRs;
    unsigned long nFPRs;

    p = info->exception_record;
    frameSize = context->frameSize;
    regOffset = context->savedRegOffset;
    nGPRs = context->savedGPRs;
    hasFPRs = *p & 0x40;

    if (hasFPRs) {
        nFPRs = context->savedFPRs;
        fprs = context->frameBase + regOffset + nFPRs * 4;
        gprs = (char *)((unsigned long)fprs + nGPRs * 16);
    } else {
        gprs = context->frameBase + regOffset + nGPRs * 16;
    }
    if (context->usesFP)
        gprs += 16;
    retaddr = *(char **)gprs;
    if (context->usesFP) {
        gprs -= 16;
        context->GPR[30] = *(GPR_Type *)gprs;
    } else if (nGPRs == 9) {
        gprs -= 16;
        context->GPR[30] = *(GPR_Type *)gprs;
        nGPRs--;
    }
    gprs -= (int)nGPRs * 16;
    for (i = 0; i < nGPRs; i++) {
        context->GPR[16 + i] = *(GPR_Type *)gprs;
        gprs += 16;
    }
    context->throwSP = context->frameBase + frameSize;
    if (hasFPRs) {
        fprs -= (int)nFPRs * 4;
        for (i = 0; i < nFPRs; i++) {
            context->FPR[20 + i] = *(FPR_Type *)fprs;
            fprs += 4;
        }
    }
    return retaddr;
}
