// JetskiGroundModel.cpp  @ 0x00337E20 (text 0x337E20-0x337FA0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's JetskiGroundModel.cpp as one translation unit: its functions in
// retail order, then its static initializer. `make link` places the object's
// data as well as its code (tools/link_c.py).



#include "JetskiGroundModel/JetskiGroundModel.h"
#include "d3dtypes.h"
#include "PhysicsBody/PhysicsBody.h"
#include "SoultreeObject/SoultreeObject.h"
#include "sinit/AxisVectors.h"

// ---- src/JetskiGroundModel/ctor.cpp

JetskiGroundModel::JetskiGroundModel(int enabled) : GameObject(enabled)
{
    m_onGround = 0;
    m_force = -100.0f;
}


// ---- src/JetskiGroundModel/dtor.cpp

JetskiGroundModel::~JetskiGroundModel()
{
}


// ---- src/JetskiGroundModel/Create.cpp

GameObject *JetskiGroundModel::Create(PhysicsRigidBody *body)
{
    m_body = body;
    return GameObject::Create();
}


// ---- src/JetskiGroundModel/SimulationTick.cpp

// Inline in retail, and by value: it copies the force into a second stack
// slot (lq/sq) before the null check.
inline void PhysicsBody::ApplyLocalForce(_D3DVECTOR force)
{
    if (m_object) {
        _D3DVECTOR world = m_object->RotateVectorLocalToWorld(&force);
        m_force += world;
    }
}

// While grounded, push the body along its local x axis.
int JetskiGroundModel::SimulationTick(float dt)
{
    if (m_onGround) {
        _D3DVECTOR force = ZeroVector;
        PhysicsBody *body = (PhysicsBody *)m_body;
        force.x = m_force * body->m_lastVelocity.x;
        body->ApplyLocalForce(force);
    }
    return 1;
}


// ---- src/sinit/JetskiGroundModel.cpp
// Static initializer of JetskiGroundModel.cpp: only the axis vectors from the shared header.

