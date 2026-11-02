// StripMesh: one triangle strip of a StripList (StripList::ReadFromFile
// reads them, StripList/StripList.h). 0x18 bytes.
#ifndef STRIPMESH_STRIPMESH_H
#define STRIPMESH_STRIPMESH_H

#include "d3dtypes.h"

struct StripVertex {                // 0x20 bytes
    float x, y, z;                  // +0x00
    char padC[0x18 - 0xC];
    float u, v;                     // +0x18
};

class StripMesh {
public:
    void ComputeTangentVectors();

    int m_x00;
    void *m_buffer04;               // +0x04
    StripVertex *m_vertices;        // +0x08
    _D3DVECTOR *m_normals;          // +0x0C
    _D3DVECTOR *m_tangents;         // +0x10, calloc'd by ComputeTangentVectors
    int m_numVertices;              // +0x14
};

#endif
