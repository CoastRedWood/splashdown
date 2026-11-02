// JetskiAirControlModel: a JetskiVehicle's mid-air control model (stunt
// inputs), a GameObject. Names come from the class's own methods.
#ifndef JETSKIAIRCONTROLMODEL_JETSKIAIRCONTROLMODEL_H
#define JETSKIAIRCONTROLMODEL_JETSKIAIRCONTROLMODEL_H

#include "GameObject/GameObject.h"
#include "d3dtypes.h"

class PhysicsBody;

class JetskiAirControlModel : public GameObject {
public:
    JetskiAirControlModel(int id);

    // --- virtuals in retail slot order ---
    virtual ~JetskiAirControlModel();
    virtual int SimulationTick(float dt);
    virtual void SetBody(PhysicsBody *body);                // slot 27
    virtual void SetInputs(int a, int b, int c, float d, float e);    // slot 28

    PhysicsBody *m_body;                // +0x30 (SetBody)
    int m_x34;                          // +0x34
    int m_x38;                          // +0x38
    int m_x3C;                          // +0x3C, a stunt is under way
    int m_x40;                          // +0x40
    float m_x44;                        // +0x44
    float m_x48;                        // +0x48
    float m_x4C;                        // +0x4C
    float m_x50;                        // +0x50
    float m_x54;                        // +0x54
    float m_x58;                        // +0x58
    float m_x5C;                        // +0x5C
    float m_x60;                        // +0x60
    float m_x64;                        // +0x64
    char pad68[0x70 - 0x68];
    _D3DVECTOR m_x70;                   // +0x70
};

#endif
