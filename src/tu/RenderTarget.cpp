// RenderTarget.cpp  @ 0x002858E0 (text 0x2858E0-0x285AC0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// RenderTarget as one translation unit without a static initializer, at the
// end of PSXRenderTarget.cpp's tu_map row: its .rodata restarts at @561 after
// the ColorMapper RTTI and ends with the RenderTarget RTTI. GetEnumerator (no
// data) closes it.


#include "RenderTarget/RenderTarget.h"
#include "Camera/Camera.h"

// Defined here for its retail .bss slot.
RenderTarget *LpRenderTarget;

// ---- src/RenderTarget/ctor.cpp

RenderTarget::RenderTarget()
{
    m_camera = 0;
    m_hasDepth = 0;
    m_frameCount = 0;
    m_bufferId = 0;
    m_cycleCount = 0;
    m_screenType = 0;
    m_x30 = 0;
    m_x34 = 0;
    m_x38 = 0;
    m_x3C = 0;
}


// ---- src/RenderTarget/Create.cpp
void RenderTarget::Create(PSXVideoCard *card)
{
}


// ---- src/RenderTarget/dtor.cpp

RenderTarget::~RenderTarget()
{
}


// ---- src/RenderTarget/NextCyclingBufferId.cpp
void RenderTarget::NextCyclingBufferId()
{
    if (++m_bufferId == m_frameCount)
        m_bufferId = 0;
    m_cycleCount++;
}


// ---- src/RenderTarget/SetCamera.cpp

// A camera whose surface differs from the target's gets the 4:3 aspect.
void RenderTarget::SetCamera(Camera *camera)
{
    m_camera = camera;
    if (m_camera) {
        if (m_width != m_camera->m_surfaceWidth || m_height != m_camera->m_surfaceHeight)
            m_camera->SurfaceChanged(1.33333337f);
    }
}


// ---- src/RenderTarget/TrySendRenderQueue.cpp

int RenderTarget::TrySendRenderQueue(int wait)
{
    return 0;
}


// ---- src/RenderTarget/EnableAlphaTest.cpp

void RenderTarget::EnableAlphaTest(unsigned char ref)
{
}


// ---- src/RenderTarget/DisableAlphaTest.cpp

void RenderTarget::DisableAlphaTest()
{
}


// ---- src/global/GetEnumerator.cpp
extern "C" int strcasecmp(const char *a, const char *b);

struct ObjectEnumerator {
    char *m_name;
    int m_value;
};

// Looks name up in a table terminated by an entry with an empty name.
int GetEnumerator(int *value, char *name, ObjectEnumerator *table)
{
    while (*table->m_name != 0) {
        if (strcasecmp(name, table->m_name) == 0) {
            *value = table->m_value;
            return 1;
        }
        table++;
    }
    return 0;
}


