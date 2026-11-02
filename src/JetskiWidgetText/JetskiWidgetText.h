// JetskiWidgetText: a UI text widget (one string, split into lines by
// ParseMultiLine), a UIWidget. 0x130 bytes (new in JetskiUIManager's widget
// factory); its data starts at UIWidget's 0x58. Members from Create (the
// parameter block's FontName, XPosition, ... Depth) and the ctor (clears
// +0x58..+0x64, +0xE8 and +0xF0..+0x118, +0x12C; scales 1).
#ifndef JETSKIWIDGETTEXT_JETSKIWIDGETTEXT_H
#define JETSKIWIDGETTEXT_JETSKIWIDGETTEXT_H

#include "UIWidget/UIWidget.h"

class ScreenText;
class ScreenTextFont;
class ScreenTextManager;

class JetskiWidgetText : public UIWidget {
public:
    JetskiWidgetText(UIManager *manager, int enabled);
    JetskiWidgetText *Create(int id, ParameterBlock *params, ScreenTextManager *manager);
    void ParseMultiLine(char *string, ScreenTextManager *manager);
    int RenderText();
    void SetColor(unsigned int color);

    // --- overrides in retail slot order ---
    virtual ~JetskiWidgetText();
    virtual int Render3D();
    virtual void WidgetSelect();
    virtual void WidgetDeSelect();
    virtual void WidgetKeepEnabled();
    virtual int WidgetLocalize();

    ScreenTextManager *m_textManager;   // +0x58
    ScreenText **m_lines;               // +0x5C
    ScreenTextFont *m_font;             // +0x60
    int m_stringID;                     // +0x64, "TextID"
    char m_fontName[0x80];              // +0x68
    int m_numLines;                     // +0xE8
    int m_justification;                // +0xEC
    unsigned int m_color;               // +0xF0
    int m_textX;                        // +0xF4
    int m_textY;                        // +0xF8
    int m_height;                       // +0xFC
    int m_maxChars;                     // +0x100
    float m_scale;                      // +0x104
    int m_hasHighlight;                 // +0x108
    unsigned int m_highlightColor;      // +0x10C
    int m_hasHighlightScale;            // +0x110
    float m_highlightScale;             // +0x114
    int m_x118;                         // +0x118
    int m_alphaEnabled;                 // +0x11C
    int m_alpha;                        // +0x120
    int m_depthEnabled;                 // +0x124
    float m_depth;                      // +0x128
    int m_x12C;                         // +0x12C
};

#endif
