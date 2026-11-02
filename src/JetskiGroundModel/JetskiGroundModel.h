// JetskiGroundModel: a JetskiVehicle's on-ground push (JetskiVehicle +0x8C),
// a GameObject. 0x40 bytes (`new JetskiGroundModel` in JetskiVehicle::Create).
// Layout from the ctor, Create and SimulationTick.
#ifndef JETSKIGROUNDMODEL_JETSKIGROUNDMODEL_H
#define JETSKIGROUNDMODEL_JETSKIGROUNDMODEL_H

#include "GameObject/GameObject.h"

class PhysicsRigidBody;

class JetskiGroundModel : public GameObject {
public:
    JetskiGroundModel(int enabled);

    // --- virtuals in retail slot order ---
    virtual ~JetskiGroundModel();
    virtual int SimulationTick(float dt);
    virtual GameObject *Create(PhysicsRigidBody *body);     // slot 27

    int m_onGround;                     // +0x30
    float m_force;                      // +0x34, -100.0f from the ctor
    PhysicsRigidBody *m_body;           // +0x38 (Create)
    char pad3C[0x40 - 0x3C];
};

#endif
