// VisibilityQuadTree: the QuadTree that collects visible objects for
// rendering. 0x8C0 bytes (`new VisibilityQuadTree`); its own data starts at
// QuadTree's 0x8B8 (SetMaxSizeOfSearchList stores +0x8B8).
#ifndef VISIBILITYQUADTREE_VISIBILITYQUADTREE_H
#define VISIBILITYQUADTREE_VISIBILITYQUADTREE_H

#include "QuadTree/QuadTree.h"

class Camera;

class VisibilityQuadTree : public QuadTree {
public:
    VisibilityQuadTree();
    VisibilityQuadTree *Create(float a, float b, float c, float d, float e, float f, float g);
    void SetMaxSizeOfSearchList(int size);
    int TraverseAsVisibleBSP(float x, float z, Camera *camera, int *count, SearchableObject ***list,
                             SearchableObject **buffer);

    // --- virtuals in retail slot order ---
    virtual ~VisibilityQuadTree();
    virtual void PerformPrepareGeometry();
    virtual void PerformRender3D();
    virtual QuadTreeNode *CreateNode();

    int m_maxSizeOfSearchList;          // +0x8B8
    char pad8BC[0x8C0 - 0x8BC];
};

#endif
