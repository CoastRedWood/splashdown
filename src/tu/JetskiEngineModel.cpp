// JetskiEngineModel.cpp  @ 0x00252EF0 (text 0x252EF0-0x253D00)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's JetskiEngineModel.cpp as one translation unit: its functions in
// retail order, then its static initializer. `make link` places the object's
// data as well as its code (tools/link_c.py).



#include "GameObject/GameObject.h"
#include "d3dtypes.h"
#include "JetskiEngineModel/JetskiEngineModel.h"
#include "SoultreeObject/SoultreeObject.h"
#include "Water/Water.h"
#include "PhysicsBody/PhysicsBody.h"
#include "PhysicsRigidBody/PhysicsRigidBody.h"
#include "PSXWater/PSXWater.h"
#include "sinit/AxisVectors.h"

// ---- src/JetskiEngineModel/ctor.cpp


JetskiEngineModel::JetskiEngineModel(int enabled) : GameObject(enabled)
{
    m_throttle = 0.0f;
    m_x48 = 0.0f;
    m_x4C = 0.0f;
    m_x44 = 0.0f;
    m_dryTime = 0.0f;
    m_maxRPM = 1256.0f;
    m_xE4 = 0.0f;
    m_xE8 = 0.25f;
    m_xEC = 0.0f;
    m_xF0 = 30.0f;
    m_xF4 = 80.0f;
    m_xF8 = 10.0f;
    m_x100 = 43.9998f;
    m_x104 = 73.333f;
    m_x50 = 135.0f;
    m_x54 = 135.0f;
    m_xFC = 1.0f;
    m_x108 = 1.0f;
    m_x10C = 1.0f;
    m_x110 = 1.0f;
    m_x30 = 0;
    m_inWater = 0;
    m_x34 = 1;
    m_reversed = 0;
    m_x3C = 0;
    m_x60 = ZeroVector;
    m_x70 = ZeroVector;
    m_x80 = ZeroVector;
    m_x90 = ZeroVector;
    m_thrust = ZeroVector;
    m_thrustPoint = ZeroVector;
    m_torque1 = ZeroVector;
    m_torque2 = ZeroVector;
}


// ---- src/JetskiEngineModel/dtor.cpp


JetskiEngineModel::~JetskiEngineModel()
{
}


// ---- src/JetskiEngineModel/Create.cpp


GameObject *JetskiEngineModel::Create(PhysicsRigidBody *body, PSXWater *water)
{
    m_body = body;
    m_water = water;
    return GameObject::Create();
}


// ---- src/JetskiEngineModel/Tick.cpp


int JetskiEngineModel::Tick(float dt)
{
    if (!m_x30)
        SimulationTick(dt);
    return GameObject::Tick(dt);
}


// ---- src/JetskiEngineModel/SimulationTick.cpp
// (Same as Tick__16UIWidgetSoultreeFf, different callees.)

int JetskiEngineModel::SimulationTick(float dt)
{
    UpdateForceTick(dt);
    SetForceTick(dt);
    return 1;
}


// ---- src/JetskiEngineModel/UpdateForceTick.cpp
#pragma inline_depth(6)


void FastSinCosTan(float angle, float *s, float *c, float *t);

static inline float Fmax(float a, float b)
{
    float r;
    asm("max.s $f8, %1, %2; swc1 $f8, 0(%0)" : : "r"(&r), "f"(a), "f"(b) : "$f8", "memory");
    return r;
}

static inline float Fmin(float a, float b)
{
    float r;
    asm("min.s $f8, %1, %2; swc1 $f8, 0(%0)" : : "r"(&r), "f"(a), "f"(b) : "$f8", "memory");
    return r;
}

// Clamps v in place to the range between a and b (either order).
static inline void ClampRef(float &v, float a, float b)
{
    if (b > a) {
        if (v > b)
            v = b;
        else if (v < a)
            v = a;
    } else {
        if (v < b)
            v = b;
        else if (v > a)
            v = a;
    }
}

// Maps x from [x0, x1] onto [y0, y1], clamped to the output range.
static inline float MapRange(float x, float x0, float y0, float x1, float y1)
{
    float y;
    if (x1 != x0) {
        // Clamped in place through a reference, then copied: the copy leaves the
        // empty join block (unfilled `b; nop`) retail has, the in-place clamp its FP registers.
        float v = y0 + (x - x0) * (y1 - y0) / (x1 - x0);
        ClampRef(v, y0, y1);
        y = v;
    } else
        y = 0.0f;
    return y;
}


int JetskiEngineModel::UpdateForceTick(float dt)
{
    m_thrust = ZeroVector;
    m_thrustPoint = ZeroVector;
    m_torque1 = ZeroVector;
    m_torque2 = ZeroVector;
    _D3DVECTOR position = m_body->m_object->TransformVectorLocalToWorld(&m_x60);
    m_x90 = position;
    m_water->GetGridPointIntersection(&m_x90, 0);
    if (m_x90.y >= position.y || m_x3C) {
        m_inWater = 1;
        m_dryTime = 0;
    } else {
        m_inWater = 0;
        m_dryTime += dt;
    }
    _D3DVECTOR velocity = m_body->m_velocity;
    _D3DVECTOR localVelocity = m_body->m_object->RotateVectorWorldToLocal(&velocity);
    float speed = localVelocity.z;
    if (m_throttle > 0.0f && !m_body->m_x30 && m_x34) {
        float angle;
        float s, c, t;
        if (m_inWater) {
            angle = m_x48 * MapRange(speed, m_xEC, 0.01745329f * m_xF0, m_xF4, 0.01745329f * m_xF8);
            float scale;
            if (m_x4C <= 0.0f)
                scale = MapRange(m_x4C, 0.0f, 1.0f, -1.0f, m_x10C);
            else
                scale = MapRange(m_x4C, 0.0f, 1.0f, 1.0f, m_x110);
            angle *= scale;
        } else {
            angle = m_x48 * m_xFC;
        }
        if (m_reversed)
            FastSinCosTan(-angle, &s, &c, &t);
        else
            FastSinCosTan(angle, &s, &c, &t);
        _D3DVECTOR forward;
        _D3DVECTOR up;
        m_body->m_object->GetOrientation(&forward, &up);
        float pitch = MapRange(forward.y, 0.707f, 1.0f, 1.0f, 0.0f);
        float base = m_x50 + m_x44 * m_x54;
        float force = pitch * MapRange(m_body->m_speed, base, 1.0f, 10.0f + base, 0.0f);
        force = m_throttle * (force * (m_maxRPM + m_x44 * m_xE4));
        static float StartDryThrustCutoffTime = 1.0f;
        static float EndDryThrustCutoffTime = 1.5f;
        if (!m_inWater)
            force *= m_xE8 * MapRange(m_dryTime, StartDryThrustCutoffTime, 1.0f, EndDryThrustCutoffTime, 0.0f);
        m_thrust.x = -force * s;
        m_thrust.z = force * c;
        m_thrustPoint.x = m_body->m_centerOfMass.x + m_x70.x;
        m_thrustPoint.y = m_body->m_centerOfMass.y;
        if (m_reversed)
            m_thrustPoint.z = -(m_body->m_centerOfMass.z + m_x70.z);
        else
            m_thrustPoint.z = m_body->m_centerOfMass.z + m_x70.z;
        if (m_inWater)
        {
            float lever = MapRange(m_body->m_speed, m_x100, m_x70.y, m_x104, m_x80.y);
            m_torque2.x = lever * m_thrust.z;
        }
        else
            m_torque2.x = 0;
    }
    if (!m_body->m_x30)
    {
        float idle = 1.0f - m_throttle;
        float scale = Fmin(Fmax(0.1f * speed, 0.0f), 1.0f);
        m_torque1.y = m_x108 * (scale * (m_x48 * idle));

    }
    return 1;
}


// ---- src/JetskiEngineModel/SetForceTick.cpp
// (Was `#pragma inline_depth(reset)`, which mwcc rejects as an illegal
// pragma and ignores: inline_depth is effectively TU-wide)

// VU0 cross product, out = a x b.
static inline void CrossProduct(_D3DVECTOR &out, const _D3DVECTOR &a, const _D3DVECTOR &b)
{
    asm("lqc2 vf4, 0x0(%1); lqc2 vf5, 0x0(%2); vopmula.xyz ACC, vf4, vf5; vopmsub.xyz vf6, vf5, vf4; vsub.w vf6, vf6, vf6; sqc2 vf6, 0x0(%0)"
        : : "r"(&out), "r"(&a), "r"(&b) : "memory");
}


// Apply the engine thrust (and the torque it makes about the centre of mass)
// to the body, plus the engine's own torques.
int JetskiEngineModel::SetForceTick(float)
{
    SoultreeObject *object = m_body->m_object;
    _D3DVECTOR thrust = m_thrust;
    if (object) {
        _D3DVECTOR worldThrust = object->RotateVectorLocalToWorld(&thrust);
        m_body->m_force += worldThrust;
        _D3DVECTOR torque;
        CrossProduct(torque, m_thrustPoint - m_body->m_centerOfMass, thrust);
        m_body->m_torque += torque;
    }
    m_body->m_torque += m_torque2 + m_torque1;
    return 1;
}


// ---- src/sinit/JetskiEngineModel.cpp
// Static initializer of JetskiEngineModel.cpp: only the axis vectors from the shared header.

