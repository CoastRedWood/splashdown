// Smoker.cpp  @ 0x002A0EF0 (text 0x2A0EF0-0x2A1660)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's Smoker.cpp as one translation unit: Smoker and
// SmogParticleEmitter, then the static initializer (the axis vectors). The
// rest of the tu_map row is JetskiInGameOverlay.cpp (0x2A1660-0x2AF930).



#include "GameObject/GameObject.h"
#include "d3dtypes.h"
#include "SoultreeObject/SoultreeObject.h"
#include "SmogParticleEmitter/SmogParticleEmitter.h"
#include "ParticleEmitter/ParticleEmitter.h"
#include "ParticleManager/ParticleManager.h"
#include "Particle/Particle.h"
#include "Smoker/Smoker.h"
#include "sinit/AxisVectors.h"

extern "C" int rand();

// inline_depth is TU-wide (SmogParticleEmitter::Tick's).
#pragma inline_depth(4)

// ---- src/Smoker/ctor.cpp
Smoker::Smoker(int enabled) : GameObject(enabled)
{
    m_object = 0;
    m_emitter = 0;
}


// ---- src/Smoker/dtor.cpp


Smoker::~Smoker()
{
}


// ---- src/Smoker/Create.cpp


GameObject *Smoker::Create(SoultreeObject *object, _D3DVECTOR offset, _D3DVECTOR velocity, float size, float growth,
                           float variance)
{
    m_object = object;
    m_offset = offset;
    m_emitter = new SmogParticleEmitter(1);
    m_emitter->Create(0, 0, 0);
    m_emitter->SetVelocity(velocity);
    m_emitter->SetSize(size);
    m_emitter->SetGrowth(growth);
    m_emitter->SetVarianceFactor(variance);
    AddChild(m_emitter, -1);
    m_emitter->m_position = m_object->TransformVectorLocalToWorld(&m_offset);
    m_emitter->EnableParticles();
    return GameObject::Create();
}


// ---- src/Smoker/Tick.cpp
int Smoker::Tick(float dt)
{
    m_emitter->UpdatePosition(m_object->TransformVectorLocalToWorld(&m_offset));
    return GameObject::Tick(dt);
}


// ---- src/Smoker/SetParticleManager_002A1180.cpp


int Smoker::SetParticleManager(ParticleManager *manager)
{
    m_emitter->SetParticleManager(manager);
    return 1;
}


// ---- src/SmogParticleEmitter/ctor.cpp


SmogParticleEmitter::SmogParticleEmitter(int enabled) : ParticleEmitter(enabled)
{
    m_particleLife = 4000.0f;
    m_flags = 46;
    m_color = 0x323232;
    m_velocity.y = 50.0f;
    m_velocity.x = 30.0f;
    m_velocity.z = 0.0f;
    m_lastIndex = 13;
    m_firstIndex = 0;
    m_x1A0 = 0;
}


// ---- src/SmogParticleEmitter/dtor.cpp


SmogParticleEmitter::~SmogParticleEmitter()
{
}


// ---- src/SmogParticleEmitter/SetVelocity_audit.cpp
// Retail's sqrt is an inline-asm helper: sqrt.s into $f8, stored through a
// pointer to a stack temporary.
inline float Sqrt(float x)
{
    float r;
    asm("sqrt.s $f8, %1; swc1 $f8, 0(%0)" : : "r"(&r), "f"(x) : "$f8", "memory");
    return r;
}


void SmogParticleEmitter::SetVelocity(_D3DVECTOR velocity)
{
    m_velocity = velocity;
    m_rate = Sqrt(velocity.x * velocity.x + velocity.y * velocity.y + velocity.z * velocity.z);
    m_particleLife *= 20.0f / m_rate;
}


// ---- src/Smoker/SmogSetSize.cpp

void SmogParticleEmitter::SetSize(float size)
{
    m_initialSize = m_x190 = m_size = size;
}


// ---- src/Smoker/SmogSetGrowth.cpp

void SmogParticleEmitter::SetGrowth(float growth) { m_growth = growth; }


// ---- src/Smoker/SmogSetVarianceFactor.cpp

void SmogParticleEmitter::SetVarianceFactor(float factor) { m_varianceFactor = factor; }


// ---- src/SmogParticleEmitter/Tick.cpp


// velocity * 0.05f, written vector-first (d3dtypes.h's operator* multiplies
// scalar-first).
static inline _D3DVECTOR Scale(const _D3DVECTOR &v, float s)
{
    return _D3DVECTOR(v.x * s, v.y * s, v.z * s);
}

static inline int HasRoom(ParticleManager *m)
{
    return m->m_numParticles < m->m_maxParticles;
}

static inline float RandomUnit()
{
    return 4.65661287e-10f * rand();    // 1 / 2^31
}

int SmogParticleEmitter::Tick(float dt)
{
    if (ParticleEmitter::Tick(dt))
        return 1;
    float count;
    float rate = m_rate;
    _D3DVECTOR velocity = m_velocity;
    count = 0.035f * (rate / 20.0f) / (0.45f * (m_initialSize + m_x190));
    _D3DVECTOR r(RandomUnit(), RandomUnit(), RandomUnit());
    float rx, ry, rz, dx, dy, dz;
    r *= m_varianceFactor;
    rx = r.x;
    ry = r.y;
    rz = r.z;
    dx = rx * velocity.x;
    dz = rz * velocity.z;
    dy = ry * velocity.y;
    if (dx == 0.0f)
        dx = rx;
    if (dy == 0.0f)
        dy = ry;
    if (dz == 0.0f)
        dz = rz;
    velocity.x += dx;
    velocity.y += dy;
    velocity.z += dz;
    _D3DVECTOR position = m_position;
    _D3DVECTOR step = Scale(velocity, 0.05f);
    for (int i = 0; i < count; i++) {
        ParticleManager *manager = m_particleManager;
        if (HasRoom(manager)) {
            Particle *p = manager->m_particles[manager->m_numParticles];
            p->m_age = 0;
            p->m_position = position;
            position += step;
            p->m_frame = m_x1A0;
            p->m_size = m_size;
            p->m_lifetime = m_particleLife;
            p->m_oneOverLifetime = 1.0f / p->m_lifetime;
            p->m_rise = m_growth;
            p->m_flags = m_flags;
            p->m_x4C = m_color;
            p->m_velocity = velocity;
            m_particleManager->m_numParticles++;
        } else {
            return 1;
        }
        m_x1A0++;
        if (m_x1A0 >= m_lastIndex)
            m_x1A0 = m_firstIndex;
    }
    return 1;
}


// ---- src/SmogParticleEmitter/ReloadFromFile.cpp
int SmogParticleEmitter::ReloadFromFile()
{
    return 0;
}
