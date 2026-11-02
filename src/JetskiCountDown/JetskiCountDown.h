// JetskiCountDown: the 3-2-1 race start overlay, a GameObject. 0x60 bytes
// (new in JetskiCompetition::LoadOverlays). Create news a set of three
// digit models per player (PSXRenderableObjects); SetCompetition
// stores the competition and the players' cameras, which PrepareGeometry
// compares with the render target's camera.
#ifndef JETSKICOUNTDOWN_JETSKICOUNTDOWN_H
#define JETSKICOUNTDOWN_JETSKICOUNTDOWN_H

#include "GameObject/GameObject.h"
#include "ViewPortCode/ViewPortCode.h"

class LightManager;

class Camera;
class JetskiCompetition;
class LightManager;
class PSXCamera;
class PSXRenderableObject;

class JetskiCountDown : public GameObject {
public:
    JetskiCountDown(int enabled);
    JetskiCountDown *Create(LightManager *lights, ViewPortCode viewport);
    int SetCompetition(JetskiCompetition *competition, PSXCamera **cameras);
    int Hide();

    // --- overrides in retail slot order ---
    virtual ~JetskiCountDown();
    virtual int Tick(float dt);
    virtual int PrepareGeometry();

    JetskiCompetition *m_competition;   // +0x30
    PSXRenderableObject *m_sprites[2][3]; // +0x34, per player
    Camera *m_cameras[2];               // +0x4C, PSXCameras
    float m_scale;                      // +0x54
    float m_x58;                        // +0x58
    int m_viewport;                     // +0x5C, a ViewPortCode (nonzero: split screen)
};

#endif
