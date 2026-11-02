// SoundEmitter: a GameObject that plays sounds from a PSXSoundGroup at a
// position (AuralScape attenuates them). 0x200 bytes: SoultreeSoundEmitter's
// members start at +0x200. Names come from the class's own methods.
#ifndef SOUNDEMITTER_H
#define SOUNDEMITTER_H

#include "d3dtypes.h"
#include "GameObject/GameObject.h"

class AuralScape;
class PSXSound;
class PSXSoundGroup;
class SoundListener;
#include "_DS3DBUFFER/_DS3DBUFFER.h"

// pack(4): the class stays 4-aligned although m_buffer's vectors are 16-byte
// aligned, so derived classes start their members right at 0x200 + n (a
// 16-aligned SoundEmitter moves VehicleEmitter's subclasses: retail's `new
// JetskiEngineSoundEmitter` is 0x340, not 0x350).
#pragma pack(4)
class SoundEmitter : public GameObject {
public:
    SoundEmitter(AuralScape *auralScape, PSXSoundGroup *group, short id, int enabled);
    SoundEmitter *Create(_DS3DBUFFER buffer, unsigned int flags, float a, float b, int c, int d);
    int LoadSound(char *name, int looped);
    void UnloadSound();
    PSXSound *GetActiveSound();
    int SetActiveSound(PSXSound *sound);
    int IsPlaying();
    float GetBaseVolume();
    int SetBaseVolume(float volume);
    void EnableEmitter();
    void DisableEmitter();
    void UpdateEmitter(_DS3DBUFFER buffer);
    void UpdateEmitter(_D3DVECTOR *position, _D3DVECTOR *velocity);
    void UpdateFromAuralscape(int numListeners, SoundListener **listeners);

    // --- virtuals in retail slot order ---
    virtual ~SoundEmitter();
    virtual int Stop();
    virtual int Start();
    virtual int UpdateSettings(float volume, long flags, _D3DVECTOR position, int priority);

    static int UsedEmitterID;           // the next m_emitterID

    PSXSoundGroup *m_group;             // +0x30
    char pad34[0x40 - 0x34];
    _D3DVECTOR m_lastPosition;          // +0x40
    int m_emitterID;                    // +0x50
    char m_name[0x104];                 // +0x54
    unsigned int m_flags;               // +0x158
    char pad15C[0x160 - 0x15C];
    _DS3DBUFFER m_buffer;               // +0x160
    float m_closestDistance;            // +0x1D0
    unsigned char m_disabled : 1;       // +0x1D4 bit 0
    unsigned char m_bit1 : 1;           //        bit 1
    unsigned char m_positional : 1;     //        bit 2
    unsigned char m_looped : 1;         //        bit 3
    unsigned char m_playing : 1;        //        bit 4
    AuralScape *m_system;               // +0x1D8
    float m_volume;                     // +0x1DC
    float m_pitch;                      // +0x1E0
    short m_id;                         // +0x1E4
    int m_forcePlaying;                 // +0x1E8
    int m_enabled;                      // +0x1EC
    PSXSound *m_sound;                  // +0x1F0
    char pad1F4[0x200 - 0x1F4];
};
#pragma pack()

#endif
