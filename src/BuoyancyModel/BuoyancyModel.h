// BuoyancyModel: spring forces keeping a PhysicsBody's collision points out
// of the water (a GameObject). 0x70 bytes; the ctor zeroes +0x30..+0x6C.
// Names follow its own methods (AddHard/SoftCollisionPoint,
// SetCollisionParameters).
#ifndef BUOYANCYMODEL_BUOYANCYMODEL_H
#define BUOYANCYMODEL_BUOYANCYMODEL_H

#include "GameObject/GameObject.h"
#include "d3dtypes.h"

class PhysicsBody;
class PSXWater;
class SoultreeObject;
struct BuoyancyCollisionPoint;

class BuoyancyModel : public GameObject {
public:
    BuoyancyModel(int enabled);
    BuoyancyModel *Create(PhysicsBody *body, PSXWater *water);
    void AddHardCollisionPoint(_D3DVECTOR point, SoultreeObject *object);
    void AddSoftCollisionPoint(_D3DVECTOR point, SoultreeObject *object);
    void SetCollisionParameters(float a, float stiffness, float damping, float stiffness2, float damping2,
                                float b, float c);

    // --- virtuals in retail slot order ---
    virtual ~BuoyancyModel();
    virtual int SimulationTick(float dt);

    PhysicsBody *m_body;                // +0x30
    PSXWater *m_water;                  // +0x34
    int m_x38;                          // +0x38
    int m_numHardPoints;                // +0x3C
    BuoyancyCollisionPoint *m_hardPoints;   // +0x40
    BuoyancyCollisionPoint *m_lastHardPoint; // +0x44
    int m_numSoftPoints;                // +0x48
    BuoyancyCollisionPoint *m_softPoints;   // +0x4C
    BuoyancyCollisionPoint *m_lastSoftPoint; // +0x50
    float m_x54;                        // +0x54
    float m_spring;                     // +0x58
    float m_damping;                    // +0x5C
    float m_spring2;                    // +0x60
    float m_damping2;                   // +0x64
    float m_x68;                        // +0x68
    float m_x6C;                        // +0x6C
};

#endif
