// SurfaceShaderInstance: one shader and the geometry queued for it this
// frame (0x1C bytes; the vptr comes after the data).
#ifndef SURFACESHADERINSTANCE_H
#define SURFACESHADERINSTANCE_H

class ShaderDescriptor;
class SurfaceShaderList;
class SurfaceGeometry;
struct _sceDmaTag;

// A queued geometry (a block of SurfaceShaderList::m_allocator).
struct SurfaceGeometryNode {
    SurfaceGeometryNode *next;          // +0x0
    SurfaceGeometry *geometry;          // +0x4
    _sceDmaTag *start;                  // +0x8 (PSXSurfaceShaderInstance::AddGeometry)
    _sceDmaTag *end;                    // +0xC
};

class SurfaceShaderInstance {
public:
    SurfaceShaderInstance();
    SurfaceGeometryNode *AddGeometry(SurfaceGeometry *geometry);  // returns the new node

    ShaderDescriptor *m_descriptor;     // +0x00
    SurfaceShaderList *m_list;          // +0x04 (rendered instead when there is no descriptor; the dtor deletes it)
    SurfaceGeometryNode *m_head;        // +0x08
    SurfaceGeometryNode *m_tail;        // +0x0C
    int m_frame;                        // +0x10 (the list's frame when m_head was started)
    SurfaceShaderInstance *m_next;      // +0x14 (an identical shader's instance, which gets the geometry)

    // --- virtuals in retail slot order ---
    virtual ~SurfaceShaderInstance();   // vptr +0x18
};

#endif
