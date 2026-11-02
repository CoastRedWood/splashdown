// CameraController: a GameObject that drives a Camera through a list of
// CameraBehaviors (AddBehavior/SetBehavior/Tick). 0x40 bytes; the ctor
// zeroes +0x30..+0x3C, the dtor frees the behavior array.
#ifndef CAMERACONTROLLER_H
#define CAMERACONTROLLER_H

extern "C" void free(void *p);
void PSXprintf(const char *fmt, ...);

class Camera;
class CameraBehavior;

#include "GameObject/GameObject.h"

class CameraController : public GameObject {
public:
    CameraController(int enabled);
    CameraController *Create(Camera *camera);
    int AddBehavior(CameraBehavior *behavior);
    int SetBehavior(CameraBehavior *behavior);

    // --- virtuals in retail slot order ---
    virtual ~CameraController();
    virtual int Tick(float dt);

    int m_numBehaviors;                 // +0x30
    int m_currentBehavior;              // +0x34
    CameraBehavior **m_behaviors;       // +0x38 (realloc'd by AddBehavior)
    Camera *m_camera;                   // +0x3C
};

#endif
