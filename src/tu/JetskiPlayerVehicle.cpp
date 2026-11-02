// JetskiPlayerVehicle.cpp  @ 0x00254D80 (text 0x254D80-0x255A40)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's JetskiPlayerVehicle.cpp as one translation unit: its functions in
// retail order, then its static initializer. `make link` places the object's
// data as well as its code (tools/link_c.py). JetskiVehicle's
// RSSetResetParameters and RSGetVehicleStatus are defined here, and the
// vtable brings the eight @48@ thunks into the RaceStatusVehicleInterface
// part. The old files' views map onto the shared headers: Tick's "rider
// options" are m_competition->m_raceStatus, its steer/throttle axes
// m_axis[0]/[1], its max-performance sound m_stuntSound, its camera position
// the wreck camera's look point of the player's camera controller;
// LoadAudio's "replay" flag is m_cheats->fullPerformance.



#include "GameObject/GameObject.h"
#include "RaceStatusVehicleInterface/RaceStatusVehicleInterface.h"
#include "RaceStatusClass/RaceStatusClass.h"
#include "JetskiPlayerVehicle/JetskiPlayerVehicle.h"
#include "AnalogAxis/AnalogAxis.h"
#include "JoystickDevice/JoystickDevice.h"
#include "ControlInterface/ControlInterface.h"
#include "_DS3DBUFFER/_DS3DBUFFER.h"
#include "PSXSound/PSXSound.h"
#include "AuralScape/AuralScape.h"
#include "JetskiGame/JetskiGame.h"
#include "JetskiGameOptions/JetskiGameOptions.h"
#include "CheatFlags/CheatFlags.h"
#include "JetskiCompetition/JetskiCompetition.h"
#include "JetskiCameraController/JetskiCameraController.h"
#include "JetskiWreckCamera/JetskiWreckCamera.h"
#include "Scene/Scene.h"
#include "SoundEmitter/SoundEmitter.h"
#include "JetskiEngineSoundEmitter/JetskiEngineSoundEmitter.h"
#include "d3dtypes.h"
#include "KeyInfo/KeyInfo.h"
#include "Water/Water.h"
#include "PSXWater/PSXWater.h"
#include "JetskiVehicle/JetskiVehicle.h"
#include "PhysicsRigidBody/PhysicsRigidBody.h"
#include "PSXRenderableObject/PSXRenderableObject.h"
// The axis vectors every Soultree TU defines (and __sinit constructs).
#include "sinit/AxisVectors.h"

void PSXprintf(const char *fmt, ...);
extern JetskiGame *LpGame;

JetskiPlayerVehicle::JetskiPlayerVehicle(int playerIndex)
    : JetskiVehicle(playerIndex)
{
    m_x584 = 0;
    m_axis[0] = 0;
    m_axis[1] = 0;
    m_axis[2] = 0;
    m_axis[3] = 0;
    m_stuntSound = 0;
    m_gateSound = 0;
    m_wreckSound = 0;
}

JetskiPlayerVehicle::~JetskiPlayerVehicle()
{
    if (m_gateSound)
        m_gateSound->Release();
    if (m_axis[0])
        delete m_axis[0];
    if (m_axis[1])
        delete m_axis[1];
    if (m_axis[2])
        delete m_axis[2];
    if (m_axis[3])
        delete m_axis[3];
}

// Session shim. mwcc never initialises the "has a call" byte of a constant's
// expression node, so it keeps whatever an earlier declaration left in the
// parser's heap, and FunctionCalls.c evaluates arguments with that byte set
// first. Retail's earlier code left it set for -1 (axes 0 and 1), 0.24 (axis 0),
// 0 (axis 2) and 1, 0 (axis 3), which is why those constants are loaded into
// $f20/$f21 before NewAxisID. This unused inline function's initializer rewrites
// that heap region: zeros, with a 1 at the flag byte of each of those nodes. No
// code; its image object goes to .mwccshim (mwcc_shim.h), which the link
// discards. Still needed in the whole TU (80 words off without it); recalibrated
// for it (the tail zeroes the rest of Create's epoch).


struct MwccHeapImage {
    char pad0[0x1C3D];
    char flag0;
    char pad1[0x3B7];
    char flag1;
    char pad2[0x76F];
    char flag2;
    char pad3[0xBA7];
    char flag3;
    char pad4[0xAE7];
    char flag4;
    char pad5[0x3F];
    char flag5;
    char tail[0x2B0A];
};
#pragma section mwccshim begin
inline void MwccHeapShim() { const MwccHeapImage image = { {0}, 1, {0}, 1, {0}, 1, {0}, 1, {0}, 1, {0}, 1 }; }
#pragma section mwccshim end

int JetskiPlayerVehicle::Create(JetskiCompetition *competition, char *a, char *b, char *c, char *d, char *e,
                                LightManager *lights, ParticleManager *particles, _D3DVECTOR f, _D3DVECTOR g,
                                Terrain *terrain, PSXWater *water, int player, int i, Scene *scene)
{
    m_input = LpGame->m_controls->m_joysticks[player];
    if (!m_input) {
        PSXprintf("Can't find joystick!! Failed JetskiPlayerVehicle::SetupControls\n");
        return 0;
    }
    m_axis[0] = new AnalogAxis(-1.0f, 1.0f, 0.0f, LpGame->m_controls->NewAxisID(), (AxisType)0, 0.24f);
    m_axis[1] = new AnalogAxis(-1.0f, 1.0f, 0.0f, LpGame->m_controls->NewAxisID(), (AxisType)1, 0.24f);
    m_axis[2] = new AnalogAxis(-1.0f, 1.0f, 0.0f, LpGame->m_controls->NewAxisID(), (AxisType)2, 0.33f);
    m_axis[3] = new AnalogAxis(-1.0f, 1.0f, 0.0f, LpGame->m_controls->NewAxisID(), (AxisType)5, 0.24f);
    ((JoystickDevice *)m_input)->AttachAxis(m_axis[0], 0.01f, 0.01f);
    ((JoystickDevice *)m_input)->AttachAxis(m_axis[1], 0.01f, 0.01f);
    ((JoystickDevice *)m_input)->AttachAxis(m_axis[2], 0.01f, 0.01f);
    ((JoystickDevice *)m_input)->AttachAxis(m_axis[3], 0.01f, 0.01f);
    SetupControls();
    m_player = player;
    // The three-float constructor: retail stores x, y, z (the default one stores z, y, x).
    JetskiVehicle::Create(competition, a, b, c, d, e, f, g, _D3DVECTOR(0.0f, 0.0f, 0.0f), _D3DVECTOR(0.0f, 0.0f, 0.0f),
                          lights, particles, terrain, water, i, scene);
    LoadAudio();
    return (int)this;
}

int JetskiPlayerVehicle::LoadAudio()
{
    _DS3DBUFFER buffer;
    if (LpGame->m_gameOptions->m_raceType != 5 && LpGame->m_gameOptions->m_raceType != 4 && !LpGame->m_cheats->fullPerformance) {
        m_stuntSound = new PSXSound(LpGame->m_auralScape->m_soundInterface, m_data->m_soundGroup, 1);
        if (!m_stuntSound->CreateFromFile("stuntm01.wav", 1, 3)) {
            m_stuntSound->Release();
            m_stuntSound = 0;
        } else {
            m_stuntSound->SetVolume(100.0f);
        }
        m_wreckSound = new PSXSound(LpGame->m_auralScape->m_soundInterface, m_data->m_soundGroup, 1);
        if (!m_wreckSound->CreateFromFile("dedstik.wav", 1, 3)) {
            m_wreckSound->Release();
            m_wreckSound = 0;
        } else {
            m_wreckSound->SetVolume(100.0f);
        }
    }
    m_gateSound = (SoundEmitter *)(new SoundEmitter(LpGame->m_auralScape, m_data->m_soundGroup, 10, 1))
                            ->Create(buffer, 2, 20.0f, 100.0f, 0, 0);
    if (m_gateSound)
        m_gateSound->LoadSound("Waypoint\\waypnt01.wav", 0);
    return 1;
}

void JetskiPlayerVehicle::TriggerWreck(WreckType type)
{
    // Plain float truth test: retail compares c.eq.s m_performance, 0 (an explicit
    // "!= 0.0f" swaps the c.eq.s operands).
    if (m_performance) {
        if (m_wreckSound)
            m_wreckSound->Play(0, 0);
    }
    JetskiVehicle::TriggerWreck(type);
}

void JetskiPlayerVehicle::StopWreck(int x)
{
    JetskiVehicle::StopWreck(x);
    if (m_engineSound)
        m_engineSound->PlayEngineResetSound();
}

int JetskiPlayerVehicle::RSGatePassedCallback(RaceStatusClass *rs, GateClass *gate)
{
    if (m_gateSound)
        m_gateSound->Start();
    JetskiVehicle::RSGatePassedCallback(rs, gate);
}

#ifdef SPLASHDOWN_PC
// PORT_AUTOPILOT (ports/pc/input.cpp): steer player N toward the point 40 units
// ahead on the track (as VehicleAI finds it), throttle held, for scripted
// test runs (career, time trials).
#include "TrackType/TrackType.h"
extern "C" float atan2f(float y, float x);
extern "C" int port_autopilot(int player);
static void PortAutopilot(JetskiPlayerVehicle *v)
{
    RaceStatusStruct_Tag *status = v->m_ownStatus;
    TrackType *track = v->m_competition ? v->m_competition->m_track : 0;
    if (!status || !v->m_renderable || !track)
        return;
    TrackPosition_Tag from = status->m_trackPosition;
    if (!from.m_segment || !from.m_data)
        from = status->m_lastTrackPosition;
    if (!from.m_segment || !from.m_data)
        return;
    _D3DMATRIX &m = v->m_renderable->m_worldMatrix;
    _D3DVECTOR position(m._41, m._42, m._43), target;
    TrackPosition_Tag next = from, closest;
    static float random[10];
    if (!track->GetNextTrackPosition(from, &next, position, &closest, 40.0f, 0.0f, random) ||
        !track->CoordinatesFromNode(&next, &target))
        return;
    float dx = target.x - position.x, dz = target.z - position.z;
    float fx = m._31, fz = m._33;
    float angle = atan2f(fx * dz - fz * dx, fx * dx + fz * dz);
    float steer = -angle * 2.0f;
    v->m_steer = steer < -1.0f ? -1.0f : steer > 1.0f ? 1.0f : steer;
    v->m_throttle = 1.0f;
}
#endif

int JetskiPlayerVehicle::Tick(float dt)
{
    KeyInfo key;
    float oldPerformance = m_performance;
    int boost = 0;
    if (m_competition->m_raceStatus->m_flags & 1)
        boost = ((JoystickDevice *)m_input)->IsKeyDown((AssignableActionIndex)0x10, 0x3F, &key);
    if (((JoystickDevice *)m_input)->IsKeyDown((AssignableActionIndex)2, 0x3F, &key))
        m_throttle = -1.0f;
    else if (((JoystickDevice *)m_input)->IsKeyDown((AssignableActionIndex)3, 0x3F, &key))
        m_throttle = 1.0f;
    else
        m_throttle = m_axis[1]->m_value;
    if (((JoystickDevice *)m_input)->IsKeyDown((AssignableActionIndex)0, 0x3F, &key))
        m_steer = -1.0f;
    else if (((JoystickDevice *)m_input)->IsKeyDown((AssignableActionIndex)1, 0x3F, &key))
        m_steer = 1.0f;
    else
        m_steer = m_axis[0]->m_value;
#ifdef SPLASHDOWN_PC
    if (port_autopilot(m_player))
        PortAutopilot(this);
#endif
    if (boost)
        m_boost = 1.0f;
    else
        m_boost = 0.0f;
    if (!m_x530) {
        m_water->ScrollWakePatch(m_player, m_body->m_x60);
    } else {
        _D3DVECTOR position;
        if (LpGame->m_gameOptions->m_raceType == 5)
            position = LpGame->m_competition->m_cameraControllers[0]->m_wreckCamera->m_lookPoint;
        else
            position = LpGame->m_competition->m_cameraControllers[m_player]->m_wreckCamera->m_lookPoint;
        m_water->ScrollWakePatch(m_player, position);
    }
    JetskiVehicle::Tick(dt);
    if (m_performance > oldPerformance) {
        if (!(m_performance < 1.0f) && oldPerformance < 1.0f && m_stuntSound)
            m_stuntSound->Play(0, 0);
    }
    TestForImpact(dt);
    return 1;
}

int JetskiPlayerVehicle::SetupControls()
{
    return 1;
}

int JetskiPlayerVehicle::SetCompetition(JetskiCompetition *competition)
{
    m_competition = competition;
    return 1;
}

int JetskiVehicle::RSSetResetParameters(_D3DVECTOR *position, _D3DVECTOR *orientation)
{
    if (position)
        m_resetPosition = *position;
    if (orientation)
        m_resetOrientation = *orientation;
    return 1;
}

int JetskiVehicle::RSGetVehicleStatus(_D3DVECTOR *position, _D3DVECTOR *orientation, float *speed, int *flags)
{
    if (position) {
        position->x = m_renderable->m_worldMatrix._41;
        position->y = m_renderable->m_worldMatrix._42;
        position->z = m_renderable->m_worldMatrix._43;
    }
    if (orientation)
        *orientation = m_body->m_velocity;
    if (speed)
        *speed = m_body->m_speed;
    if (flags) {
        if (m_body->m_x30)
            *flags &= ~1;
        else
            *flags |= 1;
    }
    return 1;
}
