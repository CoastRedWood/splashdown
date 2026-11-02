// JetskiRaceOverMenuCountdown: the end-of-race results table with a
// countdown to the next race, a JetskiMenu. 0x138 bytes (new in the
// competitions' overlays); its data starts at JetskiMenu's 0xF8. The ctor
// clears +0xF8..+0x134 except +0x120 (5); Create makes the text manager,
// three fonts, the label/text arrays and two JetskiScreenTexts, and sets the
// table's column positions (+0x124..+0x134).
#ifndef JETSKIRACEOVERMENUCOUNTDOWN_H
#define JETSKIRACEOVERMENUCOUNTDOWN_H

#include "JetskiMenu/JetskiMenu.h"

class JetskiScreenText;
class ScreenText;
class ScreenTextFont;
class ScreenTextManager;

class JetskiRaceOverMenuCountdown : public JetskiMenu {
public:
    JetskiRaceOverMenuCountdown(int enabled);
    JetskiRaceOverMenuCountdown *Create(ScreenIconManager *icons, ScreenTextManager *texts, ViewPortCode viewport);
    void RenderLabels();
    void RenderText();

    // --- overrides in retail slot order ---
    virtual ~JetskiRaceOverMenuCountdown();
    virtual int Tick(float dt);
    virtual int PrepareGeometry();
    virtual int Render3D();
    virtual int KeyDownHandler(ProcessNode *node, KeyInfo *key);
    virtual void DisableMenu();
    virtual void SetupInfo();

    ScreenTextManager *m_textManager;   // +0xF8
    ScreenText **m_labels;              // +0xFC
    JetskiScreenText **m_bigTexts;      // +0x100, 2
    ScreenText **m_texts;               // +0x104
    ScreenText *m_text108;              // +0x108
    ScreenText *m_text10C;              // +0x10C
    ScreenTextFont *m_font110;          // +0x110
    ScreenTextFont *m_font114;          // +0x114
    ScreenTextFont *m_font118;          // +0x118
    int m_x11C;                         // +0x11C
    float m_x120;                       // +0x120, 5 from the ctor
    int m_columns;                      // +0x124
    int m_numTexts;                     // +0x128
    int m_x12C;                         // +0x12C
    int m_timeColumn;                   // +0x130
    int m_scoreColumn;                  // +0x134
};

#endif
