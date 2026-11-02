// JetskiTrainingInGameMenu: the training mode's pause menu, a JetskiMenu.
// 0x130 bytes (new in JetskiTraining's SetupOverlays); its data starts at
// JetskiMenu's 0xF8. The ctor clears +0xF8..+0x11C and sets the item and
// text counts per menu type (4, 6 and 6, 9); Create makes the text manager,
// the three ScreenText arrays and the three fonts.
#ifndef JETSKITRAININGINGAMEMENU_H
#define JETSKITRAININGINGAMEMENU_H

#include "JetskiMenu/JetskiMenu.h"

class ScreenText;
class ScreenTextFont;
class ScreenTextManager;

class JetskiTrainingInGameMenu : public JetskiMenu {
public:
    JetskiTrainingInGameMenu(int enabled);
    JetskiTrainingInGameMenu *Create(ScreenIconManager *icons, ScreenTextManager *texts, ViewPortCode viewport);
    void SelectText();
    void DeSelectText();

    // --- overrides in retail slot order ---
    virtual ~JetskiTrainingInGameMenu();
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
};

#endif  // JETSKITRAININGINGAMEMENU_H
