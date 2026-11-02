// ScreenIcon: a textured quad drawn by ScreenIconManager. It has a vtable of
// its own (only the dtor, the retail vtable ScreenIcon), so the vptr is at +0x0.
// Data from its own methods (src/ScreenIcon/) and ScreenIconManager's; the
// other names are the most common among the files that declared it locally
// (mwcc's layout ScreenIcon). The ctor runs the two _D3DVECTOR default
// ctors; `new ScreenIcon` allocates 0x90 bytes.
#ifndef SCREENICON_SCREENICON_H
#define SCREENICON_SCREENICON_H

#include "d3dtypes.h"

class PSXTextureMap;
struct QWORD;
struct VifDataItem;

class ScreenIcon {
public:
    ScreenIcon();
    ScreenIcon *Create(char *name, float x, float y);
    ScreenIcon *CreateClone();
    void Clone(ScreenIcon *other);
    void CreateVifDataItem(VifDataItem *item, int size, QWORD **out);
    virtual ~ScreenIcon();

    // Inline setters, as the files that use them declared them. Each marks
    // what ScreenIconManager::ComposeRenderItem has to redo.
    void SetPosition(float x, float y)
    {
        m_position.x = x;
        m_position.y = y;
        m_dirty = 1;
    }
    void SetX(float x)
    {
        m_position.x = x;
        m_dirty = 1;
    }
    void SetY(float y)
    {
        m_position.y = y;
        m_dirty = 1;
    }
    void SetPivot(float x, float y)
    {
        m_pivot.x = x;
        m_pivot.y = y;
        m_dirty = 1;
    }
    void SetWidth(float width)
    {
        m_width = width;
        m_dirty = 1;
    }
    void SetHeight(float height)
    {
        m_height = height;
        m_dirty = 1;
    }
    void SetAngle(float angle)
    {
        m_angle = angle;
        m_dirty = 1;
    }
    void SetDepth(float depth)
    {
        m_depth = depth;
        m_dirty = 1;
    }
    void SetColor(unsigned int color)
    {
        m_color = color;
        m_colorDirty = 1;
    }
    void SetAlpha(unsigned char alpha)
    {
        m_alphaFix = alpha;
        m_colorDirty = 1;
    }
    // Texel rectangle, scaled to 0..1 by the texture size.
    void SetUVRect(float u0, float u1, float v0, float v1)
    {
        m_u0 = u0 * m_invWidth;
        m_u1 = u1 * m_invWidth;
        m_v0 = v0 * m_invHeight;
        m_v1 = v1 * m_invHeight;
        m_uvDirty = 1;
    }

    // vptr at +0x00
    PSXTextureMap *m_texture;           // +0x04 (the render state takes a PSXTextureMap)
    char pad08[0x10 - 0x08];
    _D3DVECTOR m_position;              // +0x10
    _D3DVECTOR m_pivot;                 // +0x20 (m_pivotX/m_pivotY in the copies)
    unsigned int m_color;               // +0x30, -1 from the ctor
    float m_34;                         // +0x34, 1.0 from the ctor
    float m_width;                      // +0x38
    float m_height;                     // +0x3C
    float m_u0;                         // +0x40
    float m_u1;                         // +0x44
    float m_v0;                         // +0x48
    float m_v1;                         // +0x4C
    float m_angle;                      // +0x50
    float m_depth;                      // +0x54
    unsigned char m_alphaFix;           // +0x58, 0xFF from the ctor
    char pad59[0x5C - 0x59];
    float m_invWidth;                   // +0x5C, 1 / texture width
    float m_invHeight;                  // +0x60
    unsigned char m_alphaBlendMode;     // +0x64
    unsigned char m_alphaCompare;       // +0x65, 6 from the ctor
    unsigned char m_alphaRef;           // +0x66
    unsigned char m_alphaFail;          // +0x67
    int m_addressMode;                  // +0x68, 1 from the ctor
    unsigned char m_zBuffer : 1;        // +0x6C bit 0
    unsigned char m_alphaTest : 1;      //        bit 1
    unsigned char m_dirty : 1;          //        bit 2
    unsigned char m_colorDirty : 1;     //        bit 3
    unsigned char m_uvDirty : 1;        //        bit 4
    unsigned char m_isClone : 1;        //        bit 5, texture not owned
    VifDataItem *m_vifData;             // +0x70, memalign'd 0xC0 bytes
    QWORD *m_vifQwords[4];              // +0x74, CreateVifDataItem's out[0..3]
    char pad84[0x90 - 0x84];
};

#endif
