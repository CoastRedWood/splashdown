// ScreenIconManager: queues ScreenIcons each frame (AddIcon, optionally as a
// copy in m_icons) and sends them as VU1 packets in Render3D. Data from the
// ctor (src/ScreenIconManager/ctor.cpp) and its own methods; 0x48 bytes.
#ifndef SCREENICONMANAGER_H
#define SCREENICONMANAGER_H

#include "GameObject/GameObject.h"

class DmaRenderItem;
class ScreenIcon;

class ScreenIconManager : public GameObject {
public:
    ScreenIconManager(int enabled);
    ScreenIconManager *Create(int maxIcons);
    void AddIcon(ScreenIcon *icon, int copy);
    void RenderQueuedIcons();
    void AddIconToRenderItem(DmaRenderItem *item, ScreenIcon *icon);
    void ComposeRenderItem(ScreenIcon *icon);

    // --- virtuals in retail slot order ---
    virtual ~ScreenIconManager();
    virtual int Render3D();

    int m_maxIcons;                 // +0x30, at most 0x80
    ScreenIcon **m_iconQueue;       // +0x34
    ScreenIcon *m_icons;            // +0x38, copies for queued clones
    int m_numQueued;                // +0x3C, cleared by Render3D
    int m_x40;                      // +0x40, 1 from the ctor: Render3D renders only if set
    DmaRenderItem *m_renderItems;   // +0x44
};

#endif
