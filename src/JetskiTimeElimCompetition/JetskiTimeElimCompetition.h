// JetskiTimeElimCompetition: a time-elimination race (the last racer is
// dropped each time the timer runs out). Adds no members that its methods
// use; 0x3F0 bytes (`new` in JetskiGame::StartEvent).
#ifndef JETSKITIMEELIMCOMPETITION_H
#define JETSKITIMEELIMCOMPETITION_H

#include "JetskiCompetition/JetskiCompetition.h"

class JetskiTimeElimCompetition : public JetskiCompetition {
public:
    JetskiTimeElimCompetition(int id);
    void SetupOverlays();

    // --- virtuals: overrides keep their base slots ---
    virtual ~JetskiTimeElimCompetition();
    virtual JetskiTimeElimCompetition *Create();
    virtual void EventComplete();
    virtual void ModifyLocalGameOptions();
    virtual int InitializeRace();
    virtual void EnableOverlays();

    char pad3E0[0x3F0 - 0x3E0];
};

#endif
