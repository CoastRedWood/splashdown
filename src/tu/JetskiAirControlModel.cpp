// JetskiAirControlModel.cpp  @ 0x002F4620 (text 0x2F4620-0x2F4C80)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's JetskiAirControlModel.cpp as one translation unit: its functions in
// retail order, then its static initializer. `make link` places the object's data
// as well as its code (tools/link_c.py).



#include "GameObject/GameObject.h"
#include "d3dtypes.h"
#include "JetskiAirControlModel/JetskiAirControlModel.h"
#include "PhysicsBody/PhysicsBody.h"
#include "SoultreeObject/SoultreeObject.h"
#include "sinit/AxisVectors.h"

// ---- src/JetskiAirControlModel/ctor.cpp

JetskiAirControlModel::JetskiAirControlModel(int id) : GameObject(id)
{
    m_x34 = 0;
    m_x38 = 1;
    m_x3C = 0;
    m_x40 = 0;
    m_x44 = 0;
    m_x48 = 0;
    m_x4C = 0;
    m_x50 = 0.25f;
    m_x58 = 400.0f;
    m_x54 = 250.0f;
    m_x5C = 1.0f;
    m_x64 = 1.0f;
    m_x70.x = -0.5f;
    m_x70.y = 0.5f;
    m_x70.z = -0.25f;
}


// ---- src/JetskiAirControlModel/dtor.cpp

JetskiAirControlModel::~JetskiAirControlModel()
{
}


// ---- src/HydrodynamicsModel/ResetHullPoints.cpp (SetBody)

void JetskiAirControlModel::SetBody(PhysicsBody *body)
{
    m_body = body;
}


// ---- src/JetskiAirControlModel/SetInputs.cpp

void JetskiAirControlModel::SetInputs(int a, int b, int c, float d, float e)
{
    m_x38 = a;
    m_x3C = b;
    m_x40 = c;
    m_x44 = d;
    m_x48 = e;
}


// ---- src/JetskiAirControlModel/SimulationTick.cpp

static inline float Fabs(float x)
{
    float r;
    asm("abs.s $f8, %1; swc1 $f8, 0(%0)" : : "r"(&r), "f"(x) : "$f8", "memory");
    return r;
}

inline float RSqrt(float x)
{
    float r;
    asm("rsqrt.s $f8, %0, %1; swc1 $f8, 0(%2)" : : "f"(1.0f), "f"(x), "r"(&r) : "$f8", "memory");
    return r;
}

// Normalizes v in place (the zero vector stays zero).
inline void Normalize(_D3DVECTOR &v)
{
    float len2 = v.x * v.x + v.y * v.y + v.z * v.z;
    if (len2 == 0.0f) {
        v = ZeroVector;
    } else {
        float r = RSqrt(len2);
        v.x *= r;
        v.y *= r;
        v.z *= r;
    }
}

// xyz dot product on VU0.
static inline float DotProduct(_D3DVECTOR *a, _D3DVECTOR *b)
{
    float r;
    asm("lqc2 vf4, 0x0(%1); lqc2 vf5, 0x0(%2); vmul.xyz vf5, vf4, vf5; vaddy.x vf5, vf5, vf5y; vaddz.x vf5, vf5, vf5z; qmfc2.ni $2, vf5; mtc1 $2, %0"
        : "=f"(r) : "r"(a), "r"(b) : "$2");
    return r;
}

int JetskiAirControlModel::SimulationTick(float dt)
{
    _D3DVECTOR torque;
    if (m_x38) {
        m_x4C = 0.0f;
    } else {
        torque = 20.0f * -m_x60 * m_body->m_xB0;
        m_body->AddTorque(torque);
        m_body->AddForce(32.2f * (-(m_x64 - 1.0f) * m_body->m_mass) * YVector);
        m_x4C += dt;
        if (!m_x40) {
            torque.x = m_x48 * (m_x70.x * m_x54);
            torque.y = m_x44 * (m_x70.y * m_x54);
            torque.z = m_x44 * (m_x70.z * m_x54);
            if (m_x3C)
                torque *= 0.5f;
            m_body->AddTorque(torque);
            _D3DVECTOR dir;
            dir.x = m_body->m_object->m_worldMatrix._11;
            dir.y = 0.0f;
            dir.z = m_body->m_object->m_worldMatrix._13;
            Normalize(dir);
            float speed = DotProduct(&m_body->m_velocity, &dir);
            _D3DVECTOR force = m_x58 * m_x44 * dir;
            if (m_x3C)
                force *= 0.5f;
            m_body->AddForce(force);
        }
        if (m_x3C || Fabs(m_x44) < 0.15f) {
            if (Fabs(m_x48) < 0.15f) {
            torque.x = 0.0f;
            torque.y = 0.0f;
            torque.z = 100.0f * -m_x5C * m_body->m_x100.z;
            m_body->AddTorque(torque);
            }
        }
    }
    return 1;
}


// ---- src/sinit/JetskiAirControlModel.cpp
// Static initializer of JetskiAirControlModel.cpp: only the axis vectors from the shared header.

