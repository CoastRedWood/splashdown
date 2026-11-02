// SimpleTVCamera: the trackside "TV" camera behaviour (a CameraBehavior,
// vptr at +0x00), choosing among camera crews. Its data ends at 0x54
// (the ctor's last store; JetskiTVCamera's starts there). Names follow its
// own methods (src/SimpleTVCamera/). +0x0C is the look height
// (ComputeLookPoint; SetPhysicsBody calls it m_a). +0x48 is the crew
// array (malloc'd, AddCameraCrew; the dtor frees it).
#ifndef SIMPLETVCAMERA_SIMPLETVCAMERA_H
#define SIMPLETVCAMERA_SIMPLETVCAMERA_H

#include "CameraBehavior/CameraBehavior.h"
#include "d3dtypes.h"

class PhysicsBody;
class SoultreeObject;

// One camera position around the course (0x30 bytes, AddCameraCrew).
struct CameraCrew {
    _D3DVECTOR m_position;              // +0x00
    float m_maxFOV;                     // +0x10
    float m_minFOV;                     // +0x14
    float m_farDistance;                // +0x18
    float m_nearDistance;               // +0x1C
    int m_x20;                          // +0x20
    char pad24[0x30 - 0x24];
};

class SimpleTVCamera : public CameraBehavior {
public:
    SimpleTVCamera();
    ~SimpleTVCamera();
    int AddCameraCrew(_D3DVECTOR *position, float maxFOV, float minFOV, float farDistance, float nearDistance,
                      int arg);
    void SetPhysicsBody(PhysicsBody *body, float a, float b);

    // --- virtuals in retail slot order ---
    virtual void SwitchOn(CameraStateParams params, int copyFilter);
    virtual CameraStateParams SwitchOff();
    virtual void ComputeCameraData(float dt, int &state, _D3DVECTOR *position, _D3DVECTOR *look, float *fov,
                                   _D3DVECTOR *a, _D3DVECTOR *b, float *c);
    virtual _D3DVECTOR ComputeLookPoint(_D3DVECTOR target);
    virtual _D3DVECTOR ComputePosition(float time, _D3DVECTOR target, _D3DVECTOR velocity);
    virtual float ComputeFOV(_D3DVECTOR target);
    virtual int GetTheBestCamera(float time, _D3DVECTOR *target, _D3DVECTOR *position);

    float m_fov;                        // +0x04, 60 from the ctor
    float m_scaledB;                    // +0x08 (SetPhysicsBody)
    float m_lookHeight;                 // +0x0C
    _D3DVECTOR m_position;              // +0x10
    PhysicsBody *m_body;                // +0x20
    SoultreeObject *m_object;           // +0x24
    char pad28[0x30 - 0x28];
    _D3DVECTOR m_lastTarget;            // +0x30
    int m_currentCrew;                  // +0x40, -1 if none
    int m_previousCrew;                 // +0x44
    CameraCrew *m_crews;                // +0x48
    int m_numCrews;                     // +0x4C, slots allocated
    int m_nextCrew;                     // +0x50, slots filled
};

#endif
