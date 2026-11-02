// AuralScape.cpp  @ 0x002AF930 (text 0x2AF930-0x2B17A0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's AuralScape.cpp as one translation unit: its functions in retail
// order, then its static initializer. `make link` places the object's data as
// well as its code (tools/link_c.py). The ContainerList<SoundEmitter *> members
// and ContainerList<Sound *>::GetItem at the end of its tu_map row are
// instantiated here (see the end). The old files' views map onto the shared
// headers: +0xA0 is the real PSXSoundInterface (its sounds are
// SoundInterface::m_sounds, its driver m_snd), an emitter's "in range" bit
// SoundEmitter::m_bit1, its buffer the _DS3DBUFFER bytes at +0x160, its "active
// sound" m_sound (+0x198 streaming).



#include "soultree_assert_block.h"
#include "ContainerList/ContainerList.h"
#include "GameObject/GameObject.h"
#include "d3dtypes.h"
#include "_DS3DBUFFER/_DS3DBUFFER.h"
#include "Sound/Sound.h"
#include "RainbowPS2Snd/RainbowPS2Snd.h"
#include "SoundInterface/SoundInterface.h"
#include "PSXSoundInterface/PSXSoundInterface.h"
#include "PSXSound/PSXSound.h"
#include "SoundListener/SoundListener.h"
#include "SoundEmitter/SoundEmitter.h"
#include "AuralScape/AuralScape.h"
#include "DebugOverlay/DebugOverlay.h"
#include "Game/Game.h"
// The axis vectors every Soultree TU defines (and __sinit constructs).
#include "sinit/AxisVectors.h"

class ProcessNode;
class KeyInfo;
class SoultreeObject;
enum DeviceType {};
int KEYDOWN_HANDLER_TEST(unsigned int key, DeviceType device, ProcessNode *node, unsigned int flags);
int GetVolumeAndPan(_D3DVECTOR &front, _D3DVECTOR &relative, float distance, float minDistance,
                    float maxDistance, _D3DVECTOR velocity, _D3DVECTOR sourceVelocity,
                    float *volume, long *pan, int *flags);
int DistanceCompareDescend(const void *a, const void *b);
extern Game *LpGame;

class SoultreeSoundEmitter : public SoundEmitter {
public:
    SoultreeSoundEmitter(AuralScape *scape, PSXSoundGroup *group, short type, int enabled);
    virtual ~SoultreeSoundEmitter();
    int Create(char *name, SoultreeObject *object, _DS3DBUFFER buffer, unsigned int flags, float a, float b, int c,
               int d);
#ifdef SPLASHDOWN_PC
    char pad200[2 * sizeof(void *) + 0x210 - 0x208];  // x86-64: m_target, m_object, pad208 (SoultreeSoundEmitter.h)
#else
    char pad200[0x210 - 0x200];
#endif
};


// SoundEmitter.h keeps m_buffer as bytes (several users define _DS3DBUFFER
// themselves); Evaluate reads it through this view of the emitter, so each
// field is one offset from the emitter as in retail (a cast of m_buffer, or
// an indexed byte, schedules the argument addresses differently).
struct SoundEmitterBufferView {
#ifdef SPLASHDOWN_PC
    char pad0[__builtin_offsetof(SoundEmitter, m_buffer)];
#else
    char pad0[0x160];
#endif
    _DS3DBUFFER m_buffer;               // +0x160
};
#define EMITTER_BUFFER(e) (((SoundEmitterBufferView *)(e))->m_buffer)

// The listener distances and positions Evaluate compares (retail's .bss
// order).
float TempDistTable[MAX_NUM_LISTENERS];
_D3DVECTOR TempPosTable[MAX_NUM_LISTENERS];

static inline float Fabs(float x)
{
    float r;
    asm("abs.s $f8, %1; swc1 $f8, 0(%0)" : : "r"(&r), "f"(x) : "$f8", "memory");
    return r;
}

static inline float Fsqrt(float x)
{
    float r;
    asm("sqrt.s $f8, %1; swc1 $f8, 0(%0)" : : "r"(&r), "f"(x) : "$f8", "memory");
    return r;
}

int GetVolumeAndPan(_D3DVECTOR &front, _D3DVECTOR &relative, float distance, float minDistance,
                    float maxDistance, _D3DVECTOR velocity, _D3DVECTOR sourceVelocity,
                    float *volume, long *pan, int *flags);
int DistanceCompareDescend(const void *a, const void *b);

AuralScape::AuralScape(int id) : GameObject(id)
{
    m_x38 = 0;
    m_num3DVoices = 0;
    m_x44 = 0;
    m_x40 = 0;
    m_numListeners = 0;
    m_x34 = 0;
    m_x30 = 0;
    for (int i = 0; i < MAX_NUM_LISTENERS; i++)
        m_listeners[i] = 0;
    m_debugPage = -1;
    m_x9C_bit0 = 1;
}

AuralScape::~AuralScape()
{
    for (int i = 0; i < 4; i++) {
        if (m_listeners[i])
            delete m_listeners[i];
    }
    if (m_soundInterface)
        delete m_soundInterface;
}

AuralScape *AuralScape::Create(int x38, unsigned int flags)
{
    if (!GameObject::Create())
        return 0;
    m_soundInterface = new PSXSoundInterface;
    if (!m_soundInterface)
        return 0;
    if (m_soundInterface->Create(flags))
        return 0;
    m_soundInterface->LoadSoundStringTable("AppStart\\AudioStr.txt");
    m_x38 = x38;
    m_num3DVoices = m_soundInterface->GetAvailable3DHardwareVoices();
    if (m_emitters.Create(8, 16) && m_audible.Create(8, 16) && m_playing.Create(8, 16))
        return this;
    Release();
    return 0;
}

void AuralScape::SetDolby(int dolby)
{
    if (m_soundInterface)
        m_soundInterface->SetDolby(dolby);
}

void AuralScape::SetMono(int mono)
{
    if (m_soundInterface)
        m_soundInterface->SetMono(mono);
}

#pragma alignlabel off

int AuralScape::AddListener(int listenerID, float a, float b)
{
#line 304 "AuralScape.cpp"
    ASSERT(listenerID < 4, "AddListener: listenerID >= MAX_NUM_LISTENERS");
    m_listeners[listenerID] = new SoundListener;
    if (!m_listeners[listenerID] || !m_listeners[listenerID]->Create(this, a, b)) {
        delete m_listeners[listenerID];
        m_listeners[listenerID] = 0;
        return 0;
    }
    m_numListeners++;
    return 1;
}

int AuralScape::RemoveListener(int index)
{
    if (m_listeners[index]) {
        delete m_listeners[index];
        m_listeners[index] = 0;
        m_numListeners--;
        m_x34--;
        return 1;
    }
    return 0;
}

void AuralScape::UpdateListener(int listener, _D3DVECTOR *position, _D3DVECTOR *front,
                                _D3DVECTOR *top, _D3DVECTOR *velocity)
{
#line 356 "AuralScape.cpp"
    ASSERT(listener < MAX_NUM_LISTENERS, "UpdateListener: listener >= MAX_NUM_LISTENERS");
    if (m_listeners[listener]) {
        if (position)
            m_listeners[listener]->SetPosition(position);
        if (front && top)
            m_listeners[listener]->SetOrientation(front, top);
        if (velocity)
            m_listeners[listener]->SetVelocity(velocity);
    }
}

#pragma alignlabel reset

void AuralScape::StopAllAuralScapeSounds()
{
    for (int i = 0; i < m_emitters.m_numItems; i++)
        m_emitters.GetItem(i)->Stop();
}

// Per frame: decides which emitters are in range of a listener, starts and
// stops them, unloads quiet ones when 3D voices run out, and sets each
// playing emitter's volume and pan from its nearest listener.
void AuralScape::Evaluate()
{
    int i;
    int j;
    int nearest;
    int freeVoices;
    SoundEmitter *emitter;
    float baseVolume;
    nearest = 0;
    for (i = 0; i < m_emitters.m_numItems; i++) {
        emitter = m_emitters.GetItem(i);
        emitter->UpdateFromAuralscape(m_numListeners, m_listeners);
    }
    m_audible.Clear();
    m_playing.Clear();
    for (i = 0; i < m_emitters.m_numItems; i++) {
        emitter = m_emitters.GetItem(i);
        if (emitter->m_enabled) {
            if (emitter->m_bit1) {
                m_audible.AddItem(emitter);
                if (emitter->IsPlaying()) {
                    m_playing.AddItem(emitter);
                } else {
                    baseVolume = emitter->GetBaseVolume();
                    emitter->SetBaseVolume(0.0f);
                    if (emitter->Start())
                        m_playing.AddItem(emitter);
                    emitter->SetBaseVolume(baseVolume);
                }
            } else if (emitter->IsPlaying()) {
                emitter->Stop();
            }
        }
    }
    // Retail compares the free-voice count with nearest's initial 0 (one
    // register), and stops unloading once the count is back at or below it.
    freeVoices = m_x38 + m_num3DVoices - m_x44;
    if (freeVoices < nearest) {
        m_emitters.Sort(DistanceCompareDescend);
        for (i = 0; i < m_emitters.m_numItems; i++) {
            emitter = m_emitters.GetItem(i);
            if (!emitter->IsPlaying()) {
                freeVoices++;
                emitter->UnloadSound();
                m_x44--;
                if (freeVoices <= nearest)
                    break;
            }
        }
    }
    _D3DVECTOR closest;
    _D3DVECTOR front;
    _D3DVECTOR top;
    _D3DVECTOR relative;
    _D3DVECTOR listenerPosition;
    _D3DVECTOR soundPosition;
    _D3DVECTOR velocity;
    long pan;
    float volume;
    int flags;
    float minDistance;
    for (i = 0; i < m_playing.m_numItems; i++) {
        SoundEmitter *emitter = m_playing.GetItem(i);
        for (j = 0; j < m_numListeners; j++) {
            m_listeners[j]->GetPosition(&TempPosTable[j]);
            float dx = Fabs(TempPosTable[j].x - EMITTER_BUFFER(emitter).vPosition.x);
            float d = dx * dx;
            float dz = Fabs(TempPosTable[j].z - EMITTER_BUFFER(emitter).vPosition.z);
            TempDistTable[j] = Fsqrt(d + dz * dz);
        }
        minDistance = TempDistTable[0];
        closest = TempPosTable[0];
        nearest = 0;
        for (j = 0; j < m_numListeners; j++) {
            if (TempDistTable[j] < minDistance) {
                minDistance = TempDistTable[j];
                closest = TempPosTable[j];
                nearest = j;
            }
        }
        m_listeners[nearest]->GetOrientation(&front, &top);
        m_listeners[nearest]->GetVelocity(&velocity);
        listenerPosition = closest;
        soundPosition = EMITTER_BUFFER(emitter).vPosition;
        relative.x = soundPosition.x - listenerPosition.x;
        relative.y = soundPosition.y - listenerPosition.y;
        relative.z = soundPosition.z - listenerPosition.z;
        if (GetVolumeAndPan(front, relative, minDistance, EMITTER_BUFFER(emitter).flMinDistance,
                            EMITTER_BUFFER(emitter).flMaxDistance, velocity, EMITTER_BUFFER(emitter).vVelocity,
                            &volume, &pan, &flags))
            emitter->UpdateSettings(volume, pan, listenerPosition, flags);
    }
}


int AuralScape::Tick(float dt)
{
    m_soundInterface->m_snd.GetPlayStatus(dt);
    Evaluate();
    int i;
    Sound *sound;
    int reverbZone;
    int reverbEnviro;
    int sounds;
    int playing;
    int memory;
    reverbZone = 0;                 // never set
    reverbEnviro = 0;
    for (i = 0; i < m_numListeners; i++)
        m_listeners[i]->CommitDeferredSettings();
    if (LpGame->m_debugOverlay) {
        if (m_debugPage < 0) {
            int page = LpGame->m_debugOverlay->m_numPages;
            LpGame->m_debugOverlay->m_numPages = page + 1;
            m_debugPage = page;
        }
        LpGame->m_debugOverlay->PrintfHeading(m_debugPage, "Audio");
        LpGame->m_debugOverlay->PrintfDetail(m_debugPage, "Hardware");
        LpGame->m_debugOverlay->PrintfDetail(m_debugPage, "");
        sounds = 0;
        playing = 0;
        memory = 0;
        for (i = 0; i < m_soundInterface->m_sounds.m_numItems; i++) {
            sound = m_soundInterface->m_sounds.GetItem(i);
#line 664 "AuralScape.cpp"
            ASSERT(sound, "Tick: SOUND NOT FOUND");
            if (sound) {
                sounds++;
                if (sound->IsPlaying())
                    playing++;
                memory += sound->m_waveMemory;
            }
        }
        LpGame->m_debugOverlay->PrintfDetail(m_debugPage, "Sounds %d Playing %d", sounds, playing);
        LpGame->m_debugOverlay->PrintfDetail(m_debugPage, "");
        LpGame->m_debugOverlay->PrintfDetail(m_debugPage, "AuralScape");
        LpGame->m_debugOverlay->PrintfDetail(m_debugPage, "NumEmitters %d", m_emitters.m_numItems);
        LpGame->m_debugOverlay->PrintfDetail(m_debugPage, "Audible %d", m_audible.m_numItems);
        int loaded = 0;
        for (i = 0; i < m_emitters.m_numItems; i++) {
            if (m_emitters.GetItem(i)->m_sound ? 1 : 0)
                loaded++;
        }
        LpGame->m_debugOverlay->PrintfDetail(m_debugPage, "Loaded %d", loaded);
        LpGame->m_debugOverlay->PrintfDetail(m_debugPage, "Playing %d", m_playing.m_numItems);
        LpGame->m_debugOverlay->PrintfDetail(m_debugPage, "");
        LpGame->m_debugOverlay->PrintfDetail(m_debugPage, "ReverbZone %d", reverbZone);
        LpGame->m_debugOverlay->PrintfDetail(m_debugPage, "ReverbEnviro %d", reverbEnviro);
        LpGame->m_debugOverlay->PrintfDetail(m_debugPage, "");
        LpGame->m_debugOverlay->PrintfDetail(m_debugPage, "Total Wave Memory %d", memory);
    }
    return GameObject::Tick(dt);
}

int AuralScape::KeyDownHandler(ProcessNode *node, KeyInfo *key)
{
    if (KEYDOWN_HANDLER_TEST(0x16, (DeviceType)0, node, 0x80))
        m_soundInterface->m_snd.ShowDriverMemoryUsed();
    return GameObject::KeyDownHandler(node, key);
}

SoundEmitter *AuralScape::CreateSoundEmitter(PSXSoundGroup *group, char *sound, _DS3DBUFFER buffer, int enabled,
                                             short type, unsigned int flags, float a, float b, int c, int d)
{
    SoundEmitter *emitter = new SoundEmitter(this, group, type, enabled);
    if (!emitter)
        return 0;
    if (!emitter->Create(buffer, flags, a, b, c, d)) {
        delete emitter;
        return 0;
    }
    if (sound)
        emitter->LoadSound(sound, 0);
    m_emitters.AddItem(emitter);
    AddChild(emitter, -1);
    return emitter;
}

#pragma alignlabel off

int AuralScape::DestroySoundEmitter(SoundEmitter *emitter)
{
#line 784 "AuralScape.cpp"
    ASSERT(emitter, "DestroySoundEmitter: soundEmitter==NULL");
    if (!emitter)
        return 0;
    m_emitters.RemoveItem(emitter);
    emitter->RemoveFromChain();
    emitter->Release();
    return 1;
}

#pragma alignlabel reset

int AuralScape::DestroyAllSoundEmitters()
{
    while (m_emitters.m_numItems)
        DestroySoundEmitter(m_emitters.GetItem(0));
    return 1;
}

SoultreeSoundEmitter *AuralScape::CreateSoultreeEmitter(char *name, SoultreeObject *object, PSXSoundGroup *group,
                                                      char *sound, _DS3DBUFFER buffer, int enabled, short type,
                                                      unsigned int flags, float a, float b, int c, int d)
{
    SoultreeSoundEmitter *emitter = new SoultreeSoundEmitter(this, group, type, enabled);
    if (!emitter)
        return 0;
    if (!emitter->Create(name, object, buffer, flags, a, b, c, d)) {
        delete emitter;
        return 0;
    }
    if (sound) {
        if (flags & 8)
            emitter->LoadSound(sound, 1);
        else
            emitter->LoadSound(sound, 0);
    }
    m_emitters.AddItem(emitter);
    AddChild(emitter, 2);
    return emitter;
}

#pragma alignlabel off

int AuralScape::AddEmitter(SoundEmitter *emitter)
{
    if (!emitter)
        return 0;
#line 922 "AuralScape.cpp"
    ASSERT(emitter->m_sound, "AddEmitter: Attempt to add an emitter without an Active Sound");
    ASSERT(!emitter->m_sound->m_x198, "AddEmitter: Attempt to add positional Streaming sound");
    return m_emitters.AddItem(emitter);
}

#pragma alignlabel reset

int AuralScape::RemoveEmitter(SoundEmitter *emitter)
{
    if (!emitter)
        return 0;
    return m_emitters.RemoveItem(emitter);
}

int AuralScape::CreateMusic(int a, int b, int c)
{
    return m_soundInterface->CreateMusic(a, b, c);
}

// Volume and pan of a sound heard from the nearest listener (distance in
// the x-z plane).
int AuralScape::AttenuateSound(PSXSound *sound, _D3DVECTOR *position)
{
    _D3DVECTOR closest;
    _D3DVECTOR front;
    _D3DVECTOR top;
    _D3DVECTOR relative;
    _D3DVECTOR listenerPosition;
    _D3DVECTOR soundPosition;
    _D3DVECTOR velocity;
    _D3DVECTOR positions[2];
    long pan;
    float distance[3];
    float volume;
    int flags;
    float minDistance;
    int i;
    int nearest;

    if (m_numListeners > 0) {
        for (i = 0; i < m_numListeners; i++) {
            m_listeners[i]->GetPosition(&positions[i]);
            float dx = Fabs(positions[i].x - position->x);
            float d = dx * dx;
            float dz = Fabs(positions[i].z - position->z);
            distance[i] = Fsqrt(d + dz * dz);
        }
        minDistance = distance[0];
        nearest = 0;
        closest = positions[0];
        for (i = 0; i < m_numListeners; i++) {
            if (distance[i] < minDistance) {
                minDistance = distance[i];
                closest = positions[i];
                nearest = i;
            }
        }
        m_listeners[nearest]->GetOrientation(&front, &top);
        m_listeners[nearest]->GetVelocity(&velocity);
        listenerPosition = closest;
        soundPosition = *position;
        relative.x = soundPosition.x - listenerPosition.x;
        relative.y = soundPosition.y - listenerPosition.y;
        relative.z = soundPosition.z - listenerPosition.z;
        if (GetVolumeAndPan(front, relative, minDistance, 0.0f, 450.0f, velocity, ZeroVector, &volume, &pan, &flags)) {
            if (sound) {
                sound->UpdateSettings(volume, pan, listenerPosition, flags);
                return 1;
            }
        }
    }
    return 0;
}



// ---- ContainerList<SoundEmitter *> and ContainerList<Sound *>::GetItem: mwcc
// instantiates the members this TU calls at its end, where retail has them
// (0x2B12A0-0x2B17A0).
#define CONTAINERLIST_MEMBERS_ALL
#include "ContainerList/ContainerListMembers.h"
