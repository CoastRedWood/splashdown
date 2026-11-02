// JetskiChallengeCompetition.cpp  @ 0x00376600 (text 0x376600-0x376D80)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's JetskiChallengeCompetition.cpp as one translation unit: its functions in
// retail order, then its static initializer. `make link` places the object's data as
// well as its code (tools/link_c.py). The old files' 15 local types were views of
// JetskiGameOptions: the "teams"/"circuits"/"challenge events" at +0x5FBC are
// m_career's five CareerRacers (their m_choices are a circuit's riders), the "last
// roster" at +0x5FB4 is m_team, +0x4A50 is m_challengerRider and +0x104
// m_characterUnlocked.




#include "GameObject/GameObject.h"
#include "JetskiChallengeCompetition/JetskiChallengeCompetition.h"
#include "TextureMapManager/TextureMapManager.h"
#include "Game/Game.h"
#include "JetskiPerformanceMeter/JetskiPerformanceMeter.h"
#include "JetskiHeadingArrows/JetskiHeadingArrows.h"
#include "JetskiCountDown/JetskiCountDown.h"
#include "JetskiRadar/JetskiRadar.h"
#include "JetskiInGameStats/JetskiInGameStats.h"
#include "JetskiVehicle/JetskiVehicle.h"
#include "RaceStatusStruct_Tag/RaceStatusStruct_Tag.h"
#include "JetskiGame/JetskiGame.h"
#include "JetskiGameOptions/JetskiGameOptions.h"
#include "JetskiCareer/JetskiCareer.h"
#include "JetskiLoadManager/JetskiLoadManager.h"
#include "sinit/AxisVectors.h"

void PSXprintf(const char *fmt, ...);
extern TextureMapManager *LpTextureMapManager;
extern JetskiGame *LpGame;

// ---- src/JetskiChallengeCompetition/ctor.cpp

JetskiChallengeCompetition::JetskiChallengeCompetition(int id) : JetskiCompetition(id)
{
    m_won = 0;
    m_x3E8 = 0;
    m_beatenIndex = -1;
    m_nextCircuit = -1;
    m_foundNext = 0;
}


// ---- src/JetskiChallengeCompetition/dtor.cpp


JetskiChallengeCompetition::~JetskiChallengeCompetition()
{
}


// ---- src/JetskiChallengeCompetition/Create.cpp

JetskiChallengeCompetition *JetskiChallengeCompetition::Create()
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


// ---- src/JetskiChallengeCompetition/InitializeRace.cpp
int JetskiChallengeCompetition::InitializeRace()
{
    JetskiGameOptions *settings;
    JetskiCareer *teams;
    int player;
    CareerRacer *last;
    int team;
    settings = LpGame->m_gameOptions;
    player = settings->m_challengerRider;
    teams = settings->m_career;
    last = settings->m_team;
    team = m_circuitOf[player];
    // m_x334..m_x344: five ints JetskiCompetition.h declares one by one.
    (&m_x334)[0] = 2;
    (&m_x334)[1] = 1;
    (&m_x334)[2] = 3;
    (&m_x334)[3] = 4;
    (&m_x334)[4] = 5;
    CareerRacer *roster = &teams->m_racers[team];
    if (m_won) {
        m_won = 0;
        if (m_x3E8) {
            m_x3E8 = 0;
            settings->m_characterUnlocked[player] = 0;
        }
        last->m_numChoices--;
        last->m_choices[last->m_numChoices] = 0;
        roster->m_numChoices++;
        for (int j = roster->m_numChoices - 1; j > -1; j--) {
            if (j != m_beatenIndex)
                roster->m_choices[j] = roster->m_choices[j - 1];
            else
                roster->m_choices[j] = player;
        }
        m_beatenIndex = -1;
        if (m_circuitComplete) {
            m_circuitComplete = 0;
            if (m_foundNext) {
                CareerRacer *other = &teams->m_racers[m_nextCircuit];
                other->m_choices[other->m_numChoices] = roster->m_choices[1];
                other->m_numChoices++;
            }
            roster->m_choices[1] = 0;
            roster->m_numChoices = 1;
        }
    }
    if (!JetskiCompetition::InitializeRace()) {
        PSXprintf("JetskiCareerCompetition::InitializeRace FAILED!  Aborting...\n");
        return 0;
    }
    SetupOverlays();
    return 1;
}


// ---- src/JetskiChallengeCompetition/SetupOverlays.cpp

void JetskiChallengeCompetition::SetupOverlays()
{
    m_inGameStats->SetCompetition(this);
    EnableOverlays();
    if (m_performanceMeter)
        m_performanceMeter->SetCompetition(this);
    if (m_headingArrows)
        m_headingArrows->SetCompetition(this, (PSXCamera **)m_cameras, m_vehicles);
    m_countDown->SetCompetition(this, (PSXCamera **)m_cameras);
    if (m_radar) {
        m_radar->Enable();
        if (m_radar->SetCompetition(this))
            return;
        // No radar for this course: drop it.
        m_radar->RemoveFromChain();
        if (m_radar && !m_radar->Release())
            m_radar = 0;
    }
}


// ---- src/JetskiChallengeCompetition/EnableOverlays.cpp


void JetskiChallengeCompetition::EnableOverlays()
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


// ---- src/JetskiChallengeCompetition/EndOfRaceRecordKeeping.cpp
int JetskiChallengeCompetition::EndOfRaceRecordKeeping()
{
    JetskiGameOptions *options = LpGame->m_gameOptions;
    JetskiCareer *circuits = options->m_career;
    CareerRacer *beaten = options->m_team;
    int racer = options->m_challengerRider;
    int circuit = m_circuitOf[racer];
    m_won = 0;
    m_circuitComplete = 0;
    CareerRacer *remaining = &circuits->m_racers[circuit];
    if (m_vehicles[0]->m_ownStatus->m_place == 1) {
        m_won = 1;
        if (!options->m_characterUnlocked[racer]) {
            m_x3E8 = 1;
            options->m_characterUnlocked[racer] = 1;
        }
        beaten->m_choices[beaten->m_numChoices] = racer;
        beaten->m_numChoices++;
        int k = 0;
        for (int i = 0; i < remaining->m_numChoices; i++) {
            if (remaining->m_choices[i] != racer) {
                remaining->m_choices[k] = remaining->m_choices[i];
                k++;
            } else {
                m_beatenIndex = i;
            }
        }
        remaining->m_numChoices--;
        if (remaining->m_numChoices == 0) {
            m_circuitComplete = 1;
            m_completedCircuit = circuit;
            m_foundNext = 0;
            CareerRacer *other;
            for (int c = 0; c < 5; c++) {
                other = &circuits->m_racers[c];
                if (other->m_enabled) {
                    if (other->m_numChoices > 1) {
                        m_nextCircuit = c;
                        m_foundNext = 1;
                        other->m_numChoices--;
                        m_nextRacer = other->m_choices[other->m_numChoices];
                        other->m_choices[other->m_numChoices] = 0;
                        remaining->m_choices[0] = m_nextRacer;
                        remaining->m_numChoices = 1;
                        c = 5;
                    }
                }
            }
            if (!m_foundNext) {
                m_nextRacer = 8;
                remaining->m_choices[0] = 8;
                remaining->m_numChoices = 1;
            }
        }
    }
    return 1;
}


// ---- src/JetskiChallengeCompetition/EventComplete.cpp

void JetskiChallengeCompetition::EventComplete()
{
    JetskiGameOptions *options = LpGame->m_gameOptions;
    JetskiCareer *career = options->m_career;
    JetskiCompetition::EventComplete();
    options->RestoreSplitScreenMode();
    if (m_won || options->m_careerMode) {
        options->m_x5FD0 = 0x19;
        options->SetUI(1, 0x1D);
        LpGame->m_loadManager->SetStatus(3, 0, 0);
    } else {
        options->SetUI(1, 0x19);
        LpGame->m_loadManager->SetStatus(3, 0, 0);
    }
    career->m_currentCircuit++;
}


// ---- src/JetskiChallengeCompetition/ModifyLocalGameOptions.cpp
void JetskiChallengeCompetition::ModifyLocalGameOptions()
{
    JetskiCareer *events = LpGame->m_gameOptions->m_career;
    CareerRacer *tracks;
    int i;
    int j;
    for (i = 0; i < 5; i++) {
        tracks = &events->m_racers[i];
        for (j = 0; j < tracks->m_numChoices; j++)
            m_circuitOf[tracks->m_choices[j]] = i;
    }
    m_numPlayers = 1;
    LpGame->m_gameOptions->SaveCurrentSplitScreenMode(0, (ViewPortCode)0);
}


// ---- src/sinit/JetskiChallengeCompetition.cpp
// Static initializer of JetskiChallengeCompetition.cpp: only the axis vectors from the shared header.

