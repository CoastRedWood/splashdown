// QuadTree: spatial search tree over the XZ plane (SearchMethod subclass).
#ifndef QUADTREE_QUADTREE_H
#define QUADTREE_QUADTREE_H

#include "d3dtypes.h"

#include "SearchableObject/SearchableObject.h"
#include "SearchMethod/SearchMethod.h"
class BlockAllocator;
// A node's object list: buckets of four (0x14 bytes, from the tree's
// m_bucketAllocator).
class Bucket {
public:
    Bucket *m_next;                     // +0x00
    SearchableObject *m_objects[4];     // +0x04
};

class QuadTreeNode;

// A search's node chain (8 bytes, from the tree's m_linkAllocator).
class QuadTreeNodeLink {
public:
    QuadTreeNodeLink *m_next;           // +0x00
    QuadTreeNode *m_node;               // +0x04
};

class QuadTreeNode {
public:
    QuadTreeNode();
    int RemoveObject(SearchableObject *obj);
    QuadTreeNode *AddChildList();
    void InsertObject(SearchableObject *obj);
    void AddIfIntersectBox(int x0, int z0, int x1, int z1, int a, int b, int c, int d);
    QuadTreeNodeLink *ChainIfIntersectBox(QuadTreeNodeLink *link, int x0, int z0, int x1, int z1, int a, int b, int c, int d);
    void AddUnconditional();
    QuadTreeNodeLink *ChainUnconditional(QuadTreeNodeLink *link);

    QuadTreeNode **m_children;          // +0x00, 4 entries
    Bucket *m_objects;                  // +0x04
    float m_centerY;                    // +0x08 height extent of the objects below
    float m_halfY;                      // +0x0C
    virtual ~QuadTreeNode();            // vptr +0x10
    virtual QuadTreeNodeLink *TraverseAsBSP(int x, int z, QuadTreeNodeLink *link, int left, int top, int half);
    virtual void TraverseAsBSP(int px, int pz, int x, int z, int size);
};

// A SearchMethod (vptr +0x08, m_searchId/m_searchOpen at +0x14/+0x16), so
// QuadTree's own data starts at +0x30. 0x8B8 bytes: VisibilityQuadTree adds
// m_maxSizeOfSearchList at +0x8B8 and is 0x8C0 (`new VisibilityQuadTree`).
// Its constructor is inline (VisibilityQuadTree's ctor has `QuadTree() {}`).
// Names follow QuadTree's own methods (src/QuadTree/).
class QuadTree : public SearchMethod {
public:
    void Initialize();
    void CreateRootNode(float minX, float minZ, float maxX, float maxZ, float smallest);
    int PlacementCodeIsInSingleTile(unsigned int code);
    void AddBucketContentsToSearchList(Bucket *bucket);
    void AddSortedBucketContentsToSearchList(Bucket *bucket, int tag);

    // --- virtuals in retail slot order ---
    virtual ~QuadTree();
    virtual unsigned int CalculatePointPlacementCode(_D3DVECTOR *point, unsigned int code);
    virtual unsigned int CalculatePointPlacementCode(float x, float y, float z, unsigned int code);
    virtual unsigned int CalculateBoxPlacementCode(_D3DVECTOR *min, _D3DVECTOR *max, unsigned int code);
    virtual void InsertObject(SearchableObject *obj, unsigned int code, _D3DVECTOR *center, _D3DVECTOR *half);
    virtual void UpdateExtents(SearchableObject *obj, unsigned int code, _D3DVECTOR *min, _D3DVECTOR *max);
    virtual void RemoveObject(SearchableObject *obj, unsigned int code);
    virtual int OpenSearch(_D3DVECTOR *center, _D3DVECTOR *extent, _D3DVECTOR *outMin, _D3DVECTOR *outMax);
    virtual int OpenSearch(_D3DVECTOR *center, _D3DVECTOR *extent, int *count, SearchableObject ***list, SearchableObject **buffer);
    virtual SearchableObject *NextSearchObject();
    virtual void RewindSearch();
    virtual void CloseSearch();
    virtual QuadTreeNode *CreateNode();

    float m_minX;                       // +0x30
    float m_maxX;                       // +0x34
    float m_minZ;                       // +0x38
    float m_maxZ;                       // +0x3C
    char pad40[0x48 - 0x40];
    float m_sizeX;                      // +0x48
    float m_sizeZ;                      // +0x4C
    char pad50[0x54 - 0x50];
    int m_numLevels;                    // +0x54
    int m_smallestTile;                 // +0x58
    float m_scale;                      // +0x5C, 32768 / the larger size
    int m_tileSizes[12];                // +0x60
    char pad90[0x94 - 0x90];
    QuadTreeNode *m_root;               // +0x94
    BlockAllocator *m_bucketAllocator;  // +0x98, 0x14-byte Buckets
    BlockAllocator *m_linkAllocator;    // +0x9C, 8-byte QuadTreeNodeLinks (purged by CloseSearch)
    char m_keepBuckets : 1;             // +0xA0 bit 0, set by the dtor before deleting the nodes
    QuadTreeNodeLink *m_searchHead;     // +0xA4
    QuadTreeNodeLink *m_searchNode;     // +0xA8
    int m_searchIndex;                  // +0xAC
    Bucket *m_searchBucket;             // +0xB0
    int m_searchCurrent;                // +0xB4
    char padB8[0x888 - 0xB8];
    int m_numNodes;                     // +0x888
    int m_x88C;                         // +0x88C, -1 from Initialize
    int m_numObjects;                   // +0x890, default list size
    int m_listCapacity;                 // +0x894
    SearchableObject **m_listCursor;    // +0x898
    SearchableObject **m_list;          // +0x89C, the tree's own growable list
    int m_listCount;                    // +0x8A0
    int m_listMax;                      // +0x8A4
    unsigned int m_searchBounds[4];     // +0x8A8 x0, z0, x1, z1
};

extern QuadTree *LpQuadTree;

#endif
