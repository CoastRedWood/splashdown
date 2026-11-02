// SoultreeObject.h plus the members the VU0 functions need; fold
// these back into SoultreeObject.h and drop this copy.
#ifndef SOULTREEOBJECT_SOULTREEVU0_H
#define SOULTREEOBJECT_SOULTREEVU0_H

#include "GameObject/GameObject.h"

extern "C" int strcasecmp(const char *a, const char *b);

#include "d3dtypes.h"

struct Quaternion;

_D3DMATRIX IdentityMatrix();
class SearchableObject;

// SoultreeObject's first base: 8 bytes in front of its GameObject subobject.
#include "SearchableObject/SearchableObject.h"

#include "SearchPlacement/SearchPlacement.h"

#include "SoultreeObject/SoultreeObject.h"

// out = b * a (row vectors), libvu0 sceVu0MulMatrix with the matrices passed
// by value (retail copies them to the stack first: an aligned local with
// lq/sq, anything else word by word). Operands (out, b, a) for retail's
// register choice.
static inline void MulMatrixCopy(_D3DMATRIX *out, _D3DMATRIX a, _D3DMATRIX b)
{
    asm("lqc2 vf5, 0x0(%1); lqc2 vf1, 0x0(%2); lqc2 vf2, 0x10(%2); lqc2 vf3, 0x20(%2); lqc2 vf4, 0x30(%2); lqc2 vf6, 0x10(%1); vmulax.xyzw ACC, vf1, vf5x; vmadday.xyzw ACC, vf2, vf5y; vmaddaz.xyzw ACC, vf3, vf5z; vmaddw.xyzw vf9, vf4, vf5w; lqc2 vf7, 0x20(%1); vmulax.xyzw ACC, vf1, vf6x; vmadday.xyzw ACC, vf2, vf6y; vmaddaz.xyzw ACC, vf3, vf6z; vmaddw.xyzw vf10, vf4, vf6w; lqc2 vf8, 0x30(%1); vmulax.xyzw ACC, vf1, vf7x; vmadday.xyzw ACC, vf2, vf7y; vmaddaz.xyzw ACC, vf3, vf7z; vmaddw.xyzw vf11, vf4, vf7w; vmulax.xyzw ACC, vf1, vf8x; vmadday.xyzw ACC, vf2, vf8y; vmaddaz.xyzw ACC, vf3, vf8z; vmaddw.xyzw vf12, vf4, vf8w; sqc2 vf9, 0x0(%0); sqc2 vf10, 0x10(%0); sqc2 vf11, 0x20(%0); sqc2 vf12, 0x30(%0)"
        : : "r"(out), "r"(&b), "r"(&a) : "memory");
}

// out = a * inverse(b), for rigid b: libvu0-style transposes plus
// sceVu0MulMatrix's loop. (The written nop after the loop's bne survives:
// mwcc's assembler fills the delay slot with the addi before it. Operands
// (out, b, a) for retail's register choice.)
static inline void MulInverseMatrix(_D3DMATRIX *out, _D3DMATRIX a, _D3DMATRIX b)
{
    asm("lqc2 vf1, 0x30(%2); lqc2 vf2, 0x30(%1); vsub.xyz vf30, vf1, vf2; lqc2 vf1, 0x0(%2); lqc2 vf2, 0x10(%2); lqc2 vf3, 0x20(%2); lqc2 vf4, 0x30(%2); vsub.xyzw vf31, vf31, vf31; vadd.xyzw vf5, vf1, vf31; vadd.xyzw vf6, vf2, vf31; vadd.xyzw vf7, vf3, vf31; vadd.xyzw vf8, vf4, vf31; vaddy.x vf6, vf0, vf1y; vaddz.x vf7, vf0, vf1z; vaddz.y vf7, vf0, vf2z; vaddx.y vf5, vf0, vf2x; vaddx.z vf5, vf0, vf3x; vaddy.z vf6, vf0, vf3y; addi $7, $0, 4; addi $8, %0, 0; addi $9, %1, 0; _loopMulInverse: lqc2 vf11, 0x0($9); vmulax.xyzw ACC, vf5, vf11x; vmadday.xyzw ACC, vf6, vf11y; vmaddaz.xyzw ACC, vf7, vf11z; vmaddw.xyzw vf10, vf8, vf11w; sqc2 vf10, 0x0($8); addi $7, $7, -1; addi $9, $9, 0x10; addi $8, $8, 0x10; bne $0, $7, _loopMulInverse; nop; lqc2 vf11, 0x0(%0); lqc2 vf12, 0x10(%0); lqc2 vf13, 0x20(%0); lqc2 vf14, 0x30(%0); vadd.xyzw vf15, vf11, vf31; vadd.xyzw vf16, vf12, vf31; vadd.xyzw vf17, vf13, vf31; vadd.xyzw vf18, vf14, vf31; vaddy.x vf16, vf0, vf11y; vaddz.x vf17, vf0, vf11z; vaddz.y vf17, vf0, vf12z; vaddx.y vf15, vf0, vf12x; vaddx.z vf15, vf0, vf13x; vaddy.z vf16, vf0, vf13y; sqc2 vf15, 0x0(%0); sqc2 vf16, 0x10(%0); sqc2 vf17, 0x20(%0); sqc2 vf18, 0x30(%0); lqc2 vf21, 0x0(%1); lqc2 vf22, 0x10(%1); lqc2 vf23, 0x20(%1); lqc2 vf24, 0x30(%1); vadd.xyzw vf25, vf21, vf31; vadd.xyzw vf26, vf22, vf31; vadd.xyzw vf27, vf23, vf31; vadd.xyzw vf28, vf24, vf31; vaddy.x vf26, vf0, vf21y; vaddz.x vf27, vf0, vf21z; vaddz.y vf27, vf0, vf22z; vaddx.y vf25, vf0, vf22x; vaddx.z vf25, vf0, vf23x; vaddy.z vf26, vf0, vf23y; vmulax.xyz ACC, vf25, vf30x; vmadday.xyz ACC, vf26, vf30y; vmaddz.xyz vf31, vf27, vf30z; vsub.w vf31, vf31, vf31; vadd.w vf31, vf31, vf0; sqc2 vf31, 0x30(%0)"
        : : "r"(out), "r"(&b), "r"(&a) : "$7", "$8", "$9", "memory");
}

#endif
