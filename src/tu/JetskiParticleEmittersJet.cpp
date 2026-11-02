// JetskiParticleEmittersJet.cpp  @ 0x003D9130 (text 0x3D9130-0x3D9DB0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's JetskiParticleEmittersJet.cpp as one translation unit: an
// unanchored TU in JetskiRollControlModel.cpp's tu_map row,
// JetParticleEmitter's six functions, then the static initializer (the .ctor
// slot after JetskiParticleEmittersSplash.cpp's), which constructs the axis
// vectors and WidthOffsets. `make link` places the object's data as well as
// its code (tools/link_c.py).



#include "JetParticleEmitter/JetParticleEmitter.h"
#include "ParticleEmitter/ParticleEmitter.h"
#include "ParticleManager/ParticleManager.h"
#include "LightManager/LightManager.h"
#include "LightEmitter/LightEmitter.h"
#include "ParticleVUPhysics/ParticleVUPhysics.h"
#include "JetskiGame/JetskiGame.h"
#include "sinit/AxisVectors.h"

// ---- src/JetParticleEmitter/ctor.cpp

extern "C" int rand();

// Per-particle random blend weights, speed scales and width offsets, shared
// by all jets (the first two with the bow sprays) and filled in by the first
// one constructed.
// (The first two are JetskiParticleEmitters.cpp's; WidthOffsets is
// constructed by the static initializer.)
extern int InterpolantsCalculated;
extern _D3DVECTOR Interpolants[2048];
extern _D3DVECTOR VelocityMultiplier[2048];
int WidthOffsetsCalculated;
_D3DVECTOR WidthOffsets[250];

static inline float RandomUnit() { return 4.656613e-10f * rand(); }

JetParticleEmitter::JetParticleEmitter(int enabled) : ParticleEmitter(enabled)
{
    int i;
    if (!InterpolantsCalculated) {
        for (i = 0; i < 1024; i++) {
            Interpolants[i].x = 4.656613e-10f * rand();
            Interpolants[i].y = 1.0f - Interpolants[i].x;
            float t = 4.656613e-10f * rand();
            Interpolants[i].z = t;
            Interpolants[i].w = 1.0f - t;
            VelocityMultiplier[i].x = 1.0f - 0.05f * (4.656613e-10f * rand());
            t = 4.656613e-10f * rand();
            VelocityMultiplier[i].y = 1.0f - 0.05f * t;
            t = 4.656613e-10f * rand();
            VelocityMultiplier[i].z = 1.0f - 0.05f * t;
        }
        InterpolantsCalculated = 1;
    }
    m_width = 0.25f;
    if (!WidthOffsetsCalculated) {
        for (i = 0; i < 250; i++) {
            WidthOffsets[i].x = 2.0f * (m_width * RandomUnit()) - m_width;
            WidthOffsets[i].y = 2.0f * (m_width * RandomUnit()) - m_width;
            WidthOffsets[i].z = 2.0f * (m_width * RandomUnit()) - m_width;
            WidthOffsets[i].w = 2.0f * (m_width * RandomUnit()) - m_width;
        }
        // Retail sets the bow spray's flag, not WidthOffsetsCalculated, so
        // every jet refills the table.
        InterpolantsCalculated = 1;
    }
    m_x230 = 0;
    m_x254 = 0;
    m_x234 = 0;
    m_x238 = 0.35f;
    m_x23C = 1.0f / m_x238;
    m_x248 = 0.3f;
    m_emitRate = 360.0f;
    m_particleLife = 250.0f;
    m_oneOverLife = 1.0f / m_particleLife;
    m_jetVelocity = _D3DVECTOR(0.0f, 0.0f, 0.0f);
    m_lastJetVelocity = _D3DVECTOR(0.0f, 0.0f, 0.0f);
    m_position = _D3DVECTOR(0.0f, 0.0f, 0.0f);
    m_lastPosition = m_position;
    m_x250 = 190.0f;
}


// ---- src/JetParticleEmitter/FasterGenerateParticles.cpp
#pragma fast_fptosi on


static inline float Fabs(float x)
{
    float r;
    asm("abs.s $f8, %1; swc1 $f8, 0(%0)" : : "r"(&r), "f"(x) : "$f8", "memory");
    return r;
}

static inline float Fmax(float a, float b)
{
    float r;
    asm("max.s $f8, %1, %2; swc1 $f8, 0(%0)" : : "r"(&r), "f"(a), "f"(b) : "$f8", "memory");
    return r;
}

extern LightManager *LpLightManager;

struct ParticleFrame {
    float m_uv[8];                      // four corners
};

// Session shim: a heap image for mwcc's uninitialised "has call" bytes. It flags SetPacketAlphaRange's
// 1.0f, so its second argument is evaluated before the m_vuPhysics load, as in retail. Image object in
// .mwccshim.


struct MwccHeapImage {
    char pad0[0x491D];
    char flag0;
};
#pragma section mwccshim begin
inline void MwccHeapShim() { const MwccHeapImage image = { {0}, 1 }; }
#pragma section mwccshim end

int JetParticleEmitter::FasterGenerateParticles(float dt)
{
    static int BowSprayTick3 = -1;  // unused profiler ID
    if (!m_x24C)
        return 1;
    _D3DVECTOR position = m_position;
    _D3DVECTOR positionDelta = m_lastPosition - m_position;
    _D3DVECTOR jet = m_jetVelocity;
    _D3DVECTOR jetDelta = m_lastJetVelocity - m_jetVelocity;
    float x = Fmax(Fabs(m_lastJetVelocity.x), Fabs(m_jetVelocity.x));
    float y = Fmax(Fabs(m_lastJetVelocity.y), Fabs(m_jetVelocity.y));
    float z = Fmax(Fabs(m_lastJetVelocity.z), Fabs(m_jetVelocity.z));
    float v = 1.5f * Fmax(x, z);
    m_particleManager->m_vuPhysics->SetVelocityRange(v, y, v);
    m_particleManager->m_vuPhysics->SetPacketAlphaRange(0.0f, m_x250 * (1.0f - m_x234 * m_x23C));
    m_x234 += dt;
    if (m_x234 > m_x238)
        m_x24C = 0;
    ParticleVUPhysics *vu = m_particleManager->m_vuPhysics;
    int count = m_emitRate * dt;
    if (vu && !vu->m_18)
        vu->Initialize((TextureMap *)m_particleManager->m_texture, 900, 12, 10000);
    if (!vu || !vu->m_18)
        return 1;
    LightEmitter *light = LpLightManager->FindLight((LightEmitterType)3);
    if (light) {
        float r = light->m_ambient.r;
        float g = light->m_ambient.g;
        float b = light->m_ambient.b;
        light = LpLightManager->FindLight((LightEmitterType)2);
        if (light) {
            r += 0.5f * light->m_diffuse.r;
            g += 0.5f * light->m_diffuse.g;
            b += 0.5f * light->m_diffuse.b;
        }
        *(_D3DVECTOR *)&vu->m_v40.x = _D3DVECTOR(r, g, b);
    }
    vu->m_v90.y = -100.0f;
    *(_D3DVECTOR *)&vu->m_v30.x = ZeroVector;
    VUParticle *particle = &vu->m_particles[vu->m_dirty];
    if (!count)
        return 1;
    vu->m_dirty += count;
    if (count + m_x230 >= 0x400)
        m_x230 = 0;
    if (count + m_x254 >= 250)
        m_x254 = 0;
    float *interpolant, *width;
    interpolant = &Interpolants[m_x230].x;
    width = &WidthOffsets[m_x254].x;
    m_x230 += count;
    m_x254 += count;
    _D3DVECTOR params;
    params.x = 1000.0f * dt;
    params.y = m_particleLife;
    params.z = m_oneOverLife;
    params.w = m_x248;
    int zero = 0;
    // Emit count particles on VU0: each lerps between the last and current
    // position and jet velocity by its interpolant and spreads it by its width
    // offset. Retail's loop is hand-written (`addi`, `bne $0, ...`); `volatile`
    // keeps mwcc from rescheduling it.
    asm volatile(".set noreorder; .set noat; lqc2 vf3, 0x0(%0); lqc2 vf4, 0x0(%1); lqc2 vf5, 0x0(%4); lqc2 vf13, 0x0(%5); vsub.xyzw vf6, vf13, vf5; lqc2 vf7, 0x0(%6); lqc2 vf14, 0x0(%7); vsub.xyzw vf8, vf14, vf7; lqc2 vf9, 0x0(%8); lqc2 vf10, 0x0(%9); lw $1, 0x0(%11); nop; 1: lqc2 vf3, 0x0(%0); lqc2 vf4, 0x0(%1); lqc2 vf11, 0x0(%10); vmulx.xyz vf12, vf8, vf3x; vadd.xyz vf2, vf7, vf12; vmulax.xyz ACC, vf6, vf3x; vmulax.xyz ACC, vf9, vf4x; vmaddy.xyz vf1, vf10, vf4y; vadd.xyz vf1, vf1, vf5; vsubw.x vf3, vf3, vf0w; vmulx.x vf11, vf11, vf3x; sqc2 vf1, 0x0(%2); sqc2 vf2, 0x10(%2); sqc2 vf11, 0x20(%2); sw $1, 0x30(%2); addi %2, %2, 0x40; addi %0, %0, 0x10; addi %1, %1, 0x10; addi %3, %3, -1; bne $0, %3, 1b; nop; .set at; .set reorder"
        : "+r"(interpolant), "+r"(width), "+r"(particle), "+r"(count)
        : "r"(&m_position), "r"(&m_lastPosition), "r"(&m_jetVelocity), "r"(&m_lastJetVelocity), "r"(&m_x220),
          "r"(&m_x210), "r"(&params), "r"(&zero)
        : "memory");
    static int currentUV = 0;
    vu->m_v50[0].x = m_particleManager->m_frames[currentUV].m_uv[0];
    vu->m_v50[0].y = m_particleManager->m_frames[currentUV].m_uv[1];
    vu->m_v50[1].x = m_particleManager->m_frames[currentUV].m_uv[4];
    vu->m_v50[1].y = m_particleManager->m_frames[currentUV].m_uv[5];
    currentUV++;
    if (currentUV >= 4)
        currentUV = 0;
    vu->UpdateParticlePackets(0, vu->m_dirty, 2);
    return 1;
}


// ---- src/JetParticleEmitter/Tick.cpp

extern JetskiGame *LpGame;

int JetParticleEmitter::Tick(float dt)
{
    if (LpGame->m_paused)
        return 1;
    if (m_reset) {
        m_reset = 0;
        m_lastPosition = m_position;
        return 1;
    }
    if (!m_particlesEnabled) {
        m_x1B0 += dt;
        m_lastPosition = m_position;
        m_x1A4 = 0;
        return 1;
    }
    if (!m_x1A4)
        m_x1A4 = 1;
    FasterGenerateParticles(dt);
    return 1;
}


// ---- src/JetParticleEmitter/UpdateVelocity.cpp

void JetParticleEmitter::UpdateVelocity(_D3DVECTOR velocity)
{
    m_lastJetVelocity = m_jetVelocity;
    m_jetVelocity = velocity;
}


// ---- src/JetskiRollControlModel/Create.cpp (SetJet)

void JetParticleEmitter::SetJet()
{
    m_x234 = 0;
    m_x24C = 1;
}


// ---- src/JetParticleEmitter/dtor.cpp

JetParticleEmitter::~JetParticleEmitter()
{
}


// ---- src/sinit/JetskiParticleEmittersJet.cpp
// Static initializer of JetskiParticleEmittersJet.cpp: the axis vectors and the
// jet width offsets.
