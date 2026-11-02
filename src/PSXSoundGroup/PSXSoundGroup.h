// PSXSoundGroup: the PS2 sound group (a RainbowPS2Snd group of PSXSounds).
// SoundGroup (0x40 bytes, its volume is SoundGroup::m_38) plus the group ID,
// the sound list and the PSXSoundInterface; 0x60 bytes (8-aligned
// GameObject, data ends at +0x5C). Names come from the class's own methods.
#ifndef PSXSOUNDGROUP_PSXSOUNDGROUP_H
#define PSXSOUNDGROUP_PSXSOUNDGROUP_H

#include "SoundGroup/SoundGroup.h"
#include "RainbowPS2Snd/RainbowPS2Snd.h"
// PSXSoundInterface.h brings SoundInterface.h's ContainerList (inline
// destructor, no Create/AddItem), so m_sounds has that type; files that call
// the list's methods include ContainerList/ContainerList.h instead and keep
// their own PSXSoundGroup.
#include "PSXSoundInterface/PSXSoundInterface.h"

class PSXSound;

class PSXSoundGroup : public SoundGroup {
public:
    PSXSoundGroup(PSXSoundInterface *soundInterface, int id);
    int AddSound(PSXSound *sound);
    void RemoveSound(PSXSound *sound);

    // --- virtuals in retail slot order ---
    // SoundGroup declares Disable..Pause and the last four pure (its vtable
    // has zeros there).
    virtual ~PSXSoundGroup();
    virtual void Disable();
    virtual void Enable();
    virtual void AncestorsDisabled();
    virtual void AncestorsEnabled();
    virtual GameObject *Create();
    virtual int Tick(float dt);
    virtual int Pause(int pause);
    virtual void ReleaseAllSounds();
    virtual int StopAllSounds();
    virtual void DisplayStats();
    virtual void SetVolume(long volume);

    // Signed: the implicit int->unsigned conversion at each RainbowPS2Snd
    // call orders the loads like retail.
    int m_groupID;                      // +0x40
    ContainerList<PSXSound *> m_sounds; // +0x44
    PSXSoundInterface *m_interface;     // +0x58
};

#endif
