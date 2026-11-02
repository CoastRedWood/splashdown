// UIGroup: a group of UI controls with one selected control (1-based index)
// and a second list of always-on controls. Data from the ctor
// (src/UIGroup/ctor.cpp) and UIGroup's own methods; 0x50 bytes.
#ifndef UIGROUP_H
#define UIGROUP_H

#include "GameObject/GameObject.h"
#include "UIControl/UIControl.h"

class UIGroup : public GameObject {
public:
    UIGroup(int enabled);
    virtual ~UIGroup();
    void EnableWidgets();
    void DisableWidgets();
    void Select();
    void DeSelect();
    void SetFocus(int control);
    void TurnOnOff(int on);
    void Transition(int direction);
    int ProcessTransition(float dt);
    void NextControl(UIControl **selected);
    void PreviousControl(UIControl **selected);
    void SelectControl(int control);
    void GridControlNext(int columns);
    void GridControlPrevious(int columns);
    void GridControlUp(int columns);
    void GridControlDown(int columns);
    void Initialize();

    int m_on;                       // +0x30 (TurnOnOff; read as "selectable")
    int m_numControls;              // +0x34
    UIControl **m_controls;         // +0x38
    int m_selected;                 // +0x3C (1-based)
    UIControl *m_selectedControl;   // +0x40
    int m_numOthers;                // +0x44
    UIControl **m_others;           // +0x48
    int m_keepEnabled;              // +0x4C, 1 from the ctor
};

#endif  // UIGROUP_H
