// CinematographerCam.cpp  @ 0x002F7EF0 (text 0x2F7EF0-0x2F98C0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's CinematographerCam.cpp as one translation unit: its functions in
// retail order, then its static initializer. `make link` places the object's
// data as well as its code (tools/link_c.py).



#include "d3dtypes.h"
#include "CinematographerCam/CinematographerCam.h"
#include "CameraFilter/CameraFilter.h"
#include "CameraStateParams/CameraStateParams.h"
#include "sinit/AxisVectors.h"

extern "C" void *memcpy(void *dst, const void *src, unsigned int n);
#include "CameraBehavior/CameraBehavior.h"
#include "PhysicsBody/PhysicsBody.h"
#include "SoultreeObject/SoultreeObject.h"
#include "Terrain/Terrain.h"
#include "DebugOverlay/DebugOverlay.h"
#include "JetskiGame/JetskiGame.h"

// ---- src/CinematographerCam/ctor.cpp


CinematographerCam::CinematographerCam()
{
    m_x50 = 1.5707964f;     // pi/2
    m_pitch = 0;
    m_yaw = 0;
    m_fov = 80.0f;
    m_lastPosition = _D3DVECTOR(1500.0f, 100.0f, 1500.0f);
    m_position = m_lastPosition;
    m_terrain = 0;
    m_x7C = 0;
    m_x84 = 1;
    m_mode = 0;
    m_x68 = 90.0f;
    m_x6C = 25.0f;
    m_xA4 = 1;
    m_manual = 0;
    m_filterX = 0;
    m_filterY = 0;
    m_filterZ = 0;
    m_pitchFilter = 0;
    m_yawFilter = 0;
    m_fovFilter = 0;
    m_body = 0;
    m_target = 0;
    m_x88 = 0;
    m_speed = 0;
    m_distance = 0;
    m_x64 = 0;
}


// ---- src/CinematographerCam/Initialize.cpp
// A smoothed value: moves towards its target at the given rate, clamped.


int CinematographerCam::Initialize(int mode, float targetRate, float xB0, float xB4, float x68, float x6C,
                                   float rate54, float rate10, float rate14)
{
    m_filterX = new CameraFilter(m_lastPosition.x, rate10);
    m_filterZ = new CameraFilter(m_lastPosition.z, rate10);
    m_filterY = new CameraFilter(m_lastPosition.y, rate14);
    m_pitchFilter = new CameraFilter(m_pitch, rate54);
    m_yawFilter = new CameraFilter(m_yaw, rate54);
    m_fovFilter = new CameraFilter(m_fov, targetRate);
    m_maxFov = xB4;
    m_minFov = xB0;
    m_mode = mode;
    m_x68 = x68;
    m_x6C = x6C;
    return 1;
}


// ---- src/CinematographerCam/SetPhysicsBody.cpp


void CinematographerCam::SetPhysicsBody(PhysicsBody *body, float distance, float speedMph)
{
    m_body = body;
    m_target = 0;
    m_speed = 1.46666f * speedMph;   // mph -> ft/s
    m_distance = distance;
}


// ---- src/CinematographerCam/SetInputs.cpp


void CinematographerCam::SetInputs(CameraJoystickInput input)
{
    if (input.m_button[4])        // toggle manual
        m_manual = !m_manual;
    memcpy(&m_input, &input, sizeof(input));
}


// ---- src/CinematographerCam/SwitchOn.cpp


float FastASin(float x);
float FastAtan2(float y, float x);

// Retail's reciprocal square root is an inline-asm helper like its sqrt:
// rsqrt.s into $f8, stored through a pointer to a stack temporary.
static inline float RSqrt(float x)
{
    float r;
    asm("rsqrt.s $f8, %0, %1; swc1 $f8, 0(%2)" : : "f"(1.0f), "f"(x), "r"(&r) : "$f8", "memory");
    return r;
}

// Retail copies the by-value params with lq/sq (CameraStateParams is
// 16-byte aligned) and the vectors as _D3DVECTORs.
void CinematographerCam::SwitchOn(CameraStateParams params, int)
{
    m_position = params.position;
    m_lastPosition = m_position;
    m_filterX->m_value = m_position.x;
    m_filterY->m_value = m_position.y;
    m_filterZ->m_value = m_position.z;
    _D3DVECTOR direction = params.lookPoint;
    m_pitch = FastASin(-direction.y);
    direction.y = 0;
    float len2 = direction.x * direction.x + direction.y * direction.y + direction.z * direction.z;
    if (len2 == 0.0f) {
        direction = ZeroVector;
    } else {
        float s = RSqrt(len2);
        direction.x *= s;
        direction.y *= s;
        direction.z *= s;
    }
    m_yaw = FastAtan2(direction.x, direction.z);
}


// ---- src/CinematographerCam/SwitchOff.cpp
// Camera state handed from one camera behavior to the next: SwitchOff()
// returns it, SwitchOn() takes it. The three vectors are set by their
// constructors; this camera leaves everything else unset (retail stores
// whatever happens to be in $f0/$v0).


CameraStateParams CinematographerCam::SwitchOff()
{
    CameraStateParams params;
    params.p58 = (float *)m_fovFilter;
    return params;
}


// ---- src/CinematographerCam/ComputeCameraData.cpp
// ComputeCameraData__18CinematographerCamFfRiP10_D3DVECTORP10_D3DVECTORPfP10_D3DVECTORP10_D3DVECTORPf  @ 0x002F8530
// inline_depth(7): at 6, the (move * dt) * forward argument temps sit one inline level too deep
// for CInline to hoist them into statements, so IroLinearForm gives the x product a new temp
// (colours $f10 instead of retail's $f7).
// The free-fly cinematographer camera: joystick strafe/move/raise, yaw/pitch, zoom, and
// optionally looking at the tracked body. Fmin/RSqrt are asm helpers, so the optimizer is off.
#pragma inline_depth(7)


extern JetskiGame *LpGame;
extern int g_DebugOverlayID;

int RotateAroundAxis(float angle, _D3DVECTOR axis, _D3DVECTOR v, _D3DVECTOR *out);
float FastAtan2(float y, float x);

static inline float Fmin(float a, float b)
{
    float r;
    asm("min.s $f8, %1, %2; swc1 $f8, 0(%0)" : : "r"(&r), "f"(a), "f"(b) : "$f8", "memory");
    return r;
}

static inline void Vu0Normalize(_D3DVECTOR *out, _D3DVECTOR *in)
{
    asm("lqc2 vf4, 0x0(%1); vmul.xyz vf5, vf4, vf4; vaddy.x vf5, vf5, vf5y; vaddz.x vf5, vf5, vf5z; vrsqrt Q, vf0w, vf5x; vsub.xyzw vf6, vf0, vf0; vaddw.xyzw vf6, vf6, vf4w; vwaitq; vmulq.xyz vf6, vf4, Q; sqc2 vf6, 0x0(%0)"
        : : "r"(out), "r"(in) : "memory");
}

// libvu0 sceVu0OuterProduct: out = a x b.
static inline void CrossProduct(_D3DVECTOR *out, const _D3DVECTOR *a, const _D3DVECTOR *b)
{
    asm("lqc2 vf4, 0x0(%1); lqc2 vf5, 0x0(%2); vopmula.xyz ACC, vf4, vf5; vopmsub.xyz vf6, vf5, vf4; vsub.w vf6, vf6, vf6; sqc2 vf6, 0x0(%0)"
        : : "r"(out), "r"(a), "r"(b) : "memory");
}

// Normalizes v in place (the zero vector stays zero).
static inline void Normalize(_D3DVECTOR &v)
{
    float len2 = v.x * v.x + v.y * v.y + v.z * v.z;
    if (len2 == 0.0f) {
        v = ZeroVector;
    } else {
        float r = RSqrt(len2);
        v.x *= r;
        v.y *= r;
        v.z *= r;
    }
}

// Moves the filter towards target; returns the new value.
static inline float UpdateFilter(CameraFilter *filter, float dt, float target)
{
    filter->m_scale = Fmin(dt, filter->m_rate) / filter->m_rate;
    filter->m_value = filter->m_value + filter->m_scale * (target - filter->m_value);
    return filter->m_value;
}

void CinematographerCam::ComputeCameraData(float dt, int &state, _D3DVECTOR *position, _D3DVECTOR *look,
                                           float *fov, _D3DVECTOR *a, _D3DVECTOR *b, float *c)
{
    if (m_mode && m_xA4) {
        float move = -m_input.m_axis[1];
        float strafe = m_input.m_axis[0];
        _D3DVECTOR right;
        _D3DVECTOR up;
        _D3DVECTOR forward;
        CrossProduct(&right, b, a);
        if (!m_manual)
            right.y = 0.0f;
        Vu0Normalize(&right, &right);
        up = _D3DVECTOR(0.0f, 1.0f, 0.0f);
        if (a->x == 0.0f && a->z == 0.0f)
            forward = *b;
        else
            forward = *a;
        if (!m_manual)
            forward.y = 0.0f;
        Vu0Normalize(&forward, &forward);
        m_position = m_position + (move * dt) * forward * m_x68 + (strafe * dt) * right * m_x68;
    }

    float yawInput = m_input.m_axis[2];
    float pitchInput = m_input.m_axis[3];
    if (m_xA4) {
        if (!m_x88) {
            static float fudgeFactor = 20.0f;
            float scale = (*fov + fudgeFactor) / (m_maxFov + fudgeFactor);
            m_yaw = m_yaw - scale * (dt * (yawInput * m_x50));
            m_pitch = m_pitch + scale * (dt * (pitchInput * Fmin(m_x50, 1.5707964f)));
        }
        if (m_input.m_button[0])                // up
            m_position.y += dt * m_x6C;
        else if (m_input.m_button[1])           // down
            m_position.y -= dt * m_x6C;
    }

    float yaw = UpdateFilter(m_yawFilter, dt, m_yaw);
    while (yaw < 0.0f)
        yaw += 6.2831855f;
    while (yaw > 6.2831855f)
        yaw -= 6.2831855f;
    float pitch = UpdateFilter(m_pitchFilter, dt, m_pitch);
    if (pitch <= -1.499f) {
        m_pitch = -1.499f;
        pitch = -1.499f;
    }
    if (pitch >= 1.5676548f) {
        m_pitch = 1.5676548f;
        pitch = 1.5676548f;
    }

    _D3DVECTOR lookDir;
    _D3DVECTOR upDir;
    _D3DVECTOR pitched;
    _D3DVECTOR rotated;
    RotateAroundAxis(pitch, XVector, ZVector, &pitched);
    RotateAroundAxis(yaw, YVector, pitched, &rotated);
    _D3DVECTOR side;
    lookDir = rotated;
    CrossProduct(&side, &YVector, &lookDir);
    Vu0Normalize(&side, &side);
    Vu0Normalize(&lookDir, &lookDir);
    CrossProduct(&upDir, &lookDir, &side);

    m_lastPosition.x = UpdateFilter(m_filterX, dt, m_position.x);
    m_lastPosition.y = UpdateFilter(m_filterY, dt, m_position.y);
    m_lastPosition.z = UpdateFilter(m_filterZ, dt, m_position.z);

    if (m_x84) {
        if (m_terrain) {
            _D3DVECTOR ground = m_lastPosition;
            m_terrain->GetGridPointIntersection(&ground, 0, 0, 0);
            float height = 1.0f + ground.y;
            if (m_lastPosition.y < height)
                m_filterY->m_value = m_position.y = m_lastPosition.y = height;
        }
        float height = 1.0f + m_x7C;
        if (m_lastPosition.y < height)
            m_filterY->m_value = m_position.y = m_lastPosition.y = height;
    }

    {
        _D3DVECTOR side2;
        CrossProduct(&side2, &upDir, &lookDir);
        Vu0Normalize(&side2, &side2);
        CrossProduct(&upDir, &lookDir, &side2);
    }

    if (position)
        *position = m_lastPosition;
    if (a)
        *a = lookDir;
    if (b)
        *b = upDir;

    if (m_body || m_target) {
        if (m_x88) {
            _D3DVECTOR target;
            if (m_body) {
                _D3DVECTOR velocity;
                m_body->m_object->GetPosition(0, &target);
                velocity = m_body->m_velocity;
                velocity.y = 0.0f;
                Normalize(velocity);
                target += 3.0f * velocity;
            } else {
                m_target->GetPosition(0, &target);
            }
            lookDir = target - m_lastPosition;
            Normalize(lookDir);
            CrossProduct(&side, &YVector, &lookDir);
            Normalize(side);
            Normalize(lookDir);
            CrossProduct(&upDir, &lookDir, &side);
            if (a)
                *a = lookDir;
            if (b)
                *b = upDir;
        } else if (m_input.m_button[5]) {       // look at target
            _D3DVECTOR target;
            if (m_body)
                m_body->m_object->GetPosition(0, &target);
            else
                m_target->GetPosition(0, &target);
            lookDir = target - m_lastPosition;
            Normalize(lookDir);
            CrossProduct(&side, &YVector, &lookDir);
            Normalize(side);
            Normalize(lookDir);
            CrossProduct(&upDir, &lookDir, &side);
            m_yaw = FastAtan2(lookDir.x, lookDir.z);
        }
    }

    static float fovValue = 23.0f;
    if (m_input.m_button[2]) {                  // zoom in
        m_fov = m_fov - fovValue * dt;
        if (m_fov < m_minFov)
            m_fov = m_minFov;
    }
    if (m_input.m_button[3]) {                  // zoom out
        m_fov = m_fov + fovValue * dt;
        if (m_fov > m_maxFov)
            m_fov = m_maxFov;
    }
    if (fov)
        *fov = UpdateFilter(m_fovFilter, dt, m_fov);

    state = 0;
    DebugOverlay *overlay = LpGame->m_debugOverlay;
    if (overlay) {
        if (g_DebugOverlayID < 0) {
            int page = overlay->m_numPages;
            overlay->m_numPages = page + 1;
            g_DebugOverlayID = page;
        }
        LpGame->m_debugOverlay->PrintfHeading(g_DebugOverlayID, "Cinema Camera Info");
        LpGame->m_debugOverlay->PrintfDetail(g_DebugOverlayID, "POS: %4.2f,%4.2f,%4.2f", m_lastPosition.x, m_lastPosition.y, m_lastPosition.z);
        LpGame->m_debugOverlay->PrintfDetail(g_DebugOverlayID, "FOV: %f", m_fov);
    }
}


// ---- src/CinematographerCam/SetTerrain.cpp


void CinematographerCam::SetTerrain(Terrain *terrain)
{
    m_terrain = terrain;
    _D3DVECTOR normal;
    terrain->GetGridPointIntersection(&m_lastPosition, &normal, 0, 0);
    m_position = m_lastPosition;
}


// ---- src/CinematographerCam/dtor.cpp


CinematographerCam::~CinematographerCam()
{
    if (m_filterX)
        delete m_filterX;
    if (m_filterY)
        delete m_filterY;
    if (m_filterZ)
        delete m_filterZ;
    if (m_pitchFilter)
        delete m_pitchFilter;
    if (m_yawFilter)
        delete m_yawFilter;
    if (m_fovFilter)
        delete m_fovFilter;
}


// ---- src/sinit/CinematographerCam.cpp
// Static initializer of CinematographerCam.cpp: only the axis vectors from the shared header.

