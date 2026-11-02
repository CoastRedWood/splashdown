// ScreenPrimitiveVertex: one vertex of a ScreenPrimitive, 0x30 bytes (the
// primitive's array stride). The ctor sets the position to (0, 0, 1, 1),
// m_alpha 0xFF, m_color -1 and clears +0x18..+0x28. ScreenPrimitive's
// CreateVifDataItem points +0x20/+0x24/+0x28 at the vertex's position, colour
// and UV slots in its VIF data (copies view them as QWORD pointers), and the
// inline setters write through them.
#ifndef SCREENPRIMITIVEVERTEX_SCREENPRIMITIVEVERTEX_H
#define SCREENPRIMITIVEVERTEX_SCREENPRIMITIVEVERTEX_H

#include "d3dtypes.h"

class ScreenPrimitiveVertex {
public:
    ScreenPrimitiveVertex();
    ~ScreenPrimitiveVertex();

    void SetPosition(float x, float y)
    {
        m_position.x = x;
        m_position.y = y;
        if (m_vifPosition)
            *m_vifPosition = m_position;
    }
    void UpdateColor()
    {
        // Declaration and statement order reproduce retail's registers.
        int blue;
        int *vif;
        unsigned int color;
        int alpha;
        int green;
        int red;
        color = m_color;
        red = ((color >> 16) & 0xFF) >> 1;
        alpha = m_alpha >> 2;
        vif = m_vifColor;
        green = ((color >> 8) & 0xFF) >> 1;
        blue = (color & 0xFF) >> 1;
        if (vif) {
            vif[0] = red;
            vif[1] = green;
            vif[2] = blue;
            vif[3] = alpha;
        }
    }
    void SetColor(unsigned int color)
    {
        m_color = color;
        UpdateColor();
    }
    void SetAlpha(unsigned char alpha)
    {
        m_alpha = alpha;
        UpdateColor();
    }

    _D3DVECTOR m_position;      // +0x00
    unsigned char m_alpha;      // +0x10
    unsigned int m_color;       // +0x14
    int m_x18;                  // +0x18
    int m_x1C;                  // +0x1C
    _D3DVECTOR *m_vifPosition;  // +0x20
    int *m_vifColor;            // +0x24
    PS2_PTR_INT m_x28;          // +0x28, the UV slot (a pointer: ScreenPrimitiveManager.cpp's view)
    int m_x2C;                  // +0x2C
};

#endif
