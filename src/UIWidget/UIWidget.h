// UIWidget: base class of UI page widgets. Own virtual slots in retail order
// (retail vtable); data from the ctor
// (src/UIWidget/ctor.cpp), which zeroes +0x34..+0x54. Names follow
// UIWidget's own methods, otherwise the most common among the files that
// declared UIWidget locally (mwcc's layout UIWidget). 0x58 bytes.
#ifndef UIWIDGET_H
#define UIWIDGET_H

#include "GameObject/GameObject.h"

class ParameterBlock;
class UIManager;

class UIWidget : public GameObject {
public:
    UIWidget(UIManager *manager, int enabled);
    void TurnOnOff(int on);

    virtual ~UIWidget();
    virtual UIWidget *Create(ParameterBlock *params);
    virtual void WidgetSelect();
    virtual void WidgetDeSelect();
    virtual void WidgetDisable();
    virtual void WidgetKeepEnabled();
    virtual void WidgetKeyDownHandler(ProcessNode *node, KeyInfo *key, int a, int b);
    virtual void Initialize();
    virtual void Transition(int direction);
    virtual int ProcessTransition(float dt);
    virtual int WidgetLocalize();

    UIManager *m_manager;       // +0x30
    int m_id;                   // +0x34
    int m_selected;             // +0x38, also the TurnOnOff state
    int m_enableOnSelect;       // +0x3C
    int m_disableOnSelect;      // +0x40
    int m_enableOnDeSelect;     // +0x44
    int m_disableOnDeSelect;    // +0x48
    union {
        int m_wantsJoystickInput[2]; // +0x4C
        int m_joystickEvents[1];    // +0x4C; UIWidgetEventHandler::AddToJoystickEventList reads 8 (into the subclass)
    };
    int m_x54;                  // +0x54
};

#endif
