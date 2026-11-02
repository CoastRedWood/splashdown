// Portal: a quad joining two SearchNodes (a GameObject). Data +0x30..+0xC3:
// four corners, the plane normal, four edge normals (SetNormals) and the
// half-diagonal (SetDiagonal); 0xD0 bytes (Scene::ReadSceneInfo's `new Portal[n]`).
#ifndef PORTAL_PORTAL_H
#define PORTAL_PORTAL_H

#include "GameObject/GameObject.h"
#include "d3dtypes.h"

class ScreenQuad2D;

class Portal : public GameObject {
public:
    Portal();
    Portal *Create(_D3DVECTOR *corners);
    int ProjectToScreen(ScreenQuad2D &quad);
    int RayIntersects(_D3DVECTOR *a, _D3DVECTOR *b, _D3DVECTOR *hit);
    int SphereIntersects(_D3DVECTOR *center, float radius, float *distance);
    void SetDiagonal();
    void SetNormals();

    // --- virtuals in retail slot order ---
    virtual ~Portal();
    virtual int Render3D();

    static ScreenQuad2D WholeScreen;

    _D3DVECTOR m_corners[4];            // +0x30
    _D3DVECTOR m_normal;                // +0x70
    _D3DVECTOR m_edgeNormals[4];        // +0x80
    float m_diagonal;                   // +0xC0 (SetDiagonal)
    char padC4[0xD0 - 0xC4];            // `new Portal[n]` (Scene::ReadSceneInfo) uses 0xD0
};

#endif
