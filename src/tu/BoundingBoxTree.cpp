// BoundingBoxTree.cpp  @ 0x00204800 (text 0x204800-0x211020)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's BoundingBoxTree.cpp as one translation unit: the BMP loader, the OBB
// overlap tests and the bounding-box tree queries, the weak _D3DVECTOR ctor,
// then the static initializer (axis vectors, NormalList, TestAxis).
// BlockAllocator.cpp precedes it. Its .rodata labels run from @473 through
// "BoundingBoxTree.cpp" (@2585) to @4635; Camera.cpp restarts at @889.
// BoundingBoxTreeBuild.cpp follows at 0x211020. The per-file inline_depth
// pragmas are kept (each until the next one).



#include "BlockAllocator/BlockAllocator.h"
#include "d3dtypes.h"
#include "sinit/AxisVectors.h"
#include "global/Vu0Inline.h"
#include "global/Vu0More.h"
#include "global/FabsInline.h"
#include "soultree_assert_block.h"
#include "global/OBBVsFace.h"
#include "global/CollisionTree.h"
#include "BoundingBoxTree/BoundingBoxTree.h"
#include "SoultreeObject/SoultreeVU0.h"
#include "global/OBBOverlap.h"
#include "global/RayFace.h"


// Defined here for its retail .bss slot.
RayToFaceIntersectionData *CurrentCollisionData;

// Constructed by the static initializer (with the axis vectors); defined
// before the functions: their .bss precedes the functions' statics.
_D3DVECTOR NormalList[128];

// Defined here for its retail .bss slot.
int NumberOfNormals;
int LastNormal;
BoundingBoxTreeType BoundingBoxTreeAType;
_D3DMATRIX BtoASpaceTransform;
_D3DMATRIX AtoBSpaceTransform;
_D3DMATRIX AtoADeltaTransform;
_D3DVECTOR *BoundingBoxTreeVertices;

_D3DVECTOR TestAxis[15];

// ---- src/global/ImportBmpFile.cpp
// Retail built this function without the global optimizer (unrotated row
// loop, the inner loop tested at the bottom).
#pragma global_optimizer off
extern "C" void *malloc(unsigned int size);
extern "C" void free(void *p);
extern "C" char *strcpy(char *dst, const char *src);

struct FILE_HANDLE_STRUCT;
FILE_HANDLE_STRUCT *FOPEN(char *name, const char *mode);
unsigned int FREAD(void *buf, unsigned int size, unsigned int count, FILE_HANDLE_STRUCT *f);
unsigned int FREAD(void *buf, unsigned int size, FILE_HANDLE_STRUCT *f);
int FSEEK(FILE_HANDLE_STRUCT *f, long offset, int whence);
int FGETC(FILE_HANDLE_STRUCT *f);
int FCLOSE(FILE_HANDLE_STRUCT *f);

struct BmpFile {
    unsigned short type;            // +0x00
    unsigned int size;              // +0x04
    unsigned short reserved1;       // +0x08
    unsigned short reserved2;       // +0x0A
    unsigned int offBits;           // +0x0C
    unsigned int infoSize;          // +0x10
    int width;                      // +0x14
    unsigned int height;            // +0x18
    unsigned short planes;          // +0x1C
    unsigned short bitCount;        // +0x1E
    unsigned int compression;       // +0x20
    unsigned int sizeImage;         // +0x24
    int xPelsPerMeter;              // +0x28
    int yPelsPerMeter;              // +0x2C
    unsigned int clrUsed;           // +0x30
    unsigned int clrImportant;      // +0x34
    unsigned char palette[256][4];  // +0x38
    unsigned char *pixels;          // +0x438
    char name[0x80];                // +0x43C
};

// Loads an 8-bit BMP: header, palette and pixel rows (row padding skipped).
BmpFile *ImportBmpFile(char *filename, BmpFile *buffer)
{
    FILE_HANDLE_STRUCT *f;
    BmpFile *bmp;
    unsigned int y;
    unsigned char *p;
    int rowSize;
    int x;

    if (!(f = FOPEN(filename, "rb")))
        return 0;
    if (buffer) {
        bmp = buffer;
    } else {
        if (!(bmp = (BmpFile *)malloc(sizeof(BmpFile))))
            goto fail;
        bmp->pixels = 0;
    }
    if (FREAD(&bmp->type, 2, 1, f) != 1)
        goto fail;
    if (FREAD(&bmp->size, 4, 1, f) != 1)
        goto fail;
    if (FREAD(&bmp->reserved1, 2, 1, f) != 1)
        goto fail;
    if (FREAD(&bmp->reserved2, 2, 1, f) != 1)
        goto fail;
    if (FREAD(&bmp->offBits, 4, 1, f) != 1)
        goto fail;
    if (FREAD(&bmp->infoSize, 40, 1, f) != 1)
        goto fail;
    if (bmp->bitCount != 8)
        goto fail;
    rowSize = bmp->width;
    if (rowSize & 3) {
        rowSize |= 3;
        rowSize++;
    }
    FREAD(bmp->palette, 4, 256, f);
    if (!bmp->pixels) {
        if (!(bmp->pixels = (unsigned char *)malloc(bmp->width * bmp->height)))
            goto fail;
    }
    FSEEK(f, bmp->offBits, 0);
    p = bmp->pixels;
    for (y = 0; y < bmp->height; y++) {
        FREAD(p, bmp->width, f);
        x = bmp->width;
        p += x;
        while (x < rowSize) {
            FGETC(f);
            x++;
        }
    }
    strcpy(bmp->name, filename);
    FCLOSE(f);
    return bmp;

fail:
    if (bmp) {
        if (bmp->pixels)
            free(bmp->pixels);
        free(bmp);
    }
    if (f)
        FCLOSE(f);
    return 0;
}
#pragma global_optimizer reset



// ---- src/global/FreeBmpFile.cpp

void FreeBmpFile(BmpFile *bmp)
{
    if (!bmp)
        return;
    if (bmp->pixels)
        free(bmp->pixels);
    free(bmp);
}


// ---- src/global/OBBOverlapTestWithDelta.cpp


void ModifyBoundingBox(_D3DVECTOR *min, _D3DVECTOR *max, _D3DMATRIX *matrix);
int OBBOverlapTest(_D3DVECTOR *center, _D3DVECTOR *half, _D3DVECTOR center2, _D3DVECTOR half2, _D3DMATRIX *matrix);

int OBBOverlapTestWithDelta(_D3DVECTOR center, _D3DVECTOR half, _D3DVECTOR *center2, _D3DVECTOR *half2, _D3DMATRIX *matrix, _D3DMATRIX *delta)
{
    ModifyBoundingBox(&center, &half, delta);
    return OBBOverlapTest(&center, &half, *center2, *half2, matrix);
}


// ---- src/global/OBBOverlapTest.cpp
#pragma inline_depth(4)



// Do the box (center, half) and the box (center2, half2) in m's space
// overlap?  Separating-axis test over the 15 candidate axes.
int OBBOverlapTest(_D3DVECTOR *center, _D3DVECTOR *half, _D3DVECTOR center2, _D3DVECTOR half2, _D3DMATRIX *m)
{
    Vu0TransformPoint(m, &center2, &center2);
    _D3DVECTOR a[3], b[3];
    a[0] = _D3DVECTOR(half->x, 0.0f, 0.0f);
    a[1] = _D3DVECTOR(0.0f, half->y, 0.0f);
    a[2] = _D3DVECTOR(0.0f, 0.0f, half->z);
    b[0] = _D3DVECTOR(half2.x, 0.0f, 0.0f);
    b[1] = _D3DVECTOR(0.0f, half2.y, 0.0f);
    b[2] = _D3DVECTOR(0.0f, 0.0f, half2.z);
    Vu0ApplyMatrixVector(&b[0], m, b[0]);
    Vu0ApplyMatrixVector(&b[1], m, b[1]);
    Vu0ApplyMatrixVector(&b[2], m, b[2]);
    _D3DVECTOR d = center2 - *center;
    for (int i = 0; i < 15; i++) {
        switch (i) {
        case 0:
            TestAxis[i] = _D3DVECTOR(1.0f, 0.0f, 0.0f);
            break;
        case 1:
            TestAxis[i] = _D3DVECTOR(0.0f, 1.0f, 0.0f);
            break;
        case 2:
            TestAxis[i] = _D3DVECTOR(0.0f, 0.0f, 1.0f);
            break;
        case 3:
            TestAxis[i] = _D3DVECTOR(m->_11, m->_12, m->_13);
            break;
        case 4:
            TestAxis[i] = _D3DVECTOR(m->_21, m->_22, m->_23);
            break;
        case 5:
            TestAxis[i] = _D3DVECTOR(m->_31, m->_32, m->_33);
            break;
        case 6:
            Vu0CrossProduct(&TestAxis[i], &TestAxis[0], &TestAxis[3]);
            break;
        case 7:
            Vu0CrossProduct(&TestAxis[i], &TestAxis[0], &TestAxis[4]);
            break;
        case 8:
            Vu0CrossProduct(&TestAxis[i], &TestAxis[0], &TestAxis[5]);
            break;
        case 9:
            Vu0CrossProduct(&TestAxis[i], &TestAxis[1], &TestAxis[3]);
            break;
        case 10:
            Vu0CrossProduct(&TestAxis[i], &TestAxis[1], &TestAxis[4]);
            break;
        case 11:
            Vu0CrossProduct(&TestAxis[i], &TestAxis[1], &TestAxis[5]);
            break;
        case 12:
            Vu0CrossProduct(&TestAxis[i], &TestAxis[2], &TestAxis[3]);
            break;
        case 13:
            Vu0CrossProduct(&TestAxis[i], &TestAxis[2], &TestAxis[4]);
            break;
        case 14:
            Vu0CrossProduct(&TestAxis[i], &TestAxis[2], &TestAxis[5]);
            break;
        }
        float rd = Fabs(Vu0DotProduct(&d, &TestAxis[i]));
        float ra = Fabs(Vu0DotProduct(&a[0], &TestAxis[i])) + Fabs(Vu0DotProduct(&a[1], &TestAxis[i])) + Fabs(Vu0DotProduct(&a[2], &TestAxis[i]));
        float rb = Fabs(Vu0DotProduct(&b[0], &TestAxis[i])) + Fabs(Vu0DotProduct(&b[1], &TestAxis[i])) + Fabs(Vu0DotProduct(&b[2], &TestAxis[i]));
        if (rd > ra + rb)
            return 0;
    }
    return 1;
}


// ---- src/global/OBBVsRayOverlapTest.cpp
// Nested vector operators need a deeper inline depth than mwcc's default.
#pragma inline_depth(5)
struct _D3DMATRIX;



// Does the segment start-end (world space) cross the box given by center
// and half-size in m's space?  Separating-axis test.
int OBBVsRayOverlapTest(_D3DVECTOR *center, _D3DVECTOR *half, _D3DVECTOR &start, _D3DVECTOR &end, _D3DMATRIX *m)
{
    _D3DVECTOR p0;
    _D3DVECTOR p1;
    Vu0TransformPoint(m, &start, &p0);
    Vu0TransformPoint(m, &end, &p1);
    p0.x -= center->x;
    p0.y -= center->y;
    p0.z -= center->z;
    p1.x -= center->x;
    p1.y -= center->y;
    p1.z -= center->z;
    _D3DVECTOR mid = 0.5f * (p0 + p1);
    _D3DVECTOR dir = mid - p0;
    _D3DVECTOR absDir;
    absDir.x = Fabs(dir.x);
    if (Fabs(mid.x) > absDir.x + half->x)
        return 0;
    absDir.y = Fabs(dir.y);
    if (Fabs(mid.y) > absDir.y + half->y)
        return 0;
    absDir.z = Fabs(dir.z);
    if (Fabs(mid.z) > absDir.z + half->z)
        return 0;
    if (Fabs(mid.y * dir.z - mid.z * dir.y) > half->y * absDir.z + half->z * absDir.y)
        return 0;
    if (Fabs(mid.x * dir.z - mid.z * dir.x) > half->x * absDir.z + half->z * absDir.x)
        return 0;
    return Fabs(mid.x * dir.y - mid.y * dir.x) > half->x * absDir.y + half->y * absDir.x ? 0 : 1;
}


// ---- src/global/OBBVsSphereOverlapTest.cpp
struct _D3DMATRIX;

// Retail's fabs/fmin: abs.s/min.s into $f8, stored through a stack temporary.


// out = inverse(m) * v for a rigid m: subtract the translation, then
// multiply by the transposed rotation.
static inline void Vu0TransformPointToLocal(_D3DMATRIX *m, _D3DVECTOR v, _D3DVECTOR *out)
{
    asm("lqc2 vf1, 0x0(%0); lqc2 vf2, 0x10(%0); lqc2 vf3, 0x20(%0); lqc2 vf4, 0x30(%0); vmulw.w vf21, vf0, vf0w; vsub.xyzw vf30, vf30, vf30; vadd.xyzw vf5, vf1, vf30; vadd.xyzw vf6, vf2, vf30; vadd.xyzw vf7, vf3, vf30; vadd.xyzw vf8, vf4, vf30; vaddy.x vf6, vf0, vf1y; vaddz.x vf7, vf0, vf1z; vaddz.y vf7, vf0, vf2z; vaddx.y vf5, vf0, vf2x; vaddx.z vf5, vf0, vf3x; vaddy.z vf6, vf0, vf3y; lqc2 vf20, 0x0(%1); vsub.xyz vf20, vf20, vf8; vmulax.xyz ACC, vf5, vf20x; vmadday.xyz ACC, vf6, vf20y; vmaddz.xyz vf21, vf7, vf20z; sqc2 vf21, 0x0(%2)"
        : : "r"(m), "r"(&v), "r"(out) : "memory");
}

// Does a sphere (world space) overlap a box given by center/half-size in the
// space of matrix m?
int OBBVsSphereOverlapTest(_D3DVECTOR *center, _D3DVECTOR *half, _D3DVECTOR &sphereCenter, float radius, float radiusSquared, _D3DMATRIX *m)
{
    _D3DVECTOR p;
    Vu0TransformPointToLocal(m, sphereCenter, &p);
    p.x -= center->x;
    p.y -= center->y;
    p.z -= center->z;
    p.x = Fabs(p.x);
    if (p.x - half->x > radius)
        return 0;
    p.y = Fabs(p.y);
    if (p.y - half->y > radius)
        return 0;
    p.z = Fabs(p.z);
    if (p.z - half->z > radius)
        return 0;
    // Clamp to the box surface.
    p.x -= Fmin(p.x, half->x);
    p.y -= Fmin(p.y, half->y);
    p.z -= Fmin(p.z, half->z);
    // Distance from the sphere center to the box, squared.
    return radiusSquared > p.x * p.x + p.y * p.y + p.z * p.z ? 1 : 0;
}


// ---- src/global/SphereVsSphereOverlapTest.cpp



// Do two spheres overlap?  Each center is in its object's local space when a
// matrix is given, else already in world space.
int SphereVsSphereOverlapTest(_D3DVECTOR &center1, float radius1, _D3DMATRIX *m1,
                              _D3DVECTOR &center2, float radius2, _D3DMATRIX *m2)
{
    _D3DVECTOR p1 __attribute__((aligned(16)));
    _D3DVECTOR p2 __attribute__((aligned(16)));
    if (m1)
        Vu0TransformPoint(m1, &center1, &p1);
    else
        p1 = center1;
    if (m2)
        Vu0TransformPoint(m2, &center2, &p2);
    else
        p2 = center2;
    _D3DVECTOR d = p2 - p1;
    float dist = Fsqrt(d.x * d.x + d.y * d.y + d.z * d.z);
    return dist <= radius1 + radius2 ? 1 : 0;
}


// ---- src/global/OBBVsRadiusedLineSegmentOverlapTest.cpp
#pragma inline_depth(5)

// Direct3D's componentwise product (D3D_OVERLOADS).
inline _D3DVECTOR operator*(const _D3DVECTOR &a, const _D3DVECTOR &b)
{
    return _D3DVECTOR(a.x * b.x, a.y * b.y, a.z * b.z);
}

// Does the capsule (segment points[0]-points[1], world space, with radius)
// overlap the box given by center and half-size in m's space?
// Separating-axis test.
int OBBVsRadiusedLineSegmentOverlapTest(_D3DVECTOR *center, _D3DVECTOR *half, _D3DVECTOR *points, float radius,
                                        float radiusSquared, _D3DMATRIX *m)
{
    _D3DVECTOR p0, p1;
    Vu0InverseTransformPoint(&p0, points[0], m);
    Vu0InverseTransformPoint(&p1, points[1], m);
    p0.x -= center->x;
    p0.y -= center->y;
    p0.z -= center->z;
    p1.x -= center->x;
    p1.y -= center->y;
    p1.z -= center->z;
    _D3DVECTOR mid = 0.5f * (p0 + p1);
    _D3DVECTOR dir = mid - p0;
    _D3DVECTOR absDir;
    absDir.x = Fabs(dir.x);
    if (Fabs(mid.x) > half->x + (absDir.x + radius))
        return 0;
    absDir.y = Fabs(dir.y);
    if (Fabs(mid.y) > half->y + (absDir.y + radius))
        return 0;
    absDir.z = Fabs(dir.z);
    if (Fabs(mid.z) > half->z + (absDir.z + radius))
        return 0;
    _D3DVECTOR ax = _D3DVECTOR(half->x, 0.0f, 0.0f);
    _D3DVECTOR ay = _D3DVECTOR(0.0f, half->y, 0.0f);
    _D3DVECTOR az = _D3DVECTOR(0.0f, 0.0f, half->z);
    _D3DVECTOR n;
    Vu0Normalize(&n, p1 - p0);
    _D3DVECTOR axis;
    float rm, ra, rd;       // per axis: |mid . axis|, the box's and the segment's extents
    axis.x = -n.z;
    axis.y = 0.0f;
    axis.z = n.x;
    Vu0Normalize(&axis, &axis);
    rm = Fabs(Vu0DotProduct(&mid, &axis));
    ra = Fabs(Vu0DotProduct(&ax, &axis)) + Fabs(Vu0DotProduct(&ay, &axis)) + Fabs(Vu0DotProduct(&az, &axis));
    rd = Fabs(Vu0DotProduct(&dir, &axis));
    if (rm > rd + (radius + ra))
        return 0;
    axis.x = n.y;
    axis.y = -n.x;
    axis.z = 0.0f;
    Vu0Normalize(&axis, &axis);
    rm = Fabs(Vu0DotProduct(&mid, &axis));
    ra = Fabs(Vu0DotProduct(&ax, &axis)) + Fabs(Vu0DotProduct(&ay, &axis)) + Fabs(Vu0DotProduct(&az, &axis));
    rd = Fabs(Vu0DotProduct(&dir, &axis));
    if (rm > rd + (radius + ra))
        return 0;
    axis.x = 0.0f;
    axis.y = n.z;
    axis.z = -n.y;
    Vu0Normalize(&axis, &axis);
    rm = Fabs(Vu0DotProduct(&mid, &axis));
    ra = Fabs(Vu0DotProduct(&ax, &axis)) + Fabs(Vu0DotProduct(&ay, &axis)) + Fabs(Vu0DotProduct(&az, &axis));
    rd = Fabs(Vu0DotProduct(&dir, &axis));
    if (rm > rd + (radius + ra))
        return 0;
    // Squared distances of each end's components from the box faces.
    _D3DVECTOR d0, d1;
    d0.x = Fabs(half->x) - Fabs(p0.x);
    d0.y = Fabs(half->y) - Fabs(p0.y);
    d0.z = Fabs(half->z) - Fabs(p0.z);
    d1.x = Fabs(half->x) - Fabs(p1.x);
    d1.y = Fabs(half->y) - Fabs(p1.y);
    d1.z = Fabs(half->z) - Fabs(p1.z);
    d0 = d0 * d0;
    d1 = d1 * d1;
    if (d0.x + d0.y < d1.x + d1.y) {
        if (p0.x < 0.0f)
            axis.x = -d0.x;
        else
            axis.x = d0.x;
        if (p0.y < 0.0f)
            axis.y = -d0.y;
        else
            axis.y = d0.y;
    } else {
        if (p1.x < 0.0f)
            axis.x = -d1.x;
        else
            axis.x = d1.x;
        if (p1.y < 0.0f)
            axis.y = -d1.y;
        else
            axis.y = d1.y;
    }
    axis.z = 0.0f;
    Vu0Normalize(&axis, &axis);
    rm = Fabs(Vu0DotProduct(&mid, &axis));
    ra = Fabs(Vu0DotProduct(&ax, &axis)) + Fabs(Vu0DotProduct(&ay, &axis)) + Fabs(Vu0DotProduct(&az, &axis));
    rd = Fabs(Vu0DotProduct(&dir, &axis));
    if (rm > rd + (radius + ra))
        return 0;
    if (d0.x + d0.z < d1.x + d1.z) {
        if (p0.x < 0.0f)
            axis.x = -d0.x;
        else
            axis.x = d0.x;
        if (p0.z < 0.0f)
            axis.z = -d0.z;
        else
            axis.z = d0.z;
    } else {
        if (p1.x < 0.0f)
            axis.x = -d1.x;
        else
            axis.x = d1.x;
        if (p1.z < 0.0f)
            axis.z = -d1.z;
        else
            axis.z = d1.z;
    }
    axis.y = 0.0f;
    Vu0Normalize(&axis, &axis);
    rm = Fabs(Vu0DotProduct(&mid, &axis));
    ra = Fabs(Vu0DotProduct(&ax, &axis)) + Fabs(Vu0DotProduct(&ay, &axis)) + Fabs(Vu0DotProduct(&az, &axis));
    rd = Fabs(Vu0DotProduct(&dir, &axis));
    if (rm > rd + (radius + ra))
        return 0;
    if (d0.y + d0.z < d1.y + d1.z) {
        if (p0.y < 0.0f)
            axis.y = -d0.y;
        else
            axis.y = d0.y;
        if (p0.z < 0.0f)
            axis.z = -d0.z;
        else
            axis.z = d0.z;
    } else {
        if (p1.y < 0.0f)
            axis.y = -d1.y;
        else
            axis.y = d1.y;
        if (p1.z < 0.0f)
            axis.z = -d1.z;
        else
            axis.z = d1.z;
    }
    axis.x = 0.0f;
    Vu0Normalize(&axis, &axis);
    rm = Fabs(Vu0DotProduct(&mid, &axis));
    ra = Fabs(Vu0DotProduct(&ax, &axis)) + Fabs(Vu0DotProduct(&ay, &axis)) + Fabs(Vu0DotProduct(&az, &axis));
    rd = Fabs(Vu0DotProduct(&dir, &axis));
    return rm > rd + (radius + ra) ? 0 : 1;
}


// ---- src/global/TestLeafAgainstBox.cpp
// inline_depth(6): with 5, the delta copy of the face test (one level deeper)
// colours the e[0]/e[1] temporaries differently from retail.
#pragma inline_depth(6)

void ModifyBoundingBox(_D3DVECTOR *center, _D3DVECTOR *half, _D3DMATRIX *m);
int OBBVsRayOverlapTest(_D3DVECTOR *center, _D3DVECTOR *half, _D3DVECTOR &start, _D3DVECTOR &end, _D3DMATRIX *m);

static inline int OBBVsFaceOverlapTestWithDelta(_D3DVECTOR center, _D3DVECTOR half, FaceLeaf *leaf, _D3DMATRIX *delta)
{
    ModifyBoundingBox(&center, &half, delta);
    return OBBVsFaceOverlapTest(&center, &half, leaf, &BtoASpaceTransform);
}

// A leaf of one tree against a box of the other: a ray leaf of tree A
// against B's box, or A's box (moved by the delta transform if asked) against
// a face leaf of B.
int TestLeafAgainstBox(BoundingBoxTree *a, BoundingBoxTree *b, int useDelta)
{
    if (a->m_size < 0.0f) {
#line 1035 "BoundingBoxTree.cpp"
        ASSERT(BoundingBoxTreeAType != 0, "Invalid TreeA");
        RaySegment *ray = (RaySegment *)&a->m_min;
        return OBBVsRayOverlapTest(&b->m_min, &b->m_max, ray->m_start, ray->m_end, &AtoBSpaceTransform);
    }
    if (useDelta)
        return OBBVsFaceOverlapTestWithDelta(a->m_min, a->m_max, (FaceLeaf *)b, &AtoADeltaTransform);
    return OBBVsFaceOverlapTest(&a->m_min, &a->m_max, (FaceLeaf *)b, &BtoASpaceTransform);
}


// ---- src/global/TestLeafAgainstLeaf.cpp
#pragma inline_depth(4)

// Vu0TransformPoint with the operands listed (out, v, m): retail's registers.
static inline void Vu0TransformPointOVM(_D3DMATRIX *m, _D3DVECTOR *v, _D3DVECTOR *out)
{
    asm("lqc2 vf4, 0x0(%2); lqc2 vf5, 0x10(%2); lqc2 vf6, 0x20(%2); lqc2 vf7, 0x30(%2); lqc2 vf8, 0x0(%1); vmulw.w vf9, vf0, vf0w; vmulax.xyz ACC, vf4, vf8x; vmadday.xyz ACC, vf5, vf8y; vmaddz.xyz vf9, vf6, vf8z; vadd.xyz vf9, vf9, vf7; sqc2 vf9, 0x0(%0)"
        : : "r"(out), "r"(v), "r"(m) : "memory");
}

void PSXprintf(const char *fmt, ...);

static inline int Equal(const _D3DVECTOR &a, const _D3DVECTOR &b)
{
    return a.x == b.x && a.y == b.y && a.z == b.z;
}

static inline int TestRayLeafAgainstFace(RaySegment *ray, BoundingBoxTreeFace *face)
{
    RayToFaceIntersectionData *data = CurrentCollisionData;
    _D3DVECTOR dir, point, normal;
    _D3DVECTOR seg[2];      // the ray in B space, seg[0] = start, seg[1] = end: an array in retail (element 0 is constructed first)
    _D3DVECTOR faceNormal(face->m_normalX, face->m_normalY, face->m_normalZ);
    float length;
    float t;
    Vu0TransformPointOVM(&AtoBSpaceTransform, &ray->m_start, &seg[0]);
    Vu0TransformPointOVM(&AtoBSpaceTransform, &ray->m_end, &seg[1]);
    dir = seg[1] - seg[0];
    t = -1.0f;
    length = Magnitude(dir);
    dir *= 1.0f / length;
    if (RayIntersectBoundingBoxTreeFace(face, dir, seg[0], length, &point, &normal, &t)) {
        if (data && data->m_numRays < data->m_maxRays && data->m_rayT && data->m_rays && data->m_rayNormals) {
            int i = 0;
            int found = 0;
            while (i < data->m_numRays && !found) {
                if (Equal(data->m_rays[i]->m_start, ray->m_start) && Equal(data->m_rays[i]->m_end, ray->m_end))
                    found = 1;
                else
                    i++;
            }
            if (found) {
                if (t < data->m_rayT[i]) {
                    data->m_rayTransformed[i] = 0;
                    data->m_rayT[i] = t;
                    data->m_rays[i] = ray;
                    data->m_rayPoints[i] = point;
                    Vu0Normalize(&data->m_rayNormals[i], &faceNormal);
                }
            } else {
                data->m_rayTransformed[data->m_numRays] = 0;
                data->m_rayT[data->m_numRays] = t;
                data->m_rays[data->m_numRays] = ray;
                data->m_rayPoints[data->m_numRays] = point;
                Vu0Normalize(&data->m_rayNormals[data->m_numRays], &faceNormal);
                data->m_numRays++;
            }
            if (t <= data->m_t) {
                data->m_x00 = 0;
                data->m_t = t;
                data->m_ray = ray;
                data->m_point = point;
                data->m_penetration = Vu0DotProduct(&(length * dir * (1.0f - t)), &faceNormal) * faceNormal;
                Vu0Normalize(&data->m_normal, &faceNormal);
            }
        } else if (data && t < data->m_t) {
            data->m_t = t;
            Vu0Normalize(&data->m_normal, &normal);
        }
        return 1;
    }
    return 0;
}

// A ray leaf of tree A against a face leaf of tree B: on a hit, records the
// ray's hit in CurrentCollisionData (one entry per ray, keeping the nearest)
// and the nearest hit overall.
int TestLeafAgainstLeaf(BoundingBoxTree *a, BoundingBoxTree *b)
{
    BoundingBoxTreeFace *face = (BoundingBoxTreeFace *)&b->m_radius;
    switch (BoundingBoxTreeAType) {
    case 0:
        PSXprintf("Invalid TreeA");
        break;
    case 1:
        {
        RaySegment *ray = (RaySegment *)&a->m_min;
        return TestRayLeafAgainstFace(ray, face);
    }
    }
    return 0;
}


// ---- src/global/TestBoundingBoxTreesForIntersectionBranch.cpp



extern _D3DMATRIX AtoBSpaceTransform;
extern _D3DMATRIX BtoASpaceTransform;
extern _D3DMATRIX AtoADeltaTransform;

int TestLeafAgainstLeaf(BoundingBoxTree *a, BoundingBoxTree *b);
int TestLeafAgainstBox(BoundingBoxTree *a, BoundingBoxTree *b, int useDelta);
int OBBOverlapTestWithDelta(_D3DVECTOR centerA, _D3DVECTOR halfA, _D3DVECTOR *centerB, _D3DVECTOR *halfB,
                            _D3DMATRIX *bToA, _D3DMATRIX *delta);
int OBBVsSphereOverlapTest(_D3DVECTOR *center, _D3DVECTOR *half, _D3DVECTOR &sphereCenter, float radius,
                           float radiusSquared, _D3DMATRIX *transform);

// Recursively tests two bounding-box trees, descending into the larger box
// first; returns whether any pair of leaves intersects.
int TestBoundingBoxTreesForIntersectionBranch(BoundingBoxTree *a, BoundingBoxTree *b, int useDelta)
{
    int hit;
    int overlap;

    if (a->m_size < 0.0f && b->m_size < 0.0f)
        return TestLeafAgainstLeaf(a, b);
    if (a->m_size < 0.0f || b->m_size < 0.0f) {
        if (TestLeafAgainstBox(a, b, useDelta)) {
            if (a->m_size < 0.0f) {
                hit = 0;
                if (TestBoundingBoxTreesForIntersectionBranch(a, b->m_left, useDelta))
                    hit = 1;
                if (TestBoundingBoxTreesForIntersectionBranch(a, b->m_right, useDelta))
                    hit = 1;
                return hit;
            } else {
                hit = 0;
                if (TestBoundingBoxTreesForIntersectionBranch(a->m_left, b, useDelta))
                    hit = 1;
                if (TestBoundingBoxTreesForIntersectionBranch(a->m_right, b, useDelta))
                    hit = 1;
                return hit;
            }
        }
    } else {
        if (useDelta) {
            overlap = OBBOverlapTestWithDelta(a->m_min, a->m_max, &b->m_min, &b->m_max,
                                              &BtoASpaceTransform, &AtoADeltaTransform);
        } else {
            overlap = OBBVsSphereOverlapTest(&a->m_min, &a->m_max, b->m_min, b->m_radius,
                                             b->m_radius * b->m_radius, &AtoBSpaceTransform);
            if (overlap)
                overlap = OBBVsSphereOverlapTest(&b->m_min, &b->m_max, a->m_min, a->m_radius,
                                                 a->m_radius * a->m_radius, &BtoASpaceTransform);
        }
        if (overlap) {
            hit = 0;
            if (a->m_size > b->m_size) {
                if (TestBoundingBoxTreesForIntersectionBranch(a->m_left, b, useDelta))
                    hit = 1;
                if (TestBoundingBoxTreesForIntersectionBranch(a->m_right, b, useDelta))
                    hit = 1;
            } else {
                if (TestBoundingBoxTreesForIntersectionBranch(a, b->m_left, useDelta))
                    hit = 1;
                if (TestBoundingBoxTreesForIntersectionBranch(a, b->m_right, useDelta))
                    hit = 1;
            }
            return hit;
        }
    }
    return 0;
}


// ---- src/global/TestBoundingBoxTreesForIntersection.cpp


extern _D3DVECTOR *BoundingBoxTreeVertices;
extern BoundingBoxTreeType BoundingBoxTreeAType;
extern _D3DMATRIX AtoBSpaceTransform;
extern _D3DMATRIX BtoASpaceTransform;
extern _D3DMATRIX AtoADeltaTransform;

int TestBoundingBoxTreesForIntersectionBranch(BoundingBoxTree *a, BoundingBoxTree *b, int useDelta);

// Sets up the transforms between the two trees' spaces (and the optional
// delta transform of a over the step) and tests the trees against each other.
int TestBoundingBoxTreesForIntersection(BoundingBoxTree *a, BoundingBoxTree *b, _D3DMATRIX *matrixA,
                                        _D3DMATRIX *matrixB, BoundingBoxTreeType type, _D3DVECTOR *vertices,
                                        _D3DMATRIX *delta)
{
    BoundingBoxTreeVertices = vertices;
    BoundingBoxTreeAType = type;
    BtoASpaceTransform = *matrixA;
    Vu0InversMatrix(&BtoASpaceTransform);
    MulMatrixCopy(&BtoASpaceTransform, BtoASpaceTransform, *matrixB);
    AtoBSpaceTransform = *matrixB;
    Vu0InversMatrix(&AtoBSpaceTransform);
    MulMatrixCopy(&AtoBSpaceTransform, AtoBSpaceTransform, *matrixA);
    if (delta) {
        AtoADeltaTransform = *delta;
        return TestBoundingBoxTreesForIntersectionBranch(a, b, 1);
    }
    return TestBoundingBoxTreesForIntersectionBranch(a, b, 0);
}


// ---- src/global/TestBoundingBoxTreesForOverlap.cpp
#pragma inline_depth(4)

// Do the root boxes of the two trees overlap?  Also leaves the B-to-A
// transform in BtoASpaceTransform.
int TestBoundingBoxTreesForOverlap(BoundingBoxTree *a, BoundingBoxTree *b, _D3DMATRIX *matrixA, _D3DMATRIX *matrixB)
{
    BtoASpaceTransform = *matrixA;
    Vu0InversMatrix(&BtoASpaceTransform);
    MulMatrixCopy(&BtoASpaceTransform, BtoASpaceTransform, *matrixB);
    return OBBOverlapTestInline(&a->m_min, &a->m_max, b->m_min, b->m_max, &BtoASpaceTransform);
}


// ---- src/global/ModifyBoundingBox.cpp






// out = rotation part of m applied to v.
static inline void Vu0RotateVector(_D3DVECTOR *out, _D3DMATRIX *m, _D3DVECTOR v)
{
    asm("lqc2 vf4, 0x0(%1); lqc2 vf5, 0x10(%1); lqc2 vf6, 0x20(%1); lqc2 vf8, 0x0(%2); vmulax.xyz ACC, vf4, vf8x; vmadday.xyz ACC, vf5, vf8y; vmaddz.xyz vf9, vf6, vf8z; vadd.xyzw vf9, vf9, vf0; sqc2 vf9, 0x0(%0)"
        : : "r"(out), "r"(m), "r"(&v) : "memory");
}

// Grow the box (center, half) so it also holds the box moved by m.
void ModifyBoundingBox(_D3DVECTOR *center, _D3DVECTOR *half, _D3DMATRIX *m)
{
    _D3DVECTOR axis[3];
    axis[0] = _D3DVECTOR(half->x, 0.0f, 0.0f);
    axis[1] = _D3DVECTOR(0.0f, half->y, 0.0f);
    axis[2] = _D3DVECTOR(0.0f, 0.0f, half->z);
    Vu0RotateVector(&axis[0], m, axis[0]);
    Vu0RotateVector(&axis[1], m, axis[1]);
    Vu0RotateVector(&axis[2], m, axis[2]);
    float extent[3];
    float moved[3];
    float offset[3];
    float shift[3];
    extent[0] = half->x;
    extent[1] = half->y;
    extent[2] = half->z;
    moved[0] = Fabs(axis[2].x) + (Fabs(axis[0].x) + Fabs(axis[1].x));
    moved[1] = Fabs(axis[2].y) + (Fabs(axis[0].y) + Fabs(axis[1].y));
    moved[2] = Fabs(axis[2].z) + (Fabs(axis[0].z) + Fabs(axis[1].z));
    _D3DVECTOR c;
    Vu0TransformPoint(m, center, &c);
    offset[0] = c.x - center->x;
    offset[1] = c.y - center->y;
    offset[2] = c.z - center->z;
    for (int i = 0; i < 3; i++) {
        float lo = offset[i] - moved[i];
        float hi = offset[i] + moved[i];
        lo = Fmin(-extent[i], lo);
        hi = Fmax(extent[i], hi);
        shift[i] = 0.5f * (lo + hi);
        extent[i] = 0.5f * (hi - lo);
    }
    center->x += shift[0];
    center->y += shift[1];
    center->z += shift[2];
    half->x = extent[0];
    half->y = extent[1];
    half->z = extent[2];
}


// ---- src/global/ModifyBoundingBox_0020A050.cpp






// out = rotation part of m applied to v.

// Grow the box (center, half) so it also holds the box (otherCenter,
// otherHalf) moved by m.
void ModifyBoundingBox(_D3DVECTOR *center, _D3DVECTOR *half, _D3DVECTOR otherCenter, _D3DVECTOR otherHalf, _D3DMATRIX *m)
{
    _D3DVECTOR axis[3];
    axis[0] = _D3DVECTOR(otherHalf.x, 0.0f, 0.0f);
    axis[1] = _D3DVECTOR(0.0f, otherHalf.y, 0.0f);
    axis[2] = _D3DVECTOR(0.0f, 0.0f, otherHalf.z);
    Vu0RotateVector(&axis[0], m, axis[0]);
    Vu0RotateVector(&axis[1], m, axis[1]);
    Vu0RotateVector(&axis[2], m, axis[2]);
    float extent[3];
    float moved[3];
    float offset[3];
    float shift[3];
    extent[0] = half->x;
    extent[1] = half->y;
    extent[2] = half->z;
    moved[0] = Fabs(axis[2].x) + (Fabs(axis[0].x) + Fabs(axis[1].x));
    moved[1] = Fabs(axis[2].y) + (Fabs(axis[0].y) + Fabs(axis[1].y));
    moved[2] = Fabs(axis[2].z) + (Fabs(axis[0].z) + Fabs(axis[1].z));
    Vu0TransformPoint(m, &otherCenter, &otherCenter);
    offset[0] = otherCenter.x - center->x;
    offset[1] = otherCenter.y - center->y;
    offset[2] = otherCenter.z - center->z;
    for (int i = 0; i < 3; i++) {
        float lo = offset[i] - moved[i];
        float hi = offset[i] + moved[i];
        lo = Fmin(-extent[i], lo);
        hi = Fmax(extent[i], hi);
        shift[i] = 0.5f * (lo + hi);
        extent[i] = 0.5f * (hi - lo);
    }
    center->x += shift[0];
    center->y += shift[1];
    center->z += shift[2];
    half->x = extent[0];
    half->y = extent[1];
    half->z = extent[2];
}


// ---- src/global/SphereIntersectsBoundingBoxTree.cpp
struct _D3DVECTOR;
struct _D3DMATRIX;
class BoundingBoxTree;

extern _D3DVECTOR *BoundingBoxTreeVertices;

int SphereIntersectsBoundingBoxTree(_D3DVECTOR *center, float a, float b, BoundingBoxTree *tree, _D3DMATRIX *m, int flags, int g);

int SphereIntersectsBoundingBoxTree(_D3DVECTOR *center, float a, float b, BoundingBoxTree *tree, _D3DMATRIX *m, int flags, _D3DVECTOR *vertices, int g)
{
    BoundingBoxTreeVertices = vertices;
    return SphereIntersectsBoundingBoxTree(center, a, b, tree, m, flags, g);
}


// ---- src/global/SphereIntersectsBoundingBoxTree_0020A470.cpp
#pragma inline_depth(5)

int OBBVsSphereOverlapTest(_D3DVECTOR *center, _D3DVECTOR *half, _D3DVECTOR &sphereCenter, float radius,
                           float radiusSquared, _D3DMATRIX *transform);

// a . b with b a temporary.
static inline float Vu0DotProductRef(_D3DVECTOR *a, const _D3DVECTOR &b)
{
    float r;
    asm("lqc2 vf4, 0x0(%1); lqc2 vf5, 0x0(%2); vmul.xyz vf5, vf4, vf5; vaddy.x vf5, vf5, vf5y; vaddz.x vf5, vf5, vf5z; qmfc2 $2, vf5; mtc1 $2, %0"
        : "=f"(r) : "r"(a), "r"(&b) : "$2");
    return r;
}

// vf4 . vf4 (vf4 left by a previous Vu0DotProduct).
static inline float Vu0LengthSquaredVf4()
{
    float r;
    asm("vmove.xyzw vf5, vf4; vmul.xyz vf5, vf4, vf5; vaddy.x vf5, vf5, vf5y; vaddz.x vf5, vf5, vf5z; qmfc2 $2, vf5; mtc1 $2, %0"
        : "=f"(r) : : "$2");
    return r;
}

#define FACE_VERTEX(leaf, k) BoundingBoxTreeVertices[(leaf)->m_face.m_vertexIndex[k]]

// Is p (on the face's plane) inside the face?  Tested at two corners: the
// edges from each must have p on their inner side.
static inline int PointInFace(_D3DVECTOR *p, FaceLeaf *leaf)
{
    _D3DVECTOR e1, e2, d;
    _D3DVECTOR normal(leaf->m_face.m_normalX, leaf->m_face.m_normalY, leaf->m_face.m_normalZ);
    e1 = FACE_VERTEX(leaf, 2) - FACE_VERTEX(leaf, 1);
    e2 = FACE_VERTEX(leaf, 0) - FACE_VERTEX(leaf, 1);
    d = *p - FACE_VERTEX(leaf, 1);
    _D3DVECTOR cross;
    Vu0CrossProduct(&cross, &e1, &d);
    if (Vu0DotProduct(&normal, &cross) < 0.0f)
        return 0;
    Vu0CrossProduct(&cross, &d, &e2);
    if (Vu0DotProduct(&normal, &cross) < 0.0f)
        return 0;
    e1 = FACE_VERTEX(leaf, 0) - FACE_VERTEX(leaf, 2);
    e2 = FACE_VERTEX(leaf, 1) - FACE_VERTEX(leaf, 2);
    d = *p - FACE_VERTEX(leaf, 2);
    Vu0CrossProduct(&cross, &e1, &d);
    if (Vu0DotProduct(&normal, &cross) < 0.0f)
        return 0;
    Vu0CrossProduct(&cross, &d, &e2);
    if (Vu0DotProduct(&normal, &cross) < 0.0f)
        return 0;
    return 1;
}

static inline float SquareRoot(float x)
{
    return Fsqrt(x);
}

// |v|^2.
static inline float LengthSquared(const _D3DVECTOR &v)
{
    return v.x * v.x + v.y * v.y + v.z * v.z;
}

// |v|.
static inline float Length(const _D3DVECTOR &v)
{
    return SquareRoot(v.x * v.x + v.y * v.y + v.z * v.z);
}

// v clamped to [lo, hi].
static inline float Clamp(float v, float lo, float hi)
{
    v = Fmin(v, hi);
    v = Fmax(v, lo);
    return v;
}

// The distance from c to the edge a-b.
static inline float DistanceToEdge(_D3DVECTOR *c, _D3DVECTOR *a, _D3DVECTOR *b)
{
    _D3DVECTOR e = *b - *a;
    float t = Vu0DotProductRef(&e, *c - *a) / Vu0LengthSquaredVf4();
    t = Clamp(t, 0.0f, 1.0f);
    return Length(*c - (e * t + *a));
}

// Records a hit at p (dist from the face plane) in CurrentCollisionData if
// it is nearer than the one already there. (A helper: its `return hit` copy
// leaves an empty join block, which keeps the record arm's `b` as in retail.)
static inline int RecordSphereHit(int hit, float dist, _D3DVECTOR &normal, _D3DVECTOR &p)
{
    SphereIntersectionData *data = (SphereIntersectionData *)CurrentCollisionData;
    if (data && data->m_count < data->m_maxHits) {
        if (data->m_count == 0 || dist < data->m_distance) {
            Vu0Normalize(&data->m_normals[0], &normal);
            data->m_positions[0] = p;
            data->m_distance = dist;
            data->m_transformed[0] = 0;
            data->m_count = 1;
        } else {
            hit = 0;
        }
    }
    return hit;
}

// The sphere against a face leaf, recording the hit nearest the face plane
// in CurrentCollisionData. Only the face's front side counts.
static inline int SphereVsFaceRecord(_D3DVECTOR c, float radius, FaceLeaf *leaf, _D3DMATRIX *m)
{
    int hit = 0;
    _D3DVECTOR normal(leaf->m_face.m_normalX, leaf->m_face.m_normalY, leaf->m_face.m_normalZ);
    Vu0InverseTransformPoint(&c, c, m);
    float dist = Vu0DotProductRef(&normal, c - FACE_VERTEX(leaf, 0));
    if (dist < 0.0f || dist > radius) {
        hit = 0;
    } else {
        _D3DVECTOR p = c - dist * normal;
        if (PointInFace(&p, leaf)) {
            hit = 1;
        } else {
            float d = DistanceToEdge(&c, &FACE_VERTEX(leaf, 0), &FACE_VERTEX(leaf, 1));
            if (Fabs(d) < radius) {
                hit = 1;
            } else {
                d = DistanceToEdge(&c, &FACE_VERTEX(leaf, 1), &FACE_VERTEX(leaf, 2));
                if (Fabs(d) < radius) {
                    hit = 1;
                } else {
                    d = DistanceToEdge(&c, &FACE_VERTEX(leaf, 2), &FACE_VERTEX(leaf, 0));
                    if (Fabs(d) < radius)
                        hit = 1;
                }
            }
        }
        if (hit)
            hit = RecordSphereHit(hit, dist, normal, p);
    }
    return hit;
}

// The sphere against a face leaf, either side. One variable d holds all three
// edge distances (it shares one register, as in retail).
static inline int SphereVsFace(_D3DVECTOR c, float radius, FaceLeaf *leaf, _D3DMATRIX *m)
{
    _D3DVECTOR normal(leaf->m_face.m_normalX, leaf->m_face.m_normalY, leaf->m_face.m_normalZ);
    Vu0InverseTransformPoint(&c, c, m);
    float dist = Vu0DotProductRef(&normal, c - FACE_VERTEX(leaf, 0));
    if (Fabs(dist) > radius)
        return 0;
    _D3DVECTOR p = c - dist * normal;
    if (PointInFace(&p, leaf))
        return 1;
    float d = DistanceToEdge(&c, &FACE_VERTEX(leaf, 0), &FACE_VERTEX(leaf, 1));
    if (Fabs(d) < radius)
        return 1;
    d = DistanceToEdge(&c, &FACE_VERTEX(leaf, 1), &FACE_VERTEX(leaf, 2));
    if (Fabs(d) < radius)
        return 1;
    d = DistanceToEdge(&c, &FACE_VERTEX(leaf, 2), &FACE_VERTEX(leaf, 0));
    if (Fabs(d) < radius)
        return 1;
    return 0;
}

// Does the sphere (center, radius) intersect the tree (in m's space)?  Face
// leaves (faces set) are tested against the sphere, recording the nearest hit
// if asked; point leaves must lie inside it.
int SphereIntersectsBoundingBoxTree(_D3DVECTOR *center, float radius, float radiusSquared, BoundingBoxTree *tree, _D3DMATRIX *m, int faces, int record)
{
    if (tree->m_size < 0.0f) {
        if (faces) {
            if (record)
                return SphereVsFaceRecord(*center, radius, (FaceLeaf *)tree, m);
            return SphereVsFace(*center, radius, (FaceLeaf *)tree, m);
        }
        _D3DVECTOR world;
        Vu0TransformPointOut(&world, m, &tree->m_max);
        return (bool)(LengthSquared(world - *center) < radiusSquared);
    }
    if (OBBVsSphereOverlapTest(&tree->m_min, &tree->m_max, *center, radius, radiusSquared, m)) {
        int hit = SphereIntersectsBoundingBoxTree(center, radius, radiusSquared, tree->m_left, m, faces, record);
        if (!hit)
            hit = SphereIntersectsBoundingBoxTree(center, radius, radiusSquared, tree->m_right, m, faces, record);
        return hit;
    }
    return 0;
}


// ---- src/global/BoxIntersectsBoundingBoxTree.cpp
struct _D3DVECTOR;
struct _D3DMATRIX;
class BoundingBoxTree;

extern _D3DVECTOR *BoundingBoxTreeVertices;

int BoxIntersectsBoundingBoxTree(_D3DVECTOR *min, _D3DVECTOR *max, _D3DMATRIX *m, BoundingBoxTree *tree, _D3DMATRIX *treeMatrix, int flags);

int BoxIntersectsBoundingBoxTree(_D3DVECTOR *min, _D3DVECTOR *max, _D3DMATRIX *m, BoundingBoxTree *tree, _D3DMATRIX *treeMatrix, int flags, _D3DVECTOR *vertices)
{
    BoundingBoxTreeVertices = vertices;
    return BoxIntersectsBoundingBoxTree(min, max, m, tree, treeMatrix, flags);
}


// ---- src/global/BoxIntersectsBoundingBoxTree_0020BEA0.cpp
#pragma inline_depth(5)

// Is the point (in world space) inside the box (center, half) in m's space?
static inline int PointInBox(_D3DVECTOR *center, _D3DVECTOR *half, _D3DVECTOR *point, _D3DMATRIX *m)
{
    _D3DMATRIX toBox = *m;
    Vu0InversMatrix(&toBox);
    _D3DVECTOR p;
    Vu0TransformPoint(&toBox, point, &p);
    if (p.x >= center->x - half->x && p.x <= center->x + half->x &&
        p.y >= center->y - half->y && p.y <= center->y + half->y &&
        p.z >= center->z - half->z && p.z <= center->z + half->z)
        return 1;
    return 0;
}

// Does the box (center, half) in m's space intersect the tree (in
// treeMatrix's space)?  Face leaves (flags set) get the separating-axis test,
// point leaves a point-in-box test.
int BoxIntersectsBoundingBoxTree(_D3DVECTOR *center, _D3DVECTOR *half, _D3DMATRIX *m, BoundingBoxTree *tree, _D3DMATRIX *treeMatrix, int flags)
{
    if (tree->m_size < 0.0f) {
        if (flags) {
            _D3DMATRIX toBox = *m;
            Vu0InversMatrix(&toBox);
            MulMatrixCopy(&toBox, toBox, *treeMatrix);
            return OBBVsFaceOverlapTest(center, half, (FaceLeaf *)tree, &toBox);
        } else {
            _D3DVECTOR world;
            Vu0TransformPointOut(&world, treeMatrix, &tree->m_max);
            return PointInBox(center, half, &world, m);
        }
    }
    _D3DMATRIX toBox = *m;
    Vu0InversMatrix(&toBox);
    MulMatrixCopy(&toBox, toBox, *treeMatrix);
    if (OBBOverlapTestInline(center, half, tree->m_min, tree->m_max, &toBox)) {
        int hit = BoxIntersectsBoundingBoxTree(center, half, m, tree->m_left, treeMatrix, flags);
        if (!hit)
            hit = BoxIntersectsBoundingBoxTree(center, half, m, tree->m_right, treeMatrix, flags);
        return hit;
    }
    return 0;
}


// ---- src/global/RadiusedLineSegmentIntersectsBoundingBoxTree.cpp
struct _D3DVECTOR;
struct _D3DMATRIX;
class BoundingBoxTree;

extern _D3DVECTOR *BoundingBoxTreeVertices;

int RadiusedLineSegmentIntersectsBoundingBoxTree(_D3DVECTOR *line, float a, float b, BoundingBoxTree *tree, _D3DMATRIX *m, int flags);

int RadiusedLineSegmentIntersectsBoundingBoxTree(_D3DVECTOR *line, float a, float b, BoundingBoxTree *tree, _D3DMATRIX *m, int flags, _D3DVECTOR *vertices)
{
    BoundingBoxTreeVertices = vertices;
    return RadiusedLineSegmentIntersectsBoundingBoxTree(line, a, b, tree, m, flags);
}


// ---- src/global/RadiusedLineSegmentIntersectsBoundingBoxTree_0020D7A0.cpp
#pragma inline_depth(6)

int OBBVsRadiusedLineSegmentOverlapTest(_D3DVECTOR *center, _D3DVECTOR *half, _D3DVECTOR *line, float radius,
                                         float radiusSquared, _D3DMATRIX *m);

// out = inverse(m) * v for a rigid m, rotation only (w ends up 1); v by
// value and before m, as Vu0InverseTransformPoint.
static inline void Vu0InverseApplyMatrixVector(_D3DVECTOR *out, _D3DVECTOR v, _D3DMATRIX *m)
{
    asm("lqc2 vf1, 0x0(%1); lqc2 vf2, 0x10(%1); lqc2 vf3, 0x20(%1); lqc2 vf4, 0x30(%1); vmulw.w vf21, vf0, vf0w; vsub.xyzw vf30, vf30, vf30; vadd.xyzw vf5, vf1, vf30; vadd.xyzw vf6, vf2, vf30; vadd.xyzw vf7, vf3, vf30; vadd.xyzw vf8, vf4, vf30; vaddy.x vf6, vf0, vf1y; vaddz.x vf7, vf0, vf1z; vaddz.y vf7, vf0, vf2z; vaddx.y vf5, vf0, vf2x; vaddx.z vf5, vf0, vf3x; vaddy.z vf6, vf0, vf3y; lqc2 vf20, 0x0(%2); vmulax.xyz ACC, vf5, vf20x; vmadday.xyz ACC, vf6, vf20y; vmaddz.xyz vf21, vf7, vf20z; sqc2 vf21, 0x0(%0)"
        : : "r"(out), "r"(m), "r"(&v) : "memory");
}

// a . b with a a temporary.
static inline float Vu0DotProductRefA(const _D3DVECTOR &a, _D3DVECTOR *b)
{
    float r;
    asm("lqc2 vf4, 0x0(%1); lqc2 vf5, 0x0(%2); vmul.xyz vf5, vf4, vf5; vaddy.x vf5, vf5, vf5y; vaddz.x vf5, vf5, vf5z; qmfc2 $2, vf5; mtc1 $2, %0"
        : "=f"(r) : "r"(&a), "r"(b) : "$2");
    return r;
}

#define FACE_VERTEX(leaf, k) BoundingBoxTreeVertices[(leaf)->m_face.m_vertexIndex[k]]

// Is p (on the face's plane) inside the face?  Tested at two corners: the
// edges from each must have p on their inner side.

// |v|.

// The distance between the segment start + t dir and the edge a + s e
// (t, s in [0, 1]); 0 for parallel lines.
static inline float SegmentToEdgeDistance(_D3DVECTOR *start, _D3DVECTOR *dir, _D3DVECTOR *e, _D3DVECTOR *a)
{
    _D3DVECTOR cross;
    Vu0CrossProduct(&cross, dir, e);
    if (cross.x * cross.x + cross.y * cross.y + cross.z * cross.z == 0.0f)
        return 0.0f;
    _D3DVECTOR n;
    Vu0CrossProduct(&n, dir, &cross);
    Vu0Normalize(&n, &n);
    float plane = -Vu0DotProduct(&n, a);
    float t = -(plane + Vu0DotProduct(&n, start)) / Vu0DotProduct(&n, dir);
    Vu0CrossProduct(&n, e, &cross);
    Vu0Normalize(&n, &n);
    plane = -Vu0DotProduct(&n, start);
    float s = -(plane + Vu0DotProduct(&n, a)) / Vu0DotProduct(&n, e);
    if (t >= 0.0f && t <= 1.0f && s >= 0.0f && s <= 1.0f)
        return Length((t * *dir + *start) - (s * *e + *a));
    _D3DVECTOR onSegment = t * *dir + *start;
    _D3DVECTOR onEdge = s * *e + *a;
    float d1 = Length(Fmax(Fmin(Vu0DotProductRefA(onSegment - *a, e), 1.0f), 0.0f) * *e + *a - onSegment);
    float d2 = Length(Fmax(Fmin(Vu0DotProductRefA(onEdge - *start, dir), 1.0f), 0.0f) * *dir + *start - onEdge);
    if (d1 > d2)
        return d1;
    return d2;
}

// The radiused segment line[0]-line[1] (in the tree's space through m)
// against a face leaf.
static inline int RadiusedLineSegmentVsFace(_D3DVECTOR *line, float radius, FaceLeaf *leaf, _D3DMATRIX *m)
{
    _D3DVECTOR start, dir;
    _D3DVECTOR normal(leaf->m_face.m_normalX, leaf->m_face.m_normalY, leaf->m_face.m_normalZ);
    Vu0InverseTransformPoint(&start, line[0], m);
    Vu0InverseApplyMatrixVector(&dir, line[1] - line[0], m);
    float d1 = Vu0DotProduct(&normal, &start);
    float d2 = Vu0DotProductVf4(&dir);
    float t = -(leaf->m_face.m_distance + d1) / d2;
    t = Fmin(t, 1.0f);
    t = Fmax(t, 0.0f);
    _D3DVECTOR p;
    p = t * dir + start;
    float dist = Vu0DotProductRefA(p - FACE_VERTEX(leaf, 0), &normal);
    if (Fabs(dist) > radius)
        return 0;
    _D3DVECTOR q = p - dist * normal;
    if (PointInFace(&q, leaf))
        return 1;
    _D3DVECTOR e = FACE_VERTEX(leaf, 1) - FACE_VERTEX(leaf, 0);
    if (radius < SegmentToEdgeDistance(&start, &dir, &e, &FACE_VERTEX(leaf, 0)))
        return 1;
    e = FACE_VERTEX(leaf, 2) - FACE_VERTEX(leaf, 1);
    if (radius < SegmentToEdgeDistance(&start, &dir, &e, &FACE_VERTEX(leaf, 1)))
        return 1;
    e = FACE_VERTEX(leaf, 0) - FACE_VERTEX(leaf, 2);
    if (radius < SegmentToEdgeDistance(&start, &dir, &e, &FACE_VERTEX(leaf, 2)))
        return 1;
    return 0;
}

// Does the radiused segment line[0]-line[1] intersect the tree (in m's
// space)?  Only face leaves (faces set) can be hit.
int RadiusedLineSegmentIntersectsBoundingBoxTree(_D3DVECTOR *line, float radius, float radiusSquared, BoundingBoxTree *tree, _D3DMATRIX *m, int faces)
{
    if (tree->m_size < 0.0f) {
        if (faces)
            return RadiusedLineSegmentVsFace(line, radius, (FaceLeaf *)tree, m);
        return 0;
    }
    if (OBBVsRadiusedLineSegmentOverlapTest(&tree->m_min, &tree->m_max, line, radius, radiusSquared, m)) {
        int hit = RadiusedLineSegmentIntersectsBoundingBoxTree(line, radius, radiusSquared, tree->m_left, m, faces);
        if (!hit)
            hit = RadiusedLineSegmentIntersectsBoundingBoxTree(line, radius, radiusSquared, tree->m_right, m, faces);
        return hit;
    }
    return 0;
}


// ---- src/global/DeleteBoundingBoxTree.cpp

void DeleteBoundingBoxTree(BoundingBoxTree *tree)
{
    if (tree->m_size >= 0) {
        DeleteBoundingBoxTree(tree->m_left);
        DeleteBoundingBoxTree(tree->m_right);
    }
    delete tree;
}


// ---- src/global/RayIntersectBoundingBoxTreeFace.cpp
#pragma inline_depth(5)

// x where the line y = c.y crosses the edge a-b (2D, in the projection plane).

// Does the segment start + [0, length] * dir hit the face from its front?
// On a hit nearer than *t (or with *t negative) records t, the point and the
// face normal.
int RayIntersectBoundingBoxTreeFace(BoundingBoxTreeFace *face, _D3DVECTOR dir, _D3DVECTOR start, float length,
                                    _D3DVECTOR *point, _D3DVECTOR *normal, float *t)
{
    _D3DVECTOR hit;
    _D3DVECTOR faceNormal(face->m_normalX, face->m_normalY, face->m_normalZ);
    _D3DVECTOR *v0 = &BoundingBoxTreeVertices[face->m_vertexIndex[0]];
    _D3DVECTOR *v1 = &BoundingBoxTreeVertices[face->m_vertexIndex[1]];
    _D3DVECTOR *v2 = &BoundingBoxTreeVertices[face->m_vertexIndex[2]];
    _D3DVECTOR absNormal, p0, p1, p2, q, c1, c2;
    float d = Vu0DotProduct(&faceNormal, &dir);
    if (d > 0.0f)
        return 0;
    d *= length;
    float s = -(face->m_distance + Vu0DotProductVf4(&start)) / d;
    if (s >= 0.0f && s <= 1.0f) {
        hit = start + length * (s * dir);
        absNormal.x = Fabs(face->m_normalX);
        absNormal.y = Fabs(face->m_normalY);
        absNormal.z = Fabs(face->m_normalZ);
        if (absNormal.z > absNormal.x && absNormal.z > absNormal.y) {
            p0.x = v0->x;
            p0.y = v0->y;
            p1.x = v1->x;
            p1.y = v1->y;
            p2.x = v2->x;
            p2.y = v2->y;
            q.x = hit.x;
            q.y = hit.y;
        } else if (absNormal.y > absNormal.x) {
            p0.x = v0->x;
            p0.y = v0->z;
            p1.x = v1->x;
            p1.y = v1->z;
            p2.x = v2->x;
            p2.y = v2->z;
            q.x = hit.x;
            q.y = hit.z;
        } else {
            p0.x = v0->y;
            p0.y = v0->z;
            p1.x = v1->y;
            p1.y = v1->z;
            p2.x = v2->y;
            p2.y = v2->z;
            q.x = hit.y;
            q.y = hit.z;
        }
        c2.y = c1.y = q.y;
        if (q.y <= p0.y && q.y >= p1.y) {
            if (q.y > p2.y) {
                c1.x = CrossingX(p0, p1, c1);
                c2.x = CrossingX(p0, p2, c2);
            } else {
                c1.x = CrossingX(p0, p1, c1);
                c2.x = CrossingX(p1, p2, c2);
            }
        } else if (q.y <= p1.y && q.y >= p0.y) {
            if (q.y > p2.y) {
                c1.x = CrossingX(p0, p1, c1);
                c2.x = CrossingX(p1, p2, c2);
            } else {
                c1.x = CrossingX(p0, p1, c1);
                c2.x = CrossingX(p0, p2, c2);
            }
        } else if (q.y <= p2.y && q.y >= p1.y) {
            if (q.y > p0.y) {
                c1.x = CrossingX(p1, p2, c1);
                c2.x = CrossingX(p0, p2, c2);
            } else {
                c1.x = CrossingX(p0, p1, c1);
                c2.x = CrossingX(p1, p2, c2);
            }
        } else if (q.y <= p1.y && q.y >= p2.y) {
            if (q.y > p0.y) {
                c1.x = CrossingX(p0, p1, c1);
                c2.x = CrossingX(p1, p2, c2);
            } else {
                c1.x = CrossingX(p0, p2, c1);
                c2.x = CrossingX(p1, p2, c2);
            }
        } else if (q.y <= p0.y && q.y >= p2.y) {
            if (q.y > p1.y) {
                c1.x = CrossingX(p0, p1, c1);
                c2.x = CrossingX(p0, p2, c2);
            } else {
                c1.x = CrossingX(p0, p2, c1);
                c2.x = CrossingX(p1, p2, c2);
            }
        } else if (q.y <= p2.y && q.y >= p0.y) {
            if (q.y > p1.y) {
                c1.x = CrossingX(p1, p2, c1);
                c2.x = CrossingX(p0, p2, c2);
            } else {
                c1.x = CrossingX(p0, p1, c1);
                c2.x = CrossingX(p0, p2, c2);
            }
        } else {
            return 0;
        }
        if ((q.x <= c1.x && q.x >= c2.x) || (q.x <= c2.x && q.x >= c1.x)) {
            if (t) {
                if (s < *t || *t < 0.0f) {
                    *t = s;
                    if (point)
                        *point = hit;
                    if (normal)
                        *normal = faceNormal;
                }
            }
            return 1;
        }
        return 0;
    }
    return 0;
}


// ---- src/global/RayBundleIntersectsBoundingBoxTree_002100D0.cpp
#pragma inline_depth(6)

int RayBundleIntersectsBoundingBoxTree(DynamicRayCollisionObject *rays, BoundingBoxTree *tree, RayToFaceIntersectionData *data);

// Tests the ray bundle (in the tree's space) against a face tree: at a leaf
// every ray against the face, recording each ray's nearest hit and the
// nearest hit overall; at a node, descends while any ray may reach the box.
int RayBundleIntersectsBoundingBoxTree(DynamicRayCollisionObject *rays, BoundingBoxTree *tree, RayToFaceIntersectionData *data)
{
    _D3DVECTOR unused, delta, absDelta;
    int i;
    int n;
    _D3DVECTOR *dir;
    _D3DVECTOR *start;
    float *length;
    if (tree->m_size < 0.0f) {
        n = rays->m_numRays;
        dir = rays->m_localDirections;
        start = rays->m_localStarts;
        length = rays->m_lengths;
        // Declared in this order for retail's registers.
        int *hit;
        _D3DVECTOR *point;
        float *t;
        _D3DVECTOR *normal;
        point = data->m_rayPoints;
        normal = data->m_rayNormals;
        t = data->m_rayT;
        hit = data->m_rayHit;
        BoundingBoxTreeFace *face = (BoundingBoxTreeFace *)&tree->m_radius;
        int *count = &data->m_numRays;
        for (i = 0; i < n; i++) {
            if (RayIntersectFace(face, *dir, *start, *length, point, normal, t)) {
                *hit = 1;
                *count++;       // as retail: moves the pointer, the count stays
                if (*t < data->m_t) {
                    data->m_t = *t;
                    data->m_normal = *normal;
                    data->m_point = *point;
                    data->m_penetration = Vu0DotProduct(&((1.0f - *t) * *dir), normal) * *normal;
                }
            }
            dir++;
            start++;
            length++;
            point++;
            normal++;
            t++;
            hit++;
        }
    } else {
        delta = tree->m_min - rays->m_localCenter;
        if (Fabs(delta.x) <= tree->m_max.x + rays->m_radius && Fabs(delta.y) <= tree->m_max.y + rays->m_radius &&
            Fabs(delta.z) <= tree->m_max.z + rays->m_radius) {
            n = rays->m_numRays;
            dir = rays->m_localDirections;
            start = rays->m_localStarts;
            length = rays->m_lengths;
            i = 0;
            while (i < n) {
                delta = tree->m_min - *start;
                absDelta.x = Fabs(delta.x);
                absDelta.y = Fabs(delta.y);
                absDelta.z = Fabs(delta.z);
                // The third test checks x, not z, as in retail.
                if (!((absDelta.x > tree->m_max.x &&
                     *length * (delta.x * dir->x) < absDelta.x * (absDelta.x - tree->m_max.x)) ||
                    (absDelta.y > tree->m_max.y &&
                     *length * (delta.y * dir->y) < absDelta.y * (absDelta.y - tree->m_max.y)) ||
                    (absDelta.x > tree->m_max.x &&
                     *length * (delta.z * dir->z) < absDelta.z * (absDelta.z - tree->m_max.z)))) {
                    i = n;
                    RayBundleIntersectsBoundingBoxTree(rays, tree->m_left, data);
                    RayBundleIntersectsBoundingBoxTree(rays, tree->m_right, data);
                } else {
                    i++;
                    dir++;
                    start++;
                    length++;
                }
            }
        }
    }
    return 1;
}


// ---- src/global/RayBundleIntersectsBoundingBoxTree.cpp
struct _D3DVECTOR;
class BoundingBoxTree;
class DynamicRayCollisionObject;
struct RayToFaceIntersectionData;

extern _D3DVECTOR *BoundingBoxTreeVertices;

int RayBundleIntersectsBoundingBoxTree(DynamicRayCollisionObject *rays, BoundingBoxTree *tree, RayToFaceIntersectionData *data);

int RayBundleIntersectsBoundingBoxTree(DynamicRayCollisionObject *rays, BoundingBoxTree *tree, _D3DVECTOR *vertices, RayToFaceIntersectionData *data)
{
    BoundingBoxTreeVertices = vertices;
    return RayBundleIntersectsBoundingBoxTree(rays, tree, data);
}


// ---- src/_D3DVECTOR/ctor_00211000.cpp
// The weak _D3DVECTOR ctor (0x211000) is emitted for the static
// initializer's arrays (end of file).


// ---- static initializer (was src/sinit/BoundingBoxTree.cpp): the axis
// vectors and these arrays.
