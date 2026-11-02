// JetskiFollowCamera: Splashdown's chase camera (a SimpleFollowCamera).
// 0x1B0 bytes (`new` in JetskiCameraController::Initialize); its own data
// starts after SimpleFollowCamera's 0x11C. The ctor builds +0x150..+0x1A0
// as _D3DVECTORs (w = 1); SwitchFilterValueForPlayback keeps the filter
// rates in +0x170/+0x180 (as float[3] in its copy).
#ifndef JETSKIFOLLOWCAMERA_JETSKIFOLLOWCAMERA_H
#define JETSKIFOLLOWCAMERA_JETSKIFOLLOWCAMERA_H

#include "SimpleFollowCamera/SimpleFollowCamera.h"

class Terrain;
class Water;
struct FollowCameraPreset;

class JetskiFollowCamera : public SimpleFollowCamera {
public:
    JetskiFollowCamera();
    int Initialize(_D3DVECTOR position, SearchNode *node, float a, int b, FollowCameraPreset *preset,
                   float maxHeight, float c, float d, float e, float minHeight, char *name);
    _D3DVECTOR ComputeLookPoint(_D3DVECTOR position, _D3DVECTOR, _D3DVECTOR forward, _D3DVECTOR velocity,
                                float speed, float b, float c, int);
    _D3DVECTOR ComputePosition(float, float, _D3DVECTOR offset, _D3DVECTOR target, _D3DVECTOR, _D3DVECTOR,
                               _D3DVECTOR, _D3DVECTOR, float speed);
    void SwitchFilterValueForPlayback(int playback);

    // --- virtuals in retail slot order ---
    virtual CameraStateParams SwitchOff();
    virtual void ComputeCameraData(float dt, int &state, _D3DVECTOR *position, _D3DVECTOR *look, float *fov,
                                   _D3DVECTOR *a, _D3DVECTOR *b, float *c);
    virtual int ConstrainCamera(_D3DVECTOR *position, int a, int b);
    virtual float CompensateForLookPointObstacles(_D3DVECTOR look);
    virtual void ComputeAngleXAndY(float dt);

    char pad11C[0x120 - 0x11C];
    Terrain *m_terrain;                 // +0x120
    Water *m_water;                     // +0x124
    float m_lookPointRaise;             // +0x128
    float m_minHeight;                  // +0x12C
    int m_x130;                         // +0x130
    float m_maxHeight;                  // +0x134
    int m_x138;                         // +0x138
    int m_x13C;                         // +0x13C
    int m_rotating;                     // +0x140 (JetskiCameraController::StartRotate)
    int m_freezeRotate;                 // +0x144 (ToggleFreezeRotate)
    float m_rotateTime;                 // +0x148, rotate-mode clock
    int m_x14C;                         // +0x14C
    _D3DVECTOR m_position;              // +0x150
    _D3DVECTOR m_x160;                  // +0x160
    _D3DVECTOR m_savedLookRate;         // +0x170
    _D3DVECTOR m_savedPositionRate;     // +0x180
    int m_playbackFilters;              // +0x190
    int m_playback;                     // +0x194
    char pad198[0x1A0 - 0x198];
    _D3DVECTOR m_x1A0;                  // +0x1A0
};

#endif
