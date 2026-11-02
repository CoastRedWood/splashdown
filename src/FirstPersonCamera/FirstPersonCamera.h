// FirstPersonCamera: the rider's-eye camera behaviour (a CameraBehavior,
// vptr at +0x00). 0x30 bytes (`new` in JetskiCameraController::Initialize).
// +0x20 is the PhysicsBody SetPhysicsBody stores (ComputeCameraData's copy
// calls it a CameraTarget); +0x10 is Initialize's offset from it.
#ifndef FIRSTPERSONCAMERA_FIRSTPERSONCAMERA_H
#define FIRSTPERSONCAMERA_FIRSTPERSONCAMERA_H

#include "CameraBehavior/CameraBehavior.h"
#include "d3dtypes.h"

class PhysicsBody;
class SoultreeObject;

class FirstPersonCamera : public CameraBehavior {
public:
    FirstPersonCamera();
    ~FirstPersonCamera();
    int Initialize(_D3DVECTOR position, float fov);
    void AddNoise(_D3DVECTOR *v);
    void SetPhysicsBody(PhysicsBody *body);

    // --- virtuals in retail slot order ---
    virtual void SwitchOn(CameraStateParams params, int copyFilter);
    virtual CameraStateParams SwitchOff();
    virtual void ComputeCameraData(float dt, int &state, _D3DVECTOR *position, _D3DVECTOR *look, float *fov,
                                   _D3DVECTOR *a, _D3DVECTOR *b, float *c);

    float m_fov;                        // +0x04
    char pad8[0x10 - 0x8];
    _D3DVECTOR m_offset;                // +0x10
    PhysicsBody *m_body;                // +0x20 (SetPhysicsBody)
    SoultreeObject *m_object;           // +0x24
    float m_noiseScale;                 // +0x28
    float m_noiseAmount;                // +0x2C
};

#endif
