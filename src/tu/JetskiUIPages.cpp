// JetskiUIPages.cpp  @ 0x003551C0 (text 0x3551C0-0x3690F0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// The JetskiUIPage base and its pages (JetskiUIPageCareer .. JetskiUIPageCenterScreen)
// as one translation unit (its static initializer only builds the axis vectors),
// unanchored inside JetskiPracticeCompetition.cpp's tu_map row: its .rodata runs from
// @987 to @6183 plus the JetskiUIPage RTTI block, after the FileIO.cpp strings. The
// page classes are in JetskiUIPage/JetskiUIPages.h. The local views left here
// (JetskiGameOptions and JetskiPageSettings views, the sound-group stand-ins)
// reproduce retail's access patterns where the shared headers type the same memory
// differently.



#include "GameObject/GameObjectGlobals.h"
#include "JetskiUIPage/JetskiUIPage.h"
#include "JetskiUIPage/JetskiUIPages.h"
#include "ScreenTextFont/ScreenTextFont.h"
#include "LightData/LightData.h"
#include "PSXUIWaterEffect/PSXUIWaterEffect.h"
#include "JetskiPageSettings/JetskiPageSettings.h"
#include "PSXSoundGroup/PSXSoundGroup.h"
#include "UIWidget/UIWidget.h"
#include "ControlInterface/ControlInterface.h"
// The sound groups (JetskiUIManager::m_musicGroup, ...) as these pages call them: retail
// passes an int, where PSXSoundGroup::SetVolume takes a long. Read through a reference:
// a plain pointer cast loads it before LpGame.
class MusicPlayer {
public:
    char pad0[0x8];
    virtual ~MusicPlayer();
    virtual void Slot1();
    virtual void Slot2();
    virtual void Slot3();
    virtual void Slot4();
    virtual void Slot5();
    virtual void Slot6();
    virtual void Slot7();
    virtual void Slot8();
    virtual void Slot9();
    virtual void Slot10();
    virtual void Slot11();
    virtual void Slot12();
    virtual void Slot13();
    virtual void Slot14();
    virtual void Slot15();
    virtual void Slot16();
    virtual void Slot17();
    virtual void Slot18();
    virtual void Slot19();
    virtual void Slot20();
    virtual void Slot21();
    virtual void Slot22();
    virtual void Slot23();
    virtual void Slot24();
    virtual void Slot25();
    virtual void Slot26();
    virtual void Slot27();
    virtual void Slot28();
    virtual void Slot29();
    virtual void SetVolume(int volume); // slot 30
};
// JetskiUIManager::m_heliSound as SLevels pans it: retail passes an int, where
// PSXSound::SetPan takes a long.
class PannedSound {
public:
    char pad0[0x8];
    virtual ~PannedSound();
    virtual void Slot1();
    virtual void Slot2();
    virtual void Slot3();
    virtual void Slot4();
    virtual void Slot5();
    virtual void Slot6();
    virtual void Slot7();
    virtual void Slot8();
    virtual void Slot9();
    virtual void SetPan(int pan); // slot 10
};
struct ModeSelection {
    int m_event;                        // +0x0
    int m_group3;                       // +0x4
    int m_group5;                       // +0x8
    int m_track;                        // +0xC
};
#include "JoystickDevice/JoystickDevice.h"
#include "JetskiWidgetLap/JetskiWidgetLap.h"
#include "JetskiPageSettingsMain/JetskiPageSettingsMain.h"
#include "JetskiPageSettingsOptions/JetskiPageSettingsOptions.h"
#include "JetskiPageSettingsGOptions/JetskiPageSettingsGOptions.h"
#include "JetskiPageSettingsVideos/JetskiPageSettingsVideos.h"
#include "JetskiPageSettingsCharSelect/JetskiPageSettingsCharSelect.h"
#include "JetskiPageSettingsSetup/JetskiPageSettingsSetup.h"
#include "JetskiPageSettingsLoadSave/JetskiPageSettingsLoadSave.h"
#include "ResponseEmitter/ResponseEmitter.h"
#include "ScreenText/ScreenText.h"
#include "RenderTarget/D3DEnums.h"
#include "ScreenTextManager/ScreenTextManager.h"
#include "ScreenTextFontItemList/ScreenTextFontItem.h"
#include "ScreenTextFontItemList/ScreenTextFontItemList.h"
#include "JetskiGame/JetskiGame.h"
#include "GameObject/GameObject.h"
#include "_RECT/_RECT.h"
#include "RenderTarget/RenderTarget.h"
#include "PSXMediaControl/PSXMediaControl.h"
#include "JetskiGameOptions/JetskiGameOptions.h"
#include "soultree_assert_block.h"
#include "JetskiCareer/JetskiCareer.h"
#include "CareerRacer/CareerRacer.h"
#include "JetskiUIManager/JetskiUIManager.h"
#include "MediaControl/MediaControl.h"
#include "DataAccess/DataAccess.h"
#include "ScreenIcon/ScreenIcon.h"
#include "ScreenIconManager/ScreenIconManager.h"
#include "MotionLibrary/MotionLibrary.h"
#include "JetskiWidgetCharacter/JetskiWidgetCharacter.h"
#include "JetskiUIManMain/JetskiUIManMain.h"
#include "UIManager/UIManager.h"
#include "JetskiWidgetSeadoo/JetskiWidgetSeadoo.h"
#include "LightEmitter/_D3DCOLORVALUE.h"
#include "d3dtypes.h"
#include "LightEmitter/LightEmitter.h"
#include "UIGroup/UIGroup.h"
#include "JetskiUIRiderSoundEmitter/JetskiUIRiderSoundEmitter.h"
#include "Character/Character.h"
#include "PSXTextureMap/PSXTextureMap.h"
#include "UIPage/UIPage.h"
#include "GameText/GameText.h"
#include "LocalizedString/LocalizedString.h"
#include "UIControl/UIControl.h"
#include "JetskiWidgetButton/JetskiWidgetButton.h"
#include "KeyInfo/KeyInfo.h"
#include "JoystickDevice/JoystickDevice.h"
#include "JetskiCheatKey/JetskiCheatKey.h"
#include "PSXSound/PSXSound.h"
#include "TextureMap/TextureMap.h"
#include "PSXRenderTarget/PSXRenderTarget.h"
#include "AuralScape/AuralScape.h"
#include "ScreenPrimitive/ScreenPrimitive.h"
#include "ScreenPrimitiveVertex/ScreenPrimitiveVertex.h"
#include "ScreenPrimitiveManager/ScreenPrimitiveManager.h"
#include "JetskiWidgetText/JetskiWidgetText.h"
#include "Sound/Sound.h"
#include "PSXVideoCard/PSXVideoCard.h"
#include "sinit/AxisVectors.h"

extern JetskiGame *LpGame;

// MediaControl setters as these pages inline them (the position pair through one load of the player).
static inline void SetVideoPosition(PSXMediaControl *video, int x, int y)
{
    video->m_xD0 = x;
    video->m_xD4 = y;
}

// ---- src/JetskiUIPage/ctor.cpp

JetskiUIPage::JetskiUIPage(int enabled)
    : UIPage(enabled)
{
    m_fadeTime = 0.0f;
}


// ---- src/JetskiUIPage/dtor.cpp


JetskiUIPage::~JetskiUIPage()
{
    // Make sure nothing queued still references this page.
    LpRenderTarget->TrySendRenderQueue(1);
    LpRenderTarget->WaitForRenderingToComplete();
}


// ---- src/JetskiUIPage/Tick.cpp

int JetskiUIPage::Tick(float dt)
{
    return UIPage::Tick(dt);
}


// ---- src/JetskiUIPage/ProcessInputSounds.cpp

void JetskiUIPage::ProcessInputSounds(int player, int input)
{
    switch (player) {
    case 0:
        switch (input) {
        case 4:
            PlayPageSound(2);
            break;
        case 6:
            PlayPageSound(3);
            break;
        case 2:
            PlayPageSound(5);
            break;
        case 3:
            PlayPageSound(5);
            break;
        case 0:
            PlayPageSound(6);
            break;
        case 1:
            PlayPageSound(6);
            break;
        }
        break;
    }
}


// ---- src/JetskiUIPage/PlayPageSound.cpp
void JetskiUIPage::PlayPageSound(int sound)
{
    if (((JetskiUIManager *)m_manager)->m_responses)
        ((JetskiUIManager *)m_manager)->m_responses->TriggerResponse(sound, 1, 0);
}


// ---- src/JetskiUIPageCareer/ctor.cpp

JetskiUIPageCareer::JetskiUIPageCareer(int enabled)
    : JetskiUIPage(enabled)
{
    m_font180 = 0;
    m_font184 = 0;
    m_font188 = 0;
    m_font18C = 0;
    m_fadeMusic = 0;
}


// ---- src/JetskiUIPageCareer/dtor.cpp


JetskiUIPageCareer::~JetskiUIPageCareer()
{
    if (m_name)
        delete m_name;
    if (m_character)
        delete m_character;
    if (m_racesLeft)
        delete m_racesLeft;
    if (m_event)
        delete m_event;
    if (m_difficulty)
        delete m_difficulty;
}


// ---- src/JetskiUIPageCareer/Create.cpp
extern "C" int sprintf(char *buf, const char *fmt, ...);

extern char UITextureContentDirectory[0x104];

class Palette8;
class PSXTextureMap;

PSXTextureMap *GetTextureMapFromResourceName(char *name, int format, Palette8 *palette, unsigned int flags,
                                             _D3DBLEND src, _D3DBLEND dst, int alpha, unsigned int color, int mip);

class ScreenTextFont;






class UIManager;


struct CareerRect {
    int left, top, right, bottom;
};

static inline ScreenTextFont *GetFont(char *name)
{
    ScreenTextFontItem *item = LpGame->m_fonts->GetScreenTextFont(name);
    return item ? item->m_font : 0;
}


JetskiUIPageCareer *JetskiUIPageCareer::Create(char *a, char *b, UIManager *manager, ScreenTextManager *textManager)
{
    char name[0x104];

    m_rects[0].left = 0x112;
    m_rects[0].right = 0x152;
    m_rects[0].top = 0x37;
    m_rects[0].bottom = 0x77;
    m_rects[1].left = 0x1F3;
    m_rects[1].right = 0x233;
    m_rects[1].top = 0x37;
    m_rects[1].bottom = 0x77;
    m_rects[2].left = 0x112;
    m_rects[2].right = 0x152;
    m_rects[2].top = 0x117;
    m_rects[2].bottom = 0x157;
    m_rects[3].left = 0x1F5;
    m_rects[3].right = 0x235;
    m_rects[3].top = 0x11A;
    m_rects[3].bottom = 0x15A;
    m_fadeMusic = 0;
    sprintf(name, "%sULAlpha", UITextureContentDirectory);
    m_corners[0] = GetTextureMapFromResourceName(name, 0x22B8, 0, 0, (_D3DBLEND)5, (_D3DBLEND)6, 0x80, 0xFF00FF, 1);
    sprintf(name, "%sURAlpha", UITextureContentDirectory);
    m_corners[1] = GetTextureMapFromResourceName(name, 0x22B8, 0, 0, (_D3DBLEND)5, (_D3DBLEND)6, 0x80, 0xFF00FF, 1);
    sprintf(name, "%sBLAlpha", UITextureContentDirectory);
    m_corners[2] = GetTextureMapFromResourceName(name, 0x22B8, 0, 0, (_D3DBLEND)5, (_D3DBLEND)6, 0x80, 0xFF00FF, 1);
    sprintf(name, "%sBRAlpha", UITextureContentDirectory);
    m_corners[3] = GetTextureMapFromResourceName(name, 0x22B8, 0, 0, (_D3DBLEND)5, (_D3DBLEND)6, 0x80, 0xFF00FF, 1);
    UIPage::Create(a, b, manager, textManager);
    m_font180 = GetFont("title_b");
    m_font184 = GetFont("title_b");
    m_font188 = GetFont("info_b");
    m_font18C = GetFont("info_b");
    m_pageTextManager = textManager;

    m_name = new ScreenText(0x28, 0);
    m_name->SetScale(0.95f);
    m_name->SetColor(0xFFFFDE00);
    m_name->SetPosition(131.0f, 227.0f);
    m_name->SetFont(m_font184);
    m_name->SetJustification(ScreenTextManager::CenterHorizontalJustification);

    m_character = new ScreenText(0x28, 0);
    m_character->SetColor(0xFFFFDE00);
    m_character->SetPosition(131.0f, 287.0f);
    m_character->SetFont(m_font18C);
    m_character->SetJustification(ScreenTextManager::CenterHorizontalJustification);

    m_racesLeft = new ScreenText(0x28, 0);
    m_racesLeft->SetColor(0xFFFFDE00);
    m_racesLeft->SetPosition(228.0f, 342.0f);
    m_racesLeft->SetFont(m_font180);
    m_racesLeft->SetJustification(ScreenTextManager::RightJustification);

    m_event = new ScreenText(0x28, 0);
    m_event->SetColor(0xFFFFDE00);
    m_event->SetPosition(418.0f, 83.0f);
    m_event->SetFont(m_font188);
    m_event->SetJustification(ScreenTextManager::CenterHorizontalJustification);

    m_difficulty = new ScreenText(0x28, 0);
    m_difficulty->SetColor(0xFFFFDE00);
    m_difficulty->SetPosition(194.0f, 55.0f);
    m_difficulty->SetFont(m_font180);
    m_difficulty->SetJustification(ScreenTextManager::CenterHorizontalJustification);
    return this;
}


// ---- src/JetskiUIPageCareer/PrepareGeometry.cpp




int JetskiUIPageCareer::PrepareGeometry()
{
    if (m_font180) {
        TextureMap *t = m_font180->m_texture;
        if (10.0f > t->m_usage)
            t->m_usage = 10.0f;
    }
    if (m_font184) {
        TextureMap *t = m_font184->m_texture;
        if (10.0f > t->m_usage)
            t->m_usage = 10.0f;
    }
    if (m_font188) {
        TextureMap *t = m_font188->m_texture;
        if (10.0f > t->m_usage)
            t->m_usage = 10.0f;
    }
    if (m_font18C) {
        TextureMap *t = m_font18C->m_texture;
        if (10.0f > t->m_usage)
            t->m_usage = 10.0f;
    }
    return GameObject::PrepareGeometry();
}


// ---- src/JetskiUIPageCareer/Render3D.cpp

class TextureMap;
class ScreenText;
class ScreenTextFont;

extern RenderTarget *LpRenderTarget;









int JetskiUIPageCareer::Render3D()
{
    JetskiCareer *career = LpGame->m_gameOptions->m_career;
    switch (career->m_events[career->m_currentCircuit].m_type) {
    case 6:
        m_pageTextManager->AddText(m_racesLeft, 0);
        m_pageTextManager->AddText(m_character, 0);
        break;
    case 7:
        break;
    }
    m_pageTextManager->AddText(m_name, 0);
    m_pageTextManager->AddText(m_event, 0);
    m_pageTextManager->AddText(m_difficulty, 0);
    if (m_videoPlayer) {
        LpRenderTarget->Blt(&m_rects[0], m_corners[0], 0, 0);
        LpRenderTarget->Blt(&m_rects[1], m_corners[1], 0, 0);
        LpRenderTarget->Blt(&m_rects[2], m_corners[2], 0, 0);
        LpRenderTarget->Blt(&m_rects[3], m_corners[3], 0, 0);
        m_videoPlayer->BltCurrentDecodedFrame();
    }
    return GameObject::Render3D();
}


// ---- src/JetskiUIPageCareer/ResetPage.cpp
#pragma inline_depth(5)

extern "C" char *strncpy(char *dst, const char *src, unsigned int n);

extern "C" int sprintf(char *buf, const char *fmt, ...);












void JetskiUIPageCareer::ResetPage()
{
    JetskiUIManMain *manager = ((JetskiUIManMain *)m_manager);
    JetskiGameOptions *options = LpGame->m_gameOptions;
    CareerRacer *profile = options->m_team;
    JetskiCareer *career = options->m_career;
    int scene;
    unsigned int color;
    char characterName[0x40];
    char buffer[0x100];
    int left;
    char *eventName;

    manager->m_waterEffect->Pause(0);
    options->m_difficulty = career->m_difficulty;
    m_name->SetString(profile->m_name);
    manager->GetTeamColor(options->m_teamIndex, &color);
    m_name->SetColor(color);
    m_pageTextManager->PositionText(m_name);
    scene = career->m_events[career->m_currentCircuit].m_scene;
    switch (career->m_events[career->m_currentCircuit].m_type) {
    case 6:
        SetGroupFocus(3, 1);
        options->GetCharacterName(profile->m_character, characterName);
        m_character->SetString(characterName);
        m_pageTextManager->PositionText(m_character);
        left = career->m_events[career->m_currentCircuit].m_pointsToPass - profile->m_points;
#line 333 "JetskiUIPages.cpp"
        ASSERT(left < 11, "Impossible.  Should not have passed last Race");
        if (left > 8)
            m_racesLeft->SetString("1");
        else if (left > 5)
            m_racesLeft->SetString("2");
        else if (left > 3)
            m_racesLeft->SetString("3");
        else if (left > 2)
            m_racesLeft->SetString("4");
        else
            m_racesLeft->SetString("5");
        m_pageTextManager->PositionText(m_racesLeft);
        break;
    case 7:
        SetGroupFocus(3, 2);
        break;
    }
    eventName = LpGame->m_uiText->GetString(284);
    sprintf(buffer, "%s %d %s %d", eventName, career->m_currentCircuit + 1,
            LpGame->m_uiText->GetString(286), career->m_numEvents);
    m_event->SetString(buffer);
    m_pageTextManager->PositionText(m_event);
    switch (career->m_difficulty) {
    case 3:
        m_difficulty->SetString(LpGame->m_uiText->GetString(247));
        break;
    case 2:
        m_difficulty->SetString(LpGame->m_uiText->GetString(246));
        break;
    case 1:
        m_difficulty->SetString(LpGame->m_uiText->GetString(245));
        break;
    }
    m_pageTextManager->PositionText(m_difficulty);
    SetGroupFocus(2, scene);
    SetPlayerFocus(1, 3, 0);
    int player = 1;     // held in a local: retail loads it first and copies it to $a1
    JetskiWidgetButton *button = (JetskiWidgetButton *)GetWidgetPtr(1, 2, player);
    switch (career->m_events[career->m_currentCircuit].m_type) {
    case 6:
        TurnGroupControlOnOff(1, 1, 2);
        if (GetGroupControlID(1) == 2)
            button->SetColor(0, 0xFF, 0xFF, 0xFF);
        else
            button->SetColor(0xEC, 0x85, 0x31, 0xFF);
        break;
    case 7:
        TurnGroupControlOnOff(0, 1, 2);
        button->SetColor(0xC8, 0xC8, 0xC8, 0xFF);
        break;
    }
    StartNewVideo();
    manager->m_x2FC = 0;
}


// ---- src/JetskiUIPageCareer/ProcessUserInput.cpp
class UIManager;










int JetskiUIPageCareer::ProcessUserInput(int player, int input)
{
    ProcessInputSounds(player, input);
    JetskiGameOptions *options = LpGame->m_gameOptions;
    JetskiUIManMain *state;
    JetskiCareer *career;
    career = options->m_career;
    // retail goes through the manager pointer each time, except for m_x2E8
    state = ((JetskiUIManMain *)m_manager);

    switch (player) {
    case 0:
        switch (input) {
        case 4:
            switch (GetGroupControlID(1)) {
            case 1:
                ((JetskiUIManMain *)m_manager)->m_state = 7;
                ((JetskiUIManMain *)m_manager)->m_nextPage = 0x1A;
                break;
            case 2:
                ((JetskiUIManMain *)m_manager)->m_state = 7;
                ((JetskiUIManMain *)m_manager)->m_nextPage = 0x1B;
                state->m_selectMode = 1;
                break;
            case 3:
                options->m_x30 = 3;
                if (career->m_events[career->m_currentCircuit].m_type == 6) {
                    SetAIRacers();
                    options->m_x2C = career->m_events[career->m_currentCircuit].m_scene - 1;
                    options->m_raceType = 6;
                    options->m_numRacers = 5;
                    options->m_numHumans = 1;
                    options->m_x3C = 4;
                    int ai = 1;
                    for (int i = 0; i < 5; i++) {
                        CareerRacer *racer = &career->m_racers[i];
                        if (!racer->m_enabled) {
                            options->m_racerActive[0] = 0;
                            options->m_racerCharacter[0] = racer->m_character;
                            options->m_racerSkin[0] = racer->m_skin;
                            options->m_racerVehicle[0] = racer->m_vehicle;
                            options->m_racerVehicleSkin[0] = racer->m_vehicleSkin;
                        } else {
                            options->m_racerActive[ai] = 1;
                            options->m_racerCharacter[ai] = career->m_racers[i].m_character;
                            options->m_racerSkin[ai] = career->m_racers[i].m_skin;
                            options->m_racerVehicle[ai] = career->m_racers[i].m_vehicle;
                            options->m_racerVehicleSkin[ai] = career->m_racers[i].m_vehicleSkin;
                            ai++;
                        }
                    }
                    ((JetskiUIManMain *)m_manager)->m_state = 8;
                    m_fadeMusic = 1;
                } else if (career->m_events[career->m_currentCircuit].m_type == 7) {
                    ((JetskiUIManMain *)m_manager)->m_state = 7;
                    ((JetskiUIManMain *)m_manager)->m_nextPage = 0x1C;
                }
                break;
            }
            break;
        case 6:
            ((JetskiUIManMain *)m_manager)->m_state = 7;
            ((JetskiUIManMain *)m_manager)->m_nextPage = 0x16;
            break;
        }
        break;
    }
    return 1;
}


// ---- src/JetskiUIPageCareer/ProcessInputSounds.cpp









void JetskiUIPageCareer::ProcessInputSounds(int player, int input)
{
    switch (player) {
    case 0:
        switch (input) {
        case 4: {
            JetskiCareer *career = LpGame->m_gameOptions->m_career;
            switch (GetGroupControlID(1)) {
            case 3:
                if (career->m_events[career->m_currentCircuit].m_type == 6) {
                    PlayPageSound(4);
                    m_fadeTime = 0;
                } else {
                    PlayPageSound(2);
                }
                break;
            case 1:
            case 2:
                PlayPageSound(2);
                break;
            }
            break;
        }
        case 6:
            PlayPageSound(3);
            break;
        case 2:
            PlayPageSound(5);
            break;
        case 3:
            PlayPageSound(5);
            break;
        }
        break;
    }
}


// ---- src/JetskiUIPageCareer/SetAIRacers.cpp
#pragma fast_fptosi on

extern "C" int rand();






// Give each enabled AI slot a random character from its choices.
void JetskiUIPageCareer::SetAIRacers()
{
    JetskiGameOptions *options;
    JetskiCareer *career;
    CareerRacer *racer;
    int i;
    options = LpGame->m_gameOptions;
    career = options->m_career;
    for (i = 0; i < 5; i++) {
        racer = &career->m_racers[i];
        if (racer->m_enabled) {
            // rand() / 2^31 picks an index in [0, m_numChoices).
            int character = racer->m_choices[(int)(racer->m_numChoices * (4.65661287e-10f * rand()))];
            racer->m_character = character;
            racer->m_skin = i + 2;
            racer->m_vehicle = options->GetVehicleID(character);
            racer->m_vehicleSkin = i + 2;
        }
    }
}
#pragma fast_fptosi reset


// ---- src/JetskiUIPageCareer/Transition.cpp
class UIManager;





int JetskiUIPageCareer::Transition(int)
{
    if (((JetskiUIManMain *)m_manager)->m_direction == 1)
        PlayPageSound(1);
    return 1;
}


// ---- src/JetskiUIPageCareer/ProcessTransition.cpp
class UIManager;

#pragma fast_fptosi on






// Fade the music out over 1.2 seconds.
int JetskiUIPageCareer::ProcessTransition(float dt)
{
    JetskiUIManager *state = ((JetskiUIManager *)m_manager);
    int done = 1;
    if (state->m_direction == 2 && m_fadeMusic) {
        m_fadeTime += dt;
        int volume = (int)((1.0f - m_fadeTime / 1.2f) * (float)LpGame->m_gameOptions->m_musicVolume);
        if (volume > 0) {
            (*(MusicPlayer **)&state->m_musicGroup)->SetVolume(volume);
            done = 0;
        }
    } else if (state->m_direction == 2) {
        EndVideo();
    }
    return done;
}
#pragma fast_fptosi reset


// ---- src/JetskiUIPageCareer/StartNewVideo.cpp
extern "C" int sprintf(char *buf, const char *fmt, ...);

struct CareerCircuitInfo {
    int m_track;                        // +0x00 (1-based)
    char pad04[0x20 - 0x04];
};







// Play the preview video for the career's current track.
void JetskiUIPageCareer::StartNewVideo()
{
    char name[100];
    JetskiCareer *career = LpGame->m_gameOptions->m_career;
    int track = career->m_events[career->m_currentCircuit].m_scene - 1;
    int rider = career->m_difficulty;
    const char *tracks[21] = {
        "Hawaii", "Evergl", "Three", "Madrid1", "Missio", "Rhine", "Barrie",
        "Seoul1", "Quilit", "Nice", "Lochne", "Madrid2", "Bali", "Tokyo",
        "Nile", "Seoul2", "Venice", "Bering", "Amazon", "Havasu", "Bermud",
    };
    const char *riders[3] = { "E", "E", "H" };
    sprintf(name, "Videos\\Tracks\\%s%s.pss",
            tracks[track], riders[rider - 1]);
    EndVideo();
    SetBackground(name, 0);
    SetVideoPosition(m_videoPlayer, 0x113, 0x37);
    m_videoPlayer->m_xE0 = 1;
    m_videoPlayer->SetColor(0, 1, 1, 1, 0x80);
    m_videoPlayer->Start();
}


// ---- src/JetskiUIPageCareer/EndVideo.cpp



void JetskiUIPageCareer::EndVideo()
{
    if (m_videoPlayer) {
        m_videoPlayer->Stop();
        m_videoPlayer->RemoveFromChain();
        m_videoPlayer->Release();
        m_videoPlayer = 0;
    }
}


// ---- src/JetskiUIPageCarRacerSelect/ctor.cpp

JetskiUIPageCarRacerSelect::JetskiUIPageCarRacerSelect(int enabled)
    : JetskiUIPage(enabled)
{
    m_icons = 0;
    m_headShots = 0;
    m_portraits = 0;
    m_ring = 0;
    m_teamIndex = 0;
    m_x180 = 0;
    m_fadeMusic = 0;
    m_riderChosen = 1;
    m_deviceName[0] = 0;
}


// ---- src/JetskiUIPageCarRacerSelect/dtor.cpp
extern "C" void free(void *p);


extern DataAccess *LpDataAccess;

// Objects owned by the page (vptr at +0).

JetskiUIPageCarRacerSelect::~JetskiUIPageCarRacerSelect()
{
    if (m_deviceName[0])
        LpDataAccess->DropDevice(m_deviceName, 1);
    for (int i = 0; i < 5; i++)
        delete m_icons[i];
    free(m_icons);
    m_icons = 0;
    delete m_ring;
    m_ring = 0;
}


// ---- src/JetskiUIPageCarRacerSelect/Create.cpp
// Block-form ASSERT/STRCPY: retail pads only the labels inside loops.

extern "C" void *malloc(unsigned int size);
extern "C" char *strcat(char *dst, const char *src);

extern char UITextureContentDirectory[0x104];

class UIManager;
class ScreenTextManager;





JetskiUIPageCarRacerSelect *JetskiUIPageCarRacerSelect::Create(char *a, char *b, UIManager *manager, ScreenTextManager *textManager)
{
    UIPage::Create(a, b, manager, textManager);
    m_icons = (ScreenIcon **)malloc(5 * sizeof(ScreenIcon *));
    for (int i = 0; i < 5; i++) {
        m_icons[i] = new ScreenIcon;
        m_icons[i]->CreateClone();
    }
    char path[50];
#line 720 "JetskiUIPages.cpp"
    STRCPY(path, UITextureContentDirectory);
    strcat(path, "JewelColor.tga");
    m_ring = (new ScreenIcon)->Create(path, 0.0f, 0.0f);
    m_ring->SetWidth(44.0f);
    m_ring->SetHeight(44.0f);
    m_ring->SetUVRect(11.0f, 55.0f, 10.0f, 54.0f);
    return this;
}


// ---- src/JetskiUIPageCarRacerSelect/PrepareGeometry.cpp
class UIManager;






int JetskiUIPageCarRacerSelect::PrepareGeometry()
{
    int i;
    switch (((JetskiUIManMain *)m_manager)->m_selectMode) {
    case 0:
        break;
    case 1:
        for (i = 0; i < 5; i++) {
            PSXTextureMap *t = m_icons[i]->m_texture;
            if (t && 10.0f > t->m_usage)
                t->m_usage = 10.0f;
            if (m_portraits[i + 2]) {
                t = m_portraits[i + 2]->m_texture;
                if (10.0f > t->m_usage)
                    t->m_usage = 10.0f;
            }
        }
        if (m_ring) {
            TextureMap *t = m_ring->m_texture;
            if (10.0f > t->m_usage)
                t->m_usage = 10.0f;
        }
        break;
    case 3:
        if (m_x180) {
            for (i = 0; i < 5; i++) {
                PSXTextureMap *t = m_icons[i]->m_texture;
                if (t && 10.0f > t->m_usage)
                    t->m_usage = 10.0f;
                if (m_portraits[i + 2]) {
                    t = m_portraits[i + 2]->m_texture;
                    if (10.0f > t->m_usage)
                        t->m_usage = 10.0f;
                }
            }
            if (m_ring) {
                TextureMap *t = m_ring->m_texture;
                if (10.0f > t->m_usage)
                    t->m_usage = 10.0f;
            }
        }
        break;
    case 2:
        break;
    }
    return GameObject::PrepareGeometry();
}


// ---- src/JetskiUIPageCarRacerSelect/Render3D.cpp
class UIManager;

class ScreenIcon;





int JetskiUIPageCarRacerSelect::Render3D()
{
    JetskiUIManMain *manager = ((JetskiUIManMain *)m_manager);
    int i;
    switch (manager->m_selectMode) {
    case 0:
        break;
    case 1:
        for (i = 0; i < 5; i++) {
            if (m_portraits[i + 2])
                manager->m_iconManager->AddIcon(m_portraits[i + 2], 0);
            if (m_icons[i])
                manager->m_iconManager->AddIcon(m_icons[i], 0);
        }
        if (m_ring)
            manager->m_iconManager->AddIcon(m_ring, 0);
        break;
    case 3:
        if (m_x180) {
            for (i = 0; i < 5; i++) {
                if (m_portraits[i + 2])
                    manager->m_iconManager->AddIcon(m_portraits[i + 2], 0);
                if (m_icons[i])
                    manager->m_iconManager->AddIcon(m_icons[i], 0);
            }
            if (m_ring)
                manager->m_iconManager->AddIcon(m_ring, 0);
        }
        break;
    case 2:
        break;
    }
    return GameObject::Render3D();
}


// ---- src/JetskiUIPageCarRacerSelect/Tick.cpp
class UIManager;

extern "C" char *strcpy(char *dst, const char *src);

class Motion;
class MotionFilter;

extern MotionLibrary *LpMotionLibrary;




int JetskiUIPageCarRacerSelect::Tick(float dt)
{
    char motionName[64];
    if (((JetskiUIManager *)m_manager)->m_state) {
        return GameObject::Tick(dt);
    }
    int racer = GetGroupControlID(2) - 1;
    JetskiWidgetCharacter *widget = (JetskiWidgetCharacter *)GetWidgetPtr(2, racer + 1, 2);
    GetWidgetPtr(2, racer + 1, 1);
    if (widget->m_character) {
        if (widget->m_character->m_x10) {
            switch (racer) {
            case 0:
                strcpy(motionName, "RaceNeutral");
                break;
            case 4:
                strcpy(motionName, "RaceNeutralSJ");
                break;
            case 2:
                strcpy(motionName, "RaceNeutral");
                break;
            case 3:
                strcpy(motionName, "RaceNeutralUJ");
                break;
            case 1:
                strcpy(motionName, "RaceNeutralBR");
                break;
            case 5:
                strcpy(motionName, "RaceNeutralSJ");
                break;
            case 6:
                strcpy(motionName, "RaceNeutralUJ");
                break;
            case 7:
                strcpy(motionName, "RaceNeutral");
                break;
            case 8:
                strcpy(motionName, "RaceNeutralSJ");
                break;
            }
            widget->m_character->PerformActionByMotion(LpMotionLibrary->MotionOfName(motionName, 1));
        } else {
            widget->m_character->TickBlendedFrame(dt, 0, 0);
        }
    }
    return JetskiUIPage::Tick(dt);
}


// ---- src/JetskiUIPageCarRacerSelect/ResetPage.cpp
class UIManager;



class ScreenIcon;



void JetskiUIPageCarRacerSelect::ResetPage()
{
    JetskiGameOptions *options;
    JetskiUIManMain *manager;
    JetskiCareer *career;
    int i;
    CareerRacer *team;
    int k2;
    int focus;
    int r;
    options = LpGame->m_gameOptions;
    manager = ((JetskiUIManMain *)m_manager);
    career = LpGame->m_gameOptions->m_career;
    m_headShots = manager->m_headShots;
    m_portraits = manager->m_portraits;
    m_bioState = 0;
    if (options->m_characterUnlocked[8]) {
        EnableGroupControl(2, 9);
        EnableGroupControl(4, 9);
        EnableGroupControl(5, 9);
    }
    for (i = 0; i < 9; i++) {
        DisableGroupControl(4, i + 1);
        DisableGroupControl(2, i + 1);
    }
    DisableGroupControlWidget(0, 1, 0x12);
    DisableGroupControlWidget(0, 1, 0x13);
    switch (manager->m_selectMode) {
    case 0:
        EnableGroup(1);
        for (i = 0; i < 9; i++) {
            EnableGroupControl(4, i + 1);
            EnableGroupControl(2, i + 1);
        }
        SetGroupFocus(5, 1);
        SetGroupFocus(3, 1);
        SetGroupFocus(2, 1);
        SetGroupFocus(1, 1);
        SetPlayerFocus(4, 1, 0);
        for (i = 0; i < 9; i++)
            options->m_skins[i] = 2;
        break;
    case 1: {
        team = LpGame->m_gameOptions->m_team;
        DisableGroup(1);
        for (i = 0; i < team->m_numChoices; i++) {
            EnableGroupControl(4, team->m_choices[i] + 1);
            EnableGroupControl(2, team->m_choices[i] + 1);
        }
        EnableGroupControlWidget(0, 1, 0x12);
        EnableGroupControlWidget(0, 1, 0x13);
        SetGroupFocus(5, team->m_character + 1);
        SetGroupFocus(3, 1);
        SetGroupFocus(2, team->m_character + 1);
        SetPlayerFocus(4, team->m_character + 1, 0);
        for (i = 0; i < 9; i++)
            options->m_skins[i] = options->m_teamIndex + 2;
        TeamHeadShots();
        break;
    }
    case 2: {
        focus = 1;
        team = LpGame->m_gameOptions->m_team;
        DisableGroup(1);
        for (k2 = 0; k2 < 8; k2++) {
            EnableGroupControl(4, k2 + 1);
            EnableGroupControl(2, k2 + 1);
        }
        for (k2 = 0; k2 < team->m_numChoices; k2++) {
            DisableGroupControl(4, team->m_choices[k2] + 1);
            DisableGroupControl(2, team->m_choices[k2] + 1);
        }
        for (k2 = 0; k2 < 8; k2++) {
            if (m_groups[4]->m_controls[k2]->m_on) {
                focus = k2 + 1;
                k2 = 8;
            }
        }
        SetGroupFocus(5, focus);
        SetGroupFocus(3, 2);
        SetGroupFocus(2, focus);
        SetPlayerFocus(4, focus, 0);
        for (k2 = 0; k2 < 5; k2++) {
            team = &career->m_racers[k2];
            for (r = 0; r < team->m_numChoices; r++)
                options->m_skins[team->m_choices[r]] = k2 + 2;
        }
        break;
    }
    case 3: {
        m_x180 = 1;
        team = LpGame->m_gameOptions->m_team;
        DisableGroup(1);
        for (k2 = 0; k2 < team->m_numChoices; k2++) {
            EnableGroupControl(4, team->m_choices[k2] + 1);
            EnableGroupControl(2, team->m_choices[k2] + 1);
        }
        SetGroupFocus(5, team->m_character + 1);
        SetGroupFocus(3, 3);
        SetGroupFocus(2, team->m_character + 1);
        SetPlayerFocus(4, team->m_character + 1, 0);
        TeamHeadShots();
        break;
    }
    }
    TurnGroupOnOff(0, 3);
    LoadCharacter();
    DisableGroup(5);
    if (!options->m_characterUnlocked[8]) {
        DisableGroupControl(2, 9);
        DisableGroupControl(4, 9);
        DisableGroupControl(5, 9);
    }
}


// ---- src/JetskiUIPageCarRacerSelect/TeamHeadShots.cpp
class UIManager;






// Show the selected team's riders around the ring.
void JetskiUIPageCarRacerSelect::TeamHeadShots()
{
    JetskiUIManMain *manager = ((JetskiUIManMain *)m_manager);
    JetskiGameOptions *options = LpGame->m_gameOptions;
    CareerRacer *team = options->m_team;
    int rider;
    int i;
    for (i = 0; i < 5; i++) {
        if (i < team->m_numChoices) {
            rider = team->m_choices[i];
            if (rider == team->m_character) {
                m_teamIndex = i;
                SetRing(m_teamIndex);
            }
        } else {
            rider = 9;                  // empty slot
        }
        m_icons[i]->Clone(m_headShots[rider]);
        switch (i) {
        case 0:
            m_icons[i]->SetPosition(405.0f, 25.0f);
            break;
        case 1:
            m_icons[i]->SetPosition(457.0f, 34.0f);
            break;
        case 2:
            m_icons[i]->SetPosition(501.0f, 58.0f);
            break;
        case 3:
            m_icons[i]->SetPosition(536.0f, 96.0f);
            break;
        case 4:
            m_icons[i]->SetPosition(557.0f, 143.0f);
            break;
        }
    }
    for (i = 0; i < 8; i++) {
        if (m_portraits[i]) {
            unsigned int color;
            manager->GetTeamColor(options->m_teamIndex, &color);
            m_portraits[i]->SetColor(color);
        }
    }
}


// ---- src/JetskiUIPageCarRacerSelect/SetRing.cpp


static inline void Place(ScreenIcon *icon, float x, float y)
{
    icon->m_position.x = x;
    icon->m_position.y = y;
    icon->m_dirty = 1;
}

// Moves the selection ring to one of the five slots.
void JetskiUIPageCarRacerSelect::SetRing(int ring)
{
    switch (ring) {
    case 0:
        Place(m_ring, 405.0f, 25.0f);
        break;
    case 1:
        Place(m_ring, 457.0f, 34.0f);
        break;
    case 2:
        Place(m_ring, 501.0f, 58.0f);
        break;
    case 3:
        Place(m_ring, 536.0f, 96.0f);
        break;
    case 4:
        Place(m_ring, 557.0f, 143.0f);
        break;
    }
}


// ---- src/JetskiUIPageCarRacerSelect/ProcessUserInput.cpp
class UIManager;



int JetskiUIPageCarRacerSelect::ProcessUserInput(int player, int input)
{
    int result;     // left unset for an unknown mode, as in retail
    if (player)
        return 0;
    ProcessInputSounds(player, input);
    switch (((JetskiUIManMain *)m_manager)->m_selectMode) {
    case 0:
        result = ProcessNewInput(input);
        break;
    case 1:
        result = ProcessContinueInput(input);
        break;
    case 2:
        result = ProcessChallengerNewInput(input);
        break;
    case 3:
        result = ProcessChallengerTeamInput(input);
        m_fadeMusic = 1;
        break;
    }
    switch (input) {
    case 5:
        BioRequested();
    }
    return result;
}


// ---- src/JetskiUIPageCarRacerSelect/ProcessInputSounds.cpp
class UIManager;









void JetskiUIPageCarRacerSelect::ProcessInputSounds(int player, int input)
{
    JetskiUIManMain *manager = ((JetskiUIManMain *)m_manager);
    JetskiGameOptions *options = LpGame->m_gameOptions;
    int rider = GetGroupControlID(2) - 1;
    switch (player) {
    case 0:
        switch (manager->m_selectMode) {
        case 0:
            switch (input) {
            case 4:
                if (options->m_characterUnlocked[rider])
                    PlayPageSound(2);
                else
                    PlayPageSound(7);
                break;
            case 2:
                PlayPageSound(5);
                break;
            case 3:
                PlayPageSound(5);
                break;
            case 1:
                PlayPageSound(6);
                break;
            case 0:
                PlayPageSound(6);
                break;
            case 5:
                PlayPageSound(6);
                break;
            }
            break;
        case 1:
            switch (input) {
            case 4:
                PlayPageSound(2);
                break;
            case 5:
                PlayPageSound(6);
                break;
            case 1:
                PlayPageSound(6);
                break;
            case 0:
                PlayPageSound(6);
                break;
            }
            break;
        case 2:
            switch (input) {
            case 4:
                PlayPageSound(2);
                break;
            case 1:
                PlayPageSound(6);
                break;
            case 0:
                PlayPageSound(6);
                break;
            case 5:
                PlayPageSound(6);
                break;
            }
            break;
        case 3:
            switch (input) {
            case 4:
                PlayPageSound(4);
                m_fadeTime = 0;
                break;
            case 1:
                PlayPageSound(6);
                break;
            case 0:
                PlayPageSound(6);
                break;
            case 5:
                PlayPageSound(6);
                break;
            }
            break;
        }
        break;
    }
}


// ---- src/JetskiUIPageCarRacerSelect/LoadCharacter.cpp
extern "C" int sprintf(char *buf, const char *fmt, ...);


extern DataAccess *LpDataAccess;




void JetskiUIPageCarRacerSelect::LoadCharacter()
{
    int selected = GetGroupControlID(2) - 1;
    JetskiWidgetCharacter *character = (JetskiWidgetCharacter *)GetWidgetPtr(2, selected + 1, 2);
    JetskiWidgetSeadoo *seadoo = (JetskiWidgetSeadoo *)GetWidgetPtr(2, selected + 1, 1);
    if (m_deviceName[0])
        LpDataAccess->DropDevice(m_deviceName, 1);
    char *names[] = { "der", "bri", "and", "umi", "jua", "seb", "jer", "ama", "ste" };
    sprintf(m_deviceName, "UI\\char_%s.res", names[selected]);
    LpDataAccess->LoadResourceFile(m_deviceName, 1, 0, 0, 0, 0, 0, 0);
    character->CreateCharacter(selected, 0x1B);
    seadoo->CreateSeadoo(selected);
    m_manager->LoadRequestedMaterials();
}


// ---- src/JetskiUIPageCarRacerSelect/DeleteCharacter.cpp

extern DataAccess *LpDataAccess;

class UIWidget;






void JetskiUIPageCarRacerSelect::DeleteCharacter(int selected)
{
    if (m_deviceName[0]) {
        LpDataAccess->DropDevice(m_deviceName, 1);
        m_deviceName[0] = 0;
    }
    JetskiWidgetCharacter *character = (JetskiWidgetCharacter *)GetWidgetPtr(2, selected + 1, 2);
    JetskiWidgetSeadoo *seadoo = (JetskiWidgetSeadoo *)GetWidgetPtr(2, selected + 1, 1);
    character->DeleteCharacter();
    seadoo->DeleteSeadoo();
}


// ---- src/JetskiUIPageCarRacerSelect/BioLocked.cpp
class UIManager;











// Shows the rider select controls for an unlocked rider, or the "locked"
// group with dimmed lights for a locked one.
void JetskiUIPageCarRacerSelect::BioLocked()
{
    JetskiUIManMain *manager = ((JetskiUIManMain *)m_manager);
    JetskiGameOptions *options = LpGame->m_gameOptions;
    int rider = GetGroupControlID(2) - 1;
    _D3DCOLORVALUE off;
    _D3DCOLORVALUE ambient;
    _D3DCOLORVALUE key;
    ambient.r = ambient.g = ambient.b = 30.0f;
    key.r = key.g = key.b = 125.0f;
    off.r = off.g = off.b = 0.0f;
    off.a = 0.0f;
    ambient.a = 0.0f;
    key.a = 0.0f;
    if (options->m_characterUnlocked[rider]) {
        m_bioState = 0;
        EnableGroupControlWidget(0, 1, 0xF);
        EnableGroupControlWidget(0, 1, 0xE);
        EnableGroupControlWidget(0, 1, 0x11);
        EnableGroupControlWidget(0, 1, 0x10);
        DisableGroup(5);
        manager->m_lights[0].m_emitter->SetColor(ambient);
        manager->m_lights[1].m_emitter->SetColor(key);
        manager->m_lights[1].m_emitter->m_direction = _D3DVECTOR(-0.234182f, -0.9188457f, -0.3176188f);
    } else {
        m_bioState = 1;
        DisableGroupControlWidget(0, 1, 0xF);
        DisableGroupControlWidget(0, 1, 0xE);
        DisableGroupControlWidget(0, 1, 0x11);
        DisableGroupControlWidget(0, 1, 0x10);
        EnableGroup(5);
        DisableGroupControlWidget(5, rider + 1, 2);
        manager->m_lights[0].m_emitter->SetColor(off);
        manager->m_lights[1].m_emitter->SetColor(off);
        manager->m_lights[1].m_emitter->m_direction = _D3DVECTOR(0.0f, 1.0f, 0.0f);
    }
}


// ---- src/JetskiUIPageCarRacerSelect/BioRequested.cpp
class UIManager;









// Toggles the rider biography: shows group 5 and darkens the lights, or back.
void JetskiUIPageCarRacerSelect::BioRequested()
{
    JetskiUIManMain *manager = ((JetskiUIManMain *)m_manager);
    _D3DCOLORVALUE off;
    _D3DCOLORVALUE ambient;
    _D3DCOLORVALUE key;
    off.r = off.g = off.b = 0.0f;
    ambient.r = ambient.g = ambient.b = 30.0f;
    key.r = key.g = key.b = 125.0f;
    off.a = 0.0f;
    ambient.a = 0.0f;
    key.a = 0.0f;
    switch (m_bioState) {
    case 0:
        m_bioState = 2;
        DisableGroupControlWidget(0, 1, 0x11);
        EnableGroup(5);
        manager->m_lights[0].m_emitter->SetColor(off);
        manager->m_lights[1].m_emitter->SetColor(off);
        manager->m_lights[1].m_emitter->m_direction = _D3DVECTOR(0.0f, 1.0f, 0.0f);
        break;
    case 2:
        m_bioState = 0;
        EnableGroupControlWidget(0, 1, 0x11);
        DisableGroup(5);
        manager->m_lights[0].m_emitter->SetColor(ambient);
        manager->m_lights[1].m_emitter->SetColor(key);
        manager->m_lights[1].m_emitter->m_direction = _D3DVECTOR(-0.234182f, -0.9188457f, -0.3176188f);
        break;
    }
}


// ---- src/JetskiUIPageCarRacerSelect/ProcessNewInput.cpp
class UIManager;

class UIControl;









int JetskiUIPageCarRacerSelect::ProcessNewInput(int input)
{
    JetskiUIManMain *manager;
    int newRider;
    JetskiGameOptions *options;
    int skin;
    CareerRacer *team;
    int teamIndex;
    int rider;

    manager = ((JetskiUIManMain *)m_manager);
    options = LpGame->m_gameOptions;
    rider = GetGroupControlID(2) - 1;
    switch (input) {
    case 4:
        if (!options->m_characterUnlocked[rider])
            return 0;
        m_riderChosen = 1;
        skin = GetGroupControlID(1) + 1;
        teamIndex = skin - 2;
        options->m_teamIndex = teamIndex;
        options->m_team = &options->m_career->m_racers[teamIndex];
        team = LpGame->m_gameOptions->m_team;
        manager->m_riderSounds->PlayDialogue(rider, 0);
        team->m_enabled = 0;
        team->m_numChoices = 1;
        team->m_choices[0] = rider;
        team->m_character = rider;
        team->m_skin = skin;
        team->m_vehicle = options->GetVehicleID(rider);
        team->m_vehicleSkin = skin;
        SetUpAITeams(rider, teamIndex);
        manager->m_mode = 0;
        manager->m_state = 7;
        manager->m_nextPage = 0x18;
        break;
    case 0:
        switch (GetJoystickGroupID(0)) {
        case 1:
            CharacterSkinPrevious();
            break;
        case 4:
            m_groups[2]->PreviousControl(0);
            newRider = GetGroupControlID(2) - 1;
            if (newRider != rider) {
                DeleteCharacter(rider);
                LoadCharacter();
                SetGroupFocus(5, newRider + 1);
                SetPlayerFocus(4, newRider + 1, 0);
                BioLocked();
            }
            break;
        }
        break;
    case 1:
        switch (GetJoystickGroupID(0)) {
        case 1:
            CharacterSkinNext();
            break;
        case 4:
            m_groups[2]->NextControl(0);
            newRider = GetGroupControlID(2) - 1;
            if (newRider != rider) {
                DeleteCharacter(rider);
                LoadCharacter();
                SetGroupFocus(5, newRider + 1);
                SetPlayerFocus(4, newRider + 1, 0);
                BioLocked();
            }
            break;
        }
        break;
    }
    return 1;
}


// ---- src/JetskiUIPageCarRacerSelect/CharacterSkinNext.cpp






// Skins 2..6 cycle; every character takes the new skin. (Declaration order
// and the separate increment reproduce retail's register allocation.)
void JetskiUIPageCarRacerSelect::CharacterSkinNext()
{
    JetskiGameOptions *options;
    JetskiWidgetCharacter *character;
    int selected;
    JetskiWidgetSeadoo *seadoo;
    int skin;
    int i;
    options = LpGame->m_gameOptions;
    selected = GetGroupControlID(2) - 1;
    skin = options->m_skins[selected];
    skin++;
    if (skin > 6)
        skin = 2;
    for (i = 0; i < 9; i++)
        options->m_skins[i] = skin;
    character = (JetskiWidgetCharacter *)GetWidgetPtr(2, selected + 1, 2);
    seadoo = (JetskiWidgetSeadoo *)GetWidgetPtr(2, selected + 1, 1);
    character->ChangeSkin(skin);
    seadoo->ChangeSkin(skin);
    m_manager->LoadRequestedMaterials();
    options->m_racerSkin[0] = skin;
    options->m_racerVehicleSkin[0] = skin;
}


// ---- src/JetskiUIPageCarRacerSelect/CharacterSkinPrevious.cpp






// Skins 2..6 cycle; every character takes the new skin. (Declaration order
// and the separate increment reproduce retail's register allocation.)
void JetskiUIPageCarRacerSelect::CharacterSkinPrevious()
{
    JetskiGameOptions *options;
    JetskiWidgetCharacter *character;
    int selected;
    JetskiWidgetSeadoo *seadoo;
    int skin;
    int i;
    options = LpGame->m_gameOptions;
    selected = GetGroupControlID(2) - 1;
    skin = options->m_skins[selected];
    skin--;
    if (skin < 2)
        skin = 6;
    for (i = 0; i < 9; i++)
        options->m_skins[i] = skin;
    character = (JetskiWidgetCharacter *)GetWidgetPtr(2, selected + 1, 2);
    seadoo = (JetskiWidgetSeadoo *)GetWidgetPtr(2, selected + 1, 1);
    character->ChangeSkin(skin);
    seadoo->ChangeSkin(skin);
    m_manager->LoadRequestedMaterials();
    options->m_racerSkin[0] = skin;
    options->m_racerVehicleSkin[0] = skin;
}


// ---- src/JetskiUIPageCarRacerSelect/SetUpAITeams.cpp
// SetUpAITeams__26JetskiUIPageCarRacerSelectFii  @ 0x00359310

#pragma fast_fptosi on

extern "C" int rand(void);

struct TeamRoster {
    char pad0[0x18];
    int m_numRacers;                // +0x18
    int m_racers[(0x34 - 0x1C) / 4];// +0x1C
};

struct Team {
    char pad0[0xC];
    TeamRoster m_roster;            // +0x0C
};                                  // size 0x40




// Fill every other team with two random characters nobody has taken yet.
void JetskiUIPageCarRacerSelect::SetUpAITeams(int playerCharacter, int playerTeam)
{
    TeamRoster *roster;
    int added;
    Team *teams;
    int t;
    int allUsed;
    int j;
    int used[8 + PS2_OVERRUN(1)];  // the player may be character 8 (Steve)
    teams = (Team *)LpGame->m_gameOptions->m_career;
    for (t = 0; t < 8; t++)        // t doubles as the clearing counter (register order)
        used[t] = 0;
    used[playerCharacter] = 1;
    for (t = 0; t < 5; t++, teams++) {
        if (t == playerTeam)
            continue;
        roster = &teams->m_roster;
        added = 0;
        allUsed = 0;
        while (added < 2 && !allUsed) {
            int character = (int)(8.0f * (4.656613e-10f * rand()));
            if (!used[character]) {
                used[character] = 1;
                roster->m_racers[roster->m_numRacers] = character;
                roster->m_numRacers++;
                added++;
            }
            allUsed = 1;
            for (j = 0; j < 8; j++) {
                if (!used[j])
                    allUsed = 0;
            }
        }
    }
}
#pragma fast_fptosi reset


// ---- src/JetskiUIPageCarRacerSelect/ProcessContinueInput.cpp
class UIManager;

class UIControl;









int JetskiUIPageCarRacerSelect::ProcessContinueInput(int input)
{
    JetskiUIManMain *manager = ((JetskiUIManMain *)m_manager);
    JetskiGameOptions *options = LpGame->m_gameOptions;
    CareerRacer *team = options->m_team;
    int rider = GetGroupControlID(2) - 1;
    int newRider;

    switch (input) {
    case 4:
        m_riderChosen = 1;
        manager->m_riderSounds->PlayDialogue(rider, 0);
        team->m_character = rider;
        team->m_vehicle = options->GetVehicleID(rider);
        manager->m_nextPage = 0x19;
        manager->m_state = 7;
        break;
    case 6:
        m_riderChosen = 0;
        manager->m_nextPage = 0x19;
        manager->m_state = 7;
        break;
    case 0:
        m_teamIndex--;
        if (m_teamIndex < 0)
            m_teamIndex = team->m_numChoices - 1;
        newRider = team->m_choices[m_teamIndex];
        if (newRider != rider) {
            SetRing(m_teamIndex);
            SetGroupFocus(2, newRider + 1);
            DeleteCharacter(rider);
            LoadCharacter();
            SetGroupFocus(5, newRider + 1);
            SetPlayerFocus(4, newRider + 1, 0);
            BioLocked();
        }
        break;
    case 1:
        m_teamIndex++;
        if (m_teamIndex >= team->m_numChoices)
            m_teamIndex = 0;
        newRider = team->m_choices[m_teamIndex];
        if (newRider != rider) {
            SetRing(m_teamIndex);
            SetGroupFocus(2, newRider + 1);
            DeleteCharacter(rider);
            LoadCharacter();
            SetGroupFocus(5, newRider + 1);
            SetPlayerFocus(4, newRider + 1, 0);
            BioLocked();
        }
        break;
    }
    return 1;
}


// ---- src/JetskiUIPageCarRacerSelect/ProcessChallengerNewInput.cpp
class UIManager;

class UIControl;









int JetskiUIPageCarRacerSelect::ProcessChallengerNewInput(int input)
{
    JetskiGameOptions *options;
    JetskiUIManMain *manager;
    int rider;
    int newRider;
    int skin;

    manager = ((JetskiUIManMain *)m_manager);
    options = LpGame->m_gameOptions;
    rider = GetGroupControlID(2) - 1;
    switch (input) {
    case 4: {
        m_riderChosen = 1;
        skin = options->m_skins[rider];
        manager->m_riderSounds->PlayDialogue(rider, 1);
        options->m_challengerRider = rider;
        options->m_challengerSkin = skin;
        options->m_challengerVehicle = options->GetVehicleID(rider);
        options->m_challengerVehicleSkin = skin;
        manager->m_state = 7;
        ((JetskiUIManMain *)m_manager)->m_nextPage = 0x1B;
        m_x180 = 0;
        break;
    }
    case 0: {
        m_groups[2]->PreviousControl(0);
        newRider = GetGroupControlID(2) - 1;
        if (newRider != rider) {
            DeleteCharacter(rider);
            LoadCharacter();
            SetGroupFocus(5, newRider + 1);
            SetPlayerFocus(4, newRider + 1, 0);
        }
        break;
    }
    case 1: {
        m_groups[2]->NextControl(0);
        newRider = GetGroupControlID(2) - 1;
        if (newRider != rider) {
            DeleteCharacter(rider);
            LoadCharacter();
            SetGroupFocus(5, newRider + 1);
            SetPlayerFocus(4, newRider + 1, 0);
        }
        break;
    }
    }
    return 1;
}


// ---- src/JetskiUIPageCarRacerSelect/ProcessChallengerTeamInput.cpp
class UIManager;

class UIControl;









int JetskiUIPageCarRacerSelect::ProcessChallengerTeamInput(int input)
{
    JetskiUIManMain *manager;
    JetskiGameOptions *options;
    JetskiCareer *career;
    CareerRacer *team;
    int rider;
    int newRider;
    int skin;

    manager = ((JetskiUIManMain *)m_manager);
    options = LpGame->m_gameOptions;
    career = options->m_career;
    team = options->m_team;
    rider = GetGroupControlID(2) - 1;
    switch (input) {
    case 4:
        m_riderChosen = 1;
        skin = options->m_skins[rider];
        manager->m_riderSounds->PlayDialogue(rider, 0);
        team->m_character = rider;
        team->m_skin = skin;
        team->m_vehicle = options->GetVehicleID(rider);
        team->m_vehicleSkin = skin;
        options->m_numRacers = 2;
        options->m_numHumans = 1;
        options->m_x3C = 1;
        options->m_racerActive[0] = 0;
        options->m_racerCharacter[0] = rider;
        options->m_racerSkin[0] = skin;
        options->m_racerVehicle[0] = team->m_vehicle;
        options->m_racerVehicleSkin[0] = skin;
        options->m_racerActive[1] = 1;
        options->m_racerCharacter[1] = options->m_challengerRider;
        options->m_racerSkin[1] = options->m_challengerSkin;
        options->m_racerVehicle[1] = options->m_challengerVehicle;
        options->m_racerVehicleSkin[1] = options->m_challengerVehicleSkin;
        options->m_x2C = career->m_events[career->m_currentCircuit].m_scene - 1;
        options->m_raceType = 7;
        ((JetskiUIManMain *)m_manager)->m_state = 8;
        break;
    case 0:
        m_teamIndex--;
        if (m_teamIndex < 0)
            m_teamIndex = team->m_numChoices - 1;
        newRider = team->m_choices[m_teamIndex];
        if (newRider != rider) {
            SetRing(m_teamIndex);
            SetGroupFocus(2, newRider + 1);
            DeleteCharacter(rider);
            LoadCharacter();
            SetGroupFocus(5, newRider + 1);
            SetPlayerFocus(4, newRider + 1, 0);
            BioLocked();
        }
        break;
    case 1:
        m_teamIndex++;
        if (m_teamIndex >= team->m_numChoices)
            m_teamIndex = 0;
        newRider = team->m_choices[m_teamIndex];
        if (newRider != rider) {
            SetRing(m_teamIndex);
            SetGroupFocus(2, newRider + 1);
            DeleteCharacter(rider);
            LoadCharacter();
            SetGroupFocus(5, newRider + 1);
            SetPlayerFocus(4, newRider + 1, 0);
            BioLocked();
        }
        break;
    }
    return 1;
}


// ---- src/JetskiUIPageCarRacerSelect/Transition.cpp
class UIManager;

extern "C" char *strcpy(char *dst, const char *src);
extern "C" char *strcat(char *dst, const char *src);

class Motion;

extern MotionLibrary *LpMotionLibrary;


class UIWidget;








// Plays the chosen rider's "selected" (or "challenge") animation.
int JetskiUIPageCarRacerSelect::Transition(int direction)
{
    int rider;
    JetskiWidgetCharacter *widget;
    JetskiUIManMain *manager;

    manager = ((JetskiUIManMain *)m_manager);
    if (manager->m_direction == 1) {
        PlayPageSound(1);
    } else {
        m_bioState = 2;
        BioRequested();
        rider = GetGroupControlID(2) - 1;
        widget = (JetskiWidgetCharacter *)GetWidgetPtr(2, rider + 1, 2);
        if (m_riderChosen) {
            char name[32];
            char action[32];
            if (manager->m_selectMode == 2) {
                manager->m_selectMode = 3;
                strcpy(action, "challenge");
            } else {
                strcpy(action, "selected");
            }
            // An unknown rider leaves name unset (as in retail).
            switch (rider) {
            case 0:
                strcpy(name, "derrick_");
                strcat(name, action);
                break;
            case 4:
                strcpy(name, "juana_");
                strcat(name, action);
                break;
            case 2:
                strcpy(name, "andreas_");
                strcat(name, action);
                break;
            case 3:
                strcpy(name, "umi_");
                strcat(name, action);
                break;
            case 1:
                strcpy(name, "britt_");
                strcat(name, action);
                break;
            case 5:
                strcpy(name, "sebastian_");
                strcat(name, action);
                break;
            case 6:
                strcpy(name, "jeremy_");
                strcat(name, action);
                break;
            case 7:
                strcpy(name, "ammanra_");
                strcat(name, action);
                break;
            case 8:
                strcpy(name, "steve_");
                strcat(name, action);
                break;
            }
            widget->m_character->PerformActionByMotion(LpMotionLibrary->MotionOfName(name, 1));
        }
    }
    return 1;
}


// ---- src/JetskiUIPageCarRacerSelect/ProcessTransition.cpp
class UIManager;

#pragma fast_fptosi on




class MotionFilter;





int JetskiUIPageCarRacerSelect::ProcessTransition(float dt)
{
    int speechDone = 1;
    int musicDone = speechDone;
    int animationDone = speechDone;
    int rider;
    JetskiUIManMain *state = ((JetskiUIManMain *)m_manager);
    if (state->m_direction == 2 && m_fadeMusic) {
        m_fadeTime += dt;
        int volume = (int)((1.0f - m_fadeTime / 1.2f) * (float)LpGame->m_gameOptions->m_musicVolume);
        if (volume > 0) {
            (*(MusicPlayer **)&state->m_musicGroup)->SetVolume(volume);
            musicDone = 0;
        }
    }
    if (((JetskiUIManMain *)m_manager)->m_direction == 2) {
        rider = GetGroupControlID(2) - 1;
        JetskiWidgetCharacter *widget = (JetskiWidgetCharacter *)GetWidgetPtr(2, rider + 1, 2);
        if (m_riderChosen) {
            if (!widget->m_character->m_x10) {
                widget->m_character->TickBlendedFrame(dt, 0, 0);
                animationDone = 0;
            }
            if (state->m_riderSounds->m_playing)
                speechDone = 0;
        } else {
            DeleteCharacter(rider);
            return 1;
        }
        if (musicDone && animationDone && speechDone)
            DeleteCharacter(rider);
    }
    return musicDone && animationDone && speechDone;
}
#pragma fast_fptosi reset


// ---- src/JetskiUIPageCharSelect/ctor.cpp

JetskiUIPageCharSelect::JetskiUIPageCharSelect(int enabled)
    : JetskiUIPage(enabled)
{
    m_headShots = 0;
    m_headShotIcons = 0;
    m_portraits = 0;
    m_ring = 0;
    m_x178 = 1;
    m_character = 0;
    m_skin = 0;
    m_deviceName[0] = 0;
}


// ---- src/JetskiUIPageCharSelect/dtor.cpp
extern "C" void free(void *p);

extern DataAccess *LpDataAccess;





JetskiUIPageCharSelect::~JetskiUIPageCharSelect()
{
    int i = 0;
    if (m_deviceName[0]) {
        LpDataAccess->DropDevice(m_deviceName, 1);
        m_deviceName[0] = 0;
    }
    for (i = 0; i < 8; i++)
        delete m_headShots[i];
    free(m_headShots);
    m_headShots = 0;
    delete m_ring;
    m_ring = 0;
}


// ---- src/JetskiUIPageCharSelect/Create.cpp

extern "C" void *malloc(unsigned int size);
extern "C" char *strcat(char *dst, const char *src);

extern char UITextureContentDirectory[];



JetskiUIPageCharSelect *JetskiUIPageCharSelect::Create(char *name, char *file, UIManager *manager, ScreenTextManager *textManager)
{
    char path[50];
    int i;

    UIPage::Create(name, file, manager, textManager);
    m_headShots = (ScreenIcon **)malloc(8 * sizeof(ScreenIcon *));
    for (i = 0; i < 8; i++) {
        m_headShots[i] = new ScreenIcon;
        m_headShots[i]->CreateClone();
    }
#line 2118 "JetskiUIPages.cpp"
    STRCPY(path, UITextureContentDirectory);
    strcat(path, "JewelColor.tga");
    m_ring = (new ScreenIcon)->Create(path, 0.0f, 0.0f);
    m_ring->SetWidth(44.0f);
    m_ring->SetHeight(44.0f);
    m_ring->SetUVRect(11.0f, 55.0f, 10.0f, 54.0f);
    return this;
}


// ---- src/JetskiUIPageCharSelect/PrepareGeometry.cpp

static inline void ClampMinWidth(ScreenIcon *icon)
{
    if (icon) {
        TextureMap *texture = icon->m_texture;
        if (10.0f > texture->m_usage)
            texture->m_usage = 10.0f;
    }
}

int JetskiUIPageCharSelect::PrepareGeometry()
{
    for (int i = 0; i < 8; i++) {
        ClampMinWidth(m_headShots[i]);
        ClampMinWidth(m_portraits[i]);
    }
    ClampMinWidth(m_ring);
    return GameObject::PrepareGeometry();
}


// ---- src/JetskiUIPageCharSelect/Render3D.cpp
class UIManager;


class ScreenIcon;








int JetskiUIPageCharSelect::Render3D()
{
    JetskiUIManager *state = ((JetskiUIManager *)m_manager);
    JetskiGameOptions *options = LpGame->m_gameOptions;
    int character = GetGroupControlID(2) - 1;

    if (options->m_characterUnlocked[character]) {
        // The last skin is secret until unlocked.
        for (int i = 0; i < 8; i++) {
            if (i == 7 && !options->m_secretSkinUnlocked[character][7])
                continue;
            if (m_portraits[i])
                state->m_iconManager->AddIcon(m_portraits[i], 0);
            if (m_headShots[i])
                state->m_iconManager->AddIcon(m_headShots[i], 0);
        }
        if (m_ring)
            state->m_iconManager->AddIcon(m_ring, 0);
    }
    return GameObject::Render3D();
}


// ---- src/JetskiUIPageCharSelect/Tick.cpp
class UIManager;

extern "C" char *strcpy(char *dst, const char *src);

class Motion;

extern MotionLibrary *LpMotionLibrary;

class MotionFilter;


class UIWidget;












// Plays the highlighted rider's idle animation; in two-player modes, backs
// out when a controller has been unplugged.
int JetskiUIPageCharSelect::Tick(float dt)
{
    JetskiUIManMain *manager = ((JetskiUIManMain *)m_manager);
    int rider;
    JetskiWidgetCharacter *widget;

    if (manager->m_state) {
        return GameObject::Tick(dt);
    }
    switch (manager->m_x2E0) {
    case 2:
    case 3:
        if (!LpGame->m_controls->m_joysticks[1]->IsControllerPluggedIn() || !LpGame->m_controls->m_joysticks[0]->IsControllerPluggedIn()) {
            m_x178 = 0;
            manager->m_state = 7;
            manager->m_nextPage = 1;
            return GameObject::Tick(dt);
        }
    }
    rider = GetGroupControlID(2) - 1;
    widget = (JetskiWidgetCharacter *)GetWidgetPtr(2, rider + 1, 2);
    GetWidgetPtr(2, rider + 1, 1);
    if (widget->m_character->m_x10) {
        char name[64];
        // An unknown rider leaves name unset (as in retail).
        switch (rider) {
        case 0:
            strcpy(name, "RaceNeutral");
            break;
        case 4:
            strcpy(name, "RaceNeutralSJ");
            break;
        case 2:
            strcpy(name, "RaceNeutral");
            break;
        case 3:
            strcpy(name, "RaceNeutralUJ");
            break;
        case 1:
            strcpy(name, "RaceNeutralBR");
            break;
        case 5:
            strcpy(name, "RaceNeutralSJ");
            break;
        case 6:
            strcpy(name, "RaceNeutralUJ");
            break;
        case 7:
            strcpy(name, "RaceNeutral");
            break;
        case 8:
            strcpy(name, "RaceNeutralSJ");
            break;
        }
        widget->m_character->PerformActionByMotion(LpMotionLibrary->MotionOfName(name, 1));
    } else {
        widget->m_character->TickBlendedFrame(dt, 0, 0);
    }
    return JetskiUIPage::Tick(dt);
}



// ---- src/JetskiUIPageCharSelect/ResetPage.cpp

struct CharacterSelection {
    char pad0[4];
    int *m_choices;                 // +0x04, character/skin pairs per player
};

struct CharacterInfo {              // 0x20 bytes
    int m_hasSkins;                 // +0x00
    char pad4[0x20 - 0x4];
};






void JetskiUIPageCharSelect::ResetPage()
{
    JetskiGameOptions *options = LpGame->m_gameOptions;
    JetskiUIManMain *manager = ((JetskiUIManMain *)m_manager);
    int player;
    int character;
    int skin;
    int i;

    m_headShotIcons = manager->m_headShots;
    m_portraits = manager->m_portraits;
    m_bioState = 0;
    if (options->m_characterUnlocked[8]) {
        EnableGroupControl(2, 9);
        EnableGroupControl(3, 9);
        EnableGroupControl(5, 9);
    }
    player = -1;
    switch (manager->m_x2E0) {
    case 1:
        SetGroupFocus(1, 3);
        options->SetPlayerTypes(0);
        player = 0;
        m_character = &((CharacterSelection *)options->m_pageSettings)->m_choices[0];
        m_skin = &((CharacterSelection *)options->m_pageSettings)->m_choices[1];
        break;
    case 2:
        SetGroupFocus(1, 1);
        options->SetPlayerTypes(0);
        player = 0;
        m_character = &((CharacterSelection *)options->m_pageSettings)->m_choices[2];
        m_skin = &((CharacterSelection *)options->m_pageSettings)->m_choices[3];
        break;
    case 3:
        SetGroupFocus(1, 2);
        player = 1;
        m_character = &((CharacterSelection *)options->m_pageSettings)->m_choices[4];
        m_skin = &((CharacterSelection *)options->m_pageSettings)->m_choices[5];
        break;
    }
    character = *m_character;
    skin = *m_skin;
    SetGroupFocus(5, character + 1);
    SetGroupFocus(4, skin + 1);
    SetGroupFocus(2, character + 1);
    SetPlayerFocus(3, character + 1, player);
    for (i = 0; i < 9; i++)
        options->m_skins[i] = 0;
    options->m_skins[character] = skin;
    LoadCharacter();
    ChangeHeadShots(character);
    SetRing(skin);
    for (i = 0; i < 8; i++) {
        if (m_portraits[i]) {
            switch (i) {
            case 0:
                m_portraits[i]->SetColor(0xFFBEBEBE);
                break;
            case 1:
                m_portraits[i]->SetColor(0xFFBEBEBE);
                break;
            case 2:
                m_portraits[i]->SetColor(0xFF820000);
                break;
            case 3:
                m_portraits[i]->SetColor(0xFFB4AE00);
                break;
            case 4:
                m_portraits[i]->SetColor(0xFF009600);
                break;
            case 5:
                m_portraits[i]->SetColor(0xFF000096);
                break;
            case 6:
                m_portraits[i]->SetColor(0xFF9600C8);
                break;
            case 7:
                m_portraits[i]->SetColor(0xFF787878);
                break;
            }
        }
    }
    DisableGroup(5);
    if (!options->m_secretSkinUnlocked[character][7])
        DisableGroupControl(4, 8);
    if (!options->m_characterUnlocked[8]) {
        DisableGroupControl(2, 9);
        DisableGroupControl(3, 9);
        DisableGroupControl(5, 9);
    }
}


// ---- src/JetskiUIPageCharSelect/SetRing.cpp
class UIManager;

// Moves the selection ring to one of the eight rider slots and puts the
// selecting player's focus on it.
void JetskiUIPageCharSelect::SetRing(int ring)
{
    switch (ring) {
    case 0:
        Place(m_ring, 282.0f, 68.0f);
        break;
    case 1:
        Place(m_ring, 321.0f, 38.0f);
        break;
    case 2:
        Place(m_ring, 405.0f, 25.0f);
        break;
    case 3:
        Place(m_ring, 457.0f, 34.0f);
        break;
    case 4:
        Place(m_ring, 501.0f, 58.0f);
        break;
    case 5:
        Place(m_ring, 536.0f, 96.0f);
        break;
    case 6:
        Place(m_ring, 557.0f, 143.0f);
        break;
    case 7:
        Place(m_ring, 547.0f, 224.0f);
        break;
    }
    JetskiUIManMain *manager = ((JetskiUIManMain *)m_manager);
    int rider = GetGroupControlID(2) - 1;
    switch (manager->m_x2E0) {
    case 1:
    case 2:
        switch (GetJoystickGroupID(0)) {
        case 3:
            SetGroupFocus(4, ring + 1);
            SetGroupFocus(5, rider + 1);
            SetPlayerFocus(3, rider + 1, 0);
            break;
        case 4:
            SetPlayerFocus(4, ring + 1, 0);
            break;
        }
        break;
    case 3:
        switch (GetJoystickGroupID(1)) {
        case 3:
            SetGroupFocus(4, ring + 1);
            SetGroupFocus(5, rider + 1);
            SetPlayerFocus(3, rider + 1, 1);
            break;
        case 4:
            SetPlayerFocus(4, ring + 1, 1);
            break;
        }
        break;
    }
}


// ---- src/JetskiUIPageCharSelect/ChangeHeadShots.cpp

// Shows each rider's head shot, locked or unlocked for the given player, in
// its slot.
void JetskiUIPageCharSelect::ChangeHeadShots(int player)
{
    JetskiGameOptions *options = LpGame->m_gameOptions;
    for (int i = 0; i < 8; i++) {
        if (options->m_secretSkinUnlocked[player][i])
            m_headShots[i]->Clone(m_headShotIcons[10]);
        else
            m_headShots[i]->Clone(m_headShotIcons[9]);
        switch (i) {
        case 0:
            Place(m_headShots[i], 282.0f, 68.0f);
            break;
        case 1:
            Place(m_headShots[i], 321.0f, 38.0f);
            break;
        case 2:
            Place(m_headShots[i], 405.0f, 25.0f);
            break;
        case 3:
            Place(m_headShots[i], 457.0f, 34.0f);
            break;
        case 4:
            Place(m_headShots[i], 501.0f, 58.0f);
            break;
        case 5:
            Place(m_headShots[i], 536.0f, 96.0f);
            break;
        case 6:
            Place(m_headShots[i], 557.0f, 143.0f);
            break;
        case 7:
            Place(m_headShots[i], 547.0f, 224.0f);
            break;
        }
    }
}


// ---- src/JetskiUIPageCharSelect/ProcessUserInput.cpp

struct CharacterSelection2 {
    int *m_numPlayers;              // +0x00
    int *m_choices;                 // +0x04, character/skin pairs per player
};




class UIControl;






int JetskiUIPageCharSelect::ProcessUserInput(int player, int input)
{
    JetskiUIManMain *manager = ((JetskiUIManMain *)m_manager);
    JetskiGameOptions *options = LpGame->m_gameOptions;
    int selected;
    int character;

    switch (manager->m_x2E0) {
    case 1:
    case 2:
        if (player != 0)
            return 0;
        break;
    case 3:
        if (player != 1)
            return 0;
        break;
    }
    if (!manager->m_x2F4)
        return 0;
    selected = GetGroupControlID(2) - 1;
    ProcessInputSounds(player, input);
    switch (input) {
    case 4:
        if (!options->m_characterUnlocked[selected])
            return 0;
        m_x178 = 1;
        manager->m_riderSounds->PlayDialogue(selected, 0);
        options->m_racerActive[player] = 0;
        options->m_racerCharacter[player] = selected;
        options->m_racerSkin[player] = options->m_skins[selected];
        options->m_racerVehicle[player] = options->GetVehicleID(selected);
        options->m_racerVehicleSkin[player] = options->m_skins[selected];
        manager->m_state = 7;
        switch (manager->m_x2E0) {
        case 1:
            *((CharacterSelection2 *)LpGame->m_gameOptions->m_pageSettings)->m_numPlayers = 1;
            *m_character = selected;
            *m_skin = options->m_skins[selected];
            manager->m_x2E4 = 1;
            manager->m_nextPage = 3;
            break;
        case 2:
            *m_character = selected;
            *m_skin = options->m_skins[selected];
            manager->m_x2E0 = 3;
            manager->m_nextPage = 2;
            break;
        case 3:
            *((CharacterSelection2 *)LpGame->m_gameOptions->m_pageSettings)->m_numPlayers = 3;
            *m_character = selected;
            *m_skin = options->m_skins[selected];
            manager->m_x2E4 = 2;
            manager->m_nextPage = 3;
            break;
        }
        break;
    case 6:
        m_x178 = 0;
        switch (manager->m_x2E0) {
        case 1:
        case 2:
            manager->m_nextPage = 1;
            break;
        case 3:
            manager->m_x2E0 = 2;
            manager->m_nextPage = 2;
            break;
        }
        manager->m_state = 7;
        break;
    case 0:
        switch (GetJoystickGroupID(player)) {
        case 4:
            SwitchSkinPrevious(GetGroupControlID(2), player);
            break;
        case 3:
            DeleteCharacter();
            m_groups[2]->PreviousControl(0);
            character = GetGroupControlID(2) - 1;
            ChangeHeadShots(character);
            SetRing(options->m_skins[character]);
            BioLocked();
            LoadCharacter();
            break;
        }
        break;
    case 1:
        switch (GetJoystickGroupID(player)) {
        case 4:
            SwitchSkinNext(GetGroupControlID(2), player);
            break;
        case 3:
            DeleteCharacter();
            m_groups[2]->NextControl(0);
            character = GetGroupControlID(2) - 1;
            ChangeHeadShots(character);
            SetRing(options->m_skins[character]);
            BioLocked();
            LoadCharacter();
            break;
        }
        break;
    case 5:
        BioRequested();
        break;
    }
    return 1;
}


// ---- src/JetskiUIPageCharSelect/ProcessInputSounds.cpp
class UIManager;









void JetskiUIPageCharSelect::ProcessInputSounds(int player, int input)
{
    JetskiUIManMain *state = ((JetskiUIManMain *)m_manager);
    JetskiGameOptions *options = LpGame->m_gameOptions;
    int active = 0;
    switch (state->m_x2E0) {
    case 2:
    case 1:
        if (player == 0)
            active = 1;
        break;
    case 3:
        if (player == 1)
            active = 1;
        break;
    }
    int selected = GetGroupControlID(2) - 1;
    if (active) {
        switch (input) {
        case 4:
            if (options->m_characterUnlocked[selected])
                PlayPageSound(2);
            else
                PlayPageSound(7);
            break;
        case 6:
            PlayPageSound(3);
            break;
        case 2:
            PlayPageSound(5);
            break;
        case 3:
            PlayPageSound(5);
            break;
        case 0:
            PlayPageSound(6);
            break;
        case 1:
            PlayPageSound(6);
            break;
        case 5:
            PlayPageSound(6);
            break;
        }
    }
}


// ---- src/JetskiUIPageCharSelect/LoadCharacter.cpp
class UIManager;

extern "C" int sprintf(char *buf, const char *fmt, ...);

extern DataAccess *LpDataAccess;



class UIWidget;

// The page state object; only the slot used here (name guessed).




void JetskiUIPageCharSelect::LoadCharacter()
{
    int character = GetGroupControlID(2) - 1;
    JetskiWidgetCharacter *characterWidget = (JetskiWidgetCharacter *)GetWidgetPtr(2, character + 1, 2);
    JetskiWidgetSeadoo *seadooWidget = (JetskiWidgetSeadoo *)GetWidgetPtr(2, character + 1, 1);
    if (m_deviceName[0])
        LpDataAccess->DropDevice(m_deviceName, 1);
    char *names[9] = { "der", "bri", "and", "umi", "jua", "seb", "jer", "ama", "ste" };
    sprintf(m_deviceName, "UI\\char_%s.res", names[character]);
    LpDataAccess->LoadResourceFile(m_deviceName, 1, 0, 0, 0, 0, 0, 0);
    characterWidget->CreateCharacter(character, 2);
    seadooWidget->CreateSeadoo(character);
    ((JetskiUIManMain *)m_manager)->LoadRequestedMaterials();
}


// ---- src/JetskiUIPageCharSelect/DeleteCharacter.cpp

extern DataAccess *LpDataAccess;

class UIWidget;






void JetskiUIPageCharSelect::DeleteCharacter()
{
    if (m_deviceName[0]) {
        LpDataAccess->DropDevice(m_deviceName, 1);
        m_deviceName[0] = 0;
    }
    int selected = GetGroupControlID(2) - 1;
    JetskiWidgetCharacter *character = (JetskiWidgetCharacter *)GetWidgetPtr(2, selected + 1, 2);
    JetskiWidgetSeadoo *seadoo = (JetskiWidgetSeadoo *)GetWidgetPtr(2, selected + 1, 1);
    character->DeleteCharacter();
    seadoo->DeleteSeadoo();
}


// ---- src/JetskiUIPageCharSelect/BioLocked.cpp
class UIManager;












// Shows the rider select controls for an unlocked rider, or the "locked"
// group with dimmed lights for a locked one.
void JetskiUIPageCharSelect::BioLocked()
{
    JetskiUIManMain *manager = ((JetskiUIManMain *)m_manager);
    JetskiGameOptions *options = LpGame->m_gameOptions;
    int rider = GetGroupControlID(2) - 1;
    _D3DCOLORVALUE off;
    _D3DCOLORVALUE ambient;
    _D3DCOLORVALUE key;
    ambient.r = ambient.g = ambient.b = 30.0f;
    key.r = key.g = key.b = 125.0f;
    off.r = off.g = off.b = 0.0f;
    off.a = 0.0f;
    ambient.a = 0.0f;
    key.a = 0.0f;
    if (options->m_characterUnlocked[rider]) {
        m_bioState = 0;
        EnableGroupControlWidget(0, 1, 0x11);
        EnableGroupControlWidget(0, 1, 0xF);
        EnableGroupControlWidget(0, 1, 0x13);
        EnableGroupControlWidget(0, 1, 0x12);
        DisableGroup(5);
        manager->m_lights[0].m_emitter->SetColor(ambient);
        manager->m_lights[1].m_emitter->SetColor(key);
        manager->m_lights[1].m_emitter->m_direction = _D3DVECTOR(-0.234182f, -0.9188457f, -0.3176188f);
    } else {
        m_bioState = 1;
        DisableGroupControlWidget(0, 1, 0x11);
        DisableGroupControlWidget(0, 1, 0xF);
        DisableGroupControlWidget(0, 1, 0x13);
        DisableGroupControlWidget(0, 1, 0x12);
        EnableGroup(5);
        DisableGroupControlWidget(5, rider + 1, 2);
        manager->m_lights[0].m_emitter->SetColor(off);
        manager->m_lights[1].m_emitter->SetColor(off);
        manager->m_lights[1].m_emitter->m_direction = _D3DVECTOR(0.0f, 1.0f, 0.0f);
    }
}


// ---- src/JetskiUIPageCharSelect/BioRequested.cpp
class UIManager;



static inline void Gray(_D3DCOLORVALUE &c, float level)
{
    c.r = c.g = c.b = level;
    c.a = 0.0f;
}







// Toggle the character biography: dim the scene lights while it shows.
void JetskiUIPageCharSelect::BioRequested()
{
    JetskiUIManMain *state = ((JetskiUIManMain *)m_manager);
    _D3DCOLORVALUE black, dim, normal;
    Gray(black, 0.0f);
    Gray(dim, 30.0f);
    Gray(normal, 125.0f);
    switch (m_bioState) {
    case 0:
        m_bioState = 2;
        DisableGroupControlWidget(0, 1, 0x13);
        EnableGroup(5);
        state->m_lights[0].m_emitter->SetColor(black);
        state->m_lights[1].m_emitter->SetColor(black);
        state->m_lights[1].m_emitter->m_direction = _D3DVECTOR(0.0f, 1.0f, 0.0f);
        break;
    case 2:
        m_bioState = 0;
        EnableGroupControlWidget(0, 1, 0x13);
        DisableGroup(5);
        state->m_lights[0].m_emitter->SetColor(dim);
        state->m_lights[1].m_emitter->SetColor(normal);
        state->m_lights[1].m_emitter->m_direction = _D3DVECTOR(-0.234182f, -0.9188457f, -0.3176188f);
        break;
    }
}


// ---- src/JetskiUIPageCharSelect/Transition.cpp
extern "C" char *strcpy(char *dst, const char *src);

class Motion;

extern MotionLibrary *LpMotionLibrary;


class UIWidget;







// Plays the chosen rider's "selected" animation.
int JetskiUIPageCharSelect::Transition(int direction)
{
    int rider;
    JetskiWidgetCharacter *widget;

    if (direction == 1) {
        PlayPageSound(1);
    } else {
        m_bioState = 2;
        BioRequested();
        rider = GetGroupControlID(2) - 1;
        widget = (JetskiWidgetCharacter *)GetWidgetPtr(2, rider + 1, 2);
        GetWidgetPtr(2, rider + 1, 1);
        if (m_x178) {
            char name[32];
            // An unknown rider leaves name unset (as in retail).
            switch (rider) {
            case 0:
                strcpy(name, "derrick_selected");
                break;
            case 4:
                strcpy(name, "juana_selected");
                break;
            case 2:
                strcpy(name, "andreas_selected");
                break;
            case 3:
                strcpy(name, "umi_selected");
                break;
            case 1:
                strcpy(name, "britt_selected");
                break;
            case 5:
                strcpy(name, "sebastian_selected");
                break;
            case 6:
                strcpy(name, "jeremy_selected");
                break;
            case 7:
                strcpy(name, "ammanra_selected");
                break;
            case 8:
                strcpy(name, "steve_selected");
                break;
            }
            widget->m_character->TransitionToMotion(LpMotionLibrary->MotionOfName(name, 1), 0.4f, 0);
        }
    }
    return 1;
}


// ---- src/JetskiUIPageCharSelect/ProcessTransition.cpp
class UIManager;

class UIWidget;
class MotionFilter;









int JetskiUIPageCharSelect::ProcessTransition(float dt)
{
    JetskiUIManMain *state = ((JetskiUIManMain *)m_manager);
    if (state->m_direction == 2) {
        int characterDone = 1;
        int seadooDone = 1;
        int selected = GetGroupControlID(2) - 1;
        JetskiWidgetCharacter *character = (JetskiWidgetCharacter *)GetWidgetPtr(2, selected + 1, 2);
        JetskiWidgetSeadoo *seadoo = (JetskiWidgetSeadoo *)GetWidgetPtr(2, selected + 1, 1);
        if (m_x178) {
            if (!character->m_character->m_x10) {
                characterDone = 0;
                character->m_character->TickBlendedFrame(dt, 0, 0);
            }
            if (state->m_riderSounds->m_playing)
                seadooDone = 0;
            if (characterDone && seadooDone) {
                character->DeleteCharacter();
                seadoo->DeleteSeadoo();
                return 1;
            }
            return 0;
        }
        character->DeleteCharacter();
        seadoo->DeleteSeadoo();
        return characterDone;
    }
    return 1;
}


// ---- src/JetskiUIPageCharSelect/SwitchSkinNext.cpp
class UIManager;

class UIWidget;



class SkinOptionsView {
public:
    char pad0[0x100];
    int m_character[2];                 // +0x100, per player
    char pad108[0x234 - 0x108];
    int m_skin[5];                      // +0x234
    int m_skin2[5];                     // +0x248
    char pad25C[0x6BC - 0x25C];
    int m_skinAvailable[2][8];          // +0x6BC, per player
    char pad6FC[0x7F8 - 0x6FC];
    int m_currentSkin[2];               // +0x7F8, per player
};






void JetskiUIPageCharSelect::SwitchSkinNext(int player, int slot)
{
    SkinOptionsView *options = (SkinOptionsView *)LpGame->m_gameOptions;
    if (!options->m_character[player])
        return;
    int id = GetGroupControlID(2);
    JetskiWidgetCharacter *character = (JetskiWidgetCharacter *)GetWidgetPtr(2, id, 2);
    JetskiWidgetSeadoo *seadoo = (JetskiWidgetSeadoo *)GetWidgetPtr(2, id, 1);
    int current = options->m_currentSkin[player];
    int skin = current;
    do {
        skin++;
        if (skin == 8) {
            skin = 0;
        } else if (skin == 7 && !options->m_skinAvailable[player][7]) {
            skin++;
            if (skin == 8)
                skin = 0;
        }
    } while (!options->m_skinAvailable[player][skin]);
    if (skin != current) {
        SetRing(skin);
        character->ChangeSkin(skin);
        seadoo->ChangeSkin(skin);
        ((JetskiUIManMain *)m_manager)->LoadRequestedMaterials();
    }
    options->m_skin[slot] = skin;
    options->m_skin2[slot] = skin;
}


// ---- src/JetskiUIPageCharSelect/SwitchSkinPrevious.cpp
class UIManager;

class UIWidget;





// The page's UI manager; only the slot used here.




void JetskiUIPageCharSelect::SwitchSkinPrevious(int player, int slot)
{
    SkinOptionsView *options = (SkinOptionsView *)LpGame->m_gameOptions;
    if (!options->m_character[player])
        return;
    int id = GetGroupControlID(2);
    JetskiWidgetCharacter *character = (JetskiWidgetCharacter *)GetWidgetPtr(2, id, 2);
    JetskiWidgetSeadoo *seadoo = (JetskiWidgetSeadoo *)GetWidgetPtr(2, id, 1);
    int current = options->m_currentSkin[player];
    int skin = current;
    do {
        skin--;
        if (skin < 0) {
            skin = 7;
        } else if (skin == 7 && !options->m_skinAvailable[player][7]) {
            skin--;
            if (skin < 0)
                skin = 7;
        }
    } while (!options->m_skinAvailable[player][skin]);
    if (skin != current) {
        SetRing(skin);
        character->ChangeSkin(skin);
        seadoo->ChangeSkin(skin);
        ((JetskiUIManMain *)m_manager)->LoadRequestedMaterials();
    }
    options->m_skin[slot] = skin;
    options->m_skin2[slot] = skin;
}


// ---- src/JetskiUIPageDiff/ctor.cpp

JetskiUIPageDiff::JetskiUIPageDiff(int enabled)
    : JetskiUIPage(enabled)
{
}


// ---- src/JetskiUIPageDiff/dtor.cpp


JetskiUIPageDiff::~JetskiUIPageDiff()
{
}


// ---- src/JetskiUIPageDiff/ResetPage.cpp



void JetskiUIPageDiff::ResetPage()
{
    SetPlayerFocus(1, 1, 0);
}


// ---- src/JetskiUIPageDiff/ProcessUserInput.cpp
struct CareerProgress {
    char pad0[0x4];
    int m_difficulty;               // +0x04
};







int JetskiUIPageDiff::ProcessUserInput(int player, int input)
{
    ProcessInputSounds(player, input);
    JetskiGameOptions *options = LpGame->m_gameOptions;
    JetskiUIManMain *state = ((JetskiUIManMain *)m_manager);
    switch (player) {
    case 0:
        switch (input) {
        case 4:
            state->m_state = 7;
            ((JetskiUIManMain *)m_manager)->m_nextPage = 0x1B;
            state->m_selectMode = 0;
            options->m_career->m_difficulty = GetGroupControlID(1);
            options->SetCareerPath(GetGroupControlID(1), LpGame->m_gameOptions->m_x5FC0);
            break;
        case 6:
            break;
        }
        break;
    }
    return 1;
}


// ---- src/JetskiUIPageDiff/ProcessInputSounds.cpp


void JetskiUIPageDiff::ProcessInputSounds(int player, int input)
{
    switch (player) {
    case 0:
        switch (input) {
        case 4:
            PlayPageSound(2);
            break;
        case 2:
            PlayPageSound(5);
            break;
        case 3:
            PlayPageSound(5);
            break;
        }
        break;
    }
}


// ---- src/JetskiUIPageDiff/Transition.cpp

int JetskiUIPageDiff::Transition(int)
{
    return 1;
}


// ---- src/JetskiUIPageDiff/ProcessTransition.cpp

int JetskiUIPageDiff::ProcessTransition(float)
{
    return 1;
}


// ---- src/JetskiUIPageGOptions/ctor.cpp


JetskiUIPageGOptions::JetskiUIPageGOptions(int enabled)
    : JetskiUIPage(enabled)
{
}


// ---- src/JetskiUIPageGOptions/dtor.cpp


JetskiUIPageGOptions::~JetskiUIPageGOptions()
{
}


// ---- src/JetskiUIPageGOptions/ResetPage.cpp


struct ControllerInfo {
#ifdef SPLASHDOWN_PC
    char pad0[__builtin_offsetof(JetskiPageSettings, m_gameOptions)];
#else
    char pad0[0x14];
#endif
    int *m_controller;                  // +0x14
};






void JetskiUIPageGOptions::ResetPage()
{
    JetskiGameOptions *options;
    JetskiUIManMain *state;
    state = ((JetskiUIManMain *)m_manager);
    options = LpGame->m_gameOptions;
    state->m_waterEffect->Pause(0);
    if (options->m_x50 == 1)
        SetGroupFocus(2, 1);
    else
        SetGroupFocus(2, 2);
    switch (options->m_x834) {
    case 0:
        SetGroupFocus(6, 1);
        break;
    case 1:
        SetGroupFocus(6, 2);
        break;
    case 2:
        SetGroupFocus(6, 3);
        break;
    case 3:
        SetGroupFocus(6, 4);
        break;
    }
    SetGroupFocus(4, 1);
    SetGroupFocus(5, 1);
    SetGroupFocus(3, 1);
    if (LpGame->m_gameOptions->m_x4A4C)
        SetGroupFocus(1, 1);
    else
        SetGroupFocus(1, 2);
    switch (*((ControllerInfo *)options->m_pageSettings)->m_controller) {
    case 3:
        SetPlayerFocus(3, 1, 0);
        break;
    case 4:
        SetPlayerFocus(4, 1, 0);
        break;
    case 5:
        SetPlayerFocus(5, 1, 0);
        break;
    default:
        if (LpGame->m_gameOptions->m_x4A4C)
            SetPlayerFocus(1, 1, 0);
        else
            SetPlayerFocus(1, 2, 0);
        break;
    }
}


// ---- src/JetskiUIPageGOptions/ProcessUserInput.cpp
class UIControl;









int JetskiUIPageGOptions::ProcessUserInput(int player, int input)
{
    ProcessInputSounds(player, input);
    JetskiGameOptions *options = LpGame->m_gameOptions;
    int group = GetJoystickGroupID(player);
    switch (player) {
    case 0:
        switch (input) {
        case 4:
            *((ControllerInfo *)options->m_pageSettings)->m_controller = group;
            switch (group) {
            case 3:
                options->m_x5FD4 = 6;
                ((JetskiUIManMain *)m_manager)->m_state = 7;
                ((JetskiUIManMain *)m_manager)->m_nextPage = 12;
                break;
            case 4:
                options->m_x5FD4 = 6;
                ((JetskiUIManMain *)m_manager)->m_state = 7;
                ((JetskiUIManMain *)m_manager)->m_nextPage = 17;
                break;
            case 5:
                ((JetskiUIManMain *)m_manager)->m_state = 7;
                ((JetskiUIManMain *)m_manager)->m_nextPage = 11;
                break;
            }
            break;
        case 0:
            switch (group) {
            case 1:
                m_groups[group]->PreviousControl(0);
                switch (GetGroupControlID(1)) {
                case 1:
                    options->m_x4A4C = 1;
                    break;
                case 2:
                    options->m_x4A4C = 0;
                    break;
                }
                break;
            case 2:
                m_groups[group]->PreviousControl(0);
                switch (GetGroupControlID(2)) {
                case 1:
                    options->m_x50 = 1;
                    break;
                case 2:
                    options->m_x50 = 2;
                    break;
                }
                break;
            case 6:
                m_groups[group]->PreviousControl(0);
                switch (GetGroupControlID(6)) {
                case 1:
                    options->m_x834 = 0;
                    break;
                case 2:
                    options->m_x834 = 1;
                    break;
                case 3:
                    options->m_x834 = 2;
                    break;
                case 4:
                    options->m_x834 = 3;
                    break;
                }
                break;
            }
            break;
        case 1:
            switch (group) {
            case 1:
                m_groups[group]->NextControl(0);
                switch (GetGroupControlID(1)) {
                case 1:
                    options->m_x4A4C = 1;
                    break;
                case 2:
                    options->m_x4A4C = 0;
                    break;
                }
                break;
            case 2:
                m_groups[group]->NextControl(0);
                switch (GetGroupControlID(2)) {
                case 1:
                    options->m_x50 = 1;
                    break;
                case 2:
                    options->m_x50 = 2;
                    break;
                }
                break;
            case 6:
                m_groups[group]->NextControl(0);
                switch (GetGroupControlID(6)) {
                case 1:
                    options->m_x834 = 0;
                    break;
                case 2:
                    options->m_x834 = 1;
                    break;
                case 3:
                    options->m_x834 = 2;
                    break;
                case 4:
                    options->m_x834 = 3;
                    break;
                }
                break;
            }
            break;
        case 6:
            ((JetskiUIManMain *)m_manager)->m_state = 7;
            ((JetskiUIManMain *)m_manager)->m_nextPage = 5;
            break;
        }
        break;
    }
    return 1;
}


// ---- src/JetskiUIPageGOptions/ProcessInputSounds.cpp



void JetskiUIPageGOptions::ProcessInputSounds(int player, int input)
{
    switch (player) {
    case 0:
        switch (input) {
        case 6:
            PlayPageSound(3);
            break;
        case 2:
        case 3:
            PlayPageSound(5);
            break;
        case 0:
        case 1:
            switch (GetJoystickGroupID(player)) {
            case 1:
            case 2:
            case 6:
                PlayPageSound(6);
                break;
            }
            break;
        }
        break;
    }
}


// ---- src/JetskiUIPageGOptions/Transition.cpp
int JetskiUIPageGOptions::Transition(int direction)
{
    if (direction == 1)
        PlayPageSound(1);
    return 1;
}


// ---- src/JetskiUIPageGOptions/ProcessTransition.cpp

int JetskiUIPageGOptions::ProcessTransition(float)
{
    return 1;
}


// ---- src/JetskiUIPageLoadSave/ctor.cpp

class ScreenText;



JetskiUIPageLoadSave::JetskiUIPageLoadSave(int enabled)
    : JetskiUIPage(enabled)
{
    m_pageTextManager = 0;
    m_slotTexts = 0;
    m_titleText = 0;
    m_infoTexts = 0;
    m_promptTexts = 0;
    m_font = 0;
    m_controlsMoving = 0;
    m_groupMoving = 0;
    m_confirming = 0;
    m_confirmNo = 0;
    m_selectedCareer = 0;
}


// ---- src/JetskiUIPageLoadSave/dtor.cpp
extern "C" void free(void *p);




JetskiUIPageLoadSave::~JetskiUIPageLoadSave()
{
    int i;
    for (i = 0; i < 5; i++)
        delete m_slotTexts[i];
    free(m_slotTexts);
    m_slotTexts = 0;
    for (i = 0; i < 6; i++)
        delete m_infoTexts[i];
    free(m_infoTexts);
    m_infoTexts = 0;
    for (i = 0; i < 3; i++)
        delete m_promptTexts[i];
    free(m_promptTexts);
    m_promptTexts = 0;
    delete m_titleText;
    m_titleText = 0;
}


// ---- src/JetskiUIPageLoadSave/Create.cpp
// GetString is only inlined at a deeper inline depth than the default.
#pragma inline_depth(5)

extern "C" void *malloc(unsigned int size);
extern "C" int sprintf(char *buf, const char *fmt, ...);




// Retail's ScreenText::SetJustification takes its argument by const
// reference (the shared ScreenText.h takes it by value): the global is then
// loaded inside the setter, which allocates its register before the text's.
static inline void SetJustification(ScreenText *text, const int &justification)
{
    text->m_justification = justification;
    text->m_positionDirty = 1;
}

// Colours go through block-local copies of the colour and then of the text
// pointer (ScreenText::SetColor copies again): that is what gives retail's
// register choice for the colour constants and the texts.
#define SET_COLOR(text, value)              \
    {                                       \
        unsigned int color = value;         \
        ScreenText *colored = (text);       \
        colored->SetColor(color);           \
    }

// Creates the load/save page: the five slot numbers, six info texts, three
// prompt texts and the title.
JetskiUIPageLoadSave *JetskiUIPageLoadSave::Create(char *name, char *fileName, UIManager *manager,
                                                   ScreenTextManager *textManager)
{
    char number[0x100];
    int i;
    UIPage::Create(name, fileName, manager, textManager);
    ScreenTextFontItem *item = LpGame->m_fonts->GetScreenTextFont("info_c");
    m_font = item ? item->m_font : 0;
    m_pageTextManager = textManager;
    m_slotTexts = (ScreenText **)malloc(5 * sizeof(ScreenText *));
    m_infoTexts = (ScreenText **)malloc(6 * sizeof(ScreenText *));
    m_promptTexts = (ScreenText **)malloc(3 * sizeof(ScreenText *));
    for (i = 0; i < 5; i++) {
        m_slotTexts[i] = new ScreenText(40, 0);
        sprintf(number, "%d", i + 1);
        m_slotTexts[i]->Create(2, number);
        m_slotTexts[i]->SetScale(1.286f);
        SET_COLOR(m_slotTexts[i], 0xFFFFFFFF)
        switch (i) {
        case 0:
            SET_COLOR(m_slotTexts[i], 0xFFFFDE00)
            m_slotTexts[i]->SetPosition(353.0f, 89.0f);
            break;
        case 1:
            m_slotTexts[i]->SetPosition(385.0f, 89.0f);
            break;
        case 2:
            m_slotTexts[i]->SetPosition(417.0f, 89.0f);
            break;
        case 3:
            m_slotTexts[i]->SetPosition(447.0f, 89.0f);
            break;
        case 4:
            m_slotTexts[i]->SetPosition(479.0f, 89.0f);
            break;
        }
        m_pageTextManager->PositionText(m_slotTexts[i], ScreenTextManager::CenterHorizontalJustification, m_font);
    }
    for (i = 0; i < 6; i++) {
        m_infoTexts[i] = new ScreenText(40, 0);
        m_infoTexts[i]->SetString("");
        SET_COLOR(m_infoTexts[i], 0xFFFFFFFF)
        switch (i) {
        case 0: {
            ScreenTextFontItem *title = LpGame->m_fonts->GetScreenTextFont("title_b");
            m_infoTexts[i]->SetFont(title ? title->m_font : 0);
            SetJustification(m_infoTexts[i], ScreenTextManager::CenterBothJustification);
            m_infoTexts[i]->SetPosition(417.0f, 150.0f);
            break;
        }
        case 1:
            m_infoTexts[i]->SetScale(1.286f);
            m_infoTexts[i]->SetFont(m_font);
            SetJustification(m_infoTexts[i], ScreenTextManager::CenterHorizontalJustification);
            m_infoTexts[i]->SetPosition(417.0f, 180.0f);
            break;
        case 2:
            m_infoTexts[i]->SetScale(1.286f);
            m_infoTexts[i]->SetFont(m_font);
            SetJustification(m_infoTexts[i], ScreenTextManager::CenterHorizontalJustification);
            m_infoTexts[i]->SetPosition(417.0f, 200.0f);
            break;
        case 3:
            m_infoTexts[i]->SetScale(1.286f);
            m_infoTexts[i]->SetFont(m_font);
            SetJustification(m_infoTexts[i], ScreenTextManager::CenterHorizontalJustification);
            m_infoTexts[i]->SetPosition(417.0f, 240.0f);
            break;
        case 4:
            m_infoTexts[i]->SetScale(1.286f);
            m_infoTexts[i]->SetFont(m_font);
            SetJustification(m_infoTexts[i], ScreenTextManager::CenterHorizontalJustification);
            m_infoTexts[i]->SetPosition(417.0f, 260.0f);
            break;
        case 5:
            m_infoTexts[i]->SetScale(1.286f);
            m_infoTexts[i]->SetFont(m_font);
            SetJustification(m_infoTexts[i], ScreenTextManager::CenterHorizontalJustification);
            m_infoTexts[i]->SetPosition(417.0f, 300.0f);
            break;
        }
    }
    for (i = 0; i < 3; i++) {
        m_promptTexts[i] = new ScreenText(40, 0);
        m_promptTexts[i]->SetScale(1.286f);
        SET_COLOR(m_promptTexts[i], 0xFFFFFFFF)
        switch (i) {
        case 0:
            m_promptTexts[i]->SetString(LpGame->m_uiText->GetString(2));
            m_promptTexts[i]->SetPosition(417.0f, 140.0f);
            break;
        case 1:
            m_promptTexts[i]->SetString(LpGame->m_uiText->GetString(0));
            m_promptTexts[i]->SetPosition(417.0f, 180.0f);
            SET_COLOR(m_slotTexts[i], 0xFFFFFFFF)    // retail: the slot text, not the prompt
            break;
        case 2:
            m_promptTexts[i]->SetString(LpGame->m_uiText->GetString(1));
            m_promptTexts[i]->SetPosition(417.0f, 200.0f);
            SET_COLOR(m_promptTexts[i], 0xFFFFDE00)
            break;
        }
        m_pageTextManager->PositionText(m_promptTexts[i], ScreenTextManager::CenterHorizontalJustification, m_font);
    }
    m_titleText = new ScreenText(40, 0);
    m_titleText->SetScale(1.286f);
    SET_COLOR(m_titleText, 0xFFFFFFFF)
    m_titleText->SetPosition(418.0f, 282.0f);
    m_titleText->SetString(LpGame->m_uiText->GetString(0x101));
    m_pageTextManager->PositionText(m_titleText, ScreenTextManager::CenterHorizontalJustification, m_font);
    return this;
}


// ---- src/JetskiUIPageLoadSave/PrepareGeometry.cpp




int JetskiUIPageLoadSave::PrepareGeometry()
{
    if (m_font) {
        TextureMap *t = m_font->m_texture;
        if (10.0f > t->m_usage)
            t->m_usage = 10.0f;
    }
    return GameObject::PrepareGeometry();
}


// ---- src/JetskiUIPageLoadSave/Render3D.cpp
#pragma inline_depth(5)

extern "C" char *strncpy(char *dst, const char *src, unsigned int n);

extern "C" int sprintf(char *buf, const char *fmt, ...);




class ScreenTextFont;




class SaveView {
public:
    char pad0[0x4A60];
    JetskiCareer m_savedCareers[1]; // +0x4A60
};





class ScreenTextFont;


int JetskiUIPageLoadSave::Render3D()
{
    char buffer[0x100];
    int slot;
    JetskiCareer *career;
    JetskiUIManMain *manager;
    CareerRacer *player;
    int i;
    int playerIndex;
    unsigned int color;

    slot = m_selectedCareer;
    manager = ((JetskiUIManMain *)m_manager);
    player = 0;
    career = &(*(SaveView **)&LpGame->m_gameOptions)->m_savedCareers[slot];

    for (i = 0; i < 5; i++)
        m_pageTextManager->AddText(m_slotTexts[i], m_font);
    if (m_confirming) {
        for (i = 0; i < 3; i++)
            m_pageTextManager->AddText(m_promptTexts[i], m_font);
    } else if (!career->m_valid) {
        m_pageTextManager->AddText(m_titleText, m_font);
    } else {
        playerIndex = 0;
        for (i = 0; i < 5; i++) {
            if (!career->m_racers[i].m_enabled) {
                playerIndex = i;
                player = &career->m_racers[i];
                i = 5;  // ends the search (retail does not break)
            }
        }
        for (i = 0; i < 6; i++) {
            switch (i) {
            case 0:
                m_infoTexts[i]->SetString(player->m_name);
                manager->GetTeamColor(playerIndex, &color);
                m_infoTexts[i]->SetColor(color);
                break;
            case 1:
                m_infoTexts[i]->SetString(LpGame->m_uiText->GetString(284));
                break;
            case 2:
                sprintf(buffer, "%d %s %d", career->m_currentCircuit + 1, LpGame->m_uiText->GetString(286),
                        career->m_numEvents);
                m_infoTexts[i]->SetString(buffer);
                break;
            case 3:
                m_infoTexts[i]->SetString(LpGame->m_uiText->GetString(285));
                break;
            case 4:
                sprintf(buffer, "%d", player->m_place);
                m_infoTexts[i]->SetString(buffer);
                break;
            case 5:
                switch (career->m_difficulty) {
                case 3:
                    m_infoTexts[i]->SetString(LpGame->m_uiText->GetString(247));
                    break;
                case 2:
                    m_infoTexts[i]->SetString(LpGame->m_uiText->GetString(246));
                    break;
                case 1:
                    m_infoTexts[i]->SetString(LpGame->m_uiText->GetString(245));
                    break;
                }
                break;
            }
            m_pageTextManager->PositionText(m_infoTexts[i]);
            m_pageTextManager->AddText(m_infoTexts[i], 0);
        }
    }
    return GameObject::Render3D();
}


// ---- src/JetskiUIPageLoadSave/ResetPage.cpp

struct SaveSlotInfo {
    int m_slot;                     // +0x0
};

struct MemoryCardState {
#ifdef SPLASHDOWN_PC
    char pad0[__builtin_offsetof(JetskiPageSettings, m_loadSave)];
#else
    char pad0[0x18];
#endif
    SaveSlotInfo *m_slotInfo;       // +0x18
};



struct PromptTexts {
    int m_x0;
    ScreenText *m_yes;              // +0x4
    ScreenText *m_no;               // +0x8
};



void JetskiUIPageLoadSave::ResetPage()
{
    m_slotTexts[m_selectedCareer]->SetColor(0xFFFFFFFF);
    m_selectedCareer = ((MemoryCardState *)LpGame->m_gameOptions->m_pageSettings)->m_slotInfo->m_slot;
    m_slotTexts[m_selectedCareer]->SetColor(0xFFFFDE00);
    m_confirmNo = 0;
    SetGroupFocus(2, 1);
    SetPlayerFocus(1, 1, 0);
    m_promptTexts[1]->SetColor(0xFFFFFFFF);
    m_promptTexts[2]->SetColor(0xFFFFDE00);
}


// ---- src/JetskiUIPageLoadSave/ProcessUserInput.cpp
struct CareerSave {                 // 0x3D0 bytes
    int m_used;                     // +0x00
    char pad4[0x3D0 - 0x4];
};

struct ControllerInfo2 {
#ifdef SPLASHDOWN_PC
    char pad0[__builtin_offsetof(JetskiPageSettings, m_loadSave)];
#else
    char pad0[0x18];
#endif
    int *m_saveSlot;                // +0x18
};

class SaveView2 {
public:
    void DefaultSaveSlot(int slot);

    char pad0[0x4A60];
    CareerSave m_saves[5];          // +0x4A60
#ifdef SPLASHDOWN_PC
    // JetskiGameOptions's m_career, m_x5FC0, m_pageSettings: 0xC-0x10 further
    // on the PC (its pointers from m_team on)
    char pad5D70[__builtin_offsetof(JetskiGameOptions, m_career) - 0x5D70];
    CareerSave *m_newSave;
    int m_newSaveSlot;
    char pad5FC4[__builtin_offsetof(JetskiGameOptions, m_pageSettings) - __builtin_offsetof(JetskiGameOptions, m_x5FC0) - 4];
#else
    char pad5D70[0x5FBC - 0x5D70];
    CareerSave *m_newSave;          // +0x5FBC
    int m_newSaveSlot;              // +0x5FC0
    char pad5FC4[0x5FD8 - 0x5FC4];
#endif
    ControllerInfo2 *m_controllerInfo;   // +0x5FD8
};






#define WHITE  0xFFFFFFFF
#define YELLOW 0xFFFFDE00


int JetskiUIPageLoadSave::ProcessUserInput(int player, int input)
{
    ProcessInputSounds(player, input);
    CareerSave *save;
    SaveView2 *options;
    options = (SaveView2 *)LpGame->m_gameOptions;
    save = &options->m_saves[m_selectedCareer];
    if (m_confirming && player == 0) {
        // Overwrite-this-save prompt.
        switch (input) {
        case 4:
            EnableGroupControlWidget(0, 1, 0x11);
            EnableGroupControlWidget(0, 1, 0x13);
            m_confirming = 0;
            if (m_confirmNo) {
                m_confirmNo = 0;
                ((JetskiGameOptions *)options)->DefaultSaveSlot(m_selectedCareer);
            }
            SetPlayerFocus(1, 1, 0);
            return 1;
        case 2:
        case 3:
            if (m_confirmNo) {
                m_promptTexts[1]->SetColor(WHITE);
                m_promptTexts[2]->SetColor(YELLOW);
            } else {
                m_promptTexts[1]->SetColor(YELLOW);
                m_promptTexts[2]->SetColor(WHITE);
            }
            m_confirmNo = !m_confirmNo;
            break;
        }
        return 0;
    }
    switch (player) {
    case 0:
        switch (input) {
        case 4:
            switch (GetJoystickGroupID(0)) {
            case 1:
                *options->m_controllerInfo->m_saveSlot = m_selectedCareer;
                if (save->m_used) {
                    SetSavedCareer(m_selectedCareer);
                    ((JetskiUIManMain *)m_manager)->m_state = 7;
                    ((JetskiUIManMain *)m_manager)->m_nextPage = 0x19;
                } else {
                    options->m_newSave = save;
                    options->m_newSaveSlot = m_selectedCareer;
                    ((JetskiUIManMain *)m_manager)->m_state = 7;
                    ((JetskiUIManMain *)m_manager)->m_nextPage = 0x17;
                }
                break;
            case 2:
                if (save->m_used) {
                    DisableGroupControlWidget(0, 1, 0x11);
                    DisableGroupControlWidget(0, 1, 0x13);
                    m_confirming = 1;
                    m_confirmNo = 0;
                    m_promptTexts[1]->SetColor(WHITE);
                    m_promptTexts[2]->SetColor(YELLOW);
                }
                break;
            }
            break;
        case 6:
            ((JetskiUIManMain *)m_manager)->m_state = 7;
            ((JetskiUIManMain *)m_manager)->m_nextPage = 1;
            break;
        case 0:
            if (GetJoystickGroupID(0) == 1) {
                m_slotTexts[m_selectedCareer]->SetColor(WHITE);
                m_selectedCareer--;
                if (m_selectedCareer < 0)
                    m_selectedCareer = 4;
                m_slotTexts[m_selectedCareer]->SetColor(YELLOW);
            }
            break;
        case 1:
            if (GetJoystickGroupID(0) == 1) {
                m_slotTexts[m_selectedCareer]->SetColor(WHITE);
                m_selectedCareer++;
                if (m_selectedCareer > 4)
                    m_selectedCareer = 0;
                m_slotTexts[m_selectedCareer]->SetColor(YELLOW);
            }
            break;
        }
        return 1;
    }
    return 0;
}


// ---- src/JetskiUIPageLoadSave/Transition.cpp





int JetskiUIPageLoadSave::Transition(int direction)
{
    if (((JetskiUIManMain *)m_manager)->m_direction == 1)
        PlayPageSound(1);
    m_controlsMoving = 1;
    for (int i = 1; i <= m_numGroups; i++)
        m_groups[i]->m_others[0]->Transition(direction);
    m_groupMoving = 1;
    m_groups[0]->Transition(direction);
    return 1;
}


// ---- src/JetskiUIPageLoadSave/ProcessTransition.cpp



int JetskiUIPageLoadSave::ProcessTransition(float dt)
{
    if (m_controlsMoving) {
        int done = 1;
        for (int i = 1; i <= m_numGroups; i++) {
            if (!m_groups[i]->m_others[0]->ProcessTransition(dt))
                done = 0;
        }
        if (done)
            m_controlsMoving = 0;
    }
    if (m_groupMoving && m_groups[0]->ProcessTransition(dt))
        m_groupMoving = 0;
    return !m_controlsMoving && !m_groupMoving;
}


// ---- src/JetskiUIPageLoadSave/SetSavedCareer.cpp
struct CareerRace {
    char pad0[0xC];
    char m_name[0xC];               // +0x0C
    int m_completed;                // +0x18
    char pad1C[0x40 - 0x1C];
};

struct CareerSave2 {
    CareerRace m_races[5];          // +0x000
    char pad140[0x3D0 - 0x140];
};

struct SaveView3 {
    char pad0[0x4A60];
    CareerSave2 m_careers[5];        // +0x4A60
#ifdef SPLASHDOWN_PC
    char pad5D70[__builtin_offsetof(JetskiGameOptions, m_team) - 0x5D70];  // m_team .. m_x5FC0 as on the PS2, 8-byte pointers
#else
    char pad5D70[0x5FB4 - 0x5D70];
#endif
    char *m_currentRaceName;        // +0x5FB4
    int m_currentRace;              // +0x5FB8
    CareerSave2 *m_currentCareer;    // +0x5FBC
    int m_saveSlot;                 // +0x5FC0
};



void JetskiUIPageLoadSave::SetSavedCareer(int slot)
{
    (*(SaveView3 **)&LpGame->m_gameOptions)->m_currentCareer = &(*(SaveView3 **)&LpGame->m_gameOptions)->m_careers[m_selectedCareer];
    (*(SaveView3 **)&LpGame->m_gameOptions)->m_saveSlot = slot;
    CareerSave2 *career = (*(SaveView3 **)&LpGame->m_gameOptions)->m_currentCareer;
    // Resume at the first race not yet completed.
    for (int i = 0; i < 5; i++) {
        if (career->m_races[i].m_completed == 0) {
            (*(SaveView3 **)&LpGame->m_gameOptions)->m_currentRaceName = career->m_races[i].m_name;
            (*(SaveView3 **)&LpGame->m_gameOptions)->m_currentRace = i;
            i = 5;
        }
    }
}


// ---- src/JetskiUIPageMain/ctor.cpp

JetskiUIPageMain::JetskiUIPageMain(int enabled)
    : JetskiUIPage(enabled)
{
    m_cardPresent = 1;
    m_x174 = 0;
    m_idleTime = 30.0f;
}


// ---- src/JetskiUIPageMain/dtor.cpp


JetskiUIPageMain::~JetskiUIPageMain()
{
}


// ---- src/JetskiUIPageMain/Tick.cpp
#pragma fast_fptosi on

extern "C" char *strcpy(char *dst, const char *src);














int JetskiUIPageMain::Tick(float dt)
{
    JetskiGameOptions *options = LpGame->m_gameOptions;
    JetskiUIManMain *state = ((JetskiUIManMain *)m_manager);
    if (state->m_state)
        return GameObject::Tick(dt);
    int present = LpGame->m_controls->m_joysticks[1]->IsControllerPluggedIn();
    if (present != m_cardPresent || m_x174) {
        m_x174 = 0;
        m_cardPresent = present;
        JetskiWidgetButton *button = (JetskiWidgetButton *)GetWidgetPtr(1, 4, 1);
        if (present) {
            TurnGroupControlOnOff(1, 1, 4);
            if (GetGroupControlID(1) == 4)
                button->SetColor(0, 0xFF, 0xFF, 0xFF);
            else
                button->SetColor(0xEC, 0x85, 0x31, 0xFF);
        } else {
            if (GetGroupControlID(1) == 4)
                SetPlayerFocus(1, 3, 0);
            TurnGroupControlOnOff(0, 1, 4);
            button->SetColor(0xC8, 0xC8, 0xC8, 0xFF);
        }
    }
    if (m_idleTime <= 0.0f) {
        m_musicFade += dt;
        int volume = (1.0f - m_musicFade / 1.2f) * options->m_musicVolume;
        if (volume > 0) {
            (*(MusicPlayer **)&state->m_musicGroup)->SetVolume(volume);
        } else {
            state->m_tuneSound->Stop(0);
            strcpy(options->m_creditsVideo, "Videos\\Attract.pss");
            options->m_x89C = 1;
            state->m_nextPage = 10;
            state->m_state = 7;
        }
    } else {
        m_idleTime -= dt;
    }
    return JetskiUIPage::Tick(dt);
}
#pragma fast_fptosi reset


// ---- src/JetskiUIPageMain/ResetPage.cpp




struct ControllerInfo3 {
#ifdef SPLASHDOWN_PC
    char pad0[__builtin_offsetof(JetskiPageSettings, m_main)];
#else
    char pad0[0xC];
#endif
    int *m_controller;                  // +0xC
};






void JetskiUIPageMain::ResetPage()
{
    JetskiGameOptions *options;
    JetskiUIManMain *state;
    state = ((JetskiUIManMain *)m_manager);
    options = LpGame->m_gameOptions;
    if (!state->m_tuneSound->IsPlaying()) {
        (*(MusicPlayer **)&state->m_musicGroup)->SetVolume(LpGame->m_gameOptions->m_musicVolume);
        state->m_tuneSound->Play(0, 0);
    }
    if (state->m_waterEffect) {
        state->m_waterEffect->Pause(0);
        state->m_waterEffect->EnableAll();
    }
    SetPlayerFocus(1, *((ControllerInfo3 *)options->m_pageSettings)->m_controller, 0);
    m_x174 = 1;
    m_idleTime = 30.0f;
    m_musicFade = 0;
}


// ---- src/JetskiUIPageMain/ProcessUserInput.cpp







int JetskiUIPageMain::ProcessUserInput(int player, int input)
{
    ProcessInputSounds(player, input);
    JetskiUIManMain *state;
    JetskiGameOptions *options;
    options = LpGame->m_gameOptions;
    state = ((JetskiUIManMain *)m_manager);
    switch (player) {
    case 0:
        switch (input) {
        case 4:
            options->m_pageSettings->m_main->m_x0 = GetGroupControlID(1);
            switch (GetGroupControlID(1)) {
            case 1:
                state->m_state = 7;
                state->m_nextPage = 4;
                break;
            case 2:
                options->m_pageSettings->ResetCareer();
                ((JetskiUIManMain *)m_manager)->m_state = 7;
                state->m_nextPage = 0x16;
                options->m_splitScreen = 0;
                break;
            case 3:
                state->m_x2E0 = 1;
                ((JetskiUIManMain *)m_manager)->m_state = 7;
                ((JetskiUIManMain *)m_manager)->m_nextPage = 2;
                options->m_splitScreen = 0;
                break;
            case 4:
                state->m_x2E0 = 2;
                state->m_state = 7;
                state->m_nextPage = 2;
                options->m_splitScreen = 1;
                break;
            case 5:
                options->m_pageSettings->ResetOptions();
                ((JetskiUIManMain *)m_manager)->m_state = 7;
                state->m_nextPage = 5;
                options->m_splitScreen = 0;
                break;
            }
            break;
        case 2:
            m_idleTime = 30.0f;
            m_musicFade = 0;
            break;
        case 3:
            m_idleTime = 30.0f;
            m_musicFade = 0;
            break;
        }
        break;
    }
    return 1;
}


// ---- src/JetskiUIPageMain/ProcessInputSounds.cpp


void JetskiUIPageMain::ProcessInputSounds(int player, int input)
{
    switch (player) {
    case 0:
        switch (input) {
        case 4:
            PlayPageSound(2);
            break;
        case 2:
            PlayPageSound(5);
            break;
        case 3:
            PlayPageSound(5);
            break;
        }
        break;
    }
}


// ---- src/JetskiUIPageMain/Transition.cpp




int JetskiUIPageMain::Transition(int direction)
{
    if (((JetskiUIManMain *)m_manager)->m_direction == 1)
        PlayPageSound(1);
    m_transitionB = 1;
    m_groups[1]->Transition(direction);
    m_transitionA = 1;
    m_groups[0]->Transition(direction);
    return 1;
}


// ---- src/JetskiUIPageMain/ProcessTransition.cpp


int JetskiUIPageMain::ProcessTransition(float dt)
{
    if (m_transitionB && m_groups[1]->ProcessTransition(dt))
        m_transitionB = 0;
    if (m_transitionA && m_groups[0]->ProcessTransition(dt))
        m_transitionA = 0;
    return !m_transitionB && !m_transitionA;
}


// ---- src/JetskiUIPageName/ctor.cpp

JetskiUIPageName::JetskiUIPageName(int enabled) : JetskiUIPage(enabled)
{
    m_font = 0;
}


// ---- src/JetskiUIPageName/dtor.cpp
extern "C" void free(void *p);





JetskiUIPageName::~JetskiUIPageName()
{
    if (m_cursor)
        delete m_cursor;
    if (m_name)
        delete m_name;
    if (m_letters) {
        for (int i = 0; i < 78; i++)
            delete m_letters[i];
        free(m_letters);
    }
}


// ---- src/JetskiUIPageName/Create.cpp
extern "C" void *malloc(unsigned int size);
extern "C" char *strncpy(char *dst, const char *src, unsigned int n);

class ScreenTextFont;






class UIManager;




JetskiUIPageName *JetskiUIPageName::Create(char *a, char *b, UIManager *manager, ScreenTextManager *textManager)
{
    UIPage::Create(a, b, manager, textManager);
    ScreenTextFontItem *item = LpGame->m_fonts->GetScreenTextFont("info_b");
    m_font = item ? item->m_font : 0;
    m_pageTextManager = textManager;
    m_letters = (ScreenText **)malloc(78 * PS2_PTR_SIZE);
    m_name = new ScreenText(0x28, 0);
    m_name->SetColor(0xFFFFFFFF);
    m_name->SetString("");
    m_name->SetColor(0xFFFFFFFF);
    m_name->SetPosition(225.0f, 114.0f);
    m_name->SetJustification(ScreenTextManager::LeftJustification);
    m_name->SetScale(1.286f);
    m_name->SetFont(m_font);
    m_pageTextManager->PositionText(m_name);
    m_cursor = new ScreenText(0x28, 0);
    m_cursor->SetString("_");
    m_cursor->SetColor(0xFFFFDE00);
    m_cursor->SetScale(1.286f);
    m_cursor->SetFont(m_font);
    m_cursor->SetJustification(ScreenTextManager::LeftJustification);
    SetupKeyBoard();
    return this;
}


// ---- src/JetskiUIPageName/KeyDownHandler.cpp

class KeyInfo;

int KEYDOWN_HANDLER_TEST(AssignableActionIndex action, DeviceType device, ProcessNode *node, unsigned int mods);



int JetskiUIPageName::KeyDownHandler(ProcessNode *node, KeyInfo *key)
{
    if (node->m_controller)
        return 0;
    SetSelectionWhite();
    if (KEYDOWN_HANDLER_TEST((AssignableActionIndex)3, DEVICE_JOYSTICK, node, 0x80000000)) {
        if (m_selection == 78 || m_selection == 74 || m_selection == 70) {
            m_selection = m_column;
        } else if (m_selection >= 53 && m_selection < 66) {
            m_selection = 78;
        } else {
            m_selection += 13;
            if (m_selection > 78)
                m_selection -= 78;
        }
    } else if (KEYDOWN_HANDLER_TEST((AssignableActionIndex)2, DEVICE_JOYSTICK, node, 0x80000000)) {
        if (m_selection == 78 || m_selection == 74 || m_selection == 70) {
            m_selection = m_column + 52;
        } else if (m_selection > 0 && m_selection < 14) {
            m_selection = 78;
        } else {
            m_selection -= 13;
            if (m_selection <= 0)
                m_selection += 78;
        }
    } else if (KEYDOWN_HANDLER_TEST((AssignableActionIndex)0, DEVICE_JOYSTICK, node, 0x80000000)) {
        switch (m_selection) {
        case 78:
            m_selection = 74;
            break;
        case 74:
            m_selection = 70;
            break;
        case 70:
            m_selection = 78;
            break;
        default:
            m_column--;
            if (m_column <= 0)
                m_column = 13;
            m_selection--;
            if (m_selection % 13 == 0)
                m_selection += 13;
            break;
        }
    } else if (KEYDOWN_HANDLER_TEST((AssignableActionIndex)1, DEVICE_JOYSTICK, node, 0x80000000)) {
        switch (m_selection) {
        case 78:
            m_selection = 70;
            break;
        case 74:
            m_selection = 78;
            break;
        case 70:
            m_selection = 74;
            break;
        default:
            m_column++;
            if (m_column > 13)
                m_column = 1;
            m_selection++;
            if (m_selection % 13 == 1)
                m_selection -= 13;
            break;
        }
    }
    SetSelectionYellow();
    return GameObject::KeyDownHandler(node, key);
}


// ---- src/JetskiUIPageName/Render3D.cpp


class ScreenTextFont;




int JetskiUIPageName::Render3D()
{
    m_pageTextManager->AddText(m_name, m_font);
    for (int i = 0; i < 78; i++)
        m_pageTextManager->AddText(m_letters[i], m_font);
    if (m_showCursor) {
        // The cursor follows the name, which holds up to 8 characters.
        if ((int)strlen(m_name->m_text) < 8) {
            int width = m_pageTextManager->GetStringWidthInPixels(m_name);
            float x = m_name->m_position.x + width;
            float y = m_name->m_position.y;
            ScreenText *cursor = m_cursor;
            cursor->m_position.x = x;
            cursor->m_position.y = y;
            cursor->m_positionDirty = 1;
            cursor->m_dirty = 1;
            m_pageTextManager->PositionText(m_cursor);
            m_pageTextManager->AddText(m_cursor, 0);
        }
    }
    return GameObject::Render3D();
}


// ---- src/JetskiUIPageName/Tick.cpp


// Blinks the name-entry cursor twice a second.
int JetskiUIPageName::Tick(float dt)
{
    m_cursorTimer += dt;
    if (m_cursorTimer >= 0.5f) {
        m_cursorTimer -= 0.5f;
        m_showCursor = !m_showCursor;
    }
    return GameObject::Tick(dt);
}


// ---- src/JetskiUIPageName/ResetPage.cpp
class UIManager;

extern "C" char *strncpy(char *dst, const char *src, unsigned int n);






void JetskiUIPageName::ResetPage()
{
    JetskiUIManMain *state = ((JetskiUIManMain *)m_manager);
    SetSelectionWhite();
    m_selection = 1;
    SetSelectionYellow();
    m_column = 1;
    m_cursorTimer = 0;
    m_showCursor = 1;
    ScreenText *name = m_name;
    name->m_positionDirty = 1;
    strncpy(name->m_text, "", name->m_maxChars);
    name->m_text[name->m_maxChars] = 0;
    name->m_x5D_bit4 = 1;
    name->m_dirty = 1;
    name->m_cellsDirty = 1;
    switch (state->m_mode) {
    case 0:
        SetPlayerFocus(1, 1, 0);
        break;
    case 1:
        SetPlayerFocus(1, 2, 0);
        break;
    }
}


// ---- src/JetskiUIPageName/ProcessUserInput.cpp



int JetskiUIPageName::ProcessUserInput(int player, int input)
{
    ProcessInputSounds(player, input);
    switch (player) {
    case 0:
        switch (input) {
        case 4:
            ProcessLetter();
            break;
        }
        break;
    }
    return 1;
}


// ---- src/JetskiUIPageName/ProcessInputSounds.cpp


void JetskiUIPageName::ProcessInputSounds(int player, int input)
{
    switch (player) {
    case 0:
        switch (input) {
        case 4:
            PlayPageSound(2);
            break;
        case 2:
            PlayPageSound(6);
            break;
        case 3:
            PlayPageSound(6);
            break;
        case 0:
            PlayPageSound(6);
            break;
        case 1:
            PlayPageSound(6);
            break;
        }
        break;
    }
}


// ---- src/JetskiUIPageName/Transition.cpp

int JetskiUIPageName::Transition(int direction)
{
    return 1;
}

int JetskiUIPageName::ProcessTransition(float dt)
{
    return 1;
}


// ---- src/JetskiUIPageName/SetupKeyBoard.cpp
// GetString is only inlined at a deeper inline depth than the default.
#pragma inline_depth(5)
#pragma fast_fptosi on





class ScreenTextFont;



// Colours go through a block-local copy at the call (and SetColor copies
// again, as in the shared ScreenText.h), which orders the colour load as
// retail does.
#define SET_COLOR(text, value)          \
    {                                   \
        unsigned int color = value;     \
        (text)->SetColor(color);        \
    }

// Builds the on-screen keyboard: 6 rows of 13 keys.
void JetskiUIPageName::SetupKeyBoard()
{
    char *done = LpGame->m_uiText->GetString(0x102);
    char *space = LpGame->m_uiText->GetString(0x103);
    char *back = LpGame->m_uiText->GetString(0x104);
    char *keys[78] = {
        "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M",
        "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z",
        "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m",
        "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z",
        "!", ".", "#", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0",
        "", "", "", "", done,
        "", "", "", space,
        "", "", "", back,
    };
    for (int i = 0; i < 78; i++) {
        int col = i % 13;
        int row = i / 13;
        m_letters[i] = new ScreenText(0x28, 0);
        int justification = ScreenTextManager::CenterHorizontalJustification;
        if (i + 1 == 78 || i + 1 == 74 || i + 1 == 70)
            justification = ScreenTextManager::RightJustification;
        m_letters[i]->Create(strlen(keys[i]), keys[i]);
        SET_COLOR(m_letters[i], 0xFFFFFFFF)
        m_letters[i]->SetPosition(col * 33 + 123, row * 34 + 150);
        m_letters[i]->SetScale(1.286f);
        m_pageTextManager->PositionText(m_letters[i], justification, m_font);
    }
    m_selection = 1;
    SetSelectionYellow();
}
#pragma fast_fptosi reset


// ---- src/JetskiUIPageName/SetSelectionYellow.cpp


void JetskiUIPageName::SetSelectionYellow()
{
    ScreenText *letter = m_letters[m_selection - 1];
    letter->m_scale = 1.4f;
    letter->m_dirty = 1;
    letter->m_positionDirty = 1;
    letter = m_letters[m_selection - 1];
    letter->m_color = 0xFFFFDE00;
    letter->m_x5D_bit4 = 1;
}


// ---- src/JetskiUIPageName/SetSelectionWhite.cpp


void JetskiUIPageName::SetSelectionWhite()
{
    ScreenText *letter = m_letters[m_selection - 1];
    letter->m_scale = 1.286f;
    letter->m_dirty = 1;
    letter->m_positionDirty = 1;
    letter = m_letters[m_selection - 1];
    letter->m_color = 0xFFFFFFFF;
    letter->m_x5D_bit4 = 1;
}


// ---- src/JetskiUIPageName/ProcessLetter.cpp
class UIManager;


extern "C" char *strcpy(char *dst, const char *src);
extern "C" char *strcat(char *dst, const char *src);
extern "C" int strcmp(const char *a, const char *b);
extern "C" int strcasecmp(const char *a, const char *b);


struct ProfileName {
    char m_name[9];
};

struct SaveSlots {
    int m_x0;                           // +0x00
    char pad4[0xC - 0x4];
    char m_names[5][0x40];              // +0x0C
};

class NameView {
public:
    void CheatUnlockEverything();
    void CheatFullPerformance();
    void CheatUnlockAllTracks();
    void CheatUnlockAllCharacters();
    void CheatUnlockAllMovies();
    void CheatAllOutAI();
    void CheatHardToNormalAI();
    void CheatNoJoust();

    char pad0[0x54];
    int m_x54;                          // +0x54
#ifdef SPLASHDOWN_PC
    char pad58[__builtin_offsetof(JetskiGameOptions, m_team) - 0x58];
    ProfileName *m_profileName;
    char pad5FB8[__builtin_offsetof(JetskiGameOptions, m_career) - __builtin_offsetof(JetskiGameOptions, m_team) - sizeof(void *)];
    SaveSlots *m_slots;
    char pad5FC0[__builtin_offsetof(JetskiGameOptions, m_x5FD0) - __builtin_offsetof(JetskiGameOptions, m_career) - sizeof(void *)];
#else
    char pad58[0x5FB4 - 0x58];
    ProfileName *m_profileName;         // +0x5FB4
    char pad5FB8[0x5FBC - 0x5FB8];
    SaveSlots *m_slots;                 // +0x5FBC
    char pad5FC0[0x5FD0 - 0x5FC0];
#endif
    int m_x5FD0;                        // +0x5FD0
};






void JetskiUIPageName::ProcessLetter()
{
    char name[16];
    NameView *options;
    JetskiUIManMain *state;
    ScreenText *text;
    options = (NameView *)LpGame->m_gameOptions;
    state = ((JetskiUIManMain *)m_manager);
    text = m_name;
    strcpy(name, text->m_text);
    int length = strlen(name);
    switch (m_selection) {
    case 70:
        if (length >= 8) {
            SetSelectionWhite();
            m_selection = 78;
            SetSelectionYellow();
            return;
        }
        strcat(name, " ");
        break;
    case 74:
        if (length > 0)
            name[length - 1] = 0;
        break;
    case 78:
        switch (state->m_mode) {
        case 0:
            if (!strcasecmp(name, "Storm"))
                strcpy(options->m_slots->m_names[0], options->m_profileName->m_name);
            else if (!strcasecmp(name, "Typhoon"))
                strcpy(options->m_slots->m_names[1], options->m_profileName->m_name);
            else if (!strcasecmp(name, "Riptide"))
                strcpy(options->m_slots->m_names[2], options->m_profileName->m_name);
            else if (!strcasecmp(name, "Neptune"))
                strcpy(options->m_slots->m_names[3], options->m_profileName->m_name);
            else if (!strcasecmp(name, "Tsunami"))
                strcpy(options->m_slots->m_names[4], options->m_profileName->m_name);
#line 4714 "JetskiUIPages.cpp"
            STRCPY(options->m_profileName->m_name, name);
            options->m_slots->m_x0 = 1;
            options->m_x5FD0 = 25;
            ((JetskiUIManMain *)m_manager)->m_nextPage = 29;
            ((JetskiUIManMain *)m_manager)->m_state = 7;
            return;
        case 1: {
            int cheat = 1;
            if (!strcmp(name, "LaPinata"))
                ((JetskiGameOptions *)options)->CheatUnlockEverything();
            else if (!strcmp(name, "PMeterGo"))
                ((JetskiGameOptions *)options)->CheatFullPerformance();
            else if (!strcmp(name, "Passport"))
                ((JetskiGameOptions *)options)->CheatUnlockAllTracks();
            else if (!strcmp(name, "AllChar"))
                ((JetskiGameOptions *)options)->CheatUnlockAllCharacters();
            else if (!strcmp(name, "Festival"))
                ((JetskiGameOptions *)options)->CheatUnlockAllMovies();
            else if (!strcmp(name, "AllOutAI"))
                ((JetskiGameOptions *)options)->CheatAllOutAI();
            else if (!strcmp(name, "Hobble"))
                ((JetskiGameOptions *)options)->CheatHardToNormalAI();
            else if (!strcmp(name, "TopBird"))
                ((JetskiGameOptions *)options)->CheatNoJoust();
            else if (!strcmp(name, "IBelieve"))
                options->m_x54 = 0;
            else if (!strcmp(name, "F18"))
                options->m_x54 = 1;
            else if (!strcmp(name, "SEADOO"))
                options->m_x54 = 2;
            else
                cheat = 0;
            if (cheat)
                PlayPageSound(4);
            else
                PlayPageSound(7);
            ((JetskiUIManMain *)m_manager)->m_state = 7;
            ((JetskiUIManMain *)m_manager)->m_nextPage = 5;
            return;
        }
        }
        break;
    default:
        if (length >= 8) {
            SetSelectionWhite();
            m_selection = 78;
            SetSelectionYellow();
            return;
        }
        strcat(name, m_letters[m_selection - 1]->m_text);
        break;
    }
    m_name->SetString(name);
    m_showCursor = 1;
    m_cursorTimer = 0;
}


// ---- src/JetskiUIPageOptions/ctor.cpp

JetskiUIPageOptions::JetskiUIPageOptions(int enabled)
    : JetskiUIPage(enabled)
{
    m_pad2Plugged = 1;
    m_refresh = 0;
    m_selectHeld = 0;
}


// ---- src/JetskiUIPageOptions/dtor.cpp


JetskiUIPageOptions::~JetskiUIPageOptions()
{
}


// ---- src/JetskiUIPageOptions/Tick.cpp
struct DIEFFECTINFOA;
// A joypad; vptr at +0x18. Slots in retail order (retail vtable).







class UIWidget;






int JetskiUIPageOptions::Tick(float dt)
{
    JetskiUIManMain *state = ((JetskiUIManMain *)m_manager);
    int plugged = LpGame->m_controls->m_joysticks[1]->IsControllerPluggedIn();
    if (plugged != m_pad2Plugged || m_refresh) {
        m_refresh = 0;
        m_pad2Plugged = plugged;
        JetskiWidgetButton *button = (JetskiWidgetButton *)GetWidgetPtr(1, 5, 1);
        if (plugged) {
            TurnGroupControlOnOff(1, 1, 5);
            if (GetGroupControlID(1) == 5)
                button->SetColor(0, 0xFF, 0xFF, 0xFF);
            else
                button->SetColor(0xEC, 0x85, 0x31, 0xFF);
        } else {
            if (GetGroupControlID(1) == 5)
                SetPlayerFocus(1, 4, 0);
            TurnGroupControlOnOff(0, 1, 5);
            button->SetColor(0xC8, 0xC8, 0xC8, 0xFF);
        }
    }
    if (state->m_state) {
        return GameObject::Tick(dt);
    }
    if (state->m_cheatKey) {
        state->m_cheatKey->CheckKeyInputs(dt);
        if (!state->m_cheatKey->CheckCheatActivation(dt) && !state->m_x2F8) {
            PlayPageSound(2);
            state->m_x2F8 = 1;
            state->m_mode = 1;
            ((JetskiUIManMain *)m_manager)->m_state = 7;
            ((JetskiUIManMain *)m_manager)->m_nextPage = 0x18;
        } else {
            state->m_x2F8 = 0;
        }
    }
    KeyInfo info;
    if (LpGame->m_controls->m_joysticks[0]->IsKeyDown((AssignableActionIndex)0xD, 0x3F, &info))
        m_selectHeld = 1;
    else
        m_selectHeld = 0;
    return JetskiUIPage::Tick(dt);
}


// ---- src/JetskiUIPageOptions/ResetPage.cpp
struct PlayerInfo {
    int m_x0;               // +0x0
};

struct PlayerSlot {
#ifdef SPLASHDOWN_PC
    char pad0[__builtin_offsetof(JetskiPageSettings, m_options)];
#else
    char pad0[0x10];
#endif
    PlayerInfo *m_info;     // +0x10
};





void JetskiUIPageOptions::ResetPage()
{
    JetskiGameOptions *data = LpGame->m_gameOptions;
    m_refresh = 1;
    SetPlayerFocus(1, ((PlayerSlot *)data->m_pageSettings)->m_info->m_x0, 0);
}


// ---- src/JetskiUIPageOptions/ProcessUserInput.cpp
extern "C" char *strcpy(char *dst, const char *src);









int JetskiUIPageOptions::ProcessUserInput(int player, int input)
{
    JetskiUIManMain *state = ((JetskiUIManMain *)m_manager);
    JetskiGameOptions *options = LpGame->m_gameOptions;
    if (m_selectHeld)
        return 0;
    ProcessInputSounds(player, input);
    switch (player) {
    case 0:
        switch (input) {
        case 4:
            options->m_pageSettings->m_options->m_x0 = GetGroupControlID(1);
            switch (GetGroupControlID(1)) {
            case 0:
                break;
            case 1:
                LpGame->m_gameOptions->m_pageSettings->m_videos->Reset();
                ((JetskiUIManMain *)m_manager)->m_state = 7;
                ((JetskiUIManMain *)m_manager)->m_nextPage = 0x1E;
                break;
            case 2:
                strcpy(options->m_creditsVideo, "Videos\\Credits.pss");
                options->m_x89C = 5;
                ((JetskiUIManMain *)m_manager)->m_state = 7;
                ((JetskiUIManMain *)m_manager)->m_nextPage = 10;
                m_fadeTime = 0;
                break;
            case 3:
                ((JetskiUIManMain *)m_manager)->m_state = 7;
                ((JetskiUIManMain *)m_manager)->m_nextPage = 8;
                break;
            case 4:
                state->m_numPlayers = 1;
                ((JetskiUIManMain *)m_manager)->m_state = 7;
                ((JetskiUIManMain *)m_manager)->m_nextPage = 9;
                break;
            case 5:
                state->m_numPlayers = 2;
                ((JetskiUIManMain *)m_manager)->m_state = 7;
                ((JetskiUIManMain *)m_manager)->m_nextPage = 9;
                break;
            case 6:
                options->m_pageSettings->m_gameOptions->Reset();
                ((JetskiUIManMain *)m_manager)->m_state = 7;
                ((JetskiUIManMain *)m_manager)->m_nextPage = 6;
                break;
            case 7:
                ((JetskiUIManMain *)m_manager)->m_state = 7;
                ((JetskiUIManMain *)m_manager)->m_nextPage = 7;
                break;
            }
            break;
        case 6:
            ((JetskiUIManMain *)m_manager)->m_state = 7;
            ((JetskiUIManMain *)m_manager)->m_nextPage = 1;
            break;
        }
        break;
    }
    return 1;
}


// ---- src/JetskiUIPageOptions/ProcessInputSounds.cpp


void JetskiUIPageOptions::ProcessInputSounds(int player, int input)
{
    switch (player) {
    case 0:
        switch (input) {
        case 4:
            PlayPageSound(2);
            break;
        case 6:
            PlayPageSound(3);
            break;
        case 2:
            PlayPageSound(5);
            break;
        case 3:
            PlayPageSound(5);
            break;
        }
        break;
    }
}


// ---- src/JetskiUIPageOptions/Transition.cpp




int JetskiUIPageOptions::Transition(int type)
{
    if (((JetskiUIManMain *)m_manager)->m_direction == 1)
        PlayPageSound(1);
    m_x168 = 1;
    m_groups[1]->Transition(type);
    m_x16C = 1;
    m_groups[0]->Transition(type);
    return 1;
}


// ---- src/JetskiUIPageVideos/ProcessTransition.cpp
#pragma fast_fptosi on







int JetskiUIPageOptions::ProcessTransition(float dt)
{
    JetskiUIManager *state = ((JetskiUIManMain *)m_manager);
    if (state->m_nextPage == 10 && state->m_direction == 2) {
        // Fade the music out over 1.2 seconds, then stop the video.
        m_fadeTime += dt;
        int volume = (int)((1.0f - m_fadeTime / 1.2f) * (float)LpGame->m_gameOptions->m_musicVolume);
        if (volume > 0) {
            (*(MusicPlayer **)&state->m_musicGroup)->SetVolume(volume);
            return 0;
        }
        state->m_tuneSound->Stop(0);
        return 1;
    }
    (*(MusicPlayer **)&state->m_musicGroup)->SetVolume(LpGame->m_gameOptions->m_musicVolume);
    if (!state->m_tuneSound->IsPlaying())
        state->m_tuneSound->Play(0, 0);
    return 1;
}
#pragma fast_fptosi reset


// ---- src/JetskiUIPageRecords/ctor.cpp



JetskiUIPageRecords::JetskiUIPageRecords(int enabled)
    : JetskiUIPage(enabled)
{
    m_font = 0;
    m_labels = 0;
    m_values = 0;
    m_labels2 = 0;
    m_values2 = 0;
}


// ---- src/JetskiUIPageRecords/dtor.cpp
extern "C" void free(void *p);




JetskiUIPageRecords::~JetskiUIPageRecords()
{
    int i;
    for (i = 0; i < 6; i++) {
        if (m_labels[i])
            delete m_labels[i];
        if (m_values[i])
            delete m_values[i];
    }
    free(m_labels);
    free(m_values);
    m_labels = 0;
    m_values = 0;
    for (i = 0; i < 6; i++) {
        if (m_labels2[i])
            delete m_labels2[i];
        if (m_values2[i])
            delete m_values2[i];
    }
    free(m_labels2);
    free(m_values2);
    m_labels2 = 0;
    m_values2 = 0;
}


// ---- src/JetskiUIPageRecords/Create.cpp
extern "C" void *malloc(unsigned int size);

class ScreenTextFont;






class UIManager;



JetskiUIPageRecords *JetskiUIPageRecords::Create(char *a, char *b, UIManager *manager, ScreenTextManager *textManager)
{
    UIPage::Create(a, b, manager, textManager);
    ScreenTextFontItem *item = LpGame->m_fonts->GetScreenTextFont("info_b");
    m_font = item ? item->m_font : 0;
    m_pageTextManager = textManager;
    m_labels = (ScreenText **)malloc(6 * sizeof(ScreenText *));
    m_values = (ScreenText **)malloc(6 * sizeof(ScreenText *));
    m_labels2 = (ScreenText **)malloc(6 * sizeof(ScreenText *));
    m_values2 = (ScreenText **)malloc(6 * sizeof(ScreenText *));
    int i;
    for (i = 0; i < 6; i++) {
        m_labels[i] = new ScreenText(0x28, 0);
        m_values[i] = new ScreenText(0x28, 0);
        m_labels[i]->SetColor(0xFFFFFFFF);
        m_labels[i]->SetPosition(332.0f, 85 + i * 20);
        m_labels[i]->SetFont(m_font);
        m_labels[i]->SetJustification(ScreenTextManager::LeftJustification);
    }
    for (i = 0; i < 6; i++) {
        m_labels2[i] = new ScreenText(0x28, 0);
        m_values2[i] = new ScreenText(0x28, 0);
        m_labels2[i]->SetColor(0xFFFFFFFF);
        m_labels2[i]->SetPosition(332.0f, 249 + i * 20);
        m_labels2[i]->SetFont(m_font);
        m_labels2[i]->SetJustification(ScreenTextManager::LeftJustification);
    }
    return this;
}


// ---- src/JetskiUIPageRecords/PrepareGeometry.cpp




int JetskiUIPageRecords::PrepareGeometry()
{
    if (m_font) {
        TextureMap *t = m_font->m_texture;
        if (10.0f > t->m_usage)
            t->m_usage = 10.0f;
    }
    return GameObject::PrepareGeometry();
}


// ---- src/JetskiUIPageRecords/Render3D.cpp



int JetskiUIPageRecords::Render3D()
{
    switch (GetGroupControlID(1)) {
    case 1:
        RenderCircuitStats();
        break;
    case 2:
        RenderTrialStats();
        break;
    case 3:
        RenderCountDownStats();
        break;
    }
    return GameObject::Render3D();
}


// ---- src/JetskiUIPageRecords/RenderCircuitStats.cpp
// RenderCircuitStats__19JetskiUIPageRecordsFv  @ 0x00361F40
// NEAR-MISS: 119/468 words, registers only: s-register assignment (retail: this $s0, data $s1,
// records $s4; declaration order has no effect) and the first loop's inline SetPosition
// temporaries two registers higher, as in RenderCountDownStats/RenderTrialStats (parked setter shift).
// Session-state shim added (research/shift): 119 -> 91 words; the notes above predate it.

// Compiler session state: retail compiled
// this file in an mwcc session that had already generated a two-argument runtime
// helper call (64-bit / or %, double arithmetic or compare). mwcc then models
// every later helper call (__floatdisf, fptoui, fptodp, ...) as reading $a0-$a1,
// so temporaries live before such a call skip $a0/$a1. This unused function
// recreates that state; it is not part of the retail file.
static void MWCC_SHIM_TEXT MwccSessionState(long *a) { a[0] = a[0] / a[1]; }

#pragma fast_fptosi on

extern "C" char *strncpy(char *dst, const char *src, unsigned int n);
extern "C" char *strcpy(char *dst, const char *src);
extern "C" int sprintf(char *buf, const char *fmt, ...);

#define FLT_MAX 3.40282347e+38f

class ScreenTextFont;



// One track's best times (0x50 bytes): six names with times.
struct TrialRecords {
    char m_names[6][9];             // +0x00
    float m_times[6];               // +0x38
};

// Circuit records per track (0xA0 bytes): best race times, then best laps.
struct CircuitRecords {
    TrialRecords m_race;            // +0x00
    TrialRecords m_lap;             // +0x50
};

struct RecordsView {
    char pad0[0x8A8];
    CircuitRecords m_circuitRecords[21];        // +0x08A8
    CircuitRecords m_circuitRecords2[1];        // +0x15C8
};




static inline void FormatTime(char *buf, float time)
{
    if (time == 0.0f || time == FLT_MAX) {
        strcpy(buf, "--:--.--");
    } else {
        int minutes;
        if (time < 0.0f)
            time = -1.0f * time;
        if (time >= 5999.99f) {
            minutes = 99;
            time = 59.99f;
        } else {
            for (minutes = 0; time >= 60.0f; minutes++)
                time -= 60.0f;
        }
        sprintf(buf, "%2d:%05.2f", minutes, time);
    }
}

void JetskiUIPageRecords::RenderCircuitStats()
{
    char buf[256];
    RecordsView *data = (RecordsView *)LpGame->m_gameOptions;
    CircuitRecords *records;
    int i;
    if (GetGroupControlID(3) == 3)
        records = &data->m_circuitRecords2[GetGroupControlID(2) - 1];
    else
        records = &data->m_circuitRecords[GetGroupControlID(2) - 1];
    for (i = 0; i < 6; i++) {
        sprintf(buf, "%d. %s", i + 1, records->m_race.m_names[i]);
        m_labels[i]->SetString(buf);
        m_pageTextManager->PositionText(m_labels[i]);
        m_pageTextManager->AddText(m_labels[i], 0);
        m_values[i]->Clone(m_labels[i]);
        m_values[i]->SetPosition(595.0f, m_values[i]->m_position.y);
        FormatTime(buf, records->m_race.m_times[i]);
        m_values[i]->SetString(buf);
        m_values[i]->SetJustification(ScreenTextManager::RightJustification);
        m_pageTextManager->PositionText(m_values[i]);
        m_pageTextManager->AddText(m_values[i], 0);
    }
    for (i = 0; i < 6; i++) {
        sprintf(buf, "%d. %s", i + 1, records->m_lap.m_names[i]);
        m_labels2[i]->SetString(buf);
        m_pageTextManager->PositionText(m_labels2[i]);
        m_pageTextManager->AddText(m_labels2[i], 0);
        m_values2[i]->Clone(m_labels2[i]);
        m_values2[i]->SetPosition(595.0f, m_values2[i]->m_position.y);
        FormatTime(buf, records->m_lap.m_times[i]);
        m_values2[i]->SetString(buf);
        m_values2[i]->SetJustification(ScreenTextManager::RightJustification);
        m_pageTextManager->PositionText(m_values2[i]);
        m_pageTextManager->AddText(m_values2[i], 0);
    }
}
#pragma fast_fptosi reset


// ---- src/JetskiUIPageRecords/RenderTrialStats.cpp
// RenderTrialStats__19JetskiUIPageRecordsFv  @ 0x00362690
// NEAR-MISS: same size, 87/255 words: register colouring only. Retail: data/records $s0,
// this $s3, i $s4, text $s5, and the inline setters use $a2/$a3/$t0/$t1 where this uses
// $a0..$a3 (looks like the inline timeGetTime register shift). Tried declaration orders,
// scopes, out-of-class inline setters.
// Session-state shim added (research/shift): 87 -> 73 words; the notes above predate it.



#pragma fast_fptosi on

extern "C" char *strncpy(char *dst, const char *src, unsigned int n);
extern "C" char *strcpy(char *dst, const char *src);
extern "C" int sprintf(char *buf, const char *fmt, ...);

#define FLT_MAX 3.40282347e+38f

class ScreenTextFont;



// Best times for one track: six names and times.

struct RecordsView2 {
    char pad0[0x22E8];
    TrialRecords m_trialRecords[21];    // +0x22E8
    TrialRecords m_trialRecords2[1];    // +0x2978
};




void JetskiUIPageRecords::RenderTrialStats()
{
    char buf[256];
    RecordsView2 *data = (RecordsView2 *)LpGame->m_gameOptions;
    TrialRecords *records;
    if (GetGroupControlID(3) == 3)
        records = &data->m_trialRecords2[GetGroupControlID(2) - 1];
    else
        records = &data->m_trialRecords[GetGroupControlID(2) - 1];
    for (int i = 0; i < 6; i++) {
        sprintf(buf, "%d. %s", i + 1, records->m_names[i]);
        m_labels[i]->SetString(buf);
        m_pageTextManager->PositionText(m_labels[i]);
        m_pageTextManager->AddText(m_labels[i], 0);
        m_values[i]->Clone(m_labels[i]);
        m_values[i]->SetPosition(595.0f, m_values[i]->m_position.y);
        float time = records->m_times[i];
        if (time == 0.0f || time == FLT_MAX) {
            strcpy(buf, "--:--.--");
        } else {
            int minutes;
            if (time < 0.0f)
                time = -1.0f * time;
            if (time >= 5999.99f) {
                minutes = 99;
                time = 59.99f;
            } else {
                for (minutes = 0; time >= 60.0f; minutes++)
                    time -= 60.0f;
            }
            sprintf(buf, "%2d:%05.2f", minutes, time);
        }
        m_values[i]->SetString(buf);
        m_values[i]->SetJustification(ScreenTextManager::RightJustification);
        m_pageTextManager->PositionText(m_values[i]);
        m_pageTextManager->AddText(m_values[i], 0);
    }
}
#pragma fast_fptosi reset


// ---- src/JetskiUIPageRecords/RenderCountDownStats.cpp

#pragma fast_fptosi on

extern "C" char *strncpy(char *dst, const char *src, unsigned int n);
extern "C" char *strcpy(char *dst, const char *src);
extern "C" int sprintf(char *buf, const char *fmt, ...);

#define FLT_MAX 3.40282347e+38f

class ScreenTextFont;



// One track's best times (0x50 bytes): six names with times.

// Countdown records per track (0xA0 bytes): best times, then six names with
// checkpoint counts.
struct CountDownRecords {
    TrialRecords m_times;           // +0x00
    char m_names[6][9];             // +0x50
    int m_counts[6];                // +0x88
};

struct RecordsView3 {
    char pad0[0x3008];
    CountDownRecords m_countDownRecords[1];     // +0x3008
};




void JetskiUIPageRecords::RenderCountDownStats()
{
    char buf[256];
    RecordsView3 *data = (RecordsView3 *)LpGame->m_gameOptions;
    CountDownRecords *records = &data->m_countDownRecords[GetGroupControlID(2) - 1];
    int i;
    for (i = 0; i < 6; i++) {
        sprintf(buf, "%d. %s", i + 1, records->m_times.m_names[i]);
        m_labels[i]->SetString(buf);
        m_pageTextManager->PositionText(m_labels[i]);
        m_pageTextManager->AddText(m_labels[i], 0);
        m_values[i]->Clone(m_labels[i]);
        m_values[i]->SetPosition(595.0f, m_values[i]->m_position.y);
        float time = records->m_times.m_times[i];
        if (time == 0.0f || time == FLT_MAX) {
            strcpy(buf, "--:--.--");
        } else {
            int minutes;
            if (time < 0.0f)
                time = -1.0f * time;
            if (time >= 5999.99f) {
                time = 59.99f;
                minutes = 99;
            } else {
                for (minutes = 0; time >= 60.0f; minutes++)
                    time -= 60.0f;
            }
            sprintf(buf, "%2d:%05.2f", minutes, time);
        }
        m_values[i]->SetString(buf);
        m_values[i]->SetJustification(ScreenTextManager::RightJustification);
        m_pageTextManager->PositionText(m_values[i]);
        m_pageTextManager->AddText(m_values[i], 0);
    }
    for (i = 0; i < 6; i++) {
        sprintf(buf, "%d. %s", i + 1, records->m_names[i]);
        m_labels2[i]->SetString(buf);
        m_pageTextManager->PositionText(m_labels2[i]);
        m_pageTextManager->AddText(m_labels2[i], 0);
        m_values2[i]->Clone(m_labels2[i]);
        m_values2[i]->SetPosition(595.0f, m_values2[i]->m_position.y);
        sprintf(buf, "%d", records->m_counts[i]);
        m_values2[i]->SetString(buf);
        m_values2[i]->SetJustification(ScreenTextManager::RightJustification);
        m_pageTextManager->PositionText(m_values2[i]);
        m_pageTextManager->AddText(m_values2[i], 0);
    }
}
#pragma fast_fptosi reset


// ---- src/JetskiUIPageRecords/ResetPage.cpp

void JetskiUIPageRecords::ResetPage()
{
    SetGroupFocus(2, 1);
    SetGroupFocus(3, 1);
    SetPlayerFocus(1, 1, 0);
    SetupEventGroups();
}


// ---- src/JetskiUIPageRecords/SetupEventGroups.cpp
struct EventView2 {
    char pad0[0xAC];
    int m_xAC;                          // +0xAC
    char padB0[0x1A4 - 0xB0];
    int m_eventUnlocked[21];            // +0x1A4
};





void JetskiUIPageRecords::SetupEventGroups()
{
    EventView2 *options = (EventView2 *)LpGame->m_gameOptions;
    int mode = GetGroupControlID(1);
    int event = GetGroupControlID(2);
    int group3 = GetGroupControlID(3);
    switch (mode) {
    case 1:
        EnableGroup(2);
        EnableGroup(3);
        if (!options->m_xAC)
            DisableGroupControl(2, 21);
        SetGroupFocus(2, event);
        SetGroupFocus(3, group3);
        SetPlayerFocus(1, 1, 0);
        break;
    case 2:
        EnableGroup(2);
        EnableGroup(3);
        if (!options->m_xAC)
            DisableGroupControl(2, 21);
        SetGroupFocus(2, event);
        SetGroupFocus(3, group3);
        SetPlayerFocus(1, 2, 0);
        break;
    case 3:
        DisableGroup(3);
        EnableGroup(2);
        for (int i = 0; i < 21; i++) {
            if (options->m_eventUnlocked[i])
                EnableGroupControl(2, i + 1);
            else
                DisableGroupControl(2, i + 1);
        }
        if (!options->m_eventUnlocked[event - 1]) {
            for (int j = 0; j < 21; j++) {
                if (options->m_eventUnlocked[j]) {
                    event = j + 1;
                    j = 21;
                }
            }
        }
        SetGroupFocus(2, event);
        SetPlayerFocus(1, 3, 0);
        break;
    }
}


// ---- src/JetskiUIPageRecords/ProcessUserInput.cpp

class UIControl;




int JetskiUIPageRecords::ProcessUserInput(int player, int input)
{
    int handled = 0;
    ProcessInputSounds(player, input);
    switch (player) {
    case 0:
        handled = 1;
        switch (input) {
        case 6:
            ((JetskiUIManMain *)m_manager)->m_state = 7;
            ((JetskiUIManMain *)m_manager)->m_nextPage = 5;
            break;
        case 0:
            switch (GetJoystickGroupID(0)) {
            case 1:
                m_groups[1]->NextControl(0);
                SetupEventGroups();
                break;
            }
            break;
        case 1:
            switch (GetJoystickGroupID(0)) {
            case 1:
                m_groups[1]->PreviousControl(0);
                SetupEventGroups();
                break;
            }
            break;
        }
        break;
    }
    return handled;
}


// ---- src/JetskiUIPageRecords/ProcessInputSounds.cpp


void JetskiUIPageRecords::ProcessInputSounds(int player, int input)
{
    switch (player) {
    case 0:
        switch (input) {
        case 6:
            PlayPageSound(3);
            break;
        case 2:
            PlayPageSound(5);
            break;
        case 3:
            PlayPageSound(5);
            break;
        case 0:
            PlayPageSound(6);
            break;
        case 1:
            PlayPageSound(6);
            break;
        }
        break;
    }
}


// ---- (JetskiUIPageRecords::Transition, ProcessTransition)

int JetskiUIPageRecords::Transition(int direction)
{
    return 1;
}

int JetskiUIPageRecords::ProcessTransition(float dt)
{
    return 1;
}


// ---- src/JetskiUIPageSetup/ctor.cpp

JetskiUIPageSetup::JetskiUIPageSetup(int enabled)
    : JetskiUIPage(enabled)
{
    m_courseUnlocked = 0;
    m_transitioning = 0;
    m_videoDelay = -1.0f;
    m_currentVideo = -1;
    m_alphaMaps[0] = 0;
    m_alphaMaps[1] = 0;
    m_alphaMaps[2] = 0;
    m_alphaMaps[3] = 0;
}


// ---- src/JetskiUIPageSetup/dtor.cpp

JetskiUIPageSetup::~JetskiUIPageSetup()
{
    if (m_alphaMaps[0])
        m_alphaMaps[0]->Release();
    if (m_alphaMaps[1])
        m_alphaMaps[1]->Release();
    if (m_alphaMaps[2])
        m_alphaMaps[2]->Release();
    if (m_alphaMaps[3])
        m_alphaMaps[3]->Release();
}


// ---- src/JetskiUIPageSetup/Create.cpp
extern "C" int sprintf(char *buf, const char *fmt, ...);
extern char UITextureContentDirectory[];

class Palette8;
class TextureMap;
class UIManager;
class ScreenTextManager;





JetskiUIPageSetup *JetskiUIPageSetup::Create(char *a, char *b, UIManager *manager, ScreenTextManager *text)
{
    char name[0x110];
    SetupMap();
    m_alphaRects[0].left = 0x112;
    m_alphaRects[0].right = 0x152;
    m_alphaRects[0].top = 0x37;
    m_alphaRects[0].bottom = 0x77;
    m_alphaRects[1].left = 0x1F3;
    m_alphaRects[1].right = 0x233;
    m_alphaRects[1].top = 0x37;
    m_alphaRects[1].bottom = 0x77;
    m_alphaRects[2].left = 0x112;
    m_alphaRects[2].right = 0x152;
    m_alphaRects[2].top = 0x117;
    m_alphaRects[2].bottom = 0x157;
    m_alphaRects[3].left = 0x1F5;
    m_alphaRects[3].right = 0x235;
    m_alphaRects[3].top = 0x11A;
    m_alphaRects[3].bottom = 0x15A;
    sprintf(name, "%sULAlpha", UITextureContentDirectory);
    m_alphaMaps[0] = GetTextureMapFromResourceName(name, 0x22B8, 0, 0, (_D3DBLEND)5, (_D3DBLEND)6, 0x80, 0xFF00FF, 1);
    sprintf(name, "%sURAlpha", UITextureContentDirectory);
    m_alphaMaps[1] = GetTextureMapFromResourceName(name, 0x22B8, 0, 0, (_D3DBLEND)5, (_D3DBLEND)6, 0x80, 0xFF00FF, 1);
    sprintf(name, "%sBLAlpha", UITextureContentDirectory);
    m_alphaMaps[2] = GetTextureMapFromResourceName(name, 0x22B8, 0, 0, (_D3DBLEND)5, (_D3DBLEND)6, 0x80, 0xFF00FF, 1);
    sprintf(name, "%sBRAlpha", UITextureContentDirectory);
    m_alphaMaps[3] = GetTextureMapFromResourceName(name, 0x22B8, 0, 0, (_D3DBLEND)5, (_D3DBLEND)6, 0x80, 0xFF00FF, 1);
    UIPage::Create(a, b, manager, text);
    return this;
}


// ---- src/JetskiUIPageSetup/Tick.cpp
class KeyInfo;
struct DIEFFECTINFOA;
// A joypad; vptr at +0x18. Slots in retail order (retail vtable).







int JetskiUIPageSetup::Tick(float dt)
{
    JetskiUIManMain *state = ((JetskiUIManMain *)m_manager);
    if (state->m_state) {
        return GameObject::Tick(dt);
    }
    if (state->m_x2E0 == 3) {
        if (!LpGame->m_controls->m_joysticks[1]->IsControllerPluggedIn() ||
            !LpGame->m_controls->m_joysticks[0]->IsControllerPluggedIn()) {
            state->m_state = 7;
            state->m_nextPage = 1;
        }
    }
    if (m_videoDelay > 0.0f) {
        m_videoDelay -= dt;
        if (m_videoDelay < 0.0f)
            StartNewVideo();
    }
    return JetskiUIPage::Tick(dt);
}


// ---- src/JetskiUIPageSetup/Render3D.cpp



int JetskiUIPageSetup::Render3D()
{
    if (m_videoPlayer) {
        LpRenderTarget->Blt(&m_alphaRects[0], m_alphaMaps[0], 0, 0);
        LpRenderTarget->Blt(&m_alphaRects[1], m_alphaMaps[1], 0, 0);
        LpRenderTarget->Blt(&m_alphaRects[2], m_alphaMaps[2], 0, 0);
        LpRenderTarget->Blt(&m_alphaRects[3], m_alphaMaps[3], 0, 0);
        m_videoPlayer->BltCurrentDecodedFrame();
    }
    return 1;
}


// ---- src/JetskiUIPageSetup/ResetPage.cpp
class UIManager;

                                  // size 0x50

struct TrackModes {                     // 0x50 bytes: one ModeSelection per group
    ModeSelection m_sel[5];
};

struct TrackList {
    char pad0[2 * PS2_PTR_SIZE];        // JetskiPageSettings's first two pointers
    TrackModes *m_tracks;       // +0x8
};








void JetskiUIPageSetup::ResetPage()
{
    JetskiGameOptions *options = LpGame->m_gameOptions;
    switch (((JetskiUIManMain *)m_manager)->m_x2E4) {
    case 1:
        m_mode = &((TrackList *)options->m_pageSettings)->m_tracks[2].m_sel[0].m_event;
        m_selections = &((TrackList *)options->m_pageSettings)->m_tracks[0].m_sel[0];
        DisableGroupControl(1, 5);
        SetGroupFocus(6, 1);
        EnableGroupControl(1, 3);
        break;
    case 2:
        m_mode = &((TrackList *)options->m_pageSettings)->m_tracks[2].m_sel[0].m_group3;
        m_selections = &((TrackList *)options->m_pageSettings)->m_tracks[1].m_sel[0];
        DisableGroupControl(1, 3);
        EnableGroupControl(1, 5);
        SetGroupFocus(6, 2);
        break;
    }
    JetskiWidgetLap *widget = (JetskiWidgetLap *)GetWidgetPtr(4, 1, 1);
    widget->m_lap = m_selections->m_track;
    SetupEventGroups();
    CourseLocked();
    m_transitioning = 0;
    m_currentVideo = -1;
    StartNewVideo();
}


// ---- src/JetskiUIPageSetup/ProcessUserInput.cpp
class UIManager;

class UIControl;



struct TrackInfo {
    char pad0[0x4];
    int m_x4;                       // +0x04
};

struct TrackList2 {
    TrackInfo *m_current;           // +0x00
};







int JetskiUIPageSetup::ProcessUserInput(int player, int input)
{
    ProcessInputSounds(player, input);
    JetskiUIManMain *state;
    JetskiGameOptions *options;
    options = LpGame->m_gameOptions;
    state = ((JetskiUIManMain *)m_manager);
    switch (player) {
    case 0:
        switch (input) {
        case 4:
            if (m_courseUnlocked)
                return 0;
            m_transitioning = 1;
            LpGame->m_gameOptions->m_raceType = GetGroupControlID(1);
            state->m_state = 8;
            if (GetGroupControlID(5) == 1)
                options->m_x58 = 1;
            else
                options->m_x58 = 0;
            ((TrackList2 *)LpGame->m_gameOptions->m_pageSettings)->m_current->m_x4 = state->m_x2E4;
            LpGame->m_gameOptions->m_x2C = GetGroupControlID(2) - 1;
            LpGame->m_gameOptions->m_difficulty = GetGroupControlID(3);
            m_selections->m_track = ((JetskiWidgetLap *)GetWidgetPtr(4, 1, 1))->m_lap;
            SetupGameOptions();
            break;
        case 6:
            m_selections->m_track = ((JetskiWidgetLap *)GetWidgetPtr(4, 1, 1))->m_lap;
            state->m_state = 7;
            state->m_nextPage = 2;
            break;
        case 2:
            SelectGroup(player, m_upGroup[GetJoystickGroupID(0) - 1][GetGroupControlID(1) - 1]);
            break;
        case 3:
            SelectGroup(player, m_downGroup[GetJoystickGroupID(0) - 1][GetGroupControlID(1) - 1]);
            break;
        case 0:
            switch (GetJoystickGroupID(player)) {
            case 1:
                m_groups[1]->PreviousControl(0);
                *m_mode = GetGroupControlID(1);
                SetupEventGroups();
                CourseLocked();
                m_videoDelay = 1.5f;
                break;
            case 2: {
                m_groups[2]->PreviousControl(0);
                int event = GetGroupControlID(2);
                m_selections[0].m_event = event;
                m_selections[1].m_event = event;
                m_selections[2].m_event = event;
                m_selections[3].m_event = event;
                m_selections[4].m_event = event;
                CourseLocked();
                m_videoDelay = 1.5f;
                break;
            }
            case 3: {
                m_groups[3]->PreviousControl(0);
                int value = GetGroupControlID(3);
                m_selections[0].m_group3 = value;
                m_selections[1].m_group3 = value;
                m_selections[2].m_group3 = value;
                CourseLocked();
                m_videoDelay = 1.5f;
                break;
            }
            case 5:
                m_groups[5]->PreviousControl(0);
                m_selections[*m_mode - 1].m_group5 = GetGroupControlID(5);
                break;
            }
            break;
        case 1:
            switch (GetJoystickGroupID(player)) {
            case 1:
                m_groups[1]->NextControl(0);
                *m_mode = GetGroupControlID(1);
                SetupEventGroups();
                CourseLocked();
                m_videoDelay = 1.5f;
                break;
            case 2: {
                m_groups[2]->NextControl(0);
                int event = GetGroupControlID(2);
                m_selections[0].m_event = event;
                m_selections[1].m_event = event;
                m_selections[2].m_event = event;
                m_selections[3].m_event = event;
                m_selections[4].m_event = event;
                CourseLocked();
                m_videoDelay = 1.5f;
                break;
            }
            case 3: {
                m_groups[3]->NextControl(0);
                int value = GetGroupControlID(3);
                m_selections[0].m_group3 = value;
                m_selections[1].m_group3 = value;
                m_selections[2].m_group3 = value;
                CourseLocked();
                m_videoDelay = 1.5f;
                break;
            }
            case 5:
                m_groups[5]->NextControl(0);
                m_selections[*m_mode - 1].m_group5 = GetGroupControlID(5);
                break;
            }
            break;
        }
        break;
    }
    return 1;
}


// ---- src/JetskiUIPageSetup/SetupEventGroups.cpp
struct EventView {
    char pad0[0xAC];
    int m_xAC;                          // +0xAC
    char padB0[0x150 - 0xB0];
    int m_x150[21];                     // +0x150, per event: unlocked (mode 5)
    int m_eventUnlocked[21];            // +0x1A4
};







void JetskiUIPageSetup::SetupEventGroups()
{
    EventView *options = (EventView *)LpGame->m_gameOptions;
    int mode = *m_mode - 1;
    int event = m_selections[mode].m_event;
    int group3 = m_selections[mode].m_group3;
    int group5 = m_selections[mode].m_group5;
    int group4 = m_selections[mode].m_track;
    if (group3 == -1) {
        DisableGroup(3);
    } else {
        EnableGroup(3);
        SetGroupFocus(3, group3);
    }
    if (group5 == -1) {
        DisableGroup(5);
    } else {
        EnableGroup(5);
        SetGroupFocus(5, group5);
    }
    if (group4 == -1) {
        DisableGroup(4);
    } else {
        EnableGroup(4);
        SetGroupFocus(4, 1);
    }
    int i;
    for (i = 0; i < 21; i++)
        DisableGroupControl(2, i + 1);
    switch (*m_mode) {
    case 1:
        for (i = 0; i < 21; i++)
            EnableGroupControl(2, i + 1);
        SetGroupFocus(2, event);
        break;
    case 5: {
        int first = -1;
        for (i = 0; i < 21; i++) {
            if (options->m_x150[i]) {
                if (first == -1)
                    first = i + 1;
                EnableGroupControl(2, i + 1);
            }
        }
        if (options->m_x150[event - 1]) {
            SetGroupFocus(2, event);
        } else {
            m_selections[0].m_event = first;
            m_selections[1].m_event = first;
            m_selections[2].m_event = first;
            m_selections[3].m_event = first;
            m_selections[4].m_event = first;
            SetGroupFocus(2, first);
        }
        break;
    }
    case 4: {
        int first = -1;
        for (i = 0; i < 21; i++) {
            if (options->m_eventUnlocked[i]) {
                if (first == -1)
                    first = i + 1;
                EnableGroupControl(2, i + 1);
            }
        }
        if (options->m_eventUnlocked[event - 1]) {
            SetGroupFocus(2, event);
        } else {
            m_selections[0].m_event = first;
            m_selections[1].m_event = first;
            m_selections[2].m_event = first;
            m_selections[3].m_event = first;
            m_selections[4].m_event = first;
            SetGroupFocus(2, first);
        }
        break;
    }
    case 3:
        for (i = 0; i < 21; i++)
            EnableGroupControl(2, i + 1);
        SetGroupFocus(2, event);
        break;
    case 2:
        for (i = 0; i < 21; i++)
            EnableGroupControl(2, i + 1);
        SetGroupFocus(2, event);
        break;
    }
    SetPlayerFocus(1, *m_mode, 0);
    if (!options->m_xAC)
        DisableGroupControl(2, 21);
}


// ---- src/JetskiUIPageSetup/SetupMap.cpp

void JetskiUIPageSetup::SetupMap()
{
    m_upGroup[0][0] = 4;
    m_upGroup[0][1] = 3;
    m_upGroup[0][2] = 5;
    m_upGroup[0][3] = 2;
    m_upGroup[0][4] = 2;
    m_upGroup[1][3] = 1;
    m_upGroup[1][4] = 1;
    m_upGroup[1][2] = 1;
    m_upGroup[1][1] = 1;
    m_upGroup[1][0] = 1;
    m_upGroup[2][2] = 2;
    m_upGroup[2][1] = 2;
    m_upGroup[2][0] = 2;
    m_upGroup[2][3] = -1;
    m_upGroup[2][4] = -1;
    m_upGroup[3][0] = 3;
    m_upGroup[3][3] = -1;
    m_upGroup[3][4] = -1;
    m_upGroup[3][2] = -1;
    m_upGroup[3][1] = -1;
    m_upGroup[4][2] = 3;
    m_upGroup[4][3] = -1;
    m_upGroup[4][4] = -1;
    m_upGroup[4][1] = -1;
    m_upGroup[4][0] = -1;
    m_downGroup[0][3] = 2;
    m_downGroup[0][4] = 2;
    m_downGroup[0][2] = 2;
    m_downGroup[0][1] = 2;
    m_downGroup[0][0] = 2;
    m_downGroup[1][2] = 3;
    m_downGroup[1][1] = 3;
    m_downGroup[1][0] = 3;
    m_downGroup[1][3] = 1;
    m_downGroup[1][4] = 1;
    m_downGroup[2][0] = 4;
    m_downGroup[2][1] = 1;
    m_downGroup[2][2] = 5;
    m_downGroup[2][3] = -1;
    m_downGroup[2][4] = -1;
    m_downGroup[3][0] = 1;
    m_downGroup[3][3] = -1;
    m_downGroup[3][4] = -1;
    m_downGroup[3][2] = -1;
    m_downGroup[3][1] = -1;
    m_downGroup[4][2] = 1;
    m_downGroup[4][3] = -1;
    m_downGroup[4][4] = -1;
    m_downGroup[4][1] = -1;
    m_downGroup[4][0] = -1;
}


// ---- src/JetskiUIPageSetup/CourseLocked.cpp
struct CourseView {
    char pad0[0x58];
    int m_trackLocked[21];          // +0x58
    int m_hardTrackLocked[21];      // +0xAC
    char pad100[0x14C - 0x100];
    int m_x14C[21];                 // +0x14C
};





// The start group (7) is only enabled for an unlocked course. Retail repeats
// this code in every branch.
#define SET_LOCKED(locked)          \
    if (locked) {                   \
        m_courseUnlocked = 0;       \
        DisableGroup(7);            \
    } else {                        \
        m_courseUnlocked = 1;       \
        EnableGroup(7);             \
    }

void JetskiUIPageSetup::CourseLocked()
{
    int course = GetGroupControlID(2);
    CourseView *options = (CourseView *)LpGame->m_gameOptions;
    switch (GetGroupControlID(1)) {
    case 1:
    case 2:
    case 3:
        if (GetGroupControlID(3) == 3) {
            SET_LOCKED(options->m_hardTrackLocked[course])
        } else {
            SET_LOCKED(options->m_trackLocked[course])
        }
        break;
    case 4: {
        int locked = 0;
        switch (course) {
        case 7:
        case 9:
        case 11:
        case 13:
        case 18:
            locked = 1;
            break;
        case 20:
            if (options->m_trackLocked[course])
                locked = 1;
            break;
        }
        SET_LOCKED(locked)
        break;
    }
    case 5:
        SET_LOCKED(options->m_x14C[course] && options->m_trackLocked[course])
        break;
    }
}


// ---- src/JetskiUIPageSetup/ProcessInputSounds.cpp



void JetskiUIPageSetup::ProcessInputSounds(int player, int input)
{
    switch (player) {
    case 0:
        switch (input) {
        case 4:
            if (!m_courseUnlocked) {
                PlayPageSound(4);
                m_fadeTime = 0.0f;
            } else {
                PlayPageSound(7);
            }
            break;
        case 6:
            PlayPageSound(3);
            break;
        case 2:
            PlayPageSound(5);
            break;
        case 3:
            PlayPageSound(5);
            break;
        case 0:
            PlayPageSound(6);
            break;
        case 1:
            PlayPageSound(6);
            break;
        }
        break;
    }
}


// ---- src/JetskiUIPageSetup/Transition.cpp
class UIManager;





int JetskiUIPageSetup::Transition(int direction)
{
    if (((JetskiUIManMain *)m_manager)->m_direction == 1)
        PlayPageSound(1);
    return 1;
}


// ---- src/JetskiUIPageSetup/ProcessTransition.cpp
class UIManager;

#pragma fast_fptosi on  // retail never calls fptosi: float->int is an inline cvt.w.s








int JetskiUIPageSetup::ProcessTransition(float dt)
{
    JetskiUIManMain *state = ((JetskiUIManMain *)m_manager);
    int done = 1;
    if (state->m_direction == 2 && m_transitioning) {
        m_fadeTime += dt;
        int level = (int)((1.0f - m_fadeTime / 1.2f) * LpGame->m_gameOptions->m_musicVolume);
        if (level > 0) {
            (*(MusicPlayer **)&state->m_musicGroup)->SetVolume(level);
            done = 0;
        }
    } else if (state->m_direction == 2) {
        EndVideo();
    }
    return done;
}
#pragma fast_fptosi reset


// ---- src/JetskiUIPageSetup/EndVideo.cpp


void JetskiUIPageSetup::EndVideo()
{
    if (m_videoPlayer) {
        m_videoPlayer->Stop();
        m_videoPlayer->RemoveFromChain();
        m_videoPlayer->Release();
        m_videoPlayer = 0;
    }
}


// ---- src/JetskiUIPageSetup/SetupGameOptions.cpp
class GameSetupView {
public:
    void SetupAIRacers(int count);

    char pad0[0x20];
    int m_gameMode;                 // +0x20
    char pad24[0x30 - 0x24];
    int m_track;                    // +0x30
    int m_numRacers;                // +0x34
    int m_numHumans;                // +0x38
    int m_numAI;                    // +0x3C
    char pad40[0x54 - 0x40];
    int m_trainingType;             // +0x54
    char pad58[0x1FC - 0x58];
    int m_singlePlayer;             // +0x1FC
    char pad200[0x20C - 0x200];
    int m_unk20C;                   // +0x20C
    int m_unk210;                   // +0x210
    char pad214[0x220 - 0x214];
    int m_unk220;                   // +0x220
    int m_unk224;                   // +0x224
    char pad228[0x238 - 0x228];
    int m_unk238;                   // +0x238
    char pad23C[0x24C - 0x23C];
    int m_unk24C;                   // +0x24C
};





void JetskiUIPageSetup::SetupGameOptions()
{
    GameSetupView *options = (GameSetupView *)LpGame->m_gameOptions;
    int humans = (options->m_singlePlayer == 0) ? 2 : 1;
    switch (options->m_gameMode) {
    case 1:
    case 2:
        options->m_track = m_selections->m_track;
        options->m_numRacers = 5;
        options->m_numHumans = humans;
        options->m_numAI = 5 - humans;
        ((JetskiGameOptions *)options)->SetupAIRacers(humans);
        break;
    case 3:
        if (options->m_trainingType == 2) {
            options->m_numRacers = 2;
            options->m_numHumans = 1;
            options->m_numAI = 0;
            options->m_track = 1;
            options->m_singlePlayer = 2;
            options->m_unk210 = options->m_unk20C;
            options->m_unk238 = 0;
            options->m_unk224 = options->m_unk220;
            options->m_unk24C = 0;
        } else {
            options->m_numRacers = 1;
            options->m_numHumans = 1;
            options->m_numAI = 0;
            options->m_track = 1;
        }
        break;
    case 4:
    case 5:
        options->m_numHumans = humans;
        options->m_numRacers = humans;
        options->m_numAI = 0;
        break;
    }
}


// ---- src/JetskiUIPageSetup/StartNewVideo.cpp

extern "C" int sprintf(char *buf, const char *fmt, ...);






void JetskiUIPageSetup::StartNewVideo()
{
    char filename[0x70];
    int mode = *m_mode - 1;
    int event = m_selections[mode].m_event;
    int group3 = m_selections[mode].m_group3;
    int previous = m_currentVideo;
    // Declared after the loads above: retail fills the array afterwards and
    // then reloads *m_mode for the switch.
    char *tracks[21] = {
        "Hawaii", "Evergl", "Three", "Madrid1", "Missio", "Rhine", "Barrie",
        "Seoul1", "Quilit", "Nice", "Lochne", "Madrid2", "Bali", "Tokyo",
        "Nile", "Seoul2", "Venice", "Bering", "Amazon", "Havasu", "Bermud",
    };
    switch (*m_mode) {
    case 1:
    case 2:
    case 3:
        switch (group3) {   // a one-case switch: retail's beq/b layout
        case 3:
            m_currentVideo = event + 21;
            sprintf(filename, "Videos\\Tracks\\%sH.pss", tracks[event - 1]);
            break;
        default:
            m_currentVideo = event + 42;
            sprintf(filename, "Videos\\Tracks\\%sE.pss", tracks[event - 1]);
            break;
        }
        break;
    case 4:
        m_currentVideo = event + 63;
        sprintf(filename, "Videos\\Tracks\\%sD.pss", tracks[event - 1]);
        break;
    case 5:
        m_currentVideo = event + 84;
        sprintf(filename, "Videos\\Tracks\\%sC.pss", tracks[event - 1]);
        break;
    }
    if (m_currentVideo != previous) {
        EndVideo();
        SetBackground(filename, 0);
        SetVideoPosition(m_videoPlayer, 0x113, 0x37);
        m_videoPlayer->m_xE0 = 1;
        m_videoPlayer->SetColor(0, 1, 1, 1, 0x80);
        m_videoPlayer->Start();
    }
}


// ---- src/JetskiUIPageSLevels/ctor.cpp

JetskiUIPageSLevels::JetskiUIPageSLevels(int enabled)
    : JetskiUIPage(enabled)
{
    m_soundMode = 0;
    m_pan = 0.0f;
}


// ---- src/JetskiUIPageSLevels/dtor.cpp


JetskiUIPageSLevels::~JetskiUIPageSLevels()
{
}


// ---- src/JetskiUIPageSLevels/Tick.cpp
#pragma fast_fptosi on





int JetskiUIPageSLevels::Tick(float dt)
{
    if (m_soundMode) {
        JetskiUIManMain *frontEnd = ((JetskiUIManMain *)m_manager);
        switch (m_soundMode) {
        case 1:
        case 3:
            m_pan += 180.0f * (dt / 4.0f);
            if (m_pan >= 360.0f)
                m_pan -= 360.0f;
            (*(PannedSound **)&frontEnd->m_heliSound)->SetPan((int)m_pan);
            break;
        }
    }
    return JetskiUIPage::Tick(dt);
}
#pragma fast_fptosi reset


// ---- src/JetskiUIPageSLevels/ResetPage.cpp




// Each count unlocks a level every 12 (levels 1-9).
void JetskiUIPageSLevels::ResetPage()
{
    JetskiGameOptions *options = LpGame->m_gameOptions;
    int count = options->m_musicVolume;
    SetGroupFocus(4, options->m_x830);
    if (count >= 96)
        SetGroupFocus(2, 9);
    else if (count >= 84)
        SetGroupFocus(2, 8);
    else if (count >= 72)
        SetGroupFocus(2, 7);
    else if (count >= 60)
        SetGroupFocus(2, 6);
    else if (count >= 48)
        SetGroupFocus(2, 5);
    else if (count >= 36)
        SetGroupFocus(2, 4);
    else if (count >= 24)
        SetGroupFocus(2, 3);
    else if (count >= 12)
        SetGroupFocus(2, 2);
    else
        SetGroupFocus(2, 1);

    if (options->m_x82C >= 96)
        SetGroupFocus(3, 9);
    else if (options->m_x82C >= 84)
        SetGroupFocus(3, 8);
    else if (options->m_x82C >= 72)
        SetGroupFocus(3, 7);
    else if (options->m_x82C >= 60)
        SetGroupFocus(3, 6);
    else if (options->m_x82C >= 48)
        SetGroupFocus(3, 5);
    else if (options->m_x82C >= 36)
        SetGroupFocus(3, 4);
    else if (options->m_x82C >= 24)
        SetGroupFocus(3, 3);
    else if (options->m_x82C >= 12)
        SetGroupFocus(3, 2);
    else
        SetGroupFocus(3, 1);

    if (options->m_x828 >= 96)
        SetPlayerFocus(1, 9, 0);
    else if (options->m_x828 >= 84)
        SetPlayerFocus(1, 8, 0);
    else if (options->m_x828 >= 72)
        SetPlayerFocus(1, 7, 0);
    else if (options->m_x828 >= 60)
        SetPlayerFocus(1, 6, 0);
    else if (options->m_x828 >= 48)
        SetPlayerFocus(1, 5, 0);
    else if (options->m_x828 >= 36)
        SetPlayerFocus(1, 4, 0);
    else if (options->m_x828 >= 24)
        SetPlayerFocus(1, 3, 0);
    else if (options->m_x828 >= 12)
        SetPlayerFocus(1, 2, 0);
    else
        SetPlayerFocus(1, 1, 0);
}


// ---- src/JetskiUIPageSLevels/ProcessUserInput.cpp






class UIControl;




// Plays the test sound in the selected speaker mode.
#define SET_SOUND_MODE()                                        \
    switch (GetGroupControlID(4)) {                             \
    case 1:                                                     \
        m_soundMode = 1;                                        \
        m_pan = 270.0f;                                         \
        ui->m_heliSound->SetBasePan(270);                           \
        break;                                                  \
    case 2:                                                     \
        m_soundMode = 2;                                        \
        m_pan = 0;                                              \
        ui->m_heliSound->SetBasePan(0);                             \
        LpGame->m_auralScape->SetMono(1);                       \
        break;                                                  \
    case 3:                                                     \
        m_soundMode = 3;                                        \
        m_pan = 0;                                              \
        ui->m_heliSound->SetBasePan(0);                             \
        LpGame->m_auralScape->SetDolby(1);                      \
        break;                                                  \
    }                                                           \
    ui->m_heliSound->Play(0, 0);

#define CHANGE_GROUP()                                          \
    if (ui->m_heliSound->IsPlaying())                               \
        ui->m_heliSound->Stop(0);                                   \
    switch (GetJoystickGroupID(0)) {                            \
    case 1:                                                     \
        m_soundMode = 0;                                        \
        ui->m_tuneSound->Pause(1);                                \
        break;                                                  \
    case 2:                                                     \
        m_soundMode = 0;                                        \
        ui->m_tuneSound->Pause(0);                                \
        break;                                                  \
    case 3:                                                     \
        m_soundMode = 0;                                        \
        ui->m_tuneSound->Pause(1);                                \
        break;                                                  \
    case 4:                                                     \
        ui->m_tuneSound->Pause(1);                                \
        LpGame->m_auralScape->SetDolby(0);                      \
        LpGame->m_auralScape->SetMono(0);                       \
        ui->m_heliSound->SetBaseVolume(100.0f);                     \
        SET_SOUND_MODE()                                        \
        break;                                                  \
    }

#define SET_LEVEL(group, field)                                 \
    switch (GetGroupControlID(group)) {                         \
    case 1:                                                     \
        options->field = 0;                                     \
        break;                                                  \
    case 2:                                                     \
        options->field = 12;                                    \
        break;                                                  \
    case 3:                                                     \
        options->field = 24;                                    \
        break;                                                  \
    case 4:                                                     \
        options->field = 36;                                    \
        break;                                                  \
    case 5:                                                     \
        options->field = 48;                                    \
        break;                                                  \
    case 6:                                                     \
        options->field = 60;                                    \
        break;                                                  \
    case 7:                                                     \
        options->field = 72;                                    \
        break;                                                  \
    case 8:                                                     \
        options->field = 84;                                    \
        break;                                                  \
    case 9:                                                     \
        options->field = 96;                                    \
        break;                                                  \
    case 0:                                                     \
        break;                                                  \
    }

#define CHANGE_CONTROL(Move, limit)                             \
    switch (group) {                                            \
    case 1:                                                     \
        if (GetGroupControlID(1) != limit)                      \
            m_groups[1]->Move(0);                               \
        SET_LEVEL(1, m_x828)                                    \
        (*(MusicPlayer **)&ui->m_soundGroup)->SetVolume(options->m_x828);                  \
        break;                                                  \
    case 2:                                                     \
        if (GetGroupControlID(2) != limit)                      \
            m_groups[2]->Move(0);                               \
        SET_LEVEL(2, m_musicVolume)                                    \
        (*(MusicPlayer **)&ui->m_musicGroup)->SetVolume(options->m_musicVolume);                  \
        break;                                                  \
    case 3:                                                     \
        if (GetGroupControlID(3) != limit)                      \
            m_groups[3]->Move(0);                               \
        SET_LEVEL(3, m_x82C)                                    \
        (*(MusicPlayer **)&ui->m_riderGroup)->SetVolume(options->m_x82C);                  \
        ui->m_negativeSound->Play(0, 0);                                 \
        break;                                                  \
    case 4:                                                     \
        if (ui->m_heliSound->IsPlaying())                           \
            ui->m_heliSound->Stop(0);                               \
        ui->m_heliSound->SetBaseVolume(100.0f);                     \
        LpGame->m_auralScape->SetDolby(0);                      \
        LpGame->m_auralScape->SetMono(0);                       \
        m_groups[4]->Move(0);                                   \
        SET_SOUND_MODE()                                        \
        break;                                                  \
    }

// Sound-levels page: up/down moves between the level sliders and the speaker
// mode, left/right changes the selected setting.
int JetskiUIPageSLevels::ProcessUserInput(int player, int input)
{
    ProcessInputSounds(player, input);
    JetskiGameOptions *options = LpGame->m_gameOptions;
    JetskiUIManMain *ui = ((JetskiUIManMain *)m_manager);
    switch (player) {
    case 0: {
        int group = GetJoystickGroupID(0);
        switch (input) {
        case 6:
            options->m_x830 = GetGroupControlID(4);
            ((JetskiUIManMain *)m_manager)->m_state = 7;
            ((JetskiUIManMain *)m_manager)->m_nextPage = 5;
            break;
        case 2:
            PreviousGroup(0);
            CHANGE_GROUP()
            break;
        case 3:
            NextGroup(0);
            CHANGE_GROUP()
            break;
        case 0:
            CHANGE_CONTROL(PreviousControl, 1)
            break;
        case 1:
            CHANGE_CONTROL(NextControl, 9)
            break;
        }
        break;
    }
    }
    return 1;
}


// ---- src/JetskiUIPageSLevels/ProcessInputSounds.cpp


void JetskiUIPageSLevels::ProcessInputSounds(int player, int input)
{
    switch (player) {
    case 0:
        switch (input) {
        case 6:
            PlayPageSound(3);
            break;
        case 2:
            PlayPageSound(5);
            break;
        case 3:
            PlayPageSound(5);
            break;
        case 0:
            PlayPageSound(6);
            break;
        case 1:
            PlayPageSound(6);
            break;
        }
        break;
    }
}


// ---- src/JetskiUIPageSLevels/Transition.cpp







int JetskiUIPageSLevels::Transition(int direction)
{
    JetskiUIManMain *state = ((JetskiUIManMain *)m_manager);
    if (state->m_direction == 1) {
        state->m_tuneSound->Pause(1);
        PlayPageSound(1);
    } else {
        state->m_tuneSound->Pause(0);
        if (state->m_heliSound->IsPlaying())
            state->m_heliSound->Stop(0);
    }
    m_levelsTransitioning = 1;
    for (int i = 1; i <= m_numGroups; i++)
        m_groups[i]->m_others[0]->Transition(direction);
    m_pageTransitioning = 1;
    m_groups[0]->Transition(direction);
    return 1;
}


// ---- src/JetskiUIPageSLevels/ProcessTransition.cpp



int JetskiUIPageSLevels::ProcessTransition(float dt)
{
    if (m_levelsTransitioning) {
        int done = 1;
        for (int i = 1; i <= m_numGroups; i++) {
            if (!m_groups[i]->m_others[0]->ProcessTransition(dt))
                done = 0;
        }
        if (done)
            m_levelsTransitioning = 0;
    }
    if (m_pageTransitioning) {
        if (m_groups[0]->ProcessTransition(dt))
            m_pageTransitioning = 0;
    }
    return !m_levelsTransitioning && !m_pageTransitioning;
}


// ---- src/JetskiUIPageStandings/ctor.cpp

JetskiUIPageStandings::JetskiUIPageStandings(int enabled)
    : JetskiUIPage(enabled)
{
    m_font = 0;
    m_numVertices = 4;
    for (int i = 0; i < 5; i++) {
        m_barVertices[i] = 0;
        m_bars[i] = 0;
        m_frameVertices[i] = 0;
        m_frames[i] = 0;
    }
    m_primitiveManager = 0;
}


// ---- src/JetskiUIPageStandings/dtor.cpp
extern "C" void free(void *p);






JetskiUIPageStandings::~JetskiUIPageStandings()
{
    int i;
    for (i = 0; i < 10; i++)
        delete m_texts[i];
    free(m_texts);
    m_texts = 0;
    for (i = 0; i < 5; i++) {
        if (m_bars[i])
            delete m_bars[i];
        if (m_barVertices[i])
            delete[] m_barVertices[i];
        if (m_frames[i])
            delete m_frames[i];
        if (m_frameVertices[i])
            delete[] m_frameVertices[i];
    }
}


// ---- src/JetskiUIPageStandings/ResetPage.cpp
#pragma inline_depth(4)
extern "C" int sprintf(char *buf, const char *fmt, ...);
extern "C" char *strncpy(char *dst, const char *src, unsigned int n);













void JetskiUIPageStandings::ResetPage()
{
    char buf[0x100];
    unsigned int teamColor;
    JetskiUIManMain *manager = ((JetskiUIManMain *)m_manager);
    manager->m_waterEffect->Pause(0);
    JetskiCareer *career = LpGame->m_gameOptions->m_career;
    int row = 0;
    for (int place = 0; place < 5; place++) {
        for (int i = 0; i < 5; i++) {
            if (career->m_racers[i].m_place == place + 1) {
                unsigned int color = career->m_racers[i].m_enabled == 0 ? 0xFFFFDE00 : 0xFFFFFFFF;
                int k;
                for (k = 0; k < m_numVertices; k++)
                    m_bars[row]->m_vertices[k].SetColor(color);
                m_bars[row]->m_dirty = 1;
                m_texts[row]->SetColor(color);
                m_texts[row + 5]->SetColor(color);
                manager->GetTeamColor(i, &teamColor);
                for (k = 0; k < m_numVertices; k++)
                    m_frames[row]->m_vertices[k].SetColor(teamColor);
                m_frames[row]->m_dirty = 1;
                sprintf(buf, "%d. %s", place + 1, career->m_racers[i].m_name);
                m_texts[row]->SetString(buf);
                m_pageTextManager->PositionText(m_texts[row]);
                sprintf(buf, "%d", career->m_racers[i].m_points);
                m_texts[row + 5]->SetString(buf);
                m_pageTextManager->PositionText(m_texts[row + 5]);
                row++;
            }
        }
    }
    if (manager->m_x2FC) {
        DisableGroupControlWidget(0, 1, 0x11);
        DisableGroupControlWidget(0, 1, 0x12);
        EnableGroupControlWidget(0, 1, 0xF);
        EnableGroupControlWidget(0, 1, 0x10);
    } else {
        DisableGroupControlWidget(0, 1, 0xF);
        DisableGroupControlWidget(0, 1, 0x10);
        EnableGroupControlWidget(0, 1, 0x11);
        EnableGroupControlWidget(0, 1, 0x12);
    }
}


// ---- src/JetskiUIPageStandings/Create.cpp
#pragma inline_depth(4)
extern "C" void *malloc(unsigned int size);
extern "C" char *strncpy(char *dst, const char *src, unsigned int n);

class ScreenTextFont;
class UIManager;
class ScreenPrimitiveManager;





struct QWORDI {
    int x, y, z, w;
};

struct QWORDF {
    QWORDF() {}
    float x, y, z, w;
};






JetskiUIPageStandings *JetskiUIPageStandings::Create(char *name, char *fileName, UIManager *manager,
                                                     ScreenTextManager *textManager,
                                                     ScreenPrimitiveManager *primitiveManager)
{
    int i;
    int j;
    int y;
    UIPage::Create(name, fileName, manager, textManager);
    m_primitiveManager = primitiveManager;
    m_pageTextManager = textManager;
    ScreenTextFontItem *item = LpGame->m_fonts->GetScreenTextFont("title_b");
    m_font = item ? item->m_font : 0;
    m_texts = (ScreenText **)malloc(10 * sizeof(ScreenText *));
    for (i = 0; i < 10; i++) {
        m_texts[i] = new ScreenText(0x28, 0);
        m_texts[i]->SetColor(0xFFFFFFFF);
        m_texts[i]->SetText("");
        m_texts[i]->SetFont(m_font);
        if (i > 4) {
            m_texts[i]->SetPosition(520.0f, (i - 5) * 40 + 0x94);
            m_texts[i]->SetJustification(ScreenTextManager::RightJustification);
        } else {
            m_texts[i]->SetPosition(188.0f, i * 40 + 0x94);
            m_texts[i]->SetJustification(ScreenTextManager::LeftJustification);
        }
    }
    for (i = 0, y = 0; i < 5; i++) {
        m_barVertices[i] = new ScreenPrimitiveVertex[m_numVertices];
        m_bars[i] = (new ScreenPrimitive)->Create(m_barVertices[i], m_numVertices,
                                                            (ScreenPrimitiveType)1, 0);
        m_frameVertices[i] = new ScreenPrimitiveVertex[m_numVertices];
        m_frames[i] = (new ScreenPrimitive)->Create(m_frameVertices[i], m_numVertices,
                                                      (ScreenPrimitiveType)1, 0);
        for (j = 0; j < m_numVertices; j++) {
            m_bars[i]->m_vertices[j].SetColor(0xFFFFFFFF);
            m_frames[i]->m_vertices[j].SetColor(0xFFFFFFFF);
        }
        m_bars[i]->m_vertices[0].SetPosition(115.0f, y + 0x96);
        m_bars[i]->m_vertices[1].SetPosition(165.0f, y + 0x96);
        m_bars[i]->m_vertices[2].SetPosition(115.0f, y + 0xAF);
        m_bars[i]->m_vertices[3].SetPosition(165.0f, y + 0xAF);
        m_frames[i]->m_vertices[0].SetPosition(120.0f, y + 0x9B);
        m_frames[i]->m_vertices[1].SetPosition(160.0f, y + 0x9B);
        m_frames[i]->m_vertices[2].SetPosition(120.0f, y + 0xAA);
        m_frames[i]->m_vertices[3].SetPosition(160.0f, y + 0xAA);
        m_bars[i]->m_dirty = 1;
        m_frames[i]->m_dirty = 1;
        y += 40;
    }
    return this;
}


// ---- src/JetskiUIPageStandings/PrepareGeometry.cpp




int JetskiUIPageStandings::PrepareGeometry()
{
    if (m_font) {
        TextureMap *t = m_font->m_texture;
        if (10.0f > t->m_usage)
            t->m_usage = 10.0f;
    }
    return GameObject::PrepareGeometry();
}


// ---- src/JetskiUIPageStandings/Render3D.cpp
class ScreenText;
class ScreenTextFont;
class ScreenPrimitive;





int JetskiUIPageStandings::Render3D()
{
    int i;
    for (i = 0; i < 10; i++)
        m_pageTextManager->AddText(m_texts[i], 0);
    for (i = 0; i < 5; i++) {
        m_primitiveManager->AddPrimitive(m_bars[i]);
        m_primitiveManager->AddPrimitive(m_frames[i]);
    }
    return GameObject::Render3D();
}


// ---- src/JetskiUIPageStandings/ProcessUserInput.cpp



int JetskiUIPageStandings::ProcessUserInput(int player, int input)
{
    ProcessInputSounds(player, input);
    JetskiUIManMain *state = ((JetskiUIManMain *)m_manager);
    switch (player) {
    case 0:
        if (state->m_x2FC) {
            switch (input) {
            case 4:
                state->m_state = 7;
                ((JetskiUIManMain *)m_manager)->m_nextPage = 0x19;
                PlayPageSound(2);
                state->m_x2FC = 0;
                break;
            }
        } else {
            switch (input) {
            case 6:
                state->m_state = 7;
                ((JetskiUIManMain *)m_manager)->m_nextPage = 0x19;
                PlayPageSound(3);
                break;
            }
        }
        break;
    }
    return 1;
}


// ---- src/JetskiUIPageStandings/ProcessTransition.cpp

void JetskiUIPageStandings::ProcessInputSounds(int player, int input)
{
}

int JetskiUIPageStandings::Transition(int direction)
{
    return 1;
}

int JetskiUIPageStandings::ProcessTransition(float dt)
{
    return 1;
}


// ---- src/JetskiUIPageController/ctor.cpp

JetskiUIPageController::JetskiUIPageController(int enabled)
    : JetskiUIPage(enabled)
{
}


// ---- src/JetskiUIPageController/dtor.cpp


JetskiUIPageController::~JetskiUIPageController()
{
}


// ---- src/JetskiUIPageController/ResetPage.cpp






void JetskiUIPageController::ResetPage()
{
    JetskiGameOptions *mp = LpGame->m_gameOptions;
    switch (((JetskiUIManMain *)m_manager)->m_numPlayers) {
    case 1:
        SetGroupFocus(1, 1);
        SetPlayerFocus(2, mp->m_controllerPreset[0], 0);
        break;
    case 2:
        SetGroupFocus(1, 2);
        SetPlayerFocus(2, mp->m_controllerPreset[1], 1);
        break;
    }
}


// ---- src/JetskiUIPageController/ProcessUserInput.cpp






int JetskiUIPageController::ProcessUserInput(int player, int input)
{
    ProcessInputSounds(player, input);
    JetskiGameOptions *options = LpGame->m_gameOptions;
    if (player != ((JetskiUIManMain *)m_manager)->m_numPlayers - 1)
        return 0;
    switch (input) {
    case 6:
        ((JetskiUIManMain *)m_manager)->m_state = 7;
        ((JetskiUIManMain *)m_manager)->m_nextPage = 5;
        int preset = GetGroupControlID(2);
        if (options->m_controllerPreset[player] != preset)
            options->ChangeControllerPreset(preset, player);
        if (preset != 1)
            options->m_x86C = 0;
        break;
    }
    return 1;
}


// ---- src/JetskiUIPageController/ProcessInputSounds.cpp



// Each player's controller group plays the page sounds for its own input.
void JetskiUIPageController::ProcessInputSounds(int player, int input)
{
    switch (GetGroupControlID(1)) {
    case 1:
        switch (player) {
        case 0:
            switch (input) {
            case 6:
                PlayPageSound(3);
                break;
            case 0:
                PlayPageSound(6);
                break;
            case 1:
                PlayPageSound(6);
                break;
            }
            break;
        }
        break;
    case 2:
        switch (player) {
        case 1:
            switch (input) {
            case 6:
                PlayPageSound(3);
                break;
            case 0:
                PlayPageSound(6);
                break;
            case 1:
                PlayPageSound(6);
                break;
            }
            break;
        }
        break;
    }
}


// ---- (JetskiUIPageController::Transition, ProcessTransition)

int JetskiUIPageController::Transition(int direction)
{
    return 1;
}

int JetskiUIPageController::ProcessTransition(float dt)
{
    return 1;
}


// ---- src/JetskiUIPageChallenge/ctor.cpp

JetskiUIPageChallenge::JetskiUIPageChallenge(int enabled)
    : JetskiUIPage(enabled)
{
}


// ---- src/JetskiUIPageChallenge/dtor.cpp


JetskiUIPageChallenge::~JetskiUIPageChallenge()
{
}


// ---- src/JetskiUIPageChallenge/ResetPage.cpp
// GetString is only inlined at a deeper inline depth than the default.
#pragma inline_depth(5)
#pragma alignlabel off

extern "C" int sprintf(char *buf, const char *fmt, ...);
extern "C" char *strncpy(char *dst, const char *src, unsigned int n);




struct ScreenTextHolder {
    ScreenText *m_text;                 // +0x0
};

struct NameHolder {
    char *m_name;                       // +0x0
};


struct ChallengeEvent {                 // 0x20 bytes
    int m_track;                        // +0x00 (1-based)
    char pad04[0x20 - 0x4];
};

struct ChallengeProgress {
    char pad0[0x4];
    int m_mode;                         // +0x04
    int m_event;                        // +0x08
    char pad0C[0x174 - 0xC];
    ChallengeEvent m_events[1];         // +0x174
};

class ChallengeView {
public:
    char pad0[0x5C];
    int m_trackUnlocked[0x15];          // +0x5C
    int m_x0B0[0x15];                   // +0xB0
#ifdef SPLASHDOWN_PC
    char pad104[__builtin_offsetof(JetskiGameOptions, m_career) - 0x104];
#else
    char pad104[0x5FBC - 0x104];
#endif
    ChallengeProgress *m_challenge;     // +0x5FBC
};




void JetskiUIPageChallenge::ResetPage()
{
    char text[0xD0];
//PB
    ChallengeView *options;
    ChallengeProgress *challenge;
    int unlocked;
    int message;
    int track;
//PE
    unlocked = 0;
    message = 0;
    options = (ChallengeView *)LpGame->m_gameOptions;
    challenge = options->m_challenge;
    track = challenge->m_events[challenge->m_event].m_track - 1;
    DisableGroup(2);
    if (challenge->m_mode == 3) {
        if (!options->m_x0B0[track]) {
            unlocked = 1;
            options->m_x0B0[track] = 1;
            message = 1;
            if (!options->m_trackUnlocked[track])
                options->m_trackUnlocked[track] = 1;
            else
                message = 2;
        }
    } else if (!options->m_trackUnlocked[track]) {
        unlocked = 1;
        message = 3;
        options->m_trackUnlocked[track] = 1;
    }
    if (unlocked) {
        int control = 1;
        switch (track + 1) {
        case 9:
            control = 1;
            break;
        case 13:
            control = 2;
            break;
        case 17:
            control = 3;
            break;
        }
        EnableGroup(2);
        SetGroupFocus(2, control);
        int index = 2;
        JetskiWidgetText *widget = (JetskiWidgetText *)GetWidgetPtr(2, control, index);
        char *name;
        switch (message) {
        case 1:
            name = ((NameHolder *)widget->m_lines[0])->m_name;
            sprintf(text, "%s %s", name, LpGame->m_uiText->GetString(0x123));
            break;
        case 2:
            name = ((NameHolder *)widget->m_lines[0])->m_name;
            sprintf(text, "%s %s", name, LpGame->m_uiText->GetString(0x122));
            break;
        case 3:
            name = ((NameHolder *)widget->m_lines[0])->m_name;
            sprintf(text, "%s %s", name, LpGame->m_uiText->GetString(0x124));
            break;
        }
        widget->m_lines[0]->SetString(text);
    }
    SetPlayerFocus(1, 1, 0);
}
#pragma alignlabel reset


// ---- src/JetskiUIPageChallenge/ProcessUserInput.cpp
struct ChallengeProgress2 {
    char pad0[0x8];
    int m_attempts;                 // +0x08
};







int JetskiUIPageChallenge::ProcessUserInput(int player, int input)
{
    ProcessInputSounds(player, input);
    JetskiUIManMain *state = ((JetskiUIManMain *)m_manager);
    ChallengeProgress2 *challenge = (ChallengeProgress2 *)LpGame->m_gameOptions->m_career;
    switch (player) {
    case 0:
        switch (input) {
        case 4:
            switch (GetGroupControlID(1)) {
            case 1:
                state->m_selectMode = 2;
                ((JetskiUIManMain *)m_manager)->m_state = 7;
                ((JetskiUIManMain *)m_manager)->m_nextPage = 0x1B;
                break;
            case 2:
                challenge->m_attempts++;
                ((JetskiUIManMain *)m_manager)->m_state = 7;
                ((JetskiUIManMain *)m_manager)->m_nextPage = 0x19;
                break;
            }
            break;
        }
        break;
    }
    return 1;
}


// ---- src/JetskiUIPageChallenge/ProcessInputSounds.cpp


void JetskiUIPageChallenge::ProcessInputSounds(int player, int input)
{
    switch (player) {
    case 0:
        switch (input) {
        case 4:
            PlayPageSound(2);
            break;
        case 2:
            PlayPageSound(5);
            break;
        case 3:
            PlayPageSound(5);
            break;
        }
        break;
    }
}


// ---- (JetskiUIPageChallenge::Transition)

int JetskiUIPageChallenge::Transition(int direction)
{
    return 1;
}


// ---- src/JetskiUIPageChallenge/ProcessTransition.cpp

int JetskiUIPageChallenge::ProcessTransition(float)
{
    return 1;
}


// ---- src/JetskiUIPageTraining/ctor.cpp

JetskiUIPageTraining::JetskiUIPageTraining(int enabled)
    : JetskiUIPage(enabled)
{
    m_transitioning = 0;
}


// ---- src/JetskiUIPageTraining/dtor.cpp


JetskiUIPageTraining::~JetskiUIPageTraining()
{
}


// ---- src/JetskiUIPageTraining/ResetPage.cpp


void JetskiUIPageTraining::ResetPage()
{
    SetPlayerFocus(1, 1, 0);
}


// ---- src/JetskiUIPageTraining/ProcessUserInput.cpp




int JetskiUIPageTraining::ProcessUserInput(int player, int input)
{
    JetskiGameOptions *options = LpGame->m_gameOptions;
    ProcessInputSounds(player, input);
    JetskiUIManMain *state = (JetskiUIManMain *)m_manager;
    switch (player) {
    case 0:
        switch (input) {
        case 4:
            options->m_x4C = GetGroupControlID(1);
            options->m_raceType = 8;
            options->m_numRacers = 2;
            options->m_numHumans = 1;
            options->m_x3C = 0;
            ((JetskiGameOptions *)options)->SetupAIRacers(options->m_numHumans);
            state->m_state = 8;
            break;
        case 6:
            state->m_state = 7;
            ((JetskiUIManMain *)m_manager)->m_nextPage = 1;
            break;
        }
        break;
    }
    return 1;
}


// ---- src/JetskiUIPageTraining/ProcessInputSounds.cpp

void JetskiUIPageTraining::ProcessInputSounds(int player, int input)
{
    switch (player) {
    case 0:
        switch (input) {
        case 4:
            PlayPageSound(4);
            m_fadeTime = 0.0f;
            m_transitioning = 1;
            break;
        case 6:
            PlayPageSound(3);
            break;
        case 2:
            PlayPageSound(5);
            break;
        case 3:
            PlayPageSound(5);
            break;
        }
        break;
    }
}


// ---- src/JetskiUIPageTraining/Transition.cpp

int JetskiUIPageTraining::Transition(int)
{
    return 1;
}


// ---- src/JetskiUIPageTraining/ProcessTransition.cpp
#pragma fast_fptosi on  // retail never calls fptosi: float->int is an inline cvt.w.s






int JetskiUIPageTraining::ProcessTransition(float dt)
{
    JetskiUIManMain *state = (JetskiUIManMain *)m_manager;
    int done = 1;
    if (state->m_direction == 2 && m_transitioning) {
        m_fadeTime += dt;
        int level = (int)((1.0f - m_fadeTime / 1.2f) * LpGame->m_gameOptions->m_musicVolume);
        if (level > 0) {
            (*(MusicPlayer **)&state->m_musicGroup)->SetVolume(level);
            done = 0;
        }
    }
    return done;
}
#pragma fast_fptosi reset


// ---- src/JetskiUIPageVideos/ctor.cpp

JetskiUIPageVideos::JetskiUIPageVideos(int enabled)
    : JetskiUIPage(enabled)
{
}


// ---- src/JetskiUIPageVideos/dtor.cpp


JetskiUIPageVideos::~JetskiUIPageVideos()
{
}


// ---- src/JetskiUIPageVideos/ResetPage.cpp








// Show only the unlocked videos (grey the rest) and restore the focus.
void JetskiUIPageVideos::ResetPage()
{
    JetskiGameOptions *options = LpGame->m_gameOptions;
    int first = -1;
    int i;
    if (options->m_movieUnlocked[9])
        EnableGroupControl(1, 10);
    for (i = 0; i < 10; i++) {
        TurnGroupControlOnOff(0, 1, i + 1);
        if (options->m_movieUnlocked[i]) {
            if (first == -1)
                first = i + 1;
            TurnGroupControlOnOff(1, 1, i + 1);
        }
    }
    int focus = LpGame->m_gameOptions->m_pageSettings->m_videos->m_x0;
    if (focus == -1) {
        if (first == -1) {
            SetGroupFocus(1, 1);
            SetPlayerFocus(2, 1, 0);
        } else {
            SetPlayerFocus(1, first, 0);
        }
    } else {
        SetPlayerFocus(1, focus, 0);
    }
    for (i = 0; i < 10; i++) {
        if (!options->m_movieUnlocked[i])
            ((JetskiWidgetText *)m_groups[1]->m_controls[i]->m_widgets[0])->SetColor(0xFFA0A0A0);
    }
    if (!options->m_movieUnlocked[9])
        DisableGroupControl(1, 10);
}


// ---- src/JetskiUIPageVideos/ProcessUserInput.cpp
extern "C" char *strcpy(char *dst, const char *src);

struct VideoSelection {
    int m_selected;                     // +0x0
};

struct JetskiUIPages {
#ifdef SPLASHDOWN_PC
    char pad0[__builtin_offsetof(JetskiPageSettings, m_videos)];
#else
    char pad0[0x1C];
#endif
    VideoSelection *m_videos;           // +0x1C
};







// Starts a movie and switches to the video player.
#define PLAY_VIDEO(file)                        \
    strcpy(options->m_creditsVideo, file);         \
    options->m_x89C = 0x1E;        \
    ((JetskiUIManMain *)m_manager)->m_state = 7;                 \
    ((JetskiUIManMain *)m_manager)->m_nextPage = 10

int JetskiUIPageVideos::ProcessUserInput(int player, int input)
{
    JetskiGameOptions *options = LpGame->m_gameOptions;
    ProcessInputSounds(player, input);
    switch (player) {
    case 0:
        switch (input) {
        case 4:
            if (GetJoystickGroupID(player) == 1) {
                int video = GetGroupControlID(1);
                ((JetskiUIPages *)LpGame->m_gameOptions->m_pageSettings)->m_videos->m_selected = video;
                switch (video) {
                case 1:
                    PLAY_VIDEO("Videos\\EndAP.pss");
                    break;
                case 3:
                    PLAY_VIDEO("Videos\\EndJL.pss");
                    break;
                case 5:
                    PLAY_VIDEO("Videos\\EndRM.pss");
                    break;
                case 7:
                    PLAY_VIDEO("Videos\\EndKT.pss");
                    break;
                case 2:
                    PLAY_VIDEO("Videos\\EndHH.pss");
                    break;
                case 4:
                    PLAY_VIDEO("Videos\\EndSK.pss");
                    break;
                case 8:
                    PLAY_VIDEO("Videos\\EndJW.pss");
                    break;
                case 6:
                    PLAY_VIDEO("Videos\\EndAR.pss");
                    break;
                case 10:
                    PLAY_VIDEO("Videos\\EndS.pss");
                    break;
                case 9:
                    PLAY_VIDEO("Videos\\StuntMan.pss");
                    break;
                }
            }
            break;
        case 6:
            ((JetskiUIManMain *)m_manager)->m_state = 7;
            ((JetskiUIManMain *)m_manager)->m_nextPage = 5;
            break;
        }
        break;
    }
    return 1;
}


// ---- src/JetskiUIPageVideos/ProcessInputSounds.cpp


void JetskiUIPageVideos::ProcessInputSounds(int player, int input)
{
    switch (player) {
    case 0:
        switch (input) {
        case 4:
            PlayPageSound(2);
            m_fadeTime = 0;
            break;
        case 6:
            PlayPageSound(3);
            break;
        case 2:
            PlayPageSound(5);
            break;
        case 3:
            PlayPageSound(5);
            break;
        case 0:
            PlayPageSound(5);
            break;
        case 1:
            PlayPageSound(5);
            break;
        }
        break;
    }
}


// ---- src/JetskiUIPageVideos/Transition.cpp

int JetskiUIPageVideos::Transition(int)
{
    return 1;
}


// ---- src/JetskiUIPageOptions/ProcessTransition.cpp
#pragma fast_fptosi on







int JetskiUIPageVideos::ProcessTransition(float dt)
{
    JetskiUIManager *state = ((JetskiUIManMain *)m_manager);
    if (state->m_nextPage == 10 && state->m_direction == 2) {
        // Fade the music out over 1.2 seconds, then stop the video.
        m_fadeTime += dt;
        int volume = (int)((1.0f - m_fadeTime / 1.2f) * (float)LpGame->m_gameOptions->m_musicVolume);
        if (volume > 0) {
            (*(MusicPlayer **)&state->m_musicGroup)->SetVolume(volume);
            return 0;
        }
        state->m_tuneSound->Stop(0);
        return 1;
    }
    (*(MusicPlayer **)&state->m_musicGroup)->SetVolume(LpGame->m_gameOptions->m_musicVolume);
    if (!state->m_tuneSound->IsPlaying())
        state->m_tuneSound->Play(0, 0);
    return 1;
}
#pragma fast_fptosi reset


// ---- src/JetskiUIPageCredits/ctor.cpp

JetskiUIPageCredits::JetskiUIPageCredits(int enabled)
    : JetskiUIPage(enabled)
{
}


// ---- src/JetskiUIPageCredits/dtor.cpp


JetskiUIPageCredits::~JetskiUIPageCredits()
{
}


// ---- src/JetskiUIPageCredits/Tick.cpp




int JetskiUIPageCredits::Tick(float dt)
{
    if (((JetskiUIManMain *)m_manager)->m_state) {
        return GameObject::Tick(dt);
    } else {
        if (m_videoPlayer) {
            if (!m_videoPlayer->IsPlaying())
                EndVideo();
        }
        return GameObject::Tick(dt);
    }
}


// ---- src/JetskiUIPageCredits/ResetPage.cpp








void JetskiUIPageCredits::ResetPage()
{
    JetskiGameOptions *options = LpGame->m_gameOptions;
    if (((JetskiUIManMain *)m_manager)->m_waterEffect)
        ((JetskiUIManMain *)m_manager)->m_waterEffect->DisableAll();
    SetBackground(options->m_creditsVideo, 0);
    PSXMediaControl *video = m_videoPlayer;
    if (video) {
        video->m_xE4 = 0;
        video->m_red = 2;
        video->m_green = 2;
        video->m_blue = 2;
        video->m_alpha = 0x80;
        m_videoPlayer->Start();
    }
}


// ---- src/JetskiUIPageCredits/ProcessUserInput.cpp

int JetskiUIPageCredits::ProcessUserInput(int player, int input)
{
    switch (player) {
    case 0:
        switch (input) {
        case 4:
            EndVideo();
            break;
        }
        break;
    }
    return 1;
}


// ---- src/JetskiUIPageCredits/EndVideo.cpp








void JetskiUIPageCredits::EndVideo()
{
    JetskiUIManMain *frontEnd = ((JetskiUIManMain *)m_manager);
    JetskiGameOptions *options = LpGame->m_gameOptions;
    if (frontEnd->m_waterEffect)
        frontEnd->m_waterEffect->EnableAll();
    if (m_videoPlayer) {
        m_videoPlayer->Stop();
        m_videoPlayer->RemoveFromChain();
        m_videoPlayer->Release();
        m_videoPlayer = 0;
    }
    frontEnd->m_state = 7;
    frontEnd->m_nextPage = options->m_x89C;
}


// ---- src/JetskiUIPageCovers/ctor.cpp

JetskiUIPageCovers::JetskiUIPageCovers(int enabled)
    : JetskiUIPage(enabled)
{
    m_timer = 2.2f;
}


// ---- src/JetskiUIPageCovers/dtor.cpp


JetskiUIPageCovers::~JetskiUIPageCovers()
{
}


// ---- src/JetskiUIPageCovers/Tick.cpp



int JetskiUIPageCovers::Tick(float dt)
{
    JetskiUIManager *state = ((JetskiUIManMain *)m_manager);
    if (state->m_state) {
        return GameObject::Tick(dt);
    }
    if (m_timer <= 0.0f) {
        state->m_nextPage = 1;
        state->m_state = 7;
    } else {
        m_timer -= dt;
    }
    return GameObject::Tick(dt);
}


// ---- src/JetskiUIPageCovers/ResetPage.cpp




void JetskiUIPageCovers::ResetPage()
{
    JetskiUIManMain *state = ((JetskiUIManMain *)m_manager);
    if (state->m_waterEffect)
        state->m_waterEffect->DisableAll();
    if (!state->m_tuneSound->IsPlaying())
        state->m_tuneSound->Play(0, 0);
    m_timer = 2.2f;
}


// ---- src/JetskiUIPageCenterScreen/ctor.cpp

JetskiUIPageCenterScreen::JetskiUIPageCenterScreen(int enabled)
    : JetskiUIPage(enabled)
{
    m_repeatTimer = 0.0f;
    m_direction = 1;
    m_lastDirection = 1;
}


// ---- src/JetskiUIPageCenterScreen/dtor.cpp


JetskiUIPageCenterScreen::~JetskiUIPageCenterScreen()
{
}


// ---- src/JetskiUIPageCenterScreen/Create.cpp

JetskiUIPageCenterScreen *JetskiUIPageCenterScreen::Create(char *name, char *fileName, UIManager *manager,
                                                           ScreenTextManager *textManager)
{
    UIPage::Create(name, fileName, manager, textManager);
    return this;
}


// ---- src/JetskiUIPageCenterScreen/Tick.cpp


extern PSXVideoCard *LpVideoCard;



// Moves the picture with the d-pad, repeating five times a second while held.
int JetskiUIPageCenterScreen::Tick(float dt)
{
    KeyInfo info;
    int pressed = 0;
    if (LpGame->m_controls->m_joysticks[0]->IsKeyDown((AssignableActionIndex)0, 0x3F, &info)) {
        pressed = 1;
        m_direction = 1;
    } else if (LpGame->m_controls->m_joysticks[0]->IsKeyDown((AssignableActionIndex)1, 0x3F, &info)) {
        pressed = 1;
        m_direction = 2;
    } else if (LpGame->m_controls->m_joysticks[0]->IsKeyDown((AssignableActionIndex)2, 0x3F, &info)) {
        pressed = 1;
        m_direction = 3;
    } else if (LpGame->m_controls->m_joysticks[0]->IsKeyDown((AssignableActionIndex)3, 0x3F, &info)) {
        pressed = 1;
        m_direction = 4;
    }
    if (m_direction != m_lastDirection || !pressed) {
        m_lastDirection = m_direction;
        m_repeatTimer = 0;
    } else {
        m_repeatTimer += 5.0f * dt;
        if (m_repeatTimer > 1.0f) {
            m_repeatTimer -= 1.0f;
            switch (m_direction) {
            case 1:
                if (LpVideoCard->GetShiftX() > -50)
                    LpVideoCard->SetShiftX(LpVideoCard->GetShiftX() - 1);
                break;
            case 2:
                if (LpVideoCard->GetShiftX() < 50)
                    LpVideoCard->SetShiftX(LpVideoCard->GetShiftX() + 1);
                break;
            case 3:
                if (LpVideoCard->GetShiftY() > -50)
                    LpVideoCard->SetShiftY(LpVideoCard->GetShiftY() - 1);
                break;
            case 4:
                if (LpVideoCard->GetShiftY() < 50)
                    LpVideoCard->SetShiftY(LpVideoCard->GetShiftY() + 1);
                break;
            }
        }
    }
    return GameObject::Tick(dt);
}


// ---- src/JetskiUIPageCenterScreen/ResetPage.cpp

void JetskiUIPageCenterScreen::ResetPage()
{
    m_repeatTimer = 0;
    m_direction = 1;
}


// ---- src/JetskiUIPageCenterScreen/ProcessUserInput.cpp
extern PSXVideoCard *LpVideoCard;





// Move the picture around with the pad (within 50 pixels either way).
int JetskiUIPageCenterScreen::ProcessUserInput(int player, int input)
{
    ProcessInputSounds(player, input);
    JetskiUIManager *state;
    JetskiGameOptions *options;
    options = LpGame->m_gameOptions;
    state = ((JetskiUIManMain *)m_manager);
    switch (player) {
    case 0:
        switch (input) {
        case 4:
            options->m_screenShiftX = LpVideoCard->GetShiftX();
            options->m_screenShiftY = LpVideoCard->GetShiftY();
            state->m_state = 7;
            state->m_nextPage = 6;
            break;
        case 5:
            LpVideoCard->SetShiftX(0);
            LpVideoCard->SetShiftY(0);
            break;
        case 2:
            if (LpVideoCard->GetShiftY() > -50)
                LpVideoCard->SetShiftY(LpVideoCard->GetShiftY() - 1);
            break;
        case 3:
            if (LpVideoCard->GetShiftY() < 50)
                LpVideoCard->SetShiftY(LpVideoCard->GetShiftY() + 1);
            break;
        case 0:
            if (LpVideoCard->GetShiftX() > -50)
                LpVideoCard->SetShiftX(LpVideoCard->GetShiftX() - 1);
            break;
        case 1:
            if (LpVideoCard->GetShiftX() < 50)
                LpVideoCard->SetShiftX(LpVideoCard->GetShiftX() + 1);
            break;
        }
        break;
    }
    return 1;
}


// ---- src/JetskiUIPageCenterScreen/ProcessInputSounds.cpp


void JetskiUIPageCenterScreen::ProcessInputSounds(int player, int input)
{
    switch (player) {
    case 0:
        switch (input) {
        case 4:
            PlayPageSound(2);
            break;
        case 5:
            PlayPageSound(3);
            break;
        case 1:
            PlayPageSound(6);
            break;
        case 0:
            PlayPageSound(6);
            break;
        case 2:
            PlayPageSound(6);
            break;
        case 3:
            PlayPageSound(6);
            break;
        }
        break;
    }
}

