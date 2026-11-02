// JetParticleEmitter: the jetski's rear jet spray (JetskiVehicle +0x640).
// 0x260 bytes (`new JetParticleEmitter`). Layout from the ctor,
// UpdateVelocity and SetJet.
#ifndef JETPARTICLEEMITTER_JETPARTICLEEMITTER_H
#define JETPARTICLEEMITTER_JETPARTICLEEMITTER_H

#include "ParticleEmitter/ParticleEmitter.h"

class JetParticleEmitter : public ParticleEmitter {
public:
    JetParticleEmitter(int enabled);
    void UpdateVelocity(_D3DVECTOR velocity);
    void SetJet();
    int FasterGenerateParticles(float dt);

    // --- virtuals in retail slot order ---
    virtual ~JetParticleEmitter();
    virtual int Tick(float dt);

    char pad1E8[0x1F0 - 0x1E8];
    _D3DVECTOR m_jetVelocity;           // +0x1F0
    _D3DVECTOR m_lastJetVelocity;       // +0x200
    _D3DVECTOR m_x210;                  // +0x210
    _D3DVECTOR m_x220;                  // +0x220
    int m_x230;                         // +0x230
    float m_x234;                       // +0x234, the jet's age (SetJet clears it)
    float m_x238;                       // +0x238, 0.35
    float m_x23C;                       // +0x23C, 1 / m_x238
    float m_oneOverLife;                // +0x240, 1 / m_particleLife
    float m_width;                      // +0x244, 0.25
    float m_x248;                       // +0x248, 0.3
    int m_x24C;                         // +0x24C (SetJet sets 1)
    float m_x250;                       // +0x250, 190.0
    int m_x254;                         // +0x254
    char pad258[0x260 - 0x258];
};

#endif
