// PSXRenderableObject: the PS2 RenderableObject (SoultreeObject's GameObject
// subobject at +8). 0x1D0 bytes (`new` in JetskiVehicle::Create and many
// others); its own data starts at RenderableObject's 0x1C0. +0x1C0 is the
// per-part packet buffers the dtor frees; +0x1C4..+0x1CF are never named.
#ifndef PSXRENDERABLEOBJECT_PSXRENDERABLEOBJECT_H
#define PSXRENDERABLEOBJECT_PSXRENDERABLEOBJECT_H

#include "RenderableObject/RenderableObject.h"

class PSXRenderableObject : public RenderableObject {
public:
    PSXRenderableObject(int id);
    static void SetMultipassCullDistance(float distance);
    // Render3D's inlined helper (src/tu/PSXRenderableObject.cpp).
    inline char *CopyObjectMatrices(unsigned char *indices, int count, char *cursor);

    // --- virtuals in retail slot order ---
    virtual ~PSXRenderableObject();
    virtual GameObject *Create(char *name, int a, int flags);
    virtual int ChangeFillColor(int, int);
    virtual int BoundingBoxVisible();
    virtual int Render3D();

    void **m_partBuffers;               // +0x1C0
    char pad1C4[0x1D0 - 0x1C4];
};

#endif
