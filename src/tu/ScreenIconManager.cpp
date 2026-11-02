// ScreenIconManager.cpp  @ 0x003712B0 (text 0x3712B0-0x3724D0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's ScreenIconManager.cpp as one translation unit: ScreenIcon's methods, then
// ScreenIconManager's, in retail order, then its static initializer. `make link` places the
// object's data as well as its code (tools/link_c.py).



#include "GameObject/GameObjectGlobals.h"
#include "ScreenIcon/ScreenIcon.h"
#include "DmaRenderItem/DmaRenderItem.h"
#include "ScreenIconManager/ScreenIconManager.h"
#include "PSXTextureMapManager/PSXTextureMapManager.h"
#include "PSXRenderStateBlock/PSXRenderStateBlock.h"
#include "RenderTarget/RenderTarget.h"
#include "Camera/Camera.h"
#include "PSXVideoCard/PSXVideoCard.h"
#include "PSXRenderTarget/PSXRenderTarget.h"
#include "TextureMap/TextureMap.h"
#include "PSXTextureMap/PSXTextureMap.h"
#include "sceGifTag/sceGifTag.h"
#include "soultree_assert_block.h"
#include "sinit/AxisVectors.h"

void PSXprintf(const char *fmt, ...);
extern "C" void *memalign(unsigned int align, unsigned int size);
extern "C" void *memset(void *dst, int c, unsigned int n);
extern "C" void free(void *p);
typedef int int128_t __attribute__((mode(TI)));
struct Palette8;
extern PSXVideoCard *LpVideoCard;
void FastSinCos(float x, float *sine, float *cosine);

// ---- src/ScreenIcon/ctor.cpp


ScreenIcon::ScreenIcon()
{
    m_texture = 0;
    m_invWidth = 1.0f;
    m_invHeight = 1.0f;
    m_position.y = 0.0f;
    m_position.x = 0.0f;
    m_color = -1;
    m_alphaFix = 0xFF;
    m_alphaTest = 0;
    m_alphaBlendMode = 0;
    m_alphaCompare = 6;
    m_alphaRef = 0;
    m_alphaFail = 0;
    m_34 = 1.0f;
    m_v0 = 0.0f;
    m_u0 = 0.0f;
    m_v1 = 1.0f;
    m_u1 = 1.0f;
    m_width = 0.0f;
    m_height = 0.0f;
    m_angle = 0.0f;
    m_pivot.y = 0.0f;
    m_pivot.x = 0.0f;
    m_depth = 1.0f;
    m_zBuffer = 0;
    m_addressMode = 1;
    m_isClone = 0;
    m_dirty = 1;
    m_colorDirty = 1;
    m_uvDirty = 1;
    m_vifData = 0;
}


// ---- src/ScreenIcon/Create.cpp


PSXTextureMap *GetTextureMapFromResourceName(char *name, int flags, Palette8 *palette, unsigned int a,
                                          _D3DBLEND src, _D3DBLEND dst, int alpha, unsigned int colorKey, int b);

ScreenIcon *ScreenIcon::Create(char *name, float x, float y)
{
    m_texture = GetTextureMapFromResourceName(name, 0x22B8, 0, 0, (_D3DBLEND)2, (_D3DBLEND)1, 0x80, 0xFF00FF, 1);
#line 72 "ScreenIconManager.cpp"
    ASSERTF(m_texture != 0, ("could not load icon texture %s\n", name));
    m_invWidth = 1.0f / m_texture->m_width;
    m_invHeight = 1.0f / m_texture->m_height;
    m_width = m_texture->m_width;
    m_height = m_texture->m_height;
    m_position.x = x;
    m_position.y = y;
    int size = 0xC0;
    m_vifData = (VifDataItem *)memalign(0x40, size);
    memset(m_vifData, 0, size);
    CreateVifDataItem(m_vifData, size, m_vifQwords);
    return this;
}


// ---- src/ScreenIcon/CreateClone.cpp


ScreenIcon *ScreenIcon::CreateClone()
{
    m_texture = 0;
    m_invWidth = 1.0f;
    m_invHeight = 1.0f;
    m_width = 0;
    m_height = 0;
    m_position.x = 0;
    m_position.y = 0;
    m_vifData = (VifDataItem *)memalign(64, 0xC0);
    memset(m_vifData, 0, 0xC0);
    CreateVifDataItem(m_vifData, 0xC0, m_vifQwords);
    m_isClone = 1;
    return this;
}


// ---- src/ScreenIcon/CreateVifDataItem.cpp

struct QWORD {
    unsigned int x, y, z, w;
};


struct IconPacket {
    unsigned int m_vif[4];      // +0x00
    sceGifTag m_gifTag;         // +0x10
    QWORD m_tex0;               // +0x20 (field names guessed; returned through out[])
    QWORD m_xyz[1];             // +0x30
    QWORD m_verts[4];           // +0x40
    float m_uv[2][4];           // +0x80
    unsigned int m_vifEnd[4];   // +0xA0
};

struct VifDataItem {
    short m_numQwords;          // +0x00
    unsigned char m_02;         // +0x02
    unsigned char m_03;         // +0x03
    PS2_PTR(IconPacket) m_data;         // +0x04
    int m_08;                   // +0x08
    int m_0C;                   // +0x0C
    IconPacket m_packet;        // +0x10
};


void ScreenIcon::CreateVifDataItem(VifDataItem *item, int size, QWORD **out)
{
    IconPacket *packet = &item->m_packet;
    packet->m_vif[0] = 0x11000000;          // FLUSH
    packet->m_vif[1] = 0x03000000;          // BASE
    packet->m_vif[2] = 0x02000200;          // OFFSET
    packet->m_vif[3] = 0x6C098000;          // UNPACK
    *(int128_t *)&packet->m_gifTag = 0;
    packet->m_gifTag.EOP = 1;
    packet->m_gifTag.PRE = 1;
    packet->m_gifTag.PRIM = 0x5C;
    packet->m_gifTag.FLG = 0;
    packet->m_gifTag.NREG = 3;
    packet->m_gifTag.REGS0 = 2;
    packet->m_gifTag.REGS1 = 1;
    packet->m_gifTag.REGS2 = 5;
    packet->m_gifTag.NLOOP = 4;
    packet->m_tex0.x = 0;
    packet->m_tex0.y = 0;
    packet->m_tex0.z = 0;
    packet->m_tex0.w = 0;
    out[0] = &packet->m_tex0;
    packet->m_xyz[0].x = 0;
    packet->m_xyz[0].y = 0;
    packet->m_xyz[0].z = 0;
    packet->m_xyz[0].w = 0;
    out[1] = &packet->m_xyz[0];
    packet->m_verts[0].x = 0;
    packet->m_verts[0].y = 0;
    packet->m_verts[0].z = 0;
    packet->m_verts[0].w = 0;
    packet->m_verts[1].x = 0;
    packet->m_verts[1].y = 0;
    packet->m_verts[1].z = 0;
    packet->m_verts[1].w = 0;
    packet->m_verts[2].x = 0;
    packet->m_verts[2].y = 0;
    packet->m_verts[2].z = 0;
    packet->m_verts[2].w = 0;
    packet->m_verts[3].x = 0;
    packet->m_verts[3].y = 0;
    packet->m_verts[3].z = 0;
    packet->m_verts[3].w = 0;
    out[2] = &packet->m_verts[0];
    packet->m_uv[0][0] = 0.0f;
    packet->m_uv[0][1] = 0.0f;
    packet->m_uv[0][2] = 1.0f;
    packet->m_uv[0][3] = 0.0f;
    packet->m_uv[1][0] = 0.0f;
    packet->m_uv[1][1] = 0.0f;
    packet->m_uv[1][2] = 1.0f;
    packet->m_uv[1][3] = 0.0f;
    out[3] = (QWORD *)packet->m_uv;
    packet->m_vifEnd[0] = 0x14000019;       // MSCAL
    packet->m_vifEnd[1] = 0x11000000;       // FLUSH
    packet->m_vifEnd[2] = 0;
    packet->m_vifEnd[3] = 0;
    item->m_numQwords = 11;
    item->m_02 = 0;
    item->m_03 = 0x30;
    item->m_data = packet;
    item->m_08 = 0;
    item->m_0C = 0;
}


// ---- src/ScreenIcon/dtor.cpp


ScreenIcon::~ScreenIcon()
{
    // Let the renderer finish with this icon first.
    LpRenderTarget->TrySendRenderQueue(1);
    LpRenderTarget->WaitForRenderingToComplete();
    if (m_texture && !m_isClone) {
        m_texture->Release();
        m_texture = 0;
    }
    if (m_vifData) {
        free(m_vifData);
        m_vifData = 0;
    }
}


// ---- src/ScreenIcon/Clone.cpp


void ScreenIcon::Clone(ScreenIcon *other)
{
    static int timerID = -1;    // profiler id, unused in retail
    if (this == other)
        return;
    if (m_texture != other->m_texture) {
        if (!m_isClone) {
            if (m_texture) {
                m_texture->Release();
                m_texture = 0;
            }
            m_texture = other->m_texture;
            if (m_texture)
                m_texture->AddRef();
        } else {
            m_texture = other->m_texture;
        }
    }
    m_invWidth = 1.0f / m_texture->m_width;
    m_invHeight = 1.0f / m_texture->m_height;
    m_position.x = other->m_position.x;
    m_position.y = other->m_position.y;
    m_color = other->m_color;
    m_alphaFix = other->m_alphaFix;
    m_alphaTest = other->m_alphaTest;
    m_alphaBlendMode = other->m_alphaBlendMode;
    m_alphaCompare = other->m_alphaCompare;
    m_alphaRef = other->m_alphaRef;
    m_alphaFail = other->m_alphaFail;
    m_34 = other->m_34;
    m_u0 = other->m_u0;
    m_u1 = other->m_u1;
    m_v0 = other->m_v0;
    m_v1 = other->m_v1;
    m_width = other->m_width;
    m_height = other->m_height;
    m_angle = other->m_angle;
    m_pivot.x = other->m_pivot.x;
    m_pivot.y = other->m_pivot.y;
    m_depth = other->m_depth;
    m_zBuffer = other->m_zBuffer;
    m_addressMode = other->m_addressMode;
    m_dirty = 1;
    m_colorDirty = 1;
    m_uvDirty = 1;
}


// ---- src/ScreenIconManager/ctor.cpp

ScreenIconManager::ScreenIconManager(int enabled) : GameObject(enabled)
{
    m_numQueued = 0;
    m_x40 = 1;
    m_maxIcons = 0;
    m_iconQueue = 0;
    m_icons = 0;
    m_renderItems = 0;
}


// ---- src/ScreenIconManager/dtor.cpp

ScreenIconManager::~ScreenIconManager()
{
    PSXprintf("Releasing an ScreenIconManager object\n");
    LpRenderTarget->TrySendRenderQueue(1);
    LpRenderTarget->WaitForRenderingToComplete();
    if (m_iconQueue) {
        delete[] m_iconQueue;
        m_iconQueue = 0;
    }
    if (m_icons) {
        delete[] m_icons;
        m_icons = 0;
    }
    if (m_renderItems) {
        delete[] m_renderItems;
        m_renderItems = 0;
    }
}


// ---- src/ScreenIconManager/Create.cpp

ScreenIconManager *ScreenIconManager::Create(int maxIcons)
{
    m_maxIcons = maxIcons < 0x80 ? maxIcons : 0x80;
    if (maxIcons > 0) {
        m_iconQueue = new ScreenIcon *[m_maxIcons];
        m_icons = new ScreenIcon[m_maxIcons];
        m_renderItems = new DmaRenderItem[m_maxIcons];
    }
    for (int i = 0; i < m_maxIcons; i++)
        m_icons[i].CreateClone();
    return this;
}


// ---- src/ScreenIconManager/AddIcon.cpp


void ScreenIconManager::AddIcon(ScreenIcon *icon, int copy)
{
    if (m_numQueued < m_maxIcons) {
        if (!copy) {
            m_iconQueue[m_numQueued] = icon;
        } else {
            m_icons[m_numQueued].Clone(icon);
            m_iconQueue[m_numQueued] = &m_icons[m_numQueued];
        }
        m_numQueued++;
    }
}


// ---- src/ScreenIconManager/Render3D.cpp
// LpRenderTarget is the PSXRenderTarget (GameObjectGlobals.h declares the
// base class).
static inline PSXRenderTarget *Target() { return (PSXRenderTarget *)LpRenderTarget; }


int ScreenIconManager::Render3D()
{
    static int timerID = -1;    // (unused; left from profiling code)
    if (Target()->m_xC4 > 0)  // rendering paused
        return 1;
    if (m_x40)
        RenderQueuedIcons();
    m_numQueued = 0;
    return 1;
}


// ---- src/ScreenIconManager/RenderQueuedIcons.cpp
// VU1 microcode packet for 2D quads (also used by Overlay::Render3D).
extern unsigned int Quad2DRendererCode[];

void ScreenIconManager::RenderQueuedIcons()
{
    if (m_numQueued > 0 && m_renderItems && m_iconQueue) {
        Target()->AddMPGToRenderQueue(Quad2DRendererCode);
        for (int i = 0; i < m_numQueued; i++) {
            AddIconToRenderItem(&m_renderItems[i], m_iconQueue[i]);
            Target()->AddToRenderQueue(&m_renderItems[i], "");
            ComposeRenderItem(m_iconQueue[i]);
            Target()->AddToRenderQueue((_sceDmaTag *)m_iconQueue[i]->m_vifData, 1, "");
        }
    }
}


// ---- src/ScreenIconManager/AddIconToRenderItem.cpp


extern PSXTextureMapManager *LpTextureMapManager;


// Set up the icon's render state and compose it into the item.
void ScreenIconManager::AddIconToRenderItem(DmaRenderItem *item, ScreenIcon *icon)
{
    PSXRenderStateBlock state;
    if (icon->m_texture) {
        LpTextureMapManager->AddTextureToGifList(icon->m_texture);
        LpTextureMapManager->AddTextureInterruptToRenderQueue();
        state.SetTextureMap(icon->m_texture);
    }
    state.AlphaTestEnable(icon->m_alphaTest);
    state.SetAlphaBlendMode(icon->m_alphaBlendMode, icon->m_alphaFix);
    state.SetAlphaCompare(icon->m_alphaCompare);
    state.SetAlphaRefValue(icon->m_alphaRef);
    state.SetAlphaFailMethod(icon->m_alphaFail);
    state.ZTestEnable(icon->m_zBuffer);
    state.ZWriteEnable(icon->m_zBuffer);
    state.SetTextureAddressMode(icon->m_addressMode);
    item->Start((DmaRenderDestination)1, 0x80);
    if (icon->m_texture)
        state.ComposeRenderItem(item);
    else
        state.ComposeSimpleRenderItem(item);
    item->End();
}


// ---- src/ScreenIconManager/ComposeRenderItem.cpp
// ComposeRenderItem__17ScreenIconManagerFP10ScreenIcon  @ 0x003720C0

#pragma fast_fptosi on

// One quadword of the icon's VIF data, as ints or floats.
struct IconColor {
    int r, g, b, a;
};
struct IconUV {
    float u, v, q, pad;
};

// Rebuild the parts of the icon's VIF packet its setters marked dirty:
// the screen size, the GS colour, the four corners (rotated about the
// pivot) and the texture coordinates.
void ScreenIconManager::ComposeRenderItem(ScreenIcon *icon)
{
    static int timerID = -1; // profiler ID (unused)
    float y0;
    float y1;
    float x0;
    float x1;
    float z;
    float offset;
    offset = 0.0f;
    // One pointer for all four qwords: retail keeps it in one register.
    IconUV *q = (IconUV *)icon->m_vifQwords[0];
    q->u = LpRenderTarget->m_camera->m_viewport.Width;
    q->v = LpRenderTarget->m_camera->m_viewport.Height;
    q->q = 0.0f;
    q->pad = 0.0f;
    if (icon->m_colorDirty || icon->m_dirty || icon->m_uvDirty) {
        if (icon->m_colorDirty) {
            IconColor color;
            color.r = ((icon->m_color >> 16) & 0xFF) >> 1;
            color.g = ((icon->m_color >> 8) & 0xFF) >> 1;
            color.b = (icon->m_color & 0xFF) >> 1;
            color.a = icon->m_alphaFix >> 2;
            q = (IconUV *)icon->m_vifQwords[1];
            *(IconColor *)q = color;
            icon->m_colorDirty = 0;
        }
        if (icon->m_dirty) {
            int ix = (int)icon->m_position.x;
            int iy = (int)icon->m_position.y;
            int iw = (int)(offset + icon->m_width * icon->m_34);
            int ih = (int)(offset + icon->m_height * icon->m_34);
            x0 = ix;
            y0 = iy;
            x1 = x0 + iw;
            y1 = y0 + ih;
            // Depth read three times (mwcc keeps one load); a named local colours differently.
            if (icon->m_depth < 0.0f)
                z = -icon->m_depth;
            else
                z = LpVideoCard->m_maxDepth - icon->m_depth;
            _D3DVECTOR v[4];
            v[0].x = x0; v[0].y = y0; v[0].z = z;
            v[1].x = x1; v[1].y = y0; v[1].z = z;
            v[2].x = x0; v[2].y = y1; v[2].z = z;
            v[3].x = x1; v[3].y = y1; v[3].z = z;
            v[0].w = v[1].w = v[2].w = v[3].w = 0.0f;
            if (icon->m_angle != 0.0f) {
                float px = (int)icon->m_pivot.x;
                float py = (int)icon->m_pivot.y;
                float cosine, sine;
                FastSinCos(icon->m_angle, &sine, &cosine);
                for (int i = 0; i < 4; i++) {
                    float nx = px + (cosine * (v[i].x - px) - sine * (v[i].y - py));
                    float ny = py + (sine * (v[i].x - px) + cosine * (v[i].y - py));
                    v[i].x = nx;
                    v[i].y = ny;
                }
            }
            q = (IconUV *)icon->m_vifQwords[2];
            asm("lqc2 vf1, 0x0(%0); lqc2 vf2, 0x10(%0); lqc2 vf3, 0x20(%0); lqc2 vf4, 0x30(%0); sqc2 vf1, 0x0(%1); sqc2 vf2, 0x10(%1); sqc2 vf3, 0x20(%1); sqc2 vf4, 0x30(%1)"
                : : "r"(v), "r"(q) : "memory");
            icon->m_dirty = 0;
        }
        if (icon->m_uvDirty) {
            q = (IconUV *)icon->m_vifQwords[3];
            q->u = icon->m_u0;
            q->v = icon->m_v0;
            q->q = 1.0f;
            q->pad = 0.0f;
            q++;
            q->u = icon->m_u1;
            q->v = icon->m_v1;
            q->q = 1.0f;
            q->pad = 0.0f;
            icon->m_uvDirty = 0;
        }
    }
}


// ---- src/sinit/ScreenIconManager.cpp
// Static initializer of ScreenIconManager.cpp: only the axis vectors from the shared header.

