// SimpleThrillCam.cpp  @ 0x0030B7F0 (text 0x30B7F0-0x30C1A0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's SimpleThrillCam.cpp as one translation unit: SimpleThrillCamera,
// then the static initializer (only the axis vectors).




#include "d3dtypes.h"
#include "SimpleThrillCamera/SimpleThrillCamera.h"
#include "CameraBehavior/CameraBehavior.h"
#include "SoultreeObject/SoultreeObject.h"
#include "PhysicsBody/PhysicsBody.h"
#include "CameraVector/CameraVector.h"
#include "CameraStateParams/CameraStateParams.h"
#include "sinit/AxisVectors.h"

// inline_depth is TU-wide (ComputePosition's).
#pragma inline_depth(6)

// ---- src/SimpleThrillCamera/ctor.cpp






SimpleThrillCamera::SimpleThrillCamera()
{
    m_fov = 80.0f;
    m_body = 0;
    m_object = 0;
    m_lastTarget = ZeroVector;
    m_scaledB = 0;
    m_a = 3.0f;
    m_positioned = 0;
}


// ---- src/SimpleThrillCamera/dtor.cpp


SimpleThrillCamera::~SimpleThrillCamera()
{
}


// ---- src/SimpleThrillCamera/SetPhysicsBody.cpp
class PhysicsBody;
class SoultreeObject;


void SimpleThrillCamera::SetPhysicsBody(PhysicsBody *target, float a, float b)
{
    m_body = target;
    m_object = 0;
    m_scaledB = 1.46666002f * b;
    m_a = a;
}


// ---- src/SimpleThrillCamera/ComputeCameraData.cpp




struct CameraStateParams;


void SimpleThrillCamera::ComputeCameraData(float dt, int &state, _D3DVECTOR *position, _D3DVECTOR *lookPoint,
                                           float *fov, _D3DVECTOR *a, _D3DVECTOR *b, float *c)
{
    _D3DVECTOR target;
    _D3DVECTOR velocity;
    if (m_body) {
        m_body->m_object->GetPosition(0, &target);
        velocity = m_body->m_velocity;
    } else {
        m_object->GetPosition(0, &target);
        if (m_lastTarget == ZeroVector) {
            velocity.x = 0.0f;
            velocity.y = 0.0f;
            velocity.z = 0.0f;
            velocity.w = 1.0f;
        } else {
            velocity = (target - m_lastTarget) / dt;
        }
        m_lastTarget = target;
    }
    _D3DVECTOR look;
    look = ComputeLookPoint(target);
    if (!m_positioned) {
        m_lookPoint = ComputePosition(target, velocity);
        m_positioned = 1;
    }
    float f = ComputeFOV(target);
    if (position)
        *position = m_lookPoint;
    if (lookPoint)
        *lookPoint = look;
    if (fov)
        *fov = f;
}


// ---- src/SimpleThrillCamera/SwitchOn.cpp



void SimpleThrillCamera::SwitchOn(CameraStateParams, int)
{
    m_positioned = 0;
}


// ---- src/SimpleThrillCamera/SwitchOff.cpp
// Camera state handed from one camera behavior to the next: SwitchOff()
// returns it, SwitchOn() takes it. The three vectors are set by their
// constructors; this camera leaves everything else unset (retail stores
// whatever happens to be in $f0/$v0).


struct CameraJoystickInput;
struct _D3DVECTOR;


CameraStateParams SimpleThrillCamera::SwitchOff()
{
    CameraStateParams params;
    return params;
}


// ---- src/SimpleThrillCamera/ConstrainCamera.cpp
struct _D3DVECTOR;

int SimpleThrillCamera::ConstrainCamera(_D3DVECTOR *, int)
{
    return 1;
}


// ---- src/SimpleThrillCamera/ComputeFOV.cpp

static inline float Sqrt(float x)
{
    float r;
    asm("sqrt.s $f8, %1; swc1 $f8, 0(%0)" : : "r"(&r), "f"(x) : "$f8", "memory");
    return r;
}


// Widens the field of view as the target comes closer (horizontal distance).
float SimpleThrillCamera::ComputeFOV(_D3DVECTOR target)
{
    float dx;
    float dz;
    float distance;
    float fov;
    target.y += 3.0f;  // the look point (see ComputeLookPoint)
    dx = m_lookPoint.x - target.x;
    dz = m_lookPoint.z - target.z;
    distance = Sqrt(dx * dx + dz * dz);
    float span = m_farDistance - m_nearDistance;
    float range = m_maxFov - m_minFov;
    fov = m_minFov + range * ((m_farDistance - distance) / span);
    if (fov < m_minFov)
        fov = m_minFov;
    if (fov > m_maxFov)
        fov = m_maxFov;
    return fov;
}


// ---- src/SimpleThrillCamera/ComputeLookPoint.cpp


// Look 3 units above the target.
_D3DVECTOR SimpleThrillCamera::ComputeLookPoint(_D3DVECTOR target)
{
    _D3DVECTOR look = target;
    look.y += 3.0f;
    return look;
}


// ---- src/SimpleThrillCamera/ComputePosition.cpp






// Retail's fabs: abs.s into $f8, stored through a stack temporary.
static inline float Fabs(float x)
{
    float r;
    asm("abs.s $f8, %1; swc1 $f8, 0(%0)" : : "r"(&r), "f"(x) : "$f8", "memory");
    return r;
}

struct CameraStateParams;


// Places the camera ahead of the target along its ballistic path: t is the
// time to the top of the jump (g = 32.2).
_D3DVECTOR SimpleThrillCamera::ComputePosition(_D3DVECTOR target, _D3DVECTOR velocity)
{
    _D3DVECTOR apex;
    _D3DVECTOR horizontal;
    _D3DVECTOR position;
    float height;
    float lead;
    float s;
    float t;
    float factor;
    factor = 1.5f;
    t = 1.0f / 32.2f;
    t *= velocity.y;
    s = 2.0f;
    horizontal = velocity;
    horizontal.y = 0;
    s *= t;
    apex = target + s * horizontal;
    height = apex.y;
    lead = factor * t;
    ConstrainCamera(&apex, 1);
    float drop = height - apex.y;
    if (drop > 1.0f)
        lead += 0.5f * (drop / Fabs(velocity.y));
    position = target + lead * velocity + -0.2f * velocity + 0.45f * _D3DVECTOR(velocity.z, 0, -velocity.x);
    ConstrainCamera(&position, 1);
    position.y = target.y - 0.5f * (target.y - position.y);
    return position;
}

