// HydrodynamicsModel: the water forces on a PhysicsBody from its hull, wake
// and froth points (a GameObject). 0x170 bytes (`new` in JetskiVehicle::Create).
// Most of +0x3C..+0xCC are parameters JetskiVehicle::LoadPhysicsParameters
// fills (named by offset here). The point counts are 64-bit longs.
#ifndef HYDRODYNAMICSMODEL_HYDRODYNAMICSMODEL_H
#define HYDRODYNAMICSMODEL_HYDRODYNAMICSMODEL_H

#include "GameObject/GameObject.h"
#include "d3dtypes.h"

class PhysicsBody;
class PSXWater;

// A wake or froth point: its strength and body-local position. 0x20 bytes
// (Create allocates the arrays as raw 32-byte records).
struct HydroPoint {
    float m_strength;                   // +0x00
    char pad04[0x10 - 0x04];
    _D3DVECTOR m_position;              // +0x10
};

class HydrodynamicsModel : public GameObject {
public:
    HydrodynamicsModel(int enabled);
    void AddFrothPoint(_D3DVECTOR point, float strength);
    void AddHullPoint(_D3DVECTOR point);
    void AddWakePoint(_D3DVECTOR point, float strength);
    int ComputeForceTick(float dt);
    void ResetFrothPoints();
    void ResetHullPoints();
    void ResetWakePoints();
    int SetForceTick(float dt);
    int UpdateWater(float dt);

    // --- virtuals in retail slot order ---
    virtual ~HydrodynamicsModel();
    virtual int Tick(float dt);
    virtual int Render3D();
    virtual HydrodynamicsModel *Create(PSXWater *water, PhysicsBody *body, long maxHull, long maxWake, long maxFroth);

    int m_x30;                          // +0x30 (Tick tests it as an int; the ctor copy declares a float)
    PSXWater *m_water;                  // +0x34
    float m_x38;                        // +0x38
    float m_x3C;                        // +0x3C
    float m_x40;                        // +0x40
    float m_x44;                        // +0x44
    float m_x48;                        // +0x48
    float m_x4C;                        // +0x4C
    float m_x50;                        // +0x50
    float m_x54;                        // +0x54
    float m_x58;                        // +0x58
    float m_x5C;                        // +0x5C
    float m_x60;                        // +0x60
    float m_x64;                        // +0x64
    float m_x68;                        // +0x68
    float m_x6C;                        // +0x6C
    float m_x70;                        // +0x70
    float m_x74;                        // +0x74
    float m_x78;                        // +0x78
    float m_x7C;                        // +0x7C
    float m_x80;                        // +0x80
    float m_x84;                        // +0x84
    float m_x88;                        // +0x88
    char pad8C[0x90 - 0x8C];
    _D3DVECTOR m_x90;                   // +0x90
    float m_xA0;                        // +0xA0
    float m_xA4;                        // +0xA4
    float m_xA8;                        // +0xA8
    float m_xAC;                        // +0xAC
    float m_xB0;                        // +0xB0
    float m_xB4;                        // +0xB4
    float m_xB8;                        // +0xB8
    float m_xBC;                        // +0xBC
    float m_xC0;                        // +0xC0
    float m_xC4;                        // +0xC4
    float m_xC8;                        // +0xC8
    float m_xCC;                        // +0xCC
    float m_aligningMoment;             // +0xD0, "AligningMomentCoefficient" (JetskiVehicle::LoadPhysicsParameters)
    float m_wakeStrength;               // +0xD4 (UpdateWater)
    int m_xD8;                          // +0xD8
    int m_inWater;                      // +0xDC, a hull point is under water (UpdateWater)
    int m_outOfWater;                   // +0xE0, cleared when one isn't (UpdateWater)
    char padE4[0xF0 - 0xE4];
    _D3DVECTOR m_xF0;                   // +0xF0
    _D3DVECTOR m_force;                 // +0x100
    _D3DVECTOR m_torque;                // +0x110
    float m_x120;                       // +0x120
    PhysicsBody *m_body;                // +0x124
    _D3DVECTOR *m_hullPoints;           // +0x128
    _D3DVECTOR *m_lastHullPoint;        // +0x12C
    HydroPoint *m_wakePoints;           // +0x130
    HydroPoint *m_lastWakePoint;        // +0x134
    HydroPoint *m_frothPoints;          // +0x138
    HydroPoint *m_lastFrothPoint;       // +0x13C
    long m_numHullPoints;               // +0x140, 64-bit
    long m_maxHullPoints;               // +0x148
    long m_numWakePoints;               // +0x150
    long m_maxWakePoints;               // +0x158
    long m_numFrothPoints;              // +0x160
    long m_maxFrothPoints;              // +0x168
};

#endif
