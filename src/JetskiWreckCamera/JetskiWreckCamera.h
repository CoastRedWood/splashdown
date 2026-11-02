// JetskiWreckCamera: Splashdown's wreck camera (a SimpleWreckCamera).
// 0x340 bytes (`new` in JetskiCameraController::Create); its own data starts
// after SimpleWreckCamera's 0x32C. Overrides only ConstrainCamera
// (retail vtable); the dtor is not virtual
// (CameraBehavior has no dtor slot), so JetskiCameraController's dtor calls
// it directly. Initialize returns int: Create tests it.
#ifndef JETSKIWRECKCAMERA_JETSKIWRECKCAMERA_H
#define JETSKIWRECKCAMERA_JETSKIWRECKCAMERA_H

#include "SimpleWreckCamera/SimpleWreckCamera.h"

class Terrain;
class Water;

class JetskiWreckCamera : public SimpleWreckCamera {
public:
    JetskiWreckCamera();
    ~JetskiWreckCamera();
    int Initialize(float fovRate, WreckCameraParams params, float distance, float lookRate, float positionRate,
                   float height, float x54);

    // --- virtuals in retail slot order ---
    virtual int ConstrainCamera(_D3DVECTOR *position, int adjust);

    char pad32C[0x330 - 0x32C];
    Terrain *m_terrain;                 // +0x330
    Water *m_water;                     // +0x334
    float m_height;                     // +0x338
    float m_distance;                   // +0x33C
};

#endif
