// StripList: the triangle strips of one water/mesh surface (0xC bytes).
// Retail's StripMesh.cpp holds its methods after StripMesh's; WaterObject
// (WaterObjectData's strip groups) owns the lists.
#ifndef STRIPLIST_STRIPLIST_H
#define STRIPLIST_STRIPLIST_H

#include "StripMesh/StripMesh.h"

class StripList {
public:
    StripList();
    ~StripList();
    void ReadFromFile(long handle);

    StripMesh *m_strips;                // +0x00, malloc'd
    int m_numStrips;                    // +0x04
    int m_totalVertices;                // +0x08
};

#endif
