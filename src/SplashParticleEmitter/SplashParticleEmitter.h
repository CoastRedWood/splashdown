// SplashParticleEmitter: a one-shot splash (InitiateSplash starts it).
// 0x250 bytes (`new SplashParticleEmitter`). Layout from the ctor and
// InitiateSplash.
#ifndef SPLASHPARTICLEEMITTER_SPLASHPARTICLEEMITTER_H
#define SPLASHPARTICLEEMITTER_SPLASHPARTICLEEMITTER_H

#include "ParticleEmitter/ParticleEmitter.h"

class SplashParticleEmitter : public ParticleEmitter {
public:
    SplashParticleEmitter(int enabled);
    void InitiateSplash(_D3DVECTOR position, _D3DVECTOR velocity);
    int SlowGenerateParticles(float dt);

    // --- virtuals in retail slot order ---
    virtual ~SplashParticleEmitter();
    virtual int Tick(float dt);

    char pad1E8[0x1F0 - 0x1E8];
    int m_x1F0;                         // +0x1F0
    int m_x1F4;                         // +0x1F4
    char pad1F8[0x200 - 0x1F8];
    _D3DVECTOR m_splashPosition;        // +0x200
    _D3DVECTOR m_splashVelocity;        // +0x210
    float m_elapsed;                    // +0x220
    float m_x224;                       // +0x224
    float m_duration;                   // +0x228
    float m_oneOverDuration;            // +0x22C
    float m_x230;                       // +0x230
    float m_x234;                       // +0x234
    float m_x238;                       // +0x238
    int m_x23C;                         // +0x23C
    float m_x240;                       // +0x240
    char pad244[0x250 - 0x244];
};

#endif
