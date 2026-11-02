// JetskiParticleEmittersDrip.cpp  @ 0x003D9DB0 (text 0x3D9DB0-0x3DA870)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's JetskiParticleEmittersDrip.cpp as one translation unit: an
// unanchored TU at the end of JetskiRollControlModel.cpp's tu_map row,
// DripParticleEmitter's seven functions, then the static initializer (the
// .ctor slot after JetskiParticleEmittersJet.cpp's), which constructs the
// axis vectors and DripWidthOffsets. `make link` places the object's data as
// well as its code (tools/link_c.py).



#include "DripParticleEmitter/DripParticleEmitter.h"
#include "ParticleEmitter/ParticleEmitter.h"
#include "ParticleManager/ParticleManager.h"
#include "ParticleVUPhysics/ParticleVUPhysics.h"
#include "LightManager/LightManager.h"
#include "LightEmitter/LightEmitter.h"
#include "JetskiGame/JetskiGame.h"
#include "sinit/AxisVectors.h"

// ---- src/DripParticleEmitter/ctor.cpp

extern "C" int rand();

// Per-particle random width offsets, shared by all drips and filled in by
// the first one constructed.
// (InterpolantsCalculated is JetskiParticleEmitters.cpp's; DripWidthOffsets is
// constructed by the static initializer.)
int DripWidthOffsetsCalculated;
_D3DVECTOR DripWidthOffsets[250];
extern int InterpolantsCalculated;

static inline float RandomUnit() { return 4.656613e-10f * rand(); }

DripParticleEmitter::DripParticleEmitter(int enabled) : ParticleEmitter(enabled)
{
    m_emitRate = 240.0f;
    m_x1F0 = 0;
    m_dripPosition = _D3DVECTOR(0.0f, 0.0f, 0.0f);
    m_direction = _D3DVECTOR(0.0f, 0.0f, 0.0f);
    m_lastDripPosition = m_dripPosition;
    m_lastDirection = m_direction;
    m_x240 = 1.0f;
    m_x244 = 1.0f / m_x240;
    m_x24C = 96.0f;
    m_x250 = 1.0f;
    m_x284 = 0;
    m_width = 2.0f;
    if (!DripWidthOffsetsCalculated) {
        for (int i = 0; i < 250; i++) {
            DripWidthOffsets[i].x = 2.0f * (m_width * RandomUnit()) - m_width;
            DripWidthOffsets[i].y = 2.0f * (m_width * RandomUnit()) - m_width;
            DripWidthOffsets[i].z = 2.0f * (m_width * RandomUnit()) - m_width;
            DripWidthOffsets[i].w = 2.0f * (m_width * RandomUnit()) - m_width;
        }
        // Retail sets the bow spray's flag, not DripWidthOffsetsCalculated,
        // so every drip refills the table.
        InterpolantsCalculated = 1;
    }
}


// ---- src/DripParticleEmitter/FasterGenerateParticles.cpp
#pragma fast_fptosi on

extern LightManager *LpLightManager;
extern _D3DVECTOR Interpolants[2048];

struct ParticleFrame {
    float m_uv[8];                      // four corners
};

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

int DripParticleEmitter::FasterGenerateParticles(float dt)
{
    static int BowSprayTick2 = -1; // profiler ID (unused; named as in BowSprayParticleEmitter)
    _D3DVECTOR p0 = m_dripPosition;
    _D3DVECTOR d0 = m_direction - m_dripPosition;
    _D3DVECTOR d1 = m_lastDripPosition - m_dripPosition;
    _D3DVECTOR d2 = m_lastDirection - m_direction;
    float speed = Fmax(Fabs(m_x260.x), Fabs(m_x260.z));
    m_particleManager->m_vuPhysics->SetVelocityRange(speed, m_x260.y, speed);
    if (!m_dripping || m_x248 > m_x240)
        return 1;
    float fade = 1.0f - m_x248 * m_x244;
    float alpha = fade * m_x24C;
    float size = 500.0f * fade;
    m_particleManager->m_vuPhysics->SetPacketAlphaRange(0.0f, alpha);
    m_x248 += dt;
    int count = m_emitRate * dt;
    ParticleVUPhysics *physics = m_particleManager->m_vuPhysics;
    if (physics && !physics->m_18)
        physics->Initialize((TextureMap *)m_particleManager->m_texture, 900, 12, 1000);
    if (!physics || !physics->m_18)
        return 1;
    LightEmitter *light = LpLightManager->FindLight((LightEmitterType)3);
    if (light) {
        float r = light->m_ambient.r;
        float g = light->m_ambient.g;
        float b = light->m_ambient.b;
        LightEmitter *sun = LpLightManager->FindLight((LightEmitterType)2);
        if (sun) {
            r += 0.5f * sun->m_diffuse.r;
            g += 0.5f * sun->m_diffuse.g;
            b += 0.5f * sun->m_diffuse.b;
        }
        *(_D3DVECTOR *)&physics->m_v40.x = _D3DVECTOR(r, g, b);
    }
    physics->m_v90.y = -100.0f;
    physics->m_v30.y = -24.1500015f;    // retail's constant is 0xC1C13334, one ulp past -24.15f
    if (!count)
        return 1;
    VUParticle *dst = &physics->m_particles[physics->m_dirty];
    physics->m_dirty += count;
    if (count + m_x1F0 >= 1024)
        m_x1F0 = 0;
    m_x1F0 += count;
    if (count + m_x284 >= 250)
        m_x284 = 0;
    m_x284 += count;
    float *interp = &Interpolants[m_x1F0].x;
    float *width = &DripWidthOffsets[m_x284].x;
    _D3DVECTOR scale;
    scale.x = 1000.0f * dt;
    scale.y = size;
    scale.z = 1.0f / size;
    scale.w = m_x250;
    int zero = 0;
    // Emit count particles on VU0. The operand list order is what gives
    // retail's registers (width $v1, positions $a0-$a2, interpolant $a3, ...).
    asm("lqc2 vf2, 0x0(%6); lqc2 vf5, 0x0(%1); lqc2 vf11, 0x0(%2); vsub.xyzw vf6, vf11, vf5; lqc2 vf12, 0x0(%3); vsub.xyzw vf7, vf12, vf5; lqc2 vf13, 0x0(%5); vsub.xyzw vf8, vf13, vf11; lqc2 vf9, 0x0(%9); lw $1, 0x0(%8); _dripLoop: lqc2 vf3, 0x0(%4); lqc2 vf4, 0x0(%0); lqc2 vf14, 0x0(%7); vmulaz.xyz ACC, vf8, vf3z; vmaddw.xyz vf1, vf7, vf3w; vmulax.xyz ACC, vf1, vf3x; vmulaz.xyz ACC, vf9, vf4z; vmaddx.xyz vf1, vf6, vf3x; vadd.xyz vf1, vf1, vf5; vsubw.x vf3, vf3, vf0w; vmulx.x vf14, vf14, vf3x; sqc2 vf1, 0x0(%10); sqc2 vf2, 0x10(%10); sqc2 vf14, 0x20(%10); sw $1, 0x30(%10); addi %10, %10, 0x40; addi %4, %4, 0x10; addi %0, %0, 0x10; addi %11, %11, -1; .set noreorder; bne $0, %11, _dripLoop; nop; .set reorder"
        : : "r"(width), "r"(&m_dripPosition), "r"(&m_direction), "r"(&m_lastDripPosition), "r"(interp), "r"(&m_lastDirection), "r"(&m_x260), "r"(&scale), "r"(&zero), "r"(&m_x270), "r"(dst), "r"(count) : "memory");
    physics->m_v50[0].x = m_particleManager->m_frames[4].m_uv[0];
    physics->m_v50[0].y = m_particleManager->m_frames[4].m_uv[1];
    physics->m_v50[1].x = m_particleManager->m_frames[4].m_uv[4];
    physics->m_v50[1].y = m_particleManager->m_frames[4].m_uv[5];
    physics->UpdateParticlePackets(0, physics->m_dirty, 2);
    return 1;
}


// ---- src/DripParticleEmitter/Tick.cpp

extern JetskiGame *LpGame;

int DripParticleEmitter::Tick(float dt)
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


// ---- src/DripParticleEmitter/UpdatePosition.cpp

void DripParticleEmitter::UpdatePosition(_D3DVECTOR position, _D3DVECTOR direction)
{
    m_lastDripPosition = m_dripPosition;
    m_lastDirection = m_direction;
    m_dripPosition = position;
    m_direction = direction;
}


// ---- src/DripParticleEmitter/SetDrip.cpp

void DripParticleEmitter::SetDrip()
{
    if (!m_dripping) {
        m_dripping = 1;
        m_x248 = 0;
    }
}


// ---- src/JetskiRollControlModel/Create.cpp (StopDrip)

void DripParticleEmitter::StopDrip()
{
    m_dripping = 0;
}


// ---- src/DripParticleEmitter/dtor.cpp

DripParticleEmitter::~DripParticleEmitter()
{
}


// ---- src/sinit/JetskiParticleEmittersDrip.cpp
// Static initializer of JetskiParticleEmittersDrip.cpp: the axis vectors and the
// drip width offsets.
