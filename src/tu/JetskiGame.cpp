// JetskiGame.cpp  @ 0x00253D00 (text 0x253D00-0x254D80)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's JetskiGame.cpp as one translation unit: its functions in
// retail order, then its static initializer. `make link` places the
// object's data as well as its code (tools/link_c.py). The old files'
// views map onto the shared headers: the "game data" at +0x28C is
// m_gameOptions (m_language, m_raceType), the one at +0x29C m_cheats
// (CheatFlags), the sound system at +0x0 Game::m_auralScape, and
// m_fonts is the ScreenTextFontManager.



#include "GameObject/GameObject.h"
#include "SceneManager/SceneManager.h"
#include "GameText/GameText.h"
#include "MotionLibrary/MotionLibrary.h"
#include "JetskiCareer/JetskiCareer.h"
#include "PSXGame/PSXGame.h"
#include "StringTable/StringTable.h"
#include "PSXVideoCard/PSXVideoCard.h"
#include "JoystickDevice/JoystickDevice.h"
#include "MappableKeyLookupTable/MappableKeyLookupTable.h"
#include "ControlInterface/ControlInterface.h"
#include "JetskiGameOptions/JetskiGameOptions.h"
#include "ScreenTextFontItemList/ScreenTextFontItemList.h"
#include "ScreenTextFontManager/ScreenTextFontManager.h"
#include "JetskiLoadManager/JetskiLoadManager.h"
#include "Game/Game.h"
#include "AuralScape/AuralScape.h"
#include "PSXSoundInterface/PSXSoundInterface.h"
#include "RainbowPS2Snd/RainbowPS2Snd.h"
#include "RenderTarget/RenderTarget.h"
#include "JetskiGame/JetskiGame.h"
#include "JetskiUserOptions/JetskiUserOptions.h"
#include "CheatFlags/CheatFlags.h"
#include "DataAccess/DataAccess.h"
#include "UIGlobalManager/UIGlobalManager.h"
#include "JetskiUIGlobalManager/JetskiUIGlobalManager.h"
#include "JetskiCompetition/JetskiCompetition.h"
#include "JetskiCareerCompetition/JetskiCareerCompetition.h"
#include "JetskiChallengeCompetition/JetskiChallengeCompetition.h"
#include "JetskiCircuitCompetition/JetskiCircuitCompetition.h"
#include "JetskiPracticeCompetition/JetskiPracticeCompetition.h"
#include "JetskiCopycatCompetition/JetskiCopycatCompetition.h"
#include "JetskiTimeElimCompetition/JetskiTimeElimCompetition.h"
#include "JetskiTimeTrialCompetition/JetskiTimeTrialCompetition.h"
#include "JetskiTraining/JetskiTraining.h"
// The axis vectors every Soultree TU defines (and __sinit constructs).
#include "sinit/AxisVectors.h"

extern "C" char *strcpy(char *dst, const char *src);
extern "C" int sprintf(char *buf, const char *fmt, ...);
void PSXprintf(const char *fmt, ...);

extern MotionLibrary *LpMotionLibrary;
extern StringTable *LpMaterialStringTable;
extern PSXVideoCard *LpVideoCard;
extern RenderTarget *LpRenderTarget;
extern DataAccess *LpDataAccess;
extern Game *LpGame;
extern char RootDirectory[0x104];
extern char LocalizationContentDirectory[0x104];
extern char AnimContentDirectory[0x104];
extern char AudioContentDirectory[0x104];
extern char DebugOverlayContentDirectory[0x104];
extern char EcoGeometryContentDirectory[0x104];
extern char EcoTextureContentDirectory[0x104];
extern char FontContentDirectory[0x104];
extern char ParticleTextureContentDirectory[0x104];
extern char SceneContentDirectory[0x104];
extern char SltGeometryContentDirectory[0x104];
extern char SltTextureContentDirectory[0x104];
extern char TerrainTextureContentDirectory[0x104];
extern char UITextureContentDirectory[0x104];

// ---- src/JetskiGame/ctor.cpp

JetskiGame::JetskiGame()
{
    m_keyLookup = 0;
    m_gameOptions = 0;
    m_text = 0;
    m_uiText = 0;
    m_userOptions = 0;
    m_sceneManager = 0;
    m_competition = 0;
    m_uiManager = 0;
    m_fonts = 0;
}


// ---- src/JetskiGame/dtor.cpp

JetskiGame::~JetskiGame()
{
    ScreenTextFontManager *fonts = (ScreenTextFontManager *)m_fonts;
    if (fonts) {
        delete fonts;
        m_fonts = 0;
    }
    if (m_sceneManager) {
        delete m_sceneManager;
        m_sceneManager = 0;
    }
    if (m_keyLookup) {
        delete m_keyLookup;
        m_keyLookup = 0;
    }
    if (m_gameOptions) {
        m_gameOptions->Release();
        m_gameOptions = 0;
    }
    if (m_text) {
        delete m_text;
        m_text = 0;
    }
    if (m_uiText) {
        delete m_uiText;
        m_uiText = 0;
    }
    if (m_cheats) {
        delete m_cheats;
        m_cheats = 0;
    }
    if (m_loadManager) {
        m_loadManager->Release();
        m_loadManager = 0;
    }
    if (m_userOptions) {
        m_userOptions->Release();
        m_userOptions = 0;
    }
    if (LpMotionLibrary) {
        delete LpMotionLibrary;
        LpMotionLibrary = 0;
    }
}


// ---- src/JetskiGame/Initialize.cpp

int JetskiGame::Initialize(char *cmdLine)
{
    static int event1 = -1;     // unused profiler event ID

    SetupCompetitionDirectories();
    LpMaterialStringTable->Load("AppStart\\MatStr.txt");
    m_gameOptions = new JetskiGameOptions;
    m_gameOptions->Create();
    int result = PSXGame::Initialize(cmdLine);
    LpVideoCard->EnableVSyncTo30Hz(1);
    delete m_controls->m_joysticks[2];
    m_controls->m_joysticks[2] = 0;
    if (m_controls->m_joysticks[3]) {
        delete m_controls->m_joysticks[3];
        m_controls->m_joysticks[3] = 0;
    }
    m_controls->ResetAllControllers(1);
    m_keyLookup = new MappableKeyLookupTable;
    if (!m_keyLookup)
        return 0;
    m_controls->AttachKeyLookupTable(m_keyLookup);
    for (int i = 0; i < 2; i++)
        m_gameOptions->ChangeControllerPreset(1, i);
    return result;
}


// ---- src/JetskiGame/CreateScene.cpp

int JetskiGame::CreateScene()
{
    int result = 1;
    m_sceneManager = new SceneManager;
    if (!GetRegistryBOOL("BackFaceCull", 1))
        LpRenderTarget->SetRenderState((_D3DRENDERSTATETYPE)0x16, 1, 0);   // D3DRS_CULLMODE
    LpGame->m_textureCache = 1;
    LpRenderTarget->m_hasDepth = 1;
    m_userOptions = new JetskiUserOptions;
    m_gameOptions->m_x5FC8 = 1;
    m_fonts = (ScreenTextFontItemList *)new ScreenTextFontManager;
    m_fonts = (ScreenTextFontItemList *)((ScreenTextFontManager *)m_fonts)->Create();
    m_loadManager = new JetskiLoadManager(1);
    m_loadManager->Create();
    m_cheats = new CheatFlags;
    if (!((PSXSoundInterface *)m_auralScape->m_soundInterface)->m_snd.LoadStreamLibrary("AppStart\\streams.juk")) {
#line 261 "JetskiGame.cpp"
        ASSERTF(0, ("Error loading stream library data file %s\n", "AppStart\\streams.juk"));
    }
    // A debug switch, always off in retail.
    int quickStart = 0;
    if (result) {
        if (quickStart) {
            if (m_gameOptions->m_raceType != 3)
                m_gameOptions->SetupAIRacers(m_gameOptions->m_numHumans);
            m_loadManager->SetStatus(4, 0, 0);
            SetLanguage(0);
        } else {
            SetLanguage(0);
            m_gameOptions->SetUI(2, 5);
            m_loadManager->SetStatus(1, 0, 0);
        }
    }
    return result;
}


// ---- src/JetskiGame/PreGameInitialize.cpp

int JetskiGame::PreGameInitialize()
{
    PSXGame::PreGameInitialize();
    return 1;
}


// ---- src/JetskiGame/SetLanguage.cpp

// Loads the game text and the localized font resource for `language`
// (negative: the current one).
int JetskiGame::SetLanguage(int language)
{
    char savedRoot[0x40];
    char path[0x110];
    char code[4];

    LpRenderTarget->TrySendRenderQueue(1);
    LpRenderTarget->WaitForRenderingToComplete();
    static int bFontsLoaded = 0;
    if (language < 0)
        language = m_gameOptions->m_language;
    m_gameOptions->m_language = language;
    strcpy(savedRoot, RootDirectory);
    strcpy(RootDirectory, "AppStart\\");
    if (!m_text) {
        m_text = new GameText;
        m_text->Create("IG", (Language)m_gameOptions->m_language);
    }
    if (!m_uiText) {
        m_uiText = new GameText;
        m_uiText->Create("UI", (Language)m_gameOptions->m_language);
    }

    const char *s;
    if (language == 1)
        s = "FRE";
    else if (language == 2)
        s = "GER";
    else if (language == 3)
        s = "ITA";
    else if (language == 4)
        s = "SPA";
    else if (language == 5)
        s = "BRI";
    else
        s = "ENG";
    strcpy(code, s);
    // The directory is chosen inside the call: retail's .rodata has the format
    // before the names.
    sprintf(path, "%s%s%s\\f_IG_%s.res", RootDirectory, LocalizationContentDirectory,
            language == 2 ? "German" : language == 3 ? "Italian" : language == 1 ? "French"
            : language == 4 ? "Spanish" : language == 5 ? "British" : "English", code);
    LpDataAccess->LoadResourceFile(path, 1, 0, 0, 0, 0, 0, 0);
    if (!bFontsLoaded) {
        char *fonts[6] = {"info_a", "title_b", "title_a", "info_b", "info_c", "45LCD"};
        ((ScreenTextFontManager *)m_fonts)->CreateFonts(6, fonts, 0);
        bFontsLoaded = 1;
    } else {
        ((ScreenTextFontManager *)m_fonts)->ReloadAllFonts();
    }
    LpDataAccess->DropDevice(path, 1);
    m_text->LoadGameText((Language)language);
    m_uiText->LoadGameText((Language)language);
    if (m_uiManager)
        m_uiManager->Localize(language);
    strcpy(RootDirectory, savedRoot);
    return 1;
}


// ---- src/JetskiGame/StartUI.cpp

int JetskiGame::StartUI()
{
    // Profiler event IDs, initialized but never read.
    static int event1 = -1;
    if (m_uiManager) {
        m_uiManager->RemoveUIManager();
        m_uiManager->CreateUIManager(m_gameOptions->m_uiMode, m_gameOptions->m_x28);
    } else {
        m_uiManager = new JetskiUIGlobalManager(1);
        m_uiManager->Create(m_gameOptions->m_uiMode, m_gameOptions->m_x28);
        m_root->AddChild(m_uiManager, -1);
    }
    m_controls->ResetAllControllers(1);
    static int event2 = -1;
    m_uiManager->AllocateIpuCache();
    return 1;
}


// ---- src/JetskiGame/EndUI.cpp

void JetskiGame::EndUI()
{
    if (m_uiManager) {
        m_uiManager->RemoveFromChain();
        m_uiManager->FreeIpuCache();
        m_uiManager->Release();
        m_uiManager = 0;
    }
}


// ---- src/JetskiGame/StartEvent.cpp

int JetskiGame::StartEvent()
{
    static int event1 = -1;             // profiler IDs, unused
    SetupCompetitionDirectories();
    DisplayLoadScreen();
    static int event2 = -1;
    switch (m_gameOptions->m_raceType) {
    case 6:
        m_competition = (JetskiCompetition *)(new JetskiCareerCompetition(1))->Create();
        break;
    case 7:
        m_competition = (JetskiCompetition *)(new JetskiChallengeCompetition(1))->Create();
        break;
    case 1:
        m_competition = (JetskiCompetition *)(new JetskiCircuitCompetition(1))->Create();
        break;
    case 2:
        m_competition = (JetskiCompetition *)(new JetskiPracticeCompetition(1))->Create();
        break;
    case 5:
        m_competition = (JetskiCompetition *)(new JetskiCopycatCompetition(1))->Create();
        break;
    case 4:
        m_competition = (JetskiCompetition *)(new JetskiTimeElimCompetition(1))->Create();
        break;
    case 3:
        m_competition = (JetskiCompetition *)(new JetskiTimeTrialCompetition(1))->Create();
        break;
    case 8:
        m_competition = (JetskiCompetition *)(new JetskiTraining(1))->Create();
        break;
    default:
#line 589 "JetskiGame.cpp"
        ASSERT(0, "StartEvent: Invalid CompetitionType");
        break;
    }
    if (!m_competition) {
        PSXprintf("Could not create competition object in JetskiGame::StartGame\n");
        return 0;
    }
    if (LpGame->m_auralScape)
        m_competition->InsertBefore(LpGame->m_auralScape, -1);
    else
        m_root->AddChild(m_competition, -1);
    m_controls->ResetAllControllers(1);
    static int event3 = -1;
    m_competition->AllocateIpuCache();
    return 1;
}


// ---- src/JetskiGame/EndEvent.cpp

void JetskiGame::EndEvent()
{
    if (m_competition) {
        m_competition->RemoveFromChain();
        m_competition->FreeIpuCache();
        m_competition->Release();
        m_competition = 0;
    }
}


// ---- src/JetskiGame/DisplayLoadScreen.cpp

void JetskiGame::DisplayLoadScreen() {}


// ---- src/JetskiGame/Terminate.cpp

int JetskiGame::Terminate()
{
    EndUI();
    EndEvent();
    m_fonts->Clear();
    return Game::Terminate();
}


// ---- src/JetskiGame/Tick.cpp

int JetskiGame::Tick()
{
    return Game::Tick();
}


// ---- src/JetskiGame/SetupCompetitionDirectories.cpp

void JetskiGame::SetupCompetitionDirectories()
{
    strcpy(RootDirectory, "\\");
    strcpy(AnimContentDirectory, "Anims\\");
    strcpy(AudioContentDirectory, "Audio\\");
    strcpy(DebugOverlayContentDirectory, "Debug\\");
    strcpy(EcoGeometryContentDirectory, "Scenes\\Eco\\");
    strcpy(EcoTextureContentDirectory, "Scenes\\Eco\\Textures\\");
    strcpy(FontContentDirectory, "Fonts\\");
    strcpy(ParticleTextureContentDirectory, "");
    strcpy(SceneContentDirectory, "Scenes\\");
    strcpy(SltGeometryContentDirectory, "Scenes\\Geometry\\");
    strcpy(SltTextureContentDirectory, "Scenes\\Geometry\\Textures\\");
    strcpy(TerrainTextureContentDirectory, "TerrTex\\");
    strcpy(UITextureContentDirectory, "UI\\Textures\\");
}


// ---- src/sinit/JetskiGame.cpp
// Static initializer of JetskiGame.cpp: only the axis vectors from the shared header.
