// DebugOverlay: the debug text pages (Game::m_debugOverlay), a GameObject:
// PrintfHeading/PrintfDetail test GameObject::m_enabled (+0x29 bit 0) and its
// data starts at +0x30. 64 text lines are formatted into m_buffer and drawn
// by RenderDelayed. Retail has no ctor or vtable for it (never created in
// this build), so its size isn't known; the last member used ends at 0x195C.
#ifndef DEBUGOVERLAY_DEBUGOVERLAY_H
#define DEBUGOVERLAY_DEBUGOVERLAY_H

#include "GameObject/GameObject.h"

class ScreenText;
class ScreenTextFont;
class ScreenTextManager;

struct DebugOverlayLine {               // 0x5C bytes
    float m_x;                          // +0x00
    float m_y;                          // +0x04
    char m_text[0x50];                  // +0x08
    int m_color;                        // +0x58, 1: red
};

class DebugOverlay : public GameObject {
public:
    void SetLine(int line, const char *text, int color);
    void PrintfHeading(int page, const char *fmt, ...);
    void PrintfDetail(int page, const char *fmt, ...);
    int RenderDelayed();

    char m_buffer[0x200];               // +0x30
    int m_line;                         // +0x230, last line used (-1: none)
    DebugOverlayLine m_lines[64];       // +0x234
    char pad1934[0x1940 - 0x1934];
    int m_numPages;                     // +0x1940, next page id to hand out
    int m_page;                         // +0x1944, the page shown
    int m_numLines;                     // +0x1948
    char pad194C[0x1950 - 0x194C];
    ScreenTextManager *m_textManager;   // +0x1950
    ScreenText *m_texts;                // +0x1954, one per line
    ScreenTextFont *m_font;             // +0x1958
};

#endif
