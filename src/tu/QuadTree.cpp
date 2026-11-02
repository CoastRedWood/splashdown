// QuadTree.cpp  @ 0x00282760 (text 0x282760-0x2853A0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// QuadTree, QuadTreeNode and SearchMethod's two Perform* functions as one
// translation unit without a static initializer, in PSXRenderTarget.cpp's
// tu_map row: its .rodata restarts at @690 after the PSXVideoCard RTTI and
// ends with the QuadTreeNode/BaseObject/SearchMethod/ QuadTree RTTI. The file
// name is a guess.


#include "QuadTree/QuadTree.h"
#include "BlockAllocator/BlockAllocator.h"
#include "GameObject/GameObject.h"
#include "d3dtypes.h"
#include "soultree_assert_block.h"
#include "QuadTreeNode/QuadTreeNode.h"
#include "SearchMethod/SearchMethod.h"

// Defined here for its retail .bss slot.
QuadTree *LpQuadTree;
float TreeToUserScaleFactor;


extern "C" void *realloc(void *p, unsigned int size);

// ---- src/QuadTree/Initialize.cpp
class QuadTree;
extern QuadTree *LpQuadTree;

void QuadTree::Initialize()
{
    LpQuadTree = this;
    m_numNodes = 0;
    m_x88C = -1;
    m_keepBuckets = 0;
    m_numObjects = 0;
    m_listCapacity = 0;
    m_listCursor = 0;
    m_listCount = 0;
    m_list = 0;
    m_listMax = 0;
}


// ---- src/QuadTree/CreateRootNode.cpp
#pragma fast_fptosi on

static inline float Fmax(float a, float b)
{
    float r;
    asm("max.s $f8, %1, %2; swc1 $f8, 0(%0)" : : "r"(&r), "f"(a), "f"(b) : "$f8", "memory");
    return r;
}


class QuadTreeNode;

extern float TreeToUserScaleFactor;

// Sets up the tree's extent, its 32768-unit integer scale and the tile size
// for each level down to `smallest`.
void QuadTree::CreateRootNode(float minX, float minZ, float maxX, float maxZ, float smallest)
{
    m_root = CreateNode();
    m_minX = minX;
    m_maxX = maxX;
    m_minZ = minZ;
    m_maxZ = maxZ;
    m_sizeX = maxX - minX;
    m_sizeZ = maxZ - minZ;
    m_scale = 32768.0f / Fmax(m_sizeX, m_sizeZ);
    TreeToUserScaleFactor = 1.0f / m_scale;
    int smallestTile = (int)(smallest * m_scale);
    m_numLevels = 0;
    int tile = 0x8000;
    do {
        tile >>= 1;
        m_tileSizes[m_numLevels++] = tile;
    } while (m_numLevels < 12 && tile > smallestTile);
    m_numLevels--;
    m_smallestTile = m_tileSizes[m_numLevels];
    m_searchOpen = 0;
    m_searchId = 0;
    m_searchNode = 0;
    m_searchHead = 0;
    m_searchIndex = 0;
    m_searchCurrent = 0;
    m_linkAllocator = new BlockAllocator(sizeof(QuadTreeNodeLink), 0x1000);
    m_bucketAllocator = new BlockAllocator(sizeof(Bucket), 0x1000);
}
#pragma fast_fptosi reset


// ---- src/QuadTree/CreateNode.cpp

QuadTreeNode *QuadTree::CreateNode()
{
    return new QuadTreeNode;
}


// ---- src/QuadTree/dtor.cpp
extern "C" void free(void *p);

class QuadTree;
extern QuadTree *LpQuadTree;





QuadTree::~QuadTree()
{
    LpQuadTree = this;
    m_keepBuckets = 1;
    if (m_list)
        free(m_list);
    if (m_root)
        delete m_root;
    if (m_bucketAllocator)
        m_bucketAllocator->Release();
    if (m_linkAllocator)
        m_linkAllocator->Release();
}


// ---- src/QuadTree/CalculatePointPlacementCode.cpp


unsigned int QuadTree::CalculatePointPlacementCode(_D3DVECTOR *point, unsigned int code)
{
    return CalculatePointPlacementCode(point->x, point->y, point->z, 0);
}


// ---- src/QuadTree/CalculatePointPlacementCode_00282AC0.cpp
// Quadrant code of a point: which child tile (bits 4..7) plus the clamped
// parent-tile coordinates and the level count.
#pragma fast_fptosi on

unsigned int QuadTree::CalculatePointPlacementCode(float x, float y, float z, unsigned int code)
{
    int cx;
    int cz;
    int ix;
    int parentMask;
    int tileMask;
    int T;
    int bx;
    int bz;

    x -= m_minX;
    z -= m_minZ;
    if (x < 0.0f || !(x <= m_sizeX) || z < 0.0f || !(z <= m_sizeX))
        return 0;

    // The masks go through one running local; the z grid coordinate reuses
    // `code`, and the x tile bits reuse `cz` before it gets its real value:
    // those reuses (and the statement order) give retail's registers.
    T = m_smallestTile - 1; tileMask = ~T; T += m_smallestTile; parentMask = ~T;
    ix = (int)(x * m_scale);
    cz = ix & tileMask; cx = ix & parentMask; bx = (cx ^ cz) != 0;
    code = (int)(z * m_scale);
    bx <<= 1;
    cz = code & parentMask; bz = (cz ^ (code & tileMask)) != 0;
    code = 0x10 << (bx | bz);

    if (cx >= 0x8000)
        cx = 0x7FFF;
    if (cz >= 0x8000)
        cz = 0x7FFF;
    return m_numLevels | (code | ((cx << 17) | (cz << 5)));
}
#pragma fast_fptosi reset


// ---- src/QuadTree/CalculateBoxPlacementCode.cpp
#pragma fast_fptosi on

// Placement code of a box (center +- extent) clipped to the tree: the level
// whose tiles hold it, the quadrant bits it spans and its tile coordinates.
// `code` is ignored.
unsigned int QuadTree::CalculateBoxPlacementCode(_D3DVECTOR *center, _D3DVECTOR *extent, unsigned int code)
{
    float minX;
    float minZ;
    float maxX;
    float maxZ;
    int z1;
    int x1;
    int dz;
    int z0;
    int level;
    int dx;
    unsigned int bits;
    int x0;
    int tile;
    int mask;

    minX = center->x - extent->x;
    maxX = center->x + extent->x;
    minZ = center->z - extent->z;
    maxZ = center->z + extent->z;

    if (minX < m_minX)
        minX = m_minX;
    if (!(maxX <= m_maxX))
        maxX = m_maxX;
    if (minZ < m_minZ)
        minZ = m_minZ;
    if (!(maxZ <= m_maxZ))
        maxZ = m_maxZ;
    maxX -= m_minX;
    minX -= m_minX;
    minZ -= m_minZ;
    maxZ -= m_minZ;
    if (maxX < 0.0f || !(minX <= m_sizeX) || maxZ < 0.0f || !(minZ <= m_sizeZ))
        return 0;

    x0 = (int)(minX * m_scale);
    z0 = (int)(minZ * m_scale);
    x1 = (int)(maxX * m_scale) + 1;
    z1 = (int)(maxZ * m_scale) + 1;
    dx = x1 - x0;
    dz = z1 - z0;
    for (level = 0; level <= m_numLevels; level++) {
        if (!(dx < m_tileSizes[level]) || !(dz < m_tileSizes[level]))
            break;
    }
    if (level > m_numLevels)
        level = m_numLevels;

    tile = m_tileSizes[level];
    mask = ~(tile - 1);
    x0 &= mask;
    x1 &= mask;
    z0 &= mask;
    z1 &= mask;
    if (x1 - x0 > tile || z1 - z0 > tile) {
        if (level != 0) {
            tile += tile;
            mask = ~(tile - 1);
            level--;
            x0 &= mask;
            x1 &= mask;
            z0 &= mask;
            z1 &= mask;
        }
    }
    bits = 0x10;
    if (z1 != z0)
        bits |= 0x20;
    if (x1 != x0)
        bits |= 0x40;
    if ((bits & 0x60) == 0x60)
        bits |= 0x80;
    if (x0 >= 0x8000)
        x0 = 0x7FFF;
    if (z0 >= 0x8000)
        z0 = 0x7FFF;
    return level | (bits | ((x0 << 17) | (z0 << 5)));
}
#pragma fast_fptosi reset


// ---- src/QuadTree/InsertObject.cpp

// ASSERT_ONCE with printf-style arguments.
#define ASSERTF_ONCE(cond, printf_args)                                 \
    {                                                                   \
        static int allowThisAssertion = 1;                              \
        if (allowThisAssertion && _AssertFunc_((int)(cond), __FILE__, __LINE__)) { \
            allowThisAssertion = 0;                                     \
            PSXprintf printf_args;                                      \
            PSXprintf("\n");                                            \
            ASSERT_BREAK                                                \
        }                                                               \
    }

void QuadTree::InsertObject(SearchableObject *obj, unsigned int code, _D3DVECTOR *center, _D3DVECTOR *half)
{
    float minHeight;
    QuadTreeNode *node;
    int qx;
    int tileSize;
    int bit;
    int i;
    int count;
    int level;
    QuadTreeNode *parent;
    int j;
    int child;
    int x;
    int mask;
    int z;
    int prevChild;
    float maxHeight;
    int qz;
    QuadTreeNode *next;
    float lo;
    float hi;

    minHeight = 3.40282347e+38f;
    maxHeight = -3.40282347e+38f;
    count = 0;
#line 340 "Quadtree.cpp"
    ASSERT(center, "InsertObject: lpCenter == 0");
    ASSERT(half, "InsertObject: lpHalf == 0");
    if (center && half) {
        minHeight = center->y - half->y;
        maxHeight = center->y + half->y;
    }
#line 347 "Quadtree.cpp"
    ASSERTF_ONCE(minHeight >= -100000.0f, ("Attempt to place bad data in QuadTree. minHeight=%d", minHeight));
    ASSERTF_ONCE(maxHeight <= 100000.0f, ("Attempt to place bad data in QuadTree. maxHeight=%d", maxHeight));
    if (code) {
        if (m_searchOpen)
            CloseSearch();
        i = 0;
        m_numObjects++;
        level = code & 0xF;
        x = (code & 0xFFF00000) >> 17;
        LpQuadTree = this;
        obj->m_x00 = 0;
        z = (code & 0xFFF00) >> 5;
        tileSize = m_tileSizes[level];
        for (bit = 0x10; i < 4; i++, bit <<= 1) {
            if (!(code & bit))
                continue;
            node = m_root;
            parent = 0;
            if (minHeight < maxHeight) {
                lo = m_root->m_centerY - m_root->m_halfY;
                hi = m_root->m_centerY + m_root->m_halfY;
                if (minHeight < lo)
                    lo = minHeight;
                if (!(maxHeight <= hi))
                    hi = maxHeight;
                m_root->m_centerY = (lo + hi) / 2.0f;
                m_root->m_halfY = (hi - lo) / 2.0f;
            }
            qx = x + ((i & 2) ? tileSize : 0);
            qz = z + ((i & 1) ? tileSize : 0);
            mask = 0x4000;
            for (j = 0; j <= level; j++, mask >>= 1) {
                child = ((qx & mask) != 0) << 1;
                child += (qz & mask) != 0;
                if (node->m_children) {
                    next = node->m_children[child];
                } else {
                    if (node == m_root)
                        m_root = node = node->AddChildList();
                    else
                        node = node->AddChildList();
                    if (parent)
                        parent->m_children[prevChild] = node;
                    next = 0;
                }
                if (!next) {
                    node->m_children[child] = next = CreateNode();
                }
                if (minHeight < maxHeight) {
                    lo = next->m_centerY - next->m_halfY;
                    hi = next->m_centerY + next->m_halfY;
                    if (minHeight < lo)
                        lo = minHeight;
                    if (!(maxHeight <= hi))
                        hi = maxHeight;
                    next->m_centerY = (lo + hi) / 2.0f;
                    next->m_halfY = (hi - lo) / 2.0f;
                }
                parent = node;
                prevChild = child;
                node = next;
            }
            next->InsertObject(obj);
            count++;
        }
        // Retail loads count and 1 here and nothing else: a test of count whose
        // result was never used (the branch and the store are optimized away).
        int r;
        if (count == 1)
            r = 1;
    }
}


// ---- src/QuadTree/UpdateExtents.cpp

// Grows the height extent (center/half Y) of every node on the path to the
// object's tiles so it covers the object's new height range.
void QuadTree::UpdateExtents(SearchableObject *obj, unsigned int code, _D3DVECTOR *center, _D3DVECTOR *half)
{
    int j;
    float minHeight;
    QuadTreeNode *node;
    int child;
    int mask;
    int tileSize;
    int qx;
    float lo;
    int bit;
    float hi;
    float maxHeight;
    int i;
    int qz;
    int level;
    int x;
    int z;

#line 492 "Quadtree.cpp"
    ASSERT(center, "UpdateExtents: lpCenter == 0");
    ASSERT(half, "UpdateExtents: lpHalf == 0");
    if (center && half) {
        minHeight = center->y - half->y;
        maxHeight = center->y + half->y;
    }
    if (minHeight < -100000.0f || !(maxHeight <= 100000.0f)) {
        // Retail's conditions are inverted: these only fire when the height is in range.
#line 501 "Quadtree.cpp"
        ASSERT(minHeight < -100000.0f, "UpdateExtents: minHeight < -100,000!");
        ASSERT(maxHeight > 100000.0f, "UpdateExtents: maxHeight > 100,000!");
    }
    if (code == 0)
        return;
    if (!(minHeight <= maxHeight))
        return;

    x = (code & 0xFFF00000) >> 17;
    level = code & 0xF;
    z = (code & 0xFFF00) >> 5;
    tileSize = m_tileSizes[level];
    bit = 0x10;
    for (i = 0; i < 4; i++, bit <<= 1) {
        if (code & bit) {
            node = m_root;
            if (minHeight < maxHeight) {
                lo = m_root->m_centerY - m_root->m_halfY;
                hi = m_root->m_centerY + m_root->m_halfY;
                if (minHeight < lo)
                    lo = minHeight;
                if (!(maxHeight <= hi))
                    hi = maxHeight;
                m_root->m_centerY = (lo + hi) / 2.0f;
                m_root->m_halfY = (hi - lo) / 2.0f;
            }
            qx = x + ((i & 2) ? tileSize : 0);
            qz = z + ((i & 1) ? tileSize : 0);
            mask = 0x4000;
            for (j = 0; j <= level; j++, mask >>= 1) {
                child = ((qx & mask) != 0) << 1;
                child += (qz & mask) != 0;
                if (!node->m_children)
                    break;
                node = node->m_children[child];
                if (minHeight < maxHeight) {
                    lo = node->m_centerY - node->m_halfY;
                    hi = node->m_centerY + node->m_halfY;
                    if (minHeight < lo)
                        lo = minHeight;
                    if (!(maxHeight <= hi))
                        hi = maxHeight;
                    node->m_centerY = (lo + hi) / 2.0f;
                    node->m_halfY = (hi - lo) / 2.0f;
                }
            }
        }
    }
}


// ---- src/QuadTree/RemoveObject.cpp

// Walks down to the object's tile in each quadrant its placement code covers
// and removes it there, deleting a node left empty.
void QuadTree::RemoveObject(SearchableObject *obj, unsigned int code)
{
    QuadTreeNode *node;
    QuadTreeNode *parent;
    int child;
    int tileSize;
    int bit;
    int i;
    int level;
    int j;
    int mask;
    int qx;
    int qz;
    int found;
    int x;
    int z;

    if (code == 0)
        return;
    if (m_searchOpen)
        CloseSearch();

    m_numObjects--;
    x = (code & 0xFFF00000) >> 17;
    level = code & 0xF;
    z = (code & 0xFFF00) >> 5;
    LpQuadTree = this;
    tileSize = m_tileSizes[level];
    bit = 0x10;
    for (i = 0; i < 4; i++, bit <<= 1) {
        if (code & bit) {
            qx = x + ((i & 2) ? tileSize : 0);
            node = m_root;
            parent = 0;
            qz = z + ((i & 1) ? tileSize : 0);
            found = 1;
            mask = 0x4000;
            for (j = 0; j <= level; j++, mask >>= 1) {
                child = ((qx & mask) != 0) << 1;
                child += (qz & mask) != 0;
                parent = node;
                if (node->m_children)
                    node = node->m_children[child];
                else
                    node = 0;
                if (!node) {
                    found = 0;
                    break;
                }
            }
            if (found && !node->RemoveObject(obj)) {
                delete node;
                if (parent)
                    parent->m_children[child] = 0;
            }
        }
    }
}


// ---- src/QuadTreeNode/ctor.cpp

QuadTreeNode::QuadTreeNode()
{
    m_objects = 0;
    m_children = 0;
    LpQuadTree->m_numNodes++;
    m_centerY = 0;
    m_halfY = -3.40282347e+38f;
}


// ---- src/QuadTreeNode/dtor.cpp

QuadTreeNode::~QuadTreeNode()
{
    Bucket *bucket = m_objects;
    if (m_children) {
        for (int i = 0; i < 4; i++) {
            if (m_children[i])
                delete m_children[i];
        }
    }
    if (!LpQuadTree->m_keepBuckets) {
        while (bucket) {
            Bucket *next = bucket->m_next;
            LpQuadTree->m_bucketAllocator->FreeBlock(bucket);
            bucket = next;
        }
    }
    LpQuadTree->m_numNodes--;
}


// ---- src/QuadTreeNode/InsertObject.cpp
extern "C" void *memset(void *, int, unsigned int);

class SearchableObject;


extern QuadTree *LpQuadTree;

// A node's objects are kept in a chain of buckets of four.


// Put object in the first free slot, appending a new bucket when all are full.
void QuadTreeNode::InsertObject(SearchableObject *object)
{
    Bucket *last;
    for (Bucket *bucket = m_objects; bucket; bucket = bucket->m_next) {
        for (int i = 0; i < 4; i++) {
            if (!bucket->m_objects[i]) {
                bucket->m_objects[i] = object;
                return;
            }
        }
        last = bucket;
    }
    Bucket *bucket = (Bucket *)LpQuadTree->m_bucketAllocator->AllocBlock();
    memset(bucket, 0, sizeof(Bucket));
    bucket->m_objects[0] = object;
    if (!m_objects)
        m_objects = bucket;
    else
        last->m_next = bucket;
}


// ---- src/QuadTreeNode/RemoveObject.cpp
class SearchableObject;


extern QuadTree *LpQuadTree;

// A node's objects are kept in a chain of buckets of four.


// Remove object from this node's buckets, freeing buckets that empty out.
// Returns whether the node still holds anything.
int QuadTreeNode::RemoveObject(SearchableObject *object)
{
    Bucket *prev;
    Bucket *bucket = m_objects;
    while (bucket) {
        int empty = 1;
        int found = 0;
        for (int i = 0; i < 4; i++) {
            if (bucket->m_objects[i] == object) {
                found = 1;
                bucket->m_objects[i] = 0;
            } else if (bucket->m_objects[i]) {
                empty = 0;
            }
        }
        if (empty) {
            if (bucket == m_objects)
                m_objects = bucket->m_next;
            else
                prev->m_next = bucket->m_next;
            LpQuadTree->m_bucketAllocator->FreeBlock(bucket);
        }
        if (found)
            break;
        prev = bucket;
        bucket = bucket->m_next;
    }
    if (!m_objects && !(m_children && (m_children[0] || m_children[1] || m_children[2] || m_children[3])))
        return 0;
    return 1;
}


// ---- src/QuadTree/OpenSearch.cpp
// Compiler session state: retail compiled
// this file in an mwcc session that had already generated a two-argument runtime
// helper call (64-bit / or %, double arithmetic or compare). mwcc then models
// every later helper call (__floatdisf, fptoui, fptodp, ...) as reading $a0-$a1,
// so temporaries live before such a call skip $a0/$a1. This unused function
// recreates that state; it is not part of the retail file.
static void MWCC_SHIM_TEXT MwccSessionState(long *a) { a[0] = a[0] / a[1]; }

#pragma fast_fptosi on

extern float TreeToUserScaleFactor;

int QuadTree::OpenSearch(_D3DVECTOR *center, _D3DVECTOR *extent, _D3DVECTOR *outMin, _D3DVECTOR *outMax)
{
    float minX;
    float maxX;
    float minZ;
    float maxZ;
    float t;

    minX = center->x - extent->x - m_minX;
    maxX = center->x + extent->x - m_minX;
    minZ = center->z - extent->z - m_minZ;
    maxZ = center->z + extent->z - m_minZ;
//IB
    unsigned int round;
    unsigned int mask;
    unsigned int x1;
    unsigned int z0;
    unsigned int x0;
    unsigned int z1;
//IE

    if (maxX < minX) {
        t = maxX;
        maxX = minX;
        minX = t;
    }
    if (maxZ < minZ) {
        t = maxZ;
        maxZ = minZ;
        minZ = t;
    }
    if (maxX < 0.0f || !(minX <= m_sizeX) || maxZ < 0.0f || !(minZ <= m_sizeZ))
        return 0;

    float scale = m_scale;
    x0 = (unsigned int)(minX * scale);
    z0 = (unsigned int)(minZ * scale);
    round = m_smallestTile - 1;
    mask = -1 - round;
    x0 &= mask;
    z0 &= mask;
    x1 = (unsigned int)(maxX * scale);
    x1 = round + (x1 & mask);
    z1 = (unsigned int)(maxZ * scale);
    z1 = round + (z1 & mask);

    if (outMin) {
        outMin->x = x0 * TreeToUserScaleFactor;
        outMin->y = -3.40282347e+38f;
        outMin->z = z0 * TreeToUserScaleFactor;
    }
    if (outMax) {
        outMax->x = x1 * TreeToUserScaleFactor;
        outMax->y = 3.40282347e+38f;
        outMax->z = z1 * TreeToUserScaleFactor;
    }
    if (m_searchOpen) {
        if (x0 == m_searchBounds[0] && x1 == m_searchBounds[2] && z0 == m_searchBounds[1] && z1 == m_searchBounds[3]) {
            RewindSearch();
            return 1;
        }
        CloseSearch();
    }
    m_listCursor = 0;
    m_searchOpen = 1;
    m_searchId++;
    m_searchBounds[0] = x0;
    m_searchBounds[1] = z0;
    m_searchBounds[2] = x1;
    m_searchBounds[3] = z1;
    LpQuadTree = this;
    m_searchNode = 0;
    m_searchBucket = 0;
    m_searchIndex = 0;
    m_searchCurrent = 0;
    if (m_root)
        m_searchNode = m_root->ChainIfIntersectBox(m_searchNode, x0, z0, x1, z1, 0, 0, 0x4000, m_smallestTile);
    m_searchHead = m_searchNode;
    return 1;
}
#pragma fast_fptosi reset


// ---- src/QuadTreeNode/ChainIfIntersectBox.cpp
class Bucket;
class QuadTreeNode;


extern QuadTree *LpQuadTree;



// Chain this node and every child that overlaps the box (whole subtrees when
// a child lies inside it) onto link.
QuadTreeNodeLink *QuadTreeNode::ChainIfIntersectBox(QuadTreeNodeLink *link, int minX, int minY, int maxX, int maxY,
                                                    int x, int y, int childSize, int minSize)
{
    if (childSize >= minSize) {
        int i;
        for (i = 0; i < 4; i++) {
            if (m_children && m_children[i]) {
                int cx = x + ((i & 2) ? childSize : 0);
                int cy = y + ((i & 1) ? childSize : 0);
                int right = cx + childSize;
                int bottom = cy + childSize;
                if (minX > right || maxX < cx || minY > bottom || maxY < cy)
                    continue;
                if (!(cx < minX || right > maxX || cy < minY || bottom > maxY))
                    link = m_children[i]->ChainUnconditional(link);
                else
                    link = m_children[i]->ChainIfIntersectBox(link, minX, minY, maxX, maxY, cx, cy, childSize >> 1,
                                                              minSize);
            }
        }
    }
    QuadTreeNodeLink *node = (QuadTreeNodeLink *)LpQuadTree->m_linkAllocator->AllocBlock();
    node->m_node = this;
    node->m_next = link;
    return node;
}


// ---- src/QuadTreeNode/ChainUnconditional.cpp

QuadTreeNodeLink *QuadTreeNode::ChainUnconditional(QuadTreeNodeLink *link)
{
    for (int i = 0; i < 4; i++) {
        if (m_children && m_children[i])
            link = m_children[i]->ChainUnconditional(link);
    }
    QuadTreeNodeLink *newLink = (QuadTreeNodeLink *)LpQuadTree->m_linkAllocator->AllocBlock();
    newLink->m_node = this;
    newLink->m_next = link;
    return newLink;
}


// ---- src/QuadTree/NextSearchObject.cpp




class QuadTree;
extern QuadTree *LpQuadTree;


SearchableObject *QuadTree::NextSearchObject()
{
    if (!m_searchOpen)
        return 0;
    if (m_listCursor)
        return 0;
    LpQuadTree = this;
    while (m_searchNode) {
        if (!m_searchBucket)
            m_searchBucket = m_searchNode->m_node->m_objects;
        while (m_searchBucket) {
            while (m_searchIndex < 4) {
                SearchableObject *obj = m_searchBucket->m_objects[m_searchIndex++];
                if (obj && m_searchId != (unsigned short)obj->m_x00) {
                    obj->m_x00 = m_searchId;
                    return obj;
                }
            }
            m_searchIndex = 0;
            m_searchBucket = m_searchBucket->m_next;
        }
        m_searchNode = m_searchNode->m_next;
    }
    return 0;
}


// ---- src/QuadTree/OpenSearch_002844B0.cpp
#pragma fast_fptosi on

extern "C" void free(void *p);
extern "C" void *malloc(unsigned int size);

// Starts a box search. With `list`, results go to the tree's own growable
// buffer (returned through *list); otherwise into the caller's `buffer`
// of *count entries. *count gets the number found.
int QuadTree::OpenSearch(_D3DVECTOR *center, _D3DVECTOR *extent, int *count, SearchableObject ***list, SearchableObject **buffer)
{
    float minX = center->x - extent->x - m_minX;
    float maxX = center->x + extent->x - m_minX;
    float minZ = center->z - extent->z - m_minZ;
    float maxZ = center->z + extent->z - m_minZ;
    float t;

    if (maxX < minX) {
        t = maxX;
        maxX = minX;
        minX = t;
    }
    if (maxZ < minZ) {
        t = maxZ;
        maxZ = minZ;
        minZ = t;
    }
    if (maxX < 0.0f || !(minX <= m_sizeX) || maxZ < 0.0f || !(minZ <= m_sizeZ))
        return 0;
    if (!list && !buffer)
        return 0;
    if (!count)
        return 0;
    if (m_searchOpen)
        CloseSearch();
    m_searchId++;
    if (list) {
        if (m_listCapacity < m_numObjects) {
            if (m_list)
                free(m_list);
            m_list = 0;
        }
        if (!m_list) {
            m_list = (SearchableObject **)malloc(m_numObjects * PS2_PTR_SIZE);
            m_listCapacity = m_numObjects;
        }
        m_listCount = 0;
        m_listMax = m_numObjects;
        m_listCursor = m_list;
        *list = m_listCursor;
        m_searchOpen = 1;
    } else if (buffer) {
        m_listMax = *count;
        m_listCursor = buffer;
        m_searchOpen = 0;
    }
    LpQuadTree = this;
    int x0 = (int)(minX * m_scale);
    int z0 = (int)(minZ * m_scale);
    int x1 = (int)(maxX * m_scale);
    int z1 = (int)(maxZ * m_scale);
    if (m_root)
        m_root->AddIfIntersectBox(x0, z0, x1, z1, 0, 0, 0x4000, m_smallestTile);
    *count = m_listCount;
    return 1;
}
#pragma fast_fptosi reset


// ---- src/QuadTreeNode/AddIfIntersectBox.cpp
class Bucket;

extern QuadTree *LpQuadTree;


// Add this node's objects and those of every child that overlaps the box
// (whole subtrees when a child lies inside it).
void QuadTreeNode::AddIfIntersectBox(int minX, int minY, int maxX, int maxY, int x, int y, int childSize, int minSize)
{
    if (childSize >= minSize) {
        int i;
        for (i = 0; i < 4; i++) {
            if (m_children && m_children[i]) {
                int cx = x + ((i & 2) ? childSize : 0);
                int cy = y + ((i & 1) ? childSize : 0);
                int right = cx + childSize;
                int bottom = cy + childSize;
                if (minX > right || maxX < cx || minY > bottom || maxY < cy)
                    continue;
                if (!(cx < minX || right > maxX || cy < minY || bottom > maxY))
                    m_children[i]->AddUnconditional();
                else
                    m_children[i]->AddIfIntersectBox(minX, minY, maxX, maxY, cx, cy, childSize >> 1, minSize);
            }
        }
    }
    LpQuadTree->AddBucketContentsToSearchList(m_objects);
}


// ---- src/QuadTreeNode/AddUnconditional.cpp

void QuadTreeNode::AddUnconditional()
{
    for (int i = 0; i < 4; i++) {
        if (m_children && m_children[i])
            m_children[i]->AddUnconditional();
    }
    LpQuadTree->AddBucketContentsToSearchList(m_objects);
}


// ---- src/QuadTree/AddBucketContentsToSearchList.cpp



void QuadTree::AddBucketContentsToSearchList(Bucket *bucket)
{
    SearchableObject *obj;
    while (bucket) {
        for (int i = 0; i < 4; i++) {
            obj = bucket->m_objects[i];
            if (obj) {
                if (m_searchId != (unsigned short)obj->m_x00) {
                    obj->m_x00 = m_searchId;
                    m_listCursor[m_listCount++] = obj;
                    if (m_listCount >= m_listMax)
                        return;
                }
            }
        }
        bucket = bucket->m_next;
    }
}


// ---- src/QuadTree/RewindSearch.cpp
void QuadTree::RewindSearch()
{
    m_searchNode = m_searchHead;
    m_searchIndex = 0;
    m_searchCurrent = 0;
    m_searchId++;
}


// ---- src/QuadTree/CloseSearch.cpp

void QuadTree::CloseSearch()
{
    if (m_searchOpen) {
        m_linkAllocator->Purge();
        m_searchHead = 0;
        m_searchNode = 0;
        m_searchIndex = 0;
        m_searchCurrent = 0;
        m_searchBounds[0] = 0x80000000;
        m_searchBounds[1] = 0x80000000;
        m_searchBounds[2] = 0x80000000;
        m_searchBounds[3] = 0x80000000;
        m_searchOpen = 0;
    }
}


// ---- src/QuadTreeNode/TraverseAsBSP.cpp
class Bucket;



extern QuadTree *LpQuadTree;

class QuadTreeNode;



// Builds a chain of links to the nodes that hold objects, visiting the
// children front-to-back relative to the point (x, z): the child containing
// the point first, the diagonally opposite one last. Children are
// laid out as 0 = (left, top), 1 = (left, mid), 2 = (mid, top), 3 = (mid, mid).
QuadTreeNodeLink *QuadTreeNode::TraverseAsBSP(int x, int z, QuadTreeNodeLink *link, int left, int top, int half)
{
    int midX = left + half;
    int midZ = top + half;

    if (m_children) {
        if (x >= midX) {
            if (z >= midZ) {
                if (m_children[0]) link = m_children[0]->TraverseAsBSP(x, z, link, left, top, half >> 1);
                if (m_children[1]) link = m_children[1]->TraverseAsBSP(x, z, link, left, midZ, half >> 1);
                if (m_children[2]) link = m_children[2]->TraverseAsBSP(x, z, link, midX, top, half >> 1);
                if (m_children[3]) link = m_children[3]->TraverseAsBSP(x, z, link, midX, midZ, half >> 1);
            } else {
                if (m_children[1]) link = m_children[1]->TraverseAsBSP(x, z, link, left, midZ, half >> 1);
                if (m_children[0]) link = m_children[0]->TraverseAsBSP(x, z, link, left, top, half >> 1);
                if (m_children[3]) link = m_children[3]->TraverseAsBSP(x, z, link, midX, midZ, half >> 1);
                if (m_children[2]) link = m_children[2]->TraverseAsBSP(x, z, link, midX, top, half >> 1);
            }
        } else {
            if (z >= midZ) {
                if (m_children[2]) link = m_children[2]->TraverseAsBSP(x, z, link, midX, top, half >> 1);
                if (m_children[3]) link = m_children[3]->TraverseAsBSP(x, z, link, midX, midZ, half >> 1);
                if (m_children[0]) link = m_children[0]->TraverseAsBSP(x, z, link, left, top, half >> 1);
                if (m_children[1]) link = m_children[1]->TraverseAsBSP(x, z, link, left, midZ, half >> 1);
            } else {
                if (m_children[3]) link = m_children[3]->TraverseAsBSP(x, z, link, midX, midZ, half >> 1);
                if (m_children[2]) link = m_children[2]->TraverseAsBSP(x, z, link, midX, top, half >> 1);
                if (m_children[1]) link = m_children[1]->TraverseAsBSP(x, z, link, left, midZ, half >> 1);
                if (m_children[0]) link = m_children[0]->TraverseAsBSP(x, z, link, left, top, half >> 1);
            }
        }
    }
    if (m_objects) {
        QuadTreeNodeLink *newLink = (QuadTreeNodeLink *)LpQuadTree->m_linkAllocator->AllocBlock();
        if (link)
            link->m_next = newLink;
        newLink->m_next = 0;
        newLink->m_node = this;
        return newLink;
    }
    return link;
}


// ---- src/QuadTreeNode/TraverseAsBSP_00284EA0.cpp
class Bucket;


extern QuadTree *LpQuadTree;

class QuadTreeNodeLink;


// Visits the children front-to-back relative to the point (x, z): the child
// containing the point first, the diagonally opposite one last. Children are
// laid out as 0 = (left, top), 1 = (left, mid), 2 = (mid, top), 3 = (mid, mid).
void QuadTreeNode::TraverseAsBSP(int x, int z, int left, int top, int half)
{
    int midX = left + half;
    int midZ = top + half;

    if (m_children) {
        if (x >= midX) {
            if (z >= midZ) {
                if (m_children[0]) m_children[0]->TraverseAsBSP(x, z, left, top, half >> 1);
                if (m_children[1]) m_children[1]->TraverseAsBSP(x, z, left, midZ, half >> 1);
                if (m_children[2]) m_children[2]->TraverseAsBSP(x, z, midX, top, half >> 1);
                if (m_children[3]) m_children[3]->TraverseAsBSP(x, z, midX, midZ, half >> 1);
            } else {
                if (m_children[1]) m_children[1]->TraverseAsBSP(x, z, left, midZ, half >> 1);
                if (m_children[0]) m_children[0]->TraverseAsBSP(x, z, left, top, half >> 1);
                if (m_children[3]) m_children[3]->TraverseAsBSP(x, z, midX, midZ, half >> 1);
                if (m_children[2]) m_children[2]->TraverseAsBSP(x, z, midX, top, half >> 1);
            }
        } else {
            if (z >= midZ) {
                if (m_children[2]) m_children[2]->TraverseAsBSP(x, z, midX, top, half >> 1);
                if (m_children[3]) m_children[3]->TraverseAsBSP(x, z, midX, midZ, half >> 1);
                if (m_children[0]) m_children[0]->TraverseAsBSP(x, z, left, top, half >> 1);
                if (m_children[1]) m_children[1]->TraverseAsBSP(x, z, left, midZ, half >> 1);
            } else {
                if (m_children[3]) m_children[3]->TraverseAsBSP(x, z, midX, midZ, half >> 1);
                if (m_children[2]) m_children[2]->TraverseAsBSP(x, z, midX, top, half >> 1);
                if (m_children[1]) m_children[1]->TraverseAsBSP(x, z, left, midZ, half >> 1);
                if (m_children[0]) m_children[0]->TraverseAsBSP(x, z, left, top, half >> 1);
            }
        }
    }
    LpQuadTree->AddSortedBucketContentsToSearchList(m_objects, 0);
}


// ---- src/QuadTreeNode/AddChildList.cpp

QuadTreeNode *QuadTreeNode::AddChildList()
{
    // The four child pointers live in the same block, right behind the node.
    QuadTreeNode *node = (QuadTreeNode *)realloc(this, sizeof(QuadTreeNode) + 4 * sizeof(QuadTreeNode *));
    node->m_children = (QuadTreeNode **)(node + 1);
    node->m_children[0] = 0;
    node->m_children[1] = 0;
    node->m_children[2] = 0;
    node->m_children[3] = 0;
    return node;
}


// ---- src/QuadTree/AddSortedBucketContentsToSearchList.cpp



// Like AddBucketContentsToSearchList, but entries can be tagged by setting
// the low bit of the stored pointer.
void QuadTree::AddSortedBucketContentsToSearchList(Bucket *bucket, int tag)
{
    SearchableObject *obj;
    while (bucket) {
        SearchableObject **objects = bucket->m_objects;
        for (int i = 0; i < 4; i++) {
            if (m_listCount >= m_listMax)
                return;
            obj = *objects++;
            if (obj) {
                if (m_searchId != (unsigned short)obj->m_x00) {
                    obj->m_x00 = m_searchId;
                    if (tag)
                        obj = (SearchableObject *)((char *)obj + 1);
                    m_listCursor[m_listCount++] = obj;
                }
            }
        }
        bucket = bucket->m_next;
    }
}


// ---- src/QuadTree/PlacementCodeIsInSingleTile.cpp
// True when the code's level matches and exactly one quadrant bit is set.
int QuadTree::PlacementCodeIsInSingleTile(unsigned int code)
{
    unsigned int quadrants = code & 0xF0;
    if ((code & 0xF) == m_numLevels && (quadrants == 0x10 || quadrants == 0x20 || quadrants == 0x40 || quadrants == 0x80))
        return 1;
    return 0;
}


// ---- src/SearchMethod/PerformPrepareGeometry.cpp

void SearchMethod::PerformPrepareGeometry()
{
}


// ---- src/SearchMethod/PerformRender3D.cpp

void SearchMethod::PerformRender3D()
{
}

