// BowSprayParticleEmitter: the spray off a jetski's bow (JetskiVehicle
// +0x628, five of them). 0x2A0 bytes (`new BowSprayParticleEmitter` in
// JetskiVehicle::Create). Layout from the ctor, UpdatePosition,
// UpdateVelocity and FasterGenerateParticles. Its own position/direction
// pair at +0x200 is separate from ParticleEmitter's m_position (+0x30),
// which Tick uses.
#ifndef BOWSPRAYPARTICLEEMITTER_BOWSPRAYPARTICLEEMITTER_H
#define BOWSPRAYPARTICLEEMITTER_BOWSPRAYPARTICLEEMITTER_H

#include "ParticleEmitter/ParticleEmitter.h"

class BowSprayParticleEmitter : public ParticleEmitter {
public:
    BowSprayParticleEmitter(int enabled);
    void UpdatePosition(_D3DVECTOR position, _D3DVECTOR direction);
    void UpdateVelocity(_D3DVECTOR heading, _D3DVECTOR side, _D3DVECTOR position, float speed, float turn);
    int FasterGenerateParticles(float dt);

    // --- virtuals in retail slot order ---
    virtual ~BowSprayParticleEmitter();
    virtual int Tick(float dt);

    char pad1E8[0x1F0 - 0x1E8];
    int m_x1F0;                         // +0x1F0, next interpolant
    char pad1F4[0x200 - 0x1F4];
    _D3DVECTOR m_sprayPosition;         // +0x200 (UpdatePosition)
    _D3DVECTOR m_direction;             // +0x210
    _D3DVECTOR m_lastSprayPosition;     // +0x220
    _D3DVECTOR m_lastDirection;         // +0x230
    _D3DVECTOR m_forward;               // +0x240, w: forward speed (UpdateVelocity)
    _D3DVECTOR m_side;                  // +0x250, w: side speed
    _D3DVECTOR m_lastForward;           // +0x260
    _D3DVECTOR m_lastSide;              // +0x270
    _D3DVECTOR m_velocityRange;         // +0x280
    int m_x290;                         // +0x290
    float m_x294;                       // +0x294, 0.5
    char pad298[0x2A0 - 0x298];
};

#endif
