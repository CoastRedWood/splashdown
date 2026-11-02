// UIControl: a GameObject holding a list of UIWidgets. Data from the ctor
// (src/UIControl/ctor.cpp) and UIControl's own methods; 0x40 bytes.
#ifndef UICONTROL_H
#define UICONTROL_H

#include "GameObject/GameObject.h"
#include "UIWidget/UIWidget.h"

class UIControl : public GameObject {
public:
    UIControl(int enabled);
    virtual ~UIControl();
    void EnableWidgets();
    void DisableWidgets();
    void Select();
    void DeSelect();
    void KeepEnabled();
    void TurnOnOff(int on);
    void Transition(int type);
    int ProcessTransition(float dt);
    void Initialize();

    int m_on;                   // +0x30 (TurnOnOff)
    int m_numWidgets;           // +0x34
    UIWidget **m_widgets;       // +0x38
};

#endif
