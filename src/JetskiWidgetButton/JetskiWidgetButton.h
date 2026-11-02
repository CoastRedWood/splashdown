// JetskiWidgetButton: a UI button (an icon from a texture rect plus a label),
// a UIWidget. 0x128 bytes (new in JetskiUIManager's widget factory); its data
// starts at UIWidget's 0x58. Members from Create's parameter keys (MapRect,
// Texture, FontName, XPosition, ..., Color). +0x80 and +0x124 are used by no
// matched code that names them (the label ScreenText is at +0x80).
#ifndef JETSKIWIDGETBUTTON_JETSKIWIDGETBUTTON_H
#define JETSKIWIDGETBUTTON_JETSKIWIDGETBUTTON_H

#include "UIWidget/UIWidget.h"
#include "_RECT/_RECT.h"

class ScreenIcon;
class ScreenIconManager;
class ScreenText;
class ScreenTextFont;
class ScreenTextManager;

class JetskiWidgetButton : public UIWidget {
public:
    JetskiWidgetButton(UIManager *manager, int enabled);
    JetskiWidgetButton *Create(int id, ParameterBlock *params, UIManager *manager,
                               ScreenTextManager *textManager);
    void SetColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a);

    // --- overrides in retail slot order ---
    virtual ~JetskiWidgetButton();
    virtual int Tick(float dt);
    virtual int Render3D();
    virtual void WidgetSelect();
    virtual void WidgetDeSelect();
    virtual int WidgetLocalize();

    ScreenIconManager *m_iconManager;   // +0x58
    ScreenIcon *m_icon;                 // +0x5C
    _RECT m_rect;                       // +0x60
    float m_selectTimer;                // +0x70
    int m_selected;                     // +0x74
    UIManager *m_manager;               // +0x78, hides UIWidget::m_manager
    ScreenTextManager *m_textManager;   // +0x7C
    ScreenText *m_text;                 // +0x80
    ScreenTextFont *m_font;             // +0x84
    int m_textID;                       // +0x88, "TextID"
    char m_fontName[0x80];              // +0x8C
    char pad10C[0x110 - 0x10C];
    int m_justification;                // +0x110
    unsigned int m_color;               // +0x114
    int m_textX;                        // +0x118
    int m_textY;                        // +0x11C
    float m_scale;                      // +0x120
    char pad124[0x128 - 0x124];
};

#endif
