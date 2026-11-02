// soultree.cpp  @ 0x00291460 (text 0x291460-0x296850)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp)
//
// Retail's soultree.cpp as one translation unit: SoultreeObject, unanchored
// inside SelectiveGravityModel.cpp's tu_map row (its ASSERTs say
// "soultree.cpp"; __sinit_soultree.cpp, the .ctor slot after SkyCube.cpp's
// row neighbour SelectiveGravityModel.cpp's, builds only the axis vectors).
// Its .rodata (@813.., ending with SoultreeObject's RTTI at 0x41A920)
// follows SkyCube.cpp's; SoultreeMaterial.cpp's (labels restart at @485)
// follows it. `make link` places its data as well as its code.


// UpdateBoundingBox's per-function file used inline_depth(4); it is TU-wide.
#pragma inline_depth(4)

#include "d3dtypes.h"
#include "GameObject/GameObject.h"
#include "SearchableObject/SearchableObject.h"
#include "SoultreeObject/SoultreeObject.h"
#include "ParameterBlock/ParameterBlock.h"
#include "DataAccess/DataAccess.h"
#include "TypeRegistry/TypeRegistry.h"
#include "SearchPlacement/SearchPlacement.h"
#include "SoultreeObject/SoultreeVU0.h"
#include "SoultreeObject/SoultreeOrient.h"
#include "Quaternion/Quaternion.h"
#include "SoultreeObject/SoultreeObjectMore.h"
#include "soultree_assert_block.h"
#include "sinit/AxisVectors.h"

typedef float sceVu0FVECTOR[4] __attribute__((aligned(16)));
typedef float sceVu0FMATRIX[4][4] __attribute__((aligned(16)));

float FastCos(float);
float FastSin(float);

// The rotation helpers of SetRotation and AddRotation, defined ahead of
// every function as in retail: mwcc gives each aligned `{x, y, z}`
// initializer a .bss template where the helper is defined, and retail's
// .bss has SetRotation's two, then AddRotation's, before the functions'
// statics.
static inline void BuildRotationMatrix(_D3DMATRIX *m, float x, float y, float z, float c, float s, float angle)
{
    sceVu0FVECTOR axis = {x, y, z};
    sceVu0FVECTOR cs = {c, s, 1.0f - c, angle};
    asm("lqc2 vf1, 0x0(%1); lqc2 vf30, 0x0(%2); vmul.xyz vf2, vf1, vf1; vaddy.x vf2, vf2, vf2y; vaddz.x vf2, vf2, vf2z; vrsqrt Q, vf0w, vf2x; vsub.xyzw vf3, vf0, vf0; vaddw.xyzw vf3, vf3, vf1w; vwaitq; vmulq.xyz vf3, vf1, Q; vmuly.xyz vf4, vf3, vf30y; vmulx.xyz vf11, vf3, vf3x; vmulz.xyz vf11, vf11, vf30z; vaddx.x vf11, vf11, vf30x; vaddz.y vf11, vf11, vf4z; vsuby.z vf11, vf11, vf4y; vsub.w vf11, vf11, vf11; vmuly.xyz vf12, vf3, vf3y; vmulz.xyz vf12, vf12, vf30z; vsubz.x vf12, vf12, vf4z; vaddx.y vf12, vf12, vf30x; vaddx.z vf12, vf12, vf4x; vsub.w vf12, vf12, vf12; vmulz.xyz vf13, vf3, vf3z; vmulz.xyz vf13, vf13, vf30z; vaddy.x vf13, vf13, vf4y; vsubx.y vf13, vf13, vf4x; vaddx.z vf13, vf13, vf30x; vsub.w vf13, vf13, vf13; sqc2 vf11, 0x0(%0); sqc2 vf12, 0x10(%0); sqc2 vf13, 0x20(%0); sqc2 vf0, 0x30(%0)"
        : : "r"(m), "r"(axis), "r"(cs) : "memory");
}

// Rotate m's 3x3 part by `angle` about (x, y, z), keeping its translation:
// libvu0's sceVu0RotMatrix-style axis/angle build, then sceVu0MulMatrix's
// loop (m = rot * m), with the translation row parked in vf20 meanwhile.
// (Same as JetskiVehicle/AnimateJetski.cpp's, but computing cos/sin itself:
// that is what loads axis.y/z before the calls and axis.x after.)
static inline void RotateMatrixAxis(_D3DMATRIX *m, float x, float y, float z, float angle)
{
    float c = FastCos(angle);
    float s = FastSin(angle);
    sceVu0FMATRIX rot;
    sceVu0FVECTOR axis = {x, y, z};
    sceVu0FVECTOR cs = {c, s, 1.0f - c, angle};
    asm("lqc2 vf20, 0x30(%0); lqc2 vf1, 0x0(%2); lqc2 vf30, 0x0(%3); vmul.xyz vf2, vf1, vf1; vaddy.x vf2, vf2, vf2y; vaddz.x vf2, vf2, vf2z; vrsqrt Q, vf0w, vf2x; vsub.xyzw vf3, vf0, vf0; vaddw.xyzw vf3, vf3, vf1w; vwaitq; vmulq.xyz vf3, vf1, Q; vmuly.xyz vf4, vf3, vf30y; vmulx.xyz vf11, vf3, vf3x; vmulz.xyz vf11, vf11, vf30z; vaddx.x vf11, vf11, vf30x; vaddz.y vf11, vf11, vf4z; vsuby.z vf11, vf11, vf4y; vsub.w vf11, vf11, vf11; vmuly.xyz vf12, vf3, vf3y; vmulz.xyz vf12, vf12, vf30z; vsubz.x vf12, vf12, vf4z; vaddx.y vf12, vf12, vf30x; vaddx.z vf12, vf12, vf4x; vsub.w vf12, vf12, vf12; vmulz.xyz vf13, vf3, vf3z; vmulz.xyz vf13, vf13, vf30z; vaddy.x vf13, vf13, vf4y; vsubx.y vf13, vf13, vf4x; vaddx.z vf13, vf13, vf30x; vsub.w vf13, vf13, vf13; sqc2 vf11, 0x0(%1); sqc2 vf12, 0x10(%1); sqc2 vf13, 0x20(%1); sqc2 vf0, 0x30(%1)"
        : : "r"(m), "r"(rot), "r"(axis), "r"(cs) : "memory");
    // mwcc's assembler fills the bne delay slot itself with the addi before it.
    asm("lqc2 vf4, 0x0(%0); lqc2 vf5, 0x10(%0); lqc2 vf6, 0x20(%0); lqc2 vf7, 0x30(%0); addi $5, %0, 0; addi $6, %1, 0; addi $7, $0, 4; _loopMulMatrix: lqc2 vf8, 0x0($6); vmulax.xyzw ACC, vf4, vf8x; vmadday.xyzw ACC, vf5, vf8y; vmaddaz.xyzw ACC, vf6, vf8z; vmaddw.xyzw vf9, vf7, vf8w; sqc2 vf9, 0x0($5); addi $7, $7, -1; addi $6, $6, 0x10; addi $5, $5, 0x10; bne $0, $7, _loopMulMatrix; sqc2 vf20, 0x30(%0)"
        : : "r"(m), "r"(rot) : "$5", "$6", "$7", "memory");
}

// ---- src/SoultreeObject/ctor.cpp



extern "C" int strcasecmp(const char *a, const char *b);


struct Quaternion;
class SearchNode;
class ParameterBlock;

_D3DMATRIX IdentityMatrix();
class SearchableObject;

// SoultreeObject's first base: 8 bytes in front of its GameObject subobject.




extern DataAccess *LpDataAccess;


extern "C" void *memset(void *dst, int c, unsigned int n);

extern TypeRegistry *LpTypeRegistry;

SoultreeObject::SoultreeObject(int enabled) : GameObject(enabled)
{
    m_parent = 0;
    m_child = 0;
    m_prevSibling = 0;
    m_sibling = 0;
    memset(&m_localMatrix, 0, sizeof(m_localMatrix));
    m_localMatrix._11 = m_localMatrix._22 = m_localMatrix._33 = m_localMatrix._44 = 1.0f;
    memset(&m_worldMatrix, 0, sizeof(m_worldMatrix));
    m_worldMatrixValid = 0;
    m_updateGeometry = 1;
    m_searchPlacement = 0;
    m_hidden = 0;
    m_hiddenWhenEnabled = 0;
    m_x190_bit2 = 0;
    m_x190_bit3 = 0;
    m_x190_bit4 = 0;
    m_boundingBoxValid = 0;
    m_boundingBoxMin.x = 0.0f;
    m_boundingBoxMin.y = 0.0f;
    m_boundingBoxMin.z = 0.0f;
    m_boundingBoxMax.x = 0.0f;
    m_boundingBoxMax.y = 0.0f;
    m_boundingBoxMax.z = 0.0f;
    m_x190_bit6 = 0;
    m_localCenter.x = 0.0f;
    m_localCenter.y = 0.0f;
    m_localCenter.z = 0.0f;
    m_localHalf.x = 0.0f;
    m_localHalf.y = 0.0f;
    m_localHalf.z = 0.0f;
    m_name[0] = 0;
    m_numObjects = 0;
    m_typeID = LpTypeRegistry->GetTypeID("SoultreeObject");
    m_28 = m_typeID;
    m_visible = 1;
    m_x191_bit1 = 0;
    m_paramBlock = 0;
}


// ---- src/SoultreeObject/dtor.cpp

SoultreeObject::~SoultreeObject()
{
    if (m_parent)
        m_parent->RemoveSoultreeChild(this);
    if (m_child) {
        while (m_child->m_sibling)
            m_child->m_sibling->Release();
        m_child->Release();
    }
    if (m_searchPlacement)
        delete m_searchPlacement;
}


// ---- src/SoultreeObject/ValidateWorldMatrix.cpp

void SoultreeObject::ValidateWorldMatrix()
{
    if (!m_worldMatrixValid) {
        if (m_parent) {
            m_parent->ValidateWorldMatrix();
            asm("lqc2 vf5, 0x0(%1); lqc2 vf1, 0x0(%0); lqc2 vf2, 0x10(%0); lqc2 vf3, 0x20(%0); lqc2 vf4, 0x30(%0); lqc2 vf6, 0x10(%1); vmulax.xyzw ACC, vf1, vf5x; vmadday.xyzw ACC, vf2, vf5y; vmaddaz.xyzw ACC, vf3, vf5z; vmaddw.xyzw vf9, vf4, vf5w; lqc2 vf7, 0x20(%1); vmulax.xyzw ACC, vf1, vf6x; vmadday.xyzw ACC, vf2, vf6y; vmaddaz.xyzw ACC, vf3, vf6z; vmaddw.xyzw vf10, vf4, vf6w; lqc2 vf8, 0x30(%1); vmulax.xyzw ACC, vf1, vf7x; vmadday.xyzw ACC, vf2, vf7y; vmaddaz.xyzw ACC, vf3, vf7z; vmaddw.xyzw vf11, vf4, vf7w; vmulax.xyzw ACC, vf1, vf8x; vmadday.xyzw ACC, vf2, vf8y; vmaddaz.xyzw ACC, vf3, vf8z; vmaddw.xyzw vf12, vf4, vf8w; sqc2 vf9, 0x0(%2); sqc2 vf10, 0x10(%2); sqc2 vf11, 0x20(%2); sqc2 vf12, 0x30(%2)"
                : : "r"(&m_parent->m_worldMatrix), "r"(&m_localMatrix), "r"(&m_worldMatrix) : "memory");
        } else {
            m_worldMatrix = m_localMatrix;
        }
        m_worldMatrixValid = 1;
    }
}


// ---- src/SoultreeObject/InvalidateWorldMatrix.cpp

void SoultreeObject::InvalidateWorldMatrix()
{
    SetUpdateGeometry();
    if (m_worldMatrixValid) {
        m_worldMatrixValid = 0;
        if (m_child)
            m_child->InvalidateChildWorldMatrix();
    }
}


// ---- src/SoultreeObject/InvalidateChildWorldMatrix.cpp

void SoultreeObject::InvalidateChildWorldMatrix()
{
    if (m_sibling)
        m_sibling->InvalidateChildWorldMatrix();
    if (m_worldMatrixValid) {
        m_worldMatrixValid = 0;
        if (m_child)
            m_child->InvalidateChildWorldMatrix();
    }
}


// ---- src/SoultreeObject/SetMatrix_00291AF0.cpp

// Set the local matrix from a matrix given relative to another object (or
// in world space when relativeTo is null). m is overwritten.
void SoultreeObject::SetMatrix(SoultreeObject *relativeTo, _D3DMATRIX *m)
{
    if (relativeTo == this) {
        SetMatrix(m);
        return;
    }
    if (relativeTo) {
        relativeTo->ValidateWorldMatrix();
        MulMatrixCopy(m, relativeTo->m_worldMatrix, *m);
    }
    if (m_parent) {
        m_parent->ValidateWorldMatrix();
        MulInverseMatrix(&m_localMatrix, *m, m_parent->m_worldMatrix);
    } else {
        m_localMatrix = *m;
    }
    InvalidateWorldMatrix();
}


// ---- src/SoultreeObject/SetOrientation_00291E40.cpp

// Point the object along front with the given up vector, keeping its
// position. Unless the caller says the axes are already orthonormal
// (orthonormal set, mode 0), VU0 re-derives and normalises them.
void SoultreeObject::SetOrientation(_D3DVECTOR *front, _D3DVECTOR *up, int mode, int keep, int orthonormal)
{
    static int SetOrientationID = -1; // profiler ID (unused)
    // Emits nothing, but gives mwcc's block optimizer the event retail's
    // delay-slot fills need.
    if (orthonormal) orthonormal = orthonormal; else orthonormal = orthonormal;
    if (!mode && orthonormal) {
        Vu0SetAxes(&m_localMatrix, front, up);
    } else {
        _D3DVECTOR pos;
        pos.x = m_localMatrix._41;
        pos.y = m_localMatrix._42;
        pos.z = m_localMatrix._43;
        Vu0OrientAxes(&m_localMatrix, &pos, front, up, mode, keep);
    }
    InvalidateWorldMatrix();
}


// ---- src/SoultreeObject/SetOrientation_00292020.cpp

// Point the object along front with the given up vector, both given
// relative to relativeTo (world space when it is null), keeping its
// position. The axes are built on VU0 as in the other overload.
void SoultreeObject::SetOrientation(SoultreeObject *relativeTo, _D3DVECTOR *front, _D3DVECTOR *up, int mode, int keep, int orthonormal)
{
    static int SetOrientationID = -1; // profiler ID (unused)
    // Emits nothing, but gives mwcc's block optimizer the event retail's
    // delay-slot fills need.
    if (orthonormal) orthonormal = orthonormal; else orthonormal = orthonormal;
    _D3DVECTOR pos;
    _D3DMATRIX m;
    pos.x = m_localMatrix._41;
    pos.y = m_localMatrix._42;
    pos.z = m_localMatrix._43;
    if (!mode && orthonormal) {
        // Through a pointer: retail computes &m before &pos.
        _D3DMATRIX *pm = &m;
        Vu0SetAxes(pm, &pos, front, up);
    } else {
        Vu0OrientAxes(&m, &pos, front, up, mode, keep);
    }
    if (relativeTo == this) {
        MulMatrixCopy(&m_localMatrix, m_localMatrix, m);
        InvalidateWorldMatrix();
        return;
    }
    if (relativeTo) {
        relativeTo->ValidateWorldMatrix();
        MulMatrixCopy(&m, relativeTo->m_worldMatrix, m);
    }
    if (m_parent) {
        m_parent->ValidateWorldMatrix();
        MulInverseMatrix(&m_localMatrix, m, m_parent->m_worldMatrix);
    } else {
        m_localMatrix = m;
    }
    m_localMatrix._41 = pos.x;
    m_localMatrix._42 = pos.y;
    m_localMatrix._43 = pos.z;
    InvalidateWorldMatrix();
}


// ---- src/SoultreeObject/GetOrientation.cpp

// The local front (third row) and up (second row) axes.
void SoultreeObject::GetOrientation(_D3DVECTOR *front, _D3DVECTOR *up)
{
    if (front) {
        front->x = m_localMatrix._31;
        front->y = m_localMatrix._32;
        front->z = m_localMatrix._33;
    }
    if (up) {
        up->x = m_localMatrix._21;
        up->y = m_localMatrix._22;
        up->z = m_localMatrix._23;
    }
}

// ---- src/SoultreeObject/SetOrientation.cpp


// Same shape as ComputeMatrix (src/global/ComputeMatrix.cpp): quaternion
// fields read in place, products in retail order.
void SoultreeObject::SetOrientation(Quaternion *q)
{
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
    float len2;
    len2 = q->w * q->w + q->x * q->x + q->y * q->y + q->z * q->z;
    if (len2 > 0.0f) {
        s = 2.0f / len2;
        ys = q->y * s;
        xs = q->x * s;
        zs = q->z * s;
        zz = q->z * zs;
        wx = q->w * xs;
        xx = q->x * xs;
        wy = q->w * ys;
        xy = q->x * ys;
        wz = q->w * zs;
        xz = q->x * zs;
        yz = q->y * zs;
        yy = q->y * ys;
        m_localMatrix._11 = 1.0f - (yy + zz);
        m_localMatrix._12 = xy + wz;
        m_localMatrix._13 = xz - wy;
        m_localMatrix._14 = 0.0f;
        m_localMatrix._21 = xy - wz;
        m_localMatrix._22 = 1.0f - (xx + zz);
        m_localMatrix._23 = yz + wx;
        m_localMatrix._24 = 0.0f;
        m_localMatrix._31 = xz + wy;
        m_localMatrix._32 = yz - wx;
        m_localMatrix._33 = 1.0f - (xx + yy);
        m_localMatrix._34 = 0.0f;
    } else {
        m_localMatrix._11 = 1.0f;
        m_localMatrix._12 = 0.0f;
        m_localMatrix._13 = 0.0f;
        m_localMatrix._14 = 0.0f;
        m_localMatrix._21 = 0.0f;
        m_localMatrix._22 = 1.0f;
        m_localMatrix._23 = 0.0f;
        m_localMatrix._24 = 0.0f;
        m_localMatrix._31 = 0.0f;
        m_localMatrix._32 = 0.0f;
        m_localMatrix._33 = 1.0f;
        m_localMatrix._34 = 0.0f;
    }
    InvalidateWorldMatrix();
}


// ---- src/SoultreeObject/GetOrientation_00292770.cpp

void SoultreeObject::GetOrientation(SoultreeObject *relativeTo, _D3DVECTOR *front, _D3DVECTOR *up)
{
    if (relativeTo == this) {
        if (front) {
            front->x = 0.0f;
            front->y = 0.0f;
            front->z = 1.0f;
        }
        if (up) {
            up->x = 0.0f;
            up->y = 1.0f;
            up->z = 0.0f;
        }
    } else {
        ValidateWorldMatrix();
        if (front) {
            front->x = m_worldMatrix._31;
            front->y = m_worldMatrix._32;
            front->z = m_worldMatrix._33;
        }
        if (up) {
            up->x = m_worldMatrix._21;
            up->y = m_worldMatrix._22;
            up->z = m_worldMatrix._23;
        }
        if (relativeTo) {
            if (front)
                *front = relativeTo->RotateVectorWorldToLocal(front);
            if (up)
                *up = relativeTo->RotateVectorWorldToLocal(up);
        }
    }
}


// ---- src/SoultreeObject/GetOrientation_002928B0.cpp


static inline float Sqrt(float x)
{
    float r;
    asm("sqrt.s $f8, %1; swc1 $f8, 0(%0)" : : "r"(&r), "f"(x) : "$f8", "memory");
    return r;
}

// Rotation part of the local matrix as a quaternion, picking the largest
// component to divide by (same shape as ComputeQuaternion, src/global/).
void SoultreeObject::GetOrientation(Quaternion *q)
{
    float t[4];
    float w2;
    float x2;
    float y2;
    float z2;
    w2 = 0.25f * (1.0f + m_localMatrix._11 + m_localMatrix._22 + m_localMatrix._33);
    x2 = 0.25f * (1.0f + m_localMatrix._11 - m_localMatrix._22 - m_localMatrix._33);
    y2 = 0.25f * (1.0f - m_localMatrix._11 + m_localMatrix._22 - m_localMatrix._33);
    z2 = 0.25f * (1.0f - m_localMatrix._11 - m_localMatrix._22 + m_localMatrix._33);
    t[0] = w2;
    t[1] = x2;
    t[2] = y2;
    t[3] = z2;

    if (t[0] > t[1] && t[0] > t[2] && t[0] > t[3]) {
        q->w = Sqrt(w2);
        float s = 0.25f / q->w;
        q->x = s * (m_localMatrix._23 - m_localMatrix._32);
        q->y = s * (m_localMatrix._31 - m_localMatrix._13);
        q->z = s * (m_localMatrix._12 - m_localMatrix._21);
    } else if (t[1] > t[2] && t[1] > t[3]) {
        q->x = Sqrt(x2);
        float s = 0.25f / q->x;
        q->w = s * (m_localMatrix._23 - m_localMatrix._32);
        q->y = s * (m_localMatrix._12 + m_localMatrix._21);
        q->z = s * (m_localMatrix._13 + m_localMatrix._31);
    } else if (t[2] > t[3]) {
        q->y = Sqrt(y2);
        float s = 0.25f / q->y;
        q->w = s * (m_localMatrix._31 - m_localMatrix._13);
        q->x = s * (m_localMatrix._12 + m_localMatrix._21);
        q->z = s * (m_localMatrix._23 + m_localMatrix._32);
    } else {
        q->z = Sqrt(z2);
        float s = 0.25f / q->z;
        q->w = s * (m_localMatrix._12 - m_localMatrix._21);
        q->x = s * (m_localMatrix._13 + m_localMatrix._31);
        q->y = s * (m_localMatrix._23 + m_localMatrix._32);
    }
}


// ---- src/SoultreeObject/SetPosition_00292B40.cpp

void SoultreeObject::SetPosition(float x, float y, float z)
{
    m_localMatrix._41 = x;
    m_localMatrix._42 = y;
    m_localMatrix._43 = z;
    InvalidateWorldMatrix();
}


// ---- src/SoultreeObject/SetPosition.cpp

void SoultreeObject::SetPosition(_D3DVECTOR position)
{
    m_localMatrix._41 = position.x;
    m_localMatrix._42 = position.y;
    m_localMatrix._43 = position.z;
    InvalidateWorldMatrix();
}


// ---- src/SoultreeObject/SetPosition_00292BB0.cpp

void SoultreeObject::SetPosition(_D3DVECTOR *position)
{
    m_localMatrix._41 = position->x;
    m_localMatrix._42 = position->y;
    m_localMatrix._43 = position->z;
    InvalidateWorldMatrix();
}


// ---- src/SoultreeObject/SetPosition_00292BD0.cpp

// Set the position, given relative to another object (or move by it, when
// relative to this one).
void SoultreeObject::SetPosition(SoultreeObject *relativeTo, _D3DVECTOR position)
{
    // Never read; probably a profiler ID.
    static int SetPositionID = -1;
    if (relativeTo == this) {
        // m_localMatrix row 4 (the position) += position, xyz only.
        asm("lqc2 vf1, 0x0(%1); lqc2 vf2, 0x30(%0); vadd.xyz vf3, vf1, vf2; sqc2 vf3, 0x30(%0)" : : "r"(&m_localMatrix), "r"(&position) : "memory");
        InvalidateWorldMatrix();
    } else {
        if (relativeTo) {
            position = relativeTo->TransformVectorLocalToWorld(&position);
            position = m_parent->TransformVectorWorldToLocal(&position);
        }
        SetPosition(&position);
    }
}


// ---- src/SoultreeObject/SetPosition_00292CD0.cpp

// Set the position, given relative to another object (or move by it, when
// relative to this one). A null relativeTo means world space.
void SoultreeObject::SetPosition(SoultreeObject *relativeTo, _D3DVECTOR *position)
{
    // Never read; probably a profiler ID.
    static int SetPositionID = -1;
    if (relativeTo == this) {
        // m_localMatrix row 4 (the position) += *position, xyz only.
        asm("lqc2 vf1, 0x0(%1); lqc2 vf2, 0x30(%0); vadd.xyz vf3, vf1, vf2; sqc2 vf3, 0x30(%0)" : : "r"(&m_localMatrix), "r"(position) : "memory");
        InvalidateWorldMatrix();
    } else if (relativeTo) {
        _D3DVECTOR local;
        local = relativeTo->TransformVectorLocalToWorld(position);
        local = m_parent->TransformVectorWorldToLocal(&local);
        SetPosition(&local);
    } else {
        _D3DVECTOR local;
        if (m_parent)
            local = m_parent->TransformVectorWorldToLocal(position);
        else
            local = *position;
        SetPosition(&local);
    }
}


// ---- src/SoultreeObject/Translate_00292E40.cpp

void SoultreeObject::Translate(_D3DVECTOR *offset)
{
    m_localMatrix._41 += offset->x;
    m_localMatrix._42 += offset->y;
    m_localMatrix._43 += offset->z;
    InvalidateWorldMatrix();
}


// ---- src/SoultreeObject/Translate.cpp

void SoultreeObject::Translate(_D3DVECTOR offset)
{
    m_localMatrix._41 += offset.x;
    m_localMatrix._42 += offset.y;
    m_localMatrix._43 += offset.z;
    InvalidateWorldMatrix();
}


// ---- src/SoultreeObject/Translate_00292EF0.cpp

// Move by offset, given in relativeTo's frame (world space when null).
void SoultreeObject::Translate(SoultreeObject *relativeTo, _D3DVECTOR *offset)
{
    if (relativeTo == this) {
        Translate(offset);
        return;
    }
    if (relativeTo) {
        _D3DVECTOR v;
        v = relativeTo->RotateVectorLocalToWorld(offset);
        v = m_parent->RotateVectorWorldToLocal(&v);
        Translate(&v);
    } else {
        _D3DVECTOR v;
        if (m_parent)
            v = m_parent->TransformVectorWorldToLocal(offset);
        else
            v = *offset;
        Translate(&v);
    }
}


// ---- src/SoultreeObject/GetPosition_00293060.cpp

void SoultreeObject::GetPosition(_D3DVECTOR *position)
{
    position->x = m_localMatrix._41;
    position->y = m_localMatrix._42;
    position->z = m_localMatrix._43;
}


// ---- src/SoultreeObject/GetPosition.cpp

void SoultreeObject::GetPosition(SoultreeObject *relativeTo, _D3DVECTOR *position)
{
    if (relativeTo == this) {
        position->x = m_localMatrix._41;
        position->y = m_localMatrix._42;
        position->z = m_localMatrix._43;
        return;
    }
    ValidateWorldMatrix();
    position->x = m_worldMatrix._41;
    position->y = m_worldMatrix._42;
    position->z = m_worldMatrix._43;
    if (relativeTo)
        *position = relativeTo->TransformVectorWorldToLocal(position);
}


// ---- src/SoultreeObject/GetPosition_00293130.cpp

_D3DVECTOR SoultreeObject::GetPosition(SoultreeObject *relativeTo)
{
    if (relativeTo == this)
        return _D3DVECTOR(m_localMatrix._41, m_localMatrix._42, m_localMatrix._43);
    _D3DVECTOR position;
    ValidateWorldMatrix();
    position.x = m_worldMatrix._41;
    position.y = m_worldMatrix._42;
    position.z = m_worldMatrix._43;
    if (relativeTo)
        position = relativeTo->TransformVectorWorldToLocal(&position);
    return position;
}


// ---- src/SoultreeObject/SetMatrix.cpp

void SoultreeObject::SetMatrix(_D3DMATRIX matrix)
{
    m_localMatrix = matrix;
    InvalidateWorldMatrix();
}


// ---- src/SoultreeObject/SetMatrix_00293300.cpp

void SoultreeObject::SetMatrix(_D3DMATRIX *matrix)
{
    m_localMatrix = *matrix;
    InvalidateWorldMatrix();
}


// ---- src/SoultreeObject/GetMatrix.cpp

void SoultreeObject::GetMatrix(_D3DMATRIX *matrix)
{
    *matrix = m_localMatrix;
}


// ---- src/SoultreeObject/GetMatrix_00293420.cpp

extern "C" void *memset(void *, int, unsigned int);

// The world matrix, or relative to another object (identity for this one).
// Retail actually goes through relativeTo's parent's world matrix.
void SoultreeObject::GetMatrix(SoultreeObject *relativeTo, _D3DMATRIX *m)
{
    if (relativeTo == this) {
        memset(m, 0, sizeof(*m));
        m->_11 = m->_22 = m->_33 = m->_44 = 1.0f;
        return;
    }
    ValidateWorldMatrix();
    *m = m_worldMatrix;
    if (relativeTo) {
        if (relativeTo->m_parent) {
            relativeTo->m_parent->ValidateWorldMatrix();
            MulInverseMatrix(m, *m, relativeTo->m_parent->m_worldMatrix);
        } else {
            *m = *m;
        }
    }
}


// ---- src/SoultreeObject/ResetMatrices.cpp

void SoultreeObject::ResetMatrices()
{
    m_worldMatrixValid = 0;
    m_localMatrix = IdentityMatrix();
    m_worldMatrix = IdentityMatrix();
    if (m_sibling)
        m_sibling->ResetMatrices();
    if (m_child)
        m_child->ResetMatrices();
}


// ---- src/SoultreeObject/AddRotation.cpp



// Rotate the local matrix by `angle` about `axis`, in place (the position is
// kept).
void SoultreeObject::AddRotation(_D3DVECTOR axis, float angle)
{
    RotateMatrixAxis(&m_localMatrix, axis.x, axis.y, axis.z, angle);
    InvalidateWorldMatrix();
}


// ---- src/SoultreeObject/SetRotation_00293A60.cpp



void SoultreeObject::SetRotation(float x, float y, float z, float angle)
{
    _D3DVECTOR position;
    GetPosition(&position);
    float c = FastCos(angle);
    float s = FastSin(angle);
    BuildRotationMatrix(&m_localMatrix, x, y, z, c, s, angle);
    SetPosition(&position);
}


// ---- src/SoultreeObject/SetRotation.cpp

void SoultreeObject::SetRotation(_D3DVECTOR axis, float angle)
{
    SetRotation(axis.x, axis.y, axis.z, angle);
}


// ---- src/SoultreeObject/AddScale.cpp

extern "C" void *memset(void *, int, unsigned int);

// Scale the local matrix (about its own origin: the position is kept).
void SoultreeObject::AddScale(float sx, float sy, float sz)
{
    _D3DMATRIX scale;
    _D3DVECTOR position;
    position.x = m_localMatrix._41;
    position.y = m_localMatrix._42;
    position.z = m_localMatrix._43;
    memset(&scale, 0, sizeof(scale));
    scale._11 = scale._22 = scale._33 = scale._44 = 1.0f;
    scale._11 = sx;
    scale._22 = sy;
    scale._33 = sz;
    MulMatrixCopy(&m_localMatrix, m_localMatrix, scale);
    m_localMatrix._41 = position.x;
    m_localMatrix._42 = position.y;
    m_localMatrix._43 = position.z;
    InvalidateWorldMatrix();
}


// ---- src/SoultreeObject/SetScale.cpp

static inline void Normalize(_D3DVECTOR *v)
{
    asm("lqc2 vf4, 0x0(%0); vmul.xyz vf5, vf4, vf4; vaddy.x vf5, vf5, vf5y; vaddz.x vf5, vf5, vf5z; vrsqrt Q, vf0w, vf5x; vsub.xyzw vf6, vf0, vf0; vaddw.xyzw vf6, vf6, vf4w; vwaitq; vmulq.xyz vf6, vf4, Q; sqc2 vf6, 0x0(%0)"
        : : "r"(v) : "memory");
}

void SoultreeObject::SetScale(float sx, float sy, float sz)
{
    _D3DVECTOR v;
    v.x = m_localMatrix._11; v.y = m_localMatrix._12; v.z = m_localMatrix._13;
    Normalize(&v);
    v.x *= sx; v.y *= sx; v.z *= sx;
    m_localMatrix._11 = v.x; m_localMatrix._12 = v.y; m_localMatrix._13 = v.z;
    v.x = m_localMatrix._21; v.y = m_localMatrix._22; v.z = m_localMatrix._23;
    Normalize(&v);
    v.x *= sy; v.y *= sy; v.z *= sy;
    m_localMatrix._21 = v.x; m_localMatrix._22 = v.y; m_localMatrix._23 = v.z;
    v.x = m_localMatrix._31; v.y = m_localMatrix._32; v.z = m_localMatrix._33;
    Normalize(&v);
    v.x *= sz; v.y *= sz; v.z *= sz;
    m_localMatrix._31 = v.x; m_localMatrix._32 = v.y; m_localMatrix._33 = v.z;
    InvalidateWorldMatrix();
}


// ---- src/SoultreeObject/RotateVectorLocalToWorld.cpp

_D3DVECTOR SoultreeObject::RotateVectorLocalToWorld(_D3DVECTOR *v)
{
    _D3DVECTOR result;
    ValidateWorldMatrix();
    asm("lqc2 vf4, 0x0(%0); lqc2 vf5, 0x10(%0); lqc2 vf6, 0x20(%0); lqc2 vf8, 0x0(%1); vmulax.xyz ACC, vf4, vf8x; vmadday.xyz ACC, vf5, vf8y; vmaddz.xyz vf9, vf6, vf8z; vadd.xyzw vf9, vf9, vf0; sqc2 vf9, 0x0(%2)"
        : : "r"(&m_worldMatrix), "r"(v), "r"(&result) : "memory");
    return result;
}


// ---- src/SoultreeObject/TransformVectorLocalToWorld.cpp

_D3DVECTOR SoultreeObject::TransformVectorLocalToWorld(_D3DVECTOR *v)
{
    _D3DVECTOR result;
    ValidateWorldMatrix();
    asm("lqc2 vf4, 0x0(%0); lqc2 vf5, 0x10(%0); lqc2 vf6, 0x20(%0); lqc2 vf7, 0x30(%0); lqc2 vf8, 0x0(%1); vmulw.w vf9, vf0, vf0w; vmulax.xyz ACC, vf4, vf8x; vmadday.xyz ACC, vf5, vf8y; vmaddz.xyz vf9, vf6, vf8z; vadd.xyz vf9, vf9, vf7; sqc2 vf9, 0x0(%2)"
        : : "r"(&m_worldMatrix), "r"(v), "r"(&result) : "memory");
    return result;
}


// ---- src/SoultreeObject/RotateVectorWorldToLocal.cpp

static inline void InvertOrthonormalMatrix(_D3DMATRIX *out, _D3DMATRIX *in)
{
    asm("lqc2 vf1, 0x0(%1); lqc2 vf2, 0x10(%1); lqc2 vf3, 0x20(%1); vsub.xyzw vf30, vf30, vf30; vadd.xyzw vf4, vf1, vf30; vadd.xyzw vf5, vf2, vf30; vadd.xyzw vf6, vf3, vf30; vaddy.x vf5, vf0, vf1y; vaddz.x vf6, vf0, vf1z; vaddz.y vf6, vf0, vf2z; vaddx.y vf4, vf0, vf2x; vaddx.z vf4, vf0, vf3x; vaddy.z vf5, vf0, vf3y; lqc2 vf10, 0x30(%1); vsub.xyzw vf11, vf11, vf11; vadd.xyzw vf11, vf10, vf11; vmulax.xyz ACC, vf4, vf10x; vmadday.xyz ACC, vf5, vf10y; vmaddz.xyz vf11, vf6, vf10z; vsub.xyzw vf12, vf12, vf12; vsub.w vf12, vf12, vf0; vmulw.xyz vf11, vf11, vf12w; sqc2 vf4, 0x0(%0); sqc2 vf5, 0x10(%0); sqc2 vf6, 0x20(%0); sqc2 vf11, 0x30(%0)"
        : : "r"(out), "r"(in) : "memory");
}

static inline void ApplyMatrixVector(_D3DVECTOR *out, _D3DMATRIX *m, _D3DVECTOR v)
{
    asm("lqc2 vf4, 0x0(%1); lqc2 vf5, 0x10(%1); lqc2 vf6, 0x20(%1); lqc2 vf8, 0x0(%2); vmulax.xyz ACC, vf4, vf8x; vmadday.xyz ACC, vf5, vf8y; vmaddz.xyz vf9, vf6, vf8z; vadd.xyzw vf9, vf9, vf0; sqc2 vf9, 0x0(%0)"
        : : "r"(out), "r"(m), "r"(&v) : "memory");
}

_D3DVECTOR SoultreeObject::RotateVectorWorldToLocal(_D3DVECTOR *v)
{
    _D3DVECTOR result;
    ValidateWorldMatrix();
    _D3DMATRIX inv;
    inv = m_worldMatrix;
    InvertOrthonormalMatrix(&inv, &inv);
    ApplyMatrixVector(&result, &inv, *v);
    return result;
}


// ---- src/SoultreeObject/TransformVectorWorldToLocal.cpp



static inline void ApplyMatrixPoint(_D3DVECTOR *out, _D3DMATRIX *m, _D3DVECTOR *v)
{
    asm("lqc2 vf4, 0x0(%1); lqc2 vf5, 0x10(%1); lqc2 vf6, 0x20(%1); lqc2 vf7, 0x30(%1); lqc2 vf8, 0x0(%2); vmulw.w vf9, vf0, vf0w; vmulax.xyz ACC, vf4, vf8x; vmadday.xyz ACC, vf5, vf8y; vmaddz.xyz vf9, vf6, vf8z; vadd.xyz vf9, vf9, vf7; sqc2 vf9, 0x0(%0)"
        : : "r"(out), "r"(m), "r"(v) : "memory");
}

_D3DVECTOR SoultreeObject::TransformVectorWorldToLocal(_D3DVECTOR *v)
{
    _D3DVECTOR result;
    ValidateWorldMatrix();
    _D3DMATRIX inv;
    inv = m_worldMatrix;
    InvertOrthonormalMatrix(&inv, &inv);
    ApplyMatrixPoint(&result, &inv, v);
    return result;
}


// ---- src/SoultreeObject/AddSoultreeChild.cpp

void SoultreeObject::AddSoultreeChild(SoultreeObject *child)
{
    // Retail tests the child's parent but detaches it via this object's own parent.
    if (child->m_parent)
        m_parent->RemoveSoultreeChild(child);
    child->m_parent = this;
    if (!m_child)
        m_child = child;
    else
        m_child->AddSoultreeSibling(child);
}


// ---- src/SoultreeObject/AddSoultreeSibling.cpp

void SoultreeObject::AddSoultreeSibling(SoultreeObject *sibling)
{
    if (!m_sibling) {
        m_sibling = sibling;
        sibling->m_prevSibling = this;
    } else {
        m_sibling->AddSoultreeSibling(sibling);
    }
}


// ---- src/SoultreeObject/RemoveSoultreeChild.cpp

void SoultreeObject::RemoveSoultreeChild(SoultreeObject *child)
{
    child->InvalidateWorldMatrix();
    SoultreeObject *c = m_child;
    while (c != child)
        c = c->m_sibling;
    if (c == m_child)
        m_child = c->m_sibling;
    if (c->m_sibling)
        c->m_sibling->m_prevSibling = c->m_prevSibling;
    if (c->m_prevSibling)
        c->m_prevSibling->m_sibling = c->m_sibling;
    if (c->m_parent)
        c->m_parent = 0;
    if (c->m_sibling)
        c->m_sibling = 0;
    if (c->m_prevSibling)
        c->m_prevSibling = 0;
}


// ---- src/SoultreeObject/NumberOfObjectsInHierarchy.cpp

int SoultreeObject::NumberOfObjectsInHierarchy()
{
    int count = 0;
    if (m_sibling)
        count += m_sibling->NumberOfObjectsInHierarchy();
    if (m_child)
        count += m_child->NumberOfObjectsInHierarchy();
    return count + 1;
}


// ---- src/SoultreeObject/FillObjectList.cpp

void SoultreeObject::FillObjectList(int *count, SoultreeObject **list)
{
    SoultreeObject *child;
    child = m_child;
    while (child) {
        list[*count] = child;
        child = child->m_sibling;
        (*count)++;
    }
    for (child = m_child; child; child = child->m_sibling)
        child->FillObjectList(count, list);
}


// ---- src/SoultreeObject/SetUpdateGeometry.cpp

void SoultreeObject::SetUpdateGeometry()
{
    m_updateGeometry = 1;
    if (m_parent)
        m_parent->SetUpdateGeometry();
}


// ---- src/SoultreeObject/GetNamedObject.cpp

SoultreeObject *SoultreeObject::GetNamedObject(char *name)
{
    SoultreeObject *found = 0;
    if (strcasecmp(m_name, name) == 0)
        return this;
    if (m_sibling)
        found = m_sibling->GetNamedObject(name);
    if (found)
        return found;
    if (m_child)
        found = m_child->GetNamedObject(name);
    return found;
}


// ---- src/SoultreeObject/Hide_00294730.cpp

void SoultreeObject::Hide()
{
    m_hidden = 1;
}


// ---- src/SoultreeObject/Unhide.cpp

void SoultreeObject::Unhide()
{
    m_hidden = 0;
}


// ---- src/SoultreeObject/ReadFromBinaryFile.cpp



static inline float Magnitude(_D3DVECTOR *v)
{
    return Sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
}

// Reads the hierarchy's objects (this is the first) from a binary file, then
// links each to its parent by index.
void SoultreeObject::ReadFromBinaryFile(long file)
{
    SoultreeObject *objects[256];
    int flag;
    int i;
    LpDataAccess->FRead(file, &m_numObjects, 4, 1);
    objects[0] = this;
    for (i = 0; i < m_numObjects; i++) {
        if (i)
            CreateNewObject(&objects[i]);
        LpDataAccess->FRead(file, objects[i]->m_name, 0x80, 1);
        LpDataAccess->FRead(file, &objects[i]->m_localMatrix, 0x40, 1);
        LpDataAccess->FRead(file, &flag, 4, 1);
        objects[i]->m_hidden = flag;
        LpDataAccess->FRead(file, &flag, 4, 1);
        objects[i]->m_boundingBoxValid = flag;
        LpDataAccess->FRead(file, &objects[i]->m_boundingBoxMin, 0xC, 1);
        LpDataAccess->FRead(file, &objects[i]->m_boundingBoxMax, 0xC, 1);
        LpDataAccess->FRead(file, &flag, 4, 1);
        objects[i]->m_x190_bit6 = flag;
        LpDataAccess->FRead(file, &objects[i]->m_localCenter, 0xC, 1);
        LpDataAccess->FRead(file, &objects[i]->m_localHalf, 0xC, 1);
        objects[i]->m_x190_bit6 = 0;
        objects[i]->m_boundingRadius = Magnitude(&objects[i]->m_boundingBoxMax);
    }
    int *parents = new int[m_numObjects];
    LpDataAccess->FRead(file, parents, 4, m_numObjects);
    for (i = 0; i < m_numObjects; i++) {
        if (parents[i] != -1)
            objects[parents[i]]->AddSoultreeChild(objects[i]);
    }
    if (parents)
        delete[] parents;
}


// ---- src/SoultreeObject/ReadHierarchyFromFile.cpp

// Reads the hierarchy's objects (this is the first), then links each to its
// parent by index. Returns the object count.
int SoultreeObject::ReadHierarchyFromFile(long file, SoultreeObject **objects)
{
    LpDataAccess->FRead(file, &m_numObjects, 4, 1);
    objects[0] = this;
    int i;
    int *parents = new int[m_numObjects];
    for (i = 0; i < m_numObjects; i++) {
        if (i)
            CreateNewObject(&objects[i]);
        LpDataAccess->FRead(file, objects[i]->m_name, 0x80, 1);
        LpDataAccess->FRead(file, &objects[i]->m_localMatrix, 0x40, 1);
        objects[i]->m_hidden = 0;
        objects[i]->m_boundingBoxValid = 1;
        objects[i]->m_x190_bit2 = 1;
        LpDataAccess->FRead(file, &objects[i]->m_boundingBoxMin, 0xC, 1);
        LpDataAccess->FRead(file, &objects[i]->m_boundingBoxMax, 0xC, 1);
        LpDataAccess->FRead(file, &objects[i]->m_boundingRadius, 4, 1);
        LpDataAccess->FRead(file, &parents[i], 4, 1);
    }
    for (i = 0; i < m_numObjects; i++) {
        if (parents[i] != -1)
            objects[parents[i]]->AddSoultreeChild(objects[i]);
    }
    delete[] parents;
    return m_numObjects;
}


// ---- src/SoultreeObject/ReadFromParameterBlockFile.cpp

// Reads the "Object Hierarchy" block: one line per object with its name, its
// parent's name and its local matrix (3x3 plus position), then links each
// object to its parent.
void SoultreeObject::ReadFromParameterBlockFile()
{
    SoultreeObject *objects[256];
    char parents[256][256];
    int i;
    SoultreeObject *parent;
    int count;
    int j;
    parent = m_parent;
    m_parent = 0;
    count = m_paramBlock->GotoDataBlock("Object Hierarchy");
    m_numObjects = count;
    objects[0] = this;
    for (i = 1; i < count; i++)
        CreateNewObject(&objects[i]);
    for (i = 0; i < count; i++) {
        m_paramBlock->ReadDataLine(0);
        m_paramBlock->GetData(0, objects[i]->m_name);
        m_paramBlock->GetData(1, parents[i]);
        m_paramBlock->GetData(2, &objects[i]->m_localMatrix._11);
        m_paramBlock->GetData(3, &objects[i]->m_localMatrix._12);
        m_paramBlock->GetData(4, &objects[i]->m_localMatrix._13);
        m_paramBlock->GetData(5, &objects[i]->m_localMatrix._21);
        m_paramBlock->GetData(6, &objects[i]->m_localMatrix._22);
        m_paramBlock->GetData(7, &objects[i]->m_localMatrix._23);
        m_paramBlock->GetData(8, &objects[i]->m_localMatrix._31);
        m_paramBlock->GetData(9, &objects[i]->m_localMatrix._32);
        m_paramBlock->GetData(10, &objects[i]->m_localMatrix._33);
        m_paramBlock->GetData(11, &objects[i]->m_localMatrix._41);
        m_paramBlock->GetData(12, &objects[i]->m_localMatrix._42);
        m_paramBlock->GetData(13, &objects[i]->m_localMatrix._43);
    }
    for (i = 0; i < count; i++) {
        if (strcasecmp(parents[i], "NONE")) {
            for (j = 0; j < count; j++) {
                if (!strcasecmp(parents[i], objects[j]->m_name))
                    objects[j]->AddSoultreeChild(objects[i]);
            }
        }
    }
    m_parent = parent;
}


// ---- src/SoultreeObject/CreateNewObject.cpp

void SoultreeObject::CreateNewObject(SoultreeObject **slot)
{
    *slot = new SoultreeObject(1);
}

// ---- src/SoultreeObject/GetBoundingBox.cpp

void SoultreeObject::GetBoundingBox(_D3DVECTOR *min, _D3DVECTOR *max)
{
    if (!m_boundingBoxValid)
        CalculateBoundingBox();
    *min = m_boundingBoxMin;
    *max = m_boundingBoxMax;
}


// ---- src/SoultreeObject/CalculateBoundingBox.cpp

void SoultreeObject::CalculateBoundingBox()
{
}


// ---- src/SoultreeObject/UpdateBoundingBox.cpp





// Recomputes the local bounding volume from this object's own box and its
// children: a sphere for objects in a hierarchy, else an axis-aligned box.
void SoultreeObject::UpdateBoundingBox()
{
    SoultreeObject *child;
    if (m_x190_bit4 || m_parent || m_child) {
        static int meterID = -1;          // unused profiler ID
        ValidateWorldMatrix();
        _D3DVECTOR center;
        float radius = 0.0f;
        if (m_x190_bit2) {
            center = TransformVectorLocalToWorld(&m_boundingBoxMin);
            radius = m_boundingRadius;
        }
        for (child = m_child; child; child = child->m_sibling)
            child->AccumulateExtents(&center, &radius);
        m_localCenter = TransformVectorWorldToLocal(&center);
        m_localHalf.x = radius;
        m_localHalf.y = radius;
        m_localHalf.z = radius;
        m_searchRadius = radius;
        if (m_searchRadius > 0.0f)
            m_x190_bit6 = 1;
    } else {
        static int meterID2 = -1;         // unused profiler ID
        ValidateWorldMatrix();
        _D3DVECTOR min, max;
        int count = 0;
        if (m_x190_bit2) {
            count = 1;
            min = m_boundingBoxMin - m_boundingBoxMax;
            max = m_boundingBoxMin + m_boundingBoxMax;
        }
        for (child = m_child; child; child = child->m_sibling)
            child->AccumulateExtents(&count, &min, &max, &m_worldMatrix);
        m_localCenter = 0.5f * (min + max);
        m_localHalf = 0.5f * (max - min);
        m_searchRadius = Magnitude(&m_localHalf);
        if (m_searchRadius > 0.0f)
            m_x190_bit6 = 1;
    }
}


// ---- src/SoultreeObject/AccumulateExtents_002954F0.cpp

static inline float Fabs(float x)
{
    float r;
    asm("abs.s $f8, %1; swc1 $f8, 0(%0)" : : "r"(&r), "f"(x) : "$f8", "memory");
    return r;
}

// Grows a bounding sphere (center, radius) over the visible hierarchy.
void SoultreeObject::AccumulateExtents(_D3DVECTOR *center, float *radius)
{
    if (m_x190_bit2) {
        if (*radius > 0.0f) {
            _D3DVECTOR c;
            c = TransformVectorLocalToWorld(&m_boundingBoxMin);
            float distance = Fabs(c.x - center->x);
            float d = Fabs(c.y - center->y);
            if (d > distance)
                distance = d;
            d = Fabs(c.z - center->z);
            if (d > distance)
                distance = d;
            distance += m_boundingRadius;
            if (distance > *radius)
                *radius = distance;
        } else {
            *radius = m_boundingRadius;
            *center = TransformVectorLocalToWorld(&m_boundingBoxMin);
        }
    }
    for (SoultreeObject *child = m_child; child; child = child->m_sibling)
        child->AccumulateExtents(center, radius);
}


// ---- src/SoultreeObject/AccumulateExtents.cpp



static inline float Fmin(float a, float b)
{
    float r;
    asm("min.s $f8, %1, %2; swc1 $f8, 0(%0)" : : "r"(&r), "f"(a), "f"(b) : "$f8", "memory");
    return r;
}

static inline float Fmax(float a, float b)
{
    float r;
    asm("max.s $f8, %1, %2; swc1 $f8, 0(%0)" : : "r"(&r), "f"(a), "f"(b) : "$f8", "memory");
    return r;
}


// out = inverse(m) * v for a rigid m (v a point). v by value.
static inline void InverseTransformPoint(_D3DVECTOR *out, _D3DMATRIX *m, _D3DVECTOR v)
{
    asm("lqc2 vf1, 0x0(%1); lqc2 vf2, 0x10(%1); lqc2 vf3, 0x20(%1); lqc2 vf4, 0x30(%1); vmulw.w vf21, vf0, vf0w; vsub.xyzw vf30, vf30, vf30; vadd.xyzw vf5, vf1, vf30; vadd.xyzw vf6, vf2, vf30; vadd.xyzw vf7, vf3, vf30; vadd.xyzw vf8, vf4, vf30; vaddy.x vf6, vf0, vf1y; vaddz.x vf7, vf0, vf1z; vaddz.y vf7, vf0, vf2z; vaddx.y vf5, vf0, vf2x; vaddx.z vf5, vf0, vf3x; vaddy.z vf6, vf0, vf3y; lqc2 vf20, 0x0(%2); vsub.xyz vf20, vf20, vf8; vmulax.xyz ACC, vf5, vf20x; vmadday.xyz ACC, vf6, vf20y; vmaddz.xyz vf21, vf7, vf20z; sqc2 vf21, 0x0(%0)"
        : : "r"(out), "r"(m), "r"(&v) : "memory");
}

// out = inverse(m) * v for a rigid m (v a direction). v by value.
static inline void InverseRotate(_D3DVECTOR *out, _D3DMATRIX *m, _D3DVECTOR v)
{
    asm("lqc2 vf1, 0x0(%1); lqc2 vf2, 0x10(%1); lqc2 vf3, 0x20(%1); lqc2 vf4, 0x30(%1); vmulw.w vf21, vf0, vf0w; vsub.xyzw vf30, vf30, vf30; vadd.xyzw vf5, vf1, vf30; vadd.xyzw vf6, vf2, vf30; vadd.xyzw vf7, vf3, vf30; vadd.xyzw vf8, vf4, vf30; vaddy.x vf6, vf0, vf1y; vaddz.x vf7, vf0, vf1z; vaddz.y vf7, vf0, vf2z; vaddx.y vf5, vf0, vf2x; vaddx.z vf5, vf0, vf3x; vaddy.z vf6, vf0, vf3y; lqc2 vf20, 0x0(%2); vmulax.xyz ACC, vf5, vf20x; vmadday.xyz ACC, vf6, vf20y; vmaddz.xyz vf21, vf7, vf20z; sqc2 vf21, 0x0(%0)"
        : : "r"(out), "r"(m), "r"(&v) : "memory");
}

// Grow [min, max] by this hierarchy's bounding boxes, as axis-aligned boxes
// in matrix's space. *count says whether min/max hold anything yet.
void SoultreeObject::AccumulateExtents(int *count, _D3DVECTOR *min, _D3DVECTOR *max, _D3DMATRIX *matrix)
{
    if (m_x190_bit2) {
        _D3DVECTOR center;
        _D3DVECTOR axisX = _D3DVECTOR(m_boundingBoxMax.x, 0.0f, 0.0f);
        _D3DVECTOR axisY = _D3DVECTOR(0.0f, m_boundingBoxMax.y, 0.0f);
        _D3DVECTOR axisZ = _D3DVECTOR(0.0f, 0.0f, m_boundingBoxMax.z);
        center = TransformVectorLocalToWorld(&m_boundingBoxMin);
        axisX = RotateVectorLocalToWorld(&axisX);
        axisY = RotateVectorLocalToWorld(&axisY);
        axisZ = RotateVectorLocalToWorld(&axisZ);
        InverseTransformPoint(&center, matrix, center);
        InverseRotate(&axisX, matrix, axisX);
        InverseRotate(&axisY, matrix, axisY);
        InverseRotate(&axisZ, matrix, axisZ);
        _D3DVECTOR half;
        half.x = Fabs(axisZ.x) + (Fabs(axisX.x) + Fabs(axisY.x));
        half.y = Fabs(axisZ.y) + (Fabs(axisX.y) + Fabs(axisY.y));
        half.z = Fabs(axisZ.z) + (Fabs(axisX.z) + Fabs(axisY.z));
        if (*count) {
            _D3DVECTOR lo = center - half;
            _D3DVECTOR hi = center + half;
            min->x = Fmin(min->x, lo.x);
            min->y = Fmin(min->y, lo.y);
            min->z = Fmin(min->z, lo.z);
            max->x = Fmax(max->x, hi.x);
            max->y = Fmax(max->y, hi.y);
            max->z = Fmax(max->z, hi.z);
        } else {
            *min = center - half;
            *max = center + half;
            *count = 1;
        }
    }
    for (SoultreeObject *child = m_child; child; child = child->m_sibling)
        child->AccumulateExtents(count, min, max, matrix);
}


// ---- src/SoultreeObject/GetHierarchyBoundingBox.cpp

// Returns the hierarchy's bounding box as a local center and half size.
void SoultreeObject::GetHierarchyBoundingBox(_D3DVECTOR *center, _D3DVECTOR *half)
{
    if (m_updateGeometry) {
        if (m_x190_bit3 || !m_x190_bit6)
            UpdateBoundingBox();
        else
            ValidateWorldMatrix();
    }
    if (center)
        *center = m_localCenter;
    if (half)
        *half = m_localHalf;
}


// ---- src/SoultreeObject/GetCenterHalf.cpp



// out = m * (v.xyz, 1): rotate and translate a point. (Operand order (v, m,
// out) makes mwcc compute &v before &m, as retail does here.)
static inline void ApplyMatrixPointVMO(_D3DVECTOR *out, _D3DMATRIX *m, _D3DVECTOR *v)
{
    asm("lqc2 vf4, 0x0(%1); lqc2 vf5, 0x10(%1); lqc2 vf6, 0x20(%1); lqc2 vf7, 0x30(%1); lqc2 vf8, 0x0(%0); vmulw.w vf9, vf0, vf0w; vmulax.xyz ACC, vf4, vf8x; vmadday.xyz ACC, vf5, vf8y; vmaddz.xyz vf9, vf6, vf8z; vadd.xyz vf9, vf9, vf7; sqc2 vf9, 0x0(%2)"
        : : "r"(v), "r"(m), "r"(out) : "memory");
}



// World-space bounding box as a center and an axis-aligned half size.
void SoultreeObject::GetCenterHalf(_D3DVECTOR *center, _D3DVECTOR *half)
{
    if (m_updateGeometry) {
        if (m_x190_bit3 || !m_x190_bit6)
            UpdateBoundingBox();
        else
            ValidateWorldMatrix();
    }
    _D3DVECTOR axes[3];
    axes[0] = _D3DVECTOR(m_localHalf.x, 0.0f, 0.0f);
    axes[1] = _D3DVECTOR(0.0f, m_localHalf.y, 0.0f);
    axes[2] = _D3DVECTOR(0.0f, 0.0f, m_localHalf.z);
    ValidateWorldMatrix();
    ApplyMatrixPointVMO(center, &m_worldMatrix, &m_localCenter);
    ApplyMatrixVector(&axes[0], &m_worldMatrix, axes[0]);
    ApplyMatrixVector(&axes[1], &m_worldMatrix, axes[1]);
    ApplyMatrixVector(&axes[2], &m_worldMatrix, axes[2]);
    half->x = Fabs(axes[2].x) + (Fabs(axes[0].x) + Fabs(axes[1].x));
    half->y = Fabs(axes[2].y) + (Fabs(axes[0].y) + Fabs(axes[1].y));
    half->z = Fabs(axes[2].z) + (Fabs(axes[0].z) + Fabs(axes[1].z));
}


// ---- src/SoultreeObject/Clone.cpp

void SoultreeObject::Clone(SoultreeObject *other)
{
    m_parent = 0;
    CloneObject(other, this);
}


// ---- src/SoultreeObject/CloneObject.cpp

extern "C" char *strcpy(char *dst, const char *src);

// Copies src's own state into dst, then clones src's siblings and children
// into new objects under dst.
void SoultreeObject::CloneObject(SoultreeObject *src, SoultreeObject *dst)
{
    strcpy(dst->m_name, src->m_name);
    dst->m_localMatrix = src->m_localMatrix;
    dst->m_worldMatrixValid = 0;
    dst->m_hidden = src->m_hidden;
    dst->m_x190_bit2 = src->m_x190_bit2;
    dst->m_boundingBoxValid = src->m_boundingBoxValid;
    dst->m_boundingBoxMin = src->m_boundingBoxMin;
    dst->m_boundingBoxMax = src->m_boundingBoxMax;
    dst->m_boundingRadius = src->m_boundingRadius;
    dst->m_localCenter = src->m_localCenter;
    dst->m_localHalf = src->m_localHalf;
    dst->m_searchRadius = src->m_searchRadius;
    dst->m_x190_bit6 = src->m_x190_bit6;
    if (src->m_sibling) {
        CreateNewObject(&dst->m_sibling);
        dst->m_sibling->m_parent = dst->m_parent;
        dst->m_sibling->m_prevSibling = dst;
        CloneObject(src->m_sibling, dst->m_sibling);
    } else {
        dst->m_sibling = 0;
    }
    if (src->m_child) {
        CreateNewObject(&dst->m_child);
        dst->m_child->m_parent = dst;
        CloneObject(src->m_child, dst->m_child);
    } else {
        dst->m_child = 0;
    }
}


// ---- src/SoultreeObject/ResetTransforms.cpp

void SoultreeObject::ResetTransforms()
{
    m_worldMatrixValid = 0;
    m_localMatrix = IdentityMatrix();
    if (m_sibling)
        m_sibling->ResetTransforms();
    if (m_child)
        m_child->ResetTransforms();
}


// ---- src/SoultreeObject/AddSearchPlacement.cpp
#pragma alignlabel off


void SoultreeObject::AddSearchPlacement(SearchNode *node, unsigned int flags)
{
    m_searchPlacement = new SearchPlacement;
#line 1718 "soultree.cpp"
    ASSERT(m_searchPlacement, "AddSearchPlacement: LpSearchPlacement==0");
    _D3DVECTOR center;
    _D3DVECTOR half;
    GetCenterHalf(&center, &half);
    m_searchPlacement->PerformInitialPlacement(this, node, flags, &center, &half, 0.0f);
#line 1726 "soultree.cpp"
    ASSERT(m_searchPlacement->FirstPlacementItem.LpSearchNode != 0, "AddSearchPlacement: FirstPlacementItem.LpSearchNode == 0");
}
#pragma alignlabel reset


// ---- src/SoultreeObject/UpdateSearchPlacement.cpp

void SoultreeObject::UpdateSearchPlacement()
{
    if (m_searchPlacement) {
        _D3DVECTOR center;
        _D3DVECTOR half;
        GetCenterHalf(&center, &half);
        m_searchPlacement->UpdatePlacement(this, &center, &half, m_searchRadius);
    }
}


// ---- src/SoultreeObject/RemoveFromSearch.cpp

void SoultreeObject::RemoveFromSearch()
{
    if (m_searchPlacement) {
        m_searchPlacement->Remove((SearchableObject *)this);
        delete m_searchPlacement;
        m_searchPlacement = 0;
    }
}


// ---- src/SoultreeObject/Enable.cpp

// Local view of SoultreeObject (see SoultreeObject.h) that exposes the
// GameObject fields.


void SoultreeObject::Enable()
{
    if (!m_enabled)
        SearchableObject::m_hidden = m_hiddenWhenEnabled;
    GameObject::Enable();
}


// ---- src/SoultreeObject/Disable.cpp

// Local view of SoultreeObject (see SoultreeObject.h) that exposes the
// GameObject fields.


// Remember whether the object was hidden, then hide it while disabled.
void SoultreeObject::Disable()
{
    if (m_enabled) {
        m_hiddenWhenEnabled = SearchableObject::m_hidden;
        SearchableObject::m_hidden = 1;
    }
    GameObject::Disable();
}


