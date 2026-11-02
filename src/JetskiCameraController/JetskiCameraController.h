// JetskiCameraController: the per-player CameraController that owns the six
// camera behaviours and switches between them. 0x88 bytes (`new` in
// JetskiCompetition); its own data starts after CameraController's 0x40.
// +0x60 is the JetskiVehicle SetJetski stores (StartRotate reads its
// +0x530); some copies called it a JetskiCompetition.
#ifndef JETSKICAMERACONTROLLER_JETSKICAMERACONTROLLER_H
#define JETSKICAMERACONTROLLER_JETSKICAMERACONTROLLER_H

#include "CameraController/CameraController.h"
#include "ViewPortCode/ViewPortCode.h"

class CinematographerCam;
class FirstPersonCamera;
class JetskiFollowCamera;
class JetskiThrillCamera;
class JetskiTVCamera;
class JetskiVehicle;
class JetskiWreckCamera;
class PSXWater;
class Scene;
class Terrain;

enum CameraType {};
enum FollowCameraPresetType {};

class JetskiCameraController : public CameraController {
public:
    JetskiCameraController(int enabled);
    JetskiCameraController *Create(Camera *camera, ViewPortCode viewport, Scene *scene, int limitHeight);
    int Initialize(Terrain *terrain, PSXWater *water, int player, int viewport);
    void SetJetski(JetskiVehicle *jetski, int player);
    int SwitchCamera(CameraType type);
    int SwitchFollowCameraPresets(FollowCameraPresetType preset);
    void StartRotate();
    void StopRotate();
    void ToggleFreezeRotate();

    // --- virtuals in retail slot order ---
    virtual ~JetskiCameraController();
    virtual int Tick(float dt);
    virtual int KeyUpHandler(ProcessNode *node, KeyInfo *key);
    virtual int KeyDownHandler(ProcessNode *node, KeyInfo *key);

    int m_x40;                              // +0x40
    int m_switchRequested;                  // +0x44
    JetskiFollowCamera *m_followCamera;     // +0x48
    JetskiThrillCamera *m_thrillCamera;     // +0x4C
    JetskiTVCamera *m_tvCamera;             // +0x50
    JetskiWreckCamera *m_wreckCamera;       // +0x54
    FirstPersonCamera *m_firstPersonCamera; // +0x58
    CinematographerCam *m_cinematographer;  // +0x5C
    JetskiVehicle *m_jetski;                // +0x60 (SetJetski)
    int m_x64;                              // +0x64, SetJetski's player
    int m_x68;                              // +0x68, -1 from the ctor and SetJetski
    int m_player;                           // +0x6C (Initialize; -1 until then)
    int m_viewport;                         // +0x70 (Initialize)
    int m_pickCamera;                       // +0x74
    int m_x78;                              // +0x78
    int m_cameraType;                       // +0x7C, the current CameraType
    int m_x80;                              // +0x80
    int m_presetID;                         // +0x84
};

#endif
