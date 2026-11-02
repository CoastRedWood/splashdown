// PatchCacheItem: one cached terrain grid cell for
// Terrain::GetGridPointIntersection (Terrain.cpp's static PatchCache[32]),
// 0x80 bytes: the cell's corner heights, normals and materials.
#ifndef PATCHCACHEITEM_PATCHCACHEITEM_H
#define PATCHCACHEITEM_PATCHCACHEITEM_H

#include "d3dtypes.h"

class PatchCacheItem {
public:
    PatchCacheItem();

    int m_key;                          // +0x00, x << 16 | z; -1 unused
    char pad04[0xC - 0x4];
    float m_heights[4];                 // +0x0C
    char pad1C[0x20 - 0x1C];
    _D3DVECTOR m_normals[4];            // +0x20
    unsigned char m_materials[4];       // +0x60
    int m_x;                            // +0x64
    int m_z;                            // +0x68
    float m_fx;                         // +0x6C
    float m_fz;                         // +0x70
    char pad74[0x80 - 0x74];
};

#endif
