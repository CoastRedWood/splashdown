// HiResMeter.cpp  @ 0x00241BE0 (text 0x241BE0-0x242150)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's HiResMeter.cpp as one translation unit: the seven HiResMeter
// methods and the static initializer (GlobalHiResMeter). `make link`
// places the object's data as well as its code (tools/link_c.py). The rest
// of the tu_map row is other TUs without an initializer (ImageData from
// 0x242150; InputDevice; JoystickDevice, KeyboardDevice and
// ContainerList<AnalogAxis *> from InputDeviceType.cpp) and stays
// per-function.



class ProcessNode;
class KeyInfo;

#include "GameObject/GameObject.h"
#include "HiResMeter/HiResMeter.h"
#include "DebugOverlay/DebugOverlay.h"
#include "Game/Game.h"
#include "PSXVideoCard/PSXVideoCard.h"
#include "mwcc_shim.h"

extern Game *LpGame;
extern PSXVideoCard *LpVideoCard;

// ---- src/HiResMeter/ctor.cpp
// Retail's HiResMeter constructor is barely optimized (this kept on the
// stack, no register allocation) but scheduled: optimization level 1 plus
// the instruction scheduler reproduces it. The other methods are -O4.
#pragma optimization_level 1
#pragma schedule on

HiResMeter::HiResMeter(int id) : GameObject(id)
{
    m_page = -1;
    m_secondsPerTick = 0.0f;
    DeleteAllMeters();
    if (Create())
        m_created = 1;
    else
        m_created = 0;
}

#pragma optimization_level 4

// ---- src/HiResMeter/dtor.cpp
HiResMeter::~HiResMeter()
{
}

// ---- src/HiResMeter/Create.cpp
HiResMeter *HiResMeter::Create()
{
    GameObject::Create();
    m_secondsPerTick = 1.0f / 150000.0f;
    m_secondsPerTick *= 256.0f;
    return this;
}

// ---- src/HiResMeter/ClearCalls.cpp
void HiResMeter::ClearCalls()
{
    for (int i = 0; i < m_numMeters; i++) {
        m_meters[i].m_calls = 0.0f;
        m_meters[i].m_msThisTick = 0.0f;
    }
}

// ---- src/HiResMeter/ResetMeter.cpp
void HiResMeter::ResetMeter(int index)
{
    m_meters[index].m_ms = 0.0f;
    m_meters[index].m_peak = 0.0f;
    m_meters[index].m_startTime = 0;
    m_meters[index].m_calls = 0.0f;
    m_meters[index].m_msThisTick = 0.0f;
    m_meters[index].m_totalMs = 0.0f;
    m_meters[index].m_ticks = 0.0f;
    m_meters[index].m_perCallPeak = 0.0f;
}

// ---- src/HiResMeter/DeleteAllMeters.cpp
void HiResMeter::DeleteAllMeters()
{
    for (int i = 0; i < 150; i++) {
        m_meters[i].m_ms = 0.0f;
        m_meters[i].m_peak = 0.0f;
        m_meters[i].m_startTime = 0;
        m_meters[i].m_calls = 0.0f;
        m_meters[i].m_msThisTick = 0.0f;
        m_meters[i].m_totalMs = 0.0f;
        m_meters[i].m_ticks = 0.0f;
        m_meters[i].m_perCallPeak = 0.0f;
        m_meters[i].m_name[0] = 0;
    }
    m_numMeters = 0;
    m_current = 0;
}

// ---- src/HiResMeter/Render3D.cpp
// Compiler session state: retail had
// already generated a two-argument runtime helper call (64-bit / or %,
// double arithmetic or compare) before Render3D, so temporaries live before
// a later helper call (fptodp, ...) skip $a0/$a1. Nothing earlier in this
// TU does (probably inline 64-bit timer methods retail's header had); this
// unused function recreates the state and is not part of the retail file.
static void MWCC_SHIM_TEXT MwccSessionState(long *a) { a[0] = a[0] / a[1]; }

int HiResMeter::Render3D()
{
    // Declared up front in this order: it fixes which $s registers the
    // loop variables get.
    int page; int i; HiResMeterEntry *t; int width; int meterPage; float ms;
    width = LpVideoCard->m_width;
    if (LpGame->m_currentPlayer == LpGame->m_numCameras - 1) {
        DebugOverlay *overlay = LpGame->m_debugOverlay;
        if (overlay) {
            if (m_page < 0)
                m_page = overlay->m_numPages++;
            page = LpGame->m_debugOverlay->m_page;
            i = 0;
            if (page == m_page) {
                if (width <= 0x200)
                    LpGame->m_debugOverlay->PrintfHeading(m_page, "[CallsThisTick->MSThisTick(MSPerTick)](Peak/Avg)");
                else
                    LpGame->m_debugOverlay->PrintfHeading(m_page, "Timers [CallsThisTick->MSThisTick(MSPerTick)](Per call Peak/Avg)");
            }
            for (i = 0; i < m_numMeters; i++) {
                t = &m_meters[i];
                meterPage = t->m_page == -1 ? m_page : t->m_page;
                ms = t->m_ms;
                if (ms > 0.0f) {
                    if (meterPage != page) {
                        ResetMeter(i);
                    } else {
                        float perCall = t->m_peak / ms;
                        t->m_ticks += 1.0f;
                        t->m_totalMs += t->m_msThisTick;
                        float perTick = t->m_totalMs / t->m_ticks;
                        if (width <= 0x200)
                            LpGame->m_debugOverlay->PrintfDetail(meterPage, "%12.12s[%2.0f ->%5.2f (%5.2f)] (%5.2f/%7.5f)ms", t->m_name, t->m_calls, t->m_msThisTick, perTick, t->m_perCallPeak, perCall);
                        else if (width <= 0x280)
                            LpGame->m_debugOverlay->PrintfDetail(meterPage, "%19.19s[%4.0f -> %5.2f (%5.2f)] (%5.2f / %7.5f)ms", t->m_name, t->m_calls, t->m_msThisTick, perTick, t->m_perCallPeak, perCall);
                        else
                            LpGame->m_debugOverlay->PrintfDetail(meterPage, "%24.24s [%4.0f -> %6.3f (%6.3f)] (%6.3f / %7.5f) MS", t->m_name, t->m_calls, t->m_msThisTick, perTick, t->m_perCallPeak, perCall);
                    }
                }
            }
        }
        ClearCalls();
    }
    return 1;
}

// ---- src/sinit/HiResMeter.cpp
// The global meter set.
HiResMeter GlobalHiResMeter(0);
HiResMeter *LpHiResMeter = &GlobalHiResMeter;
