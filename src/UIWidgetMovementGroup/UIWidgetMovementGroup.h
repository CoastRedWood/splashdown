// UIWidgetMovementGroup: a UI widget that moves the selection between groups
// (UIWidgets.cpp); 0x98 bytes (`new` in UIManager::GetWidget).
#ifndef UIWIDGETMOVEMENTGROUP_UIWIDGETMOVEMENTGROUP_H
#define UIWIDGETMOVEMENTGROUP_UIWIDGETMOVEMENTGROUP_H

#include "UIWidget/UIWidget.h"

class UIWidgetMovementGroup : public UIWidget {
public:
    UIWidgetMovementGroup(UIManager *manager, int enabled);
    UIWidgetMovementGroup *Create(int id, ParameterBlock *block, UIManager *manager);
    void SelectionFocus(int direction, int player, int key);

    // --- virtuals in retail slot order ---
    virtual ~UIWidgetMovementGroup();
    virtual void WidgetSelect();
    virtual void WidgetDeSelect();
    virtual void WidgetKeyDownHandler(ProcessNode *node, KeyInfo *key, int player, int direction);

    int m_moves[16];                    // +0x58, per input: -1 next, -2 previous, > 0 group
};

#endif
