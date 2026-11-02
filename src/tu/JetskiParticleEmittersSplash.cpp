// JetskiParticleEmittersSplash.cpp  @ 0x003D8590 (text 0x3D8590-0x3D9130)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's JetskiParticleEmittersSplash.cpp as one translation unit: an
// unanchored TU in JetskiRollControlModel.cpp's tu_map row,
// SplashParticleEmitter's five functions, then the static initializer (the
// .ctor slot after JetskiRollControlModel.cpp's), which constructs the
// axis vectors and RandomDirection. `make link` places the object's data
// as well as its code (tools/link_c.py).


// SlowGenerateParticles' per-function file set it; TU-wide.
#pragma inline_depth(6)

#include "SplashParticleEmitter/SplashParticleEmitter.h"
#include "ParticleManager/ParticleManager.h"
#include "ParticleVUPhysics/ParticleVUPhysics.h"
#include "LightManager/LightManager.h"
#include "LightEmitter/LightEmitter.h"
#include "JetskiGame/JetskiGame.h"
#include "sinit/AxisVectors.h"

// ---- src/SplashParticleEmitter/ctor.cpp

extern "C" int rand();
float FastCos(float);
float FastSin(float);

// Per-particle random horizontal directions, shared by all splashes and
// filled in by the first one constructed.
// (Constructed by the static initializer.)
int DirectionsCalculated;
_D3DVECTOR RandomDirection[1024];

SplashParticleEmitter::SplashParticleEmitter(int enabled) : ParticleEmitter(enabled)
{
    m_emitRate = 0.0f;
    m_x1F0 = 0;
    m_splashPosition = ZeroVector;
    m_splashVelocity = ZeroVector;
    m_elapsed = 0;
    m_duration = 0.0f;
    m_x23C = 0;
    if (!DirectionsCalculated) {
        for (int i = 0; i < 1024; i++) {
            float angle = 6.28318548f * (4.656613e-10f * rand());
            float r = 4.656613e-10f * rand();
            float c = FastCos(angle);
            RandomDirection[i].x = c * (0.9f + 0.1f * r);
            RandomDirection[i].y = 0.0f;
            r = 4.656613e-10f * rand();
            float s = FastSin(angle);
            RandomDirection[i].z = s * (0.9f + 0.1f * r);
            RandomDirection[i].w = 0.0f;
        }
        DirectionsCalculated = 1;
    }
    m_x1F4 = 0;
}


// ---- src/SplashParticleEmitter/dtor.cpp

SplashParticleEmitter::~SplashParticleEmitter()
{
}


// ---- src/SplashParticleEmitter/InitiateSplash.cpp

void SplashParticleEmitter::InitiateSplash(_D3DVECTOR position, _D3DVECTOR velocity)
{
    float fall = -velocity.y;
    velocity.y = 0.0f;
    m_splashPosition = position;
    m_splashVelocity = 0.5f * velocity;
    m_elapsed = 0;
    static float duration = 0.25f;
    m_duration = duration;
    m_oneOverDuration = 1.0f / m_duration;
    m_x224 = 6.0f;
    m_x230 = 0.5f;
    m_particleLife = 1000.0f;
    m_x23C = 1;
    m_x238 = m_x230 * m_x224;
    m_x234 = m_x224;
    m_x240 = 1.0f;
    static float maxYVel = 10.0f;
    static float oneOverMaxYVel = 1.0f / maxYVel;
    if (fall > maxYVel)
        fall = maxYVel;
    fall *= oneOverMaxYVel;
    static float vert = 15.0f;
    static float horiz = 5.0f;
    m_x238 = vert * fall;
    m_x234 = horiz * fall;
    m_emitRate = 100.0f * fall;
}


// ---- src/SplashParticleEmitter/SlowGenerateParticles.cpp
// The bow spray's table (JetskiParticleEmitters.cpp).
extern _D3DVECTOR Interpolants[2048];

// One frame of the particle texture's UV animation (0x20 bytes).
struct ParticleFrame {
    float m_u0;                         // +0x00
    float m_v0;                         // +0x04
    char pad08[0x10 - 0x08];
    float m_u1;                         // +0x10
    float m_v1;                         // +0x14
    char pad18[0x20 - 0x18];
};

static inline float Sqrt(float x)
{
    float r;
    asm("sqrt.s $f8, %1; swc1 $f8, 0(%0)" : : "r"(&r), "f"(x) : "$f8", "memory");
    return r;
}

static inline float LengthSquared(const _D3DVECTOR &v)
{
    return v.x * v.x + v.y * v.y + v.z * v.z;
}

static inline float Magnitude(float x)
{
    return Sqrt(x);
}

static inline float Length(const _D3DVECTOR &v)
{
    return Magnitude(LengthSquared(v));
}

static inline float Max(float a, float b) { return a > b ? a : b; }

int SplashParticleEmitter::SlowGenerateParticles(float dt)
{
    static int SplashTick = -1;
    float count;
    float r;
    float g;
    float b;
    if (!m_x23C)
        return 1;
    count = m_emitRate * (1.0f - m_elapsed * m_oneOverDuration);
    m_elapsed += dt;
    if (m_elapsed > m_duration)
        m_x23C = 0;
    {
        float spread = m_x234 + Length(m_splashVelocity);
        m_particleManager->m_vuPhysics->SetVelocityRange(1.1f * spread, m_x238, 1.1f * spread);
    }
    m_particleManager->m_vuPhysics->SetPacketAlphaRange(64.0f, 128.0f);
    ParticleVUPhysics *vu = m_particleManager->m_vuPhysics;
    if (vu && !vu->m_18)
        vu->Initialize((TextureMap *)m_particleManager->m_texture, 900, 12, 1000);
    if (!vu || !vu->m_18)
        return 1;
    LightEmitter *light = LpLightManager->FindLight((LightEmitterType)3);
    if (light) {
        r = light->m_ambient.r;
        g = light->m_ambient.g;
        b = light->m_ambient.b;
        LightEmitter *sun = LpLightManager->FindLight((LightEmitterType)2);
        if (sun) {
            r += 0.5f * sun->m_diffuse.r;
            g += 0.5f * sun->m_diffuse.g;
            b += 0.5f * sun->m_diffuse.b;
        }
        int unused;
        float brightest = Max(r, Max(g, b));
        brightest *= 1.0f / 128.0f;
        // Retail's test of the light scale has nothing left in either arm.
        if (brightest > 1.0f)
            unused = 1;
        else
            unused = 2;
        *(_D3DVECTOR *)&vu->m_v40.x = _D3DVECTOR(r, g, b);
    }
    vu->m_v90.y = -100.0f;
    vu->m_v30.y = -32.2f;
    for (int i = 0; (float)i < count; i++) {
        if (vu->m_dirty < vu->m_numTotal) {
            float interpolant = Interpolants[m_x1F4].x;
            _D3DVECTOR dir = RandomDirection[m_x1F4];
            m_x1F4++;
            if (m_x1F4 == 1024)
                m_x1F4 = 0;
            VUParticle *p;

            p = &vu->m_particles[vu->m_dirty];
            // VUParticle keeps float[4]s (ParticleVUPhysics.h), so the 16-byte
            // vector stores go through a _D3DVECTOR cast.
            *(_D3DVECTOR *)p->m_position = m_splashPosition + m_x240 * dir;
            dir.x *= m_x234;
            dir.y = m_x238;
            dir.z *= m_x234;
            dir = dir + m_splashVelocity;
            // Written at the store (not as an earlier `interpolant = ...` statement):
            // the scheduler hoists it, and its temporaries take retail's FP registers.
            p->m_x20 = 1000.0f * (interpolant * dt - dt);
            static float size = 0.35f;
            static float growth = 2.0f;
            p->m_size = size;
            p->m_life = m_particleLife;
            p->m_oneOverLife = 1.0f / m_particleLife;
            *(_D3DVECTOR *)p->m_velocity = dir;
            p->m_growth = growth;
            vu->m_dirty++;
        } else {
            return 1;
        }
    }
    static int currentUV = 0;
    vu->m_v50[0].x = m_particleManager->m_frames[currentUV].m_u0;
    vu->m_v50[0].y = m_particleManager->m_frames[currentUV].m_v0;
    vu->m_v50[1].x = m_particleManager->m_frames[currentUV].m_u1;
    vu->m_v50[1].y = m_particleManager->m_frames[currentUV].m_v1;
    currentUV++;
    if (currentUV >= 4)
        currentUV = 0;
    vu->UpdateParticlePackets(0, vu->m_dirty, 2);
    return 1;
}


// ---- src/SplashParticleEmitter/Tick.cpp

extern JetskiGame *LpGame;

int SplashParticleEmitter::Tick(float dt)
{
    if (LpGame->m_paused)
        return 1;
    if (m_reset) {
        m_reset = 0;
        m_lastPosition = m_splashPosition;
        return 1;
    }
    if (!m_particlesEnabled) {
        m_x1B0 += dt;
        m_lastPosition = m_splashPosition;
        m_x1A4 = 0;
        return 1;
    }
    if (!m_x1A4)
        m_x1A4 = 1;
    SlowGenerateParticles(dt);
    return 1;
}


// ---- src/sinit/JetskiParticleEmittersSplash.cpp
// Static initializer of JetskiParticleEmittersSplash.cpp: the axis vectors and
// the random direction table.
