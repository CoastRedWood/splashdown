// PeakInt.cpp  @ 0x00264930 (text 0x264930-0x264DD0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// The end of NormalDistribution.cpp's tu_map row, after Particles.cpp:
// PeakInt's three functions and PeakFloat's ctor, as a TU without a static
// initializer. Its name is a guess: these functions have no data of their own
// (the timeGetTime statics are the copy every TU shares, placed from
// ParticleVUPhysics.cpp), so nothing decides between a TU of its own and
// Particles.cpp's tail; link order puts a "Peak*.cpp" here. `make link` places
// the object's code (tools/link_c.py).



#include "PeakInt/PeakInt.h"
#include "mwcc_shim.h"

// Compiler session state: retail compiled
// this file in an mwcc session that had already generated a two-argument
// runtime helper call, so later helper calls (fptoui in timeGetTime) count as
// reading $a0-$a1. Every per-function file had it; one at the top serves the
// TU. This unused function is not part of the retail file.
static void MWCC_SHIM_TEXT MwccSessionState(long *a) { a[0] = a[0] / a[1]; }

extern int TimerOverflowCounterT0;

// Win32-style timeGetTime: EE timer 0 (T0_COUNT at 0x10000000) ticks plus the
// overflows counted by TimerHandlerT0, accumulated and scaled to milliseconds.
inline unsigned int timeGetTime()
{
    static long Time = 0;
    static int lastCount = 0;
    int count;
    int elapsed;
    int overflows;
    count = *(volatile int *)0x10000000;
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

// PeakFloat: PeakInt with a float peak (no other users).
class PeakFloat {
public:
    PeakFloat(unsigned int window);

    unsigned int m_time;                // +0x0
    float m_peak;                       // +0x4
    unsigned int m_window;              // +0x8
};

// ---- src/PeakInt/ctor.cpp
PeakInt::PeakInt(unsigned int window)
{
    m_peak = 0;
    m_time = timeGetTime();
    m_window = window;
}

// ---- src/PeakInt/GetPeak.cpp
int PeakInt::GetPeak()
{
    if (timeGetTime() - m_time > m_window)
        return 0;
    return m_peak;
}

// ---- src/PeakInt/Record.cpp
void PeakInt::Record(int value)
{
    unsigned int now = timeGetTime();
    if (value > m_peak || now - m_time > m_window) {
        m_peak = value;
        m_time = now;
    }
}

// ---- src/PeakFloat/ctor.cpp
PeakFloat::PeakFloat(unsigned int window)
{
    m_peak = 0.0f;
    m_time = timeGetTime();
    m_window = window;
}
