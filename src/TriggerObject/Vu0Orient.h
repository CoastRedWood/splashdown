// Vu0Orient.h: the VU0 "orientation matrix from two axes" block retail
// inlines in SoultreeObject::SetOrientation, TriggerObject::Create,
// ParticleManager::PrepareGeometry, Scene::LoadStaticModels and
// CarProcedural::Tick. GCC-style extended asm, copied from retail in order
// (the `addi` are retail's hand-written instructions).
//
// mode 1 re-orthogonalises the axes first (keeping `forward` when keep is 1,
// else `up`); the rows come out as (up x forward, up, forward, position).
#ifndef TRIGGEROBJECT_VU0ORIENT_H
#define TRIGGEROBJECT_VU0ORIENT_H

#include "d3dtypes.h"

// The axes are copied to locals (retail's field-by-field copies). mwcc
// hands operand registers out from the last operand down, so the operand
// order reproduces retail's ($v0 position, $v1 matrix, ...). Retail declared
// only $7 clobbered although the asm also writes $6.
static inline void Vu0OrientMatrix(_D3DMATRIX *m, _D3DVECTOR *position, _D3DVECTOR &forwardIn, _D3DVECTOR &upIn, int mode, int keep)
{
    _D3DVECTOR zero;
    _D3DVECTOR forward = forwardIn;
    _D3DVECTOR up = upIn;
    asm("lqc2 vf1, 0x0(%4); lqc2 vf2, 0x0(%2); lqc2 vf3, 0x0(%3); lqc2 vf4, 0x0(%6); addi $7, $0, 1; addi $6, $0, 2; bne %0, $7, _orientNormalize; nop; vopmula.xyz ACC, vf2, vf3; vopmsub.xyz vf5, vf3, vf2; vsub.w vf5, vf5, vf5; bne %1, $7, _orientKeepUp; nop; vopmula.xyz ACC, vf3, vf5; vopmsub.xyz vf2, vf5, vf3; vsub.w vf2, vf2, vf2; b _orientNormalize; _orientKeepUp: vopmula.xyz ACC, vf5, vf2; vopmsub.xyz vf3, vf2, vf5; vsub.w vf3, vf3, vf3; _orientNormalize: vmul.xyz vf5, vf2, vf2; vaddy.x vf5, vf5, vf5y; vaddz.x vf5, vf5, vf5z; vrsqrt Q, vf0w, vf5x; vsub.xyzw vf6, vf0, vf0; vaddw.xyzw vf6, vf6, vf2w; vwaitq; vmulq.xyz vf6, vf2, Q; vmul.xyz vf5, vf3, vf3; vaddy.x vf5, vf5, vf5y; vaddz.x vf5, vf5, vf5z; vrsqrt Q, vf0w, vf5x; vsub.xyzw vf7, vf0, vf0; vaddw.xyzw vf7, vf7, vf3w; vwaitq; vmulq.xyz vf7, vf3, Q; vopmula.xyz ACC, vf6, vf7; vopmsub.xyz vf5, vf7, vf6; vsub.w vf5, vf5, vf5; vsub.w vf5, vf5, vf5; vsub.w vf6, vf6, vf6; vsub.w vf7, vf7, vf7; vsub.w vf4, vf4, vf4; vadd.w vf4, vf4, vf0; sqc2 vf5, 0x0(%5); sqc2 vf6, 0x10(%5); sqc2 vf7, 0x20(%5); sqc2 vf4, 0x30(%5)"
        : : "r"(mode), "r"(keep), "r"(&up), "r"(&forward), "r"(&zero), "r"(m), "r"(position) : "$7", "memory");
}

#endif
