// JetskiThrillCamera: Splashdown's stunt camera (a SimpleThrillCamera).
// 0x80 bytes (`new` in JetskiCameraController::Create; no code seen so far
// touches +0x78..+0x7F). Its own data
// starts at +0x60 (the ctor clears +0x60/+0x64/+0x68/+0x74 and sets +0x6C to
// 3, +0x70 to FLT_MAX). Overrides only ComputePosition (the retail vtable
// JetskiThrillCamera); the dtor is not virtual (CameraBehavior has no dtor
// slot), so JetskiCameraController's dtor calls it directly.
#ifndef JETSKITHRILLCAMERA_JETSKITHRILLCAMERA_H
#define JETSKITHRILLCAMERA_JETSKITHRILLCAMERA_H

#include "SimpleThrillCamera/SimpleThrillCamera.h"

class JetskiVehicle;
class Terrain;
class Water;

class JetskiThrillCamera : public SimpleThrillCamera {
public:
    JetskiThrillCamera();
    ~JetskiThrillCamera();
    int Initialize(float maxFov, float minFov, float farDistance, float nearDistance, float minHeight,
                   float maxHeight);
    int ConstrainCamera(_D3DVECTOR *camera, int adjust, _D3DVECTOR *ground);

    // --- virtuals in retail slot order ---
    virtual _D3DVECTOR ComputePosition(_D3DVECTOR target, _D3DVECTOR velocity);

    char pad54[0x60 - 0x54];
    Terrain *m_terrain;                 // +0x60
    Water *m_water;                     // +0x64
    int m_x68;                          // +0x68
    float m_minHeight;                  // +0x6C, above the ground
    float m_maxHeight;                  // +0x70
    JetskiVehicle *m_jetski;            // +0x74 (JetskiCameraController::SetJetski)
    char pad78[0x80 - 0x78];
};

#endif
