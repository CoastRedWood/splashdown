// JetskiWreckCam.cpp  @ 0x0033B930 (text 0x33B930-0x33BBE0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's JetskiWreckCam.cpp as one translation unit: JetskiWreckCamera,
// then the static initializer (only the axis vectors). Its .rodata starts at
// @873 (labels restart after SimpleWreckCam.cpp's @1927; RTTI
// JetskiWreckCamera, its own CameraBehavior and SimpleWreckCamera copies).



#include "SimpleWreckCamera/SimpleWreckCamera.h"
#include "d3dtypes.h"
#include "sinit/AxisVectors.h"
#include "Terrain/Terrain.h"
#include "Water/Water.h"
#include "JetskiWreckCamera/JetskiWreckCamera.h"
#include "CameraFilter/CameraFilter.h"

// ---- src/JetskiWreckCamera/ctor.cpp

// The SimpleWreckCamera view of Initialize's parameters (8 floats).
struct WreckCameraParams {
    float v[8];
};

JetskiWreckCamera::JetskiWreckCamera()
{
    m_terrain = 0;
    m_water = 0;
    m_distance = 3.40282347e+38f;
}


// ---- src/JetskiWreckCamera/dtor.cpp


JetskiWreckCamera::~JetskiWreckCamera()
{
}


// ---- src/JetskiWreckCamera/Initialize.cpp

int JetskiWreckCamera::Initialize(float time, WreckCameraParams params, float distance, float a, float b, float height,
                                  float c)
{
    m_height = height;
    m_distance = distance;
    SimpleWreckCamera::Initialize(time, params, a, b, c);
    if (m_positionFilters[1])
        m_positionFilters[1]->m_rate = 0.2f;
}


// ---- src/JetskiWreckCamera/ConstrainCamera.cpp
static inline float Fmin(float a, float b)
{
    float r;
    asm("min.s $f8, %1, %2; swc1 $f8, 0(%0)" : : "r"(&r), "f"(a), "f"(b) : "$f8", "memory");
    return r;
}





// Keep the camera at least m_height above the terrain or water, and no
// higher than m_distance.
int JetskiWreckCamera::ConstrainCamera(_D3DVECTOR *position, int adjust)
{
    _D3DVECTOR ground = *position;
    _D3DVECTOR water = ground;

    if (m_terrain) {
        m_terrain->GetGridPointIntersection(&ground, 0, 0, 0);
        if (m_water) {
            m_water->GetGridPointIntersection(&water, 0);
            if (ground.y < water.y)
                ground.y = water.y;
        }
    } else if (m_water) {
        m_water->GetGridPointIntersection(&ground, 0);
    } else {
        return 1;
    }
    position->y = Fmin(position->y, m_distance);
    float height = position->y - ground.y;
    if (height < m_height) {
        if (adjust) {
            position->y = ground.y + m_height;
            return 1;
        }
        return 0;
    }
    return 1;
}

