// Vu0More.h — more libvu0-style VU0 macro-mode helpers for src/global (round
// 33), in the style of Vu0Inline.h: one asm string, retail's order.
#ifndef GLOBAL_VU0MORE_H
#define GLOBAL_VU0MORE_H

#include "d3dtypes.h"

// libvu0 sceVu0OuterProduct: out = a x b.
static inline void Vu0CrossProduct(_D3DVECTOR *out, _D3DVECTOR *a, _D3DVECTOR *b)
{
    asm("lqc2 vf4, 0x0(%1); lqc2 vf5, 0x0(%2); vopmula.xyz ACC, vf4, vf5; vopmsub.xyz vf6, vf5, vf4; vsub.w vf6, vf6, vf6; sqc2 vf6, 0x0(%0)"
        : : "r"(out), "r"(a), "r"(b) : "memory");
}

// libvu0 sceVu0Normalize.
static inline void Vu0Normalize(_D3DVECTOR *out, _D3DVECTOR *in)
{
    asm("lqc2 vf4, 0x0(%1); vmul.xyz vf5, vf4, vf4; vaddy.x vf5, vf5, vf5y; vaddz.x vf5, vf5, vf5z; vrsqrt Q, vf0w, vf5x; vsub.xyzw vf6, vf0, vf0; vaddw.xyzw vf6, vf6, vf4w; vwaitq; vmulq.xyz vf6, vf4, Q; sqc2 vf6, 0x0(%0)"
        : : "r"(out), "r"(in) : "memory");
}

// The same, normalizing a temporary (e.g. a difference of two vectors), as
// Camera/CameraVU0.h's overload.
static inline void Vu0Normalize(_D3DVECTOR *out, const _D3DVECTOR &in)
{
    asm("lqc2 vf4, 0x0(%1); vmul.xyz vf5, vf4, vf4; vaddy.x vf5, vf5, vf5y; vaddz.x vf5, vf5, vf5z; vrsqrt Q, vf0w, vf5x; vsub.xyzw vf6, vf0, vf0; vaddw.xyzw vf6, vf6, vf4w; vwaitq; vmulq.xyz vf6, vf4, Q; sqc2 vf6, 0x0(%0)"
        : : "r"(out), "r"(&in) : "memory");
}

// Retail's reciprocal square root (as src/global/Abs.cpp): rsqrt.s into $f8,
// stored through a pointer to a stack temporary.
static inline float RSqrt(float x)
{
    float r;
    asm("rsqrt.s $f8, %0, %1; swc1 $f8, 0(%2)" : : "f"(1.0f), "f"(x), "r"(&r) : "$f8", "memory");
    return r;
}

// out = inverse(m) * v for a rigid m: v minus the translation, rotated by the
// transposed rotation; w ends up 1. v by value and before m: the order the
// arguments are evaluated in decides the registers.
static inline void Vu0InverseTransformPoint(_D3DVECTOR *out, _D3DVECTOR v, _D3DMATRIX *m)
{
    asm("lqc2 vf1, 0x0(%1); lqc2 vf2, 0x10(%1); lqc2 vf3, 0x20(%1); lqc2 vf4, 0x30(%1); vmulw.w vf21, vf0, vf0w; vsub.xyzw vf30, vf30, vf30; vadd.xyzw vf5, vf1, vf30; vadd.xyzw vf6, vf2, vf30; vadd.xyzw vf7, vf3, vf30; vadd.xyzw vf8, vf4, vf30; vaddy.x vf6, vf0, vf1y; vaddz.x vf7, vf0, vf1z; vaddz.y vf7, vf0, vf2z; vaddx.y vf5, vf0, vf2x; vaddx.z vf5, vf0, vf3x; vaddy.z vf6, vf0, vf3y; lqc2 vf20, 0x0(%2); vsub.xyz vf20, vf20, vf8; vmulax.xyz ACC, vf5, vf20x; vmadday.xyz ACC, vf6, vf20y; vmaddz.xyz vf21, vf7, vf20z; sqc2 vf21, 0x0(%0)"
        : : "r"(out), "r"(m), "r"(&v) : "memory");
}

// out = m * in (point: w = 1); operands (out, m, in) as
// CollisionObject/vu0col.h's TransformPoint.
static inline void Vu0TransformPointOut(_D3DVECTOR *out, _D3DMATRIX *m, _D3DVECTOR *in)
{
    asm("lqc2 vf4, 0x0(%1); lqc2 vf5, 0x10(%1); lqc2 vf6, 0x20(%1); lqc2 vf7, 0x30(%1); lqc2 vf8, 0x0(%2); vmulw.w vf9, vf0, vf0w; vmulax.xyz ACC, vf4, vf8x; vmadday.xyz ACC, vf5, vf8y; vmaddz.xyz vf9, vf6, vf8z; vadd.xyz vf9, vf9, vf7; sqc2 vf9, 0x0(%0)"
        : : "r"(out), "r"(m), "r"(in) : "memory");
}

// The same transform with the operands listed (out, in, m): retail has both
// orders (they decide which registers get the addresses).
static inline void Vu0TransformPointOutIn(_D3DVECTOR *out, _D3DMATRIX *m, _D3DVECTOR *in)
{
    asm("lqc2 vf4, 0x0(%2); lqc2 vf5, 0x10(%2); lqc2 vf6, 0x20(%2); lqc2 vf7, 0x30(%2); lqc2 vf8, 0x0(%1); vmulw.w vf9, vf0, vf0w; vmulax.xyz ACC, vf4, vf8x; vmadday.xyz ACC, vf5, vf8y; vmaddz.xyz vf9, vf6, vf8z; vadd.xyz vf9, vf9, vf7; sqc2 vf9, 0x0(%0)"
        : : "r"(out), "r"(in), "r"(m) : "memory");
}

// out = m's 3x3 part * v (w ends up 1), v by value.
static inline void Vu0ApplyMatrixVector(_D3DVECTOR *out, _D3DMATRIX *m, _D3DVECTOR v)
{
    asm("lqc2 vf4, 0x0(%1); lqc2 vf5, 0x10(%1); lqc2 vf6, 0x20(%1); lqc2 vf8, 0x0(%2); vmulax.xyz ACC, vf4, vf8x; vmadday.xyz ACC, vf5, vf8y; vmaddz.xyz vf9, vf6, vf8z; vadd.xyzw vf9, vf9, vf0; sqc2 vf9, 0x0(%0)"
        : : "r"(out), "r"(m), "r"(&v) : "memory");
}

// v . v (xyz).
static inline float Vu0LengthSquared(_D3DVECTOR *v)
{
    float r;
    asm("lqc2 vf4, 0x0(%1); vmove.xyzw vf5, vf4; vmul.xyz vf5, vf4, vf5; vaddy.x vf5, vf5, vf5y; vaddz.x vf5, vf5, vf5z; qmfc2 $2, vf5; mtc1 $2, %0"
        : "=f"(r) : "r"(v) : "$2");
    return r;
}

// The xyz dot product of vf4 (left by a previous Vu0DotProduct) and b.
static inline float Vu0DotProductVf4(_D3DVECTOR *b)
{
    float r;
    asm("lqc2 vf5, 0x0(%1); vmul.xyz vf5, vf4, vf5; vaddy.x vf5, vf5, vf5y; vaddz.x vf5, vf5, vf5z; qmfc2 $2, vf5; mtc1 $2, %0"
        : "=f"(r) : "r"(b) : "$2");
    return r;
}

// |v|, 0 for a zero vector.
static inline float Magnitude(const _D3DVECTOR &v)
{
    float len2 = v.x * v.x + v.y * v.y + v.z * v.z;
    if (len2 == 0.0f)
        return 0.0f;
    return 1.0f / RSqrt(len2);
}

#endif
