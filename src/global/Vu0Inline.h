// Vu0Inline.h — libvu0-style VU0 macro-mode helpers, written the way retail
// inlined them: GCC-style extended asm, one string literal, instruction order
// copied from retail (mwcc never reorders inside an asm).
//
// Only pointers are taken, so _D3DVECTOR/_D3DMATRIX may be defined however
// the including file needs (with or without constructors).
#ifndef VU0INLINE_H
#define VU0INLINE_H

struct _D3DVECTOR;
struct _D3DMATRIX;

// xyz dot product; the result comes back through $v0 (qmfc2/mtc1).
static inline float Vu0DotProduct(_D3DVECTOR *a, _D3DVECTOR *b)
{
    float r;
    asm("lqc2 vf4, 0x0(%1); lqc2 vf5, 0x0(%2); vmul.xyz vf5, vf4, vf5; vaddy.x vf5, vf5, vf5y; vaddz.x vf5, vf5, vf5z; qmfc2 $2, vf5; mtc1 $2, %0"
        : "=f"(r) : "r"(a), "r"(b) : "$2");
    return r;
}

// sceVu0MulMatrix: out = m0 * m1.  Operands listed as libvu0 does
// (out, m1, m0); the order decides which registers get the addresses.
static inline void Vu0MulMatrix(_D3DMATRIX *out, _D3DMATRIX *m0, _D3DMATRIX *m1)
{
    asm("lqc2 vf5, 0x0(%1); lqc2 vf1, 0x0(%2); lqc2 vf2, 0x10(%2); lqc2 vf3, 0x20(%2); lqc2 vf4, 0x30(%2); lqc2 vf6, 0x10(%1); vmulax.xyzw ACC, vf1, vf5x; vmadday.xyzw ACC, vf2, vf5y; vmaddaz.xyzw ACC, vf3, vf5z; vmaddw.xyzw vf9, vf4, vf5w; lqc2 vf7, 0x20(%1); vmulax.xyzw ACC, vf1, vf6x; vmadday.xyzw ACC, vf2, vf6y; vmaddaz.xyzw ACC, vf3, vf6z; vmaddw.xyzw vf10, vf4, vf6w; lqc2 vf8, 0x30(%1); vmulax.xyzw ACC, vf1, vf7x; vmadday.xyzw ACC, vf2, vf7y; vmaddaz.xyzw ACC, vf3, vf7z; vmaddw.xyzw vf11, vf4, vf7w; vmulax.xyzw ACC, vf1, vf8x; vmadday.xyzw ACC, vf2, vf8y; vmaddaz.xyzw ACC, vf3, vf8z; vmaddw.xyzw vf12, vf4, vf8w; sqc2 vf9, 0x0(%0); sqc2 vf10, 0x10(%0); sqc2 vf11, 0x20(%0); sqc2 vf12, 0x30(%0)"
        : : "r"(out), "r"(m1), "r"(m0) : "memory");
}

// sceVu0InversMatrix for a rigid transform (transpose the rotation, negate
// the rotated translation), in place.
static inline void Vu0InversMatrix(_D3DMATRIX *m)
{
    asm("lqc2 vf1, 0x0(%0); lqc2 vf2, 0x10(%0); lqc2 vf3, 0x20(%0); vsub.xyzw vf30, vf30, vf30; vadd.xyzw vf4, vf1, vf30; vadd.xyzw vf5, vf2, vf30; vadd.xyzw vf6, vf3, vf30; vaddy.x vf5, vf0, vf1y; vaddz.x vf6, vf0, vf1z; vaddz.y vf6, vf0, vf2z; vaddx.y vf4, vf0, vf2x; vaddx.z vf4, vf0, vf3x; vaddy.z vf5, vf0, vf3y; lqc2 vf10, 0x30(%0); vsub.xyzw vf11, vf11, vf11; vadd.xyzw vf11, vf10, vf11; vmulax.xyz ACC, vf4, vf10x; vmadday.xyz ACC, vf5, vf10y; vmaddz.xyz vf11, vf6, vf10z; vsub.xyzw vf12, vf12, vf12; vsub.w vf12, vf12, vf0; vmulw.xyz vf11, vf11, vf12w; sqc2 vf4, 0x0(%0); sqc2 vf5, 0x10(%0); sqc2 vf6, 0x20(%0); sqc2 vf11, 0x30(%0)"
        : : "r"(m) : "memory");
}

// out = m * (v.xyz, 1): rotate and translate a point (w set to 1).
static inline void Vu0TransformPoint(_D3DMATRIX *m, _D3DVECTOR *v, _D3DVECTOR *out)
{
    asm("lqc2 vf4, 0x0(%0); lqc2 vf5, 0x10(%0); lqc2 vf6, 0x20(%0); lqc2 vf7, 0x30(%0); lqc2 vf8, 0x0(%1); vmulw.w vf9, vf0, vf0w; vmulax.xyz ACC, vf4, vf8x; vmadday.xyz ACC, vf5, vf8y; vmaddz.xyz vf9, vf6, vf8z; vadd.xyz vf9, vf9, vf7; sqc2 vf9, 0x0(%2)"
        : : "r"(m), "r"(v), "r"(out) : "memory");
}

// Retail's sqrt: sqrt.s into $f8, stored through a stack temporary.
static inline float Fsqrt(float x)
{
    float r;
    asm("sqrt.s $f8, %1; swc1 $f8, 0(%0)" : : "r"(&r), "f"(x) : "$f8", "memory");
    return r;
}

#endif
