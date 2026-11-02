// VisibilityQuadTree.cpp  @ 0x0029D5F0 (text 0x29D5F0-0x29E920)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's VisibilityQuadTree.cpp as one translation unit:
// VisibilityQuadTree's and VisibilityQuadTreeNode's functions, then the
// static initializer. `make link` places the object's data as well as its
// code (tools/link_c.py). The rest of the tu_map row is two TUs: visible.cpp
// (0x29E920-0x29EB50, VisibleTest; src/tu/visible.cpp) and AgeManager.cpp
// (0x29EB50-0x29F020, its "AgeManager.cpp" asserts; src/tu/AgeManager.cpp).



#include "soultree_assert.h"
#include "VisibilityQuadTree/VisibilityQuadTree.h"
#include "SearchMethod/SearchMethod.h"
#include "QuadTree/QuadTree.h"
#include "d3dtypes.h"
#include "VisibleTest/VisibleTest.h"
#include "Camera/Camera.h"
#include "RenderTarget/RenderTarget.h"
#include "PeakInt/PeakInt.h"

extern "C" void free(void *p);
extern "C" void *malloc(unsigned int size);

extern VisibilityQuadTree *LpVisibilityQuadTree;
extern QuadTree *LpQuadTree;
extern VisibleTest *VisTest;
extern float TreeToUserScaleFactor;
extern RenderTarget *LpRenderTarget;

// The QuadTreeNode that walks itself front to back for the visibility
// search. 0x14 bytes (`new` in CreateNode); no other file uses it.
class VisibilityQuadTreeNode : public QuadTreeNode {
public:
    VisibilityQuadTreeNode();
    void TraverseAsVisibleBSP(int px, int pz, int x, int z, int size, Camera *camera);
    virtual ~VisibilityQuadTreeNode();
};

// The PrepareGeometry timing peak, constructed by the static initializer;
// its .bss precedes the functions' statics in retail.
static PeakInt PeakPrepareGeometryTime(5000);

// Defined here for its retail .bss slot.
VisibilityQuadTree *LpVisibilityQuadTree;


// ---- src/VisibilityQuadTree/ctor.cpp
#pragma alignlabel off

VisibilityQuadTree::VisibilityQuadTree()
{
    if (LpVisibilityQuadTree) {
        PSXprintf("A VisibilityQuadTree has already been constructed!\n");
#line 40 "VisibilityQuadTree.cpp"
        ASSERT(0, "VisibilityQuadTree constructor: A VisibilityQuadTree has already been constructed!");
    }
    LpVisibilityQuadTree = this;
    m_numNodes = 0;
    m_root = 0;
    m_maxSizeOfSearchList = 0;
}

#pragma alignlabel reset


// ---- src/VisibilityQuadTree/dtor.cpp

VisibilityQuadTree::~VisibilityQuadTree()
{
    LpVisibilityQuadTree = 0;
}


// ---- src/VisibilityQuadTree/Create.cpp


// The quadtree is 2D: the y extents are ignored.
VisibilityQuadTree *VisibilityQuadTree::Create(float minX, float minY, float minZ, float maxX, float maxY, float maxZ, float minSize)
{
    Initialize();
    CreateRootNode(minX, minZ, maxX, maxZ, minSize);
    return this;
}


// ---- src/VisibilityQuadTree/SetMaxSizeOfSearchList.cpp

void VisibilityQuadTree::SetMaxSizeOfSearchList(int size)
{
    m_maxSizeOfSearchList = size;
}


// ---- src/VisibilityQuadTree/CreateNode.cpp

QuadTreeNode *VisibilityQuadTree::CreateNode()
{
    return new VisibilityQuadTreeNode;
}


// ---- src/VisibilityQuadTreeNode/ctor.cpp

VisibilityQuadTreeNode::VisibilityQuadTreeNode()
{
}


// ---- src/VisibilityQuadTree/TraverseAsVisibleBSP.cpp
#pragma fast_fptosi on

int VisibilityQuadTree::TraverseAsVisibleBSP(float x, float z, Camera *camera, int *count, SearchableObject ***list, SearchableObject **buffer)
{
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
        if (*count == 0)
            m_listMax = m_numObjects;
        else
            m_listMax = *count;
        if (!m_list) {
            m_list = (SearchableObject **)malloc(m_listMax * PS2_PTR_SIZE);
            m_listCapacity = m_listMax;
        }
        m_listCount = 0;
        m_listCursor = m_list;
        *list = m_listCursor;
        m_searchOpen = 1;
    } else if (buffer) {
        m_listMax = *count;
        m_listCursor = buffer;
        m_searchOpen = 0;
    }
    LpQuadTree = this;
    x -= m_minX;
    z -= m_minZ;
    int gridX = x * m_scale;
    int gridZ = z * m_scale;
    if (m_root)
        ((VisibilityQuadTreeNode *)m_root)->TraverseAsVisibleBSP(gridX, gridZ, 0, 0, 0x4000, camera);
    *count = m_listCount;
    return 1;
}


// ---- src/VisibilityQuadTreeNode/TraverseAsVisibleBSP.cpp
#pragma fast_fptosi reset

#define CHILD(k) ((VisibilityQuadTreeNode *)m_children[k])

// Tests one child's box; fully inside adds the whole subtree, partly inside recurses.
#define VISIT(k, sx, sz, nx, nz)                                                    \
    if (m_children[k]) {                                                            \
        center.y = m_children[k]->m_centerY;                                        \
        extent.y = m_children[k]->m_halfY;                                          \
        center.x = cx sx h;                                                         \
        center.z = cz sz h;                                                         \
        if (VisTest->BoundingBoxVisible(&camera->m_viewProjection, &center, &extent, &clip)) { \
            if (!clip)                                                              \
                m_children[k]->TraverseAsBSP(px, pz, nx, nz, half);                 \
            else                                                                    \
                CHILD(k)->TraverseAsVisibleBSP(px, pz, nx, nz, half, camera);  \
        }                                                                           \
    }

void VisibilityQuadTreeNode::TraverseAsVisibleBSP(int px, int pz, int x, int z, int size, Camera *camera)
{
    int x1 = x + size;
    int z1 = z + size;
    int half = size >> 1;
    float cx = LpQuadTree->m_minX + x1 * TreeToUserScaleFactor;
    float cz = LpQuadTree->m_minZ + z1 * TreeToUserScaleFactor;
    float h = half * TreeToUserScaleFactor;
    _D3DVECTOR center;
    _D3DVECTOR extent;
    unsigned int clip;
    extent.x = h;
    extent.z = h;
    LpQuadTree->AddSortedBucketContentsToSearchList(m_objects, 1);
    if (m_children) {
        if (px >= x1) {
            if (pz >= z1) {
                VISIT(0, -, -, x, z)
                VISIT(1, -, +, x, z1)
                VISIT(2, +, -, x1, z)
                VISIT(3, +, +, x1, z1)
            } else {
                VISIT(1, -, +, x, z1)
                VISIT(0, -, -, x, z)
                VISIT(3, +, +, x1, z1)
                VISIT(2, +, -, x1, z)
            }
        } else {
            if (pz >= z1) {
                VISIT(2, +, -, x1, z)
                VISIT(3, +, +, x1, z1)
                VISIT(0, -, -, x, z)
                VISIT(1, -, +, x, z1)
            } else {
                VISIT(3, +, +, x1, z1)
                VISIT(2, +, -, x1, z)
                VISIT(1, -, +, x, z1)
                VISIT(0, -, -, x, z)
            }
        }
    }
}


// ---- src/VisibilityQuadTree/PerformRender3D.cpp

void VisibilityQuadTree::PerformRender3D()
{
    Render3DOnSearchListContents(m_listCursor, m_listCount);
}


// ---- src/VisibilityQuadTree/PerformPrepareGeometry_audit.cpp

void VisibilityQuadTree::PerformPrepareGeometry()
{
    // Retail initializes this function-local static but never reads it.
    static int PrepareGeometryID = -1;
    CloseSearch();
    Camera *camera = LpRenderTarget->m_camera;
    SearchableObject **list;
    int count = m_maxSizeOfSearchList;
    TraverseAsVisibleBSP(camera->m_position.x, camera->m_position.z, camera, &count, &list, 0);
    PrepareGeometryOnSearchListContents(m_listCursor, &m_listCount);
}


// ---- src/VisibilityQuadTreeNode/dtor.cpp

VisibilityQuadTreeNode::~VisibilityQuadTreeNode()
{
}
