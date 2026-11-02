// Sound: the abstract sound (PSXSound's base). A BaseObject (0x10 bytes,
// vptr +0x8) plus four words; 0x20 bytes. Its vtable holds BaseObject's
// slots and twelve pure virtuals, declared here with PSXSound's
// signatures.
#ifndef SOUND_SOUND_H
#define SOUND_SOUND_H

#include "d3dtypes.h"
#include "GameObject/GameObject.h"

class Sound : public BaseObject {
public:
    Sound();

    // --- virtuals in retail slot order ---
    virtual ~Sound();
    virtual int CreateFromFile(char *name, unsigned int flags, unsigned int arg) = 0;
    virtual int Play(int restart, int loop) = 0;
    virtual int Stop(int fade) = 0;
    virtual int IsPlaying() = 0;
    virtual int SetFrequency(unsigned int frequency) = 0;
    virtual int SetVolume(float volume) = 0;
    virtual int SetPan(long pan) = 0;
    virtual int Pause(int pause) = 0;
    virtual int Tick(float dt) = 0;
    virtual int SetPosition(_D3DVECTOR position) = 0;
    virtual int SetVelocity(_D3DVECTOR velocity) = 0;
    virtual int SetMinMaxDistance(float minDistance, float maxDistance) = 0;

    int m_state;                        // +0x10 (2 = has a voice)
    int m_x14;                          // +0x14
    int m_x18;                          // +0x18
    int m_waveMemory;                   // +0x1C
};

#endif
