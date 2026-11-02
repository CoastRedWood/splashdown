// JetskiCollisionModel: the jetski's SpringCollisionModel, with one extra
// float at +0x130 (1.0 from the ctor). 0x140 bytes (`new JetskiCollisionModel`).
#ifndef JETSKICOLLISIONMODEL_JETSKICOLLISIONMODEL_H
#define JETSKICOLLISIONMODEL_JETSKICOLLISIONMODEL_H

#include "SpringCollisionModel/SpringCollisionModel.h"

class JetskiCollisionModel : public SpringCollisionModel {
public:
    JetskiCollisionModel(int enabled);

    // --- virtuals in retail slot order ---
    virtual ~JetskiCollisionModel();
    virtual int SimulationTick(float dt);

    float m_x130;                       // +0x130, 1.0 from the ctor
    char pad134[0x140 - 0x134];
};

#endif
