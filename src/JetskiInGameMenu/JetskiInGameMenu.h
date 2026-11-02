// JetskiInGameMenu: the race pause menu, a JetskiMenu laid out like
// JetskiTrainingInGameMenu. 0x138 bytes (new in the competitions'
// LoadOverlays); its data starts at JetskiMenu's 0xF8. The ctor clears
// +0xF8..+0x11C, sets the item and text counts per menu type (7, 4 and
// 12, 15), m_track -1 and m_musicChanged 0; Create makes the text manager,
// the three ScreenText arrays and the three fonts.
#ifndef JETSKIINGAMEMENU_H
#define JETSKIINGAMEMENU_H

#include "JetskiMenu/JetskiMenu.h"

class ScreenText;
class ScreenTextFont;
class ScreenTextManager;

class JetskiInGameMenu : public JetskiMenu {
public:
    JetskiInGameMenu(int enabled);
    JetskiInGameMenu *Create(ScreenIconManager *icons, ScreenTextManager *texts, ViewPortCode viewport);
    void SelectText();
    void DeSelectText();

    // --- overrides in retail slot order ---
    virtual ~JetskiInGameMenu();
    virtual int Tick(float dt);
    virtual int PrepareGeometry();
    virtual int Render3D();
    virtual int KeyDownHandler(ProcessNode *node, KeyInfo *key);
    virtual void EnableMenu();
    virtual void DisableMenu();
    virtual void SetupInfo();

    ScreenTextManager *m_textManager;   // +0xF8
    ScreenText **m_texts;               // +0xFC
    ScreenText **m_menuTexts;           // +0x100
    ScreenText **m_infoTexts;           // +0x104
    ScreenTextFont *m_font;             // +0x108
    ScreenTextFont *m_titleFont;        // +0x10C
    ScreenTextFont *m_headerFont;       // +0x110
    int m_selection;                    // +0x114
    int m_x118;                         // +0x118
    int m_x11C;                         // +0x11C
    int m_numItems[2];                  // +0x120, per menu type
    int m_numTexts[2];                  // +0x128, per menu type
    int m_track;                        // +0x130, -1 from the ctor (also the challenge level)
    int m_musicChanged;                 // +0x134
};

#endif  // JETSKIINGAMEMENU_H
