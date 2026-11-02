// JetskiNameMenu: the name-entry menu (record lists and the type writer), a
// JetskiMenu. Its own data starts at JetskiMenu's 0xF8 and ends at 0x148
// (new JetskiNameMenu in JetskiCompetition::LoadOverlays). Members from the
// ctor (src/JetskiNameMenu/ctor.cpp) and Create (unmatched): +0xF8 is the
// ScreenTextManager Create builds, +0xFC..+0x104 three fonts from
// ScreenTextFontItemList::GetScreenTextFont, +0x108..+0x110 malloc'd text
// arrays. The ctor clears +0x114/+0x120/+0x140 in an i < 2 loop.
#ifndef JETSKINAMEMENU_H
#define JETSKINAMEMENU_H

#include "JetskiMenu/JetskiMenu.h"

class ScreenText;
class ScreenTextFont;
class ScreenTextManager;

class JetskiNameMenu : public JetskiMenu {
public:
    JetskiNameMenu(int enabled);
    JetskiNameMenu *Create(ScreenIconManager *icons, ScreenTextManager *texts, ViewPortCode viewport);
    void Initialize(int mode);
    void InitializeListRecords();
    void InitializeTypeWriter();
    void KeyListRecords(ProcessNode *node, KeyInfo *key);
    void KeyTypeWriter(ProcessNode *node, KeyInfo *key);
    void RenderListRecords();
    void RenderTypeWriter();
    void TickListRecords(float dt);
    void TickTypeWriter(float dt);
    void SetupKeyBoard();
    void BuildRecordLists();
    void ProcessLetter();
    void ReplaceName();
    void SetSelectionWhite();
    void SetSelectionYellow();

    // --- overrides in retail slot order ---
    virtual ~JetskiNameMenu();
    virtual int Tick(float dt);
    virtual int PrepareGeometry();
    virtual int Render3D();
    virtual int KeyDownHandler(ProcessNode *node, KeyInfo *key);
    virtual void DisableMenu();
    virtual void SetupInfo();

    ScreenTextManager *m_textManager;   // +0xF8
    ScreenTextFont *m_font;             // +0xFC
    ScreenTextFont *m_smallFont;        // +0x100
    ScreenTextFont *m_titleFont;        // +0x104
    ScreenText **m_letters;             // +0x108, 78 keys
    ScreenText **m_texts;               // +0x10C, 6 record texts
    ScreenText **m_nameTexts;           // +0x110, 2
    ScreenText *m_text114;              // +0x114 (ctor: [2] with +0x118)
    ScreenText *m_text118;              // +0x118
    ScreenText *m_cursor;               // +0x11C
    ScreenText *m_text120;              // +0x120 (ctor: [2] with +0x124)
    ScreenText *m_text124;              // +0x124
    int m_cursorOn;                     // +0x128
    float m_cursorTimer;                // +0x12C
    int m_mode;                         // +0x130: 0 list records, 1 type writer
    int m_selection;                    // +0x134, 1-based
    int m_column;                       // +0x138
    int m_player;                       // +0x13C, 1-based (also called m_page)
    int m_recordCounts[2];              // +0x140, per player
};

#endif  // JETSKINAMEMENU_H
