// AmbientListSoundEmitter: an AmbientEmitter that picks each time from a
// list of sounds (Scene::ReadSoundData news it, 0x230 bytes; nothing is
// known past the list).
#ifndef AMBIENTLISTSOUNDEMITTER_AMBIENTLISTSOUNDEMITTER_H
#define AMBIENTLISTSOUNDEMITTER_AMBIENTLISTSOUNDEMITTER_H

#include "AmbientEmitter/AmbientEmitter.h"
#include "ContainerList/ContainerList.h"

class AmbientListSoundEmitter : public AmbientEmitter {
public:
    AmbientListSoundEmitter(AuralScape *scape, PSXSoundGroup *group, short type, int enabled);
    AmbientListSoundEmitter *Create(_DS3DBUFFER buffer, unsigned int flags, float a, float b, int c, int d);
    PSXSound *LoadSound(char *name);
    int LoadSoundIntoList(char *name);

    // --- virtuals in retail slot order ---
    virtual ~AmbientListSoundEmitter();
    virtual int Tick(float dt);
    virtual int Start();
    virtual int UpdateSettings(float volume, long pan, _D3DVECTOR position, int flags);

    ContainerList<PSXSound *> m_sounds; // +0x210
    char pad224[0x230 - 0x224];
};

#endif
