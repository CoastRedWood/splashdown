// ScreenQuad2D: a screen-space quad (a portal's projection), its corners
// clockwise after Orient. 0x40 bytes: SearchNode::PrepareGeometry's stack
// frame holds two of them at that size; nothing matched uses +0x20..+0x40.
#ifndef SCREENQUAD2D_SCREENQUAD2D_H
#define SCREENQUAD2D_SCREENQUAD2D_H

#include "ScreenPoint2D/ScreenPoint2D.h"
#include "d3dtypes.h"

class ScreenQuad2D {
public:
    void Set(ScreenPoint2D *points);
    void Set(_D3DVECTOR *points);
    void Set(ScreenQuad2D &other);
    void SetToMinMax(ScreenPoint2D *points, int count);
    void Orient();
    int IsConvex();
    int PointInQuad(ScreenPoint2D point);
    static int MinMaxIntersect(ScreenQuad2D &a, ScreenQuad2D &b, ScreenQuad2D &out);

    ScreenPoint2D m_points[4];          // +0x00
    char pad20[0x40 - 0x20];
};

#endif
