// VertexRayToFaceIntersectionData: a CollisionObject's last ray-to-face hit
// (CollisionObject::m_intersectionData). 0x40 bytes. Names as in the CollisionObject TU's
// former VU0 header.
#ifndef VERTEXRAYTOFACEINTERSECTIONDATA_VERTEXRAYTOFACEINTERSECTIONDATA_H
#define VERTEXRAYTOFACEINTERSECTIONDATA_VERTEXRAYTOFACEINTERSECTIONDATA_H

#include "d3dtypes.h"

struct VertexRayToFaceIntersectionData {
    _D3DVECTOR m_x00;                   // +0x00 (CollisionCharacter::Tick pushes back along it)
    _D3DVECTOR m_normal;                // +0x10
    _D3DVECTOR m_position;              // +0x20
    float m_x30;                        // +0x30
    int m_count;                        // +0x34, hits summed into m_position
    char pad38[0x40 - 0x38];
};

#endif
