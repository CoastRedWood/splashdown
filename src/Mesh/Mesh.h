// Mesh — abstract GameObject holding one SurfaceGeometry (layout from
// Mesh::Create, the ctor and the dtor).
#ifndef MESH_H
#define MESH_H

#include "GameObject/GameObject.h"

struct _D3DVECTOR;

#include "SurfaceGeometry/SurfaceGeometry.h"

#include "PSXSurfaceGeometry/PSXSurfaceGeometry.h"

class Mesh : public GameObject {
public:
    enum MeshLightingType {};

    Mesh(int);
    virtual ~Mesh();
    virtual Mesh *Create(char *name, unsigned char flags, int count, int *values,
                         MeshLightingType lighting, int color);
    // Pure in Mesh; PSXMesh implements them (retail vtable).
    virtual void SetXYZ(int strip, int vertex, _D3DVECTOR *xyz) = 0;
    virtual void SetNormal(int strip, int vertex, _D3DVECTOR *normal) = 0;
    virtual void SetTangent(int strip, int vertex, _D3DVECTOR *tangent) = 0;
    virtual void SetRGBA(int strip, int vertex, unsigned int rgba) = 0;
    virtual void SetUV(int strip, int vertex, float u, float v) = 0;
    virtual void LockVertices() = 0;
    virtual void UnlockVertices() = 0;
    virtual void SetStripLength(int strip, int length) = 0;
    virtual void SetStripCount(int count) = 0;

    SurfaceGeometry *m_geometry;        // +0x30
    int m_count;                        // +0x34
    int *m_values;                      // +0x38
    unsigned char m_flags;              // +0x3C
    MeshLightingType m_lighting;        // +0x40
    int m_color;                        // +0x44
    int m_x48;                          // +0x48
};

#endif  // MESH_H
