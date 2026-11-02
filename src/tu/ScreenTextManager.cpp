// ScreenTextManager.cpp  @ 0x0036CC00 (text 0x36CC00-0x36F0D0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's ScreenTextManager.cpp as one translation unit: ScreenText's
// methods, then ScreenTextManager's, then the static initializer. `make link`
// places the object's data as well as its code (tools/link_c.py). The old
// files' views map onto the shared headers: the font's "cells" are
// ScreenTextFont::m_glyphs, its "info" scales the texture's m_width/m_height;
// the render-state test compares m_font and the alpha/z/modulate fields.



#include "ScreenText/ScreenText.h"
#include "soultree_assert_block.h"
#include "ScreenTextManager/ScreenTextManager.h"
#include "sceGifTag/sceGifTag.h"
#include "GameObject/GameObject.h"
#include "DmaRenderItem/DmaRenderItem.h"
#include "PSXTextureMapManager/PSXTextureMapManager.h"
#include "PSXRenderStateBlock/PSXRenderStateBlock.h"
#include "ScreenTextFont/ScreenTextFont.h"
#include "TextureMap/TextureMap.h"
#include "PSXTextureMap/PSXTextureMap.h"
#include "RenderTarget/RenderTarget.h"
#include "PSXRenderTarget/PSXRenderTarget.h"
#include "Camera/Camera.h"
#include "PSXVideoCard/PSXVideoCard.h"
// The axis vectors every Soultree TU defines (and __sinit constructs).
#include "sinit/AxisVectors.h"

typedef int int128_t __attribute__((mode(TI)));

extern "C" void *memalign(unsigned int alignment, unsigned int size);
extern "C" void *memset(void *dst, int c, unsigned int n);
extern "C" void free(void *);
extern "C" char *strncpy(char *dst, const char *src, unsigned int n);
extern "C" unsigned int strlen(const char *s);
void PSXprintf(const char *fmt, ...);
void FastSinCos(float x, float *sine, float *cosine);

extern RenderTarget *LpRenderTarget;
extern PSXVideoCard *LpVideoCard;
extern PSXTextureMapManager *LpTextureMapManager;
// VU1 microcode packet for 2D quads (also used by ScreenIconManager::RenderQueuedIcons).
extern unsigned int Quad2DRendererCode[];
int FontReuseFlag = 1;

// A text VIF packet (ScreenText::CreateVifDataItem): a DMA tag, VIF codes,
// the GIF tag, then 7 quadwords per character and an MSCNT.
struct VifDataItem {
    short m_numQwords;                  // +0x00
    unsigned char m_02;                 // +0x02
    unsigned char m_03;                 // +0x03
    PS2_PTR(void) m_data;                       // +0x04
    int m_08;                           // +0x08
    int m_0C;                           // +0x0C
    unsigned int m_code[8];             // +0x10, VIF codes
    sceGifTag m_gifTag;                 // +0x30
    unsigned int m_glyphs[1];           // +0x40
};

// Where each part of a packet lives, for later patching.
struct ScreenTextVifDataItemOffset {
    void *m_a;
    void *m_b;
    void *m_c;
};

// A line's offsets as one block (AllocateMemory news one per line).
struct ScreenTextLineBlock {
    PS2_PTR_INT m_offsets[63];
};

// One line of up to 20 characters: the header's offsets, then each character's.
struct ScreenTextLineOffsets {
    ScreenTextVifDataItemOffset m_header;
    ScreenTextVifDataItemOffset m_chars[20];
};

// The start of a line's packet that m_header.m_a points at.
struct ScreenTextPacketHeader {
    unsigned int m_unpack[4];
    unsigned short m_nloop : 15;        // the GIF tag's NLOOP
    unsigned short m_eop : 1;
    char pad12[0x10 - 0x2];
    float m_size[4];                    // screen width, height
};

struct TextColor {
    int r, g, b, a;
};

// Writes a screen-size qword (width, height, 0, 0).
static inline void SetScreenSize(float *q, float width, float height)
{
    q[0] = width;
    q[1] = height;
    q[2] = q[3] = 0.0f;
}

// Writes a character's two texture-coordinate qwords (u, v, 1, 0).
static inline void SetUVs(float *uv, _D3DVECTOR *v)
{
    uv[0] = v->x;
    uv[1] = v->y;
    uv[2] = 1.0f;
    uv[3] = 0.0f;
    uv += 4;
    uv[0] = v->z;
    uv[1] = v->w;
    uv[2] = 1.0f;
    uv[3] = 0.0f;
}

// ScreenTextFontManager.h's inline glyph lookup (an inline member, so its
// assertion flag is a $localstatic shared across TUs; the copy retail kept
// is an earlier TU's).
inline ScreenTextGlyph *ScreenTextFont::GetCell(int ascii) const
{
#line 123 "ScreenTextFontManager.h"
    ASSERTF(ascii >= 0 && ascii < 256, ("Value out of range : ascii=%d\n", ascii));
    return m_glyphs[ascii];
}

// One VIF packet per run of up to 20 characters.
void ScreenText::AllocateMemory()
{
    m_text = new char[m_maxChars + 1];
    if (m_numLines) {
        m_lines = new VifDataItem *[m_numLines];
        m_lineOffsets = (ScreenTextVifDataItemOffset *)new ScreenTextLineBlock[m_numLines];
        int remaining = m_maxChars;
        int count;
        for (int i = 0; i < m_numLines; i++) {
            if (remaining / 20 == 0)
                count = remaining;
            else
                count = 20;
            int size = (count * 7 + 5) * 16 + 16;
            m_lines[i] = (VifDataItem *)memalign(0x40, size);
            memset(m_lines[i], 0, size);
            CreateVifDataItem(m_lines[i], count, size, (ScreenTextVifDataItemOffset *)&((ScreenTextLineBlock *)m_lineOffsets)[i]);
            remaining -= count;
        }
    }
    m_x74 = 0;
}


void ScreenText::ReleaseMemory()
{
    if (m_text) {
        delete[] m_text;
        m_text = 0;
    }
    if (m_lines) {
        for (int i = 0; i < m_numLines; i++) {
            if (m_lines[i]) {
                free(m_lines[i]);
                m_lines[i] = 0;
            }
        }
        if (m_lines) {      // retail re-tests the array after the loop
            delete[] m_lines;
            m_lines = 0;
        }
    }
    if (m_lineOffsets) {
        delete[] m_lineOffsets;
        m_lineOffsets = 0;
    }
}

#pragma alignlabel off

void ScreenText::Create(int maxChars, char *string)
{
    ReleaseMemory();
#line 93 "ScreenTextManager.cpp"
    ASSERT(maxChars < 101, "Exceeded maximum number of characters allowed in a ScreenText object");
    m_maxChars = maxChars;
    // One VIF packet per run of up to 20 characters.
    if (m_maxChars % 20)
        m_numLines = m_maxChars / 20 + 1;
    else
        m_numLines = m_maxChars / 20;
    m_x60 = m_numLines;
    m_text = 0;
    m_lines = 0;
    m_lineOffsets = 0;
    AllocateMemory();
    if (string) {
        strncpy(m_text, string, m_maxChars);
        m_text[m_maxChars] = 0;
    } else {
        m_text[0] = 0;
    }
    m_x20.x = 0.0f;
    m_x20.y = 0.0f;
    m_position.x = 0.0f;
    m_position.y = 0.0f;
    m_color = 0xFFFFFFFF;
    m_alphaFix = 0xFF;
    m_alphaTest = 0;
    m_alphaBlendMode = 0;
    m_alphaCompare = 6;
    m_alphaRef = 0;
    m_alphaFail = 0;
    m_scale = 1.0f;
    m_x48 = 0.0f;
    m_x30.x = 0.0f;
    m_x30.y = 0.0f;
    m_x4C = 1.0f;
    m_zBuffer = 0;
    m_modulate = 1;
    m_font = 0;
    m_justification = ScreenTextManager::LeftJustification;
    m_dirty = 1;
    m_x5D_bit4 = 1;
    m_cellsDirty = 1;
    m_positionDirty = 1;
    m_x68 = 0;
}

ScreenText::ScreenText(int maxChars, char *string)
{
#line 150 "ScreenTextManager.cpp"
    ASSERT(maxChars < 101, "Exceeded maximum number of characters allowed in a ScreenText object");
    m_maxChars = maxChars;
    // One VIF packet per run of up to 20 characters.
    if (m_maxChars % 20)
        m_numLines = m_maxChars / 20 + 1;
    else
        m_numLines = m_maxChars / 20;
    m_x60 = m_numLines;
    m_text = 0;
    m_lines = 0;
    m_lineOffsets = 0;
    AllocateMemory();
    if (string) {
        strncpy(m_text, string, m_maxChars);
        m_text[m_maxChars] = 0;
    } else {
        m_text[0] = 0;
    }
    m_x20.x = 0.0f;
    m_x20.y = 0.0f;
    m_position.x = 0.0f;
    m_position.y = 0.0f;
    m_color = 0xFFFFFFFF;
    m_alphaFix = 0xFF;
    m_alphaTest = 0;
    m_alphaBlendMode = 0;
    m_alphaCompare = 6;
    m_alphaRef = 0;
    m_alphaFail = 0;
    m_scale = 1.0f;
    m_x48 = 0.0f;
    m_x30.x = 0.0f;
    m_x30.y = 0.0f;
    m_x4C = 1.0f;
    m_zBuffer = 0;
    m_modulate = 1;
    m_font = 0;
    m_justification = ScreenTextManager::LeftJustification;
    m_dirty = 1;
    m_x5D_bit4 = 1;
    m_cellsDirty = 1;
    m_positionDirty = 1;
    m_x68 = 0;
    m_cells = (char *)0xBADBEEF;
}

#pragma alignlabel reset

// Builds a text VIF packet: an UNPACK of a GIF tag (sprites, 3 registers per
// loop) and 7 quadwords per character, then MSCNT. `offsets` receives where each
// part landed.
void ScreenText::CreateVifDataItem(VifDataItem *item, int count, int a, ScreenTextVifDataItemOffset *offsets)
{
    unsigned int *code = item->m_code;
    code[0] = 0;
    code[1] = 0;
    code[2] = 0;
    code[3] = 0;
    offsets[0].m_a = &code[4];
    code[4] = 0;
    code[5] = 0;
    code[6] = 0;
    code[7] = (count * 7 + 2) << 16 | 0x6C008000;  // UNPACK
    *(int128_t *)&item->m_gifTag = 0;
    item->m_gifTag.EOP = 1;
    item->m_gifTag.PRE = 1;
    item->m_gifTag.PRIM = 0x5C;
    item->m_gifTag.FLG = 0;
    item->m_gifTag.NREG = 3;
    item->m_gifTag.REGS0 = 2;
    item->m_gifTag.REGS1 = 1;
    item->m_gifTag.REGS2 = 5;
    item->m_gifTag.NLOOP = count * 4;
    offsets[0].m_b = &code[8];
    offsets[0].m_c = &code[12];
    unsigned int *p = &code[16];
    for (int i = 0; i < count; i++) {
        offsets[i + 1].m_a = p;
        offsets[i + 1].m_b = p + 4;
        offsets[i + 1].m_c = p + 20;
        p += 28;
    }
    p[0] = 0x14000019;  // MSCNT
    p[1] = 0;
    p[2] = 0;
    p[3] = 0;
    item->m_numQwords = count * 7 + 5;
    item->m_02 = 0;
    item->m_03 = 0x30;
    item->m_data = item->m_code;
    item->m_08 = 0;
    item->m_0C = 0;
}


ScreenText::~ScreenText()
{
    ReleaseMemory();
}

void ScreenText::Clone(ScreenText *other)
{
    static int timerID = -1;  // profiler ID, unused
    if (this == other)
        return;
    if (m_maxChars < other->m_maxChars) {
        ReleaseMemory();
        m_maxChars = other->m_maxChars;
        m_numLines = other->m_numLines;
        m_x60 = other->m_x60;
        AllocateMemory();
    }
    strncpy(m_text, other->m_text, m_maxChars - 1);
    m_text[m_maxChars - 1] = 0;
    m_x20.x = other->m_x20.x;
    m_x20.y = other->m_x20.y;
    m_position.x = other->m_position.x;
    m_position.y = other->m_position.y;
    m_color = other->m_color;
    m_alphaFix = other->m_alphaFix;
    m_alphaTest = other->m_alphaTest;
    m_alphaBlendMode = other->m_alphaBlendMode;
    m_alphaCompare = other->m_alphaCompare;
    m_alphaRef = other->m_alphaRef;
    m_alphaFail = other->m_alphaFail;
    m_scale = other->m_scale;
    m_x48 = other->m_x48;
    m_x30.x = other->m_x30.x;
    m_x30.y = other->m_x30.y;
    m_x4C = other->m_x4C;
    m_zBuffer = other->m_zBuffer;
    m_modulate = other->m_modulate;
    m_font = other->m_font;
    m_justification = other->m_justification;
    m_dirty = 1;
    m_x5D_bit4 = 1;
    m_cellsDirty = 1;
    m_positionDirty = 1;
    m_x74 = 0;
    m_x68 = 1;
}

// ScreenTextManager's vertex buffer (5 quadwords per character); retail's
// .bss has it after ScreenText's statics, before ScreenTextManager's.
_D3DVECTOR ScreenTextManager::TempVertices[100][5];

ScreenTextManager::ScreenTextManager(ScreenTextFont *font, int enabled) : GameObject(enabled)
{
    m_numTexts = 0;
    m_defaultFont = font;
    m_x40 = 1;
    m_maxTexts = 0;
    m_texts = 0;
    m_renderItems = 0;
    m_startItem = 0;
    m_endItem = 0;
}

ScreenTextManager::~ScreenTextManager()
{
    PSXprintf("Releasing an ScreenTextManager object\n");
    if (m_texts) {
        delete[] m_texts;
        m_texts = 0;
    }
    if (m_renderItems)
        delete[] m_renderItems;
    if (m_startItem)
        delete m_startItem;
    if (m_endItem)
        delete m_endItem;
}

ScreenTextManager *ScreenTextManager::Create(int maxTexts)
{
    if (maxTexts < 128)
        m_maxTexts = maxTexts;
    else
        m_maxTexts = maxTexts = 128;
    if (m_texts)
        delete[] m_texts;
    if (m_renderItems) {
        delete[] m_renderItems;
        m_renderItems = 0;
    }
    if (m_startItem) {
        delete m_startItem;
        m_startItem = 0;
    }
    if (m_endItem) {
        delete m_endItem;
        m_endItem = 0;
    }
    if (m_maxTexts > 0) {
        m_texts = new ScreenText *[m_maxTexts];
        m_renderItems = new DmaRenderItem[m_maxTexts];
        m_startItem = new DmaRenderItem;
        m_endItem = new DmaRenderItem;
        m_startItem->Start((DmaRenderDestination)1, 0x80);
        m_startItem->DmaAddVifCode(0x11000000);     // FLUSH
        m_startItem->DmaAddVifCode(0x03000000);     // BASE 0
        m_startItem->DmaAddVifCode(0x02000200);     // OFFSET 0x200
        m_startItem->End();
        m_endItem->Start((DmaRenderDestination)1, 0x80);
        m_endItem->DmaAddVifCode(0x11000000);       // FLUSH
        m_endItem->End();
    }
    return this;
}

#pragma alignlabel off

void ScreenTextManager::AddText(ScreenText *text, ScreenTextFont *font)
{
    if (m_numTexts < m_maxTexts) {
        m_texts[m_numTexts] = text;
        if (!font) {
            if (!m_texts[m_numTexts]->m_font && !m_defaultFont) {
#line 711 "ScreenTextManager.cpp"
                ASSERT(0, "AddText method : NO FONT TO RENDER TEXT WITH!");
            } else {
                if (m_texts[m_numTexts]->m_font)
                    font = m_texts[m_numTexts]->m_font;
                else
                    font = m_defaultFont;
            }
        }
        if (m_texts[m_numTexts]->m_font != font) {
            m_texts[m_numTexts]->m_font = font;
            m_texts[m_numTexts]->m_positionDirty = 1;
            m_texts[m_numTexts]->m_cellsDirty = 1;
            m_texts[m_numTexts]->m_dirty = 1;
        }
        if (m_texts[m_numTexts]->m_positionDirty)
            PositionText(m_texts[m_numTexts]);
        SetCharacterCells(m_texts[m_numTexts]);
        m_numTexts++;
    }
}

#pragma alignlabel reset

void ScreenTextManager::SetCharacterCells(ScreenText *text)
{
    text->m_cells = (char *)text->m_font->m_glyphs;
}

int ScreenTextManager::Render3D()
{
    static int timerID = -1;   // unused in retail
    if (m_x40)
        RenderQueuedText();
    m_numTexts = 0;
    return 1;
}

// Whether b can reuse the render state set up for a.
static inline bool SameRenderState(ScreenText *a, ScreenText *b)
{
    return a->m_font == b->m_font && a->m_alphaTest == b->m_alphaTest && a->m_alphaBlendMode == b->m_alphaBlendMode
        && a->m_alphaCompare == b->m_alphaCompare && a->m_alphaRef == b->m_alphaRef && a->m_alphaFail == b->m_alphaFail
        && a->m_zBuffer == b->m_zBuffer && a->m_modulate == b->m_modulate;
}


void ScreenTextManager::RenderQueuedText()
{
    if (m_numTexts <= 0)
        return;
    int i;
    int j;
    int numTags;
    ScreenText *last;
    last = 0;
    ((PSXRenderTarget *)LpRenderTarget)->AddToRenderQueue(m_startItem, "");
    ((PSXRenderTarget *)LpRenderTarget)->AddMPGToRenderQueue(Quad2DRendererCode);
    for (i = 0; i < m_numTexts; i++) {
        numTags = ComposeRenderItem(m_texts[i]);
        if (!numTags)
            continue;
        if (!FontReuseFlag || !last || SameRenderState(last, m_texts[i]) == 0) {
            AddTextToRenderItem(&m_renderItems[i], m_texts[i]);
            ((PSXRenderTarget *)LpRenderTarget)->AddToRenderQueue(&m_renderItems[i], "");
            last = m_texts[i];
        }
        for (j = 0; j < numTags; j++)
            ((PSXRenderTarget *)LpRenderTarget)->AddToRenderQueue((_sceDmaTag *)m_texts[i]->m_lines[j], 1, "");
    }
    ((PSXRenderTarget *)LpRenderTarget)->AddToRenderQueue(m_endItem, "");
}

// Set up the text's render state and compose it into the item.
void ScreenTextManager::AddTextToRenderItem(DmaRenderItem *item, ScreenText *text)
{
    static int timerID = -1;    // unused profiler ID
    ScreenTextFont *font = text->m_font;
    PSXRenderStateBlock state;
    // The redundant cast orders the loads as retail does (manager, then texture).
    LpTextureMapManager->AddTextureToGifList((PSXTextureMap *)font->m_texture);
    LpTextureMapManager->AddTextureInterruptToRenderQueue();
    state.SetTextureMap((PSXTextureMap *)font->m_texture);
    state.AlphaTestEnable(text->m_alphaTest);
    state.SetAlphaBlendMode(text->m_alphaBlendMode, text->m_alphaFix);
    state.SetAlphaCompare(text->m_alphaCompare);
    state.SetAlphaRefValue(text->m_alphaRef);
    state.SetAlphaFailMethod(text->m_alphaFail);
    state.ZTestEnable(text->m_zBuffer);
    state.ZWriteEnable(text->m_zBuffer);
    state.SetTextureAddressMode(1);
    state.SetTextureFilter((_D3DTEXTUREFILTERTYPE)1);
    if (!text->m_modulate)
        state.SetTextureFunction(2);
    item->Start((DmaRenderDestination)1, 0x80);
    state.ComposeRenderItem(item);
    item->DmaAddVifCode(0x11000000);    // FLUSH
    item->DmaAddVifCode(0x03000000);    // BASE
    item->DmaAddVifCode(0x02000200);    // OFFSET
    item->End();
}


#pragma fast_fptosi on

int ScreenTextManager::ComposeRenderItem(ScreenText *text)
{
    static int timerID = -1; // profiler ID (unused)
    int numChars;
    int line;
    float width;
    float height;
    ScreenTextFont *font;
    TextColor color;
    int col;
    float x;
    float y;
    float x1;
    float z;
    float ty;
    float tx;

    int i;
    int numGlyphs;
    int texWidth;
    int texHeight;
    float cellWidth;
    float cellHeight;
    int count;
    int c;
    ScreenTextGlyph *glyph;
    _D3DVECTOR *v;
    numChars = strlen(text->m_text);
    if (numChars == 0)
        return 0;
    width = LpRenderTarget->m_camera->m_viewport.Width;
    height = LpRenderTarget->m_camera->m_viewport.Height;
    if (!text->m_x5D_bit4 && !text->m_dirty && !text->m_cellsDirty) {
        int n = text->m_x60;
        for (line = 0; n > 0; n--, line++) {
            float *q = (float *)text->m_lineOffsets[line * 21].m_c;
            q[0] = width;
            q[1] = height;
            q[2] = q[3] = 0.0f;
        }
        return line;
    }
    font = text->m_font;
    color.r = ((text->m_color >> 16) & 0xFF) >> 1;
    color.g = ((text->m_color >> 8) & 0xFF) >> 1;
    color.b = (text->m_color & 0xFF) >> 1;
    color.a = text->m_alphaFix >> 2;
    if (text->m_x5D_bit4 && !text->m_dirty && !text->m_cellsDirty) {
        int left = text->m_x74;
        for (line = 0; left > 0; line++) {
            int count = (left < 20) ? left : 20;
            // One variable for the offsets array and then the line's size qword:
            // retail keeps both in the same register.
            ScreenTextVifDataItemOffset *lo = text->m_lineOffsets;
            lo = (ScreenTextVifDataItemOffset *)lo[line * 21].m_c;
            SetScreenSize((float *)lo, width, height);
            for (int k = 0; count > 0; count--, left--, k++)
                *(TextColor *)(&text->m_lineOffsets[line * 21])[k + 1].m_a = color;
        }
        text->m_x5D_bit4 = 0;
        return line;
    }
    col = 0;
    tx = text->m_x20.x;
    ty = text->m_x20.y;
    int ix = (int)tx;
    int iy = (int)ty;
    x = ix;
    y = iy;
    x1 = 0.0f;
    // Depth read three times (mwcc keeps one load); a named local colours differently.
    if (text->m_x4C < 0.0f)
        z = -text->m_x4C;
    else
        z = LpVideoCard->m_maxDepth - text->m_x4C;
    i = 0;
    numGlyphs = 0;
    texWidth = (float)font->m_texture->m_width;
    texHeight = (float)font->m_texture->m_height;
    cellWidth = texWidth * text->m_scale;
    cellHeight = texHeight * text->m_scale;
    for (; numChars > 0; numChars -= 20) {
        count = (numChars < 20) ? numChars : 20;
        for (; count > 0; count--) {
            glyph = ((ScreenTextGlyph **)text->m_cells)[c = (unsigned char)text->m_text[i]];
            if (glyph) {
                v = TempVertices[i];
                if (text->m_dirty) {
                    float du = glyph->m_right - glyph->m_left;
                    float dv = glyph->m_bottom - glyph->m_top;
                    v->x = x; v->y = y; v->z = z; v->w = 0.0f;
                    v++;
                    x1 = x + cellWidth * du;
                    v->x = x1; v->y = y; v->z = z; v->w = 0.0f;
                    v++;
                    float y1 = y + cellHeight * dv;
                    v->x = x; v->y = y1; v->z = z; v->w = 0.0f;
                    v++;
                    v->x = x1; v->y = y1; v->z = z; v->w = 0.0f;
                    v++;
                } else {
                    v += 4;
                }
                if (text->m_cellsDirty) {
                    v->x = glyph->m_left;
                    v->y = glyph->m_top;
                    v->z = glyph->m_right;
                    v->w = glyph->m_bottom;
                }
#line 1096 "ScreenTextManager.cpp"
                ASSERT(i <= text->m_maxChars, "");
                i++;
                numGlyphs++;
            } else {
#line 1102 "ScreenTextManager.cpp"
                ASSERTF(0, ("ERROR! String '%s': character[%d] '%c' [ascii %d] missing in font '%s'\n", text->m_text, i, c, c, font->m_name));
                i++;
            }
            if (text->m_dirty)
                x = x1;
            col++;
        }
    }
    if (text->m_x48 != 0.0f && text->m_dirty) {
        float cosine, sine;
        FastSinCos(text->m_x48, &sine, &cosine);
        float px = (int)text->m_x30.x;
        float py = (int)text->m_x30.y;
        for (int j = 0; j < numGlyphs; j++) {
            for (int k = 0; k < 4; k++) {
                float nx = px + (cosine * (TempVertices[j][k].x - px) - sine * (TempVertices[j][k].y - py));
                float ny = py + (sine * (TempVertices[j][k].x - px) + cosine * (TempVertices[j][k].y - py));
                TempVertices[j][k].x = nx;
                TempVertices[j][k].y = ny;
            }
        }
    }
    int k;
    ScreenTextLineOffsets *offs;
    ScreenTextVifDataItemOffset *o;
    ScreenTextPacketHeader *item;
    i = 0;
    line = 0;
    if (text->m_dirty)
        text->m_x74 = numGlyphs;
    for (; numGlyphs > 0; numGlyphs -= 20, line++) {
        count = (numGlyphs < 20) ? numGlyphs : 20;
        offs = (ScreenTextLineOffsets *)&text->m_lineOffsets[line * 21];
        item = (ScreenTextPacketHeader *)offs->m_header.m_a;
        if (text->m_dirty)
            item->m_nloop = count * 4;
        float *q = item->m_size;
        q[0] = width;
        q[1] = height;
        q[2] = q[3] = 0.0f;
        for (k = 0; count > 0; i++, k++, count--) {
            float *p; // one pointer for the colour, corner and UV qwords, as retail
            o = &offs->m_chars[k];
            if (text->m_x5D_bit4) {
                p = (float *)o->m_a;
                *(TextColor *)p = color;
            }
            v = TempVertices[i];
            if (text->m_dirty) {
                p = (float *)o->m_b;
                asm("lqc2 vf1, 0x0(%0); lqc2 vf2, 0x10(%0); lqc2 vf3, 0x20(%0); lqc2 vf4, 0x30(%0); sqc2 vf1, 0x0(%1); sqc2 vf2, 0x10(%1); sqc2 vf3, 0x20(%1); sqc2 vf4, 0x30(%1)"
                    : : "r"(v), "r"(p) : "memory");
            }
            v += 4;
            if (text->m_cellsDirty) {
                p = (float *)o->m_c;
                SetUVs(p, v);
            }
        }
    }//F>>
    text->m_x5D_bit4 = text->m_dirty = text->m_cellsDirty = 0;
    text->m_x60 = line;
    return line;
}

int ScreenTextManager::GetStringWidthInPixels(ScreenText *text)
{
    return GetStringWidthInPixels(text, text->m_font, text->m_scale);
}

int ScreenTextManager::GetStringWidthInPixels(ScreenText *text, ScreenTextFont *font, float scale)
{
    int length = strlen(text->m_text);
    float width = 0.0f;
    if (!font) {
        ScreenTextFont *defaultFont = m_defaultFont;
        if (!defaultFont) {
#line 1270 "ScreenTextManager.cpp"
            ASSERT(0, "GetStringWidthInPixels method : NO FONT TO ASSOCIATE WITH TEXT!");
        } else {
            font = defaultFont;
        }
    }
    for (int i = 0; i < length; i++) {
        ScreenTextGlyph *glyph = font->GetCell((unsigned char)text->m_text[i]);
        if (glyph)
            width += glyph->m_right - glyph->m_left;
    }
    return scale * (width * font->m_texture->m_width);
}

// The justification codes, in retail's .rodata. The header has them non-const:
// const there moves UIWidgetOverlayText::Create's heap shim (d3dtypes.h).
// Defined here for their place in .rodata (before PositionText's string);
// non-const, so nothing folds them.
int __attribute__((section(".rodata"))) ScreenTextManager::LeftJustification = 0;
int __attribute__((section(".rodata"))) ScreenTextManager::CenterHorizontalJustification = 1;
int __attribute__((section(".rodata"))) ScreenTextManager::CenterVerticalJustification = 2;
int __attribute__((section(".rodata"))) ScreenTextManager::CenterBothJustification = 3;
int __attribute__((section(".rodata"))) ScreenTextManager::RightJustification = 4;

void ScreenTextManager::PositionText(ScreenText *text)
{
    PositionText(text, text->m_justification, text->m_font);
}

void ScreenTextManager::PositionText(ScreenText *text, int justification, ScreenTextFont *font)
{
    if (!font) {
        ScreenTextFont *defaultFont = m_defaultFont;
        if (!defaultFont) {
#line 1328 "ScreenTextManager.cpp"
            ASSERT(0, "PositionText method : NO FONT TO ASSOCIATE WITH TEXT!");
        } else {
            font = defaultFont;
        }
    }
    if (justification == 0 || text->m_text[0] == 0) {
        text->m_x20.x = text->m_position.x;
        text->m_x20.y = text->m_position.y;
    } else {
        ScreenTextGlyph *glyph = 0;
        unsigned int length = strlen(text->m_text);
        float width = 0.0f;
        for (unsigned int i = 0; i < length; i++) {
            glyph = font->GetCell((unsigned char)text->m_text[i]);
            if (glyph) {
                width += glyph->m_right - glyph->m_left;
            } else {
                int c = (unsigned char)text->m_text[i];
#line 1353 "ScreenTextManager.cpp"
                ASSERTF(0, ("ERROR! String '%s': character[%d] '%c' [ascii %d] missing in font '%s'\n", text->m_text, i, (unsigned char)c, (unsigned char)c, font->m_name));
            }
        }
        float scaleX = text->m_scale * font->m_texture->m_width;
        width *= scaleX;
        float glyphHeight = glyph->m_bottom - glyph->m_top;
        float scaleY = text->m_scale * font->m_texture->m_height;
        float height = scaleY * glyphHeight;
        if (justification == 1) {
            text->m_x20.x = (int)(text->m_position.x - width / 2.0f);
            text->m_x20.y = text->m_position.y;
        } else if (justification == 2) {
            text->m_x20.x = text->m_position.x;
            text->m_x20.y = (int)(text->m_position.y - height / 2.0f);
        } else if (justification == 3) {
            text->m_x20.x = (int)(text->m_position.x - width / 2.0f);
            text->m_x20.y = (int)(text->m_position.y - height / 2.0f);
        } else {
            text->m_x20.x = (int)(text->m_position.x - width);
            text->m_x20.y = text->m_position.y;
        }
    }
    if (text->m_justification != justification)
        text->m_justification = justification;
    text->m_dirty = 1;
    text->m_positionDirty = 0;
}

#pragma fast_fptosi reset

