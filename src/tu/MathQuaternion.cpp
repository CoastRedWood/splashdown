// MathQuaternion.cpp  @ 0x0025C1D0 (text 0x25C1D0-0x25CC40)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp)
//
// Retail's MathQuaternion.cpp as one translation unit: the Quaternion
// helpers, unanchored inside LightEmitter.cpp's tu_map row
// (__sinit_MathQuaternion.cpp, the .ctor slot after Math3d.cpp's, builds only
// the axis vectors). It has no .rodata; its start (Abs, the first Quaternion
// function) is undecided against Math3d.cpp's AdamsBashforthIntegration.
// `make link` places its data as well as its code.


#include "d3dtypes.h"
#include "Quaternion/Quaternion.h"
#include "sinit/AxisVectors.h"

extern "C" double acos(double);
extern "C" double sin(double);
float FastAtan2(float y, float x);
float FastACos(float x);
float FastSin(float x);
// .data 0x3FB6A0, before Mygame.cpp's LpGame (all zeros, but initialized, so
// not .bss). Other files declare it `extern const`.
Quaternion ZeroQuaternion = { 0.0f, 0.0f, 0.0f, 0.0f };

struct ByteAngleAxis;
struct ShortAngleAxis;
void GetQuaternion(ByteAngleAxis *aa, Quaternion *q);
void GetQuaternion(ShortAngleAxis *aa, Quaternion *q);

// ---- src/global/Abs.cpp


// Retail's reciprocal square root is an inline-asm helper like its sqrt:
// rsqrt.s into $f8, stored through a pointer to a stack temporary.
inline float RSqrt(float x)
{
    float r;
    asm("rsqrt.s $f8, %0, %1; swc1 $f8, 0(%2)" : : "f"(1.0f), "f"(x), "r"(&r) : "$f8", "memory");
    return r;
}

// Unit quaternion in q's direction.
Quaternion Abs(Quaternion &q)
{
    Quaternion r;
    float len2 = q.x * q.x + q.w * q.w + q.y * q.y + q.z * q.z;
    if (len2 == 0.0f) {
        r = ZeroQuaternion;
    } else {
        float s = RSqrt(len2);
        r.w = q.w * s;
        r.x = q.x * s;
        r.y = q.y * s;
        r.z = q.z * s;
    }
    return r;
}



// ---- src/global/ComputeMatrix.cpp


// Quaternion to rotation matrix (row-vector convention), scaled by 2/|q|^2
// so a non-unit quaternion still gives a pure rotation. The declaration and
// product orders reproduce retail's register allocation and scheduling.
_D3DMATRIX ComputeMatrix(Quaternion &q)
{
    _D3DMATRIX m;
    float s;
    float xs;
    float ys;
    float zs;
    float yy;
    float wx;
    float wz;
    float wy;
    float xy;
    float xz;
    float yz;
    float xx;
    float zz;
    float len2 = q.w * q.w + q.x * q.x + q.y * q.y + q.z * q.z;

    s = (len2 > 0.0f) ? (2.0f / len2) : 0.0f;

    ys = q.y * s;
    xs = q.x * s;
    zs = q.z * s;
    zz = q.z * zs;
    wx = q.w * xs;
    wy = q.w * ys;
    xx = q.x * xs;
    xy = q.x * ys;
    wz = q.w * zs;
    xz = q.x * zs;
    yz = q.y * zs;
    yy = q.y * ys;

    m._11 = 1.0f - (yy + zz);
    m._12 = xy + wz;
    m._13 = xz - wy;
    m._14 = 0.0f;
    m._21 = xy - wz;
    m._22 = 1.0f - (xx + zz);
    m._23 = yz + wx;
    m._24 = 0.0f;
    m._31 = xz + wy;
    m._32 = yz - wx;
    m._33 = 1.0f - (xx + yy);
    m._34 = 0.0f;
    m._41 = 0.0f;
    m._42 = 0.0f;
    m._43 = 0.0f;
    m._44 = 1.0f;
    return m;
}


// ---- src/global/ComputeQuaternionRate.cpp


// dq/dt = 1/2 q * (0, omega).
Quaternion ComputeQuaternionRate(Quaternion &q, _D3DVECTOR &omega)
{
    Quaternion r;
    r.w = -0.5f * (q.x * omega.x + q.y * omega.y + q.z * omega.z);
    r.x = 0.5f * (q.w * omega.x + q.y * omega.z - q.z * omega.y);
    r.y = 0.5f * (q.w * omega.y + q.z * omega.x - q.x * omega.z);
    r.z = 0.5f * (q.w * omega.z + q.x * omega.y - q.y * omega.x);
    return r;
}


// ---- src/global/ComputeAngleAxis.cpp
// Compiler session state: retail compiled
// this file in an mwcc session that had already generated a two-argument runtime
// helper call (64-bit / or %, double arithmetic or compare). mwcc then models
// every later helper call (__floatdisf, fptoui, fptodp, ...) as reading $a0-$a1,
// so temporaries live before such a call skip $a0/$a1. This unused function
// recreates that state; it is not part of the retail file.
static void MWCC_SHIM_TEXT MwccSessionState(long *a) { a[0] = a[0] / a[1]; }




void ComputeAngleAxis(Quaternion *q, float *angle, _D3DVECTOR *axis)
{
    *angle = 2.0f * (float)acos(q->w);
    if (*angle != 0.0f) {
        float inv = 1.0f / (float)sin(0.5f * *angle);
        axis->x = q->x * inv;
        axis->y = q->y * inv;
        axis->z = q->z * inv;
    } else {
        axis->x = 1.0f;
        axis->y = 0.0f;
        axis->z = 0.0f;
    }
}


// ---- src/global/ComputeQuaternion.cpp
inline float Sqrt(float x)
{
    float r;
    asm("sqrt.s $f8, %1; swc1 $f8, 0(%0)" : : "r"(&r), "f"(x) : "$f8", "memory");
    return r;
}



// Rotation matrix to quaternion: take the root of the largest of the four
// squared components and derive the other three from it. The squares are
// compared through the array but the root is taken of the scalar, and `s`
// is block-local; both show in retail's register use.
Quaternion ComputeQuaternion(_D3DMATRIX &m)
{
    Quaternion q;
    float t[4];
    float w2;
    float x2;
    float y2;
    float z2;
    w2 = 0.25f * (1.0f + m._11 + m._22 + m._33);
    x2 = 0.25f * (1.0f + m._11 - m._22 - m._33);
    y2 = 0.25f * (1.0f - m._11 + m._22 - m._33);
    z2 = 0.25f * (1.0f - m._11 - m._22 + m._33);
    t[0] = w2;
    t[1] = x2;
    t[2] = y2;
    t[3] = z2;

    if (t[0] > t[1] && t[0] > t[2] && t[0] > t[3]) {
        q.w = Sqrt(w2);
        float s = 0.25f / q.w;
        q.x = s * (m._23 - m._32);
        q.y = s * (m._31 - m._13);
        q.z = s * (m._12 - m._21);
    } else if (t[1] > t[2] && t[1] > t[3]) {
        q.x = Sqrt(x2);
        float s = 0.25f / q.x;
        q.w = s * (m._23 - m._32);
        q.y = s * (m._12 + m._21);
        q.z = s * (m._13 + m._31);
    } else if (t[2] > t[3]) {
        q.y = Sqrt(y2);
        float s = 0.25f / q.y;
        q.w = s * (m._31 - m._13);
        q.x = s * (m._12 + m._21);
        q.z = s * (m._23 + m._32);
    } else {
        q.z = Sqrt(z2);
        float s = 0.25f / q.z;
        q.w = s * (m._12 - m._21);
        q.x = s * (m._13 + m._31);
        q.y = s * (m._23 + m._32);
    }
    return q;
}


// ---- src/global/ComputeEulerAngles_audit.cpp




// Euler angles (radians) of a unit quaternion.
_D3DVECTOR ComputeEulerAngles(Quaternion &q)
{
    _D3DVECTOR r;
    float a = 2.0f * (q.x * q.y + q.w * q.z);
    float b = 1.0f - 2.0f * (q.x * q.x + q.z * q.z);
    float c = 2.0f * (q.x * q.z + q.w * q.y);
    float e = 2.0f * (q.y * q.z - q.w * q.x);
    float d = 1.0f - 2.0f * (q.x * q.x + q.y * q.y);
    r.y = -FastAtan2(c, d);
    r.z = -FastAtan2(a, b);
    r.x = -FastAtan2(-e, Sqrt(a * a + b * b));
    return r;
}


// ---- src/global/FastInterpolateQuaternion.cpp


// Spherical linear interpolation using the fast trig approximations.
void FastInterpolateQuaternion(Quaternion *a, Quaternion *b, Quaternion *out, float t)
{
    static int FastInterpQuatID = -1;  // profiler ID, unused
    float dot = a->x * b->x + a->y * b->y + a->z * b->z + a->w * b->w;
    float sign;
    if (dot < 0.0f) {
        sign = -1.0f;
        dot = -dot;
    } else {
        sign = 1.0f;
    }
    if (dot >= 1.0f) {
        out->w = a->w;
        out->x = a->x;
        out->y = a->y;
        out->z = a->z;
    } else {
        float omega = FastACos(dot);
        float sinOmega = FastSin(omega);
        float inv = 1.0f / sinOmega;
        float tOmega = t * omega;
        sign *= inv * FastSin(tOmega);
        float scaleA = inv * FastSin(omega - tOmega);
        out->w = scaleA * a->w + sign * b->w;
        out->x = scaleA * a->x + sign * b->x;
        out->y = scaleA * a->y + sign * b->y;
        out->z = scaleA * a->z + sign * b->z;
    }
}


// ---- src/global/MakeClosest.cpp



// Flips q to the hemisphere of aa's rotation, so interpolating between them
// takes the short way round.
void MakeClosest(Quaternion *q, ByteAngleAxis *aa)
{
    Quaternion t;
    GetQuaternion(aa, &t);
    if (q->x * t.x + q->y * t.y + q->z * t.z + q->w * t.w < 0.0f) {
        q->x = -q->x;
        q->y = -q->y;
        q->z = -q->z;
        q->w = -q->w;
    }
}



// ---- src/global/MakeClosest_0025CB90.cpp



// Flips q to the hemisphere of aa's rotation, so interpolating between them
// takes the short way round.
void MakeClosest(Quaternion *q, ShortAngleAxis *aa)
{
    Quaternion t;
    GetQuaternion(aa, &t);
    if (q->x * t.x + q->y * t.y + q->z * t.z + q->w * t.w < 0.0f) {
        q->x = -q->x;
        q->y = -q->y;
        q->z = -q->z;
        q->w = -q->w;
    }
}


// ---- src/sinit/MathQuaternion.cpp
// __sinit_MathQuaternion.cpp builds only the axis vectors (sinit/AxisVectors.h, included above).
