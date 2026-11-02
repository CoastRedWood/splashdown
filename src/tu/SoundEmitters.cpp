// SoundEmitters.cpp  @ 0x0034B3D0 (text 0x34B3D0-0x34DC00)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's SoundEmitters.cpp (its ASSERTs' name) as one translation unit,
// the last TU of PSXSurfaceMaterial.cpp's tu_map row: SoundListener,
// SoundEmitter, SoultreeSoundEmitter, AmbientEmitter,
// AmbientListSoundEmitter, ResponseEmitter, VehicleEmitter and the
// ContainerList<_SoundResponse *> members, then the static initializer (the
// axis vectors). Its .rodata starts with a BaseObject RTTI restart region
// (0x436940).


#include "d3dtypes.h"
#include "_DS3DBUFFER/_DS3DBUFFER.h"
#include "GameObject/GameObject.h"
#include "SoundListener/SoundListener.h"
#include "Listener/Listener.h"
#include "soultree_assert_block.h"
#include "PSXSoundInterface/PSXSoundInterface.h"
#include "PSXSoundGroup/PSXSoundGroup.h"
#include "AuralScape/AuralScape.h"
#include "SoundEmitter/SoundEmitter.h"
#include "PSXSound/PSXSound.h"
#include "SoultreeObject/SoultreeObject.h"
#include "SoultreeSoundEmitter/SoultreeSoundEmitter.h"
#include "VehicleEmitter/VehicleEmitter.h"
#include "AmbientEmitter/AmbientEmitter.h"
#include "AmbientListSoundEmitter/AmbientListSoundEmitter.h"
#include "ContainerList/ContainerList.h"
#include "ResponseEmitter/ResponseEmitter.h"
#include "ParameterBlock/ParameterBlock.h"
// The members of every ContainerList this TU calls are instantiated here:
// the <_SoundResponse *> ones are this TU's; the <PSXSound *> ones are weak
// copies of PhysicsRigidBody.cpp's row's (0x26E200), which retail's linker
// dropped.
#define CONTAINERLIST_MEMBERS_ALL
#include "ContainerList/ContainerListMembers.h"
#include "sinit/AxisVectors.h"


extern "C" void *memset(void *dst, int c, unsigned int n);
extern "C" char *strcpy(char *dst, const char *src);
extern "C" int sprintf(char *buf, const char *fmt, ...);
extern "C" int rand();
extern char AudioContentDirectory[0x104];

// inline_depth is TU-wide (SoultreeSoundEmitter::Tick's).
#pragma inline_depth(4)

// ---- src/SoundListener/ctor.cpp
SoundListener::SoundListener()
{
    m_scape = 0;
    m_listener = 0;
}


// ---- src/SoundListener/dtor.cpp

SoundListener::~SoundListener()
{
    if (m_listener) {
        m_listener->Release();
        m_listener = 0;
    }
}


// ---- src/SoundListener/Create.cpp


int SoundListener::Create(AuralScape *scape, float doppler, float rolloff)
{
#line 52 "SoundEmitters.cpp"
    ASSERT(scape, "Create: lpAuralScape == NULL");
    if (!scape)
        return 0;
    m_scape = scape;
    m_listener = m_scape->m_soundInterface->CreateListener();
    if (!m_listener)
        return 0;
    if (!SetDopplerFactor(doppler))
        return 0;
    if (!SetRolloffFactor(rolloff))
        return 0;
    if (!SetDistanceFactor(0.3048f))    // feet to metres
        return 0;
    if (!CommitDeferredSettings())
        return 0;
    return 1;
}


// ---- src/SoundListener/CommitDeferredSettings.cpp
int SoundListener::CommitDeferredSettings()
{
    if (m_listener)
        return m_listener->CommitDeferredSettings() != 0;
    return 0;
}


// ---- src/SoundListener/SetDistanceFactor.cpp
int SoundListener::SetDistanceFactor(float factor)
{
    if (m_listener)
        return m_listener->SetDistanceFactor(factor, 1) != 0;
    return 0;
}


// ---- src/SoundListener/SetDopplerFactor.cpp
int SoundListener::SetDopplerFactor(float factor)
{
    if (m_listener)
        return m_listener->SetDopplerFactor(factor, 1) != 0;
    return 0;
}


// ---- src/SoundListener/SetRolloffFactor.cpp
int SoundListener::SetRolloffFactor(float factor)
{
    if (m_listener)
        return m_listener->SetRolloffFactor(factor, 1) != 0;
    return 0;
}


// ---- src/SoundListener/GetOrientation.cpp
int SoundListener::GetOrientation(_D3DVECTOR *front, _D3DVECTOR *top)
{
    Listener *listener = m_listener;
    if (listener == 0)
        return 0;
    front->x = listener->m_front.x;
    front->y = listener->m_front.y;
    front->z = listener->m_front.z;
    top->x = listener->m_top.x;
    top->y = listener->m_top.y;
    top->z = listener->m_top.z;
    return 1;
}


// ---- src/SoundListener/SetOrientation.cpp
int SoundListener::SetOrientation(_D3DVECTOR *front, _D3DVECTOR *top)
{
    Listener *listener = m_listener;
    if (listener == 0)
        return 0;
    listener->m_front.x = front->x;
    listener->m_front.y = front->y;
    listener->m_front.z = front->z;
    listener->m_top.x = top->x;
    listener->m_top.y = top->y;
    listener->m_top.z = top->z;
    return 1;
}


// ---- src/SoundListener/GetPosition.cpp
int SoundListener::GetPosition(_D3DVECTOR *position)
{
    Listener *listener = m_listener;
    if (listener == 0)
        return 0;
    position->x = listener->m_position.x;
    position->y = listener->m_position.y;
    position->z = listener->m_position.z;
    return 1;
}


// ---- src/SoundListener/SetPosition.cpp
int SoundListener::SetPosition(_D3DVECTOR *position)
{
    Listener *listener = m_listener;
    if (listener == 0)
        return 0;
    listener->m_position.x = position->x;
    listener->m_position.y = position->y;
    listener->m_position.z = position->z;
    return 1;
}


// ---- src/SoundListener/GetVelocity.cpp
int SoundListener::GetVelocity(_D3DVECTOR *velocity)
{
    Listener *listener = m_listener;
    if (listener == 0)
        return 0;
    velocity->x = listener->m_velocity.x;
    velocity->y = listener->m_velocity.y;
    velocity->z = listener->m_velocity.z;
    return 1;
}


// ---- src/SoundListener/SetVelocity.cpp
int SoundListener::SetVelocity(_D3DVECTOR *velocity)
{
    Listener *listener = m_listener;
    if (listener == 0)
        return 0;
    listener->m_velocity.x = velocity->x;
    listener->m_velocity.y = velocity->y;
    listener->m_velocity.z = velocity->z;
    return 1;
}


// Defined between SoundListener's functions and SoundEmitter's (retail's
// .bss order).
int SoundEmitter::UsedEmitterID;

// ---- src/SoundEmitter/ctor.cpp


// 16-byte aligned here: _DS3DBUFFER's vectors sit on 16-byte boundaries.


// DirectSound 3D buffer parameters (16-byte vectors, so 0x70 bytes).


SoundEmitter::SoundEmitter(AuralScape *auralScape, PSXSoundGroup *group, short id, int enabled)
    : GameObject(enabled)
{
#line 163 "SoundEmitters.cpp"
    ASSERT(auralScape, "SoundEmitter constructor: lpAuralScape == NULL");
    memset(&m_buffer, 0, sizeof(m_buffer));
    m_system = auralScape;
    m_group = group;
    m_sound = 0;
    m_lastPosition = _D3DVECTOR(3.40282347e+38f, 3.40282347e+38f, 3.40282347e+38f);
    _D3DVECTOR *last = &m_lastPosition;
    m_buffer.vPosition = *last;
    m_flags = 0;
    m_volume = 0.0f;
    m_pitch = 100.0f;
    strcpy(m_name, "");
    m_id = id;
    m_disabled = 0;
    m_bit1 = 0;
    m_positional = 0;
    m_looped = 0;
    m_playing = 0;
    m_emitterID = UsedEmitterID;
    UsedEmitterID++;
    m_closestDistance = 0;
    m_forcePlaying = 0;
    m_enabled = 1;
    m_lastPosition = ZeroVector;
}


// ---- src/SoundEmitter/dtor.cpp


SoundEmitter::~SoundEmitter()
{
}


// ---- src/SoundEmitter/Create.cpp


struct SoundSystem;


SoundEmitter *SoundEmitter::Create(_DS3DBUFFER buffer, unsigned int flags, float volume, float pitch,
                                   int positional, int looped)
{
    if (GameObject::Create() && m_system) {
        if (!(flags & 2) || (flags & 4) || positional)
            m_positional = 1;
        m_looped = looped;
        buffer.dwSize = sizeof(_DS3DBUFFER);
        m_buffer = buffer;
        m_flags = flags;
        m_volume = volume;
        m_pitch = pitch;
        return this;
    }
    Release();
    return 0;
}


// ---- src/SoundEmitter/UpdateFromAuralscape.cpp
inline float Fabs(float x)
{
    float r;
    asm("abs.s $f8, %1; swc1 $f8, 0(%0)" : : "r"(&r), "f"(x) : "$f8", "memory");
    return r;
}

inline float Sqrt(float x)
{
    float r;
    asm("sqrt.s $f8, %1; swc1 $f8, 0(%0)" : : "r"(&r), "f"(x) : "$f8", "memory");
    return r;
}

// 16-byte aligned here: _DS3DBUFFER's vectors sit on 16-byte boundaries.


static inline float Square(float x) { return x * x; }

// The XZ-plane distance between two points.
static inline float DistanceXZ(_D3DVECTOR *a, _D3DVECTOR *b)
{
    float dx = Fabs(a->x - b->x);
    float d = dx * dx;
    float dz = Fabs(a->z - b->z);
    return Sqrt(d + dz * dz);
}


void SoundEmitter::UpdateFromAuralscape(int numListeners, SoundListener **listeners)
{
    _D3DVECTOR listener;
    float distance;
    m_closestDistance = 3.40282347e+38f;
    for (int i = 0; i < numListeners; i++) {
        listeners[i]->GetPosition(&listener);
        if (m_flags & 2) {
            distance = DistanceXZ(&m_buffer.vPosition, &listener);
        } else {
            _D3DVECTOR ahead = m_buffer.vPosition + m_buffer.vVelocity;
            distance = DistanceXZ(&ahead, &listener);
        }
        if (distance < m_closestDistance)
            m_closestDistance = distance;
    }
    m_bit1 = 0;
    if (m_flags & 1) {
        if (m_closestDistance > m_volume)
            return;
    } else if (!(m_closestDistance < 1.2f * m_buffer.flMaxDistance)) {
        return;
    }
    m_bit1 = 1;
}


// ---- src/SoundEmitter/GetActiveSound.cpp


PSXSound *SoundEmitter::GetActiveSound()
{
    return m_sound;
}


// ---- src/SoundEmitter/SetActiveSound.cpp


int SoundEmitter::SetActiveSound(PSXSound *sound)
{
    m_sound = sound;
    return 1;
}


// ---- src/SoundEmitter/IsPlaying.cpp


int SoundEmitter::IsPlaying()
{
    if (!m_sound)
        return 0;
    if (m_forcePlaying)
        return 1;
    return m_sound->IsPlaying();
}


// ---- src/SoundEmitter/UpdateEmitter.cpp


void SoundEmitter::UpdateEmitter(_DS3DBUFFER buffer)
{
    m_lastPosition = m_buffer.vPosition;
    m_buffer = buffer;
    // Retail reads m_sound here and does nothing else; whatever followed
    // was compiled out.
    if (!m_sound)
        return;
}


// ---- src/SoundEmitter/UpdateEmitter_0034C160.cpp
// Local copy of the old partial SoundEmitter.h: these use its 16-byte
// aligned _D3DVECTOR / _DS3DBUFFER, which the shared header doesn't define.
// 16-byte aligned here: _DS3DBUFFER's vectors sit on 16-byte boundaries.

// DirectSound 3D buffer parameters (16-byte vectors, so 0x70 bytes).


void SoundEmitter::UpdateEmitter(_D3DVECTOR *position, _D3DVECTOR *velocity)
{
    if (position) {
        m_lastPosition = m_buffer.vPosition;
        m_buffer.vPosition = *position;
    }
    if (velocity)
        m_buffer.vVelocity = *velocity;
    if (m_sound) {
        m_sound->SetPosition(m_buffer.vPosition);
        m_sound->SetVelocity(m_buffer.vVelocity);
    }
}


// ---- src/SoundEmitter/SetBaseVolume.cpp


int SoundEmitter::SetBaseVolume(float volume)
{
    if (m_sound)
        return m_sound->SetBaseVolume(volume);
    return 0;
}


// ---- src/SoundEmitter/GetBaseVolume.cpp


float SoundEmitter::GetBaseVolume()
{
    if (m_sound)
        return m_sound->m_baseVolume;
    return -1.0f;
}


// ---- src/SoundEmitter/UpdateSettings.cpp


int SoundEmitter::UpdateSettings(float volume, long flags, _D3DVECTOR position, int priority)
{
    if (m_sound)
        return m_sound->UpdateSettings(volume, flags, position, priority);
    return 0;
}


// ---- src/SoundEmitter/Stop.cpp


int SoundEmitter::Stop()
{
    m_playing = 0;
    if (!m_sound)
        return 0;
    if (m_forcePlaying)
        m_forcePlaying = 0;
    return m_sound->Stop(0);
}


// ---- src/SoundEmitter/LoadSound.cpp
// Local copy of the old partial SoundEmitter.h: these use its 16-byte
// aligned _D3DVECTOR / _DS3DBUFFER, which the shared header doesn't define.
// 16-byte aligned here: _DS3DBUFFER's vectors sit on 16-byte boundaries.

// DirectSound 3D buffer parameters (16-byte vectors, so 0x70 bytes).


int SoundEmitter::LoadSound(char *name, int looped)
{
    if (m_sound || m_disabled)
        return 0;
    unsigned int flags = 0;
    if (looped || (m_flags & 8))
        flags |= 0x80;
    m_sound = m_system->m_soundInterface->GetSoundFromResourceName(m_group, name, flags | 4, 3);
    if (!m_sound)
        return 0;
    if (m_flags & 4) {
        UpdateEmitter(m_buffer);
        return 1;
    }
    m_sound->SetMinMaxDistance(m_buffer.flMinDistance, m_buffer.flMaxDistance);
    UpdateEmitter(&m_buffer.vPosition, &m_buffer.vVelocity);
    return 1;
}


// ---- src/SoundEmitter/UnloadSound.cpp


// Retail clears m_sound twice (once only when the last reference went away).
void SoundEmitter::UnloadSound()
{
    if (m_sound) {
        if (!m_sound->Release())
            m_sound = 0;
    }
    m_sound = 0;
}


// ---- src/SoundEmitter/EnableEmitter.cpp

void SoundEmitter::EnableEmitter()
{
    m_enabled = 1;
}


// ---- src/SoundEmitter/DisableEmitter.cpp


void SoundEmitter::DisableEmitter()
{
    m_enabled = 0;
    Stop();
}


// ---- src/SoundEmitter/Start.cpp


int SoundEmitter::Start()
{
    if (!m_sound)
        return 0;
    if ((m_flags & 1) && m_disabled)
        return 0;
    if (m_pitch < 100.0f) {
        if (m_flags & 8) {
            if (m_playing)
                return 1;
            m_playing = 1;
        }
        if (100.0f * (4.656612873e-10f * rand()) > m_pitch)
            return 1;
    }
    if (m_flags & 1)
        m_disabled = 1;
    if (m_flags & 8)
        m_forcePlaying = 1;
    return m_sound->Play(0, 0);
}


// ---- src/SoultreeSoundEmitter/ctor.cpp
SoultreeSoundEmitter::SoultreeSoundEmitter(AuralScape *scape, PSXSoundGroup *group, short type, int enabled)
    : SoundEmitter(scape, group, type, enabled)
{
    m_target = 0;
    m_object = 0;
}


// ---- src/SoultreeSoundEmitter/dtor.cpp


SoultreeSoundEmitter::~SoultreeSoundEmitter()
{
}


// ---- src/SoultreeSoundEmitter/Create.cpp


SoultreeSoundEmitter *SoultreeSoundEmitter::Create(char *name, SoultreeObject *object, _DS3DBUFFER buffer, unsigned int flags, float a, float b, int c, int d)
{
    SoundEmitter::Create(buffer, flags, a, b, c, d);
    m_object = object;
    if (m_object)
        m_target = m_object->GetNamedObject(name);
    else
        m_target = 0;
    return this;
}


// ---- src/SoultreeSoundEmitter/Tick.cpp
// v * s with v's components on the left (d3dtypes.h's operator* puts s first).
static inline _D3DVECTOR ScaleVS(const _D3DVECTOR &v, float s)
{
    return _D3DVECTOR(v.x * s, v.y * s, v.z * s);
}



int SoultreeSoundEmitter::Tick(float dt)
{
    _D3DVECTOR position = ZeroVector;
    if (m_target)
        m_target->GetPosition(0, &position);
    else if (m_object)
        m_object->GetPosition(0, &position);
    else
        position = m_buffer.vPosition;
    m_lastPosition = m_buffer.vPosition;
    m_buffer.vPosition = position;
    if (GetActiveSound())
        GetActiveSound()->SetPosition(position);
    if (GetActiveSound() && !(m_flags & 2)) {
        m_buffer.vVelocity = ScaleVS(m_buffer.vPosition - m_lastPosition, 1.0f / dt);
        GetActiveSound()->SetVelocity(m_buffer.vVelocity);
    }
    return GameObject::Tick(dt);
}


// ---- src/SoultreeSoundEmitter/UpdateSettings.cpp


int SoultreeSoundEmitter::UpdateSettings(float volume, long flags, _D3DVECTOR position, int priority)
{
    return SoundEmitter::UpdateSettings(volume, flags, position, priority);
}


// ---- src/AmbientEmitter/ctor.cpp
AmbientEmitter::AmbientEmitter(AuralScape *scape, PSXSoundGroup *group, short type, int enabled)
    : SoundEmitter(scape, group, type, enabled)
{
    m_interval = 0;
    m_elapsed = 0;
}


// ---- src/AmbientEmitter/dtor.cpp


AmbientEmitter::~AmbientEmitter()
{
}


// ---- src/AmbientEmitter/Start.cpp


int AmbientEmitter::Start()
{
    if (!GetActiveSound())
        return 0;
    if (m_flags & 8)
        m_forcePlaying = 1;
    return GetActiveSound()->Play(0, 0);
}


// ---- src/AmbientEmitter/Tick.cpp


// Every m_interval ms, if not already playing, start with m_chance percent.
int AmbientEmitter::Tick(float dt)
{
    m_elapsed += 1000.0f * dt;
    if (m_elapsed >= m_interval && !IsPlaying()) {
        if (!IsPlaying()) {  // retail asks twice
            if (m_pitch < 100.0f) {
                if (100.0f * (4.65661287e-10f * rand()) <= m_pitch) {
                    m_elapsed = 0.0f;
                    Start();
                }
            } else {
                m_elapsed = 0.0f;
                Start();
            }
        } else {
            m_elapsed = 0.0f;
        }
    }
    return GameObject::Tick(dt);
}


// ---- src/AmbientEmitter/UpdateSettings.cpp

int AmbientEmitter::UpdateSettings(float volume, long pan, _D3DVECTOR position, int flags)
{
    return SoundEmitter::UpdateSettings(volume, pan, position, flags);
}


// ---- src/AmbientListSoundEmitter/ctor.cpp


AmbientListSoundEmitter::AmbientListSoundEmitter(AuralScape *scape, PSXSoundGroup *group, short a, int b)
    : AmbientEmitter(scape, group, a, b)
{
    m_sounds.Create(1, 0x10);
}


// ---- src/AmbientListSoundEmitter/dtor.cpp


AmbientListSoundEmitter::~AmbientListSoundEmitter()
{
    for (int i = 0; i < m_sounds.m_numItems; i++) {
        PSXSound *sound = m_sounds.GetItem(i);
        m_group->RemoveSound(sound);
        sound->Release();
    }
}


// ---- src/AmbientListSoundEmitter/Create.cpp


AmbientListSoundEmitter *AmbientListSoundEmitter::Create(_DS3DBUFFER buffer, unsigned int flags, float a, float b, int c, int d)
{
    SoundEmitter::Create(buffer, flags, a, b, c, d);
    return this;
}


// ---- src/AmbientListSoundEmitter/LoadSoundIntoList.cpp
int AmbientListSoundEmitter::LoadSoundIntoList(char *name)
{
    PSXSound *sound = LoadSound(name);
    if (sound) {
        m_sounds.AddItem(sound);
        m_group->AddSound(sound);
        SetActiveSound(sound);
        return 1;
    }
    return 0;
}


// ---- src/AmbientListSoundEmitter/LoadSound.cpp


PSXSound *AmbientListSoundEmitter::LoadSound(char *name)
{
    return m_system->m_soundInterface->GetSoundFromResourceName(m_group, name, 4, 3);
}


// ---- src/AmbientListSoundEmitter/Tick.cpp


int AmbientListSoundEmitter::Tick(float dt)
{
    m_elapsed += 1000.0f * dt;
    if (!(m_elapsed < m_interval) && !IsPlaying()) {
        if (m_pitch < 100.0f) {
            // rand() / 2^31 as a percentage.
            if (100.0f * (rand() * 4.656612873e-10f) <= m_pitch) {
                m_elapsed = 0;
                Start();
            }
        } else {
            m_elapsed = 0;
            Start();
        }
    }
    return GameObject::Tick(dt);
}


// ---- src/AmbientListSoundEmitter/Start.cpp
#pragma fast_fptosi on


int AmbientListSoundEmitter::Start()
{
    if (m_sounds.m_numItems) {
        // rand() / 2^31 scaled to the list size.
        int index = (int)((m_sounds.m_numItems - 1) * (rand() * 4.656612873e-10f));
        SetActiveSound(m_sounds.GetItem(index));
        return AmbientEmitter::Start();
    }
    return 0;
}


// ---- src/AmbientListSoundEmitter/UpdateSettings.cpp

int AmbientListSoundEmitter::UpdateSettings(float volume, long flags, _D3DVECTOR position, int priority)
{
    return SoundEmitter::UpdateSettings(volume, flags, position, priority);
}


// ---- src/ResponseEmitter/ctor.cpp

ResponseEmitter::ResponseEmitter(AuralScape *scape, PSXSoundGroup *group, short type, int flags)
    : SoundEmitter(scape, group, type, flags)
{
    m_responses.Create(1, 16);
}


// ---- src/ResponseEmitter/dtor.cpp


ResponseEmitter::~ResponseEmitter()
{
    for (int i = 0; i < m_responses.m_numItems; i++) {
        _SoundResponse *r = m_responses.GetItem(i);
        if (r)
            delete r;
    }
    m_responses.Clear();
}


// ---- src/ResponseEmitter/Create.cpp


ResponseEmitter *ResponseEmitter::Create(_DS3DBUFFER buffer, unsigned int flags, float a, float b, int c, int d)
{
    SoundEmitter::Create(buffer, flags, a, b, c, d);
    return this;
}


// ---- src/ResponseEmitter/TriggerResponse.cpp


int ResponseEmitter::TriggerResponse(int id, int loop, int priority)
{
    for (int i = 0; i < m_responses.m_numItems; i++) {
        if (id == m_responses.GetItem(i)->m_id)
            return Play(m_responses.GetItem(i)->m_sound, loop, priority);
    }
    return 0;
}


// ---- src/ResponseEmitter/StopAllResponses.cpp


void ResponseEmitter::StopAllResponses()
{
    for (int i = 0; i < m_responses.m_numItems; i++) {
        _SoundResponse *r = m_responses.GetItem(i);
        if (r->m_sound)
            Stop(r->m_sound);
    }
}


// ---- src/ResponseEmitter/LoadResponseList.cpp


int ResponseEmitter::LoadResponseList(char *name)
{
    char path[260];
    ParameterBlock block;
    char fileName[260];
    char blockName[64];
    int isStereo;
    int isLoop;
    int numEvents;
    sprintf(path, "%s%s", AudioContentDirectory, name);
    if (!block.OpenFile(path, 0))
        return 0;
    if (block.ReadParameterBlock("ResponseEmitter")) {
        block.GetParameter("NumEvents", 0, &numEvents);
        for (int i = 1; i <= numEvents; i++) {
            sprintf(blockName, "Event%d", i);
            if (block.ReadParameterBlock(blockName)) {
                block.GetParameter("FileName", fileName, -1);
                block.GetParameter("IsStereo", 0, &isStereo);
                block.GetParameter("IsLoop", 0, &isLoop);
                _SoundResponse *response = new _SoundResponse;
                response->m_id = i;
                unsigned int flags = 1;
                if (isStereo)
                    flags |= 0x40;
                if (isLoop)
                    flags |= 0x80;
                response->m_sound = m_system->m_soundInterface->GetSoundFromResourceName(m_group, fileName, flags, 1);
                m_responses.AddItem(response);
            }
        }
    }
    return 1;
}


// ---- src/ResponseEmitter/LoadResponseList_0034D430.cpp


int ResponseEmitter::LoadResponseList(ParameterBlock *block)
{
    char fileName[260];
    char blockName[64];
    int isStereo;
    int isLoop;
    int numEvents;
    if (!block)
        return 0;
    if (block->ReadParameterBlock("ResponseEmitter")) {
        block->GetParameter("NumEvents", 0, &numEvents);
        for (int i = 1; i <= numEvents; i++) {
            sprintf(blockName, "Event%d", i);
            if (block->ReadParameterBlock(blockName)) {
                block->GetParameter("FileName", fileName, -1);
                block->GetParameter("IsStereo", 0, &isStereo);
                block->GetParameter("IsLoop", 0, &isLoop);
                _SoundResponse *response = new _SoundResponse;
                response->m_id = i;
                unsigned int flags = 1;
                if (isStereo)
                    flags |= 0x40;
                if (isLoop)
                    flags |= 0x80;
                response->m_sound = m_system->m_soundInterface->GetSoundFromResourceName(m_group, fileName, flags, 1);
                m_responses.AddItem(response);
            }
        }
    }
    return 1;
}


// ---- src/ResponseEmitter/Play.cpp


int ResponseEmitter::Play(PSXSound *sound, int loop, int priority)
{
    SetActiveSound(sound);
    return sound->Play(loop, priority);
}


// ---- src/ResponseEmitter/Stop.cpp


void ResponseEmitter::Stop(PSXSound *sound)
{
    sound->Stop(0);
}


// ---- src/ResponseEmitter/UpdateSettings.cpp


int ResponseEmitter::UpdateSettings(float volume, long pan, _D3DVECTOR position, int flags)
{
    return SoundEmitter::UpdateSettings(volume, pan, position, flags);
}


// ---- src/VehicleEmitter/ctor.cpp

VehicleEmitter::VehicleEmitter(AuralScape *scape, PSXSoundGroup *group, short type, int flags)
    : SoundEmitter(scape, group, type, flags)
{
    m_x214 = 0;
    m_sounds.Create(1, 0x10);
}


// ---- src/VehicleEmitter/dtor.cpp

VehicleEmitter::~VehicleEmitter()
{
}


// ---- src/VehicleEmitter/Create.cpp


VehicleEmitter *VehicleEmitter::Create(_DS3DBUFFER buffer, unsigned int flags, float a, float b, int c, int d)
{
    SoundEmitter::Create(buffer, flags, a, b, c, d);
    return this;
}


// ---- src/VehicleEmitter/Start.cpp


int VehicleEmitter::Start()
{
    return SoundEmitter::Start();
}


// ---- src/VehicleEmitter/UpdateSettings.cpp

int VehicleEmitter::UpdateSettings(float volume, long frequency, _D3DVECTOR position, int flags)
{
    return SoundEmitter::UpdateSettings(volume, frequency, position, flags);
}


// Retail's RTTI block has BaseObject's, GameObject's, SoundEmitter's and
// ResponseEmitter's records before VehicleEmitter's name (@2016-@2021, then
// @2026): mwcc built them as
// the bases of a class whose vtable it wrote first, pushed last on its LIFO
// list (key function defined after VehicleEmitter's). Nothing constructed that
// class, so retail's linker stripped its vtable, RTTI and functions; link_c
// leaves them out ("Section
// order").
class StrippedResponseEmitter : public ResponseEmitter {
public:
    virtual ~StrippedResponseEmitter();
};

StrippedResponseEmitter::~StrippedResponseEmitter()
{
}
