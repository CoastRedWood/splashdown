// UIWidgetMovementControl: a UI widget that moves the selection between
// controls (UIWidgets.cpp); 0xA0 bytes (`new` in UIManager::GetWidget).
#ifndef UIWIDGETMOVEMENTCONTROL_H
#define UIWIDGETMOVEMENTCONTROL_H

#include "UIWidget/UIWidget.h"

class UIManager;

class UIWidgetMovementControl : public UIWidget {
public:
    UIWidgetMovementControl(UIManager *manager, int id);
    UIWidgetMovementControl *Create(int id, ParameterBlock *block, UIManager *manager);
    void SelectionFocus(int input, int group, int key);

    // --- virtuals in retail slot order ---
    virtual ~UIWidgetMovementControl();
    virtual void WidgetSelect();
    virtual void WidgetDeSelect();
    virtual void WidgetKeyDownHandler(ProcessNode *node, KeyInfo *key, int player, int input);

    int m_columns;                      // +0x58, "GridSize"
    int m_targets[16];                  // +0x5C, per input: control id or a move (< 0)
    char pad9C[0xA0 - 0x9C];
};

#endif
