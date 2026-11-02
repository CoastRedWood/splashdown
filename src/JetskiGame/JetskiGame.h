// JetskiGame: Splashdown's game object (the global LpGame). Its own data
// starts after PSXGame's 0x274 bytes. Member names are the most common among
// the files that declared it (or Game) locally: mwcc's layout.
#ifndef JETSKIGAME_JETSKIGAME_H
#define JETSKIGAME_JETSKIGAME_H

#include "PSXGame/PSXGame.h"

class CheatFlags;
class GameText;
class JetskiCompetition;
class JetskiGameOptions;
class JetskiLoadManager;
class JetskiUIGlobalManager;
class JetskiUserOptions;
class MappableKeyLookupTable;
class SceneManager;
class ScreenTextFontItemList;

class JetskiGame : public PSXGame {
public:
    JetskiGame();
    int Initialize(char *cmdLine);
    int SetLanguage(int language);
    int StartUI();
    void EndUI();
    int StartEvent();
    void EndEvent();
    void DisplayLoadScreen();
    void SetupCompetitionDirectories();

    // --- virtuals in retail slot order ---
    virtual ~JetskiGame();
    virtual int PreGameInitialize();
    virtual int CreateScene();
    virtual int Tick();
    virtual int Terminate();

    char pad274[0x280 - 0x274];
    JetskiCompetition *m_competition;       // +0x280
    JetskiLoadManager *m_loadManager;       // +0x284
    JetskiUIGlobalManager *m_uiManager;     // +0x288
    JetskiGameOptions *m_gameOptions;       // +0x28C
    JetskiUserOptions *m_userOptions;       // +0x290
    GameText *m_text;                       // +0x294
    GameText *m_uiText;                     // +0x298
    CheatFlags *m_cheats;                   // +0x29C
    MappableKeyLookupTable *m_keyLookup;    // +0x2A0
    SceneManager *m_sceneManager;           // +0x2A4
    ScreenTextFontItemList *m_fonts;        // +0x2A8, a ScreenTextFontManager (its list is at +0)
};

#endif
