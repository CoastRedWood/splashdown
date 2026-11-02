// JetskiCircuitCompetition: a single-player circuit race. Its members
// start at +0x3E0 after JetskiCompetition but none is known yet; 0x400
// bytes (`new` in JetskiGame::StartEvent).
#ifndef JETSKICIRCUITCOMPETITION_JETSKICIRCUITCOMPETITION_H
#define JETSKICIRCUITCOMPETITION_JETSKICIRCUITCOMPETITION_H

#include "JetskiCompetition/JetskiCompetition.h"

class JetskiCircuitCompetition : public JetskiCompetition {
public:
    JetskiCircuitCompetition(int id);
    void SetupOverlays();

    // --- virtuals: overrides keep their base slots ---
    // The whole TU (src/tu/JetskiCircuitCompetition.cpp) emits the vtable,
    // as retail's did.
    virtual void ModifyLocalGameOptions();
    virtual ~JetskiCircuitCompetition();
    virtual JetskiCircuitCompetition *Create();
    virtual void EventComplete();
    virtual int InitializeRace();
    virtual void EnableOverlays();

    char pad3E0[0x400 - 0x3E0];
};

#endif
