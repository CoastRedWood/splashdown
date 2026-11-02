// SoultreeHierarchyCollisionInstanceData: the data shared by the instances
// of one soultree hierarchy's collision object (its m_instanceData),
// ref-counted. 0x28 bytes. CollisionObject's per-function files still
// keep local views; fold them in with the CollisionObject TU.
#ifndef SOULTREEHIERARCHYCOLLISIONINSTANCEDATA_SOULTREEHIERARCHYCOLLISIONINSTANCEDATA_H
#define SOULTREEHIERARCHYCOLLISIONINSTANCEDATA_SOULTREEHIERARCHYCOLLISIONINSTANCEDATA_H

#include "d3dtypes.h"

struct SoultreeHierarchyCollisionInstanceData {
    int m_x00;                          // +0x00
    int m_count;                        // +0x04
    int *m_x08;                         // +0x08
    int *m_x0C;                         // +0x0C
    int m_x10;                          // +0x10
    int *m_x14;                         // +0x14
    _D3DMATRIX *m_matrices;             // +0x18
    int *m_objectIndices;               // +0x1C
    int m_x20;                          // +0x20
    int m_refCount;                     // +0x24
};

#endif
