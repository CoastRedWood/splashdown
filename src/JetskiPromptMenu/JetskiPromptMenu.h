// JetskiPromptMenu: the in-race prompts (continue, career over, copycat
// turns), a JetskiMenu. 0x1F0 bytes (new in JetskiCompetition::LoadOverlays);
// its data starts at JetskiMenu's 0xF8. The ctor clears +0xF8..+0x110 and
// sets +0x114, +0x11C and +0x120; Create makes the text manager, the
// ScreenText and JetskiScreenText arrays and four fonts. SetCopyCatPrompt
// writes up to five stunt names from +0x124 (the fifth runs past 0x1F0).
#ifndef JETSKIPROMPTMENU_H
#define JETSKIPROMPTMENU_H

#include "JetskiMenu/JetskiMenu.h"

class JetskiScreenText;
class ScreenText;
class ScreenTextFont;
class ScreenTextManager;

class JetskiPromptMenu : public JetskiMenu {
public:
    JetskiPromptMenu(int enabled);
    JetskiPromptMenu *Create(ScreenIconManager *icons, ScreenTextManager *texts, ViewPortCode viewport, int a);
    void SetCareerOverPrompt(int mode);
    void SetCopyCatPrompt(int a, int player);

    // --- overrides in retail slot order ---
    virtual ~JetskiPromptMenu();
    virtual int Tick(float dt);
    virtual int PrepareGeometry();
    virtual int Render3D();
    virtual int KeyDownHandler(ProcessNode *node, KeyInfo *key);
    virtual void EnableMenu();
    virtual void DisableMenu();
    virtual void SetupInfo();

    ScreenTextManager *m_textManager;   // +0xF8
    ScreenText **m_texts;               // +0xFC
    JetskiScreenText **m_prompts;       // +0x100
    ScreenTextFont *m_font104;          // +0x104
    ScreenTextFont *m_font108;          // +0x108
    ScreenTextFont *m_font10C;          // +0x10C
    ScreenTextFont *m_font110;          // +0x110
    int m_promptType;                   // +0x114
    int m_controller;                   // +0x118, the player (SetCopyCatPrompt)
    int m_numTexts[2];                  // +0x11C
    char m_stunts[4][0x32];             // +0x124, SetCopyCatPrompt's stunt names (it fills [0..4])
    char pad1EC[0x1F0 - 0x1EC];
};

#endif  // JETSKIPROMPTMENU_H
