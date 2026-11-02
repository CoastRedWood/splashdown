// PSXUIWaterEffect: the PS2 menu water (a UIWaterEffect). 0x150 bytes (`new`
// in JetskiUIManager::Create): its QWORD light blocks make it 16-aligned, so
// its data starts at +0x70 after UIWaterEffect's 0x68. The three light blocks
// are what Render3D passes to PSXMesh::SetLightInfo(directions, colors, ambient).
#ifndef PSXUIWATEREFFECT_PSXUIWATEREFFECT_H
#define PSXUIWATEREFFECT_PSXUIWATEREFFECT_H

#include "UIWaterEffect/UIWaterEffect.h"
#include "QWORD/QWORD.h"

class LightManager;
class Mesh;

// The mesh vertices at one grid point (m_shares, one per grid point): the
// strip and vertex of each, at most three.
struct PSXUIWaterEffectMeshVertexInfo {
    PSXUIWaterEffectMeshVertexInfo();
    int m_count;                        // +0x00
    int m_strips[3];                    // +0x04
    int m_vertices[3];                  // +0x10
};

class PSXUIWaterEffect : public UIWaterEffect {
public:
    PSXUIWaterEffect(int enabled);
    PSXUIWaterEffect *Create(char *name, int size);
    void Initialize(char *name);
    void SetLightManager(LightManager *lightManager);
    void SetUpLights();

    // --- virtuals in retail slot order ---
    virtual ~PSXUIWaterEffect();
    virtual int Tick(float dt);
    virtual int PrepareGeometry();
    virtual int Render3D();
    virtual void ModifyMesh();
    virtual void UpdateNormals();

    QWORD m_lightDirections[4];         // +0x70 (SetUpLights' m_lightDirection)
    QWORD m_lightColors[4];             // +0xB0
    QWORD m_lightAmbient[4];            // +0xF0
    unsigned char m_meshFlags;          // +0x130
    int m_numStrips;                    // +0x134
    int *m_stripLengths;                // +0x138
    PSXUIWaterEffectMeshVertexInfo *m_shares;   // +0x13C
    Mesh *m_mesh;                       // +0x140
    int m_material;                     // +0x144 (material string-table index)
    LightManager *m_lightManager;       // +0x148
};

#endif
