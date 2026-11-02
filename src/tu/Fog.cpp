// Fog.cpp  @ 0x00235030 (text 0x235030-0x235D60)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// A translation unit without a static initializer between EcoSystem.cpp and
// Griddraw.cpp: Fog, RotateAroundAxis and Parametric2DLineIntersection. Its
// .rodata restarts at @551 after the EcoSystem RTTI and ends with the Fog
// RTTI (own BaseObject/GameObject copies, @709-@723); its .bss is
// InFogEditMode (0x451EE0). DrawableGridNodeSharedTextures.cpp and
// GridNodeGeometry.cpp follow. The file name is a guess (alphabetical link
// order puts it between EcoSystem.cpp and Griddraw.cpp).





#include "GameObject/GameObject.h"
#include "DmaRenderItem/DmaRenderItem.h"
#include "Fog/Fog.h"
#include "RenderTarget/RenderTarget.h"
#include "PSXRenderTarget/PSXRenderTarget.h"
#include "Camera/Camera.h"
#include "sceGifTag/sceGifTag.h"
#include "d3dtypes.h"
#include "TextureMap/TextureMap.h"
#include "PSXTextureMap/PSXTextureMap.h"
#include "Terrain/Terrain.h"
#include "AgeManager/AgeManager.h"
#include "DrawableGridNodeSharedTextures/DrawableGridNodeSharedTextures.h"
#include "DrawableGridNode/DrawableGridNode.h"
#include "GridNodeGeometryBase/GridNodeGeometryBase.h"
#include "GridNodeGeometryLOD/GridNodeGeometryLOD.h"
#include "DataAccess/DataAccess.h"
#include "ControlInterface/ControlTypes.h"
#include "GridNodeGeometryPlain/GridNodeGeometryPlain.h"

extern "C" void *malloc(unsigned int size);
extern "C" void free(void *p);

#define D3DTSS_COLOROP ((_D3DTEXTURESTAGESTATETYPE)1)
#define D3DTSS_ALPHAOP ((_D3DTEXTURESTAGESTATETYPE)4)
#define D3DTOP_DISABLE 1

// Fog.cpp's own global (.bss 0x451EE0).
int InFogEditMode;

// ---- src/global/ctor.cpp

Fog::Fog(int enabled)
    : GameObject(enabled)
{
    m_x44 = 0;
    m_x4C = 0;
    m_x48 = 1;
    m_changed = 1;
    m_renderItem = 0;
}


// ---- src/global/dtor.cpp



Fog::~Fog()
{
    if (m_renderItem)
        delete m_renderItem;
}


// ---- src/global/Create.cpp



Fog *Fog::Create(unsigned int color, float visibility, float haziness, float maxFar, float minFar, float minNear)
{
    GameObject::Create();
    m_maxFar = maxFar;
    m_minFar = minFar;
    m_minNear = minNear;
    m_renderItem = new DmaRenderItem;
    Change(color, visibility, haziness);
    return this;
}


// ---- src/global/Change.cpp
// Compiler session state: retail compiled
// this file in an mwcc session that had already generated a two-argument runtime
// helper call (64-bit / or %, double arithmetic or compare). mwcc then models
// every later helper call (__floatdisf, fptoui, fptodp, ...) as reading $a0-$a1,
// so temporaries live before such a call skip $a0/$a1. This unused function
// recreates that state; it is not part of the retail file.
static void MWCC_SHIM_TEXT MwccSessionState(long *a) { a[0] = a[0] / a[1]; }

void PSXprintf(const char *fmt, ...);


extern RenderTarget *LpRenderTarget;



void Fog::Change(unsigned int color, float visibility, float haziness)
{
    m_color = color | 0xFF000000;
    m_visibility = visibility;
    m_haziness = haziness;
    m_far = m_minFar + m_visibility * (m_maxFar - m_minFar);
    m_near = (1.0f - m_haziness) * (m_far - m_minNear);
    LpRenderTarget->m_clearColor = m_color;
    m_changed = 1;
    PSXprintf("FOG: Visibility = %2.2f Haziness = %2.2f MaxFar %2.2f R = %d G = %d B = %d\n",
              m_visibility, m_haziness, m_maxFar,
              (m_color & 0xFF0000) >> 16, (m_color & 0xFF00) >> 8, m_color & 0xFF);
}


// ---- (Tick)
int Fog::Tick(float dt)
{
    return 1;
}


// ---- src/global/PrepareGeometry.cpp

struct _D3DMATRIX;
struct _D3DVECTOR;



extern RenderTarget *LpRenderTarget;


int Fog::PrepareGeometry()
{
    LpRenderTarget->m_camera->SetClipPlanes(1.0f, m_far);
    LpRenderTarget->m_camera->SetTransformProjection(0);
    return 1;
}


// ---- src/Fog/PSXRender3D.cpp
typedef int int128_t __attribute__((mode(TI)));


// A+D data: a 64-bit register value and the register's address.
struct AdQword {
    unsigned long data;
    unsigned long addr;
};

// Sets the camera clip planes and fog range; when the fog changed, sends its
// colour to the GS (FOGCOL through an A+D packet).
int Fog::PSXRender3D()
{
    if (m_enabled) {
        LpRenderTarget->m_camera->SetClipPlanes(1.0f, m_far);
        if (!LpRenderTarget->m_camera->SetTransformProjection(0))
            goto fail;
        LpRenderTarget->SetRenderState((_D3DRENDERSTATETYPE)0x24, *(unsigned int *)&m_near, 0);   // D3DRS_FOGSTART
        LpRenderTarget->SetRenderState((_D3DRENDERSTATETYPE)0x25, *(unsigned int *)&m_far, 0);    // D3DRS_FOGEND
        if (m_changed) {
            sceGifTag tag;
            AdQword fogcol;
            *(int128_t *)&tag = 0;
            m_renderItem->Start((DmaRenderDestination)1, 0x80);
            m_renderItem->DmaAddVifCode(0x11000000);    // FLUSH
            m_renderItem->DmaAddVifCode(0x03000000);    // BASE
            m_renderItem->DmaAddVifCode(0x02000000);    // OFFSET
            m_renderItem->DmaBeginUnpack(0x320, 12);
            tag.FLG = 0;
            tag.EOP = 1;
            tag.NREG = 1;
            tag.NLOOP = 1;
            tag.REGS0 = 0xE;    // A+D
            m_renderItem->DmaAddGifTagToUnpack(&tag);
            fogcol.data = (unsigned long)((m_color >> 16) & 0xFF) | ((unsigned long)((m_color >> 8) & 0xFF) << 8) |
                          ((unsigned long)(m_color & 0xFF) << 16);
            fogcol.addr = 0x3D;     // FOGCOL
            m_renderItem->DmaAddQwordToUnpack((QWORD *)&fogcol);
            m_renderItem->DmaEndUnpack();
            m_renderItem->DmaAddVifCode(0x14000000);    // MSCAL
            m_renderItem->End();
            ((PSXRenderTarget *)LpRenderTarget)->AddToRenderQueue(m_renderItem, "");
            m_changed = 0;
        }
    }
    return 1;
fail:
    return 0;
}


// ---- src/Fog/Render3D.cpp

int Fog::Render3D()
{
    return PSXRender3D();
}


// ---- src/Fog/KeyDownHandler.cpp
class ProcessNode;
class KeyInfo;

int KEYDOWN_HANDLER_TEST(unsigned int key, DeviceType device, ProcessNode *node, unsigned int mods);
void PSXprintf(const char *fmt, ...);



// Fog edit mode: toggled with one key, then keys nudge the visibility,
// haziness, colour channels and m_x50 (the second modifier lowers them).
int Fog::KeyDownHandler(ProcessNode *node, KeyInfo *)
{
    if (KEYDOWN_HANDLER_TEST(9, DEVICE_KEYBOARD, node, 0x80)) {
        InFogEditMode = 1 - InFogEditMode;
        if (InFogEditMode) {
            unsigned int color = m_color;
            PSXprintf("FOG: In Fog Edit Mode. Visibility = %f Haxiness = %f R = %d G = %d B = %d\n",
                      (double)m_visibility, (double)m_haziness, (color & 0xFF0000) >> 16, (color & 0xFF00) >> 8,
                      color & 0xFF);
        }
    }
    if (InFogEditMode) {
        if (KEYDOWN_HANDLER_TEST(0x19, DEVICE_KEYBOARD, node, 1)) {
            m_visibility += 0.01f;
            if (m_visibility > 1.0f)
                m_visibility = 1.0f;
            Change(m_color, m_visibility, m_haziness);
            return 1;
        }
        if (KEYDOWN_HANDLER_TEST(0x19, DEVICE_KEYBOARD, node, 4)) {
            m_visibility -= 0.01f;
            if (m_visibility < 0.0f)
                m_visibility = 0.0f;
            Change(m_color, m_visibility, m_haziness);
            return 1;
        }
        if (KEYDOWN_HANDLER_TEST(0xB, DEVICE_KEYBOARD, node, 1)) {
            m_haziness += 0.01f;
            if (m_haziness > 1.0f)
                m_haziness = 1.0f;
            Change(m_color, m_visibility, m_haziness);
            return 1;
        }
        if (KEYDOWN_HANDLER_TEST(0xB, DEVICE_KEYBOARD, node, 4)) {
            m_haziness -= 0.01f;
            if (m_haziness < 0.0f)
                m_haziness = 0.0f;
            Change(m_color, m_visibility, m_haziness);
            return 1;
        }
        if (KEYDOWN_HANDLER_TEST(0x15, DEVICE_KEYBOARD, node, 1)) {
            int r = (m_color & 0xFF0000) >> 16;
            if (r < 0xFF) {
                m_color = (m_color & 0xFF00FFFF) | ((r + 1) << 16);
                Change(m_color, m_visibility, m_haziness);
            }
            return 1;
        }
        if (KEYDOWN_HANDLER_TEST(0x15, DEVICE_KEYBOARD, node, 4)) {
            int r = (m_color & 0xFF0000) >> 16;
            if (r > 0) {
                m_color = (m_color & 0xFF00FFFF) | ((r - 1) << 16);
                Change(m_color, m_visibility, m_haziness);
            }
            return 1;
        }
        if (KEYDOWN_HANDLER_TEST(0xA, DEVICE_KEYBOARD, node, 1)) {
            int g = (m_color & 0xFF00) >> 8;
            if (g < 0xFF) {
                m_color = (m_color & 0xFFFF00FF) | ((g + 1) << 8);
                Change(m_color, m_visibility, m_haziness);
            }
            return 1;
        }
        if (KEYDOWN_HANDLER_TEST(0xA, DEVICE_KEYBOARD, node, 4)) {
            int g = (m_color & 0xFF00) >> 8;
            if (g > 0) {
                m_color = (m_color & 0xFFFF00FF) | ((g - 1) << 8);
                Change(m_color, m_visibility, m_haziness);
            }
            return 1;
        }
        if (KEYDOWN_HANDLER_TEST(5, DEVICE_KEYBOARD, node, 1)) {
            int b = m_color & 0xFF;
            if (b < 0xFF) {
                m_color = (m_color & 0xFFFFFF00) | (b + 1);
                Change(m_color, m_visibility, m_haziness);
            }
            return 1;
        }
        if (KEYDOWN_HANDLER_TEST(5, DEVICE_KEYBOARD, node, 4)) {
            int b = m_color & 0xFF;
            if (b > 0) {
                m_color = (m_color & 0xFFFFFF00) | (b - 1);
                Change(m_color, m_visibility, m_haziness);
            }
            return 1;
        }
        if (KEYDOWN_HANDLER_TEST(0x1D, DEVICE_KEYBOARD, node, 1)) {
            m_maxFar += 10.0f;
            Change(m_color, m_visibility, m_haziness);
            return 1;
        }
        if (KEYDOWN_HANDLER_TEST(0x1D, DEVICE_KEYBOARD, node, 4)) {
            m_maxFar -= 10.0f;
            Change(m_color, m_visibility, m_haziness);
            return 1;
        }
    }
    return 0;
}


// ---- src/global/RotateAroundAxis.cpp

void FastSinCos(float angle, float *s, float *c);

// VU0 xyz dot product.
static inline float DotProduct(_D3DVECTOR *a, _D3DVECTOR *b)
{
    float r;
    asm("lqc2 vf4, 0x0(%1); lqc2 vf5, 0x0(%2); vmul.xyz vf5, vf4, vf5; vaddy.x vf5, vf5, vf5y; vaddz.x vf5, vf5, vf5z; qmfc2 $2, vf5; mtc1 $2, %0"
        : "=f"(r) : "r"(a), "r"(b) : "$2");
    return r;
}

// Rotate v by angle about the unit vector axis (Rodrigues' formula).
int RotateAroundAxis(float angle, _D3DVECTOR axis, _D3DVECTOR v, _D3DVECTOR *out)
{
    _D3DVECTOR perp;
    _D3DVECTOR cross;
    _D3DVECTOR proj;
    float c, s;
    if (!out)
        return 0;
    FastSinCos(angle, &s, &c);
    float d = DotProduct(&axis, &v);
    proj.x = d * axis.x;
    proj.y = d * axis.y;
    proj.z = d * axis.z;
    perp.x = v.x - proj.x;
    perp.y = v.y - proj.y;
    perp.z = v.z - proj.z;
    // axis x perp, operands written perp-first as retail multiplies them.
    cross.x = perp.z * axis.y - perp.y * axis.z;
    cross.y = perp.x * axis.z - perp.z * axis.x;
    cross.z = perp.y * axis.x - perp.x * axis.y;
    out->x = proj.x + (c * perp.x + s * cross.x);
    out->y = proj.y + (c * perp.y + s * cross.y);
    out->z = proj.z + (c * perp.z + s * cross.z);
    return 1;
}


// ---- src/global/Parametric2DLineIntersection.cpp
// Intersects the lines (x1,y1)+t1*(dx1,dy1) and (x2,y2)+t2*(dx2,dy2).
// Returns 0 if an output pointer is missing or the lines are parallel.
int Parametric2DLineIntersection(float x1, float y1, float dx1, float dy1,
                                 float x2, float y2, float dx2, float dy2,
                                 float *t1, float *t2)
{
    if (!t1 || !t2)
        return 0;
    *t1 = dy1 * dx2 - dy2 * dx1;
    if (*t1 == 0.0f)
        return 0;
    *t1 = (dx2 * (y2 - y1) + dy2 * (x1 - x2)) / *t1;
    if (dx2 != 0.0f)
        *t2 = (x1 + dx1 * *t1 - x2) / dx2;
    else
        *t2 = (y1 + dy1 * *t1 - y2) / dy2;
    return 1;
}


// Retail's RTTI block has BaseObject's and GameObject's records before Fog's
// name (@709-@711, then @722-@723): mwcc built them as
// the bases of a class whose vtable it wrote first, pushed last on its LIFO
// list (key function defined after Fog's). Nothing constructed that
// class, so retail's linker stripped its vtable, RTTI and functions; link_c
// leaves them out ("Section
// order").
class StrippedFogObject : public GameObject {
public:
    virtual ~StrippedFogObject();
};

StrippedFogObject::~StrippedFogObject()
{
}
