// JetskiThrillCam.cpp  @ 0x0030E580 (text 0x30E580-0x30EE90)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's JetskiThrillCam.cpp as one translation unit, in BuoyObject.cpp's tu_map
// row (no class is named after the file): JetskiThrillCamera's five functions,
// then the static initializer (__sinit_JetskiThrillCam.cpp, the .ctor slot after
// BuoyObject.cpp's: the axis vectors only). Its .rodata (0x430C60-0x430CE0) is the
// JetskiThrillCamera/SimpleThrillCamera/CameraBehavior RTTI records. `make link`
// places the object's data as well as its code (tools/link_c.py).




// ComputePosition's per-function file set it; TU-wide (read at the end of the TU).
#pragma inline_depth(6)

#include "d3dtypes.h"
#include "JetskiThrillCamera/JetskiThrillCamera.h"
#include "SimpleThrillCamera/SimpleThrillCamera.h"
#include "JetskiVehicle/JetskiVehicle.h"
#include "PhysicsBody/PhysicsBody.h"
#include "PhysicsRigidBody/PhysicsRigidBody.h"
#include "Terrain/Terrain.h"
#include "Water/Water.h"

#include "sinit/AxisVectors.h"

static inline float Fmin(float a, float b)
{
    float r;
    asm("min.s $f8, %1, %2; swc1 $f8, 0(%0)" : : "r"(&r), "f"(a), "f"(b) : "$f8", "memory");
    return r;
}

static inline float Fmax(float a, float b)
{
    float r;
    asm("max.s $f8, %1, %2; swc1 $f8, 0(%0)" : : "r"(&r), "f"(a), "f"(b) : "$f8", "memory");
    return r;
}

static inline float Fabs(float x)
{
    float r;
    asm("abs.s $f8, %1; swc1 $f8, 0(%0)" : : "r"(&r), "f"(x) : "$f8", "memory");
    return r;
}

// ---- src/JetskiThrillCamera/ctor.cpp
JetskiThrillCamera::JetskiThrillCamera()
{
    m_terrain = 0;
    m_water = 0;
    m_jetski = 0;
    m_x68 = 0;
    m_minHeight = 3.0f;
    m_maxHeight = 3.40282347e+38f;      // FLT_MAX
}

// ---- src/JetskiThrillCamera/dtor.cpp

JetskiThrillCamera::~JetskiThrillCamera()
{
}

// ---- src/JetskiThrillCamera/Initialize.cpp

int JetskiThrillCamera::Initialize(float a, float b, float c, float d, float e, float f)
{
    m_minHeight = e;
    m_maxHeight = f;
    m_maxFov = a;
    m_minFov = b;
    m_farDistance = c;
    m_nearDistance = d;
    return 1;
}

// ---- src/JetskiThrillCamera/ConstrainCamera.cpp

// Keep the camera between the ground (or water surface) and its ceiling.
// Returns 0 when it's too low and may not be moved.
int JetskiThrillCamera::ConstrainCamera(_D3DVECTOR *camera, int adjust, _D3DVECTOR *ground)
{
    *ground = *camera;
    _D3DVECTOR water = *ground;
    if (m_terrain) {
        m_terrain->GetGridPointIntersection(ground, 0, 0, 0);
        if (m_water) {
            m_water->GetGridPointIntersection(&water, 0);
            if (ground->y < water.y)
                ground->y = water.y;
        }
    } else {
        if (m_water)
            m_water->GetGridPointIntersection(ground, 0);
        else
            return 1;
    }
    camera->y = Fmin(camera->y, m_maxHeight);
    float clearance = camera->y - ground->y;
    if (clearance < m_minHeight) {
        if (adjust) {
            camera->y = ground->y + m_minHeight;
            return 1;
        }
        return 0;
    }
    return 1;
}

// ---- src/JetskiThrillCamera/ComputePosition.cpp
// Statement order from osearch.py, then declaration order `before, back, s, t, k`.




_D3DVECTOR JetskiThrillCamera::ComputePosition(_D3DVECTOR target, _D3DVECTOR velocity)
{
    _D3DVECTOR look;
    _D3DVECTOR flat;
    _D3DVECTOR position;
    if (m_jetski->m_body->m_x30) {
        _D3DVECTOR vpos = m_jetski->m_x5A0;
        _D3DVECTOR vdir = m_jetski->m_x590;
        float back = 1.5f * ((1.0f / 32.2f) * vdir.y);
        position = vpos + back * vdir + -0.2f * vdir + 0.45f * _D3DVECTOR(vdir.z, 0.0f, -vdir.x);
        if (m_terrain) {
            m_terrain->GetGridPointIntersection(&position, 0, 0, 0);
            if (m_water) {
                _D3DVECTOR water = position;
                m_water->GetGridPointIntersection(&water, 0);
                if (position.y < water.y)
                    position.y = water.y;
            }
        } else if (m_water) {
            m_water->GetGridPointIntersection(&position, 0);
        }
        if (!(vdir.y <= 10.0f))
            vpos.y += 0.19f * (vdir.y * back);
        position.y = Fmax(vpos.y - 1.0f, 1.0f + (position.y + m_minHeight));
    } else {
        float before;
        float back;
        float s;
        float t;
        float k;
        k = 1.5f;
        t = (1.0f / 32.2f) * velocity.y;
        s = 2.0f;
        flat = velocity;
        flat.y = 0.0f;
        s *= t;
        look = target + s * flat;
        before = look.y;
        _D3DVECTOR ground;
        back = k * t;
        ConstrainCamera(&look, 1, &ground);
        float drop = before - look.y;
        if (!(drop <= 1.0f))
            back += 0.5f * (drop / Fabs(velocity.y));
        position = target + back * velocity + -0.2f * velocity + 0.45f * _D3DVECTOR(velocity.z, 0.0f, -velocity.x);
        ConstrainCamera(&position, 1, &ground);
        position.y = Fmin(15.0f + (ground.y + m_minHeight), 1.0f + (position.y + m_minHeight));
    }
    return position;
}

// ---- src/sinit/JetskiThrillCam.cpp
// Static initializer of JetskiThrillCam.cpp: only the axis vectors from the shared header.
