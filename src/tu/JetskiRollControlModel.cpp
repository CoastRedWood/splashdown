// JetskiRollControlModel.cpp  @ 0x003D82A0 (text 0x3D82A0-0x3D8590)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's JetskiRollControlModel.cpp as one translation unit: its four
// functions, then its static initializer. `make link` places the object's
// data as well as its code (tools/link_c.py). The rest of the tu_map row is
// three unanchored TUs, in .ctor order: JetskiParticleEmittersSplash.cpp
// (0x3D8590-0x3D9130), JetskiParticleEmittersJet.cpp (0x3D9130-0x3D9DB0)
// and JetskiParticleEmittersDrip.cpp (0x3D9DB0-0x3DA870), each a src/tu
// file.


#include "JetskiRollControlModel/JetskiRollControlModel.h"
#include "PhysicsBody/PhysicsBody.h"
#include "SoultreeObject/SoultreeObject.h"
#include "sinit/AxisVectors.h"

// ---- src/JetskiRollControlModel/ctor.cpp

JetskiRollControlModel::JetskiRollControlModel(int enabled) : GameObject(enabled)
{
    m_x44 = 1.0f;
    m_x48 = 1.0f;
    m_x4C = 1.0f;
    m_x58 = 15.0f;
    m_x50 = 0.0f;
    m_maxAngle = 45.0f * 0.01745329f;   // 45 degrees
    m_x40 = 1.0f / 32.2f;
    m_enabled = 1;
    m_x38 = 0;
    m_x3C = 1;
    m_up = YVector;
}


// ---- src/JetskiRollControlModel/dtor.cpp

JetskiRollControlModel::~JetskiRollControlModel()
{
}


// ---- src/JetskiRollControlModel/Create.cpp
// (The per-function file also held JetParticleEmitter::SetJet and
// DripParticleEmitter::StopDrip, now in their own TUs.)

void JetskiRollControlModel::Create(PhysicsBody *body)
{
    m_body = body;
}


// ---- src/JetskiRollControlModel/SetForceTick.cpp

static inline float Fabs(float x)
{
    float r;
    asm("abs.s $f8, %1; swc1 $f8, 0(%0)" : : "r"(&r), "f"(x) : "$f8", "memory");
    return r;
}

// Steer the hull's roll towards a lean proportional to the yaw rate.
int JetskiRollControlModel::SetForceTick(float dt)
{
    _D3DVECTOR torque;
    _D3DVECTOR front;
    _D3DVECTOR up;
    if (m_enabled) {
        m_body->m_object->GetOrientation(0, &front, &up);
        if (Fabs(front.y) < 0.8f) {
            float angle = m_x40 * (0.222000003f * m_x44 * m_body->m_xB0.y);
            if (angle > m_maxAngle)
                angle = m_maxAngle;
            else if (angle < -m_maxAngle)
                angle = -m_maxAngle;
            if (m_body->m_lastVelocity.z < 0.0f)
                angle = -angle;
            float roll = 500.0f * m_x4C * (10.0f * -m_x48 * (angle - m_body->m_x100.z) - m_body->m_xB0.z);
            torque.x = 0.0f;
            torque.y = 0.0f;
            torque.z = roll;
            m_body->AddTorque(torque);
        }
    }
    return 1;
}


// ---- src/sinit/JetskiRollControlModel.cpp
// Static initializer of JetskiRollControlModel.cpp: only the axis vectors from the shared header.
