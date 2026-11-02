// JetskiMenu: base of the in-race menus (JetskiInGameMenu, JetskiNameMenu,
// the race-over/start menus...). Data from the ctor (src/JetskiMenu/ctor.cpp)
// and its own methods: ten arrays of ScreenIcons in two groups (+0x34..+0x44
// shown while m_x48 is set, +0x4C..+0x5C while m_x60 is), the viewport it
// switches to while open, and seven key-repeat timers. 0xF8 bytes.
#ifndef JETSKIMENU_JETSKIMENU_H
#define JETSKIMENU_JETSKIMENU_H

#include "GameObject/GameObject.h"
#include "d3dtypes.h"
#include "JetskiCompetition/JetskiCompetition.h"    // PlaybackType

class DmaRenderItem;
class ScreenIcon;
class ScreenIconManager;
#include "ViewPortCode/ViewPortCode.h"

struct MenuTimer {
    int m_active;                       // +0x0
    float m_time;                       // +0x4, counts down in Tick
};

class JetskiMenu : public GameObject {
public:
    JetskiMenu(int enabled);
    void SetViewport();
    void BuildMenuFull(int numItems);
    void BuildMenuSmall(int numItems);
    void RestoreViewport();
    void PreparePlayback(PlaybackType type);
    void PostTextPositioning();

    // --- virtuals in retail slot order ---
    virtual ~JetskiMenu();
    virtual int Tick(float dt);
    virtual int PrepareGeometry();
    virtual int Render3D();
    virtual int KeyDownHandler(ProcessNode *node, KeyInfo *key);
    virtual JetskiMenu *Create(ScreenIconManager *icons, ViewPortCode viewport);
    virtual void EnableMenu();
    virtual void DisableMenu();
    virtual void SetupInfo();

    ScreenIconManager *m_iconManager;   // +0x30, created by Create
    ScreenIcon **m_x34;                 // +0x34, 4 icons
    ScreenIcon **m_x38;                 // +0x38, 7
    ScreenIcon **m_x3C;                 // +0x3C, 7
    ScreenIcon **m_x40;                 // +0x40, 4
    ScreenIcon **m_x44;                 // +0x44, 1
    int m_x48;                          // +0x48, first group shown
    ScreenIcon **m_x4C;                 // +0x4C, 2
    ScreenIcon **m_x50;                 // +0x50, 7
    ScreenIcon **m_x54;                 // +0x54, 7
    ScreenIcon **m_x58;                 // +0x58, 6
    ScreenIcon **m_x5C;                 // +0x5C, 4 (arrows)
    int m_x60;                          // +0x60, second group shown
    ViewPortCode m_viewport;            // +0x64
    int m_x68;                          // +0x68
    int m_x6C;                          // +0x6C, first group's selection
    int m_x70;                          // +0x70, second group's selection
    int m_infoDirty;                    // +0x74, 1: SetupInfo is due
    int m_closing;                      // +0x78
    int m_closeAction;                  // +0x7C, -1 from the ctor
    DmaRenderItem *m_menuViewportItem;  // +0x80
    DmaRenderItem *m_viewportItem;      // +0x84
    _D3DVIEWPORT8 m_menuViewport;       // +0x88 (SetViewport: full screen)
    _D3DVIEWPORT8 m_savedViewport;      // +0xA0, the camera's
    int m_xB8;                          // +0xB8
    MenuTimer m_timers[7];              // +0xBC
};

#endif
