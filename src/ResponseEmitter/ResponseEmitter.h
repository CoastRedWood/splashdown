// ResponseEmitter: a SoundEmitter that plays one of a list of sound
// responses (LoadResponseList, TriggerResponse). 0x220 bytes (`new` in the
// UI managers' Create): SoundEmitter's 0x200, then the response list. Names come from the class's own methods.
#ifndef RESPONSEEMITTER_RESPONSEEMITTER_H
#define RESPONSEEMITTER_RESPONSEEMITTER_H

#include "SoundEmitter/SoundEmitter.h"
#include "ContainerList/ContainerList.h"

class AuralScape;
class ParameterBlock;
class PSXSound;
class PSXSoundGroup;
// One loaded response: its event number and sound (8 bytes, `new`ed by
// LoadResponseList).
struct _SoundResponse {
    int m_id;                           // +0x0
    PSXSound *m_sound;                  // +0x4
};

class ResponseEmitter : public SoundEmitter {
public:
    ResponseEmitter(AuralScape *scape, PSXSoundGroup *group, short type, int flags);
    ResponseEmitter *Create(_DS3DBUFFER buffer, unsigned int flags, float a, float b, int c, int d);
    int LoadResponseList(char *name);
    int LoadResponseList(ParameterBlock *block);
    int TriggerResponse(int id, int loop, int priority);
    int Play(PSXSound *sound, int loop, int priority);
    void StopAllResponses();
    void Stop(PSXSound *sound);

    // --- virtuals in retail slot order ---
    virtual ~ResponseEmitter();
    virtual int UpdateSettings(float volume, long pan, _D3DVECTOR position, int flags);

    ContainerList<_SoundResponse *> m_responses;    // +0x200
    char pad214[0x220 - 0x214];
};

#endif
