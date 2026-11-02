// JetskiCircuitCompetition.cpp  @ 0x00244810 (text 0x244810-0x244C60)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's JetskiCircuitCompetition.cpp as one translation unit: the eight
// member functions in retail order, then __sinit_JetskiCircuitCompetition.cpp.
// Merged from src/JetskiCircuitCompetition/*.cpp and
// src/sinit/JetskiCircuitCompetition.cpp. Besides the code, the object's data
// is retail's: the string literals and RTTI records (.rodata
// 0x4124A0-0x412650), the vtable (.data 0x446A90), the axis vectors (.bss
// 0x47AC30), the initializer (.init 0x441720) and its .ctor slot (0x445D2C);
// `make link` places all of them (tools/link_c.py, TU_FILES).


class KeyInfo;
class ProcessNode;

#include "GameObject/GameObject.h"
#include "RaceStatusEventInterface/RaceStatusEventInterface.h"
#include "JetskiCompetition/JetskiCompetition.h"
#include "JetskiCircuitCompetition/JetskiCircuitCompetition.h"
#include "TextureMapManager/TextureMapManager.h"
#include "JetskiGame/JetskiGame.h"
#include "JetskiPerformanceMeter/JetskiPerformanceMeter.h"
#include "JetskiHeadingArrows/JetskiHeadingArrows.h"
#include "JetskiCountDown/JetskiCountDown.h"
#include "JetskiRadar/JetskiRadar.h"
#include "JetskiInGameStats/JetskiInGameStats.h"
#include "JetskiGameOptions/JetskiGameOptions.h"
#include "JetskiLoadManager/JetskiLoadManager.h"
// The axis vectors every Soultree TU defines (and __sinit constructs).
#include "sinit/AxisVectors.h"

void PSXprintf(const char *fmt, ...);
extern TextureMapManager *LpTextureMapManager;
extern JetskiGame *LpGame;

JetskiCircuitCompetition::JetskiCircuitCompetition(int id)
    : JetskiCompetition(id)
{
}

JetskiCircuitCompetition::~JetskiCircuitCompetition()
{
}

JetskiCircuitCompetition *JetskiCircuitCompetition::Create()
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
        PSXprintf("SUCCESSFUL CREATION OF CIRCUIT RACE\n");
        return this;
    }
    PSXprintf("FAILED CREATION OF CIRCUIT RACE\n");
    return 0;
}

int JetskiCircuitCompetition::InitializeRace()
{
    if (!JetskiCompetition::InitializeRace()) {
        PSXprintf("JetskiCircuitCompetition::InitializeRace FAILED!  Aborting...\n");
        return 0;
    }
    SetupOverlays();
    return 1;
}

void JetskiCircuitCompetition::SetupOverlays()
{
    m_inGameStats->SetCompetition(this);
    EnableOverlays();
    if (m_performanceMeter)
        m_performanceMeter->SetCompetition(this);
    if (m_headingArrows)
        m_headingArrows->SetCompetition(this, (PSXCamera **)m_cameras, m_vehicles);
    m_countDown->SetCompetition(this, (PSXCamera **)m_cameras);
    if (m_radar && !m_radar->SetCompetition(this)) {
        // No radar for this course: drop it.
        m_radar->RemoveFromChain();
        if (m_radar && !m_radar->Release())
            m_radar = 0;
    }
}

void JetskiCircuitCompetition::EnableOverlays()
{
    m_inGameStats->SetTotalRaceTime(1);
    m_inGameStats->SetBestLap(1);
    m_inGameStats->SetLapTime(1);
    m_inGameStats->SetTimeBehindLeader(1);
    m_inGameStats->SetPosition(1);
    m_inGameStats->SetLap(1);
    m_inGameStats->SetWrongWayText(1);
    m_inGameStats->SetWings(1, 1);
}

void JetskiCircuitCompetition::EventComplete()
{
    JetskiGameOptions *options = LpGame->m_gameOptions;
    JetskiCompetition::EventComplete();
    if (options->m_numHumans == 1)
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

void JetskiCircuitCompetition::ModifyLocalGameOptions()
{
    JetskiGameOptions *options = LpGame->m_gameOptions;
    if (options->m_numHumans == 1)
        options->SaveCurrentSplitScreenMode(0, (ViewPortCode)0);
    m_numPlayers = m_options->m_numHumans;
}
