// JetskiTVCamera: Splashdown's TV camera (a SimpleTVCamera). 0x70 bytes
// (`new` in JetskiCameraController::Initialize); its own data starts at
// SimpleTVCamera's 0x54. +0x68 is LoadPositions' camera links (others call
// it m_neighbors/m_cameraIndices); +0x6C is "SwitchEvery" (SetPlayback
// calls it m_traceTime).
#ifndef JETSKITVCAMERA_JETSKITVCAMERA_H
#define JETSKITVCAMERA_JETSKITVCAMERA_H

#include "SimpleTVCamera/SimpleTVCamera.h"

// A crew's links along the course (LoadPositions: the next crew and the
// branch, -1 for none; GetTheBestCamera walks them as its neighbours).
struct CameraLink {
    int m_next;                         // +0x0
    int m_branch;                       // +0x4
};

// Cumulative percentages for the four camera kinds (TV, Follow, Side, LookBack).
struct CameraProbabilities {
    float p[4];
};

class JetskiTVCamera : public SimpleTVCamera {
public:
    JetskiTVCamera();
    ~JetskiTVCamera();
    int LoadPositions(char *directory);
    void GetProbabilities(int *out);
    void ResetTrace();

    // --- virtuals in retail slot order ---
    virtual void ComputeCameraData(float dt, int &state, _D3DVECTOR *position, _D3DVECTOR *look, float *fov,
                                   _D3DVECTOR *a, _D3DVECTOR *b, float *c);
    virtual _D3DVECTOR ComputePosition(float time, _D3DVECTOR target, _D3DVECTOR velocity);
    virtual int GetTheBestCamera(float time, _D3DVECTOR *target, _D3DVECTOR *position);

    char pad54[0x60 - 0x54];
    float m_switchDelay;                    // +0x60, 0.6 from ResetTrace
    CameraProbabilities *m_probabilities;   // +0x64, malloc'd
    CameraLink *m_links;                    // +0x68, malloc'd
    float m_switchEvery;                    // +0x6C
};

#endif
