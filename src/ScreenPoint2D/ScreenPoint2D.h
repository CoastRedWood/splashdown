// ScreenPoint2D: a 2D screen point (ScreenQuad2D's corners, the portal
// clipping in SearchNode). Both ctors are out of line in retail: the default
// one is empty, the other stores x and y.
#ifndef SCREENPOINT2D_SCREENPOINT2D_H
#define SCREENPOINT2D_SCREENPOINT2D_H

class ScreenPoint2D {
public:
    ScreenPoint2D();
    ScreenPoint2D(float x, float y);

    float x;                            // +0x0
    float y;                            // +0x4
};

#endif
