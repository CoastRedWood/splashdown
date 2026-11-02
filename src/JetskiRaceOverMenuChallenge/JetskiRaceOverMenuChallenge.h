// JetskiRaceOverMenuChallenge: the end-of-race menu of a challenge, a
// JetskiMenu. 0x120 bytes (new in JetskiCompetition); its data starts at
// JetskiMenu's 0xF8. Names come from the class's own methods.
#ifndef JETSKIRACEOVERMENUCHALLENGE_H
#define JETSKIRACEOVERMENUCHALLENGE_H

#include "JetskiMenu/JetskiMenu.h"

class JetskiScreenText;
class ScreenText;
class ScreenTextFont;
class ScreenTextManager;

class JetskiRaceOverMenuChallenge : public JetskiMenu {
public:
    JetskiRaceOverMenuChallenge(int id);
    JetskiRaceOverMenuChallenge *Create(ScreenIconManager *icons, ScreenTextManager *texts, ViewPortCode viewport);

    // --- overrides in retail slot order ---
    virtual ~JetskiRaceOverMenuChallenge();
    virtual int Tick(float dt);
    virtual int PrepareGeometry();
    virtual int Render3D();
    virtual int KeyDownHandler(ProcessNode *node, KeyInfo *key);
    virtual void EnableMenu();
    virtual void DisableMenu();
    virtual void SetupInfo();

    ScreenTextManager *m_textManager;   // +0xF8
    ScreenText **m_texts;               // +0xFC (malloc'd, 17 entries)
    JetskiScreenText **m_headers;       // +0x100 (malloc'd, 3 entries)
    ScreenTextFont *m_font;             // +0x104
    ScreenTextFont *m_titleFont;        // +0x108
    ScreenTextFont *m_smallFont;        // +0x10C
    int m_x110;                         // +0x110
    float m_idleTime;                   // +0x114
    float m_attractDelay;               // +0x118, 5 from the ctor
    char pad11C[0x120 - 0x11C];
};

#endif
