// JetskiLoadManager: the loading screen (background, progress bar, icon)
// shown between events (LpGame->m_loadManager). A GameObject, 0xD8 bytes
// (the ctor clears +0xB4..+0xD0). Names come from the class's own methods.
#ifndef JETSKILOADMANAGER_JETSKILOADMANAGER_H
#define JETSKILOADMANAGER_JETSKILOADMANAGER_H

#include "GameObject/GameObject.h"

class JetskiProgressBar;
class LoadingIcon;
class PSXTextureMap;
class UICamera;

class JetskiLoadManager : public GameObject {
public:
    JetskiLoadManager(int enabled);
    void SetStatus(int status, int progress, int total);
    void AddProgress(float amount);
    int LoadBackground(char *name, PSXTextureMap **texture);
    void CreateLoadBar(char *folder);
    void RemoveLoadBar();
    void CreateLoadIcon(char *directory);
    void RemoveLoadIcon();
    void RemoveLoad();

    // --- virtuals in retail slot order ---
    virtual ~JetskiLoadManager();
    virtual JetskiLoadManager *Create();
    virtual int Tick(float dt);

    char pad30[0xB0 - 0x30];
    UICamera *m_camera;                 // +0xB0
    GameObject *m_screen;               // +0xB4, language/legal/title/video (Tick)
    GameObject *m_loadScreen;           // +0xB8
    LoadingIcon *m_loadIcon;            // +0xBC
    JetskiProgressBar *m_loadBar;       // +0xC0
    int m_progress;                     // +0xC4
    int m_total;                        // +0xC8
    int m_status;                       // +0xCC
    int m_current;                      // +0xD0, what is showing (Tick)
};

#endif
