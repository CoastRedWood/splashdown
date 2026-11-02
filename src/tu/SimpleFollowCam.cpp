// SimpleFollowCam.cpp  @ 0x00300670 (text 0x300670-0x303A20)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's SimpleFollowCam.cpp as one translation unit:
// FollowCamIHitSomethingCallback and SimpleFollowCamera, then the static
// initializer (only the axis vectors). Its .rodata ends with the
// SimpleFollowCamera RTTI and its own copy of CameraBehavior's (0x4306E8).
// CameraBehavior.cpp follows.



#include "d3dtypes.h"
#include "CameraBehavior/CameraBehavior.h"
#include "SimpleFollowCamera/SimpleFollowCamera.h"
#include "SearchableObject/SearchableObject.h"
#include "GameObject/GameObject.h"
#include "CollisionObject/CollisionObject.h"
#include "ViewPortCode/ViewPortCode.h"
#include "CameraFilter/CameraFilter.h"
#include "CameraPreset/CameraPreset.h"
#include "ParameterBlock/ParameterBlock.h"
#include "PhysicsBody/PhysicsBody.h"
#include "SoultreeObject/SoultreeObject.h"
#include "DebugOverlay/DebugOverlay.h"
#include "JetskiGame/JetskiGame.h"
#include "CameraVector/CameraVector.h"
#include "CameraStateParams/CameraStateParams.h"
#include "soultree_assert_block.h"
#include "sinit/AxisVectors.h"

// inline_depth is TU-wide (ComputeLookPoint's).
#pragma inline_depth(6)

// ---- src/global/FollowCamIHitSomethingCallback.cpp

extern const unsigned int WORLDOBJECT_STATICOBJECT;

// What the follow camera's CollisionObject holds at +0x70 when a ray hits.
#ifdef SPLASHDOWN_PC
// x86-64: global/CollisionTree.h's RayToFaceIntersectionData (m_normal, m_rayPoints)
#include "global/CollisionTree.h"
struct FollowCamTarget {
    char pad0[__builtin_offsetof(RayToFaceIntersectionData, m_normal)];
    _D3DVECTOR m_position;
    char pad20[__builtin_offsetof(RayToFaceIntersectionData, m_rayPoints) -
               __builtin_offsetof(RayToFaceIntersectionData, m_normal) - sizeof(_D3DVECTOR)];
    _D3DVECTOR *m_hitPoint;
};
#else
struct FollowCamTarget {
    char pad0[0x10];
    _D3DVECTOR m_position;          // +0x10
    char pad20[0x5C - 0x20];
    _D3DVECTOR *m_hitPoint;         // +0x5C
};
#endif

void FollowCamIHitSomethingCallback(CollisionObject *me, CollisionObject *other)
{
    if (other->m_ownerType & WORLDOBJECT_STATICOBJECT) {
        SimpleFollowCamera *cam = (SimpleFollowCamera *)me->m_owner;
        FollowCamTarget *target = (FollowCamTarget *)me->m_intersectionData;
        cam->m_E0 = *target->m_hitPoint;
        cam->m_F0 = target->m_position;
        cam->m_x114 = 1;
    }
}


// ---- src/SimpleFollowCamera/ctor.cpp


SimpleFollowCamera::SimpleFollowCamera()
{
    m_fov = 80.0f;
    m_body = 0;
    m_object = 0;
    m_collision = 0;
    m_lookFilters[0] = 0;
    m_lookFilters[1] = 0;
    m_lookFilters[2] = 0;
    m_positionFilters[0] = 0;
    m_positionFilters[1] = 0;
    m_positionFilters[2] = 0;
    m_x40 = 0;
    m_x44 = 0;
    m_x48 = 0;
    m_fovFilter = 0;
    m_distanceFilter = 0;
    m_distance = 0;
    m_settings[0] = 0;
    m_settings[1] = 0;
    m_settings[2] = 0;
    m_settings[3] = 0;
    m_savedSettings[0] = 0;
    m_savedSettings[1] = 0;
    m_savedSettings[2] = 0;
    m_savedSettings[3] = 0;
    m_reverseOffset[0] = 0;
    m_reverseOffset[1] = 0;
    m_reverseOffset[2] = 0;
    m_presetFile = 0;
    m_scaledB = 0;
    m_a = 3.0f;
    m_angleX = 0;
    m_angleY = 0;
    m_angleXSpeed = 1.0f;
    m_x18 = 1.0f;
    m_numPresets = 0;
    m_curPreset = 0;
    m_presets = 0;
    m_resetFilters = 0;
    m_x110 = 0;
    m_x114 = 0;
    m_E0 = ZeroVector;
    m_A0 = ZeroVector;
    m_B0 = ZeroVector;
    m_presetSet = 0;
}


// ---- src/SimpleFollowCamera/dtor.cpp
extern "C" void free(void *p);





struct CameraPreset;

SimpleFollowCamera::~SimpleFollowCamera()
{
    if (m_lookFilters[0])
        delete m_lookFilters[0];
    if (m_lookFilters[1])
        delete m_lookFilters[1];
    if (m_lookFilters[2])
        delete m_lookFilters[2];
    if (m_positionFilters[0])
        delete m_positionFilters[0];
    if (m_positionFilters[1])
        delete m_positionFilters[1];
    if (m_positionFilters[2])
        delete m_positionFilters[2];
    if (m_x40)
        delete m_x40;
    if (m_x44)
        delete m_x44;
    if (m_x48)
        delete m_x48;
    if (m_fovFilter)
        delete m_fovFilter;
    if (m_distanceFilter)
        delete m_distanceFilter;
    if (m_presets)
        free(m_presets);
    if (m_presetFile)
        delete[] m_presetFile;
    if (m_collision) {
        m_collision->RemoveFromSearch();
        m_collision->Release();
    }
}


// ---- src/SimpleFollowCamera/Initialize.cpp
extern "C" void *malloc(unsigned int size);
extern "C" void *memcpy(void *dst, const void *src, unsigned int n);
extern "C" char *strcpy(char *dst, const char *src);


struct CameraStateParams;
struct CameraJoystickInput;
class SearchNode;
class ParameterBlock;


// A filtered camera parameter (0x14 bytes).


int SimpleFollowCamera::Initialize(_D3DVECTOR position, SearchNode *node, float distanceRate, int numPresets,
                                   FollowCameraPreset *presets, float lookRate, float angleRate, float heightRate,
                                   char *presetFile, int collision, float fovRate, ViewPortCode viewport)
{
    if (!m_fovFilter)
        m_fovFilter = new CameraFilter(m_fov, distanceRate);
    if (!m_distanceFilter)
        m_distanceFilter = new CameraFilter(0.0f, heightRate);
    if (!m_lookFilters[0])
        m_lookFilters[0] = new CameraFilter(0.0f, lookRate);
    if (!m_lookFilters[1])
        m_lookFilters[1] = new CameraFilter(0.0f, lookRate);
    if (!m_lookFilters[2])
        m_lookFilters[2] = new CameraFilter(0.0f, lookRate);
    if (!m_positionFilters[0])
        m_positionFilters[0] = new CameraFilter(0.0f, angleRate);
    if (!m_positionFilters[1])
        m_positionFilters[1] = new CameraFilter(0.0f, angleRate);
    if (!m_positionFilters[2])
        m_positionFilters[2] = new CameraFilter(0.0f, angleRate);
    if (!m_x40)
        m_x40 = new CameraFilter(0.4f, angleRate);
    if (!m_x44)
        m_x44 = new CameraFilter(0.4f, angleRate);
    if (!m_x48)
        m_x48 = new CameraFilter(0.4f, angleRate);
    _D3DVECTOR zero(0.0f, 0.0f, 0.0f);
    ResetLookFilters(zero);
    ResetPositionFilters(zero);
    m_presetSet = viewport;
    m_fovRate = fovRate;
    if (!presetFile) {
        m_numPresets = numPresets;
        if (m_numPresets <= 0)
            return 0;
        int size = m_numPresets * sizeof(CameraPreset);
        if (!m_presets)
            m_presets = (CameraPreset *)malloc(size);
        if (m_presets)
            memcpy(m_presets, presets, size);
        else
            return 0;
    } else {
        if (!m_presetFile)
            m_presetFile = new char[0x104];
        strcpy(m_presetFile, presetFile);
        if (!LoadPresetsFromFile())
            return 0;
    }
    ChangeToPreset(0);
    if (collision)
        SetupCollisionObject(position, node);
    return 1;
}


// ---- src/SimpleFollowCamera/SetupCollisionObject.cpp

class SoultreeObject;
class SearchNode;
class CollisionObject;




void SimpleFollowCamera::SetupCollisionObject(_D3DVECTOR position, SearchNode *node)
{
    if (!m_collision) {
        m_collision = new CollisionObject(1);
        m_collision->Create(2);
        m_collision->m_owner = this;
        m_collision->m_callback = FollowCamIHitSomethingCallback;
        m_collision->SetTestListType((CollisionObjectTestListType)0);
        m_collision->m_ignoreVegetation = 1;
        m_collision->SetDynamicRayCollision(1, 0, 0);
        _D3DVECTOR rays[2];
        rays[0] = rays[1] = position;
        m_collision->SetDynamicRays(rays, 0);
    }
    m_collision->RemoveFromSearch();
    m_collision->AddSearchPlacement(node, 2);
}


// ---- src/SimpleFollowCamera/ForcePreset.cpp


// Switch to the preset with this id; returns the previous preset index.
int SimpleFollowCamera::ForcePreset(int id, int save, int restore)
{
    int i;
    int previous = m_curPreset;
    if (save && !restore) {
        m_savedSettings[0] = m_settings[0];
        m_savedSettings[1] = m_settings[1];
        m_savedSettings[2] = m_settings[2];
        m_savedSettings[3] = m_settings[3];
    }
    for (i = 0; i < m_numPresets; i++) {
        if (m_presets[i].m_id == id) {
            ChangeToPreset(i);
            break;
        }
    }
    if (restore && !save) {
        m_settings[0] = m_savedSettings[0];
        m_settings[1] = m_savedSettings[1];
        m_settings[2] = m_savedSettings[2];
        m_settings[3] = m_savedSettings[3];
    }
    return previous;
}


// ---- src/Portal/GetCurrentPresetID.cpp


int SimpleFollowCamera::GetCurrentPresetID()
{
    return m_presets[m_curPreset].m_id;
}


// ---- src/SimpleFollowCamera/NextPreset.cpp


// Next enabled preset, wrapping around.
int SimpleFollowCamera::NextPreset()
{
    int next = m_curPreset + 1;
    int found = 0;
    if (m_numPresets == 1) {
        ChangeToPreset(0);
        return;
    }
    do {
        if (next < m_numPresets && m_presets[next].m_cycleThruThisPreset)
            found = 1;
        else if (next >= m_numPresets)
            next = 0;
        else
            next++;
    } while (!found);
    ChangeToPreset(next);
}


// ---- src/SimpleFollowCamera/SetPhysicsBody.cpp
class PhysicsBody;
class SoultreeObject;


void SimpleFollowCamera::SetPhysicsBody(PhysicsBody *target, float a, float b)
{
    m_body = target;
    m_object = 0;
    m_scaledB = 1.46666002f * b;
    m_a = a;
}


// ---- src/SimpleFollowCamera/SetSoultreeObject.cpp
class PhysicsBody;
class SoultreeObject;


void SimpleFollowCamera::SetSoultreeObject(SoultreeObject *target, float a, float b)
{
    m_object = target;
    m_body = 0;
    m_scaledB = 1.46666002f * b;
    m_a = a;
}


// ---- src/SimpleFollowCamera/SetInputs.cpp
void PSXprintf(const char *fmt, ...);



void SimpleFollowCamera::SetInputs(CameraJoystickInput input)
{
    if (input.m_button[2] && m_presetFile) {
        ParameterBlock block;
        if (!block.OpenFile(m_presetFile, 0))
            PSXprintf("Could not open parameter block file %s!\n", m_presetFile);
        else
            ReadPresetInfoFromFile(&block);
    }
    if (input.m_button[0])
        m_settings[0] += -0.3f * input.m_axis[3];
    if (input.m_button[1])
        m_settings[3] += -0.5f * input.m_axis[3];
    if (!input.m_button[0] && !input.m_button[1]) {
        m_settings[1] = m_settings[1] + 0.05f * input.m_axis[2];
        if (m_settings[1] > 3.14159274f)
            m_settings[1] -= 6.28318548f;
        else if (m_settings[1] < -3.14159274f)
            m_settings[1] += 6.28318548f;
        float dy = 0.05f * input.m_axis[3];
        if (dy < 0.0f) {
            if (!m_x110)
                m_settings[2] += dy;
        } else {
            m_settings[2] += dy;
        }
        if (m_settings[2] > 3.14159274f)
            m_settings[2] -= 6.28318548f;
        else if (m_settings[2] < -3.14159274f)
            m_settings[2] += 6.28318548f;
    }
}


// ---- src/SimpleFollowCamera/ComputeCameraData.cpp



extern JetskiGame *LpGame;
// Also read by JetskiFollowCam.cpp, CinematographerCam.cpp and JetskiTVCamera.cpp;
// retail's .data puts it before BuoyObject.cpp's, so this TU or JetskiFollowCam.cpp.
int g_DebugOverlayID = -1;

int RotateAroundAxis(float angle, _D3DVECTOR axis, _D3DVECTOR v, _D3DVECTOR *out);

// libvu0 sceVu0Normalize, in place.
static inline void Normalize(_D3DVECTOR *v)
{
    asm("lqc2 vf4, 0x0(%0); vmul.xyz vf5, vf4, vf4; vaddy.x vf5, vf5, vf5y; vaddz.x vf5, vf5, vf5z; vrsqrt Q, vf0w, vf5x; vsub.xyzw vf6, vf0, vf0; vaddw.xyzw vf6, vf6, vf4w; vwaitq; vmulq.xyz vf6, vf4, Q; sqc2 vf6, 0x0(%0)"
        : : "r"(v) : "memory");
}

// VU0 cross product, out = a x b.
static inline void CrossProduct(_D3DVECTOR *out, _D3DVECTOR *a, _D3DVECTOR *b)
{
    asm("lqc2 vf4, 0x0(%1); lqc2 vf5, 0x0(%2); vopmula.xyz ACC, vf4, vf5; vopmsub.xyz vf6, vf5, vf4; vsub.w vf6, vf6, vf6; sqc2 vf6, 0x0(%0)"
        : : "r"(out), "r"(a), "r"(b) : "memory");
}

// xyz dot product on VU0.
static inline float DotProduct(_D3DVECTOR *a, _D3DVECTOR *b)
{
    float r;
    asm("lqc2 vf4, 0x0(%1); lqc2 vf5, 0x0(%2); vmul.xyz vf5, vf4, vf5; vaddy.x vf5, vf5, vf5y; vaddz.x vf5, vf5, vf5z; qmfc2.ni $2, vf5; mtc1 $2, %0"
        : "=f"(r) : "r"(a), "r"(b) : "$2");
    return r;
}

static inline float Sqrt(float x)
{
    float r;
    asm("sqrt.s $f8, %1; swc1 $f8, 0(%0)" : : "r"(&r), "f"(x) : "$f8", "memory");
    return r;
}

// 1/sqrt into a caller variable (retail's stack slots need it to be a named
// local of the caller).
static inline float RSqrtTo(float *r, float x)
{
    asm("rsqrt.s $f8, %1, %2; swc1 $f8, 0(%0)" : : "r"(r), "f"(1.0f), "f"(x) : "$f8", "memory");
    return *r;
}

void SimpleFollowCamera::ComputeCameraData(float dt, int &state, _D3DVECTOR *position, _D3DVECTOR *lookPoint,
                                           float *fov, _D3DVECTOR *a, _D3DVECTOR *b, float *c)
{
    _D3DVECTOR camPos;
    _D3DVECTOR look;
    _D3DVECTOR target;
    _D3DVECTOR forward;
    _D3DVECTOR up;
    _D3DVECTOR offset;
    _D3DVECTOR velocity;
    float speed;
    float zoom;
    float turn;
    float swing;
    float curFov;
    float distance;
    float invLength;
    int reversing;
    curFov = ComputeFOV();
    if (m_body) {
        SoultreeObject *object = m_body->m_object;
        object->GetPosition(0, &target);
        object->GetOrientation(0, &forward, &up);
        forward.y = 0.0f;
        Normalize(&forward);
        velocity = m_body->m_velocity;
        turn = -m_body->m_xA0.y;
    } else {
        m_object->GetPosition(0, &target);
        m_object->GetOrientation(0, &forward, &up);
        forward.y = 0.0f;
        Normalize(&forward);
        if (m_A0 == ZeroVector)
            velocity = _D3DVECTOR();
        else
            velocity = (target - m_A0) / dt;
        if (m_B0 == ZeroVector) {
            turn = 0.0f;
        } else {
            _D3DVECTOR cross;
            CrossProduct(&cross, &forward, &m_B0);
            turn = cross.y / dt;
        }
        m_A0 = target;
        m_B0 = forward;
    }
    zoom = curFov / m_presets[m_curPreset].m_maxFOV;
    swing = zoom * (0.21f * m_angleXSpeed * turn);
    speed = Sqrt(velocity.x * velocity.x + velocity.y * velocity.y + velocity.z * velocity.z);
    reversing = ComputeReverseOffsets(forward, velocity, speed);
    ComputeAngleXAndY(swing);
    _D3DVECTOR side;
    CrossProduct(&side, &forward, &YVector);
    RotateAroundAxis(m_angleY, side, forward, &camPos);
    RotateAroundAxis(m_angleX, YVector, camPos, &offset);
    m_distance = m_reverseOffset[1] + (m_settings[0] + m_presets[m_curPreset].m_followingDistance);
    if (m_distance < m_presets[m_curPreset].m_minFollowingDistance) {
        m_distance = m_presets[m_curPreset].m_minFollowingDistance;
        m_settings[0] = m_distance - m_presets[m_curPreset].m_followingDistance;
    } else if (m_distance > m_presets[m_curPreset].m_maxFollowingDistance) {
        m_distance = m_presets[m_curPreset].m_maxFollowingDistance;
        m_settings[0] = m_distance - m_presets[m_curPreset].m_followingDistance;
    }
    m_distance += m_reverseOffset[1];
    look = ComputeLookPoint(target, forward, velocity, speed, zoom, swing, reversing);
    camPos = ComputePosition(offset, target, forward, look, side, velocity);
    distance = 0.0f;
    FilterCameraParameters(&camPos, &look, &curFov, &distance, dt);
    if (m_collision) {
        _D3DVECTOR rays[2];
        rays[0] = target + 2.0f * up;
        rays[1] = camPos;
        m_x114 = 0;
        m_collision->UpdateCollisionInfo();
        m_collision->SetDynamicRays(rays, 0);
        m_collision->CheckForCollision(0);
        if (m_x114) {
            _D3DVECTOR dir = rays[1] - rays[0];
            float len2 = dir.x * dir.x + dir.y * dir.y + dir.z * dir.z;
            if (len2 == 0.0f) {
                dir = ZeroVector;
            } else {
                float r = RSqrtTo(&invLength, len2);
                dir.x *= r;
                dir.y *= r;
                dir.z *= r;
            }
            camPos = m_E0 - 2.0f * dir;
        }
    }
    if (position)
        *position = camPos;
    if (lookPoint)
        *lookPoint = look;
    if (fov)
        *fov = curFov;
    m_C0 = camPos;
    m_D0 = look;
    DebugOverlay *overlay = LpGame->m_debugOverlay;
    if (overlay) {
        if (g_DebugOverlayID < 0) {
            int page = overlay->m_numPages;
            overlay->m_numPages = page + 1;
            g_DebugOverlayID = page;
        }
        LpGame->m_debugOverlay->PrintfHeading(g_DebugOverlayID, "Follow Camera Preset Info");
        if (m_body)
            LpGame->m_debugOverlay->PrintfDetail(g_DebugOverlayID, "Following a PhysicsBody");
        else
            LpGame->m_debugOverlay->PrintfDetail(g_DebugOverlayID, "Following a SoultreeObject");
        LpGame->m_debugOverlay->PrintfDetail(g_DebugOverlayID, "AngleY: %f = %f degrees", m_angleX, 57.29578f * m_angleX);
        LpGame->m_debugOverlay->PrintfDetail(g_DebugOverlayID, "AngleX: %f = %f degrees", m_angleY, 57.29578f * m_angleY);
        LpGame->m_debugOverlay->PrintfDetail(g_DebugOverlayID, "FollowingDistance: %f", m_distance);
        LpGame->m_debugOverlay->PrintfDetail(g_DebugOverlayID, "FOV: %f", m_fov + m_settings[3]);
    }
}


// ---- src/SimpleFollowCamera/SwitchOn.cpp


void SimpleFollowCamera::SwitchOn(CameraStateParams params, int copyFilter)
{
    if (!copyFilter || !CopyFilterData(params))
        m_resetFilters = 1;
}


// ---- src/SimpleFollowCamera/SwitchOff.cpp




CameraStateParams SimpleFollowCamera::SwitchOff()
{
    CameraStateParams params;
    float f38 = m_angleY + m_settings[2];
    float f3C = m_angleX + m_settings[1];
    if (!m_presets[m_curPreset].m_filterThisPreset || m_resetFilters) {
        m_positionFilters[0]->m_value = m_C0.x;
        m_positionFilters[1]->m_value = m_C0.y;
        m_positionFilters[2]->m_value = m_C0.z;
        m_lookFilters[0]->m_value = m_D0.x;
        m_lookFilters[1]->m_value = m_D0.y;
        m_lookFilters[2]->m_value = m_D0.z;
    }
    params.f38 = f38;
    params.f3C = f3C;
    params.p40 = (float *)m_positionFilters[0];
    params.p44 = (float *)m_positionFilters[1];
    params.p48 = (float *)m_positionFilters[2];
    params.p4C = (float *)m_lookFilters[0];
    params.p50 = (float *)m_lookFilters[1];
    params.p54 = (float *)m_lookFilters[2];
    params.p58 = (float *)m_fovFilter;
    params.p5C = (float *)m_distanceFilter;
    return params;
}


// ---- src/SimpleFollowCamera/CopyFilterData.cpp


// Copies the filter states of the camera being switched from. The last
// check looks at the wrong source entry (retail bug, reproduced).
int SimpleFollowCamera::CopyFilterData(CameraStateParams params)
{
    int ok = 1;
    if (m_positionFilters[0] && params.p40) {
        m_positionFilters[0]->m_value = params.p40[0];
        m_positionFilters[0]->m_scale = params.p40[2];
    } else {
        ok = 0;
    }
    if (m_positionFilters[1] && params.p44) {
        m_positionFilters[1]->m_value = params.p44[0];
        m_positionFilters[1]->m_scale = params.p44[2];
    } else {
        ok = 0;
    }
    if (m_positionFilters[2] && params.p48) {
        m_positionFilters[2]->m_value = params.p48[0];
        m_positionFilters[2]->m_scale = params.p48[2];
    } else {
        ok = 0;
    }
    if (m_lookFilters[0] && params.p4C) {
        m_lookFilters[0]->m_value = params.p4C[0];
        m_lookFilters[0]->m_scale = params.p4C[2];
    } else {
        ok = 0;
    }
    if (m_lookFilters[1] && params.p50) {
        m_lookFilters[1]->m_value = params.p50[0];
        m_lookFilters[1]->m_scale = params.p50[2];
    } else {
        ok = 0;
    }
    if (m_lookFilters[2] && params.p54) {
        m_lookFilters[2]->m_value = params.p54[0];
        m_lookFilters[2]->m_scale = params.p54[2];
    } else {
        ok = 0;
    }
    if (m_fovFilter && params.p58) {
        m_fovFilter->m_value = params.p58[0];
        m_fovFilter->m_scale = params.p58[2];
    } else {
        ok = 0;
    }
    if (m_distanceFilter && params.p58) {
        m_distanceFilter->m_value = params.p5C[0];
        m_distanceFilter->m_scale = params.p5C[2];
    } else {
        ok = 0;
    }
    return ok;
}


// ---- src/SimpleFollowCamera/ChangeToPreset.cpp



void SimpleFollowCamera::ChangeToPreset(int index)
{
#line 741 "SimpleFollowCam.cpp"
    ASSERTF(index >= 0 && index < m_numPresets, ("ChangeToPreset: newPresetIndex (%d) out of range", index));
    if (m_presets[m_curPreset].m_filterThisPreset == 0 && m_presets[index].m_filterThisPreset == 1)
        m_resetFilters = 1;
    m_curPreset = index;
    m_angleX = m_presets[index].m_angleY;
    m_angleY = m_presets[index].m_angleX;
    m_distance = m_presets[index].m_followingDistance;
    m_fov = m_presets[index].m_fov;
    m_angleXSpeed = m_presets[index].m_cameraSwing;
    m_x18 = m_presets[index].m_lookSwing;
    m_settings[0] = 0;
    m_settings[1] = 0;
    m_settings[2] = 0;
    m_settings[3] = 0;
}


// ---- src/SimpleFollowCamera/ResetLookFilters.cpp

void SimpleFollowCamera::ResetLookFilters(_D3DVECTOR &v)
{
    if (m_lookFilters[0])
        m_lookFilters[0]->m_value = v.x;
    if (m_lookFilters[1])
        m_lookFilters[1]->m_value = v.y;
    if (m_lookFilters[2])
        m_lookFilters[2]->m_value = v.z;
}


// ---- src/SimpleFollowCamera/ResetPositionFilters.cpp

void SimpleFollowCamera::ResetPositionFilters(_D3DVECTOR &v)
{
    if (m_positionFilters[0])
        m_positionFilters[0]->m_value = v.x;
    if (m_positionFilters[1])
        m_positionFilters[1]->m_value = v.y;
    if (m_positionFilters[2])
        m_positionFilters[2]->m_value = v.z;
}


// ---- src/SimpleFollowCamera/ConstrainCamera.cpp
int SimpleFollowCamera::ConstrainCamera(_D3DVECTOR *, int, int)
{
    return 1;
}

float SimpleFollowCamera::CompensateForLookPointObstacles(_D3DVECTOR)
{
    return 0.0f;
}

// ---- src/SimpleFollowCamera/ComputeFOV.cpp

float SimpleFollowCamera::ComputeFOV()
{
    float fov = m_fov + m_settings[3];
    if (fov > m_presets[m_curPreset].m_maxFOV) {
        fov = m_presets[m_curPreset].m_maxFOV;
        m_settings[3] = fov - m_fov;
    } else if (fov < m_presets[m_curPreset].m_minFOV) {
        fov = m_presets[m_curPreset].m_minFOV;
        m_settings[3] = fov - m_fov;
    }
    return fov;
}


// ---- src/SimpleFollowCamera/ComputeReverseOffsets.cpp

// libvu0 sceVu0Normalize.
static inline void Normalize(_D3DVECTOR *out, _D3DVECTOR *in)
{
    asm("lqc2 vf4, 0x0(%1); vmul.xyz vf5, vf4, vf4; vaddy.x vf5, vf5, vf5y; vaddz.x vf5, vf5, vf5z; vrsqrt Q, vf0w, vf5x; vsub.xyzw vf6, vf0, vf0; vaddw.xyzw vf6, vf6, vf4w; vwaitq; vmulq.xyz vf6, vf4, Q; sqc2 vf6, 0x0(%0)"
        : : "r"(out), "r"(in) : "memory");
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

float FastACos(float);



// When moving fast enough while facing backwards (angle between pi/2 and
// 3pi/2), grow the reverse offsets towards the preset's limits and return 1;
// otherwise shrink them back to zero.
int SimpleFollowCamera::ComputeReverseOffsets(_D3DVECTOR forward, _D3DVECTOR direction, float speed)
{
    Normalize(&direction, &direction);
    float angle = FastACos(DotProduct(&forward, &direction));
    if (angle > 1.5707964f && angle < 4.712389f && speed > m_fovRate) {
        m_reverseOffset[0] += 0.05f * m_presets[m_curPreset].m_forwardReverseSwitchSpeed;
        m_reverseOffset[0] = Fmin(m_presets[m_curPreset].m_reverseAngleXOffset, m_reverseOffset[0]);
        m_reverseOffset[1] += m_presets[m_curPreset].m_forwardReverseSwitchSpeed;
        m_reverseOffset[1] = Fmin(m_presets[m_curPreset].m_reverseFollowingOffset, m_reverseOffset[1]);
        m_reverseOffset[2] += 0.05f * m_presets[m_curPreset].m_forwardReverseSwitchSpeed;
        m_reverseOffset[2] = Fmin(m_presets[m_curPreset].m_reverseLookAheadOffset, m_reverseOffset[2]);
        return 1;
    }
    if (m_reverseOffset[1] != 0.0f) {
        m_reverseOffset[0] -= 0.05f * m_presets[m_curPreset].m_forwardReverseSwitchSpeed;
        m_reverseOffset[0] = Fmax(0.0f, m_reverseOffset[0]);
        m_reverseOffset[1] -= m_presets[m_curPreset].m_forwardReverseSwitchSpeed;
        m_reverseOffset[1] = Fmax(0.0f, m_reverseOffset[1]);
        m_reverseOffset[2] -= m_presets[m_curPreset].m_forwardReverseSwitchSpeed;
        m_reverseOffset[2] = Fmax(0.0f, m_reverseOffset[2]);
    }
    return 0;
}


// ---- src/SimpleFollowCamera/ComputeAngleXAndY.cpp
void PlusMinusPi(float &angle);


// Clamp the vertical angle to +-85 degrees (1.48353243f), pushing the excess back into
// the input.
void SimpleFollowCamera::ComputeAngleXAndY(float dt)
{
    m_angleX = m_presets[m_curPreset].m_angleY - m_settings[1];
    m_angleX -= m_angleXSpeed * dt;
    PlusMinusPi(m_angleX);
    m_angleY = m_settings[2] + m_presets[m_curPreset].m_angleX;
    m_angleY -= m_reverseOffset[0];
    if (m_angleY < -1.48353243f) {
        m_angleY = -1.48353243f;
        m_settings[2] = m_reverseOffset[0] + (-1.48353243f - m_presets[m_curPreset].m_angleX);
    } else if (m_angleY > 1.48353243f) {
        m_angleY = 1.48353243f;
        m_settings[2] = (1.48353243f - m_presets[m_curPreset].m_angleX) - m_reverseOffset[0];
    }
}


// ---- src/SimpleFollowCamera/ComputeLookPoint.cpp
// SimpleFollowCamera::ComputeLookPoint returns the look point.


static inline float Fabs(float x)
{
    float r;
    asm("abs.s $f8, %1; swc1 $f8, 0(%0)" : : "r"(&r), "f"(x) : "$f8", "memory");
    return r;
}

inline _D3DVECTOR operator-(const _D3DVECTOR &v)
{
    return _D3DVECTOR(-v.x, -v.y, -v.z);
}




_D3DVECTOR SimpleFollowCamera::ComputeLookPoint(_D3DVECTOR position, _D3DVECTOR direction, _D3DVECTOR velocity,
                                                float a, float b, float angle, int reversing)
{
    _D3DVECTOR look;
    _D3DVECTOR rotated;
    // Declared up front in this order: the order sets the FP registers.
    float limit;
    float speedFactor;
    float d;
    float grow;
    float shrink;
    float height;
    float angleX;
    float zero;
    float halfPi;
    float facing;
    float distance;
    speedFactor = Fmin(1.0f, DotProduct(&velocity, &direction) / m_scaledB);
    d = m_distance;
    grow = 1.0f + d * b / 90.0f;
    shrink = 1.0f - d / 600.0f;
    height = speedFactor * (grow * (1.8f * shrink * grow));
    if (a != 0.0f) {
        if (velocity.y < -5.0f)
            height *= 1.0f + velocity.y / a;
        else if (velocity.y > 5.0f)
            height *= 1.0f - velocity.y / a;
    }
    // A dead angle-range check (both arms empty): retail keeps the compares
    // and the bc1f + b branch shape of an if/else-if. 'zero' is a named
    // variable in retail too (its register is coloured after height).
    halfPi = 1.5707964f;
    limit = 6.2831855f;
    limit -= halfPi;
    zero = 0.0f;
    angleX = m_angleX;
    if (angleX >= zero && angleX < halfPi) {
        angle = angle;
    } else if (m_angleX > limit) {
        angle = angle;
    }
    facing = Fmax(0.0f, 1.0f - Fabs(m_angleY) / 1.5707964f);
    if (reversing)
        distance = m_reverseOffset[2];
    else
        distance = height * m_presets[m_curPreset].m_lookAheadDistance;
    if (!m_presets[m_curPreset].m_isLookBackCam) {
        angle *= -0.7f * m_x18;
        RotateAroundAxis(angle, YVector, direction, &rotated);
        Normalize(&rotated, &rotated);
        look = position + distance * rotated;
        look.y += (1.0f - b) * (0.25f * velocity.y);
        _D3DVECTOR ahead = (0.3f + 1.2f * (Fabs(m_angleY) / 1.5707964f)) * rotated + position;
        look.y += CompensateForLookPointObstacles(ahead);
        look.y += m_a;
    } else {
        look = position + m_presets[m_curPreset].m_lookAheadDistance * -direction;
    }
    return look;
}


// ---- src/SimpleFollowCamera/ComputePosition.cpp
// Camera position: `m_distance` along the look direction from the origin.


_D3DVECTOR SimpleFollowCamera::ComputePosition(_D3DVECTOR direction, _D3DVECTOR origin, _D3DVECTOR, _D3DVECTOR, _D3DVECTOR, _D3DVECTOR)
{
    _D3DVECTOR position;
    float d = m_distance;
    // Only x goes through a temporary; retail's register choice for the
    // first component needs it.
    float x = d * direction.x;
    position.x = origin.x + x;
    position.y = origin.y + d * direction.y;
    position.z = origin.z + d * direction.z;
    position.w = 1.0f;
    return position;
}


// ---- src/SimpleFollowCamera/FilterCameraParameters.cpp

// CameraFilter's inline Update.
static inline float UpdateFilter(CameraFilter *f, float dt, float target)
{
    f->m_scale = Fmin(dt, f->m_rate) / f->m_rate;
    f->m_value = f->m_value + f->m_scale * (target - f->m_value);
    return f->m_value;
}

void SimpleFollowCamera::FilterCameraParameters(_D3DVECTOR *position, _D3DVECTOR *look, float *fov, float *distance,
                                                float dt)
{
    ConstrainCamera(position, 1, 1);
    _D3DVECTOR newLook;
    _D3DVECTOR newPosition;
    if (m_presets[m_curPreset].m_filterThisPreset && !m_resetFilters) {
        newLook.x = UpdateFilter(m_lookFilters[0], dt, look->x);
        newLook.y = UpdateFilter(m_lookFilters[1], dt, look->y);
        newLook.z = UpdateFilter(m_lookFilters[2], dt, look->z);
        newPosition.x = UpdateFilter(m_positionFilters[0], dt, position->x);
        newPosition.y = UpdateFilter(m_positionFilters[1], dt, position->y);
        newPosition.z = UpdateFilter(m_positionFilters[2], dt, position->z);
        static int i = 1;           // unused
        float newFov = *fov;
        if (0)                      // compiled out, but its temporary keeps a stack slot
            newFov = UpdateFilter(m_fovFilter, dt, *fov);
        float newDistance = UpdateFilter(m_distanceFilter, dt, *distance);
        ConstrainCamera(&newPosition, 1, 0);
        *position = newPosition;
        *look = newLook;
        *fov = newFov;
        *distance = newDistance;
    } else if (m_resetFilters) {
        ResetLookFilters(*look);
        ResetPositionFilters(*position);
        m_fovFilter->m_value = *fov;
        m_resetFilters = 0;
    }
}


// ---- src/SimpleFollowCamera/LoadPresetsFromFile.cpp
extern "C" void *malloc(unsigned int size);
void PSXprintf(const char *fmt, ...);



struct CameraStateParams;
struct CameraJoystickInput;
struct _D3DVECTOR;
class SearchNode;


int SimpleFollowCamera::LoadPresetsFromFile()
{
    if (!m_presetFile) {
        PSXprintf("No preset filename specified!");
        return 0;
    }
    ParameterBlock block;
    if (!block.OpenFile(m_presetFile, 0)) {
        PSXprintf("Could not open parameter block file %s!\n", m_presetFile);
        return 0;
    }
    block.ReadParameterBlock("NumberOfPresets");
    block.GetParameter("NumberOfPresets", &m_numPresets);
    if (m_numPresets <= 0) {
        PSXprintf("Number of presets can't be 0!\n");
        return 0;
    }
    int size = m_numPresets * sizeof(CameraPreset);
    if (!m_presets)
        m_presets = (CameraPreset *)malloc(size);
    ReadPresetInfoFromFile(&block);
    return 1;
}


// ---- src/SimpleFollowCamera/ReadPresetInfoFromFile.cpp
extern "C" int sprintf(char *buf, const char *fmt, ...);



struct CameraStateParams;
struct CameraJoystickInput;
struct _D3DVECTOR;
class SearchNode;


// Degrees to radians; retail's constant is one ulp below the nearest float to pi/180.
#define DEG_TO_RAD 0.017453291f

void SimpleFollowCamera::ReadPresetInfoFromFile(ParameterBlock *block)
{
    char name[260];
    for (int i = 0; i < m_numPresets; i++) {
        if (m_presetSet == 0)
            sprintf(name, "Preset%d", i + 1);
        else if (m_presetSet == 1)
            sprintf(name, "Preset%dh", i + 1);
        else if (m_presetSet == 2)
            sprintf(name, "Preset%dv", i + 1);
        else
            sprintf(name, "Preset%dq", i + 1);
        block->ReadParameterBlock(name);
        block->GetParameter("ID", &m_presets[i].m_id);
        block->GetParameter("AngleY", &m_presets[i].m_angleY);
        m_presets[i].m_angleY *= DEG_TO_RAD;
        block->GetParameter("AngleX", &m_presets[i].m_angleX);
        m_presets[i].m_angleX *= DEG_TO_RAD;
        block->GetParameter("FollowingDistance", &m_presets[i].m_followingDistance);
        block->GetParameter("FOV", &m_presets[i].m_fov);
        block->GetParameter("ReverseAngleXOffset", &m_presets[i].m_reverseAngleXOffset);
        block->GetParameter("ReverseFollowingOffset", &m_presets[i].m_reverseFollowingOffset);
        block->GetParameter("ReverseLookAheadOffset", &m_presets[i].m_reverseLookAheadOffset);
        block->GetParameter("ForwardReverseSwitchSpeed", &m_presets[i].m_forwardReverseSwitchSpeed);
        block->GetParameter("CycleThruThisPreset", &m_presets[i].m_cycleThruThisPreset);
        block->GetParameter("FilterThisPreset", &m_presets[i].m_filterThisPreset);
        block->GetParameter("DontRotate", &m_presets[i].m_dontRotate);
        block->GetParameter("LookAheadDistance", &m_presets[i].m_lookAheadDistance);
        block->GetParameter("IsLookBackCam", &m_presets[i].m_isLookBackCam);
        block->GetParameter("CameraSwing", &m_presets[i].m_cameraSwing);
        block->GetParameter("LookSwing", &m_presets[i].m_lookSwing);
        block->GetParameter("MinFollowingDistance", &m_presets[i].m_minFollowingDistance);
        block->GetParameter("MaxFollowingDistance", &m_presets[i].m_maxFollowingDistance);
        block->GetParameter("MinFOV", &m_presets[i].m_minFOV);
        block->GetParameter("MaxFOV", &m_presets[i].m_maxFOV);
    }
}
