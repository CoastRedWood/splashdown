// JetskiCopycatCompetition.cpp  @ 0x00252C00 (text 0x252C00-0x252EF0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's JetskiCopycatCompetition.cpp as one translation unit: its functions in
// retail order, then its static initializer. `make link` places the object's data as
// well as its code (tools/link_c.py).



#include "GameObject/GameObject.h"
#include "RaceStatusEventInterface/RaceStatusEventInterface.h"
#include "JetskiCompetition/JetskiCompetition.h"
#include "JetskiCopycatCompetition/JetskiCopycatCompetition.h"
#include "TextureMapManager/TextureMapManager.h"
#include "JetskiGame/JetskiGame.h"
#include "JetskiPerformanceMeter/JetskiPerformanceMeter.h"
#include "JetskiInGameStats/JetskiInGameStats.h"
#include "JetskiGameOptions/JetskiGameOptions.h"
#include "JetskiLoadManager/JetskiLoadManager.h"
#include "sinit/AxisVectors.h"

void PSXprintf(const char *fmt, ...);
extern TextureMapManager *LpTextureMapManager;
extern JetskiGame *LpGame;

// ---- src/JetskiCopycatCompetition/ctor.cpp

JetskiCopycatCompetition::JetskiCopycatCompetition(int id) : JetskiCompetition(id)
{
}


// ---- src/JetskiCopycatCompetition/dtor.cpp

JetskiCopycatCompetition::~JetskiCopycatCompetition()
{
}


// ---- src/JetskiCopycatCompetition/Create.cpp

JetskiCopycatCompetition *JetskiCopycatCompetition::Create()
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
        PSXprintf("SUCCESSFUL CREATION OF COPYCAT RACE\n");
        return this;
    }
    PSXprintf("FAILED CREATION OF COPYCAT RACE\n");
    return 0;
}


// ---- src/JetskiCopycatCompetition/InitializeRace.cpp

int JetskiCopycatCompetition::InitializeRace()
{
    if (!JetskiCompetition::InitializeRace()) {
        PSXprintf("JetskiCopycatCompetition::InitializeRace FAILED!  Aborting...\n");
        return 0;
    }
    SetupOverlays();
    return 1;
}


// ---- src/JetskiCopycatCompetition/SetupOverlays.cpp

void JetskiCopycatCompetition::SetupOverlays()
{
    m_inGameStats->SetCompetition(this);
    EnableOverlays();
    if (m_performanceMeter) {
        m_performanceMeter->SetCompetition(this);
        m_performanceMeter->SetPerformance(0);
    }
}


// ---- src/JetskiCopycatCompetition/EnableOverlays.cpp

void JetskiCopycatCompetition::EnableOverlays()
{
    m_inGameStats->SetCountdownText(1);
    m_inGameStats->SetScoreText(1);
}


// ---- src/JetskiCopycatCompetition/EventComplete.cpp

void JetskiCopycatCompetition::EventComplete()
{
    JetskiCompetition::EventComplete();
    LpGame->m_gameOptions->RestoreSplitScreenMode();
    LpGame->m_gameOptions->SetUI(1, 3);
    LpGame->m_loadManager->SetStatus(3, 0, 0);
}


// ---- src/JetskiCopycatCompetition/ModifyLocalGameOptions.cpp
void JetskiCopycatCompetition::ModifyLocalGameOptions()
{
    m_numPlayers = 1;
    LpGame->m_gameOptions->SaveCurrentSplitScreenMode(0, (ViewPortCode)0);
}


// ---- src/sinit/JetskiCopycatCompetition.cpp
// Static initializer of JetskiCopycatCompetition.cpp: only the axis vectors from the shared header.

