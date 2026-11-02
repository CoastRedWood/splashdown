// SoultreeOrient.h: the VU0 "orientation matrix from two axes" blocks that
// both SoultreeObject::SetOrientation overloads inline. GCC-style extended
// asm copied from retail in order; the `addi` are retail's hand-written
// instructions. TriggerObject/Vu0Orient.h holds the same general block
// with the operand order its callers need.
#ifndef SOULTREEOBJECT_SOULTREEORIENT_H
#define SOULTREEOBJECT_SOULTREEORIENT_H

#include "d3dtypes.h"

// Rows (up x front, up, front, m's own position row) for axes that are
// already orthonormal: no normalisation.
static inline void Vu0SetAxes(_D3DMATRIX *m, _D3DVECTOR *front, _D3DVECTOR *up)
{
    asm("lqc2 vf4, 0x0(%1); lqc2 vf5, 0x0(%2); lqc2 vf7, 0x30(%0); vopmula.xyz ACC, vf4, vf5; vopmsub.xyz vf6, vf5, vf4; vsub.w vf6, vf6, vf6; vsub.w vf4, vf4, vf4; vsub.w vf5, vf5, vf5; vsub.w vf7, vf7, vf7; vadd.w vf7, vf0, vf7; sqc2 vf6, 0x0(%0); sqc2 vf4, 0x10(%0); sqc2 vf5, 0x20(%0); sqc2 vf7, 0x30(%0)"
        : : "r"(m), "r"(up), "r"(front) : "memory");
}

// The same with the position row taken from pos.
static inline void Vu0SetAxes(_D3DMATRIX *m, _D3DVECTOR *pos, _D3DVECTOR *front, _D3DVECTOR *up)
{
    asm("lqc2 vf4, 0x0(%1); lqc2 vf5, 0x0(%2); lqc2 vf7, 0x0(%0); vopmula.xyz ACC, vf4, vf5; vopmsub.xyz vf6, vf5, vf4; vsub.w vf6, vf6, vf6; vsub.w vf4, vf4, vf4; vsub.w vf5, vf5, vf5; vsub.w vf7, vf7, vf7; vadd.w vf7, vf0, vf7; sqc2 vf6, 0x0(%3); sqc2 vf4, 0x10(%3); sqc2 vf5, 0x20(%3); sqc2 vf7, 0x30(%3)"
        : : "r"(pos), "r"(up), "r"(front), "r"(m) : "memory");
}

// Normalised rows (up x front, up, front, pos). mode 1 re-orthogonalises
// first (keeping front when keep is 1, else up). The axes are copied to
// locals (retail's field-by-field copies); `zero` is loaded and unused.
// mwcc hands operand registers out from the last operand down, so the
// operand order reproduces retail's.
static inline void Vu0OrientAxes(_D3DMATRIX *m, _D3DVECTOR *pos, _D3DVECTOR *frontIn, _D3DVECTOR *upIn, int mode, int keep)
{
    _D3DVECTOR zero;
    _D3DVECTOR front = *frontIn;
    _D3DVECTOR up = *upIn;
    asm("lqc2 vf1, 0x0(%2); lqc2 vf2, 0x0(%0); lqc2 vf3, 0x0(%1); lqc2 vf4, 0x0(%4); addi $7, $0, 1; addi $6, $0, 2; bne %5, $7, _orientNormalize; nop; vopmula.xyz ACC, vf2, vf3; vopmsub.xyz vf5, vf3, vf2; vsub.w vf5, vf5, vf5; bne %6, $7, _orientKeepUp; nop; vopmula.xyz ACC, vf3, vf5; vopmsub.xyz vf2, vf5, vf3; vsub.w vf2, vf2, vf2; b _orientNormalize; _orientKeepUp: vopmula.xyz ACC, vf5, vf2; vopmsub.xyz vf3, vf2, vf5; vsub.w vf3, vf3, vf3; _orientNormalize: vmul.xyz vf5, vf2, vf2; vaddy.x vf5, vf5, vf5y; vaddz.x vf5, vf5, vf5z; vrsqrt Q, vf0w, vf5x; vsub.xyzw vf6, vf0, vf0; vaddw.xyzw vf6, vf6, vf2w; vwaitq; vmulq.xyz vf6, vf2, Q; vmul.xyz vf5, vf3, vf3; vaddy.x vf5, vf5, vf5y; vaddz.x vf5, vf5, vf5z; vrsqrt Q, vf0w, vf5x; vsub.xyzw vf7, vf0, vf0; vaddw.xyzw vf7, vf7, vf3w; vwaitq; vmulq.xyz vf7, vf3, Q; vopmula.xyz ACC, vf6, vf7; vopmsub.xyz vf5, vf7, vf6; vsub.w vf5, vf5, vf5; vsub.w vf5, vf5, vf5; vsub.w vf6, vf6, vf6; vsub.w vf7, vf7, vf7; vsub.w vf4, vf4, vf4; vadd.w vf4, vf4, vf0; sqc2 vf5, 0x0(%3); sqc2 vf6, 0x10(%3); sqc2 vf7, 0x20(%3); sqc2 vf4, 0x30(%3)"
        : : "r"(&up), "r"(&front), "r"(&zero), "r"(m), "r"(pos), "r"(mode), "r"(keep) : "$6", "$7", "memory");
}

#endif
