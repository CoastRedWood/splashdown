// JetskiCareerCompetition: a career event (a qualifier or a circuit race
// of the career path). Members start at +0x3E0 after JetskiCompetition;
// 0x470 bytes (`new` in JetskiGame::StartEvent; nothing is known past
// +0x464). Names come from the class's own methods.
#ifndef JETSKICAREERCOMPETITION_JETSKICAREERCOMPETITION_H
#define JETSKICAREERCOMPETITION_JETSKICAREERCOMPETITION_H

#include "JetskiCompetition/JetskiCompetition.h"

class JetskiCareerCompetition : public JetskiCompetition {
public:
    JetskiCareerCompetition(int id);
    void SetupOverlays();
    void SetupQualifier();
    void SetupCircuit();
    int QualifierRecordKeeping();
    void CircuitRecordKeeping();

    // --- virtuals in retail slot order ---
    // Overrides keep their base slots. The TU (src/tu/) emits the vtable
    // and JetskiCompetition's @48@ thunks.
    virtual ~JetskiCareerCompetition();
    virtual int EndOfRaceRecordKeeping();
    virtual JetskiCareerCompetition *Create();
    virtual void EventComplete();
    virtual void ModifyLocalGameOptions();
    virtual int InitializeRace();
    virtual void EnableOverlays();

    int m_circuitOver;                  // +0x3E0
    int m_playerWon;                    // +0x3E4
    int m_unlockedTrack;                // +0x3E8
    int m_unlockType;                   // +0x3EC
    int m_recordKept;                   // +0x3F0
    int m_qualified;                    // +0x3F4
    int m_circuitQualified;             // +0x3F8, enough points to go on (CircuitRecordKeeping)
    int m_x3FC;                         // +0x3FC
    int m_x400;                         // +0x400
    int m_x404;                         // +0x404
    int m_x408;                         // +0x408
    int m_raceType;                     // +0x40C (1, 2: which overlays to show)
    int m_qualifyPlace;                 // +0x410
    int m_racerIndex[5];                // +0x414
    int m_humanIndex[5];                // +0x428
    int m_oldPoints[5];                 // +0x43C
    int m_oldRank[5];                   // +0x450
    char pad464[0x470 - 0x464];
};

#endif
