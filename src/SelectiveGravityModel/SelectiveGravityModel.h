// SelectiveGravityModel: applies gravity to a chosen set of PhysicsBodies
// (a GameObject). 0x60 bytes (`new` in JetskiVehicle::Create); data
// +0x30..+0x53 (the ctor zeroes +0x50; +0x54..+0x5F are never named).
#ifndef SELECTIVEGRAVITYMODEL_SELECTIVEGRAVITYMODEL_H
#define SELECTIVEGRAVITYMODEL_SELECTIVEGRAVITYMODEL_H

#include "GameObject/GameObject.h"
#include "d3dtypes.h"

class PhysicsBody;

class SelectiveGravityModel : public GameObject {
public:
    SelectiveGravityModel(int enabled);
    int SetForceTick(float dt);

    // --- virtuals in retail slot order ---
    virtual ~SelectiveGravityModel();
    virtual void Create(int maxBodies);  // returns nothing in retail
    virtual void AddBody(PhysicsBody *body);
    virtual void SetGravity(float gravity);

    int m_numBodies;                    // +0x30
    int m_maxBodies;                    // +0x34
    float m_gravity;                    // +0x38
    int m_unk3C;                        // +0x3C
    _D3DVECTOR m_force;                 // +0x40
    PhysicsBody **m_bodies;             // +0x50
    char pad54[0x60 - 0x54];
};

#endif
