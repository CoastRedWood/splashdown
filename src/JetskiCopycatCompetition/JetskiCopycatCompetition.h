// JetskiCopycatCompetition: a copycat race (follow the leader's stunts).
// Adds no members that its methods use.
#ifndef JETSKICOPYCATCOMPETITION_H
#define JETSKICOPYCATCOMPETITION_H

#include "JetskiCompetition/JetskiCompetition.h"

class JetskiCopycatCompetition : public JetskiCompetition {
public:
    JetskiCopycatCompetition(int id);
    void SetupOverlays();

    // --- virtuals: overrides keep their base slots ---
    virtual ~JetskiCopycatCompetition();
    virtual JetskiCopycatCompetition *Create();
    virtual void EventComplete();
    virtual void ModifyLocalGameOptions();
    virtual int InitializeRace();
    virtual void EnableOverlays();
};

#endif
