// UIPage: one page of the front-end UI, loaded from a .dat file by Create
// (src/UIPage/Create.cpp). Data from the ctor and UIPage's own methods; the
// other names are the most common among the files that declared UIPage
// locally (mwcc's layout UIPage). 0x160 bytes; JetskiUIPage adds
// members from +0x160.
#ifndef UIPAGE_UIPAGE_H
#define UIPAGE_UIPAGE_H

#include "GameObject/GameObject.h"
#include "UIGroup/UIGroup.h"

class PSXMediaControl;
class ScreenTextManager;
class TextureMap;
class UIManager;
class UIWidget;
class UIWidgetEventHandler;

// Per-player selection state (m_states[2]).
struct UIPageState {
    int m_groupNumber;              // +0x0
    UIGroup *m_group;               // +0x4
};

class UIPage : public GameObject {
public:
    UIPage(int enabled);
    int SetBackground(char *name, int loop);
    void NextGroup(int state);
    void PreviousGroup(int state);
    void SelectGroup(int state, int newGroupNumber);
    void SetGroupFocus(int group, int focus);
    void SetPlayerFocus(int group, int control, int player);
    void EnableGroup(int group);
    void EnableGroupControl(int group, int control);
    void EnableGroupControlWidget(int group, int control, int widget);
    void DisableGroup(int group);
    void DisableGroupControl(int group, int control);
    void DisableGroupControlWidget(int group, int control, int widget);
    int TurnGroupOnOff(int on, int group);
    int TurnGroupControlOnOff(int on, int group, int control);
    int CreateVideoPlayer();
    int GetJoystickGroupID(int joystick);
    int GetGroupControlID(int group);
    UIWidget *GetWidgetPtr(int group, int control, int widget);

    // --- virtuals in retail slot order ---
    virtual ~UIPage();
    virtual int Tick(float dt);
    virtual int RenderPre3D();
    virtual void ResetPage();
    virtual int ProcessUserInput(int player, int input);
    virtual int Transition(int direction);
    virtual int ProcessTransition(float dt);
    virtual UIPage *Create(char *name, char *fileName, UIManager *manager, ScreenTextManager *textManager);
    virtual void Initialize();
    virtual void PageSelect();
    virtual void PageDeSelect();

    char m_name[0x130 - 0x30];              // +0x030
    UIManager *m_manager;                   // +0x130
    ScreenTextManager *m_textManager;       // +0x134
    UIWidgetEventHandler *m_eventHandler;   // +0x138
    int m_pageNo;                           // +0x13C
    int m_numStaticWidgets;                 // +0x140
    int m_numControls;                      // +0x144, highest index
    UIControl **m_controls;                 // +0x148
    int m_numGroups;                        // +0x14C, highest index
    UIGroup **m_groups;                     // +0x150
    UIPageState **m_states;                 // +0x154, one per player
    TextureMap *m_background;               // +0x158
    PSXMediaControl *m_videoPlayer;         // +0x15C
};

#endif
