// PhysicsBody: a GameObject holding a rigid body's state (PhysicsRigidBody
// derives from it and supplies the pure slot 40). 0x298 bytes: data from
// +0x30 to the integrator pointer at +0x290, rounded up to 8.
// Names follow PhysicsBody's own methods (src/PhysicsBody/), otherwise the
// most common name among the files that declared it locally.
#ifndef PHYSICSBODY_PHYSICSBODY_H
#define PHYSICSBODY_PHYSICSBODY_H

#include "GameObject/GameObject.h"
#include "d3dtypes.h"
#include "Quaternion/Quaternion.h"

class SoultreeObject;

class PhysicsBody : public GameObject {
public:
    PhysicsBody(int enabled);
    void AddAttachedBody(PhysicsBody *body);
    void CreateAttachedBodyList(int capacity);
    void AddForce(_D3DVECTOR force) { m_force += force; }       // inline in retail
    void AddTorque(_D3DVECTOR torque) { m_torque += torque; }
    // Inline in retail; defined where SoultreeObject is complete
    // (src/tu/JetskiGroundModel.cpp).
    void ApplyLocalForce(_D3DVECTOR force);

    // --- virtuals in retail slot order ---
    virtual ~PhysicsBody();
    virtual int Render3D();
    virtual void SetSoultreeObject(SoultreeObject *object);
    virtual void SetMass(float mass);
    virtual void SetInertia(_D3DMATRIX &inertia);
    virtual void SetElectricCharge(float charge);
    virtual void SetMagneticDipole(_D3DVECTOR dipole);
    virtual void SetInitialVelocity(_D3DVECTOR &v);
    virtual void SetInitialBodyAngularRate(_D3DVECTOR &v);
    virtual void SetEnabled(int enabled);
    virtual void ResetBodyCollisions();
    virtual void AddBodyCollision(void *body);
    virtual void ResetOtherBodyCollisions();
    virtual void AddOtherBodyCollision(void *body);
    virtual int OtherBodyAlreadyCollided(void *other);
    virtual void ResetDerivatives() = 0;    // slot 40: PhysicsRigidBody's

    int m_x30;                          // +0x30
    float m_speed;                      // +0x34 (0 from the ctor)
    char pad38[0x40 - 0x38];
    _D3DVECTOR m_initialVelocity;       // +0x40
    _D3DVECTOR m_initialBodyAngularRate; // +0x50
    _D3DVECTOR m_x60;                   // +0x60
    _D3DVECTOR m_velocity;              // +0x70
    _D3DVECTOR m_lastVelocity;          // +0x80
    _D3DVECTOR m_x90;                   // +0x90
    _D3DVECTOR m_xA0;                   // +0xA0
    _D3DVECTOR m_xB0;                   // +0xB0
    _D3DVECTOR m_xC0;                   // +0xC0
    _D3DVECTOR m_xD0;                   // +0xD0
    _D3DVECTOR m_xE0;                   // +0xE0
    _D3DVECTOR m_xF0;                   // +0xF0
    _D3DVECTOR m_x100;                  // +0x100
    _D3DVECTOR m_magneticDipole;        // +0x110
    _D3DMATRIX m_inertia;               // +0x120
    _D3DMATRIX m_inverseInertia;        // +0x160
    _D3DVECTOR m_force;                 // +0x1A0
    _D3DVECTOR m_torque;                // +0x1B0
    _D3DVECTOR m_x1C0;                  // +0x1C0
    _D3DVECTOR m_x1D0;                  // +0x1D0
    _D3DVECTOR m_centerOfMass;          // +0x1E0
    Quaternion m_orientation;           // +0x1F0
    int m_x200;                         // +0x200 (PhysicsRigidBody's ctor clears it)
    int m_enabled;                      // +0x204 (SetEnabled)
    int m_attachedEnabled;              // +0x208
    int m_numBodyCollisions;            // +0x20C
    void *m_bodyCollisions[10];         // +0x210
    int m_numOtherBodyCollisions;       // +0x238
    void *m_otherBodyCollisions[10];    // +0x23C
    int m_maxAttachedBodies;            // +0x264
    int m_numAttachedBodies;            // +0x268
    float m_mass;                       // +0x26C
    float m_inverseMass;                // +0x270
    float m_x274;                       // +0x274, 966^2 from the ctor
    float m_x278;                       // +0x278, 500^2
    float m_x27C;                       // +0x27C, 135^2
    float m_x280;                       // +0x280, 25^2
    float m_electricCharge;             // +0x284
    SoultreeObject *m_object;           // +0x288 (SetSoultreeObject)
    PhysicsBody **m_attachedBodies;     // +0x28C
    void (*m_integrator)(float, int, float *, float **); // +0x290, AdamsBashforthIntegration
};

#endif
