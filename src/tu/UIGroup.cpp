// UIGroup.cpp  @ 0x003167B0 (text 0x3167B0-0x317AB0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// UIGroup and UIControl as one translation unit without a static
// initializer, between UICamera.cpp and UIManager.cpp. Its .rodata
// (0x431880-0x431918: @1096 "UIControl" .. the UIGroup RTTI) is one run of
// labels with a single BaseObject/GameObject RTTI copy for both classes. It
// has no ASSERT, so the file name is a guess (UIControl.cpp would fit the
// link order as well). UIControl::TurnOnOff and the UIWidget one-liners
// Transition and ProcessTransition are defined where retail placed them.



#include "UIGroup/UIGroup.h"
#include "UIControl/UIControl.h"
#include "UIWidget/UIWidget.h"

extern "C" void free(void *p);

// ---- src/UIGroup/ctor.cpp
UIGroup::UIGroup(int enabled)
    : GameObject(enabled)
{
    m_keepEnabled = 1;
    m_selectedControl = 0;
    m_others = 0;
    m_controls = 0;
    m_numControls = 0;
    m_numOthers = 0;
}


// ---- src/UIGroup/dtor.cpp


UIGroup::~UIGroup()
{
    if (m_controls) {
        free(m_controls);
        m_controls = 0;
    }
    if (m_others) {
        free(m_others);
        m_others = 0;
    }
}


// ---- src/UIGroup/EnableWidgets.cpp

void UIGroup::EnableWidgets()
{
    int i;
    for (i = 0; i < m_numControls; i++) {
        if (m_controls[i])
            m_controls[i]->EnableWidgets();
    }
    for (i = 0; i < m_numOthers; i++)
        m_others[i]->EnableWidgets();
}


// ---- src/UIGroup/DisableWidgets.cpp

void UIGroup::DisableWidgets()
{
    int i;
    for (i = 0; i < m_numControls; i++) {
        if (m_controls[i])
            m_controls[i]->DisableWidgets();
    }
    for (i = 0; i < m_numOthers; i++)
        m_others[i]->DisableWidgets();
}


// ---- src/UIGroup/Select.cpp


void UIGroup::Select()
{
    for (int i = 0; i < m_numOthers; i++)
        m_others[i]->Select();
}


// ---- src/UIGroup/DeSelect.cpp


void UIGroup::DeSelect()
{
    int i;
    for (i = 0; i < m_numOthers; i++) {
        if (m_others[i])
            m_others[i]->DeSelect();
    }
    if (!m_keepEnabled) {
        for (i = 0; i < m_numControls; i++) {
            if (m_controls[i])
                m_controls[i]->DeSelect();
        }
    } else {
        for (i = 0; i < m_numControls; i++) {
            if (m_controls[i])
                m_controls[i]->KeepEnabled();
        }
    }
}


// ---- src/UIGroup/SetFocus.cpp

void UIGroup::SetFocus(int control)
{
    if (m_numControls) {
        m_selected = control;
        m_selectedControl = m_controls[control - 1];
        for (int i = 0; i < m_numControls; i++) {
            if (m_controls[i])
                m_controls[i]->DeSelect();
        }
        if (m_selectedControl)
            m_selectedControl->Select();
    }
}


// ---- src/UIGroup/TurnOnOff.cpp

void UIGroup::TurnOnOff(int on)
{
    int i;
    m_on = on;
    for (i = 0; i < m_numControls; i++) {
        if (m_controls[i])
            m_controls[i]->TurnOnOff(on);
    }
    for (i = 0; i < m_numOthers; i++) {
        if (m_others[i])
            m_others[i]->TurnOnOff(on);
    }
}


// ---- src/UIGroup/Transition.cpp

void UIGroup::Transition(int direction)
{
    int i;
    for (i = 0; i < m_numControls; i++)
        m_controls[i]->Transition(direction);
    for (i = 0; i < m_numOthers; i++)
        m_others[i]->Transition(direction);
}


// ---- src/UIGroup/ProcessTransition.cpp


// Returns 1 once every control has finished its transition.
int UIGroup::ProcessTransition(float dt)
{
    int done = 1;
    int i;
    for (i = 0; i < m_numControls; i++) {
        if (!m_controls[i]->ProcessTransition(dt))
            done = 0;
    }
    for (i = 0; i < m_numOthers; i++) {
        if (!m_others[i]->ProcessTransition(dt))
            done = 0;
    }
    return done;
}


// ---- src/UIGroup/NextControl.cpp

// Steps to the next control that is on, wrapping past the end.
void UIGroup::NextControl(UIControl **selected)
{
    if (m_numControls) {
        m_selectedControl->DeSelect();
        do {
            m_selected++;
            if (m_selected > m_numControls)
                m_selected = 1;
            m_selectedControl = m_controls[m_selected - 1];
        } while (!m_selectedControl || !m_selectedControl->m_on);
        if (selected)
            *selected = m_selectedControl;
        else
            m_selectedControl->Select();
    }
}


// ---- src/UIGroup/PreviousControl.cpp

// Steps to the previous control that is on, wrapping past the start.
void UIGroup::PreviousControl(UIControl **selected)
{
    if (m_numControls) {
        m_selectedControl->DeSelect();
        do {
            m_selected--;
            if (m_selected <= 0)
                m_selected = m_numControls;
            m_selectedControl = m_controls[m_selected - 1];
        } while (!m_selectedControl || !m_selectedControl->m_on);
        if (selected)
            *selected = m_selectedControl;
        else
            m_selectedControl->Select();
    }
}


// ---- src/UIGroup/SelectControl.cpp


void UIGroup::SelectControl(int control)
{
    m_selectedControl->DeSelect();
    m_selected = control;
    m_selectedControl = m_controls[m_selected - 1];
    if (m_selectedControl && m_selectedControl->m_on)
        m_selectedControl->Select();
}


// ---- src/UIGroup/GridControlNext.cpp

// Moves right along a grid row with the given number of columns, wrapping
// within the row. If no other control in the row is on, searches the other
// columns for one, falling back to the starting control. (Retail reuses one
// local for the column and the next row's index, so the final test can
// compare a row index against the column.)
void UIGroup::GridControlNext(int columns)
{
    int start;
    int c;
    int column;
    int row;

    if (m_selectedControl)
        m_selectedControl->DeSelect();
    start = m_selected;
    do {
        m_selected++;
        if (m_selected % columns == 1)
            m_selected -= columns;
        m_selectedControl = m_controls[m_selected - 1];
    } while (!m_selectedControl || !m_selectedControl->m_on);

    if (m_selected == start) {
        column = m_selected % columns;
        do {
            m_selected++;
            c = m_selected % columns;
            if (c == 1)
                m_selected -= columns;
            if (c == column)
                break;
            row = m_selected;
            do {
                c = m_selected + columns;
                if (c > m_numControls)
                    m_selected = c - m_numControls;
                else
                    m_selected = c;
                m_selectedControl = m_controls[m_selected - 1];
            } while (row != m_selected && (!m_selectedControl || !m_selectedControl->m_on));
        } while (row == m_selected);
        if (c == column) {
            m_selected = start;
            m_selectedControl = m_controls[m_selected - 1];
        }
    }
    if (m_selectedControl)
        m_selectedControl->Select();
}


// ---- src/UIGroup/GridControlPrevious.cpp

// Moves left along a grid row with the given number of columns, wrapping
// within the row. If no other control in the row is on, searches the other
// columns for one, falling back to the starting control. (Retail reuses one
// local for the column and the next row's index, so the final test can
// compare a row index against the column.)
void UIGroup::GridControlPrevious(int columns)
{
    int start;
    int c;
    int column;
    int row;

    if (m_selectedControl)
        m_selectedControl->DeSelect();
    start = m_selected;
    do {
        m_selected--;
        if (m_selected % columns == 0)
            m_selected += columns;
        m_selectedControl = m_controls[m_selected - 1];
    } while (!m_selectedControl || !m_selectedControl->m_on);

    if (m_selected == start) {
        column = m_selected % columns;
        do {
            m_selected--;
            c = m_selected % columns;
            if (c == 0)
                m_selected += columns;
            if (c == column)
                break;
            row = m_selected;
            do {
                c = m_selected + columns;
                if (c > m_numControls)
                    m_selected = c - m_numControls;
                else
                    m_selected = c;
                m_selectedControl = m_controls[m_selected - 1];
            } while (row != m_selected && (!m_selectedControl || !m_selectedControl->m_on));
        } while (row == m_selected);
        if (c == column) {
            m_selected = start;
            m_selectedControl = m_controls[m_selected - 1];
        }
    }
    if (m_selectedControl)
        m_selectedControl->Select();
}


// ---- src/UIGroup/GridControlUp.cpp

// Moves up one row of a grid with the given number of columns, wrapping.
void UIGroup::GridControlUp(int columns)
{
    if (m_selectedControl)
        m_selectedControl->DeSelect();
    do {
        int selected = m_selected - columns;
        if (selected <= 0)
            m_selected = m_numControls + selected;
        else
            m_selected = selected;
        m_selectedControl = m_controls[m_selected - 1];
    } while (!m_selectedControl || !m_selectedControl->m_on);
    if (m_selectedControl)
        m_selectedControl->Select();
}


// ---- src/UIGroup/GridControlDown.cpp

// Moves down one row of a grid with the given number of columns, wrapping.
void UIGroup::GridControlDown(int columns)
{
    if (m_selectedControl)
        m_selectedControl->DeSelect();
    do {
        int selected = m_selected + columns;
        if (selected > m_numControls)
            m_selected = selected - m_numControls;
        else
            m_selected = selected;
        m_selectedControl = m_controls[m_selected - 1];
    } while (!m_selectedControl || !m_selectedControl->m_on);
    if (m_selectedControl)
        m_selectedControl->Select();
}


// ---- src/UIGroup/Initialize.cpp


void UIGroup::Initialize()
{
    int i;
    for (i = 0; i < m_numControls; i++) {
        if (m_controls[i])
            m_controls[i]->Initialize();
    }
    for (i = 0; i < m_numOthers; i++) {
        if (m_others[i])
            m_others[i]->Initialize();
    }
}


// ---- src/UIControl/ctor.cpp
UIControl::UIControl(int enabled)
    : GameObject(enabled)
{
    m_on = 0;
    m_numWidgets = 0;
    m_widgets = 0;
}


// ---- src/UIControl/dtor.cpp


UIControl::~UIControl()
{
    if (m_widgets)
        free(m_widgets);
}


// ---- src/UIControl/EnableWidgets.cpp


void UIControl::EnableWidgets()
{
    for (int i = 0; i < m_numWidgets; i++)
        m_widgets[i]->EnableAll();
}


// ---- src/UIControl/DisableWidgets.cpp


void UIControl::DisableWidgets()
{
    for (int i = 0; i < m_numWidgets; i++)
        m_widgets[i]->DisableAll();
}


// ---- src/UIControl/Select.cpp


void UIControl::Select()
{
    for (int i = 0; i < m_numWidgets; i++)
        m_widgets[i]->WidgetSelect();
}


// ---- src/UIControl/DeSelect.cpp

void UIControl::DeSelect()
{
    for (int i = 0; i < m_numWidgets; i++) {
        if (m_widgets[i])
            m_widgets[i]->WidgetDeSelect();
    }
}


// ---- src/UIControl/KeepEnabled.cpp


void UIControl::KeepEnabled()
{
    for (int i = 0; i < m_numWidgets; i++)
        m_widgets[i]->WidgetKeepEnabled();
}


// ---- src/PSXUIWaterEffect/UIControlTurnOnOff.cpp

void UIControl::TurnOnOff(int on) { m_on = on; }


// ---- src/UIControl/Transition.cpp


void UIControl::Transition(int type)
{
    for (int i = 0; i < m_numWidgets; i++)
        m_widgets[i]->Transition(type);
}


// ---- src/PSXUIWaterEffect/UIWidgetTransition.cpp

void UIWidget::Transition(int) {}


// ---- src/UIControl/ProcessTransition.cpp

int UIControl::ProcessTransition(float dt)
{
    int done = 1;
    for (int i = 0; i < m_numWidgets; i++) {
        if (!m_widgets[i]->ProcessTransition(dt))
            done = 0;
    }
    return done;
}


// ---- src/UIWidget/ProcessTransition.cpp

int UIWidget::ProcessTransition(float)
{
    return 1;
}


// ---- src/UIControl/Initialize.cpp

void UIControl::Initialize()
{
    for (int i = 0; i < m_numWidgets; i++) {
        if (m_widgets[i])
            m_widgets[i]->Initialize();
    }
}

