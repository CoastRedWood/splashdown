// ParticleEmitter.cpp  @ 0x002FB1C0 (text 0x2FB1C0-0x2FBFD0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's ParticleEmitter.cpp as one translation unit: ParticleEmitter's
// methods and the static initializer. `make link` places the object's data
// as well as its code (tools/link_c.py). The rest of the tu_map row is TUs
// without an initializer, left per-function: SearchMethod.cpp (with
// ContainerList<GameObject *>), SearchNode.cpp, PSXMemoryCard.cpp and
// SearchPlacement.cpp (.rodata: ParticleEmitter.cpp's strings and RTTI end
// at 0x421A00; a data-only TU's CD file list follows, then SearchMethod's
// RTTI at 0x42F8F8, SearchNode's, PSXMemoryCard's strings).



#include "GameObject/GameObject.h"
#include "d3dtypes.h"
#include "ParticleEmitter/ParticleEmitter.h"
#include "SoultreeObject/SoultreeObject.h"
#include "Game/Game.h"
#include "ParameterBlock/ParameterBlock.h"
#include "soultree_assert_block.h"
// The axis vectors every Soultree TU defines (and __sinit constructs).
#include "sinit/AxisVectors.h"

extern "C" int rand();
void PSXprintf(const char *fmt, ...);
struct FILE_HANDLE_STRUCT;
FILE_HANDLE_STRUCT *FOPEN(char *name, const char *mode);
void FCLOSE(FILE_HANDLE_STRUCT *file);

extern Game *LpGame;

// ---- src/ParticleEmitter/ctor.cpp




ParticleEmitter::ParticleEmitter(int enabled) : GameObject(enabled)
{
    m_position = _D3DVECTOR(0.0f, 0.0f, 0.0f);
    m_lastPosition = m_position;
    m_offset = _D3DVECTOR(0.0f, 0.0f, 0.0f);
    m_velocity = _D3DVECTOR(0.0f, 0.0f, 0.0f);
    m_reset = 1;
    m_particleManager = 0;
    m_owner = 0;
    m_particleLife = 4000.0f;
    m_growth = 6.0f;
    m_initialSize = 1.0f;
    m_emitRate = 10.0f;
    m_x190 = 0;
    m_particlesEnabled = 0;
    m_randomMirrorX = 0;
    m_randomMirrorY = 0;
    m_x1A0 = 0;
    m_x1A4 = 0;
    m_color = 0xFFFFFF;
    m_flags = 0;
    m_x1B0 = 0;
    m_ageAtBirth = 0;
    m_firstIndex = 0;
    m_lastIndex = 0;
    m_group = -1;
    m_mode = 0;
    m_startDelay = 0;
    m_delayTime = 0;
    m_timer = 0;
    m_onTimeMin = 0;
    m_onTimeMax = 0;
    m_offTimeMin = 0;
    m_offTimeMax = 0;
}


// ---- src/ParticleEmitter/dtor.cpp


ParticleEmitter::~ParticleEmitter()
{
}


// ---- src/ParticleEmitter/Tick.cpp
int ParticleEmitter::Tick(float dt)
{
    if (LpGame->m_paused)
        return 1;
    if (m_reset) {
        m_reset = 0;
        m_lastPosition = m_position;
        return 1;
    }
    if (m_owner) {
        m_lastPosition = m_position;
        m_owner->GetPosition(&m_position);
        m_position += m_owner->RotateVectorLocalToWorld(&m_offset);
    }
    if (m_startDelay > 0.0f) {
        if (m_particlesEnabled)
            m_startDelay -= dt;
        return 1;
    }
    if (m_mode == 1) {
        if (m_particlesEnabled) {
            if (m_timer > 0.0f) {
                m_timer -= dt;
                return 0;
            }
            DisableParticles();
            return 1;
        }
        return Idle(dt);
    } else if (m_mode == 2) {
        if (m_particlesEnabled) {
            if (m_timer > 0.0f) {
                m_timer -= dt;
                if (m_timer <= 0.0f) {
                    float r = 4.656613e-10f * rand();
                float range = m_offTimeMax - m_offTimeMin;
                m_timer = m_offTimeMin + range * r;
                    DisableParticles();
                    return 1;
                }
                return 0;
            }
        } else if (m_timer > 0.0f) {
            m_timer -= dt;
            if (m_timer <= 0.0f) {
                float r = 4.656613e-10f * rand();
                float range = m_onTimeMax - m_onTimeMin;
                m_timer = m_onTimeMin + range * r;
                EnableParticles();
                m_x1B0 = 0;
                return 0;
            }
            return Idle(dt);
        }
    } else if (m_mode == 3) {
        if (m_particlesEnabled) {
            if (m_timer > 0.0f) {
                m_timer -= dt;
                if (m_timer <= 0.0f) {
                    m_timer = m_offTimeMax;
                    DisableParticles();
                    return 1;
                }
                return 0;
            }
        } else if (m_timer > 0.0f) {
            m_timer -= dt;
            if (m_timer <= 0.0f) {
                m_timer = m_onTimeMax;
                EnableParticles();
                m_x1B0 = 0;
                return 0;
            }
            return Idle(dt);
        }
    }
    if (!m_particlesEnabled)
        return Idle(dt);
    if (!m_x1A4)
        m_x1A4 = 1;
    return 0;
}


// ---- src/ParticleEmitter/Create.cpp




ParticleEmitter *ParticleEmitter::Create(ParticleManager *manager, SoultreeObject *object, _D3DVECTOR *offset)
{
    GameObject::Create();
    m_particleManager = manager;
    if (object) {
        m_owner = object;
        if (offset) {
        m_offset.x = offset->x;
        m_offset.y = offset->y;
        m_offset.z = offset->z;
        m_offset.w = offset->w;
        }
    }
    return this;
}


// ---- src/ParticleEmitter/SetParticleManager.cpp


void ParticleEmitter::SetParticleManager(ParticleManager *manager)
{
    m_particleManager = manager;
}


// ---- src/ParticleEmitter/UpdatePosition.cpp


void ParticleEmitter::UpdatePosition(_D3DVECTOR position)
{
    m_lastPosition = m_position;
    m_position = position;
}


// ---- src/ParticleEmitter/EnableParticles.cpp


void ParticleEmitter::EnableParticles()
{
    m_particlesEnabled = 1;
    m_x1B0 = 0;
}


// ---- src/ParticleEmitter/DisableParticles.cpp



void ParticleEmitter::DisableParticles()
{
    m_particlesEnabled = 0;
    m_x1A4 = 0;
    m_lastPosition.x = m_position.x;
    m_lastPosition.y = m_position.y;
    m_lastPosition.z = m_position.z;
    m_lastPosition.w = m_position.w;
    m_x1B0 = 0;
}


// ---- src/ParticleEmitter/ResetParticles.cpp
void ParticleEmitter::ResetParticles()
{
    m_startDelay = m_delayTime;
    if (m_mode == 1) {
        m_timer = m_onTimeMax;
    } else if (m_mode == 2) {
        float t = (1.0f / 2147483648.0f) * (float)rand();  // rand() / 2^31
        float lo = m_onTimeMin;
        float d = m_onTimeMax - lo;
        m_timer = lo + d * t;  // temporaries fix mul.s operand order
    } else if (m_mode == 3) {
        m_timer = m_onTimeMax;
    }
}


// ---- src/ParticleEmitter/KeyDownHandler.cpp


int ParticleEmitter::KeyDownHandler(ProcessNode *node, KeyInfo *key)
{
    return GameObject::KeyDownHandler(node, key);
}


// ---- src/ParticleEmitter/LoadFromFile.cpp
void PSXprintf(const char *fmt, ...);




int ParticleEmitter::LoadFromFile(char *name)
{
    if (name) {
        ParameterBlock block;
        if (block.OpenFile(name, 0)) {
            LoadFromFile(&block);
            return 1;
        }
        PSXprintf("Could not find emitter file %s!\n", name);
        return 0;
    }
    return 0;
}


// ---- src/ParticleEmitter/LoadFromFile_002FBA60.cpp





int ParticleEmitter::LoadFromFile(ParameterBlock *block)
{
    if (block) {
        block->ReadParameterBlock("General Info");
        block->GetParameter("ParticleGroup", -1, &m_group);
#line 305 "ParticleEmitter.cpp"
        ASSERT(m_group > -1, "ParticleEmitter::LoadFromFile: Invalid Particle Group!!\n");
        block->GetParameter("ParticleLife", 1000.0f, &m_particleLife);
        block->GetParameter("InitialSize", 1.0f, &m_initialSize);
        block->GetParameter("Growth", 0.0f, &m_growth);
        block->GetParameter("EmitRate", 500.0f, &m_emitRate);
        block->GetParameter("AgeAtBirth", 0.0f, &m_ageAtBirth);
        m_ageAtBirth = m_ageAtBirth * m_particleLife;
        block->GetParameter("Velocity", &ZeroVector, &m_velocity);
        int mode;
        block->GetParameter("EmitMode", 0, &mode);
        m_mode = mode;
        block->GetParameter("DelayTime", 0.0f, &m_delayTime);
        m_startDelay = m_delayTime;
        if (m_mode == 1) {
            block->GetParameter("OneShotTime", 0.0f, &m_onTimeMax);
            m_timer = m_onTimeMax;
        } else if (m_mode == 2) {
            block->GetParameter("RandomMinTimeOn", 0.0f, &m_onTimeMin);
            block->GetParameter("RandomMaxTimeOn", 0.0f, &m_onTimeMax);
            block->GetParameter("RandomMinTimeOff", 0.0f, &m_offTimeMin);
            // Retail bug: the minimum is read twice and RandomMaxTimeOff never.
            block->GetParameter("RandomMinTimeOff", 0.0f, &m_offTimeMin);
            float r = 4.656613e-10f * rand();
            float range = m_onTimeMax - m_onTimeMin;
            m_timer = m_onTimeMin + range * r;
        } else if (m_mode == 3) {
            block->GetParameter("TimedTimeOn", 0.0f, &m_onTimeMax);
            block->GetParameter("TimedTimeOff", 0.0f, &m_offTimeMax);
            m_timer = m_onTimeMax;
        }
        block->ReadParameterBlock("Properties");
        int fades;
        int hasVelocity;
        int grows;
        int fadeNonLinear;
        int glows;
        int gravity;
        int lit;
        block->GetParameter("Fades", 0, &fades);
        block->GetParameter("HasVelocity", 0, &hasVelocity);
        block->GetParameter("Grows", 0, &grows);
        block->GetParameter("FadeNonLinear", 0, &fadeNonLinear);
        block->GetParameter("Glows", 0, &glows);
        block->GetParameter("Gravity", 0, &gravity);
        block->GetParameter("Lit", 1, &lit);
        block->GetParameter("RandomMirrorAboutX", 0, &m_randomMirrorX);
        block->GetParameter("RandomMirrorAboutY", 0, &m_randomMirrorY);
        if (fades)
            m_flags |= 0x2;
        else
            m_flags &= ~0x2;
        if (hasVelocity)
            m_flags |= 0x4;
        else
            m_flags &= ~0x4;
        if (grows)
            m_flags |= 0x8;
        else
            m_flags &= ~0x8;
        if (fadeNonLinear)
            m_flags |= 0x1000;
        else
            m_flags &= ~0x1000;
        if (glows)
            m_flags |= 0x2000;
        else
            m_flags &= ~0x2000;
        if (gravity)
            m_flags |= 0x20;
        else
            m_flags &= ~0x20;
        if (!lit)
            m_flags |= 0x800;
        else
            m_flags &= ~0x800;
        return 1;
    }
    return 0;
}


// ---- src/ParticleEmitter/ReloadFromFile.cpp



// Reload only if the file can still be opened.
int ParticleEmitter::ReloadFromFile()
{
    FILE_HANDLE_STRUCT *file = FOPEN(m_fileName, "r");
    if (file) {
        FCLOSE(file);
        LoadFromFile(m_fileName);
    }
    return 1;
}


// ---- src/sinit/ParticleEmitter.cpp
// Only the axis vectors from the shared header.


// Retail's RTTI block has BaseObject's and GameObject's records before
// ParticleEmitter's name (@2094-@2096, then @2097-@2098): mwcc built them as
// the bases of a class whose vtable it wrote first, pushed last on its LIFO
// list (key function defined after ParticleEmitter's). Nothing constructed that
// class, so retail's linker stripped its vtable, RTTI and functions; link_c
// leaves them out ("Section
// order").
class StrippedParticleObject : public GameObject {
public:
    virtual ~StrippedParticleObject();
};

StrippedParticleObject::~StrippedParticleObject()
{
}
