// JetskiTerrainSoundEmitter: a jetski's sounds for running over terrain
// (slow/medium/fast lists picked by speed). A SoundEmitter (0x200) plus
// the vehicle and three sound lists; 0x240 bytes. Names come from the
// class's own methods.
#ifndef JETSKITERRAINSOUNDEMITTER_JETSKITERRAINSOUNDEMITTER_H
#define JETSKITERRAINSOUNDEMITTER_JETSKITERRAINSOUNDEMITTER_H

#include "SoundEmitter/SoundEmitter.h"
#include "ContainerList/ContainerList.h"

class JetskiVehicle;
struct VehicleAudType;

class JetskiTerrainSoundEmitter : public SoundEmitter {
public:
    JetskiTerrainSoundEmitter(AuralScape *scape, PSXSoundGroup *group, short id, int flags);
    int Create(JetskiVehicle *vehicle, VehicleAudType *audType, float maxDistance);
    int LoadTerrainSounds(VehicleAudType *audType);
    int PlayTerrainSounds();
    int SwitchSounds(PSXSound *sound);
    int PositionEmitter();

    // --- virtuals in retail slot order ---
    virtual ~JetskiTerrainSoundEmitter();
    virtual int Tick(float dt);

    JetskiVehicle *m_vehicle;                   // +0x200
    ContainerList<PSXSound *> m_slowSounds;     // +0x204
    ContainerList<PSXSound *> m_mediumSounds;   // +0x218
    ContainerList<PSXSound *> m_fastSounds;     // +0x22C
};

#endif
