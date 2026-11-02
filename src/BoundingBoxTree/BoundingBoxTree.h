// BoundingBoxTree: one node of a collision tree (a plain struct, 0x30
// bytes; built by BuildFaceTree/BuildRayTree, freed by DeleteBoundingBoxTree).
// +0x00 is negative for a leaf; leaves reuse +0x04.. for vertex indices
// (global/CollisionTree.h's BoundingBoxTreeFace). +0x10/+0x20 are the box (some copies
// read them as center/half size, or a ray's start/end).
#ifndef BOUNDINGBOXTREE_BOUNDINGBOXTREE_H
#define BOUNDINGBOXTREE_BOUNDINGBOXTREE_H

#include "d3dtypes.h"

struct BoundingBoxTree {
    float m_size;                       // +0x00, negative for a leaf
    float m_radius;                     // +0x04
    BoundingBoxTree *m_left;            // +0x08
    BoundingBoxTree *m_right;           // +0x0C
    _D3DVECTOR m_min;                   // +0x10
    _D3DVECTOR m_max;                   // +0x20
};

#endif
