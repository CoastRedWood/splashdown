// JetskiTimeTrialCompetition: a time trial against the recorded ghost.
// Members start at +0x3E0 after JetskiCompetition; 0x3F0 bytes (`new` in
// JetskiGame::StartEvent; nothing is known past +0x3E4).
#ifndef JETSKITIMETRIALCOMPETITION_JETSKITIMETRIALCOMPETITION_H
#define JETSKITIMETRIALCOMPETITION_JETSKITIMETRIALCOMPETITION_H

#include "JetskiCompetition/JetskiCompetition.h"

class JetskiTimeTrialCompetition : public JetskiCompetition {
public:
    JetskiTimeTrialCompetition(int enabled);
    void SetupOverlays();

    // --- virtuals in retail slot order ---
    // Overrides keep their base slots. The TU (src/tu/) emits the vtable
    // and JetskiCompetition's @48@ thunks.
    virtual ~JetskiTimeTrialCompetition();
    virtual int InitializeRace();
    virtual JetskiTimeTrialCompetition *Create();
    virtual int Tick(float dt);
    virtual void EventComplete();
    virtual void ModifyLocalGameOptions();
    virtual void EnableOverlays();

    JetskiVehicle *m_ghostVehicle;      // +0x3E0 (InitializeRace: vehicle 0, reset to the start)
    char pad3E4[0x3F0 - 0x3E4];
};

#endif
