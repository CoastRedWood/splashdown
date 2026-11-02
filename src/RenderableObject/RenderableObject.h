// RenderableObject on top of a copy of SoultreeObject/SoultreeObject.h that
// also names the +0x191 bitfield (RenderableObject's visible flag).
#ifndef RENDERABLEOBJECT_RENDERABLEOBJECT_H
#define RENDERABLEOBJECT_RENDERABLEOBJECT_H

#include "GameObject/GameObject.h"

extern "C" int strcasecmp(const char *a, const char *b);

#include "d3dtypes.h"

struct Quaternion;

_D3DMATRIX IdentityMatrix();
class SearchableObject;

// SoultreeObject's first base: 8 bytes in front of its GameObject subobject.
#include "SearchableObject/SearchableObject.h"

#include "SearchPlacement/SearchPlacement.h"

#include "SoultreeObject/SoultreeObject.h"

#include "RenderableObjectData/RenderableObjectData.h"

// RenderableObject: a SoultreeObject with renderable data. 0x1C0 bytes; its
// own data starts at +0x1B0 (SoultreeObject is 0x1AC bytes but 8-aligned).
// Names follow its own methods (src/RenderableObject/). +0x1B1 is a byte the
// ctor clears; its bits (m_x1B1Bits) are mostly PSXRenderableObject's. Create returns the object as a GameObject (this + 8).
class RenderableObject : public SoultreeObject {
public:
    RenderableObject(int id);
    void UpdateLOD();
    int ChangeMaterial(int from, int to);
    static void SetRenderableLODBias(float geometry, float texture);

    // --- virtuals in retail slot order ---
    virtual ~RenderableObject();
    virtual GameObject *Create(char *name, int a, int flags);
    virtual int PrepareGeometry();
    virtual int ChangeFillColor(int, int);
    virtual int BoundingBoxVisible();

    unsigned char m_lod;                // +0x1B0 (UpdateLOD)
    union {
        char m_x1B1;                    // +0x1B1
        struct {
            unsigned char m_boxVisible : 1;     // bit 0: PSXRenderableObject::BoundingBoxVisible's result
            unsigned char m_lit : 1;            // bit 1: Create's flags bit 0; Render3D copies two light blocks
            unsigned char m_fogged : 1;         // bit 2: reaches past the fog start
            unsigned char m_clip : 1;           // bit 3: the box crosses a clip plane
            unsigned char m_instanced : 1;      // bit 4: instanced geometry (.pgi/.dgi)
            unsigned char m_insideGuardBand : 1;    // bit 5
        } m_x1B1Bits;
    };
    RenderableObjectData *m_data;       // +0x1B4
    SoultreeObject **m_objects;         // +0x1B8, the hierarchy's objects, new[]'d by Create
    float m_distance;                   // +0x1BC (UpdateLOD)
};

#endif
