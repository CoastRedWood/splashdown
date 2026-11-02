// StripMesh.cpp  @ 0x00297040 (text 0x297040-0x2976D0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp)
//
// Retail's StripMesh.cpp as one translation unit: a TU without a static
// initializer inside SelectiveGravityModel.cpp's tu_map row,
// StripMesh::ComputeTangentVectors (its ASSERT says "StripMesh.cpp") and
// StripList's methods. Its .rodata (@609..@611, 0x41AA50-0x41AAA0) restarts
// the labels after SoultreeMaterial.cpp's RTTI and its .bss is one
// allowThisAssertion (0x474A60). StripList follows StripMesh here, out of
// alphabetical order for a StripList.cpp of its own, so it is placed in this
// TU (no data decides between this and SurfaceMap.cpp's head). `make link`
// places its data as well as its code.


#include "d3dtypes.h"
#include "DataAccess/DataAccess.h"
#include "StripMesh/StripMesh.h"
#include "StripList/StripList.h"
#include "soultree_assert_block.h"

extern "C" void *calloc(unsigned int n, unsigned int size);
extern "C" void *malloc(unsigned int size);
extern "C" void free(void *p);
extern "C" void *memset(void *dst, int c, unsigned int n);

extern DataAccess *LpDataAccess;

// ---- src/StripMesh/ComputeTangentVectors.cpp

// libvu0 sceVu0Normalize.
static inline void Normalize(_D3DVECTOR *out, _D3DVECTOR *in)
{
    asm("lqc2 vf4, 0x0(%1); vmul.xyz vf5, vf4, vf4; vaddy.x vf5, vf5, vf5y; vaddz.x vf5, vf5, vf5z; vrsqrt Q, vf0w, vf5x; vsub.xyzw vf6, vf0, vf0; vaddw.xyzw vf6, vf6, vf4w; vwaitq; vmulq.xyz vf6, vf4, Q; sqc2 vf6, 0x0(%0)"
        : : "r"(out), "r"(in) : "memory");
}

// libvu0 sceVu0OuterProduct: out = a x b.
static inline void CrossProduct(_D3DVECTOR *out, _D3DVECTOR *a, _D3DVECTOR *b)
{
    asm("lqc2 vf4, 0x0(%1); lqc2 vf5, 0x0(%2); vopmula.xyz ACC, vf4, vf5; vopmsub.xyz vf6, vf5, vf4; vsub.w vf6, vf6, vf6; sqc2 vf6, 0x0(%0)"
        : : "r"(out), "r"(a), "r"(b) : "memory");
}

void StripMesh::ComputeTangentVectors()
{
    _D3DVECTOR binormal;
    if (!m_tangents) {
        m_tangents = (_D3DVECTOR *)calloc(m_numVertices, sizeof(_D3DVECTOR));
#line 135 "StripMesh.cpp"
        ASSERT(m_tangents, "StripMesh::ComputeTangentVectors - LpTangents is NULL");
    }
    int i;
    StripVertex *v = m_vertices;
    _D3DVECTOR *normal = m_normals;
    _D3DVECTOR *tangent = m_tangents;
    for (i = 0; i < m_numVertices - 2; i++) {
        // Declaration order sets the FP colouring of the ten differences.
        float u1;
        float x1;
        float y1;
        float v1;
        float z1;
        float u2;
        float y2;
        float z2;
        float x2;
        float v2;
        x1 = v[1].x - v[0].x;
        y1 = v[1].y - v[0].y;
        z1 = v[1].z - v[0].z;
        u1 = v[1].u - v[0].u;
        v1 = v[1].v - v[0].v;
        x2 = v[2].x - v[0].x;
        y2 = v[2].y - v[0].y;
        z2 = v[2].z - v[0].z;
        u2 = v[2].u - v[0].u;
        v2 = v[2].v - v[0].v;
        if (v2) {
            float s = -v1 / v2;
            float t = u1 + s * u2;
            tangent->x = x1 + s * x2;
            tangent->y = y1 + s * y2;
            tangent->z = z1 + s * z2;
            if (t < 0.0f) {
                tangent->x = -tangent->x;
                tangent->y = -tangent->y;
                tangent->z = -tangent->z;
            }
        } else {
            float s = -v2 / v1;
            float t = u2 + s * u1;
            tangent->x = x2 + s * x1;
            tangent->y = y2 + s * y1;
            tangent->z = z2 + s * z1;
            if (t < 0.0f) {
                tangent->x = -tangent->x;
                tangent->y = -tangent->y;
                tangent->z = -tangent->z;
            }
        }
        Normalize(tangent, tangent);
        CrossProduct(&binormal, normal, tangent);
        Normalize(&binormal, &binormal);
        CrossProduct(tangent, &binormal, normal);
        v++;
        normal++;
        tangent++;
    }
    CrossProduct(&binormal, normal, tangent);
    Normalize(&binormal, &binormal);
    CrossProduct(tangent, &binormal, normal);
    normal++;
    tangent++;
    CrossProduct(&binormal, normal, tangent);
    Normalize(&binormal, &binormal);
    CrossProduct(tangent, &binormal, normal);
}

// ---- src/StripList/ctor.cpp

StripList::StripList()
{
    m_strips = 0;
    m_numStrips = 0;
    m_totalVertices = 0;
}

// ---- src/StripList/dtor.cpp

StripList::~StripList()
{
    for (int i = 0; i < m_numStrips; i++) {
        if (m_strips[i].m_vertices)
            free(m_strips[i].m_vertices);
        if (m_strips[i].m_normals)
            free(m_strips[i].m_normals);
        if (m_strips[i].m_tangents)
            free(m_strips[i].m_tangents);
        if (m_strips[i].m_buffer04)
            free(m_strips[i].m_buffer04);
    }
    if (m_strips)
        free(m_strips);
}

// ---- src/StripList/ReadFromFile.cpp

void StripList::ReadFromFile(long file)
{
    int count;
    LpDataAccess->FRead(file, &m_numStrips, 4, 1);
    m_strips = (StripMesh *)malloc(m_numStrips * sizeof(StripMesh));
    memset(m_strips, 0, m_numStrips * sizeof(StripMesh));
    for (int i = 0; i < m_numStrips; i++) {
        LpDataAccess->FRead(file, &count, 4, 1);
        m_strips[i].m_normals = (_D3DVECTOR *)malloc(count * sizeof(_D3DVECTOR));
        m_strips[i].m_tangents = 0;
        m_strips[i].m_buffer04 = 0;
        m_strips[i].m_numVertices = count;
        m_strips[i].m_vertices = (StripVertex *)malloc(count * sizeof(StripVertex));
        LpDataAccess->FRead(file, m_strips[i].m_vertices, sizeof(StripVertex), m_strips[i].m_numVertices);
        for (int j = 0; j < m_strips[i].m_numVertices; j++)
            LpDataAccess->FRead(file, &m_strips[i].m_normals[j], 0xC, 1);
        m_strips[i].ComputeTangentVectors();
        m_totalVertices += m_strips[i].m_numVertices;
    }
}
