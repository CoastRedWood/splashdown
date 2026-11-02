// SoultreeCollisionInstanceData: collision data shared by the instances of
// one soultree's SoultreeCollisionObject (its m_instance), ref-counted.
// 0x70 bytes. The CollisionObject per-function files still keep local
// views (ReadCollisionData, InstanceSoultreeCollisionData); fold them in
// with the CollisionObject TU.
#ifndef SOULTREECOLLISIONINSTANCEDATA_SOULTREECOLLISIONINSTANCEDATA_H
#define SOULTREECOLLISIONINSTANCEDATA_SOULTREECOLLISIONINSTANCEDATA_H

#include "d3dtypes.h"

struct BoundingBoxTree;

struct SoultreeCollisionInstanceData {
    int m_isHierarchy;                  // +0x00
    int m_x04;                          // +0x04
    BoundingBoxTree *m_faceTree;        // +0x08
    int m_x0C;                          // +0x0C
    void *m_vertices;                   // +0x10
    char pad14[0x20 - 0x14];
    _D3DMATRIX m_matrix;                // +0x20
    int m_x60;                          // +0x60
    int m_refCount;                     // +0x64
    char pad68[0x70 - 0x68];
};

#endif
