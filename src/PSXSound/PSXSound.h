// PSXSound — a sound played through the PSXSoundInterface's RainbowPS2Snd.
// 0x1B0 bytes (`new` in PSXSoundInterface::GetSoundFromResourceName).
//
// Derives from Sound (BaseObject + four words at +0x10..+0x1C). Virtuals are
// Sound's, in retail slot order.
#ifndef PSXSOUND_H
#define PSXSOUND_H

#include "d3dtypes.h"
#include "GameObject/GameObject.h"
#include "Sound/Sound.h"
class PSXSoundGroup;
class PSXSoundInterface;       // PSXSoundInterface/PSXSoundInterface.h

class PSXSound : public Sound {
public:
    PSXSound(PSXSoundInterface *soundInterface, PSXSoundGroup *group, short arg);
    int LoadPreviouslyCreatedSound(char *name);
    int SetBaseVolume(float volume);
    int SetBasePan(long pan);
    int SetFrequencyModifier(float modifier);
    int UpdateSettings(float volume, long pan, _D3DVECTOR velocity, int doppler);

    virtual ~PSXSound();
    virtual int CreateFromFile(char *name, unsigned int flags, unsigned int arg);
    virtual int Play(int restart, int loop);
    virtual int Stop(int fade);
    virtual int IsPlaying();
    virtual int SetFrequency(unsigned int frequency);
    virtual int SetVolume(float volume);
    virtual int SetPan(long pan);
    virtual int Pause(int pause);
    virtual int Tick(float dt);
    virtual int SetPosition(_D3DVECTOR position);
    virtual int SetVelocity(_D3DVECTOR velocity);
    virtual int SetMinMaxDistance(float minDistance, float maxDistance);

    float m_duration;                   // +0x20
    float m_elapsed;                    // +0x24, play time (Tick adds dt while there is a voice)
    int m_soundHandle;                  // +0x28
    int m_handle;                       // +0x2C
    PSXSoundInterface *m_interface;     // +0x30
    PSXSoundGroup *m_group;             // +0x34
    int m_baseVolume;                   // +0x38
    int m_basePan;                      // +0x3C
    int m_pitch;                        // +0x40
    int m_volume;                       // +0x44
    int m_pan;                          // +0x48, degrees
    int m_playPitch;                    // +0x4C
    int m_basePitch;                    // +0x50
    float m_frequencyModifier;          // +0x54
    char pad58[0x60 - 0x58];
    _D3DVECTOR m_position;              // +0x60
    int m_x70;                          // +0x70
    int m_unk74;                        // +0x74, started
    int m_paused;                       // +0x78
    char pad7C[0x80 - 0x7C];
    _D3DVECTOR m_velocity;              // +0x80
    int m_nameIndex;                    // +0x90
    char m_name[0x104];                 // +0x94
    int m_x198;                         // +0x198
    int m_voice;                        // +0x19C
    int m_streamed;                     // +0x1A0
    int m_stream;                       // +0x1A4
    char pad1A8[0x1B0 - 0x1A8];
};

#endif
