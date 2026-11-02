// vu0Functions.c  @ 0x002000C0 (text 0x2000C0-0x2003B0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: c) — byte-identical to retail.
//
// Retail's vu0Functions.c as one translation unit: the two hand-written VU0
// bounding-box tests, then its static initializer (the axis vectors).
// Although a .c file, it defined the axis vectors with C++ constructors, so
// retail compiled it as C++ (mwcc's `#pragma cplusplus on`); the initializer
// is still named after the .c file (__sinit_vu0Functions.c), so this file is
// built in C mode (mode: c above: match.py and link_c.py copy it to
// vu0Functions.c). It is the first TU of the link: its text opens the game's
// code and its initializer is the first .ctor slot.

#pragma cplusplus on

// The timer and random-number inlines (their header, unused here). An
// inline's local statics are emitted where it is defined, used or not, and
// these are the copies retail's linker kept, the first in link order: their
// .sbss/.bss lead those sections (0x44EA00, 0x44F948), before the axis
// vectors. The other TUs' copies are discarded (linkonce).
extern "C" void srand(unsigned int seed);
extern "C" int rand(void);
extern int TimerOverflowCounterT0;

inline unsigned int timeGetTime()
{
    static long Time = 0;
    static int lastCount = 0;
    int count, elapsed, overflows;
    count = *(volatile int *)0x10000000;    // T0_COUNT
    overflows = TimerOverflowCounterT0;
    TimerOverflowCounterT0 = 0;
    if (lastCount <= count)
        elapsed = count - lastCount;
    else
        elapsed = 0x10000 - lastCount + count;
    if (lastCount >= count && overflows > 0)
        overflows--;
    lastCount = count;
    Time += elapsed + (overflows << 16);
    return 0.0017066667f * Time;
}

inline float f_timeGetTime()
{
    static long Time = 0;
    static int lastCount = 0;
    int count, elapsed, overflows;
    count = *(volatile int *)0x10000000;    // T0_COUNT
    overflows = TimerOverflowCounterT0;
    TimerOverflowCounterT0 = 0;
    if (lastCount <= count)
        elapsed = count - lastCount;
    else
        elapsed = 0x10000 - lastCount + count;
    if (lastCount >= count && overflows > 0)
        overflows--;
    lastCount = count;
    Time += elapsed + (overflows << 16);
    return 0.0017066667f * Time;
}

inline int GetRandomNumber(int lo, int hi)
{
    static volatile unsigned int seeder = 0;
    if (!seeder) {
        seeder = timeGetTime();
        srand(seeder);
    }
    int r = (int)((hi - lo + 1) * (rand() * 4.656613e-10f));
    r = r > hi ? hi : r;
    r = lo > r ? lo : r;
    return r;
}

#include "sinit/AxisVectors.h"
#ifndef SPLASHDOWN_PC /* ports/pc/game/vu0Functions_pc.cpp */
// ---- src/VisibleTest/Vu0BoundingBoxVisible.cpp
extern "C" asm int Vu0BoundingBoxVisible(void *m, void *center, void *half)
{
    .set noreorder
    lqc2       vf4, 0x0(a0)
    lqc2       vf5, 0x10(a0)
    lqc2       vf6, 0x20(a0)
    lqc2       vf7, 0x30(a0)
    lqc2       vf8, 0x0(a1)
    lqc2       vf9, 0x0(a2)
    addi       v0, zero, 0x3F
    vsub.xyz   vf8, vf8, vf9
    vadd.xyz   vf9, vf9, vf9
    vmulax.xyzw ACC, vf4, vf8x
    vmadday.xyzw ACC, vf5, vf8y
    vmaddaz.xyzw ACC, vf6, vf8z
    vmaddw.xyzw vf10, vf7, vf8w
    vclipw.xyz vf10, vf10w
    vadd.x     vf8, vf8, vf9
    vmulax.xyzw ACC, vf4, vf8x
    vmadday.xyzw ACC, vf5, vf8y
    vmaddaz.xyzw ACC, vf6, vf8z
    vmaddw.xyzw vf10, vf7, vf8w
    cfc2    v1, vi18
    and        v0, v0, v1
    beqz       v0, L00200238
    nop
    vclipw.xyz vf10, vf10w
    vadd.z     vf8, vf8, vf9
    vmulax.xyzw ACC, vf4, vf8x
    vmadday.xyzw ACC, vf5, vf8y
    vmaddaz.xyzw ACC, vf6, vf8z
    vmaddw.xyzw vf10, vf7, vf8w
    cfc2    v1, vi18
    and        v0, v0, v1
    beqz       v0, L00200238
    nop
    vclipw.xyz vf10, vf10w
    vsub.x     vf8, vf8, vf9
    vmulax.xyzw ACC, vf4, vf8x
    vmadday.xyzw ACC, vf5, vf8y
    vmaddaz.xyzw ACC, vf6, vf8z
    vmaddw.xyzw vf10, vf7, vf8w
    cfc2    v1, vi18
    and        v0, v0, v1
    beqz       v0, L00200238
    nop
    vclipw.xyz vf10, vf10w
    vadd.y     vf8, vf8, vf9
    vmulax.xyzw ACC, vf4, vf8x
    vmadday.xyzw ACC, vf5, vf8y
    vmaddaz.xyzw ACC, vf6, vf8z
    vmaddw.xyzw vf10, vf7, vf8w
    cfc2    v1, vi18
    and        v0, v0, v1
    beqz       v0, L00200238
    nop
    vclipw.xyz vf10, vf10w
    vadd.x     vf8, vf8, vf9
    vmulax.xyzw ACC, vf4, vf8x
    vmadday.xyzw ACC, vf5, vf8y
    vmaddaz.xyzw ACC, vf6, vf8z
    vmaddw.xyzw vf10, vf7, vf8w
    cfc2    v1, vi18
    and        v0, v0, v1
    beqz       v0, L00200238
    nop
    vclipw.xyz vf10, vf10w
    vsub.z     vf8, vf8, vf9
    vmulax.xyzw ACC, vf4, vf8x
    vmadday.xyzw ACC, vf5, vf8y
    vmaddaz.xyzw ACC, vf6, vf8z
    vmaddw.xyzw vf10, vf7, vf8w
    cfc2    v1, vi18
    and        v0, v0, v1
    beqz       v0, L00200238
    nop
    vclipw.xyz vf10, vf10w
    vsub.x     vf8, vf8, vf9
    vmulax.xyzw ACC, vf4, vf8x
    vmadday.xyzw ACC, vf5, vf8y
    vmaddaz.xyzw ACC, vf6, vf8z
    vmaddw.xyzw vf10, vf7, vf8w
    cfc2    v1, vi18
    and        v0, v0, v1
    beqz       v0, L00200238
    nop
    vclipw.xyz vf10, vf10w
    vnop
    vnop
    vnop
    vnop
    vnop
    cfc2    v1, vi18
    and        v0, v0, v1
    beqz       v0, L00200238
    nop
    addi       v0, zero, 0x1
L00200238:
    jr         ra
    nop
}
#endif
#ifndef SPLASHDOWN_PC /* ports/pc/game/vu0Functions_pc.cpp */
// ---- src/VisibleTest/Vu0BoundingBoxVisibleClipCodes.cpp
extern "C" asm int Vu0BoundingBoxVisibleClipCodes(void *m, void *center, void *half, unsigned int *codes)
{
    .set noreorder
    lqc2       vf4, 0x0(a0)
    lqc2       vf5, 0x10(a0)
    lqc2       vf6, 0x20(a0)
    lqc2       vf7, 0x30(a0)
    lqc2       vf8, 0x0(a1)
    lqc2       vf9, 0x0(a2)
    addi       v0, zero, 0x3F
    addi       a0, zero, 0x0
    vsub.xyz   vf8, vf8, vf9
    vadd.xyz   vf9, vf9, vf9
    vmulax.xyzw ACC, vf4, vf8x
    vmadday.xyzw ACC, vf5, vf8y
    vmaddaz.xyzw ACC, vf6, vf8z
    vmaddw.xyzw vf10, vf7, vf8w
    vclipw.xyz vf10, vf10w
    vadd.x     vf8, vf8, vf9
    vmulax.xyzw ACC, vf4, vf8x
    vmadday.xyzw ACC, vf5, vf8y
    vmaddaz.xyzw ACC, vf6, vf8z
    vmaddw.xyzw vf10, vf7, vf8w
    cfc2    v1, vi18
    and        v0, v0, v1
    or         a0, a0, v1
    vclipw.xyz vf10, vf10w
    vadd.z     vf8, vf8, vf9
    vmulax.xyzw ACC, vf4, vf8x
    vmadday.xyzw ACC, vf5, vf8y
    vmaddaz.xyzw ACC, vf6, vf8z
    vmaddw.xyzw vf10, vf7, vf8w
    cfc2    v1, vi18
    and        v0, v0, v1
    or         a0, a0, v1
    vclipw.xyz vf10, vf10w
    vsub.x     vf8, vf8, vf9
    vmulax.xyzw ACC, vf4, vf8x
    vmadday.xyzw ACC, vf5, vf8y
    vmaddaz.xyzw ACC, vf6, vf8z
    vmaddw.xyzw vf10, vf7, vf8w
    cfc2    v1, vi18
    and        v0, v0, v1
    or         a0, a0, v1
    vclipw.xyz vf10, vf10w
    vadd.y     vf8, vf8, vf9
    vmulax.xyzw ACC, vf4, vf8x
    vmadday.xyzw ACC, vf5, vf8y
    vmaddaz.xyzw ACC, vf6, vf8z
    vmaddw.xyzw vf10, vf7, vf8w
    cfc2    v1, vi18
    and        v0, v0, v1
    or         a0, a0, v1
    vclipw.xyz vf10, vf10w
    vadd.x     vf8, vf8, vf9
    vmulax.xyzw ACC, vf4, vf8x
    vmadday.xyzw ACC, vf5, vf8y
    vmaddaz.xyzw ACC, vf6, vf8z
    vmaddw.xyzw vf10, vf7, vf8w
    cfc2    v1, vi18
    and        v0, v0, v1
    or         a0, a0, v1
    vclipw.xyz vf10, vf10w
    vsub.z     vf8, vf8, vf9
    vmulax.xyzw ACC, vf4, vf8x
    vmadday.xyzw ACC, vf5, vf8y
    vmaddaz.xyzw ACC, vf6, vf8z
    vmaddw.xyzw vf10, vf7, vf8w
    cfc2    v1, vi18
    and        v0, v0, v1
    or         a0, a0, v1
    vclipw.xyz vf10, vf10w
    vsub.x     vf8, vf8, vf9
    vmulax.xyzw ACC, vf4, vf8x
    vmadday.xyzw ACC, vf5, vf8y
    vmaddaz.xyzw ACC, vf6, vf8z
    vmaddw.xyzw vf10, vf7, vf8w
    cfc2    v1, vi18
    and        v0, v0, v1
    or         a0, a0, v1
    vclipw.xyz vf10, vf10w
    vnop
    vnop
    vnop
    vnop
    vnop
    cfc2    v1, vi18
    and        v0, v0, v1
    beqz       v0, L002003A0
    or        a0, a0, v1
    addi       v0, zero, 0x1
L002003A0:
    addi       v1, zero, 0x3F
    and        a0, a0, v1
    jr         ra
    sw        a0, 0x0(a3)
}
#endif

// ---- src/sinit/vu0Functions.cpp
// (The initializer is the axis vectors' constructors from sinit/AxisVectors.h.)
