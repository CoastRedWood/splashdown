// SoultreeHierarchyCollisionObject: the collision data of a SoultreeObject
// hierarchy, one SoultreeCollisionObject per node (a plain struct: no vtable;
// CollisionObject reads, instances and tests it). Data to +0x144, 0x150 in
// the copies that pad it. Names are the most common among the copies.
#ifndef SOULTREEHIERARCHYCOLLISIONOBJECT_SOULTREEHIERARCHYCOLLISIONOBJECT_H
#define SOULTREEHIERARCHYCOLLISIONOBJECT_SOULTREEHIERARCHYCOLLISIONOBJECT_H

#include "d3dtypes.h"

class SoultreeObject;
struct SoultreeCollisionObject;
struct SoultreeHierarchyCollisionInstanceData;

struct SoultreeHierarchyCollisionObject {
    int m_count;                        // +0x000
    int *m_x04;                         // +0x004
    int *m_x08;                         // +0x008
    int m_x0C;                          // +0x00C
    SoultreeObject *m_object;           // +0x010
    SoultreeCollisionObject *m_objects; // +0x014, m_count of them
    char pad18[0x20 - 0x18];
    _D3DVECTOR m_center;                // +0x020
    _D3DVECTOR m_half;                  // +0x030
    _D3DVECTOR m_x40;                   // +0x040
    _D3DVECTOR m_x50;                   // +0x050
    _D3DMATRIX m_prevMatrix;            // +0x060
    _D3DMATRIX m_matrix;                // +0x0A0
    _D3DMATRIX m_xE0;                   // +0x0E0
    _D3DVECTOR m_sweptCenter;           // +0x120
    _D3DVECTOR m_sweptHalf;             // +0x130
    SoultreeHierarchyCollisionInstanceData *m_instanceData; // +0x140
    char pad144[0x150 - 0x144];
};

#endif
