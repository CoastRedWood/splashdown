// JetskiCollisionModel.cpp  @ 0x003DA870 (text 0x3DA870-0x3E0000)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's JetskiCollisionModel.cpp as one translation unit: its functions in
// retail order, then its static initializer. `make link` places the object's data
// as well as its code (tools/link_c.py).



#include "JetskiCollisionModel/JetskiCollisionModel.h"
#include "PhysicsBody/PhysicsBody.h"
#include "SoultreeObject/SoultreeObject.h"
#include "CollisionObject/CollisionObject.h"
#include "sinit/AxisVectors.h"

// ---- src/JetskiCollisionModel/ctor.cpp

JetskiCollisionModel::JetskiCollisionModel(int enabled) : SpringCollisionModel(enabled)
{
    m_x130 = 1.0f;
}


// ---- src/JetskiCollisionModel/dtor.cpp

JetskiCollisionModel::~JetskiCollisionModel()
{
}


// ---- src/JetskiCollisionModel/SimulationTick.cpp

extern const unsigned int WORLDOBJECT_RAMP;
extern const unsigned int COLLISION_BIKE;

// out = m * in (point: w = 1), libvu0 sceVu0ApplyMatrix plus the translation row.
static inline void TransformPoint(_D3DVECTOR *out, _D3DMATRIX *m, _D3DVECTOR *in)
{
    asm("lqc2 vf4, 0x0(%1); lqc2 vf5, 0x10(%1); lqc2 vf6, 0x20(%1); lqc2 vf7, 0x30(%1); lqc2 vf8, 0x0(%2); vmulw.w vf9, vf0, vf0w; vmulax.xyz ACC, vf4, vf8x; vmadday.xyz ACC, vf5, vf8y; vmaddz.xyz vf9, vf6, vf8z; vadd.xyz vf9, vf9, vf7; sqc2 vf9, 0x0(%0)"
        : : "r"(out), "r"(m), "r"(in) : "memory");
}

// xyz dot product on VU0.
static inline float DotProduct(_D3DVECTOR *a, _D3DVECTOR *b)
{
    float r;
    asm("lqc2 vf4, 0x0(%1); lqc2 vf5, 0x0(%2); vmul.xyz vf5, vf4, vf5; vaddy.x vf5, vf5, vf5y; vaddz.x vf5, vf5, vf5z; qmfc2.ni $2, vf5; mtc1 $2, %0"
        : "=f"(r) : "r"(a), "r"(b) : "$2");
    return r;
}

int JetskiCollisionModel::SimulationTick(float dt)
{
    _D3DVECTOR world, offset, velocity;
    int i;
    if (m_groundCollision || m_numContacts > 0) {
        if (m_useCenter)
            TransformPoint(&m_x110, &m_body->m_object->m_worldMatrix, &m_center);
        else
        {
            PhysicsBody *body = m_body;
            TransformPoint(&m_x110, &body->m_object->m_worldMatrix, &body->m_centerOfMass);
        }
    }
    static int GroundCollisionID = -1;  // unused profiler ID
    if (m_groundCollision) {
        m_x54 = 1;
        m_xA8 = m_x130;
        m_xAC = 1.0f;
        SpringCollisionPoint *spring = m_springs;
        for (i = 0; i < m_numSprings; i++) {
            TransformPoint(&world, &spring->m_object->m_worldMatrix, &spring->m_point);
            offset = spring->m_worldPosition - world;
            velocity = spring->m_worldPosition;
            float depth = spring->m_stiffness + DotProduct(&offset, &spring->m_groundNormal);
            if (depth > 0.0f) {
                m_x3C = 1;
                m_x6C = 1;
                spring->m_colliding = 1;
                if (m_x54)
                    CalculateImpulseResponse(depth, dt, spring->m_groundNormal, world, 0);
                if (m_x60)
                    RemoveNormalVelocity(spring->m_groundNormal, velocity);
                if (m_x4C) {
                    if (m_x58 && !(depth < m_friction) && !m_x54)
                        RemoveNormalVelocity(spring->m_groundNormal, velocity);
                    CalculateCollisionResponse(spring->m_id, dt, m_frictions[spring->m_surfaceType], depth,
                                               spring->m_groundNormal, world, 0);
                }
            } else {
                spring->m_colliding = 0;
            }
            spring++;
        }
    }
    static int ObjectCollisionID = -1;  // unused profiler ID
    ObjectCollisionPoint *contact = m_contacts;
    int type = 1;
    for (i = 0; i < m_numContacts; i++) {
        if (contact->m_collision->m_ownerType & WORLDOBJECT_RAMP)
            m_x54 = 0;
        else
            m_x54 = 1;
        if (contact->m_collision->m_ownerType & COLLISION_BIKE) {
            m_xA8 = 0.0f;
            m_xAC = 0.0f;
        } else {
            m_xA8 = m_x130;
            m_xAC = 1.0f;
        }
        if (contact->m_object)
            TransformPoint(&world, &contact->m_object->m_worldMatrix, &contact->m_normal);
        else
            TransformPoint(&world, &m_body->m_object->m_worldMatrix, &contact->m_normal);
        offset = contact->m_x40 - world;
        velocity = world + offset;
        float depth = DotProduct(&offset, &contact->m_x30);
        if (depth > 0.0f) {
            m_x3C = 1;
            m_x70 = 1;
            contact->m_colliding = 1;
            if (contact->m_body && m_x50) {
                CalculateCollisionResponse(type, m_frictions[contact->m_x64], dt, depth, contact->m_x30, world,
                                           contact->m_body);
            } else {
                if (m_x54)
                    CalculateImpulseResponse(depth, dt, contact->m_x30, world, contact->m_body);
                if (m_x60)
                    RemoveNormalVelocity(contact->m_x30, velocity);
                if (m_x4C) {
                    if (m_x58 && !(depth < m_friction) && !m_x54)
                        RemoveNormalVelocity(contact->m_x30, velocity);
                    CalculateCollisionResponse(type, dt, m_frictions[contact->m_x64], depth, contact->m_x30, world,
                                               contact->m_body);
                }
            }
        } else {
            contact->m_colliding = 0;
        }
        contact++;
    }
    m_x38 = 0;
    return 1;
}


// ---- src/sinit/JetskiCollisionModel.cpp
// Static initializer of JetskiCollisionModel.cpp: only the axis vectors from the shared header.

