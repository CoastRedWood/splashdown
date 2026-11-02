// JetskiEngineModel: the jet thrust and steering torques on a jetski's
// PhysicsRigidBody (a GameObject). 0x120 bytes (`new` in JetskiVehicle::Create);
// Create stores the body (+0x114) and water (+0x118).
#ifndef JETSKIENGINEMODEL_JETSKIENGINEMODEL_H
#define JETSKIENGINEMODEL_JETSKIENGINEMODEL_H

#include "GameObject/GameObject.h"
#include "d3dtypes.h"

class PhysicsRigidBody;
class PSXWater;

class JetskiEngineModel : public GameObject {
public:
    JetskiEngineModel(int enabled);
    int SetForceTick(float dt);
    int UpdateForceTick(float dt);

    // --- virtuals in retail slot order ---
    virtual ~JetskiEngineModel();
    virtual int Tick(float dt);
    virtual int SimulationTick(float dt);
    virtual GameObject *Create(PhysicsRigidBody *body, PSXWater *water);

    int m_x30;                          // +0x30
    int m_x34;                          // +0x34
    int m_reversed;                     // +0x38
    int m_x3C;                          // +0x3C
    float m_throttle;                   // +0x40
    float m_x44;                        // +0x44
    float m_x48;                        // +0x48
    float m_x4C;                        // +0x4C
    float m_x50;                        // +0x50
    float m_x54;                        // +0x54
    float m_dryTime;                    // +0x58
    char pad5C[0x60 - 0x5C];
    _D3DVECTOR m_x60;                   // +0x60
    _D3DVECTOR m_x70;                   // +0x70
    _D3DVECTOR m_x80;                   // +0x80
    _D3DVECTOR m_x90;                   // +0x90
    _D3DVECTOR m_thrust;                // +0xA0
    _D3DVECTOR m_thrustPoint;           // +0xB0
    _D3DVECTOR m_torque1;               // +0xC0
    _D3DVECTOR m_torque2;               // +0xD0
    float m_maxRPM;                     // +0xE0
    float m_xE4;                        // +0xE4
    float m_xE8;                        // +0xE8
    float m_xEC;                        // +0xEC
    float m_xF0;                        // +0xF0
    float m_xF4;                        // +0xF4
    float m_xF8;                        // +0xF8
    float m_xFC;                        // +0xFC
    float m_x100;                       // +0x100
    float m_x104;                       // +0x104
    float m_x108;                       // +0x108
    float m_x10C;                       // +0x10C
    float m_x110;                       // +0x110
    PhysicsRigidBody *m_body;           // +0x114, Create
    PSXWater *m_water;                  // +0x118, Create
    int m_inWater;                      // +0x11C
};

#endif
