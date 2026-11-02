#ifndef PSXSURFACEGEOMETRY_H
#define PSXSURFACEGEOMETRY_H

#include "SurfaceGeometry/SurfaceGeometry.h"

struct _sceDmaTag;
class DmaRenderItem;
class InstanceMaster;

// PS2 surface geometry: a DMA packet of strip data (0x24 bytes).
class PSXSurfaceGeometry : public SurfaceGeometry {
public:
    PSXSurfaceGeometry();
    int ReadFromFile(long file, InstanceMaster *master, int instance);
    void SetupInstanceTags(int *stripStarts, int end, InstanceMaster *master, int instance);
    void AddToRenderQueue(_sceDmaTag *start, _sceDmaTag *end, int flags);
    void UpdateLOD(float textureScale, float geometryScale);

    // --- virtuals in retail slot order ---
    virtual ~PSXSurfaceGeometry();
    virtual int Create(char *name, unsigned char flags);
    virtual int ChangeMaterial(int oldMaterial, int newMaterial);

    unsigned char m_flags;              // +0x08 (bit 0: m_xC follows in the file, bit 1: m_x10)
    unsigned char m_numLods;            // +0x09
    unsigned char m_xA;                 // +0x0A (0xFF)
    union {                             // the ctor clears the byte, then its bit 0 again
        unsigned char m_xB;             // +0x0B
        struct {
            unsigned char bit0 : 1;     // m_packet was memalign'd (the dtor frees it)
        } m_xBbits;
    };
    float m_xC;                         // +0x0C
    float m_x10;                        // +0x10
    float m_lods[2];                    // +0x14
    union {
        _sceDmaTag *m_packet;           // +0x1C
        DmaRenderItem *m_renderItem;    // +0x1C, EcoSystem's billboard geometry: its render item
    };
    _sceDmaTag *m_instanceTags;         // +0x20 (new[]'d)
};

#endif
