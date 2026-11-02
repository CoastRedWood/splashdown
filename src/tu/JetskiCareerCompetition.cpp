// JetskiCareerCompetition.cpp  @ 0x00375590 (text 0x375590-0x376600)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's JetskiCareerCompetition.cpp as one translation unit: its functions in
// retail order, then its static initializer. `make link` places the object's data as
// well as its code (tools/link_c.py). The old files' JetskiGameOptions/JetskiCareer
// views map onto the shared headers: the career's racer records (m_racers:
// m_points/m_place, the player flag m_enabled), its events (m_qualifyTimes),
// m_trackUnlocked, m_characterUnlocked, m_movieUnlocked and m_racerCharacter.




#include "GameObject/GameObject.h"
#include "RaceStatusVehicleInterface/RaceStatusVehicleInterface.h"
#include "RaceStatusStruct_Tag/RaceStatusStruct_Tag.h"
#include "JetskiCareerCompetition/JetskiCareerCompetition.h"
#include "TextureMapManager/TextureMapManager.h"
#include "JetskiGame/JetskiGame.h"
#include "JetskiPerformanceMeter/JetskiPerformanceMeter.h"
#include "JetskiHeadingArrows/JetskiHeadingArrows.h"
#include "JetskiCountDown/JetskiCountDown.h"
#include "JetskiRadar/JetskiRadar.h"
#include "JetskiInGameStats/JetskiInGameStats.h"
#include "JetskiLoadManager/JetskiLoadManager.h"
#include "ViewPortCode/ViewPortCode.h"
#include "JetskiVehicle/JetskiVehicle.h"
#include "JetskiGameOptions/JetskiGameOptions.h"
#include "JetskiCareer/JetskiCareer.h"
#include "JetskiPageSettings/JetskiPageSettings.h"
#include "JetskiPageSettingsMain/JetskiPageSettingsMain.h"
// The axis vectors every Soultree TU defines (and __sinit constructs).
#include "sinit/AxisVectors.h"

void PSXprintf(const char *fmt, ...);
extern TextureMapManager *LpTextureMapManager;
extern JetskiGame *LpGame;

// ---- src/JetskiCareerCompetition/ctor.cpp

JetskiCareerCompetition::JetskiCareerCompetition(int id)
    : JetskiCompetition(id)
{
    m_circuitOver = 0;
    m_playerWon = 0;
    m_recordKept = 0;
    m_qualified = 0;
    m_circuitQualified = 0;
    m_x3FC = 0;
    m_x400 = 0;
    m_x404 = 0;
    m_unlockedTrack = -1;
    m_unlockType = 0;
    m_x408 = 0;
    for (int i = 0; i < 5; i++) {
        m_racerIndex[i] = 0;
        m_humanIndex[i] = 0;
        m_oldPoints[i] = 0;
        m_oldRank[i] = 0;
    }
}


// ---- src/JetskiCareerCompetition/dtor.cpp

JetskiCareerCompetition::~JetskiCareerCompetition()
{
}


// ---- src/JetskiCareerCompetition/Create.cpp
JetskiCareerCompetition *JetskiCareerCompetition::Create()
{
    int ok = 1;
    if (!JetskiCompetition::Create())
        ok = 0;
    SetupQualifier();
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


// ---- src/JetskiCareerCompetition/InitializeRace.cpp
int JetskiCareerCompetition::InitializeRace()
{
    int i;
    JetskiCareer *career = LpGame->m_gameOptions->m_career;
    JetskiGameOptions *options = LpGame->m_gameOptions;

    if (m_raceType == 2) {
        if (m_x400 && m_circuitQualified) {
            if (!m_circuitOver) {
                career->m_currentCircuit--;
                switch (m_unlockType) {
                case 1:
                    options->m_trackUnlocked[1][m_unlockedTrack] = 0;
                    options->m_trackUnlocked[0][m_unlockedTrack] = 0;
                    break;
                case 2:
                    options->m_trackUnlocked[1][m_unlockedTrack] = 0;
                    break;
                case 3:
                    options->m_trackUnlocked[0][m_unlockedTrack] = 0;
                    break;
                }
            }
            if (m_x408) {
                m_x408 = 0;
                options->m_characterUnlocked[8] = 0;
                options->m_trackUnlocked[0][20] = 0;
                options->m_trackUnlocked[1][20] = 0;
            }
            for (i = 0; i < 5; i++) {
                CareerRacer *racer = &career->m_racers[i];
                racer->m_points = m_oldPoints[i];
                racer->m_place = m_oldRank[i];
            }
            m_circuitQualified = 0;
            m_circuitOver = 0;
            m_playerWon = 0;
            m_unlockedTrack = -1;
            m_unlockType = 0;
        }
        m_x400 = 1;
        for (i = 0; i < 5; i++) {
            if (i == 0)
                (&m_x334)[i] = m_qualifyPlace;
            else if (i < m_qualifyPlace)
                (&m_x334)[i] = i;
            else
                (&m_x334)[i] = i + 1;
        }
    } else {
        if (m_x404)
            m_qualified = 0;
        m_x404 = 1;
        for (i = 0; i < 5; i++)
            (&m_x334)[i] = i + 1;
    }
    if (!JetskiCompetition::InitializeRace()) {
        PSXprintf("JetskiCareerCompetition::InitializeRace FAILED!  Aborting...\n");
        return 0;
    }
    return 1;
}


// ---- src/JetskiCareerCompetition/SetupOverlays.cpp

void JetskiCareerCompetition::SetupOverlays()
{
    switch (m_raceType) {
    case 1:
        m_inGameStats->SetCompetition(this);
        EnableOverlays();
        if (m_performanceMeter)
            m_performanceMeter->SetCompetition(this);
        if (m_headingArrows)
            m_headingArrows->SetCompetition(this, (PSXCamera **)m_cameras, m_vehicles);
        m_countDown->SetCompetition(this, (PSXCamera **)m_cameras);
        if (m_radar) {
            m_radar->Enable();
            if (!m_radar->SetCompetition(this)) {
                // No radar for this course: drop it.
                m_radar->RemoveFromChain();
                if (m_radar && !m_radar->Release())
                    m_radar = 0;
            }
        }
        break;
    case 2:
        m_inGameStats->SetCompetition(this);
        EnableOverlays();
        if (m_performanceMeter)
            m_performanceMeter->SetCompetition(this);
        if (m_headingArrows)
            m_headingArrows->SetCompetition(this, (PSXCamera **)m_cameras, m_vehicles);
        m_countDown->SetCompetition(this, (PSXCamera **)m_cameras);
        if (m_radar) {
            m_radar->Enable();
            if (!m_radar->SetCompetition(this)) {
                m_radar->RemoveFromChain();
                if (m_radar && !m_radar->Release())
                    m_radar = 0;
            }
        }
        break;
    }
}


// ---- src/JetskiCareerCompetition/EnableOverlays.cpp

void JetskiCareerCompetition::EnableOverlays()
{
    switch (m_raceType) {
    case 1:
        m_inGameStats->SetTotalRaceTime(1);
        m_inGameStats->SetBestLap(1);
        m_inGameStats->SetLapTime(1);
        m_inGameStats->SetTimeBehindLeader(0);
        m_inGameStats->SetPosition(0);
        m_inGameStats->SetLap(1);
        m_inGameStats->SetWrongWayText(1);
        break;
    case 2:
        m_inGameStats->SetTotalRaceTime(1);
        m_inGameStats->SetBestLap(1);
        m_inGameStats->SetLapTime(1);
        m_inGameStats->SetTimeBehindLeader(1);
        m_inGameStats->SetPosition(1);
        m_inGameStats->SetLap(1);
        m_inGameStats->SetWrongWayText(1);
        break;
    }
    m_inGameStats->SetWings(1, 1);
}


// ---- src/JetskiCareerCompetition/EndOfRaceRecordKeeping.cpp
int JetskiCareerCompetition::EndOfRaceRecordKeeping()
{
    switch (m_raceType) {
    case 1:
        return QualifierRecordKeeping();
    case 2:
        CircuitRecordKeeping();
        break;
    }
    return 6;
}


// ---- src/JetskiCareerCompetition/EventComplete.cpp
// Movie unlocked by winning each circuit.
void JetskiCareerCompetition::EventComplete()
{
    JetskiLoadManager *loadManager;
    int i;
    int movie;
    JetskiGameOptions *options;
    JetskiCareer *career;
    options = LpGame->m_gameOptions;
    loadManager = LpGame->m_loadManager;
    career = options->m_career;

    if (m_recordKept) {
        JetskiCompetition::EventComplete();
        if (m_circuitOver) {
            options->RestoreSplitScreenMode();
            for (i = 0; i < 5; i++) {
                CareerRacer *racer = &career->m_racers[i];
                racer->m_points = m_oldPoints[i];
                racer->m_place = m_oldRank[i];
            }
            options->SetUI(1, 0x1D);
            if (m_playerWon) {
                movie = -1;
                switch (options->m_racerCharacter[0]) {
                case 0:
                    movie = 1;
                    break;
                case 4:
                    movie = 3;
                    break;
                case 2:
                    movie = 5;
                    break;
                case 3:
                    movie = 7;
                    break;
                case 1:
                    movie = 2;
                    break;
                case 5:
                    movie = 4;
                    break;
                case 6:
                    movie = 8;
                    break;
                case 7:
                    movie = 6;
                    break;
                case 8:
                    movie = 10;
                    break;
                }
                options->m_pageSettings->m_main->Reset();
                options->m_x5FD0 = 1;
                options->m_movieUnlocked[movie - 1] = 1;
                loadManager->SetStatus(7, options->m_racerCharacter[0], 7);
                return;
            }
            options->m_x5FD0 = 0x19;
            loadManager->SetStatus(3, 0, 0);
        } else if (m_circuitQualified) {
            options->m_x5FD0 = 0x1A;
            options->RestoreSplitScreenMode();
            options->SetUI(1, 0x1D);
            loadManager->SetStatus(3, 0, 0);
        } else {
            options->RestoreSplitScreenMode();
            if (options->m_careerMode) {
                options->m_x5FD0 = 0x19;
                options->SetUI(1, 0x1D);
                loadManager->SetStatus(3, 0, 0);
            } else {
                options->SetUI(1, 0x19);
                loadManager->SetStatus(3, 0, 0);
            }
        }
    } else if (m_qualified && !m_x3FC) {
        SetupCircuit();
    } else {
        JetskiCompetition::EventComplete();
        options->RestoreSplitScreenMode();
        if (options->m_careerMode) {
            options->m_x5FD0 = 0x19;
            options->SetUI(1, 0x1D);
            loadManager->SetStatus(3, 0, 0);
        } else {
            options->SetUI(1, 0x19);
            loadManager->SetStatus(3, 0, 0);
        }
    }
}


// ---- src/JetskiCareerCompetition/ModifyLocalGameOptions.cpp

// Humans are ordered first (from slot 1), the computer player goes in slot 0.
void JetskiCareerCompetition::ModifyLocalGameOptions()
{
    JetskiCareer *players = LpGame->m_gameOptions->m_career;
    int i;
    int human = 1;
    for (i = 0; i < 5; i++) {
        if (!players->m_racers[i].m_enabled) {
            m_humanIndex[i] = 0;
            m_racerIndex[0] = i;
        } else {
            m_humanIndex[i] = human;
            m_racerIndex[human] = i;
            human++;
        }
    }
    m_numPlayers = 1;
    LpGame->m_gameOptions->SaveCurrentSplitScreenMode(0, (ViewPortCode)0);
}


// ---- src/JetskiCareerCompetition/SetupQualifier.cpp

void JetskiCareerCompetition::SetupQualifier()
{
    m_raceType = 1;
    SetupOverlays();
    for (int i = 0; i < 5; i++) {
        if (m_vehicles[i]->m_player < 0)
            m_vehicles[i]->DisableAll();
    }
}


// ---- src/JetskiCareerCompetition/SetupCircuit.cpp

void JetskiCareerCompetition::SetupCircuit()
{
    m_raceType = 2;
    m_x3FC = 1;
    Pause(0);
    RestartRace((GamePlayStatus)3);
    SetupOverlays();
    for (int i = 0; i < 5; i++) {
        if (m_vehicles[i]->m_player < 0)
            m_vehicles[i]->EnableAll();
    }
}


// ---- src/JetskiCareerCompetition/QualifierRecordKeeping.cpp
int JetskiCareerCompetition::QualifierRecordKeeping()
{
    JetskiCareer *career;
    int i;
    RaceStatusStruct_Tag *stats;
    int place;
    career = LpGame->m_gameOptions->m_career;
    stats = m_vehicles[0]->m_ownStatus;
    place = 6;
    for (i = 0; i < 5; i++) {
        if (stats->m_bestLap <= career->m_events[career->m_currentCircuit].m_qualifyTimes[i]) {
            place = i + 1;
            i = 5;  // ends the loop (retail sets the counter rather than breaking)
        }
    }
    if (place < 6) {
        m_qualified = 1;
        m_qualifyPlace = place;
    } else {
        m_qualified = 0;
    }
    return place;
}


// ---- src/JetskiCareerCompetition/CircuitRecordKeeping.cpp
// CircuitRecordKeeping__23JetskiCareerCompetitionFv  @ 0x003761C0
// The locals are declared up front in retail's colouring order. `racer` first holds the
// player's record and is then reused for the points loop, and the ranking loop reuses
// `i`: both reuses make mwcc treat the later ranges as temporaries, which is what gives
// retail's registers.

// Awards circuit points for the race, re-ranks the racers and records which
// track the result unlocks.
void JetskiCareerCompetition::CircuitRecordKeeping()
{
    CareerRacer *racer;
    int i;
    int place;
    int best;
    JetskiCareer *career;
    int index;
    int points;
    int k;
    int rank;
    JetskiGameOptions *options;
    options = LpGame->m_gameOptions;
    racer = options->m_team;
    career = options->m_career;
    place = m_vehicles[0]->m_ownStatus->m_place;
    points = 0;
    switch (place) {
    case 1:
        points = 10;
        break;
    case 2:
        points = 8;
        break;
    case 3:
        points = 5;
        break;
    case 4:
        points = 3;
        break;
    case 5:
        points = 2;
        break;
    }
    if (racer->m_points + points >= career->m_events[career->m_currentCircuit].m_pointsToPass) {
        m_circuitQualified = 1;
        for (i = 0; i < 5; i++) {
            m_oldPoints[i] = career->m_racers[i].m_points;
            m_oldRank[i] = career->m_racers[i].m_place;
        }
        for (i = 0; i < 5; i++) {
            racer = &career->m_racers[m_racerIndex[i]];
            switch (m_vehicles[i]->m_ownStatus->m_place) {
            case 1:
                racer->m_points += 10;
                break;
            case 2:
                racer->m_points += 8;
                break;
            case 3:
                racer->m_points += 5;
                break;
            case 4:
                racer->m_points += 3;
                break;
            case 5:
                racer->m_points += 2;
                break;
            }
        }
        int taken[5];
        int order[5];
        for (i = 0; i < 5; i++)
            taken[i] = 0;
        for (i = 0; i < 5; i++) {
            best = -1;
            for (k = 0; k < 5; k++) {
                if (career->m_racers[k].m_points > best && !taken[k]) {
                    best = career->m_racers[k].m_points;
                    index = k;
                }
            }
            order[i] = index;
            taken[index] = 1;
        }
        rank = 1;
        for (i = 0; i < 5; i++) {
            career->m_racers[order[i]].m_place = rank;
            if (i < 4 && career->m_racers[order[i]].m_points > career->m_racers[order[i + 1]].m_points)
                rank++;
        }
        if (career->m_currentCircuit + 1 >= career->m_numEvents) {
            m_circuitOver = 1;
            for (i = 0; i < 5; i++) {
                CareerRacer *racer = &career->m_racers[i];
                if (racer->m_place == 1) {
                    if (!racer->m_enabled) {
                        m_playerWon = 1;
                        if (career->m_difficulty == 3) {
                            if (!options->m_characterUnlocked[8]) {
                                m_x408 = 1;
                                options->m_characterUnlocked[8] = 1;
                                options->m_trackUnlocked[0][20] = 1;
                                options->m_trackUnlocked[1][20] = 1;
                            }
                        }
                        i = 5;
                    }
                }
            }
        } else {
            career->m_currentCircuit = career->m_currentCircuit + 1;
            int track = career->m_events[career->m_currentCircuit].m_scene - 1;
            m_unlockedTrack = -1;
            m_unlockType = 0;
            if (career->m_difficulty == 3) {
                if (!options->m_trackUnlocked[1][track]) {
                    m_unlockType = 1;
                    m_unlockedTrack = track + 1;
                    options->m_trackUnlocked[1][track] = 1;
                    if (!options->m_trackUnlocked[0][track])
                        options->m_trackUnlocked[0][track] = 1;
                    else
                        m_unlockType = 2;
                }
            } else if (!options->m_trackUnlocked[0][track]) {
                m_unlockType = 3;
                m_unlockedTrack = track + 1;
                options->m_trackUnlocked[0][track] = 1;
            }
        }
    } else {
        m_circuitQualified = 0;
    }
    m_recordKept = 1;
}


// ---- src/sinit/JetskiCareerCompetition.cpp
// Static initializer of JetskiCareerCompetition.cpp: only the axis vectors from the shared header.

