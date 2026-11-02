// UIWaterEffect: the menus' rippling water surface, a grid of vertices
// driven by a UIWaterPatch (the base of PSXUIWaterEffect, whose data starts
// at +0x130). Members from the ctor (clears +0x30..+0x60, -1 at +0x50 and
// +0x54) and InternalCreate (the patch, the per-vertex arrays, the grid
// size). Its own size isn't known (never new'd alone); the ctor's last
// store is +0x60.
#ifndef UIWATEREFFECT_H
#define UIWATEREFFECT_H

#include "GameObject/GameObject.h"
#include "d3dtypes.h"

class UIWaterPatch;

struct WaterUV {
    float u, v;
};

class UIWaterEffect : public GameObject {
public:
    UIWaterEffect(int enabled);
    void InternalCreate(int size);
    void ApplyExternalForce(float force, float x, float y);

    static float WIDTH;
    static float HEIGHT;
    static float OriginX;
    static float OriginZ;
    static int EnableUIWaterEffectLighting;

    // --- overrides and own virtuals in retail slot order ---
    virtual ~UIWaterEffect();
    virtual int Tick(float dt);
    virtual int Render3D();
    virtual UIWaterEffect *Create(int size);
    virtual void ModifyMesh();
    virtual void UpdateNormals();

    UIWaterPatch *m_patch;              // +0x30
    _D3DVECTOR *m_vertices;             // +0x34
    _D3DVECTOR *m_normals;              // +0x38
    WaterUV *m_uvs;                     // +0x3C
    int *m_heightChanged;               // +0x40, per vertex
    int *m_normalDirty;                 // +0x44, per vertex
    int m_width;                        // +0x48
    int m_height;                       // +0x4C
    int m_x50;                          // +0x50, -1 from the ctor
    int m_x54;                          // +0x54, -1 from the ctor
    int m_x58;                          // +0x58
    int m_x5C;                          // +0x5C
    int m_x60;                          // +0x60
};

#endif  // UIWATEREFFECT_H
