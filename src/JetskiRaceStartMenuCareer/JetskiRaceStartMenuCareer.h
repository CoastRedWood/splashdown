// JetskiRaceStartMenuCareer: the career pre-race menu, a JetskiMenu. 0x128
// bytes (new in JetskiCompetition::LoadOverlays); its data starts at
// JetskiMenu's 0xF8. The ctor clears +0xF8..+0x120; Create makes the text
// manager, the ScreenTexts, a JetskiScreenText title and three fonts.
// +0x124 is used by no matched code.
#ifndef JETSKIRACESTARTMENUCAREER_H
#define JETSKIRACESTARTMENUCAREER_H

#include "JetskiMenu/JetskiMenu.h"

class JetskiScreenText;
class ScreenText;
class ScreenTextFont;
class ScreenTextManager;

class JetskiRaceStartMenuCareer : public JetskiMenu {
public:
    JetskiRaceStartMenuCareer(int enabled);
    JetskiRaceStartMenuCareer *Create(ScreenIconManager *icons, ScreenTextManager *texts, ViewPortCode viewport);

    // --- overrides in retail slot order ---
    virtual ~JetskiRaceStartMenuCareer();
    virtual int Tick(float dt);
    virtual int PrepareGeometry();
    virtual int Render3D();
    virtual int KeyDownHandler(ProcessNode *node, KeyInfo *key);
    virtual void EnableMenu();
    virtual void DisableMenu();
    virtual void SetupInfo();

    ScreenTextManager *m_textManager;   // +0xF8
    ScreenText **m_texts;               // +0xFC
    ScreenText *m_text100;              // +0x100
    ScreenText *m_text104;              // +0x104
    JetskiScreenText *m_title;          // +0x108
    ScreenText *m_text10C;              // +0x10C
    ScreenText *m_prompts[2];           // +0x110 (Create's loop)
    ScreenTextFont *m_font118;          // +0x118
    ScreenTextFont *m_font;             // +0x11C
    ScreenTextFont *m_font120;          // +0x120
    char pad124[0x128 - 0x124];
};

#endif
