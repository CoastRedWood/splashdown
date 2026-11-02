// JetskiTimeTrialCompetition.cpp  @ 0x00255DE0 (text 0x255DE0-0x2565D0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's JetskiTimeTrialCompetition.cpp as one translation unit: its functions in
// retail order, then its static initializer. `make link` places the object's data as
// well as its code (tools/link_c.py). The _D3DTLVERTEX ctor and the LensFlare setters
// after Tick in its tu_map row are LensFlare.cpp, a TU without an initializer
// (src/tu/LensFlare.cpp).



#include "GameObject/GameObject.h"
#include "RaceStatusEventInterface/RaceStatusEventInterface.h"
#include "RaceStatusVehicleInterface/RaceStatusVehicleInterface.h"
#include "RaceStatusStruct_Tag/RaceStatusStruct_Tag.h"
#include "JetskiCompetition/JetskiCompetition.h"
#include "JetskiTimeTrialCompetition/JetskiTimeTrialCompetition.h"
#include "TextureMapManager/TextureMapManager.h"
#include "Game/Game.h"
#include "JetskiInGameStats/JetskiInGameStats.h"
#include "JetskiPerformanceMeter/JetskiPerformanceMeter.h"
#include "JetskiHeadingArrows/JetskiHeadingArrows.h"
#include "JetskiCountDown/JetskiCountDown.h"
#include "JetskiRadar/JetskiRadar.h"
#include "soultree_assert_block.h"
#include "d3dtypes.h"
#include "LocalizedString/LocalizedString.h"
#include "GameText/GameText.h"
#include "JetskiGameOptions/JetskiGameOptions.h"
#include "JetskiGame/JetskiGame.h"
#include "JetskiVehicle/JetskiVehicle.h"
#include "JetskiVCRController/JetskiVCRController.h"
#include "JetskiInfoText/JetskiInfoText.h"
#include "JetskiLoadManager/JetskiLoadManager.h"
#include "Scene/Scene.h"
// The axis vectors every Soultree TU defines (and __sinit constructs).
#include "sinit/AxisVectors.h"

void PSXprintf(const char *fmt, ...);
extern "C" int sprintf(char *buf, const char *fmt, ...);
extern "C" char *strcpy(char *dst, const char *src);
extern "C" char *strcat(char *dst, const char *src);
extern char SceneContentDirectory[];
extern TextureMapManager *LpTextureMapManager;
extern JetskiGame *LpGame;

// ---- src/JetskiTimeTrialCompetition/ctor.cpp

JetskiTimeTrialCompetition::JetskiTimeTrialCompetition(int enabled)
    : JetskiCompetition(enabled)
{
}


// ---- src/JetskiTimeTrialCompetition/dtor.cpp

JetskiTimeTrialCompetition::~JetskiTimeTrialCompetition()
{
}


// ---- src/JetskiTimeTrialCompetition/Create.cpp

JetskiTimeTrialCompetition *JetskiTimeTrialCompetition::Create()
{
    int ok = 1;
    if (!JetskiCompetition::Create())
        ok = 0;
    if (ok && !AttachScene())
        ok = 0;
    if (LpGame->m_textureCache)
        LpTextureMapManager->SetupCache();
    if (ok)
        SetupOverlays();
    if (ok) {
        PSXprintf("SUCCESSFUL CREATION OF TIME TRIAL RACE\n");
        return this;
    }
    PSXprintf("FAILED CREATION OF TIME TRIAL RACE\n");
    return 0;
}


// ---- src/JetskiTimeTrialCompetition/SetupOverlays.cpp

void JetskiTimeTrialCompetition::SetupOverlays()
{
    m_inGameStats->SetCompetition(this);
    EnableOverlays();
    if (m_performanceMeter)
        m_performanceMeter->SetCompetition(this);
    if (m_headingArrows)
        m_headingArrows->SetCompetition(this, (PSXCamera **)m_cameras, m_vehicles);
    m_countDown->SetCompetition(this, (PSXCamera **)m_cameras);
    if (!m_radar->SetCompetition(this)) {
        m_radar->RemoveFromChain();
        if (m_radar && !m_radar->Release())
            m_radar = 0;
    }
}


// ---- src/JetskiTimeTrialCompetition/EnableOverlays.cpp


void JetskiTimeTrialCompetition::EnableOverlays()
{
    m_inGameStats->SetBestLap(1);
    m_inGameStats->SetLapTime(1);
    m_inGameStats->SetWrongWayText(1);
    m_inGameStats->SetTimeBehindLeader(1);
}


// ---- src/JetskiTimeTrialCompetition/InitializeRace.cpp
// Compiler session state: retail compiled
// this file in an mwcc session that had already generated a two-argument runtime
// helper call (64-bit / or %, double arithmetic or compare). mwcc then models
// every later helper call (__floatdisf, fptoui, fptodp, ...) as reading $a0-$a1,
// so temporaries live before such a call skip $a0/$a1. This unused function
// recreates that state; it is not part of the retail file. The whole TU
// still needs it (no earlier function in it makes such a call), so the
// state came from before the TU.
static void MWCC_SHIM_TEXT MwccSessionState(long *a) { a[0] = a[0] / a[1]; }

int JetskiTimeTrialCompetition::InitializeRace()
{
    _D3DVECTOR position = m_scene->m_startPosition;
    _D3DVECTOR direction = m_scene->m_startDirection;
    _D3DVECTOR start;
    char time[0x80];
    char text[0x80];
    char path[0x110];

    m_ghostVehicle = m_vehicles[0];
    start = position;
    m_ghostVehicle->m_x790 = 0;
    m_ghostVehicle->m_resetPosition = position;
    m_ghostVehicle->m_resetOrientation = direction;
    m_ghostVehicle->m_resetUp = _D3DVECTOR(0.0f, 1.0f, 0.0f);
    m_ghostVehicle->ResetPosition(1);
    m_bestTime = 11999.98f;
    if (!LpGame->m_gameOptions->m_x58) {
        sprintf(path, "%sghost", SceneContentDirectory);
        switch (LpGame->m_gameOptions->m_difficulty) {
        case 1:
        case 2:
            strcat(path, "N.gho");
            break;
        case 3:
            strcat(path, "H.gho");
            break;
        }
        if (!m_vcr->LoadGhostFromDisk(path, &m_bestTime))
            PSXprintf("JetskiTimeTrialCompetition::InitializeRace:  LoadFromDisk FAILED!\n");
        // The ghost is a JetskiVehicle when m_x54 is 2, else a
        // JetskiGhostVehicle (its RaceStatusVehicleInterface at +0).
        if (m_ghost) {
            if (m_options->m_x54 == 2)
                ((JetskiVehicle *)m_ghost)->m_ownStatus->m_bestLap = m_bestTime;
            else
                ((RaceStatusVehicleInterface *)m_ghost)->m_ownStatus->m_bestLap = m_bestTime;
        }
    }
    int result = JetskiCompetition::InitializeRace();
    if (!LpGame->m_gameOptions->m_x58) {
        float t = m_bestTime;
        if (t == 0.0f || t == 3.40282347e+38f) {
            strcpy(time, "--:--.--");
        } else {
            int minutes;
            if (t < 0.0f)
                t = -1.0f * t;
            if (!(t < 5999.99f)) {
                t = 59.99f;
                minutes = 99;
            } else {
                minutes = 0;
                while (!(t < 60.0f)) {
                    t -= 60.0f;
                    minutes++;
                }
            }
            sprintf(time, "%2d:%05.2f", minutes, t);
        }
        GameText *localization = LpGame->m_text;
        sprintf(text, "%s: %s", localization->GetString(69), time);
        float scale = 1.0f;
        m_infoText->AddInfoText(text, 0, 10.0f, 0, scale);
    }
    SetupOverlays();
    return result;
}


// ---- src/JetskiTimeTrialCompetition/EventComplete.cpp

void JetskiTimeTrialCompetition::EventComplete()
{
    JetskiGameOptions *options = LpGame->m_gameOptions;
    JetskiCompetition::EventComplete();
    options->RestoreSplitScreenMode();
    if (options->m_careerMode) {
        options->m_x5FD4 = 3;
        options->SetUI(1, 0x11);
        LpGame->m_loadManager->SetStatus(3, 0, 0);
    } else {
        options->SetUI(1, 3);
        LpGame->m_loadManager->SetStatus(3, 0, 0);
    }
}


// ---- src/JetskiTimeTrialCompetition/ModifyLocalGameOptions.cpp
void JetskiTimeTrialCompetition::ModifyLocalGameOptions()
{
    LpGame->m_gameOptions->SaveCurrentSplitScreenMode(0, (ViewPortCode)0);
    m_numPlayers = m_options->m_numHumans;
}


// ---- src/JetskiTimeTrialCompetition/Tick.cpp



int JetskiTimeTrialCompetition::Tick(float dt)
{
    return JetskiCompetition::Tick(dt);
}


// ---- src/sinit/JetskiTimeTrialCompetition.cpp
// Static initializer of JetskiTimeTrialCompetition.cpp: only the axis vectors from the shared header.

