// CutSceneCamera.cpp  @ 0x002F98C0 (text 0x2F98C0-0x2F9D10)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's CutSceneCamera.cpp as one translation unit: CutSceneCamera's five
// functions, then its static initializer. `make link` places the object's
// data as well as its code (tools/link_c.py). The rest of the tu_map row is
// two more TUs: MemoryCard.cpp (0x2F9D10-0x2F9D70, src/tu/MemoryCard.cpp) and
// JetskiParticleEmitters.cpp (0x2F9D70-0x2FB1C0, BowSprayParticleEmitter;
// src/tu/JetskiParticleEmitters.cpp: its .rodata restarts the BaseObject RTTI
// at 0x4216F8 and its compiler labels jump from @1050 to @1948).



#include "CutSceneCamera/CutSceneCamera.h"
#include "sinit/AxisVectors.h"

// ---- src/CutSceneCamera/ctor.cpp

CutSceneCamera::CutSceneCamera(int enabled) : PSXCamera(enabled)
{
    m_frameRate = 30.0f;
    m_time = 0;
    m_playing = 0;
    m_direction = 2;
    m_lastFrame = 0;
    m_matrices = 0;
    m_fovs = 0;
    m_380 = 0;
    m_384 = 0;
    m_388 = 0;
    m_loaded = 0;
}


// ---- src/CutSceneCamera/Create.cpp

GameObject *CutSceneCamera::Create()
{
    PSXCamera::Create();
    return this;
}


// ---- src/CutSceneCamera/Tick.cpp
// Compiler session state: retail compiled
// this file in an mwcc session that had already generated a runtime helper call
// with a float argument (fptodp/fptoui take it in $f12). mwcc then treats $f12
// as read by later code, which makes the prologue copy `this` before saving dt
// (dt's copy moves into the delay slot). This unused function recreates that
// state; it is not part of the retail file.
static double MWCC_SHIM_TEXT MwccSessionStateF(float f) { return f; }

int CutSceneCamera::Tick(float dt)
{
    if (!m_playing || !m_loaded) {
        return PSXCamera::Tick(dt);
    }
    if (m_direction == 1)
        m_time -= dt;
    else
        m_time += dt;
    while (!(m_time <= m_frameRate * m_numFrames))
        m_time -= m_frameRate * m_numFrames;

    unsigned long frame = (unsigned long)(m_time * m_frameRate);
    while (frame >= m_numFrames)
        frame -= m_numFrames;

    if (m_direction == 0) {
        if (m_lastFrame >= frame) {
            frame = m_numFrames - 1;
            m_playing = 0;
        } else {
            m_lastFrame = frame;
        }
    } else if (m_direction == 1) {
        if (m_firstFrame < frame || frame == 0) {
            m_time = 0;
            m_playing = 0;
            frame = 0;
        } else {
            m_firstFrame = frame;
        }
    }

    m_view = m_matrices[frame];
    m_lookAt.x = m_view._31;
    m_lookAt.y = m_view._32;
    m_lookAt.z = m_view._33;
    m_up.x = m_view._21;
    m_up.y = m_view._22;
    m_up.z = m_view._23;
    m_position.x = m_view._41;
    m_position.y = m_view._42;
    m_position.z = m_view._43;
    SetFOV(m_fovs[frame]);
    return PSXCamera::Tick(dt);
}


// ---- src/CutSceneCamera/ReleaseMemory.cpp
extern "C" void free(void *p);

void CutSceneCamera::ReleaseMemory()
{
    if (m_matrices) {
        free(m_matrices);
        m_matrices = 0;
    }
    if (m_fovs) {
        free(m_fovs);
        m_fovs = 0;
    }
    if (m_380) {
        free(m_380);
        m_380 = 0;
    }
    if (m_384) {
        free(m_384);
        m_384 = 0;
    }
    if (m_388) {
        free(m_388);
        m_388 = 0;
    }
}


// ---- src/CutSceneCamera/dtor.cpp
// Retail inlines ~PSXCamera here (with its own this != 0 test): it is
// inline in PSXCamera.h.

CutSceneCamera::~CutSceneCamera()
{
    if (m_loaded)
        ReleaseMemory();
}


// ---- src/sinit/CutSceneCamera.cpp
// Static initializer of CutSceneCamera.cpp: only the axis vectors from the shared header.
