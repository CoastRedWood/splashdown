// CollisionTree.h — declarations shared by the VU0 bounding-box-tree
// collision functions in src/global: the face stored in a face tree's
// leaf, the ray-to-face intersection data being gathered, and the globals
// the tree tests set up.
#ifndef GLOBAL_COLLISIONTREE_H
#define GLOBAL_COLLISIONTREE_H

#include "d3dtypes.h"
#include "BoundingBoxTree/BoundingBoxTree.h"

class SoultreeObject;

// A face-tree leaf's face, stored from +0x04 of its BoundingBoxTree node.
struct BoundingBoxTreeFace {
    unsigned short m_vertexIndex[3];    // +0x00, into BoundingBoxTreeVertices
    short m_x06;                        // +0x06
    float m_distance;                   // +0x08, plane: normal . p + distance = 0
    float m_normalX;                    // +0x0C
    float m_normalY;                    // +0x10
    float m_normalZ;                    // +0x14
};

// A ray leaf's segment: its node's box members, read as the ray's ends.
struct RaySegment {
    _D3DVECTOR m_start;                 // +0x00
    _D3DVECTOR m_end;                   // +0x10
};

// The face hits gathered for a ray set (CurrentCollisionData).
struct RayToFaceIntersectionData {
    int m_x00;                          // +0x00
    float m_t;                          // +0x04, nearest hit along its ray
    RaySegment *m_ray;                  // +0x08, nearest hit's ray (start, end)
    char padC[0x10 - 0xC];
    _D3DVECTOR m_normal;                // +0x10, nearest hit's face normal
    _D3DVECTOR m_point;                 // +0x20, nearest hit point
    _D3DVECTOR m_penetration;           // +0x30
    int m_maxRays;                      // +0x40
    int m_numRays;                      // +0x44
    int *m_rayTransformed;              // +0x48, per ray
    int *m_rayHit;                      // +0x4C, per dynamic ray: hit something
    float *m_rayT;                      // +0x50
    RaySegment **m_rays;                // +0x54, per ray
    _D3DVECTOR *m_rayNormals;           // +0x58
    _D3DVECTOR *m_rayPoints;            // +0x5C
};

// The hits gathered for a sphere (CurrentCollisionData of a sphere test).
struct SphereIntersectionData {
    int m_maxHits;                      // +0x00
    int m_count;                        // +0x04
    float m_distance;                   // +0x08, nearest hit's distance from the face plane
    char pad0C[0x20 - 0x0C];
    _D3DVECTOR m_center;                // +0x20
    _D3DVECTOR m_normals[3];            // +0x30
    _D3DVECTOR m_positions[3];          // +0x60
    int m_transformed[3];               // +0x90
    char pad9C[0xA0 - 0x9C];            // 0xA0 bytes (CollisionObject::SetSphereCollision's new)
};

// m_collisionData of a COLLISION_RAYS object.
#include "RayCollisionObject/RayCollisionObject.h"

// m_collisionData of a COLLISION_DYNAMIC_RAYS object.
struct DynamicRayCollisionObject {
    int m_numRays;                      // +0x00
    SoultreeObject **m_objects;         // +0x04, per ray
    _D3DVECTOR *m_localDirections;      // +0x08, per ray, in the tested object's space (also
                                        // the rays' end points, two per ray, in their
                                        // objects' spaces: CollisionObject::SetDynamicRays)
    _D3DVECTOR *m_localStarts;          // +0x0C, per ray, in the tested object's space
    float *m_lengths;                   // +0x10, per ray
    _D3DVECTOR *m_directions;           // +0x14, per ray
    _D3DVECTOR *m_starts;               // +0x18, per ray
    char pad1C[0x20 - 0x1C];
    _D3DVECTOR m_localCenter;           // +0x20, m_center in the tested object's space
    _D3DVECTOR m_center;                // +0x30, bounding sphere
    float m_radius;                     // +0x40
    char pad44[0x50 - 0x44];            // 0x50 bytes (CollisionObject::SetDynamicRayCollision's new)
};

// m_collisionData of a COLLISION_SOULTREE object (one soultree of a
// hierarchy too) and of a COLLISION_SOULTREE_HIERARCHY object.
#include "SoultreeCollisionObject/SoultreeCollisionObject.h"
#include "SoultreeHierarchyCollisionObject/SoultreeHierarchyCollisionObject.h"

enum BoundingBoxTreeType {};

extern RayToFaceIntersectionData *CurrentCollisionData;
extern _D3DVECTOR *BoundingBoxTreeVertices;
extern BoundingBoxTreeType BoundingBoxTreeAType;
extern _D3DMATRIX AtoBSpaceTransform;
extern _D3DMATRIX BtoASpaceTransform;
extern _D3DMATRIX AtoADeltaTransform;

int RayBundleIntersectsBoundingBoxTree(DynamicRayCollisionObject *rays, BoundingBoxTree *tree, _D3DVECTOR *vertices,
                                       RayToFaceIntersectionData *data);
int RayIntersectBoundingBoxTreeFace(BoundingBoxTreeFace *face, _D3DVECTOR dir, _D3DVECTOR start, float length,
                                    _D3DVECTOR *point, _D3DVECTOR *normal, float *t);

#endif
