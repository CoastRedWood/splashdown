// CameraController.cpp  @ 0x0031E890 (text 0x31E890-0x31EC10)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// CameraController as one translation unit without a static initializer, at
// the end of UIPage.cpp's tu_map row: its .rodata restarts at @564 after the
// UIPage RTTI and ends with the CameraController RTTI.



#include "CameraController/CameraController.h"
#include "CameraBehavior/CameraBehavior.h"
#include "d3dtypes.h"
#include "GameObject/GameObject.h"
#include "Camera/Camera.h"

// ---- src/CameraController/ctor.cpp

CameraController::CameraController(int enabled)
    : GameObject(enabled)
{
    m_numBehaviors = 0;
    m_currentBehavior = 0;
    m_behaviors = 0;
    m_camera = 0;
}


// ---- src/CameraController/dtor.cpp

CameraController::~CameraController()
{
    if (m_behaviors)
        free(m_behaviors);
}


// ---- src/CameraController/Create.cpp

CameraController *CameraController::Create(Camera *camera)
{
    if (!GameObject::Create())
        return 0;
    if (camera) {
        m_camera = camera;
    } else {
        PSXprintf("Cannot create a CameraController without a camera!\n");
        Release();
        return 0;
    }
    return this;
}


// ---- src/CameraController/Tick.cpp
int CameraController::Tick(float dt)
{
    _D3DVECTOR position;
    _D3DVECTOR up;
    _D3DVECTOR lookAt;
    _D3DVECTOR target;
    float aspect;
    float fov;
    int useTarget = 1;
    m_camera->GetViewParameters(&position, &lookAt, &up, &fov, &aspect);
    m_behaviors[m_currentBehavior]->ComputeCameraData(dt, useTarget, &position, &target, &aspect, &lookAt, &up, &fov);
    if (useTarget) {
        m_camera->SetViewParameters(&position, 0, &up, &fov, &aspect);
        m_camera->LookAt(target);
    } else {
        m_camera->SetViewParameters(&position, &lookAt, &up, &fov, &aspect);
    }
    m_camera->Tick(0.0f);
    return GameObject::Tick(dt);
}


// ---- src/CameraController/AddBehavior.cpp
extern "C" void *realloc(void *p, unsigned int size);

int CameraController::AddBehavior(CameraBehavior *behavior)
{
    m_behaviors = (CameraBehavior **)realloc(m_behaviors, (m_numBehaviors + 1) * sizeof(CameraBehavior *));
    if (!m_behaviors)
        return 0;
    m_behaviors[m_numBehaviors] = behavior;
    m_numBehaviors++;
    return 1;
}


// ---- src/CameraController/SetBehavior.cpp

int CameraController::SetBehavior(CameraBehavior *behavior)
{
    for (int i = 0; i < m_numBehaviors; i++) {
        if (behavior == m_behaviors[i]) {
            m_currentBehavior = i;
            return 1;
        }
    }
    return 0;
}

