// PSXMesh: the PS2 Mesh (vertex strips built into a DMA render item).
// 0x68 bytes (`new PSXMesh`); its own data starts at Mesh's 0x50 (the ctor
// zeroes +0x50..+0x60). +0x50 is the transform packet (0x130 bytes), typed
// PSXMeshPacket (a QWORD array, defined in src/tu/PSXMesh.cpp): SetLightInfo
// matches only through a typed member, not a cast of a void *. BuildTransform-
// RenderItem and Render3D view it through casts. +0x60 is the per-strip
// maximum lengths (Create calls it m_stripLengths, the dtor m_indices).
#ifndef PSXMESH_PSXMESH_H
#define PSXMESH_PSXMESH_H

#include "Mesh/Mesh.h"

class DmaRenderItem;
struct PSXMeshPacket;
struct QWORD;
struct StripOffset;

class PSXMesh : public Mesh {
public:
    PSXMesh(int enabled);
    void BuildRenderList(DmaRenderItem *item, int count, int *lengths, StripOffset *offsets);
    void BuildTransformRenderItem();
    void SetLightInfo(QWORD *directions, QWORD *colors, QWORD *ambient);

    // --- virtuals in retail slot order ---
    virtual ~PSXMesh();
    virtual int Render3D();
    virtual Mesh *Create(char *name, unsigned char flags, int count, int *values, MeshLightingType lighting,
                         int color);
    virtual void SetXYZ(int strip, int vertex, _D3DVECTOR *xyz);
    virtual void SetNormal(int strip, int vertex, _D3DVECTOR *normal);
    virtual void SetTangent(int strip, int vertex, _D3DVECTOR *tangent);
    virtual void SetRGBA(int strip, int vertex, unsigned int rgba);
    virtual void SetUV(int strip, int vertex, float u, float v);
    virtual void LockVertices();
    virtual void UnlockVertices();
    virtual void SetStripLength(int strip, int length);
    virtual void SetStripCount(int count);

    PSXMeshPacket *m_vertexData;        // +0x50, memalign'd transform packet
    DmaRenderItem *m_renderItem;        // +0x54
    StripOffset *m_stripOffsets;        // +0x58
    int m_maxStrips;                    // +0x5C
    int *m_maxStripLengths;             // +0x60
    int m_x64;                          // +0x64
};

#endif
