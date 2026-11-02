// JetskiWidgetLap: the lap-count selector widget (a number changed with
// left/right), a UIWidget. 0x100 bytes (new in JetskiUIManager's widget
// factory); its data starts at UIWidget's 0x58. Members from Create's
// parameter keys and the ctor (clears +0x58..+0x68 and +0xE8).
#ifndef JETSKIWIDGETLAP_JETSKIWIDGETLAP_H
#define JETSKIWIDGETLAP_JETSKIWIDGETLAP_H

#include "UIWidget/UIWidget.h"

class ScreenText;
class ScreenTextFont;
class ScreenTextManager;

class JetskiWidgetLap : public UIWidget {
public:
    JetskiWidgetLap(UIManager *manager, int enabled);
    JetskiWidgetLap *Create(int id, ParameterBlock *params, ScreenTextManager *manager);

    // --- overrides in retail slot order ---
    virtual ~JetskiWidgetLap();
    virtual int Render3D();
    virtual void WidgetSelect();
    virtual void WidgetDeSelect();
    virtual void WidgetKeepEnabled();
    virtual void WidgetKeyDownHandler(ProcessNode *node, KeyInfo *key, int a, int direction);

    int m_lap;                          // +0x58
    ScreenTextManager *m_textManager;   // +0x5C
    ScreenText *m_text;                 // +0x60
    ScreenTextFont *m_font;             // +0x64
    char m_fontName[0x80];              // +0x68
    int m_xE8;                          // +0xE8
    int m_textX;                        // +0xEC
    int m_textY;                        // +0xF0
    unsigned int m_color;               // +0xF4
    unsigned int m_highlightColor;      // +0xF8
    int m_justification;                // +0xFC
};

#endif
