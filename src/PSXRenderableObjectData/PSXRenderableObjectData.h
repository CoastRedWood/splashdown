// PSXRenderableObjectData: the PS2 RenderableObjectData. 0x2C bytes: the
// ctor clears +0x28, the per-part block ReadFromBinaryFile reads after the
// base's data. PSXRenderableObject reads that block as its hierarchy (per
// part, the number of objects and their indices), Create as triangle
// strips (per strip, the vertex count), the dtor as the part count.
#ifndef PSXRENDERABLEOBJECTDATA_PSXRENDERABLEOBJECTDATA_H
#define PSXRENDERABLEOBJECTDATA_PSXRENDERABLEOBJECTDATA_H

#include "RenderableObjectData/RenderableObjectData.h"

// Read with the object, 4-byte aligned in one malloc'd block.
struct RenderableStrings {
    int m_count;                        // +0x00, parts (strips)
    int *m_lengths;                     // +0x04, per part: objects (strip vertices)
    unsigned char **m_strings;          // +0x08, per part: the object indices
    char *m_data;                       // +0x0C
};

class PSXRenderableObjectData : public RenderableObjectData {
public:
    PSXRenderableObjectData();

    // --- virtuals in retail slot order ---
    virtual ~PSXRenderableObjectData();
    virtual void ReadFromBinaryFile(long file, InstanceMaster *master);

    RenderableStrings *m_strings;       // +0x28
};

#endif
