// JetskiAudio.cpp  @ 0x0033C0F0 (text 0x33C0F0-0x343480)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's JetskiAudio.cpp as one translation unit: the jetski sound emitters
// (engine, splash, spray, terrain, rider, edge monster, UI rider, crowd),
// JetskiDialogueStreamer and StripAudioNumbers, then its
// ContainerList<Rider_Stream_Info *> members (instantiated at the end,
// CONTAINERLIST_MEMBERS_ALL) and the static initializer (only the axis
// vectors).


#include "ContainerList/ContainerList.h"
#include "sinit/AxisVectors.h"
#include "d3dtypes.h"
#include "_DS3DBUFFER/_DS3DBUFFER.h"
#include "GameObject/GameObject.h"
#include "JetskiEngineSoundEmitter/JetskiEngineSoundEmitter.h"
#include "Sound/Sound.h"
#include "PSXSound/PSXSound.h"
#include "ParameterBlock/ParameterBlock.h"
#include "AuralScape/AuralScape.h"
#include "PhysicsRigidBody/PhysicsRigidBody.h"
#include "JetskiVehicle/JetskiVehicle.h"
#include "JetskiSplashSoundEmitter/JetskiSplashSoundEmitter.h"
#include "PhysicsBody/PhysicsBody.h"
#include "SoundEmitter/SoundEmitter.h"
#include "JetskiTerrainSoundEmitter/JetskiTerrainSoundEmitter.h"
#include "JetskiRiderSoundEmitter/JetskiRiderSoundEmitter.h"
#include "Rider_Stream_Info/Rider_Stream_Info.h"
#include "RainbowPS2Snd/RainbowPS2Snd.h"
#include "SoundListener/SoundListener.h"
#include "JetskiGameOptions/JetskiGameOptions.h"
#include "JetskiGame/JetskiGame.h"
#include "PSXSoundInterface/PSXSoundInterface.h"
#include "JetskiEdgeMonsterSoundEmitter/JetskiEdgeMonsterSoundEmitter.h"
#include "JetskiUIRiderSoundEmitter/JetskiUIRiderSoundEmitter.h"
#include "JetskiCrowdSoundEmitter/JetskiCrowdSoundEmitter.h"
#include "JetskiDialogueStreamer/JetskiDialogueStreamer.h"
#include "VehicleEmitter/VehicleEmitter.h"
#include "JetskiEngineModel/JetskiEngineModel.h"
#include "JetskiGroundModel/JetskiGroundModel.h"
#include "HydrodynamicsModel/HydrodynamicsModel.h"
#include "JetskiCompetition/JetskiCompetition.h"
#include "ParticleEmitter/ParticleEmitter.h"
#include "JetskiSpraySoundEmitter/JetskiSpraySoundEmitter.h"
#include "BowSprayParticleEmitter/BowSprayParticleEmitter.h"
#include "soultree_assert_block.h"
#define CONTAINERLIST_MEMBERS_ALL
#include "ContainerList/ContainerListMembers.h"

// The vehicle's .aud sound lists (each a list of resource names).
struct VehicleAudType {
    ContainerList<char *> m_spraySounds;        // +0x00
    ContainerList<char *> m_edgeMonsterSounds;  // +0x14
    ContainerList<char *> m_splashSounds[5];    // +0x28
    ContainerList<char *> m_terrainSounds[3];   // +0x8C
    ContainerList<char *> m_x0C8;               // +0xC8
    ContainerList<char *> m_x0DC;               // +0xDC
    ContainerList<char *> m_x0F0;               // +0xF0
};

// One dialogue track: its file name and stream handle.
struct DialogueLine {
    char m_name[0x104];      // +0x000
    int m_stream;            // +0x104
};

// ---- src/JetskiEngineSoundEmitter/ctor.cpp
class PSXSound;
class AuralScape;
class PSXSoundGroup;




JetskiEngineSoundEmitter::JetskiEngineSoundEmitter(AuralScape *scape, PSXSoundGroup *group, short id, int player)
    : VehicleEmitter(scape, group, id, player)
{
    m_state = 0;
    m_previousSound = 0;
    m_x248 = 0;
    m_started = 1;
    m_startDelay = 0.0f;
    m_stallTime = 0.5f;
    m_resetSound = 0;
    m_inIdle.Create(1, 16);
    m_inAccel.Create(1, 16);
    m_inDecel.Create(1, 16);
    m_inDecelHi.Create(1, 16);
    m_inPower.Create(1, 16);
    m_outIdle.Create(1, 16);
    m_outAccel.Create(1, 16);
    m_outDecel.Create(1, 16);
    m_outPower.Create(1, 16);
    m_submerged.Create(1, 16);
    m_stall.Create(1, 16);
    m_x230 = 0;
    m_idling = 1;
    m_x240 = 0;
    m_lastStallSound = -1;
    m_idleTimer = 0.25f;
    m_x23C = 0;
}


// ---- src/JetskiEngineSoundEmitter/dtor.cpp
class PSXSound;




JetskiEngineSoundEmitter::~JetskiEngineSoundEmitter()
{
    m_inIdle.Clear();
    m_inAccel.Clear();
    m_inDecel.Clear();
    m_inDecelHi.Clear();
    m_inPower.Clear();
    m_outIdle.Clear();
    m_outAccel.Clear();
    m_outDecel.Clear();
    m_outPower.Clear();
    m_submerged.Clear();
    m_stall.Clear();
}


// ---- src/JetskiEngineSoundEmitter/Create.cpp




int JetskiEngineSoundEmitter::Create(JetskiVehicle *vehicle, char *name, float volume)
{
    m_vehicle = vehicle;
    m_engine = vehicle->m_engine;
    _DS3DBUFFER buffer;
    buffer.flMinDistance = 70.0f;
    buffer.flMaxDistance = volume;
    if (m_vehicle->m_player == -1)
        m_x23C = 1;
    LoadEngineSounds(name);
    return (int)VehicleEmitter::Create(buffer, 2, volume, 100.0f, 0, 0);
}


// ---- src/JetskiEngineSoundEmitter/Tick.cpp
// Retail's fmax is an inline-asm helper like its fabs: max.s into $f8, stored
// through a pointer to a stack temporary.
inline float Fmax(float a, float b)
{
    float r;
    asm("max.s $f8, %1, %2; swc1 $f8, 0(%0)" : : "r"(&r), "f"(a), "f"(b) : "$f8", "memory");
    return r;
}



int JetskiEngineSoundEmitter::Tick(float dt)
{
    if (m_state == 9)
        m_stallTime = Fmax(m_stallTime - dt, 0.0f);
    ChooseState(dt);
    PositionEmitter();
    if (!m_started) {
        if (m_startDelay < dt) {
            m_started = 1;
            m_startDelay = 0.0f;
        } else {
            m_startDelay -= dt;
        }
    }
    return GameObject::Tick(dt);
}


// ---- src/JetskiEngineSoundEmitter/PositionEmitter_0033C620.cpp






int JetskiEngineSoundEmitter::PositionEmitter()
{
    UpdateEmitter(&m_vehicle->m_body->m_x60, &ZeroVector);
    return 1;
}


// ---- src/JetskiEngineSoundEmitter/StopEngineSounds.cpp

int JetskiEngineSoundEmitter::StopEngineSounds()
{
    Stop();
    return 1;
}


// ---- src/JetskiEngineSoundEmitter/PlayEngineResetSound.cpp


int JetskiEngineSoundEmitter::PlayEngineResetSound()
{
    return m_resetSound ? m_resetSound->Play(1, 0) : 0;
}


// ---- src/JetskiEngineSoundEmitter/PlayEngineStallSound.cpp
#pragma fast_fptosi on  // retail never calls fptosi: float->int is an inline cvt.w.s


extern "C" int rand();


int JetskiEngineSoundEmitter::PlayEngineStallSound()
{
    float r = 4.656613e-10f * rand();     // rand() / 2^31
    int n = m_stall.m_numItems;
    int i = (int)((n - 1) * r);
    if (n > 1 && i == m_lastStallSound)
        i = (i + 1) % n;
    m_lastStallSound = i;
    PSXSound *sound = m_stall.GetItem(i);
    if (!sound)
        return 0;
    return sound->Play(1, 0);
}
#pragma fast_fptosi reset


// ---- src/JetskiEngineSoundEmitter/LoadEngineSounds.cpp
extern "C" int sprintf(char *buf, const char *fmt, ...);

class PSXSoundGroup;
class PSXSound;








// Every "Sound_%d" of a section, loaded through the sound interface.
#define LOAD_SOUNDS(section, list, flags)                                                       \
    if (block.ReadParameterBlock(section)) {                                                    \
        block.GetParameter("NumSounds", 0, &count);                                             \
        for (i = 1; i <= count; i++) {                                                          \
            sprintf(key, "Sound_%d", i);                                                        \
            block.GetParameter(key, soundFile, -1);                                             \
            sound = m_system->m_soundInterface->GetSoundFromResourceName(m_group, soundFile, flags, 1); \
            if (sound)                                                                          \
                list.AddItem(sound);                                                            \
        }                                                                                       \
    }

// Loads the engine's sound sets from a parameter file; the last sound
// loaded becomes the active one.
int JetskiEngineSoundEmitter::LoadEngineSounds(char *file)
{
    PSXSound *sound = 0;
    ParameterBlock block;
    char soundFile[0x110];
    char key[0x4C];
    int count;
    int i;

    if (!block.OpenFile(file, 0))
        return 0;
    LOAD_SOUNDS("InIdle", m_inIdle, 0x81)
    LOAD_SOUNDS("InAccel", m_inAccel, 1)
    LOAD_SOUNDS("InDecel", m_inDecel, 1)
    LOAD_SOUNDS("InDecelHi", m_inDecelHi, 1)
    LOAD_SOUNDS("InPower", m_inPower, 0x81)
    LOAD_SOUNDS("OutIdle", m_outIdle, 0x81)
    LOAD_SOUNDS("OutAccel", m_outAccel, 1)
    LOAD_SOUNDS("OutDecel", m_outDecel, 1)
    LOAD_SOUNDS("OutPower", m_outPower, 1)
    if (block.ReadParameterBlock("Reset")) {
        block.GetParameter("NumSounds", 0, &count);
        if (count) {
            sprintf(key, "Sound_1");
            block.GetParameter(key, soundFile, -1);
            m_resetSound = m_system->m_soundInterface->GetSoundFromResourceName(m_group, soundFile, 1, 1);
            if (m_resetSound)
                m_resetSound->SetBaseVolume(100.0f);
        }
    }
    if (block.ReadParameterBlock("Stall")) {
        block.GetParameter("NumSounds", 0, &count);
        for (i = 1; i <= count; i++) {
            sprintf(key, "Sound_%d", i);
            block.GetParameter(key, soundFile, -1);
            PSXSound *stall = new PSXSound(m_system->m_soundInterface, m_group, 1);
            if (!stall->CreateFromFile(soundFile, 1, 1)) {
                if (stall) {
                    delete stall;
                    stall = 0;
                }
            }
            if (stall) {
                stall->SetBaseVolume(100.0f);
                m_stall.AddItem(stall);
            }
        }
    }
    LOAD_SOUNDS("Submerged", m_submerged, 0x81)
    return SetActiveSound(sound);
}


// ---- src/JetskiEngineSoundEmitter/ResetState.cpp

void JetskiEngineSoundEmitter::ResetState()
{
    m_state = 0;
}


// ---- src/JetskiEngineSoundEmitter/ChooseState.cpp





void JetskiEngineSoundEmitter::ChooseState(float dt)
{
    // unsigned char, not bool: retail keeps the || as a value, then andi 0xFF.
    unsigned char airborne = m_vehicle->m_hydrodynamics->m_x40 < -2.0f || m_vehicle->m_ground->m_onGround;
    if (!(m_engine->m_throttle <= 0.1f) && airborne && m_stallTime == 0.0f) {
        ChooseOutOfWaterIdleState(dt);
        return;
    }
    if (m_state != 9)
        m_stallTime = 0.5f;
    if (m_engine->m_throttle <= 0.1f) {
        m_idleTimer -= dt;
        if (!m_x23C || m_idleTimer <= 0.0f) {
            m_idling = 1;
            if (airborne)
                ChooseOutOfWaterIdleState(dt);
            else
                ChooseInWaterIdleState(dt);
        }
    } else {
        m_idleTimer = 0.25f;
        if (airborne)
            ChooseOutOfWaterAccelState(dt);
        else
            ChooseInWaterAccelState(dt);
        m_idling = 0;
    }
}


// ---- src/JetskiEngineSoundEmitter/ChooseInWaterIdleState.cpp






void JetskiEngineSoundEmitter::ChooseInWaterIdleState(float dt)
{
    PSXSound *sound = GetActiveSound();
    int state = m_state;
    if (state == 5 || state == 2) {
        m_x248 = 0;
        if (m_vehicle->m_body->m_speed < 40.0f) {
            if (m_inDecel.m_numItems) {
                m_inDecel.GetItem(0)->SetFrequencyModifier(1.0f);
                SwitchSounds(m_inDecel.GetItem(0));
            }
            m_state = 3;
        } else {
            if (m_inDecelHi.m_numItems) {
                m_inDecelHi.GetItem(0)->SetFrequencyModifier(1.0f);
                SwitchSounds(m_inDecelHi.GetItem(0));
            }
            m_state = 3;
        }
    } else if (state == 4) {
        if ((sound && !sound->IsPlaying()) || m_vehicle->m_body->m_speed < 40.0f) {
            m_x248 = 0;
            if (m_inDecel.m_numItems) {
                m_inDecel.GetItem(0)->SetFrequencyModifier(1.0f);
                SwitchSounds(m_inDecel.GetItem(0));
            }
            m_state = 3;
        }
    } else {
        if (sound && state == 3) {
            if (sound->m_duration - sound->m_elapsed < dt || m_vehicle->m_body->m_speed < 7.0f)
                m_state = 0;
        }
        if (m_state != 1 && m_state != 3 && m_state != 4) {
            m_x248 = 0;
            if (m_inIdle.m_numItems)
                SwitchSounds(m_inIdle.GetItem(0));
            m_state = 1;
        }
    }
}


// ---- src/JetskiEngineSoundEmitter/ChooseOutOfWaterIdleState.cpp




void JetskiEngineSoundEmitter::ChooseOutOfWaterIdleState(float)
{
    PSXSound *sound = GetActiveSound();
    switch (m_state) {
    case 8:
    case 6:
        break;
    default:
        if (m_state == 7 && sound && sound->IsPlaying())
            break;
        m_x248 = 0;
        if (m_outIdle.m_numItems) {
            m_outIdle.GetItem(0)->SetFrequencyModifier(1.0f);
            SwitchSounds(m_outIdle.GetItem(0));
        }
        m_state = 6;
        break;
    }
}


// ---- src/JetskiEngineSoundEmitter/ChooseInWaterAccelState.cpp







void JetskiEngineSoundEmitter::ChooseInWaterAccelState(float dt)
{
    PSXSound *sound = GetActiveSound();
    if (m_vehicle->m_hydrodynamics->m_x40 > 3.0f) {
        m_x230 += dt;
        if (m_state != 11 && !(m_x230 < 0.2f)) {
            m_x248 = 1;
            if (m_submerged.m_numItems)
                SwitchSounds(m_submerged.GetItem(0));
            m_state = 11;
        }
        return;
    }
    if (m_state != 2 && m_state != 5) {
        if (m_vehicle->m_body->m_speed < 20.0f) {
            m_x248 = 0;
            if (m_inAccel.m_numItems)
                SwitchSounds(m_inAccel.GetItem(0));
            m_state = 2;
            if (sound)
                m_stallTime = sound->m_duration;
            else
                m_stallTime = 0.5f;
        } else {
            m_x248 = 0;
            if (m_inPower.m_numItems)
                SwitchSounds(m_inPower.GetItem(0));
            m_state = 5;
        }
    } else if (m_state == 2) {
        if (sound && sound->m_duration - sound->m_elapsed < dt) {
            m_x248 = 0;
            if (m_inPower.m_numItems) {
                PSXSound *next = m_inPower.GetItem(0);
                SwitchSounds(next);
            }
            m_state = 5;
        }
        PitchShiftAccel(sound);
    }
    if (m_state == 5 || m_state == 11)
        PitchShiftAccel(sound);
    m_x230 = 0;
}


// ---- src/JetskiEngineSoundEmitter/ChooseOutOfWaterAccelState.cpp




void JetskiEngineSoundEmitter::ChooseOutOfWaterAccelState(float)
{
    PSXSound *sound = GetActiveSound();
    int state = m_state;
    if (!(state == 9 || state == 5 || state == 7) || (state == 7 && m_idling)) {
        m_x248 = 0;
        if (m_outAccel.m_numItems) {
            m_outAccel.GetItem(0)->SetFrequencyModifier(1.0f);
            SwitchSounds(m_outAccel.GetItem(0));
        }
        m_state = 7;
        return;
    }
    if (state == 7 && sound && !sound->IsPlaying())
        m_state = 0;
    if (m_state != 9 && m_state != 7) {
        m_x248 = 0;
        if (m_outPower.m_numItems)
            SwitchSounds(m_outPower.GetItem(0));
        m_state = 9;
    }
}


// ---- src/JetskiEngineSoundEmitter/UpdateSettings.cpp



int JetskiEngineSoundEmitter::UpdateSettings(float volume, long flags, _D3DVECTOR position, int priority)
{
    return m_started ? VehicleEmitter::UpdateSettings(volume, flags, position, priority) : 0;
}


// ---- src/JetskiEngineSoundEmitter/Start.cpp


int JetskiEngineSoundEmitter::Start()
{
    return VehicleEmitter::Start();
}


// ---- src/JetskiEngineSoundEmitter/SwitchSounds.cpp

struct _D3DVECTOR;




int JetskiEngineSoundEmitter::SwitchSounds(PSXSound *sound)
{
    PSXSound *previous = GetActiveSound();
    Stop();
    m_previousSound = previous;
    SetActiveSound(sound);
    if (sound->m_streamed) {
        m_flags = 0;
        m_flags |= 8;
    } else {
        m_disabled = 0;
        m_flags = 0;
        m_flags |= 1;
    }
    return 1;
}


// ---- src/JetskiEngineSoundEmitter/PitchShiftAccel.cpp




int JetskiEngineSoundEmitter::PitchShiftAccel(PSXSound *sound)
{
    if (!sound)
        return 0;
    float speed = m_vehicle->m_body->m_speed;
    if (speed > 0.0f)
        sound->SetFrequencyModifier(1.0f - 0.4f * (1.0f - (0.68182f * speed) / 75.0f));
    return 1;
}


// ---- src/JetskiSplashSoundEmitter/ctor.cpp

class AuralScape;
class PSXSoundGroup;
class JetskiVehicle;

JetskiSplashSoundEmitter::JetskiSplashSoundEmitter(AuralScape *scape, PSXSoundGroup *group, short type, int enabled)
    : SoundEmitter(scape, group, type, enabled)
{
    m_vehicle = 0;
    m_playing = 0;
    m_jumping = 0;
    m_delay = 0;
    m_sprays.Create(1, 16);
    m_fastSplashes.Create(1, 16);
    m_bigSplashes.Create(1, 16);
    m_mediumSplashes.Create(1, 16);
    m_smallSplashes.Create(1, 16);
}


// ---- src/JetskiSplashSoundEmitter/dtor.cpp


class PSXSound;



JetskiSplashSoundEmitter::~JetskiSplashSoundEmitter()
{
    m_sprays.Clear();
    m_fastSplashes.Clear();
    m_bigSplashes.Clear();
    m_mediumSplashes.Clear();
    m_smallSplashes.Clear();
}


// ---- src/JetskiSplashSoundEmitter/Create.cpp

class JetskiVehicle;
struct VehicleAudType;



// Returns int (JetskiVehicle::LoadGenericSounds tests it) but has no return
// statement: retail leaves SoundEmitter::Create's $v0 in place.
int JetskiSplashSoundEmitter::Create(JetskiVehicle *vehicle, VehicleAudType *audType, float volume)
{
    m_vehicle = vehicle;
    _DS3DBUFFER buffer;
    buffer.flMinDistance = 70.0f;
    buffer.flMaxDistance = volume;
    LoadSplashSounds(audType);
    SoundEmitter::Create(buffer, 2, volume, 100.0f, 0, 0);
    m_flags |= 1;
}


// ---- src/JetskiSplashSoundEmitter/Tick.cpp
// Retail's fmax is an inline-asm helper like its fabs: max.s into $f8, stored
// through a pointer to a stack temporary.







int JetskiSplashSoundEmitter::Tick(float dt)
{
    if (m_playing) {
        if (m_vehicle->m_body->m_speed >= 7.0f) {
            Stop();
            m_playing = 0;
            m_delay = 0;
        }
    }
    if (m_delay) {
        m_delay = Fmax(m_delay - dt, 0.0f);
    } else {
        PositionEmitter();
        PlaySplashSounds();
    }
    return GameObject::Tick(dt);
}


// ---- src/JetskiSplashSoundEmitter/PositionEmitter.cpp

int JetskiSplashSoundEmitter::PositionEmitter()
{
    UpdateEmitter(&m_vehicle->m_body->m_x60, &ZeroVector);
    return 1;
}


// ---- src/JetskiSplashSoundEmitter/LoadSplashSounds.cpp

class PSXSoundGroup;






int JetskiSplashSoundEmitter::LoadSplashSounds(VehicleAudType *audType)
{
    PSXSound *sound = 0;
    int i;
    if (!audType)
        return 0;
    for (i = 1; i <= audType->m_splashSounds[0].m_numItems; i++) {
        sound = m_system->m_soundInterface->GetSoundFromResourceName(m_group, audType->m_splashSounds[0].GetItem(i - 1), 1, 1);
        if (sound)
            m_sprays.AddItem(sound);
    }
    for (i = 1; i <= audType->m_splashSounds[1].m_numItems; i++) {
        sound = m_system->m_soundInterface->GetSoundFromResourceName(m_group, audType->m_splashSounds[1].GetItem(i - 1), 1, 1);
        if (sound)
            m_fastSplashes.AddItem(sound);
    }
    for (i = 1; i <= audType->m_splashSounds[2].m_numItems; i++) {
        sound = m_system->m_soundInterface->GetSoundFromResourceName(m_group, audType->m_splashSounds[2].GetItem(i - 1), 1, 1);
        if (sound)
            m_mediumSplashes.AddItem(sound);
    }
    for (i = 1; i <= audType->m_splashSounds[3].m_numItems; i++) {
        sound = m_system->m_soundInterface->GetSoundFromResourceName(m_group, audType->m_splashSounds[3].GetItem(i - 1), 1, 1);
        if (sound)
            m_bigSplashes.AddItem(sound);
    }
    for (i = 1; i <= audType->m_splashSounds[4].m_numItems; i++) {
        sound = m_system->m_soundInterface->GetSoundFromResourceName(m_group, audType->m_splashSounds[4].GetItem(i - 1), 1, 1);
        if (sound)
            m_smallSplashes.AddItem(sound);
    }
    return SetActiveSound(sound);
}


// ---- src/JetskiSplashSoundEmitter/PlaySplashSounds.cpp
#pragma fast_fptosi on

extern "C" int rand();





// A random sound from a list.
#define RANDOM_ITEM(list) (list).GetItem(((list).m_numItems - 1) * (4.656613e-10f * rand()))

int JetskiSplashSoundEmitter::PlaySplashSounds()
{
    PSXSound *active = GetActiveSound();
    if (!m_vehicle->m_x48C)
        return 0;
    if (m_vehicle->m_airborne) {
        if (!(m_vehicle->m_hydrodynamics->m_x40 <= 3.0f)) {
            if (m_jumping)
                return 0;
            m_jumping = 1;
            PSXSound *sound = RANDOM_ITEM(m_mediumSplashes);
            m_delay = 0.5f;
            Play(sound, 1);
            return 1;
        }
        if (m_jumping) {
            m_jumping = 0;
            PSXSound *sound = RANDOM_ITEM(m_mediumSplashes);
            m_delay = 0.5f;
            Play(sound, 1);
            return 1;
        }
        if (m_vehicle->m_body->m_speed < 7.0f && (!active || !active->IsPlaying())) {
            Play(RANDOM_ITEM(m_sprays), 1);
            m_delay = 0.5f;
            m_playing = 1;
            return 1;
        }
        return 0;
    }
    if (m_vehicle->m_body->m_velocity.y < -20.0f) {
        PSXSound *sound = RANDOM_ITEM(m_bigSplashes);
        m_delay = 1.0f;
        Play(sound, 1);
    } else if (m_vehicle->m_body->m_velocity.y < -10.0f) {
        PSXSound *sound = RANDOM_ITEM(m_mediumSplashes);
        m_delay = 1.0f;
        Play(sound, 1);
    } else if (m_vehicle->m_body->m_velocity.y < -2.0f) {
        if (!(m_vehicle->m_body->m_speed <= 30.0f)) {
            PSXSound *sound = RANDOM_ITEM(m_smallSplashes);
            m_delay = 0.5f;
            Play(sound, 1);
        } else {
            PSXSound *sound = RANDOM_ITEM(m_fastSplashes);
            m_delay = 0.5f;
            Play(sound, 1);
        }
    }
    return 1;
}
#pragma fast_fptosi reset


// ---- src/JetskiSplashSoundEmitter/Play.cpp

class PSXSound;




int JetskiSplashSoundEmitter::Play(PSXSound *sound, int)
{
    Stop();
    SetActiveSound(sound);
    m_disabled = 0;
    return 1;
}


// ---- src/JetskiSpraySoundEmitter/ctor.cpp

JetskiSpraySoundEmitter::JetskiSpraySoundEmitter(AuralScape *scape, PSXSoundGroup *group, short id, int enabled)
    : SoundEmitter(scape, group, id, enabled)
{
    m_vehicle = 0;
    m_volume = 0;
    m_x208 = 0;
}


// ---- src/JetskiSpraySoundEmitter/dtor.cpp


JetskiSpraySoundEmitter::~JetskiSpraySoundEmitter()
{
}


// ---- src/JetskiSpraySoundEmitter/Create.cpp


class JetskiVehicle;
struct VehicleAudType;




int JetskiSpraySoundEmitter::Create(JetskiVehicle *vehicle, VehicleAudType *audType, float volume)
{
    m_vehicle = vehicle;
    _DS3DBUFFER buffer;
    buffer.flMinDistance = 70.0f;
    buffer.flMaxDistance = volume;
    LoadSpraySounds(audType);
    int result = (int)SoundEmitter::Create(buffer, 2, volume, 100.0f, 0, 0);
    PSXSound *sound = GetActiveSound();
    if (sound)
        sound->SetBaseVolume(0.0f);
    return result;
}


// ---- src/JetskiSpraySoundEmitter/Tick.cpp

int JetskiSpraySoundEmitter::Tick(float dt)
{
    PositionEmitter();
    PlaySpraySounds();
    return GameObject::Tick(dt);
}


// ---- src/JetskiSpraySoundEmitter/PositionEmitter.cpp

int JetskiSpraySoundEmitter::PositionEmitter()
{
    UpdateEmitter(&m_vehicle->m_body->m_x60, &ZeroVector);
    return 1;
}


// ---- src/JetskiSpraySoundEmitter/LoadSpraySounds.cpp




int JetskiSpraySoundEmitter::LoadSpraySounds(VehicleAudType *aud)
{
    if (!aud)
        return 0;
    return LoadSound(aud->m_spraySounds.GetItem(0), 1);
}


// ---- src/JetskiSpraySoundEmitter/PlaySpraySounds.cpp
#pragma fast_fptosi on

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

static inline float Fmin(float a, float b)
{
    float r;
    asm("min.s $f8, %1, %2; swc1 $f8, 0(%0)" : : "r"(&r), "f"(a), "f"(b) : "$f8", "memory");
    return r;
}






// Spray volume follows speed (full at 500), rising by at most 2 and
// falling by at most 1 per call; out of the water it fades out.
int JetskiSpraySoundEmitter::PlaySpraySounds()
{
    PSXSound *sound = GetActiveSound();
    ParticleEmitter *physics;
    if (sound && (physics = m_vehicle->m_bowSpray[1])->m_particlesEnabled) {
        int volume = (int)(100.0f * (Fmin(500.0f, physics->m_emitRate) / 500.0f));
        volume = MIN(volume, 100);
        if (m_volume < volume) {
            volume = MIN(volume, m_volume + 2);
        } else if (volume < m_volume) {
            volume = MAX(volume, m_volume - 1);
        }
        if (volume != m_volume) {
            m_volume = volume;
            sound->SetBaseVolume(volume);
        }
    } else if (sound && m_volume) {
        int volume = m_volume - 1;
        volume = MAX(volume, 0);
        m_volume = volume;
        sound->SetBaseVolume(m_volume);
    }
    return 1;
}
#pragma fast_fptosi reset


// ---- src/JetskiTerrainSoundEmitter/ctor.cpp

class AuralScape;
class PSXSoundGroup;


class JetskiVehicle;



JetskiTerrainSoundEmitter::JetskiTerrainSoundEmitter(AuralScape *scape, PSXSoundGroup *group, short id, int flags)
    : SoundEmitter(scape, group, id, flags)
{
    m_vehicle = 0;
    m_slowSounds.Create(1, 16);
    m_mediumSounds.Create(1, 16);
    m_fastSounds.Create(1, 16);
}


// ---- src/JetskiTerrainSoundEmitter/dtor.cpp
class PSXSound;



class JetskiVehicle;


JetskiTerrainSoundEmitter::~JetskiTerrainSoundEmitter()
{
    m_slowSounds.Clear();
    m_mediumSounds.Clear();
    m_fastSounds.Clear();
}


// ---- src/JetskiTerrainSoundEmitter/Create.cpp

// DirectSound 3D buffer settings, PS2 port: vectors are 16-byte, 4-float.
class JetskiVehicle;
class VehicleAudType;




int JetskiTerrainSoundEmitter::Create(JetskiVehicle *vehicle, VehicleAudType *audType, float maxDistance)
{
    m_vehicle = vehicle;
    _DS3DBUFFER buffer;
    buffer.flMinDistance = 70.0f;
    buffer.flMaxDistance = maxDistance;
    LoadTerrainSounds(audType);
    return (int)SoundEmitter::Create(buffer, 2, maxDistance, 100.0f, 0, 0);
}


// ---- src/JetskiTerrainSoundEmitter/Tick.cpp

int JetskiTerrainSoundEmitter::Tick(float dt)
{
    PlayTerrainSounds();
    return GameObject::Tick(dt);
}


// ---- src/JetskiTerrainSoundEmitter/LoadTerrainSounds.cpp

class PSXSoundGroup;



int JetskiTerrainSoundEmitter::LoadTerrainSounds(VehicleAudType *audType)
{
    PSXSound *sound = 0;
    int i;
    if (!audType)
        return 0;
    for (i = 1; i <= audType->m_terrainSounds[0].m_numItems; i++) {
        sound = m_system->m_soundInterface->GetSoundFromResourceName(m_group, audType->m_terrainSounds[0].GetItem(i - 1), 0x81, 1);
        if (sound)
            m_slowSounds.AddItem(sound);
    }
    for (i = 1; i <= audType->m_terrainSounds[1].m_numItems; i++) {
        sound = m_system->m_soundInterface->GetSoundFromResourceName(m_group, audType->m_terrainSounds[1].GetItem(i - 1), 0x81, 1);
        if (sound)
            m_mediumSounds.AddItem(sound);
    }
    for (i = 1; i <= audType->m_terrainSounds[2].m_numItems; i++) {
        sound = m_system->m_soundInterface->GetSoundFromResourceName(m_group, audType->m_terrainSounds[2].GetItem(i - 1), 0x81, 1);
        if (sound)
            m_fastSounds.AddItem(sound);
    }
    return SetActiveSound(sound);
}


// ---- src/JetskiTerrainSoundEmitter/PlayTerrainSounds.cpp
#pragma fast_fptosi on

extern "C" int rand();





// Retail tests the volume through a small predicate (value-form bool).
static inline int IsAbove(int v, int max)
{
    return v > max;
}

int JetskiTerrainSoundEmitter::PlayTerrainSounds()
{
    PSXSound *active;
    float speed;
    int volume;
    PSXSound *sound;
    active = GetActiveSound();
    speed = m_vehicle->m_body->m_speed;
    if (!m_vehicle->m_ground->m_onGround || speed < 1.0f) {
        if (m_enabled)
            DisableEmitter();
        return 0;
    }
    EnableEmitter();
    PositionEmitter();
    if (speed < 10.0f && m_slowSounds.m_numItems) {
        sound = m_slowSounds.GetItem((m_slowSounds.m_numItems - 1) * (4.656613e-10f * rand()));
        volume = 100.0f * (speed / 40.0f);
        if (IsAbove(volume, 100))
            volume = 100;
        sound->SetBaseVolume(volume);
        if (sound != active)
            return SwitchSounds(sound);
    } else if (speed < 20.0f && m_mediumSounds.m_numItems) {
        sound = m_mediumSounds.GetItem((m_mediumSounds.m_numItems - 1) * (4.656613e-10f * rand()));
        volume = 100.0f * (speed / 40.0f);
        if (IsAbove(volume, 100))
            volume = 100;
        sound->SetBaseVolume(volume);
        if (sound != active)
            return SwitchSounds(sound);
    } else if (m_fastSounds.m_numItems) {
        sound = m_fastSounds.GetItem((m_fastSounds.m_numItems - 1) * (4.656613e-10f * rand()));
        volume = 100.0f * (speed / 40.0f);
        if (IsAbove(volume, 100))
            volume = 100;
        sound->SetBaseVolume(volume);
        if (sound != active)
            return SwitchSounds(sound);
    }
    return 0;
}
#pragma fast_fptosi reset


// ---- src/JetskiTerrainSoundEmitter/PositionEmitter.cpp

int JetskiTerrainSoundEmitter::PositionEmitter()
{
    UpdateEmitter(&m_vehicle->m_body->m_x60, &ZeroVector);
    return 1;
}


// ---- src/JetskiTerrainSoundEmitter/SwitchSounds.cpp





int JetskiTerrainSoundEmitter::SwitchSounds(PSXSound *sound)
{
    Stop();
    SetActiveSound(sound);
    if (sound->m_streamed) {
        m_flags = 0;
        m_flags |= 8;
    } else {
        m_disabled = 0;
        m_flags = 0;
        m_flags |= 1;
    }
    return 1;
}


// ---- src/JetskiRiderSoundEmitter/ctor.cpp

class PSXSoundInterface;



JetskiRiderSoundEmitter::JetskiRiderSoundEmitter(PSXSoundInterface *soundInterface, unsigned int flags, int enabled)
    : GameObject(enabled)
{
    m_wreckSounds.Create(1, 16);
    m_crashSounds.Create(1, 16);
    m_splashSounds.Create(1, 16);
    m_smallSplashSounds.Create(1, 16);
    m_bigSplashSounds.Create(1, 16);
    m_passSounds.Create(1, 16);
    m_passedSounds.Create(1, 16);
    m_winSounds.Create(1, 16);
    m_joinSounds.Create(1, 16);
    m_stuntSounds.Create(1, 16);
    m_passChance = 0.4f;
    m_passedChance = 0.4f;
    m_crashChance = 0.75f;
    m_stuntChance = 0.5f;
    m_splashChance = 0.75f;
    m_jetski = 0;
    m_540 = 0;
    m_voice = -1;
    m_playTime = 0;
    m_soundInterface = soundInterface;
    m_playing = 0;
    m_flags = flags;
    m_lastWreckSound = -1;
    m_lastCrashSound[0] = 0;
    m_lastSplashSound = -1;
    m_lastStuntSound[0] = 0;
    m_lastPassSound[0] = 0;
    m_lastPassedSound[0] = 0;
}


// ---- src/JetskiRiderSoundEmitter/dtor.cpp

struct Rider_Stream_Info;



JetskiRiderSoundEmitter::~JetskiRiderSoundEmitter()
{
    UnloadSounds();
    m_wreckSounds.Clear();
    m_crashSounds.Clear();
    m_splashSounds.Clear();
    m_smallSplashSounds.Clear();
    m_bigSplashSounds.Clear();
    m_passSounds.Clear();
    m_passedSounds.Clear();
    m_winSounds.Clear();
    m_joinSounds.Clear();
    m_stuntSounds.Clear();
}


// ---- src/JetskiRiderSoundEmitter/Create.cpp

JetskiRiderSoundEmitter *JetskiRiderSoundEmitter::Create(char *file, JetskiVehicle *jetski, float a, int b)
{
    m_jetski = jetski;
    m_540 = b;
    if (b) {
        m_passChance = 0.3f;
        m_passedChance = 0.3f;
        m_crashChance = 0.63f;
        m_stuntChance = 0.38f;
        m_splashChance = 0.75f;
    }
    LoadSounds(file);
    return this;
}


// ---- src/JetskiRiderSoundEmitter/UnloadSounds.cpp
extern "C" void free(void *p);



typedef ContainerList<Rider_Stream_Info *> StreamList;

static inline void FreeStreams(StreamList &list)
{
    while (list.m_numItems) {
        Rider_Stream_Info *stream = list.GetItem(0);
        list.RemoveItemByIndex(0);
        free(stream->m_name);
        delete stream;
    }
}


int JetskiRiderSoundEmitter::UnloadSounds()
{
    FreeStreams(m_wreckSounds);
    FreeStreams(m_crashSounds);
    FreeStreams(m_splashSounds);
    FreeStreams(m_stuntSounds);
    FreeStreams(m_smallSplashSounds);
    FreeStreams(m_bigSplashSounds);
    FreeStreams(m_passSounds);
    FreeStreams(m_passedSounds);
    FreeStreams(m_winSounds);
    FreeStreams(m_joinSounds);
    return 1;
}


// ---- src/JetskiRiderSoundEmitter/LoadSounds.cpp

extern "C" int sprintf(char *buf, const char *fmt, ...);
extern "C" void *malloc(unsigned int size);
extern "C" char *strcpy(char *dst, const char *src);

extern char AudioContentDirectory[];






typedef ContainerList<Rider_Stream_Info *> StreamList;


// Each section of the rider's sound file lists NumSounds stream names
// (Sound_1, Sound_2, ...); each is preloaded and added to its list.
int JetskiRiderSoundEmitter::LoadSounds(char *fileName)
{
    ParameterBlock block;
    char value[0x110];
    char key[0x40];
    char message[0x100];
    int numSounds;
    int i;
    Rider_Stream_Info *info;
    int dirLength;          // unused beyond the malloc size
    dirLength = strlen(AudioContentDirectory);
    if (!block.OpenFile(fileName, 0))
        return 0;
    if (block.ReadParameterBlock("FullWreckGrunt")) {
        block.GetParameter("NumSounds", 0, &numSounds);
        for (i = 1; i <= numSounds; i++) {
            sprintf(key, "Sound_%d", i);
            block.GetParameter(key, value, -1);
            info = new Rider_Stream_Info;
            // Room for the directory too, though only the name is copied.
            info->m_name = (char *)malloc(dirLength + strlen(value) + 1);
            strcpy(info->m_name, value);
            m_soundInterface->m_snd.PreLoadStream(info->m_name, &info->m_stream);
            if (info->m_stream == -1) {
                sprintf(message, "Invalid FullWreckGrunt %d in %s\n", i, fileName);
#line 1926 "JetskiAudio.cpp"
                ASSERT(0, message);
            }
            m_wreckSounds.AddItem(info);
        }
    }
    if (block.ReadParameterBlock("FullWreckDialogue")) {
        block.GetParameter("NumSounds", 0, &numSounds);
        for (i = 1; i <= numSounds; i++) {
            sprintf(key, "Sound_%d", i);
            block.GetParameter(key, value, -1);
            info = new Rider_Stream_Info;
            // Room for the directory too, though only the name is copied.
            info->m_name = (char *)malloc(dirLength + strlen(value) + 1);
            strcpy(info->m_name, value);
            m_soundInterface->m_snd.PreLoadStream(info->m_name, &info->m_stream);
            if (info->m_stream == -1) {
                sprintf(message, "Invalid FullWreckDialogue %d in %s\n", i, fileName);
#line 1950 "JetskiAudio.cpp"
                ASSERT(0, message);
            }
            m_crashSounds.AddItem(info);
        }
    }
    if (block.ReadParameterBlock("PartialWreck")) {
        block.GetParameter("NumSounds", 0, &numSounds);
        for (i = 1; i <= numSounds; i++) {
            sprintf(key, "Sound_%d", i);
            block.GetParameter(key, value, -1);
            info = new Rider_Stream_Info;
            // Room for the directory too, though only the name is copied.
            info->m_name = (char *)malloc(dirLength + strlen(value) + 1);
            strcpy(info->m_name, value);
            m_soundInterface->m_snd.PreLoadStream(info->m_name, &info->m_stream);
            if (info->m_stream == -1) {
                sprintf(message, "Invalid PartialWreck %d in %s\n", i, fileName);
#line 1974 "JetskiAudio.cpp"
                ASSERT(0, message);
            }
            m_splashSounds.AddItem(info);
        }
    }
    if (block.ReadParameterBlock("Stunt")) {
        block.GetParameter("NumSounds", 0, &numSounds);
        for (i = 1; i <= numSounds; i++) {
            sprintf(key, "Sound_%d", i);
            block.GetParameter(key, value, -1);
            info = new Rider_Stream_Info;
            // Room for the directory too, though only the name is copied.
            info->m_name = (char *)malloc(dirLength + strlen(value) + 1);
            strcpy(info->m_name, value);
            m_soundInterface->m_snd.PreLoadStream(info->m_name, &info->m_stream);
            if (info->m_stream == -1) {
                sprintf(message, "Invalid Stunt %d in %s\n", i, fileName);
#line 1998 "JetskiAudio.cpp"
                ASSERT(0, message);
            }
            m_stuntSounds.AddItem(info);
        }
    }
    if (block.ReadParameterBlock("SmallSplash")) {
        block.GetParameter("NumSounds", 0, &numSounds);
        for (i = 1; i <= numSounds; i++) {
            sprintf(key, "Sound_%d", i);
            block.GetParameter(key, value, -1);
            info = new Rider_Stream_Info;
            // Room for the directory too, though only the name is copied.
            info->m_name = (char *)malloc(dirLength + strlen(value) + 1);
            strcpy(info->m_name, value);
            m_soundInterface->m_snd.PreLoadStream(info->m_name, &info->m_stream);
            if (info->m_stream == -1) {
                sprintf(message, "Invalid SmallSplash %d in %s\n", i, fileName);
#line 2022 "JetskiAudio.cpp"
                ASSERT(0, message);
            }
            m_smallSplashSounds.AddItem(info);
        }
    }
    if (block.ReadParameterBlock("BigSplash")) {
        block.GetParameter("NumSounds", 0, &numSounds);
        for (i = 1; i <= numSounds; i++) {
            sprintf(key, "Sound_%d", i);
            block.GetParameter(key, value, -1);
            info = new Rider_Stream_Info;
            // Room for the directory too, though only the name is copied.
            info->m_name = (char *)malloc(dirLength + strlen(value) + 1);
            strcpy(info->m_name, value);
            m_soundInterface->m_snd.PreLoadStream(info->m_name, &info->m_stream);
            if (info->m_stream == -1) {
                sprintf(message, "Invalid BigSplash %d in %s\n", i, fileName);
#line 2046 "JetskiAudio.cpp"
                ASSERT(0, message);
            }
            m_bigSplashSounds.AddItem(info);
        }
    }
    if (block.ReadParameterBlock("Pass")) {
        block.GetParameter("NumSounds", 0, &numSounds);
        for (i = 1; i <= numSounds; i++) {
            sprintf(key, "Sound_%d", i);
            block.GetParameter(key, value, -1);
            info = new Rider_Stream_Info;
            // Room for the directory too, though only the name is copied.
            info->m_name = (char *)malloc(dirLength + strlen(value) + 1);
            strcpy(info->m_name, value);
            m_soundInterface->m_snd.PreLoadStream(info->m_name, &info->m_stream);
            if (info->m_stream == -1) {
                sprintf(message, "Invalid Pass %d in %s\n", i, fileName);
#line 2070 "JetskiAudio.cpp"
                ASSERT(0, message);
            }
            m_passSounds.AddItem(info);
        }
    }
    if (block.ReadParameterBlock("Passed")) {
        block.GetParameter("NumSounds", 0, &numSounds);
        for (i = 1; i <= numSounds; i++) {
            sprintf(key, "Sound_%d", i);
            block.GetParameter(key, value, -1);
            info = new Rider_Stream_Info;
            // Room for the directory too, though only the name is copied.
            info->m_name = (char *)malloc(dirLength + strlen(value) + 1);
            strcpy(info->m_name, value);
            m_soundInterface->m_snd.PreLoadStream(info->m_name, &info->m_stream);
            if (info->m_stream == -1) {
                sprintf(message, "Invalid Passed %d in %s\n", i, fileName);
#line 2094 "JetskiAudio.cpp"
                ASSERT(0, message);
            }
            m_passedSounds.AddItem(info);
        }
    }
    if (block.ReadParameterBlock("Winning")) {
        block.GetParameter("NumSounds", 0, &numSounds);
        for (i = 1; i <= numSounds; i++) {
            sprintf(key, "Sound_%d", i);
            block.GetParameter(key, value, -1);
            info = new Rider_Stream_Info;
            // Room for the directory too, though only the name is copied.
            info->m_name = (char *)malloc(dirLength + strlen(value) + 1);
            strcpy(info->m_name, value);
            m_soundInterface->m_snd.PreLoadStream(info->m_name, &info->m_stream);
            if (info->m_stream == -1) {
                sprintf(message, "Invalid Win %d in %s\n", i, fileName);
#line 2117 "JetskiAudio.cpp"
                ASSERT(0, message);
            }
            m_winSounds.AddItem(info);
        }
    }
    if (block.ReadParameterBlock("Join")) {
        block.GetParameter("NumSounds", 0, &numSounds);
        for (i = 1; i <= numSounds; i++) {
            sprintf(key, "Sound_%d", i);
            block.GetParameter(key, value, -1);
            info = new Rider_Stream_Info;
            // Room for the directory too, though only the name is copied.
            info->m_name = (char *)malloc(dirLength + strlen(value) + 1);
            strcpy(info->m_name, value);
            m_soundInterface->m_snd.PreLoadStream(info->m_name, &info->m_stream);
            if (info->m_stream == -1) {
                sprintf(message, "Invalid Win %d in %s\n", i, fileName);  // (sic: retail copy-paste)
#line 2140 "JetskiAudio.cpp"
                ASSERT(0, message);
            }
            m_joinSounds.AddItem(info);
        }
    }
    return 1;
}


// ---- src/JetskiRiderSoundEmitter/Tick.cpp



int JetskiRiderSoundEmitter::Tick(float dt)
{
    if (m_voice != -1)
        m_playTime += dt;
    if (m_playing && !m_soundInterface->m_snd.IsSoundPlaying(m_voice, 0)) {
        m_voice = -1;
        m_playing = 0;
        m_playTime = 0;
    }
    return 1;
}


// ---- src/JetskiRiderSoundEmitter/PlayWreckSounds.cpp
#pragma fast_fptosi on


extern "C" int rand();
extern "C" char *strcpy(char *dst, const char *src);
extern "C" int strcmp(const char *a, const char *b);

void StripAudioNumbers(char *name);

struct _D3DVECTOR;



int JetskiRiderSoundEmitter::PlayWreckSounds(_D3DVECTOR *position, int wrecked, int big, int force)
{
    char name[0x110];

    if (wrecked) {
        if (big) {
            int index = (int)((float)(m_wreckSounds.m_numItems - 1) * (4.656612873e-10f * (float)rand()));
            if (m_wreckSounds.m_numItems > 1 && index == m_lastWreckSound)
                index = (index + 1) % m_wreckSounds.m_numItems;
            m_lastWreckSound = index;
            if (force)
                return SwitchSounds(m_wreckSounds.GetItem(index));
            if (m_voice < 0)
                return SwitchSounds(m_wreckSounds.GetItem(index));
            return 1;
        }
        if (m_wreckSounds.m_numItems && m_crashSounds.m_numItems && 4.656612873e-10f * (float)rand() < m_crashChance) {
            if (4.656612873e-10f * (float)rand() <= 0.3) {
                int index = (int)((float)(m_crashSounds.m_numItems - 1) * (4.656612873e-10f * (float)rand()));
                strcpy(name, m_crashSounds.GetItem(index)->m_name);
                StripAudioNumbers(name);
                if (m_crashSounds.m_numItems > 1) {
                    while (!strcmp(name, m_lastCrashSound)) {
                        index = (index + 1) % m_crashSounds.m_numItems;
                        strcpy(name, m_crashSounds.GetItem(index)->m_name);
                        StripAudioNumbers(name);
                    }
                }
                strcpy(m_lastCrashSound, name);
                return SwitchSounds(m_crashSounds.GetItem(index));
            } else {
                int index = (int)((float)(m_wreckSounds.m_numItems - 1) * (4.656612873e-10f * (float)rand()));
                if (m_wreckSounds.m_numItems > 1 && index == m_lastWreckSound)
                    index = (index + 1) % m_wreckSounds.m_numItems;
                m_lastWreckSound = index;
                return SwitchSounds(m_wreckSounds.GetItem(index));
            }
        }
    } else if (m_splashSounds.m_numItems && 4.656612873e-10f * (float)rand() < m_splashChance) {
        int index = (int)((float)(m_splashSounds.m_numItems - 1) * (4.656612873e-10f * (float)rand()));
        if (m_splashSounds.m_numItems > 1 && index == m_lastSplashSound)
            index = (index + 1) % m_splashSounds.m_numItems;
        m_lastSplashSound = index;
        return SwitchSounds(m_splashSounds.GetItem(index));
    }
    return 0;
}
#pragma fast_fptosi reset


// ---- src/JetskiRiderSoundEmitter/PlayStuntSounds.cpp
#pragma fast_fptosi on


extern "C" int rand();
extern "C" char *strcpy(char *dst, const char *src);
extern "C" int strcmp(const char *a, const char *b);

void StripAudioNumbers(char *name);



extern JetskiGame *LpGame;


// Play a random stunt sound, avoiding the one played last time.
int JetskiRiderSoundEmitter::PlayStuntSounds()
{
    char name[0x110];

    if (m_stuntSounds.m_numItems && 4.656612873e-10f * (float)rand() < m_stuntChance
        && LpGame->m_competition->m_raceTime >= 6.0f) {
        int index = (int)((float)(m_stuntSounds.m_numItems - 1) * (4.656612873e-10f * (float)rand()));
        strcpy(name, m_stuntSounds.GetItem(index)->m_name);
        StripAudioNumbers(name);
        if (m_stuntSounds.m_numItems > 1) {
            while (!strcmp(name, m_lastStuntSound)) {
                index = (index + 1) % m_stuntSounds.m_numItems;
                strcpy(name, m_stuntSounds.GetItem(index)->m_name);
                StripAudioNumbers(name);
            }
        }
        strcpy(m_lastStuntSound, name);
        LpGame->m_competition->m_raceTime = 0;
        return SwitchSounds(m_stuntSounds.GetItem(index));
    }
    return 0;
}
#pragma fast_fptosi reset


// ---- src/JetskiRiderSoundEmitter/PlaySplashSounds.cpp
#pragma fast_fptosi on


extern "C" int rand();




// Play a random splash sound, small or big by speed, unless one is playing.
int JetskiRiderSoundEmitter::PlaySplashSounds(float speed)
{
    if (m_voice >= 0 && m_soundInterface->m_snd.IsSoundPlaying(m_voice, 0))
        return 1;
    if (speed < 100.0f) {
        if (m_smallSplashSounds.m_numItems)
            return SwitchSounds(m_smallSplashSounds.GetItem(
            (int)((float)(m_smallSplashSounds.m_numItems - 1) * (4.656612873e-10f * (float)rand()))));
    } else {
        if (m_bigSplashSounds.m_numItems)
            return SwitchSounds(m_bigSplashSounds.GetItem(
            (int)((float)(m_bigSplashSounds.m_numItems - 1) * (4.656612873e-10f * (float)rand()))));
    }
    return 0;
}
#pragma fast_fptosi reset


// ---- src/JetskiRiderSoundEmitter/PlayPassSounds.cpp
#pragma fast_fptosi on


extern "C" int rand();
extern "C" char *strcpy(char *dst, const char *src);
extern "C" int strcmp(const char *a, const char *b);

void StripAudioNumbers(char *name);



extern JetskiGame *LpGame;


// Play a random pass taunt, avoiding the one played last time.
int JetskiRiderSoundEmitter::PlayPassSounds()
{
    char name[0x110];

    if (m_passSounds.m_numItems && 4.656612873e-10f * (float)rand() < m_passChance
        && LpGame->m_competition->m_raceTime >= 6.0f) {
        int index = (int)((float)(m_passSounds.m_numItems - 1) * (4.656612873e-10f * (float)rand()));
        strcpy(name, m_passSounds.GetItem(index)->m_name);
        StripAudioNumbers(name);
        if (m_passSounds.m_numItems > 1) {
            while (!strcmp(name, m_lastPassSound)) {
                index = (index + 1) % m_passSounds.m_numItems;
                strcpy(name, m_passSounds.GetItem(index)->m_name);
                StripAudioNumbers(name);
            }
        }
        strcpy(m_lastPassSound, name);
        LpGame->m_competition->m_raceTime = 0;
        return SwitchSounds(m_passSounds.GetItem(index));
    }
    return 0;
}
#pragma fast_fptosi reset


// ---- src/JetskiRiderSoundEmitter/PlayPassedSounds.cpp
#pragma fast_fptosi on


extern "C" int rand();
extern "C" char *strcpy(char *dst, const char *src);
extern "C" int strcmp(const char *a, const char *b);

void StripAudioNumbers(char *name);



extern JetskiGame *LpGame;


// Play a random "got passed" sound, avoiding the one played last time.
int JetskiRiderSoundEmitter::PlayPassedSounds()
{
    char name[0x110];

    if (m_passedSounds.m_numItems && 4.656612873e-10f * (float)rand() < m_passedChance
        && LpGame->m_competition->m_raceTime >= 6.0f) {
        int index = (int)((float)(m_passedSounds.m_numItems - 1) * (4.656612873e-10f * (float)rand()));
        strcpy(name, m_passedSounds.GetItem(index)->m_name);
        StripAudioNumbers(name);
        if (m_passedSounds.m_numItems > 1) {
            while (!strcmp(name, m_lastPassedSound)) {
                index = (index + 1) % m_passedSounds.m_numItems;
                strcpy(name, m_passedSounds.GetItem(index)->m_name);
                StripAudioNumbers(name);
            }
        }
        strcpy(m_lastPassedSound, name);
        LpGame->m_competition->m_raceTime = 0;
        return SwitchSounds(m_passedSounds.GetItem(index));
    }
    return 0;
}
#pragma fast_fptosi reset


// ---- src/JetskiRiderSoundEmitter/PlayWinSounds.cpp
#pragma fast_fptosi on


extern "C" int rand();


// Play a random one of the win sounds.
int JetskiRiderSoundEmitter::PlayWinSounds()
{
    if (!m_winSounds.m_numItems)
        return 0;
    // rand() / 2^31 is in [0, 1).
    return SwitchSounds(m_winSounds.GetItem(
        (int)((float)(m_winSounds.m_numItems - 1) * (4.656612873e-10f * (float)rand()))));
}
#pragma fast_fptosi reset


// ---- src/JetskiRiderSoundEmitter/PlayJoinSounds.cpp
#pragma fast_fptosi on


extern "C" int rand();


// Play a random one of the join sounds.
int JetskiRiderSoundEmitter::PlayJoinSounds()
{
    if (!m_joinSounds.m_numItems)
        return 0;
    // rand() / 2^31 is in [0, 1).
    return SwitchSounds(m_joinSounds.GetItem(
        (int)((float)(m_joinSounds.m_numItems - 1) * (4.656612873e-10f * (float)rand()))));
}
#pragma fast_fptosi reset


// ---- src/JetskiRiderSoundEmitter/SwitchSounds.cpp
#pragma fast_fptosi on

void PSXprintf(const char *fmt, ...);

static inline float Fabs(float x)
{
    float r;
    asm("abs.s $f8, %1; swc1 $f8, 0(%0)" : : "r"(&r), "f"(x) : "$f8", "memory");
    return r;
}

static inline float Sqrt(float x)
{
    float r;
    asm("sqrt.s $f8, %1; swc1 $f8, 0(%0)" : : "r"(&r), "f"(x) : "$f8", "memory");
    return r;
}






extern JetskiGame *LpGame;





// Stream a rider sound, its volume falling off with the distance (in x/z)
// to the nearest listener; nothing plays beyond 100 units.
int JetskiRiderSoundEmitter::SwitchSounds(Rider_Stream_Info *stream)
{
    if (m_voice >= 0)
        Stop();
    _D3DVECTOR position;
    float nearest = 3.40282347e+38f;
    for (int i = 0; i < LpGame->m_auralScape->m_numListeners; i++) {
        LpGame->m_auralScape->m_listeners[i]->GetPosition(&position);
        JetskiVehicle *rider = m_jetski;
        float dx = Fabs(rider->m_x2C0.x - position.x);
        float distance = dx * dx;
        float dz = Fabs(rider->m_x2C0.z - position.z);
        distance = Sqrt(distance + dz * dz);
        if (distance < nearest)
            nearest = distance;
    }
    if (!(nearest < 100.0f))
        return 0;
    int volume;
    if (nearest <= 70.0f)
        volume = 100;
    else
        volume = (int)(100.0f - 100.0f * ((nearest - 70.0f) / 30.0f));
    if (m_voice < 0) {
        PSXprintf("Free Streams %d\n", m_soundInterface->m_snd.GetFreeStreams());
        if (LpGame->m_gameOptions->m_raceType == 8)
            volume /= 2;
        m_voice = m_soundInterface->m_snd.LoadAndPlayStream(stream->m_name, &m_streamHandle, m_flags, 0, volume, -1, -1, stream->m_stream);
    }
    if (m_voice < 0) {
        m_playing = 0;
        PSXprintf("Failed to LoadAndPlay RiderSound %s !\n", stream->m_name);
        return 0;
    }
    m_playing = 1;
    return 1;
}
#pragma fast_fptosi reset


// ---- src/JetskiRiderSoundEmitter/Stop.cpp



int JetskiRiderSoundEmitter::Stop()
{
    if (m_voice != -1)
        m_soundInterface->m_snd.StopVoice(m_voice, 0, 0);
    m_voice = -1;
    m_playing = 0;
    return 1;
}


// ---- src/JetskiEdgeMonsterSoundEmitter/ctor.cpp

struct Rider_Stream_Info;
class PSXSoundInterface;



JetskiEdgeMonsterSoundEmitter::JetskiEdgeMonsterSoundEmitter(PSXSoundInterface *sound, unsigned int soundGroup, int enabled)
    : GameObject(enabled)
{
    m_streams.Create(1, 16);
    m_voice = -1;
    m_x40 = 0;
    m_soundSystem = sound;
    m_playing = 0;
    m_soundGroup = soundGroup;
    m_currentSound = -1;
}


// ---- src/JetskiEdgeMonsterSoundEmitter/dtor.cpp
struct Rider_Stream_Info;




JetskiEdgeMonsterSoundEmitter::~JetskiEdgeMonsterSoundEmitter()
{
    UnloadSounds();
    m_streams.Clear();
}


// ---- src/JetskiEdgeMonsterSoundEmitter/Create.cpp

JetskiEdgeMonsterSoundEmitter *JetskiEdgeMonsterSoundEmitter::Create(VehicleAudType *audType)
{
    LoadSounds(audType);
    return this;
}


// ---- src/JetskiEdgeMonsterSoundEmitter/UnloadSounds.cpp
extern "C" void free(void *p);



int JetskiEdgeMonsterSoundEmitter::UnloadSounds()
{
    while (m_streams.m_numItems) {
        Rider_Stream_Info *info = m_streams.GetItem(0);
        m_streams.RemoveItemByIndex(0);
        free(info->m_name);
        delete info;
    }
    return 1;
}


// ---- src/JetskiEdgeMonsterSoundEmitter/LoadSounds.cpp

extern "C" unsigned int strlen(const char *s);
extern "C" char *strcpy(char *dst, const char *src);
extern "C" void *malloc(unsigned int size);
extern "C" int sprintf(char *buf, const char *fmt, ...);


extern char AudioContentDirectory[];






int JetskiEdgeMonsterSoundEmitter::LoadSounds(VehicleAudType *audType)
{
    int i;
    Rider_Stream_Info *info;
    int directoryLength = strlen(AudioContentDirectory);
    if (!audType)
        return 0;
    for (i = 1; i <= audType->m_edgeMonsterSounds.m_numItems; i++) {
        info = new Rider_Stream_Info;
        info->m_name = (char *)malloc(directoryLength + strlen(audType->m_edgeMonsterSounds.GetItem(i - 1)) + 1);
        strcpy(info->m_name, audType->m_edgeMonsterSounds.GetItem(i - 1));
        m_soundSystem->m_snd.PreLoadStream(info->m_name, &info->m_stream);
        if (info->m_stream == -1) {
            char message[256];
            sprintf(message, "Invalid EdgeMonster %d\n", i);
#line 2614 "JetskiAudio.cpp"
            ASSERT(0, message);
        }
        m_streams.AddItem(info);
    }
    return 1;
}


// ---- src/JetskiEdgeMonsterSoundEmitter/Tick.cpp

int JetskiEdgeMonsterSoundEmitter::Tick(float dt)
{
    if (m_voice != -1)
        m_x40 += dt;
    if (m_playing) {
        if (!m_soundSystem->m_snd.IsSoundPlaying(m_voice, 0)) {
            m_soundSystem->m_snd.IsSoundPlaying(m_voice, 0);
            m_voice = -1;
            m_playing = 0;
            m_x40 = 0.0f;
        }
    }
    return 1;
}


// ---- src/JetskiEdgeMonsterSoundEmitter/PlayEdgeMonsterSounds.cpp
// Retail's fabs/fmin: abs.s/min.s into $f8, stored through a stack temporary.


struct Rider_Stream_Info;


int JetskiEdgeMonsterSoundEmitter::PlayEdgeMonsterSounds(int sound, float speed)
{
    if (m_streams.m_numItems) {
        switch (sound) {
        case 5:
            speed = Fabs(speed);
            speed = Fmin(speed, 100.0f);
            break;
        default:
            speed = 100.0f;
            break;
        }
        m_currentSound = sound;
        return SwitchSounds(m_streams.GetItem(sound), speed);
    }
    return 0;
}


// ---- src/JetskiEdgeMonsterSoundEmitter/SwitchSounds.cpp
#pragma fast_fptosi on

void PSXprintf(const char *fmt, ...);





int JetskiEdgeMonsterSoundEmitter::SwitchSounds(Rider_Stream_Info *info, float volume)
{
    if (m_voice >= 0)
        Stop();
    int vol = (int)volume;
    if (m_voice < 0) {
        PSXprintf("Free Streams %d\n", m_soundSystem->m_snd.GetFreeStreams());
        m_voice = m_soundSystem->m_snd.LoadAndPlayStream(info->m_name, &m_x3C, m_soundGroup, 0, vol, -1, -1, info->m_stream);
    }
    if (m_voice < 0) {
        m_playing = 0;
        PSXprintf("Failed to LoadAndPlay EdgeMonsterSound %s!\n", info->m_name);
        return 0;
    }
    m_playing = 1;
    return 1;
}
#pragma fast_fptosi reset


// ---- src/JetskiEdgeMonsterSoundEmitter/Stop.cpp



int JetskiEdgeMonsterSoundEmitter::Stop()
{
    if (m_voice != -1)
        m_soundSystem->m_snd.StopVoice(m_voice, 0, 0);
    m_voice = -1;
    m_playing = 0;
    return 1;
}


// ---- src/JetskiEdgeMonsterSoundEmitter/IsPlaying (was in Create.cpp)

int JetskiEdgeMonsterSoundEmitter::IsPlaying()
{
    return m_playing;
}


// ---- src/JetskiUIRiderSoundEmitter/ctor.cpp

class PSXSoundInterface;

JetskiUIRiderSoundEmitter::JetskiUIRiderSoundEmitter(PSXSoundInterface *soundInterface, unsigned int flags, int id)
    : GameObject(id)
{
    m_sounds = 0;
    m_dialogue = 0;
    m_numRiders = 0;
    m_lastSound = -1;
    m_lastDialogue = -1;
    m_voice = -1;
    m_playTime = 0.0f;
    m_soundInterface = soundInterface;
    m_playing = 0;
    m_flags = flags;
}


// ---- src/JetskiUIRiderSoundEmitter/dtor.cpp
extern "C" void free(void *p);





JetskiUIRiderSoundEmitter::~JetskiUIRiderSoundEmitter()
{
    for (int i = 0; i < m_numRiders; i++) {
        while (m_sounds[i].m_numItems) {
            Rider_Stream_Info *stream = m_sounds[i].GetItem(0);
            m_sounds[i].RemoveItemByIndex(0);
            free(stream->m_name);
            delete stream;
        }
        while (m_dialogue[i].m_numItems) {
            Rider_Stream_Info *stream = m_dialogue[i].GetItem(0);
            m_dialogue[i].RemoveItemByIndex(0);
            free(stream->m_name);
            delete stream;
        }
    }
    delete[] m_sounds;
    delete[] m_dialogue;
}


// ---- src/JetskiUIRiderSoundEmitter/Create.cpp

JetskiUIRiderSoundEmitter *JetskiUIRiderSoundEmitter::Create(char *file)
{
    LoadSounds(file);
    return this;
}


// ---- src/JetskiUIRiderSoundEmitter/LoadSounds.cpp

extern "C" int sprintf(char *buf, const char *fmt, ...);
extern "C" char *strcpy(char *dst, const char *src);
extern "C" void *malloc(unsigned int size);







int JetskiUIRiderSoundEmitter::LoadSounds(char *file)
{
    ParameterBlock block;
    char value[0x110];
    char riderName[0x40];
    char key[0x40];
    char message[0x10C];
    int count;
    int i;
    int j;
    Rider_Stream_Info *info;

    if (!block.OpenFile(file, 0))
        return 0;
    if (block.ReadParameterBlock("Rider Information")) {
        block.GetParameter("TotalNumberOfRiders", 0, &m_numRiders);
        m_sounds = new StreamList[m_numRiders];
        m_dialogue = new StreamList[m_numRiders];
        for (i = 0; i < m_numRiders; i++) {
            sprintf(riderName, "Rider %d", i + 1);
            block.ReadParameterBlock(riderName);
            block.GetParameter("ChallengeSounds", 0, &count);
            if (count > 0) {
                m_dialogue[i].Create(count, 16);
                for (j = 1; j <= count; j++) {
                    sprintf(key, "Challenge_%d", j);
                    block.GetParameter(key, value, -1);
                    info = new Rider_Stream_Info;
                    info->m_name = (char *)malloc(strlen(value) + 1);
                    strcpy(info->m_name, value);
                    m_soundInterface->m_snd.PreLoadStream(info->m_name, &info->m_stream);
                    if (info->m_stream == -1) {
                        sprintf(message, "Invalid ChallengeSounds %d in rider %d in %s\n", j, i + 1, file);
#line 2822 "JetskiAudio.cpp"
                        ASSERT(0, message);
                    }
                    m_dialogue[i].AddItem(info);
                }
            }
            block.GetParameter("SelectSounds", 0, &count);
            if (count > 0) {
                m_sounds[i].Create(count, 16);
                for (j = 1; j <= count; j++) {
                    sprintf(key, "Select_%d", j);
                    block.GetParameter(key, value, -1);
                    info = new Rider_Stream_Info;
                    info->m_name = (char *)malloc(strlen(value) + 1);
                    strcpy(info->m_name, value);
                    m_soundInterface->m_snd.PreLoadStream(info->m_name, &info->m_stream);
                    if (info->m_stream == -1) {
                        sprintf(message, "Invalid SelectSounds %d in rider %d in %s\n", j, i + 1, file);
#line 2845 "JetskiAudio.cpp"
                        ASSERT(0, message);
                    }
                    m_sounds[i].AddItem(info);
                }
            }
        }
    }
    return 1;
}


// ---- src/JetskiUIRiderSoundEmitter/PlayDialogue.cpp
#pragma fast_fptosi on

extern "C" int rand();

struct Rider_Stream_Info;

// rand() / 2^31, in [0, 1).
static inline float RandomUnit()
{
    return 4.656612873e-10f * (float)rand();
}



// Play a random clip of the rider's, not the one played last time.
int JetskiUIRiderSoundEmitter::PlayDialogue(int rider, int type)
{
    Rider_Stream_Info *stream;
    int index;

    if (rider >= m_numRiders)
        return 0;
    switch (type) {
    case 0:
        if (!m_sounds[rider].m_numItems)
            return 0;
        index = (int)((float)(m_sounds[rider].m_numItems - 1) * RandomUnit());
        if (m_sounds[rider].m_numItems > 1 && index == m_lastSound)
            index = (index + 1) % m_sounds[rider].m_numItems;
        stream = m_sounds[rider].GetItem(index);
        m_lastSound = index;
        break;
    case 1:
        if (!m_dialogue[rider].m_numItems)
            return 0;
        index = (int)((float)(m_dialogue[rider].m_numItems - 1) * RandomUnit());
        if (m_dialogue[rider].m_numItems > 1 && index == m_lastDialogue)
            index = (index + 1) % m_dialogue[rider].m_numItems;
        stream = m_dialogue[rider].GetItem(index);
        m_lastDialogue = index;
        break;
    default:
        return 0;
    }
    return SwitchSounds(stream);
}
#pragma fast_fptosi reset


// ---- src/JetskiUIRiderSoundEmitter/Tick.cpp

int JetskiUIRiderSoundEmitter::Tick(float dt)
{
    if (m_voice != -1)
        m_playTime += dt;
    if (m_playing && !m_soundInterface->m_snd.IsSoundPlaying(m_voice, 0)) {
        m_voice = -1;
        m_playing = 0;
        m_playTime = 0.0f;
    }
    return 1;
}


// ---- src/JetskiUIRiderSoundEmitter/SwitchSounds.cpp
void PSXprintf(const char *fmt, ...);



int JetskiUIRiderSoundEmitter::SwitchSounds(Rider_Stream_Info *info)
{
    if (m_voice >= 0)
        Stop();
    if (m_voice < 0)
        m_voice = m_soundInterface->m_snd.LoadAndPlayStream(info->m_name, &m_stream, m_flags, 0, -1, -1, -1, info->m_stream);
    if (m_voice < 0) {
        m_playing = 0;
        PSXprintf("Failed to LoadAndPlay UIRiderSound %s!\n", info->m_name);
        return 0;
    }
    m_playing = 1;
    return 1;
}


// ---- src/JetskiUIRiderSoundEmitter/Stop.cpp

int JetskiUIRiderSoundEmitter::Stop()
{
    if (m_voice != -1)
        m_soundInterface->m_snd.StopVoice(m_voice, 0, 0);
    m_voice = -1;
    m_playing = 0;
    return 1;
}


// ---- src/JetskiCrowdSoundEmitter/ctor.cpp

class AuralScape;
class PSXSoundGroup;




// Retail keeps `this` in a stack slot and reloads it for every use: the
// signature of a function containing inline asm (the global optimizer is
// off). The empty asm statement reproduces that.
JetskiCrowdSoundEmitter::JetskiCrowdSoundEmitter(AuralScape *scape, PSXSoundGroup *group, short type, int enabled)
    : SoundEmitter(scape, group, type, enabled)
{
    asm("");
    m_cheerSounds.Create(1, 16);
    m_booSounds.Create(1, 16);
    m_ohhSounds.Create(1, 16);
}


// ---- src/JetskiCrowdSoundEmitter/dtor.cpp
class PSXSound;




JetskiCrowdSoundEmitter::~JetskiCrowdSoundEmitter()
{
    m_cheerSounds.Clear();
    m_booSounds.Clear();
    m_ohhSounds.Clear();
}


// ---- src/JetskiCrowdSoundEmitter/Create.cpp

// Heap shim, kept as it was: without it
// Create loads 20.0f before 100.0f (4 words differ). Image object in .mwccshim. (mwcc
// stops with a syntax error on the old comment text here.)
struct MwccHeapImage {
    char pad0[0x73D];
    char flag0;
};
#pragma section mwccshim begin
inline void MwccHeapShim() { const MwccHeapImage image = { {0}, 1 }; }
#pragma section mwccshim end
JetskiCrowdSoundEmitter *JetskiCrowdSoundEmitter::Create(char *name)
{
    _DS3DBUFFER buffer;
    LoadSounds(name);
    float minDistance = 20.0f;
    float maxDistance = 100.0f;    // with the shared headers, the plain order loads 100 first (the old local copy needed it declared before LoadSounds)
    return (JetskiCrowdSoundEmitter *)SoundEmitter::Create(buffer, 2, minDistance, maxDistance, 0, 0);
}


// ---- src/JetskiCrowdSoundEmitter/LoadSounds.cpp
extern "C" int sprintf(char *buf, const char *fmt, ...);


class PSXSound;
class PSXSoundGroup;


#define LOAD_SOUND_LIST(block, list)                                                    \
    if (params.ReadParameterBlock(block)) {                                             \
        params.GetParameter("NumSounds", 0, &numSounds);                                \
        for (i = 1; i <= numSounds; i++) {                                              \
            sprintf(key, "Sound_%d", i);                                                \
            params.GetParameter(key, name, -1);                                         \
            sound = m_system->m_soundInterface->GetSoundFromResourceName(m_group, name, 1, 1); \
            if (sound)                                                                  \
                list.AddItem(sound);                                                    \
        }                                                                               \
    }

int JetskiCrowdSoundEmitter::LoadSounds(char *file)
{
    PSXSound *sound = 0;
    ParameterBlock params;
    char name[0x110];
    char key[0x4C];
    int numSounds;
    int i;
    if (!params.OpenFile(file, 0))
        return 0;
    LOAD_SOUND_LIST("Cheer", m_cheerSounds)
    LOAD_SOUND_LIST("Boo", m_booSounds)
    LOAD_SOUND_LIST("Oh", m_ohhSounds)
    return SetActiveSound(sound);
}


// ---- src/JetskiCrowdSoundEmitter/PlayCheerSound.cpp
#pragma fast_fptosi on


extern "C" int rand();





// Plays a random one of the sounds unless one is still playing.
int JetskiCrowdSoundEmitter::PlayCheerSound()
{
    PSXSound *active = GetActiveSound();
    if (active && active->IsPlaying())
        return 0;
    if (m_cheerSounds.m_numItems) {
        int index = (m_cheerSounds.m_numItems - 1) * ((1.0f / 2147483648.0f) * rand());
        return SwitchSounds(m_cheerSounds.GetItem(index));
    }
    return 0;
}
#pragma fast_fptosi reset


// ---- src/JetskiCrowdSoundEmitter/PlayBooSound.cpp
#pragma fast_fptosi on


extern "C" int rand();





// Plays a random one of the sounds unless one is still playing.
int JetskiCrowdSoundEmitter::PlayBooSound()
{
    PSXSound *active = GetActiveSound();
    if (active && active->IsPlaying())
        return 0;
    if (m_booSounds.m_numItems) {
        int index = (m_booSounds.m_numItems - 1) * ((1.0f / 2147483648.0f) * rand());
        return SwitchSounds(m_booSounds.GetItem(index));
    }
    return 0;
}
#pragma fast_fptosi reset


// ---- src/JetskiCrowdSoundEmitter/PlayOhhSound.cpp
#pragma fast_fptosi on


extern "C" int rand();





// Plays a random one of the sounds unless one is still playing.
int JetskiCrowdSoundEmitter::PlayOhhSound()
{
    PSXSound *active = GetActiveSound();
    if (active && active->IsPlaying())
        return 0;
    if (m_ohhSounds.m_numItems) {
        int index = (m_ohhSounds.m_numItems - 1) * ((1.0f / 2147483648.0f) * rand());
        return SwitchSounds(m_ohhSounds.GetItem(index));
    }
    return 0;
}
#pragma fast_fptosi reset


// ---- src/JetskiCrowdSoundEmitter/SwitchSounds.cpp

struct _D3DVECTOR;




int JetskiCrowdSoundEmitter::SwitchSounds(PSXSound *sound)
{
    Stop();
    if (!SetActiveSound(sound))
        return 0;
    sound->SetBaseVolume(100.0f);
    return sound->Play(1, 0);
}


// ---- src/JetskiDialogueStreamer/ctor.cpp

JetskiDialogueStreamer::JetskiDialogueStreamer(PSXSoundInterface *soundInterface, int enabled)
    : GameObject(enabled)
{
    m_voice = -1;
    m_lines = 0;
    m_active = 0;
    m_x50 = 0;
    m_track = -1;
    m_time = 0;
    m_system = soundInterface;
    m_playing = 0;
    m_paused = 0;
}


// ---- src/JetskiDialogueStreamer/dtor.cpp

extern "C" void free(void *p);



JetskiDialogueStreamer::~JetskiDialogueStreamer()
{
    if (m_voice >= 0)
        m_system->m_snd.StopVoice(m_voice, 1, 0);
    if (m_lines)
        free(m_lines);
}


// ---- src/JetskiDialogueStreamer/Create.cpp

extern "C" void *malloc(unsigned int size);
extern "C" int sprintf(char *buf, const char *fmt, ...);





JetskiDialogueStreamer *JetskiDialogueStreamer::Create(int numLines, char *directory)
{
    m_numLines = numLines;
    m_lines = (DialogueLine *)malloc(m_numLines * sizeof(DialogueLine));
    for (int i = 1; i <= m_numLines; i++) {
        sprintf(m_lines[i - 1].m_name, "%sline%02d.vag", directory, i);
        m_system->m_snd.PreLoadStream(m_lines[i - 1].m_name, &m_lines[i - 1].m_stream);
        if (m_lines[i - 1].m_stream == -1) {
#line 3225 "JetskiAudio.cpp"
            ASSERTF(0, ("Invalid dialogue %s", m_lines[i - 1].m_name));
        }
    }
    m_active = 1;
    return this;
}


// ---- src/JetskiDialogueStreamer/Play.cpp
void PSXprintf(const char *fmt, ...);





int JetskiDialogueStreamer::Play(int track, int interrupt)
{
    if (!m_active) {
        PSXprintf("Dialogue Not Ready!\n");
        return 0;
    }
    if (m_voice >= 0 && interrupt)
        Stop();
    else if (m_voice >= 0)
        return 1;
    if (m_track == track && !interrupt)
        return 1;
    if (track <= m_numLines) {
        if (m_voice < 0)
            m_voice = m_system->m_snd.LoadAndPlayStream(m_lines[track - 1].m_name, &m_x48, 1, 0, -1, -1, -1,
                                                        m_lines[track - 1].m_stream);
        if (m_voice < 0) {
            m_x50 = 0;
            m_playing = 0;
            PSXprintf("Failed to Load Dialogue %s (track %d) !\n", m_lines[track - 1].m_name, track);
            return 0;
        }
        m_x50 = 1;
        m_playing = 1;
    } else {
        PSXprintf("Cannot play dialogue track %d.\n", track);
        return 0;
    }
    m_track = track;
    return 1;
}


// ---- src/JetskiDialogueStreamer/Stop.cpp


int JetskiDialogueStreamer::Stop()
{
    if (!m_active)
        return 0;
    if (m_voice != -1)
        m_system->m_snd.StopVoice(m_voice, 1, 0);
    m_voice = -1;
    m_playing = 0;
    m_x50 = 0;
    m_track = -1;
    return 1;
}


// ---- src/JetskiDialogueStreamer/Pause.cpp

int JetskiDialogueStreamer::Pause(int pause)
{
    if (!m_active)
        return 0;
    if (m_voice == -1)
        return 0;
    if (pause) {
        if (m_paused)
            return 1;
        m_system->m_snd.PauseVoice(m_voice);
        m_paused = 1;
        return 1;
    }
    if (!m_paused)
        return 1;
    m_system->m_snd.PauseVoice(m_voice);
    m_paused = 0;
    return 1;
}


// ---- src/JetskiDialogueStreamer/Tick.cpp


int JetskiDialogueStreamer::Tick(float dt)
{
    if (m_voice != -1)
        m_time += dt;
    if (m_playing && !m_system->m_snd.IsSoundPlaying(m_voice, 0)) {
        m_voice = -1;
        m_playing = 0;
        m_x50 = 0;
        m_time = 0;
        m_track = -1;
    }
    return 1;
}


// ---- src/global/StripAudioNumbers.cpp
extern "C" unsigned int strcspn(const char *s, const char *reject);

// Cut a sound name at its first digit ("Splash03" -> "Splash").
void StripAudioNumbers(char *name)
{
    unsigned int n = strcspn(name, "0123456789");
    if (n)
        name[n] = 0;
}

