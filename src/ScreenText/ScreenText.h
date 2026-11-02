// ScreenText: one string drawn by ScreenTextManager, with its own VIF packets
// (one per run of up to 20 characters). No vtable. Data from its own methods
// (src/ScreenText/) and ScreenTextManager's; the other names are the most
// common among the files that declared it locally (mwcc's layout
// ScreenText). The ctor runs the three _D3DVECTOR default ctors, then sets
// x and y; `new ScreenText` allocates 0x80 bytes.
#ifndef SCREENTEXT_SCREENTEXT_H
#define SCREENTEXT_SCREENTEXT_H

#include "d3dtypes.h"

class ScreenTextFont;
struct ScreenTextVifDataItemOffset;
struct VifDataItem;

extern "C" char *strncpy(char *dst, const char *src, unsigned int n);

class ScreenText {
public:
    ScreenText(int maxChars, char *string);
    ~ScreenText();
    void Create(int maxChars, char *string);
    void Clone(ScreenText *other);
    void AllocateMemory();
    void ReleaseMemory();
    void CreateVifDataItem(VifDataItem *item, int count, int size, ScreenTextVifDataItemOffset *offsets);

    // Inline setters, as the files that use them declared them. Each marks
    // what ScreenTextManager has to redo (AddText, RenderQueuedText).
    void SetString(char *s)
    {
        m_positionDirty = 1;
        strncpy(m_text, s, m_maxChars);
        m_text[m_maxChars] = 0;
        m_x5D_bit4 = 1;
        m_dirty = 1;
        m_cellsDirty = 1;
    }
    void SetText(char *s)
    {
        m_positionDirty = 1;
        strncpy(m_text, s, m_maxChars);
        m_text[m_maxChars] = 0;
        m_x5D_bit4 = 1;
        m_dirty = 1;
        m_cellsDirty = 1;
    }
    void SetColor(unsigned int color)
    {
        unsigned int c = color; // the copy moves the setter temporaries one register up, as retail
        m_color = c;
        m_x5D_bit4 = 1;
    }
    void SetAlpha(unsigned char alpha)
    {
        m_alphaFix = alpha;
        m_x5D_bit4 = 1;
    }
    void SetPosition(float x, float y)
    {
        m_position.x = x;
        m_position.y = y;
        m_positionDirty = 1;
        m_dirty = 1;
    }
    // TextOverlay's names: these set +0x44 and +0x48, not the position.
    void SetX(float x)
    {
        m_scale = x;
        m_dirty = 1;
        m_positionDirty = 1;
    }
    void SetY(float y)
    {
        m_x48 = y;
        m_dirty = 1;
    }
    _D3DVECTOR GetPosition() { return m_position; }
    void SetScale(float scale)
    {
        m_scale = scale;
        m_dirty = 1;
        m_positionDirty = 1;
    }
    void SetFont(ScreenTextFont *font)
    {
        m_font = font;
        m_positionDirty = 1;
        m_dirty = 1;
        m_cellsDirty = 1;
    }
    void SetJustification(const int &justification) // retail takes it by reference: the reference fixes the text/value register order
    {
        m_justification = justification;
        m_positionDirty = 1;
    }

    char *m_text;                       // +0x00, m_maxChars + 1 bytes
    char pad04[0x10 - 0x04];
    _D3DVECTOR m_position;              // +0x10
    _D3DVECTOR m_x20;                   // +0x20, screen position (PositionText)
    _D3DVECTOR m_x30;                   // +0x30
    unsigned int m_color;               // +0x40
    float m_scale;                      // +0x44
    float m_x48;                        // +0x48
    float m_x4C;                        // +0x4C, 1.0 from the ctor
    ScreenTextFont *m_font;             // +0x50
    int m_justification;                // +0x54
    unsigned char m_alphaFix;           // +0x58, 0xFF from the ctor
    unsigned char m_alphaRef;           // +0x59
    unsigned char m_alphaCompare;       // +0x5A, 6 from the ctor
    unsigned char m_alphaFail;          // +0x5B
    unsigned char m_alphaBlendMode;     // +0x5C
    unsigned char m_alphaTest : 1;      // +0x5D bit 0
    unsigned char m_zBuffer : 1;        //        bit 1
    unsigned char m_modulate : 1;       //        bit 2
    unsigned char m_dirty : 1;          //        bit 3
    unsigned char m_x5D_bit4 : 1;       //        bit 4, color changed (SetColor)
    unsigned char m_cellsDirty : 1;     //        bit 5
    unsigned char m_positionDirty : 1;  //        bit 6
    unsigned char m_maxChars;           // +0x5E
    unsigned char m_numLines;           // +0x5F
    unsigned char m_x60;                // +0x60
    char pad61[0x64 - 0x61];
    char *m_cells;                      // +0x64, the font's (0xBADBEEF from the ctor)
    int m_x68;                          // +0x68
    VifDataItem **m_lines;              // +0x6C
    ScreenTextVifDataItemOffset *m_lineOffsets; // +0x70, one per line
    int m_x74;                          // +0x74
    char pad78[0x80 - 0x78];
};

#endif
