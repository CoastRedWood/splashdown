// Overlay.cpp  @ 0x0025F2D0 (text 0x25F2D0-0x260390)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's Overlay.cpp as one translation unit, a TU without a static
// initializer in NormalDistribution.cpp's tu_map row: Overlay's nine functions.
// Its .rodata (0x415FB8-0x416040) is Overlay's RTTI record with the
// BaseObject/GameObject records after it; Parameterblocks.cpp's strings follow.
// `make link` places the object's data as well as its code (tools/link_c.py).



#include "d3dtypes.h"
#include "Overlay/Overlay.h"
#include "GameObject/GameObject.h"
#include "DmaRenderItem/DmaRenderItem.h"
#include "_RECT/_RECT.h"
#include "RenderTarget/RenderTarget.h"
#include "PSXRenderTarget/PSXRenderTarget.h"
#include "Camera/Camera.h"
#include "TextureMap/TextureMap.h"
#include "PSXTextureMap/PSXTextureMap.h"
#include "PSXTextureMapManager/PSXTextureMapManager.h"
#include "PSXRenderStateBlock/PSXRenderStateBlock.h"
#include "sceGifTag/sceGifTag.h"
#include "PSXVideoCard/PSXVideoCard.h"

extern "C" void *memset(void *dst, int c, unsigned int n);
extern "C" void *memcpy(void *dst, const void *src, unsigned int n);

extern PSXTextureMapManager *LpTextureMapManager;
extern PSXVideoCard *LpVideoCard;
extern unsigned int Quad2DRendererCode[];   // VU1 microcode

typedef int int128_t __attribute__((mode(TI)));

// ---- src/Overlay/ctor.cpp

Overlay::Overlay(int id, int layer) : GameObject(id)
{
    m_texture = 0;
    m_screenType = 0;
    m_mode = 1;
    memset(m_vertices, 0, sizeof(m_vertices));
    memset(&m_srcRect, 0, sizeof(_RECT));
    memset(&m_dstRect, 0, sizeof(_RECT));
    m_hasDstRect = 0;
    m_alpha = 0xFF;
    m_useAlpha = 0;
    m_depth = 1.0f;
    m_x10C = 0;
    m_buffer = 0;
    m_layer = layer;
}

// ---- src/Overlay/dtor.cpp

Overlay::~Overlay()
{
}

// ---- src/Overlay/Create.cpp

Overlay *Overlay::Create(TextureMap *texture, _RECT *srcRect, int mode, _RECT *dstRect, float depth)
{
    GameObject::Create();
    m_depth = depth;
    m_texture = texture;
    m_mode = mode;
    m_screenType = LpRenderTarget->m_screenType;
    if (srcRect)
        memcpy(&m_srcRect, srcRect, sizeof(_RECT));
    if (dstRect) {
        memcpy(&m_dstRect, dstRect, sizeof(_RECT));
        m_hasDstRect = 1;
    }
    m_buffer = 0;
    SetUpRect();
    SetVertexColor(0xFF, 0xFF, 0xFF);
    return this;
}

// ---- src/Overlay/Tick.cpp

int Overlay::Tick(float dt)
{
    return GameObject::Tick(dt);
}

// ---- src/Overlay/PrepareGeometry.cpp

int Overlay::PrepareGeometry()
{
    TextureMap *t = m_texture;
    if (t && 10.0f > t->m_usage)
        t->m_usage = 10.0f;
    return GameObject::PrepareGeometry();
}

// ---- src/Overlay/Render3D.cpp
// D3D state numbers used here.
#define RS_ZENABLE          ((_D3DRENDERSTATETYPE)7)
#define RS_ZWRITEENABLE     ((_D3DRENDERSTATETYPE)14)
#define RS_ALPHATESTENABLE  ((_D3DRENDERSTATETYPE)15)
#define RS_ALPHABLENDENABLE ((_D3DRENDERSTATETYPE)27)
#define RS_FOGENABLE        ((_D3DRENDERSTATETYPE)28)
#define TSS_COLOROP         ((_D3DTEXTURESTAGESTATETYPE)1)
#define TSS_COLORARG1       ((_D3DTEXTURESTAGESTATETYPE)2)
#define TSS_COLORARG2       ((_D3DTEXTURESTAGESTATETYPE)3)
#define TSS_ALPHAOP         ((_D3DTEXTURESTAGESTATETYPE)4)
#define TSS_ALPHAARG1       ((_D3DTEXTURESTAGESTATETYPE)5)
#define TSS_ALPHAARG2       ((_D3DTEXTURESTAGESTATETYPE)6)
#define TSS_ADDRESSU        ((_D3DTEXTURESTAGESTATETYPE)13)
#define TSS_ADDRESSV        ((_D3DTEXTURESTAGESTATETYPE)14)

int Overlay::Render3D()
{
    if (m_texture) {
        unsigned int addressMode;
        LpRenderTarget->SetRenderState(RS_ZENABLE, 0, 0);
        m_texture->SetRenderStates();
        LpRenderTarget->GetTextureStageState(0, TSS_ADDRESSU, &addressMode);
        LpRenderTarget->SetTextureStageState(0, TSS_ADDRESSU, 1);
        LpRenderTarget->SetTextureStageState(0, TSS_ADDRESSV, 1);
        LpRenderTarget->SetRenderState(RS_FOGENABLE, 0, 0);
        if (m_layer)
            SetUpRect();
        if (m_texture->m_format == 0x115C || m_texture->m_format == 0x22B8) {
            LpRenderTarget->SetTextureStageState(0, TSS_COLOROP, 4);
            LpRenderTarget->SetTextureStageState(0, TSS_COLORARG1, 2);
            LpRenderTarget->SetTextureStageState(0, TSS_COLORARG2, 0);
            if (m_useAlpha) {
                LpRenderTarget->SetTextureStageState(0, TSS_ALPHAOP, 4);
                LpRenderTarget->SetTextureStageState(0, TSS_ALPHAARG1, 2);
                LpRenderTarget->SetTextureStageState(0, TSS_ALPHAARG2, 0);
            } else {
                LpRenderTarget->SetTextureStageState(0, TSS_ALPHAOP, 2);
                LpRenderTarget->SetTextureStageState(0, TSS_ALPHAARG1, 2);
            }
            LpRenderTarget->SetRenderState(RS_ALPHABLENDENABLE, 1, 0);
        } else {
            LpRenderTarget->SetTextureStageState(0, TSS_COLOROP, 4);
            LpRenderTarget->SetTextureStageState(0, TSS_COLORARG1, 2);
            LpRenderTarget->SetTextureStageState(0, TSS_COLORARG2, 0);
            if (m_useAlpha) {
                LpRenderTarget->SetTextureStageState(0, TSS_ALPHAOP, 2);
                LpRenderTarget->SetTextureStageState(0, TSS_ALPHAARG1, 2);
            } else {
                LpRenderTarget->SetTextureStageState(0, TSS_ALPHAOP, 1);
            }
        }
        LpRenderTarget->EnableAlphaTest(0);
        LpRenderTarget->SetRenderState(RS_ZWRITEENABLE, 0, 0);
        LpRenderTarget->SetRenderState(RS_ZENABLE, 0, 0);
        m_vertices[4] = m_vertices[0];
        ((PSXRenderTarget *)LpRenderTarget)->AddMPGToRenderQueue(Quad2DRendererCode);
        PSXRenderStateBlock block;
        DmaRenderItem *item = &m_renderItem;
        item->Start((DmaRenderDestination)1, 0x80);
        block.AlphaTestEnable(0);
        if (!m_x10C) {
            block.ZTestEnable(0);
            block.ZWriteEnable(1);
        }
        block.SetTextureAddressMode(1);
        LpTextureMapManager->AddTextureToGifList((PSXTextureMap *)m_texture);    // redundant cast: retail loads the manager first
        LpTextureMapManager->AddTextureInterruptToRenderQueue();
        block.SetTextureMap((PSXTextureMap *)m_texture);
        block.ComposeRenderItem(item);
        item->End();
        ((PSXRenderTarget *)LpRenderTarget)->AddToRenderQueue(item, "");
        ((PSXRenderTarget *)LpRenderTarget)->AddToRenderQueue(&m_renderItems[m_buffer], "");
        LpRenderTarget->SetRenderState(RS_ZWRITEENABLE, 1, 0);
        LpRenderTarget->SetRenderState(RS_ZENABLE, 1, 0);
        LpRenderTarget->SetRenderState(RS_ALPHATESTENABLE, 0, 0);
        LpRenderTarget->SetTextureStageState(0, TSS_ADDRESSU, addressMode);
        LpRenderTarget->SetTextureStageState(0, TSS_ADDRESSV, addressMode);
    }
    return 1;
}

// ---- src/Overlay/Build2DQuad.cpp
// Build2DQuad__7OverlayFv  @ 0x0025FB20
// The colour components are named locals that each read m_vertices[0].color (one CSE'd
// load); declared before the quadword pointer and stored after it, g first, they keep
// retail's interleaved r/g chains ($v1/$a3). A single `color` local instead gets them
// forward-substituted into the stores. The local order (quadWidth, left, quadHeight, z,
// g, r, b, top, q) came from a random-permutation search (a statement-order search).

// Appends one quadword to the open unpack.
#define ADD_QWORD(item, type, a, b, c, d)                                       \
    {                                                                           \
        type *q = (type *)&(item)->m_dataList[(item)->m_dataListCurrentSize];  \
        q[0] = a;                                                               \
        q[1] = b;                                                               \
        q[2] = c;                                                               \
        q[3] = d;                                                               \
        DmaRenderItem::UnpackCount += 4;                                        \
        (item)->m_dataListCurrentSize += 4;                                     \
    }

// The VU1 packet for a 2D sprite: GIF tag, screen size, colour, the four
// corners and the two texture coordinates.
void Overlay::Build2DQuad()
{
    float width = LpRenderTarget->m_camera->m_viewport.Width;
    float height = LpRenderTarget->m_camera->m_viewport.Height;
    DmaRenderItem *item = &m_renderItems[m_buffer];
    item->Start((DmaRenderDestination)1, 0x48);
    sceGifTag tag;
    *(int128_t *)&tag = 0;
    tag.EOP = 1;
    tag.PRE = 1;
    tag.PRIM = 0x5C;
    tag.FLG = 0;
    tag.NREG = 3;
    tag.NLOOP = 4;
    tag.REGS0 = 2;      // ST
    tag.REGS1 = 1;      // RGBAQ
    tag.REGS2 = 5;      // XYZ2
    item->DmaAddVifCode(0x11000000);    // FLUSH
    item->DmaAddVifCode(0x03000000);    // BASE
    item->DmaAddVifCode(0x02000200);    // OFFSET
    item->DmaBeginUnpack(0, 12);
    item->DmaAddGifTagToUnpack(&tag);
    ADD_QWORD(item, float, width, height, 0, 0);
    // Colour components and the corner floats, in the order retail evaluates them.
    float quadWidth = m_vertices[2].sx - m_vertices[0].sx;
    float left = m_vertices[0].sx;
    float quadHeight = m_vertices[2].sy - m_vertices[0].sy;
    float z = (float)LpVideoCard->m_maxDepth - m_depth;
    int g = (((int)m_vertices[0].color >> 8) & 0xFF) >> 1;
    int r = (((int)m_vertices[0].color >> 16) & 0xFF) >> 1;
    int b = ((int)m_vertices[0].color & 0xFF) >> 1;
    float top = m_vertices[0].sy;
    int *q = (int *)&item->m_dataList[item->m_dataListCurrentSize];
    q[0] = r;
    q[1] = g;
    q[2] = b;
    q[3] = 0x40;
    DmaRenderItem::UnpackCount += 4;
    item->m_dataListCurrentSize += 4;
    ADD_QWORD(item, float, left, top, z, 0);
    ADD_QWORD(item, float, left + quadWidth, top, z, 0);
    ADD_QWORD(item, float, left, top + quadHeight, z, 0);
    ADD_QWORD(item, float, left + quadWidth, top + quadHeight, z, 0);
    ADD_QWORD(item, float, m_vertices[0].tu, m_vertices[0].tv, 1.0f, 0);
    ADD_QWORD(item, float, m_vertices[2].tu, m_vertices[2].tv, 1.0f, 0);
    item->DmaEndUnpack();
    item->DmaAddVifCodeMSCAL();
    item->DmaAddVifCode(0x10000000);    // FLUSHE
    item->End();
}

// ---- src/Overlay/SetUpRect.cpp
static inline float Fmin(float a, float b)
{
    float r;
    asm("min.s $f8, %1, %2; swc1 $f8, 0(%0)" : : "r"(&r), "f"(a), "f"(b) : "$f8", "memory");
    return r;
}

static inline float Fmax(float a, float b)
{
    float r;
    asm("max.s $f8, %1, %2; swc1 $f8, 0(%0)" : : "r"(&r), "f"(a), "f"(b) : "$f8", "memory");
    return r;
}

// Builds the overlay's screen quad, clipping it (and its texture
// coordinates) to the render target.
void Overlay::SetUpRect()
{
    float u0, u1, v0, v1;
    if (m_hasDstRect) {
        u0 = (float)m_dstRect.left / (float)m_texture->m_width;
        v0 = (float)m_dstRect.top / (float)m_texture->m_height;
        u1 = (float)m_dstRect.right / (float)m_texture->m_width;
        v1 = (float)m_dstRect.bottom / (float)m_texture->m_height;
    } else {
        v0 = u0 = 0.0f;
        v1 = u1 = 1.0f;
    }
    float maxX = (float)LpRenderTarget->m_width - 1.0f;
    float maxY = (float)LpRenderTarget->m_height - 1.0f;
    float left, top, right, bottom;
    if (m_mode) {
        // Declaration order sets the colouring order (scaleY first); the height is
        // read before the width so its load is scheduled first, as in retail.
        float scaleY, scaleX, width, height;
        height = (float)LpVideoCard->m_height;
        width = (float)LpVideoCard->m_width;
        scaleX = maxX / width;
        scaleY = maxY / height;
        top = m_srcRect.top * scaleY;
        left = m_srcRect.left * scaleX;
        right = m_srcRect.right * scaleX;
        bottom = m_srcRect.bottom * scaleY;
    } else {
        left = m_srcRect.left;
        right = m_srcRect.right;
        top = m_srcRect.top;
        bottom = m_srcRect.bottom;
    }
    if (!(right < maxX)) {
        float t = (right - maxX) / (right - left);
        float du = u1 - u0;
        u1 = u0 + du * (1.0f - t);
        right = maxX;
    }
    if (!(bottom < maxY)) {
        float t = (bottom - maxY) / (bottom - top);
        float dv = v1 - v0;
        v1 = v0 + dv * (1.0f - t);
        bottom = maxY;
    }
    // (retail scales 1 - u0 here, not u1 - u0)
    if (left < 0.0f)
        u0 = (1.0f - u0) * (1.0f - -left / (right - left));
    if (top < 0.0f)
        v0 = (1.0f - v0) * (1.0f - -top / (bottom - top));
    left = Fmax(left, 0.0f);
    top = Fmax(top, 0.0f);
    right = Fmin(right, 1.0f + maxX);
    bottom = Fmin(bottom, 1.0f + maxY);
    m_vertices[0].sx = left;
    m_vertices[0].sy = top;
    m_vertices[0].sz = m_depth;
    m_vertices[0].rhw = 1.0f;
    m_vertices[0].specular = 0;
    m_vertices[0].tu = u0;
    m_vertices[0].tv = v0;
    m_vertices[1].sx = right;
    m_vertices[1].sy = top;
    m_vertices[1].sz = m_depth;
    m_vertices[1].rhw = 1.0f;
    m_vertices[1].specular = 0;
    m_vertices[1].tu = u1;
    m_vertices[1].tv = v0;
    m_vertices[2].sx = right;
    m_vertices[2].sy = bottom;
    m_vertices[2].sz = m_depth;
    m_vertices[2].rhw = 1.0f;
    m_vertices[2].specular = 0;
    m_vertices[2].tu = u1;
    m_vertices[2].tv = v1;
    m_vertices[3].sx = left;
    m_vertices[3].sy = bottom;
    m_vertices[3].sz = m_depth;
    m_vertices[3].rhw = 1.0f;
    m_vertices[3].specular = 0;
    m_vertices[3].tu = u0;
    m_vertices[3].tv = v1;
    Build2DQuad();
}

// ---- src/Overlay/SetVertexColor.cpp

void Overlay::SetVertexColor(int r, int g, int b)
{
    if (m_useAlpha) {
        m_vertices[0].color = m_alpha << 24 | r << 16 | g << 8 | b;
        m_vertices[1].color = m_alpha << 24 | r << 16 | g << 8 | b;
        m_vertices[2].color = m_alpha << 24 | r << 16 | g << 8 | b;
        m_vertices[3].color = m_alpha << 24 | r << 16 | g << 8 | b;
    } else {
        unsigned int color = 0xFF000000 | r << 16 | g << 8 | b;
        m_vertices[0].color = color;
        m_vertices[1].color = color;
        m_vertices[2].color = color;
        m_vertices[3].color = color;
    }
}
