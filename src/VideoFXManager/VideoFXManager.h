// VideoFXManager: per-viewport (4) full-screen video effects: colour
// overlays, blur and fades (0xC8 bytes).
#ifndef VIDEOFXMANAGER_H
#define VIDEOFXMANAGER_H

#include "GameObject/GameObject.h"

class VideoFXManager : public GameObject {
public:
    VideoFXManager(int enabled);
    int FadeIn(int duration, int viewport);
    int FadeOut(int duration, int viewport);

    // --- virtuals in retail slot order ---
    virtual ~VideoFXManager();
    virtual GameObject *Create();
    virtual int RenderPre3D();
    virtual int Render3D();

    unsigned char m_overlayR[4];        // +0x30
    unsigned char m_overlayG[4];        // +0x34
    unsigned char m_overlayB[4];        // +0x38
    unsigned char m_overlayA[4];        // +0x3C
    unsigned long m_overlayMode[4];     // +0x40 (GS ALPHA register value, 0x8000000041)
    int m_overlayState[4];              // +0x60
    int m_blurAmount[4];                // +0x70
    int m_blurState[4];                 // +0x80
    int m_duration[4];                  // +0x90 (fade time)
    int m_state[4];                     // +0xA0 (fade request: 1 out, 2 in, 3 fading out, ...)
    int m_dynamicViewportFade[4];       // +0xB0
    int m_dynamicScreenFade;            // +0xC0
};

#endif
