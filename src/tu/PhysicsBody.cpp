// PhysicsBody.cpp  @ 0x00264DD0 (text 0x264DD0-0x265AF0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's PhysicsBody.cpp as one translation unit: its functions in
// retail order, then its static initializer. `make link` places the
// object's data as well as its code (tools/link_c.py).



#include "d3dtypes.h"
#include "Quaternion/Quaternion.h"
#include "GameObject/GameObject.h"
#include "PhysicsBody/PhysicsBody.h"
#include "sinit/AxisVectors.h"

// ---- src/PhysicsBody/ctor.cpp


_D3DMATRIX ZeroMatrix();
extern Quaternion ZeroQuaternion;


void AdamsBashforthIntegration(float dt, int n, float *state, float **derivatives);


PhysicsBody::PhysicsBody(int enabled) : GameObject(enabled)
{
    m_enabled = 1;
    m_attachedEnabled = 0;
    m_x30 = 0;
    m_initialVelocity = ZeroVector;
    m_initialBodyAngularRate = ZeroVector;
    m_x60 = ZeroVector;
    m_velocity = ZeroVector;
    m_lastVelocity = ZeroVector;
    m_xA0 = ZeroVector;
    m_xB0 = ZeroVector;
    m_xC0 = ZeroVector;
    m_xD0 = ZeroVector;
    m_x1C0 = ZeroVector;
    m_x1D0 = ZeroVector;
    m_orientation = ZeroQuaternion;
    m_x100 = ZeroVector;
    m_x274 = 933156.0f;                 // 966^2
    m_x278 = 250000.0f;                 // 500^2
    m_x27C = 18225.0f;                  // 135^2
    m_x280 = 625.0f;                    // 25^2
    m_mass = 0.0f;
    m_inverseMass = 0.0f;
    m_speed = 0;
    m_electricCharge = 0;
    m_inertia = ZeroMatrix();
    m_inverseInertia = ZeroMatrix();
    m_magneticDipole = ZeroVector;
    m_integrator = AdamsBashforthIntegration;
    m_centerOfMass = ZeroVector;
    m_maxAttachedBodies = 0;
    m_numAttachedBodies = 0;
    m_attachedBodies = 0;
    m_object = 0;
    m_numBodyCollisions = 0;
    m_numOtherBodyCollisions = 0;
}


// ---- src/PhysicsBody/dtor.cpp


PhysicsBody::~PhysicsBody()
{
    if (m_attachedBodies)
        delete[] m_attachedBodies;
}


// ---- src/PhysicsBody/SetSoultreeObject.cpp

void PhysicsBody::SetSoultreeObject(SoultreeObject *object)
{
    if (object)
        m_object = object;
}


// ---- src/PhysicsBody/SetMass.cpp

void PhysicsBody::SetMass(float mass)
{
    if (mass > 0.0f) {
        m_mass = mass;
        m_inverseMass = 1.0f / m_mass;
    }
}


// ---- src/PhysicsBody/SetInertia.cpp

_D3DMATRIX MatrixInverse(_D3DMATRIX m);


void PhysicsBody::SetInertia(_D3DMATRIX &inertia)
{
    m_inertia = inertia;
    m_inverseInertia = MatrixInverse(m_inertia);
}


// ---- src/PhysicsBody/CreateAttachedBodyList_00265890.cpp

void PhysicsBody::CreateAttachedBodyList(int capacity)
{
    m_attachedBodies = new PhysicsBody *[capacity];
    m_maxAttachedBodies = capacity;
    m_numAttachedBodies = 0;
}


// ---- src/PhysicsBody/AddAttachedBody.cpp

void PhysicsBody::AddAttachedBody(PhysicsBody *body)
{
    if (m_numAttachedBodies < m_maxAttachedBodies) {
        m_attachedBodies[m_numAttachedBodies] = body;
        m_numAttachedBodies++;
    }
}


// ---- src/PhysicsBody/SetEnabled.cpp

void PhysicsBody::SetEnabled(int enabled)
{
    m_enabled = enabled;
    if (enabled && !m_attachedEnabled) {
        for (int i = 0; i < m_numAttachedBodies; i++)
            m_attachedBodies[i]->m_enabled = 1;
        m_attachedEnabled = 1;
    }
}


// ---- src/PhysicsBody/ResetBodyCollisions.cpp (with ResetOtherBodyCollisions
// below, in retail order)

void PhysicsBody::ResetBodyCollisions()
{
    m_numBodyCollisions = 0;
}


// ---- src/PhysicsBody/AddBodyCollision.cpp

void PhysicsBody::AddBodyCollision(void *body)
{
    if (m_numBodyCollisions < 10) {
        int i = 0;
        int found = 0;
        while (i < m_numBodyCollisions && !found) {
            if (m_bodyCollisions[i] == body)
                found = 1;
            else
                i++;
        }
        if (!found) {
            m_bodyCollisions[m_numBodyCollisions] = body;
            m_numBodyCollisions++;
        }
    }
}


void PhysicsBody::ResetOtherBodyCollisions()
{
    m_numOtherBodyCollisions = 0;
}


// ---- src/PhysicsBody/AddOtherBodyCollision.cpp

void PhysicsBody::AddOtherBodyCollision(void *body)
{
    if (m_numOtherBodyCollisions < 10) {
        int i = 0;
        int found = 0;
        while (i < m_numOtherBodyCollisions && !found) {
            if (m_otherBodyCollisions[i] == body)
                found = 1;
            else
                i++;
        }
        if (!found) {
            m_otherBodyCollisions[m_numOtherBodyCollisions] = body;
            m_numOtherBodyCollisions++;
        }
    }
}


// ---- src/PhysicsBody/OtherBodyAlreadyCollided.cpp
int PhysicsBody::OtherBodyAlreadyCollided(void *other)
{
    int found = 0;
    for (int i = 0; i < m_numOtherBodyCollisions; i++) {
        if (m_otherBodyCollisions[i] == other)
            found = 1;
    }
    return found;
}


// ---- src/PhysicsBody/Render3D.cpp (its two PhysicsRigidBody setters are
// in src/PhysicsRigidBody/DegreesOfFreedom.cpp)

int PhysicsBody::Render3D()
{
    GameObject::Render3D();
    return 1;
}


// ---- src/sinit/PhysicsBody.cpp
// Static initializer of PhysicsBody.cpp: only the axis vectors from the shared header.

