// SoultreeCollisionObject: the collision data of one SoultreeObject (a plain
// struct: no vtable, no methods of its own; CollisionObject reads, instances
// and tests it). 0x1B0 bytes. Names follow the former CollisionObject/vu0col.h where
// the copies disagree; +0x150 is declared float[16] by three copies.
#ifndef SOULTREECOLLISIONOBJECT_SOULTREECOLLISIONOBJECT_H
#define SOULTREECOLLISIONOBJECT_SOULTREECOLLISIONOBJECT_H

#include "d3dtypes.h"

struct BoundingBoxTree;
class SoultreeObject;
struct SoultreeCollisionInstanceData;

struct SoultreeCollisionObject {
    int m_x00;                          // +0x000
    SoultreeObject *m_object;           // +0x004
    char pad8[0x10 - 0x8];
    _D3DMATRIX m_x10;                   // +0x010
    _D3DMATRIX m_matrix;                // +0x050
    _D3DMATRIX m_x90;                   // +0x090
    _D3DMATRIX m_xD0;                   // +0x0D0
    _D3DMATRIX m_x110;                  // +0x110, last update's m_xD0
    _D3DMATRIX m_transform;             // +0x150, delta transform
    BoundingBoxTree *m_faceTree;        // +0x190
    BoundingBoxTree *m_tree;            // +0x194
    _D3DVECTOR *m_vertices;             // +0x198
    int m_numVertices;                  // +0x19C
    int m_x1A0;                         // +0x1A0
    SoultreeCollisionInstanceData *m_instance; // +0x1A4
    char pad1A8[0x1B0 - 0x1A8];
};

#endif
