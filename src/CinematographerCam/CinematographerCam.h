// CinematographerCam: the free-roaming "cinematographer" camera behaviour (a
// CameraBehavior, vptr at +0x00). 0xE0 bytes (`new` in
// JetskiCameraController::Initialize). +0x8C..+0xA0 are the CameraFilters
// Initialize builds for +0x10/+0x14/+0x18 (position), +0x54/+0x58 (pitch,
// yaw) and +0xAC; two copies declare them float *. +0xB8 is SetInputs'
// CameraJoystickInput (defined in CameraBehavior.h).
#ifndef CINEMATOGRAPHERCAM_CINEMATOGRAPHERCAM_H
#define CINEMATOGRAPHERCAM_CINEMATOGRAPHERCAM_H

#include "CameraBehavior/CameraBehavior.h"
#include "d3dtypes.h"

class CameraFilter;
class PhysicsBody;
class SoultreeObject;
class Terrain;

class CinematographerCam : public CameraBehavior {
public:
    CinematographerCam();
    ~CinematographerCam();
    int Initialize(int mode, float targetRate, float xB0, float xB4, float x68, float x6C, float a, float b,
                   float c);
    void SetPhysicsBody(PhysicsBody *body, float distance, float speedMph);
    void SetTerrain(Terrain *terrain);

    // --- virtuals in retail slot order ---
    virtual void SwitchOn(CameraStateParams params, int copyFilter);
    virtual CameraStateParams SwitchOff();
    virtual void SetInputs(CameraJoystickInput input);
    virtual void ComputeCameraData(float dt, int &state, _D3DVECTOR *position, _D3DVECTOR *look, float *fov,
                                   _D3DVECTOR *a, _D3DVECTOR *b, float *c);

    char pad4[0x10 - 0x4];
    _D3DVECTOR m_lastPosition;          // +0x10
    _D3DVECTOR m_position;              // +0x20
    _D3DVECTOR m_x30;                   // +0x30
    _D3DVECTOR m_x40;                   // +0x40
    float m_x50;                        // +0x50
    float m_pitch;                      // +0x54
    float m_yaw;                        // +0x58
    float m_speed;                      // +0x5C
    float m_distance;                   // +0x60
    int m_x64;                          // +0x64
    float m_x68;                        // +0x68
    float m_x6C;                        // +0x6C
    PhysicsBody *m_body;                // +0x70 (SetPhysicsBody)
    SoultreeObject *m_target;           // +0x74, looked at when m_input says so (ComputeCameraData)
    Terrain *m_terrain;                 // +0x78 (SetTerrain)
    float m_x7C;                        // +0x7C
    int m_mode;                         // +0x80
    int m_x84;                          // +0x84
    int m_x88;                          // +0x88
    CameraFilter *m_filterX;            // +0x8C
    CameraFilter *m_filterY;            // +0x90
    CameraFilter *m_filterZ;            // +0x94
    CameraFilter *m_pitchFilter;        // +0x98
    CameraFilter *m_yawFilter;          // +0x9C
    CameraFilter *m_fovFilter;          // +0xA0
    int m_xA4;                          // +0xA4
    int m_manual;                       // +0xA8
    float m_fov;                        // +0xAC
    float m_minFov;                     // +0xB0
    float m_maxFov;                     // +0xB4
    CameraJoystickInput m_input;        // +0xB8 (SetInputs)
};

#endif
