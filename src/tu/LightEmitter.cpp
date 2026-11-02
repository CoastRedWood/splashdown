// LightEmitter.cpp  @ 0x00256620 (text 0x256620-0x259B50)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp)
//
// Retail's LightEmitter.cpp as one translation unit: LightEmitter,
// LightManager and LightSearchCache (the ASSERTs say "LightEmitter.cpp").
// Lzw.cpp (no initializer) follows at 0x259B50, then Math3d.cpp.
// __sinit_LightEmitter.cpp builds the axis vectors and maxAttenuation.
// `make link` places its data as well as its code.


// Retail never calls fptosi here: float->int is an inline cvt.w.s.
#pragma fast_fptosi on

#include "d3dtypes.h"
#include "LightEmitter/_D3DCOLORVALUE.h"
#include "TypeRegistry/TypeRegistry.h"
#include "GameObject/GameObject.h"
#include "SearchableObject/SearchableObject.h"
#include "QWORD/QWORD.h"
#include "LightManager/LightManager.h"
#include "SearchPlacement/SearchPlacement.h"
#include "LightEmitter/LightEmitter.h"
#include "LightSearchCache/LightSearchCache.h"
#include "LensFlare/LensFlare.h"
#include "soultree_assert.h"
#include "DebugOverlay/DebugOverlay.h"
#include "ControlInterface/ControlInterface.h"
#include "Game/Game.h"
#include "SearchNode/SearchNode.h"
#include "SearchList/SearchList.h"
#include "Portal/Portal.h"
#include "_D3DLVERTEX/_D3DLVERTEX.h"
#include "global/Vu0Inline.h"
#include "sinit/AxisVectors.h"

int InAmbientEditMode;
int InLightEditMode;
LightManager *LpLightManager;

extern "C" double sqrt(double x);
extern "C" void free(void *p);
extern "C" void *realloc(void *p, unsigned int size);
void PSXprintf(const char *fmt, ...);
int KEYDOWN_HANDLER_TEST(unsigned int key, DeviceType device, ProcessNode *node, unsigned int mods);

extern TypeRegistry *LpTypeRegistry;
unsigned char LightEmitterTypeID = 0xFF;   // until registered
extern Game *LpGame;

class SearchMethod;

// No header declares LightFlare (src/tu/LightFlare.cpp).
class LightFlare {
public:
    void SetColor(const int &color);

    char pad0[0x40];
    _D3DVECTOR m_position;          // +0x40
};

// A light's flicker (LightEmitter::m_flicker).
struct LightFlicker {
    unsigned int m_pattern;         // +0x00, one bit per 1/33 of the period
    unsigned int m_color;           // +0x04, colour while the bit is set
    float m_period;                 // +0x08
    float m_time;                   // +0x0C
    float m_blend;                  // +0x10
};

// D3DLIGHT7, as SimpleLightSearch views a LightEmitter's light (+0x38).
struct _D3DLIGHT7 {
    int dltType;                    // +0x00
    _D3DCOLORVALUE dcvDiffuse;      // +0x04
    _D3DCOLORVALUE dcvSpecular;     // +0x14
    _D3DCOLORVALUE dcvAmbient;      // +0x24
    float dvPosition[3];            // +0x34
    float dvDirection[3];           // +0x40
    float dvRange;                  // +0x4C
    float dvFalloff;                // +0x50
    float dvAttenuation[3];         // +0x54 (dvAttenuation0..2)
};

// The spatial search over a placement's nodes (no header).
class SpatialSearch {
public:
    SpatialSearch();
    int OpenSearch(SearchPlacement *placement, unsigned int flags, _D3DVECTOR *min, _D3DVECTOR *max,
                   _D3DVECTOR *a, _D3DVECTOR *b);
    SearchableObject *NextSearchObject();
    void CloseSearch();

#ifdef SPLASHDOWN_PC
    _D3DVECTOR pad[6];  // x86-64: the real class (SpatialSearch.h) is 0x58 bytes, 16-aligned
#else
    char pad[0x40];
#endif
};

// d3dtypes.h colour packing (long is 64-bit under mwccps2, so the alpha
// constant must be unsigned to stay zero-extended).
#define D3DRGB(r, g, b) \
    (0xff000000UL | (((long)((r) * 255)) << 16) | (((long)((g) * 255)) << 8) | (long)((b) * 255))
#define D3DRGBA(r, g, b, a) \
    ((((long)((a) * 255)) << 24) | (((long)((r) * 255)) << 16) | (((long)((g) * 255)) << 8) | (long)((b) * 255))

#define RGBA_GETALPHA(c) (((c) >> 24) & 0xff)
#define RGBA_GETRED(c) (((c) >> 16) & 0xff)
#define RGBA_GETGREEN(c) (((c) >> 8) & 0xff)
#define RGBA_GETBLUE(c) ((c) & 0xff)

// D3D's RGBA_MAKE; the components are longs here (64-bit), so the packing
// is done in 64-bit registers and narrowed on the store.
#define RGBA_MAKE(r, g, b, a) ((unsigned int)(((a) << 24) | ((r) << 16) | ((g) << 8) | (b)))

// The edit keys step the colour channel at a byte offset from the colour,
// so green and blue land one and two bytes in (as retail does).
#define CHANNEL(c, i) (*(float *)((char *)&(c) + (i)))

#define FLT_MAX 3.402823466e+38F

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

// out = m * (v.xyz, 1), with its operands listed (out, m, v) for retail's
// registers here.
static inline void TransformPoint(_D3DMATRIX *m, _D3DVECTOR *v, _D3DVECTOR *out)
{
    asm("lqc2 vf4, 0x0(%1); lqc2 vf5, 0x10(%1); lqc2 vf6, 0x20(%1); lqc2 vf7, 0x30(%1); lqc2 vf8, 0x0(%2); vmulw.w vf9, vf0, vf0w; vmulax.xyz ACC, vf4, vf8x; vmadday.xyz ACC, vf5, vf8y; vmaddz.xyz vf9, vf6, vf8z; vadd.xyz vf9, vf9, vf7; sqc2 vf9, 0x0(%0)"
        : : "r"(out), "r"(m), "r"(v) : "memory");
}

// libvu0 sceVu0Normalize.
static inline void Vu0Normalize(_D3DVECTOR *out, _D3DVECTOR *in)
{
    asm("lqc2 vf4, 0x0(%1); vmul.xyz vf5, vf4, vf4; vaddy.x vf5, vf5, vf5y; vaddz.x vf5, vf5, vf5z; vrsqrt Q, vf0w, vf5x; vsub.xyzw vf6, vf0, vf0; vaddw.xyzw vf6, vf6, vf4w; vwaitq; vmulq.xyz vf6, vf4, Q; sqc2 vf6, 0x0(%0)"
        : : "r"(out), "r"(in) : "memory");
}

// VU0 xyz dot product.
static inline float DotProduct(const _D3DVECTOR &a, const _D3DVECTOR &b)
{
    float r;
    asm("lqc2 vf4, 0x0(%1); lqc2 vf5, 0x0(%2); vmul.xyz vf5, vf4, vf5; vaddy.x vf5, vf5, vf5y; vaddz.x vf5, vf5, vf5z; qmfc2 $2, vf5; mtc1 $2, %0"
        : "=f"(r) : "r"(&a), "r"(&b) : "$2");
    return r;
}

// VU0 squared length, returned in a GPR (SimpleLightSearch).
static inline float LengthSquaredGPR(_D3DVECTOR *a)
{
    float r;
    asm("lqc2 vf4, 0x0(%1); vmove.xyzw vf5, vf4; vmul.xyz vf5, vf4, vf5; vaddy.x vf5, vf5, vf5y; vaddz.x vf5, vf5, vf5z; qmfc2 %0, vf5"
        : "=r"(r) : "r"(a));
    return r;
}

// VU0 squared length, through $2 like DotProduct (GetThreeLights): the $2
// clobber keeps the 1.0f constant live across it out of $v0 (the stores of
// the result then become `sw $v0`).
static inline float LengthSquared(_D3DVECTOR *a)
{
    float r;
    asm("lqc2 vf4, 0x0(%1); vmove.xyzw vf5, vf4; vmul.xyz vf5, vf4, vf5; vaddy.x vf5, vf5, vf5y; vaddz.x vf5, vf5, vf5z; qmfc2 $2, vf5; mtc1 $2, %0"
        : "=f"(r) : "r"(a) : "$2");
    return r;
}

// ---- src/LightEmitter/ctor.cpp

LightEmitter::LightEmitter(int enabled) : GameObject(enabled)
{
    m_type = 0;
    m_lensFlare = 0;
    m_lightFlare = 0;
    m_flicker = 0;
    m_mode = 3;
    m_diffuse.r = 128.0f;
    m_diffuse.g = 128.0f;
    m_diffuse.b = 128.0f;
    m_diffuse.a = 0.0f;
    m_specular.r = 128.0f;
    m_specular.g = 128.0f;
    m_specular.b = 128.0f;
    m_specular.a = 0.0f;
    m_ambient.r = 128.0f;
    m_ambient.g = 128.0f;
    m_ambient.b = 128.0f;
    m_ambient.a = 0.0f;
    m_pos[0] = 0.0f;
    m_pos[1] = 0.0f;
    m_pos[2] = 0.0f;
    m_dir[0] = 1.0f;
    m_dir[1] = 0.0f;
    m_dir[2] = 0.0f;
    m_range = sqrt(3.40282347e+38f);
    m_falloff = 1.0f;
    m_axis[0] = 1.0f;
    m_axis[1] = 0.0f;
    m_axis[2] = 0.0f;
    m_angle = 1.57079637f;
    m_halfAngle = m_angle / 2.0f;
    m_luminance = -1.0f;
    m_xC8 = -1;
    m_position = ZeroVector;
    m_placement = 0;
    m_searchNode = 0;
    if (LightEmitterTypeID == 0xFF)
        LightEmitterTypeID = LpTypeRegistry->GetTypeID("LightEmitter");
    m_typeID = LightEmitterTypeID;
}

// ---- src/LightEmitter/dtor.cpp

LightEmitter::~LightEmitter()
{
    if (LpLightManager)
        LpLightManager->RemoveLight(this);
    if (m_placement)
        delete m_placement;
    if (m_flicker)
        delete m_flicker;
}

// ---- src/LightEmitter/GetColor.cpp

unsigned int LightEmitter::GetColor()
{
    if (m_type == 3)
        return (int)m_ambient.a << 24 | (int)m_ambient.r << 16 | (int)m_ambient.g << 8 | (int)m_ambient.b;
    return (int)m_diffuse.a << 24 | (int)m_diffuse.r << 16 | (int)m_diffuse.g << 8 | (int)m_diffuse.b;
}

// ---- src/LightEmitter/SetColor.cpp

void LightEmitter::SetColor(_D3DCOLORVALUE color)
{
    if (m_type == 3) {
        m_ambient = color;
        m_diffuse.r = 0.0f;
        m_diffuse.g = 0.0f;
        m_diffuse.b = 0.0f;
        m_diffuse.a = 0.0f;
        m_specular.r = 0.0f;
        m_specular.g = 0.0f;
        m_specular.b = 0.0f;
        m_specular.a = 0.0f;
    } else {
        m_diffuse = color;
        m_specular = color;
        m_ambient.r = 0.0f;
        m_ambient.g = 0.0f;
        m_ambient.b = 0.0f;
        m_ambient.a = 0.0f;
    }
    if (m_lensFlare)
        m_lensFlare->SetColor(D3DRGB(color.r, color.g, color.b));
    if (m_lightFlare) {
        int flareColor = D3DRGBA(color.r * (1.0f / 255.0f), color.g * (1.0f / 255.0f), color.b * (1.0f / 255.0f), color.a * (1.0f / 255.0f));
        m_lightFlare->SetColor(flareColor);
    }
    if (LpLightManager)
        LpLightManager->LightingChanged();
    m_luminance = CalcLuminance();
}

// ---- src/LightEmitter/CalcLuminance.cpp

float LightEmitter::CalcLuminance()
{
    return 0.114 * m_diffuse.b + 0.587 * m_diffuse.g + 0.299 * m_diffuse.r;
}

// ---- src/LightEmitter/SetRange.cpp

void LightEmitter::SetRange(float range)
{
    // 0 or anything past sqrt(FLT_MAX) means "infinite".
    if (range == 0.0f || range > (float)sqrt(FLT_MAX))
        range = (float)sqrt(FLT_MAX);
    if (range != m_range) {
        m_range = range;
        if (LpLightManager)
            LpLightManager->LightingChanged();
    }
}

// ---- src/LightEmitter/SetPosition.cpp

void LightEmitter::SetPosition(_D3DVECTOR &position)
{
    int changed = position != m_position;
    if (changed) {
        m_position = position;
        UpdateSearchPlacement();
        m_pos[0] = m_position.x;
        m_pos[1] = m_position.y;
        m_pos[2] = m_position.z;
        if (m_lensFlare)
            m_lensFlare->SetPosition(m_position);
        if (m_lightFlare)
            m_lightFlare->m_position = position;
        if (LpLightManager)
            LpLightManager->LightingChanged();
    }
}

// ---- src/LightEmitter/SetDirection.cpp

void LightEmitter::SetDirection(_D3DVECTOR &direction)
{
    int changed = direction != m_direction;
    if (changed) {
        m_direction = direction;
        m_dir[0] = direction.x;
        m_dir[1] = direction.y;
        m_dir[2] = direction.z;
        if (LpLightManager)
            LpLightManager->LightingChanged();
    }
}

// ---- src/LightEmitter/Create.cpp
// Retail built this without the global optimizer (no asm or ASSERT here).
#pragma global_optimizer off

LightEmitter *LightEmitter::Create(LightEmitterType type, _D3DCOLORVALUE color, _D3DVECTOR *position,
                                   _D3DVECTOR *direction, _D3DVECTOR *axis, float range, int a, int b, char *name,
                                   int c)
{
    GameObject::Create();
    m_type = type;
    m_xC8 = c;
    // Halves the colour. The references (bound in this order, green after
    // red's division) give retail's address registers.
    float &blue = color.b;
    float &alpha = color.a;
    color.r /= 2.0f;
    float &green = color.g;
    green /= 2.0f;
    blue /= 2.0f;
    alpha /= 2.0f;
    SetColor(color);
    SetRange(range);
    if (position)
        SetPosition(*position);
    if (direction)
        SetDirection(*direction);
    if (axis) {
        m_axis[0] = axis->x;
        m_axis[1] = axis->y;
        m_axis[2] = axis->z;
    } else {
        m_axis[0] = 1.0f;
        m_axis[1] = 0.0f;
        m_axis[2] = 0.0f;
    }
    if (type) {
        switch (type) {
        case 2:
            m_mode = 3;
            break;
        case 1:
            m_mode = 1;
            break;
        case 3:
            break;
        }
    }
    return this;
}
#pragma global_optimizer reset

// ---- src/LightEmitter/AddSearchPlacement.cpp
#pragma alignlabel off

void LightEmitter::AddSearchPlacement(SearchNode *node, unsigned int flags)
{
#line 349 "LightEmitter.cpp"
    ASSERT(m_placement == 0, "AddSearchPlacement: LpSearchPlacement already exists");
    m_placement = new SearchPlacement;
    m_searchNode = node;
    m_placement->PerformInitialPlacement(this, node, flags, &m_position, &ZeroVector, 0.0f);
#line 353 "LightEmitter.cpp"
    ASSERT(m_placement->FirstPlacementItem.LpSearchNode, "AddSearchPlacement: FirstPlacementItem.LpSearchNode==NULL");
}
#pragma alignlabel reset

// ---- src/LightEmitter/UpdateSearchPlacement.cpp

void LightEmitter::UpdateSearchPlacement()
{
    if (m_placement)
        m_placement->UpdatePlacement(this, &m_position, &ZeroVector, m_falloff);
}

// ---- src/LightEmitter/RemoveFromSearch.cpp

void LightEmitter::RemoveFromSearch()
{
    if (m_placement) {
        m_placement->Remove(this);
        delete m_placement;
        m_placement = 0;
    }
}

// ---- src/LightEmitter/Tick.cpp

int LightEmitter::Tick(float dt)
{
    // Retail built this without the global optimizer (value-form loop test);
    // an empty asm statement reproduces that.
    asm("");
    if (m_flicker && m_flicker->m_pattern != -1) {
        m_flicker->m_time += dt;
        float period;
        float time;
        while ((time = m_flicker->m_time) > (period = m_flicker->m_period))
            m_flicker->m_time -= period;
        unsigned int color = GetColor();
        int bit = (int)(33.0f * time / period);
        unsigned int flicker = (m_flicker->m_pattern >> bit) & 1 ? m_flicker->m_color : 0;
        _D3DCOLORVALUE c;
        c.r = m_flicker->m_blend * RGBA_GETRED(color) + (1.0f - m_flicker->m_blend) * RGBA_GETRED(flicker);
        c.g = m_flicker->m_blend * RGBA_GETGREEN(color) + (1.0f - m_flicker->m_blend) * RGBA_GETGREEN(flicker);
        c.b = m_flicker->m_blend * RGBA_GETBLUE(color) + (1.0f - m_flicker->m_blend) * RGBA_GETBLUE(flicker);
        c.a = m_flicker->m_blend * RGBA_GETALPHA(color) + (1.0f - m_flicker->m_blend) * RGBA_GETALPHA(flicker);
        SetColor(c);
    }
    return GameObject::Tick(dt);
}

// ---- src/LightManager/ctor.cpp
// ASSERT contains inline asm; match retail's label alignment for asm functions.
#pragma alignlabel off

LightManager::LightManager() : GameObject(1)
{
#line 410 "LightEmitter.cpp"
    ASSERT(LpLightManager == 0, "LightManager constructor: A LightManager has already been constructed!");
    LpLightManager = this;
    m_x30 = ZeroVector;
    m_sun = 0;
    m_lightColors = 0;
    m_savedLightColors = 0;
    m_lightingVersion = 0;
    m_lists = 0;
    m_nodes = 0;
    m_numLightColors = 0;
    m_numLights = 0;
    m_maxLights = 0;
    m_lights = 0;
    m_haveType2 = 0;
    m_searchCache = 0;
    m_overlayPage = -1;
}
#pragma alignlabel reset

// ---- src/LightManager/dtor.cpp

LightManager::~LightManager()
{
    if (m_lights)
        free(m_lights);
    if (m_lightColors)
        delete[] m_lightColors;
    if (m_savedLightColors)
        delete[] m_savedLightColors;
    if (m_searchCache)
        delete m_searchCache;
    ReleaseLightGraph();
    LpLightManager = 0;
}

// ---- src/LightManager/ReleaseLightGraph.cpp

void LightManager::ReleaseLightGraph()
{
    int i;
    if (m_nodes) {
        for (i = 0; i < m_numLightColors; i++)
            m_nodes[i]->Release();
        delete[] m_nodes;
        m_nodes = 0;
    }
    if (m_lists) {
        for (i = 0; i < m_numLightColors; i++)
            m_lists[i]->Release();
        delete[] m_lists;
        m_lists = 0;
    }
    m_numLightColors = 0;
    if (m_lightColors) {
        delete[] m_lightColors;
        m_lightColors = 0;
    }
    if (m_savedLightColors) {
        delete[] m_savedLightColors;
        m_savedLightColors = 0;
    }
}

// ---- src/LightManager/Create.cpp

// Retail returns this (move v0, a0).
LightManager *LightManager::Create()
{
    return this;
}

// ---- src/LightManager/KeyDownHandler.cpp

// Debug keys: toggle the ambient and light colour editors.
int LightManager::KeyDownHandler(ProcessNode *node, KeyInfo *key)
{
    int result = GameObject::KeyDownHandler(node, key);
    if (KEYDOWN_HANDLER_TEST(4, (DeviceType)0, node, 0x80)) {
        InAmbientEditMode = 1 - InAmbientEditMode;
        PSXprintf("LIGHTS: %s Ambient Edit Mode. R = %d G = %d B = %d\n", InAmbientEditMode ? "Enter" : "Exit ",
                  (int)(2.0f * m_ambientColor[0]), (int)(2.0f * m_ambientColor[1]), (int)(2.0f * m_ambientColor[2]));
        return 1;
    }
    if (KEYDOWN_HANDLER_TEST(0xF, (DeviceType)0, node, 0x80)) {
        InLightEditMode = 1 - InLightEditMode;
        LightEmitter *light = m_sun;
        PSXprintf("LIGHTA: %s Light   Edit Mode. R = %d G = %d B = %d\n", InLightEditMode ? "Enter" : "Exit ",
                  (int)(2.0f * light->m_diffuse.r), (int)(2.0f * light->m_diffuse.g), (int)(2.0f * light->m_diffuse.b));
        return 1;
    }
    return result;
}

// ---- src/LightManager/Tick.cpp

// Ambient and light edit modes, then the debug overlay page.
int LightManager::Tick(float dt)
{
    if (InAmbientEditMode) {
        int changed = 0;
        if (LpGame->m_controls->IsKeyDown(0x15, DEVICE_KEYBOARD, 1, 0)) {
            m_ambientColor[0] = Fmin(m_ambientColor[0] + 1.0f, 255.0f);
            changed = 1;
        }
        if (LpGame->m_controls->IsKeyDown(0xA, DEVICE_KEYBOARD, 1, 0)) {
            m_ambientColor[1] = Fmin(m_ambientColor[1] + 1.0f, 255.0f);
            changed = 1;
        }
        if (LpGame->m_controls->IsKeyDown(5, DEVICE_KEYBOARD, 1, 0)) {
            m_ambientColor[2] = Fmin(m_ambientColor[2] + 1.0f, 255.0f);
            changed = 1;
        }
        if (LpGame->m_controls->IsKeyDown(0x15, DEVICE_KEYBOARD, 4, 0)) {
            m_ambientColor[0] = Fmax(m_ambientColor[0] - 1.0f, 0.0f);
            changed = 1;
        }
        if (LpGame->m_controls->IsKeyDown(0xA, DEVICE_KEYBOARD, 4, 0)) {
            m_ambientColor[1] = Fmax(m_ambientColor[1] - 1.0f, 0.0f);
            changed = 1;
        }
        if (LpGame->m_controls->IsKeyDown(5, DEVICE_KEYBOARD, 4, 0)) {
            m_ambientColor[2] = Fmax(m_ambientColor[2] - 1.0f, 0.0f);
            changed = 1;
        }
        if (m_lightColors)
            m_lightColors->m_value = m_ambientValue;
        if (m_savedLightColors)
            m_savedLightColors->m_value = m_ambientValue;
        if (changed)
            PSXprintf("Ambient R = %d G = %d B = %d\n", (int)(2.0f * m_ambientColor[0]), (int)(2.0f * m_ambientColor[1]),
                      (int)(2.0f * m_ambientColor[2]));
    }
    if (InLightEditMode) {
        int changed = 0;
        if (LpGame->m_controls->IsKeyDown(0x15, DEVICE_KEYBOARD, 1, 0)) {
            float *c = &CHANNEL(m_sun->m_diffuse, 0);
            *c = Fmin(*c + 1.0f, 255.0f);
            changed = 1;
        }
        if (LpGame->m_controls->IsKeyDown(0xA, DEVICE_KEYBOARD, 1, 0)) {
            float *c = &CHANNEL(m_sun->m_diffuse, 1);
            *c = Fmin(*c + 1.0f, 255.0f);
            changed = 1;
        }
        if (LpGame->m_controls->IsKeyDown(5, DEVICE_KEYBOARD, 1, 0)) {
            float *c = &CHANNEL(m_sun->m_diffuse, 2);
            *c = Fmin(*c + 1.0f, 255.0f);
            changed = 1;
        }
        if (LpGame->m_controls->IsKeyDown(0x15, DEVICE_KEYBOARD, 4, 0)) {
            LightEmitter *light = m_sun;
            CHANNEL(light->m_diffuse, 0) = Fmax(CHANNEL(light->m_diffuse, 0) - 1.0f, 0.0f);
            changed = 1;
        }
        if (LpGame->m_controls->IsKeyDown(0xA, DEVICE_KEYBOARD, 4, 0)) {
            LightEmitter *light = m_sun;
            CHANNEL(light->m_diffuse, 1) = Fmax(CHANNEL(light->m_diffuse, 1) - 1.0f, 0.0f);
            changed = 1;
        }
        if (LpGame->m_controls->IsKeyDown(5, DEVICE_KEYBOARD, 4, 0)) {
            LightEmitter *light = m_sun;
            CHANNEL(light->m_diffuse, 2) = Fmax(CHANNEL(light->m_diffuse, 2) - 1.0f, 0.0f);
            changed = 1;
        }
        if (changed)
            PSXprintf("Directional R = %d G = %d B = %d\n", (int)(2.0f * m_sun->m_diffuse.r),
                      (int)(2.0f * m_sun->m_diffuse.g), (int)(2.0f * m_sun->m_diffuse.b));
    }
    DebugOverlay *overlay = LpGame->m_debugOverlay;
    if (overlay) {
        if (m_overlayPage < 0) {
            int page = overlay->m_numPages;
            overlay->m_numPages = page + 1;
            m_overlayPage = page;
        }
        LpGame->m_debugOverlay->PrintfHeading(m_overlayPage, "LightManager");
        LpGame->m_debugOverlay->PrintfDetail(m_overlayPage, "Ambient Red: %f", 2.0f * m_ambientColor[0]);
        LpGame->m_debugOverlay->PrintfDetail(m_overlayPage, "Ambient Grn: %f", 2.0f * m_ambientColor[1]);
        LpGame->m_debugOverlay->PrintfDetail(m_overlayPage, "Ambient Blu: %f", 2.0f * m_ambientColor[2]);
        if (m_sun) {
            LpGame->m_debugOverlay->PrintfDetail(m_overlayPage, "Directional Red: %f", 2.0f * m_sun->m_diffuse.r);
            LpGame->m_debugOverlay->PrintfDetail(m_overlayPage, "Directional Grn: %f", 2.0f * m_sun->m_diffuse.g);
            LpGame->m_debugOverlay->PrintfDetail(m_overlayPage, "Directional Blu: %f", 2.0f * m_sun->m_diffuse.b);
        }
    }
    return GameObject::Tick(dt);
}

// ---- src/LightManager/CreateLightGraph.cpp

void LightManager::CreateLightGraph(int numNodes)
{
    QWORD ambient;
    ambient.f[0] = ambient.f[1] = ambient.f[2] = 50.0f;
    ambient.f[3] = 64.0f;
    m_ambientValue = m_savedAmbientValue = ambient.m_value;
    ReleaseLightGraph();
    m_numLightColors = numNodes;
    m_nodes = new SearchNode *[numNodes];
    m_lists = new SearchList *[numNodes];
    m_lightColors = new QWORD[numNodes];
    m_savedLightColors = new QWORD[numNodes];
    for (int i = 0; i < numNodes; i++) {
        m_nodes[i] = new SearchNode(1);
        m_lists[i] = new SearchList(0x400);
        m_nodes[i]->SetStaticSearchMethod((SearchMethod *)m_lists[i]);
        m_nodes[i]->SetDynamicSearchMethod((SearchMethod *)m_lists[i]);
        m_nodes[i]->m_x48 = i;
        m_lightColors[i] = m_savedLightColors[i] = ambient;
    }
}

// ---- src/LightManager/AddLight.cpp

void LightManager::AddLight(LightEmitter *light, int index)
{
    if (m_lightColors && light->m_type == 3) {
        m_ambientColor[0] = m_savedAmbientColor[0] = m_lightColors[index].f[0] = m_savedLightColors[index].f[0] = light->m_ambient.r;
        m_ambientColor[1] = m_savedAmbientColor[1] = m_lightColors[index].f[1] = m_savedLightColors[index].f[1] = light->m_ambient.g;
        m_ambientColor[2] = m_savedAmbientColor[2] = m_lightColors[index].f[2] = m_savedLightColors[index].f[2] = light->m_ambient.b;
        m_ambientColor[3] = m_savedAmbientColor[3] = m_lightColors[index].f[3] = m_savedLightColors[index].f[3] = 64.0f;
        if (index == 0) {
            for (int i = 0; i < m_numLightColors; i++) {
                m_lightColors[i].m_value = m_ambientValue;
                m_savedLightColors[i].m_value = m_ambientValue;
            }
        }
    }
    if (m_nodes && light->m_type == 1)
        light->AddSearchPlacement(m_nodes[index], 1);
    if (light->m_type == 2 || light->m_type == 1) {
        m_x30.x = light->m_diffuse.r;
        m_x30.y = light->m_diffuse.g;
        m_x30.z = light->m_diffuse.b;
        m_x30.w = 0.0f;
    }
    if (light->m_type == 2)
        m_sun = light;
    switch (light->m_type) {
    case 1:
        m_haveType2 = 1;
        break;
    case 2:
    case 3:
        break;
    }
    if (m_maxLights < m_numLights + 1) {
        m_maxLights = m_numLights + 10;
        m_lights = (LightEmitter **)realloc(m_lights, m_maxLights * sizeof(LightEmitter *));
    }
    m_lights[m_numLights] = light;
    m_numLights++;
}

// ---- src/LightManager/RemoveLight.cpp

void LightManager::RemoveLight(LightEmitter *light)
{
    int i;
    // (Retail only drops the count; the array itself isn't compacted.)
    for (i = 0; i < m_numLights; i++) {
        if (m_lights[i] == light)
            m_numLights--;
    }
    if (light->m_type == 1)
        light->RemoveFromSearch();
    m_haveType2 = 0;
    for (i = 0; i < m_numLights; i++) {
        if (m_lights[i]->m_type == 2) {
            m_haveType2 = 1;
            return;
        }
    }
}

// ---- src/LightManager/LightingChanged.cpp

void LightManager::LightingChanged()
{
    m_lightingVersion++;
}

// ---- src/LightManager/LightVertices.cpp

// Light vertices by the (last) directional light plus ambient, given their
// normals in m's space.
void LightManager::LightVertices(_D3DMATRIX *m, int count, _D3DVECTOR *normals, _D3DLVERTEX *vertices, int stride)
{
    _D3DVECTOR direction;
    float r = 0.0f, g = 0.0f, b = 0.0f;
    _D3DMATRIX inv __attribute__((aligned(16)));
    inv = *m;
    Vu0InversMatrix(&inv);
    int i;
    for (i = 0; i < m_numLights; i++) {
        LightEmitter *light = m_lights[i];
        if (light->m_type != 2)
            continue;
        r = light->m_diffuse.r;
        g = light->m_diffuse.g;
        b = light->m_diffuse.b;
        TransformPoint(&inv, &light->m_position, &direction);
        Vu0Normalize(&direction, &direction);
    }
    for (i = 0; i < count; i++) {
        float d = -Vu0DotProduct(normals, &direction);
        if (d > 0.0f) {
            float red = (m_ambientColor[0] + d * r) / 128.0f;
            float green = (m_ambientColor[1] + d * g) / 128.0f;
            float blue = (m_ambientColor[2] + d * b) / 128.0f;
            vertices->color = RGBA_MAKE((long)(255.0f * Fmin(red, 1.0f)), (long)(255.0f * Fmin(green, 1.0f)), (long)(255.0f * Fmin(blue, 1.0f)), 0xFFL);
        } else {
            vertices->color = RGBA_MAKE((long)(255.0f * (m_ambientColor[0] / 128.0f)), (long)(255.0f * (m_ambientColor[1] / 128.0f)), (long)(255.0f * (m_ambientColor[2] / 128.0f)), 0xFFL);
        }
        vertices->specular = 0;
        normals = (_D3DVECTOR *)((char *)normals + stride);
        vertices++;
    }
}

// ---- src/LightManager/FindLight.cpp

LightEmitter *LightManager::FindLight(LightEmitterType type)
{
    for (int i = 0; i < m_numLights; i++) {
        if (m_lights[i]->m_type == type)
            return m_lights[i];
    }
    return 0;
}

// ---- src/LightManager/BlendedAmbientColor.cpp

// The placement's zone ambient color, blended with neighbouring zones whose
// portals the sphere reaches into (weighted by penetration depth). Returned
// as a QWORD (copied out with lq/sq).
QWORD LightManager::BlendedAmbientColor(SearchPlacement *placement, _D3DVECTOR *position, float radius)
{
    SearchNode *zone = placement->FirstPlacementItem.LpSearchNode;
    QWORD color = m_lightColors[zone->m_x48];
    // (Declaration and assignment order reproduce retail's register choice.)
    float b, weight, g, r;
    b = 0.0f;
    g = b;
    r = b;
    weight = b;
    for (PortalLink *link = zone->m_portals; link; link = link->m_next) {
        float distance;
        if (link->m_portal->SphereIntersects(position, radius, &distance)) {
            float t = (radius - distance) / radius;
            QWORD *other = &m_lightColors[link->m_node->m_x48];
            r += t * other->f[0];
            g += t * other->f[1];
            weight += t;
            b += t * other->f[2];
        }
    }
    float scale = 1.0f / (1.0f + weight);
    color.f[0] = scale * (color.f[0] + r);
    color.f[1] = scale * (color.f[1] + g);
    color.f[2] = scale * (color.f[2] + b);
    color.f[3] = 64.0f;
    return color;
}

// ---- src/LightManager/ChangeAmbientColor.cpp

void LightManager::ChangeAmbientColor(QWORD &color)
{
    m_ambientValue = color.m_value;
    for (int i = 0; i < m_numLightColors; i++)
        m_lightColors[i] = color;
}

// ---- src/LightManager/RestoreAmbientColor.cpp

void LightManager::RestoreAmbientColor()
{
    m_ambientValue = m_savedAmbientValue;
    for (int i = 0; i < m_numLightColors; i++)
        m_lightColors[i] = m_savedLightColors[i];
}

// ---- src/LightManager/GetDefaultLights.cpp

// Light matrices for the VU: directions are stored transposed (one QWORD
// per axis), colors one QWORD per light, ambient in the last row.
void LightManager::GetDefaultLights(QWORD *directions, QWORD *colors, QWORD *)
{
    colors[0].m_value = 0;
    colors[1].m_value = 0;
    colors[2].m_value = 0;
    colors[0].ui[3] = colors[1].ui[3] = colors[2].ui[3] = 64;
    directions[0].m_value = 0;
    directions[1].m_value = 0;
    directions[2].m_value = 0;
    directions[3].f[0] = directions[3].f[1] = directions[3].f[2] = directions[3].f[3] = 1.0f;
    if (m_sun) {
        directions[0].f[0] = -m_sun->m_dir[0];
        directions[1].f[0] = -m_sun->m_dir[1];
        directions[2].f[0] = -m_sun->m_dir[2];
        colors[0].f[0] = m_sun->m_diffuse.r;
        colors[0].f[1] = m_sun->m_diffuse.g;
        colors[0].f[2] = m_sun->m_diffuse.b;
    }
    colors[3].m_value = m_ambientValue;
}

// ---- src/LightManager/SimpleLightSearch.cpp

// Fill the VU light matrices from up to every light: directional lights as
// is, point lights as a direction from point with the colour attenuated by
// 1 / (a0 + a1 d + a2 d^2).
void LightManager::SimpleLightSearch(int ambient, _D3DVECTOR point, QWORD *directions, QWORD *colors, QWORD *speculars)
{
    _D3DVECTOR distance(1.0f, 1.0f, 1.0f);
    _D3DVECTOR toLight;
    int i;
    int count;
    directions[0].m_value = 0;
    directions[1].m_value = 0;
    directions[2].m_value = 0;
    directions[3].m_value = 0;
    colors[0].m_value = 0;
    colors[1].m_value = 0;
    colors[2].m_value = 0;
    colors[3].m_value = 0;
    count = 0;
    if (speculars) {
        speculars[0].m_value = 0;
        speculars[1].m_value = 0;
        speculars[2].m_value = 0;
        speculars[3].m_value = 0;
    }
    for (i = 0; i < m_numLights; i++) {
        int type;
        LightEmitter *emitter = m_lights[i];
        type = emitter->m_type;
        if (type == 3)
            continue;
        _D3DLIGHT7 *light = (_D3DLIGHT7 *)&emitter->m_mode;
        if (type == 2) {
            directions[0].f[count] = -light->dvDirection[0];
            directions[1].f[count] = -light->dvDirection[1];
            directions[2].f[count] = -light->dvDirection[2];
            colors[count].f[0] = light->dcvDiffuse.r;
            colors[count].f[1] = light->dcvDiffuse.g;
            colors[count].f[2] = light->dcvDiffuse.b;
            colors[count].ui[3] = 64;
            if (speculars) {
                speculars[count].f[0] = light->dcvSpecular.r;
                speculars[count].f[1] = light->dcvSpecular.g;
                speculars[count].f[2] = light->dcvSpecular.b;
                speculars[count].ui[3] = 64;
            }
        } else {
            toLight = emitter->m_position - point;
            // distance = (1, d, d^2)
            distance.z = LengthSquaredGPR(&toLight);
            distance.y = Fsqrt(distance.z);
            float scale = 1.0f / DotProduct(_D3DVECTOR(light->dvAttenuation), distance);
            Vu0Normalize(&toLight, &toLight);
            directions[0].f[count] = toLight.x;
            directions[1].f[count] = toLight.y;
            directions[2].f[count] = toLight.z;
            colors[count].f[0] = scale * light->dcvDiffuse.r;
            colors[count].f[1] = scale * light->dcvDiffuse.g;
            colors[count].f[2] = scale * light->dcvDiffuse.b;
            colors[count].ui[3] = 64;
            if (speculars) {
                speculars[count].f[0] = scale * light->dcvSpecular.r;
                speculars[count].f[1] = scale * light->dcvSpecular.g;
                speculars[count].f[2] = scale * light->dcvSpecular.b;
            }
        }
        // Retail clears both w words here, overwriting the 64s above, and
        // writes speculars even when it is null.
        colors[count].ui[3] = 0;
        speculars[count].ui[3] = 0;
        count++;
    }
    colors[3] = m_lightColors[ambient];
}

// Defined here, after the last function with literals: retail's .rodata has it
// after them; __sinit still builds it
// after the axis vectors.
const _D3DVECTOR maxAttenuation(1.0f, 1.0f, 1.0f);

// ---- src/LightManager/GetThreeLights.cpp

// A colour indexed as an array (an inline call, so the member's address is
// formed before the index is added).
static inline float &ColorChannel(_D3DCOLORVALUE &c, int i)
{
    return (&c.r)[i];
}

// The three lights that light point most (smallest attenuation within
// radius, plus the sun), as VU light matrices; few lights take the simple
// all-lights path, and results go through the light search cache.
void LightManager::GetThreeLights(SearchPlacement *placement, _D3DVECTOR point, float radius, QWORD *directions,
                                  QWORD *colors, QWORD *speculars)
{
    if (m_searchCache && m_searchCache->Retrieve(placement, directions, colors, speculars))
        return;
    int ambient;
    if (placement)
        ambient = placement->FirstPlacementItem.LpSearchNode->m_x48;
    else
        ambient = 0;
    if (m_numLights < 5) {
        SimpleLightSearch(ambient, point, directions, colors, speculars);
        return;
    }
    _D3DVECTOR toLight;
    _D3DVECTOR distance(1.0f, radius, radius * radius);
    _D3DVECTOR extent(radius, radius, radius);
    int worst = 0;
    LightEmitter *best[3] = {0, 0, 0};
    float size[3] = {radius, radius, radius};
    SpatialSearch search;
    _D3DVECTOR outMin;
    _D3DVECTOR outMax;
    SearchPlacement searchPlacement;
    searchPlacement.FirstPlacementItem.LpSearchNode = m_nodes[ambient];
    search.OpenSearch(&searchPlacement, 3, &point, &extent, &outMin, &outMax);
    float limit = DotProduct(maxAttenuation, distance);
    float attenuation[3] = {limit, limit, limit};
    SearchableObject *object;
    while ((object = search.NextSearchObject())) {
        if (object->m_typeID != LightEmitterTypeID)
            continue;
        LightEmitter *emitter = (LightEmitter *)object;
        toLight = emitter->m_position - point;
        distance.z = LengthSquared(&toLight);
        distance.y = Fsqrt(distance.z);
        float a = DotProduct(_D3DVECTOR(emitter->m_axis), distance);
        if (a < attenuation[worst]) {
            attenuation[worst] = a;
            best[worst] = emitter;
            if (attenuation[0] > attenuation[1]) {
                if (attenuation[0] > attenuation[2])
                    worst = 0;
                else
                    worst = 2;
            } else
                worst = attenuation[1] > attenuation[2] ? 1 : 2;
        }
    }
    search.CloseSearch();
    if (m_sun) {
        best[worst] = m_sun;
        attenuation[worst] = 1.0f;
    }
    _D3DVECTOR dirs[3];
    for (int i = 0; i < 3; i++) {
        if (best[i]) {
            float scale = 1.0f / attenuation[i];
            if (best[i]->m_type == 2) {
                dirs[i].x = -best[i]->m_dir[0];
                dirs[i].y = -best[i]->m_dir[1];
                dirs[i].z = -best[i]->m_dir[2];
            } else {
                dirs[i] = best[i]->m_position - point;
            }
            for (int j = 0; j < 3; j++) {
                colors[i].f[j] = scale * ColorChannel(best[i]->m_diffuse, j);
                if (speculars)
                    speculars[i].f[j] = scale * ColorChannel(best[i]->m_specular, j);
            }
            colors[i].ui[3] = 64;
            speculars[i].ui[3] = 64;
            Vu0Normalize(&dirs[i], &dirs[i]);
            directions[0].f[i] = dirs[i].x;
            directions[1].f[i] = dirs[i].y;
            directions[2].f[i] = dirs[i].z;
            directions[3].f[i] = 1.0f;
        } else {
            directions[3].f[i] = 0.0f;
            directions[2].f[i] = 0.0f;
            directions[1].f[i] = 0.0f;
            directions[0].f[i] = 0.0f;
            colors[i].f[2] = 0.0f;
            colors[i].f[1] = 0.0f;
            colors[i].f[0] = 0.0f;
            colors[i].ui[3] = 64;
            if (speculars) {
                speculars[i].f[2] = 0.0f;
                speculars[i].f[1] = 0.0f;
                speculars[i].f[0] = 0.0f;
                speculars[i].ui[3] = 64;
            }
        }
    }
    if (m_numLightColors > 1) {
        static float rad = 200.0f;
        colors[3] = BlendedAmbientColor(placement, &point, rad);
    } else {
        colors[3] = m_lightColors[ambient];
    }
    directions[2].f[3] = 0.0f;
    directions[1].f[3] = 0.0f;
    directions[0].f[3] = 0.0f;
    directions[3].f[3] = 1.0f;
    if (m_searchCache)
        m_searchCache->Cache(placement, directions, colors, speculars);
}

// ---- src/LightSearchCache/Cache.cpp

int LightSearchCache::Cache(SearchPlacement *placement, QWORD *a, QWORD *b, QWORD *c)
{
    int i = LRU();
    for (int j = 0; j < 4; j++) {
        m_entries[i].m_a[j] = *a;
        a++;
        m_entries[i].m_b[j] = *b;
        b++;
        m_entries[i].m_c[j] = *c;
        c++;
    }
    m_entries[i].m_lastUsed = m_time;
    m_entries[i].placement = placement;
    return 1;
}

// ---- src/LightSearchCache/Retrieve.cpp

int LightSearchCache::Retrieve(SearchPlacement *placement, QWORD *a, QWORD *b, QWORD *c)
{
    int i = Hit(placement);
    if (i < 0)
        return 0;
    for (int j = 0; j < 4; j++) {
        *a = m_entries[i].m_a[j];
        a++;
        *b = m_entries[i].m_b[j];
        b++;
        *c = m_entries[i].m_c[j];
        c++;
    }
    m_entries[i].m_lastUsed = m_time;
    return 1;
}

// ---- src/LightSearchCache/Hit.cpp

int LightSearchCache::Hit(SearchPlacement *placement)
{
    for (int i = 0; i < m_numEntries; i++) {
        if (m_entries[i].placement == placement)
            return i;
    }
    return -1;
}

// ---- src/LightSearchCache/LRU.cpp

int LightSearchCache::LRU()
{
    int lru = 0;
    unsigned int oldest = m_entries[0].m_lastUsed;
    if (oldest == 0)
        return 0;
    for (int i = 1; i < m_numEntries; i++) {
        unsigned int t = m_entries[i].m_lastUsed;
        if (t == 0)
            return i;
        if (t < oldest) {
            oldest = t;
            lru = i;
        }
    }
    return lru;
}
