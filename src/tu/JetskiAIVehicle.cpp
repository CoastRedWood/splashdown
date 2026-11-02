// JetskiAIVehicle.cpp  @ 0x002B17A0 (text 0x2B17A0-0x2B2B70)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's JetskiAIVehicle.cpp as one translation unit: JetskiAIVehicle's
// methods, VehicleAIInterface's default virtuals, then the static
// initializer. `make link` places the object's data as well as its code
// (tools/link_c.py). The rest of the tu_map row (TextureMapList, the IPU
// cache) opens PSXTextureMapManager.cpp (the TU map). The C-mode stubs
// (ResetPosition, ActivateAIStunt) are members; the local views map to
// JetskiVehicle's m_renderable (world matrix), m_body, m_highSpeed, m_rider,
// m_ownStatus and m_data (the Scene: m_sceneName).



// fast_fptosi is GenerateRandomStunt's.
#pragma fast_fptosi on

#include "JetskiAIVehicle/AIVehicle.h"
#include "d3dtypes.h"
#include "JetskiVehicle/JetskiVehicle.h"
#include "VehicleAIInterface/VehicleAIInterface.h"
#include "AnalogAxis/AnalogAxis.h"
#include "JoystickDevice/JoystickDevice.h"
#include "JetskiGame/JetskiGame.h"
#include "JetskiGameOptions/JetskiGameOptions.h"
#include "SoultreeObject/SoultreeObject.h"
#include "PhysicsBody/PhysicsBody.h"
#include "PhysicsRigidBody/PhysicsRigidBody.h"
#include "JetskiAIVehicle/JetskiAIVehicle.h"
#include "ControlInterface/ControlInterface.h"
#include "CheatFlags/CheatFlags.h"
#include "VehicleAIClass/VehicleAIClass.h"
#include "JetskiCompetition/JetskiCompetition.h"
#include "PSXRenderableObject/PSXRenderableObject.h"
#include "JetskiRider/JetskiRider.h"
#include "Scene/Scene.h"
#include "sinit/AxisVectors.h"

void PSXprintf(const char *fmt, ...);
extern "C" int rand();
extern "C" int sprintf(char *buf, const char *fmt, ...);

extern JetskiGame *LpGame;
extern char SceneContentDirectory[];

// Retail's reciprocal square root is an inline-asm helper (src/global/Abs.cpp).
static inline float RSqrt(float x)
{
    float r;
    asm("rsqrt.s $f8, %0, %1; swc1 $f8, 0(%2)" : : "f"(1.0f), "f"(x), "r"(&r) : "$f8", "memory");
    return r;
}

static inline float Magnitude(const _D3DVECTOR &v)
{
    float len2 = v.x * v.x + v.y * v.y + v.z * v.z;
    if (len2 == 0.0f)
        return 0.0f;
    return 1.0f / RSqrt(len2);
}

// ---- src/JetskiAIVehicle/ctor.cpp

JetskiAIVehicle::JetskiAIVehicle(int id) : JetskiVehicle(id)
{
    m_x584 = 1;
    m_ai = 0;
    m_x538 += 1.0f;
    m_axes[0] = 0;
    m_axes[1] = 0;
    m_axes[2] = 0;
    m_axes[3] = 0;
}


// ---- src/JetskiAIVehicle/dtor.cpp
// (The vtable, and with it the eight @1968@ VehicleAIInterface thunks, is emitted in this file.)


JetskiAIVehicle::~JetskiAIVehicle()
{
    if (m_axes[0]) delete m_axes[0];
    if (m_axes[1]) delete m_axes[1];
    if (m_axes[2]) delete m_axes[2];
    if (m_axes[3]) delete m_axes[3];
}


// ---- src/JetskiAIVehicle/Create.cpp
// Create__15JetskiAIVehicleFP17JetskiCompetitionPcPcPcPcPcP12LightManagerP15ParticleManager10_D3DVECTOR10_D3DVECTORP7TerrainP8PSXWateriP5SceneP15RaceStatusClassP9TrackType @ 0x002B1900
// The constant 4 (number of colliders) is a named local initialised at function entry, as retail wrote it: live across every call in the function, it loses the callee-saved registers
// and is spilled, and a spilled constant is rematerialised (its li is re-inserted right before the argument store, after the pre-RA scheduler), which is why retail's li sits between `sd
// &offset` and its own sd. Defined later (after the AnalogAxis block, or next to the call) it gets $s0 or $t7 instead. Direct3D's one-float _D3DVECTOR ctor builds the (3,3,3) temporary;
// the heap-image shim flags axis 0's 1.0 and axis 2's -1, recalibrated for the extra declaration.







// Session shim, recalibrated for this TU with r18 image.py; the 0x30000-byte image makes
// mwcc put it in a new first heap block.
struct MwccHeapImage {
    char pad0[0x3A2D];
    char flag0;
    char pad1[0x160F];
    char flag1;
    char tail[0x2AFC2];
};
#pragma section mwccshim begin
inline void MwccHeapShim() { const MwccHeapImage image = { {0}, 1, {0}, 1 }; }
#pragma section mwccshim end
JetskiAIVehicle *JetskiAIVehicle::Create(JetskiCompetition *competition, char *a, char *b, char *c, char *d, char *e,
                                         LightManager *lights, ParticleManager *particles, _D3DVECTOR f,
                                         _D3DVECTOR g, Terrain *terrain, PSXWater *water, int i, Scene *scene,
                                         RaceStatusClass *race, TrackType *track)
{
    _D3DVECTOR position;
    _D3DVECTOR velocity;
    _D3DVECTOR forward;
    _D3DVECTOR up;
    _D3DVECTOR scale;
    _D3DVECTOR offset;
    float x254 = 1.0f;
    CollisionObject *colliders[8];
    int numColliders = 4;
    float x258;
    float x25C;
    float x260;
    float x264;
    float x268;
    float x26C;
    float x270;
    float x274;
    float x278;
    float speed;
    float fa, fb, fc;
    int ia, ib, ic;

    if (!competition->m_x118 && !i) {
        m_input = LpGame->m_controls->m_joysticks[0];
        if (!m_input) {
            PSXprintf("Can't find joystick!! Failed JetskiAIVehicle\n");
            return 0;
        }
        m_axes[0] = new AnalogAxis(-1.0f, 1.0f, 0.0f, LpGame->m_controls->NewAxisID(), (AxisType)0, 0.24f);
        m_axes[1] = new AnalogAxis(-1.0f, 1.0f, 0.0f, LpGame->m_controls->NewAxisID(), (AxisType)1, 0.24f);
        m_axes[2] = new AnalogAxis(-1.0f, 1.0f, 0.0f, LpGame->m_controls->NewAxisID(), (AxisType)2, 0.33f);
        m_axes[3] = new AnalogAxis(-1.0f, 1.0f, 0.0f, LpGame->m_controls->NewAxisID(), (AxisType)5, 0.24f);
        m_input->AttachAxis(m_axes[0], 0.01f, 0.01f);
        m_input->AttachAxis(m_axes[1], 0.01f, 0.01f);
        m_input->AttachAxis(m_axes[2], 0.01f, 0.01f);
        m_input->AttachAxis(m_axes[3], 0.01f, 0.01f);
    }
    m_data = scene;
    JetskiVehicle::Create(competition, a, b, c, d, e, f, g, _D3DVECTOR(0.0f, 0.0f, 0.0f), _D3DVECTOR(0.0f, 0.0f, 0.0f),
                          lights, particles, terrain, water, i, scene);
    position.x = m_renderable->m_worldMatrix._41;
    position.y = m_renderable->m_worldMatrix._42;
    position.z = m_renderable->m_worldMatrix._43;
    velocity = m_body->m_velocity;
    speed = Magnitude(velocity);
    forward.x = m_renderable->m_worldMatrix._31;
    forward.y = m_renderable->m_worldMatrix._32;
    forward.z = m_renderable->m_worldMatrix._33;
    up.x = m_renderable->m_worldMatrix._21;
    up.y = m_renderable->m_worldMatrix._22;
    up.z = m_renderable->m_worldMatrix._23;
    switch (LpGame->m_gameOptions->m_raceType) {
    case 1:
    case 6:
    case 7:
        ib = 1;
        ia = 2;
        break;
    case 2:
        ib = 2;
        ia = ib;
        break;
    case 5:
        ib = 1;
        break;
    case 4:
        ib = 1;
        break;
    case 3:
        ib = 2;
        break;
    case 8:
        ib = 1;
        break;
    }
    if (LpGame->m_cheats->allOutAI) {
        x258 = 4.0f;
        fa = 75.0f;
        fb = 5.0f;
        fc = 25.0f;
        ic = 4;
        x25C = 10.0f * (4.65661287e-10f * rand()) - 5.0f;
        x260 = 2.5f;
        x264 = 0.5f;
        x268 = 60.0f;
        x26C = 0.785398185f;
        x270 = 59.0f;
        x274 = 0.75f;
        x278 = 0.6f;
    } else {
        switch (LpGame->m_gameOptions->m_difficulty) {
        case 1:
            x258 = 4.0f;
            fa = 25.0f;
            fb = 15.0f;
            fc = fa;
            ic = 2;
            x25C = 10.0f * (4.65661287e-10f * rand()) - 5.0f;
            x260 = 2.5f;
            x264 = 0.5f;
            x268 = 60.0f;
            x26C = 0.785398185f;
            x270 = 98.0f;
            x274 = 0.75f;
            x278 = 0.6f;
            break;
        case 2:
            x258 = 4.0f;
            fa = 50.0f;
            fb = 10.0f;
            fc = 25.0f;
            ic = 3;
            x25C = 10.0f * (4.65661287e-10f * rand()) - 5.0f;
            x260 = 2.5f;
            x264 = 0.5f;
            x268 = 60.0f;
            x26C = 0.785398185f;
            x270 = 77.0f;
            x274 = 0.75f;
            x278 = 0.6f;
            break;
        case 3:
            if (LpGame->m_cheats->hardToNormalAI) {
                x258 = 4.0f;
                fa = 50.0f;
                fb = 10.0f;
                fc = 25.0f;
                ic = 3;
                x25C = 10.0f * (4.65661287e-10f * rand()) - 5.0f;
                x260 = 2.5f;
                x264 = 0.5f;
                x268 = 60.0f;
                x26C = 0.785398185f;
                x270 = 77.0f;
                x274 = 0.75f;
                x278 = 0.6f;
            } else {
                x258 = 4.0f;
                fa = 75.0f;
                fb = 5.0f;
                fc = 25.0f;
                ic = 4;
                x25C = 10.0f * (4.65661287e-10f * rand()) - 5.0f;
                x260 = 2.5f;
                x264 = 0.5f;
                x268 = 60.0f;
                x26C = 0.785398185f;
                x270 = 59.0f;
                x274 = 0.75f;
                x278 = 0.6f;
            }
            break;
        }
    }
    scale = _D3DVECTOR(3.0f);
    offset = _D3DVECTOR();
    colliders[0] = m_xA4;
    colliders[1] = m_collision;
    colliders[2] = m_highSpeed;
    m_ai = new VehicleAIClass(1);
    if (!AddChild(m_ai->Create(this, 0, position, velocity, speed, forward, up, ia, ib, 0, race, track, &scale,
                               &offset, numColliders, colliders, &x258, fa, terrain, &x25C, &x254, ic, &x260, &x264, &x268,
                               &x26C, &x270, &x274, &x278, scene),
                  0xFFFFFFFF)) {
        Release();
        return 0;
    }
    m_ai->m_xDE4 = fa;
    m_ai->m_xDE8 = fb;
    m_ai->m_xF38 = fc;
    m_ai->m_xF34 = 100.0f;
    if (LpGame->m_gameOptions->m_difficulty == 3)
        m_ai->m_flags &= ~0x400;
    return this;
}


// ---- src/JetskiAIVehicle/GetAIVehicleStatus.cpp


int JetskiAIVehicle::GetAIVehicleStatus(_D3DVECTOR *position, _D3DVECTOR *velocity, _D3DVECTOR *c0, _D3DVECTOR *b0,
                                        float *speed, _D3DVECTOR *forward, _D3DVECTOR *up, float *maxValue, int *flags)
{
    if (position) {
        position->x = m_renderable->m_worldMatrix._41;
        position->y = m_renderable->m_worldMatrix._42;
        position->z = m_renderable->m_worldMatrix._43;
    }
    if (velocity)
        *velocity = m_body->m_velocity;
    if (c0)
        *c0 = m_body->m_xC0;
    if (b0)
        *b0 = m_body->m_xB0;
    if (speed)
        *speed = Magnitude(m_body->m_velocity);
    if (forward) {
        forward->x = m_renderable->m_worldMatrix._31;
        forward->y = m_renderable->m_worldMatrix._32;
        forward->z = m_renderable->m_worldMatrix._33;
    }
    if (up) {
        up->x = m_renderable->m_worldMatrix._21;
        up->y = m_renderable->m_worldMatrix._22;
        up->z = m_renderable->m_worldMatrix._23;
    }
    if (maxValue) {
        if (m_x614 < m_x610)
            *maxValue = m_x610;
        else
            *maxValue = m_x614;
    }
    if (flags) {
        if (m_body->m_x30)
            *flags &= ~0x20;
        else
            *flags |= 0x20;
        if (m_rider->m_x484)
            *flags |= 0x10;
        else
            *flags &= ~0x10;
    }
    return 1;
}


// ---- src/JetskiAIVehicle/SetAIControllerInputs.cpp

int JetskiAIVehicle::SetAIControllerInputs(_D3DVECTOR *steer, float *throttle, float *brake, int *buttons)
{
    if (steer) {
        m_steer = steer->x;
        m_throttle = steer->y;
    }
    if (throttle)
        m_boost = *throttle;
    return 1;
}


// ---- src/JetskiAIVehicle/ResetPosition.cpp

void JetskiAIVehicle::ResetPosition(int reset)
{
    JetskiVehicle::ResetPosition(reset);
    if (m_ai)
        m_ai->ResetAI();
}

// ---- src/JetskiAIVehicle/ResetAIVehicle.cpp
int JetskiAIVehicle::ResetAIVehicle()
{
    ResetPosition(1);
    return 1;
}

int JetskiAIVehicle::ActivateAIStunt(int stunt)
{
    m_rider->ActivateStunt(stunt);
    return 1;
}




// ---- src/JetskiAIVehicle/SetAIVehicleStatus.cpp


// VU0 cross product, out = a x b.
static inline void CrossProduct(_D3DVECTOR &out, const _D3DVECTOR &a, const _D3DVECTOR &b)
{
    asm("lqc2 vf4, 0x0(%1); lqc2 vf5, 0x0(%2); vopmula.xyz ACC, vf4, vf5; vopmsub.xyz vf6, vf5, vf4; vsub.w vf6, vf6, vf6; sqc2 vf6, 0x0(%0)"
        : : "r"(&out), "r"(&a), "r"(&b) : "memory");
}



// Force the vehicle's placement and motion (used to replay AI state).
int JetskiAIVehicle::SetAIVehicleStatus(_D3DVECTOR *position, _D3DVECTOR *velocity, _D3DVECTOR *c0, _D3DVECTOR *b0, _D3DVECTOR *forward, _D3DVECTOR *up)
{
    _D3DVECTOR f;
    _D3DVECTOR u;
    _D3DVECTOR side;
    if (position) {
        m_renderable->m_worldMatrix._41 = position->x;
        m_renderable->m_worldMatrix._42 = position->y;
        m_renderable->m_worldMatrix._43 = position->z;
    }
    if (velocity)
        m_body->m_velocity = *velocity;
    if (c0)
        m_body->m_xC0 = *c0;
    if (b0)
        m_body->m_xB0 = *b0;
    if (forward || up) {
        if (forward) {
            f = *forward;
        } else {
            f.x = m_renderable->m_worldMatrix._31;
            f.y = m_renderable->m_worldMatrix._32;
            f.z = m_renderable->m_worldMatrix._33;
        }
        if (up) {
            u = *up;
        } else {
            u.x = m_renderable->m_worldMatrix._21;
            u.y = m_renderable->m_worldMatrix._22;
            u.z = m_renderable->m_worldMatrix._23;
        }
        CrossProduct(side, u, f);
        m_renderable->m_worldMatrix._11 = side.x;
        m_renderable->m_worldMatrix._12 = side.y;
        m_renderable->m_worldMatrix._13 = side.z;
        m_renderable->m_worldMatrix._21 = u.x;
        m_renderable->m_worldMatrix._22 = u.y;
        m_renderable->m_worldMatrix._23 = u.z;
        m_renderable->m_worldMatrix._31 = f.x;
        m_renderable->m_worldMatrix._32 = f.y;
        m_renderable->m_worldMatrix._33 = f.z;
    }
    return 1;
}


// ---- src/JetskiAIVehicle/GetAIDataFilename.cpp
int JetskiAIVehicle::GetAIDataFilename(char *filename)
{
    if (filename)
        sprintf(filename, "%s%.8s.aid", SceneContentDirectory, m_data->m_sceneName);
    return 1;
}


// ---- src/JetskiAIVehicle/GetAIParameterFilename.cpp
int JetskiAIVehicle::GetAIParameterFilename(char *filename)
{
    if (filename)
        sprintf(filename, "%s%.8s.aip", SceneContentDirectory, m_data->m_sceneName);
    return 1;
}


// ---- src/JetskiAIVehicle/GenerateRandomStunt.cpp

int JetskiAIVehicle::GenerateRandomStunt(int *stunt, float chance)
{
    int candidates[28];
    int numCandidates = 0;
    if (!stunt || !m_ownStatus)
        return 0;
    float *stuntChance = m_rider->m_table4D8;
    for (int i = 0; i < 28; i++) {
        if (stuntChance[i] < chance)
            candidates[numCandidates++] = i;
    }
    if (!numCandidates)
        return 0;
    // 4.65661287e-10f is 2^-31: rand() scaled to [0, 1).
    *stunt = candidates[(int)(numCandidates * (4.65661287e-10f * rand()))];
    return 1;
}


// VehicleAIInterface's virtuals are inline in its header; mwcc writes them
// here, with the vtable.


// ---- src/sinit/JetskiAIVehicle.cpp
// Static initializer of JetskiAIVehicle.cpp: only the axis vectors from the shared header.

