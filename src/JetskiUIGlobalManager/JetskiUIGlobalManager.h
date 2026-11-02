// JetskiUIGlobalManager: Splashdown's UIGlobalManager (the global
// LpUIGlobalManager); it picks the front end's UIManager (JetskiUIManMain or
// JetskiUIManCard) and the content directories.
#ifndef JETSKIUIGLOBALMANAGER_H
#define JETSKIUIGLOBALMANAGER_H

#include "GameObject/GameObject.h"

#include "UIGlobalManager/UIGlobalManager.h"

// 0x40 bytes: no data beyond UIGlobalManager's.
class JetskiUIGlobalManager : public UIGlobalManager {
public:
    JetskiUIGlobalManager(int enabled);
    UIGlobalManager *Create(int mode, int a2);
    void InitializeDirectories(int mode);
    void AllocateIpuCache();
    void FreeIpuCache();

    // --- overrides in retail slot order ---
    virtual ~JetskiUIGlobalManager();
    virtual int CreateUIManager(int mode, int a2);
};

#endif
