// SurfaceShaderList: every shader instance in use, drawn once per frame
// (0x4010 bytes; the vptr comes after the data).
#ifndef SURFACESHADERLIST_H
#define SURFACESHADERLIST_H

class BlockAllocator;
class SurfaceShaderInstance;
class ShaderDescriptor;

class SurfaceShaderList {
public:
    SurfaceShaderList();
    int AddShaderInstance(ShaderDescriptor *descriptor, int index);
    void Purge();
    void Render();

    int m_numShaders;                       // +0x0000
    int m_frame;                            // +0x0004 (1 after the ctor; Render bumps it)
    BlockAllocator *m_allocator;            // +0x0008 (0x10-byte SurfaceGeometryNodes)
    SurfaceShaderInstance *m_shaders[0x1000];  // +0x000C

    // --- virtuals in retail slot order ---
    virtual ~SurfaceShaderList();           // vptr +0x400C
};

#endif
