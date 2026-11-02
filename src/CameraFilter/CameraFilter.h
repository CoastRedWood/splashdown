// CameraFilter: a smoothed camera value (no vtable, no out-of-line methods;
// 0x14 bytes, `new`ed by the camera behaviours' Initialize). The ctor is
// inline in retail: every Initialize stores +0x00, +0x04, +0x0C (FLT_MAX),
// +0x10 (-FLT_MAX), +0x08 (1.0) in that order. Some copies call it Filter,
// and some swap the names of +0x0C/+0x10 (min/max); +0x0C starts at FLT_MAX.
// Update (inline, uses a min.s asm helper) stays with the files that use it.
#ifndef CAMERAFILTER_CAMERAFILTER_H
#define CAMERAFILTER_CAMERAFILTER_H

class CameraFilter {
public:
    CameraFilter(float value, float rate)
    {
        m_value = value;
        m_rate = rate;
        m_max = 3.40282347e+38f;
        m_min = -3.40282347e+38f;
        m_scale = 1.0f;
    }

    float m_value;                      // +0x00
    float m_rate;                       // +0x04
    float m_scale;                      // +0x08
    float m_max;                        // +0x0C
    float m_min;                        // +0x10
};

#endif
