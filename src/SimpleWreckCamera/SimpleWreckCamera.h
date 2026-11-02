// SimpleWreckCamera: the camera behaviour that frames a wreck (a
// CameraBehavior, vptr at +0x00). Its data ends at 0x32C (JetskiWreckCamera
// derives from it). Names follow its own methods (src/SimpleWreckCamera/).
// Where copies disagree:
//   +0x20 is m_lookPoint (ComputeLookPoint); BeginWreck reads it as 3 floats.
//   +0x38..+0x50 are 7 CameraFilter pointers (ctor/CopyFilterData: m_x38..,
//         BeginWreck: float *); JetskiWreckCamera's copy calls +0x48 a
//         WreckCameraTarget, but it sets +0x04 (CameraFilter::m_rate).
//   +0x58 m_distances[10][10]: ComputeLookPoint/ComputePosition call
//         [0][1] m_x5C.
//   +0x200 m_positions[10]; BeginWreck/ComputePosition use an 11-element
//         m_targetPositions whose last entry is m_direction (+0x2B0).
//   +0x2D0..+0x2EF is Initialize's WreckCameraParams (8 floats, memcpy'd);
//         the others name its floats, so they are declared one by one here.
#ifndef SIMPLEWRECKCAMERA_SIMPLEWRECKCAMERA_H
#define SIMPLEWRECKCAMERA_SIMPLEWRECKCAMERA_H

#include "CameraBehavior/CameraBehavior.h"
#include "d3dtypes.h"

class CameraFilter;
class PhysicsBody;
class SoultreeObject;
struct WreckCameraParams;

class SimpleWreckCamera : public CameraBehavior {
public:
    SimpleWreckCamera();
    ~SimpleWreckCamera();
    int Initialize(float fovRate, WreckCameraParams params, float lookRate, float positionRate, float x54);
    void AddPhysicsBody(PhysicsBody *body);
    int RemoveAllPhysicsBodies();
    int RemoveAllSoultreeObjects();
    void ComputeDistancesBetweenObjects();
    float ComputeFOV();
    _D3DVECTOR ComputeLookPoint(_D3DVECTOR position, _D3DVECTOR velocity, float dt, int moving);
    void ComputeNetPositionAndVelocity(float dt, _D3DVECTOR *position, _D3DVECTOR *velocity);
    _D3DVECTOR ComputePosition(_D3DVECTOR direction, _D3DVECTOR target, _D3DVECTOR, _D3DVECTOR);
    int CopyFilterData(CameraStateParams params);
    void FilterCameraParameters(_D3DVECTOR *position, _D3DVECTOR *look, float *fov, float dt);
    void ResetLookFilters(_D3DVECTOR &v);
    void ResetPositionFilters(_D3DVECTOR &v);
    void RotateAngleXAndY();
    void UpdateOffsets();

    // --- virtuals in retail slot order ---
    virtual void SwitchOn(CameraStateParams params, int copyFilter);
    virtual CameraStateParams SwitchOff();
    virtual void ComputeCameraData(float dt, int &state, _D3DVECTOR *position, _D3DVECTOR *look, float *fov,
                                   _D3DVECTOR *a, _D3DVECTOR *b, float *c);
    virtual void BeginWreck(CameraStateParams params);
    virtual int ConstrainCamera(_D3DVECTOR *position, int a);

    float m_distance;                   // +0x04
    float m_x08;                        // +0x08
    float m_x0C;                        // +0x0C
    float m_fov;                        // +0x10, 80 from the ctor
    int m_watchingBodies;               // +0x14, 1: m_bodies, 0: m_objects
    char pad18[0x20 - 0x18];
    _D3DVECTOR m_lookPoint;             // +0x20
    PhysicsBody **m_bodies;             // +0x30, malloc'd
    SoultreeObject **m_objects;         // +0x34, malloc'd
    CameraFilter *m_lookFilters[3];     // +0x38, look point x, y, z
    CameraFilter *m_positionFilters[3]; // +0x44, position x, y, z
    CameraFilter *m_fovFilter;          // +0x50
    float m_x54;                        // +0x54
    float m_distances[10][10];          // +0x58 (ComputeDistancesBetweenObjects)
    float m_distanceZoom;               // +0x1E8
    float m_zoom;                       // +0x1EC
    float m_x1F0;                       // +0x1F0
    char pad1F4[0x200 - 0x1F4];
    _D3DVECTOR m_positions[10];         // +0x200
    _D3DVECTOR **m_lastPositions;       // +0x2A0, malloc'd
    char pad2A4[0x2B0 - 0x2A4];
    _D3DVECTOR m_direction;             // +0x2B0
    _D3DVECTOR m_x2C0;                  // +0x2C0
    float m_x2D0;                       // +0x2D0, WreckCameraParams from here
    float m_x2D4;                       // +0x2D4
    float m_x2D8;                       // +0x2D8
    float m_x2DC;                       // +0x2DC
    float m_minDistance;                // +0x2E0
    float m_maxDistance;                // +0x2E4
    float m_minFOV;                     // +0x2E8
    float m_maxFOV;                     // +0x2EC
    int m_numBodies;                    // +0x2F0
    int m_numTargets;                   // +0x2F4
    int m_resetFilters;                 // +0x2F8
    int m_copyFilter;                   // +0x2FC
    int m_onlyFirst;                    // +0x300
    int m_ignore[10];                   // +0x304
};

#endif
