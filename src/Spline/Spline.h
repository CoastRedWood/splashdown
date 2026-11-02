// Spline: a CarProcedural's path (CarProcedural::m_spline, loaded from a
// .vue file by LoadVueFile; BoatProcedural drives along it too). 0x18
// bytes; its knots are 0x20 bytes each.
#ifndef SPLINE_SPLINE_H
#define SPLINE_SPLINE_H

#include "d3dtypes.h"

struct SplineKnot {
    float m_time;                       // +0x00
    float m_length;                     // +0x04, arc length to the next knot (ComputeSplineLengths)
    char pad08[0x10 - 0x08];
    _D3DVECTOR m_position;              // +0x10
};

class Spline {
public:
    unsigned int m_count;               // +0x00
    int m_refCount;                     // +0x04
    SplineKnot *m_knots;                // +0x08
    float m_time;                       // +0x0C (ResetTime)
    float m_height;                     // +0x10, ground/water height under the vehicle
    float m_rate;                       // +0x14, 1 / dt
};

#endif
