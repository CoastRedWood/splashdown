// CutSceneCamera: a PSXCamera that plays back recorded camera frames.
// Data from +0x350 to +0x38C (0x390 bytes). Its dtor's file inlines
// ~PSXCamera, so that file keeps its own copies of the classes.
#ifndef CUTSCENECAMERA_CUTSCENECAMERA_H
#define CUTSCENECAMERA_CUTSCENECAMERA_H

#include "PSXCamera/PSXCamera.h"

class CutSceneCamera : public PSXCamera {
public:
    CutSceneCamera(int enabled);
    void ReleaseMemory();

    // --- virtuals in retail slot order ---
    virtual ~CutSceneCamera();
    virtual GameObject *Create();
    virtual int Tick(float dt);

    float m_frameRate;                  // +0x350
    char pad354[0x358 - 0x354];
    unsigned long m_numFrames;          // +0x358 (64-bit)
    float m_time;                       // +0x360
    int m_playing;                      // +0x364
    int m_loaded;                       // +0x368 (the dtor calls ReleaseMemory when set)
    int m_direction;                    // +0x36C
    int m_lastFrame;                    // +0x370
    int m_firstFrame;                   // +0x374
    _D3DMATRIX *m_matrices;             // +0x378
    float *m_fovs;                      // +0x37C
    void *m_380;                        // +0x380
    void *m_384;                        // +0x384
    void *m_388;                        // +0x388
};

#endif
