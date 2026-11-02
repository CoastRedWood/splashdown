// ScreenPrimitiveManager.cpp  @ 0x00370100 (text 0x370100-0x3712B0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's ScreenPrimitiveManager.cpp as one translation unit: its functions in
// retail order, then its static initializer. `make link` places the object's data
// as well as its code (tools/link_c.py).



#include "ScreenPrimitive/ScreenPrimitive.h"
#include "PSXRenderTarget/PSXRenderTarget.h"
#include "DmaRenderItem/DmaRenderItem.h"
#include "PSXRenderStateBlock/PSXRenderStateBlock.h"
#include "PSXTextureMap/PSXTextureMap.h"
#include "soultree_assert_block.h"
#include "d3dtypes.h"
#include "sceGifTag/sceGifTag.h"
#include "PSXVideoCard/PSXVideoCard.h"
#include "ScreenPrimitiveManager/ScreenPrimitiveManager.h"
#include "GameObject/GameObject.h"
#include "PSXTextureMapManager/PSXTextureMapManager.h"
#include "GameObject/GameObjectGlobals.h"
#include "Camera/Camera.h"
#include "RenderTarget/RenderTarget.h"
#include "sinit/AxisVectors.h"

extern "C" void *memalign(unsigned int align, unsigned int size);
extern "C" void *memset(void *dst, int c, unsigned int n);
extern "C" void free(void *);
typedef int int128_t __attribute__((mode(TI)));
struct Palette8;
struct _sceDmaTag;

PSXTextureMap *GetTextureMapFromResourceName(char *name, int flags, Palette8 *palette, unsigned int a,
                                             _D3DBLEND src, _D3DBLEND dst, int alpha, unsigned int colorKey, int b);
void FastSinCos(float x, float *sine, float *cosine);
extern PSXVideoCard *LpVideoCard;
extern PSXTextureMapManager *LpTextureMapManager;
extern unsigned int Polyline2DRendererCode[];
extern unsigned int Triangle2DRendererCode[];
extern unsigned int Rectangle2DRendererCode[];

// LpRenderTarget is the PSXRenderTarget (GameObjectGlobals.h declares the
// base class).
static inline PSXRenderTarget *Target() { return (PSXRenderTarget *)LpRenderTarget; }

// A primitive's VIF packet (CreateVifDataItem).
struct QWORD {
    unsigned int x, y, z, w;
};

struct QWORDF {
    float x, y, z, w;
};

struct PrimitivePacket {
    unsigned int m_vif[4];              // +0x00
    unsigned int m_unpack[4];           // +0x10
    sceGifTag m_gifTag;                 // +0x20
    QWORD m_x30;                        // +0x30
};

class VifDataItem {
public:
    short m_numQwords;                  // +0x00
    unsigned char m_02;                 // +0x02
    unsigned char m_03;                 // +0x03
    PS2_PTR(PrimitivePacket) m_data;            // +0x04
    int m_08;                           // +0x08
    int m_0C;                           // +0x0C
    PrimitivePacket m_packet;           // +0x10
};

// One vertex the primitive's user supplies (0x30 bytes): its position, and
// where CreateVifDataItem put its slots in the DMA buffer.
class ScreenPrimitiveVertex {
public:
    float x, y, z;                      // +0x00
    char pad0C[0x20 - 0x0C];
    QWORD *m_position;                  // +0x20, DMA buffer slot (UpdateDmaBuffer writes the point)
    QWORD *m_color;                     // +0x24
    QWORDF *m_uv;                       // +0x28
    char pad2C[0x30 - 0x2C];
};


// ---- src/ScreenPrimitive/ctor.cpp

ScreenPrimitive::ScreenPrimitive()
{
    m_type = (ScreenPrimitiveType)2;
    m_vertices = 0;
    m_numVertices = 0;
    m_alphaTest = 0;
    m_alphaBlendMode = 0;
    m_alphaCompare = 6;
    m_alphaRefValue = 0;
    m_alphaFailMethod = 0;
    m_scale = 1.0f;
    m_angle = 0;
    m_z = 1.0f;
    m_zEnable = 0;
    m_center.x = m_center.y = 0;
    m_dirty = 0;
    m_texture = 0;
    m_renderItem = 0;
    m_stateBlock = 0;
    m_buffers = 0;
    m_numBuffers = 0;
}


// ---- src/ScreenPrimitive/dtor.cpp


ScreenPrimitive::~ScreenPrimitive()
{
    LpRenderTarget->TrySendRenderQueue(1);
    LpRenderTarget->WaitForRenderingToComplete();
    if (m_texture)
        m_texture->Release();
    if (m_buffers) {
        for (int i = 0; i < m_numBuffers; i++) {
            if (m_buffers[i].m_item)
                free(m_buffers[i].m_item);
        }
        delete[] m_buffers;
    }
    if (m_renderItem)
        delete m_renderItem;
    if (m_stateBlock)
        delete m_stateBlock;
}


// ---- src/ScreenPrimitive/Create.cpp


// Each DMA buffer holds up to 16 vertices.
ScreenPrimitive *ScreenPrimitive::Create(ScreenPrimitiveVertex *vertices, int numVertices, ScreenPrimitiveType type,
                                         char *textureName)
{
    int i;
    int remaining;
    int count;
    int start;
    int size;
    int n;          // only the chained assignment below uses it (retail register shape)
    m_type = type;
    m_numVertices = numVertices;
    m_vertices = vertices;
    m_dirty = 1;
    if (textureName) {
        m_texture = GetTextureMapFromResourceName(textureName, 0x22B8, 0, 0, (_D3DBLEND)2, (_D3DBLEND)1, 0x80,
                                                  0xFF00FF, 1);
#line 89 "ScreenPrimitiveManager.cpp"
        ASSERTF(m_texture != 0, ("Could not load texture map %s for this primitive", textureName));
    }
    m_renderItem = new DmaRenderItem;
    m_stateBlock = new PSXRenderStateBlock;
    if (m_texture)
        m_stateBlock->SetTextureMap(m_texture);
    m_stateBlock->AlphaTestEnable(m_alphaTest);
    m_stateBlock->SetAlphaBlendMode(m_alphaBlendMode);
    m_stateBlock->SetAlphaCompare(m_alphaCompare);
    m_stateBlock->SetAlphaRefValue(m_alphaRefValue);
    m_stateBlock->SetAlphaFailMethod(m_alphaFailMethod);
    m_stateBlock->ZTestEnable(m_zEnable);
    m_stateBlock->ZWriteEnable(m_zEnable);
    if (m_texture) {
        m_stateBlock->SetTextureAddressMode(1);
        m_stateBlock->SetTextureFilter((_D3DTEXTUREFILTERTYPE)1);
    }
    remaining = m_numVertices;
    start = 0;
    if (remaining % 16) m_numBuffers = n = m_numVertices / 16 + 1; else m_numBuffers = n = m_numVertices / 16;
    m_buffers = new ScreenPrimitiveDmaBuffer[m_numBuffers];
    for (i = 0; i < m_numBuffers; i++) {
        if (remaining / 16 == 0) count = remaining; else count = 16;
        size = (count * 3 + 5) * 16 + 16;
        m_buffers[i].m_item = (VifDataItem *)memalign(0x40, size);
        memset(m_buffers[i].m_item, 0, size);
        CreateVifDataItem(&m_buffers[i], count, start);
        start += count;
        remaining -= count;
    }
    return this;
}


// ---- src/ScreenPrimitive/CreateVifDataItem.cpp


void ScreenPrimitive::CreateVifDataItem(ScreenPrimitiveDmaBuffer *buffer, int count, int start)
{

    QWORD *q;
    int textured;
    sceGifTag *tag;
    int numQwords;
    QWORDF *uv;
    int i;

    q = (QWORD *)&buffer->m_item->m_packet;
    q[0].x = 0x11000000;                // FLUSH
    q[0].y = 0x03000000;                // BASE
    q[0].z = 0x02000200;                // OFFSET
    q[0].w = 0;
    textured = m_texture != 0;
    numQwords = count * 3;
    buffer->m_unpack = &q[1].x;
    q[1].x = 0;
    q[1].y = 0;
    q[1].z = 0;
    q[1].w = ((numQwords + 2) << 16) | 0x6C008000;     // UNPACK
    tag = (sceGifTag *)&q[2];
    *(int128_t *)tag = 0;
    tag->EOP = 1;
    tag->PRE = 1;
    if (m_type == 2) {
        tag->PRIM = ((long)textured << 4) | 0xB | ((long)m_alphaTest << 6);
        tag->FLG = 0;
        tag->NREG = 3;
        tag->REGS0 = 2;
        tag->REGS1 = 1;
        tag->REGS2 = 5;
    } else if (m_type == 1) {
        tag->PRIM = ((long)textured << 4) | 0xC | ((long)m_alphaTest << 6);
        tag->FLG = 0;
        tag->NREG = 3;
        tag->REGS0 = 2;
        tag->REGS1 = 1;
        tag->REGS2 = 5;
    } else if (m_type == 3) {
        tag->PRIM = ((long)m_alphaTest << 6) | 0xA;
        tag->FLG = 0;
        tag->NREG = 2;
        tag->REGS0 = 1;
        tag->REGS1 = 5;
    }
    tag->NLOOP = count;
    buffer->m_gifTag = (sceGifTag *)&q[2];
    q[3].x = q[3].y = q[3].z = q[3].w = 0;
    buffer->m_x0C = &q[3];
    q += 4;
    for (i = 0; i < count; i++) {
        q[0].x = 0;
        q[0].y = 0;
        q[0].z = 0;
        q[0].w = 0;
        m_vertices[start].m_color = &q[0];
        q[1].x = 0;
        q[1].y = 0;
        q[1].z = 0;
        q[1].w = 0;
        m_vertices[start].m_position = &q[1];
        uv = (QWORDF *)&q[2];
        uv->x = 0.0f;
        uv->y = 0.0f;
        uv->z = 1.0f;
        uv->w = 0.0f;
        m_vertices[start].m_uv = (QWORDF *)&q[2];     // recomputed rather than reusing uv, as retail
        q += 3;
        start++;
    }
    q[0].x = 0x14000019;                // MSCAL
    q[0].y = 0x10000000;                // FLUSHE
    q[0].z = 0;
    q[0].w = 0;
    VifDataItem *item = buffer->m_item;
    item->m_numQwords = numQwords + 5;
    item->m_02 = 0;
    item->m_03 = 0x30;
    item->m_data = &item->m_packet;
    item->m_08 = 0;
    item->m_0C = 0;
}


// ScreenPrimitive's point buffer, constructed by the static initializer
// (with the axis vectors); retail's .bss has it after Create's static and
// before Render3D's.
_D3DVECTOR ScreenPrimitive::LpPoint[100];

// ---- src/ScreenPrimitive/UpdateDmaBuffer.cpp
#pragma fast_fptosi on


void ScreenPrimitive::UpdateDmaBuffer()
{
    float width = LpRenderTarget->m_camera->m_viewport.Width;
    float height = LpRenderTarget->m_camera->m_viewport.Height;

    int i;
    float offset;
    float z;
    float cosine;
    float sine;

    for (i = 0; i < m_numBuffers; i++) {
        ((float *)m_buffers[i].m_x0C)[0] = width;
        ((float *)m_buffers[i].m_x0C)[1] = height;
        ((float *)m_buffers[i].m_x0C)[3] = 0;
        ((float *)m_buffers[i].m_x0C)[2] = 0;
    }
    if (m_dirty) {
        // j and rotate are block-scoped, declared in this order: that gives
        // retail's registers (j in $a0, rotate in $a1).
        int j;
        int rotate;

        if (m_z < 0.0f)
            z = -m_z;
        else
            z = (float)LpVideoCard->m_maxDepth - m_z;
        rotate = 0;
        if (m_angle != 0.0f) {
            FastSinCos(m_angle, &sine, &cosine);
            rotate = 1;
        }
        offset = 0.0f;
        for (j = 0; j < m_numVertices; j++) {
            LpPoint[j].x = (int)(offset + m_scale * m_vertices[j].x);
            LpPoint[j].y = (int)(offset + m_scale * m_vertices[j].y);
            LpPoint[j].z = m_vertices[j].z = z;
            if (rotate) {
                float cx = (int)(m_center.x + offset);
                float cy = (int)(m_center.y + offset);
                float dx = LpPoint[j].x - cx;
                float dy = LpPoint[j].y - cy;
                LpPoint[j].x = cx + (dx * cosine - dy * sine);
                LpPoint[j].y = cy + (dx * sine + dy * cosine);
            }
            if (m_vertices[j].m_position)
                *(_D3DVECTOR *)m_vertices[j].m_position = LpPoint[j];
        }
        m_dirty = 0;
    }
}


// ---- src/ScreenPrimitive/UpdateTextureRenderItem.cpp
#pragma fast_fptosi reset


void ScreenPrimitive::UpdateTextureRenderItem()
{
    if (m_stateBlock) {
        m_stateBlock->AlphaTestEnable(m_alphaTest);
        m_stateBlock->SetAlphaBlendMode(m_alphaBlendMode);
        m_stateBlock->SetAlphaCompare(m_alphaCompare);
        m_stateBlock->SetAlphaRefValue(m_alphaRefValue);
        m_stateBlock->SetAlphaFailMethod(m_alphaFailMethod);
        m_stateBlock->ZTestEnable(m_zEnable);
        m_stateBlock->ZWriteEnable(m_zEnable);
    }
    if (m_renderItem) {
        m_renderItem->Start((DmaRenderDestination)1, 0x80);
        if (m_texture)
            m_stateBlock->ComposeRenderItem(m_renderItem);
        else
            m_stateBlock->ComposeSimpleRenderItem(m_renderItem);
        m_renderItem->End();
    }
}


// ---- src/ScreenPrimitiveManager/ctor.cpp

ScreenPrimitiveManager::ScreenPrimitiveManager(int enabled) : GameObject(enabled)
{
    m_numPrimitives = 0;
    m_queued = 1;
    m_maxPrimitives = 0;
    m_primitives = 0;
}


// ---- src/ScreenPrimitiveManager/dtor.cpp

ScreenPrimitiveManager::~ScreenPrimitiveManager()
{
    if (m_primitives)
        delete[] m_primitives;
}


// ---- src/ScreenPrimitiveManager/Create.cpp


ScreenPrimitiveManager *ScreenPrimitiveManager::Create(int maxPrimitives)
{
    m_maxPrimitives = (maxPrimitives < 128) ? maxPrimitives : 128;
    if (m_maxPrimitives > 0)
        m_primitives = new ScreenPrimitive *[m_maxPrimitives];
    GameObject::Create();
    return this;
}


// ---- src/ScreenPrimitiveManager/Render3D.cpp
// Profiling timer registration, compiled out in retail: the timer id is
// still a guarded function-local static.
inline int RegisterTimer() { return -1; }


int ScreenPrimitiveManager::Render3D()
{
    static int SPrimMgrRndr3DID = RegisterTimer();
    if (m_queued)
        RenderQueuedPrimitives();
    m_numPrimitives = 0;
    return 1;
}


// ---- src/ScreenPrimitiveManager/AddPrimitive.cpp
void ScreenPrimitiveManager::AddPrimitive(ScreenPrimitive *p)
{
    if (m_numPrimitives < m_maxPrimitives)
        m_primitives[m_numPrimitives++] = p;
}


// ---- src/ScreenPrimitiveManager/RenderQueuedPrimitives.cpp
// RenderQueuedPrimitives__22ScreenPrimitiveManagerFv  @ 0x00370EF0
// Register colouring notes: `int i = 0; int j = 0;` are dead initialisers that only fix the order
// in which mwcc's web splitting numbers the loop-2/3 copies of i, j and loaded (variables split in
// order of first appearance; retail's order is i, j, loaded), and `loaded = 0, i = 0` in the later
// for-inits sets the order of the two zero moves. Retail passes "" as the render-queue name.


// Queues the primitives by type (polylines, triangles, rectangles), loading
// each type's VU microprogram before its first primitive.
void ScreenPrimitiveManager::RenderQueuedPrimitives()
{
    int i = 0;
    int j = 0;
    int loaded = 0;
    for (i = 0; i < m_numPrimitives; i++) {
        if (m_primitives[i]->m_type != 3)
            continue;
        if (!loaded) {
            Target()->AddMPGToRenderQueue(Polyline2DRendererCode);
            loaded = 1;
        }
        ScreenPrimitive *primitive = m_primitives[i];
        primitive->UpdateDmaBuffer();
        primitive->UpdateTextureRenderItem();
        Target()->AddToRenderQueue(m_primitives[i]->m_renderItem, "");
        for (j = 0; j < m_primitives[i]->m_numBuffers; j++)
            Target()->AddToRenderQueue((_sceDmaTag *)m_primitives[i]->m_buffers[j].m_item, 1, "");
    }
    for (loaded = 0, i = 0; i < m_numPrimitives; i++) {
        if (m_primitives[i]->m_type != 2)
            continue;
        if (!loaded) {
            Target()->AddMPGToRenderQueue(Triangle2DRendererCode);
            loaded = 1;
        }
        ScreenPrimitive *primitive = m_primitives[i];
        primitive->UpdateDmaBuffer();
        primitive->UpdateTextureRenderItem();
        if (m_primitives[i]->m_texture) {
            LpTextureMapManager->AddTextureToGifList(m_primitives[i]->m_texture);
            LpTextureMapManager->AddTextureInterruptToRenderQueue();
        }
        Target()->AddToRenderQueue(m_primitives[i]->m_renderItem, "");
        for (j = 0; j < m_primitives[i]->m_numBuffers; j++)
            Target()->AddToRenderQueue((_sceDmaTag *)m_primitives[i]->m_buffers[j].m_item, 1, "");
    }
    for (loaded = 0, i = 0; i < m_numPrimitives; i++) {
        if (m_primitives[i]->m_type != 1)
            continue;
        if (!loaded) {
            Target()->AddMPGToRenderQueue(Rectangle2DRendererCode);
            loaded = 1;
        }
        ScreenPrimitive *primitive = m_primitives[i];
        primitive->UpdateDmaBuffer();
        primitive->UpdateTextureRenderItem();
        if (m_primitives[i]->m_texture) {
            LpTextureMapManager->AddTextureToGifList(m_primitives[i]->m_texture);
            LpTextureMapManager->AddTextureInterruptToRenderQueue();
        }
        Target()->AddToRenderQueue(m_primitives[i]->m_renderItem, "");
        for (j = 0; j < m_primitives[i]->m_numBuffers; j++)
            Target()->AddToRenderQueue((_sceDmaTag *)m_primitives[i]->m_buffers[j].m_item, 1, "");
    }
}
