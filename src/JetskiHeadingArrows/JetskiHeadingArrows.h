// JetskiHeadingArrows: the two 3D arrows pointing each player to the next
// gate, a GameObject. 0x60 bytes (new in JetskiCompetition::LoadOverlays).
// Create news two PSXRenderableObjects ("arrow.slt") into m_arrows; SetCompetition stores
// the competition and the two cameras and vehicles. The ctor clears +0x30,
// +0x3C..+0x50.
#ifndef JETSKIHEADINGARROWS_JETSKIHEADINGARROWS_H
#define JETSKIHEADINGARROWS_JETSKIHEADINGARROWS_H

#include "GameObject/GameObject.h"

class Camera;
class JetskiCompetition;
class JetskiVehicle;
class PSXCamera;
class PSXRenderableObject;

class JetskiHeadingArrows : public GameObject {
public:
    JetskiHeadingArrows(int enabled);
    void SetCompetition(JetskiCompetition *competition, PSXCamera **cameras, JetskiVehicle **vehicles);

    // --- overrides in retail slot order ---
    virtual ~JetskiHeadingArrows();
    virtual JetskiHeadingArrows *Create();
    virtual int Tick(float dt);
    virtual int PrepareGeometry();

    JetskiCompetition *m_competition;   // +0x30
    PSXRenderableObject *m_arrows[2];   // +0x34
    Camera *m_cameras[2];               // +0x3C, PSXCameras
    JetskiVehicle *m_vehicles[2];       // +0x44
    int m_show[2];                      // +0x4C, per arrow (Tick)
    float m_scale;                      // +0x54
    float m_x58;                        // +0x58
};

#endif
