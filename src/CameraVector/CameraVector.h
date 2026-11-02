// CameraVector: the 4-float vector CameraStateParams is made of (no methods
// in retail; its default ctor is inline: x = y = z = 0, w = 1).
#ifndef CAMERAVECTOR_CAMERAVECTOR_H
#define CAMERAVECTOR_CAMERAVECTOR_H

struct CameraVector {
    float x, y, z, w;
    CameraVector() { x = y = z = 0; w = 1.0f; }
};

#endif
