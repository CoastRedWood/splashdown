// Fog: the scene's distance fog, sent to the GS as a FOGCOL/fog-range
// render item (0x68 bytes, JetskiCompetition::LoadFog and
// JetskiUIManager::ReadFogData new it).
#ifndef FOG_H
#define FOG_H

#include "GameObject/GameObject.h"

class DmaRenderItem;

class Fog : public GameObject {
public:
    Fog(int enabled);
    Fog *Create(unsigned int color, float visibility, float haziness, float maxFar, float minFar = 900.0f, float minNear = 200.0f);
    void Change(unsigned int color, float visibility, float haziness);
    int PSXRender3D();

    // --- virtuals in retail slot order ---
    virtual ~Fog();
    virtual int Tick(float dt);
    virtual int PrepareGeometry();
    virtual int Render3D();
    virtual int KeyDownHandler(ProcessNode *node, KeyInfo *key);

    unsigned int m_color;               // +0x30
    float m_near;                       // +0x34
    float m_far;                        // +0x38
    float m_visibility;                 // +0x3C
    float m_haziness;                   // +0x40
    int m_x44;                          // +0x44 (ctor 0)
    int m_x48;                          // +0x48 (ctor 1)
    int m_x4C;                          // +0x4C (ctor 0)
    float m_maxFar;                     // +0x50
    float m_minFar;                     // +0x54
    float m_minNear;                    // +0x58
    DmaRenderItem *m_renderItem;        // +0x5C
    int m_changed;                      // +0x60 (ctor 1)
};

#endif
