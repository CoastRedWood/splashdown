// PSXCamera: the PS2 Camera (GameObject + SearchableObject at +0x30, via
// Camera). 0x350 bytes (`new` in JetskiCompetition::LoadCameras); its own
// data starts at Camera's 0x250. Names follow its own methods
// (SetPerspectiveCamera, CalculateGuardBandFOV) and Camera/CameraVU0.h.
#ifndef PSXCAMERA_PSXCAMERA_H
#define PSXCAMERA_PSXCAMERA_H

#include "Camera/Camera.h"

class PSXCamera : public Camera {
public:
    PSXCamera(int enabled);
    void CalculateGuardBandFOV();
    void SetPerspectiveCamera(float fov, float nearZ, float farZ, float minZ, float maxZ, float centerX,
                              float centerY, float width, float height);

    // --- virtuals in retail slot order ---
    virtual ~PSXCamera() {}             // inline: CutSceneCamera's dtor inlines it
    virtual GameObject *Create();
    virtual int Tick(float dt);
    virtual void DumpSnapShot();
    virtual void UpdateMatrices();
    virtual void UpdateClipDistances(float nearZ, float farZ);

    _D3DMATRIX m_x250;                  // +0x250 = m_x140 * view-projection
    _D3DMATRIX m_guardBandProjection;   // +0x290
    _D3DMATRIX m_guardBandViewProjection; // +0x2D0
    float m_minZ;                       // +0x310
    float m_maxZ;                       // +0x314
    char pad318[0x328 - 0x318];
    float m_left;                       // +0x328
    float m_right;                      // +0x32C
    float m_top;                        // +0x330
    float m_bottom;                     // +0x334
    float m_x338;                       // +0x338, 1.0 from the ctor
    float m_x33C;                       // +0x33C, 1.0 from the ctor
    float m_guardBandFOV;               // +0x340
    char pad344[0x350 - 0x344];
};

#endif
