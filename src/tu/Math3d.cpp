// Math3d.cpp  @ 0x0025A2B0 (text 0x25A2B0-0x25C1D0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp)
//
// Retail's Math3d.cpp as one translation unit: the vector/matrix helpers,
// unanchored inside LightEmitter.cpp's tu_map row
// (GetMatrixRow/SetMatrixRow ASSERT "Math3d.cpp"; its strings @3408..@3433
// follow LightEmitter.cpp's RTTI). __sinit_Math3d.cpp (the .ctor slot after
// LightEmitter.cpp's) builds only the axis vectors; FindPlaneNormal's
// ZeroVector is this TU's. AdamsBashforthIntegration (no data) is placed at
// the tail, undecided vs MathQuaternion.cpp's head. `make link` places its
// data as well as its code.


// The chained `start + t * dir` in RayIntersectPlane needs a deeper inline
// depth than mwcc's default, or the _D3DVECTOR ctor is called out of line.
#pragma inline_depth(4)

#include "d3dtypes.h"
#include "SoultreeObject/SoultreeVU0.h"
#include "global/Vu0Inline.h"
#include "global/Vu0More.h"
#include "soultree_assert.h"
#include "sinit/AxisVectors.h"

extern "C" void *memset(void *dst, int c, unsigned int n);
extern "C" void *memcpy(void *dst, const void *src, unsigned int n);
float FastTan(float x);
void FastSinCos(float x, float *sine, float *cosine);
_D3DMATRIX ZeroMatrix();
_D3DMATRIX IdentityMatrix();
_D3DMATRIX MatrixRotateZ(float angle);

// ---- src/global/FindPlaneNormal.cpp


static inline void CrossProduct(_D3DVECTOR &out, const _D3DVECTOR &a, const _D3DVECTOR &b)
{
    asm("lqc2 vf4, 0x0(%1); lqc2 vf5, 0x0(%2); vopmula.xyz ACC, vf4, vf5; vopmsub.xyz vf6, vf5, vf4; vsub.w vf6, vf6, vf6; sqc2 vf6, 0x0(%0)"
        : : "r"(&out), "r"(&a), "r"(&b) : "memory");
}

static inline float DotProduct(_D3DVECTOR *a, _D3DVECTOR *b)
{
    float r;
    asm("lqc2 vf4, 0x0(%1); lqc2 vf5, 0x0(%2); vmul.xyz vf5, vf4, vf5; vaddy.x vf5, vf5, vf5y; vaddz.x vf5, vf5, vf5z; qmfc2 $2, vf5; mtc1 $2, %0"
        : "=f"(r) : "r"(a), "r"(b) : "$2");
    return r;
}

static inline float rsqrtf(float x)
{
    float r;
    asm("rsqrt.s $f8, %1, %2; swc1 $f8, 0(%0)" : : "r"(&r), "f"(1.0f), "f"(x) : "$f8", "memory");
    return r;
}

void FindPlaneNormal(_D3DVECTOR &a, _D3DVECTOR &b, _D3DVECTOR &c, _D3DVECTOR &normal, float *d)
{
    CrossProduct(normal, b - a, c - a);
    float len2 = normal.x * normal.x + normal.y * normal.y + normal.z * normal.z;
    if (len2 == 0.0f) {
        normal = ZeroVector;
    } else {
        float r = rsqrtf(len2);
        normal.x *= r;
        normal.y *= r;
        normal.z *= r;
    }
    if (d)
        *d = -DotProduct(&normal, &a);
}


// ---- src/global/RayIntersectPlane.cpp



// Dot product with the vector the previous DotProduct left in vf4.
static inline float DotProductVf4(_D3DVECTOR *b)
{
    float r;
    asm("lqc2 vf5, 0x0(%1); vmul.xyz vf5, vf4, vf5; vaddy.x vf5, vf5, vf5y; vaddz.x vf5, vf5, vf5z; qmfc2 $2, vf5; mtc1 $2, %0"
        : "=f"(r) : "r"(b) : "$2");
    return r;
}

// Where does the line start->end cross the plane through a, b, c?
void RayIntersectPlane(_D3DVECTOR &start, _D3DVECTOR &end, _D3DVECTOR &a, _D3DVECTOR &b, _D3DVECTOR &c, _D3DVECTOR &hit)
{
    _D3DVECTOR normal;
    _D3DVECTOR dir;
    float d;
    FindPlaneNormal(a, b, c, normal, &d);
    dir = end - start;
    float nDotStart = DotProduct(&normal, &start);
    float nDotDir = DotProductVf4(&dir);
    float t = -(d + nDotStart) / nDotDir;
    hit = start + t * dir;
}


// ---- src/global/ZeroMatrix.cpp
// _D3DMATRIX with an inline element accessor (an if-chain over row and
// column) and a memberwise copy constructor; both show in retail codegen.

_D3DMATRIX ZeroMatrix()
{
    _D3DMATRIX m;
    for (int row = 0; row < 4; row++)
        for (int column = 0; column < 4; column++)
            m(row, column) = 0.0f;
    return m;
}


// ---- src/global/IdentityMatrix.cpp


_D3DMATRIX IdentityMatrix()
{
    _D3DMATRIX m;
    memset(&m, 0, sizeof(m));
    m._11 = 1.0f;
    m._22 = 1.0f;
    m._33 = 1.0f;
    m._44 = 1.0f;
    return m;
}


// ---- src/global/ProjectionMatrix.cpp


// Perspective projection (D3D style, w = z).
_D3DMATRIX ProjectionMatrix(float nearPlane, float farPlane, float fov, float aspect)
{
    float w = 1.0f / FastTan(0.5f * fov);
    float q = 1.0f / (1.0f - nearPlane / farPlane);
    _D3DMATRIX m;
    m = ZeroMatrix();
    m._11 = w;
    m._22 = w * aspect;
    m._33 = q;
    m._34 = 1.0f;
    m._43 = -q * nearPlane;
    return m;
}


// ---- src/global/CreateViewMatrix.cpp


// View matrix for a camera at position looking along dir with the given up
// vector, rolled about its view axis by roll (radians).
_D3DMATRIX CreateViewMatrix(_D3DVECTOR position, _D3DVECTOR dir, _D3DVECTOR up, float roll)
{
    _D3DMATRIX m __attribute__((aligned(16))) = IdentityMatrix();
    _D3DVECTOR right, unused;     // retail constructs a second, unused vector
    Vu0CrossProduct(&right, &up, &dir);
    m(0, 0) = right.x;
    m(1, 0) = right.y;
    m(2, 0) = right.z;
    m(0, 1) = up.x;
    m(1, 1) = up.y;
    m(2, 1) = up.z;
    m(0, 2) = dir.x;
    m(1, 2) = dir.y;
    m(2, 2) = dir.z;
    m(3, 0) = -Vu0DotProduct(&right, &position);
    m(3, 1) = -Vu0DotProduct(&up, &position);
    m(3, 2) = -Vu0DotProduct(&dir, &position);
    if (roll != 0.0f)
        MulMatrixCopy(&m, MatrixRotateZ(-roll), m);
    return m;
}


// ---- src/global/MatrixRotateY.cpp
// _D3DMATRIX with an inline element accessor (an if-chain over row and
// column) and a memberwise copy constructor; both show in retail codegen.


_D3DMATRIX MatrixRotateY(float angle)
{
    float cosine, sine;
    FastSinCos(angle, &sine, &cosine);
    _D3DMATRIX m = IdentityMatrix();
    m(0, 0) = cosine;
    m(2, 2) = cosine;
    m(0, 2) = sine;
    m(2, 0) = -sine;
    return m;
}


// ---- src/global/MatrixRotateZ.cpp
// _D3DMATRIX with an inline element accessor (an if-chain over row and
// column) and a memberwise copy constructor; both show in retail codegen.


_D3DMATRIX MatrixRotateZ(float angle)
{
    float cosine, sine;
    FastSinCos(angle, &sine, &cosine);
    _D3DMATRIX m = IdentityMatrix();
    m(0, 0) = cosine;
    m(1, 1) = cosine;
    m(0, 1) = -sine;
    m(1, 0) = sine;
    return m;
}


// ---- src/global/MatrixMult.cpp


// Product of two matrices, element by element through the accessor;
// note the operand order: result(i, j) = sum over k of a(k, j) * b(i, k).
_D3DMATRIX MatrixMult(_D3DMATRIX a, _D3DMATRIX b)
{
    _D3DMATRIX r = ZeroMatrix();
    int i, j, k;
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            for (k = 0; k < 4; k++)
                r(i, j) += a(k, j) * b(i, k);
    return r;
}


// ---- src/global/MatrixInverse.cpp


// Inverse of the upper 3x3 (rotation/scale) part; the rest is left zero.
// Cofactors written out each time (mwcc CSEs them); named locals change
// the madd/msub fusion and register use.
_D3DMATRIX MatrixInverse(_D3DMATRIX m)
{
    _D3DMATRIX r;
    float det = m._11 * (m._22 * m._33 - m._32 * m._23) - m._12 * (m._21 * m._33 - m._31 * m._23) + m._13 * (m._21 * m._32 - m._31 * m._22);
    if (det != 0.0f) {
        float inv = 1.0f / det;
        r._11 = inv * (m._22 * m._33 - m._32 * m._23);
        r._12 = inv * -(m._12 * m._33 - m._32 * m._13);
        r._13 = inv * (m._12 * m._23 - m._22 * m._13);
        r._14 = 0.0f;
        r._21 = inv * -(m._21 * m._33 - m._31 * m._23);
        r._22 = inv * (m._11 * m._33 - m._31 * m._13);
        r._23 = inv * -(m._11 * m._23 - m._21 * m._13);
        r._24 = 0.0f;
        r._31 = inv * (m._21 * m._32 - m._31 * m._22);
        r._32 = inv * -(m._11 * m._32 - m._31 * m._12);
        r._33 = inv * (m._11 * m._22 - m._21 * m._12);
        r._34 = 0.0f;
        r._41 = 0.0f;
        r._42 = 0.0f;
        r._43 = 0.0f;
        r._44 = 0.0f;   // retail leaves _44 at 0, not 1
    } else {
        r = ZeroMatrix();
    }
    return r;
}


// ---- src/global/TransformVector.cpp

// Plain (POD) matrix with the inline element accessor: by-value arguments are
// copied into the frame, the matrix with a word loop.


// Rotates v by the upper 3x3 of m (row vector times matrix).
_D3DVECTOR TransformVector(_D3DVECTOR v, _D3DMATRIX m)
{
    _D3DVECTOR r;
    for (int i = 0; i < 3; i++) {
        r[i] = 0.0f;
        for (int j = 0; j < 3; j++)
            r[i] += v[j] * m(j, i);
    }
    return r;
}


// ---- src/global/TransformVertices.cpp



void TransformVertices(_D3DVECTOR *out, _D3DVECTOR *in, _D3DMATRIX *m, int count, int outStride, int inStride)
{
    if (!count)
        return;
    {
        if (!inStride)
            inStride = outStride;
        _D3DMATRIX mat __attribute__((aligned(16)));
        memcpy(&mat, m, sizeof(mat));
        asm("lqc2 vf1, 0x0(%0); lqc2 vf2, 0x10(%0); lqc2 vf3, 0x20(%0); lqc2 vf4, 0x30(%0)" : : "r"(&mat) : "memory");
        for (int i = 0; i < count; i++) {
            asm("lqc2 vf5, 0x0(%0); vaddx.w vf5, vf0, vf0x; vmulax.xyzw ACC, vf1, vf5x; vmadday.xyzw ACC, vf2, vf5y; vmaddaz.xyzw ACC, vf3, vf5z; vmaddw.xyzw vf6, vf4, vf5w; sqc2 vf6, 0x0(%1)"
                : : "r"(in), "r"(out) : "memory");
            in = (_D3DVECTOR *)((char *)in + inStride);
            out = (_D3DVECTOR *)((char *)out + outStride);
        }
    }
}


// ---- src/global/GetMatrixRow.cpp




#pragma alignlabel off
_D3DVECTOR GetMatrixRow(_D3DMATRIX *m, int row)
{
    _D3DVECTOR v;
    switch (row) {
    case 0:
        v.x = m->_11;
        v.y = m->_12;
        v.z = m->_13;
        break;
    case 1:
        v.x = m->_21;
        v.y = m->_22;
        v.z = m->_23;
        break;
    case 2:
        v.x = m->_31;
        v.y = m->_32;
        v.z = m->_33;
        break;
    case 3:
        v.x = m->_41;
        v.y = m->_42;
        v.z = m->_43;
        break;
    default:
        PSXprintf("Out of bounds in Math3D.h, GetMatrixRow.\n");
#line 1045 "Math3d.cpp"
        ASSERT(0, "GetMatrixRow: rowNumber out of bounds.");
        break;
    }
    return v;
}


#pragma alignlabel reset

// ---- src/global/SetMatrixRow.cpp




#pragma alignlabel off
void SetMatrixRow(_D3DMATRIX *m, int row, _D3DVECTOR v)
{
    switch (row) {
    case 0:
        m->_11 = v.x;
        m->_12 = v.y;
        m->_13 = v.z;
        break;
    case 1:
        m->_21 = v.x;
        m->_22 = v.y;
        m->_23 = v.z;
        break;
    case 2:
        m->_31 = v.x;
        m->_32 = v.y;
        m->_33 = v.z;
        break;
    case 3:
        m->_41 = v.x;
        m->_42 = v.y;
        m->_43 = v.z;
        break;
    default:
        PSXprintf("Out of bounds in Math3D.h, SetMatrixRow.\n");
#line 1077 "Math3d.cpp"
        ASSERT(0, "SetMatrixRow: rowNumber out of bounds.");
        break;
    }
}


#pragma alignlabel reset

// ---- src/global/AdamsBashforthIntegration.cpp
// Two-step Adams-Bashforth: y += h/2 * (3*f(n) - f(n-1)). The derivative
// history is declared float ** but used as one flat array holding the
// current derivatives followed by the previous ones.
void AdamsBashforthIntegration(float h, int n, float *y, float **dy)
{
    int i;
    float *cur = (float *)dy;
    float *prev = cur + n;
    for (i = 0; i < n; i++) {
        *y += 0.5f * h * (3.0f * *cur - *prev);
        *prev = *cur;
        y++;
        cur++;
        prev++;
    }
}

// ---- src/sinit/Math3d.cpp
// __sinit_Math3d.cpp builds only the axis vectors (sinit/AxisVectors.h, included above).
