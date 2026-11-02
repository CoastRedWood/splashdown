// JetskiChallengeCompetition: the challenge mode (the player's team races
// the other team's riders circuit by circuit). Members start at +0x3E0
// after JetskiCompetition; 0x430 bytes (`new` in JetskiGame::StartEvent;
// nothing is known past +0x424). Names come from the class's own methods.
#ifndef JETSKICHALLENGECOMPETITION_JETSKICHALLENGECOMPETITION_H
#define JETSKICHALLENGECOMPETITION_JETSKICHALLENGECOMPETITION_H

#include "JetskiCompetition/JetskiCompetition.h"

class JetskiChallengeCompetition : public JetskiCompetition {
public:
    JetskiChallengeCompetition(int id);
    void SetupOverlays();

    // --- virtuals in retail slot order ---
    // The dtor is the key function: dtor.cpp emits the vtable and
    // JetskiCompetition's @48@ thunks, as retail has them there.
    virtual ~JetskiChallengeCompetition();
    virtual int EndOfRaceRecordKeeping();
    virtual JetskiChallengeCompetition *Create();
    virtual void EventComplete();
    virtual void ModifyLocalGameOptions();
    virtual int InitializeRace();
    virtual void EnableOverlays();

    // The circuits are JetskiGameOptions::m_career's five racers (teams):
    // each one's m_choices are the riders still to beat on that circuit.
    int m_won;                          // +0x3E0 (InitializeRace: switch team)
    int m_circuitComplete;              // +0x3E4
    int m_x3E8;                         // +0x3E8, the win was a first
    int m_circuitOf[9];                 // +0x3EC, per racer
    int m_nextRacer;                    // +0x410
    int m_completedCircuit;             // +0x414
    int m_beatenIndex;                  // +0x418, where the beaten racer was (InitializeRace re-inserts the player there)
    int m_nextCircuit;                  // +0x41C, the circuit the next racer came from
    int m_foundNext;                    // +0x420
    char pad424[0x430 - 0x424];
};

#endif
