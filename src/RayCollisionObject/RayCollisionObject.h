// RayCollisionObject: the m_collisionData of a COLLISION_RAYS CollisionObject.
// global/CollisionTree.h and the CollisionObject TU (src/tu/CollisionObject.cpp)
// use this one.
#ifndef RAYCOLLISIONOBJECT_RAYCOLLISIONOBJECT_H
#define RAYCOLLISIONOBJECT_RAYCOLLISIONOBJECT_H

#include "d3dtypes.h"

struct BoundingBoxTree;
class SoultreeObject;

struct RayCollisionObject {
    SoultreeObject *m_object;           // +0x00
    BoundingBoxTree *m_tree;            // +0x04 (ClearRayCollisionObject deletes it)
    char pad08[0x10 - 0x08];
    _D3DMATRIX m_matrix;                // +0x10
};

#endif
