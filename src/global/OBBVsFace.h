// OBBVsFace.h — the box-against-face-leaf separating-axis test, inlined by
// TestLeafAgainstBox and BoxIntersectsBoundingBoxTree.
#ifndef GLOBAL_OBBVSFACE_H
#define GLOBAL_OBBVSFACE_H

#include "global/CollisionTree.h"
#include "global/Vu0Inline.h"
#include "global/Vu0More.h"
#include "global/FabsInline.h"

extern _D3DVECTOR TestAxis[15];

// Retail's fmin/fmax: min.s/max.s into $f8, stored through a stack temporary.
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

// out = m * (v.xyz, 1), with the operands listed (v, out, m) as retail
// inlines it here (the order decides which registers get the addresses).
static inline void Vu0TransformPointVOM(_D3DMATRIX *m, _D3DVECTOR *v, _D3DVECTOR *out)
{
    asm("lqc2 vf4, 0x0(%2); lqc2 vf5, 0x10(%2); lqc2 vf6, 0x20(%2); lqc2 vf7, 0x30(%2); lqc2 vf8, 0x0(%0); vmulw.w vf9, vf0, vf0w; vmulax.xyz ACC, vf4, vf8x; vmadday.xyz ACC, vf5, vf8y; vmaddz.xyz vf9, vf6, vf8z; vadd.xyz vf9, vf9, vf7; sqc2 vf9, 0x0(%1)"
        : : "r"(v), "r"(out), "r"(m) : "memory");
}

// A face tree's leaf node: the face is stored from +0x04.
struct FaceLeaf {
    float m_size;                       // +0x00, negative
    BoundingBoxTreeFace m_face;         // +0x04
};

// Does the box (center, half) overlap the face leaf, its vertices and normal
// taken to the box's space by m?
// Separating-axis test over 13 axes: the face normal, the box axes and the
// box axes crossed with the face's edges.
static inline int OBBVsFaceOverlapTest(_D3DVECTOR *center, _D3DVECTOR *half, FaceLeaf *leaf, _D3DMATRIX *m)
{
    _D3DVECTOR v[3];
    _D3DVECTOR normal;
    _D3DVECTOR faceNormal(leaf->m_face.m_normalX, leaf->m_face.m_normalY, leaf->m_face.m_normalZ);
    Vu0TransformPointVOM(m, &BoundingBoxTreeVertices[leaf->m_face.m_vertexIndex[0]], &v[0]);
    Vu0TransformPointVOM(m, &BoundingBoxTreeVertices[leaf->m_face.m_vertexIndex[1]], &v[1]);
    Vu0TransformPointVOM(m, &BoundingBoxTreeVertices[leaf->m_face.m_vertexIndex[2]], &v[2]);
    Vu0ApplyMatrixVector(&normal, m, faceNormal);
    _D3DVECTOR a[3];
    a[0] = _D3DVECTOR(half->x, 0.0f, 0.0f);
    a[1] = _D3DVECTOR(0.0f, half->y, 0.0f);
    a[2] = _D3DVECTOR(0.0f, 0.0f, half->z);
    _D3DVECTOR d = v[0] - *center;
    _D3DVECTOR e[3];
    _D3DVECTOR axis;
    for (int i = 0; i < 13; i++) {
        switch (i) {
        case 0:
            TestAxis[i] = normal;
            break;
        case 1:
            TestAxis[i] = _D3DVECTOR(1.0f, 0.0f, 0.0f);
            break;
        case 2:
            TestAxis[i] = _D3DVECTOR(0.0f, 1.0f, 0.0f);
            break;
        case 3:
            TestAxis[i] = _D3DVECTOR(0.0f, 0.0f, 1.0f);
            break;
        case 4:
            Vu0CrossProduct(&TestAxis[i], &TestAxis[1], &e[0]);
            axis = TestAxis[i];
            break;
        case 5:
            Vu0CrossProduct(&TestAxis[i], &TestAxis[2], &e[0]);
            break;
        case 6:
            Vu0CrossProduct(&TestAxis[i], &TestAxis[3], &e[0]);
            break;
        case 7:
            Vu0CrossProduct(&TestAxis[i], &TestAxis[1], &e[1]);
            break;
        case 8:
            Vu0CrossProduct(&TestAxis[i], &TestAxis[2], &e[1]);
            break;
        case 9:
            Vu0CrossProduct(&TestAxis[i], &TestAxis[3], &e[1]);
            break;
        case 10:
            Vu0Normalize(&e[2], v[2] - v[0]);
            Vu0CrossProduct(&TestAxis[i], &TestAxis[1], &e[2]);
            break;
        case 11:
            Vu0CrossProduct(&TestAxis[i], &TestAxis[2], &e[2]);
            break;
        case 12:
            Vu0CrossProduct(&TestAxis[i], &TestAxis[3], &e[2]);
            break;
        }
        float r = Fabs(Vu0DotProduct(&a[0], &TestAxis[i])) + Fabs(Vu0DotProduct(&a[1], &TestAxis[i])) + Fabs(Vu0DotProduct(&a[2], &TestAxis[i]));
        float p0;
        if (i == 0) {
            p0 = Fabs(Vu0DotProduct(&d, &TestAxis[i]));
            if (p0 > r)
                return 0;
            e[0] = v[0] - v[1];
            e[1] = v[0] - v[2];
        } else {
            p0 = Vu0DotProduct(&d, &TestAxis[i]);
            float p1, p2, mn, mx;
            p1 = Vu0DotProduct(&e[0], &TestAxis[i]);
            p2 = Vu0DotProduct(&e[1], &TestAxis[i]);
            // The differences written out in both calls (mwcc reuses the
            // first pair): named locals passed to the Fmax would be renamed
            // to its parameters and recoloured.
            mn = Fmin(p0 - p1, p0 - p2);
            mn = Fmin(mn, p0);
            mx = Fmax(p0 - p1, p0 - p2);
            mx = Fmax(mx, p0);
            if (-r > mx)
                return 0;
            if (r < mn)
                return 0;
        }
    }
    return 1;
}

#endif
