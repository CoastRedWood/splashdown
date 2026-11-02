// SelectiveGravityModel.cpp  @ 0x00291020 (text 0x291020-0x291270)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp)
//
// Retail's SelectiveGravityModel.cpp as one translation unit. Its .rodata
// (@824..@828, ending with SelectiveGravityModel's RTTI) is followed by
// SkyCube.cpp's (labels restart at @479), a TU without an initializer;
// soultree.cpp follows that. __sinit_SelectiveGravityModel.cpp builds only the
// axis vectors. `make link` places its data as well as its code.


#include "d3dtypes.h"
#include "GameObject/GameObject.h"
#include "PhysicsBody/PhysicsBody.h"
#include "SelectiveGravityModel/SelectiveGravityModel.h"
#include "sinit/AxisVectors.h"

// ---- src/SelectiveGravityModel/ctor.cpp

SelectiveGravityModel::SelectiveGravityModel(int id) : GameObject(id)
{
    m_maxBodies = 0;
    m_numBodies = 0;
    m_gravity = 32.2f;
    m_force = ZeroVector;
    m_bodies = 0;
}

// ---- src/SelectiveGravityModel/dtor.cpp

SelectiveGravityModel::~SelectiveGravityModel()
{
    if (m_bodies)
        delete[] m_bodies;
}

// ---- src/SelectiveGravityModel/Create.cpp

// Retail returns nothing (v0 is left as new[]'s result).
void SelectiveGravityModel::Create(int maxBodies)
{
    m_numBodies = 0;
    m_maxBodies = maxBodies;
    m_bodies = new PhysicsBody *[m_maxBodies];
}

// ---- src/SelectiveGravityModel/AddBody.cpp

void SelectiveGravityModel::AddBody(PhysicsBody *body)
{
    if (m_numBodies < m_maxBodies) {
        m_bodies[m_numBodies] = body;
        m_numBodies++;
    }
}

// ---- src/SelectiveGravityModel/SetForceTick.cpp

// The force as SetForceTick copies it: a plain four-float struct (the
// shared _D3DVECTOR's copy is folded away, 11 words).
struct ForceVector {
    float x, y, z, w;
};

int SelectiveGravityModel::SetForceTick(float dt)
{
    PhysicsBody **bodies = m_bodies;
    for (int i = 0; i < m_numBodies; i++) {
        float m = -bodies[i]->m_mass;
        m_force.y = m * m_gravity;
        PhysicsBody *body = bodies[i];
        ForceVector force = *(ForceVector *)&m_force;
        body->m_force.x += force.x;
        body->m_force.y += force.y;
        body->m_force.z += force.z;
    }
    return 1;
}

// ---- src/SelectiveGravityModel/SetGravity_00291260.cpp

void SelectiveGravityModel::SetGravity(float gravity)
{
    m_gravity = gravity;
}
