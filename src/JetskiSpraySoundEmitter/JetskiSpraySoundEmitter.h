// JetskiSpraySoundEmitter: the looping spray sound under a jetski's bow. A
// SoundEmitter whose volume follows the bow spray's emit rate (PlaySpraySounds).
// 0x210 bytes (`new JetskiSpraySoundEmitter` in JetskiVehicle::LoadGenericSounds).
#ifndef JETSKISPRAYSOUNDEMITTER_JETSKISPRAYSOUNDEMITTER_H
#define JETSKISPRAYSOUNDEMITTER_JETSKISPRAYSOUNDEMITTER_H

#include "SoundEmitter/SoundEmitter.h"

class JetskiVehicle;
struct VehicleAudType;

class JetskiSpraySoundEmitter : public SoundEmitter {
public:
    JetskiSpraySoundEmitter(AuralScape *scape, PSXSoundGroup *group, short id, int enabled);
    int Create(JetskiVehicle *vehicle, VehicleAudType *audType, float maxDistance);  // callers test the result
    int LoadSpraySounds(VehicleAudType *audType);
    int PlaySpraySounds();
    int PositionEmitter();

    // --- virtuals in retail slot order ---
    virtual ~JetskiSpraySoundEmitter();
    virtual int Tick(float dt);

    JetskiVehicle *m_vehicle;           // +0x200
    int m_volume;                       // +0x204, 0..100
    int m_x208;                         // +0x208 (the ctor clears it)
    char pad20C[0x210 - 0x20C];
};

#endif
