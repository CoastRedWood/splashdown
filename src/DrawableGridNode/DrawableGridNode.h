// DrawableGridNode: one node of Terrain's grid tree (17x17 vertices), with
// its GridNode base. GridNode holds the tree links and the node's grid
// position, with its vptr at +0x30 (0x34 bytes); DrawableGridNode adds the
// bounds, the Terrain it belongs to and the render state from +0x34.
// DrawableGridNodeSharedTextures (0xF0 bytes from its New) derives from it.
// Names follow DrawableGridNode's own methods (src/DrawableGridNode/), else
// the most common name among the local copies. GridNode is declared here
// because it has no header of its own (src/GridNode has only two methods).
#ifndef DRAWABLEGRIDNODE_DRAWABLEGRIDNODE_H
#define DRAWABLEGRIDNODE_DRAWABLEGRIDNODE_H

#include "ps2_port.h"

#include "d3dtypes.h"

class DrawableGridNode;
struct GridNodeGeometry;
class GridNodeGeometryBase;
class GridNodeGeometryPlain;
class Terrain;
class TextureMap;
class VifDataItem;
struct HeightVertex;

#include "AgeItem/AgeItem.h"
#include "QWORD/QWORD.h"
#include "ByteVector/ByteVector.h"

// DMA REF tag for a node's VIF packet (DrawableGridNode +0x90).
struct NodeDmaTag {
    unsigned short qwc;                 // +0x0
    unsigned char mark;                 // +0x2
    unsigned char id;                   // +0x3
    PS2_PTR(QWORD) next;                        // +0x4
    unsigned int p[2];                  // +0x8
};

// One authored detail texture slot of a node (GridNodeTextures::m_detail,
// one per 4x4 child; 0x2C bytes).
struct DetailTextureEntry {
    short m_ownTexture : 1;             // +0x00 bit 0: m_texture is this entry's own
    short m_index : 15;                 //        index into Terrain::m_textures, -1 if none
    short m_x02;                        // +0x02
    TextureMap *m_texture;              // +0x04
    AgeItem m_ageItem;                  // +0x08, in the Terrain's texture cache (m_size != 0: cached)
    float m_x1C;                        // +0x1C
    int m_x20;                          // +0x20
    int m_x24;                          // +0x24
    float m_projectedSize;              // +0x28
};

// A node's texture data (DrawableGridNode::m_textures).
struct GridNodeTextures {
    int m_dataSize;                     // +0x00
    void *m_data;                       // +0x04
    TextureMap *m_texture;              // +0x08
    int m_xC;                           // +0x0C
    int m_x10;                          // +0x10
    AgeItem m_ageItem;                  // +0x14, in the Terrain's texture cache (m_size != 0: cached)
    float m_x28;                        // +0x28
    int m_x2C;                          // +0x2C
    int m_x30;                          // +0x30
    float m_mipLevel;                   // +0x34
    DetailTextureEntry *m_detail;       // +0x38, 16 (one per 4x4 child), or 0

    void *Data() { return m_data; }     // LoadNode's FRead: a call in an argument puts this load first
};

// GridNodeGeometryLOD's data as Griddraw.cpp reads it (GridNode::m_lodData):
// its vertices as a POD (HeightVertex has a ctor) and the delta tables as
// shorts.
struct GridVertex {
    unsigned char userFlags;            // +0
    unsigned char m_delta;              // +1, index into Decompress
    unsigned short height;              // +2
    ByteVector normal;                  // +4
    unsigned char tessFlip : 1;         // +7 bit 0
};

struct GridNodeLODData {
#ifdef SPLASHDOWN_PC
    void *m_vtable;                     // clang puts the vptr first
    int m_baseHeight;
#else
    int m_baseHeight;                   // +0x000
    void *m_vtable;                     // +0x004
#endif
    GridVertex verts[17 * 17];          // +0x008
    short m_x910[256];                  // +0x910, start of each delta's run in m_xB10
    short m_xB10[0x122];                // +0xB10, vertex indices sorted by delta
};

class GridNode {
public:
    GridNode() {}
    void SubstituteGeometry(GridNodeGeometryBase *old, GridNodeGeometryBase *replacement);
    int GetPatch(int x, int y, float *heights, _D3DVECTOR *normals, unsigned char *flags, int step);
    float VertexHeight(int i);          // inline in retail (src/tu/GridNode.cpp)

    DrawableGridNode **m_children;      // +0x00
    DrawableGridNode *m_parent;         // +0x04
    union {
        GridNodeGeometryBase *m_lod;    // +0x08, the node's height/LOD geometry
        GridNodeLODData *m_lodData;     // +0x08, the same, as Griddraw.cpp reads a GridNodeGeometryLOD
        GridNodeGeometryPlain *m_plain; // +0x08, the same, a GridNodeGeometryPlain
    };
    float m_heightScale;                // +0x0C
    float m_x10;                        // +0x10
    float m_minHeight;                  // +0x14
    float m_maxHeight;                  // +0x18
    unsigned short m_x1C;               // +0x1C
    unsigned short m_lodIndex;          // +0x1E, index of m_lod in Terrain::m_geometries
    unsigned short m_x;                 // +0x20, grid position
    unsigned short m_y;                 // +0x22
    unsigned char m_level;              // +0x24
    unsigned char m_shift;              // +0x25
    unsigned char m_position;           // +0x26, position among the parent's children
    unsigned char m_x27;                // +0x27
    int m_childTable;                   // +0x28
    int *m_childOffsets;                // +0x2C

    // --- virtuals in retail slot order (retail vtable); vptr +0x30 ---
    virtual ~GridNode();
    virtual int RayIntersects(_D3DVECTOR &from, _D3DVECTOR &to, _D3DVECTOR &hit, GridNode **node, int *x, int *y);
    virtual int RayIntersectsNodes(_D3DVECTOR &from, _D3DVECTOR &to, GridNode **nodes, int *count, int max);
};

class DrawableGridNode : public GridNode {
public:
    DrawableGridNode(long id, int level, DrawableGridNode *parent, Terrain *terrain, int x, int y, int z,
                     int a8, int a9, _D3DVECTOR *min, _D3DVECTOR *max);
    DrawableGridNode *LoadNode(long file, int level, DrawableGridNode *parent, int nodeLevel, int shift, int x, int y,
                               int position, _D3DVECTOR *min, _D3DVECTOR *max);
    void DestroyMesh(int recurse);
    void MakeMesh();
    int MakeMeshViaQuadTree(int x, int y, int size, int quad);
    int ConstructMeshAs4x4();
    int ConstructMesh();
    int TriangulateAxisTriangleLOD(int level, int face, int x, int y, int dx, int dy, int *done);
    void TesselateLowestTriangles(int x, int y, int orientation, int direction, int face);
    int TriangulateDiagTriangleLOD(int level, int face, int x, int y, int dx, int dy, int *done);
    int MakeAxisTriangle(int face, int x, int y, int dx, int dy);
    int RenderDetail(TextureMap *detail);
    void BuildScissorRenderItem(VifDataItem *item, QWORD *scissor);
    void RenderNearClipGeometry(TextureMap *texture, TextureMap *detail, int arg, QWORD *scissor, QWORD *detailScissor);
    void RenderPlainGeometry(TextureMap *texture, TextureMap *detail, int newRenderer, QWORD *a, QWORD *b);
    void UpdateTextureCache();
    int UpdateTextureCacheQuadTreeByQuarters(int x, int z, int size, int quarter);
    int UpdateTextureCacheQuadTreeQuarter(int x, int z, int size, int quarter);
    void CompareDuplicates();
    void CheckDeltaEvaluations();
    float CalculateUncertaintyInterval(int distanceOnly);
    float CalculateDistanceFromCamera();
    int Calc4x4ChildVisible(int child);    // no return value: one caller tests whatever $v0 holds
    float CalculateDistanceTo4x4Child(int index);
    int ProjectedDeltaValueAcceptable(int x, int y);
    void SelectVisibleBlocks(long frame, _D3DMATRIX *view, int evaluateAll);
    int SelectQuadBlocksVisible(long frame, _D3DMATRIX *view, _D3DVECTOR &center, _D3DVECTOR &half,
                                int x, int y, int size, int level, int quad, int parentMeshed);
    void SetBlockLOD(long file, _D3DMATRIX *matrix, int recurse, int parentMeshed);
    void Evaluate();
    void EvaluateByBins();
    void EvaluateVertex(int x, int y);
    void EvaluateDeltas();
    void EvaluateDeltasInDecimatedGrid(int x, int z, int width, int height, int step, int offset, int colStep, int rowStep);
    void NotifyParents(int x, int y, int needed);
    void NotifyVertex(int x, int y, int needed, int mask, HeightVertex *vertex);
    int HandleDeferredEvaluation(int x, int y);
    void NotifyDuplicateVertices(int x, int y, int needed, int mask, HeightVertex *vertex, int level);
    void NotifyDuplicates(DrawableGridNode *child, int x, int y, int needed, int mask, HeightVertex *vertex);

    // --- virtuals in retail slot order ---
    virtual ~DrawableGridNode();
    virtual DrawableGridNode *New(long id, int level, DrawableGridNode *node, int a, int b, int c, int d, int e,
                                  _D3DVECTOR *min, _D3DVECTOR *max);
    virtual void SelectMipMapLevel(float uncertainty);
    virtual void PlaceMipMapInTextureCache(int child);
    virtual void LoadTexture(long file);
    virtual void SetTextureMap();
    virtual void SetDetailTextureMap(int i);
    virtual TextureMap *GetTextureMap();
    virtual int GetAuthoredDetailTextureMapSize(int i);

    char pad34[0x40 - 0x34];
    _D3DVECTOR m_center;                // +0x40
    _D3DVECTOR m_halfSize;              // +0x50
    Terrain *m_terrain;                 // +0x60
    GridNodeGeometry *m_geometry;       // +0x64, the built mesh
    GridNodeTextures *m_textures;       // +0x68
    int m_x6C;                          // +0x6C
    float m_uOffset;                    // +0x70, detail texture u offset
    float m_vOffset;                    // +0x74
    float m_uvScale;                    // +0x78
    float m_uncertainty;                // +0x7C
    unsigned int m_clipCodes;           // +0x80
    int m_x84;                          // +0x84
    char pad88[0x90 - 0x88];
    NodeDmaTag m_dmaTag;                // +0x90, REF tag of the node's packet (5 quadwords from +0xA0)
    QWORD m_vifHeader;                  // +0xA0
    QWORD m_xB0;                        // +0xB0
    QWORD m_xC0;                        // +0xC0
    QWORD m_xD0;                        // +0xD0
    QWORD m_xE0;                        // +0xE0
};

#endif
