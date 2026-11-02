// ParticleVUPhysics: the VU1 particle renderer behind ParticleManager (a
// BaseObject, vptr at +0x08). 0x140 bytes (`new` in ParticleManager); data
// +0x10..+0x13B. +0x30, +0x40, +0x50[2], +0x70 and +0x90 are _D3DVECTORs:
// the ctor constructs them (its __construct_array call is m_v50). Most
// copies read them as floats. SetVelocityRange writes +0x70..+0x78,
// SetPacketAlphaRange +0x28/+0x2C.
#ifndef PARTICLEVUPHYSICS_PARTICLEVUPHYSICS_H
#define PARTICLEVUPHYSICS_PARTICLEVUPHYSICS_H

#include "GameObject/GameObject.h"
#include "DmaRenderItem/DmaRenderItem.h"
#include "sceGifTag/sceGifTag.h"
#include "d3dtypes.h"

class TextureMap;

// One particle as ParticleVUPhysics keeps it: 0x40 bytes (Initialize news
// m_numTotal << 6), unlike ParticleManager's 0x50-byte Particle. Plain floats,
// not _D3DVECTORs: a member with a constructor would make `new VUParticle[n]`
// call __construct_array.
struct VUParticle {
    float m_position[4];                // +0x00
    float m_velocity[4];                // +0x10
    float m_x20;                        // +0x20
    // Life left (SplashParticleEmitter::SlowGenerateParticles stores the
    // emitter's particle life here and its reciprocal at +0x28; Initialize
    // gives an unused particle 0 and 1e10). UpdateParticlePacket sends it to
    // VU1 as the point's w; UpdateParticlePackets ranks packets by it.
    float m_life;                       // +0x24
    float m_oneOverLife;                // +0x28
    float m_size;                       // +0x2C, 0.35 from SlowGenerateParticles (swc1)
    float m_growth;                     // +0x30, 2.0 from SlowGenerateParticles (swc1)
    char pad34[0x40 - 0x34];
};

class ParticleVUPhysics : public BaseObject {
public:
    ParticleVUPhysics();
    void Initialize(TextureMap *texture, int count, int numParticles, int numTotal);
    int Render3D();
    void ResetPacket();
    void SetPacketAlphaRange(float a, float b);
    void SetupPacket(DmaRenderItem *item);
    void SetVelocityRange(float x, float y, float z);
    void UpdateParticlePacket(int packet, DmaRenderItem *item, int first, int count, int flags);
    void UpdateParticlePackets(int first, int count, int flags);
    void UpdatePerFrameStates();

    // --- virtuals in retail slot order ---
    virtual ~ParticleVUPhysics();
    virtual int Tick(float dt);

    VUParticle *m_particles;            // +0x10
    float m_depthBias;                  // +0x14
    int m_18;                           // +0x18
    int m_dirty;                        // +0x1C
    float m_x20;                        // +0x20
    float m_x24;                        // +0x24
    float m_x28;                        // +0x28
    float m_x2C;                        // +0x2C
    _D3DVECTOR m_v30;                   // +0x30, (0, -20, 0) from the ctor; .y is the gravity
    _D3DVECTOR m_v40;                   // +0x40, the colour (128 each)
    _D3DVECTOR m_v50[2];                // +0x50, the texture uv corners (u0, v0), (u1, v1)
    _D3DVECTOR m_velocityScale;         // +0x70 (SetVelocityRange: 32767 / range)
    int m_count;                        // +0x80, number of packets
    int m_numParticles;                 // +0x84, per packet
    int m_numTotal;                     // +0x88
    char pad8C[0x90 - 0x8C];
    _D3DVECTOR m_v90;                   // +0x90, (-100000, ..., 1) from the ctor
    _D3DVECTOR *m_xA0;                  // +0xA0
    int m_A4;                           // +0xA4
    TextureMap *m_texture;              // +0xA8
    DmaRenderItem *m_items;             // +0xAC
    DmaRenderItem m_itemB0;             // +0xB0
    DmaRenderItem m_itemE0;             // +0xE0
    sceGifTag m_gifTag;                 // +0x110
    int *m_packetDepths;                // +0x120, per-packet sort depth
    float *m_124;                       // +0x124
    float *m_packetMinDepths;           // +0x128
    int m_currentPacket;                // +0x12C
    char pad130[0x134 - 0x130];
    int m_numUpdated;                   // +0x134
    int m_lastPacket;                   // +0x138
};

#endif
