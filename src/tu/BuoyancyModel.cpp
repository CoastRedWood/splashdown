// BuoyancyModel.cpp  @ 0x00337FA0 (text 0x337FA0-0x338CE0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's BuoyancyModel.cpp as one translation unit: BuoyancyModel's
// methods, then the static initializer (only the axis vectors). Its .rodata
// is the BuoyancyModel RTTI (@1039-@1043, its own BaseObject/GameObject
// copies); SimpleWreckCam.cpp restarts at @960. BuoyancyModel/Create.cpp also
// held SimpleWreckCamera::ConstrainCamera and two FirstPersonCamera members;
// those are in their TUs.


#include "GameObject/GameObject.h"
#include "BuoyancyModel/BuoyancyModel.h"
#include "PhysicsBody/PhysicsBody.h"
#include "d3dtypes.h"
#include "sinit/AxisVectors.h"
#include "SoultreeObject/SoultreeObject.h"
#include "Water/Water.h"
#include "PSXWater/PSXWater.h"

// A collision point (0x30 bytes).
struct BuoyancyCollisionPoint {
    _D3DVECTOR m_position;              // +0x00, local to m_object
    _D3DVECTOR m_world;                 // +0x10, world position, then the water surface there
    SoultreeObject *m_object;           // +0x20
    int m_x24;                          // +0x24
    char pad28[0x30 - 0x28];
};

// ---- src/BuoyancyModel/ctor.cpp
class PhysicsBody;
class PSXWater;




BuoyancyModel::BuoyancyModel(int id) : GameObject(id)
{
    m_body = 0;
    m_water = 0;
    m_x38 = 0;
    m_numHardPoints = 0;
    m_hardPoints = 0;
    m_lastHardPoint = 0;
    m_numSoftPoints = 0;
    m_softPoints = 0;
    m_lastSoftPoint = 0;
    m_x54 = 0;
    m_spring = 0;
    m_damping = 0;
    m_spring2 = 0;
    m_damping2 = 0;
    m_x68 = 0;
    m_x6C = 0;
}


// ---- src/BuoyancyModel/dtor.cpp
extern "C" void free(void *p);




BuoyancyModel::~BuoyancyModel()
{
    if (m_hardPoints)
        free(m_hardPoints);
    if (m_softPoints)
        free(m_softPoints);
}


// ---- src/BuoyancyModel/Create.cpp

BuoyancyModel *BuoyancyModel::Create(PhysicsBody *body, PSXWater *water)
{
    m_body = body;
    m_water = water;
    return this;
}


// ---- src/BuoyancyModel/SetCollisionParameters.cpp
static inline float Fabs(float x)
{
    float r;
    asm("abs.s $f8, %1; swc1 $f8, 0(%0)" : : "r"(&r), "f"(x) : "$f8", "memory");
    return r;
}

static inline float SqrtAsm(float x)
{
    float r;
    asm("sqrt.s $f8, %1; swc1 $f8, 0(%0)" : : "r"(&r), "f"(x) : "$f8", "memory");
    return r;
}

// The extra inline level matters: it gives the square roots' stack temps
// slots after the Fabs ones, as in retail.
static inline float Sqrt(float x)
{
    return SqrtAsm(x);
}



// Spring constants scale with mass and gravity (32.2 ft/s^2); the damping
// values are fractions of critical damping, 2 * sqrt(k * m).
void BuoyancyModel::SetCollisionParameters(float a, float stiffness, float damping, float stiffness2,
                                           float damping2, float f, float g)
{
    m_x54 = a;
    m_x68 = g;
    m_x6C = f;
    m_spring = Fabs(32.2f * (stiffness * m_body->m_mass));
    if (m_body->m_mass > 0.0f && m_spring > 0.0f)
        m_damping = 2.0f * damping * Sqrt(m_body->m_mass * m_spring);
    else
        m_damping = 0.0f;
    m_spring2 = Fabs(32.2f * (stiffness2 * m_body->m_mass));
    if (m_body->m_mass > 0.0f && m_spring2 > 0.0f)
        m_damping2 = 2.0f * damping2 * Sqrt(m_body->m_mass * m_spring2);
    else
        m_damping2 = 0.0f;
}


// ---- src/BuoyancyModel/AddSoftCollisionPoint.cpp
extern "C" void *realloc(void *p, unsigned int size);



class SoultreeObject;



// Adds a collision point on object unless it is already registered.
void BuoyancyModel::AddSoftCollisionPoint(_D3DVECTOR point, SoultreeObject *object)
{
    int found = 0;
    for (int i = 0; i < m_numSoftPoints; i++) {
        if (point == m_softPoints[i].m_position && object == m_softPoints[i].m_object)
            found = 1;
    }
    if (!found) {
        m_softPoints = (BuoyancyCollisionPoint *)realloc(m_softPoints,
                                                         (m_numSoftPoints + 1) * sizeof(BuoyancyCollisionPoint));
        m_softPoints[m_numSoftPoints].m_object = object;
        m_softPoints[m_numSoftPoints].m_position = point;
        m_softPoints[m_numSoftPoints].m_x24 = 0;
        m_softPoints[m_numSoftPoints].m_world = ZeroVector;
        m_lastSoftPoint = &m_softPoints[m_numSoftPoints];
        m_numSoftPoints++;
    }
}


// ---- src/BuoyancyModel/AddHardCollisionPoint.cpp
extern "C" void *realloc(void *p, unsigned int size);



class SoultreeObject;



// Adds a collision point on object unless it is already registered.
void BuoyancyModel::AddHardCollisionPoint(_D3DVECTOR point, SoultreeObject *object)
{
    int found = 0;
    for (int i = 0; i < m_numHardPoints; i++) {
        if (point == m_hardPoints[i].m_position && object == m_hardPoints[i].m_object)
            found = 1;
    }
    if (!found) {
        m_hardPoints = (BuoyancyCollisionPoint *)realloc(m_hardPoints,
                                                         (m_numHardPoints + 1) * sizeof(BuoyancyCollisionPoint));
        m_hardPoints[m_numHardPoints].m_object = object;
        m_hardPoints[m_numHardPoints].m_position = point;
        m_hardPoints[m_numHardPoints].m_x24 = 0;
        m_hardPoints[m_numHardPoints].m_world = ZeroVector;
        m_lastHardPoint = &m_hardPoints[m_numHardPoints];
        m_numHardPoints++;
    }
}


// ---- src/BuoyancyModel/SimulationTick.cpp
#pragma inline_depth(4)  // ApplyForceAtPoint -> CrossProduct






static inline float Fmin(float a, float b)
{
    float r;
    asm("min.s $f8, %1, %2; swc1 $f8, 0(%0)" : : "r"(&r), "f"(a), "f"(b) : "$f8", "memory");
    return r;
}

// VU0 cross product, out = a x b.
static inline void CrossProduct(_D3DVECTOR &out, const _D3DVECTOR &a, const _D3DVECTOR &b)
{
    asm("lqc2 vf4, 0x0(%1); lqc2 vf5, 0x0(%2); vopmula.xyz ACC, vf4, vf5; vopmsub.xyz vf6, vf5, vf4; vsub.w vf6, vf6, vf6; sqc2 vf6, 0x0(%0)"
        : : "r"(&out), "r"(&a), "r"(&b) : "memory");
}

// Pushes body with force applied at point (both world space).
static inline void ApplyForceAtPoint(PhysicsBody *body, _D3DVECTOR force, _D3DVECTOR point)
{
    if (body->m_object) {
        _D3DVECTOR localForce = body->m_object->RotateVectorWorldToLocal(&force);
        _D3DVECTOR localPoint = body->m_object->TransformVectorWorldToLocal(&point);
        localPoint = localPoint - body->m_centerOfMass;
        body->m_force += force;
        _D3DVECTOR torque;
        CrossProduct(torque, localPoint, localForce);
        body->m_torque += torque;
    }
}

// Spring forces holding the collision points (the fast set above m_x6C,
// else the slow set) up out of the water, with the matching wakes.
int BuoyancyModel::SimulationTick(float dt)
{
    _D3DVECTOR origin;
    _D3DVECTOR up;
    up = YVector;
    origin = ZeroVector;
    float spring;
    float damping;
    BuoyancyCollisionPoint *point;
    BuoyancyCollisionPoint *last;
    if (m_body->m_speed > m_x6C && (point = m_hardPoints) != 0) {
        last = m_lastHardPoint;
        spring = m_spring;
        damping = m_damping;
    } else if ((point = m_softPoints) != 0) {
        last = m_lastSoftPoint;
        spring = m_spring2;
        damping = m_damping2;
    } else {
        return 1;
    }
    _D3DVECTOR spin;
    _D3DVECTOR world;
    _D3DVECTOR velocity;
    _D3DVECTOR relative;
    _D3DVECTOR force;
    _D3DVECTOR drag;
    m_x38 = 0;
    do {
        world = point->m_object->TransformVectorLocalToWorld(&point->m_position);
        point->m_world = world;
        m_water->GetGridPointIntersection(&point->m_world, 0);
        float depth = point->m_world.y - world.y;  // one variable: retail keeps it in $f22
        depth = Fmin(depth, m_x54);
        if (depth > 0.0f) {
            m_x38 = 1;
            CrossProduct(spin, m_body->m_xA0, world - m_body->m_x60);
            velocity = m_body->m_velocity + spin;
            relative = velocity - origin;
            float verticalSpeed = relative.y;
            float wake = 0.02f * verticalSpeed;
            if (wake < -0.2f)
                wake = -0.2f;
            else if (wake > 0.2f)
                wake = 0.2f;
            m_water->ApplyExternalForce(wake, world);
            float push = spring * depth - damping * verticalSpeed;
            force = push * up;
            ApplyForceAtPoint(m_body, force, world);
        }
        point++;
    } while (point <= last);
    if (m_x38) {
        relative = m_body->m_velocity - origin;
        drag = m_x68 * relative;
        m_body->AddForce(drag);
    }
    return 1;
}

