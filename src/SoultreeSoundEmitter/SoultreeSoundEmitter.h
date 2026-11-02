// SoultreeSoundEmitter: a SoundEmitter that follows a SoultreeObject (or a
// named object inside it), 0x210 bytes (AuralScape::CreateSoultreeEmitter
// news it).
#ifndef SOULTREESOUNDEMITTER_SOULTREESOUNDEMITTER_H
#define SOULTREESOUNDEMITTER_SOULTREESOUNDEMITTER_H

#include "SoundEmitter/SoundEmitter.h"

class SoultreeObject;

class SoultreeSoundEmitter : public SoundEmitter {
public:
    SoultreeSoundEmitter(AuralScape *scape, PSXSoundGroup *group, short type, int enabled);
    SoultreeSoundEmitter *Create(char *name, SoultreeObject *object, _DS3DBUFFER buffer, unsigned int flags,
                                 float a, float b, int c, int d);

    // --- virtuals in retail slot order ---
    virtual ~SoultreeSoundEmitter();
    virtual int Tick(float dt);
    virtual int UpdateSettings(float volume, long flags, _D3DVECTOR position, int priority);

    SoultreeObject *m_target;           // +0x200 (the named object Create looks up; Tick follows it first)
    SoultreeObject *m_object;           // +0x204
    char pad208[0x210 - 0x208];
};

#endif
