// RenderableObjectData: the shared, ref-counted data behind a
// RenderableObject (a BaseObject; RenderableObject::m_data). 0x28 bytes:
// the ctor stores +0x10..+0x24 (+0x14 = 1.0f). PSXRenderableObjectData
// derives from it and adds the mesh at +0x28 (copies of "RenderableObjectData"
// that declare +0x28 are that subclass).
#ifndef RENDERABLEOBJECTDATA_RENDERABLEOBJECTDATA_H
#define RENDERABLEOBJECTDATA_RENDERABLEOBJECTDATA_H

#include "GameObject/GameObject.h"

class InstanceMaster;
class PSXSurfaceGeometry;
class SurfaceShaderInstance;
class RenderableObject;

// One level of detail (m_lods, m_numLODs of them). 0x0C bytes.
struct RenderableObjectLOD {
    int m_numGeometries;                // +0x00
    float m_size;                       // +0x04, the level's size (UpdateLOD picks a level by it)
    PSXSurfaceGeometry *m_geometries;   // +0x08, new[]'d
};

class RenderableObjectData : public BaseObject {
public:
    RenderableObjectData();
    int ChangeMaterial(int oldMaterial, int newMaterial);

    // --- virtuals in retail slot order ---
    virtual ~RenderableObjectData();
    virtual void ReadFromBinaryFile(long file, InstanceMaster *master);

    int m_numLODs;                      // +0x10
    float m_lodScale;                   // +0x14, 1.0 from the ctor; scales the levels' sizes
    RenderableObjectLOD *m_lods;        // +0x18, malloc'd
    RenderableObject *m_owner;          // +0x1C, the object that loaded it (RenderableObject::Create)
    SurfaceShaderInstance *m_shader;    // +0x20, the one shader of a single-material object (PSXRenderableObject::Render3D)
    int m_shaderChecked;                // +0x24, m_shader looked for
};

#endif
