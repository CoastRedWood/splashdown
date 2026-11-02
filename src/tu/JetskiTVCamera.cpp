// JetskiTVCamera.cpp  @ 0x00313B00 (text 0x313B00-0x314570)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's JetskiTVCamera.cpp as one translation unit: its functions in
// retail order, then its static initializer. `make link` places the
// object's data as well as its code (tools/link_c.py).



#include "JetskiTVCamera/JetskiTVCamera.h"
#include "d3dtypes.h"
#include "ParameterBlock/ParameterBlock.h"
#include "JetskiGameOptions/JetskiGameOptions.h"
#include "JetskiGame/JetskiGame.h"
#include "DebugOverlay/DebugOverlay.h"
#include "sinit/AxisVectors.h"

extern "C" void free(void *p);
extern "C" void *malloc(unsigned int size);
extern "C" int sprintf(char *buf, const char *fmt, ...);
void PSXprintf(const char *fmt, ...);
extern JetskiGame *LpGame;
// This file's page in the debug overlay, claimed on first use.
extern int g_DebugOverlayID;

// ---- src/JetskiTVCamera/ctor.cpp

JetskiTVCamera::JetskiTVCamera()
{
    m_probabilities = 0;
    m_links = 0;
    m_numCrews = 0;
}


// ---- src/JetskiTVCamera/dtor.cpp


JetskiTVCamera::~JetskiTVCamera()
{
    if (m_probabilities)
        free(m_probabilities);
    if (m_links)
        free(m_links);
}


// ---- src/JetskiTVCamera/LoadPositions.cpp


int JetskiTVCamera::LoadPositions(char *directory)
{
    char path[0x110];
    ParameterBlock block;
    if (m_numCrews)
        return 1;
    switch (LpGame->m_gameOptions->m_difficulty) {
    case 1:
    case 2:
        sprintf(path, "%sTVCamE.pos", directory);
        break;
    case 3:
        sprintf(path, "%sTVCamH.pos", directory);
        break;
    }
    if (!block.OpenFile(path, 0)) {
        PSXprintf("Failed to load TV camera locations file (%s) in SimpleTVCamera::LoadPositions\n", path);
        return 0;
    }
    block.ReadParameterBlock("CameraPositions");
    block.GetParameter("NumberOfCameras", 0, &m_numCrews);
    if (m_numCrews <= 0) {
        PSXprintf("NumberOfCameras has to be at least 1\n");
        return 0;
    }
    block.GetParameter("SwitchEvery", 5.0f, &m_switchEvery);
    m_crews = (CameraCrew *)malloc(m_numCrews * 48);
    m_probabilities = (CameraProbabilities *)malloc(m_numCrews * sizeof(CameraProbabilities));
    m_links = (CameraLink *)malloc(m_numCrews * sizeof(CameraLink));
    for (int i = 0; i < m_numCrews; i++) {
        char name[16];
        _D3DVECTOR position;
        float maxFOV;
        float minFOV;
        float maxFOVDistance;
        float minFOVDistance;
        int percent;
        int branch;
        int next;
        sprintf(name, "Camera%d", i + 1);
        block.ReadParameterBlock(name);
        _D3DVECTOR origin(0.0f, 0.0f, 0.0f);
        block.GetParameter("Position", &origin, &position);
        block.GetParameter("FOV", -1.0f, &maxFOV);
        if (maxFOV == -1.0f) {
            block.GetParameter("MaxFOV", 50.0f, &maxFOV);
            block.GetParameter("MinFOV", 9.0f, &minFOV);
        } else {
            minFOV = maxFOV;
        }
        block.GetParameter("MaxFOVDistance", 250.0f, &maxFOVDistance);
        block.GetParameter("MinFOVDistance", 5.0f, &minFOVDistance);
        block.GetParameter("Branch", -1, &branch);
        if (branch >= 0) {
            m_links[i].m_branch = branch - 1;
            if (i == m_numCrews - 1)
                m_links[i].m_next = 0;
            else
                m_links[i].m_next = i + 1;
        } else {
            m_links[i].m_branch = -1;
            block.GetParameter("Next", -1, &next);
            if (next >= 0)
                m_links[i].m_next = next - 1;
            else if (i == m_numCrews - 1)
                m_links[i].m_next = 0;
            else
                m_links[i].m_next = i + 1;
        }
        AddCameraCrew(&position, maxFOV, minFOV, maxFOVDistance, minFOVDistance, -1);
        block.GetParameter("TV", 50, &percent);
        m_probabilities[i].p[0] = 100 - percent;
        block.GetParameter("Follow", 10, &percent);
        m_probabilities[i].p[1] = m_probabilities[i].p[0] - percent;
        block.GetParameter("Side", 20, &percent);
        m_probabilities[i].p[2] = m_probabilities[i].p[1] - percent;
        block.GetParameter("LookBack", 0, &percent);
        m_probabilities[i].p[3] = m_probabilities[i].p[2] - percent;
    }
    return 1;
}


// ---- src/SimpleTVCamera/SwitchOn.cpp (ResetTrace)

void JetskiTVCamera::ResetTrace()
{
    m_currentCrew = 0;
    m_switchDelay = 0.6f;
}


// ---- src/JetskiTVCamera/GetTheBestCamera.cpp
// GetTheBestCamera__14JetskiTVCameraFfP10_D3DVECTORP10_D3DVECTOR  @ 0x003140D0
// The previous crew's distance is written out by hand (not the helper) and the
// next one is a ?:, which give retail's FP registers. The explicit else arms in the neighbour choice make
// mwcc's block optimizer thread a branch, which rebuilds the predecessor lists, so
// the slot filler moves the reset block's lui instead of copying it and fills the
// loop branch from the fall-through.


static inline float DistanceSquared(_D3DVECTOR *a, _D3DVECTOR *b)
{
    float dx = a->x - b->x;
    float dy = a->y - b->y;
    float dz = a->z - b->z;
    return dx * dx + dy * dy + dz * dz;
}

// Starts at the nearest crew, then only moves to a neighbouring crew that is
// nearer (at most once every 0.6 seconds). Returns 1 when the crew changed.
int JetskiTVCamera::GetTheBestCamera(float dt, _D3DVECTOR *target, _D3DVECTOR *position)
{
    int changed = 0;
    int best;
    m_previousCrew = m_currentCrew;
    if (m_currentCrew < 0) {
        float bestDist = 3.40282347e+38f;
        for (int i = 0; i < m_numCrews; i++) {
            float dist = DistanceSquared(target, &m_crews[i].m_position);
            if (bestDist > dist) {
                bestDist = dist;
                best = i;
            }
        }
        m_switchDelay = 0.6f;
        changed = 1;
        m_currentCrew = best;
    } else {
        int previous = m_links[m_currentCrew].m_next;      // the links as neighbours
        int next = m_links[m_currentCrew].m_branch;
        float previousDist;
        float nextDist;
        float currentDist;
        {
            _D3DVECTOR *p = &m_crews[previous % m_numCrews].m_position;
            float dx = target->x - p->x;
            float dy = target->y - p->y;
            float dz = target->z - p->z;
            previousDist = dx * dx + dy * dy + dz * dz;
        }
        nextDist = next < 0 ? 3.40282347e+38f
                           : DistanceSquared(target, &m_crews[next % m_numCrews].m_position);
        currentDist = DistanceSquared(target, &m_crews[m_currentCrew].m_position);
        if (currentDist < previousDist) {
            if (currentDist < nextDist)
                best = m_currentCrew;
            else
                best = next;
        } else {
            if (previousDist < nextDist)
                best = previous;
            else
                best = next;
        }
        if (m_currentCrew != best) {
            if (m_switchDelay > 0.0f) {
                m_switchDelay -= dt;
            } else {
                m_switchDelay = 0.6f;
                changed = 1;
                m_currentCrew = best;
            }
        }
    }
    *position = m_crews[m_currentCrew].m_position;
    return changed;
}


// ---- src/JetskiTVCamera/ComputeCameraData.cpp


void JetskiTVCamera::ComputeCameraData(float dt, int &cameraIndex, _D3DVECTOR *a, _D3DVECTOR *b,
                                       float *c, _D3DVECTOR *d, _D3DVECTOR *e, float *f)
{
    SimpleTVCamera::ComputeCameraData(dt, cameraIndex, a, b, c, d, e, f);
    if (LpGame->m_debugOverlay) {
        if (g_DebugOverlayID < 0)
            g_DebugOverlayID = LpGame->m_debugOverlay->m_numPages++;
        LpGame->m_debugOverlay->PrintfHeading(g_DebugOverlayID, "TV Cinema Camera Info");
        LpGame->m_debugOverlay->PrintfDetail(g_DebugOverlayID, "Camera No: %d", m_currentCrew + 1);
        LpGame->m_debugOverlay->PrintfDetail(g_DebugOverlayID, "POS: %4.2f,%4.2f,%4.2f", m_position.x, m_position.y, m_position.z);
        LpGame->m_debugOverlay->PrintfDetail(g_DebugOverlayID, "FOV: %f", m_fov);
    }
}


// ---- src/JetskiTVCamera/GetProbabilities.cpp
#pragma fast_fptosi on  // retail never calls fptosi: float->int is an inline cvt.w.s

// Retail's fmax is an inline-asm helper: max.s into $f8, stored through a
// pointer to a stack temporary.
inline float Fmax(float a, float b)
{
    float r;
    asm("max.s $f8, %1, %2; swc1 $f8, 0(%0)" : : "r"(&r), "f"(a), "f"(b) : "$f8", "memory");
    return r;
}

void JetskiTVCamera::GetProbabilities(int *out)
{
    out[0] = (int)Fmax(m_probabilities[m_currentCrew].p[0] - 20.0f, 0.0f);
    out[1] = (int)m_probabilities[m_currentCrew].p[1];
    out[2] = (int)m_probabilities[m_currentCrew].p[2];
    out[3] = (int)m_probabilities[m_currentCrew].p[3];
}


// ---- src/JetskiTVCamera/ComputePosition.cpp

_D3DVECTOR JetskiTVCamera::ComputePosition(float time, _D3DVECTOR target, _D3DVECTOR velocity)
{
    return m_crews[m_currentCrew].m_position;
}


// ---- src/sinit/JetskiTVCamera.cpp
// Static initializer of JetskiTVCamera.cpp: only the axis vectors from the shared header.

