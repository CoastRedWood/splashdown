// VisibleTest: the view-frustum tests (the global VisTest). No data members
// or virtuals are known; the copies only declare the two methods.
#ifndef VISIBLETEST_VISIBLETEST_H
#define VISIBLETEST_VISIBLETEST_H

class Camera;
struct _D3DMATRIX;
struct _D3DVECTOR;

class VisibleTest {
public:
    int BoundingBoxVisible(_D3DMATRIX *m, _D3DVECTOR *center, _D3DVECTOR *half, unsigned int *clipCodes);
    int PointVisible(Camera *camera, _D3DMATRIX *m, _D3DVECTOR *point, _D3DVECTOR *screen, unsigned int *clipCodes);
};

#endif
