// SimpleTVCamera.cpp  @ 0x003131D0 (text 0x3131D0-0x313B00)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's SimpleTVCamera.cpp as one translation unit: its functions in
// retail order, then its static initializer. `make link` places the
// object's data as well as its code (tools/link_c.py).



#include "d3dtypes.h"
#include "CameraBehavior/CameraBehavior.h"
#include "SimpleTVCamera/SimpleTVCamera.h"
#include "CameraVector/CameraVector.h"
#include "CameraStateParams/CameraStateParams.h"
#include "SoultreeObject/SoultreeObject.h"
#include "PhysicsBody/PhysicsBody.h"
#include "sinit/AxisVectors.h"

extern "C" void free(void *p);
void PSXprintf(const char *fmt, ...);

// ---- src/SimpleTVCamera/ctor.cpp


SimpleTVCamera::SimpleTVCamera()
{
    m_fov = 60.0f;
    m_body = 0;
    m_object = 0;
    m_lastTarget = ZeroVector;
    m_currentCrew = -1;
    m_nextCrew = 0;
    m_previousCrew = 0;
    m_crews = 0;
}


// ---- src/SimpleTVCamera/dtor.cpp


SimpleTVCamera::~SimpleTVCamera()
{
    if (m_crews)
        free(m_crews);
}


// ---- src/SimpleTVCamera/SetPhysicsBody.cpp


void SimpleTVCamera::SetPhysicsBody(PhysicsBody *body, float a, float b)
{
    m_body = body;
    m_object = 0;
    m_scaledB = 1.46666002f * b;
    m_lookHeight = a;
}


// ---- src/SimpleTVCamera/SwitchOn.cpp (JetskiTVCamera::ResetTrace, also
// in that file, is in JetskiTVCamera.cpp)

void SimpleTVCamera::SwitchOn(CameraStateParams, int)
{
}


// ---- src/SimpleTVCamera/SwitchOff.cpp
// Camera state handed from one camera behavior to the next: SwitchOff()
// returns it, SwitchOn() takes it. The three vectors are set by their
// constructors; this camera leaves everything else unset (retail stores
// whatever happens to be in $f0/$v0).


CameraStateParams SimpleTVCamera::SwitchOff()
{
    CameraStateParams params;
    return params;
}


// ---- src/SimpleTVCamera/ComputeCameraData.cpp


void SimpleTVCamera::ComputeCameraData(float dt, int &state, _D3DVECTOR *position, _D3DVECTOR *look, float *fov,
                                       _D3DVECTOR *a, _D3DVECTOR *b, float *c)
{
    _D3DVECTOR lookPoint;
    _D3DVECTOR target;
    _D3DVECTOR velocity;

    if (m_body) {
        m_body->m_object->GetPosition(0, &target);
        velocity = m_body->m_velocity;
    } else {
        m_object->GetPosition(0, &target);
        if (m_lastTarget == ZeroVector) {
            velocity.x = 0;
            velocity.y = 0;
            velocity.z = 0;
            velocity.w = 1.0f;
        } else {
            float invDt = 1.0f / dt;
            float dx = target.x - m_lastTarget.x;
            float dy = target.y - m_lastTarget.y;
            float dz = target.z - m_lastTarget.z;
            float vx = dx * invDt;
            float vz = dz * invDt;
            float vy = dy * invDt;
            velocity.x = vx;
            velocity.y = vy;
            velocity.z = vz;
            velocity.w = 1.0f;
        }
        m_lastTarget = target;
    }
    lookPoint = ComputeLookPoint(target);
    m_position = ComputePosition(dt, target, velocity);
    m_fov = ComputeFOV(lookPoint);
    if (position)
        *position = m_position;
    if (look)
        *look = lookPoint;
    if (fov)
        *fov = m_fov;
}


// ---- src/SimpleTVCamera/ComputeLookPoint.cpp


_D3DVECTOR SimpleTVCamera::ComputeLookPoint(_D3DVECTOR target)
{
    _D3DVECTOR look = target;
    look.y += m_lookHeight;
    return look;
}


// ---- src/SimpleTVCamera/ComputePosition.cpp


_D3DVECTOR SimpleTVCamera::ComputePosition(float time, _D3DVECTOR target, _D3DVECTOR velocity)
{
    _D3DVECTOR position;
    GetTheBestCamera(time, &target, &position);
    return position;
}


// ---- src/SimpleTVCamera/ComputeFOV.cpp

static inline float Sqrt(float x)
{
    float r;
    asm("sqrt.s $f8, %1; swc1 $f8, 0(%0)" : : "r"(&r), "f"(x) : "$f8", "memory");
    return r;
}

// Widens the field of view as the target comes closer (horizontal distance).
float SimpleTVCamera::ComputeFOV(_D3DVECTOR target)
{
    if (m_currentCrew < 0)
        return 70.0f;
    float minFov = m_crews[m_currentCrew].m_minFOV;
    float maxFov = m_crews[m_currentCrew].m_maxFOV;
    float farDistance = m_crews[m_currentCrew].m_farDistance;
    float nearDistance = m_crews[m_currentCrew].m_nearDistance;
    if (minFov == maxFov)
        return minFov;
    float dx = m_position.x - target.x;
    float dz = m_position.z - target.z;
    float distance = Sqrt(dx * dx + dz * dz);
    float span = farDistance - nearDistance;
    float range = maxFov - minFov;
    float fov = minFov + range * ((farDistance - distance) / span);
    if (fov < minFov)
        fov = minFov;
    if (fov > maxFov)
        fov = maxFov;
    return fov;
}


// ---- src/SimpleTVCamera/GetTheBestCamera.cpp

static inline float DistanceSquared(_D3DVECTOR *a, _D3DVECTOR *b)
{
    float dx = a->x - b->x;
    float dy = a->y - b->y;
    float dz = a->z - b->z;
    return dx * dx + dy * dy + dz * dz;
}

// Pick the camera crew nearest the target; returns 1 if that changed.
int SimpleTVCamera::GetTheBestCamera(float time, _D3DVECTOR *target, _D3DVECTOR *position)
{
    int i;
    int best;   // uninitialized in retail when there are no crews
    float bestDist = 3.40282347e+38f;
    for (i = 0; i < m_numCrews; i++) {
        float dist = DistanceSquared(target, &m_crews[i].m_position);
        if (bestDist > dist) {
            bestDist = dist;
            best = i;
        }
    }
    m_currentCrew = best;
    *position = m_crews[best].m_position;
    if (m_currentCrew != m_previousCrew) {
        m_previousCrew = m_currentCrew;
        return 1;
    }
    return 0;
}


// ---- src/SimpleTVCamera/AddCameraCrew.cpp


int SimpleTVCamera::AddCameraCrew(_D3DVECTOR *position, float maxFOV, float minFOV, float farDistance,
                                  float nearDistance, int arg)
{
    if (m_nextCrew >= m_numCrews) {
        PSXprintf("Tried to add more camera than allowed. (TVCameraPositons)\n");
        return 0;
    }
    m_crews[m_nextCrew].m_position = *position;
    m_crews[m_nextCrew].m_maxFOV = maxFOV;
    m_crews[m_nextCrew].m_minFOV = minFOV;
    m_crews[m_nextCrew].m_farDistance = farDistance;
    m_crews[m_nextCrew].m_nearDistance = nearDistance;
    m_crews[m_nextCrew++].m_x20 = arg;
    return 1;
}


// ---- src/sinit/SimpleTVCamera.cpp
// Static initializer of SimpleTVCamera.cpp: only the axis vectors from the shared header.

