// AmbientEmitter: a SoundEmitter that replays its sound at random
// intervals (SoundEmitter::m_pitch at +0x1E0 is used as the percent chance
// per interval). 0x210 bytes (AmbientListSoundEmitter's members start
// there); nothing is known past +0x208.
#ifndef AMBIENTEMITTER_AMBIENTEMITTER_H
#define AMBIENTEMITTER_AMBIENTEMITTER_H

#include "SoundEmitter/SoundEmitter.h"

class AmbientEmitter : public SoundEmitter {
public:
    AmbientEmitter(AuralScape *scape, PSXSoundGroup *group, short type, int enabled);

    // --- virtuals in retail slot order ---
    virtual ~AmbientEmitter();
    virtual int Tick(float dt);
    virtual int Start();
    virtual int UpdateSettings(float volume, long pan, _D3DVECTOR position, int flags);

    float m_interval;                   // +0x200, ms
    float m_elapsed;                    // +0x204, ms
    char pad208[0x210 - 0x208];
};

#endif
