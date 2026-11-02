// CameraBehavior: the abstract camera-behaviour interface (SimpleFollowCamera,
// SimpleTVCamera, SimpleThrillCamera, SimpleWreckCamera, CinematographerCam).
// Only a vptr (+0x00), so 4 bytes. Its destructor is not virtual: retail's
// vtables have no dtor slot. Slots 0, 1 and 3 are pure; their signatures are
// SimpleFollowCamera's overrides (retail vtable).
#ifndef CAMERABEHAVIOR_CAMERABEHAVIOR_H
#define CAMERABEHAVIOR_CAMERABEHAVIOR_H

struct _D3DVECTOR;
struct CameraStateParams;

// The joystick state JetskiCameraController::Tick hands to a camera
// behaviour's SetInputs (by value). 0x28 bytes. Each behaviour reads the slots
// its own way. CinematographerCam: buttons up, down, zoom in, zoom out,
// toggle manual, look at target; axes strafe, forward, yaw, pitch.
// SimpleFollowCamera: buttons 0/1 pick distance/height, button 2 reloads the
// preset file; axes 2/3 turn the angles.
struct CameraJoystickInput {
    int m_button[6];                    // +0x00
    float m_axis[4];                    // +0x18
};

class CameraBehavior {
public:
    CameraBehavior();
    ~CameraBehavior();

    // --- virtuals in retail slot order ---
    virtual void SwitchOn(CameraStateParams params, int copyFilter) = 0;
    virtual CameraStateParams SwitchOff() = 0;
    virtual void SetInputs(CameraJoystickInput input);
    virtual void ComputeCameraData(float dt, int &state, _D3DVECTOR *position, _D3DVECTOR *look, float *fov,
                                   _D3DVECTOR *a, _D3DVECTOR *b, float *c) = 0;
};

#endif
