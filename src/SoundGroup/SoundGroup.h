// SoundGroup — GameObject grouping sounds.
#ifndef SOUNDGROUP_H
#define SOUNDGROUP_H

#include "GameObject/GameObject.h"

class SoundGroup : public GameObject {
public:
    SoundGroup(int enabled);
    virtual ~SoundGroup();
    // Pure in retail (zeros in SoundGroup's vtable); PSXSoundGroup defines them.
    virtual void Disable() = 0;
    virtual void Enable() = 0;
    virtual void AncestorsDisabled() = 0;
    virtual void AncestorsEnabled() = 0;
    virtual int Tick(float dt) = 0;
    virtual int Pause(int pause) = 0;
    virtual void ReleaseAllSounds() = 0;
    virtual int StopAllSounds() = 0;
    virtual void DisplayStats() = 0;
    virtual void SetVolume(long volume) = 0;
    unsigned char m_30 : 1;     // +0x30 bit 0
    long m_38;                  // +0x38 (64-bit)
};

#endif
