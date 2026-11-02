// SmogParticleEmitter: the smoke emitter (a ParticleEmitter). 0x210 bytes
// (`new` in Smoker::Create); its data starts at +0x1F0 (+0x1E8..+0x1EF are
// never touched). The setters name +0x1F0..+0x20C; SetSize also writes
// ParticleEmitter's +0x188 and +0x190.
#ifndef SMOGPARTICLEEMITTER_SMOGPARTICLEEMITTER_H
#define SMOGPARTICLEEMITTER_SMOGPARTICLEEMITTER_H

#include "ParticleEmitter/ParticleEmitter.h"

class SmogParticleEmitter : public ParticleEmitter {
public:
    SmogParticleEmitter(int enabled);
    void SetVelocity(_D3DVECTOR velocity);
    void SetSize(float size);
    void SetGrowth(float growth);
    void SetVarianceFactor(float factor);

    // --- virtuals in retail slot order ---
    virtual ~SmogParticleEmitter();
    virtual int Tick(float dt);
    virtual int ReloadFromFile();

    char pad1E8[0x1F0 - 0x1E8];
    _D3DVECTOR m_velocity;              // +0x1F0 (SetVelocity)
    float m_size;                       // +0x200 (SetSize)
    float m_growth;                     // +0x204 (SetGrowth)
    float m_rate;                       // +0x208
    float m_varianceFactor;             // +0x20C (SetVarianceFactor)
};

#endif
