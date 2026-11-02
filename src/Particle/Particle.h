// Particle: one particle of a ParticleManager (a plain struct; no retail
// methods), 0x50 bytes. ParticleVUPhysics keeps its own 0x40-byte
// VUParticle (ParticleVUPhysics.h). Names are the most common; +0x20 and
// +0x28 are unions because ParticleManager uses them as a link and a frame
// index, and copies made before VUParticle existed read them as floats.
#ifndef PARTICLE_PARTICLE_H
#define PARTICLE_PARTICLE_H

#include "d3dtypes.h"

struct Particle {
    _D3DVECTOR m_position;              // +0x00
    _D3DVECTOR m_velocity;              // +0x10
    union {
        Particle *m_next;               // +0x20, ParticleManager: next in the group
        float m_x20;                    // +0x20, ParticleVUPhysics
    };
    float m_depth;                      // +0x24
    union {
        int m_frame;                    // +0x28, ParticleManager: index into m_frames
        float m_x28;                    // +0x28, ParticleVUPhysics
    };
    float m_age;                        // +0x2C
    float m_lifetime;                   // +0x30
    float m_oneOverLifetime;            // +0x34
    float m_size;                       // +0x38
    char pad3C[0x40 - 0x3C];
    float m_x40;                        // +0x40
    float m_rise;                       // +0x44
    unsigned int m_flags;               // +0x48
    int m_x4C;                          // +0x4C
};

#endif
