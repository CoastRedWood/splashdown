// JetskiTimeElimCompetition.cpp  @ 0x00255A40 (text 0x255A40-0x255DE0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's JetskiTimeElimCompetition.cpp as one translation unit: the eight
// member functions in retail order, then
// __sinit_JetskiTimeElimCompetition.cpp. Merged from
// src/JetskiTimeElimCompetition/*.cpp and src/sinit/ by hand, on the shared
// JetskiCompetition.h. `make link` places its data (tools/link_c.py).

class KeyInfo;
class ProcessNode;

#include "GameObject/GameObject.h"
#include "RaceStatusEventInterface/RaceStatusEventInterface.h"
#include "JetskiCompetition/JetskiCompetition.h"
#include "JetskiTimeElimCompetition/JetskiTimeElimCompetition.h"
#include "TextureMapManager/TextureMapManager.h"
#include "JetskiGame/JetskiGame.h"
#include "JetskiPerformanceMeter/JetskiPerformanceMeter.h"
#include "JetskiCountDown/JetskiCountDown.h"
#include "JetskiInGameStats/JetskiInGameStats.h"
#include "JetskiGameOptions/JetskiGameOptions.h"
#include "JetskiLoadManager/JetskiLoadManager.h"
// The axis vectors every Soultree TU defines (and __sinit constructs).
#include "sinit/AxisVectors.h"

void PSXprintf(const char *fmt, ...);
extern TextureMapManager *LpTextureMapManager;
extern JetskiGame *LpGame;

JetskiTimeElimCompetition::JetskiTimeElimCompetition(int id) : JetskiCompetition(id)
{
}

JetskiTimeElimCompetition::~JetskiTimeElimCompetition()
{
}

JetskiTimeElimCompetition *JetskiTimeElimCompetition::Create()
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
        PSXprintf("SUCCESSFUL CREATION OF TIME ELIMINATION RACE\n");
        return this;
    }
    PSXprintf("FAILED CREATION OF TIME ELIMINATION RACE\n");
    return 0;
}

int JetskiTimeElimCompetition::InitializeRace()
{
    if (!JetskiCompetition::InitializeRace()) {
        PSXprintf("JetskiTimeElimCompetition::InitializeRace FAILED!  Aborting...\n");
        return 0;
    }
    SetupOverlays();
    return 1;
}

void JetskiTimeElimCompetition::SetupOverlays()
{
    m_inGameStats->SetCompetition(this);
    EnableOverlays();
    if (m_performanceMeter) {
        m_performanceMeter->SetCompetition(this);
        m_performanceMeter->SetPerformance(0);
    }
    m_countDown->SetCompetition(this, (PSXCamera **)m_cameras);
}

void JetskiTimeElimCompetition::EnableOverlays()
{
    m_inGameStats->SetScoreText(1);
    m_inGameStats->SetWings(1, 0);
}

void JetskiTimeElimCompetition::EventComplete()
{
    JetskiGameOptions *options = LpGame->m_gameOptions;
    JetskiCompetition::EventComplete();
    if (options->m_numHumans == 1)
        options->RestoreSplitScreenMode();
    // Retail has the SetStatus call duplicated in both branches.
    if (options->m_careerMode) {
        options->m_x5FD4 = 3;
        options->SetUI(1, 0x11);
        LpGame->m_loadManager->SetStatus(3, 0, 0);
    } else {
        options->SetUI(1, 3);
        LpGame->m_loadManager->SetStatus(3, 0, 0);
    }
}

void JetskiTimeElimCompetition::ModifyLocalGameOptions()
{
    JetskiGameOptions *options = LpGame->m_gameOptions;
    if (options->m_numHumans == 1)
        options->SaveCurrentSplitScreenMode(0, (ViewPortCode)0);
    m_numPlayers = m_options->m_numHumans;
}
