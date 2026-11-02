// JetskiRaceOverMenuCircuit: the end-of-race standings for a circuit, a
// JetskiMenu. 0x128 bytes (new in JetskiCompetition); its data starts at
// JetskiMenu's 0xF8. Names come from the class's own methods.
#ifndef JETSKIRACEOVERMENUCIRCUIT_H
#define JETSKIRACEOVERMENUCIRCUIT_H

#include "JetskiMenu/JetskiMenu.h"

class JetskiScreenText;
class ScreenText;
class ScreenTextFont;
class ScreenTextManager;

class JetskiRaceOverMenuCircuit : public JetskiMenu {
public:
    JetskiRaceOverMenuCircuit(int id);
    JetskiRaceOverMenuCircuit *Create(ScreenIconManager *icons, ScreenTextManager *texts, ViewPortCode viewport);
    void RenderLabels();
    void RenderText();

    // --- overrides in retail slot order ---
    virtual ~JetskiRaceOverMenuCircuit();
    virtual int Tick(float dt);
    virtual int PrepareGeometry();
    virtual int Render3D();
    virtual int KeyDownHandler(ProcessNode *node, KeyInfo *key);
    virtual void DisableMenu();
    virtual void SetupInfo();

    ScreenTextManager *m_textManager;   // +0xF8
    ScreenText **m_labels;              // +0xFC (malloc'd, 2 entries)
    ScreenText **m_texts;               // +0x100 (malloc'd, 20 entries: four columns of five)
    ScreenText *m_text104;              // +0x104
    ScreenText *m_text108;              // +0x108
    JetskiScreenText **m_bigTexts;      // +0x10C (malloc'd, 3 entries)
    ScreenTextFont *m_font110;          // +0x110
    ScreenTextFont *m_font114;          // +0x114
    ScreenTextFont *m_font118;          // +0x118
    float m_idleTime;                   // +0x11C
    float m_attractDelay;               // +0x120, 5 from the ctor
    char pad124[0x128 - 0x124];
};

#endif
