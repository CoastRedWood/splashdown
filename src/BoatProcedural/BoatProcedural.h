// BoatProcedural: a spline-driven ambient boat, a CarProcedural with one
// extra word at +0x4B0 (its water, cleared by the ctor). 0x4B8 bytes.
#ifndef BOATPROCEDURAL_BOATPROCEDURAL_H
#define BOATPROCEDURAL_BOATPROCEDURAL_H

#include "CarProcedural/CarProcedural.h"

class Water;

class BoatProcedural : public CarProcedural {
public:
    BoatProcedural(int enabled);
    void ResetTime(float t, int a);

    virtual ~BoatProcedural();
    virtual int Tick(float dt);

    Water *m_water;                     // +0x4B0 (SetUpAnimations, Tick)
    char pad4B4[0x4C0 - 0x4B4];         // `new BoatProcedural` (Scene::ReadAnimationData) is 0x4C0
};

#endif
