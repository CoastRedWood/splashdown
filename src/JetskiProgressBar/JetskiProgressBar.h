// JetskiProgressBar: the loading screen's progress bar, a texture blitted
// from m_srcRect into m_dstRect, whose top rises with m_progress (0..1).
// 0x58 bytes.
#ifndef JETSKIPROGRESSBAR_JETSKIPROGRESSBAR_H
#define JETSKIPROGRESSBAR_JETSKIPROGRESSBAR_H

#include "GameObject/GameObject.h"
#include "_RECT/_RECT.h"

class TextureMap;

class JetskiProgressBar : public GameObject {
public:
    JetskiProgressBar(int enabled);
    JetskiProgressBar *Create(char *textureName);
    int AddProgress(float amount);

    virtual ~JetskiProgressBar();

    TextureMap *m_texture;              // +0x30
    _RECT m_srcRect;                    // +0x34
    _RECT m_dstRect;                    // +0x44
    float m_progress;                   // +0x54
};

#endif
