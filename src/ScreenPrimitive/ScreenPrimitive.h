// ScreenPrimitive: a 2D polygon drawn over the 3D scene (radar blips, the
// split-screen bars, the standings rows). 0x50 bytes (new in its users);
// no vtable. Members from the ctor (the _D3DVECTOR default ctor runs at
// +0x20), Create (vertices, type, texture, render item and state block, the
// DMA buffers) and UpdateDmaBuffer (+0x14..+0x24 the transform).
#ifndef SCREENPRIMITIVE_SCREENPRIMITIVE_H
#define SCREENPRIMITIVE_SCREENPRIMITIVE_H

#include "d3dtypes.h"

class DmaRenderItem;
class PSXRenderStateBlock;
class PSXTextureMap;
class ScreenPrimitiveVertex;
class VifDataItem;
struct QWORD;
struct sceGifTag;

enum ScreenPrimitiveType {};

// One VIF packet of the primitive's vertices (CreateVifDataItem).
struct ScreenPrimitiveDmaBuffer {       // 0x14 bytes
    VifDataItem *m_item;                // +0x00
    unsigned int *m_unpack;             // +0x04
    sceGifTag *m_gifTag;                // +0x08
    QWORD *m_x0C;                       // +0x0C (UpdateDmaBuffer: the screen size)
    char pad10[0x14 - 0x10];
};

class ScreenPrimitive {
public:
    ScreenPrimitive();
    ~ScreenPrimitive();
    ScreenPrimitive *Create(ScreenPrimitiveVertex *vertices, int numVertices, ScreenPrimitiveType type,
                            char *textureName);
    void CreateVifDataItem(ScreenPrimitiveDmaBuffer *buffer, int count, int start);
    void UpdateDmaBuffer();
    void UpdateTextureRenderItem();

    static _D3DVECTOR LpPoint[];

    ScreenPrimitiveVertex *m_vertices;  // +0x00
    int m_numVertices;                  // +0x04
    ScreenPrimitiveType m_type;         // +0x08, 2 from the ctor
    int m_alphaTest;                    // +0x0C
    unsigned char m_alphaBlendMode;     // +0x10
    unsigned char m_alphaCompare;       // +0x11, 6 from the ctor
    unsigned char m_alphaRefValue;      // +0x12
    unsigned char m_alphaFailMethod;    // +0x13
    float m_scale;                      // +0x14
    float m_angle;                      // +0x18
    float m_z;                          // +0x1C
    _D3DVECTOR m_center;                // +0x20 (x, y)
    int m_zEnable;                      // +0x30
    PSXTextureMap *m_texture;           // +0x34
    int m_dirty;                        // +0x38
    DmaRenderItem *m_renderItem;        // +0x3C
    PSXRenderStateBlock *m_stateBlock;  // +0x40
    ScreenPrimitiveDmaBuffer *m_buffers; // +0x44
    int m_numBuffers;                   // +0x48
    char pad4C[0x50 - 0x4C];
};

#endif
