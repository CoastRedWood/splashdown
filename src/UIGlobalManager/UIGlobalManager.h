// UIGlobalManager: owns the front end's ScreenTextManager, the global
// widgets and the current UIManager (JetskiUIGlobalManager creates it).
// 0x40 bytes (new JetskiUIGlobalManager adds no data); the ctor clears
// +0x30..+0x3C. Slot 28 is pure here (JetskiUIGlobalManager::CreateUIManager).
#ifndef UIGLOBALMANAGER_UIGLOBALMANAGER_H
#define UIGLOBALMANAGER_UIGLOBALMANAGER_H

#include "GameObject/GameObject.h"

class ParameterBlock;
class ScreenTextManager;
class UIManager;

class UIGlobalManager : public GameObject {
public:
    UIGlobalManager(int enabled);
    int LoadScreenTextManager();
    int LoadGlobalWidgets();
    GameObject *GetWidget(ParameterBlock *params, char *name);
    int Localize(int language);

    // --- overrides and own virtuals in retail slot order ---
    virtual ~UIGlobalManager();
    virtual UIGlobalManager *Create(int mode, int a2, int language);
    virtual int CreateUIManager(int mode, int a2) = 0;
    virtual void RemoveUIManager();

    ScreenTextManager *m_screenTextManager; // +0x30
    UIManager *m_uiManager;             // +0x34
    int m_numWidgets;                   // +0x38
    GameObject **m_widgets;             // +0x3C, from GetWidget
};

#endif
