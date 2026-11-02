// JetskiRadar: the in-race track map with a blip per vehicle. 0x110 bytes
// (new in JetskiCompetition::LoadOverlays). Layout from the ctor (it clears
// +0x30, then +0x34/+0x48/+0x5C/+0x7C/+0x90 in an i < 5 loop, the blinks to
// 1), Create (+0x74 the map icon, +0x78/+0x70 the managers for split screen,
// +0xA8..+0xB4 the map rectangle, +0xCC the viewport count, +0xD0/+0xD4 two
// DmaRenderItems) and SetCompetition (+0x30; with one viewport it borrows
// the competition's managers at +0x34 and +0x230). Some copies view the
// arrays as starting at +0x30 and index them from 1.
#ifndef JETSKIRADAR_JETSKIRADAR_H
#define JETSKIRADAR_JETSKIRADAR_H

#include "GameObject/GameObject.h"
#include "d3dtypes.h"
#include "ViewPortCode/ViewPortCode.h"

class ScreenTextManager;

class DmaRenderItem;
class JetskiCompetition;
class JetskiVehicle;
class ScreenIcon;
class ScreenIconManager;
class ScreenPrimitive;
class ScreenPrimitiveManager;
class ScreenPrimitiveVertex;
class ScreenTextManager;

class JetskiRadar : public GameObject {
public:
    JetskiRadar(int enabled);
    JetskiRadar *Create(ScreenTextManager *texts, ViewPortCode viewport, JetskiCompetition *competition);
    int SetCompetition(JetskiCompetition *competition);
    void SetShowing(int showing);
    void SetViewport();
    void RestoreViewport();
    void RenderRadar();
    int CalculateTextureCoordinates(_D3DVECTOR position, float &u, float &v);

    // --- overrides in retail slot order ---
    virtual ~JetskiRadar();
    virtual int Tick(float dt);
    virtual int PrepareGeometry();
    virtual int Render3D();

    JetskiCompetition *m_competition;           // +0x30
    JetskiVehicle *m_vehicles[5];               // +0x34
    int m_blink[5];                             // +0x48, 1 from the ctor
    float m_blinkTimer[5];                      // +0x5C
    ScreenIconManager *m_iconManager;           // +0x70
    ScreenIcon *m_map;                          // +0x74
    ScreenPrimitiveManager *m_primitiveManager; // +0x78
    ScreenPrimitiveVertex *m_vertices[5];       // +0x7C
    ScreenPrimitive *m_blips[5];                // +0x90
    char padA4[0xA8 - 0xA4];
    int m_left;                                 // +0xA8
    int m_top;                                  // +0xAC
    int m_right;                                // +0xB0
    int m_bottom;                               // +0xB4
    int m_numVehicles;                          // +0xB8
    float m_scale;                              // +0xBC, 1 from the ctor
    float m_centerX;                            // +0xC0
    float m_centerZ;                            // +0xC4
    float m_vOffset;                            // +0xC8
    int m_numViewports;                         // +0xCC
    DmaRenderItem *m_radarViewportItem;         // +0xD0
    DmaRenderItem *m_restoreViewportItem;       // +0xD4
    _D3DVIEWPORT8 m_radarViewport;              // +0xD8
    _D3DVIEWPORT8 m_savedViewport;              // +0xF0
    int m_showing;                              // +0x108
};

#endif
