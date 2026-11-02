// SimpleThrillCamera: the stunt ("thrill") camera behaviour (a CameraBehavior,
// vptr at +0x00). Data ends at +0x54 (the ctor's last store, +0x50);
// JetskiThrillCamera derives from it (and keeps a JetskiVehicle at +0x74).
// Names follow its own methods; +0x30 is the look point in ComputeFOV and
// the ctor, the position in ComputeCameraData.
#ifndef SIMPLETHRILLCAMERA_SIMPLETHRILLCAMERA_H
#define SIMPLETHRILLCAMERA_SIMPLETHRILLCAMERA_H

#include "CameraBehavior/CameraBehavior.h"
#include "d3dtypes.h"

class PhysicsBody;
class SoultreeObject;

class SimpleThrillCamera : public CameraBehavior {
public:
    SimpleThrillCamera();
    ~SimpleThrillCamera();
    void SetPhysicsBody(PhysicsBody *target, float a, float b);

    // --- virtuals in retail slot order ---
    virtual void SwitchOn(CameraStateParams params, int copyFilter);
    virtual CameraStateParams SwitchOff();
    virtual void ComputeCameraData(float dt, int &state, _D3DVECTOR *position, _D3DVECTOR *look, float *fov,
                                   _D3DVECTOR *a, _D3DVECTOR *b, float *c);
    virtual int ConstrainCamera(_D3DVECTOR *position, int a);
    virtual float ComputeFOV(_D3DVECTOR target);
    virtual _D3DVECTOR ComputeLookPoint(_D3DVECTOR target);
    virtual _D3DVECTOR ComputePosition(_D3DVECTOR a, _D3DVECTOR b);

    PhysicsBody *m_body;                // +0x04 (SetPhysicsBody)
    SoultreeObject *m_object;           // +0x08
    float m_fov;                        // +0x0C, 80 from the ctor
    float m_maxFov;                     // +0x10
    float m_minFov;                     // +0x14
    float m_farDistance;                // +0x18
    float m_nearDistance;               // +0x1C
    float m_scaledB;                    // +0x20
    float m_a;                          // +0x24
    char pad28[0x30 - 0x28];
    _D3DVECTOR m_lookPoint;             // +0x30
    _D3DVECTOR m_lastTarget;            // +0x40 (the ctor calls it m_offset)
    int m_positioned;                   // +0x50
};

#endif
