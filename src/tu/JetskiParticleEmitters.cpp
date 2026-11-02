// JetskiParticleEmitters.cpp  @ 0x002F9D70 (text 0x2F9D70-0x2FB1C0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's JetskiParticleEmitters.cpp as one translation unit, at the end of CutSceneCamera.cpp's
// tu_map row (an unanchored TU: no class is named after it): BowSprayParticleEmitter's six functions,
// then the static initializer. Its .rodata (0x4216D0-0x421790) restarts the BaseObject RTTI records,
// its compiler labels jump from CutSceneCamera.cpp's @1050 to @1948, and
// __sinit_JetskiParticleEmitters.cpp (the .ctor slot after CutSceneCamera.cpp's) constructs the axis
// vectors at 0x485BE0 that UpdateVelocity reads, then the interpolation tables. `make link` places
// the object's data as well as its code (tools/link_c.py).




// UpdateVelocity's per-function file set it; TU-wide (read at the end of the TU).
#pragma inline_depth(6)

#include "d3dtypes.h"
#include "BowSprayParticleEmitter/BowSprayParticleEmitter.h"
#include "ParticleEmitter/ParticleEmitter.h"
#include "ParticleManager/ParticleManager.h"
#include "LightManager/LightManager.h"
#include "LightEmitter/LightEmitter.h"
#include "ParticleVUPhysics/ParticleVUPhysics.h"
#include "JetskiGame/JetskiGame.h"

#include "sinit/AxisVectors.h"

// ---- src/BowSprayParticleEmitter/ctor.cpp
extern "C" int rand();


// Per-particle random blend weights and speed scales, shared by all bow
// sprays and filled in by the first one constructed.
// Constructed by the static initializer as 2048 vectors each; the ctor fills
// the first 1024.
int InterpolantsCalculated;
_D3DVECTOR Interpolants[2048];
_D3DVECTOR VelocityMultiplier[2048];

BowSprayParticleEmitter::BowSprayParticleEmitter(int enabled) : ParticleEmitter(enabled)
{
    m_emitRate = 0;
    if (!InterpolantsCalculated) {
        for (int i = 0; i < 1024; i++) {
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
    m_x1F0 = 0;
    m_forward = _D3DVECTOR(0.0f, 0.0f, 0.0f);
    m_side = _D3DVECTOR(0.0f, 0.0f, 0.0f);
    m_lastForward = _D3DVECTOR(0.0f, 0.0f, 0.0f);
    m_lastSide = _D3DVECTOR(0.0f, 0.0f, 0.0f);
    m_sprayPosition = _D3DVECTOR(0.0f, 0.0f, 0.0f);
    m_direction = _D3DVECTOR(0.0f, 0.0f, 0.0f);
    m_lastSprayPosition = m_sprayPosition;
    m_lastDirection = m_direction;
    m_velocityRange = _D3DVECTOR(0.0f, 0.0f, 0.0f);
    m_x294 = 0.5f;
    m_x290 = 0;
}


// Session shim:
// after the ctor's interpolant loop, FasterGenerateParticles loads
// m_particleManager into $v0 instead of $v1 (11 words). A zeroed image bigger
// than any heap block makes mwcc start a new head block, so the function's
// parse runs from a clean block, as in its per-function file. Unused, emits
// nothing; not part of the retail file.
struct MwccHeapImage {
    char tail[0x30000];
};
#pragma section mwccshim begin
inline void MwccHeapShim() { const MwccHeapImage image = { {0} }; }
#pragma section mwccshim end


// ---- src/BowSprayParticleEmitter/FasterGenerateParticles.cpp
#pragma fast_fptosi on


static inline float Fabs(float x)
{
    float r;
    asm("abs.s $f8, %1; swc1 $f8, 0(%0)" : : "r"(&r), "f"(x) : "$f8", "memory");
    return r;
}

static inline float Fmin(float a, float b)
{
    float r;
    asm("min.s $f8, %1, %2; swc1 $f8, 0(%0)" : : "r"(&r), "f"(a), "f"(b) : "$f8", "memory");
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

int BowSprayParticleEmitter::FasterGenerateParticles(float dt)
{
    static int BowSprayTick3 = -1;  // unused profiler ID
    ParticleVUPhysics *vu = m_particleManager->m_vuPhysics;
    if (vu && !vu->m_18)
        vu->Initialize((TextureMap *)m_particleManager->m_texture, 900, 12, 10000);
    if (!vu || !vu->m_18)
        return 1;
    VUParticle *particle = &vu->m_particles[vu->m_dirty];
    _D3DVECTOR lastRange = m_velocityRange;
    m_velocityRange.x = Fmax(Fabs(m_forward.x * m_forward.w), Fabs(m_side.x * m_side.w));
    m_velocityRange.y = Fmax(Fabs(m_forward.y), Fabs(m_side.y));
    m_velocityRange.z = Fmax(Fabs(m_forward.z * m_forward.w), Fabs(m_side.z * m_side.w));
    float x = Fmax(Fabs(m_velocityRange.x), Fabs(lastRange.x));
    float y = Fmax(Fabs(m_velocityRange.y), Fabs(lastRange.y));
    float z = Fmax(Fabs(m_velocityRange.z), Fabs(lastRange.z));
    float h = Fmax(Fmax(x, z), 1.0f);
    // The assignments inside the argument list are what put the manager's
    // load ahead of y's in retail's schedule.
    float v;
    m_particleManager->m_vuPhysics->SetVelocityRange(v = 1.5f * h, y = Fmax(y, 1.0f), 1.5f * h);
    float life = 2.0f * (1000.0f * (0.01552795f * y)) + 0.5f / y;
    if (m_x290) {
        ParticleVUPhysics *physics = m_particleManager->m_vuPhysics;
        life = 0.4f * life;
        physics->SetPacketAlphaRange(0.0f, 128.0f);
    } else {
        m_particleManager->m_vuPhysics->SetPacketAlphaRange(128.0f, 64.0f);
    }
    vu->m_v90.y = m_sprayPosition.y - 0.35f;
    int count = m_emitRate * dt;
    if (!count)
        return 1;
    vu->m_dirty += count;
    m_x1F0 += count;
    if (count + m_x1F0 >= 0x400)
        m_x1F0 = 0;
    float *interpolant = &Interpolants[m_x1F0].x;
    float *multiplier = &VelocityMultiplier[m_x1F0].x;
    _D3DVECTOR params;
    params.x = 1000.0f * dt;
    params.y = life;
    params.z = 1.0f / life;
    params.w = m_x294;
    static int growth = 0;
    // Named pointers: declared in member order (their registers), assigned in
    // retail's order.
    _D3DVECTOR *position, *direction, *lastPosition, *lastDirection, *forward, *side, *lastForward, *lastSide;
    position = &m_sprayPosition;
    asm(".set reorder");    // emits nothing; splits the schedule as retail's is split
    direction = &m_direction;
    forward = &m_forward;
    side = &m_side;
    lastPosition = &m_lastSprayPosition;
    lastDirection = &m_lastDirection;
    lastForward = &m_lastForward;
    lastSide = &m_lastSide;
    // Emit count particles on VU0 (hand-written loop; `volatile` keeps mwcc
    // from rescheduling it).
    asm volatile(".set noreorder; .set noat; lqc2 vf9, 0x0(%7); lqc2 vf10, 0x0(%8); lqc2 vf5, 0x0(%3); lqc2 vf11, 0x0(%4); vsub.xyzw vf6, vf11, vf5; lqc2 vf12, 0x0(%5); vsub.xyzw vf7, vf12, vf5; lqc2 vf13, 0x0(%6); vsub.xyzw vf8, vf13, vf11; lqc2 vf15, 0x0(%9); lqc2 vf16, 0x0(%10); lqc2 vf17, 0x0(%12); lw $1, 0x0(%13); nop; 1: lqc2 vf3, 0x0(%0); lqc2 vf14, 0x0(%11); vmulaz.xyzw ACC, vf10, vf3z; vmaddw.xyzw vf2, vf9, vf3w; vmulaz.xyzw ACC, vf16, vf3z; vmaddw.xyzw vf18, vf15, vf3w; vmulay.xyzw ACC, vf2, vf3y; vmaddx.xyzw vf2, vf18, vf3x; vmul.xz vf18, vf2, vf2; vaddz.x vf18, vf18, vf18z; vrsqrt Q, vf0w, vf17x; vwaitq; vmulq.xz vf2, vf2, Q; vmulw.xz vf2, vf2, vf2w; vmul.xyz vf2, vf2, vf17; vmulaz.xyz ACC, vf8, vf3z; vmaddw.xyz vf1, vf7, vf3w; vmulax.xyz ACC, vf1, vf3x; vmaddz.xyz vf1, vf6, vf3z; vadd.xyz vf1, vf1, vf5; vsubw.x vf3, vf3, vf0w; vmulx.x vf14, vf14, vf3x; sqc2 vf1, 0x0(%1); sqc2 vf2, 0x10(%1); sqc2 vf14, 0x20(%1); sw $1, 0x30(%1); addi %1, %1, 0x40; addi %0, %0, 0x10; addi %2, %2, -1; bne $0, %2, 1b; nop; .set at; .set reorder"
        : "+r"(interpolant), "+r"(particle), "+r"(count)
        : "r"(position), "r"(direction), "r"(lastPosition), "r"(lastDirection),
          "r"(forward), "r"(side), "r"(lastForward), "r"(lastSide), "r"(&params), "r"(multiplier), "r"(&growth)
        : "memory");
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
        r = Fmin(128.0f, r);
        g = Fmin(128.0f, g);
        b = Fmin(128.0f, b);
        *(_D3DVECTOR *)&vu->m_v40.x = _D3DVECTOR(r, g, b);
    }
    vu->m_v30.y = -64.4f;
    vu->m_v50[0].x = m_particleManager->m_frames[0].m_uv[0];
    vu->m_v50[0].y = m_particleManager->m_frames[0].m_uv[1];
    vu->m_v50[1].x = m_particleManager->m_frames[0].m_uv[4];
    vu->m_v50[1].y = m_particleManager->m_frames[0].m_uv[5];
    vu->UpdateParticlePackets(0, vu->m_dirty, 2);
    return 1;
}


#pragma fast_fptosi reset


// ---- src/BowSprayParticleEmitter/Tick.cpp

extern JetskiGame *LpGame;

int BowSprayParticleEmitter::Tick(float dt)
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


// ---- src/BowSprayParticleEmitter/UpdateVelocity.cpp

static inline float Sqrt(float x)
{
    float r;
    asm("sqrt.s $f8, %1; swc1 $f8, 0(%0)" : : "r"(&r), "f"(x) : "$f8", "memory");
    return r;
}

static inline float RSqrt(float x)
{
    float r;
    asm("rsqrt.s $f8, %0, %1; swc1 $f8, 0(%2)" : : "f"(1.0f), "f"(x), "r"(&r) : "$f8", "memory");
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

static inline void Normalize(_D3DVECTOR &v)
{
    float len2 = v.x * v.x + v.y * v.y + v.z * v.z;
    if (len2 == 0.0f) {
        v = ZeroVector;
    } else {
        float r = RSqrt(len2);
        v.x *= r;
        v.y *= r;
        v.z *= r;
    }
}

// Works out the spray's forward and sideways velocities and the emit rate
// from the boat's speed and turn rate.
void BowSprayParticleEmitter::UpdateVelocity(_D3DVECTOR heading, _D3DVECTOR side, _D3DVECTOR position, float speed,
                                             float turn)
{
    heading.y = 0;
    m_lastForward = m_forward;
    m_lastSide = m_side;
    m_forward = heading;
    static float OneOverMaxHeightSpeed = 1.0f / 42.0f;
    static float OneOverMaxSpeed = 1.0f / 130.0f;
    static float OneOverMaxSideSpeed = 1.0f / 130.0f;
    static float forwardScalar = 10.0f;
    static float sideScalar = 5.0f;
    static float additionalSideScalar = 5.0f;
    static float heightScalar = 13.0f;
    static float sideSpeedFalloff = 44.0f;
    static float oneOverSpeedFalloff = 1.0f / 44.0f;
    // An emptied if/else: retail keeps the compare (and nothing else).
    if (speed < sideSpeedFalloff) {
    } else {
    }
    m_side = position + (speed * OneOverMaxSideSpeed * side) * sideScalar;
    m_side.y = 0;
    float sideSpeed = Length(m_side);
    m_side = m_side / sideSpeed;
    Normalize(m_side);
    m_side.w = sideSpeed;
    static float minimumForwardSpeed = 1.0f;
    m_forward.w = minimumForwardSpeed + (speed + forwardScalar * (speed * OneOverMaxSpeed));
    static float minimumSideSpeed = 1.0f;
    static float sideSpeedFinalScaler = 0.1f;
    m_forward.y = heightScalar * Fmin(speed * OneOverMaxHeightSpeed, 1.0f) + 0.5f * Fmax(0.5f * (turn * heightScalar), 0.0f);
    m_side.y = heightScalar * Fmin(speed * OneOverMaxHeightSpeed, 1.0f) + Fmax(0.5f * (turn * heightScalar), 0.0f);
    m_forward = m_side;
    if (side.x == 0.0f && side.y == 0.0f && side.z == 0.0f)
        m_side = ZeroVector;
    static float oneOverEmitRateMaxSpeed = 0.02f;
    static float basicEmitRate = 250.0f;
    static float additionalEmitRate = 1200.0f;
    static float oneOverMaxTurnRate = 0.5f;
    static float maxAngularRate = 2.0f;
    if (turn > maxAngularRate)
        turn = maxAngularRate;
    else if (turn < -maxAngularRate)
        turn = -maxAngularRate;
    static float minimumSpeed = 5.0f;
    static float oneOverMinimumSpeed = 0.2f;
    if (speed < minimumSpeed)
        turn = oneOverMinimumSpeed * (turn * speed);
    m_emitRate = speed * (oneOverEmitRateMaxSpeed * (oneOverMaxTurnRate * (additionalEmitRate * turn))) +
                 speed * (basicEmitRate * oneOverEmitRateMaxSpeed);
    if (m_emitRate < 0.0f)
        m_emitRate = 0;
}


// ---- src/BowSprayParticleEmitter/UpdatePosition.cpp

void BowSprayParticleEmitter::UpdatePosition(_D3DVECTOR position, _D3DVECTOR direction)
{
    m_lastSprayPosition = m_sprayPosition;
    m_lastDirection = m_direction;
    m_sprayPosition = position;
    m_direction = direction;
}


// ---- src/BowSprayParticleEmitter/dtor.cpp

BowSprayParticleEmitter::~BowSprayParticleEmitter()
{
}


// ---- src/sinit/JetskiParticleEmitters.cpp
// Static initializer of JetskiParticleEmitters.cpp: the axis vectors and the
// particle interpolation tables.
