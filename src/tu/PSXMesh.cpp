// PSXMesh.cpp  @ 0x0034DC00 (text 0x34DC00-0x34F6F0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's PSXMesh.cpp as one translation unit: its functions in retail
// order, then its static initializer. `make link` places the object's
// data as well as its code (tools/link_c.py). The old files' views map
// onto the shared headers: +0x50 m_vertexData is the transform packet
// (TransformPacket/MeshPacket/PSXMeshPacket below), +0x54 the
// DmaRenderItem (SetStripCount stores the draw count in its first
// halfword), +0x58 the StripOffsets (SetStripLength's "PSXStrip"), Mesh's
// m_count/m_values the strip count and lengths.



#include "soultree_assert_block.h"
#include "d3dtypes.h"
#include "QWORD/QWORD.h"
#include "_sceDmaTag/_sceDmaTag.h"
#include "sceGifTag/sceGifTag.h"
#include "GameObject/GameObject.h"
#include "PSXSurfaceGeometry/PSXSurfaceGeometry.h"
#include "DmaRenderItem/DmaRenderItem.h"
#include "StripOffset/StripOffset.h"
#include "PSXMesh/PSXMesh.h"
// The axis vectors every Soultree TU defines (and __sinit constructs).
#include "sinit/AxisVectors.h"

extern "C" void free(void *p);
extern "C" void *memalign(unsigned int align, unsigned int size);
extern "C" void *memset(void *dst, int c, unsigned int n);
_D3DMATRIX IdentityMatrix();

// PSXMesh::m_vertexData: the transform packet as quadwords (SetLightInfo
// fills the light directions and colours).
struct PSXMeshPacket {
    QWORD m_header;                 // +0x0
    QWORD m_data[1];                // +0x10
};

struct AlignedMatrix {
    float _11, _12, _13, _14;
    float _21, _22, _23, _24;
    float _31, _32, _33, _34;
    float _41, _42, _43, _44;
} __attribute__((aligned(16)));

// The transform packet as Render3D sees it.
struct MeshPacket {
    char pad0[0x20];
    int m_x20;                      // +0x20
    char pad24[0xE0 - 0x24];
    AlignedMatrix m_matrix;         // +0xE0
};

union QVec4 {
    int128_t q;
    struct { float x, y, z, w; } v;
};

struct Vec4 {
    float x, y, z, w;
} __attribute__((aligned(16)));

struct Matrix4 {
    float _11, _12, _13, _14;
    float _21, _22, _23, _24;
    float _31, _32, _33, _34;
    float _41, _42, _43, _44;
};

// The DMA chain that uploads the mesh transform and light settings (0x130 bytes).
struct TransformPacket {
    _sceDmaTag m_tag;               // +0x00
    unsigned int m_vif[4];          // +0x10
    char pad20[0x2C - 0x20];
    unsigned int m_x2C;             // +0x2C
    char pad30[0x3C - 0x30];
    unsigned int m_unpack0;         // +0x3C
    Matrix4 m_matrix0;              // +0x40
    char pad80[0x8C - 0x80];
    unsigned int m_unpack1;         // +0x8C
    float m_x90[12];                // +0x90
    Vec4 m_color;                   // +0xC0
    char padD0[0xDC - 0xD0];
    unsigned int m_unpack2;         // +0xDC
    Matrix4 m_matrix1;              // +0xE0
    unsigned int m_tail[4];         // +0x120
};

static inline void SetDmaTag(_sceDmaTag *tag, int qwc, int id, _sceDmaTag *next)
{
    tag->qwc = qwc;
    tag->mark = 0;
    tag->id = id;
    tag->next = next;
    tag->p[0] = 0;
    tag->p[1] = 0;
}

// BuildRenderList's scratch quadword, viewed as float x/y/z + int w (4-aligned:
// the shared 16-aligned QWORD moves its stack slot; see QWORD.h).
struct QWORDF {
    float x, y, z;
    int w;
};

PSXMesh::PSXMesh(int enabled) : Mesh(enabled)
{
    m_vertexData = 0;
    m_renderItem = 0;
    m_stripOffsets = 0;
    m_maxStrips = 0;
    m_maxStripLengths = 0;
}

PSXMesh::~PSXMesh()
{
    if (m_vertexData)
        free(m_vertexData);
    if (m_renderItem)
        delete m_renderItem;
    if (m_stripOffsets)
        delete[] m_stripOffsets;
    if (m_maxStripLengths)
        delete[] m_maxStripLengths;
}

StripOffset::~StripOffset()
{
    if (m_array14)
        delete[] m_array14;
    if (m_array18)
        delete[] m_array18;
    if (m_substripBase)
        delete[] m_substripBase;
    if (m_normalBase)
        delete[] m_normalBase;
    if (m_tangentBase)
        delete[] m_tangentBase;
    if (m_colorBase)
        delete[] m_colorBase;
    if (m_uvBase)
        delete[] m_uvBase;
    if (m_vertexInfo)
        delete[] m_vertexInfo;
}

Mesh *PSXMesh::Create(char *name, unsigned char flags, int count, int *values, MeshLightingType lighting, int color)
{
    Mesh::Create(name, flags, count, values, lighting, color);
    m_stripOffsets = new StripOffset[m_count];
    BuildTransformRenderItem();
    m_maxStrips = m_count;
    m_maxStripLengths = new int[m_maxStrips];
    for (int i = 0; i < m_maxStrips; i++)
        m_maxStripLengths[i] = *values++;
    m_renderItem = new DmaRenderItem;
    m_renderItem->Start((DmaRenderDestination)1, 0x80);
    BuildRenderList(m_renderItem, m_maxStrips, m_maxStripLengths, m_stripOffsets);
    m_renderItem->End();
    ((PSXSurfaceGeometry *)m_geometry)->m_packet = (_sceDmaTag *)m_renderItem;
    ((PSXSurfaceGeometry *)m_geometry)->m_xBbits.bit0 = 0;
    return this;
}

StripOffset::StripOffset()
{
    m_x00 = 0;
    m_x04 = 0;
    m_x08 = 0;
    m_x0C = 0;
    m_x10 = -1;
    m_array14 = 0;
    m_array18 = 0;
    m_substripBase = 0;
    m_normalBase = 0;
    m_tangentBase = 0;
    m_colorBase = 0;
    m_uvBase = 0;
    m_vertexInfo = 0;
}

void PSXMesh::SetXYZ(int strip, int vertex, _D3DVECTOR *xyz)
{
    static int PSXMesh_SetXYZ_ID = -1;  // unused profiler event ID
    StripOffset *offsets = &m_stripOffsets[strip];
#line 107 "PSXMesh.cpp"
    ASSERT(offsets->m_vertexInfo != 0, "the vertex info for this strip is NULL!");
    StripVertexInfo *info = &offsets->m_vertexInfo[vertex];
#line 111 "PSXMesh.cpp"
    ASSERT(info->m_count < 3, "A vertex can belong to at most 2 substrips!");
    for (int i = 0; i < info->m_count; i++) {
        float *p = (float *)&m_renderItem->m_dataList[offsets->m_substripBase[info->m_substrip[i]] + info->m_offset[i] * 4];
        p[0] = xyz->x;
        p[1] = xyz->y;
        p[2] = xyz->z;
    }
}

void PSXMesh::SetNormal(int strip, int vertex, _D3DVECTOR *normal)
{
    static int PSXMesh_SetNormal_ID = -1;  // unused profiler event ID
    StripOffset *offsets = &m_stripOffsets[strip];
#line 147 "PSXMesh.cpp"
    ASSERT(offsets->m_vertexInfo != 0, "the vertex info for this strip is NULL!");
    StripVertexInfo *info = &offsets->m_vertexInfo[vertex];
#line 151 "PSXMesh.cpp"
    ASSERT(info->m_count < 3, "A vertex can belong to at most 2 substrips!");
    for (int i = 0; i < info->m_count; i++) {
        float *p = (float *)&m_renderItem->m_dataList[offsets->m_normalBase[info->m_substrip[i]] + info->m_offset[i] * 3];
        p[0] = normal->x;
        p[1] = normal->y;
        p[2] = normal->z;
    }
}

void PSXMesh::SetTangent(int strip, int vertex, _D3DVECTOR *tangent)
{
    static int PSXMesh_SetTangent_ID = -1;  // unused profiler event ID
    StripOffset *offsets = &m_stripOffsets[strip];
#line 187 "PSXMesh.cpp"
    ASSERT(offsets->m_vertexInfo != 0, "the vertex info for this strip is NULL!");
    StripVertexInfo *info = &offsets->m_vertexInfo[vertex];
#line 191 "PSXMesh.cpp"
    ASSERT(info->m_count < 3, "A vertex can belong to at most 2 substrips!");
    for (int i = 0; i < info->m_count; i++) {
        float *p = (float *)&m_renderItem->m_dataList[offsets->m_tangentBase[info->m_substrip[i]] + info->m_offset[i] * 3];
        p[0] = tangent->x;
        p[1] = tangent->y;
        p[2] = tangent->z;
    }
}

// Stores a D3DCOLOR (ARGB) as four words R, G, B, A.
void PSXMesh::SetRGBA(int strip, int vertex, unsigned int rgba)
{
    static int PSXMesh_SetRGBA_ID = -1;  // unused profiler event ID
    StripOffset *offsets = &m_stripOffsets[strip];
#line 227 "PSXMesh.cpp"
    ASSERT(offsets->m_vertexInfo != 0, "the vertex info for this strip is NULL!");
    StripVertexInfo *info = &offsets->m_vertexInfo[vertex];
#line 231 "PSXMesh.cpp"
    ASSERT(info->m_count < 3, "A vertex can belong to at most 2 substrips!");
    for (int i = 0; i < info->m_count; i++) {
        int *p = (int *)&m_renderItem->m_dataList[offsets->m_colorBase[info->m_substrip[i]] + info->m_offset[i] * 4];
        p[0] = (rgba >> 16) & 0xFF;
        p[1] = (rgba >> 8) & 0xFF;
        p[2] = rgba & 0xFF;
        p[3] = (rgba >> 24) & 0xFF;
    }
}

void PSXMesh::SetUV(int strip, int vertex, float u, float v)
{
    static int PSXMesh_SetUV_ID = -1;  // unused profiler event ID
    StripOffset *offsets = &m_stripOffsets[strip];
#line 268 "PSXMesh.cpp"
    ASSERT(offsets->m_vertexInfo != 0, "the vertex info for this strip is NULL!");
    StripVertexInfo *info = &m_stripOffsets[strip].m_vertexInfo[vertex];
#line 272 "PSXMesh.cpp"
    ASSERT(info->m_count < 3, "A vertex can belong to at most 2 substrips!");
    for (int i = 0; i < info->m_count; i++) {
        float *p = (float *)&m_renderItem->m_dataList[offsets->m_uvBase[info->m_substrip[i]] + info->m_offset[i] * 2];
        p[0] = u;
        p[1] = v;
    }
}

void PSXMesh::SetLightInfo(QWORD *directions, QWORD *colors, QWORD *ambient)
{
    for (int i = 0; i < 4; i++) {
        m_vertexData->m_data[i + 3] = directions[i];
        m_vertexData->m_data[i + 8] = colors[i];
    }
}

int PSXMesh::Render3D()
{
    if (!m_x48) {
        _D3DMATRIX matrix = IdentityMatrix();
        ((MeshPacket *)m_vertexData)->m_x20 = 6;
        // Quadword copy of the matrix into the packet: retail uses $at, which
        // mwcc never allocates, so this was hand-written asm.
        asm("lq $1, 0x0(%0); lq $2, 0x10(%0); lq $3, 0x20(%0); lq %0, 0x30(%0); sq $1, 0xE0(%1); sq $2, 0xF0(%1); sq $3, 0x100(%1); sq %0, 0x110(%1)"
            : : "r"(&matrix), "r"(m_vertexData) : "$1", "$2", "$3");
        // The redundant cast loads m_geometry before m_vertexData, as retail does.
        ((PSXSurfaceGeometry *)m_geometry)->AddToRenderQueue((_sceDmaTag *)m_vertexData, 0, 0);
    }
    return GameObject::Render3D();
}

void PSXMesh::BuildTransformRenderItem()
{
    m_vertexData = (PSXMeshPacket *)memalign(0x40, sizeof(TransformPacket));
    memset(m_vertexData, 0, sizeof(TransformPacket));
    TransformPacket *p = (TransformPacket *)m_vertexData;
    p->m_vif[0] = 0x11000000;
    p->m_vif[1] = 0x03000000;
    p->m_vif[2] = 0x02000000;
    p->m_vif[3] = 0x6C018029;
    p->m_x2C = 0;
    p->m_unpack0 = 0x6C048014;
    p->m_matrix0._11 = 1.0f; p->m_matrix0._12 = 0.0f; p->m_matrix0._13 = 0.0f; p->m_matrix0._14 = 0.0f;
    p->m_matrix0._21 = 0.0f; p->m_matrix0._22 = 1.0f; p->m_matrix0._23 = 0.0f; p->m_matrix0._24 = 0.0f;
    p->m_matrix0._31 = 0.0f; p->m_matrix0._32 = 0.0f; p->m_matrix0._33 = 1.0f; p->m_matrix0._34 = 0.0f;
    p->m_matrix0._41 = 0.0f; p->m_matrix0._42 = 0.0f; p->m_matrix0._43 = 0.0f; p->m_matrix0._44 = 1.0f;
    p->m_unpack1 = 0x6C048018;
    // The colour is built in a quadword union and copied with one lq/sq.
    QVec4 color;
    int r = ((m_color >> 16) & 0xFF) >> 1;
    int g = ((m_color >> 8) & 0xFF) >> 1;
    int b = (m_color & 0xFF) >> 1;
    color.v.x = r;
    color.v.y = g;
    color.v.z = b;
    color.v.w = 64.0f;

    p->m_x90[0] = 0.0f; p->m_x90[1] = 0.0f; p->m_x90[2] = 0.0f; p->m_x90[3] = 1.0f;
    p->m_x90[4] = 0.0f; p->m_x90[5] = 0.0f; p->m_x90[6] = 0.0f; p->m_x90[7] = 1.0f;
    p->m_x90[8] = 0.0f; p->m_x90[9] = 0.0f; p->m_x90[10] = 0.0f; p->m_x90[11] = 1.0f;
    *(int128_t *)&p->m_color = color.q;

    p->m_unpack2 = 0x6C048000;
    p->m_matrix1._11 = 1.0f; p->m_matrix1._12 = 0.0f; p->m_matrix1._13 = 0.0f; p->m_matrix1._14 = 0.0f;
    p->m_matrix1._21 = 0.0f; p->m_matrix1._22 = 1.0f; p->m_matrix1._23 = 0.0f; p->m_matrix1._24 = 0.0f;
    p->m_matrix1._31 = 0.0f; p->m_matrix1._32 = 0.0f; p->m_matrix1._33 = 1.0f; p->m_matrix1._34 = 0.0f;
    p->m_matrix1._41 = 0.0f; p->m_matrix1._42 = 0.0f; p->m_matrix1._43 = 0.0f; p->m_matrix1._44 = 1.0f;
    p->m_tail[0] = 0x14000019;
    p->m_tail[1] = 0x10000000;
    p->m_tail[2] = 0x030000B5;
    p->m_tail[3] = 0x02000190;
    SetDmaTag(&((TransformPacket *)m_vertexData)->m_tag, 0x12, 0x30,
              (_sceDmaTag *)((TransformPacket *)m_vertexData)->m_vif);
}

void PSXMesh::BuildRenderList(DmaRenderItem *item, int count, int *lengths, StripOffset *offsets)
{
    QWORDF q;
    sceGifTag tag;
    int v;
    int j;
    int n;
    StripOffset *so;
    int len;
    int base;
    int i;
    int k;
    int first;
    StripVertexInfo *info;

    *(int128_t *)&tag = 0;
    tag.EOP = 1;
    tag.PRE = 1;
    tag.PRIM = 0x6C;
    tag.FLG = 0;
    tag.NREG = 3;
    tag.REGS0 = 2;
    tag.REGS1 = 1;
    tag.REGS2 = 4;

    for (i = 0; i < count; i++) {
        so = &offsets[i];
        len = lengths[i];
        m_values[i] = len;
        so->m_x0C = len;
        so->m_x10 = so->m_x0C / 16;
        so->m_x10++;
#line 553 "PSXMesh.cpp"
        ASSERT(so->m_x10 > 0, "there are no substrips in this strip!");
        so->m_array14 = new int[so->m_x10];
        so->m_array18 = new int[so->m_x10];
        so->m_substripBase = new int[so->m_x10];
        so->m_normalBase = new int[so->m_x10];
        so->m_tangentBase = new int[so->m_x10];
        so->m_colorBase = new int[so->m_x10];
        so->m_uvBase = new int[so->m_x10];
        so->m_vertexInfo = new StripVertexInfo[so->m_x0C];
        so->m_x08 = 0;
        so->m_x00 = item->m_dataListCurrentSize;
        for (j = 0; j < so->m_x10; j++) {
            if (j != so->m_x10 - 1) {
                n = 16;
            } else {
                n = so->m_x0C - j * 16;
                if (so->m_x10 > 1)
                    n += (so->m_x10 - 1) * 2;
            }
            so->m_array14[j] = n;
            first = j * 14;
            k = 0;
            for (v = first; v < first + n; v++) {
                info = &so->m_vertexInfo[v];
#line 606 "PSXMesh.cpp"
                ASSERT(info->m_count < 2, "A vertex can belong to at most 2 substrips!");
                info->m_substrip[info->m_count] = j;
                info->m_offset[info->m_count] = k;
                k++;
                info->m_count++;
            }
            item->DmaBeginUnpack(0, 0xC);
            so->m_array18[j] = item->m_dataListCurrentSize;
            tag.NLOOP = n;
            item->DmaAddGifTagToUnpack(&tag);
            item->DmaEndUnpack();
            if ((m_flags & 1) ? 1 : 0) {
                item->DmaBeginUnpack(-1, 0xC);
                base = item->m_dataListCurrentSize;
                so->m_substripBase[j] = base;
                for (v = 0; v < n; v++) {
                    q.w = 0xE;
                    item->DmaAddQwordToUnpack((QWORD *)&q);
                    base += 4;
                }
                item->DmaEndUnpack();
            }
            if ((m_flags & 2) ? 1 : 0) {
                item->DmaBeginUnpack(-1, 8);
                base = item->m_dataListCurrentSize;
                so->m_normalBase[j] = base;
                for (v = 0; v < n; v++) {
                    item->DmaAddDataToUnpack(&q, 3);
                    base += 3;
                }
                item->DmaEndUnpack();
            }
            if ((m_flags & 0x20) ? 1 : 0) {
                item->DmaBeginUnpack(-1, 8);
                base = item->m_dataListCurrentSize;
                so->m_tangentBase[j] = base;
                // Retail bounds this loop by the strip length, not the substrip size.
                for (v = 0; v < len; v++) {
                    item->DmaAddDataToUnpack(&q, 3);
                    base += 3;
                }
                item->DmaEndUnpack();
            }
            if ((m_flags & 4) ? 1 : 0) {
                item->DmaBeginUnpack(-1, 4);
                base = item->m_dataListCurrentSize;
                so->m_uvBase[j] = base;
                for (v = 0; v < n; v++) {
                    item->DmaAddDataToUnpack(&q, 2);
                    base += 2;
                }
                item->DmaEndUnpack();
            }
            if ((m_flags & 8) ? 1 : 0) {
                item->DmaBeginUnpack(-1, 0xC);
                base = item->m_dataListCurrentSize;
                so->m_colorBase[j] = base;
                for (v = 0; v < n; v++) {
                    item->DmaAddDataToUnpack(&q, 4);
                    base += 4;
                }
                item->DmaEndUnpack();
            }
            item->DmaAddVifCode(0x17000000);
        }
        so->m_x04 = item->m_dataListCurrentSize;
        so->m_x08 = so->m_x04 - so->m_x00;
    }
}

StripVertexInfo::StripVertexInfo()
{
    m_count = 0;
    m_substrip[1] = 0;
    m_substrip[0] = 0;
    m_offset[1] = 0;
    m_offset[0] = 0;
}

void PSXMesh::SetStripLength(int strip, int length)
{
    StripOffset *s = &m_stripOffsets[strip];
    // Profiler ID; retail copied the name from SetNumVertices.
    static int PSXMesh_SetNumVertices_ID = -1;
    if (length < m_maxStripLengths[strip] && s->m_x0C != length) {
        m_values[strip] = length;
        s->m_x0C = length;
        sceGifTag tag;
        tag.EOP = 1;
        tag.PRE = 1;
        tag.PRIM = 0x6C;
        tag.FLG = 0;
        tag.NREG = 3;
        tag.REGS0 = 2;
        tag.REGS1 = 1;
        tag.REGS2 = 4;
        for (int i = 0; i < s->m_x10; i++) {
            unsigned int *dst = &m_renderItem->m_dataList[s->m_array18[i]];
            if (length == 0) {
                s->m_array14[i] = 0;
                tag.NLOOP = 0;
            } else if (length - 16 > 0) {
                s->m_array14[i] = 16;
                tag.NLOOP = 16;
                length -= 16;
            } else {
                s->m_array14[i] = length;
                tag.NLOOP = length;
                length = 0;
            }
            memcpy(dst, &tag, 16);
        }
    }
}

#pragma fast_fptosi on

void PSXMesh::SetStripCount(int count)
{
    if (count > 0 && count < m_maxStrips) {
        m_count = count;
        *(short *)m_renderItem = (int)(0.5f + (float)(m_stripOffsets[m_count - 1].m_x04 >> 2)) + 1;
    }
}

#pragma fast_fptosi reset

void PSXMesh::LockVertices()
{
}

void PSXMesh::UnlockVertices()
{
}
