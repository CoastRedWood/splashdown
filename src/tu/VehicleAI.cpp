// VehicleAI.cpp  @ 0x0030F5F0 (text 0x30F5F0-0x3131D0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's VehicleAI.cpp as one translation unit, the end of BuoyObject.cpp's
// tu_map row (no class is named after the file): VehicleAIClass's twelve
// functions and their three @8@ thunks, then the static initializer
// (__sinit_VehicleAI.cpp, the .ctor slot after JetskiThrillCam.cpp's: the axis
// vectors only). Its .rodata (0x430E20-0x430EE0): the "CollisionObject" and
// "Vegetation" type names, then the VehicleAIClass/BaseObject/GameObject/
// SearchableObject RTTI records. `make link` places the object's data as well as
// its code (tools/link_c.py).


// UpdateTrackAI's per-function file set 4, CheckForCollisions' 6; TU-wide (read
// at the end of the TU).
#pragma inline_depth(6)

#include "d3dtypes.h"
#include "GameObject/GameObject.h"
#include "SearchableObject/SearchableObject.h"
#include "VehicleAIInterface/VehicleAIInterface.h"
#include "VehicleAIClass/VehicleAIClass.h"
#include "VehicleAIClass/VehicleAIVU0.h"
#include "VehicleAIClass/AIStateSpace.h"
#include "TrackType/TrackType.h"
#include "TrackData_Tag/TrackData_Tag.h"
#include "RaceStatusStruct_Tag/RaceStatusStruct_Tag.h"
#include "RaceStatusClass/RaceStatusClass.h"
#include "RaceStatusVehicleInterface/RaceStatusVehicleInterface.h"
#include "global/CollisionTree.h"
#include "CollisionObject/CollisionObject.h"
#include "Vegetation/Vegetation.h"
#include "TypeRegistry/TypeRegistry.h"
#include "SearchPlacement/SearchPlacement.h"
#include "Terrain/Terrain.h"
#include "Scene/Scene.h"

#include "sinit/AxisVectors.h"

// Defined here for its retail .bss slot.
AIStateSpaceType_Tag *AIStateSpace;


extern "C" void *memset(void *dst, int c, unsigned int n);
extern "C" void *memcpy(void *dst, const void *src, unsigned int n);
extern "C" void *calloc(unsigned int n, unsigned int size);
extern "C" void free(void *p);
extern "C" int rand();

extern TypeRegistry *LpTypeRegistry;
extern unsigned int COLLISION_BIKE;
extern unsigned int COLLISION_RIDER;
extern unsigned int WORLDOBJECT_PHYSICSBODY;

// ---- src/VehicleAIClass/Tick.cpp

int VehicleAIClass::Tick(float dt)
{
    int result = 0;
    if (m_vehicle)
        m_vehicle->GetAIVehicleStatus(&m_xD00, &m_xD10, &m_xD20, &m_xD30, &m_xD40, &m_xD50, &m_xD60, &m_xED4, (int *)&m_flags);
    switch (m_mode) {
    case 2:
        result = UpdateTrackAI(dt);
        break;
    case 4:
    case 3:
        break;
    }
    m_vehicle->SetAIControllerInputs(&m_steer, &m_throttle, &m_brake, &m_buttons);
    return result & GameObject::Tick(dt);
}

// ---- src/VehicleAIClass/ctor.cpp

VehicleAIClass::VehicleAIClass(int enabled) : GameObject(enabled)
{
    m_flags = 0x407;
    m_vehicle = 0;
    m_alodRange = 100.0f;
    m_alodSpeedScale = 0.9f;
    m_x48 = 1.5f;
    m_alodMaxScale = 0.5f;
    memset(m_path, 0, 0xCB0);           // the path points and the fields up to +0xD00
    m_xD00 = _D3DVECTOR(0.0f, 0.0f, 0.0f);
    m_xD10 = _D3DVECTOR(0.0f, 0.0f, 0.0f);
    m_xD20 = _D3DVECTOR(0.0f, 0.0f, 0.0f);
    m_xD30 = _D3DVECTOR(0.0f, 0.0f, 0.0f);
    m_xD40 = 0.0f;
    m_xD50.x = 0.0f;
    m_xD50.y = 0.0f;
    m_xD50.z = 1.0f;
    m_xD60.x = 0.0f;
    m_xD60.y = 1.0f;
    m_xD60.z = 0.0f;
    m_mode = 0;
    m_xD74 = 0;
    m_status = 0;
    m_raceStatus = 0;
    m_track = 0;
    m_throttle = 0.0f;
    m_brake = 0.0f;
    m_steer = _D3DVECTOR(0.0f, 0.0f, 0.0f);
    m_xDA0 = _D3DVECTOR(0.0f, 0.0f, 0.0f);
    m_xDB0 = _D3DVECTOR(3.0f, 3.0f, 3.0f);
    m_xDC0 = _D3DVECTOR(0.1f, 0.1f, 0.1f);
    m_numObjects = 0;
    m_objects = 0;
    m_xDD8 = 10.0f;
    m_xDDC = 0.0f;
    m_xDE0 = 0.0f;
    m_xDE4 = 0.0f;
    m_xDE8 = 0.0f;
    m_terrain = 0;
    m_xDF0 = 0.0f;
    m_xDF4 = 0.0f;
    m_xDF8 = 0.0f;
    m_xDFC = 1.0f;
    m_xE00 = 0;
    m_xE04 = 0;
    m_xE08 = 0;
    m_buttons = 0;
    m_xE10 = _D3DVECTOR(0.0f, 0.0f, 0.0f);
    m_xE20 = 0;
    m_xE24 = 0;
    m_xE2C = 0.0f;
    m_xE30 = 0;
    memset(m_xE34, 0, sizeof(m_xE34));
    m_state = 0;
    m_xE50 = _D3DVECTOR(0.0f, 0.0f, 0.0f);
    m_xE60 = _D3DVECTOR(0.0f, 0.0f, 0.0f);
    m_xE70 = _D3DVECTOR(0.0f, 0.0f, 0.0f);
    m_xE80 = _D3DVECTOR(0.0f, 0.0f, 0.0f);
    m_xE90 = 0.0f;
    m_xE94 = 0.0f;
    m_xE98 = 0.0f;
    m_xEA0 = _D3DVECTOR(0.0f, 0.0f, 0.0f);
    m_searchPlacement = 0;
    m_extents = _D3DVECTOR(0.0f, 0.0f, 0.0f);
    m_xED0 = 0.0f;
    m_xED4 = 0.0f;
    m_xED8 = 0.0f;
    m_xEDC = 0.0f;
    m_xEE0 = 0.0f;
    m_xEE4 = 0.0f;
    m_xEE8 = 0.0f;
    m_xEEC = 0.0f;
    m_xEF0 = 0.0f;
    m_xEFC = 0.0f;
    memset(&m_trackPosition, 0, sizeof(m_trackPosition));
    AIStateSpace = 0;
    m_xEF4 = 0;
    m_xEF8 = -2.5f;
    m_xEFC = -1.0f;
    for (int i = 0; i < 10; i++)
        m_random[i] = 4.656613e-10f * rand();
    m_xE28 = 10.0f;
    m_xF34 = -1.0f;
    m_xF38 = -1.0f;
}

// ---- src/VehicleAIClass/Create.cpp

GameObject *VehicleAIClass::Create(VehicleAIInterface *vehicle, int *flags, _D3DVECTOR position,
                                   _D3DVECTOR velocity, float speed, _D3DVECTOR a6, _D3DVECTOR a7, int mode,
                                   int a9, RaceStatusStruct_Tag *status, RaceStatusClass *raceStatus,
                                   TrackType *track, _D3DVECTOR *a13, _D3DVECTOR *a14, int numObjects,
                                   CollisionObject **objects, float *a17, float a18, Terrain *terrain,
                                   float *a20, float *a21, int a22, float *a23, float *a24, float *a25,
                                   float *a26, float *a27, float *a28, float *a29, Scene *scene)
{
    GameObject::Create();
    m_vehicle = vehicle;
    if (flags)
        m_flags = *flags;
    m_xD00 = position;
    m_xD10 = velocity;
    m_xD40 = speed;
    m_xD50 = a6;
    m_xD60 = a7;
    m_mode = mode;
    m_xD74 = a9;
    m_status = status;
    m_raceStatus = raceStatus;
    m_track = track;
    // Copied field by field: retail reloads m_track for each one.
    if (m_track) {
        m_alodRange = m_track->m_tuning.x;
        m_alodSpeedScale = m_track->m_tuning.y;
        m_x48 = m_track->m_tuning.z;
        m_alodMaxScale = m_track->m_tuning.w;
    }
    if (a13)
        m_xDB0 = *a13;
    if (a14)
        m_xDC0 = *a14;
    if (numObjects > 0 && objects) {
        m_numObjects = numObjects;
        if (!(m_objects = (CollisionObject **)calloc(numObjects, sizeof(CollisionObject *)))) {
            Release();
            return 0;
        }
        memcpy(m_objects, objects, numObjects * sizeof(CollisionObject *));
    }
    if (a18 >= 0.0f)
        m_xDE0 = a18;
    m_terrain = terrain;
    if (a20)
        m_xDF0 = *a20;
    else
        m_xDF0 = 12.0f * (4.656613e-10f * rand()) - 6.0f;     // random in [-6, 6)
    if (a21)
        m_xDFC = *a21;
    if (a17)
        m_xDD8 = *a17;
    if (a22 > 0)
        m_xE20 = a22;
    memset(m_xE34, 0, sizeof(m_xE34));
    m_extents.x = 50.0f;
    m_extents.y = 50.0f;
    m_extents.z = 50.0f;
    m_searchPlacement = new SearchPlacement;
    if (m_searchPlacement)
        m_searchPlacement->PerformInitialPlacement(this, scene->m_dynamicNodes[0], 2, &m_xD00, &m_extents, 0.0f);
    if (a23)
        m_xED8 = *a23;
    else
        m_xED8 = 2.5f;
    if (a24)
        m_xEDC = *a24;
    else
        m_xEDC = 0.5f;
    if (a25)
        m_xEE0 = *a25;
    else
        m_xEE0 = 60.0f;
    if (a26)
        m_xEE4 = *a26;
    else
        m_xEE4 = 0.785398185f;
    if (a27)
        m_xEE8 = *a27;
    else
        m_xEE8 = 50.0f;
    if (a28)
        m_xEEC = *a28;
    else
        m_xEEC = 0.75f;
    if (a29)
        m_xEF0 = *a29;
    else
        m_xEF0 = 0.6f;
    return this;
}

// ---- src/VehicleAIClass/dtor.cpp

VehicleAIClass::~VehicleAIClass()
{
    if (m_objects)
        free(m_objects);
    if (AIStateSpace)
        FreeStateSpace(&AIStateSpace);
    if (m_searchPlacement)
        delete m_searchPlacement;
}

// ---- src/VehicleAIClass/AvoidCollisions.cpp

int VehicleAIClass::AvoidCollisions(int numObjects, CollisionObject **objects, _D3DVECTOR position,
                                    _D3DVECTOR velocity, float time, _D3DVECTOR *direction, float *distance)
{
    // Only step and hit are used. The unused locals (an array of two and two
    // more) are retail's: an array's first element is constructed before the
    // others' stores interleave.
    _D3DVECTOR unused[2], unused60, step, hit, unused90;
    m_flags &= ~0x100;
    step = velocity * time;
    if (CheckForCollisions(&position, 2.0f, numObjects, objects, step, &hit)) {
        if (hit.x != 0.0f || hit.z != 0.0f) {
            float length = Magnitude(hit);  // unused (retail computes it anyway)
            Normalize(&hit, &hit);
            if (direction)
                *direction = hit;
        }
    }
    return 1;
}

// ---- src/VehicleAIClass/UpdateTrackAI.cpp
// UpdateTrackAI__14VehicleAIClassFf  @ 0x00310630
// Retail builds the (50, 50, 50) range temporary with Direct3D's one-float _D3DVECTOR constructor
// (x = y = z = f, stores z, y, x, then w), as d3dtypes.h declares it.

float FastAtan2(float y, float x);
float FastACos(float x);
float FastASin(float x);

// The second of two dot products with the same first vector: vf4 still
// holds it.
static inline float DotProductVF4(_D3DVECTOR *b)
{
    float r;
    asm("lqc2 vf5, 0x0(%1); vmul.xyz vf5, vf4, vf5; vaddy.x vf5, vf5, vf5y; vaddz.x vf5, vf5, vf5z; qmfc2.ni $2, vf5; mtc1 $2, %0"
        : "=f"(r) : "r"(b) : "$2");
    return r;
}

static inline float Dot(const _D3DVECTOR &a, const _D3DVECTOR &b)
{
    return DotProduct((_D3DVECTOR *)&a, (_D3DVECTOR *)&b);
}

static inline float Atan2Dots(const _D3DVECTOR &a, const _D3DVECTOR &b, const _D3DVECTOR &c, const _D3DVECTOR &d)
{
    return FastAtan2(Dot(a, b), Dot(c, d));
}

// atan2((e - f) . b, (e - f) . d) with the difference computed twice, as
// retail does. The first copy is the caller's argument temporary (made first,
// lower stack slot), the second is made inside, one inline level down.
static inline float Atan2DiffDots(const _D3DVECTOR &e, const _D3DVECTOR &f, const _D3DVECTOR &b, const _D3DVECTOR &diff, const _D3DVECTOR &d)
{
    return Atan2Dots(diff, b, e - f, d);
}

static inline void CopyPosition(TrackPosition_Tag &to, const TrackPosition_Tag &from)
{
    to.m_segment = from.m_segment;
    to.m_data = from.m_data;
    to.m_t = from.m_t;
}

static inline float RandomFraction() { return 4.656613e-10f * rand(); }

int VehicleAIClass::UpdateTrackAI(float dt)
{
    _D3DVECTOR offset;
    _D3DVECTOR unused50, unused60, unused70, unused80, unused90, unusedA0;
    _D3DVECTOR toTarget;
    _D3DVECTOR unusedC0, unusedD0, unusedE0;
    _D3DVECTOR range;
    _D3DVECTOR nodeDirection;
    static int UpdateTrackAI_ID = -1;   // unused profiler ID
    int onTrack;
    if (!m_status || !m_track)
        return 0;
    if (!(m_status->m_flags & 2)) {
        m_steer.x = 0.0f;
        m_steer.y = 0.0f;
        m_throttle = 0.0f;
        m_brake = 0.0f;
        return 1;
    }
    onTrack = m_status->m_onTrack;
    // A dead test: retail keeps its branch (to the next instruction, with the
    // m_onTrack load in the delay slot). An `&&` (or `?:`) expression statement
    // keeps it; an if with an empty or self-assigning body does not.
    (m_status->m_flags & 1) && onTrack;
    if (!m_status->m_lastTrackPosition.m_segment || !m_status->m_lastTrackPosition.m_data ||
        !m_track->m_finish.m_segment || !m_track->m_finish.m_data)
        return 0;
    if (AIStateSpace) {
        if (onTrack) {
            range = _D3DVECTOR(50.0f);
            GetNewState(m_state, &m_state, range);
        }
    } else {
        m_state = 0;
    }
    if (m_state) {
        m_xE90 = m_state->m_height;
        m_xE94 = ((float *)&m_state->m_height)[1];
        m_xE98 = ((float *)&m_state->m_height)[2];
        m_xE50 = m_state->m_position;
        m_xE70 = m_state->m_direction;
        m_xEA0 = *(_D3DVECTOR *)((char *)m_state + 0x40);
    } else if (m_track) {
        float lookAhead;
        if (!m_trackPosition.m_segment || !onTrack) {
            CopyPosition(m_trackPosition, m_status->m_lastTrackPosition);
            lookAhead = 0.0f;
        } else {
            lookAhead = m_xEE0;
        }
        TrackPosition_Tag next;
        CopyPosition(next, m_trackPosition);
        TrackPosition_Tag closest;
        if (m_track->GetNextTrackPosition(m_trackPosition, &next, m_xD00, &closest, lookAhead, m_xD40, m_random)) {
            if (closest.m_segment != m_trackPosition.m_segment) {
                for (onTrack = 0; onTrack < 10; onTrack++)
                    m_random[onTrack] = RandomFraction();
            }
            CopyPosition(m_trackPosition, closest);
            m_track->CoordinatesFromNode(&next, &m_xE50);
            m_track->GetTrackDirection(next.m_data, &m_xE70);
            m_track->GetTrackDirection(m_trackPosition.m_data, &nodeDirection);
            m_track->CoordinatesFromNode(&m_trackPosition, &offset);
            offset.x -= m_xD00.x;
            offset.y -= m_xD00.y;
            offset.z -= m_xD00.z;
            m_xDF0 = offset.z * nodeDirection.x - offset.x * nodeDirection.z;
            if (m_xDF0 > 5.0f)
                m_xDF0 = 5.0f;
            else if (m_xDF0 < -5.0f)
                m_xDF0 = -5.0f;
            float scale;
            if (m_trackPosition.m_data->m_flags & 0x10) {
                scale = -1.0f;
            } else {
                scale = FastACos(DotProduct(&m_xD50, &m_xE70));
                if (scale > m_xEE4)
                    scale = 0.0f;
                else
                    scale = (m_xEE4 - scale) / m_xEE4;
            }
            m_xE50.x += m_xE70.z * (scale * m_xDF0);
            m_xE50.z -= m_xE70.x * (scale * m_xDF0);
            m_xE70 += 0.05f * (m_xE50 - m_xD00);
            m_xE70.y = 0.0f;
            Normalize(&m_xE70, &m_xE70);
            if (m_trackPosition.m_data->m_x44 == -1.0f)
                m_xE90 = 150.0f;
            else
                m_xE90 = m_trackPosition.m_data->m_x44;
            if (m_xD40 > 10.0f + m_xE90) {
                m_xE94 = 0.0f;
                m_xE98 = 1.0f;
            } else {
                m_xE94 = 1.0f;
                m_xE98 = 0.0f;
            }
        } else {
            m_xE90 = m_xD40;
            m_xE94 = 1.0f;
            m_xE98 = 0.0f;
            m_xE50 = m_xD00;
            m_xE70 = m_xD50;
        }
        m_xEA0 = ZeroVector;
    }
    toTarget = m_xE50 - m_xD00;
    Normalize(&toTarget, &toTarget);
    if (m_flags & 1)
        AvoidCollisions(m_numObjects, m_objects, m_xD00, toTarget, m_xD40, &toTarget, &m_xE90);
    offset.x = m_xD50.z;
    offset.z = -m_xD50.x;
    offset.y = m_xD50.y;
    if (Magnitude(m_xE50 - m_xD00) < 100.0f) {
        m_xEA0.y = m_xED8 * FastAtan2(DotProduct(&m_xE70, &offset), DotProductVF4(&m_xD50));
        if (DotProduct(&m_xD50, &m_xE70) > 0.707f)
            m_xEA0.y += m_xEDC * FastAtan2(DotProduct(&toTarget, &offset), DotProductVF4(&m_xD50));
    } else {
        m_xEA0.y = m_xED8 * FastAtan2(DotProduct(&toTarget, &offset), DotProductVF4(&m_xD50));
    }
    if (!(m_flags & 0x20)) {
        m_xE70.x *= 0.9816f;
        m_xE70.z *= 0.9816f;
        m_xE70.y = -0.1908f;
    }
    m_xEA0.x -= m_xEDC * Atan2DiffDots(m_xE70, m_xD50, m_xD60, m_xE70 - m_xD50, m_xD50);
    m_xEA0.z = m_xED8 * FastAtan2(DotProduct(&m_xD60, &offset), m_xD60.y);
    if (m_vehicle && (m_flags & 8) && (m_flags & 2) && !(m_xDDC < m_xDD8) && (m_trackPosition.m_data->m_flags & 0x20)) {
        if (!(m_flags & 0x30) && (m_xF38 < 0.0f || !(m_xD40 < m_xF38))) {
            m_xE2C = m_trackPosition.m_data->m_x48;
            float chance;
            if (m_flags & 0x4000)
                chance = m_xE2C - m_xED0;
            else
                chance = m_xE2C;
            if (100.0f * RandomFraction() <= m_xDE0 && (m_xF34 < 0.0f || m_status->m_x6F4 < m_xF34)) {
                if (m_vehicle->GenerateRandomStunt(&m_xE30, chance)) {
                    m_vehicle->ActivateAIStunt(m_xE30);
                    m_xDDC = 0.0f;
                    if (100.0f * RandomFraction() < m_xDE8)
                        m_flags |= 0x2000;
                    else if (100.0f * RandomFraction() < m_xDE4)
                        m_flags |= 0x1000;
                    if (!(m_flags & 0x4000))
                        m_flags &= ~8;
                }
            } else {
                m_flags &= ~8;
            }
        }
    } else {
        m_xDDC += dt;
        if ((m_status->m_flags & 0x20) && (m_flags & 0x3000)) {
            if ((m_flags & 0x2000) || m_xE2C - m_xED0 > 0.5f) {
                m_flags |= 0x800;
            } else {
                m_flags &= ~0x800;
                m_flags &= ~0x3000;
            }
        }
        if (m_flags & 0x20) {
            m_flags |= 8;
            m_flags &= ~0x3800;
        }
    }
    if (m_flags & 0x20) {
        m_xED0 = 0.0f;
        m_xE28 += dt;
    } else {
        m_xED0 += dt;
        m_xE28 = 0.0f;
    }
    if ((m_flags & 0x20) || m_xD60.y > 0.8f)
        m_steer.x = FastASin(1.5f * (m_xEA0.y - m_xD30.y));
    else
        m_steer.x = 0.0f;
    if (m_steer.x > 1.0f)
        m_steer.x = 1.0f;
    else if (m_steer.x < -1.0f)
        m_steer.x = -1.0f;
    if (!(m_flags & 0x20)) {
        if (Fabs(m_xD50.y - m_xE70.y) > 0.2f)
            m_steer.y = FastASin(1.5f * (m_xD30.x - m_xEA0.x));
        else
            m_steer.y = 0.0f;
    } else if ((m_xE20 != 2 && m_xD40 > m_xEE8 && Fabs(m_steer.x) < 0.3f && !(m_xE28 < m_xEEC)) ||
               (m_trackPosition.m_data->m_flags & 8)) {
        m_steer.y = m_xEF0;
    } else {
        m_steer.y = 0.0f;
    }
    if (!m_state && m_trackPosition.m_data) {
        unsigned int flags = m_trackPosition.m_data->m_flags;
        if (flags & 2) {
            if (m_xD00.y - m_xED4 > m_xEFC && m_xEF4) {
                m_steer.y = -1.0f;
                m_steer.x = 0.0f;
            } else {
                m_xEF4 = 0;
                if (m_xD00.y - m_xED4 < 0.0f) {
                    m_steer.x = 0.0f;
                    m_steer.y = 0.3f;
                }
            }
        } else if (flags & 4) {
            if (m_xD00.y - m_xED4 > m_xEF8 && m_xEF4) {
                m_steer.y = -1.0f;
                m_steer.x = 0.0f;
            } else {
                m_xEF4 = 0;
                if (m_xD00.y - m_xED4 < 0.0f)
                    m_steer.x = 0.0f;
            }
        } else {
            m_xEF4 = 1;
        }
    }
    if (m_steer.y > 1.0f)
        m_steer.y = 1.0f;
    else if (m_steer.y < -1.0f)
        m_steer.y = -1.0f;
    if (!(m_trackPosition.m_data->m_flags & 8) && (m_flags & 0x400))
        ApplyALOD(&m_xE90);
    m_throttle = m_xE94;
    m_brake = m_xE98;
    if (m_xD40 < m_xE90 - 5.0f && m_throttle == 0.0f) {
        m_throttle = 1.0f;
        m_brake = 0.0f;
    } else if (m_xD40 > 5.0f + m_xE90 && m_throttle == 1.0f) {
        m_throttle = 0.0f;
        m_brake = 1.0f;
    }
    return 1;
}

// ---- src/VehicleAIClass/CheckForCollisions.cpp
// CheckForCollisions__14VehicleAIClassFP10_D3DVECTORfiPP15CollisionObject10_D3DVECTORP10_D3DVECTOR  @ 0x00311870

class SpatialSearch {
public:
    SpatialSearch();
    void OpenSearch(SearchPlacement *placement, unsigned int flags, _D3DVECTOR *center, _D3DVECTOR *half,
                    _D3DVECTOR *outMin, _D3DVECTOR *outMax);
    SearchableObject *NextSearchObject();
    void CloseSearch();

#ifdef SPLASHDOWN_PC
    _D3DVECTOR pad[6];  // x86-64: the real class (SpatialSearch.h) is 0x58 bytes, 16-aligned
#else
    char pad[0x40];
#endif
};

int TestRayBundleForCollision(DynamicRayCollisionObject *rays, CollisionObject *other, RayToFaceIntersectionData *data);

extern unsigned char CollisionTypeID;     // CollisionObject.cpp's
extern unsigned char VegetationTypeID;

// xyz length squared on VU0.
static inline float LengthSquared(_D3DVECTOR *a)
{
    float r;
    asm("lqc2 vf4, 0x0(%1); vmove.xyzw vf5, vf4; vmul.xyz vf5, vf4, vf5; vaddy.x vf5, vf5, vf5y; vaddz.x vf5, vf5, vf5z; qmfc2.ni $2, vf5; mtc1 $2, %0"
        : "=f"(r) : "r"(a) : "$2");
    return r;
}

static inline void SubtractFrom(_D3DVECTOR &a, const _D3DVECTOR &b)
{
    a.x -= b.x;
    a.y -= b.y;
    a.z -= b.z;
}

// s * v through an extra inline level: numbers the products z, y, x (as retail
// colours them; d3dtypes' operator* numbers z, x, y). Needs inline_depth(6).
static inline _D3DVECTOR MakeVec(float x, float y, float z)
{
    return _D3DVECTOR(x, y, z);
}

static inline _D3DVECTOR ScaleV(float s, const _D3DVECTOR &v)
{
    return MakeVec(s * v.x, s * v.y, s * v.z);
}

int VehicleAIClass::CheckForCollisions(_D3DVECTOR *position, float radius, int numObjects, CollisionObject **objects,
                                       _D3DVECTOR velocity, _D3DVECTOR *direction)
{
    static int CollisionAvoidance_ID = -1;  // unused profiler ID
    float nearest;
    // One offsetDir pointer for the setup loop and both ray-hit loops: its single
    // node gets retail's $a3 in all three.
    float *offset;
    int *rayHitP;
    _D3DVECTOR *point;
    _D3DVECTOR *offsetDir;
    float distance2;
    float ahead;
    float closing;
    int i;
    Vegetation *vegetation;
    int count;
    int j;
    CollisionObject **end;
    _D3DVECTOR avoid;
    _D3DVECTOR delta;
    _D3DVECTOR unusedE0;
    _D3DVECTOR unusedF0;
    _D3DVECTOR result;
    _D3DVECTOR away;
    _D3DVECTOR directions[4];
    _D3DVECTOR localDirections[4];
    _D3DVECTOR starts[4];
    _D3DVECTOR localStarts[4];
    _D3DVECTOR points[4];
    _D3DVECTOR normals[4];
    float lengths[4];
    float rayT[4];
    int rayHit[4];
    DynamicRayCollisionObject rays;
    RayToFaceIntersectionData data;
    _D3DVECTOR unused380(100.0f, 100.0f, 100.0f);
    int hit = 0;
    SpatialSearch search;
    _D3DVECTOR searchMin;
    _D3DVECTOR searchMax;
    unsigned int vehicleTypes = COLLISION_BIKE | COLLISION_RIDER;
    float offsets[4] = {-1.0f, -2.0f, 2.0f, 1.0f};
    _D3DVECTOR offsetDirections[4];
    _D3DVECTOR forward;
    _D3DVECTOR otherPosition;
    _D3DVECTOR otherVelocity;
    static float AvoidanceResponseScale = 1000.0f;
    static float NearCollisionRange = 144.0f;
    static float FarCollisionRange = 10000.0f;
    if (CollisionTypeID == 0xFF)
        CollisionTypeID = LpTypeRegistry->GetTypeID("CollisionObject");
    if (VegetationTypeID == 0xFF)
        VegetationTypeID = LpTypeRegistry->GetTypeID("Vegetation");
    Normalize(&forward, &velocity);
    offsetDir = offsetDirections;
    offset = offsets;
    for (i = 0; i < 4; i++) {
        offsetDir->x = forward.x + forward.z * *offset;
        offsetDir->z = forward.z - forward.x * *offset;
        offsetDir->y = 0.0f;
        offsetDir++;
        offset++;
    }
    result = velocity;
    if (direction)
        *direction = _D3DVECTOR();
    end = objects + numObjects;
    rays.m_numRays = 4;
    rays.m_directions = directions;
    rays.m_localDirections = localDirections;
    rays.m_starts = starts;
    rays.m_localStarts = localStarts;
    rays.m_lengths = lengths;
    rays.m_radius = 100.0f;
    rays.m_center = *position;
    float side = -2.0f;
    for (j = 0; j < 4; j++) {
        directions[j] = forward;
        starts[j] = *position;
        lengths[j] = 100.0f;
        starts[j].x += forward.z * side;
        starts[j].z -= forward.x * side;
        side += 1.3333f;
    }
    data.m_t = 0.0f;
    data.m_ray = 0;
    data.m_rays = 0;
    data.m_rayHit = rayHit;
    data.m_rayT = rayT;
    data.m_rayPoints = points;
    data.m_rayNormals = normals;
    memset(rayHit, 0, sizeof(rayHit));
    memset(rayT, 0, sizeof(rayT));
    memset(points, 0, sizeof(points));
    memset(normals, 0, sizeof(normals));
    if (!(m_flags & 0x80) && !m_status->m_onTrack && m_searchPlacement) {
        m_searchPlacement->UpdatePlacement(this, &m_xD00, &m_extents, 50.0f);
        search.OpenSearch(m_searchPlacement, 1, &m_xD00, &m_extents, &searchMin, &searchMax);
        SearchableObject *object;
        while ((object = search.NextSearchObject())) {
            if (object->m_typeID == CollisionTypeID) {
                CollisionObject *other = (CollisionObject *)object;
                int skip = 0;
                if (!other->m_enabled)
                    skip = 1;
                for (CollisionObject **p = objects; p < end && !skip; p++) {
                    if (other == *p)
                        skip = 1;
                }
                if (!skip && (other->m_ownerType & vehicleTypes) && !(other->m_ownerType & WORLDOBJECT_PHYSICSBODY))
                    skip = 1;
                if (!skip && TestRayBundleForCollision(&rays, other, &data)) {
                    nearest = 1000.0f;
                    rayHitP = rayHit;
                    point = points;
                    offsetDir = offsetDirections;
                    for (j = 0; j < 4; j++) {
                        if (*rayHitP) {
                            delta = *point - *position;
                            delta.y = 0.0f;
                            ahead = Magnitude(delta);  // the function-level float, reused (retail's colouring)
                            if (ahead < nearest) {
                                nearest = ahead;
                                avoid.x = offsetDir->x;
                                avoid.y = 0.0f;
                                avoid.z = offsetDir->z;
                                Normalize(&avoid, &avoid);
                            }
                        }
                        rayHitP++;
                        point++;
                        offsetDir++;
                    }
                    SubtractFrom(result, DotProduct(&result, &avoid) * avoid);
                    hit = 1;
                }
            } else if (!(m_flags & 0x40) && object->m_typeID == VegetationTypeID) {
                vegetation = (Vegetation *)object;
                count = vegetation->NumCollisionObjects();
                for (i = 0; i < count; i++) {
                    if (TestRayBundleForCollision(&rays, vegetation->GetCollisionObject(i), &data)) {
                        nearest = 1000.0f;
                        rayHitP = rayHit;
                        point = points;
                        offsetDir = offsetDirections;
                        for (j = 0; j < 4; j++) {
                            if (*rayHitP) {
                                delta = *point - *position;
                                delta.y = 0.0f;
                                ahead = Magnitude(delta);  // the function-level float, reused (retail's colouring)
                                if (ahead < nearest) {
                                    nearest = ahead;
                                    avoid.x = offsetDir->x;
                                    avoid.y = 0.0f;
                                    avoid.z = offsetDir->z;
                                    Normalize(&avoid, &avoid);
                                }
                            }
                            rayHitP++;
                            point++;
                            offsetDir++;
                        }
                        SubtractFrom(result, DotProduct(&result, &avoid) * avoid);
                        hit = 1;
                    }
                }
            }
            search.CloseSearch();
        }
    }
    if (m_raceStatus) {
        for (RaceStatusStruct_Tag *s = m_raceStatus->m_vehicles; s; s = s->m_next) {
            if (s == m_status || !s->m_vehicle)
                continue;
            float speed;
            int status;
            s->m_vehicle->RSGetVehicleStatus(&otherPosition, &otherVelocity, &speed, &status);
            away = otherPosition - *position;
            distance2 = LengthSquared(&away);
            Normalize(&away, &away);
            ahead = DotProduct(&away, &forward);
            if (ahead > 0.707f) {
                SubtractFrom(away, 0.01f * ahead * forward);
                Normalize(&away, &away);
            }
            if (distance2 < NearCollisionRange) {
                delta = result - otherVelocity;
                hit = 1;
                closing = DotProduct(&delta, &away);
                if (closing > 0.0f)
                    SubtractFrom(result, closing * away);
                SubtractFrom(result, 10.0f * away);
            } else if (distance2 <= FarCollisionRange) {
                delta = result - otherVelocity;
                hit = 1;
                closing = DotProduct(&delta, &away);
                if (closing > 0.0f)
                    SubtractFrom(result, ScaleV(closing, away) * AvoidanceResponseScale / (1.0f + distance2));
            }
        }
    }
    if (direction)
        *direction = result;
    return hit;
}

// ---- src/VehicleAIClass/ApplyALOD.cpp

// Artificial level of detail: rubber-band the AI's top speed to the leader's.
int VehicleAIClass::ApplyALOD(float *maxSpeed)
{
    float speed;

    if (!maxSpeed)
        return 0;
    if (m_raceStatus && m_raceStatus->m_leader) {
        m_raceStatus->m_leader->m_vehicle->RSGetVehicleStatus(0, 0, &speed, 0);
        if (speed < 60.0f)
            speed = 60.0f;
        float ahead = m_status->m_distance - m_raceStatus->m_leader->m_distance;
        float scaled = speed * m_alodSpeedScale;
        float limit = *maxSpeed * m_alodMaxScale;
        if (ahead > 0.0f) {
            if (ahead > m_alodRange) {
                if (*maxSpeed > scaled) {
                    if (scaled > limit)
                        *maxSpeed = scaled;
                    else
                        *maxSpeed = limit;
                }
            } else if (ahead > 25.0f) {
                speed = speed + (5.0f + ahead * ((5.0f + speed) * (m_alodSpeedScale - 1.0f)) / m_alodRange);
                if (*maxSpeed > speed)
                    *maxSpeed = speed;
            }
        }
    }
    return 1;
}

// ---- src/VehicleAIClass/ResetAI.cpp

int VehicleAIClass::ResetAI()
{
    m_state = 0;
    m_trackPosition.m_segment = 0;
    m_flags &= ~8;
    return 1;
}

// ---- src/VehicleAIClass/FreeStateSpace.cpp

int VehicleAIClass::FreeStateSpace(AIStateSpaceType_Tag **root)
{
    StateStack *stack, *top, *item;
    AIStateDataType_Tag *action, *next;

    if (!root)
        return 0;
    if (*root) {
        if (!(item = (StateStack *)calloc(1, sizeof(StateStack))))
            return 0;
        item->state = *root;
        item->next = 0;
        stack = item;
        while (stack) {
            top = stack;
            stack = stack->next;
            if (top->state->left) {
                if (!(item = (StateStack *)calloc(1, sizeof(StateStack))))
                    return 0;
                item->state = top->state->left;
                item->next = stack;
                stack = item;
            }
            if (top->state->right) {
                if (!(item = (StateStack *)calloc(1, sizeof(StateStack))))
                    return 0;
                item->state = top->state->right;
                item->next = stack;
                stack = item;
            }
            action = top->state->states;
            while (action) {
                next = action;
                action = action->next;
                free(next);
            }
            free(top->state);
            free(top);
        }
        *root = 0;
    }
    return 1;
}

// ---- src/VehicleAIClass/GetNewState.cpp

int VehicleAIClass::GetNewState(AIStateDataType_Tag *state, AIStateDataType_Tag **newState, _D3DVECTOR position)
{
    _D3DVECTOR toState;
    AIStateDataType_Tag *best = state;
    StateStack *item = (StateStack *)calloc(1, sizeof(StateStack));
    float bestDistance = 100000.0f;
    float bestHeight = bestDistance;
    float bestTurn = bestDistance;
    item->state = AIStateSpace;
    StateStack *stack = item;
    while (stack) {
        StateStack *top = stack;
        stack = stack->next;
        AIStateSpaceType_Tag *node = top->state;
        free(top);
        float p, d;
        switch (node->axis) {
        case 0:
            p = position.x;
            d = node->split - m_xD00.x;
            break;
        case 1:
            p = position.y;
            d = node->split - m_xD00.y;
            break;
        case 2:
            p = position.z;
            d = node->split - m_xD00.z;
            break;
        }
        float ad = Fabs(d);
        if (ad <= node->radius + p) {
            for (AIStateDataType_Tag *s = node->states; s; s = s->next) {
                toState = s->m_position - m_xD00;
                float dot = DotProduct(&toState, &s->m_direction);
                float distance = Magnitude(toState);
                float height, turn;
                if (!state) {
                    if (distance < 20.0f) {
                        height = Fabs(s->m_height - m_xD40);
                        if (height < 20.0f) {
                            turn = Magnitude(s->m_direction - m_xD50);
                            if (!(turn < 0.5f) && turn < bestTurn) {
                                bestDistance = distance;
                                bestHeight = height;
                                bestTurn = turn;
                                best = s;
                            }
                        } else if (height < bestHeight) {   // retail copies turn (and below, height) unset here
                            bestDistance = distance;
                            bestHeight = height;
                            bestTurn = turn;
                            best = s;
                        }
                    } else if (distance < bestDistance) {
                        bestDistance = distance;
                        bestHeight = height;
                        bestTurn = turn;
                        best = s;
                    }
                } else if (!(dot < 10.0f) && distance < bestDistance) {
                    bestDistance = distance;
                    best = s;
                }
            }
            if (ad <= p) {
                if (node->left) {
                    StateStack *item = (StateStack *)calloc(1, sizeof(StateStack));
                    item->state = node->left;
                    item->next = stack;
                    stack = item;
                }
                if (node->right) {
                    StateStack *item = (StateStack *)calloc(1, sizeof(StateStack));
                    item->state = node->right;
                    item->next = stack;
                    stack = item;
                }
            } else if (d <= 0.0f) {
                if (node->left) {
                    StateStack *item = (StateStack *)calloc(1, sizeof(StateStack));
                    item->state = node->left;
                    item->next = stack;
                    stack = item;
                }
            } else {
                if (node->right) {
                    StateStack *item = (StateStack *)calloc(1, sizeof(StateStack));
                    item->state = node->right;
                    item->next = stack;
                    stack = item;
                }
            }
        }
    }
    *newState = best;
    return 1;
}

// ---- src/VehicleAIClass/Render3D.cpp

int VehicleAIClass::Render3D()
{
    return GameObject::Render3D();
}

// ---- src/sinit/VehicleAI.cpp
// Static initializer of VehicleAI.cpp: only the axis vectors from the shared header.
