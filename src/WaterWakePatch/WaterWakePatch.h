// WaterWakePatch: one scrolling wake texture patch of a Water (Water's
// m_wakePatches array; a plain struct, 0x40 bytes). Its two vertex buffers
// are double-buffered (ApplyExternalForce's copy indexes them as [2] with
// +0x30 as the current one; GetGridPointIntersection calls them WakeTiles).
#ifndef WATERWAKEPATCH_WATERWAKEPATCH_H
#define WATERWAKEPATCH_WATERWAKEPATCH_H

#include "d3dtypes.h"

class PSXTextureMap;

// Eight wake grid points, 0x40 bytes (a patch is m_patchHeight of these per
// row; GetGridPointIntersection reads the heights, ApplyExternalForce adds
// to the forces).
struct WaterVertex {
    short height[8];                    // +0x00
    char pad10[0x30 - 0x10];
    short force[8];                     // +0x30
};

struct WaterWakePatch {
    float m_scrollScale;                // +0x00
    float m_invScrollScale;             // +0x04
    char pad08[0x10 - 0x08];
    _D3DVECTOR m_position;              // +0x10
    _D3DVECTOR m_scrollOrigin;          // +0x20
    int m_current;                      // +0x30
    PSXTextureMap *m_texture;           // +0x34
    WaterVertex *m_vertices[2];         // +0x38, double-buffered: [m_current] is the current one
};

#endif
