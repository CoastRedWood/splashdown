// JetskiRollControlModel: a GameObject that applies a roll-correcting torque
// to the jetski's body (SetForceTick). 0x70 bytes. Create(PhysicsBody *)
// stores the body at +0x30.
#ifndef JETSKIROLLCONTROLMODEL_JETSKIROLLCONTROLMODEL_H
#define JETSKIROLLCONTROLMODEL_JETSKIROLLCONTROLMODEL_H

#include "GameObject/GameObject.h"
#include "d3dtypes.h"

class PhysicsBody;

class JetskiRollControlModel : public GameObject {
public:
    JetskiRollControlModel(int enabled);
    int SetForceTick(float dt);

    // --- virtuals in retail slot order ---
    virtual ~JetskiRollControlModel();
    virtual void Create(PhysicsBody *body);

    PhysicsBody *m_body;                // +0x30
    int m_enabled;                      // +0x34, 1 from the ctor
    int m_x38;                          // +0x38
    int m_x3C;                          // +0x3C, 1
    float m_x40;                        // +0x40, 1 / 32.2
    float m_x44;                        // +0x44, 1.0
    float m_x48;                        // +0x48, 1.0
    float m_x4C;                        // +0x4C, 1.0
    float m_x50;                        // +0x50
    float m_maxAngle;                   // +0x54, pi / 4
    float m_x58;                        // +0x58, 15.0
    char pad5C[0x60 - 0x5C];
    _D3DVECTOR m_up;                    // +0x60, YVector
};

#endif
