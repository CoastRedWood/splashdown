// JetskiSplashSoundEmitter: a jetski's spray and splash sounds. A
// SoundEmitter (0x200) plus the vehicle, the splash state and five sound
// lists; 0x280 bytes (data ends at 0x274). Names come from the class's own methods.
#ifndef JETSKISPLASHSOUNDEMITTER_JETSKISPLASHSOUNDEMITTER_H
#define JETSKISPLASHSOUNDEMITTER_JETSKISPLASHSOUNDEMITTER_H

#include "SoundEmitter/SoundEmitter.h"
#include "ContainerList/ContainerList.h"

class JetskiVehicle;
struct VehicleAudType;

class JetskiSplashSoundEmitter : public SoundEmitter {
public:
    JetskiSplashSoundEmitter(AuralScape *scape, PSXSoundGroup *group, short type, int enabled);
    int Create(JetskiVehicle *vehicle, VehicleAudType *audType, float volume);  // callers test the result
    int LoadSplashSounds(VehicleAudType *audType);
    int Play(PSXSound *sound, int loop);
    int PlaySplashSounds();
    int PositionEmitter();

    // --- virtuals in retail slot order ---
    virtual ~JetskiSplashSoundEmitter();
    virtual int Tick(float dt);

    JetskiVehicle *m_vehicle;                   // +0x200
    float m_delay;                              // +0x204, before the next splash may play
    int m_playing;                              // +0x208
    int m_jumping;                              // +0x20C
    ContainerList<PSXSound *> m_sprays;         // +0x210
    ContainerList<PSXSound *> m_fastSplashes;   // +0x224
    ContainerList<PSXSound *> m_mediumSplashes; // +0x238
    ContainerList<PSXSound *> m_bigSplashes;    // +0x24C
    ContainerList<PSXSound *> m_smallSplashes;  // +0x260
    char pad274[0x280 - 0x274];                 // retail news 0x280 bytes (JetskiVehicle::LoadGenericSounds)
};

#endif
