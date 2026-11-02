// BoundingBoxTreeBuild.cpp  @ 0x00211020 (text 0x211020-0x213C10)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's BoundingBoxTreeBuild.cpp as one translation unit: building face and
// ray trees (splits, bounding boxes, separating planes) and reading face trees,
// then the static initializer (only the axis vectors). It has no .rodata of its
// own.


#include "d3dtypes.h"
#include "sinit/AxisVectors.h"
#include "global/Vu0Inline.h"
#include "BoundingBoxTree/BoundingBoxTree.h"
#include "DataAccess/DataAccess.h"

// ---- src/global/SeperateFaces.cpp

// The build structures keep their vectors 16-aligned (quadword copies).
typedef _D3DVECTOR AlignedVector __attribute__((aligned(16)));

struct BoundingBoxFaceBuild {
    unsigned int m_vertex[3];       // +0x00
    AlignedVector m_normal;         // +0x10
    AlignedVector m_center;         // +0x20
    float m_x30;                    // +0x30
    float m_d;                      // +0x34
} __attribute__((aligned(16)));    // size 0x40 (16-aligned: its copies are quadword moves)

// Splits faces into two sets by their centres' coordinate on one axis.  If
// either set ends up empty, the list is just cut in half and 0 is returned.
int SeperateFaces(BoundingBoxFaceBuild *faces, BoundingBoxFaceBuild *facesA, BoundingBoxFaceBuild *facesB,
                  int numFaces, int *numA, int *numB, float split, int axis)
{
    bool above = 0;     // (keeps its last value for an unknown axis)
    int i;

    *numA = 0;
    *numB = 0;
    for (i = 0; i < numFaces; i++) {
        switch (axis) {
        case 0:
            above = faces[i].m_center.x > split;
            break;
        case 1:
            above = faces[i].m_center.y > split;
            break;
        case 2:
            above = faces[i].m_center.z > split;
            break;
        }
        if (above) {
            facesA[*numA] = faces[i];
            (*numA)++;
        } else {
            facesB[*numB] = faces[i];
            (*numB)++;
        }
    }
    if (*numB == 0 || *numA == 0) {
        *numA = numFaces / 2;
        *numB = numFaces - *numA;
        for (i = 0; i < *numA; i++)
            facesA[i] = faces[i];
        for (i = 0; i < *numB; i++)
            facesB[i] = faces[i + *numA];
        return 0;
    }
    return 1;
}


// ---- src/global/SeperateRays.cpp

// Splits rays (origin/direction pairs) into two sets by their centres'
// coordinate on one axis.  If either set ends up empty, the counts are
// just halved and 0 is returned.
int SeperateRays(_D3DVECTOR *rays, _D3DVECTOR *centers, _D3DVECTOR *raysA, _D3DVECTOR *raysB,
                 _D3DVECTOR *centersA, _D3DVECTOR *centersB, int numRays, int *numA, int *numB,
                 float split, int axis)
{
    bool above;     // (left unset for an unknown axis, as in retail)
    int i;

    *numA = 0;
    *numB = 0;
    for (i = 0; i < numRays; i++) {
        switch (axis) {
        case 0:
            above = centers[i].x > split;
            break;
        case 1:
            above = centers[i].y > split;
            break;
        case 2:
            above = centers[i].z > split;
            break;
        }
        if (above) {
            raysA[*numA * 2] = rays[i * 2];
            raysA[*numA * 2 + 1] = rays[i * 2 + 1];
            centersA[*numA] = centers[i];
            (*numA)++;
        } else {
            raysB[*numB * 2] = rays[i * 2];
            raysB[*numB * 2 + 1] = rays[i * 2 + 1];
            centersB[*numB] = centers[i];
            (*numB)++;
        }
    }
    if (*numB == 0 || *numA == 0) {
        *numA = numRays / 2;
        *numB = numRays - *numA;
        return 0;
    }
    return 1;
}


// ---- src/global/GetBoundingBoxFromVertexList.cpp
// Nested vector operators and helpers need a deeper inline depth than
// mwcc's default.
#pragma inline_depth(5)



// Retail's fabs/fmax/rsqrt helpers: result in $f8, stored through a temp.
static inline float Fabs(float x)
{
    float r;
    asm("abs.s $f8, %1; swc1 $f8, 0(%0)" : : "r"(&r), "f"(x) : "$f8", "memory");
    return r;
}

static inline float Fmax(float a, float b)
{
    float r;
    asm("max.s $f8, %1, %2; swc1 $f8, 0(%0)" : : "r"(&r), "f"(a), "f"(b) : "$f8", "memory");
    return r;
}

static inline float RSqrt(float x)
{
    float r;
    asm("rsqrt.s $f8, %0, %1; swc1 $f8, 0(%2)" : : "f"(1.0f), "f"(x), "r"(&r) : "$f8", "memory");
    return r;
}

static inline float Length(const _D3DVECTOR &v)
{
    float len2 = v.x * v.x + v.y * v.y + v.z * v.z;
    if (len2 == 0.0f)
        return 0.0f;
    return 1.0f / RSqrt(len2);
}

// Axis-aligned bounds of a vertex list (optionally transformed by m): center,
// half-size, bounding-sphere radius and the box volume (each side at least
// 0.01).
void GetBoundingBoxFromVertexList(_D3DVECTOR *vertices, int count, _D3DVECTOR *center, _D3DVECTOR *half, float *volume, float *radius, _D3DMATRIX *m)
{
    _D3DVECTOR max;
    _D3DVECTOR min;
    max = vertices[0];
    min = vertices[0];
    for (int i = 0; i < count; i++) {
        _D3DVECTOR v;
        v = vertices[i];
        if (m)
            Vu0TransformPoint(m, &v, &v);
        if (v.x > max.x)
            max.x = v.x;
        if (v.y > max.y)
            max.y = v.y;
        if (v.z > max.z)
            max.z = v.z;
        if (v.x < min.x)
            min.x = v.x;
        if (v.y < min.y)
            min.y = v.y;
        if (v.z < min.z)
            min.z = v.z;
    }
    *center = (min + max) / 2.0f;
    *half = (max - min) / 2.0f;
    *radius = Length(*half);
    *volume = Fmax(Fabs(2.0f * half->z), 0.01f) * (Fmax(Fabs(2.0f * half->x), 0.01f) * Fmax(Fabs(2.0f * half->y), 0.01f));
}


// ---- src/global/GetBoundingBoxFromFaceList.cpp
// Nested vector operators and helpers need a deeper inline depth than
// mwcc's default.
#pragma inline_depth(5)



// Retail's fabs/fmax/rsqrt helpers: result in $f8, stored through a temp.


// As GetBoundingBoxFromVertexList, over the vertices of a list of faces.
void GetBoundingBoxFromFaceList(BoundingBoxFaceBuild *faces, int count, _D3DVECTOR *vertices, _D3DVECTOR *center, _D3DVECTOR *half, float *volume, float *radius, _D3DMATRIX *m)
{
    _D3DVECTOR max;
    _D3DVECTOR min;
    max = vertices[faces[0].m_vertex[0]];
    min = vertices[faces[0].m_vertex[0]];
    for (int i = 0; i < count; i++) {
        for (int j = 0; j < 3; j++) {
            _D3DVECTOR v;
            v = vertices[faces[i].m_vertex[j]];
            if (m)
                Vu0TransformPoint(m, &v, &v);
            if (v.x > max.x)
                max.x = v.x;
            if (v.y > max.y)
                max.y = v.y;
            if (v.z > max.z)
                max.z = v.z;
            if (v.x < min.x)
                min.x = v.x;
            if (v.y < min.y)
                min.y = v.y;
            if (v.z < min.z)
                min.z = v.z;
        }
    }
    *center = (min + max) / 2.0f;
    *half = (max - min) / 2.0f;
    *radius = Length(*half);
    *volume = Fmax(Fabs(2.0f * half->z), 0.01f) * (Fmax(Fabs(2.0f * half->x), 0.01f) * Fmax(Fabs(2.0f * half->y), 0.01f));
}


// ---- src/global/GetBoundingVolumeFromFaceList.cpp
struct _D3DMATRIX;
struct BoundingBoxFaceBuild;


void GetBoundingBoxFromFaceList(BoundingBoxFaceBuild *faces, int count, _D3DVECTOR *vertices,
                                _D3DVECTOR *center,
                                _D3DVECTOR *extent, float *radius, float *height,
                                _D3DMATRIX *transform);

float GetBoundingVolumeFromFaceList(BoundingBoxFaceBuild *faces, _D3DVECTOR *vertices, int count)
{
    _D3DVECTOR extent;
    _D3DVECTOR center;
    float radius;
    float height;

    GetBoundingBoxFromFaceList(faces, count, vertices, &center, &extent, &radius, &height, 0);
    return radius;
}


// ---- src/global/GetBoundingVolumeFromVertexList.cpp
struct _D3DMATRIX;


void GetBoundingBoxFromVertexList(_D3DVECTOR *vertices, int count, _D3DVECTOR *center,
                                  _D3DVECTOR *extent, float *radius, float *height,
                                  _D3DMATRIX *transform);

float GetBoundingVolumeFromVertexList(_D3DVECTOR *vertices, int count)
{
    _D3DVECTOR extent;
    _D3DVECTOR center;
    float radius;
    float height;

    GetBoundingBoxFromVertexList(vertices, count, &center, &extent, &radius, &height, 0);
    return radius;
}


// ---- src/global/GetSeperatingPlane.cpp

extern "C" void *malloc(unsigned int size);
extern "C" void free(void *p);


int SeperateFaces(BoundingBoxFaceBuild *faces, BoundingBoxFaceBuild *left, BoundingBoxFaceBuild *right, int count,
                   int *numLeft, int *numRight, float plane, int axis);
float GetBoundingVolumeFromFaceList(BoundingBoxFaceBuild *faces, _D3DVECTOR *vertices, int count);

// Picks the split plane (axis and position, in 20ths of the face centers'
// extent) that minimizes the summed bounding volumes of the two halves.
void GetSeperatingPlane(BoundingBoxFaceBuild *faces, int count, _D3DVECTOR *vertices, float *plane, int *axis)
{
    float bestVolume;
    float minX;
    float minY;
    float minZ;
    float maxZ;
    float maxY;
    float maxX;
    int i;
    float sizeX;
    float sizeY;
    float sizeZ;
    float largest;
    int bestAxis;
    int bestStep;
    minX = faces[0].m_center.x;
    minY = faces[0].m_center.y;
    minZ = faces[0].m_center.z;
    maxX = minX;
    maxY = minY;
    maxZ = minZ;
    for (i = 0; i < count; i++) {
        float z;
        float y;
        float x;
        x = faces[i].m_center.x;
        y = faces[i].m_center.y;
        z = faces[i].m_center.z;
        if (x > maxX)
            maxX = x;
        if (y > maxY)
            maxY = y;
        if (z > maxZ)
            maxZ = z;
        if (x < minX)
            minX = x;
        if (y < minY)
            minY = y;
        if (z < minZ)
            minZ = z;
    }
    sizeX = maxX - minX;
    largest = 0.0f;
    bestAxis = 0;
    if (sizeX > largest)
        largest = sizeX;
    sizeY = maxY - minY;
    if (sizeY > largest) {
        bestAxis = 1;
        largest = sizeY;
    }
    sizeZ = maxZ - minZ;
    if (sizeZ > largest)
        bestAxis = 2;
    bestStep = 3;
    BoundingBoxFaceBuild *left = (BoundingBoxFaceBuild *)malloc(count * sizeof(BoundingBoxFaceBuild));
    BoundingBoxFaceBuild *right = (BoundingBoxFaceBuild *)malloc(count * sizeof(BoundingBoxFaceBuild));
    int numLeft;
    int numRight;
    switch (bestAxis) {
    case 0:
        *plane = minX + (bestStep + 1) * (sizeX / 20.0f);
        break;
    case 1:
        *plane = minY + (bestStep + 1) * (sizeY / 20.0f);
        break;
    case 2:
        *plane = minZ + (bestStep + 1) * (sizeZ / 20.0f);
        break;
    }
    SeperateFaces(faces, left, right, count, &numLeft, &numRight, *plane, bestAxis);
    bestVolume = GetBoundingVolumeFromFaceList(left, vertices, numLeft);
    bestVolume += GetBoundingVolumeFromFaceList(right, vertices, numRight);
    int a;
    for (int step = 0; step < 18; step++) {
        for (a = 0; a < 3; a++) {
            switch (a) {
            case 0:
                *plane = minX + (step + 1) * (sizeX / 20.0f);
                break;
            case 1:
                *plane = minY + (step + 1) * (sizeY / 20.0f);
                break;
            case 2:
                *plane = minZ + (step + 1) * (sizeZ / 20.0f);
                break;
            }
            SeperateFaces(faces, left, right, count, &numLeft, &numRight, *plane, a);
            float volume = GetBoundingVolumeFromFaceList(left, vertices, numLeft) + GetBoundingVolumeFromFaceList(right, vertices, numRight);
            if (volume < bestVolume) {
                bestVolume = volume;
                bestStep = step;
                bestAxis = a;
            }
        }
    }
    *axis = bestAxis;
    switch (*axis) {
    case 0:
        *plane = minX + (bestStep + 1) * (sizeX / 20.0f);
        break;
    case 1:
        *plane = minY + (bestStep + 1) * (sizeY / 20.0f);
        break;
    case 2:
        *plane = minZ + (bestStep + 1) * (sizeZ / 20.0f);
        break;
    }
    free(left);
    free(right);
}


// ---- src/global/GetSeperatingPlane_00212340.cpp
// GetSeperatingPlane__FP10_D3DVECTORP10_D3DVECTORiPfPi  @ 0x00212340
// Retail frame (0x110) has an unused word at 0x104, just below numLeft: a dead
// `int *unusedPtr = &unused;` keeps `unused` as a 4-byte memory local with no code, but only
// with `#pragma opt_dead_assignments off` (with it on, IRO deletes the dead pointer first and
// the slot vanishes). Without the pragma every form tried needed at least one access.

typedef int int128_t __attribute__((mode(TI)));

extern "C" void *malloc(unsigned int size);
extern "C" void free(void *p);

int SeperateRays(_D3DVECTOR *rays, _D3DVECTOR *centers, _D3DVECTOR *raysA, _D3DVECTOR *raysB,
                 _D3DVECTOR *centersA, _D3DVECTOR *centersB, int numRays, int *numA, int *numB,
                 float split, int axis);
float GetBoundingVolumeFromVertexList(_D3DVECTOR *vertices, int count);

// This file was built with dead-assignment elimination off (see the header).
#pragma opt_dead_assignments off

// Ray version: picks the split plane (axis and position, in 20ths of the
// rays' extent) that minimizes the summed bounding volumes of the two halves.
void GetSeperatingPlane(_D3DVECTOR *rays, _D3DVECTOR *centers, int count, float *plane, int *axis)
{
    float bestVolume;
    float minX;
    float minY;
    float minZ;
    float maxZ;
    float maxY;
    float maxX;
    int i;
    float sizeX;
    float sizeY;
    float sizeZ;
    float largest;
    int bestAxis;
    int bestStep;
    minX = rays[0].x;
    minY = rays[0].y;
    minZ = rays[0].z;
    maxX = minX;
    maxY = minY;
    maxZ = minZ;
    for (i = 1; i < count * 2; i++) {
        float z;
        float y;
        float x;
        x = rays[i].x;
        if (x > maxX)
            maxX = x;
        else if (x < minX)
            minX = x;
        y = rays[i].y;
        if (y > maxY)
            maxY = y;
        else if (y < minY)
            minY = y;
        z = rays[i].z;
        if (z > maxZ)
            maxZ = z;
        else if (z < minZ)
            minZ = z;
    }
    sizeX = maxX - minX;
    largest = 0.0f;
    if (sizeX > largest) {
        largest = sizeX;
        bestAxis = 0;
    }
    sizeY = maxY - minY;
    if (sizeY > largest) {
        largest = sizeY;
        bestAxis = 1;
    }
    sizeZ = maxZ - minZ;
    if (sizeZ > largest)
        bestAxis = 2;
    bestStep = 9;
    _D3DVECTOR *leftRays = (_D3DVECTOR *)malloc(count * sizeof(_D3DVECTOR) * 2);
    _D3DVECTOR *rightRays = (_D3DVECTOR *)malloc(count * sizeof(_D3DVECTOR) * 2);
    _D3DVECTOR *leftCenters = (_D3DVECTOR *)malloc(count * sizeof(_D3DVECTOR));
    _D3DVECTOR *rightCenters = (_D3DVECTOR *)malloc(count * sizeof(_D3DVECTOR));
    int unused;                 // never read; only its frame slot survives
    int *unusedPtr = &unused;   // dead, kept by opt_dead_assignments off
    int numLeft;
    int numRight;
    switch (bestAxis) {
    case 0:
        *plane = minX + (bestStep + 1) * (sizeX / 20.0f);
        break;
    case 1:
        *plane = minY + (bestStep + 1) * (sizeY / 20.0f);
        break;
    case 2:
        *plane = minZ + (bestStep + 1) * (sizeZ / 20.0f);
        break;
    }
    SeperateRays(rays, centers, leftRays, rightRays, leftCenters, rightCenters, count, &numLeft, &numRight, *plane, bestAxis);
    bestVolume = GetBoundingVolumeFromVertexList(leftRays, numLeft * 2);
    bestVolume += GetBoundingVolumeFromVertexList(rightRays, numRight * 2);
    int step;
    int a;
    for (step = 0; step < 18; step++) {
        for (a = 0; a < 3; a++) {
            switch (a) {
            case 0:
                *plane = minX + (step + 1) * (sizeX / 20.0f);
                break;
            case 1:
                *plane = minY + (step + 1) * (sizeY / 20.0f);
                break;
            case 2:
                *plane = minZ + (step + 1) * (sizeZ / 20.0f);
                break;
            }
            if (SeperateRays(rays, centers, leftRays, rightRays, leftCenters, rightCenters, count, &numLeft, &numRight, *plane, a)) {
                float volume = GetBoundingVolumeFromVertexList(leftRays, numLeft * 2) + GetBoundingVolumeFromVertexList(rightRays, numRight * 2);
                if (volume < bestVolume) {
                    bestVolume = volume;
                    bestStep = step;
                    bestAxis = a;
                }
            }
        }
    }
    *axis = bestAxis;
    switch (*axis) {
    case 0:
        *plane = minX + (bestStep + 1) * (sizeX / 20.0f);
        break;
    case 1:
        *plane = minY + (bestStep + 1) * (sizeY / 20.0f);
        break;
    case 2:
        *plane = minZ + (bestStep + 1) * (sizeZ / 20.0f);
        break;
    }
    free(leftRays);
    free(rightRays);
    free(leftCenters);
    free(rightCenters);
}


// ---- src/global/AddFacesToTree.cpp
struct _D3DMATRIX;

extern "C" void *malloc(unsigned int size);
extern "C" void free(void *p);


// A leaf holds one face: its vertex indices and plane.
struct BoundingBoxFaceLeaf {
    float m_radius;                 // +0x00
    unsigned short m_vertex[3];     // +0x04
    float m_d;                      // +0x0C
    float m_normal[3];              // +0x10
    char pad1C[0x20 - 0x1C];
};


void GetBoundingBoxFromFaceList(BoundingBoxFaceBuild *faces, int count, _D3DVECTOR *vertices, _D3DVECTOR *min,
                                _D3DVECTOR *max, float *radius, float *x04, _D3DMATRIX *m);
void GetSeperatingPlane(BoundingBoxFaceBuild *faces, int count, _D3DVECTOR *vertices, float *plane, int *axis);

// Recursively splits count faces by a plane through their centers.
void AddFacesToTree(BoundingBoxTree *tree, BoundingBoxFaceBuild *faces, int count, _D3DVECTOR *vertices)
{
    if (count == 1) {
        BoundingBoxFaceLeaf *leaf = (BoundingBoxFaceLeaf *)tree;
        leaf->m_radius = -1.0f;
        leaf->m_vertex[0] = faces->m_vertex[0];
        leaf->m_vertex[1] = faces->m_vertex[1];
        leaf->m_vertex[2] = faces->m_vertex[2];
        leaf->m_normal[0] = faces->m_normal.x;
        leaf->m_normal[1] = faces->m_normal.y;
        leaf->m_normal[2] = faces->m_normal.z;
        leaf->m_d = faces->m_d;
        return;
    }
    GetBoundingBoxFromFaceList(faces, count, vertices, &tree->m_min, &tree->m_max, &tree->m_size, &tree->m_radius, 0);
    float plane;
    int axis;
    GetSeperatingPlane(faces, count, vertices, &plane, &axis);
    int numLeft = 0;
    int numRight = 0;
    unsigned char side = 0;
    BoundingBoxFaceBuild *left = (BoundingBoxFaceBuild *)malloc(count * sizeof(BoundingBoxFaceBuild));
    BoundingBoxFaceBuild *right = (BoundingBoxFaceBuild *)malloc(count * sizeof(BoundingBoxFaceBuild));
    int i;
    for (i = 0; i < count; i++) {
        switch (axis) {
        case 0:
            side = faces[i].m_center.x > plane;
            break;
        case 1:
            side = faces[i].m_center.y > plane;
            break;
        case 2:
            side = faces[i].m_center.z > plane;
            break;
        }
        if (side) {
            left[numLeft] = faces[i];
            numLeft++;
        } else {
            right[numRight] = faces[i];
            numRight++;
        }
    }
    for (i = 0; i < numLeft; i++)
        faces[i] = left[i];
    for (i = 0; i < numRight; i++)
        faces[i + numLeft] = right[i];
    free(left);
    free(right);
    if (numRight == 0 || numLeft == 0) {
        numLeft = count / 2;
        numRight = count - numLeft;
    }
    if (numLeft == 1)
        tree->m_left = (BoundingBoxTree *)new BoundingBoxFaceLeaf;
    else
        tree->m_left = new BoundingBoxTree;
    if (numRight == 1)
        tree->m_right = (BoundingBoxTree *)new BoundingBoxFaceLeaf;
    else
        tree->m_right = new BoundingBoxTree;
    AddFacesToTree(tree->m_left, faces, numLeft, vertices);
    AddFacesToTree(tree->m_right, faces + numLeft, numRight, vertices);
}


// ---- src/global/BuildFaceTree.cpp
// The chained vector operators need a deeper inline depth than mwcc's
// default (a + b + c, then / 3).
#pragma inline_depth(5)




extern "C" void *malloc(unsigned int size);
extern "C" void free(void *p);
extern "C" void *memset(void *p, int c, unsigned int n);



void FindPlaneNormal(_D3DVECTOR &a, _D3DVECTOR &b, _D3DVECTOR &c, _D3DVECTOR &normal, float *d);
void AddFacesToTree(BoundingBoxTree *tree, BoundingBoxFaceBuild *faces, int count, _D3DVECTOR *vertices);

// out = rotation part of m applied to v.
static inline void Vu0RotateVector(_D3DVECTOR *out, _D3DMATRIX *m, _D3DVECTOR v)
{
    asm("lqc2 vf4, 0x0(%1); lqc2 vf5, 0x10(%1); lqc2 vf6, 0x20(%1); lqc2 vf8, 0x0(%2); vmulax.xyz ACC, vf4, vf8x; vmadday.xyz ACC, vf5, vf8y; vmaddz.xyz vf9, vf6, vf8z; vadd.xyzw vf9, vf9, vf0; sqc2 vf9, 0x0(%0)"
        : : "r"(out), "r"(m), "r"(&v) : "memory");
}

// Build a face tree for a mesh: plane and center of each triangle, in the
// space of the mesh (m is reset to identity; the vertices are moved by its
// inverse, a no-op).
void BuildFaceTree(BoundingBoxTree *tree, _D3DMATRIX *m, _D3DVECTOR *vertices, unsigned int *indices, int numFaces, int numVertices, int)
{
    BoundingBoxFaceBuild *faces = (BoundingBoxFaceBuild *)malloc(numFaces * sizeof(BoundingBoxFaceBuild));
    int i;
    for (i = 0; i < numFaces; i++) {
        unsigned int *index = &indices[i * 3];
        faces[i].m_vertex[0] = index[0];
        faces[i].m_vertex[1] = index[1];
        faces[i].m_vertex[2] = index[2];
        FindPlaneNormal(vertices[index[0]], vertices[index[1]], vertices[index[2]], faces[i].m_normal, &faces[i].m_d);
    }
    memset(m, 0, sizeof(_D3DMATRIX));
    m->_11 = m->_22 = m->_33 = m->_44 = 1.0f;
    _D3DMATRIX inv __attribute__((aligned(16)));
    inv = *m;
    Vu0InversMatrix(&inv);
    for (int j = 0; j < numVertices; j++)
        Vu0TransformPoint(&inv, &vertices[j], &vertices[j]);
    for (i = 0; i < numFaces; i++) {
        Vu0RotateVector(&faces[i].m_normal, &inv, faces[i].m_normal);
        faces[i].m_center = (vertices[faces[i].m_vertex[0]] + vertices[faces[i].m_vertex[1]] + vertices[faces[i].m_vertex[2]]) / 3.0f;
    }
    AddFacesToTree(tree, faces, numFaces, vertices);
    free(faces);
}


// ---- src/global/AddRaysToTree.cpp
struct _D3DMATRIX;

extern "C" void *malloc(unsigned int size);
extern "C" void free(void *p);


// A leaf holds one ray (start, end) where an inner node holds its box.
struct BoundingBoxRayLeaf {
    float m_radius;                 // +0x00
    float m_x04;                    // +0x04
    BoundingBoxTree *m_left;        // +0x08
    BoundingBoxTree *m_right;       // +0x0C
    _D3DVECTOR m_ray[2];            // +0x10
};

void GetBoundingBoxFromVertexList(_D3DVECTOR *vertices, int count, _D3DVECTOR *min, _D3DVECTOR *max,
                                  float *radius, float *x04, _D3DMATRIX *m);
void GetSeperatingPlane(_D3DVECTOR *rays, _D3DVECTOR *midpoints, int count, float *plane, int *axis);
int SeperateRays(_D3DVECTOR *rays, _D3DVECTOR *midpoints, _D3DVECTOR *leftRays, _D3DVECTOR *rightRays,
                  _D3DVECTOR *leftMidpoints, _D3DVECTOR *rightMidpoints, int count, int *numLeft,
                  int *numRight, float plane, int axis);

// Recursively splits count rays (start/end pairs) by a plane through their
// midpoints.
void AddRaysToTree(BoundingBoxTree *tree, _D3DVECTOR *rays, _D3DVECTOR *midpoints, int count)
{
    if (count == 1) {
        tree->m_size = -1.0f;
        tree->m_min = rays[0];
        tree->m_max = rays[1];
        return;
    }
    GetBoundingBoxFromVertexList(rays, count * 2, &tree->m_min, &tree->m_max, &tree->m_size, &tree->m_radius, 0);
    float plane;
    int axis;
    GetSeperatingPlane(rays, midpoints, count, &plane, &axis);
    int numLeft = 0;
    int numRight = 0;
    _D3DVECTOR *leftRays = (_D3DVECTOR *)malloc(count * sizeof(_D3DVECTOR) * 2);
    _D3DVECTOR *rightRays = (_D3DVECTOR *)malloc(count * sizeof(_D3DVECTOR) * 2);
    _D3DVECTOR *leftMidpoints = (_D3DVECTOR *)malloc(count * sizeof(_D3DVECTOR));
    _D3DVECTOR *rightMidpoints = (_D3DVECTOR *)malloc(count * sizeof(_D3DVECTOR));
    SeperateRays(rays, midpoints, leftRays, rightRays, leftMidpoints, rightMidpoints, count, &numLeft, &numRight,
                 plane, axis);
    int i;
    // Retail copies the separated rays onto themselves, not back into rays
    // (only the midpoints are copied back; the right half from the wrong
    // offset).
    for (i = 0; i < numLeft; i++) {
        leftRays[i * 2] = leftRays[i * 2];
        leftRays[i * 2 + 1] = leftRays[i * 2 + 1];
        midpoints[i] = leftMidpoints[i];
    }
    for (i = 0; i < numRight; i++) {
        rightRays[(i + numLeft) * 2] = rightRays[(i + numLeft) * 2];
        rightRays[(i + numLeft) * 2 + 1] = rightRays[(i + numLeft) * 2 + 1];
        midpoints[i + numLeft] = rightMidpoints[i + numLeft];
    }
    free(leftRays);
    free(rightRays);
    free(leftMidpoints);
    free(rightMidpoints);
    if (numRight == 0 || numLeft == 0) {
        numLeft = count / 2;
        numRight = count - numLeft;
    }
    if (numLeft == 1)
        tree->m_left = (BoundingBoxTree *)new BoundingBoxRayLeaf;
    else
        tree->m_left = new BoundingBoxTree;
    if (numRight == 1)
        tree->m_right = (BoundingBoxTree *)new BoundingBoxRayLeaf;
    else
        tree->m_right = new BoundingBoxTree;
    AddRaysToTree(tree->m_left, rays, midpoints, numLeft);
    AddRaysToTree(tree->m_right, rays + numLeft * 2, midpoints + numLeft, numRight);
}


// ---- src/global/BuildRayTree.cpp
// Nested vector operators need a deeper inline depth than mwcc's default,
// or the _D3DVECTOR ctor is called out of line.
#pragma inline_depth(4)



extern "C" void *malloc(unsigned int size);
extern "C" void free(void *p);


void AddRaysToTree(BoundingBoxTree *tree, _D3DVECTOR *rays, _D3DVECTOR *midpoints, int count);

// rays holds count start/end pairs in world space; bring them into m's space
// and build the tree from them and their midpoints.
void BuildRayTree(BoundingBoxTree *tree, _D3DMATRIX *m, _D3DVECTOR *rays, int count)
{
    _D3DVECTOR *midpoints = (_D3DVECTOR *)malloc(count * sizeof(_D3DVECTOR));
    int i;
    for (i = 0; i < count; i++)
        midpoints[i] = 0.5f * (rays[i * 2] + rays[i * 2 + 1]);
    _D3DMATRIX inv __attribute__((aligned(16)));
    inv = *m;
    Vu0InversMatrix(&inv);
    for (i = 0; i < count * 2; i++)
        Vu0TransformPoint(&inv, &rays[i], &rays[i]);
    AddRaysToTree(tree, rays, midpoints, count);
    free(midpoints);
}


// ---- src/global/ReadFaceTree_00213850.cpp

extern DataAccess *LpDataAccess;


inline float Magnitude(const _D3DVECTOR &v)
{
    float len2 = v.x * v.x + v.y * v.y + v.z * v.z;
    if (len2 == 0.0f)
        return 0.0f;
    return 1.0f / RSqrt(len2);
}


// A leaf: one face.
struct BoundingBoxFace {
    float m_size;                   // +0x00, negative
    unsigned short m_vertex[3];     // +0x04
    float m_plane[4];               // +0x0C
    char pad1C[0x20 - 0x1C];
};

// Reads a face tree written depth first: boxes (bounding radius rebuilt from
// the half size) and face leaves.
void ReadFaceTree(BoundingBoxTree **tree, long file)
{
    float size;

    LpDataAccess->FRead(file, &size, 4, 1);
    if (!(size < 0.0f)) {
        BoundingBoxTree *node = new BoundingBoxTree;
        node->m_size = size;
        LpDataAccess->FRead(file, &node->m_min.x, 4, 1);
        LpDataAccess->FRead(file, &node->m_min.y, 4, 1);
        LpDataAccess->FRead(file, &node->m_min.z, 4, 1);
        LpDataAccess->FRead(file, &node->m_max.x, 4, 1);
        LpDataAccess->FRead(file, &node->m_max.y, 4, 1);
        LpDataAccess->FRead(file, &node->m_max.z, 4, 1);
        node->m_radius = Magnitude(node->m_max);
        ReadFaceTree(&node->m_left, file);
        ReadFaceTree(&node->m_right, file);
        *tree = node;
    } else {
        BoundingBoxFace *face = new BoundingBoxFace;
        face->m_size = size;
        LpDataAccess->FRead(file, &face->m_plane[0], 4, 1);
        LpDataAccess->FRead(file, &face->m_plane[1], 4, 1);
        LpDataAccess->FRead(file, &face->m_plane[2], 4, 1);
        LpDataAccess->FRead(file, &face->m_plane[3], 4, 1);
        LpDataAccess->FRead(file, &face->m_vertex[0], 2, 1);
        LpDataAccess->FRead(file, &face->m_vertex[1], 2, 1);
        LpDataAccess->FRead(file, &face->m_vertex[2], 2, 1);
        *tree = (BoundingBoxTree *)face;
    }
}


// ---- src/global/ReadFaceTree.cpp
extern "C" void *malloc(unsigned int size);



extern DataAccess *LpDataAccess;

void ReadFaceTree(BoundingBoxTree **tree, long file);

void ReadFaceTree(BoundingBoxTree **tree, _D3DVECTOR **vertices, int *numVertices, long file)
{
    LpDataAccess->FRead(file, numVertices, 4, 1);
    _D3DVECTOR *v = (_D3DVECTOR *)malloc(*numVertices * sizeof(_D3DVECTOR));
    for (int i = 0; i < *numVertices; i++) {
        LpDataAccess->FRead(file, &v[i].x, 4, 1);
        LpDataAccess->FRead(file, &v[i].y, 4, 1);
        LpDataAccess->FRead(file, &v[i].z, 4, 1);
    }
    *vertices = v;
    ReadFaceTree(tree, file);
}

