// JetskiScreenLoadUI: the loading screen's background while the UI loads
// (JetskiLoadManager::Tick creates it). A GameObject, 0x3C bytes.
#ifndef JETSKISCREENLOADUI_H
#define JETSKISCREENLOADUI_H

#include "GameObject/GameObject.h"

class JetskiLoadManager;
class PSXTextureMap;

class JetskiScreenLoadUI : public GameObject {
public:
    JetskiScreenLoadUI(int enabled);
    JetskiScreenLoadUI *Create(JetskiLoadManager *manager);

    // --- virtuals in retail slot order ---
    virtual ~JetskiScreenLoadUI();
    virtual int Tick(float dt);
    virtual int RenderPre3D();

    JetskiLoadManager *m_loadManager;   // +0x30
    PSXTextureMap *m_background;        // +0x34
    int m_frames;                       // +0x38, frames shown
};

#endif
