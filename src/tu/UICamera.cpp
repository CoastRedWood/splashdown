// UICamera.cpp  @ 0x00316640 (text 0x316640-0x3167B0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's UICamera.cpp as one translation unit: its five functions, then
// its static initializer. `make link` places the object's data as well as
// its code (tools/link_c.py). The UIGroup and UIControl functions after it
// in its tu_map row (0x3167B0-0x317AB0) are another TU without an
// initializer: its .rodata repeats the BaseObject/GameObject RTTI records
// once for both classes.


#include "CutSceneCamera/CutSceneCamera.h"
#include "UICamera/UICamera.h"
#include "GameObject/GameObjectGlobals.h"
// The axis vectors every Soultree TU defines (and __sinit constructs).
#include "sinit/AxisVectors.h"

// ---- src/UICamera/ctor.cpp

UICamera::UICamera(int enabled) : CutSceneCamera(enabled)
{
}


// ---- src/UICamera/dtor.cpp

UICamera::~UICamera()
{
}


// ---- src/UICamera/Create.cpp
// (Same as PSXCamera::Create.)

GameObject *UICamera::Create()
{
    if (!CutSceneCamera::Create())
        return 0;
    return this;
}


// ---- src/UICamera/RenderPre3D.cpp

int UICamera::RenderPre3D()
{
    if (LpRenderTarget->m_camera == this)
        Camera::RenderPre3D();
    return 1;
}


// ---- src/UICamera/Tick.cpp

int UICamera::Tick(float dt)
{
    if (LpRenderTarget->m_camera == this)
        CutSceneCamera::Tick(dt);
    return 1;
}


// ---- src/sinit/UICamera.cpp
// Static initializer of UICamera.cpp: only the axis vectors from the shared header.
