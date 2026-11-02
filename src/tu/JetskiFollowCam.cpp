// JetskiFollowCam.cpp  @ 0x00303AA0 (text 0x303AA0-0x305980)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's JetskiFollowCam.cpp as one translation unit: JetskiFollowCamera,
// then the static initializer (only the axis vectors). Its .rodata (labels
// @1457-1461) ends with the JetskiFollowCamera RTTI and its own
// SimpleFollowCamera/CameraBehavior copies (0x430800-0x430880).
// AnimTextureMap.cpp follows.



#include "d3dtypes.h"
#include "JetskiFollowCamera/JetskiFollowCamera.h"
#include "ViewPortCode/ViewPortCode.h"
#include "CameraPreset/CameraPreset.h"
#include "PhysicsBody/PhysicsBody.h"
#include "SoultreeObject/SoultreeObject.h"
#include "CollisionObject/CollisionObject.h"
#include "DebugOverlay/DebugOverlay.h"
#include "JetskiGame/JetskiGame.h"
#include "JetskiCompetition/JetskiCompetition.h"
#include "CameraFilter/CameraFilter.h"
#include "CameraStateParams/CameraStateParams.h"
#include "Terrain/Terrain.h"
#include "Water/Water.h"
#include "sinit/AxisVectors.h"

// inline_depth is TU-wide (ComputeLookPoint's and ComputePosition's).
#pragma inline_depth(6)

// ---- src/JetskiFollowCamera/ctor.cpp






JetskiFollowCamera::JetskiFollowCamera()
{
    m_x138 = 1;
    m_x13C = 0;
    m_x14C = 0;
    m_position = ZeroVector;
    m_x160 = ZeroVector;
    m_A0 = ZeroVector;
    m_B0 = ZeroVector;
    m_terrain = 0;
    m_water = 0;
    m_lookPointRaise = 0;
    m_minHeight = 3.0f;
    m_maxHeight = 3.4028235e38f;        // FLT_MAX
    m_x130 = 0;
    m_rotating = 0;
    m_rotateTime = 0.0f;
    m_freezeRotate = 0;
    m_savedLookRate = ZeroVector;
    m_savedPositionRate = ZeroVector;
    m_playbackFilters = 0;
    m_playback = 0;
    m_x1A0 = ZeroVector;
}


// ---- src/JetskiFollowCamera/Initialize.cpp

int JetskiFollowCamera::Initialize(_D3DVECTOR position, SearchNode *node, float a, int b, FollowCameraPreset *preset,
                                   float maxHeight, float c, float d, float e, float minHeight, char *name)
{
    if (!SimpleFollowCamera::Initialize(position, node, a, b, preset, c, d, e, name, 1, 5.0f, (ViewPortCode)0))
        return 0;
    m_minHeight = minHeight;
    m_maxHeight = maxHeight;
    return 1;
}


// ---- src/JetskiFollowCamera/ConstrainCamera.cpp
static inline float Fmin(float a, float b)
{
    float r;
    asm("min.s $f8, %1, %2; swc1 $f8, 0(%0)" : : "r"(&r), "f"(a), "f"(b) : "$f8", "memory");
    return r;
}


// Keeps the camera below its ceiling and at least m_minHeight above the
// terrain or water under it. Returns 0 when it is too low and not adjusted.
int JetskiFollowCamera::ConstrainCamera(_D3DVECTOR *position, int adjust, int track)
{
    _D3DVECTOR ground = *position;
    _D3DVECTOR water = ground;
    if (m_terrain) {
        m_terrain->GetGridPointIntersection(&ground, 0, 0, 0);
        if (m_water) {
            m_water->GetGridPointIntersection(&water, 0);
            if (ground.y < water.y)
                ground.y = water.y;
        }
    } else {
        if (m_water)
            m_water->GetGridPointIntersection(&ground, 0);
        else
            return 1;
    }
    position->y = Fmin(position->y, m_maxHeight);
    if (position->y - ground.y < m_minHeight) {
        if (track)
            m_x110 = 1;
        if (adjust) {
            position->y = ground.y + m_minHeight;
            return 1;
        }
        return 0;
    }
    if (track)
        m_x110 = 0;
    return 1;
}


// ---- src/JetskiFollowCamera/CompensateForLookPointObstacles.cpp

// Raises the look point over terrain in the way, easing back down otherwise.
float JetskiFollowCamera::CompensateForLookPointObstacles(_D3DVECTOR lookPoint)
{
    float y = lookPoint.y;
    _D3DVECTOR point = lookPoint;
    if (m_terrain)
        m_terrain->GetGridPointIntersection(&point, 0, 0, 0);
    if (point.y > y)
        m_lookPointRaise = 0.6f * (point.y - y);
    else
        m_lookPointRaise *= 0.02f;
    return m_lookPointRaise;
}


// ---- src/JetskiFollowCamera/ComputeCameraData.cpp


extern JetskiGame *LpGame;
extern int g_DebugOverlayID;

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

// Fmin into a caller variable: retail's stack slots need step to be a
// named local of the caller.
static inline float FminTo(float *r, float a, float b)
{
    asm("min.s $f8, %1, %2; swc1 $f8, 0(%0)" : : "r"(r), "f"(a), "f"(b) : "$f8", "memory");
    return *r;
}

void JetskiFollowCamera::ComputeCameraData(float dt, int &state, _D3DVECTOR *position, _D3DVECTOR *lookPoint,
                                           float *fov, _D3DVECTOR *a, _D3DVECTOR *b, float *c)
{
    _D3DVECTOR camPos;
    _D3DVECTOR look;
    _D3DVECTOR target;
    _D3DVECTOR forward;
    _D3DVECTOR dir;
    _D3DVECTOR velocity;
    _D3DVECTOR up;
    float speed;
    float zoom;
    float turn;
    float swing;
    float angle;
    float d;                    // computed, never used (as in retail)
    int airborne;
    float distance;
    float step;
    if (m_body) {
        m_body->m_object->GetPosition(0, &target);
        target += m_x1A0;
        m_body->m_object->GetOrientation(0, &forward, &up);
        forward.y = 0.0f;
        Normalize(&forward);
        if (m_rotating) {
            velocity = ZeroVector;
            speed = 0.0f;
            airborne = 0;
            turn = speed;
        } else {
            velocity = m_body->m_velocity;
            speed = m_body->m_speed;
            airborne = m_body->m_x30;
            turn = -m_body->m_xA0.y;
        }
    } else {
        m_object->GetPosition(&target);
        m_object->GetOrientation(&forward, &up);
        forward.y = 0.0f;
        Normalize(&forward);
        if (m_A0 == ZeroVector)
            velocity = _D3DVECTOR();
        else
            velocity = (target - m_A0) / dt;
        speed = Sqrt(velocity.x * velocity.x + velocity.y * velocity.y + velocity.z * velocity.z);
        d = DotProduct(&forward, &velocity);
        if (m_B0 == ZeroVector) {
            turn = 0.0f;
        } else {
            _D3DVECTOR cross;
            CrossProduct(&cross, &forward, &m_B0);
            turn = cross.y / dt;
        }
        m_A0 = target;
        m_B0 = forward;
        airborne = 0;
    }
    zoom = m_fov / m_presets[m_curPreset].m_maxFOV;
    swing = 0.21f * turn * zoom;
    ComputeAngleXAndY(swing);
    if ((!airborne || m_resetFilters) && up.y > 0.0f && !m_presets[m_curPreset].m_dontRotate)
        dir = forward;
    else
        dir = m_x160;
    m_x160 = dir;
    dir.y = 0.0f;
    Normalize(&dir);
    _D3DVECTOR side;
    CrossProduct(&side, &dir, &YVector);
    _D3DVECTOR offset;
    RotateAroundAxis(m_angleY, side, dir, &offset);
    if (m_rotating) {
        if (m_freezeRotate) {
            angle = 0.785398185f;
        } else {
            m_rotateTime += dt;
            if (m_rotateTime > 8.0f)
                m_rotateTime -= 8.0f;
            angle = 2.0f * (3.14159274f * m_rotateTime) / 8.0f;
        }
        RotateAroundAxis(angle, YVector, offset, &offset);
    } else {
        RotateAroundAxis(m_angleX, YVector, offset, &offset);
    }
    m_distance = m_reverseOffset[1] + (m_settings[0] + m_presets[m_curPreset].m_followingDistance);
    if (m_distance < m_presets[m_curPreset].m_minFollowingDistance) {
        m_distance = m_presets[m_curPreset].m_minFollowingDistance;
        m_settings[0] = m_distance - m_presets[m_curPreset].m_followingDistance;
    } else if (m_distance > m_presets[m_curPreset].m_maxFollowingDistance) {
        m_distance = m_presets[m_curPreset].m_maxFollowingDistance;
        m_settings[0] = m_distance - m_presets[m_curPreset].m_followingDistance;
    }
    m_distance += m_reverseOffset[1];
    _D3DVECTOR lookTarget = LpGame->m_competition->m_status != 10 ? target + FminTo(&step, 0.2f, dt) * velocity : target;
    look = ComputeLookPoint(lookTarget, forward, dir, velocity, speed, zoom, swing, 0);
    camPos = ComputePosition(dt, turn, offset, lookTarget, forward, look, side, velocity, speed);
    distance = 0.0f;
    if (!m_rotating)
        FilterCameraParameters(&camPos, &look, &m_fov, &distance, dt);
    if (LpGame->m_competition->m_status == 10 && m_collision) {
        _D3DVECTOR rays[2];
        rays[0] = target + 2.0f * up;
        rays[1] = camPos;
        m_x114 = 0;
        m_collision->UpdateCollisionInfo();
        m_collision->SetDynamicRays(rays, 0);
        m_collision->CheckForCollision(0);
    }
    if (position)
        *position = camPos;
    if (lookPoint)
        *lookPoint = look;
    if (fov)
        *fov = m_fov;
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
        LpGame->m_debugOverlay->PrintfDetail(g_DebugOverlayID, "POS: %4.2f,%4.2f,%4.2f", camPos.x, camPos.y, camPos.z);
    }
}


// ---- src/JetskiFollowCamera/ComputeLookPoint.cpp


static inline float Fabs(float x)
{
    float r;
    asm("abs.s $f8, %1; swc1 $f8, 0(%0)" : : "r"(&r), "f"(x) : "$f8", "memory");
    return r;
}

static inline float Clamp(float v, float a, float b)
{
    if (b > a) {
        if (v > b)
            v = b;
        else if (v < a)
            v = a;
    } else {
        if (v < b)
            v = b;
        else if (v > a)
            v = a;
    }
    return v;
}

// Maps x from [x0, x1] onto [y0, y1], clamped to the output range.
static inline float MapRange(float x, float x0, float x1, float y0, float y1)
{
    float y;
    if (x1 != x0)
        y = Clamp(y0 + (x - x0) * (y1 - y0) / (x1 - x0), y0, y1);
    else
        y = 0.0f;
    return y;
}


// The look point: ahead of the rider along the rotated forward vector, raised
// with speed and distance, eased back when the camera looks behind.
_D3DVECTOR JetskiFollowCamera::ComputeLookPoint(_D3DVECTOR position, _D3DVECTOR, _D3DVECTOR forward, _D3DVECTOR velocity, float speed, float b, float c, int)
{
    _D3DVECTOR lookPoint;
    _D3DVECTOR direction;
    float speedFactor = Fmin(1.0f, speed / m_scaledB);
    float a = 1.0f + m_distance * b / 90.0f;
    float d = 1.0f - m_distance / 600.0f;
    float height = speedFactor * (a * (1.8f * d * a));
    if (speed != 0.0f) {
        if (velocity.y < -5.0f)
            height *= 1.0f + velocity.y / speed;
        else if (velocity.y > 5.0f)
            height *= 1.0f - velocity.y / speed;
    }
    float scale = 2.0f + height * m_presets[m_curPreset].m_lookAheadDistance;
    if (m_presets[m_curPreset].m_isLookBackCam) {
        float angle = m_angleX;
        if (angle > 3.14159274f)
            angle -= 6.28318548f;
        else if (angle < -3.14159274f)
            angle += 6.28318548f;
        scale *= MapRange(Fabs(angle), 60.0f * 0.01745329f, 135.0f * 0.01745329f, 0.0f, 1.0f);
    }
    c *= -0.7f * m_x18;
    RotateAroundAxis(c, YVector, forward, &direction);
    lookPoint = position + scale * direction;
    static float velPct = 0.35f;
    lookPoint.y += (1.0f - b) * (velocity.y * velPct);
    _D3DVECTOR target = (0.3f + 1.2f * (Fabs(m_angleY) / 1.57079637f)) * direction + position;
    CompensateForLookPointObstacles(target);
    lookPoint.y += m_lookPointRaise;
    lookPoint.y += m_a;
    return lookPoint;
}


// ---- src/JetskiFollowCamera/ComputePosition.cpp
// ComputePosition__18JetskiFollowCameraFff10_D3DVECTOR10_D3DVECTOR10_D3DVECTOR10_D3DVECTOR10_D3DVECTOR10_D3DVECTORf  @ 0x003051F0
// FP register notes: the speed mapping reads LookBackDistanceScale into a named local
// and MapRangeR takes it by reference, so it stays that named local instead of becoming
// an inline parameter temporary; that colours it after the result ($f5/$f4 as retail).
// MapRange/Clamp are the shapes that matched ComputeLookPoint.


// MapRange with the output end passed by reference (the first call in ComputePosition).
static inline float MapRangeR(float x, float x0, float x1, float y0, const float &y1)
{
    float y;
    if (x1 != x0)
        y = Clamp(y0 + (x - x0) * (y1 - y0) / (x1 - x0), y0, y1);
    else
        y = 0.0f;
    return y;
}


_D3DVECTOR JetskiFollowCamera::ComputePosition(float, float, _D3DVECTOR offset, _D3DVECTOR target, _D3DVECTOR, _D3DVECTOR, _D3DVECTOR, _D3DVECTOR, float speed)
{
    _D3DVECTOR position;
    if (m_playback) {
        position = m_position;
    } else {
        float scale;
        if (!m_presets[m_curPreset].m_isLookBackCam) {
            scale = m_angleX;
            if (scale > 3.14159274f)
                scale -= 6.28318548f;
            else if (scale < -3.14159274f)
                scale += 6.28318548f;
            static float LookBackDistanceStartSpeed = 0.0f;
            static float LookBackDistanceStopSpeed = 73.333f;
            static float LookBackDistanceScale = 3.5f;
            static float LookBackDistanceStartAngle = 60.0f * 0.01745329f;
            static float LookBackDistanceStopAngle = 90.0f * 0.01745329f;
            float maxScale = LookBackDistanceScale;
            float speedScale = MapRangeR(speed, LookBackDistanceStartSpeed, LookBackDistanceStopSpeed, 1.0f, maxScale);
            scale = MapRange(Fabs(scale), LookBackDistanceStartAngle, LookBackDistanceStopAngle, speedScale, 1.0f);
        } else {
            scale = 1.0f;
        }
        position = target + offset * scale * m_distance;
        m_position = position;
    }
    return position;
}


// ---- src/JetskiFollowCamera/ComputeAngleXAndY.cpp


// Yaw wraps to (-pi, pi]; pitch is clamped to -40..85 degrees, pushing the
// excess back into the pitch offset.
void JetskiFollowCamera::ComputeAngleXAndY(float dt)
{
    m_angleX = m_presets[m_curPreset].m_angleY - m_settings[1];
    m_angleX -= m_angleXSpeed * dt;
    if (m_angleX > 3.1415927f)
        m_angleX -= 6.2831855f;
    else if (m_angleX < -3.1415927f)
        m_angleX += 6.2831855f;
    m_angleY = m_settings[2] + m_presets[m_curPreset].m_angleX;
    m_angleY -= m_reverseOffset[0];
    if (m_angleY < -0.6981318f) {
        m_angleY = -0.6981318f;
        m_settings[2] = m_reverseOffset[0] + (-0.6981318f - m_presets[m_curPreset].m_angleX);
    } else if (m_angleY > 1.4835324f) {
        m_angleY = 1.4835324f;
        m_settings[2] = (1.4835324f - m_presets[m_curPreset].m_angleX) - m_reverseOffset[0];
    }
}


// ---- src/JetskiFollowCamera/SwitchOff.cpp
CameraStateParams JetskiFollowCamera::SwitchOff()
{
    m_playback = 0;
    return SimpleFollowCamera::SwitchOff();
}


// ---- src/JetskiFollowCamera/SwitchFilterValueForPlayback.cpp



// Replays use fixed filter rates; switching back restores the saved ones.
// (Retail never clears m_playbackFilters.)
void JetskiFollowCamera::SwitchFilterValueForPlayback(int playback)
{
    if (playback) {
        m_savedLookRate.x = m_positionFilters[0]->m_rate;
        m_savedLookRate.y = m_positionFilters[1]->m_rate;
        m_savedLookRate.z = m_positionFilters[2]->m_rate;
        m_savedPositionRate.x = m_lookFilters[0]->m_rate;
        m_savedPositionRate.y = m_lookFilters[1]->m_rate;
        m_savedPositionRate.z = m_lookFilters[2]->m_rate;
        m_positionFilters[0]->m_rate = 0.27f;
        m_positionFilters[1]->m_rate = 0.27f;
        m_positionFilters[2]->m_rate = 0.27f;
        m_lookFilters[0]->m_rate = 0.35f;
        m_lookFilters[1]->m_rate = 0.35f;
        m_lookFilters[2]->m_rate = 0.35f;
        m_playbackFilters = 1;
    } else if (m_playbackFilters) {
        m_positionFilters[0]->m_rate = m_savedLookRate.x;
        m_positionFilters[1]->m_rate = m_savedLookRate.y;
        m_positionFilters[2]->m_rate = m_savedLookRate.z;
        m_lookFilters[0]->m_rate = m_savedPositionRate.x;
        m_lookFilters[1]->m_rate = m_savedPositionRate.y;
        m_lookFilters[2]->m_rate = m_savedPositionRate.z;
    }
}

