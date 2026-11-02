// JetskiRaceOverMenus.cpp  @ 0x0037C300 (text 0x37C300-0x3899A0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's JetskiRaceOverMenus.cpp as one translation unit: the race-over
// menus (Circuit, Countdown, Career, Challenge) and
// JetskiRaceStartMenuCareer, then the static initializer (only the axis
// vectors). Its .rodata restarts the labels at @1215 after
// JetskiLoadManager.cpp's RTTI and runs to @3807.




#include "JetskiMenu/JetskiMenu.h"
#include "JetskiRaceOverMenuCircuit/JetskiRaceOverMenuCircuit.h"
#include "soultree_assert_block.h"
#include "GameText/GameText.h"
#include "ScreenText/ScreenText.h"
#include "ScreenTextManager/ScreenTextManager.h"
#include "ScreenTextFontItemList/ScreenTextFontItem.h"
#include "ScreenTextFontItemList/ScreenTextFontItemList.h"
#include "JetskiScreenText/JetskiScreenText.h"
#include "JetskiGame/JetskiGame.h"
#include "RenderTarget/RenderTarget.h"
#include "JetskiVehicle/JetskiVehicle.h"
#include "GameObject/GameObject.h"
#include "JoystickDevice/JoystickDevice.h"
#include "ControlInterface/ControlInterface.h"
#include "ViewPortCode/ViewPortCode.h"
#include "JetskiNameMenu/JetskiNameMenu.h"
#include "JetskiCompetition/JetskiCompetition.h"
#include "JetskiGameOptions/JetskiGameOptions.h"
#include "JetskiRaceOverMenuCountdown/JetskiRaceOverMenuCountdown.h"
#include "ScreenTextFont/ScreenTextFont.h"
#include "TextureMap/TextureMap.h"
#include "GameObject/GameObjectGlobals.h"
#include "JetskiRaceOverMenuCareer/JetskiRaceOverMenuCareer.h"
#include "JetskiCareerCompetition/JetskiCareerCompetition.h"
#include "LocalizedString/LocalizedString.h"
#include "RaceStatusStruct_Tag/RaceStatusStruct_Tag.h"
#include "soultree_assert.h"
#include "ResponseEmitter/ResponseEmitter.h"
#include "AuralScape/AuralScape.h"
#include "JetskiRaceStartMenuCareer/JetskiRaceStartMenuCareer.h"
#include "RaceStatusEventInterface/RaceStatusEventInterface.h"
#include "JetskiCareer/JetskiCareer.h"
#include "sinit/AxisVectors.h"
#include "RaceStatusClass/RaceStatusClass.h"
#include "JetskiChallengeCompetition/JetskiChallengeCompetition.h"

#include "JetskiRaceOverMenuChallenge/JetskiRaceOverMenuChallenge.h"
#include "JetskiCareerCompetition/JetskiCareerCompetition.h"
#include "JetskiCareer/JetskiCareer.h"

// inline_depth is TU-wide (the Create functions').
#pragma inline_depth(5)

extern JetskiGame *LpGame;

// ---- src/JetskiRaceOverMenuCircuit/ctor.cpp


JetskiRaceOverMenuCircuit::JetskiRaceOverMenuCircuit(int id) : JetskiMenu(id)
{
    m_textManager = 0;
    m_labels = 0;
    m_bigTexts = 0;
    m_text108 = 0;
    m_text104 = 0;
    m_font110 = m_font114 = 0;
    m_font118 = 0;
    m_idleTime = 0;
    m_attractDelay = 5.0f;
}


// ---- src/JetskiRaceOverMenuCircuit/dtor.cpp

extern "C" void free(void *p);

JetskiRaceOverMenuCircuit::~JetskiRaceOverMenuCircuit()
{
    int i;
    for (i = 0; i < 2; i++)
        delete m_labels[i];
    free(m_labels);
    m_labels = 0;
    for (i = 0; i < 3; i++)
        delete m_bigTexts[i];
    free(m_bigTexts);
    m_bigTexts = 0;
    for (i = 0; i < 20; i++)
        delete m_texts[i];
    free(m_texts);
    m_texts = 0;
    delete m_text104;
    delete m_text108;
    m_text108 = 0;
    m_text104 = 0;
}


// ---- src/JetskiRaceOverMenuCircuit/Create.cpp
// GameText::GetString (with its ASSERTF) is inlined inside SetString.


extern "C" void *malloc(unsigned int size);



// Local copy: JetskiRaceOverMenuCircuit.h carries its own JetskiMenu (no
// BuildMenuFull) and types +0x110..+0x118 as MenuItem *; they are the fonts.

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

// Creates the circuit results table: two labels, three column headers, 20
// texts and the two title lines.
JetskiRaceOverMenuCircuit *JetskiRaceOverMenuCircuit::Create(ScreenIconManager *icons, ScreenTextManager *texts, ViewPortCode viewport)
{
    int i;
    JetskiMenu::Create(icons, viewport);
    BuildMenuFull(1);
    ScreenTextFontItem *item = LpGame->m_fonts->GetScreenTextFont("info_b");
    m_font110 = item ? item->m_font : 0;
    item = LpGame->m_fonts->GetScreenTextFont("title_b");
    m_font114 = item ? item->m_font : 0;
    item = LpGame->m_fonts->GetScreenTextFont("info_c");
    m_font118 = item ? item->m_font : 0;
    m_textManager = (new ScreenTextManager(0, 1))->Create(64);
    AddChild(m_textManager, -1);
    m_labels = (ScreenText **)malloc(2 * sizeof(ScreenText *));
    m_bigTexts = (JetskiScreenText **)malloc(3 * sizeof(JetskiScreenText *));
    m_texts = (ScreenText **)malloc(20 * sizeof(ScreenText *));
    for (i = 0; i < 2; i++) {
        m_labels[i] = new ScreenText(40, 0);
        SetJustification(m_labels[i], ScreenTextManager::LeftJustification);
        m_labels[i]->SetFont(m_font118);
        switch (i) {
        case 0:
            m_labels[i]->SetString(LpGame->m_text->GetString(13));
            break;
        case 1:
            m_labels[i]->SetString(LpGame->m_text->GetString(126));
            break;
        }
    }
    for (i = 0; i < 3; i++) {
        m_bigTexts[i] = new JetskiScreenText();
        m_bigTexts[i]->m_params.m_color = -1;
        m_bigTexts[i]->m_params.m_font = m_font110;
        m_bigTexts[i]->m_params.m_justification = ScreenTextManager::CenterHorizontalJustification;
        m_bigTexts[i]->m_params.m_lineSpacing = 20;
        switch (i) {
        case 0:
            m_bigTexts[i]->m_params.m_x = 310;
            m_bigTexts[i]->m_params.m_y = 90;
            m_bigTexts[i]->SetText(89, 0);
            break;
        case 1:
            m_bigTexts[i]->m_params.m_x = 408;
            m_bigTexts[i]->m_params.m_y = 90;
            m_bigTexts[i]->SetText(90, 0);
            break;
        case 2:
            m_bigTexts[i]->m_params.m_x = 508;
            m_bigTexts[i]->m_params.m_y = 90;
            m_bigTexts[i]->SetText(72, 0);
            break;
        }
    }
    for (i = 0; i < 20; i++) {
        m_texts[i] = new ScreenText(40, 0);
        m_texts[i]->SetString("");
        SET_COLOR(m_texts[i], 0xFFFFFFFF)
    }
    m_text104 = new ScreenText(40, 0);
    SetJustification(m_text104, ScreenTextManager::CenterHorizontalJustification);
    m_text104->SetFont(m_font114);
    m_text104->SetScale(1.059f);
    m_text104->SetString(LpGame->m_text->GetString(85));
    SET_COLOR(m_text104, 0xFF000000)
    m_text104->SetPosition(320.0f, 39.0f);
    m_textManager->PositionText(m_text104);
    m_text108 = new ScreenText(40, 0);
    SetJustification(m_text108, ScreenTextManager::CenterHorizontalJustification);
    m_text108->SetFont(m_font114);
    m_text108->SetScale(1.059f);
    m_text108->SetString(LpGame->m_text->GetString(86));
    SET_COLOR(m_text108, 0xFFFFFFFF)
    m_text108->SetPosition(317.0f, 36.0f);
    m_textManager->PositionText(m_text108);
    return this;
}


// ---- src/JetskiRaceOverMenuCircuit/PrepareGeometry.cpp

// Clamp the items' minimum widths to 10, rebuild the info panel if it was
// invalidated, then let the base menu prepare its geometry.
int JetskiRaceOverMenuCircuit::PrepareGeometry()
{
    if (m_font110) {
        TextureMap *style = m_font110->m_texture;
        if (10.0f > style->m_usage)
            style->m_usage = 10.0f;
    }
    if (m_font114) {
        TextureMap *style = m_font114->m_texture;
        if (10.0f > style->m_usage)
            style->m_usage = 10.0f;
    }
    if (m_font118) {
        TextureMap *style = m_font118->m_texture;
        if (10.0f > style->m_usage)
            style->m_usage = 10.0f;
    }
    if (m_infoDirty) {
        SetupInfo();
        m_infoDirty = 0;
    }
    return JetskiMenu::PrepareGeometry();
}


// ---- src/JetskiRaceOverMenuCircuit/Render3D.cpp

extern RenderTarget *LpRenderTarget;


// A per-file switch (retail has one copy in each file that uses it).
static int WaitForRenderingToFinish = 1;

// Draw once per frame, on the last player's pass.
int JetskiRaceOverMenuCircuit::Render3D()
{
    if (LpGame->m_viewport == LpGame->m_numCameras - 1) {
        if (WaitForRenderingToFinish && m_viewport) {
            LpRenderTarget->TrySendRenderQueue(1);
            LpRenderTarget->WaitForRenderingToComplete();
        }
        SetViewport();
        JetskiMenu::Render3D();
        RenderLabels();
        RenderText();
        GameObject::Render3D();
        RestoreViewport();
    }
    return 1;
}


// ---- src/JetskiRaceOverMenuCircuit/RenderLabels.cpp

void JetskiRaceOverMenuCircuit::RenderLabels()
{
    for (int i = 0; i < 3; i++)
        m_bigTexts[i]->AddText(m_textManager);
    m_textManager->PositionText(m_labels[0]);
    m_textManager->AddText(m_labels[0], 0);
    m_textManager->PositionText(m_labels[1]);
    m_textManager->AddText(m_labels[1], 0);
}


// ---- src/JetskiRaceOverMenuCircuit/RenderText.cpp

extern "C" int sprintf(char *buf, const char *fmt, ...);
extern "C" char *strcpy(char *dst, const char *src);

class ScreenTextFont;










static inline void FormatTime(char *buffer, float t)
{
    if (t == 0.0f || t == 3.4028235e38f) {
        strcpy(buffer, "--:--.--");
    } else {
        int minutes;
        if (t < 0.0f)
            t = -1.0f * t;
        if (t >= 5999.99f) {
            t = 59.99f;
            minutes = 99;
        } else {
            for (minutes = 0; !(t < 60.0f); minutes++)
                t -= 60.0f;
        }
        sprintf(buffer, "%2d:%05.2f", minutes, t);
    }
}

// The circuit results table: name, total time, gap to the leader and best
// lap for each racer, highlighting the players.
void JetskiRaceOverMenuCircuit::RenderText()
{
    char buffer[0x100];
    RaceStatusStruct_Tag *leader;
    JetskiCompetition *data;
    JetskiGameOptions *options;
    RaceStatusStruct_Tag *status;
    int i;
    data = LpGame->m_competition;
    options = LpGame->m_gameOptions;

    for (leader = status = data->m_raceStatus->m_vehicles, i = 0; status; status = status->m_next, i++) {
        unsigned int color;
        if (data->m_vehicles[status->m_id]->m_player >= 0) {
            color = 0xFFFFDE00;
            m_texts[i]->SetColor(color);
            m_texts[i + 5]->SetColor(color);
            m_texts[i + 10]->SetColor(color);
            m_texts[i + 15]->SetColor(color);
        } else {
            color = 0xFFFFFFFF;
            m_texts[i]->SetColor(color);
            m_texts[i + 5]->SetColor(color);
            m_texts[i + 10]->SetColor(color);
            m_texts[i + 15]->SetColor(color);
        }
        options->GetCharacterName(options->m_racerCharacter[status->m_id], buffer);
        m_texts[i]->SetString(buffer);
        m_textManager->PositionText(m_texts[i], ScreenTextManager::LeftJustification, m_font110);
        m_textManager->AddText(m_texts[i], m_font110);

        FormatTime(buffer, status->m_x6D4);
        m_texts[i + 5]->SetString(buffer);
        m_textManager->PositionText(m_texts[i + 5], ScreenTextManager::CenterHorizontalJustification, m_font110);
        m_textManager->AddText(m_texts[i + 5], m_font110);

        float gap = status->m_x6D4 - leader->m_x6D4;
        if (gap) {
            FormatTime(buffer, gap);
        } else {
#line 306 "JetskiRaceOverMenus.cpp"
            STRCPY(buffer, "0:00:00");
        }
        m_texts[i + 10]->SetString(buffer);
        m_textManager->PositionText(m_texts[i + 10], ScreenTextManager::CenterHorizontalJustification, m_font110);
        m_textManager->AddText(m_texts[i + 10], m_font110);

        FormatTime(buffer, status->m_bestLap);
        m_texts[i + 15]->SetString(buffer);
        m_textManager->PositionText(m_texts[i + 15], ScreenTextManager::CenterHorizontalJustification, m_font110);
        m_textManager->AddText(m_texts[i + 15], m_font110);
    }
    m_textManager->AddText(m_text104, 0);
    m_textManager->AddText(m_text108, 0);
}


// ---- src/JetskiRaceOverMenuCircuit/Tick.cpp
class KeyInfo;
class ProcessNode;









class ScreenIconManager;




int JetskiRaceOverMenuCircuit::Tick(float dt)
{
    int busy = 0;
    if (m_closing) {
        for (int i = 0; i < 7; i++) {
            if (m_timers[i].m_active)
                busy = 1;
        }
        if (!busy) {
            switch (m_closeAction) {
            case 1:
                DisableMenu();
                break;
            case 2:
                DisableAll();
                LpGame->m_competition->Pause(0);
                LpGame->m_competition->RestartRace((GamePlayStatus)3);
                LpGame->m_auralScape->Enable();
                LpGame->m_competition->ResetAllEngineSounds();
                break;
            }
            m_closing = 0;
            m_closeAction = 0;
        }
    }
    JetskiMenu::Tick(dt);
    if (!m_viewport && LpGame->m_competition && !m_closing && LpGame->m_competition->m_vcr
        && LpGame->m_controls->m_joysticks[0]->IsControllerPluggedIn()) {
        m_idleTime += dt;
        if (!(m_idleTime < m_attractDelay)) {
            PreparePlayback((PlaybackType)1);
            m_idleTime = 0;
        }
    }
    return 1;
}


// ---- src/JetskiRaceOverMenuCircuit/KeyDownHandler.cpp
class ProcessNode;
class KeyInfo;

int KEYDOWN_HANDLER_TEST(AssignableActionIndex action, DeviceType device, ProcessNode *node, unsigned int mods);



int JetskiRaceOverMenuCircuit::KeyDownHandler(ProcessNode *node, KeyInfo *key)
{
    if (KEYDOWN_HANDLER_TEST((AssignableActionIndex)4, (DeviceType)2, node, 0x80000000)) {
        m_closing = 1;
        m_closeAction = 1;
        return 1;
    } else if (KEYDOWN_HANDLER_TEST((AssignableActionIndex)5, (DeviceType)2, node, 0x80000000)) {
        m_closing = 1;
        m_closeAction = 2;
        return 1;
    }
    return JetskiMenu::KeyDownHandler(node, key);
}


// ---- src/JetskiRaceOverMenuCircuit/DisableMenu.cpp

class ScreenIconManager;







void JetskiRaceOverMenuCircuit::DisableMenu()
{
    LpGame->m_gameOptions->m_careerMode = 0;
    LpGame->m_competition->m_nameMenu->Initialize(0);
    DisableAll();
    LpGame->m_auralScape->Enable();
    LpGame->m_competition->ResetAllEngineSounds();
}


// ---- src/JetskiRaceOverMenuCircuit/SetupInfo.cpp



void JetskiRaceOverMenuCircuit::SetupInfo()
{
    int xOffset, yOffset;
    int i;
    JetskiMenu::SetupInfo();
    switch (m_viewport) {
    case 0:
    case 1:
    case 2:
        xOffset = 0;
        yOffset = 0;
        break;
    }
    for (i = 0; i < 2; i++) {
        ScreenText *text;
        switch (i) {
        case 0:
            text = m_labels[i];
            text->m_position.x = xOffset + 412;
            text->m_position.y = yOffset + 388;
            text->m_positionDirty = 1;
            text->m_dirty = 1;
            break;
        case 1:
            text = m_labels[i];
            text->m_position.x = xOffset + 298;
            text->m_position.y = yOffset + 388;
            text->m_positionDirty = 1;
            text->m_dirty = 1;
            break;
        }
    }
    for (i = 0; i < 20; i++) {
        int x, y;
        if (i >= 15) {
            x = xOffset + 498;
            y = yOffset + 140 + (i % 5) * 30;
        } else if (i >= 10) {
            x = xOffset + 398;
            y = yOffset + 140 + (i % 5) * 30;
        } else if (i >= 5) {
            x = xOffset + 300;
            y = yOffset + 140 + (i % 5) * 30;
        } else {
            x = xOffset + 98;
            y = yOffset + 140 + (i % 5) * 30;
        }
        ScreenText *text = m_texts[i];
        text->m_position.x = x;
        text->m_position.y = y;
        text->m_positionDirty = 1;
        text->m_dirty = 1;
    }
    m_idleTime = 0;
}


// ---- src/JetskiRaceOverMenuCountdown/ctor.cpp


JetskiRaceOverMenuCountdown::JetskiRaceOverMenuCountdown(int enabled)
    : JetskiMenu(enabled)
{
    m_textManager = 0;
    m_labels = 0;
    m_bigTexts = 0;
    m_text108 = m_text10C = 0;
    m_font110 = m_font114 = 0;
    m_font118 = 0;
    m_x11C = 0;
    m_x120 = 5.0f;
    m_columns = 0;
    m_numTexts = 0;
    m_x12C = 0;
    m_timeColumn = 0;
    m_scoreColumn = 0;
}


// ---- src/JetskiRaceOverMenuCountdown/dtor.cpp
extern "C" void free(void *p);





JetskiRaceOverMenuCountdown::~JetskiRaceOverMenuCountdown()
{
    int i;
    for (i = 0; i < 2; i++)
        delete m_labels[i];
    free(m_labels);
    m_labels = 0;
    for (i = 0; i < 2; i++)
        delete m_bigTexts[i];
    free(m_bigTexts);
    m_bigTexts = 0;
    for (i = 0; i < m_numTexts; i++)
        delete m_texts[i];
    free(m_texts);
    m_texts = 0;
    delete m_text108;
    delete m_text10C;
    m_text108 = m_text10C = 0;
}


// ---- src/JetskiRaceOverMenuCountdown/Create.cpp
// GameText::GetString (with its ASSERTF) is inlined inside SetString.


extern "C" void *malloc(unsigned int size);



// Retail's ScreenText::SetJustification takes its argument by const
// reference (the shared ScreenText.h takes it by value): the global is then
// loaded inside the setter, which allocates its register before the text's.

// Colours go through block-local copies of the colour and then of the text
// pointer (ScreenText::SetColor copies again): that is what gives retail's
// register choice for the colour constants and the texts.
#define SET_COLOR(text, value)              \
    {                                       \
        unsigned int color = value;         \
        ScreenText *colored = (text);       \
        colored->SetColor(color);           \
    }

// Creates the results table: two headers, two labels, three texts per player
// (name, time, score) and the two title lines.
JetskiRaceOverMenuCountdown *JetskiRaceOverMenuCountdown::Create(ScreenIconManager *icons, ScreenTextManager *texts, ViewPortCode viewport)
{
    int i;
    JetskiMenu::Create(icons, viewport);
    BuildMenuSmall(6);
    ScreenTextFontItem *item = LpGame->m_fonts->GetScreenTextFont("info_b");
    m_font110 = item ? item->m_font : 0;
    item = LpGame->m_fonts->GetScreenTextFont("title_b");
    m_font114 = item ? item->m_font : 0;
    item = LpGame->m_fonts->GetScreenTextFont("info_c");
    m_font118 = item ? item->m_font : 0;
    m_textManager = (new ScreenTextManager(0, 1))->Create(64);
    AddChild(m_textManager, -1);
    if (m_viewport == 0)
        m_columns = 1;
    else
        m_columns = 2;
    m_numTexts = m_columns * 3;
    m_x12C = 0;
    m_timeColumn = m_columns;
    m_scoreColumn = m_columns * 2;
    m_bigTexts = (JetskiScreenText **)malloc(2 * sizeof(JetskiScreenText *));
    m_labels = (ScreenText **)malloc(2 * sizeof(ScreenText *));
    m_texts = (ScreenText **)malloc(m_numTexts * sizeof(ScreenText *));
    for (i = 0; i < 2; i++) {
        m_bigTexts[i] = new JetskiScreenText();
        m_bigTexts[i]->m_params.m_color = -1;
        m_bigTexts[i]->m_params.m_font = m_font110;
        m_bigTexts[i]->m_params.m_justification = ScreenTextManager::CenterHorizontalJustification;
        m_bigTexts[i]->m_params.m_lineSpacing = 20;
        switch (i) {
        case 0:
            m_bigTexts[i]->m_params.m_x = 310;
            m_bigTexts[i]->m_params.m_y = 170;
            m_bigTexts[i]->SetText(89, 0);
            break;
        case 1:
            m_bigTexts[i]->m_params.m_x = 430;
            m_bigTexts[i]->m_params.m_y = 170;
            m_bigTexts[i]->SetText(77, 0);
            break;
        }
    }
    for (i = 0; i < 2; i++) {
        m_labels[i] = new ScreenText(40, 0);
        SetJustification(m_labels[i], ScreenTextManager::LeftJustification);
        m_labels[i]->SetFont(m_font118);
        SET_COLOR(m_labels[i], 0xFFFFFFFF)
        switch (i) {
        case 0:
            m_labels[i]->SetString(LpGame->m_text->GetString(13));
            m_labels[i]->SetPosition(382.0f, 305.0f);
            m_textManager->PositionText(m_labels[0]);
            break;
        case 1:
            m_labels[i]->SetString(LpGame->m_text->GetString(126));
            m_labels[i]->SetPosition(268.0f, 305.0f);
            m_textManager->PositionText(m_labels[1]);
            break;
        }
    }
    for (i = 0; i < m_numTexts; i++) {
        m_texts[i] = new ScreenText(40, 0);
        m_texts[i]->SetString("");
        SET_COLOR(m_texts[i], 0xFFFFFFFF)
        m_texts[i]->SetFont(m_font110);
        if (i >= m_scoreColumn)
            SetJustification(m_texts[i], ScreenTextManager::LeftJustification);
        else if (i >= m_timeColumn)
            SetJustification(m_texts[i], ScreenTextManager::CenterHorizontalJustification);
        else
            SetJustification(m_texts[i], ScreenTextManager::CenterHorizontalJustification);
    }
    m_text108 = new ScreenText(40, 0);
    m_text108->SetScale(1.059f);
    m_text108->SetString(LpGame->m_text->GetString(85));
    SET_COLOR(m_text108, 0xFF000000)
    m_text108->SetFont(m_font114);
    SetJustification(m_text108, ScreenTextManager::CenterHorizontalJustification);
    m_text108->SetPosition(320.0f, 122.0f);
    m_textManager->PositionText(m_text108);
    m_text10C = new ScreenText(40, 0);
    m_text10C->SetScale(1.059f);
    m_text10C->SetString(LpGame->m_text->GetString(86));
    SET_COLOR(m_text10C, 0xFFFFFFFF)
    m_text10C->SetFont(m_font114);
    SetJustification(m_text10C, ScreenTextManager::CenterHorizontalJustification);
    m_text10C->SetPosition(317.0f, 119.0f);
    m_textManager->PositionText(m_text10C);
    return this;
}


// ---- src/JetskiRaceOverMenuCountdown/PrepareGeometry.cpp

class ScreenIconManager;


// Keep the labels at least 10x scale, and refresh the info when asked.
int JetskiRaceOverMenuCountdown::PrepareGeometry()
{
    TextureMap *image;
    if (m_font110) {
        image = m_font110->m_texture;
        if (10.0f > image->m_usage)
            image->m_usage = 10.0f;
    }
    if (m_font114) {
        image = m_font114->m_texture;
        if (10.0f > image->m_usage)
            image->m_usage = 10.0f;
    }
    if (m_font118) {
        image = m_font118->m_texture;
        if (10.0f > image->m_usage)
            image->m_usage = 10.0f;
    }
    if (m_infoDirty) {
        SetupInfo();
        m_infoDirty = 0;
    }
    return JetskiMenu::PrepareGeometry();
}


// ---- src/JetskiRaceOverMenuCountdown/Render3D.cpp


// A per-file switch (retail has one copy in each file that uses it).


// Draw once per frame, on the last player's pass.
int JetskiRaceOverMenuCountdown::Render3D()
{
    if (LpGame->m_viewport == LpGame->m_numCameras - 1) {
        if (WaitForRenderingToFinish && m_viewport) {
            LpRenderTarget->TrySendRenderQueue(1);
            LpRenderTarget->WaitForRenderingToComplete();
        }
        SetViewport();
        JetskiMenu::Render3D();
        RenderLabels();
        RenderText();
        GameObject::Render3D();
        RestoreViewport();
    }
    return 1;
}


// ---- src/JetskiRaceOverMenuCountdown/RenderLabels.cpp
class ScreenText;
class ScreenTextFont;




void JetskiRaceOverMenuCountdown::RenderLabels()
{
    for (int i = 0; i < 2; i++)
        m_bigTexts[i]->AddText(m_textManager);
    m_textManager->AddText(m_labels[0], 0);
    m_textManager->AddText(m_labels[1], 0);
    m_textManager->AddText(m_text108, 0);
    m_textManager->AddText(m_text10C, 0);
}


// ---- src/JetskiRaceOverMenuCountdown/RenderText.cpp
extern "C" int sprintf(char *buf, const char *fmt, ...);
extern "C" char *strcpy(char *dst, const char *src);
extern "C" char *strncpy(char *dst, const char *src, unsigned int n);

class ScreenTextFont;










void JetskiRaceOverMenuCountdown::RenderText()
{
    char buffer[0x100];
    JetskiCareerCompetition *data = (JetskiCareerCompetition *)LpGame->m_competition;
    JetskiGameOptions *options = LpGame->m_gameOptions;
    RaceStatusStruct_Tag *status;
    int i;

    for (status = data->m_raceStatus->m_vehicles, i = 0; status; status = status->m_next, i++) {
        if (data->m_vehicles[status->m_id]->m_player >= 0) {
            m_texts[i]->SetColor(0xFFFFDE00);
            m_texts[i + m_timeColumn]->SetColor(0xFFFFDE00);
            m_texts[i + m_scoreColumn]->SetColor(0xFFFFDE00);
        } else {
            m_texts[i]->SetColor(0xFFFFFFFF);
            m_texts[i + m_timeColumn]->SetColor(0xFFFFFFFF);
            m_texts[i + m_scoreColumn]->SetColor(0xFFFFFFFF);
        }
        options->GetCharacterName(options->m_racerCharacter[status->m_id], buffer);
        m_texts[i]->SetString(buffer);
        m_textManager->PositionText(m_texts[i]);
        m_textManager->AddText(m_texts[i], 0);

        float t = status->m_x6D4;
        if (t == 0.0f || t == 3.4028235e38f) {
            strcpy(buffer, "--:--.--");
        } else {
            int minutes;
            if (t < 0.0f)
                t = -1.0f * t;
            if (t >= 5999.99f) {
                t = 59.99f;
                minutes = 99;
            } else {
                for (minutes = 0; !(t < 60.0f); minutes++)
                    t -= 60.0f;
            }
            sprintf(buffer, "%2d:%05.2f", minutes, t);
        }
        m_texts[i + m_timeColumn]->SetString(buffer);
        m_textManager->PositionText(m_texts[i + m_timeColumn]);
        m_textManager->AddText(m_texts[i + m_timeColumn], 0);

        sprintf(buffer, "%d", data->m_vehicles[status->m_id]->m_timeElimBuoy);
        m_texts[i + m_scoreColumn]->SetString(buffer);
        m_textManager->PositionText(m_texts[i + m_scoreColumn]);
        m_textManager->AddText(m_texts[i + m_scoreColumn], 0);
    }
}


// ---- src/JetskiRaceOverMenuCountdown/Tick.cpp

class ScreenIconManager;



// Act on the choice once every key is released.
int JetskiRaceOverMenuCountdown::Tick(float dt)
{
    int held = 0;
    if (m_closing) {
        for (int i = 0; i < 7; i++) {
            if (m_timers[i].m_active)
                held = 1;
        }
        if (!held) {
            switch (m_closeAction) {
            case 1:
                DisableMenu();
                break;
            case 2:
                DisableAll();
                LpGame->m_auralScape->Enable();
                LpGame->m_competition->ResetAllEngineSounds();
                LpGame->m_competition->Pause(0);
                LpGame->m_competition->RestartRace((GamePlayStatus)3);
                break;
            }
            m_closing = 0;
            m_closeAction = 0;
        }
    }
    JetskiMenu::Tick(dt);
    return 1;
}


// ---- src/JetskiRaceOverMenuCountdown/KeyDownHandler.cpp
class ProcessNode;
class KeyInfo;

int KEYDOWN_HANDLER_TEST(AssignableActionIndex action, DeviceType device, ProcessNode *node, unsigned int mods);




int JetskiRaceOverMenuCountdown::KeyDownHandler(ProcessNode *node, KeyInfo *key)
{
    if (KEYDOWN_HANDLER_TEST((AssignableActionIndex)4, (DeviceType)2, node, 0x80000000)) {
        m_closing = 1;
        m_closeAction = 1;
        return 1;
    } else if (KEYDOWN_HANDLER_TEST((AssignableActionIndex)5, (DeviceType)2, node, 0x80000000)) {
        m_closing = 1;
        m_closeAction = 2;
        return 1;
    }
    return JetskiMenu::KeyDownHandler(node, key);
}


// ---- src/JetskiRaceOverMenuCountdown/DisableMenu.cpp

class ScreenIconManager;







void JetskiRaceOverMenuCountdown::DisableMenu()
{
    LpGame->m_gameOptions->m_careerMode = 0;
    LpGame->m_competition->m_nameMenu->Initialize(0);
    DisableAll();
    LpGame->m_auralScape->Enable();
    LpGame->m_competition->ResetAllEngineSounds();
}


// ---- src/JetskiRaceOverMenuCountdown/SetupInfo.cpp


// Lay the result texts out in three rows. (Retail leaves the offsets unset for
// modes other than 0-2.)
void JetskiRaceOverMenuCountdown::SetupInfo()
{
    int xOffset, yOffset;
    JetskiMenu::SetupInfo();
    switch (m_viewport) {
    case 0:
    case 1:
    case 2:
        xOffset = 0;
        yOffset = 0;
        break;
    }
    int left = yOffset + 220;
    for (int i = 0; i < m_numTexts; i++) {
        int x, y;
        if (i >= m_scoreColumn) {
            x = xOffset + 430;
            y = left + (i % m_columns) * 30;
        } else if (i >= m_timeColumn) {
            x = xOffset + 310;
            y = left + (i % m_columns) * 30;
        } else {
            x = xOffset + 150;
            y = left + (i % m_columns) * 30;
        }
        ScreenText *text = m_texts[i];
        text->m_position.x = x;
        text->m_position.y = y;
        text->m_positionDirty = 1;
        text->m_dirty = 1;
    }
    m_x11C = 0;
}


// ---- src/JetskiRaceOverMenuCareer/ctor.cpp

JetskiRaceOverMenuCareer::JetskiRaceOverMenuCareer(int enabled) : JetskiMenu(enabled)
{
    m_textManager = 0;
    m_rows = 0;
    m_title = m_titleShadow = 0;
    m_x108 = m_x10C = 0;
    m_prompts[0] = m_prompts[1] = 0;
    m_lines[0] = m_lines[1] = m_lines[2] = 0;
    m_headers = 0;
    m_columns = 0;
    m_cells = 0;
    m_circuitInfoDirty = 1;
    m_idleTime = 0;
    m_attractDelay = 5.0f;
}


// ---- src/JetskiRaceOverMenuCareer/dtor.cpp
extern "C" void free(void *p);

class KeyInfo;
class ProcessNode;
class ScreenIconManager;




JetskiRaceOverMenuCareer::~JetskiRaceOverMenuCareer()
{
    int i;
    for (i = 0; i < 4; i++)
        delete m_headers[i];
    free(m_headers);
    m_headers = 0;
    for (i = 0; i < 5; i++)
        delete m_columns[i];
    free(m_columns);
    m_columns = 0;
    for (i = 0; i < 25; i++)
        delete m_cells[i];
    free(m_cells);
    m_cells = 0;
    for (i = 0; i < 6; i++)
        delete m_rows[i];
    free(m_rows);
    m_rows = 0;
    delete m_x108;
    m_x108 = 0;
    delete m_x10C;
    m_x10C = 0;
    delete m_title;
    delete m_titleShadow;
    m_title = m_titleShadow = 0;
    delete m_prompts[0];
    delete m_prompts[1];
    m_prompts[0] = m_prompts[1] = 0;
    delete m_lines[0];
    delete m_lines[1];
    delete m_lines[2];
    m_lines[0] = m_lines[1] = m_lines[2] = 0;
    m_smallFont = m_titleFont = m_font = 0;
}


// ---- src/JetskiRaceOverMenuCareer/Create.cpp
extern "C" char *strncpy(char *dst, const char *src, unsigned int n);
extern "C" void *malloc(unsigned int size);

class ScreenTextFont;








class ScreenIconManager;




// Creates the career results menu's fonts and texts: six result rows, the place
// lines, four column headers, the records table and the prompt lines.
JetskiRaceOverMenuCareer *JetskiRaceOverMenuCareer::Create(ScreenIconManager *icons, ScreenTextManager *texts, ViewPortCode viewport)
{
    int i;
    JetskiMenu::Create(icons, viewport);
    BuildMenuFull(2);
    BuildMenuSmall(11);
    m_x60 = 0;
    m_x68 = 1;
    ScreenTextFontItem *item = LpGame->m_fonts->GetScreenTextFont("info_b");
    m_font = item ? item->m_font : 0;
    item = LpGame->m_fonts->GetScreenTextFont("title_b");
    m_titleFont = item ? item->m_font : 0;
    item = LpGame->m_fonts->GetScreenTextFont("info_c");
    m_smallFont = item ? item->m_font : 0;
    m_textManager = (new ScreenTextManager(0, 1))->Create(64);
    AddChild(m_textManager, -1);
    m_rows = (ScreenText **)malloc(6 * sizeof(ScreenText *));
    for (i = 0; i < 6; i++) {
        m_rows[i] = new ScreenText(40, 0);
        m_rows[i]->SetScale(1.286f);
        m_rows[i]->SetString("");
        m_rows[i]->SetColor(0xFFFFFFFF);
        m_rows[i]->SetPosition(320.0f, 167 + i * 30);
    }
    m_x108 = new ScreenText(40, 0);
    m_x108->SetScale(1.125f);
    m_x108->SetColor(0xFFFFFFFF);
    m_x108->SetPosition(320.0f, 117.0f);
    m_x10C = new ScreenText(40, 0);
    m_x10C->SetScale(1.125f);
    m_x10C->SetColor(0xFFFFFFFF);
    m_x10C->SetPosition(320.0f, 137.0f);
    m_headers = (JetskiScreenText **)malloc(4 * sizeof(JetskiScreenText *));
    for (i = 0; i < 4; i++) {
        m_headers[i] = new JetskiScreenText();
        m_headers[i]->m_params.m_color = -1;
        m_headers[i]->m_params.m_font = m_font;
        m_headers[i]->m_params.m_justification = ScreenTextManager::CenterHorizontalJustification;
        m_headers[i]->m_params.m_lineSpacing = 20;
        switch (i) {
        case 0:
            m_headers[i]->m_params.m_x = 242;
            m_headers[i]->m_params.m_y = 90;
            m_headers[i]->SetText(88, 0);
            break;
        case 1:
            m_headers[i]->m_params.m_x = 310;
            m_headers[i]->m_params.m_y = 90;
            m_headers[i]->SetText(89, 0);
            break;
        case 2:
            m_headers[i]->m_params.m_x = 408;
            m_headers[i]->m_params.m_y = 90;
            m_headers[i]->SetText(90, 0);
            break;
        case 3:
            m_headers[i]->m_params.m_x = 508;
            m_headers[i]->m_params.m_y = 90;
            m_headers[i]->SetText(72, 0);
            break;
        }
    }
    m_columns = (ScreenText **)malloc(5 * sizeof(ScreenText *));
    for (i = 0; i < 5; i++) {
        m_columns[i] = new ScreenText(44, 0);
        m_columns[i]->SetString("");
        m_columns[i]->SetJustification(ScreenTextManager::CenterHorizontalJustification);
        m_columns[i]->SetFont(m_font);
        m_columns[i]->SetColor(0xFFFFDE00);
    }
    m_cells = (ScreenText **)malloc(25 * sizeof(ScreenText *));
    for (i = 0; i < 25; i++) {
        m_cells[i] = new ScreenText(40, 0);
        m_cells[i]->SetString("");
        m_cells[i]->SetColor(0xFFFFFFFF);
    }
    m_title = new ScreenText(40, 0);
    m_title->SetColor(0xFF000000);
    m_titleShadow = new ScreenText(40, 0);
    m_titleShadow->SetColor(0xFFFFFFFF);
    for (i = 0; i < 2; i++) {
        m_prompts[i] = new ScreenText(40, 0);
        m_prompts[i]->SetColor(0xFFFFFFFF);
        m_prompts[i]->SetScale(0.857f);
        m_prompts[i]->SetJustification(ScreenTextManager::LeftJustification);
        m_prompts[i]->SetFont(m_smallFont);
    }
    m_lines[0] = new ScreenText(40, 0);
    m_lines[0]->SetScale(1.0f);
    m_lines[0]->SetColor(0xFFFFFFFF);
    m_lines[0]->SetPosition(320.0f, 185.0f);
    m_lines[1] = new ScreenText(40, 0);
    m_lines[1]->SetScale(1.0f);
    m_lines[1]->SetColor(0xFFFFFFFF);
    m_lines[1]->SetPosition(320.0f, 215.0f);
    m_lines[2] = new ScreenText(40, 0);
    m_lines[2]->SetScale(1.0f);
    m_lines[2]->SetColor(0xFFFFFFFF);
    m_lines[2]->SetPosition(320.0f, 245.0f);
    return this;
}



// ---- src/JetskiRaceOverMenuCareer/PrepareGeometry.cpp

// The competition is a JetskiCareerCompetition here (no shared header yet).

// Raise the fonts' texture usage to 10, rebuild the info panel if it was
// invalidated, then let the base menu prepare its geometry.
int JetskiRaceOverMenuCareer::PrepareGeometry()
{
    JetskiCareerCompetition *competition = (JetskiCareerCompetition *)LpGame->m_competition;
    if (m_font) {
        TextureMap *texture = m_font->m_texture;
        if (10.0f > texture->m_usage)
            texture->m_usage = 10.0f;
    }
    if (m_titleFont) {
        TextureMap *texture = m_titleFont->m_texture;
        if (10.0f > texture->m_usage)
            texture->m_usage = 10.0f;
    }
    if (m_smallFont) {
        TextureMap *texture = m_smallFont->m_texture;
        if (10.0f > texture->m_usage)
            texture->m_usage = 10.0f;
    }
    switch (competition->m_raceType) {
    case 1:
        if (m_infoDirty) {
            SetupInfo();
            m_infoDirty = 0;
        }
        break;
    case 2:
        if (m_circuitInfoDirty) {
            SetupInfo();
            m_circuitInfoDirty = 0;
        }
        break;
    }
    return JetskiMenu::PrepareGeometry();
}


// ---- src/JetskiRaceOverMenuCareer/Render3D.cpp


// The competition is a JetskiCareerCompetition here (no shared header yet).




// A per-file switch (retail has one copy in each file that uses it).

// Draw once per frame, on the last player's pass.
int JetskiRaceOverMenuCareer::Render3D()
{
    if (LpGame->m_viewport == LpGame->m_numCameras - 1) {
        if (WaitForRenderingToFinish && m_viewport) {
            LpRenderTarget->TrySendRenderQueue(1);
            LpRenderTarget->WaitForRenderingToComplete();
        }
        SetViewport();
        JetskiMenu::Render3D();
        JetskiCareerCompetition *competition = (JetskiCareerCompetition *)LpGame->m_competition;
        if (m_x48 == 1) {
            switch (competition->m_raceType) {
            case 1:
                RenderQualifier();
                break;
            case 2:
                RenderCircuit();
                break;
            }
        }
        if (m_x60 == 1)
            RenderReQual();
        GameObject::Render3D();
        RestoreViewport();
    }
    return 1;
}



// ---- src/JetskiRaceOverMenuCareer/RenderQualifier.cpp
// Compiler session state: retail compiled
// this file in an mwcc session that had already generated a two-argument runtime
// helper call (64-bit / or %, double arithmetic or compare). mwcc then models
// every later helper call (__floatdisf, fptoui, fptodp, ...) as reading $a0-$a1,
// so temporaries live before such a call skip $a0/$a1. This unused function
// recreates that state; it is not part of the retail file.
static void MWCC_SHIM_TEXT MwccSessionState(long *a) { a[0] = a[0] / a[1]; }



extern "C" char *strncpy(char *dst, const char *src, unsigned int n);
extern "C" int sprintf(char *buf, const char *fmt, ...);
extern "C" char *strcpy(char *dst, const char *src);



class ScreenTextFont;










static inline void FormatTime2(char *buffer, float t)
{
    if (t == 0.0f || t == 3.4028235e38f) {
        strcpy(buffer, "--:--.--");
    } else {
        int minutes;
        if (t < 0.0f)
            t = -1.0f * t;
        if (t >= 5999.99f) {
            t = 59.99f;
            minutes = 99;
        } else {
            for (minutes = 0; !(t < 60.0f); minutes++)
                t -= 60.0f;
        }
        sprintf(buffer, "%2d:%05.2f", minutes, t);
    }
}

// The qualifying results: the title, whether the player qualified (and in
// which place), and the best qualifying laps with the player's highlighted.
void JetskiRaceOverMenuCareer::RenderQualifier()
{
    char buffer[0x100];
    char name[16];
    JetskiCareerCompetition *data = (JetskiCareerCompetition *)LpGame->m_competition;
    JetskiCareer *career = LpGame->m_gameOptions->m_career;
    m_title->SetPosition(320.0f, 39.0f);
    m_titleShadow->SetPosition(317.0f, 36.0f);
    m_title->SetScale(1.059f);
    m_titleShadow->SetScale(1.059f);
    m_title->SetString(LpGame->m_text->GetString(0x5C));
    m_textManager->PositionText(m_title, ScreenTextManager::CenterHorizontalJustification, m_titleFont);
    m_textManager->AddText(m_title, m_titleFont);
    m_titleShadow->SetString(LpGame->m_text->GetString(0x5D));
    m_textManager->PositionText(m_titleShadow, ScreenTextManager::CenterHorizontalJustification, m_titleFont);
    m_textManager->AddText(m_titleShadow, m_titleFont);
    if (data->m_qualified) {
        m_x108->SetString(LpGame->m_text->GetString(0x5F));
        sprintf(buffer, "%d", data->m_qualifyPlace);
        m_x10C->SetString(buffer);
        m_prompts[0]->SetString(LpGame->m_text->GetString(0x6C));
        m_prompts[1]->SetString(LpGame->m_text->GetString(0x6D));
    } else {
        m_x108->SetString(LpGame->m_text->GetString(0x60));
        m_x10C->SetString("");
        m_prompts[0]->SetString(LpGame->m_text->GetString(0x7F));
        m_prompts[1]->SetString(LpGame->m_text->GetString(0x6D));
    }
    m_textManager->PositionText(m_x108, ScreenTextManager::CenterHorizontalJustification, m_font);
    m_textManager->AddText(m_x108, m_font);
    m_textManager->PositionText(m_x10C, ScreenTextManager::CenterHorizontalJustification, m_font);
    m_textManager->AddText(m_x10C, m_font);
    m_prompts[0]->SetPosition(412.0f, 388.0f);
    m_prompts[1]->SetPosition(298.0f, 388.0f);
    m_textManager->PositionText(m_prompts[0]);
    m_textManager->PositionText(m_prompts[1]);
    m_textManager->AddText(m_prompts[0], 0);
    m_textManager->AddText(m_prompts[1], 0);
    for (int i = 0; i < 6; i++) {
        unsigned int white = 0xFFFFFFFF;    // locals as retail's registers need them
        ScreenText *row = m_rows[i];
        row->SetColor(white);
        if (i == 5) {
            if (data->m_qualified) {
                strcpy(name, "");
                m_rows[i]->SetString("");
            } else {
                m_rows[i]->SetColor(0xFFFFDE00);
                FormatTime2(buffer, data->m_vehicles[0]->m_ownStatus->m_bestLap);
                m_rows[i]->SetString(buffer);
            }
        } else {
            FormatTime2(name, career->m_events[career->m_currentCircuit].m_qualifyTimes[i]);
            if (data->m_qualified && i + 1 == data->m_qualifyPlace) {
                m_rows[i]->SetColor(0xFFFFDE00);
                FormatTime2(name, data->m_vehicles[0]->m_ownStatus->m_bestLap);
            }
            sprintf(buffer, "%d. %s", i + 1, name);
            m_rows[i]->SetString(buffer);
        }
        m_textManager->PositionText(m_rows[i], ScreenTextManager::CenterHorizontalJustification, m_font);
        m_textManager->AddText(m_rows[i], m_font);
    }
}


// ---- src/JetskiRaceOverMenuCareer/RenderCircuit.cpp
// Compiler session state: retail compiled
// this file in an mwcc session that had already generated a two-argument runtime
// helper call (64-bit / or %, double arithmetic or compare). mwcc then models
// every later helper call (__floatdisf, fptoui, fptodp, ...) as reading $a0-$a1,
// so temporaries live before such a call skip $a0/$a1. This unused function
// recreates that state; it is not part of the retail file.



extern "C" char *strncpy(char *dst, const char *src, unsigned int n);
extern "C" int sprintf(char *buf, const char *fmt, ...);
extern "C" char *strcpy(char *dst, const char *src);



class ScreenTextFont;








int GetGameTextIDFromCourse(int course);






static inline void FormatTime3(char *buffer, float t)
{
    if (t == 0.0f || t == 3.4028235e38f) {
        strcpy(buffer, "--:--.--");
    } else {
        int minutes;
        if (t < 0.0f)
            t = -1.0f * t;
        if (t >= 5999.99f) {
            t = 59.99f;
            minutes = 99;
        } else {
            for (minutes = 0; !(t < 60.0f); minutes++)
                t -= 60.0f;
        }
        sprintf(buffer, "%2d:%05.2f", minutes, t);
    }
}

// The circuit standings: one row per racer with name, points for the
// event, total time, gap to the leader and best lap.
void JetskiRaceOverMenuCareer::RenderCircuit()
{
    char time[0x100];
    char points[0x100];
    JetskiCareerCompetition *data = (JetskiCareerCompetition *)LpGame->m_competition;
    JetskiCareer *career = LpGame->m_gameOptions->m_career;
    int i;
    m_title->SetPosition(320.0f, 39.0f);
    m_titleShadow->SetPosition(317.0f, 36.0f);
    m_title->SetScale(1.059f);
    m_titleShadow->SetScale(1.059f);
    m_title->SetString(LpGame->m_text->GetString(0x61));
    m_textManager->PositionText(m_title, ScreenTextManager::CenterHorizontalJustification, m_titleFont);
    m_textManager->AddText(m_title, m_titleFont);
    m_titleShadow->SetString(LpGame->m_text->GetString(0x62));
    m_textManager->PositionText(m_titleShadow, ScreenTextManager::CenterHorizontalJustification, m_titleFont);
    m_textManager->AddText(m_titleShadow, m_titleFont);
    for (i = 0; i < 4; i++)
        m_headers[i]->AddText(m_textManager);
    if (!data->m_circuitOver) {
        if (data->m_circuitQualified) {
            m_columns[0]->SetString(LpGame->m_text->GetString(0x63));
            m_columns[1]->SetString(LpGame->m_text->GetString(0x64));
            if (data->m_unlockedTrack != -1) {
                m_columns[2]->SetString(LpGame->m_text->GetString(0x9E));
                m_columns[3]->SetString(LpGame->m_text->GetString(GetGameTextIDFromCourse(data->m_unlockedTrack)));
                switch (((JetskiCareerCompetition *)LpGame->m_competition)->m_unlockType) {
                case 0:
                    break;
                case 1:
                    m_columns[4]->SetString(LpGame->m_text->GetString(0x72));
                    break;
                case 2:
                    m_columns[4]->SetString(LpGame->m_text->GetString(0x71));
                    break;
                case 3:
                    m_columns[4]->SetString(LpGame->m_text->GetString(0x73));
                    break;
                }
                m_textManager->PositionText(m_columns[2]);
                m_textManager->AddText(m_columns[2], 0);
                m_textManager->PositionText(m_columns[3]);
                m_textManager->AddText(m_columns[3], 0);
                m_textManager->PositionText(m_columns[4]);
                m_textManager->AddText(m_columns[4], 0);
            }
            m_prompts[0]->SetString(LpGame->m_text->GetString(0x6B));
            m_prompts[1]->SetString(LpGame->m_text->GetString(0x7E));
        } else {
            m_columns[0]->SetString(LpGame->m_text->GetString(0x65));
            m_columns[1]->SetString(LpGame->m_text->GetString(0x66));
            m_prompts[0]->SetString(LpGame->m_text->GetString(0x8B));
            m_prompts[1]->SetString(LpGame->m_text->GetString(0x7E));
        }
        m_textManager->PositionText(m_columns[0]);
        m_textManager->AddText(m_columns[0], 0);
        m_textManager->PositionText(m_columns[1]);
        m_textManager->AddText(m_columns[1], 0);
    } else {
        m_prompts[0]->SetString(LpGame->m_text->GetString(0x80));
        m_prompts[1]->SetString(LpGame->m_text->GetString(0x7E));
    }
    m_prompts[0]->SetPosition(412.0f, 388.0f);
    m_prompts[1]->SetPosition(298.0f, 388.0f);
    m_textManager->PositionText(m_prompts[0]);
    m_textManager->PositionText(m_prompts[1]);
    m_textManager->AddText(m_prompts[0], 0);
    m_textManager->AddText(m_prompts[1], 0);
    RaceStatusStruct_Tag *standing = data->m_raceStatus->m_vehicles;
    RaceStatusStruct_Tag *leader = standing;
    ScreenText unused(40, 0);           // never used (retail constructs and destroys it)
    _D3DVECTOR unusedPosition;
    int score;
    i = 0;
    while (standing) {
        if (data->m_vehicles[standing->m_id]->m_player >= 0) {
            m_cells[i]->SetColor(0xFFFFDE00);
            m_cells[i + 5]->SetColor(0xFFFFDE00);
            m_cells[i + 10]->SetColor(0xFFFFDE00);
            m_cells[i + 15]->SetColor(0xFFFFDE00);
            m_cells[i + 20]->SetColor(0xFFFFDE00);
        } else {
            m_cells[i]->SetColor(0xFFFFFFFF);
            m_cells[i + 5]->SetColor(0xFFFFFFFF);
            m_cells[i + 10]->SetColor(0xFFFFFFFF);
            m_cells[i + 15]->SetColor(0xFFFFFFFF);
            m_cells[i + 20]->SetColor(0xFFFFFFFF);
        }
        m_cells[i]->SetString(career->m_racers[data->m_racerIndex[standing->m_id]].m_name);
        m_textManager->PositionText(m_cells[i], ScreenTextManager::LeftJustification, m_font);
        m_textManager->AddText(m_cells[i], m_font);
        switch (i) {    // past fifth place score keeps its last value
        case 0:
            score = 10;
            break;
        case 1:
            score = 8;
            break;
        case 2:
            score = 5;
            break;
        case 3:
            score = 3;
            break;
        case 4:
            score = 2;
            break;
        }
        sprintf(points, "%d", score);
        m_cells[i + 5]->SetString(points);
        m_textManager->PositionText(m_cells[i + 5], ScreenTextManager::CenterHorizontalJustification, m_font);
        m_textManager->AddText(m_cells[i + 5], m_font);
        FormatTime3(time, standing->m_x6D4);
        m_cells[i + 10]->SetString(time);
        m_textManager->PositionText(m_cells[i + 10], ScreenTextManager::CenterHorizontalJustification, m_font);
        m_textManager->AddText(m_cells[i + 10], m_font);
        float gap = standing->m_x6D4 - leader->m_x6D4;
        if (gap) {
            FormatTime3(time, gap);
        } else {
#line 1518 "JetskiRaceOverMenus.cpp"
            STRCPY(time, "0:00:00");
        }
        m_cells[i + 15]->SetString(time);
        m_textManager->PositionText(m_cells[i + 15], ScreenTextManager::CenterHorizontalJustification, m_font);
        m_textManager->AddText(m_cells[i + 15], m_font);
        FormatTime3(time, standing->m_bestLap);
        m_cells[i + 20]->SetString(time);
        m_textManager->PositionText(m_cells[i + 20], ScreenTextManager::CenterHorizontalJustification, m_font);
        m_textManager->AddText(m_cells[i + 20], m_font);
        standing = standing->m_next;
        i++;
    }
}


// ---- src/JetskiRaceOverMenuCareer/RenderReQual.cpp
#pragma alignlabel off

extern "C" char *strncpy(char *dst, const char *src, unsigned int n);



class ScreenTextFont;





// The "re-qualify" screen of the career results.
void JetskiRaceOverMenuCareer::RenderReQual()
{
    m_title->SetPosition(320.0f, 122.0f);
    m_titleShadow->SetPosition(317.0f, 119.0f);
    m_title->SetScale(1.0f);
    m_titleShadow->SetScale(1.0f);
    m_title->SetString(LpGame->m_text->GetString(0x40));
    m_textManager->PositionText(m_title, ScreenTextManager::CenterHorizontalJustification, m_titleFont);
    m_textManager->AddText(m_title, m_titleFont);
    m_titleShadow->SetString("Replay");
    m_textManager->PositionText(m_titleShadow, ScreenTextManager::CenterHorizontalJustification, m_titleFont);
    m_textManager->AddText(m_titleShadow, m_titleFont);
    m_lines[0]->SetString(LpGame->m_text->GetString(0x6E));
    m_textManager->PositionText(m_lines[0], ScreenTextManager::CenterHorizontalJustification, m_font);
    m_textManager->AddText(m_lines[0], m_font);
    m_lines[1]->SetString(LpGame->m_text->GetString(0x6F));
    m_textManager->PositionText(m_lines[1], ScreenTextManager::CenterHorizontalJustification, m_font);
    m_textManager->AddText(m_lines[1], m_font);
    m_lines[2]->SetString(LpGame->m_text->GetString(0x70));
    m_textManager->PositionText(m_lines[2], ScreenTextManager::CenterHorizontalJustification, m_font);
    m_textManager->AddText(m_lines[2], m_font);
    m_prompts[0]->SetString(LpGame->m_text->GetString(0x5E));
    m_prompts[1]->SetString(LpGame->m_text->GetString(0xC));
    m_prompts[0]->SetPosition(384.0f, 305.0f);
    m_prompts[1]->SetPosition(270.0f, 305.0f);
    m_textManager->PositionText(m_prompts[0]);
    m_textManager->PositionText(m_prompts[1]);
    m_textManager->AddText(m_prompts[0], 0);
    m_textManager->AddText(m_prompts[1], 0);
}


#pragma alignlabel reset

// ---- src/JetskiRaceOverMenuCareer/Tick.cpp
class KeyInfo;
class ProcessNode;










class ScreenIconManager;




int JetskiRaceOverMenuCareer::Tick(float dt)
{
    int busy = 0;
    if (m_closing) {
        for (int i = 0; i < 7; i++) {
            if (m_timers[i].m_active)
                busy = 1;
        }
        if (!busy) {
        JetskiCareerCompetition *competition = (JetskiCareerCompetition *)LpGame->m_competition;
        switch (m_closeAction) {
        case 1:
            if (competition->m_raceType == 1) {
                if (competition->m_qualified) {
                    if (m_x48) {
                        DisableMenu();
                    } else {
                        DisableAll();
                        LpGame->m_auralScape->Enable();
                        LpGame->m_competition->ResetAllEngineSounds();
                        LpGame->m_competition->Pause(0);
                        LpGame->m_competition->EnableOverlays();
                        LpGame->m_competition->RestartRace((GamePlayStatus)3);
                    }
                } else {
                    competition->EventComplete();
                }
            } else {
                DisableMenu();
            }
            break;
        case 2:
            if (competition->m_raceType == 1 && competition->m_qualified) {
                if (m_x48) {
                    m_x60 = 1;
                    m_x48 = 0;
                    m_infoDirty = 1;
                }
            } else {
                DisableAll();
                LpGame->m_auralScape->Enable();
                LpGame->m_competition->ResetAllEngineSounds();
                LpGame->m_competition->Pause(0);
                LpGame->m_competition->EnableOverlays();
                LpGame->m_competition->RestartRace((GamePlayStatus)3);
            }
            break;
        case 4:
            if (competition->m_raceType == 1 && m_x60) {
                m_x48 = 1;
                m_x60 = 0;
                m_infoDirty = 1;
                LpGame->m_competition->m_responses->TriggerResponse(3, 0, 0);
            }
            break;
        }
        m_closing = 0;
        m_closeAction = 0;
        }
    }
    JetskiMenu::Tick(dt);
    if (!m_viewport && LpGame->m_competition && !m_closing && LpGame->m_competition->m_vcr
        && LpGame->m_controls->m_joysticks[0]->IsControllerPluggedIn()) {
        m_idleTime += dt;
        if (!(m_idleTime < m_attractDelay)) {
            PreparePlayback((PlaybackType)1);
            m_idleTime = 0;
        }
    }
    return 1;
}



// ---- src/JetskiRaceOverMenuCareer/KeyDownHandler.cpp
class ProcessNode;
class KeyInfo;

int KEYDOWN_HANDLER_TEST(AssignableActionIndex action, DeviceType device, ProcessNode *node, unsigned int mods);




int JetskiRaceOverMenuCareer::KeyDownHandler(ProcessNode *node, KeyInfo *key)
{
    if (KEYDOWN_HANDLER_TEST((AssignableActionIndex)4, (DeviceType)2, node, 0x80000000)) {
        m_closing = 1;
        m_closeAction = 1;
        return 1;
    } else if (KEYDOWN_HANDLER_TEST((AssignableActionIndex)5, (DeviceType)2, node, 0x80000000)) {
        m_closing = 1;
        m_closeAction = 2;
        return 1;
    } else if (KEYDOWN_HANDLER_TEST((AssignableActionIndex)6, (DeviceType)2, node, 0x80000000)) {
        m_closing = 1;
        m_closeAction = 4;
        return 1;
    }
    return JetskiMenu::KeyDownHandler(node, key);
}


// ---- src/JetskiRaceOverMenuCareer/EnableMenu.cpp

// The competition is a JetskiCareerCompetition here (no shared header yet).

void JetskiRaceOverMenuCareer::EnableMenu()
{
    EnableAll();
    if (((JetskiCareerCompetition *)LpGame->m_competition)->m_raceType != 1)
        LpGame->m_competition->EndOfRaceRecordKeeping();
    m_x48 = 1;
    m_x60 = 0;
    m_infoDirty = 1;
    LpGame->m_auralScape->StopAllAuralScapeSounds();
    LpGame->m_auralScape->Disable();
}


// ---- src/JetskiRaceOverMenuCareer/DisableMenu.cpp

// The competition is a JetskiCareerCompetition here (no shared header yet).

void JetskiRaceOverMenuCareer::DisableMenu()
{
    JetskiGameOptions *mp = LpGame->m_gameOptions;
    if (((JetskiCareerCompetition *)LpGame->m_competition)->m_raceType == 1)
        mp->m_careerMode = 0;
    LpGame->m_competition->m_nameMenu->Initialize(0);
    LpGame->m_auralScape->Enable();
    LpGame->m_competition->ResetAllEngineSounds();
    DisableAll();
}


// ---- src/JetskiRaceOverMenuCareer/SetupInfo.cpp


void JetskiRaceOverMenuCareer::SetupInfo()
{
    int xOffset, yOffset;
    int i;
    ScreenText *text;
    JetskiMenu::SetupInfo();
    switch (m_viewport) {
    case 0:
    case 1:
    case 2:
        xOffset = 0;
        yOffset = 0;
        break;
    }
    for (i = 0; i < 5; i++) {
        switch (i) {
        case 2:
            text = m_columns[i];
            text->m_position.x = xOffset + 320;
            text->m_position.y = yOffset + 268;
            text->m_positionDirty = 1;
            text->m_dirty = 1;
            break;
        case 3:
            text = m_columns[i];
            text->m_position.x = xOffset + 320;
            text->m_position.y = yOffset + 287;
            text->m_positionDirty = 1;
            text->m_dirty = 1;
            break;
        case 4:
            text = m_columns[i];
            text->m_position.x = xOffset + 320;
            text->m_position.y = yOffset + 306;
            text->m_positionDirty = 1;
            text->m_dirty = 1;
            break;
        case 0:
            text = m_columns[i];
            text->m_position.x = xOffset + 320;
            text->m_position.y = yOffset + 331;
            text->m_positionDirty = 1;
            text->m_dirty = 1;
            break;
        case 1:
            text = m_columns[i];
            text->m_position.x = xOffset + 320;
            text->m_position.y = yOffset + 350;
            text->m_positionDirty = 1;
            text->m_dirty = 1;
            break;
        }
    }
    for (i = 0; i < 25; i++) {
        int x, y;
        if (i >= 20) {
            x = xOffset + 508;
            y = yOffset + 140 + (i % 5) * 25;
        } else if (i >= 15) {
            x = xOffset + 408;
            y = yOffset + 140 + (i % 5) * 25;
        } else if (i >= 10) {
            x = xOffset + 310;
            y = yOffset + 140 + (i % 5) * 25;
        } else if (i >= 5) {
            x = xOffset + 242;
            y = yOffset + 140 + (i % 5) * 25;
        } else {
            x = xOffset + 98;
            y = yOffset + 140 + (i % 5) * 25;
        }
        text = m_cells[i];
        text->m_position.x = x;
        text->m_position.y = y;
        text->m_positionDirty = 1;
        text->m_dirty = 1;
    }
    text = m_title;
    text->m_position.x = xOffset + 320;
    text->m_position.y = yOffset + 39;
    text->m_positionDirty = 1;
    text->m_dirty = 1;
    text = m_titleShadow;
    text->m_position.x = xOffset + 317;
    text->m_position.y = yOffset + 36;
    text->m_positionDirty = 1;
    text->m_dirty = 1;
    text = m_prompts[0];
    text->m_position.x = xOffset + 412;
    text->m_position.y = yOffset + 388;
    text->m_positionDirty = 1;
    text->m_dirty = 1;
    text = m_prompts[1];
    text->m_position.x = xOffset + 298;
    text->m_position.y = yOffset + 388;
    text->m_positionDirty = 1;
    text->m_dirty = 1;
    m_idleTime = 0;
}


// ---- src/JetskiRaceStartMenuCareer/ctor.cpp


JetskiRaceStartMenuCareer::JetskiRaceStartMenuCareer(int enabled)
    : JetskiMenu(enabled)
{
    m_textManager = 0;
    m_texts = 0;
    m_text100 = m_text104 = 0;
    m_title = 0;
    m_text10C = 0;
    m_prompts[0] = m_prompts[1] = 0;
    m_font118 = m_font = 0;
    m_font120 = 0;
}


// ---- src/JetskiRaceStartMenuCareer/dtor.cpp
// JetskiMenu as in src/JetskiRaceOverMenuCircuit/JetskiRaceOverMenuCircuit.h.
class KeyInfo;
class ProcessNode;


class ScreenIconManager;

extern "C" void free(void *p);




JetskiRaceStartMenuCareer::~JetskiRaceStartMenuCareer()
{
    for (int i = 0; i < 5; i++)
        delete m_texts[i];
    free(m_texts);
    m_texts = 0;
    delete m_title;
    m_title = 0;
    delete m_text10C;
    m_text10C = 0;
    delete m_text100;
    delete m_text104;
    m_text100 = m_text104 = 0;
    delete m_prompts[0];
    delete m_prompts[1];
    m_prompts[0] = m_prompts[1] = 0;
}


// ---- src/JetskiRaceStartMenuCareer/Create.cpp
// GameText::GetString (with its ASSERTF) is inlined inside SetString.

extern "C" void *malloc(unsigned int size);



// Colours go through a block-local copy at each call (ScreenText::SetColor
// copies again): that puts retail's colour constants in the right registers.
#define SET_COLOR(text, value)          \
    {                                   \
        unsigned int color = value;     \
        (text)->SetColor(color);        \
    }

// Local copy of JetskiRaceStartMenuCareer/JetskiRaceStartMenuCareer.h:
// +0x110/+0x114 are one two-entry array here (Create fills them in a loop).

// Retail's ScreenText::SetJustification takes its argument by const
// reference (the shared ScreenText.h takes it by value): the global is then
// loaded inside the setter, which allocates its register before the text's.

// Creates the career pre-race menu: its fonts, a text manager, five info
// rows, the title, the heading texts and the two prompt lines.
JetskiRaceStartMenuCareer *JetskiRaceStartMenuCareer::Create(ScreenIconManager *icons, ScreenTextManager *texts, ViewPortCode viewport)
{
    int i;
    JetskiMenu::Create(icons, viewport);
    BuildMenuFull(3);
    ScreenTextFontItem *item = LpGame->m_fonts->GetScreenTextFont("info_b");
    m_font = item ? item->m_font : 0;
    item = LpGame->m_fonts->GetScreenTextFont("title_b");
    m_font118 = item ? item->m_font : 0;
    item = LpGame->m_fonts->GetScreenTextFont("info_c");
    m_font120 = item ? item->m_font : 0;
    m_textManager = (new ScreenTextManager(0, 1))->Create(64);
    AddChild(m_textManager, -1);
    m_texts = (ScreenText **)malloc(5 * sizeof(ScreenText *));
    for (i = 0; i < 5; i++) {
        m_texts[i] = new ScreenText(40, 0);
        m_texts[i]->SetString("");
        SET_COLOR(m_texts[i], 0xFFFFFFFF)
        if (i == 4)
            m_texts[i]->SetPosition(320.0f, 197 + i * 30);
        else
            m_texts[i]->SetPosition(320.0f, 167 + i * 30);
    }
    m_title = new JetskiScreenText();
    m_title->m_params.m_scale = 1.125f;
    m_title->m_params.m_color = 0xFFFFFFFF;
    m_title->m_params.m_x = 320;
    m_title->m_params.m_y = 97;
    m_title->m_params.m_lineSpacing = 20;
    m_title->m_params.m_justification = ScreenTextManager::CenterHorizontalJustification;
    m_title->m_params.m_font = m_font;
    m_title->SetText(103, 0);
    m_text10C = new ScreenText(40, 0);
    m_text10C->SetScale(1.125f);
    m_text10C->SetString(LpGame->m_text->GetString(104));
    SET_COLOR(m_text10C, 0xFFFFDE00)
    m_text10C->SetPosition(320.0f, 287.0f);
    m_textManager->PositionText(m_text10C, ScreenTextManager::CenterHorizontalJustification, m_font);
    m_text100 = new ScreenText(40, 0);
    m_text100->SetScale(1.059f);
    SET_COLOR(m_text100, 0xFF000000)
    m_text100->SetPosition(320.0f, 39.0f);
    m_text100->SetString(LpGame->m_text->GetString(94));
    m_textManager->PositionText(m_text100, ScreenTextManager::CenterHorizontalJustification, m_font118);
    m_text104 = new ScreenText(40, 0);
    m_text104->SetScale(1.059f);
    SET_COLOR(m_text104, 0xFFFFFFFF)
    m_text104->SetPosition(317.0f, 36.0f);
    m_text104->SetString(LpGame->m_text->GetString(94));
    m_textManager->PositionText(m_text104, ScreenTextManager::CenterHorizontalJustification, m_font118);
    for (i = 0; i < 2; i++) {
        m_prompts[i] = new ScreenText(40, 0);
        SET_COLOR(m_prompts[i], 0xFFFFFFFF)
        m_prompts[i]->SetScale(0.857f);
        SetJustification(m_prompts[i], ScreenTextManager::LeftJustification);
        m_prompts[i]->SetFont(m_font120);
    }
    m_prompts[0]->SetString(LpGame->m_text->GetString(10));
    m_prompts[1]->SetString(LpGame->m_text->GetString(259));
    m_prompts[0]->SetPosition(412.0f, 388.0f);
    m_prompts[1]->SetPosition(298.0f, 388.0f);
    m_textManager->PositionText(m_prompts[0]);
    m_textManager->PositionText(m_prompts[1]);
    return this;
}


// ---- src/JetskiRaceStartMenuCareer/PrepareGeometry.cpp
// JetskiMenu as in src/JetskiRaceOverMenuCircuit/JetskiRaceOverMenuCircuit.h.
class KeyInfo;
class ProcessNode;



class ScreenIconManager;





// Clamp the items' minimum widths to 10, rebuild the info panel if it was
// invalidated, then let the base menu prepare its geometry.
int JetskiRaceStartMenuCareer::PrepareGeometry()
{
    if (m_font) {
        TextureMap *style = m_font->m_texture;
        if (10.0f > style->m_usage)
            style->m_usage = 10.0f;
    }
    if (m_font118) {
        TextureMap *style = m_font118->m_texture;
        if (10.0f > style->m_usage)
            style->m_usage = 10.0f;
    }
    if (m_font120) {
        TextureMap *style = m_font120->m_texture;
        if (10.0f > style->m_usage)
            style->m_usage = 10.0f;
    }
    if (m_infoDirty) {
        SetupInfo();
        m_infoDirty = 0;
    }
    return JetskiMenu::PrepareGeometry();
}


// ---- src/JetskiRaceStartMenuCareer/Render3D.cpp
// GameObject per the retail vtable (Render3D/PrepareGeometry return int here).
class KeyInfo;
class ProcessNode;


class ScreenIconManager;
class ScreenTextFont;






extern RenderTarget *LpRenderTarget;


// A per-file switch (retail has one copy in each file that uses it).

// Draw once per frame, on the last player's pass.
int JetskiRaceStartMenuCareer::Render3D()
{
    if (LpGame->m_viewport == LpGame->m_numCameras - 1) {
        if (WaitForRenderingToFinish && m_viewport) {
            LpRenderTarget->TrySendRenderQueue(1);
            LpRenderTarget->WaitForRenderingToComplete();
        }
        SetViewport();
        JetskiMenu::Render3D();
        m_textManager->AddText(m_text100, m_font118);
        m_textManager->AddText(m_text104, m_font118);
        m_title->AddText(m_textManager);
        m_textManager->AddText(m_text10C, m_font);
        for (int i = 0; i < 5; i++)
            m_textManager->AddText(m_texts[i], m_font);
        m_textManager->AddText(m_prompts[0], 0);
        m_textManager->AddText(m_prompts[1], 0);
        GameObject::Render3D();
        RestoreViewport();
    }
    return 1;
}


// ---- src/JetskiRaceStartMenuCareer/Tick.cpp
// JetskiMenu as in src/JetskiRaceOverMenuCircuit/JetskiRaceOverMenuCircuit.h.
class KeyInfo;
class ProcessNode;



class ScreenIconManager;


struct MenuSlot {
    int m_x0;
    int m_busy;                         // +0x4
};


class RaceStatusClass;
struct RaceStatusStruct_Tag;






int JetskiRaceStartMenuCareer::Tick(float dt)
{
    if (m_closing) {
        int busy = 0;
        for (int i = 0; i < 7; i++) {
            if (m_timers[i].m_active)
                busy = 1;
        }
        if (!busy) {
            switch (m_closeAction) {
            case 1:
                DisableMenu();
                break;
            case 2: {
                DisableMenu();
                JetskiCareerCompetition *competition = (JetskiCareerCompetition *)LpGame->m_competition;
                competition->m_qualified = 1;
                competition->m_qualifyPlace = 5;
                competition->EventComplete();
                break;
            }
            }
            m_closing = 0;
            m_closeAction = 0;
        }
    }
    JetskiMenu::Tick(dt);
    return 1;
}


// ---- src/JetskiRaceStartMenuCareer/KeyDownHandler.cpp
class ProcessNode;
class KeyInfo;

int KEYDOWN_HANDLER_TEST(AssignableActionIndex action, DeviceType device, ProcessNode *node, unsigned int flags);




int JetskiRaceStartMenuCareer::KeyDownHandler(ProcessNode *node, KeyInfo *key)
{
    if (KEYDOWN_HANDLER_TEST((AssignableActionIndex)4, (DeviceType)2, node, 0x80000000)) {
        m_closing = 1;
        m_closeAction = 1;
        return 1;
    } else if (KEYDOWN_HANDLER_TEST((AssignableActionIndex)5, (DeviceType)2, node, 0x80000000)) {
        m_closing = 1;
        m_closeAction = 2;
        return 1;
    } else {
        return JetskiMenu::KeyDownHandler(node, key);
    }
}


// ---- src/JetskiRaceStartMenuCareer/EnableMenu.cpp
extern "C" int sprintf(char *buf, const char *fmt, ...);
extern "C" char *strncpy(char *dst, const char *src, unsigned int n);
extern "C" char *strcpy(char *dst, const char *src);

class KeyInfo;
class ProcessNode;
class ScreenIconManager;
class ScreenTextFont;












// Shows the five best times for the current course.
void JetskiRaceStartMenuCareer::EnableMenu()
{
    if (((JetskiCareerCompetition *)LpGame->m_competition)->m_raceType == 1) {
    EnableAll();
    JetskiCareer *records = LpGame->m_gameOptions->m_career;
    for (int i = 0; i < 5; i++) {
        char line[0x100];
        char timeText[0x10];
        float time = records->m_events[records->m_currentCircuit].m_qualifyTimes[i];
        if (time == 0.0f || time == 3.40282347e+38f) {
            strcpy(timeText, "--:--.--");
        } else {
            int minutes;
            if (time < 0.0f)
                time = -1.0f * time;
            if (!(time < 5999.99f)) {
                time = 59.99f;
                minutes = 99;
            } else {
                minutes = 0;
                while (!(time < 60.0f)) {
                    time -= 60.0f;
                    minutes++;
                }
            }
            sprintf(timeText, "%2d:%05.2f", minutes, time);
        }
        if (i == 4)
            m_texts[i]->SetColor(0xFFFFDE00);
        sprintf(line, "%d. %s", i + 1, timeText);
        m_texts[i]->SetString(line);
        m_texts[i]->SetScale(1.2860000133514404f);
        m_textManager->PositionText(m_texts[i], ScreenTextManager::CenterHorizontalJustification, m_font);
    }
    LpGame->m_auralScape->StopAllAuralScapeSounds();
    LpGame->m_auralScape->Disable();
    } else {
        DisableMenu();
    }
}


// ---- src/JetskiRaceStartMenuCareer/DisableMenu.cpp




void JetskiRaceStartMenuCareer::DisableMenu()
{
    LpGame->m_competition->m_status = 3;
    LpGame->m_competition->Pause(0);
    LpGame->m_auralScape->Enable();
    LpGame->m_competition->ResetAllEngineSounds();
    DisableAll();
}


// ---- src/JetskiRaceStartMenuCareer/SetupInfo.cpp



void JetskiRaceStartMenuCareer::SetupInfo()
{
    JetskiMenu::SetupInfo();
}


// ---- src/JetskiRaceOverMenuChallenge/ctor.cpp

JetskiRaceOverMenuChallenge::JetskiRaceOverMenuChallenge(int id) : JetskiMenu(id)
{
    m_textManager = 0;
    m_texts = 0;
    m_headers = 0;
    m_idleTime = 0;
    m_attractDelay = 5.0f;
    m_font = 0;
    m_titleFont = 0;
    m_smallFont = 0;
}


// ---- src/JetskiRaceOverMenuChallenge/dtor.cpp
extern "C" void free(void *p);





JetskiRaceOverMenuChallenge::~JetskiRaceOverMenuChallenge()
{
    int i;
    for (i = 0; i < 17; i++)
        delete m_texts[i];
    free(m_texts);
    m_texts = 0;
    for (i = 0; i < 3; i++)
        delete m_headers[i];
    free(m_headers);
    m_headers = 0;
}


// ---- src/JetskiRaceOverMenuChallenge/Create.cpp
// GameText::GetString (with its ASSERTF) is inlined inside SetString.

extern "C" char *strncpy(char *dst, const char *src, unsigned int n);
extern "C" void *malloc(unsigned int size);

class ScreenTextFont;









class ScreenIconManager;


// Colours go through a block-local copy at each call (and SetColor copies
// again, as in the shared ScreenText.h): that is what puts retail's colour
// constants in the right registers.
#define SET_COLOR(text, value)          \
    {                                   \
        unsigned int color = value;     \
        (text)->SetColor(color);        \
    }



// Creates the challenge results menu's fonts and texts: the title, a column
// of six result lines, an 4x2 table, two prompts and three column headers.
JetskiRaceOverMenuChallenge *JetskiRaceOverMenuChallenge::Create(ScreenIconManager *icons, ScreenTextManager *texts,
                                                                 ViewPortCode viewport)
{
    int i;
    JetskiMenu::Create(icons, viewport);
    BuildMenuFull(4);
    m_textManager = (new ScreenTextManager(0, 1))->Create(64);
    AddChild(m_textManager, -1);
    ScreenTextFontItem *item = LpGame->m_fonts->GetScreenTextFont("info_b");
    m_font = item ? item->m_font : 0;
    item = LpGame->m_fonts->GetScreenTextFont("title_b");
    m_titleFont = item ? item->m_font : 0;
    item = LpGame->m_fonts->GetScreenTextFont("info_c");
    m_smallFont = item ? item->m_font : 0;
    m_texts = (ScreenText **)malloc(17 * sizeof(ScreenText *));
    for (i = 0; i < 17; i++) {
        m_texts[i] = new ScreenText(40, 0);
        switch (i) {
        case 0:
            m_texts[i]->SetString(LpGame->m_text->GetString(122));
            m_texts[i]->SetScale(1.059f);
            SET_COLOR(m_texts[i], 0xFFFFFFFF)
            m_texts[i]->SetPosition(320.0f, 39.0f);
            break;
        case 1:
            SET_COLOR(m_texts[i], 0xFFFFFFFF)
            m_texts[i]->SetPosition(320.0f, 250.0f);
            break;
        case 2:
            SET_COLOR(m_texts[i], 0xFFFFFFFF)
            m_texts[i]->SetPosition(320.0f, 270.0f);
            break;
        case 3:
            SET_COLOR(m_texts[i], 0xFFFFFFFF)
            m_texts[i]->SetPosition(320.0f, 290.0f);
            break;
        case 4:
            SET_COLOR(m_texts[i], 0xFFFFFFFF)
            m_texts[i]->SetPosition(320.0f, 310.0f);
            break;
        case 5:
            SET_COLOR(m_texts[i], 0xFFFFFFFF)
            m_texts[i]->SetPosition(320.0f, 330.0f);
            break;
        case 6:
            SET_COLOR(m_texts[i], 0xFFFFFFFF)
            m_texts[i]->SetPosition(320.0f, 350.0f);
            break;
        case 7:
            SET_COLOR(m_texts[i], 0xFFFFFFFF)
            m_texts[i]->SetPosition(98.0f, 140.0f);
            break;
        case 8:
            SET_COLOR(m_texts[i], 0xFFFFFFFF)
            m_texts[i]->SetPosition(310.0f, 140.0f);
            break;
        case 9:
            SET_COLOR(m_texts[i], 0xFFFFFFFF)
            m_texts[i]->SetPosition(408.0f, 140.0f);
            break;
        case 10:
            SET_COLOR(m_texts[i], 0xFFFFFFFF)
            m_texts[i]->SetPosition(508.0f, 140.0f);
            break;
        case 11:
            SET_COLOR(m_texts[i], 0xFFFFFFFF)
            m_texts[i]->SetPosition(98.0f, 170.0f);
            break;
        case 12:
            SET_COLOR(m_texts[i], 0xFFFFFFFF)
            m_texts[i]->SetPosition(310.0f, 170.0f);
            break;
        case 13:
            SET_COLOR(m_texts[i], 0xFFFFFFFF)
            m_texts[i]->SetPosition(408.0f, 170.0f);
            break;
        case 14:
            SET_COLOR(m_texts[i], 0xFFFFFFFF)
            m_texts[i]->SetPosition(508.0f, 170.0f);
            break;
        case 15:
            m_texts[i]->SetString(LpGame->m_text->GetString(126));
            SET_COLOR(m_texts[i], 0xFFFFFFFF)
            m_texts[i]->SetPosition(298.0f, 388.0f);
            break;
        case 16:
            m_texts[i]->SetString(LpGame->m_text->GetString(128));
            SET_COLOR(m_texts[i], 0xFFFFFFFF)
            m_texts[i]->SetPosition(412.0f, 388.0f);
            break;
        }
    }
    m_headers = (JetskiScreenText **)malloc(3 * sizeof(JetskiScreenText *));
    for (i = 0; i < 3; i++) {
        m_headers[i] = new JetskiScreenText();
        m_headers[i]->m_params.m_color = -1;
        m_headers[i]->m_params.m_font = m_font;
        m_headers[i]->m_params.m_justification = ScreenTextManager::CenterHorizontalJustification;
        m_headers[i]->m_params.m_lineSpacing = 20;
        switch (i) {
        case 0:
            m_headers[i]->m_params.m_x = 310;
            m_headers[i]->m_params.m_y = 90;
            m_headers[i]->SetText(89, 0);
            break;
        case 1:
            m_headers[i]->m_params.m_x = 408;
            m_headers[i]->m_params.m_y = 90;
            m_headers[i]->SetText(90, 0);
            break;
        case 2:
            m_headers[i]->m_params.m_x = 508;
            m_headers[i]->m_params.m_y = 90;
            m_headers[i]->SetText(72, 0);
            break;
        }
    }
    return this;
}


// ---- src/JetskiRaceOverMenuChallenge/PrepareGeometry.cpp

class ScreenIconManager;





// Keep the label at least 10x scale, and refresh the info when asked.
int JetskiRaceOverMenuChallenge::PrepareGeometry()
{
    TextureMap *image;
    if (m_font) {
        image = m_font->m_texture;
        if (10.0f > image->m_usage)
            image->m_usage = 10.0f;
    }
    if (m_infoDirty) {
        SetupInfo();
        m_infoDirty = 0;
    }
    return JetskiMenu::PrepareGeometry();
}


// ---- src/JetskiRaceOverMenuChallenge/Render3D.cpp

extern RenderTarget *LpRenderTarget;


// A per-file switch (retail has one copy in each file that uses it).


// Draw once per frame, on the last player's pass.
int JetskiRaceOverMenuChallenge::Render3D()
{
    if (LpGame->m_viewport == LpGame->m_numCameras - 1) {
        if (WaitForRenderingToFinish && m_viewport) {
            LpRenderTarget->TrySendRenderQueue(1);
            LpRenderTarget->WaitForRenderingToComplete();
        }
        SetViewport();
        JetskiMenu::Render3D();
        int i;
        for (i = 0; i < 3; i++)
            m_headers[i]->AddText(m_textManager);
        for (i = 0; i < 17; i++) {
            switch (i) {
            case 0:
                m_textManager->AddText(m_texts[i], m_titleFont);
                break;
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
                m_textManager->AddText(m_texts[i], m_font);
                break;
            case 15:
                m_textManager->AddText(m_texts[i], m_smallFont);
                break;
            case 16:
                m_textManager->AddText(m_texts[i], m_smallFont);
                break;
            }
        }
        GameObject::Render3D();
        RestoreViewport();
    }
    return 1;
}


// ---- src/JetskiRaceOverMenuChallenge/Tick.cpp
class KeyInfo;
class ProcessNode;







class ScreenIconManager;




int JetskiRaceOverMenuChallenge::Tick(float dt)
{
    int busy = 0;
    if (m_closing) {
        for (int i = 0; i < 7; i++) {
            if (m_timers[i].m_active)
                busy = 1;
        }
        if (!busy) {
            switch (m_closeAction) {
            case 1:
                DisableMenu();
                break;
            case 2:
                DisableAll();
                LpGame->m_auralScape->Enable();
                LpGame->m_competition->ResetAllEngineSounds();
                LpGame->m_competition->Pause(0);
                LpGame->m_competition->RestartRace((GamePlayStatus)3);
                break;
            }
            m_closing = 0;
            m_closeAction = 0;
        }
    }
    JetskiMenu::Tick(dt);
    if (!m_viewport && LpGame->m_competition && !m_closing && LpGame->m_competition->m_vcr
        && LpGame->m_controls->m_joysticks[0]->IsControllerPluggedIn()) {
        m_idleTime += dt;
        if (!(m_idleTime < m_attractDelay)) {
            PreparePlayback((PlaybackType)1);
            m_idleTime = 0;
        }
    }
    return 1;
}



// ---- src/JetskiRaceOverMenuChallenge/KeyDownHandler.cpp
class ProcessNode;
class KeyInfo;

int KEYDOWN_HANDLER_TEST(AssignableActionIndex action, DeviceType device, ProcessNode *node, unsigned int mods);



int JetskiRaceOverMenuChallenge::KeyDownHandler(ProcessNode *node, KeyInfo *key)
{
    if (KEYDOWN_HANDLER_TEST((AssignableActionIndex)4, (DeviceType)2, node, 0x80000000)) {
        m_closing = 1;
        m_closeAction = 1;
        return 1;
    } else if (KEYDOWN_HANDLER_TEST((AssignableActionIndex)5, (DeviceType)2, node, 0x80000000)) {
        m_closing = 1;
        m_closeAction = 2;
        return 1;
    }
    return JetskiMenu::KeyDownHandler(node, key);
}


// ---- src/JetskiRaceOverMenuChallenge/EnableMenu.cpp
// Compiler session state: retail
// compiled this file after a two-argument runtime helper call; this unused
// function recreates that state. It is not part of the retail file.

// GameText::GetString (with its ASSERTF) is inlined inside SetString.

extern "C" char *strncpy(char *dst, const char *src, unsigned int n);
extern "C" int sprintf(char *buf, const char *fmt, ...);
extern "C" char *strcpy(char *dst, const char *src);

class ScreenTextFont;









class ScreenIconManager;


// Colours go through a block-local copy at each call (and SetColor copies
// again, as in the shared ScreenText.h): that is what puts retail's colour
// constants in the right registers.
#define SET_COLOR(text, value)          \
    {                                   \
        unsigned int color = value;     \
        (text)->SetColor(color);        \
    }




// A race time as m:ss.ss; "--:--.--" for none (0 or FLT_MAX).
static inline void FormatTime4(char *buf, float t)
{
    if (t == 0.0f || t == 3.40282347e38f) {
        strcpy(buf, "--:--.--");
    } else {
        int minutes;
        if (t < 0.0f)
            t = -1.0f * t;
        if (t >= 5999.99f) {
            t = 59.99f;
            minutes = 99;
        } else {
            minutes = 0;
            while (t >= 60.0f) {
                t -= 60.0f;
                minutes++;
            }
        }
        sprintf(buf, "%2d:%05.2f", minutes, t);
    }
}

// Fills in the challenge results: who won, the next racer, and the two
// riders' names and times.
void JetskiRaceOverMenuChallenge::EnableMenu()
{
    char buf[0x100];
    char name[0x40];
    JetskiChallengeCompetition *competition;
    JetskiGameOptions *options;
    JetskiCareer *career;
    RaceStatusStruct_Tag *first;
    RaceStatusStruct_Tag *status;
    int i;
    options = LpGame->m_gameOptions;
    competition = (JetskiChallengeCompetition *)LpGame->m_competition;
    career = options->m_career;
    status = competition->m_raceStatus->m_vehicles;
    first = status;
    EnableAll();
    LpGame->m_competition->EndOfRaceRecordKeeping();
    LpGame->m_auralScape->StopAllAuralScapeSounds();
    LpGame->m_auralScape->Disable();
    if (competition->m_won) {
        m_texts[1]->SetString(LpGame->m_text->GetString(116));
        options->GetCharacterName(options->m_challengerRider, name);
        if (competition->m_x3E8) {
            sprintf(buf, "%s %s", LpGame->m_text->GetString(158), name);
            m_texts[2]->SetString(buf);
            sprintf(buf, "%s %s", name, LpGame->m_text->GetString(117));
            m_texts[3]->SetString(buf);
        } else {
            m_texts[2]->SetString(name);
            m_texts[3]->SetString(LpGame->m_text->GetString(117));
        }
        if (competition->m_circuitComplete) {
            options->GetCharacterName(competition->m_nextRacer, name);
            m_texts[5]->SetString(name);
            sprintf(buf, "%s %s", LpGame->m_text->GetString(156), career->m_racers[competition->m_completedCircuit].m_name);
            m_texts[6]->SetString(buf);
        } else {
            m_texts[5]->SetString("");
            m_texts[6]->SetString("");
        }
    } else {
        m_texts[1]->SetString("");
        m_texts[2]->SetString(LpGame->m_text->GetString(118));
        m_texts[3]->SetString("");
        m_texts[5]->SetString("");
        m_texts[6]->SetString("");
    }
    {
        unsigned int yellow = 0xFFFFDE00;
        SET_COLOR(m_texts[2], yellow)
        SET_COLOR(m_texts[5], yellow)
    }
    i = 0;
    while (status) {
        switch (i) {
        case 0:
            if (competition->m_vehicles[status->m_id]->m_player >= 0) {
                SET_COLOR(m_texts[7], 0xFFFFDE00)
                SET_COLOR(m_texts[8], 0xFFFFDE00)
                SET_COLOR(m_texts[9], 0xFFFFDE00)
                // The last of each four goes through a pointer copy: that is
                // what allocates its text register before the colour's.
                {
                    ScreenText *text = m_texts[10];
                    SET_COLOR(text, 0xFFFFDE00)
                }
            }
            options->GetCharacterName(options->m_racerCharacter[status->m_id], buf);
            m_texts[7]->SetString(buf);
            FormatTime4(buf, status->m_x6D4);
            m_texts[8]->SetString(buf);
            {
                float diff = status->m_x6D4 - first->m_x6D4;
                if (diff) {
                    FormatTime4(buf, diff);
                } else {
#line 2610 "JetskiRaceOverMenus.cpp"
                    STRCPY(buf, "0:00.00");
                }
            }
            m_texts[9]->SetString(buf);
            FormatTime4(buf, status->m_bestLap);
            m_texts[10]->SetString(buf);
            break;
        case 1:
            if (competition->m_vehicles[status->m_id]->m_player >= 0) {
                SET_COLOR(m_texts[11], 0xFFFFDE00)
                SET_COLOR(m_texts[12], 0xFFFFDE00)
                SET_COLOR(m_texts[13], 0xFFFFDE00)
                {
                    ScreenText *text = m_texts[14];
                    SET_COLOR(text, 0xFFFFDE00)
                }
            }
            options->GetCharacterName(options->m_racerCharacter[status->m_id], buf);
            m_texts[11]->SetString(buf);
            FormatTime4(buf, status->m_x6D4);
            m_texts[12]->SetString(buf);
            {
                float diff = status->m_x6D4 - first->m_x6D4;
                if (diff) {
                    FormatTime4(buf, diff);
                } else {
#line 2647 "JetskiRaceOverMenus.cpp"
                    STRCPY(buf, "0:00.00");
                }
            }
            m_texts[13]->SetString(buf);
            FormatTime4(buf, status->m_bestLap);
            m_texts[14]->SetString(buf);
            break;
        default:
#line 2657 "JetskiRaceOverMenus.cpp"
            ASSERT(0, "TOO MUCH RACE STATUS");
            break;
        }
        status = status->m_next;
        i++;
    }
    for (i = 0; i < 17; i++) {
        switch (i) {
        case 0:
            m_textManager->PositionText(m_texts[i], ScreenTextManager::CenterHorizontalJustification, m_titleFont);
            break;
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 8:
        case 9:
        case 10:
        case 12:
        case 13:
        case 14:
            m_textManager->PositionText(m_texts[i], ScreenTextManager::CenterHorizontalJustification, m_font);
            break;
        case 7:
        case 11:
            m_textManager->PositionText(m_texts[i], ScreenTextManager::LeftJustification, m_font);
            break;
        case 15:
        case 16:
            m_textManager->PositionText(m_texts[i], ScreenTextManager::LeftJustification, m_smallFont);
            break;
        }
    }
}


// ---- src/JetskiRaceOverMenuChallenge/DisableMenu.cpp

class ScreenIconManager;







void JetskiRaceOverMenuChallenge::DisableMenu()
{
    LpGame->m_gameOptions->m_careerMode = 0;
    LpGame->m_competition->m_nameMenu->Initialize(0);
    DisableAll();
    LpGame->m_auralScape->Enable();
    LpGame->m_competition->ResetAllEngineSounds();
}


// ---- src/JetskiRaceOverMenuChallenge/SetupInfo.cpp
// (This draft also held copies of SaveRecording__9SimpleVCRFv and REC__19JetskiVCRControllerFv; see src/SimpleVCR/, src/JetskiVCRController/.)



void JetskiRaceOverMenuChallenge::SetupInfo()
{
    JetskiMenu::SetupInfo();
    m_idleTime = 0;
}

