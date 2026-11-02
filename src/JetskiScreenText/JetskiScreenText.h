// JetskiScreenText: a localized string split into lines, one ScreenText per
// line (the menus' headers and prompts). 0x28 bytes (new in the menus'
// Create). The ctor clears +0x0/+0x20/+0x24 and runs the params' ctor at
// +0x4; SetText copies the params into each line's ScreenText (+0x54
// justification, +0x40 colour, +0x10/+0x14 position, line i at
// y + i * lineSpacing, +0x44 scale). No vtable.
#ifndef JETSKISCREENTEXT_JETSKISCREENTEXT_H
#define JETSKISCREENTEXT_JETSKISCREENTEXT_H

class ScreenText;
class ScreenTextFont;
class ScreenTextManager;

struct JetskiScreenTextParams {         // 0x1C bytes
    JetskiScreenTextParams();
    int m_justification;                // +0x00, ScreenTextManager::LeftJustification
    unsigned int m_color;               // +0x04, -1
    int m_x;                            // +0x08
    int m_y;                            // +0x0C
    int m_lineSpacing;                  // +0x10
    float m_scale;                      // +0x14, 1
    ScreenTextFont *m_font;             // +0x18
};

class JetskiScreenText {
public:
    JetskiScreenText();
    ~JetskiScreenText();
    void SetText(int id, int a);
    void AddText(ScreenTextManager *manager);

    ScreenText **m_texts;               // +0x00, realloc'd, one per line
    JetskiScreenTextParams m_params;    // +0x04
    int m_x20;                          // +0x20
    int m_numTexts;                     // +0x24
};

#endif
