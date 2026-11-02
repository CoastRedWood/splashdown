// JetskiCameraController.cpp  @ 0x0031EC10 (text 0x31EC10-0x3214F0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's JetskiCameraController.cpp as one translation unit: its functions
// in retail order, then its static initializer. `make link` places the
// object's data as well as its code (tools/link_c.py). WaterObjectData,
// after ToggleFreezeRotate, opens WaterObject.cpp (its ASSERTs say so; the
// TU map). The old files' views map onto the shared headers:
// +0x4C/+0x54/+0x58/+0x5C are the thrill, wreck, first-person and
// cinematographer cameras, +0x60 the JetskiVehicle (whose +0x530 one copy
// called a competition's m_paused), +0x7C m_cameraType (copies: m_mode,
// m_rotateFrozen), +0x70 m_viewport; the game's +0x280 "data" is
// m_competition (m_status, m_infoText); the wreck flag is the rider's
// WaterWrecker::m_xB0.



// GetString (KeyDownHandler) is only inlined at a deeper inline depth than
// the default; inline_depth is TU-wide, so it is set once here.
#pragma inline_depth(5)

#include "soultree_assert_block.h"
#include "LocalizedString/LocalizedString.h"
#include "GameText/GameText.h"
#include "JetskiInfoText/JetskiInfoText.h"
#include "CameraVector/CameraVector.h"
#include "CameraStateParams/CameraStateParams.h"
#include "CameraBehavior/CameraBehavior.h"
#include "GameObject/GameObject.h"
#include "CameraController/CameraController.h"
#include "ControlInterface/ControlInterface.h"
#include "Game/Game.h"
#include "Camera/Camera.h"
#include "PhysicsBody/PhysicsBody.h"
#include "PhysicsRigidBody/PhysicsRigidBody.h"
#include "SimpleFollowCamera/SimpleFollowCamera.h"
#include "JetskiFollowCamera/JetskiFollowCamera.h"
#include "SimpleThrillCamera/SimpleThrillCamera.h"
#include "JetskiThrillCamera/JetskiThrillCamera.h"
#include "SimpleWreckCamera/SimpleWreckCamera.h"
#include "JetskiWreckCamera/JetskiWreckCamera.h"
#include "SimpleTVCamera/SimpleTVCamera.h"
#include "JetskiTVCamera/JetskiTVCamera.h"
#include "FirstPersonCamera/FirstPersonCamera.h"
#include "CinematographerCam/CinematographerCam.h"
#include "ViewPortCode/ViewPortCode.h"
#include "Scene/Scene.h"
#include "Water/Water.h"
#include "PSXWater/PSXWater.h"
#include "JetskiGame/JetskiGame.h"
#include "JetskiGameOptions/JetskiGameOptions.h"
#include "JetskiCompetition/JetskiCompetition.h"
#include "RaceStatusClass/RaceStatusClass.h"
#include "AnalogAxis/AnalogAxis.h"
#include "JetskiAIVehicle/AIVehicle.h"
#include "JetskiPlayerVehicle/JetskiPlayerVehicle.h"
#include "JetskiVehicle/JetskiVehicle.h"
#include "JetskiRider/JetskiRider.h"
#include "WaterWrecker/WaterWrecker.h"
#include "JetskiCameraController/JetskiCameraController.h"
// The axis vectors every Soultree TU defines (and __sinit constructs).
#include "sinit/AxisVectors.h"

extern JetskiGame *LpGame;
extern char SceneContentDirectory[];
int KEYDOWN_HANDLER_TEST(AssignableActionIndex action, DeviceType device, ProcessNode *node, unsigned int mods);

// One follow-camera preset (0x50 bytes), as JetskiFollowCamera::Initialize
// takes them.
struct FollowCameraPreset {
    int m_x00;                          // +0x00, preset ID
    float m_x04;                        // +0x04
    float m_x08;                        // +0x08
    float m_x0C;                        // +0x0C
    float m_x10;                        // +0x10
    float m_x14;                        // +0x14
    float m_x18;                        // +0x18
    float m_x1C;                        // +0x1C
    float m_x20;                        // +0x20
    int m_x24;                          // +0x24
    int m_x28;                          // +0x28
    int m_x2C;                          // +0x2C
    float m_x30;                        // +0x30
    int m_x34;                          // +0x34
    float m_x38;                        // +0x38
    float m_x3C;                        // +0x3C
    float m_x40;                        // +0x40
    float m_x44;                        // +0x44
    float m_x48;                        // +0x48
    float m_x4C;                        // +0x4C
};

// SimpleWreckCamera's Initialize parameters (8 floats, by value).
struct WreckCameraParams {
    float m_x00, m_x04, m_x08, m_x0C, m_x10, m_x14, m_x18, m_x1C;
};

JetskiCameraController::JetskiCameraController(int enabled) : CameraController(enabled)
{
    m_jetski = 0;
    m_x64 = 0;
    m_x68 = -1;
    m_player = -1;
    m_viewport = 0;
    m_x78 = 0;
    m_followCamera = 0;
    m_thrillCamera = 0;
    m_tvCamera = 0;
    m_cinematographer = 0;
    m_wreckCamera = 0;
    m_firstPersonCamera = 0;
    m_cameraType = 0;
    m_x40 = 0;
    m_switchRequested = 0;
    m_pickCamera = 0;
    m_presetID = 0;
}

// Retail calls the camera destructors directly: CameraBehavior has no
// virtual dtor.
JetskiCameraController::~JetskiCameraController()
{
    if (m_followCamera)
        delete m_followCamera;
    if (m_thrillCamera)
        delete m_thrillCamera;
    if (m_cinematographer)
        delete m_cinematographer;
    if (m_wreckCamera)
        delete m_wreckCamera;
    if (m_tvCamera)
        delete m_tvCamera;
    if (m_firstPersonCamera)
        delete m_firstPersonCamera;
}

// Session shim. mwcc never initialises the "has call" byte of a constant's expression node, so
// it keeps whatever an earlier declaration left in the parser's heap, and FunctionCalls.c
// evaluates arguments with that byte set first. The global optimizer turns every 10.0f and 20.0f
// below into a copy of the first one (in the preset setup), and retail's heap had those two
// bytes set: JetskiThrillCamera::Initialize loads 10 and 20 before 70, 200 and 5. This unused
// inline function's initializer rewrites that heap region: zeros, with a 1 at each of those two
// flag bytes. No code; its image object goes to .mwccshim (mwcc_shim.h). Still needed in the
// whole TU (8 words off without it), at the per-function file's offsets.



struct MwccCreateImage {
    char pad0[0x5AE5];
    char flag0;
    char pad1[0xE72F];
    char flag1;
};
#pragma section mwccshim begin
inline void MwccCreateShim() { const MwccCreateImage image = { {0}, 1, {0}, 1 }; }
#pragma section mwccshim end

JetskiCameraController *JetskiCameraController::Create(Camera *camera, ViewPortCode viewport, Scene *scene,
                                                       int limitHeight)
{
    FollowCameraPreset presets[8];
    if (!CameraController::Create(camera))
        return 0;
    m_x80 = viewport;
    presets[0].m_x00 = 0;
    presets[0].m_x04 = 3.1415923f;
    if (m_x80 == 0) {
        if (LpGame->m_gameOptions->m_raceType == 8) {
        presets[0].m_x08 = 0.7915067f;
        presets[0].m_x0C = 8.39f;
        presets[0].m_x10 = 82.0f;
        presets[0].m_x40 = 5.0f;
        presets[0].m_x44 = 100.0f;
        presets[0].m_x48 = 40.0f;
        presets[0].m_x4C = 115.0f;
        } else {
        presets[0].m_x08 = 0.8726645f;
        presets[0].m_x0C = 8.39f;
        presets[0].m_x10 = 82.0f;
        presets[0].m_x40 = 5.0f;
        presets[0].m_x44 = 100.0f;
        presets[0].m_x48 = 40.0f;
        presets[0].m_x4C = 115.0f;
        }
    } else if (m_x80 == 1) {
        presets[0].m_x08 = 0.57997286f;
        presets[0].m_x0C = 10.11f;
        presets[0].m_x10 = 75.0f;
        presets[0].m_x40 = 5.0f;
        presets[0].m_x44 = 100.0f;
        presets[0].m_x48 = 40.0f;
        presets[0].m_x4C = 115.0f;
    } else {
        presets[0].m_x08 = 0.88313645f;
        presets[0].m_x0C = 8.39f;
        presets[0].m_x10 = 60.0f;
        presets[0].m_x40 = 5.0f;
        presets[0].m_x44 = 100.0f;
        presets[0].m_x48 = 40.0f;
        presets[0].m_x4C = 115.0f;
    }
    presets[0].m_x14 = 0.0f;
    presets[0].m_x18 = 10.0f;
    presets[0].m_x1C = 10.0f;
    presets[0].m_x20 = 0.35f;
    presets[0].m_x24 = 1;
    presets[0].m_x28 = 1;
    presets[0].m_x2C = 0;
    presets[0].m_x30 = 16.0f;
    presets[0].m_x34 = 0;
    presets[0].m_x38 = 1.0f;
    presets[0].m_x3C = 1.0f;
    presets[1].m_x00 = 1;
    presets[1].m_x04 = 3.1415923f;
    if (m_x80 == 0) {
        presets[1].m_x08 = 1.0995573f;
        presets[1].m_x0C = 5.0f;
        presets[1].m_x10 = 80.0f;
        presets[1].m_x40 = 5.0f;
        presets[1].m_x44 = 100.0f;
        presets[1].m_x48 = 40.0f;
        presets[1].m_x4C = 115.0f;
    } else if (m_x80 == 1) {
        presets[1].m_x08 = 0.8899433f;
        presets[1].m_x0C = 5.0f;
        presets[1].m_x10 = 75.0f;
        presets[1].m_x40 = 5.0f;
        presets[1].m_x44 = 100.0f;
        presets[1].m_x48 = 40.0f;
        presets[1].m_x4C = 115.0f;
    } else {
        presets[1].m_x08 = 1.0995573f;
        presets[1].m_x0C = 5.0f;
        presets[1].m_x10 = 60.0f;
        presets[1].m_x40 = 5.0f;
        presets[1].m_x44 = 100.0f;
        presets[1].m_x48 = 40.0f;
        presets[1].m_x4C = 115.0f;
    }
    presets[1].m_x14 = 0.0f;
    presets[1].m_x18 = 10.0f;
    presets[1].m_x1C = 10.0f;
    presets[1].m_x20 = 0.35f;
    presets[1].m_x24 = 1;
    presets[1].m_x28 = 1;
    presets[1].m_x2C = 0;
    presets[1].m_x30 = 15.0f;
    presets[1].m_x34 = 0;
    presets[1].m_x38 = 1.0f;
    presets[1].m_x3C = 1.0f;
    presets[2].m_x00 = 2;
    if (m_x80 == 0) {
        presets[2].m_x04 = 2.1764252f;
        presets[2].m_x08 = 0.5288347f;
        presets[2].m_x0C = 12.2f;
        presets[2].m_x10 = 85.0f;
        presets[2].m_x40 = 5.0f;
        presets[2].m_x44 = 100.0f;
        presets[2].m_x48 = 40.0f;
        presets[2].m_x4C = 115.0f;
    } else if (m_x80 == 1) {
        presets[2].m_x04 = 2.3858647f;
        presets[2].m_x08 = 0.35430178f;
        presets[2].m_x0C = 8.77f;
        presets[2].m_x10 = 75.0f;
        presets[2].m_x40 = 5.0f;
        presets[2].m_x44 = 100.0f;
        presets[2].m_x48 = 40.0f;
        presets[2].m_x4C = 115.0f;
    } else {
        presets[2].m_x04 = 2.5865777f;
        presets[2].m_x08 = 0.403171f;
        presets[2].m_x0C = 10.5f;
        presets[2].m_x10 = 60.0f;
        presets[2].m_x40 = 5.0f;
        presets[2].m_x44 = 100.0f;
        presets[2].m_x48 = 40.0f;
        presets[2].m_x4C = 115.0f;
    }
    presets[2].m_x14 = 0.0f;
    presets[2].m_x18 = 10.0f;
    presets[2].m_x1C = 10.0f;
    presets[2].m_x20 = 0.35f;
    presets[2].m_x24 = 1;
    presets[2].m_x28 = 1;
    presets[2].m_x2C = 0;
    presets[2].m_x30 = 13.56f;
    presets[2].m_x34 = 0;
    presets[2].m_x38 = 1.0f;
    presets[2].m_x3C = 1.0f;
    presets[3].m_x00 = 3;
    if (m_x80 == 0) {
        presets[3].m_x04 = 2.179916f;
        presets[3].m_x08 = 0.27401665f;
        presets[3].m_x0C = 10.0f;
        presets[3].m_x10 = 90.0f;
        presets[3].m_x40 = 5.0f;
        presets[3].m_x44 = 100.0f;
        presets[3].m_x48 = 40.0f;
        presets[3].m_x4C = 115.0f;
    } else if (m_x80 == 1) {
        presets[3].m_x04 = 2.3099432f;
        presets[3].m_x08 = 0.46006876f;
        presets[3].m_x0C = 12.96f;
        presets[3].m_x10 = 75.0f;
        presets[3].m_x40 = 5.0f;
        presets[3].m_x44 = 100.0f;
        presets[3].m_x48 = 40.0f;
        presets[3].m_x4C = 115.0f;
    } else {
        presets[3].m_x04 = 2.0996308f;
        presets[3].m_x08 = 0.34732047f;
        presets[3].m_x0C = 10.5f;
        presets[3].m_x10 = 60.0f;
        presets[3].m_x40 = 5.0f;
        presets[3].m_x44 = 100.0f;
        presets[3].m_x48 = 40.0f;
        presets[3].m_x4C = 115.0f;
    }
    presets[3].m_x14 = 0.0f;
    presets[3].m_x18 = 10.0f;
    presets[3].m_x1C = 10.0f;
    presets[3].m_x20 = 0.35f;
    presets[3].m_x24 = 1;
    presets[3].m_x28 = 1;
    presets[3].m_x2C = 0;
    presets[3].m_x30 = 20.0f;
    presets[3].m_x34 = 0;
    presets[3].m_x38 = 1.0f;
    presets[3].m_x3C = 1.0f;
    presets[4].m_x00 = 4;
    if (m_x80 == 0) {
        presets[4].m_x04 = 3.0106926f;
        presets[4].m_x08 = 0.8796459f;
        presets[4].m_x0C = 31.6f;
        presets[4].m_x10 = 82.0f;
        presets[4].m_x40 = 5.0f;
        presets[4].m_x44 = 100.0f;
        presets[4].m_x48 = 40.0f;
        presets[4].m_x4C = 115.0f;
    } else if (m_x80 == 1) {
        presets[4].m_x04 = 3.1415923f;
        presets[4].m_x08 = 0.46006876f;
        presets[4].m_x0C = 26.5f;
        presets[4].m_x10 = 75.0f;
        presets[4].m_x40 = 5.0f;
        presets[4].m_x44 = 100.0f;
        presets[4].m_x48 = 40.0f;
        presets[4].m_x4C = 115.0f;
    } else {
        presets[4].m_x04 = 3.1415923f;
        presets[4].m_x08 = 0.77003914f;
        presets[4].m_x0C = 26.6f;
        presets[4].m_x10 = 60.0f;
        presets[4].m_x40 = 5.0f;
        presets[4].m_x44 = 100.0f;
        presets[4].m_x48 = 40.0f;
        presets[4].m_x4C = 115.0f;
    }
    presets[4].m_x14 = 0.0f;
    presets[4].m_x18 = 10.0f;
    presets[4].m_x1C = 10.0f;
    presets[4].m_x20 = 0.35f;
    presets[4].m_x24 = 1;
    presets[4].m_x28 = 1;
    presets[4].m_x2C = 0;
    presets[4].m_x30 = 38.56f;
    presets[4].m_x34 = 0;
    presets[4].m_x38 = 1.0f;
    presets[4].m_x3C = 1.0f;
    presets[5].m_x00 = 5;
    if (m_x80 == 0) {
        presets[5].m_x04 = 0.0f;
        presets[5].m_x08 = 0.08901178f;
        presets[5].m_x0C = 32.1f;
        presets[5].m_x10 = 82.0f;
        presets[5].m_x40 = 47.0f;
        presets[5].m_x44 = 47.1f;
        presets[5].m_x48 = 40.0f;
        presets[5].m_x4C = 115.0f;
    } else if (m_x80 == 1) {
        presets[5].m_x04 = 0.0f;
        presets[5].m_x08 = 0.08901178f;
        presets[5].m_x0C = 30.1f;
        presets[5].m_x10 = 75.0f;
        presets[5].m_x40 = 47.0f;
        presets[5].m_x44 = 47.1f;
        presets[5].m_x48 = 40.0f;
        presets[5].m_x4C = 115.0f;
    } else {
        presets[5].m_x04 = 0.0f;
        presets[5].m_x08 = 0.08901178f;
        presets[5].m_x0C = 30.1f;
        presets[5].m_x10 = 60.0f;
        presets[5].m_x40 = 47.0f;
        presets[5].m_x44 = 47.1f;
        presets[5].m_x48 = 40.0f;
        presets[5].m_x4C = 115.0f;
    }
    presets[5].m_x14 = 0.0f;
    presets[5].m_x18 = 10.0f;
    presets[5].m_x1C = 10.0f;
    presets[5].m_x20 = 0.35f;
    presets[5].m_x24 = 0;
    presets[5].m_x28 = 1;
    presets[5].m_x2C = 0;
    presets[5].m_x30 = 28.56f;
    presets[5].m_x34 = 1;
    presets[5].m_x38 = 0.0f;
    presets[5].m_x3C = 1.0f;
    presets[6].m_x00 = 6;
    presets[6].m_x04 = 3.1415923f;
    presets[6].m_x08 = 0.39269903f;
    presets[6].m_x0C = 15.0f;
    presets[6].m_x10 = 70.0f;
    presets[6].m_x40 = 5.0f;
    presets[6].m_x44 = 20.0f;
    presets[6].m_x48 = 40.0f;
    presets[6].m_x4C = 115.0f;
    presets[6].m_x14 = 0.0f;
    presets[6].m_x18 = 10.0f;
    presets[6].m_x1C = 10.0f;
    presets[6].m_x20 = 0.35f;
    presets[6].m_x24 = 0;
    presets[6].m_x28 = 1;
    presets[6].m_x2C = 0;
    presets[6].m_x30 = 5.0f;
    presets[6].m_x34 = 0;
    presets[6].m_x38 = 1.0f;
    presets[6].m_x3C = 1.0f;
    presets[7].m_x00 = 7;
    presets[7].m_x04 = 3.1415923f;
    presets[7].m_x08 = 1.1344639f;
    presets[7].m_x0C = 100.0f;
    presets[7].m_x10 = 50.0f;
    presets[7].m_x40 = 50.0f;
    presets[7].m_x44 = 200.0f;
    presets[7].m_x48 = 40.0f;
    presets[7].m_x4C = 115.0f;
    presets[7].m_x14 = 0.0f;
    presets[7].m_x18 = 10.0f;
    presets[7].m_x1C = 10.0f;
    presets[7].m_x20 = 0.35f;
    presets[7].m_x24 = 0;
    presets[7].m_x2C = 1;
    presets[7].m_x28 = 1;
    presets[7].m_x30 = 0.0f;
    presets[7].m_x34 = 0;
    presets[7].m_x38 = 1.0f;
    presets[7].m_x3C = 1.0f;
    m_followCamera = new JetskiFollowCamera;
    if (!m_followCamera->Initialize(scene->m_startPosition, *scene->m_dynamicNodes, 0.55f, 8, presets,
                                    limitHeight ? 180.0f : 3.40282347e+38f, 0.3f, 0.225f, 0.6f, 2.0f, 0))
        return 0;
    m_thrillCamera = new JetskiThrillCamera;
    if (!m_thrillCamera->Initialize(70.0f, 10.0f, 200.0f, 20.0f, 5.0f, limitHeight ? 180.0f : 3.40282347e+38f))
        return 0;
    _D3DVECTOR offset(0.0f, 2.0f, 0.0f);
    m_firstPersonCamera = new FirstPersonCamera;
    if (!m_firstPersonCamera->Initialize(offset, 90.0f))
        return 0;
    m_tvCamera = new JetskiTVCamera;
    m_cinematographer = new CinematographerCam;
    if (!m_cinematographer->Initialize(1, 0.25f, 40.0f, 130.0f, 120.0f, 25.0f, 0.15f, 0.85f, 0.5f))
        return 0;
    WreckCameraParams params;
    params.m_x00 = 1.5707961f;
    params.m_x04 = 0.3490658f;
    params.m_x08 = 20.0f;
    params.m_x0C = 75.0f;
    params.m_x10 = 10.0f;
    params.m_x14 = 50.0f;
    params.m_x18 = 21.0f;
    params.m_x1C = 85.0f;
    m_wreckCamera = new JetskiWreckCamera;
    if (!m_wreckCamera->Initialize(0.5f, params, limitHeight ? 180.0f : 3.40282347e+38f, 0.5f, 0.6f, 8.0f, 100.0f))
        return 0;
    m_x80 = viewport;
    return this;
}

// The jetski's body is a PhysicsRigidBody; the implicit upcast at each
// SetPhysicsBody call is what orders retail's argument loads.
void JetskiCameraController::SetJetski(JetskiVehicle *jetski, int player)
{
    m_followCamera->SetPhysicsBody(jetski->m_body, 3.0f, 70.0f);
    if (m_wreckCamera->m_watchingBodies)
        m_wreckCamera->RemoveAllPhysicsBodies();
    else
        m_wreckCamera->RemoveAllSoultreeObjects();
    m_wreckCamera->AddPhysicsBody(jetski->m_rider->m_wrecker->m_body);
    m_wreckCamera->AddPhysicsBody(jetski->m_body);
    m_thrillCamera->SetPhysicsBody(jetski->m_body, 3.0f, 70.0f);
    m_thrillCamera->m_jetski = jetski;
    m_firstPersonCamera->SetPhysicsBody(jetski->m_body);
    if (m_tvCamera)
        m_tvCamera->SetPhysicsBody(jetski->m_rider->m_wrecker->m_body, 3.0f, 80.0f);
    if (m_cinematographer)
        m_cinematographer->SetPhysicsBody(jetski->m_rider->m_wrecker->m_body, 3.0f, 80.0f);
    m_jetski = jetski;
    m_x64 = player;
    m_x68 = -1;
}

int JetskiCameraController::Initialize(Terrain *terrain, PSXWater *water, int player, int viewport)
{
    if (m_player < 0) {
        m_player = player;
        m_viewport = viewport;
        if (terrain) {
            m_followCamera->m_terrain = terrain;
            m_cinematographer->SetTerrain(terrain);
            m_thrillCamera->m_terrain = terrain;
            m_wreckCamera->m_terrain = terrain;
        }
        if (water) {
            m_followCamera->m_water = water;
            m_thrillCamera->m_water = water;
            m_wreckCamera->m_water = water;
        }
        AddBehavior(m_followCamera);
        AddBehavior(m_firstPersonCamera);
        AddBehavior(m_wreckCamera);
        AddBehavior(m_cinematographer);
        AddBehavior(m_thrillCamera);
        AddBehavior(m_tvCamera);
        if (m_tvCamera && !m_tvCamera->LoadPositions(SceneContentDirectory)) {
            delete m_tvCamera;
            m_tvCamera = 0;
        }
    }
    return 1;
}

int JetskiCameraController::Tick(float dt)
{
    static int JSCamCntrTickID = -1;    // profiler ID, unused
    int player = m_player;
    if (player < 0) {
        PSXprintf("JetskiCameraController not initialized!!!!\n");
        return 0;
    }
    if (m_cameraType == 0) {
        CameraJoystickInput input;
        input.m_button[3] = 0;
        input.m_button[4] = 0;
        input.m_axis[0] = 0;
        input.m_axis[1] = 0;
        if (LpGame->m_controls->IsKeyDown((AssignableActionIndex)0xE, (DeviceType)2, 0x3F, m_player))
            input.m_button[0] = 1;
        else
            input.m_button[0] = 0;
        input.m_button[2] = 0;
        input.m_button[1] = 0;
        JetskiCompetition *competition = LpGame->m_competition;
        if (!competition->m_x118) {
            JetskiAIVehicle *vehicle = (JetskiAIVehicle *)competition->m_vehicles[m_player];
            input.m_axis[2] = -vehicle->m_axes[2]->m_value;
            input.m_axis[3] = -vehicle->m_axes[3]->m_value;
        } else {
            JetskiPlayerVehicle *vehicle = (JetskiPlayerVehicle *)competition->m_vehicles[m_player + 5];
            if (LpGame->m_gameOptions->m_raceType == 8 || (competition->m_raceStatus->m_flags & 2)) {
                input.m_axis[2] = 0;
                input.m_axis[3] = 0;
            } else {
                input.m_axis[2] = -vehicle->m_axis[2]->m_value;
                input.m_axis[3] = -vehicle->m_axis[3]->m_value;
            }
        }
        m_followCamera->SetInputs(input);
    } else if (m_cameraType == 2) {
        CameraJoystickInput input;
        if (LpGame->m_controls->IsKeyDown((AssignableActionIndex)0xA, (DeviceType)2, 0x3F, player))
            input.m_button[0] = 1;
        else
            input.m_button[0] = 0;
        if (LpGame->m_controls->IsKeyDown((AssignableActionIndex)0xB, (DeviceType)2, 0x3F, player))
            input.m_button[1] = 1;
        else
            input.m_button[1] = 0;
        if (LpGame->m_controls->IsKeyDown((AssignableActionIndex)0xC, (DeviceType)2, 0x3F, player))
            input.m_button[2] = 1;
        else
            input.m_button[2] = 0;
        if (LpGame->m_controls->IsKeyDown((AssignableActionIndex)0xD, (DeviceType)2, 0x3F, player))
            input.m_button[3] = 1;
        else
            input.m_button[3] = 0;
        if (m_x40) {
            input.m_button[4] = 1;
            m_x40 = 0;
        } else {
            input.m_button[4] = 0;
        }
        if (LpGame->m_controls->IsKeyDown((AssignableActionIndex)7, (DeviceType)2, 0x3F, player))
            input.m_button[5] = 1;
        else
            input.m_button[5] = 0;
        JetskiCompetition *competition = LpGame->m_competition;
        if (!competition->m_x118) {
            JetskiAIVehicle *vehicle = (JetskiAIVehicle *)competition->m_vehicles[m_player];
            input.m_axis[0] = vehicle->m_axes[0]->m_value;
            input.m_axis[1] = vehicle->m_axes[1]->m_value;
            input.m_axis[2] = vehicle->m_axes[2]->m_value;
            input.m_axis[3] = -vehicle->m_axes[3]->m_value;
        } else {
            JetskiPlayerVehicle *vehicle = (JetskiPlayerVehicle *)competition->m_vehicles[m_player + 5];
            if (!(competition->m_raceStatus->m_flags & 2)) {
                input.m_axis[0] = vehicle->m_axis[0]->m_value;
                input.m_axis[1] = vehicle->m_axis[1]->m_value;
                input.m_axis[2] = vehicle->m_axis[2]->m_value;
                input.m_axis[3] = -vehicle->m_axis[3]->m_value;
            }
        }
        m_cinematographer->SetInputs(input);
    }
    if (LpGame->m_competition->m_status != 10) {
        int wrecked = m_jetski->m_rider->m_wrecker->m_xB0;
        if (wrecked && !(LpGame->m_competition->m_raceStatus->m_flags & 2) && m_cameraType == 0) {
            SetBehavior(m_wreckCamera);
            CameraStateParams state;
            state = m_followCamera->SwitchOff();
            m_camera->GetViewParameters((_D3DVECTOR *)&state.position, (_D3DVECTOR *)&state.lookPoint,
                                        (_D3DVECTOR *)&state.up, &state.f30, &state.f34);
            m_wreckCamera->SwitchOn(state, 1);
            m_cameraType = 1;
        } else if ((!wrecked || (LpGame->m_competition->m_raceStatus->m_flags & 2)) && m_cameraType == 1) {
            SetBehavior(m_followCamera);
            CameraStateParams state;
            state = m_wreckCamera->SwitchOff();
            m_camera->GetViewParameters((_D3DVECTOR *)&state.position, (_D3DVECTOR *)&state.lookPoint,
                                        (_D3DVECTOR *)&state.up, &state.f30, &state.f34);
            m_followCamera->SwitchOn(state, 0);
            m_cameraType = 0;
        }
        if (LpGame->m_gameOptions->m_raceType != 8) {
            if (!m_jetski->m_x530 && !m_cameraType && m_followCamera->m_curPreset != 5 &&
                LpGame->m_controls->IsKeyDown((AssignableActionIndex)0x13, (DeviceType)2, 0x3F, m_player)) {
                m_followCamera->m_x130 = m_followCamera->m_curPreset;
                m_followCamera->ForcePreset(5, 1, 0);
                m_followCamera->m_resetFilters = 1;
            } else if (m_followCamera->m_curPreset == 5 &&
                       !LpGame->m_controls->IsKeyDown((AssignableActionIndex)0x13, (DeviceType)2, 0x3F, m_player)) {
                m_followCamera->ForcePreset(m_followCamera->m_x130, 0, 1);
                m_followCamera->m_resetFilters = 1;
            }
        }
    } else if (m_tvCamera) {
        _D3DVECTOR position;
        m_pickCamera = m_tvCamera->GetTheBestCamera(dt, &m_jetski->m_body->m_x60, &position);
    }
    return CameraController::Tick(dt);
}

// Heap shim (unused; image object in .mwccshim): a heap image that sets mwcc's uninitialised
// "has call" byte on the 2.0f and 1.0f constants of the AddInfoText call, so they load
// first as in retail. Still needed in the
// whole TU (13 words off without it); recalibrated for it with image.py (the
// tail zeroes the rest of the function's epoch). Offsets move if anything above
// this function changes.
struct MwccHeapImage {
    char pad0[0x43CD];
    char flag0;
    char pad1[0x7F];
    char flag1;
    char tail[0x6FB2];
};
#pragma section mwccshim begin
inline void MwccHeapShim() { const MwccHeapImage image = { {0}, 1, {0}, 1 }; }
#pragma section mwccshim end

int JetskiCameraController::KeyDownHandler(ProcessNode *node, KeyInfo *key)
{
    if (LpGame->m_controls->IsKeyDown((AssignableActionIndex)0x12, (DeviceType)2, 0x3F, m_player) &&
        !LpGame->m_controls->IsKeyDown((AssignableActionIndex)7, (DeviceType)2, 0x3F, m_player)) {
        if (m_cameraType == 0) {
            if (m_followCamera->m_settings[2] != 0.0f || m_followCamera->m_settings[1] != 0.0f) {
                m_followCamera->ForcePreset(m_followCamera->GetCurrentPresetID(), 0, 0);
            } else if (LpGame->m_competition->m_status != 10) {
                m_followCamera->NextPreset();
                m_presetID = m_followCamera->GetCurrentPresetID();
                int id;
                if (m_presetID == 0)
                    id = 0xF3;
                else if (m_presetID == 1)
                    id = 0xF4;
                else if (m_presetID == 2)
                    id = 0xF5;
                else if (m_presetID == 3)
                    id = 0xF6;
                else
                    id = 0xF7;
                LpGame->m_competition->m_infoText->AddInfoText((char *)LpGame->m_text->GetString(id), m_viewport,
                                                               2.0f, 1, 1.0f);
            }
        } else if (m_cameraType == 6) {
            SetBehavior(m_followCamera);
            CameraStateParams params;
            params = m_firstPersonCamera->SwitchOff();
            m_camera->GetViewParameters((_D3DVECTOR *)&params.position, (_D3DVECTOR *)&params.lookPoint,
                                        (_D3DVECTOR *)&params.up, &params.f30, &params.f34);
            m_followCamera->SwitchOn(params, 0);
            m_cameraType = 0;
            m_followCamera->NextPreset();
        }
    }
    if (m_cameraType == 2) {
        if (LpGame->m_controls->IsKeyDown((AssignableActionIndex)4, (DeviceType)2, 0x3F, m_player))
            m_x40 = 1;
        if (KEYDOWN_HANDLER_TEST((AssignableActionIndex)5, (DeviceType)2, node, 0x3F))
            m_cinematographer->m_x88 = !m_cinematographer->m_x88;
        if (KEYDOWN_HANDLER_TEST((AssignableActionIndex)9, (DeviceType)2, node, 0x3F)) {
            m_cinematographer->m_x50 += 0.62831855f;
            if (!(m_cinematographer->m_x50 <= 2.8274333f))
                m_cinematographer->m_x50 = 1.5707964f;
        }
    }
    if (!LpGame->m_paused) {
        if (LpGame->m_controls->IsKeyDown((AssignableActionIndex)0x11, (DeviceType)2, 0x3F, m_player) &&
            m_cameraType == 0) {
            SetBehavior(m_thrillCamera);
            CameraStateParams params;
            params = m_followCamera->SwitchOff();
            m_camera->GetViewParameters((_D3DVECTOR *)&params.position, (_D3DVECTOR *)&params.lookPoint,
                                        (_D3DVECTOR *)&params.up, &params.f30, &params.f34);
            m_thrillCamera->SwitchOn(params, 0);
            m_cameraType = 3;
            m_switchRequested = 1;
        }
    }
    return GameObject::KeyDownHandler(node, key);
}

// Releasing the camera-switch action (0x11) flips between the follow camera
// and the thrill camera (the wreck camera while the rider is wrecked),
// handing over the current view.
int JetskiCameraController::KeyUpHandler(ProcessNode *node, KeyInfo *key)
{
    if (!LpGame->m_controls->IsKeyDown((AssignableActionIndex)0x11, (DeviceType)2, 0x3F, m_player)) {
        if (m_switchRequested) {
            m_switchRequested = 0;
            CameraStateParams params;
            if (m_cameraType == 3) {
                if (m_jetski->m_rider->m_wrecker->m_xB0) {
                    SetBehavior(m_wreckCamera);
                    params = m_followCamera->SwitchOff();
                    m_camera->GetViewParameters((_D3DVECTOR *)&params.position, (_D3DVECTOR *)&params.lookPoint,
                                                (_D3DVECTOR *)&params.up, &params.f30, &params.f34);
                    m_wreckCamera->SwitchOn(params, 0);
                    m_cameraType = 1;
                } else {
                    SetBehavior(m_followCamera);
                    params = m_thrillCamera->SwitchOff();
                    m_camera->GetViewParameters((_D3DVECTOR *)&params.position, (_D3DVECTOR *)&params.lookPoint,
                                                (_D3DVECTOR *)&params.up, &params.f30, &params.f34);
                    m_followCamera->SwitchOn(params, 0);
                    m_cameraType = 0;
                }
            }
        }
    }
    return GameObject::KeyUpHandler(node, key);
}

// Hands the view over from the current camera behavior to the requested one.
int JetskiCameraController::SwitchCamera(CameraType type)
{
    if (type == m_cameraType)
        return 1;
    CameraStateParams params;
    switch (type) {
    case 0:
        if (!m_followCamera)
            return 0;
        break;
    case 3:
        if (!m_thrillCamera)
            return 0;
        break;
    case 4:
        if (!m_tvCamera)
            return 0;
        break;
    }
    switch (m_cameraType) {
    case 0:
        params = m_followCamera->SwitchOff();
        break;
    case 3:
        params = m_thrillCamera->SwitchOff();
        break;
    case 4:
        params = m_tvCamera->SwitchOff();
        break;
    }
    m_camera->GetViewParameters((_D3DVECTOR *)&params.position, (_D3DVECTOR *)&params.lookPoint,
                                (_D3DVECTOR *)&params.up, &params.f30, &params.f34);
    switch (type) {
    case 0:
        SetBehavior(m_followCamera);
        m_followCamera->SwitchOn(params, 0);
        m_cameraType = 0;
        break;
    case 3:
        SetBehavior(m_thrillCamera);
        m_thrillCamera->SwitchOn(params, 0);
        m_cameraType = 3;
        break;
    case 4:
        SetBehavior(m_tvCamera);
        m_tvCamera->SwitchOn(params, 0);
        m_cameraType = 4;
        break;
    }
    m_followCamera->m_resetFilters = 1;
    return 1;
}

int JetskiCameraController::SwitchFollowCameraPresets(FollowCameraPresetType preset)
{
    if (m_jetski->m_x530 || m_cameraType)
        return 0;
    JetskiFollowCamera *camera = m_followCamera;
    // Retail's view of these two is FollowCameraPresetType: read as the enum,
    // the compare and copy allocate retail's registers (as ints they swap).
    if (*(FollowCameraPresetType *)&camera->m_curPreset == preset)
        return 1;
    *(FollowCameraPresetType *)&camera->m_x130 = *(FollowCameraPresetType *)&camera->m_curPreset;
    m_followCamera->ChangeToPreset(preset);
    return 1;
}

void JetskiCameraController::StartRotate()
{
    if (m_jetski->m_x530 || m_cameraType)
        return;
    m_followCamera->m_rotating = 1;
}

void JetskiCameraController::StopRotate()
{
    m_followCamera->m_rotating = 0;
}

void JetskiCameraController::ToggleFreezeRotate()
{
    if (m_jetski->m_x530 || m_cameraType)
        return;
    m_followCamera->m_freezeRotate = !m_followCamera->m_freezeRotate;
}
