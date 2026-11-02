// SimpleFollowCamera: the chase camera behaviour (a CameraBehavior, vptr at
// +0x00). 0x11C bytes. Names follow its own methods (src/SimpleFollowCamera/)
// where they agree; where they don't:
//   +0x4C filters the FOV (FilterCameraParameters: m_fovFilter->Update(fov),
//         Initialize builds it from +0x10, which ComputeFOV and the ctor call
//         m_fov), +0x50 the distance;
//   +0x28..+0x54 are 11 filter pointers (ctor/dtor: m_filter[11]);
//   +0x58..+0x64 are m_settings[4] (SetInputs: distance, angle X/Y, height).
// Initialize takes the presets as FollowCameraPreset, retail's name for
// CameraPreset (its mangled name); +0x118 holds its ViewPortCode.
#ifndef SIMPLEFOLLOWCAMERA_SIMPLEFOLLOWCAMERA_H
#define SIMPLEFOLLOWCAMERA_SIMPLEFOLLOWCAMERA_H

#include "CameraBehavior/CameraBehavior.h"
#include "d3dtypes.h"
#include "ViewPortCode/ViewPortCode.h"

class CameraFilter;
class CollisionObject;
class ParameterBlock;
class PhysicsBody;
class SearchNode;
class SoultreeObject;
struct CameraPreset;
struct FollowCameraPreset;

class SimpleFollowCamera : public CameraBehavior {
public:
    SimpleFollowCamera();
    ~SimpleFollowCamera();
    int Initialize(_D3DVECTOR position, SearchNode *node, float fovRate, int numPresets, FollowCameraPreset *presets,
                   float lookRate, float positionRate, float distanceRate, char *presetFile, int collision,
                   float reverseSpeed, ViewPortCode viewport);
    float ComputeFOV();
    void ChangeToPreset(int index);
    int NextPreset();
    int GetCurrentPresetID();
    int LoadPresetsFromFile();
    int ForcePreset(int id, int save, int restore);
    void FilterCameraParameters(_D3DVECTOR *position, _D3DVECTOR *look, float *fov, float *distance, float dt);
    int ComputeReverseOffsets(_D3DVECTOR forward, _D3DVECTOR direction, float speed);
    _D3DVECTOR ComputePosition(_D3DVECTOR direction, _D3DVECTOR origin, _D3DVECTOR a3, _D3DVECTOR a4,
                               _D3DVECTOR a5, _D3DVECTOR a6);
    int CopyFilterData(CameraStateParams params);
    void SetSoultreeObject(SoultreeObject *target, float a, float b);
    void SetPhysicsBody(PhysicsBody *target, float a, float b);
    void ResetLookFilters(_D3DVECTOR &v);
    void ResetPositionFilters(_D3DVECTOR &v);

    // --- virtuals in retail slot order ---
    virtual void SwitchOn(CameraStateParams params, int copyFilter);
    virtual CameraStateParams SwitchOff();
    virtual void SetInputs(CameraJoystickInput input);
    virtual void ComputeCameraData(float dt, int &state, _D3DVECTOR *position, _D3DVECTOR *look, float *fov,
                                   _D3DVECTOR *a, _D3DVECTOR *b, float *c);
    virtual void SetupCollisionObject(_D3DVECTOR position, SearchNode *node);
    virtual int ConstrainCamera(_D3DVECTOR *position, int a, int b);         // returns 1
    virtual float CompensateForLookPointObstacles(_D3DVECTOR look);   // returns 0.0f
    virtual void ComputeAngleXAndY(float dt);
    virtual _D3DVECTOR ComputeLookPoint(_D3DVECTOR position, _D3DVECTOR direction, _D3DVECTOR velocity,
                                        float a, float b, float angle, int reversing);
    virtual void ReadPresetInfoFromFile(ParameterBlock *block);

    float m_distance;                   // +0x04 (ComputePosition)
    float m_angleX;                     // +0x08
    float m_angleY;                     // +0x0C
    float m_fov;                        // +0x10, 80 from the ctor
    float m_angleXSpeed;                // +0x14
    float m_x18;                        // +0x18
    PhysicsBody *m_body;                // +0x1C (SetPhysicsBody)
    SoultreeObject *m_object;           // +0x20 (SetSoultreeObject)
    CollisionObject *m_collision;       // +0x24
    CameraFilter *m_lookFilters[3];     // +0x28, look point x, y, z
    CameraFilter *m_positionFilters[3]; // +0x34, position x, y, z
    CameraFilter *m_x40;                // +0x40
    CameraFilter *m_x44;                // +0x44
    CameraFilter *m_x48;                // +0x48
    CameraFilter *m_fovFilter;          // +0x4C
    CameraFilter *m_distanceFilter;     // +0x50
    char pad54[0x58 - 0x54];
    float m_settings[4];                // +0x58, distance, angle X, angle Y, height
    float m_savedSettings[4];           // +0x68 (ForcePreset)
    float m_reverseOffset[3];           // +0x78
    float m_fovRate;                    // +0x84
    float m_scaledB;                    // +0x88
    float m_a;                          // +0x8C
    char *m_presetFile;                 // +0x90
    char pad94[0xA0 - 0x94];
    _D3DVECTOR m_A0;                    // +0xA0
    _D3DVECTOR m_B0;                    // +0xB0
    _D3DVECTOR m_C0;                    // +0xC0
    _D3DVECTOR m_D0;                    // +0xD0
    _D3DVECTOR m_E0;                    // +0xE0
    _D3DVECTOR m_F0;                    // +0xF0
    int m_curPreset;                    // +0x100
    int m_numPresets;                   // +0x104
    CameraPreset *m_presets;            // +0x108
    int m_resetFilters;                 // +0x10C
    int m_x110;                         // +0x110
    int m_x114;                         // +0x114
    int m_presetSet;                    // +0x118 (Initialize stores its ViewPortCode here)
};

#endif
