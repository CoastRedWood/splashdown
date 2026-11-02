// JetskiCrowdSoundEmitter: the crowd's cheer/boo/ohh sounds. A
// SoundEmitter (0x200) plus three sound lists; 0x240 bytes (8-aligned,
// data ends at +0x23C). Names come from the class's own methods.
#ifndef JETSKICROWDSOUNDEMITTER_JETSKICROWDSOUNDEMITTER_H
#define JETSKICROWDSOUNDEMITTER_JETSKICROWDSOUNDEMITTER_H

#include "SoundEmitter/SoundEmitter.h"
#include "ContainerList/ContainerList.h"

class JetskiCrowdSoundEmitter : public SoundEmitter {
public:
    JetskiCrowdSoundEmitter(AuralScape *scape, PSXSoundGroup *group, short type, int enabled);
    JetskiCrowdSoundEmitter *Create(char *name);    // retail returns SoundEmitter::Create's result (JetskiCompetition::Create stores it)
    int LoadSounds(char *file);
    int PlayCheerSound();
    int PlayBooSound();
    int PlayOhhSound();
    int SwitchSounds(PSXSound *sound);

    // --- virtuals in retail slot order ---
    virtual ~JetskiCrowdSoundEmitter();

    ContainerList<PSXSound *> m_cheerSounds;    // +0x200
    ContainerList<PSXSound *> m_booSounds;      // +0x214
    ContainerList<PSXSound *> m_ohhSounds;      // +0x228
};

#endif
