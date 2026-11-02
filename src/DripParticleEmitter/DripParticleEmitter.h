// DripParticleEmitter: the water dripping off a jetski (JetskiVehicle
// +0x63C). 0x290 bytes (`new DripParticleEmitter`). Layout from the ctor,
// UpdatePosition and SetDrip/StopDrip.
#ifndef DRIPPARTICLEEMITTER_DRIPPARTICLEEMITTER_H
#define DRIPPARTICLEEMITTER_DRIPPARTICLEEMITTER_H

#include "ParticleEmitter/ParticleEmitter.h"

class DripParticleEmitter : public ParticleEmitter {
public:
    DripParticleEmitter(int enabled);
    void UpdatePosition(_D3DVECTOR position, _D3DVECTOR direction);
    void SetDrip();
    void StopDrip();
    int FasterGenerateParticles(float dt);

    // --- virtuals in retail slot order ---
    virtual ~DripParticleEmitter();
    virtual int Tick(float dt);

    char pad1E8[0x1F0 - 0x1E8];
    int m_x1F0;                         // +0x1F0
    char pad1F4[0x200 - 0x1F4];
    _D3DVECTOR m_dripPosition;          // +0x200
    _D3DVECTOR m_direction;             // +0x210
    _D3DVECTOR m_lastDripPosition;      // +0x220
    _D3DVECTOR m_lastDirection;         // +0x230
    float m_x240;                       // +0x240, 1.0
    float m_x244;                       // +0x244, 1 / m_x240
    float m_x248;                       // +0x248, the drip's age (SetDrip clears it)
    float m_x24C;                       // +0x24C, 96.0
    float m_x250;                       // +0x250, 1.0
    char pad254[0x260 - 0x254];
    _D3DVECTOR m_x260;                  // +0x260
    _D3DVECTOR m_x270;                  // +0x270
    int m_dripping;                     // +0x280 (SetDrip/StopDrip)
    int m_x284;                         // +0x284
    float m_width;                      // +0x288, 2.0
    char pad28C[0x290 - 0x28C];
};

#endif
