// HiResMeter: named timing meters (at most 150), a GameObject. 0x2F28 bytes.
// Layout from retail's accessors, its ctor (src/HiResMeter/ctor.cpp) and the
// static initializers that look meters up by name (src/sinit/).
#ifndef HIRESMETER_HIRESMETER_H
#define HIRESMETER_HIRESMETER_H

#include "GameObject/GameObject.h"

extern "C" int strcmp(const char *a, const char *b);
extern "C" char *strncat(char *dst, const char *src, unsigned int n);

struct HiResMeterEntry {            // 0x50 bytes
    long m_startTime;               // +0x00
    float m_ms;                     // +0x08
    float m_peak;                   // +0x0C
    float m_calls;                  // +0x10, calls this tick
    float m_msThisTick;             // +0x14
    float m_totalMs;                // +0x18
    float m_ticks;                  // +0x1C
    int m_x20;                      // +0x20 (not reset)
    float m_perCallPeak;            // +0x24
    char m_name[0x20];              // +0x28, empty when unused
    int m_page;                     // +0x48, debug page; -1: the meter's own page
    char pad4C[0x50 - 0x4C];
};

class HiResMeter : public GameObject {
public:
    HiResMeter(int id);
    void ClearCalls();
    void ResetMeter(int index);
    void DeleteAllMeters();

    // Finds a meter by name (making it current), or appends a new one; the
    // 151st name gets 149. Inlined by the static initializers of
    // PSXRenderTarget.cpp and PSXWaterDisplacement.cpp.
    int GetMeterID(char *name)
    {
        int i;
        for (i = 0; i < m_numMeters; i++) {
            if (strcmp(m_meters[i].m_name, name) == 0) {
                m_current = i;
                return i;
            }
        }
        if (m_numMeters >= 150)
            return 149;
        m_meters[i].m_page = -1;
        strncat(m_meters[m_numMeters].m_name, name, 31);
        m_current = m_numMeters;
        m_numMeters++;
        return m_numMeters - 1;
    }

    // --- virtuals ---
    virtual ~HiResMeter();
    virtual HiResMeter *Create();
    virtual int Render3D();

    char m_created;                 // +0x30
    char pad31[0x34 - 0x31];
    float m_secondsPerTick;         // +0x34
    int m_numMeters;                // +0x38
    int m_current;                  // +0x3C
    int m_page;                     // +0x40, -1 until a debug page is assigned
    char pad44[0x48 - 0x44];
    HiResMeterEntry m_meters[150];  // +0x48
};

extern HiResMeter *LpHiResMeter;

#endif
