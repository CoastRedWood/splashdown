// FirstPersonCam.cpp  @ 0x0033BBE0 (text 0x33BBE0-0x33C0F0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's FirstPersonCam.cpp as one translation unit: FirstPersonCamera, then
// the static initializer (only the axis vectors). Its RTTI (FirstPersonCamera,
// CameraBehavior) follows JetskiWreckCam.cpp's in .rodata.



#include "CameraBehavior/CameraBehavior.h"
#include "FirstPersonCamera/FirstPersonCamera.h"
#include "d3dtypes.h"
#include "sinit/AxisVectors.h"
#include "PhysicsBody/PhysicsBody.h"
#include "SoultreeObject/SoultreeObject.h"
#include "CameraVector/CameraVector.h"
#include "CameraStateParams/CameraStateParams.h"

// ---- src/global/ctor_0033BBE0.cpp

FirstPersonCamera::FirstPersonCamera()
{
    m_fov = 80.0f;
    m_body = 0;
    m_object = 0;
    m_noiseScale = 0;
    m_noiseAmount = 0;
}


// ---- src/global/dtor_0033BC40.cpp
struct CameraJoystickInput;

// Retail vtable: slots 0, 1 and 3 are pure virtual (names unknown).


FirstPersonCamera::~FirstPersonCamera()
{
}


// ---- src/global/Initialize.cpp


int FirstPersonCamera::Initialize(_D3DVECTOR position, float fov)
{
    m_fov = fov;
    m_offset = position;
    return 1;
}

// (This and SwitchOn were in BuoyancyModel/Create.cpp, a cross-TU stub.)
void FirstPersonCamera::SetPhysicsBody(PhysicsBody *body)
{
    m_body = body;
    m_object = 0;
}



// ---- src/FirstPersonCamera/ComputeCameraData.cpp




void FirstPersonCamera::ComputeCameraData(float dt, int &cut, _D3DVECTOR *position, _D3DVECTOR *target,
                                          float *fov, _D3DVECTOR *forward, _D3DVECTOR *up, float *roll)
{
    _D3DVECTOR objectPosition;
    _D3DVECTOR objectForward;
    _D3DVECTOR objectUp;
    SoultreeObject *object;
    if (m_body)
        object = m_body->m_object;
    else
        object = m_object;
    object->GetPosition(0, &objectPosition);
    object->GetOrientation(0, &objectForward, &objectUp);
    if (position) {
        *position = objectPosition + object->RotateVectorLocalToWorld(&m_offset);
    }
    AddNoise(position);
    if (forward)
        *forward = objectForward;
    if (up)
        *up = objectUp;
    if (fov)
        *fov = m_fov;
    cut = 0;
}

void FirstPersonCamera::SwitchOn(CameraStateParams params, int copyFilter)
{
}



// ---- src/FirstPersonCamera/SwitchOff.cpp
// Camera state handed from one camera behavior to the next: SwitchOff()
// returns it, SwitchOn() takes it. The three vectors are set by their
// constructors; this camera leaves everything else unset (retail stores
// whatever happens to be in $f0/$v0).


struct CameraJoystickInput;
struct _D3DVECTOR;


CameraStateParams FirstPersonCamera::SwitchOff()
{
    CameraStateParams params;
    return params;
}


// ---- src/FirstPersonCamera/AddNoise.cpp
// mwcc's default inline depth leaves the _D3DVECTOR ctor out of line in the
// chained `noise * a * b`; retail inlined it all, so raise the depth.
#pragma inline_depth(4)
extern "C" int rand(void);


// libvu0 sceVu0Normalize, as retail's inline asm.
static inline void Vu0Normalize(_D3DVECTOR *out, _D3DVECTOR *in)
{
    asm("lqc2 vf4, 0x0(%1); vmul.xyz vf5, vf4, vf4; vaddy.x vf5, vf5, vf5y; vaddz.x vf5, vf5, vf5z; vrsqrt Q, vf0w, vf5x; vsub.xyzw vf6, vf0, vf0; vaddw.xyzw vf6, vf6, vf4w; vwaitq; vmulq.xyz vf6, vf4, Q; sqc2 vf6, 0x0(%0)"
        : : "r"(out), "r"(in) : "memory");
}


// Jitter v by a random unit vector scaled by m_noiseAmount * m_noiseScale.
void FirstPersonCamera::AddNoise(_D3DVECTOR *v)
{
    // rand() / 2^31 in [0, 1), remapped to [-1, 1) as x - (1 - x).
    float rx = rand() * (1.0f / 2147483648.0f);
    float ry = rand() * (1.0f / 2147483648.0f);
    float rz = rand() * (1.0f / 2147483648.0f);
    _D3DVECTOR noise(rx - (1.0f - rx), ry - (1.0f - ry), rz - (1.0f - rz));
    Vu0Normalize(&noise, &noise);
    *v += noise * m_noiseAmount * m_noiseScale;
}

