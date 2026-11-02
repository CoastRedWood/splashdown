// Debug.cpp  @ 0x0022CB20 (text 0x22CB20-0x22CBF0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp)
//
// _AssertFunc_ and PSXprintf as one translation unit, a TU without a static
// initializer in Cube.cpp's tu_map row: its one string ("\nAssertion: ...",
// 0x4113B0) follows D3DIMSoultreeMotnctrl.cpp's RTTI with no restart, and it
// defines AllowAllAssertions (.data 0x3F90A8, after Cubedraw.cpp's globals).
// The name is a guess that keeps the row's alphabetical link order (after
// D3DIMSoultreeMotnctrl.cpp, before DmaRenderItem.cpp); nothing in the data
// says whether DebugOverlay's four functions (next) belong to it or to their
// own DebugOverlay.cpp.


int AllowAllAssertions = 1;

// ---- src/global/AssertFunc_.cpp
extern "C" int printf(const char *fmt, ...);
extern "C" int sceGsSyncV(int mode);

int _AssertFunc_(int condition, const char *file, int line)
{
    if (!AllowAllAssertions)
        return 0;
    if (!condition) {
        printf("\nAssertion: '%s' line %ld\n", file, line);
        sceGsSyncV(0);
        return 1;
    }
    return 0;
}

// ---- src/global/PSXprintf.cpp
// mwcc's MIPS va_start: __builtin_args_info(2) is the number of integer
// argument registers the named parameters use; the unnamed ones were spilled
// to the 8-register save area that ends at __builtin_next_arg(last).
#include "mwcc_va.h"




extern "C" int vprintf(const char *fmt, va_list ap);

void PSXprintf(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
}

