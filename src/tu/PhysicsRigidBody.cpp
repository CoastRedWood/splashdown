// PhysicsRigidBody.cpp  @ 0x00265AF0 (text 0x265AF0-0x2668D0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's PhysicsRigidBody.cpp as one translation unit: PhysicsRigidBody's
// methods, then the static initializer (only the axis vectors: the ctor's and
// Reset's ZeroVector is this TU's 0x473ED0). Its .rodata is the
// PhysicsRigidBody RTTI (@1089-@1095, its own
// BaseObject/GameObject/PhysicsBody copies). PixTrans.cpp follows at 0x2668D0.


#include "GameObject/GameObject.h"
#include "PhysicsBody/PhysicsBody.h"
#include "PhysicsRigidBody/PhysicsRigidBody.h"
#include "d3dtypes.h"
#include "Quaternion/Quaternion.h"
#include "sinit/AxisVectors.h"
#include "SoultreeObject/SoultreeObject.h"

// ---- src/PhysicsRigidBody/ctor.cpp


// Per-file static, as in retail (ZeroVector_00473ED0).



PhysicsRigidBody::PhysicsRigidBody(int enabled) : PhysicsBody(enabled)
{
    m_x200 = 0;
    m_translationDOF[0] = 1;
    m_translationDOF[1] = 1;
    m_translationDOF[2] = 1;
    m_orientationDOF[0] = 1;
    m_orientationDOF[1] = 1;
    m_orientationDOF[2] = 1;
    m_force = ZeroVector;
    m_torque = ZeroVector;
    Reset();
    m_x320 = 0;
}



// ---- src/PhysicsRigidBody/dtor.cpp


PhysicsRigidBody::~PhysicsRigidBody()
{
}


// ---- src/PhysicsRigidBody/DegreesOfFreedom.cpp

void PhysicsRigidBody::SetTranslationDegreesOfFreedom(int x, int y, int z)
{
    m_translationDOF[0] = x;
    m_translationDOF[1] = y;
    m_translationDOF[2] = z;
}

void PhysicsRigidBody::SetOrientationDegreesOfFreedom(int x, int y, int z)
{
    m_orientationDOF[0] = x;
    m_orientationDOF[1] = y;
    m_orientationDOF[2] = z;
}


// ---- src/PhysicsRigidBody/Reset.cpp



// PhysicsBody's new virtuals in retail slot order (retail vtable).




void PhysicsRigidBody::Reset()
{
    m_force = ZeroVector;
    m_torque = ZeroVector;
    m_xC0 = ZeroVector;
    m_xD0 = ZeroVector;
    m_velocity = ZeroVector;
    m_lastVelocity = ZeroVector;
    m_x90 = ZeroVector;
    m_xB0 = ZeroVector;
    m_speed = 0;
    ResetDerivatives();
}


// ---- src/PhysicsRigidBody/ResetDerivatives.cpp
extern "C" void *memset(void *dst, int c, unsigned int n);


extern Quaternion ZeroQuaternion;



void PhysicsRigidBody::ResetDerivatives()
{
    for (int i = 0; i < 2; i++) {
        memset(&m_2B8[i], 0, sizeof(Vector3));
        m_2E8[i] = ZeroQuaternion;
        memset(&m_2D0[i], 0, sizeof(Vector3));
        memset(&m_308[i], 0, sizeof(Vector3));
    }
}


// ---- src/PhysicsRigidBody/Tick.cpp



_D3DVECTOR ComputeEulerAngles(Quaternion &q);

static inline float RSqrt(float x)
{
    float r;
    asm("rsqrt.s $f8, %0, %1; swc1 $f8, 0(%2)" : : "f"(1.0f), "f"(x), "r"(&r) : "$f8", "memory");
    return r;
}




int PhysicsRigidBody::Tick(float dt)
{
    if (!m_x320)
        SimulationTick(dt);
    m_x100 = ComputeEulerAngles(m_orientation);
    float len2 = m_velocity.x * m_velocity.x + m_velocity.y * m_velocity.y + m_velocity.z * m_velocity.z;
    float speed;
    if (len2 == 0.0f)
        speed = 0.0f;
    else
        speed = 1.0f / RSqrt(len2);
    m_speed = speed;
    return GameObject::Tick(dt);
}


// ---- src/PhysicsRigidBody/SimulationTick.cpp


Quaternion ComputeQuaternion(_D3DMATRIX &m);
Quaternion ComputeQuaternionRate(Quaternion &q, _D3DVECTOR &omega);
Quaternion Abs(Quaternion &q);
_D3DMATRIX ComputeMatrix(Quaternion &q);

static inline float Sqrt(float x)
{
    float r;
    asm("sqrt.s $f8, %1; swc1 $f8, 0(%0)" : : "r"(&r), "f"(x) : "$f8", "memory");
    return r;
}

// VU0 squared length, returned in a GPR.
static inline float LengthSquared(_D3DVECTOR *a)
{
    float r;
    asm("lqc2 vf4, 0x0(%1); vmove.xyzw vf5, vf4; vmul.xyz vf5, vf4, vf5; vaddy.x vf5, vf5, vf5y; vaddz.x vf5, vf5, vf5z; qmfc2 %0, vf5"
        : "=r"(r) : "r"(a));
    return r;
}

// out = m's 3x3 part * v (w ends up 1), v passed by value.
static inline void ApplyMatrixVector(_D3DVECTOR *out, _D3DMATRIX *m, _D3DVECTOR v)
{
    asm("lqc2 vf4, 0x0(%1); lqc2 vf5, 0x10(%1); lqc2 vf6, 0x20(%1); lqc2 vf8, 0x0(%2); vmulax.xyz ACC, vf4, vf8x; vmadday.xyz ACC, vf5, vf8y; vmaddz.xyz vf9, vf6, vf8z; vadd.xyzw vf9, vf9, vf0; sqc2 vf9, 0x0(%0)"
        : : "r"(out), "r"(m), "r"(&v) : "memory");
}

// Scales v down (limit by reference: retail reads it after the VU0 part) to at most sqrt(limit) long.
static inline void ClampLength(_D3DVECTOR &v, float &limit)
{
    float len2 = LengthSquared(&v);
    if (len2 > limit)
        v *= Sqrt(limit / len2);
}

// a != b as an int: retail's andi-masked || chain (the header's bool
// operator!= gives none).
static inline int Differs(const _D3DVECTOR &a, const _D3DVECTOR &b)
{
    return a.x != b.x || a.y != b.y || a.z != b.z;
}

// One integration step: applies the pending rotation/translation, then
// integrates velocity, position, angular rate and orientation (each clamped
// and masked by the degrees of freedom) and clears the accumulators.
int PhysicsRigidBody::SimulationTick(float dt)
{
    _D3DVECTOR axis;
    GameObject::Tick(dt);
    if (m_object && m_enabled) {
        m_x90 = m_velocity;
        m_x60 = m_object->TransformVectorLocalToWorld(&m_centerOfMass);
        if (Differs(m_xF0, ZeroVector)) {
            axis = m_object->TransformVectorLocalToWorld(&m_xF0);
            float len2 = axis.x * axis.x + axis.y * axis.y + axis.z * axis.z;
            float angle;
            if (len2 == 0.0f)
                angle = 0.0f;
            else
                angle = 1.0f / RSqrt(len2);
            if (angle != 0.0f)
                axis *= 1.0f / angle;
            m_object->AddRotation(axis, angle);
            m_object->Translate(m_xE0);
        }
        m_orientation = ComputeQuaternion(m_object->m_worldMatrix);
        if (m_mass > 0.0f) {
            m_xC0 = m_inverseMass * m_force;
            if (!m_translationDOF[0]) {
                m_velocity.x = 0.0f;
                m_xC0.x = 0.0f;
                m_2D0[1].x = 0.0f;
            }
            if (!m_translationDOF[1]) {
                m_velocity.y = 0.0f;
                m_xC0.y = 0.0f;
                m_2D0[1].y = 0.0f;
            }
            if (!m_translationDOF[2]) {
                m_velocity.z = 0.0f;
                m_xC0.z = 0.0f;
                m_2D0[1].z = 0.0f;
            }
            ClampLength(m_xC0, m_x274);
            m_2D0[0].x = m_xC0.x;
            m_2D0[0].y = m_xC0.y;
            m_2D0[0].z = m_xC0.z;
        } else {
            m_xC0 = ZeroVector;
            m_2D0[0].x = 0.0f;
            m_2D0[0].y = 0.0f;
            m_2D0[0].z = 0.0f;
        }
        m_integrator(dt, 3, (float *)&m_velocity, (float **)m_2D0);
        ClampLength(m_velocity, m_x27C);
        m_2B8[0].x = m_velocity.x;
        m_2B8[0].y = m_velocity.y;
        m_2B8[0].z = m_velocity.z;
        m_integrator(dt, 3, (float *)&m_x60, (float **)m_2B8);
        ApplyMatrixVector(&m_xD0, &m_inverseInertia, m_torque);
        if (!m_orientationDOF[0]) {
            m_xB0.x = 0.0f;
            m_xD0.x = 0.0f;
            m_308[1].x = 0.0f;
        }
        if (!m_orientationDOF[1]) {
            m_xB0.y = 0.0f;
            m_xD0.y = 0.0f;
            m_308[1].y = 0.0f;
        }
        if (!m_orientationDOF[2]) {
            m_xB0.z = 0.0f;
            m_xD0.z = 0.0f;
            m_308[1].z = 0.0f;
        }
        ClampLength(m_xD0, m_x278);
        m_308[0].x = m_xD0.x;
        m_308[0].y = m_xD0.y;
        m_308[0].z = m_xD0.z;
        m_integrator(dt, 3, (float *)&m_xB0, (float **)m_308);
        ClampLength(m_xB0, m_x280);
        m_2E8[0] = ComputeQuaternionRate(m_orientation, m_xB0);
        m_integrator(dt, 4, (float *)&m_orientation, (float **)m_2E8);
        m_orientation = Abs(m_orientation);
        _D3DMATRIX matrix = ComputeMatrix(m_orientation);
        m_object->SetMatrix(0, &matrix);
        _D3DVECTOR center = m_object->TransformVectorLocalToWorld(&m_centerOfMass);
        m_x60.x -= center.x;
        m_x60.y -= center.y;
        m_x60.z -= center.z;
        m_object->Translate(0, &m_x60);
        m_object->UpdateSearchPlacement();
        m_lastVelocity = m_object->RotateVectorWorldToLocal(&m_velocity);
        m_xA0 = m_object->RotateVectorLocalToWorld(&m_xB0);
    }
    m_force = ZeroVector;
    m_torque = ZeroVector;
    m_xF0 = ZeroVector;
    m_x1C0 = ZeroVector;
    m_x1D0 = ZeroVector;
    ResetOtherBodyCollisions();
    return 1;
}


// ---- src/PhysicsRigidBody/Render3D.cpp



int PhysicsRigidBody::Render3D()
{
    return PhysicsBody::Render3D();
}

