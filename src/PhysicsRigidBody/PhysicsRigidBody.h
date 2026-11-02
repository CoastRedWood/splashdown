// PhysicsRigidBody: the rigid-body PhysicsBody (JetskiVehicle's +0x80, the
// buoys', the wreckers'). 0x330 bytes (`new` in JetskiVehicle::Create);
// its own data starts after PhysicsBody's 0x298. The two Set*DegreesOfFreedom
// methods write +0x2A0/+0x2AC (the ctor sets all six to 1). +0x2B8..+0x31F
// are ResetDerivatives' integrator state (offset names from its copy).
#ifndef PHYSICSRIGIDBODY_PHYSICSRIGIDBODY_H
#define PHYSICSRIGIDBODY_PHYSICSRIGIDBODY_H

#include "PhysicsBody/PhysicsBody.h"

struct Vector3 {
    float x, y, z;
};

class PhysicsRigidBody : public PhysicsBody {
public:
    PhysicsRigidBody(int enabled);
    void SetTranslationDegreesOfFreedom(int x, int y, int z);
    void SetOrientationDegreesOfFreedom(int x, int y, int z);

    // --- virtuals in retail slot order ---
    virtual ~PhysicsRigidBody();
    virtual int Tick(float dt);
    virtual int SimulationTick(float dt);
    virtual int Render3D();
    virtual void ResetDerivatives();
    virtual void Reset();

    char pad298[0x2A0 - 0x298];
    int m_translationDOF[3];            // +0x2A0
    int m_orientationDOF[3];            // +0x2AC
    Vector3 m_2B8[2];                   // +0x2B8
    Vector3 m_2D0[2];                   // +0x2D0
    Quaternion m_2E8[2];                // +0x2E8
    Vector3 m_308[2];                   // +0x308
    int m_x320;                         // +0x320
    char pad324[0x330 - 0x324];
};

#endif
