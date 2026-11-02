// UICamera: the front end's camera, a CutSceneCamera that only ticks and
// sets up its view while it is the render target's camera. Two vectors of
// its own from +0x390 (the ctor default-constructs them). Methods in
// src/tu/UICamera.cpp.
#ifndef UICAMERA_UICAMERA_H
#define UICAMERA_UICAMERA_H

#include "CutSceneCamera/CutSceneCamera.h"

class UICamera : public CutSceneCamera {
public:
    UICamera(int enabled);

    // --- overrides (retail vtable) ---
    virtual ~UICamera();
    virtual GameObject *Create();
    virtual int RenderPre3D();
    virtual int Tick(float dt);

    _D3DVECTOR m_x390;                  // +0x390
    _D3DVECTOR m_x3A0;                  // +0x3A0
    char pad3B0[0x3C0 - 0x3B0];         // retail news 0x3C0
};

#endif
