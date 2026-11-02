// UIWidgetIcon: an icon widget (UIWidgets.cpp); 0x68 bytes (`new` in
// UIManager::GetWidget).
#ifndef UIWIDGETICON_UIWIDGETICON_H
#define UIWIDGETICON_UIWIDGETICON_H

#include "UIWidget/UIWidget.h"

class ScreenIcon;
class ScreenIconManager;

class UIWidgetIcon : public UIWidget {
public:
    UIWidgetIcon(UIManager *manager, int enabled);
    UIWidgetIcon *Create(int id, ParameterBlock *block, UIManager *manager);

    // --- virtuals in retail slot order ---
    virtual ~UIWidgetIcon();
    virtual int Tick(float dt);
    virtual int Render3D();
    virtual void WidgetSelect();
    virtual void WidgetDeSelect();

    ScreenIconManager *m_iconManager;   // +0x58
    ScreenIcon *m_icon;                 // +0x5C
    char pad60[0x68 - 0x60];
};

#endif
