// JetskiUIManMain: the main front end's JetskiUIManager (the pages reach it
// through UIPage::m_manager). 0x310 bytes (new in
// JetskiUIGlobalManager::CreateUIManager); its data starts at
// JetskiUIManager's 0x2D0. The ctor clears most of +0x2D0..+0x2F8, copies
// two values of the current profile into +0x2E0/+0x2E4 and sets +0x2FC;
// the dtor frees the 11 head shots and 8 portraits and the cheat key.
// Names past +0x2E4 come from the pages' views of it (the copies disagree
// on +0x2E0..+0x2F0); nothing matched uses +0x304..+0x310.
#ifndef JETSKIUIMANMAIN_JETSKIUIMANMAIN_H
#define JETSKIUIMANMAIN_JETSKIUIMANMAIN_H

#include "JetskiUIManager/JetskiUIManager.h"

class JetskiCheatKey;
class JetskiUIRiderSoundEmitter;
class ScreenIcon;

class JetskiUIManMain : public JetskiUIManager {
public:
    JetskiUIManMain(int enabled);
    JetskiUIManMain *Create(int page, UIGlobalManager *global);
    void GetTeamColor(int team, unsigned int *color);

    // --- overrides in retail slot order ---
    virtual ~JetskiUIManMain();
    virtual void UIPrepareClose();
    virtual UIPage *GetPage(int number, char *file);
    virtual int Localize();

    JetskiCheatKey *m_cheatKey;                 // +0x2D0
    JetskiUIRiderSoundEmitter *m_riderSounds;   // +0x2D4
    ScreenIcon **m_headShots;                   // +0x2D8, 11
    ScreenIcon **m_portraits;                   // +0x2DC, 8
    int m_x2E0;                                 // +0x2E0
    int m_x2E4;                                 // +0x2E4
    int m_selectMode;                           // +0x2E8
    int m_numPlayers;                           // +0x2EC
    int m_mode;                                 // +0x2F0
    int m_x2F4;                                 // +0x2F4
    int m_x2F8;                                 // +0x2F8
    int m_x2FC;                                 // +0x2FC, 1 from the ctor
    int m_x300;                                 // +0x300
    char pad304[0x310 - 0x304];
};

#endif
