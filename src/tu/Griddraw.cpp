// Griddraw.cpp  @ 0x00237C00 (text 0x237C00-0x23F480)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's Griddraw.cpp as one translation unit: VertexFinderTool, the
// parent-finder tables, DrawableGridNode and RemoveBuffers, then its static
// initializer. Unanchored in EcoSystem.cpp's tu_map row: it starts after
// Fog.cpp at 0x237C00; its .rodata is the "Griddraw.cpp" ASSERT string (@1948)
// through the GridNode/DrawableGridNode RTTI, its .data SizeBy ..
// TranslateQuadQuadIndex (0x3F9130-0x3F9260) and its .bss the axis vectors ..
// the function statics (0x462100-0x467C88).



#define PSX_RENDER_TARGET_GLOBAL    // LpRenderTarget is a PSXRenderTarget * here (RenderTarget.h)
#include "ByteVector/ByteVector.h"
#include "d3dtypes.h"
#include "Terrain/Terrain.h"
#include "DataAccess/DataAccess.h"
#include "DrawableGridNode/DrawableGridNode.h"
#include "GridNodeGeometry/GridNodeGeometry.h"
#include "GridNodeGeometryBase/GridNodeGeometryBase.h"
#include "GridNodeGeometryLOD/GridNodeGeometryLOD.h"
#include "GridNodeGeometryPlain/GridNodeGeometryPlain.h"
#include "LightManager/LightManager.h"
#include "AgeManager/AgeManager.h"
#include "Camera/Camera.h"
#include "RenderTarget/RenderTarget.h"
#include "PSXRenderTarget/PSXRenderTarget.h"
#include "PSXCamera/PSXCamera.h"
#include "TextureMap/TextureMap.h"
#include "PSXTextureMap/PSXTextureMap.h"
#include "PSXTextureMapManager/PSXTextureMapManager.h"
#include "VifDataItem/VifDataItem.h"
#include "soultree_assert_block.h"
#include "QWORD/QWORD.h"
#include "VisibleTest/VisibleTest.h"
#include "sinit/AxisVectors.h"

extern "C" void *calloc(unsigned int n, unsigned int size);
extern "C" void *malloc(unsigned int size);
extern "C" void free(void *p);
extern "C" void *memcpy(void *dst, const void *src, unsigned int n);
extern "C" void *memset(void *dst, int c, unsigned int n);
typedef int int128_t __attribute__((mode(TI)));

// For each vertex of the 17x17 grid, the two vertices whose midpoint it is
// at its decimation level.
struct ParentFinder {
    int x;          // +0x00
    int y;          // +0x04
    int parent0X;   // +0x08
    int parent0Y;   // +0x0C
    int parent1X;   // +0x10
    int parent1Y;   // +0x14
    int edge;       // +0x18
    int diagonal;   // +0x1C
};

struct VertexEntry {
    unsigned int m_timeStamp;
    int m_index;
};

struct FoundVertex {
    float x, y, z;
    float u, v;
    float detailU, detailV;
};

// Collects the vertices of one node's mesh (Griddraw.cpp's VertexFinder).
class VertexFinderTool {
public:
    VertexFinderTool()
    {
        m_timeStamp = (unsigned int)-1;
        ReInitialize(0);
    }
    ~VertexFinderTool();
    void ReInitialize(DrawableGridNode *node);
    int PointIndex(int x, int y);

    DrawableGridNode *m_node;           // +0x0000
    unsigned int m_timeStamp;           // +0x0004
    VertexEntry m_entries[0x121];       // +0x0008
    FoundVertex m_vertices[0x121];      // +0x0910
    int m_count;                        // +0x28AC
    int m_x;                            // +0x28B0
    int m_y;                            // +0x28B4
    int m_shift;                        // +0x28B8
    GridVertex *m_verts;                // +0x28BC
    float m_heightScale;                // +0x28C0
    float m_vOffset;                    // +0x28C4
    float m_uOffset;                    // +0x28C8
    float m_uvScale;                    // +0x28CC
    int m_x28D0;                        // +0x28D0
    float m_scale;                      // +0x28D4
    float m_minY;                       // +0x28D8
    float m_maxY;                       // +0x28DC
    int m_detailTextured;               // +0x28E0
    int m_x28E4;                        // +0x28E4
    int m_x28E8;                        // +0x28E8
    float m_detailScale;                // +0x28EC
};

// Griddraw.cpp's globals, in retail's .data/.bss order.
static int SizeBy[17] = {
    0, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xF0, 0x100,
};
static int SizeVerticesBy[18] = {
    0, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x110, 0x121,
};
int MAX_MIP_LEVELS = 9;
float TerrainDetailScalar = 0.083333336f;
int QuadIndex[16] = { 0, 4, 0x40, 0x44, 8, 0xC, 0x48, 0x4C, 0x80, 0x84, 0xC0, 0xC4, 0x88, 0x8C, 0xC8, 0xCC };
int TranslateQuadQuadIndex[16] = { 0, 1, 4, 5, 2, 3, 6, 7, 8, 9, 0xC, 0xD, 0xA, 0xB, 0xE, 0xF };
int xBase;
int yBase;
int MIP_TOP_SIZE = 1 << (MAX_MIP_LEVELS - 1);
static int TablesInitialized;
int TotalAllocated;
int CurrScisItemIndex;
static VertexFinderTool VertexFinder;
short FaceData[0xD8C / 2];
ParentFinder ParentFinderTable[17 * 17];

// ---- src/VertexFinderTool/ReInitialize.cpp

void VertexFinderTool::ReInitialize(DrawableGridNode *node)
{
    m_node = node;
    m_count = 0;
    m_minY = 3.40282347e+38f;
    m_maxY = -3.40282347e+38f;
    if (++m_timeStamp == 0) {
        for (int i = 0; i < 0x121; i++) {
            m_entries[i].m_timeStamp = 0;
            m_entries[i].m_index = 0;
        }
        m_timeStamp++;
    }
    if (!m_node)
        memset(m_vertices, 0, sizeof(m_vertices));
}


// ---- src/VertexFinderTool/PointIndex.cpp






extern int SizeVerticesBy[];
extern float TerrainDetailScalar;
extern int xBase;
extern int yBase;




// Returns the index of grid point (x, y) in the found-vertex list, adding
// the vertex (position, normal and texture coordinates) the first time it is
// asked for in this pass.
int VertexFinderTool::PointIndex(int x, int y)
{
    int i;
    FoundVertex *v;
    i = x + SizeVerticesBy[y];
    if (m_entries[i].m_timeStamp < m_timeStamp) {
        if (m_count == 0) {
            m_x = m_node->m_x;
            m_y = m_node->m_y;
            m_shift = m_node->m_shift;
            m_verts = m_node->m_lodData->verts;
            m_heightScale = m_node->m_heightScale;
            m_vOffset = m_node->m_vOffset;
            m_uOffset = m_node->m_uOffset;
            m_uvScale = m_node->m_uvScale;
            m_x28D0 = 0;
            m_scale = m_node->m_terrain->m_gridScale;
            m_minY = 3.40282347e+38f;
            m_maxY = -3.40282347e+38f;
            m_detailTextured = m_node->m_terrain->m_xCDC;
            m_x28E4 = m_node->m_terrain->m_detailInverse;
            m_x28E8 = m_node->m_terrain->m_detailModulate;
            m_x28E4 = 0xFFFFFF;
            m_detailScale = TerrainDetailScalar;
            if (m_node->m_level == 1)
                m_detailScale *= 16.0f;
        }
        v = &m_vertices[m_count];
        v->y = m_scale * (m_node->m_x10 * ((float)m_node->m_lod->m_baseHeight +
                    m_node->m_heightScale * (float)(m_verts[i].height - m_node->m_lod->m_baseHeight)));
        v->x = m_scale * (float)((m_x + x) << m_shift);
        v->z = m_scale * (float)((m_y + y) << m_shift);
        if (v->y < m_minY)
            m_minY = v->y;
        if (v->y > m_maxY)
            m_maxY = v->y;
        if (m_node->m_geometry->m_bits.m_lit) {
            if (m_node->m_heightScale < 0.0f) {
                v->u = (float)-m_verts[i].normal.x / 127.0f;
                v->v = (float)m_verts[i].normal.y / 127.0f;
                v->detailU = (float)-m_verts[i].normal.z / 127.0f;
            } else {
                v->u = (float)m_verts[i].normal.x / 127.0f;
                v->v = (float)m_verts[i].normal.y / 127.0f;
                v->detailU = (float)m_verts[i].normal.z / 127.0f;
            }
            // Retail then overwrites the first two normal components with
            // the texture coordinates.
            v->u = 0.03125f + 0.9375f * (m_uOffset + m_uvScale * (float)(x - xBase));
            v->v = 0.03125f + 0.9375f * (m_vOffset + m_uvScale * (float)(y - yBase));
        } else if (m_detailTextured) {
            v->u = 0.03125f + 0.9375f * (m_uOffset + m_uvScale * (float)(x - xBase));
            v->v = 0.03125f + 0.9375f * (m_vOffset + m_uvScale * (float)(y - yBase));
            v->detailU = m_detailScale * (float)(x - xBase);
            v->detailV = m_detailScale * (float)(y - yBase);
        }
        m_entries[i].m_index = m_count;
        m_entries[i].m_timeStamp = m_timeStamp;
        m_count++;
    }
    return m_entries[i].m_index;
}


// ---- src/global/FindParentIndices.cpp
// For each vertex of the 17x17 grid, the two vertices whose midpoint it is
// at its decimation level.

// (Each branch sets edge before parent1: retail's store order needs it.)
void FindParentIndices(int x, int y, int dx, int dy, int maskX, int maskY, ParentFinder *pf)
{
    if (dy == 0) {
        if (y & dx) {
            pf->parent0X = x - dx;
            pf->parent0Y = y;
            pf->edge = 4;
            pf->parent1X = x + dx;
            pf->parent1Y = y;
            pf->diagonal = 1;
        } else {
            pf->parent0X = x;
            pf->parent0Y = y - dx;
            pf->edge = 8;
            pf->parent1X = x;
            pf->parent1Y = y + dx;
            pf->diagonal = 2;
        }
    } else {
        int y0 = y - dy;
        int x0 = x - dx;
        if ((maskY & y0) == (maskX & x0)) {
            pf->parent0X = x + dx;
            pf->parent0Y = y0;
            pf->edge = 4;
            pf->parent1X = x0;
            pf->parent1Y = y + dy;
            pf->diagonal = 2;
        } else {
            pf->parent0X = x0;
            pf->parent0Y = y0;
            pf->edge = 8;
            pf->parent1X = x + dx;
            pf->parent1Y = y + dy;
            pf->diagonal = 1;
        }
    }
}


// ---- src/global/FindParentIndicesInDecimatedGrid.cpp
// For each vertex of the 17x17 grid, the two vertices whose midpoint it is
// at its decimation level.

extern ParentFinder ParentFinderTable[];

// Start index of each row of the 17x17 vertex grid.

void FindParentIndices(int x, int y, int dx, int dy, int maskX, int maskY, ParentFinder *pf);

void FindParentIndicesInDecimatedGrid(int size, int offset, int stepX, int stepY)
{
    int y, x, row;

    for (y = offset, row = 0; y <= 16; y += stepY, row++) {
        if (offset == 0) {
            if (row & 1)
                x = 0;
            else
                x = size;
        } else {
            x = size;
        }
        for (; x <= 16; x += stepX)
            FindParentIndices(x, y, size, offset, stepX, stepY, &ParentFinderTable[x + SizeVerticesBy[y]]);
    }
}


// ---- src/global/BuildParentFinder.cpp
// BuildParentFinder__Fv — repeat call + table init  @ 0x00238340
// The first call's 0 offset is a variable set before the loop and again at the end of each
// pass: with two reaching definitions IRO doesn't propagate the constant, so `offset` stays
// in $a1 and its latch assignment fills the back-edge delay slot as in retail (a literal 0
// argument puts `move a1, zero` at the loop head instead).

extern ParentFinder ParentFinderTable[];


void FindParentIndicesInDecimatedGrid(int size, int offset, int stepX, int stepY);


void BuildParentFinder()
{
    int step;
    int size;
    int half;
    int y;
    int x;
    int offset;

    size = 1;
    step = 2;
    half = 1;
    offset = 0;
    while (size < 16) {
        FindParentIndicesInDecimatedGrid(size, offset, step, half);
        half *= 2;
        offset = size;
        FindParentIndicesInDecimatedGrid(size, offset, step, half);
        size *= 2;
        step *= 2;
        offset = 0;     // reset for the next pass (needed for retail's codegen)
    }
    for (y = 0; y <= 16; y++) {
        for (x = 0; x <= 16; x++) {
            ParentFinderTable[x + SizeVerticesBy[y]].x = x;
            ParentFinderTable[x + SizeVerticesBy[y]].y = y;
        }
    }
}


// ---- src/DrawableGridNode/ctor.cpp

void InitializeDeltaCompressionTables();
void BuildParentFinder();



class DrawableGridNode;



// DMA REF tag for the node's VIF header.


DrawableGridNode::DrawableGridNode(long, int, DrawableGridNode *parent, Terrain *terrain, int, int, int, int, int, _D3DVECTOR *, _D3DVECTOR *)
{
    asm("");    // retail keeps `this` on the stack
    m_terrain = terrain;
    if (!TablesInitialized) {
        InitializeDeltaCompressionTables();
        BuildParentFinder();
        for (int i = 0; i < 17 * 17; i++)
            m_terrain->m_x114[i] = 0;
        TablesInitialized = 1;
    }
    m_children = 0;
    m_childOffsets = 0;
    m_parent = parent;
    m_lod = 0;
    m_x6C = 0;
    m_geometry = 0;
    m_textures = 0;
    m_x84 = 0;
    QWORD *header = &m_vifHeader;
    header->ui[0] = 0x11000000;
    header->ui[1] = 0x03000000;
    header->ui[2] = 0x02000000;
    header->ui[3] = 0x6C048015;
    m_dmaTag.qwc = 5;
    m_dmaTag.mark = 0;
    m_dmaTag.id = 0x30;
    m_dmaTag.next = &m_vifHeader;
    m_dmaTag.p[0] = 0;
    m_dmaTag.p[1] = 0;
}


// ---- src/DrawableGridNode/LoadNode.cpp
// LoadNode__16DrawableGridNodeFliP16DrawableGridNodeiiiiiP10_D3DVECTORP10_D3DVECTOR  @ 0x002385A0
// The centre block re-reads m_terrain->m_gridScale for each coordinate and writes the
// size term out in both bottom and right (no size local): CSE merges them, and because
// bottom's expression then contains loads, the m_center.x store keeps it from being
// forward-substituted into m_center.z, so bottom is computed before right as in retail.
// FRead's data argument goes through the inline Data() accessor: an argument containing
// a call is evaluated first, which puts the data load ahead of the size load.

extern "C" void *calloc(unsigned int n, unsigned int size);
extern "C" void *malloc(unsigned int size);
extern "C" void *memcpy(void *dst, const void *src, unsigned int n);
extern "C" void *memset(void *dst, int c, unsigned int n);

extern DataAccess *LpDataAccess;

extern int TotalAllocated;          // bytes of node data allocated


// D3D_OVERLOADS' component-wise compare.
// D3D_OVERLOADS' component-wise compare.
inline int operator<(const _D3DVECTOR &a, const _D3DVECTOR &b)
{
    return a.x < b.x && a.y < b.y && a.z < b.z;
}



// Direct3D's one-float constructor (x = y = z = f), which d3dtypes.h lacks.
static inline _D3DVECTOR Splat(float f)
{
    _D3DVECTOR v;
    v.x = v.y = v.z = f;
    return v;
}

// A child's entry in the node file (4 bytes).
struct ChildEntry {
    short m_flag : 1;
    short m_index : 15;             // -1: no child
    short m_x2;
};





// Reads one node (and, when level allows, its children) from the terrain file.
DrawableGridNode *DrawableGridNode::LoadNode(long file, int level, DrawableGridNode *parent, int nodeLevel, int shift,
                                             int x, int y, int position, _D3DVECTOR *min, _D3DVECTOR *max)
{
    int childTable;
    ChildEntry entries[16];
    LpDataAccess->FRead(file, &m_heightScale, 4, 1);
    LpDataAccess->FRead(file, &m_x10, 4, 1);
    LpDataAccess->FRead(file, &m_minHeight, 4, 1);
    LpDataAccess->FRead(file, &m_maxHeight, 4, 1);
    LpDataAccess->FRead(file, &m_x1C, 2, 1);
    m_level = nodeLevel;
    m_shift = shift;
    m_x = x;
    m_y = y;
    m_position = position;
    if (min && max) {
        if (x < min->x)
            min->x = x;
        if (y < min->z)
            min->z = y;
        if (x > max->x)
            max->x = x;
        if (y > max->z)
            max->z = y;
        if (m_minHeight < min->y)
            min->y = m_minHeight;
        if (m_maxHeight > max->y)
            max->y = m_maxHeight;
    }
    LpDataAccess->FRead(file, &m_lodIndex, 2, 1);
    m_lod = m_terrain->m_geometries[m_lodIndex];
    LpDataAccess->FRead(file, &childTable, 4, 1);
    LpDataAccess->FRead(file, entries, 4, 16);
    int hasChildren = 0;
    int i;
    for (i = 0; i < 16; i++) {
        if (entries[i].m_index != -1) {
            hasChildren = 1;
            break;
        }
    }
    if (hasChildren) {
        // the render data, the children's, then (unless flat) the mesh and the children's heights
        int size = GRID_NODE_HEAD + GRID_NODE_DETAILS;
        if (!m_terrain->m_isPlain)
            size += GRID_NODE_GEOMETRY + GRID_NODE_CHILD_HEIGHTS;
        m_textures = (GridNodeTextures *)calloc(1, size);
        m_textures->m_detail = (DetailTextureEntry *)((char *)m_textures + GRID_NODE_HEAD);
        for (i = 0; i < 16; i++) {
            memcpy(&m_textures->m_detail[i], &entries[i], 4);
            m_textures->m_detail[i].m_texture = 0;
        }
    } else {
        int size = GRID_NODE_HEAD;
        if (!m_terrain->m_isPlain)
            size += GRID_NODE_GEOMETRY;
        m_textures = (GridNodeTextures *)calloc(1, size);
        m_textures->m_detail = 0;
    }
    memcpy(&m_textures->m_xC, &childTable, 4);
    m_textures->m_x10 = 0;
    LpDataAccess->FRead(file, &m_textures->m_dataSize, 4, 1);
    if (m_textures->m_dataSize) {
        m_textures->m_data = malloc(m_textures->m_dataSize);
        TotalAllocated += m_textures->m_dataSize;
        LpDataAccess->FRead(file, m_textures->Data(), m_textures->m_dataSize, 1);
    }
    LpDataAccess->FRead(file, &m_x27, 1, 1);
    LpDataAccess->FRead(file, &m_childTable, 4, 1);
    float left, top, bottom, right;
    left = m_terrain->m_gridScale * (m_x << m_shift);
    top = m_terrain->m_gridScale * (m_y << m_shift);
    bottom = top + m_terrain->m_gridScale * (16 << m_shift);
    right = left + m_terrain->m_gridScale * (16 << m_shift);
    m_center.x = (left + right) / 2.0f;
    m_center.y = m_terrain->m_gridScale * (m_minHeight + m_maxHeight) / 2.0f;
    m_center.z = (top + bottom) / 2.0f;
    m_halfSize.x = m_center.x - left;
    m_halfSize.y = m_center.y - m_minHeight * m_terrain->m_gridScale;
    // (sic) retail tests m_halfSize.z before setting it
    if (m_halfSize < Splat(0.1f))
        m_halfSize = _D3DVECTOR(0.1f, 0.1f, 0.1f);
    m_halfSize.z = m_center.z - top;
    if (!m_terrain->m_isPlain) {
        if (m_textures->m_detail) {
            m_geometry = (GridNodeGeometry *)((char *)m_textures->m_detail + GRID_NODE_DETAILS);
            m_geometry->m_childHeights = (ChildHeights *)((char *)m_geometry + GRID_NODE_GEOMETRY);
        } else {
            m_geometry = (GridNodeGeometry *)((char *)m_textures + GRID_NODE_HEAD);
            m_geometry->m_childHeights = 0;
        }
        m_geometry->m_vertices = 0;
        m_geometry->m_verticesSize = 0;
        m_geometry->m_indicesSize = 0;
        m_geometry->m_numVertices = 0;
        m_geometry->m_numIndices = 0;
        m_geometry->m_indices = 0;
        m_geometry->m_bits.m_changed = 1;
        m_geometry->m_evaluatedFrame = 0;
        m_geometry->m_x122 = 0xFFFF;
        m_geometry->m_x126 = 0xFFFF;
        m_geometry->m_x124 = 0;
        m_geometry->m_x128 = 0;
        m_geometry->m_bits.m_hasMesh = 0;
        m_geometry->m_numStrips = 0;
        m_geometry->m_numChildStrips = 0;
        m_geometry->m_x158 = 1.0f;
        m_geometry->m_bits.m_as4x4 = 0;
        m_geometry->m_bits.m_x121_4 = 1;
        m_geometry->m_bits.m_x121_5 = 1;
        m_geometry->m_bits.m_lit = m_terrain->m_xC64;
        m_geometry->m_x158 = 1.0f / (m_x10 * m_heightScale);
        if (m_geometry->m_x158 < 0.0f)
            m_geometry->m_x158 = -m_geometry->m_x158;
        memset(m_geometry, 0, 17 * 17);
        m_geometry->m_childVisible = 0;
    } else {
        m_geometry = 0;
    }
    if (parent)
        m_terrain = parent->m_terrain;
    LoadTexture(file);
    if (level && m_childTable) {
        m_childOffsets = new int[256];
        m_children = new DrawableGridNode *[256];
        LpDataAccess->FSeek(file, m_childTable, 0);
        LpDataAccess->FRead(file, m_childOffsets, 0x400, 1);
        for (int j = 0; j < 256; j++) {
            if (m_childOffsets[j]) {
                LpDataAccess->FSeek(file, m_childOffsets[j], 0);
                m_children[j] = New(file, level, this, m_level - 1, m_shift - 4, (m_x + j % 16) * 16,
                                    (m_y + j / 16) * 16, j, min, max);
            } else {
                m_children[j] = 0;
            }
        }
    }
    return this;
}


// ---- src/DrawableGridNode/New.cpp
struct _D3DVECTOR;

// The PS2 build stubs out DrawableGridNode's texture interface.

DrawableGridNode *DrawableGridNode::New(long, int, DrawableGridNode *, int, int, int, int, int,
                                        _D3DVECTOR *, _D3DVECTOR *)
{
    return 0;
}


// ---- src/DrawableGridNode/LoadTexture.cpp
struct _D3DVECTOR;

// The PS2 build stubs out DrawableGridNode's texture interface.

void DrawableGridNode::LoadTexture(long)
{
}


// ---- src/DrawableGridNode/dtor.cpp
extern "C" void free(void *p);




DrawableGridNode::~DrawableGridNode()
{
    if (m_textures) {
        free(m_textures);
        m_textures = 0;
    }
}


// ---- src/global/RemoveBuffers.cpp
extern "C" void free(void *p);

extern int WaterMark;  // bytes of texture buffers allocated



int RemoveBuffers(void *item, void *context)
{
    GridNodeGeometry *b = ((DrawableGridNode *)item)->m_geometry;
    if (b->m_vertices) {
        WaterMark -= b->m_verticesSize;
        free(b->m_vertices);
        b->m_vertices = 0;
        b->m_numVertices = 0;
        b->m_verticesSize = 0;
    }
    if (b->m_indices) {
        WaterMark -= b->m_indicesSize;
        free(b->m_indices);
        b->m_indices = 0;
        b->m_numIndices = 0;
        b->m_indicesSize = 0;
    }
    b->m_ageItem.m_size = 0;
    b->m_x126 = 0;
    b->m_x122 = 0;
    b->m_x128 = 0xFFFF;
    b->m_x124 = 0xFFFF;
    b->m_numStrips = 0;
    b->m_bits.m_changed = 1;
    b->m_bits.m_hasMesh = 0;
    return 1;
}


// ---- src/DrawableGridNode/DestroyMesh.cpp
extern "C" void free(void *p);

extern int WaterMark;               // bytes of mesh memory in use






// Frees this node's mesh buffers (and its children's when asked).
void DrawableGridNode::DestroyMesh(int recurse)
{
    if (m_geometry) {
        if (m_parent) {
            m_parent->m_geometry->m_numChildStrips -= m_geometry->m_numStrips + m_geometry->m_numChildStrips;
            if (m_parent->m_geometry->m_numChildStrips < 0)
                m_parent->m_geometry->m_numChildStrips = 0;
        }
        m_geometry->m_numStrips = 0;
        m_geometry->m_bits.m_changed = 1;
        m_geometry->m_bits.m_hasMesh = 0;
        if (recurse && m_children) {
            for (int i = 0; i < 256; i++) {
                if (m_children[i])
                    m_children[i]->DestroyMesh(recurse);
            }
        }
        if (m_geometry->m_vertices) {
            WaterMark -= m_geometry->m_verticesSize;
            free(m_geometry->m_vertices);
            m_geometry->m_vertices = 0;
            m_geometry->m_numVertices = 0;
            m_geometry->m_verticesSize = 0;
        }
        if (m_geometry->m_indices) {
            WaterMark -= m_geometry->m_indicesSize;
            free(m_geometry->m_indices);
            m_geometry->m_indices = 0;
            m_geometry->m_numIndices = 0;
            m_geometry->m_indicesSize = 0;
        }
        if (m_geometry->m_numChildStrips)
            m_geometry->m_numChildStrips = 0;
        m_geometry->m_x126 = 0;
        m_geometry->m_x122 = 0;
        m_geometry->m_x128 = 0xFFFF;
        m_geometry->m_x124 = 0xFFFF;
        if (m_geometry->m_ageItem.m_size) {
            m_terrain->m_meshAgeManager->RemoveItem(&m_geometry->m_ageItem);
            m_geometry->m_ageItem.m_size = 0;
        }
    }
}


// ---- src/DrawableGridNode/MakeMesh.cpp
void DrawableGridNode::MakeMesh()
{
    if (!m_terrain->m_isPlain)
        MakeMeshViaQuadTree(0, 0, 0x10, 0);
}


// ---- src/DrawableGridNode/MakeMeshViaQuadTree.cpp
struct _D3DMATRIX;
struct _D3DVECTOR;
struct _D3DLVERTEX;

extern LightManager *LpLightManager;

// Child offset of each quarter of the 16x16 child grid.
extern int QuadIndex[];




int DrawableGridNode::MakeMeshViaQuadTree(int x, int y, int size, int quad)
{
    if (m_x6C) {
        if (size == 16)
            m_terrain->m_meshesBuilt++;
        if (m_children && !m_geometry->m_bits.m_hasMesh) {
            size >>= 1;
            if (size == 8) {
                m_geometry->m_numChildStrips = 0;
                if (m_x6C & 0x2)
                    MakeMeshViaQuadTree(x, y, size, 0);
                if (m_x6C & 0x4)
                    MakeMeshViaQuadTree(x + size, y, size, 4);
                if (m_x6C & 0x8)
                    MakeMeshViaQuadTree(x, y + size, size, 8);
                if (m_x6C & 0x10)
                    MakeMeshViaQuadTree(x + size, y + size, size, 12);
            } else if (size == 4) {
                if (m_x6C & (0x20 << quad))
                    MakeMeshViaQuadTree(x, y, size, quad);
                if (m_x6C & (0x40 << quad))
                    MakeMeshViaQuadTree(x + size, y, size, quad + 1);
                if (m_x6C & (0x80 << quad))
                    MakeMeshViaQuadTree(x, y + size, size, quad + 2);
                if (m_x6C & (0x100 << quad))
                    MakeMeshViaQuadTree(x + size, y + size, size, quad + 3);
            } else {
                int row;
                int column;
                int *index;
                index = &QuadIndex[quad];
                for (row = 0; row < 64; row += 16) {
                    for (column = 0; column < 4; column++) {
                        DrawableGridNode *child = m_children[row + *index + column];
                        if (child && child->m_x6C)
                            child->MakeMeshViaQuadTree(0, 0, 16, 0);
                    }
                }
            }
            if (size == 8)
                size = 16;
        }
        if (size == 16 && m_geometry->m_bits.m_changed) {
            int before = m_geometry->m_numStrips + m_geometry->m_numChildStrips;
            if (m_geometry->m_bits.m_as4x4)
                ConstructMeshAs4x4();
            else
                ConstructMesh();
            int after = m_geometry->m_numStrips + m_geometry->m_numChildStrips;
            if ((before >= 2 && after < 2) || (before < 2 && after >= 2)) {
                if (m_parent)
                    m_parent->m_geometry->m_bits.m_changed = 1;
            }
            m_geometry->m_bits.m_changed = 0;
            if (m_geometry->m_bits.m_lit && m_geometry->m_numVertices)
                LpLightManager->LightVertices(&m_terrain->m_xC70, m_geometry->m_numVertices,
                                              (_D3DVECTOR *)(m_geometry->m_vertices + 0xC),
                                              (_D3DLVERTEX *)m_geometry->m_vertices, 0x20);
        }
        if (m_parent)
            m_parent->m_geometry->m_numChildStrips += m_geometry->m_numStrips + m_geometry->m_numChildStrips;
    }
    return 1;
}


// ---- src/DrawableGridNode/ConstructMeshAs4x4.cpp
extern "C" void *malloc(unsigned int size);
extern "C" void free(void *p);
extern "C" void *memcpy(void *dst, const void *src, unsigned int n);

extern int WaterMark;               // bytes of mesh memory in use
extern int HighWaterMark;
extern int xBase;
extern int yBase;

class DrawableGridNode;

extern VertexFinderTool VertexFinder;

extern short FaceData[];                // three vertex indices per face



int RemoveBuffers(void *item, void *context);








// Builds the mesh as sixteen 4x4 blocks, one after another in the vertex
// and index buffers, recording each block's running totals and height range.
int DrawableGridNode::ConstructMeshAs4x4()
{
    int oldStrips;
    int y;
    int numIndices;
    int x;
    int child;
    float uvScale;
    int numVertices;
    int j;
    int done;
    int i;
    int size;
    int startVertices;
    int startIndices;
    int face;
    int start;
    int indicesSize;
    int verticesSize;
    int newSize;
    int oldSize;
    void *old;
    oldStrips = m_geometry->m_numStrips;
    // The frame counter bump goes through numVertices: that earlier live range
    // makes the vertex count a split temporary, which gives retail's $s5.
    numVertices = m_terrain->m_x100;
    m_terrain->m_x100 = numVertices + 1;
    uvScale = m_uvScale;
    y = 0;
    child = 0;
    numVertices = 0;
    numIndices = 0;
    for (j = 0; j < 4; j++) {
        x = 0;
        for (i = 0; i < 4; i++, child++) {
            size = GetAuthoredDetailTextureMapSize(child);
            m_uvScale = uvScale;
            if (size == 64)
                m_uvScale *= 4.0f;
            VertexFinder.ReInitialize(this);
            xBase = i * 4;
            yBase = j * 4;
            startVertices = numVertices;
            startIndices = numIndices;
            face = TriangulateAxisTriangleLOD(4, 0, x, y + 4, 2, -2, &done);
            if (face == 0 && (m_geometry->m_bits.m_hasMesh || !m_geometry->m_numChildStrips) && m_parent && !m_parent->m_textures->m_texture)
                face = MakeAxisTriangle(face, x, y + 4, 4, -4);
            start = face;
            face = TriangulateAxisTriangleLOD(4, face, x + 4, y, -2, 2, &done);
            if (face == start && (m_geometry->m_bits.m_hasMesh || !m_geometry->m_numChildStrips) && m_parent && !m_parent->m_textures->m_texture)
                face = MakeAxisTriangle(face, x + 4, y, -4, 4);
            numVertices += VertexFinder.m_count;
            numIndices += face * 3;
            m_geometry->m_childHeights[child].m_center = (VertexFinder.m_minY + VertexFinder.m_maxY) / 2.0f;
            m_geometry->m_childHeights[child].m_halfHeight = m_terrain->m_gridScale * (VertexFinder.m_maxY - VertexFinder.m_minY) / 2.0f;
            m_geometry->m_childHeights[child].m_numVertices = numVertices;
            m_geometry->m_childHeights[child].m_numIndices = numIndices;
            m_textures->m_detail[child].m_x1C = 1.0f;
            m_textures->m_detail[child].m_x20 = 0;
            m_textures->m_detail[child].m_x24 = 0;
            if (face) {
                indicesSize = (numVertices + numIndices) * 2;
                verticesSize = numVertices * 32;
                if (!m_geometry->m_vertices || m_geometry->m_verticesSize < verticesSize) {
                    // retail masks the size to 16 bits before adding the slack
                    newSize = (verticesSize & 0xFFFF) + 0x80;
                    WaterMark -= m_geometry->m_verticesSize;
                    oldSize = m_geometry->m_verticesSize;
                    old = m_geometry->m_vertices;
                    m_geometry->m_verticesSize = newSize;
                    if (!(m_geometry->m_vertices = (char *)malloc(m_geometry->m_verticesSize))) {
                        m_geometry->m_verticesSize = 0;
                        m_geometry->m_numVertices = 0;
                        m_geometry->m_numIndices = 0;
                        return 0;
                    }
                    if (old) {
                        memcpy(m_geometry->m_vertices, old, oldSize);
                        free(old);
                    }
                    WaterMark += m_geometry->m_verticesSize;
                    if (WaterMark > HighWaterMark)
                        HighWaterMark = WaterMark;
                }
                if (!m_geometry->m_indices || m_geometry->m_indicesSize < indicesSize) {
                    newSize = (indicesSize & 0xFFFF) + 0x20;
                    WaterMark -= m_geometry->m_indicesSize;
                    old = m_geometry->m_indices;
                    oldSize = m_geometry->m_indicesSize;
                    m_geometry->m_indicesSize = newSize;
                    if (!(m_geometry->m_indices = malloc(m_geometry->m_indicesSize))) {
                        m_geometry->m_indices = 0;
                        m_geometry->m_indicesSize = 0;
                        m_geometry->m_numVertices = 0;
                        m_geometry->m_numIndices = 0;
                        return 0;
                    }
                    if (old) {
                        memcpy(m_geometry->m_indices, old, oldSize);
                        free(old);
                    }
                    WaterMark += m_geometry->m_indicesSize;
                    if (WaterMark > HighWaterMark)
                        HighWaterMark = WaterMark;
                }
                memcpy((char *)m_geometry->m_vertices + startVertices * 32, VertexFinder.m_vertices, (numVertices - startVertices) * 32);
                memcpy((char *)m_geometry->m_indices + startIndices * 2, FaceData, (numIndices - startIndices) * 2);
            }
            x += 4;
        }
        y += 4;
    }
    if (!m_geometry->m_ageItem.m_size)
        m_terrain->m_meshAgeManager->AddItem(&m_geometry->m_ageItem, RemoveBuffers, this, 0,
                                             m_geometry->m_verticesSize + m_geometry->m_indicesSize);
    else if (m_geometry->m_ageItem.m_size != m_geometry->m_verticesSize + m_geometry->m_indicesSize)
        m_geometry->m_ageItem.m_size = m_geometry->m_verticesSize + m_geometry->m_indicesSize;
    m_geometry->m_numVertices = numVertices;
    m_geometry->m_numIndices = numIndices;
    m_geometry->m_numStrips = numIndices / 3;
    // retail runs this test twice
    if ((m_geometry->m_numStrips == 0 || oldStrips == 0) && m_geometry->m_numStrips != oldStrips && m_parent)
        m_parent->m_geometry->m_bits.m_changed = 1;
    if ((m_geometry->m_numStrips == 0 || oldStrips == 0) && m_geometry->m_numStrips != oldStrips && m_parent)
        m_parent->m_geometry->m_bits.m_changed = 1;
    if (m_geometry->m_numStrips == 0) {
        m_geometry->m_numVertices = 0;
        m_geometry->m_numIndices = 0;
    }
    m_uvScale = uvScale;
    return 1;
}


// ---- src/DrawableGridNode/GetAuthoredDetailTextureMapSize.cpp
struct _D3DVECTOR;

// The PS2 build stubs out DrawableGridNode's texture interface.

int DrawableGridNode::GetAuthoredDetailTextureMapSize(int)
{
    return 0;
}


// ---- src/DrawableGridNode/ConstructMesh.cpp
extern "C" void *malloc(unsigned int size);
extern "C" void free(void *p);
extern "C" void *memcpy(void *dst, const void *src, unsigned int n);

extern int WaterMark;               // bytes of mesh memory in use
extern int HighWaterMark;
extern int xBase;
extern int yBase;

class DrawableGridNode;

extern VertexFinderTool VertexFinder;

extern short FaceData[];                // three vertex indices per face



int RemoveBuffers(void *item, void *context);






// Triangulates the node's grid into FaceData/VertexFinder, then copies the
// result into the node's (re)allocated vertex and index buffers.
int DrawableGridNode::ConstructMesh()
{
    int done;
    int oldStrips = m_geometry->m_numStrips;
    m_terrain->m_x100++;
    xBase = 0;
    yBase = 0;
    VertexFinder.ReInitialize(this);
    int position = m_position & 0x11;
    int face;
    int start;
    if (position == 0x11 || position == 0) {
        face = TriangulateAxisTriangleLOD(8, 0, 0, 16, 8, -8, &done);
        if (face == 0 && (m_geometry->m_bits.m_hasMesh || !m_geometry->m_numChildStrips) && m_parent && !m_parent->m_textures->m_texture)
            face = MakeAxisTriangle(face, 0, 16, 16, -16);
        start = face;
        face = TriangulateAxisTriangleLOD(8, face, 16, 0, -8, 8, &done);
        if (face == start && (m_geometry->m_bits.m_hasMesh || !m_geometry->m_numChildStrips) && m_parent && !m_parent->m_textures->m_texture)
            face = MakeAxisTriangle(face, 16, 0, -16, 16);
    } else {
        face = TriangulateAxisTriangleLOD(8, 0, 0, 0, 8, 8, &done);
        if (face == 0 && (m_geometry->m_bits.m_hasMesh || !m_geometry->m_numChildStrips) && m_parent && !m_parent->m_textures->m_texture)
            face = MakeAxisTriangle(face, 0, 0, 16, 16);
        start = face;
        face = TriangulateAxisTriangleLOD(8, face, 16, 16, -8, -8, &done);
        if (face == start && (m_geometry->m_bits.m_hasMesh || !m_geometry->m_numChildStrips) && m_parent && !m_parent->m_textures->m_texture)
            face = MakeAxisTriangle(face, 16, 16, -16, -16);
    }
    m_geometry->m_numStrips = face;
    // `== 0`, not `!`: `!` on the short re-sign-extends it
    if ((m_geometry->m_numStrips == 0 || oldStrips == 0) && m_geometry->m_numStrips != oldStrips && m_parent)
        m_parent->m_geometry->m_bits.m_changed = 1;
    if (!m_geometry->m_numStrips) {
        m_geometry->m_numVertices = 0;
        m_geometry->m_numIndices = 0;
    } else {
        m_geometry->m_numVertices = VertexFinder.m_count;
        m_geometry->m_numIndices = m_geometry->m_numStrips * 3;
        int verticesSize = m_geometry->m_numVertices * 32;
        int indicesSize = (m_geometry->m_numVertices + m_geometry->m_numIndices) * 2;
        if (!m_geometry->m_vertices || m_geometry->m_verticesSize < verticesSize) {
            WaterMark -= m_geometry->m_verticesSize;
            if (m_geometry->m_vertices)
                free(m_geometry->m_vertices);
            // retail masks the size to 16 bits before adding the slack
            m_geometry->m_verticesSize = (verticesSize & 0xFFFF) + 0x80;
            if (!(m_geometry->m_vertices = (char *)malloc(m_geometry->m_verticesSize))) {
                m_geometry->m_verticesSize = 0;
                m_geometry->m_numVertices = 0;
                m_geometry->m_numIndices = 0;
                return 0;
            }
            WaterMark += m_geometry->m_verticesSize;
            if (WaterMark > HighWaterMark)
                HighWaterMark = WaterMark;
        }
        if (!m_geometry->m_indices || m_geometry->m_indicesSize < indicesSize) {
            WaterMark -= m_geometry->m_indicesSize;
            if (m_geometry->m_indices)
                free(m_geometry->m_indices);
            m_geometry->m_indicesSize = (indicesSize & 0xFFFF) + 0x20;
            if (!(m_geometry->m_indices = malloc(m_geometry->m_indicesSize))) {
                m_geometry->m_indices = 0;
                m_geometry->m_indicesSize = 0;
                m_geometry->m_numVertices = 0;
                m_geometry->m_numIndices = 0;
                return 0;
            }
            WaterMark += m_geometry->m_indicesSize;
            if (WaterMark > HighWaterMark)
                HighWaterMark = WaterMark;
        }
        memcpy(m_geometry->m_vertices, VertexFinder.m_vertices, m_geometry->m_numVertices * 32);
        memcpy(m_geometry->m_indices, FaceData, m_geometry->m_numIndices * 2);
        if (!m_geometry->m_ageItem.m_size)
            m_terrain->m_meshAgeManager->AddItem(&m_geometry->m_ageItem, RemoveBuffers, this, 0,
                                                 m_geometry->m_verticesSize + m_geometry->m_indicesSize);
        else if (m_geometry->m_ageItem.m_size != m_geometry->m_verticesSize + m_geometry->m_indicesSize)
            m_geometry->m_ageItem.m_size = m_geometry->m_verticesSize + m_geometry->m_indicesSize;
    }
    m_textures->m_x28 = 1.0f;
    m_textures->m_x2C = 0;
    m_textures->m_x30 = 0;
    return 1;
}


// ---- src/DrawableGridNode/TriangulateAxisTriangleLOD.cpp
// Start index of each row of the 17x17 vertex grid.

extern VertexFinderTool VertexFinder;

extern short FaceData[];    // three vertex indices per face



int DrawableGridNode::TriangulateAxisTriangleLOD(int level, int face, int x, int y, int dx, int dy, int *done)
{
    x += dx;
    y += dy;
    if (!(m_geometry->m_vertexFlags[x + SizeVerticesBy[y]] & 0x80)) {
        *done = 0;
        return face;
    }
    int doneA;
    int doneB;
    if (level - 1) {
        face = TriangulateDiagTriangleLOD(level - 1, face, x, y, -dx, 0, &doneA);
        face = TriangulateDiagTriangleLOD(level - 1, face, x, y, 0, -dy, &doneB);
    } else {
        if (m_children) {
            *done = 1;
            return face;
        }
        doneA = doneB = 0;
    }
    if (!doneA) {
        FaceData[face * 3] = VertexFinder.PointIndex(x, y);
        FaceData[face * 3 + 1] = VertexFinder.PointIndex(x - dx, y - dx);
        FaceData[face * 3 + 2] = VertexFinder.PointIndex(x - dx, y + dx);
        face++;
    }
    if (!doneB) {
        FaceData[face * 3] = VertexFinder.PointIndex(x, y);
        y -= dy;
        FaceData[face * 3 + 1] = VertexFinder.PointIndex(x + dy, y);
        FaceData[face * 3 + 2] = VertexFinder.PointIndex(x - dy, y);
        face++;
    }
    *done = 1;
    return face;
}


// ---- src/DrawableGridNode/TesselateLowestTriangles.cpp
// Start index of each row of the 17x17 vertex grid.

extern VertexFinderTool VertexFinder;

extern short FaceData[];    // three vertex indices per face





// Two triangles around (x, y) at the finest level; a corner moves by one
// (`1 - bit` is how retail negates the 1-bit flag: `!bit` compiles to sltu/xori)
// when the neighbouring vertex is switched on.
void DrawableGridNode::TesselateLowestTriangles(int x, int y, int orientation, int direction, int face)
{
    int i = face * 3;
    int v = x + SizeVerticesBy[y];
    unsigned char a;
    unsigned char b;
    if (orientation == 0) {
        if (direction == 1) {
            a = m_lodData->verts[v - 18].tessFlip && (m_geometry->m_vertexFlags[v - 18] & 0x80);
            FaceData[i] = VertexFinder.PointIndex(x, y);
            FaceData[i + 1] = VertexFinder.PointIndex(x, y - 1);
            FaceData[i + 2] = VertexFinder.PointIndex(x - 1, y - a);
            b = (1 - m_lodData->verts[v - 17].tessFlip) && (m_geometry->m_vertexFlags[v - 16] & 0x80);
            FaceData[i + 3] = VertexFinder.PointIndex(x, y);
            FaceData[i + 4] = VertexFinder.PointIndex(x + 1, y - b);
            FaceData[i + 5] = VertexFinder.PointIndex(x, y - 1);
        } else {
            a = m_lodData->verts[v].tessFlip && (m_geometry->m_vertexFlags[v + 18] & 0x80);
            FaceData[i] = VertexFinder.PointIndex(x, y);
            FaceData[i + 1] = VertexFinder.PointIndex(x, y + 1);
            FaceData[i + 2] = VertexFinder.PointIndex(x + 1, y + a);
            b = (1 - m_lodData->verts[v - 1].tessFlip) && (m_geometry->m_vertexFlags[v + 16] & 0x80);
            FaceData[i + 3] = VertexFinder.PointIndex(x, y);
            FaceData[i + 4] = VertexFinder.PointIndex(x - 1, y + b);
            FaceData[i + 5] = VertexFinder.PointIndex(x, y + 1);
        }
    } else if (orientation == 1) {
        a = m_lodData->verts[v - 18].tessFlip && (m_geometry->m_vertexFlags[v - 18] & 0x80);
        FaceData[i] = VertexFinder.PointIndex(x, y);
        FaceData[i + 1] = VertexFinder.PointIndex(x, y - 1);
        FaceData[i + 2] = VertexFinder.PointIndex(x - 1, y - a);
        b = (1 - m_lodData->verts[v - 1].tessFlip) && (m_geometry->m_vertexFlags[v + 16] & 0x80);
        FaceData[i + 3] = VertexFinder.PointIndex(x, y);
        FaceData[i + 4] = VertexFinder.PointIndex(x - 1, y + b);
        FaceData[i + 5] = VertexFinder.PointIndex(x, y + 1);
    } else {
        a = m_lodData->verts[v].tessFlip && (m_geometry->m_vertexFlags[v + 18] & 0x80);
        FaceData[i] = VertexFinder.PointIndex(x, y);
        FaceData[i + 1] = VertexFinder.PointIndex(x, y + 1);
        FaceData[i + 2] = VertexFinder.PointIndex(x + 1, y + a);
        b = (1 - m_lodData->verts[v - 17].tessFlip) && (m_geometry->m_vertexFlags[v - 16] & 0x80);
        FaceData[i + 3] = VertexFinder.PointIndex(x, y);
        FaceData[i + 4] = VertexFinder.PointIndex(x + 1, y - b);
        FaceData[i + 5] = VertexFinder.PointIndex(x, y - 1);
    }
}


// ---- src/DrawableGridNode/TriangulateDiagTriangleLOD.cpp
// TriangulateDiagTriangleLOD__16DrawableGridNodeFiiiiiiPi  @ 0x0023ABA0

extern int SizeBy[];        // child-table row offsets (16 per row)

extern VertexFinderTool VertexFinder;

extern short FaceData[];    // three vertex indices per face



// A child covers the triangle itself when it has a mesh of its own (or its
// children do) or isn't drawn.
static inline bool IsEmpty(DrawableGridNode *child)
{
    return child && child->m_geometry->m_numStrips +
                    (child->m_geometry->m_bits.m_hasMesh ? 0 : child->m_geometry->m_numChildStrips) == 0;
}

static inline int Covers(DrawableGridNode *node, int x, int y)
{
    return (IsEmpty(node->m_children[x + SizeBy[y]]) && node->m_children[x + SizeBy[y]]->m_x6C) ? 0 : 1;
}

// The call sites below that are not plain Covers() differ only in how mwcc numbers the
// child, geometry, strip-count and flag temporaries: retail colours those sites as if
// child/strips were address-taken locals (CoversA), the child a named local (dy > 0, doneB),
// the geometry a local (dy <= 0, doneB), child and geometry both block locals (dx > 0,
// doneB), or the covered flag a bool local set by an if (dx < 0, doneB).
static inline int ChildStrips(GridNodeGeometry *g)
{
    return g->m_bits.m_hasMesh ? 0 : g->m_numChildStrips;
}

static inline bool IsEmptyA(DrawableGridNode *child, int *strips)
{
    return child && child->m_geometry->m_numStrips + (*strips = ChildStrips(child->m_geometry)) == 0;
}

static inline DrawableGridNode *Set(DrawableGridNode **p, DrawableGridNode *v)
{
    *p = v;
    return *p;
}

static inline int CoversA(DrawableGridNode *node, int x, int y)
{
    int strips;
    DrawableGridNode *child;
    return (IsEmptyA(Set(&child, node->m_children[x + SizeBy[y]]), &strips) && child->m_x6C) ? 0 : 1;
}

static inline bool IsEmptyG(DrawableGridNode *child)
{
    GridNodeGeometry *geom;
    return child && (geom = child->m_geometry, geom->m_numStrips + (geom->m_bits.m_hasMesh ? 0 : geom->m_numChildStrips) == 0);
}

static inline int CoversG(DrawableGridNode *node, int x, int y)
{
    return (IsEmptyG(node->m_children[x + SizeBy[y]]) && node->m_children[x + SizeBy[y]]->m_x6C) ? 0 : 1;
}

// The geometry is the caller's own block local here (declared before the child, which
// numbers it above the child).
static inline bool IsEmptyR(DrawableGridNode *child, GridNodeGeometry *&geom)
{
    return child && (geom = child->m_geometry, geom->m_numStrips + (geom->m_bits.m_hasMesh ? 0 : geom->m_numChildStrips) == 0);
}

int DrawableGridNode::TriangulateDiagTriangleLOD(int level, int face, int x, int y, int dx, int dy, int *done)
{
    int doneA;
    int doneB;
    if (dx == 0)
        y += dy;
    else
        x += dx;
    if (!(m_geometry->m_vertexFlags[x + SizeVerticesBy[y]] & 0x80)) {
        *done = 0;
        return face;
    }
    if (level - 1) {
        if (dx == 0) {
            int half = (dy < 0) ? (-dy >> 1) : -(dy >> 1);
            face = TriangulateAxisTriangleLOD(level - 1, face, x, y, half, half, &doneA);
            face = TriangulateAxisTriangleLOD(level - 1, face, x, y, -half, half, &doneB);
        } else {
            int half = (dx < 0) ? (-dx >> 1) : -(dx >> 1);
            face = TriangulateAxisTriangleLOD(level - 1, face, x, y, half, half, &doneA);
            face = TriangulateAxisTriangleLOD(level - 1, face, x, y, half, -half, &doneB);
        }
    } else {
        if (m_children) {
        if (m_geometry->m_bits.m_hasMesh) {
            doneA = doneB = 0;
        } else if (dx == 0) {
            if (dy > 0) {
                doneA = CoversA(this, x - 1, y - 1);
                {
                    DrawableGridNode *child = m_children[x + SizeBy[y - 1]];
                    doneB = (IsEmpty(child) && child->m_x6C) ? 0 : 1;
                }
            } else {
                doneA = Covers(this, x, y);
                doneB = CoversG(this, x - 1, y);
            }
        } else {
            if (dx > 0) {
                doneA = Covers(this, x - 1, y - 1);
                {
                    GridNodeGeometry *geom;
                    DrawableGridNode *child = m_children[x - 1 + SizeBy[y]];
                    doneB = (IsEmptyR(child, geom) && child->m_x6C) ? 0 : 1;
                }
            } else {
                doneA = Covers(this, x, y);
                {
                    bool res = 0;
                    if (IsEmptyG(m_children[x + SizeBy[y - 1]]) && m_children[x + SizeBy[y - 1]]->m_x6C)
                        res = 1;
                    doneB = res ? 0 : 1;
                }
            }
        }
        } else {
            TesselateLowestTriangles(x, y, dx, dy, face);
            *done = 1;
            return face + 2;
        }
    }
    if (!doneA) {
        int i = face * 3;
        if (dx == 0) {
            FaceData[i] = VertexFinder.PointIndex(x, y);
            FaceData[i + 1] = VertexFinder.PointIndex(x, y - dy);
            FaceData[i + 2] = VertexFinder.PointIndex(x - dy, y);
        } else {
            FaceData[i] = VertexFinder.PointIndex(x, y);
            FaceData[i + 1] = VertexFinder.PointIndex(x, y - dx);
            FaceData[i + 2] = VertexFinder.PointIndex(x - dx, y);
        }
        face++;
    }
    if (!doneB) {
        int i = face * 3;
        if (dx == 0) {
            FaceData[i] = VertexFinder.PointIndex(x, y);
            FaceData[i + 1] = VertexFinder.PointIndex(x + dy, y);
            FaceData[i + 2] = VertexFinder.PointIndex(x, y - dy);
        } else {
            FaceData[i] = VertexFinder.PointIndex(x, y);
            FaceData[i + 1] = VertexFinder.PointIndex(x - dx, y);
            FaceData[i + 2] = VertexFinder.PointIndex(x, y + dx);
        }
        face++;
    }
    *done = 1;
    return face;
}


// ---- src/DrawableGridNode/MakeAxisTriangle.cpp
extern VertexFinderTool VertexFinder;

extern short FaceData[];    // three vertex indices per face


// Add the right triangle with legs dx and dy at (x, y); returns the next
// face index. The winding flips when the legs are equal.
int DrawableGridNode::MakeAxisTriangle(int face, int x, int y, int dx, int dy)
{
    int i = face * 3;
    if (dx == dy) {
        FaceData[i + 2] = VertexFinder.PointIndex(x, y);
        FaceData[i] = VertexFinder.PointIndex(x, y + dy);
        FaceData[i + 1] = VertexFinder.PointIndex(x + dx, y);
    } else {
        FaceData[i] = VertexFinder.PointIndex(x, y);
        FaceData[i + 1] = VertexFinder.PointIndex(x + dx, y);
        FaceData[i + 2] = VertexFinder.PointIndex(x, y + dy);
    }
    return face + 1;
}


// ---- src/DrawableGridNode/SetTextureMap.cpp
struct _D3DVECTOR;

// The PS2 build stubs out DrawableGridNode's texture interface.

void DrawableGridNode::SetTextureMap()
{
}


// ---- src/DrawableGridNode/GetTextureMap.cpp
struct _D3DVECTOR;

// The PS2 build stubs out DrawableGridNode's texture interface.

TextureMap *DrawableGridNode::GetTextureMap()
{
    return 0;
}


// ---- src/DrawableGridNode/RenderDetail.cpp
// RenderDetail__16DrawableGridNodeFP10TextureMap  @ 0x0023B5B0






extern PSXTextureMapManager *LpTextureMapManager;


struct _sceDmaTag;

extern unsigned int TerrainScisPreambleCode[];
extern unsigned int TerrainScisHalfResCode[];
extern unsigned int TerrainScissorCode[];

extern int CurrScisItemIndex;

extern VisibleTest *VisTest;

class DrawableGridNode;




// A GS register write (A+D): the value, then the register.
struct GsRegData {
    unsigned long data;
    unsigned long reg;
};

#define SCE_GS_SET_TEX0(tbp, tbw, psm, tw, th, tcc, tfx, cbp, cpsm, csm, csa, cld) \
    ((unsigned long)(tbp) | ((unsigned long)(tbw) << 14) | ((unsigned long)(psm) << 20) | ((unsigned long)(tw) << 26) | \
     ((unsigned long)(th) << 30) | ((unsigned long)(tcc) << 34) | ((unsigned long)(tfx) << 35) | ((unsigned long)(cbp) << 37) | \
     ((unsigned long)(cpsm) << 51) | ((unsigned long)(csm) << 55) | ((unsigned long)(csa) << 56) | ((unsigned long)(cld) << 61))

int DrawableGridNode::RenderDetail(TextureMap *detail)
{
    // Declaration order picks retail's s-registers.
    int nearCount;
    int wasNear;
    int i;
    int newTex;
    DrawableGridNode *node;
    PSXTextureMap *tex;
    TextureMap *lastTex;
    PSXTextureMap *locked;
    _D3DMATRIX *view;
    GsRegData tex0;
    GsRegData detailTex0;
    lastTex = 0;
    nearCount = 0;
    wasNear = 0;
    static int useTextures = 1;
    locked = 0;
    view = &((PSXCamera *)LpRenderTarget->m_camera)->m_guardBandViewProjection;
    if (detail) {
        LpTextureMapManager->LockTextureSlot(locked = (PSXTextureMap *)detail);
        LpTextureMapManager->AddTextureInterruptToRenderQueue();
        PSXMipSurface *mip = ((PSXTextureMap *)detail)->GetReqdMipLevel();
        detailTex0.reg = 6;     // TEX0_1
        int psm;
        int cbp = 0;
        int cpsm = 0;
        int csm = 0;
        int csa = 0;
        int cld = 0;
        switch (detail->m_format) {
        case 0x22B8:
        case 0x115C:
            psm = 0;            // PSMCT32
            break;
        case 0x613:
        case 0x22B:
        case 0x235:
            psm = 2;            // PSMCT16
            break;
        case 8:
            cbp = ((PSXTextureMap *)detail)->m_xF0;
            psm = 0x13;         // PSMT8
            cld = 1;
            break;
        case 4:
            cbp = ((PSXTextureMap *)detail)->m_xF0;
            psm = 0x14;         // PSMT4
            cld = 1;
            break;
        default:
#line 2048 "Griddraw.cpp"
            ASSERT(0, "");
            break;
        }
        detailTex0.data = SCE_GS_SET_TEX0(locked->m_xEC, mip->m_blocks, psm, mip->m_widthLog2, mip->m_heightLog2,
                                          1, 0, cbp, cpsm, csm, csa, cld);
    }
    CurrScisItemIndex = 0;
    for (i = 0; i < m_terrain->m_numVisibleNodes; i++) {
        node = m_terrain->m_visible[i];
        tex = (PSXTextureMap *)node->m_textures->m_texture;
        newTex = 0;
        if ((!useTextures && !lastTex) || (useTextures == 1 && tex != lastTex)) {
            newTex = 1;
            lastTex = tex;
            LpTextureMapManager->AddTextureToGifList(tex);
            LpTextureMapManager->AddTextureInterruptToRenderQueue();
            PSXMipSurface *mip = tex->GetReqdMipLevel();
            tex0.data = SCE_GS_SET_TEX0(tex->m_xEC, mip->m_blocks, 2, mip->m_widthLog2, mip->m_heightLog2,
                                        1, 0, 0, 0, 0, 0, 0);
            tex0.reg = 6;       // TEX0_1
        }
        static float distLimit;
        distLimit = 2.0f;
        if (node->m_clipCodes) {
            VisTest->BoundingBoxVisible(view, &node->m_center, &node->m_halfSize, &node->m_clipCodes);
            node->m_clipCodes &= ~0x10;
        }
        if (nearCount < 8 && node->m_clipCodes) {
            if (!wasNear) {
                newTex = 1;
                LpRenderTarget->AddMPGToRenderQueue(TerrainScisPreambleCode);
                LpRenderTarget->AddMPGToRenderQueue(m_terrain->m_halfRes ? TerrainScisHalfResCode : TerrainScissorCode);
            }
            wasNear = 1;
            node->RenderNearClipGeometry(lastTex, detail, newTex, (QWORD *)&tex0, (QWORD *)&detailTex0);
            nearCount++;
        } else {
            if (wasNear)
                newTex = 1;
            wasNear = 0;
            node->RenderPlainGeometry(lastTex, detail, newTex, (QWORD *)&tex0, (QWORD *)&detailTex0);
        }
    }
    if (detail)
        LpTextureMapManager->UnlockTextureSlot(locked);
    return 1;
}


// ---- src/DrawableGridNode/SetDetailTextureMap.cpp
struct _D3DVECTOR;

// The PS2 build stubs out DrawableGridNode's texture interface.

void DrawableGridNode::SetDetailTextureMap(int)
{
}


// ---- src/DrawableGridNode/BuildScissorRenderItem.cpp
typedef int int128_t __attribute__((mode(TI)));


// A matrix moved as four quadwords.
struct QMATRIX {
    int128_t r0, r1, r2, r3;
};







// The scissor render item: its VU data from +0x60.
struct ScissorVifItem {
    char pad0[0x60];
    QWORD m_data[1];                    // +0x60
};

// Fills the scissor render item's VU data: camera matrices, this node's
// texture placement and the scissor rectangle.
void DrawableGridNode::BuildScissorRenderItem(VifDataItem *item, QWORD *scissor)
{
    Camera *camera = LpRenderTarget->m_camera;
    QWORD *p = ((ScissorVifItem *)item)->m_data;
    // Hand-written quadword copies of the camera matrices ($t0-$t3).
#ifdef SPLASHDOWN_PC
    // x86-64: the camera's matrices by name (Camera.h, PSXCamera.h)
    __builtin_memcpy(p, &((PSXCamera *)camera)->m_x250, 0x40);
    p += 4;
    __builtin_memcpy(p, &camera->m_viewProjection, 0x40);
    p += 4;
    __builtin_memcpy(p, &camera->m_x140, 0x40);
    p += 4;
    __builtin_memcpy(p, &camera->m_view, 0x40);
#else
    asm("lq $8, 0x250(%1); lq $9, 0x260(%1); lq $10, 0x270(%1); lq $11, 0x280(%1); sq $8, 0x60(%0); sq $9, 0x70(%0); sq $10, 0x80(%0); sq $11, 0x90(%0)"
        : : "r"(item), "r"(camera) : "$8", "$9", "$10", "$11", "memory");
    p += 4;
    asm("lq $8, 0x100(%1); lq $9, 0x110(%1); lq $10, 0x120(%1); lq $11, 0x130(%1); sq $8, 0x0(%0); sq $9, 0x10(%0); sq $10, 0x20(%0); sq $11, 0x30(%0)"
        : : "r"(p), "r"(camera) : "$8", "$9", "$10", "$11", "memory");
    p += 4;
    asm("lq $8, 0x140(%1); lq $9, 0x150(%1); lq $10, 0x160(%1); lq $11, 0x170(%1); sq $8, 0x0(%0); sq $9, 0x10(%0); sq $10, 0x20(%0); sq $11, 0x30(%0)"
        : : "r"(p), "r"(camera) : "$8", "$9", "$10", "$11", "memory");
    p += 4;
    asm("lq $8, 0xC0(%1); lq $9, 0xD0(%1); lq $10, 0xE0(%1); lq $11, 0xF0(%1); sq $8, 0x0(%0); sq $9, 0x10(%0); sq $10, 0x20(%0); sq $11, 0x30(%0)"
        : : "r"(p), "r"(camera) : "$8", "$9", "$10", "$11", "memory");
#endif
    p += 5;
    float *f = (float *)p;
    f[0] = 0.03125f + 0.9375f * m_uOffset;
    f[1] = 0.03125f + 0.9375f * m_vOffset;
    f[2] = 1.0f;
    f[3] = m_plain->m_baseHeight;
    p++;
    f = (float *)p;
    f[0] = m_x;
    f[1] = m_plain->m_baseHeight * m_x10;
    f[2] = m_y;
    f[3] = m_heightScale * m_x10;
    p += 4;
    f = (float *)p;
    if (!(m_terrain->m_gridScale <= 6.0f))
        f[3] = 1.008f;
    else
        f[3] = 1.01f;
    p += 6;
    f = (float *)p;
    f[0] = camera->m_position.x;
    f[1] = camera->m_position.y;
    f[2] = camera->m_position.z;
    f[3] = 1.0f;
    p++;
    f = (float *)p;
    f[0] = 2e-05f;
    f[1] = -16.0f;
    f[2] = LpRenderTarget->m_fogEnd;
    f[3] = LpRenderTarget->m_fogScale;
    p += 5;
    *p = *scissor;
}


// ---- src/DrawableGridNode/RenderNearClipGeometry.cpp
struct _sceDmaTag;
struct QWORD;
struct VifDataItem;
class TextureMap;


extern int CurrScisItemIndex;




// Queues the node through one of 8 rotating scissor render items (a second
// pass with its own scissor when there is a detail texture).
void DrawableGridNode::RenderNearClipGeometry(TextureMap *texture, TextureMap *detail, int arg, QWORD *scissor, QWORD *detailScissor)
{
    static int RenderNearClipGeometryID = -1;   // profiler ID, unused
    if (CurrScisItemIndex >= 8 || CurrScisItemIndex < 0)
        CurrScisItemIndex = 0;
    VifDataItem *item = m_terrain->m_scissorItems[CurrScisItemIndex];
    VifDataItem *detailItem = m_terrain->m_scissorItems2[CurrScisItemIndex];
    BuildScissorRenderItem(item, scissor);
    if (detail)
        BuildScissorRenderItem(detailItem, detailScissor);
    LpRenderTarget->AddToRenderQueue(m_terrain->m_restoreScissor, 1, "");
    LpRenderTarget->AddToRenderQueue((_sceDmaTag *)item, 1, "");
    LpRenderTarget->AddToRenderQueue((_sceDmaTag *)&m_plain->m_qwc, 1, "");
    if (detail) {
        LpRenderTarget->AddToRenderQueue((_sceDmaTag *)detailItem, 1, "");
        LpRenderTarget->AddToRenderQueue((_sceDmaTag *)&m_plain->m_qwc, 1, "");
    }
    CurrScisItemIndex++;
    if (CurrScisItemIndex >= 8)
        CurrScisItemIndex = 0;
}


// ---- src/DrawableGridNode/RenderPlainGeometry.cpp
// Compiler session state: retail compiled
// this file in an mwcc session that had already generated a two-argument runtime
// helper call (64-bit / or %, double arithmetic or compare). mwcc then models
// every later helper call (__floatdisf, fptoui, fptodp, ...) as reading $a0-$a1,
// so temporaries live before such a call skip $a0/$a1. This unused function
// recreates that state; it is not part of the retail file.
static void MWCC_SHIM_TEXT MwccSessionState(long *a) { a[0] = a[0] / a[1]; }

typedef int int128_t __attribute__((mode(TI)));



struct _sceDmaTag;
class TextureMap;

extern unsigned int TerrainRendererHalfResCode[];
extern unsigned int TerrainRendererClipCode[];






void DrawableGridNode::RenderPlainGeometry(TextureMap *texture, TextureMap *detail, int newRenderer, QWORD *a, QWORD *b)
{
    static _D3DVECTOR sourceST;         // unused
    static _D3DVECTOR xzIncrement;      // unused
    static int RenderPlainGeometryID = -1;  // profiler ID, unused
    if (newRenderer) {
        LpRenderTarget->AddMPGToRenderQueue(m_terrain->m_halfRes ? TerrainRendererHalfResCode : TerrainRendererClipCode);
        LpRenderTarget->AddToRenderQueue(m_terrain->m_restoreScissor, 1, "");
    }
    m_xB0 = *b;
    m_xC0 = *a;
    float *data = (float *)&m_xB0;
    static float testFactor = m_terrain->m_gridScale < 4.5 ? 0.7f * m_terrain->m_gridScale : 0.3 * m_terrain->m_gridScale;
    static int color = 0x66;
    int alpha;
    if (detail) {
        if (!(m_textures->m_mipLevel <= m_terrain->m_lodDistance * testFactor))
            alpha = 0x7F;
        else
            alpha = color;
    } else {
        alpha = 0x7E;
    }
    data[8] = 0.03125f + 0.9375f * m_uOffset;
    data[9] = 0.03125f + 0.9375f * m_vOffset;
    ((int *)data)[10] = alpha;
    data[11] = m_plain->m_baseHeight;
    data[12] = m_x;
    data[13] = m_plain->m_baseHeight * m_x10;
    data[14] = m_y;
    data[15] = m_heightScale * m_x10;
    LpRenderTarget->AddToRenderQueue((_sceDmaTag *)&m_dmaTag, 1, "");
    LpRenderTarget->AddToRenderQueue((_sceDmaTag *)&m_plain->m_qwc, 1, "");
}


// ---- src/DrawableGridNode/UpdateTextureCache.cpp

void DrawableGridNode::UpdateTextureCache()
{
    UpdateTextureCacheQuadTreeByQuarters(0, 0, 16, 0);
}


// ---- src/DrawableGridNode/PlaceMipMapInTextureCache.cpp
struct _D3DVECTOR;

// The PS2 build stubs out DrawableGridNode's texture interface.

void DrawableGridNode::PlaceMipMapInTextureCache(int)
{
}


// ---- src/DrawableGridNode/UpdateTextureCacheQuadTreeByQuarters.cpp



int DrawableGridNode::UpdateTextureCacheQuadTreeByQuarters(int x, int z, int size, int quarter)
{
    if (m_x6C && size == 16 && m_textures->m_texture) {
        if ((!m_geometry && !m_level) || (m_geometry && m_geometry->m_numVertices && !m_geometry->m_bits.m_as4x4))
            PlaceMipMapInTextureCache(-1);
    }
    // Descend unless the children have no deltas.
    if (!(m_geometry && m_geometry->m_bits.m_hasMesh && !m_geometry->m_bits.m_as4x4)) {
        size >>= 1;
        if (size == 8) {
            if (m_geometry)
                m_geometry->m_childVisible = 0;
            if (m_x6C & 0x2)
                UpdateTextureCacheQuadTreeByQuarters(x, z, size, 0);
            if (m_x6C & 0x4)
                UpdateTextureCacheQuadTreeByQuarters(x + size, z, size, 4);
            if (m_x6C & 0x8)
                UpdateTextureCacheQuadTreeByQuarters(x, z + size, size, 8);
            if (m_x6C & 0x10)
                UpdateTextureCacheQuadTreeByQuarters(x + size, z + size, size, 12);
        } else if (size == 4) {
            if (m_x6C & (0x20 << quarter))
                UpdateTextureCacheQuadTreeQuarter(x, z, size, quarter);
            if (m_x6C & (0x40 << quarter))
                UpdateTextureCacheQuadTreeQuarter(x + size, z, size, quarter + 1);
            if (m_x6C & (0x80 << quarter))
                UpdateTextureCacheQuadTreeQuarter(x, z + size, size, quarter + 2);
            if (m_x6C & (0x100 << quarter))
                UpdateTextureCacheQuadTreeQuarter(x + size, z + size, size, quarter + 3);
        }
    }
    return 1;
}


// ---- src/DrawableGridNode/UpdateTextureCacheQuadTreeQuarter.cpp
extern int TranslateQuadQuadIndex[];
extern int QuadIndex[];





int DrawableGridNode::UpdateTextureCacheQuadTreeQuarter(int x, int z, int size, int quarter)
{
    if (m_textures->m_detail) {
        int child = TranslateQuadQuadIndex[quarter];
        if (m_geometry) {
            int count;
            if (child == 0)
                count = m_geometry->m_childHeights[child].m_numVertices;
            else
                count = m_geometry->m_childHeights[child].m_numVertices - m_geometry->m_childHeights[child - 1].m_numVertices;
            if (count && Calc4x4ChildVisible(child))
                PlaceMipMapInTextureCache(child);
        }
    }
    if (m_children && (!m_geometry || !m_geometry->m_bits.m_hasMesh)) {
        for (int row = 0; row < 64; row += 16) {
            for (int col = 0; col < 4; col++) {
                DrawableGridNode *node = m_children[col + (row + QuadIndex[quarter])];
                if (node && node->m_x6C) {
                    if (!m_geometry || (node->m_geometry->m_numVertices && !node->m_geometry->m_bits.m_as4x4))
                        node->PlaceMipMapInTextureCache(-1);
                    node->UpdateTextureCacheQuadTreeByQuarters(0, 0, 16, 0);
                }
            }
        }
    }
    return 1;
}


// ---- src/DrawableGridNode/CompareDuplicates.cpp


void DrawableGridNode::CompareDuplicates()
{
    if (m_geometry) {
        if (m_x6C && m_children && !(m_geometry->m_flags & 1)) {
            // Retail indexes by the row only, so this calls the first 15
            // children 15 times each.
            for (int y = 0; y < 15; y++) {
                for (int x = 0; x < 15; x++) {
                    if (m_children[y])
                        m_children[y]->CompareDuplicates();
                }
            }
        }
        if (m_children) {
            for (int i = 0; i < 256; i++) {
                if (m_children[i])
                    m_children[i]->CompareDuplicates();
            }
        }
    }
}


// ---- src/DrawableGridNode/CheckDeltaEvaluations.cpp


void DrawableGridNode::CheckDeltaEvaluations()
{
    if (m_geometry) {
        if (m_x6C && m_children && !(m_geometry->m_flags & 1)) {
            for (int i = 0; i < 256; i++) {
                if (m_children[i])
                    m_children[i]->CheckDeltaEvaluations();
            }
        } else if (m_x6C) {
            // 17x17 vertices; `<= 16` (not `< 17`) gives retail's slti $at.
            for (int y = 0; y <= 16; y++) {
                for (int x = 0; x <= 16; x++)
                    ProjectedDeltaValueAcceptable(x, y);
            }
        }
    }
}


// ---- src/DrawableGridNode/CalculateUncertaintyInterval.cpp
// CalculateUncertaintyInterval__16DrawableGridNodeFi  @ 0x0023C820
#pragma fast_fptosi on


static inline float Sqrt(float x)
{
    float r;
    asm("sqrt.s $f8, %1; swc1 $f8, 0(%0)" : : "r"(&r), "f"(x) : "$f8", "memory");
    return r;
}

// The fallback path's square roots: retail has a nop after each sqrt.s there (mwcc's hazard
// pass adds one only after the first), so this helper spells it out.
static inline float SqrtN(float x)
{
    float r;
    asm("sqrt.s $f8, %1; nop; swc1 $f8, 0(%0)" : : "r"(&r), "f"(x) : "$f8", "memory");
    return r;
}

static inline float Fmax2(float a, float b)
{
    return a > b ? a : b;
}




// Distance from the camera to the node's box, and (unless only the distance
// is wanted) the node's projected uncertainty interval in m_geometry.
float DrawableGridNode::CalculateUncertaintyInterval(int distanceOnly)
{
    float maxX;
    float nearZ;
    float nearX;
    float minX;
    float dHigh;
    float farX;
    float dLow;
    float minZ;
    float maxZ;
    float cz;
    // Retail reuses variables: nearZ holds the box centre x and maxX the half size until
    // they are needed (that keeps the centre live past block 0, which delays maxX's add into
    // the first branch's delay slot), and minX doubles as the far Z distance below.
    nearX = 0;
    nearZ = m_center.x;
    maxX = m_halfSize.x;
    minX = nearZ - maxX;
    minZ = m_center.z - m_halfSize.z;
    maxZ = m_center.z + m_halfSize.z;
    dLow = m_terrain->m_cameraPosition.x - minX;
    maxX = nearZ + maxX;
    if (dLow < 0.0f) {
        farX = m_terrain->m_cameraPosition.x - maxX;
        nearX = dLow * dLow;
        farX = farX * farX;
    } else {
        dHigh = m_terrain->m_cameraPosition.x - maxX;
        if (dHigh > 0.0f) {
            nearX = dHigh * dHigh;
            farX = dLow * dLow;
        } else {
            farX = m_terrain->m_cameraPosition.x + m_terrain->m_cameraPosition.x < minX + maxX ? maxX - m_terrain->m_cameraPosition.x : minX - m_terrain->m_cameraPosition.x;
            farX = farX * farX;
        }
    }
    nearZ = 0;
    cz = m_terrain->m_cameraPosition.z;
    dLow = cz - minZ;
    if (dLow < 0.0f) {
        minX = cz - maxZ;
        nearZ = dLow * dLow;
        minX = minX * minX;
    } else {
        dHigh = cz - maxZ;
        if (dHigh > 0.0f) {
            nearZ = dHigh * dHigh;
            minX = dLow * dLow;
        } else {
            if (cz + cz < minZ + maxZ)
                minX = maxZ - cz;
            else
                minX = minZ - cz;
            minX = minX * minX;
        }
    }
    float far2 = farX + minX;
    float near2 = nearX + nearZ;
    float below = m_terrain->m_cameraPosition.y - m_minHeight * m_terrain->m_gridScale;
    float above = m_terrain->m_cameraPosition.y - m_maxHeight * m_terrain->m_gridScale;
    float nearY = 0;
    float dy;
    if (below < 0.0f) {
        nearY = below * below;
        dy = below;
    } else if (above > 0.0f) {
        nearY = above * above;
        dy = above;
    } else {
        dy = nearY;
    }
    float distance = Sqrt(near2 + nearY);
    if (distanceOnly)
        return distance;
    m_terrain->m_xF4++;
    m_geometry->m_x126 = m_geometry->m_x122;
    m_geometry->m_x128 = m_geometry->m_x124;
    if (!m_terrain->m_xC8) {
        m_geometry->m_x124 = 0;
    } else if (near2 == 0.0f) {
        m_geometry->m_x124 = 0xFFFF;
    } else if (far2 == 0.0f) {
        m_geometry->m_x124 = 0xFFFF;
    } else {
        float nearDist = Sqrt(near2);
        float farDist = Sqrt(far2);
        if (nearY < near2)
            dy = nearDist;
        else if (nearY > far2)
            dy = farDist;
        float u = m_geometry->m_x158 * (m_terrain->m_xD0 * (dy + nearY / dy)) / m_terrain->m_gridScale;
        m_geometry->m_x122 = u >= 65535.0f ? 0xFFFF : (unsigned short)u;
        if (below < 0.0f)
            below = -below;
        if (above < 0.0f)
            above = -above;
        below = Fmax2(below, above);
        nearY = below * below;
        dy = Fmax2((near2 + nearY) / nearDist, (far2 + nearY) / farDist);
        u = m_geometry->m_x158 * (m_terrain->m_xD0 * dy) / m_terrain->m_gridScale;
        m_geometry->m_x124 = u >= 65535.0f ? 0xFFFF : (unsigned short)u;
        goto done;
    }
    if (nearY < near2)
        dy = SqrtN(near2);
    else if (nearY > far2)
        dy = SqrtN(far2);
    m_geometry->m_x122 = (int)(m_geometry->m_x158 * (m_terrain->m_xD0 * (dy + nearY / dy)) / m_terrain->m_gridScale);
    if (m_geometry->m_x122 > 0xFFFF)
        m_geometry->m_x122 = 0xFFFF;
done:
    return distance;
}
#pragma fast_fptosi reset


// ---- src/DrawableGridNode/CalculateDistanceFromCamera.cpp

// Retail's sqrt: sqrt.s into $f8, stored through a stack temporary.
static inline float Fsqrt(float x)
{
    float r;
    asm("sqrt.s $f8, %1; swc1 $f8, 0(%0)" : : "r"(&r), "f"(x) : "$f8", "memory");
    return r;
}



// Distance from the camera to the node's bounding box.
float DrawableGridNode::CalculateDistanceFromCamera()
{
    float dx, dz, dy;
    float minX = m_center.x - m_halfSize.x;
    float maxX = m_center.x + m_halfSize.x;
    float minZ = m_center.z - m_halfSize.z;
    float maxZ = m_center.z + m_halfSize.z;
    dx = m_terrain->m_cameraPosition.x - minX;
    if (!(dx < 0.0f)) {
        dx = m_terrain->m_cameraPosition.x - maxX;
        if (dx <= 0.0f)
            dx = 0.0f;
    }
    dz = m_terrain->m_cameraPosition.z - minZ;
    if (!(dz < 0.0f)) {
        dz = m_terrain->m_cameraPosition.z - maxZ;
        if (dz <= 0.0f)
            dz = 0.0f;
    }
    dy = m_terrain->m_cameraPosition.y - m_minHeight * m_terrain->m_gridScale;
    float distance2;
    if (dy < 0.0f) {
        distance2 = dx * dx + dz * dz + dy * dy;
    } else {
        dy = m_terrain->m_cameraPosition.y - m_maxHeight * m_terrain->m_gridScale;
        if (!(dy <= 0.0f))
            distance2 = dx * dx + dz * dz + dy * dy;
        else
            distance2 = dx * dx + dz * dz;
    }
    return Fsqrt(distance2);
}


// ---- src/DrawableGridNode/Calc4x4ChildVisible.cpp



extern VisibleTest *VisTest;






// Test one of the 4x4 sub-blocks against the view and record the result.
int DrawableGridNode::Calc4x4ChildVisible(int child)
{
    _D3DVECTOR center;
    _D3DVECTOR half;
    float size = m_halfSize.x / 2.0f;
    float quarter = size / 2.0f;
    half.x = quarter;
    half.z = quarter;
    half.y = m_geometry->m_childHeights[child].m_halfHeight;
    center.x = (m_center.x - m_halfSize.x) + (float)(child % 4) * size + quarter;
    center.z = (m_center.z - m_halfSize.z) + (float)(child / 4) * size + quarter;
    center.y = m_geometry->m_childHeights[child].m_center;
    if (VisTest->BoundingBoxVisible(&m_terrain->m_camera->m_viewProjection, &center, &half, 0))
        m_geometry->m_childVisible |= 1 << child;
    else
        m_geometry->m_childVisible &= ~(1 << child);
}


// ---- src/DrawableGridNode/CalculateDistanceTo4x4Child.cpp

// Retail's sqrt: sqrt.s into $f8, stored through a stack temporary.



// Distance from the camera to one cell of the node's 4x4 subdivision.
float DrawableGridNode::CalculateDistanceTo4x4Child(int index)
{
    float camX = m_terrain->m_cameraPosition.x;
    float camY = m_terrain->m_cameraPosition.y;
    float camZ = m_terrain->m_cameraPosition.z;
    float dz;
    float minX;
    float maxX;
    float minZ;
    float dx;
    float maxZ;
    float dy;
    int col = index % 4;
    int row = index / 4;
    float step = m_halfSize.x / 2.0f;
    minX = (m_center.x - m_halfSize.x) + col * step;
    maxX = minX + step;
    minZ = (m_center.z - m_halfSize.z) + row * step;
    maxZ = minZ + step;
    dx = camX - minX;
    if (dx < 0.0f) {
        dx = dx * dx;
    } else {
        dx = camX - maxX;
        if (!(dx <= 0.0f))
            dx = dx * dx;
        else
            dx = 0.0f;
    }
    dz = camZ - minZ;
    if (dz < 0.0f) {
        dz = dz * dz;
    } else {
        dz = camZ - maxZ;
        if (!(dz <= 0.0f))
            dz = dz * dz;
        else
            dz = 0.0f;
    }
    float horizontal = dx + dz;
    dy = camY - m_minHeight * m_terrain->m_gridScale;
    float dyMax = camY - m_maxHeight * m_terrain->m_gridScale;
    if (dy < 0.0f) {
        dy = dy * dy;
    } else if (!(dyMax <= 0.0f)) {
        dy = dyMax * dyMax;
    } else {
        dy = 0.0f;
    }
    return Fsqrt(horizontal + dy);
}


// ---- src/DrawableGridNode/ProjectedDeltaValueAcceptable.cpp

extern unsigned short Decompress[256];





int DrawableGridNode::ProjectedDeltaValueAcceptable(int x, int y)
{
    GridVertex *v = &m_lodData->verts[x + SizeVerticesBy[y]];
    float projected = m_heightScale * (m_x10 * Decompress[v->m_delta]);
    Terrain *e;
    int dh = v->height - m_lod->m_baseHeight;
    float dx;
    float dz;
    float dy;
    float horizontal;
    float distance;
    float lhs;
    float rhs;
    // Reading m_terrain here, between m_shift and m_y, delays the
    // m_y load so the x sum is shifted before the z sum is formed
    // (retail gives both sums $v0).
    dx = m_terrain->m_cameraGrid.x - (float)((m_x + x) << m_shift);
    e = m_terrain;
    dz = e->m_cameraGrid.z - (float)((m_y + y) << m_shift);
    horizontal = dx * dx + dz * dz;
    dy = e->m_cameraGrid.y - m_x10 * ((float)m_lod->m_baseHeight + m_heightScale * (float)dh);
    distance = horizontal + dy * dy;
    lhs = horizontal * (projected * projected);
    rhs = e->m_threshold * (distance * distance);
    e->m_evaluations++;
    return lhs <= rhs ? 1 : 0;
}


// ---- src/DrawableGridNode/SelectVisibleBlocks.cpp
struct _D3DMATRIX;



void DrawableGridNode::SelectVisibleBlocks(long frame, _D3DMATRIX *view, int evaluateAll)
{
    SelectQuadBlocksVisible(frame, view, m_center, m_halfSize, 0, 0, 0x10, 0, 0, 0);
    if (!m_terrain->m_isPlain) {
        m_terrain->m_frameCount++;
        if (evaluateAll)
            Evaluate();
        else
            EvaluateByBins();
        if (m_terrain->m_checkDeltas) {
            CheckDeltaEvaluations();
            CompareDuplicates();
        }
        m_terrain->m_meshesBuilt = 0;
        MakeMesh();
        UpdateTextureCache();
    }
}


// ---- src/DrawableGridNode/SelectQuadBlocksVisible.cpp
struct _D3DMATRIX;

extern VisibleTest *VisTest;

// Start index of each row of the 16x16 child grid.
extern int SizeBy[];


int DrawableGridNode::SelectQuadBlocksVisible(long frame, _D3DMATRIX *view, _D3DVECTOR &center, _D3DVECTOR &half,
                                              int x, int y, int size, int level, int quad, int parentMeshed)
{
    int visible;
    unsigned int clip;
    if (size > 1) {
        visible = VisTest->BoundingBoxVisible(view, &center, &half, &clip);
        if (size == 16 && visible) {
            m_clipCodes = clip;
            SetBlockLOD(frame, view, 0, parentMeshed);
        }
    } else {
        visible = 1;
        clip = 0x3F;
    }
    if (visible) {
        if (clip == 0) {
            for (int i = 0; i < size; i++) {
                for (int j = 0; j < size; j++) {
                    DrawableGridNode *node = m_children[SizeBy[i + y] + (j + x)];
                    if (node) {
                        node->m_clipCodes = 0;
                        node->SetBlockLOD(frame, view, 1, parentMeshed);
                    }
                }
            }
        } else {
            size >>= 1;
            if (size) {
                _D3DVECTOR childHalf;
                childHalf.x = half.x / 2.0f;
                childHalf.y = half.y;
                childHalf.z = half.z / 2.0f;
                _D3DVECTOR childCenter;
                childCenter.x = center.x - childHalf.x;
                childCenter.y = center.y;
                childCenter.z = center.z - childHalf.z;
                SelectQuadBlocksVisible(frame, view, childCenter, childHalf, x, y, size, level + 1, quad * 4, parentMeshed);
                childCenter.x += half.x;
                SelectQuadBlocksVisible(frame, view, childCenter, childHalf, x + size, y, size, level + 1, quad * 4 + 1, parentMeshed);
                childCenter.z += half.z;
                SelectQuadBlocksVisible(frame, view, childCenter, childHalf, x + size, y + size, size, level + 1, quad * 4 + 3, parentMeshed);
                childCenter.x -= half.x;
                SelectQuadBlocksVisible(frame, view, childCenter, childHalf, x, y + size, size, level + 1, quad * 4 + 2, parentMeshed);
            } else {
                DrawableGridNode *node = m_children[SizeBy[y] + x];
                if (node) {
                    if (node->m_children) {
                        node->SelectQuadBlocksVisible(frame, view, node->m_center, node->m_halfSize, 0, 0, 16, 0, 0, parentMeshed);
                    } else {
                        unsigned int nodeClip;
                        if (VisTest->BoundingBoxVisible(view, &node->m_center, &node->m_halfSize, &nodeClip)) {
                            node->m_clipCodes = nodeClip;
                            node->SetBlockLOD(frame, view, 0, parentMeshed);
                        }
                    }
                }
            }
        }
    }
    return 0;
}


// ---- src/DrawableGridNode/SetBlockLOD.cpp
struct _D3DMATRIX;
struct _D3DVECTOR;

extern DataAccess *LpDataAccess;


class GridNode;
class DrawableGridNode;





void DrawableGridNode::SetBlockLOD(long file, _D3DMATRIX *matrix, int recurse, int parentMeshed)
{
    static int CalculateUncertaintyIntervalID = -1;     // profiler IDs, unused
    static int SelectMipMapLevelID = -1;
    if (m_terrain->m_isPlain == 1) {
        if (!m_level) {
            m_uncertainty = CalculateDistanceFromCamera();
            if (m_uncertainty < LpRenderTarget->m_fogEnd) {
                SelectMipMapLevel(m_uncertainty);
                PlaceMipMapInTextureCache(-1);
                if (m_terrain->m_numVisibleNodes < 4000) {
                    m_terrain->m_visible[m_terrain->m_numVisibleNodes++] = this;
                    m_x84 = 0;
                    m_x6C = m_terrain->m_xDFC;
                }
            }
        }
    } else {
        m_uncertainty = CalculateUncertaintyInterval(parentMeshed);
        SelectMipMapLevel(m_uncertainty);
        if (!m_geometry || m_x1C > m_geometry->m_x122 || !m_textures->m_texture) {
            if (m_geometry && m_geometry->m_bits.m_hasMesh) {
                m_geometry->m_bits.m_hasMesh = 0;
                m_geometry->m_bits.m_changed = 1;
            }
            if (m_childTable) {
                if (!m_children) {
                    m_children = new DrawableGridNode *[256];
                    m_childOffsets = new int[256];
                    LpDataAccess->FSeek(file, m_childTable, 0);
                    LpDataAccess->FRead(file, m_childOffsets, 0x400, 1);
                    for (int i = 0; i < 256; i++) {
                        if (m_childOffsets[i]) {
                            LpDataAccess->FSeek(file, m_childOffsets[i], 0);
                            m_children[i] = New(file, 0, this, m_level - 1, m_shift - 4, (m_x + i % 16) << 4,
                                                (m_y + i / 16) << 4, i, 0, 0);
                        } else {
                            m_children[i] = 0;
                        }
                    }
                }
                if (recurse) {
                    for (int i = 0; i < 256; i++) {
                        if (m_children[i])
                            m_children[i]->SetBlockLOD(file, matrix, 1,
                                                       parentMeshed | (m_geometry ? m_geometry->m_bits.m_hasMesh : 0));
                    }
                }
            }
        } else {
            if (!m_geometry->m_bits.m_hasMesh) {
                m_geometry->m_bits.m_hasMesh = 1;
                m_geometry->m_bits.m_changed = 1;
                if (m_children && recurse) {
                    for (int i = 0; i < 256; i++) {
                        if (m_children[i])
                            m_children[i]->DestroyMesh(1);
                    }
                }
            }
        }
    }
}


// ---- src/DrawableGridNode/SelectMipMapLevel.cpp
struct _D3DVECTOR;

// The PS2 build stubs out DrawableGridNode's texture interface.

void DrawableGridNode::SelectMipMapLevel(float)
{
}


// ---- src/DrawableGridNode/Evaluate.cpp



void DrawableGridNode::Evaluate()
{
    if (m_children) {
        for (int i = 0; i < 256; i++) {
            if (m_children[i]) {
                if (m_x6C & 0x200000)
                    m_children[i]->m_x6C = 0x3FFFFF;
                m_children[i]->Evaluate();
            }
        }
    }
    EvaluateDeltas();
    m_geometry->m_evaluatedFrame = m_terrain->m_frameCount;
    m_geometry->m_x122 = m_geometry->m_x126 = 0;
    m_geometry->m_x124 = m_geometry->m_x128 = 0xFFFF;
}


// ---- src/DrawableGridNode/EvaluateByBins.cpp
extern unsigned char Compress[];


extern ParentFinder ParentFinderTable[];


// The node's vertices, plus their indices sorted by delta: the vertices with
// delta d are m_byDelta[m_deltaStart[d]] up to m_byDelta[m_deltaStart[d + 1]].




// Re-evaluates the vertices whose deltas fall in the bins between the old and
// new thresholds, children first.
void DrawableGridNode::EvaluateByBins()
{
    if (!m_x6C)
        return;
    if (m_children && !m_geometry->m_bits.m_hasMesh) {
        for (int i = 0; i < 256; i++) {
            if (m_children[i]) {
                if (m_x6C == 0x200000)
                    m_children[i]->m_x6C = 0x3FFFFF;
                m_children[i]->EvaluateByBins();
            }
        }
    }
    int deferEnd;
    int offStart;
    int i;
    int on;
    int onEnd;
    m_terrain->m_x598++;
    deferEnd = Compress[m_geometry->m_x122];
    offStart = Compress[m_geometry->m_x126];
    on = Compress[m_geometry->m_x124];
    if (on != 0xFF)
        on++;
    onEnd = Compress[m_geometry->m_x128];
    if (onEnd != 0xFF)
        onEnd++;
    for (i = m_lodData->m_x910[offStart]; i < 17 * 17 && i < m_lodData->m_x910[deferEnd]; i++) {
        int index = m_lodData->m_xB10[i];
        m_geometry->m_vertexFlags[index] &= 0xBF;
        if (!(m_geometry->m_vertexFlags[index] & 0xF)) {
            m_terrain->m_x108++;
            unsigned char flags = m_geometry->m_vertexFlags[index];
            if (!(flags & 0x10) && (flags & 0x80)) {
                m_geometry->m_vertexFlags[index] = flags & 0x7F;
                m_geometry->m_bits.m_changed = 1;
                NotifyParents(ParentFinderTable[index].x, ParentFinderTable[index].y, 0);
            }
        }
    }
    if (on < onEnd) {
        for (i = m_lodData->m_x910[on]; i < 17 * 17 && i <= m_lodData->m_x910[onEnd]; i++) {
            int index = m_lodData->m_xB10[i];
            m_geometry->m_vertexFlags[index] |= 0x40;
            if (!(m_geometry->m_vertexFlags[index] & 0xF)) {
                m_terrain->m_x104++;
                unsigned char flags = m_geometry->m_vertexFlags[index];
                if (!(flags & 0x80)) {
                    m_geometry->m_vertexFlags[index] = flags | 0x80;
                    m_geometry->m_bits.m_changed = 1;
                    NotifyParents(ParentFinderTable[index].x, ParentFinderTable[index].y, 0x80);
                }
            }
        }
    }
    for (i = m_lodData->m_x910[deferEnd]; i < 17 * 17 && i < m_lodData->m_x910[on]; i++) {
        int index = m_lodData->m_xB10[i];
        if (m_geometry->m_vertexFlags[index] & 0xF)
            m_terrain->m_x114[index] = m_terrain->m_x598;
        else
            EvaluateVertex(ParentFinderTable[index].x, ParentFinderTable[index].y);
    }
    m_geometry->m_evaluatedFrame = m_terrain->m_frameCount;
}


// ---- src/DrawableGridNode/EvaluateVertex.cpp
// Start index of each row of the 17x17 vertex grid.




void DrawableGridNode::EvaluateVertex(int x, int y)
{
    m_terrain->m_vertexEvaluations++;
    int index = x + SizeVerticesBy[y];
    if (!ProjectedDeltaValueAcceptable(x, y)) {
        m_geometry->m_vertexFlags[index] |= 0x40;
        if (m_geometry->m_vertexFlags[index] & 0x80)
            return;
        m_geometry->m_vertexFlags[index] |= 0x80;
    } else {
        m_geometry->m_vertexFlags[index] &= ~0x40;
        unsigned char flags = m_geometry->m_vertexFlags[index];
        if (flags & 0x10)
            return;
        if (!(flags & 0x80))
            return;
        m_geometry->m_vertexFlags[index] = flags & 0x7F;
    }
    m_geometry->m_bits.m_changed = 1;
    NotifyParents(x, y, m_geometry->m_vertexFlags[index] & 0x80);
}


// ---- src/DrawableGridNode/EvaluateDeltas.cpp

void DrawableGridNode::EvaluateDeltas()
{
    int step2 = 2;
    int step = 1;
    int c = 1;
    int offset = 0;
    // offset is reset in the loop step: retail keeps it in $t2 and reuses it for the last call
    for (; step < 16; step *= 2, step2 *= 2, offset = 0) {
        EvaluateDeltasInDecimatedGrid(0, 0, 16, 16, step, offset, step2, c);
        c *= 2;
        offset = step;
        EvaluateDeltasInDecimatedGrid(0, 0, 16, 16, step, offset, step2, c);
    }
    EvaluateDeltasInDecimatedGrid(0, 0, 16, 16, 0, offset, 16, 16);
}


// ---- src/DrawableGridNode/EvaluateDeltasInDecimatedGrid.cpp
// Start index of each row of the 17x17 vertex grid.



void DrawableGridNode::EvaluateDeltasInDecimatedGrid(int x, int z, int width, int height, int step, int offset, int colStep, int rowStep)
{
    int col;
    int row = 0;
    for (int y = z + offset; y <= z + height; y += rowStep, row++) {
        int start;
        if (offset == 0) {
            if (row & 1)
                start = 0;
            else
                start = step;
        } else {
            start = step;
        }
        for (col = x + start; col <= x + width; col += colStep) {
            int index = col + SizeVerticesBy[y];
            if ((m_geometry->m_vertexFlags[index] & 0xF) == 0) {
                int needed = ProjectedDeltaValueAcceptable(col, y) ? 0 : 1;
                if (needed) {
                    m_geometry->m_vertexFlags[index] |= 0x40;
                    if (m_geometry->m_vertexFlags[index] & 0x80)
                        continue;
                    m_geometry->m_vertexFlags[index] |= 0x80;
                } else {
                    m_geometry->m_vertexFlags[index] &= ~0x40;
                    unsigned char flags = m_geometry->m_vertexFlags[index];
                    if (!(flags & 0x80))
                        continue;
                    if (flags & 0x10)
                        continue;
                    m_geometry->m_vertexFlags[index] = flags & 0x7F;
                }
                m_geometry->m_bits.m_changed = 1;
                NotifyParents(col, y, m_geometry->m_vertexFlags[index] & 0x80);
            }
        }
    }
}


// ---- src/DrawableGridNode/NotifyParents.cpp


extern ParentFinder ParentFinderTable[];

class HeightVertex;



void DrawableGridNode::NotifyParents(int x, int y, int needed)
{
    m_terrain->m_parentNotifications++;
    int index = x + SizeVerticesBy[y];
    if (x == 8 && y == 8) {
        // The centre vertex lies on the parent's edge or diagonal.
        if (m_parent) {
            if ((m_position & 0x11) == 0x11 || (m_position & 0x11) == 0) {
                m_parent->NotifyVertex(m_position & 0xF, (m_position >> 4) + 1, needed, 2, 0);
                m_parent->NotifyVertex((m_position & 0xF) + 1, m_position >> 4, needed, 4, 0);
            } else {
                m_parent->NotifyVertex(m_position & 0xF, m_position >> 4, needed, 8, 0);
                m_parent->NotifyVertex((m_position & 0xF) + 1, (m_position >> 4) + 1, needed, 1, 0);
            }
        }
    } else {
        if (!(ParentFinderTable[index].parent0X == ParentFinderTable[index].parent1X
              && ParentFinderTable[index].parent0Y == ParentFinderTable[index].parent1Y)) {
            NotifyVertex(ParentFinderTable[index].parent0X, ParentFinderTable[index].parent0Y, needed, ParentFinderTable[index].edge, 0);
            NotifyVertex(ParentFinderTable[index].parent1X, ParentFinderTable[index].parent1Y, needed, ParentFinderTable[index].diagonal, 0);
        }
    }
}


// ---- src/DrawableGridNode/NotifyVertex.cpp
// Start index of each row of the 17x17 vertex grid.






void DrawableGridNode::NotifyVertex(int x, int y, int needed, int mask, HeightVertex *vertex)
{
    if (x < 0 || y < 0 || x > 16 || y > 16)
        return;
    m_terrain->m_vertexNotifications++;
    int changed = 0;
    int notify = 0;
    int index = x + SizeVerticesBy[y];
    GridVertex *v = &m_lodData->verts[index];
    if (needed) {
        if (!(mask & m_geometry->m_vertexFlags[index])) {
            m_geometry->m_vertexFlags[index] |= mask;
            notify = 1;
        }
        if (!(m_geometry->m_vertexFlags[index] & 0x80)) {
            m_geometry->m_vertexFlags[index] |= 0x80;
            changed = 1;
            notify = 1;
        }
    } else {
        m_geometry->m_vertexFlags[index] &= ~mask;
        notify = 1;
        if (!(m_geometry->m_vertexFlags[index] & 0xF))
            changed = HandleDeferredEvaluation(x, y);
    }
    if (changed) {
        m_geometry->m_bits.m_changed = 1;
        NotifyParents(x, y, m_geometry->m_vertexFlags[index] & 0x80);
    }
    if (notify && m_parent && !vertex) {
        if (x == 0 || x == 16 || y == 0 || y == 16)
            m_parent->NotifyDuplicates(this, x, y, needed, mask, (HeightVertex *)v);
    }
}


// ---- src/DrawableGridNode/HandleDeferredEvaluation.cpp
// Start index of each row of the 17x17 vertex grid.

extern unsigned char Compress[];






int DrawableGridNode::HandleDeferredEvaluation(int x, int y)
{
    int index = x + SizeVerticesBy[y];
    int changed = 0;
    GridVertex *v = &m_lodData->verts[index];
    if (m_x6C && (m_geometry->m_evaluatedFrame == m_terrain->m_frameCount || m_terrain->m_x114[index] == m_terrain->m_x598)) {
        if (ProjectedDeltaValueAcceptable(x, y)) {
            m_geometry->m_vertexFlags[index] &= ~0x40;
            unsigned char flags = m_geometry->m_vertexFlags[index];
            if ((flags & 0x80) && !(flags & 0x10)) {
                m_geometry->m_vertexFlags[index] = flags & 0x7F;
                changed = 1;
            }
        } else {
            m_geometry->m_vertexFlags[index] |= 0xC0;
        }
    } else {
        unsigned char flags = m_geometry->m_vertexFlags[index];
        if (!(flags & 0x40)) {
            if (v->m_delta > Compress[m_geometry->m_x124]) {
                m_geometry->m_vertexFlags[index] = flags | 0xC0;
                changed = 1;
            } else if (!(flags & 0x10)) {
                m_geometry->m_vertexFlags[index] = flags & 0x7F;
                changed = 1;
            }
        } else if (v->m_delta < Compress[m_geometry->m_x122]) {
            m_geometry->m_vertexFlags[index] = flags & 0xBF;
            flags = m_geometry->m_vertexFlags[index];
            if (!(flags & 0x10)) {
                m_geometry->m_vertexFlags[index] = flags & 0x7F;
                changed = 1;
            }
        }
    }
    return changed;
}


// ---- src/DrawableGridNode/NotifyDuplicateVertices.cpp
// Start index of each row of the 17x17 vertex grid.

extern int SizeBy[];    // child-table row offsets (16 per row)




// Passes a changed vertex (in root grid units) down the tree to every node
// that holds a copy of it, from the root to the given level.
void DrawableGridNode::NotifyDuplicateVertices(int x, int y, int needed, int mask, HeightVertex *vertex, int level)
{
    int nodeLevel = m_level;
    if (nodeLevel == 0) {
        if (&m_lodData->verts[x + SizeVerticesBy[y]] != (GridVertex *)vertex)
            NotifyVertex(x, y, needed, mask, vertex);
        return;
    }
    int shift = 0;
    for (int i = 0; i < m_level; i++)
        shift += 4;
    int size = 1 << shift;
    int sizeMask = size - 1;
    int localX = x & sizeMask;
    if (localX == 0 && (y & sizeMask) == 0) {
        if (&m_lodData->verts[(x >> shift) + SizeVerticesBy[y >> shift]] != (GridVertex *)vertex)
            NotifyVertex(x >> shift, y >> shift, needed, mask, vertex);
    }
    if (m_children && level < m_level) {
        int x0 = x;
        int y0 = y;
        int x1 = x - 1;
        int y1 = y - 1;
        int n = m_level;
        while (n--) {
            x0 /= 16;
            y0 /= 16;
            x1 /= 16;
            y1 /= 16;
        }
        DrawableGridNode *child;
        if (x1 == x0) {
            if (x0 < 16) {
                if (y1 == y0) {
                    if (y0 < 16 && (child = m_children[x0 + SizeBy[y0]]))
                        child->NotifyDuplicateVertices(localX, y & sizeMask, needed, mask, vertex, level);
                } else {
                    if (y0 < 16 && (child = m_children[x0 + SizeBy[y0]]))
                        child->NotifyDuplicateVertices(localX, y & sizeMask, needed, mask, vertex, level);
                    if (y1 < 16 && (child = m_children[x0 + SizeBy[y1]]))
                        child->NotifyDuplicateVertices(localX, size + (y & sizeMask), needed, mask, vertex, level);
                }
            }
        } else if (y1 == y0) {
            if (x0 < 16 && y0 < 16 && (child = m_children[x0 + SizeBy[y0]]))
                child->NotifyDuplicateVertices(localX, y & sizeMask, needed, mask, vertex, level);
            if (x1 < 16 && y0 < 16 && (child = m_children[x1 + SizeBy[y0]]))
                child->NotifyDuplicateVertices(size + localX, y & sizeMask, needed, mask, vertex, level);
        } else {
            if (x0 < 16 && y0 < 16 && (child = m_children[x0 + SizeBy[y0]]))
                child->NotifyDuplicateVertices(localX, y & sizeMask, needed, mask, vertex, level);
            // (sic) retail: y & sizeMask + size
            if (x0 < 16 && y1 < 16 && (child = m_children[x0 + SizeBy[y1]]))
                child->NotifyDuplicateVertices(localX, y & sizeMask + size, needed, mask, vertex, level);
            if (x1 < 16 && y0 < 16 && (child = m_children[x1 + SizeBy[y0]]))
                child->NotifyDuplicateVertices(size + localX, y & sizeMask, needed, mask, vertex, level);
            if (x1 < 16 && y1 < 16 && (child = m_children[x1 + SizeBy[y1]]))
                child->NotifyDuplicateVertices(size + localX, size + (y & sizeMask), needed, mask, vertex, level);
        }
    }
}


// ---- src/DrawableGridNode/NotifyDuplicates.cpp
class HeightVertex;


// A child's edge vertex changed: pass it on to the neighbouring children that
// share it, or, for children on this node's border, to the root.
void DrawableGridNode::NotifyDuplicates(DrawableGridNode *child, int x, int y, int needed, int mask, HeightVertex *vertex)
{
    if ((child->m_position & 0xF) == 0 || (child->m_position & 0xF) == 0xF ||
        (child->m_position & 0xF0) == 0 || (child->m_position & 0xF0) == 0xF0) {
        DrawableGridNode *root = this;
        if (root) {     // (sic) retail null-checks this
            while (root->m_parent)
                root = root->m_parent;
            int shift = 0;
            for (int i = 0; i < m_level - 1; i++)
                shift += 4;
            root->NotifyDuplicateVertices((child->m_x + x) << shift, (child->m_y + y) << shift, needed, mask, vertex,
                                          child->m_level);
        }
        return;
    }
    if (x == 0) {
        if (m_children[child->m_position - 1])
            m_children[child->m_position - 1]->NotifyVertex(16, y, needed, mask, vertex);
        if (y == 0) {
            if (m_children[child->m_position - 17])
                m_children[child->m_position - 17]->NotifyVertex(16, 16, needed, mask, vertex);
            if (m_children[child->m_position - 16])
                m_children[child->m_position - 16]->NotifyVertex(0, 16, needed, mask, vertex);
        } else if (y == 16) {
            if (m_children[child->m_position + 15])
                m_children[child->m_position + 15]->NotifyVertex(16, 0, needed, mask, vertex);
            if (m_children[child->m_position + 16])
                m_children[child->m_position + 16]->NotifyVertex(0, 0, needed, mask, vertex);
        }
    } else if (x == 16) {
        if (m_children[child->m_position + 1])
            m_children[child->m_position + 1]->NotifyVertex(0, y, needed, mask, vertex);
        if (y == 0) {
            if (m_children[child->m_position - 15])
                m_children[child->m_position - 15]->NotifyVertex(0, 16, needed, mask, vertex);
            if (m_children[child->m_position - 16])
                m_children[child->m_position - 16]->NotifyVertex(16, 16, needed, mask, vertex);
        } else if (y == 16) {
            if (m_children[child->m_position + 17])
                m_children[child->m_position + 17]->NotifyVertex(0, 0, needed, mask, vertex);
            if (m_children[child->m_position + 16])
                m_children[child->m_position + 16]->NotifyVertex(16, 0, needed, mask, vertex);
        }
    } else if (y == 0) {
        if (m_children[child->m_position - 16])
            m_children[child->m_position - 16]->NotifyVertex(x, 16, needed, mask, vertex);
    } else if (y == 16) {
        if (m_children[child->m_position + 16])
            m_children[child->m_position + 16]->NotifyVertex(x, 0, needed, mask, vertex);
    }
}


// ---- src/VertexFinderTool/dtor.cpp

VertexFinderTool::~VertexFinderTool()
{
}

