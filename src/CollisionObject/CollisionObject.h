// CollisionObject: a searchable GameObject that carries one collision shape
// (m_collisionType selects what m_collisionData points to) and tests itself
// against the objects of its test list. 0x110 bytes (`new CollisionObject`):
// SearchableObject at +0, GameObject at +0x08, data from +0x38, its own vptr
// at +0x100 (declared after the data), then 0xC unknown bytes.
// Names follow CollisionObject's own methods (src/CollisionObject/), otherwise
// the most common name among the files that declared it locally.
#ifndef COLLISIONOBJECT_COLLISIONOBJECT_H
#define COLLISIONOBJECT_COLLISIONOBJECT_H

#include "GameObject/GameObject.h"
#include "d3dtypes.h"

extern "C" void *realloc(void *p, unsigned int size);
extern "C" void free(void *p);

class SoultreeObject;
class SearchNode;
class SearchPlacement;
struct BoundingBoxTree;
struct SoultreeCollisionObject;
struct SoultreeHierarchyCollisionObject;
struct BoxCollisionObject;
struct RadiusLineSegmentCollisionObject;
struct VertexRayToFaceIntersectionData;
struct RayToFaceIntersectionData;
struct DynamicRayCollisionObject;
struct SoultreeCollisionInstanceData;
struct SoultreeHierarchyCollisionInstanceData;

struct SphereCollisionObject {
    _D3DVECTOR center;          // +0x00
    float radius;               // +0x10
    float radiusSquared;        // +0x14
    float m_radiusScale;        // +0x18
    float m_scale;              // +0x1C
    SoultreeObject *m_object;   // +0x20
    char pad24[0x30 - 0x24];
    _D3DMATRIX m_matrix;        // +0x30
};

enum CollisionObjectTestListType {};

enum CollisionType {
    COLLISION_SOULTREE = 0,
    COLLISION_SOULTREE_HIERARCHY = 1,
    COLLISION_RAYS = 2,
    COLLISION_DYNAMIC_RAYS = 3,
    COLLISION_RADIUS_LINE_SEGMENT = 4,
    COLLISION_SPHERE = 5,
    COLLISION_BOX = 6,
    COLLISION_TYPE_7 = 7            // switches list it, nothing tests against it
};

// CollisionObject's first base: 8 bytes in front of its GameObject subobject.
#include "SearchableObject/SearchableObject.h"

class CollisionObject;
typedef void (*CollisionCallback)(CollisionObject *self, CollisionObject *other);

int TestRayBundleForCollision(DynamicRayCollisionObject *rays, CollisionObject *other, RayToFaceIntersectionData *data);
int TestSphereForCollision(_D3DVECTOR *center, float radius, float radiusSquared, CollisionObject *other);

class CollisionObject : public SearchableObject, public GameObject {
public:
    CollisionObject(int enabled);
    GameObject *Create(unsigned int flags);
    void SetTestListType(CollisionObjectTestListType type);
    void SetOwner(void *owner, unsigned int ownerType);
    void ClearObjectTypeList();
    void AddToObjectTypeList(unsigned int type);
    void AddToTestList(CollisionObject *object);
    void RemoveFromTestList(CollisionObject *object);
    int ValidTestObject(CollisionObject *other);
    void AddSearchPlacement(SearchNode *node, unsigned int flags);
    void RemoveFromSearch();
    void UpdateSearchPlacement();
    void GetCenterHalf(_D3DVECTOR *center, _D3DVECTOR *half);
    void SearchForCollisions(int max, int *count, SearchableObject **results);
    float CheckForCollision(int flags);         // distance before the next full check, in $f0
    int CheckForCollision(int count, SearchableObject **objects);
    void ClearCollisionData();
    void ResetCollisionData();
    void FinishCollisionData();
    void GenerateCollisionPositionAndNormal();
    void InitializeTransform();
    void InitializeTransform(_D3DMATRIX *matrix);
    void UpdateTransform();
    void UpdateTransform(_D3DMATRIX *matrix);
    void UpdateCollisionInfo();
    void UpdateCollisionInfo(_D3DMATRIX *matrix);
    int CheckForOverlap(CollisionObject *other);
    void ClearIntersectionData();       // inline in ClearCollisionData.cpp
    void UpdateSoultreeHierarchyBoundingBox();
    void UpdateSphereCollision(_D3DVECTOR *center, float *radius);
    void SetBoxCollision(_D3DVECTOR min, _D3DVECTOR max);
    void SetDynamicRayCollision(int numRays, _D3DVECTOR *rays, SoultreeObject **objects);
    void SetDynamicRays(_D3DVECTOR *rays, SoultreeObject **objects);
    void SetGeometryCollision(_D3DVECTOR *vertices, unsigned int *faces, int numFaces, int numVertices);
    void SetRadiusLineSegmentCollision(_D3DVECTOR start, _D3DVECTOR end, float radius);
    void SetRayCollision(int numRays, _D3DVECTOR *rays, SoultreeObject *object);
    void SetSoultreeCollisionRays(SoultreeObject *object, char *name, _D3DVECTOR *origin);
    void SetSphereCollision(_D3DVECTOR center, float radius, SoultreeObject *object);
    int SetSoultreeCollision(SoultreeObject *object, char *name);
    int ReadCollisionData(SoultreeObject *object, long handle);
    void ReadSoultreeCollisionData(long handle, SoultreeCollisionObject *data);
    void ReadSoultreeHierarchyCollisionData(SoultreeObject *object, long handle, SoultreeHierarchyCollisionObject *data);
    int InstanceCollisionData(SoultreeObject *object, void *data);
    int InstanceSoultreeCollisionData(SoultreeObject *object, SoultreeCollisionInstanceData *data);
    int InstanceSoultreeHierarchyCollisionData(SoultreeObject *object, SoultreeHierarchyCollisionInstanceData *instance); // defined void: retail tail-calls it
    int TestForCollision(CollisionObject *other);
    int TestBoxAgainst(CollisionObject *other);
    int TestRaysAgainst(CollisionObject *other);
    int TestSoultreeAgainst(CollisionObject *other);
    int TestSoultreeAgainstSoultree(SoultreeCollisionObject *a, SoultreeCollisionObject *b, VertexRayToFaceIntersectionData *data);
    int TestSoultreeAgainstSoultreeReverse(SoultreeCollisionObject *a, SoultreeCollisionObject *b, VertexRayToFaceIntersectionData *data);
    int TestSoultreeAgainstSoultreeHierarchy(SoultreeCollisionObject *a, SoultreeHierarchyCollisionObject *b, VertexRayToFaceIntersectionData *data);
    int TestSoultreeAgainstSoultreeHierarchyReverse(SoultreeCollisionObject *a, SoultreeHierarchyCollisionObject *b, VertexRayToFaceIntersectionData *data);
    int TestSoultreeAgainstSphere(SoultreeCollisionObject *a, SphereCollisionObject *b);
    int TestSoultreeAgainstBox(SoultreeCollisionObject *a, BoxCollisionObject *b);
    int TestSoultreeAgainstRadiusLineSegment(SoultreeCollisionObject *a, RadiusLineSegmentCollisionObject *b);
    int TestSoultreeHierarchyAgainst(CollisionObject *other);
    int TestSoultreeHierarchyAgainstBox(SoultreeHierarchyCollisionObject *a, BoxCollisionObject *b);
    int TestSoultreeHierarchyAgainstSoultree(SoultreeHierarchyCollisionObject *a, SoultreeCollisionObject *b, VertexRayToFaceIntersectionData *data);
    int TestSoultreeHierarchyAgainstSoultreeReverse(SoultreeHierarchyCollisionObject *a, SoultreeCollisionObject *b, VertexRayToFaceIntersectionData *data);
    int TestSoultreeHierarchyAgainstSoultreeHierarchy(SoultreeHierarchyCollisionObject *a, SoultreeHierarchyCollisionObject *b, VertexRayToFaceIntersectionData *data);
    int TestSoultreeHierarchyAgainstSoultreeHierarchyReverse(SoultreeHierarchyCollisionObject *a, SoultreeHierarchyCollisionObject *b, VertexRayToFaceIntersectionData *data);
    int TestSoultreeHierarchyAgainstSphere(SoultreeHierarchyCollisionObject *a, SphereCollisionObject *b);
    int TestSoultreeHierarchyAgainstRadiusLineSegment(SoultreeHierarchyCollisionObject *a, RadiusLineSegmentCollisionObject *b);
    void RenderFaceTree(BoundingBoxTree *tree, int depth, _D3DMATRIX *matrix, _D3DMATRIX *matrix2, _D3DVECTOR *vertices);
    void RenderFaceTree(BoundingBoxTree *tree, int depth, _D3DMATRIX *matrix, _D3DVECTOR *vertices);
    void RenderFaceTreeFaces(BoundingBoxTree *tree, _D3DVECTOR *vertices, _D3DMATRIX *matrix);
    void RenderRays(BoundingBoxTree *tree, _D3DMATRIX *matrix);
    void RenderRayTree(BoundingBoxTree *tree, int depth, _D3DMATRIX *matrix);
    void RenderSoultreeCollisionObject(SoultreeCollisionObject *soultree, int depth, int mode);
    void RenderSoultreeHierarchyCollisionObject(SoultreeHierarchyCollisionObject *object, int depth, int mode);

    char pad38[0x40 - 0x38];
    _D3DVECTOR m_center;                        // +0x40, search extents center
    _D3DVECTOR m_half;                          // +0x50, search extents half size
    float m_searchRadius;                       // +0x60
    float m_x64;                                // +0x64, distance left before the next full check
    int m_collisionType;                        // +0x68, CollisionType
    void *m_collisionData;                      // +0x6C, type depends on m_collisionType
    VertexRayToFaceIntersectionData *m_intersectionData; // +0x70
    void *m_owner;                              // +0x74 (SetOwner)
    void *m_ownerData;                          // +0x78, e.g. the owner's PhysicsBody
    unsigned int m_ownerType;                   // +0x7C (SetOwner)
    char m_x80;                                 // +0x80
    char pad81[0x88 - 0x81];
    long m_fileHandle;                          // +0x88, collision file being read (-1)
    signed char m_ignoreVegetation : 1;         // +0x90 bit 0
    signed char m_searchable : 1;               //        bit 1 (1 from the ctor)
    signed char m_collidable : 1;               //        bit 2 (1 from the ctor)
    signed char m_tempPlacement : 1;            //        bit 3
    signed char m_tested : 1;                   //        bit 4
    signed char m_collided : 1;                 //        bit 5
    unsigned int m_testMask;                    // +0x94 (-1 from the ctor)
    int m_testListType;                         // +0x98, a CollisionObjectTestListType
    CollisionObject **m_testList;               // +0x9C
    int m_numTestObjects;                       // +0xA0
    int m_testList2Type;                        // +0xA4
    CollisionObject **m_testList2;              // +0xA8
    int m_numTestObjects2;                      // +0xAC
    int m_objectTypeListType;                   // +0xB0
    unsigned int *m_objectTypes;                // +0xB4
    int m_numObjectTypes;                       // +0xB8
    SearchPlacement *m_searchPlacement;         // +0xBC
    CollisionCallback m_callback;               // +0xC0
    CollisionCallback m_otherCallback;          // +0xC4
    int m_debugIndex;                           // +0xC8, debug display selection
    int m_debugMode;                            // +0xCC, cycles through 0..7
    CollisionObject *m_testObject;              // +0xD0
    char padD4[0xE0 - 0xD4];
    _D3DVECTOR m_collisionPosition;             // +0xE0
    _D3DVECTOR m_collisionNormal;               // +0xF0

    // Own virtuals: vptr at +0x100 (the GameObject subobject keeps its own).
    virtual ~CollisionObject();
    virtual int KeyDownHandler(ProcessNode *node, KeyInfo *key);
    virtual int Tick(float dt);

    char pad104[0x110 - 0x104];
};

#endif
