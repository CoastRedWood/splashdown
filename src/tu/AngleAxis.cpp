// AngleAxis.cpp  @ 0x003535F0 (text 0x3535F0-0x353BE0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's AngleAxis.cpp as one translation unit: an unanchored TU (no class
// named after it) at the end of MotionLibrary.cpp's tu_map row:
// SinCosTableCalculations' ctor and the byte/short angle-axis packers, then
// the static initializer (__sinit_AngleAxis.cpp, the .ctor slot after
// MotionLibrary.cpp's), which constructs the axis vectors and
// SinCosTableCaclulator. Its .bss holds the sin/cos tables after its axis
// vectors (0x496EC0-0x49F6C0). `make link` places the object's data as well
// as its code (tools/link_c.py).


#include "d3dtypes.h"
#include "Quaternion/Quaternion.h"
#include "ByteVector/ByteVector.h"
#include "sinit/AxisVectors.h"

extern "C" double sin(double x);
extern "C" double cos(double x);

// Packed rotations: angle in 256 or 4096 steps per turn, unit axis * 127 or
// * 32767 (the ByteAxis/ShortAxisRotationController keys).
struct ByteAngleAxis {
    unsigned char angle;        // +0x0, 256 steps per turn
    ByteVector axis;            // +0x1, unit axis * 127
};

struct ShortVector {
    short x, y, z;
};

struct ShortAngleAxis {
    unsigned short angle;       // +0x0, 4096 steps per turn (GetQuaternion reads it with lhu)
    ShortVector axis;           // +0x2, unit axis * 32767
};

class SinCosTableCalculations {
public:
    SinCosTableCalculations();
};

float HiResSinAlphaOverTwo[4096];
float HiResCosAlphaOverTwo[4096];
float ByteSinAlphaOverTwo[256];
float ByteCosAlphaOverTwo[256];

void ComputeAngleAxis(Quaternion *q, float *angle, _D3DVECTOR *axis);

inline float RSqrt(float x)
{
    float r;
    asm("rsqrt.s $f8, %0, %1; swc1 $f8, 0(%2)" : : "f"(1.0f), "f"(x), "r"(&r) : "$f8", "memory");
    return r;
}

inline int FToI(float f)
{
    int i;
    asm("cvt.w.s $f5, %1; mfc1 %0, $f5" : "=r"(i) : "f"(f) : "$f5");
    return i;
}

// ---- src/SinCosTableCalculations/ctor.cpp
// Compiler session state: retail compiled
// this file in an mwcc session that had already generated a two-argument runtime
// helper call (64-bit / or %, double arithmetic or compare). mwcc then models
// every later helper call (__floatdisf, fptoui, fptodp, ...) as reading $a0-$a1,
// so temporaries live before such a call skip $a0/$a1. This unused function
// recreates that state; it is not part of the retail file.
static void MWCC_SHIM_TEXT MwccSessionState(long *a) { a[0] = a[0] / a[1]; }

// sin and cos of half of each angle step, for the short (4096) and byte (256)
// angle encodings.
SinCosTableCalculations::SinCosTableCalculations()
{
    int i;
    for (i = 0; i < 4096.0f; i++) {
        float half = 0.5f * (6.28318548f * (i / 4096.0f));
        HiResSinAlphaOverTwo[i] = sin(half);
        HiResCosAlphaOverTwo[i] = cos(half);
    }
    for (i = 0; i < 256; i++) {
        float half = 0.5f * (6.28318548f * (i / 256.0f));
        ByteSinAlphaOverTwo[i] = sin(half);
        ByteCosAlphaOverTwo[i] = cos(half);
    }
}


// ---- src/global/SetByteAngleAxis.cpp

void SetByteAngleAxis(ByteAngleAxis *out, Quaternion *q)
{
    _D3DVECTOR axis;
    float angle;
    ByteVector v;

    ComputeAngleAxis(q, &angle, &axis);
    float lengthSquared = axis.x * axis.x + axis.y * axis.y + axis.z * axis.z;
    if (lengthSquared == 0.0f) {
        axis = ZeroVector;
    } else {
        float scale = RSqrt(lengthSquared);
        axis.x *= scale;
        axis.y *= scale;
        axis.z *= scale;
    }
    out->angle = FToI(0.5f + 255.0f * angle / 6.28318548f);
    v.x = FToI(0.5f + 127.0f * axis.x);
    v.y = FToI(0.5f + 127.0f * axis.y);
    v.z = FToI(0.5f + 127.0f * axis.z);
    out->axis = v;
}


// ---- src/global/GetQuaternion_00353930.cpp


// Rotation by angle about the packed unit axis: w = cos(angle / 2), xyz =
// axis * sin(angle / 2), both from the SinCosTableCalculations tables.
void GetQuaternion(ByteAngleAxis *aa, Quaternion *q)
{
    float s = ByteSinAlphaOverTwo[aa->angle];
    q->w = ByteCosAlphaOverTwo[aa->angle];
    float z, y, x;
    x = aa->axis.x * (1.0f / 127.0f);
    y = aa->axis.y * (1.0f / 127.0f);
    z = aa->axis.z * (1.0f / 127.0f);
    q->x = x * s;
    q->y = y * s;
    q->z = z * s;
}


// ---- src/global/SetShortAngleAxis.cpp
#pragma fast_fptosi on

// The scale constants go through locals (declared in this order) to get
// retail's load order and FP registers.
void SetShortAngleAxis(ShortAngleAxis *out, Quaternion *q)
{
    float unit;
    float twoPi;
    float steps;
    _D3DVECTOR axis;
    float angle;
    ShortVector v;

    ComputeAngleAxis(q, &angle, &axis);
    float lengthSquared = axis.x * axis.x + axis.y * axis.y + axis.z * axis.z;
    if (lengthSquared == 0.0f) {
        axis = ZeroVector;
    } else {
        float scale = RSqrt(lengthSquared);
        axis.x *= scale;
        axis.y *= scale;
        axis.z *= scale;
    }
    steps = 4096.0f;
    twoPi = 6.28318548f;
    out->angle = (int)(steps * (angle / twoPi));
    unit = 32767.0f;
    v.x = FToI(0.5f + unit * axis.x);
    v.y = FToI(0.5f + unit * axis.y);
    v.z = FToI(0.5f + unit * axis.z);
    out->axis = v;
}


// ---- src/global/GetQuaternion_00353B50.cpp

#pragma fast_fptosi reset

// Rotation by angle about the packed unit axis: w = cos(angle / 2), xyz =
// axis * sin(angle / 2), both from the SinCosTableCalculations tables.
void GetQuaternion(ShortAngleAxis *aa, Quaternion *q)
{
    float s = HiResSinAlphaOverTwo[aa->angle];
    q->w = HiResCosAlphaOverTwo[aa->angle];
    float z, y, x;
    x = aa->axis.x * (1.0f / 32767.0f);
    y = aa->axis.y * (1.0f / 32767.0f);
    z = aa->axis.z * (1.0f / 32767.0f);
    q->x = x * s;
    q->y = y * s;
    q->z = z * s;
}


// ---- src/sinit/AngleAxis.cpp
// Static initializer of AngleAxis.cpp: the axis vectors, then the object
// whose constructor fills the sin/cos tables (an empty class: 1 byte, so it
// sits in .sbss and is addressed off $gp). "Caclulator" is retail's spelling.

SinCosTableCalculations SinCosTableCaclulator;
