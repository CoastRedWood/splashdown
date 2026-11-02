// JetskiPracticeCompetition: a practice run. Its members start at +0x3E0
// after JetskiCompetition but none is known yet; 0x400 bytes (`new` in
// JetskiGame::StartEvent).
#ifndef JETSKIPRACTICECOMPETITION_JETSKIPRACTICECOMPETITION_H
#define JETSKIPRACTICECOMPETITION_JETSKIPRACTICECOMPETITION_H

#include "JetskiCompetition/JetskiCompetition.h"

class JetskiPracticeCompetition : public JetskiCompetition {
public:
    JetskiPracticeCompetition(int enabled);
    void SetupOverlays();

    // --- virtuals in retail slot order ---
    // ModifyLocalGameOptions is declared first so it is the key function:
    // the vtable and JetskiCompetition's @48@ thunks go with it, not into
    // dtor.cpp (ModifyLocalGameOptions.cpp keeps its own copy of the
    // class). Overrides keep their base slots.
    virtual void ModifyLocalGameOptions();
    virtual ~JetskiPracticeCompetition();
    virtual JetskiPracticeCompetition *Create();
    virtual void EventComplete();
    virtual void EnableOverlays();

    char pad3E0[0x400 - 0x3E0];
};

#endif
