// JetskiWidgetHighlight: a UI highlight (a selected and a deselected icon
// that flash when one of the mapped keys is pressed), a UIWidget. 0xE0 bytes
// (new in JetskiUIManager's widget factory); its data starts at UIWidget's
// 0x58. Members from Create's parameter keys (HighlightTime, MAPKEY_*,
// SelectTexture/DeselectTexture) and the ctor. +0xC0/+0xD0: the left and
// right ends of the deselect rect at its vertical middle (x, y), where
// WidgetKeyDownHandler pushes the UI water.
#ifndef JETSKIWIDGETHIGHLIGHT_JETSKIWIDGETHIGHLIGHT_H
#define JETSKIWIDGETHIGHLIGHT_JETSKIWIDGETHIGHLIGHT_H

#include "UIWidget/UIWidget.h"
#include "d3dtypes.h"

class ScreenIcon;
class ScreenIconManager;

class JetskiWidgetHighlight : public UIWidget {
public:
    JetskiWidgetHighlight(UIManager *manager, int enabled);
    JetskiWidgetHighlight *Create(int id, ParameterBlock *params, UIManager *manager);

    // --- overrides in retail slot order ---
    virtual ~JetskiWidgetHighlight();
    virtual int Tick(float dt);
    virtual int Render3D();
    virtual void WidgetSelect();
    virtual void WidgetDeSelect();
    virtual void WidgetKeyDownHandler(ProcessNode *node, KeyInfo *key, int a, int player);
    virtual void Initialize();

    ScreenIconManager *m_iconManager;   // +0x58
    ScreenIcon *m_icon;                 // +0x5C, the selected icon
    ScreenIcon *m_deselectIcon;         // +0x60
    int m_x64;                          // +0x64
    int m_flashing;                     // +0x68
    int m_keyMap[16];                   // +0x6C, "MAPKEY_LEFT" .. "MAPKEY_R3"
    float m_flashTime;                  // +0xAC, "HighlightTime"
    float m_timer;                      // +0xB0
    char padB4[0xC0 - 0xB4];
    _D3DVECTOR m_xC0;                   // +0xC0
    _D3DVECTOR m_xD0;                   // +0xD0
};

#endif
