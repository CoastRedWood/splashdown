// WaterObject.cpp  @ 0x003214F0 (text 0x3214F0-0x324670)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp)
//
// Retail's WaterObject.cpp as one translation unit: WaterObjectData and
// WaterObject (its ASSERTs say "WaterObject.cpp"). JetskiUIWidgets.cpp (the
// JetskiWidget* classes, the next .ctor slot) follows at 0x324670.
// __sinit_WaterObject.cpp builds the axis vectors, gColorBlack and
// gLightDirection. `make link` places its data as well as its code.



#pragma inline_depth(8)

#include "d3dtypes.h"
#include "WaterObjectData/WaterObjectData.h"
#include "WaterObject/WaterObject.h"
#include "DmaRenderItem/DmaRenderItem.h"
#include "DataAccess/DataAccess.h"
#include "SoultreeMaterial/SoultreeMaterial.h"
#include "SoultreeObject/SoultreeObject.h"
#include "PSXRenderStateBlock/PSXRenderStateBlock.h"
#include "VisibleTest/VisibleTest.h"
#include "Camera/Camera.h"
#include "PSXCamera/PSXCamera.h"
#include "RenderTarget/RenderTarget.h"
#include "PSXRenderTarget/PSXRenderTarget.h"
#include "TextureMap/TextureMap.h"
#include "StripMesh/StripMesh.h"
#include "StripList/StripList.h"
#include "sceGifTag/sceGifTag.h"
#include "QWORD/QWORD.h"
#include "soultree_assert_block.h"
#include "sinit/AxisVectors.h"
#include "sinit/D3DIntVector.h"

extern "C" void *malloc(unsigned int size);
extern "C" void free(void *);
extern "C" void *memset(void *dst, int c, unsigned int n);
extern "C" char *strcpy(char *dst, const char *src);
extern "C" char *strcat(char *dst, const char *src);
extern "C" int tolower(int c);

extern DataAccess *LpDataAccess;
extern VisibleTest *VisTest;

class PSXTextureMap;

class PSXTextureMapManager {
public:
    void AddTextureToGifList(PSXTextureMap *texture);
    void AddTextureInterruptToRenderQueue();
};
extern PSXTextureMapManager *LpTextureMapManager;

#define LpPSXRenderTarget ((PSXRenderTarget *)LpRenderTarget)

extern unsigned int WaterObjectCode[];      // VU1 microcode
extern unsigned int StripPreambleCode[];

int RenderWaterObject = 1;
float PolygonScale = 0.9999f;
float gfMaxAlpha = 128.0f;

// Set by the static initializer (with the axis vectors): black (GS colour,
// alpha 0x40 = opaque) and the light direction (straight down). Their .bss
// precedes the functions' statics in retail.
_D3DINTVECTOR gColorBlack(0, 0, 0, 0x40);
_D3DVECTOR gLightDirection(0.0f, -1.0f, 0.0f);

// One strip group of a WaterObjectData (WaterObjectData +0x28).
struct WaterStripGroup {                // 0x14 bytes
    int m_numSurfaces;                  // +0x00
    short *m_materialIndex;             // +0x04, malloc'd, per surface
    StripList **m_surfaces;             // +0x08, malloc'd
    int m_numVertices;                  // +0x0C
    DmaRenderItem *m_renderItems;       // +0x10, per surface
};

// ---- src/WaterObjectData/ctor.cpp

WaterObjectData::WaterObjectData()
{
    m_materials = 0;
    m_numMaterials = 0;
    m_18 = 555;
    m_numGroups = 0;
    m_groups = 0;
    m_20 = 0;
    m_24 = 1.0f;
}

// ---- src/WaterObjectData/dtor.cpp

WaterObjectData::~WaterObjectData()
{
    for (int i = 0; i < m_numMaterials; i++)
        m_materials[i]->Release();
    if (m_materials)
        free(m_materials);
    for (int i = 0; i < m_numGroups; i++) {
        for (int j = 0; j < m_groups[i].m_numSurfaces; j++)
            delete m_groups[i].m_surfaces[j];
        free(m_groups[i].m_materialIndex);
        free(m_groups[i].m_surfaces);
        delete[] m_groups[i].m_renderItems;
    }
    if (m_groups)
        free(m_groups);
    if (m_20)
        free(m_20);
}

// ---- src/WaterObjectData/ReadMaterialsFromBinaryFile.cpp

void WaterObjectData::ReadMaterialsFromBinaryFile(long handle, int a2)
{
    LpDataAccess->FRead(handle, &m_numMaterials, 4, 1);
    if (m_numMaterials > 0) {
        m_materials = (SoultreeMaterial **)malloc(m_numMaterials * sizeof(SoultreeMaterial *));
        for (int i = 0; i < m_numMaterials; i++) {
            m_materials[i] = new SoultreeMaterial(1);
            m_materials[i]->Create(m_18);
            m_materials[i]->ReadFromBinaryFile(handle, a2);
        }
    } else {
        m_materials = 0;
    }
}

// ---- src/WaterObjectData/ReadFromBinaryFile.cpp

void WaterObjectData::ReadFromBinaryFile(long handle, int)
{
    int i;
    int j;

    LpDataAccess->FRead(handle, &m_numGroups, 4, 1);
    m_groups = (WaterStripGroup *)malloc(m_numGroups * sizeof(WaterStripGroup));
#line 99 "WaterObject.cpp"
    ASSERT(m_groups, "ReadFromBinaryFile: out of memory");
    memset(m_groups, 0, m_numGroups * sizeof(WaterStripGroup));
    m_20 = (float *)malloc(m_numGroups * sizeof(float));
    for (i = 0; i < m_numGroups - 1; i++)
        LpDataAccess->FRead(handle, &m_20[i], 4, 1);
    for (i = 0; i < m_numGroups; i++) {
        LpDataAccess->FRead(handle, &m_groups[i].m_numSurfaces, 4, 1);
        m_groups[i].m_materialIndex = (short *)malloc(m_groups[i].m_numSurfaces * sizeof(short));
        m_groups[i].m_surfaces = (StripList **)malloc(m_groups[i].m_numSurfaces * sizeof(StripList *));
        m_groups[i].m_renderItems = new DmaRenderItem[m_groups[i].m_numSurfaces];
        for (j = 0; j < m_groups[i].m_numSurfaces; j++) {
            int value;
            LpDataAccess->FRead(handle, &value, 4, 1);
            m_groups[i].m_materialIndex[j] = value;
            m_groups[i].m_surfaces[j] = new StripList;
            m_groups[i].m_surfaces[j]->ReadFromFile(handle);
        }
    }
}

// ---- src/WaterObject/ctor.cpp

WaterObject::WaterObject(int enabled) : SoultreeObject(enabled)
{
    m_searchVisible = 1;
    m_addToList = 1;
    m_data = 0;
    m_material = 0;
    m_visible = 0;
    m_renderItems = 0;
}

// ---- src/WaterObject/dtor.cpp

WaterObject::~WaterObject()
{
    WaterObjectData *data = m_data;
    if (data) {
        if ((int)data->Release() <= 0)
            LpDataAccess->DetachObject(data);
    }
    delete[] m_renderItems;
}

// ---- src/WaterObject/Create.cpp

// strrchr as inlined in retail.
static inline char *FindLast(char *s, int c)
{
    char *start = s;
    while (*s++)
        ;
    do {
        s--;
    } while (s != start && *s != c);
    return *s == c ? s : 0;
}

// Loads (or shares) the water's .slx data and sets up its 50 render items.
GameObject *WaterObject::Create(char *name, int a, int flags)
{
    char path[0x110];
    long handle;
    WaterObjectData *data;
    int version;
    int i;

    GameObject::Create();
    strcpy(path, name);
    for (char *p = path; *p; p++)
        *p = tolower(*p);
    char *ext = FindLast(path, '.');
    if (ext)
        strcpy(ext, ".slx");
    else
        strcat(path, (char *)".slx");    // redundant casts: retail loads these arguments first
    strcpy(m_name, path);
    handle = -1;
    data = 0;
    if (!LpDataAccess->GetDataOrFileHandle(path, "rb", &handle, (void **)&data, 0)) {
        PSXprintf("WaterObject '%s' not found\n", path);
        Release();
        return 0;
    }
    if (data) {
        m_data = data;
        m_data->AddRef();
    } else {
        LpDataAccess->FRead(handle, &version, 4, 1);
        if (version != 2) {
            if (handle != -1)
                LpDataAccess->FClose(handle);
            Release();
            return 0;
        }
        m_data = new WaterObjectData;
        m_data->ReadMaterialsFromBinaryFile(handle, flags);
        m_data->ReadFromBinaryFile(handle, flags);
        LpDataAccess->AttachObject(handle, m_data);
        LpDataAccess->FClose(handle);
        BuildRenderLists();
    }
    CalculateBoundingBox();
    CalculateMaterialTexelSizes();
    int maxSurfaces = 0;
    WaterStripGroup *groups = m_data->m_groups;
    for (i = 0; i < m_data->m_numGroups; i++) {
        int n = groups[i].m_numSurfaces;
        if (maxSurfaces < n)
            maxSurfaces = n;
    }
#line 229 "WaterObject.cpp"
    ASSERT(maxSurfaces > 0, "Create: maxNumberOfSurfaces <= 0");
    m_renderItems = new DmaRenderItem[50];
    PSXRenderStateBlock block;
    int size = block.GetRenderItemSize() + 0xB8;
    for (i = 0; i < 50; i++) {
        m_renderItems[i].Start((DmaRenderDestination)1, size);
        m_renderItems[i].End();
    }
    return this;
}

// ---- src/WaterObject/PrepareGeometry.cpp

// libvu0 sceVu0MulMatrix: out = m0 * m1 (column vectors), as retail's inline asm.
static inline void MulMatrix(_D3DMATRIX *out, _D3DMATRIX *m0, _D3DMATRIX *m1)
{
    asm("lqc2 vf5, 0x0(%2); lqc2 vf1, 0x0(%1); lqc2 vf2, 0x10(%1); lqc2 vf3, 0x20(%1); lqc2 vf4, 0x30(%1); lqc2 vf6, 0x10(%2); vmulax.xyzw ACC, vf1, vf5x; vmadday.xyzw ACC, vf2, vf5y; vmaddaz.xyzw ACC, vf3, vf5z; vmaddw.xyzw vf9, vf4, vf5w; lqc2 vf7, 0x20(%2); vmulax.xyzw ACC, vf1, vf6x; vmadday.xyzw ACC, vf2, vf6y; vmaddaz.xyzw ACC, vf3, vf6z; vmaddw.xyzw vf10, vf4, vf6w; lqc2 vf8, 0x30(%2); vmulax.xyzw ACC, vf1, vf7x; vmadday.xyzw ACC, vf2, vf7y; vmaddaz.xyzw ACC, vf3, vf7z; vmaddw.xyzw vf11, vf4, vf7w; vmulax.xyzw ACC, vf1, vf8x; vmadday.xyzw ACC, vf2, vf8y; vmaddaz.xyzw ACC, vf3, vf8z; vmaddw.xyzw vf12, vf4, vf8w; sqc2 vf9, 0x0(%0); sqc2 vf10, 0x10(%0); sqc2 vf11, 0x20(%0); sqc2 vf12, 0x30(%0)"
        : : "r"(out), "r"(m0), "r"(m1) : "memory");
}

// The box is the one CalculateBoundingBox stores (center at +0x140, half
// size at +0x150); the matrix is the one at +0xC0.
int WaterObject::PrepareGeometry()
{
    // Profiler ID, initialized but never read.
    static int PrepareGeometryID = -1;
    _D3DMATRIX worldView __attribute__((aligned(16)));
    MulMatrix(&worldView, &LpRenderTarget->m_camera->m_viewProjection, &m_localMatrix);
    m_numRenderItems = 0;
    m_visible = VisTest->BoundingBoxVisible(&worldView, &m_boundingBoxMin, &m_boundingBoxMax, 0);
    return 1;
}

// ---- src/WaterObject/Render3D.cpp

int WaterObject::Render3D()
{
    return 0;
}

// ---- src/WaterObject/Render3D_00322180.cpp
// `_D3DVECTOR *bb = b;` right before the call is such a copy: bb merges into the
// parameter b (which keeps its low vreg, so its slot is still allocated first), and b's
// slot grows from 4 to 16 bytes, which moves i0 and the frame.





int WaterObject::Render3D(int blendMode, _D3DINTVECTOR *color, PSXTextureMap *texture, _D3DVECTOR *a, _D3DVECTOR *b,
                          float f0, float f1, int i0, int clamp, float alpha, _D3DVECTOR *c)
{
    if (!RenderWaterObject || !m_visible)
        return 1;
    // Profiler ID, initialized but never read.
    static int Render3DID = -1;
    PSXRenderStateBlock state;
    state.AlphaTestEnable(blendMode != 4);
    state.SetTextureAddressMode(clamp ? 0 : 1);
    state.AlphaTestEnable(0);
    state.SetAlphaBlendMode(blendMode, gfMaxAlpha * alpha);
    WaterStripGroup *material = &m_data->m_groups[m_material];
    for (int i = 0; i < material->m_numSurfaces; i++) {
        DmaRenderItem *item = &m_renderItems[m_numRenderItems++];
#line 317 "WaterObject.cpp"
        ASSERTF(m_numRenderItems < 50, ("Create: Too many water objects (%d)", m_numRenderItems));
        int size = state.GetRenderItemSize();
        int n = 46;
        if (texture) {
            LpTextureMapManager->AddTextureToGifList(texture);
            LpTextureMapManager->AddTextureInterruptToRenderQueue();
            state.SetTextureMap(texture);
            item->Start((DmaRenderDestination)1, size + n * 4);
            state.ComposeRenderItem(item);
        } else {
            item->Start((DmaRenderDestination)1, size + n * 4);
            state.ComposeSimpleRenderItem(item);
        }
        if (LpPSXRenderTarget->m_lastMPG != WaterObjectCode) {
            LpPSXRenderTarget->AddMPGToRenderQueue(StripPreambleCode);
            LpPSXRenderTarget->AddMPGToRenderQueue(WaterObjectCode);
        }
        _D3DINTVECTOR col = *color;
        if (col.x == 127)
            col.x = 130;
        if (col.y == 127)
            col.y = 130;
        if (col.z == 127)
            col.z = 130;
        if (col.w == 64)
            col.w = 128;
        _D3DVECTOR *bb = b;
        BuildUniqueScissorItem(item, 1, &col, texture != 0, a, bb, f0, f1, i0, clamp, c);
        item->End();
        LpPSXRenderTarget->AddToRenderQueue(item, "");
        // Named first: with the PSXRenderTarget cast, an inline argument moves
        // the lui of LpRenderTarget ahead of the index.
        DmaRenderItem *shared = &material->m_renderItems[i];
        LpPSXRenderTarget->AddToRenderQueue(shared, "");
    }
    return GameObject::Render3D();
}

// ---- src/WaterObject/CalculateBoundingBox.cpp

// CalculateBoundingBox's vector: _D3DVECTOR's layout, but aligned(16), so its
// by-value copies are lq/sq.
struct BoxVector {
    BoxVector() { x = y = z = 0.0f; w = 1.0f; }
    BoxVector(float ax, float ay, float az) { x = ax; y = ay; z = az; w = 1.0f; }
    float x, y, z, w;
#ifdef SPLASHDOWN_PC
};  // m_boundingBoxMin is at +0x148 on the PC: not 16-aligned (movaps faulted)
#else
} __attribute__((aligned(16)));
#endif

static inline BoxVector operator+(const BoxVector &a, const BoxVector &b)
{
    return BoxVector(a.x + b.x, a.y + b.y, a.z + b.z);
}

static inline BoxVector operator-(const BoxVector &a, const BoxVector &b)
{
    return BoxVector(a.x - b.x, a.y - b.y, a.z - b.z);
}

static inline BoxVector operator*(float s, const BoxVector &v)
{
    return BoxVector(s * v.x, s * v.y, s * v.z);
}

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

static inline float Sqrt(float x)
{
    float r;
    asm("sqrt.s $f8, %1; swc1 $f8, 0(%0)" : : "r"(&r), "f"(x) : "$f8", "memory");
    return r;
}

// SetBoundingBox(min, max) as retail inlines it: center at +0x140, half size
// at +0x150.
static inline void SetBoundingBox(WaterObject *object, BoxVector min, BoxVector max)
{
    BoxVector &center = *(BoxVector *)&object->m_boundingBoxMin;
    BoxVector &half = *(BoxVector *)&object->m_boundingBoxMax;
    center = 0.5f * (max + min);
    half = 0.5f * (max - min);
    object->m_boundingRadius = Sqrt(object->m_boundingBoxMax.x * object->m_boundingBoxMax.x +
                                    object->m_boundingBoxMax.y * object->m_boundingBoxMax.y +
                                    object->m_boundingBoxMax.z * object->m_boundingBoxMax.z);
    object->m_boundingBoxValid = 1;
}

// Bounding box (center, half size, radius) of every vertex of the water's
// meshes; an empty box at the origin when there are none.
void WaterObject::CalculateBoundingBox()
{
#line 398 "WaterObject.cpp"
    ASSERT(m_data, "CalculateBoundingBox: LpWaterObjectData==NULL");
    m_x190_bit2 = 0;
    BoxVector min, max;
    int i;
    int j;
    int n;
    int k;
    StripList *mesh;
    int count;
    StripVertex *vertices;
    StripMesh *strip;
    min.x = 64000.0f;
    min.y = 64000.0f;
    min.z = 64000.0f;
    max.x = -64000.0f;
    max.y = -64000.0f;
    max.z = -64000.0f;
    for (i = 0; i < m_data->m_numGroups; i++) {
        for (j = 0; j < m_data->m_groups[i].m_numSurfaces; j++) {
            mesh = m_data->m_groups[i].m_surfaces[j];
            for (k = 0; k < mesh->m_numStrips; k++) {
                strip = &mesh->m_strips[k];
                count = strip->m_numVertices;
                vertices = strip->m_vertices;
                for (n = 0; n < count; n++) {
                    max.x = Fmax(vertices[n].x, max.x);
                    max.y = Fmax(vertices[n].y, max.y);
                    max.z = Fmax(vertices[n].z, max.z);
                    min.x = Fmin(vertices[n].x, min.x);
                    min.y = Fmin(vertices[n].y, min.y);
                    min.z = Fmin(vertices[n].z, min.z);
                    m_x190_bit2 = 1;
                }
            }
        }
    }
    if (!m_x190_bit2) {
        min = BoxVector(0.0f, 0.0f, 0.0f);
        max = BoxVector(0.0f, 0.0f, 0.0f);
    }
    SetBoundingBox(this, min, max);
}

// ---- src/WaterObject/CalculateMaterialTexelSizes.cpp

static inline float RSqrt(float x)
{
    float r;
    asm("rsqrt.s $f8, %0, %1; swc1 $f8, 0(%2)" : : "f"(1.0f), "f"(x), "r"(&r) : "$f8", "memory");
    return r;
}

static inline float Length(const _D3DVECTOR &v)
{
    float len2 = v.x * v.x + v.y * v.y + v.z * v.z;
    if (len2 == 0.0f)
        return 0.0f;
    return 1.0f / RSqrt(len2);
}

// For each surface, the largest ratio of a triangle edge's length to its
// length in texture space, divided by the texture's height, raises the
// materials' texel size (SoultreeMaterial +0xB0).
void WaterObject::CalculateMaterialTexelSizes()
{
    int i;
    int j;
    int k;
    int n;
    float maxRatio;
    float uvLength;
#line 452 "WaterObject.cpp"
    ASSERT(m_data, "CalculateMaterialTexelSizes: LpWaterObjectData==NULL");
    _D3DVECTOR p0;
    _D3DVECTOR p1;
    _D3DVECTOR p2;
    for (i = 0; i < m_data->m_numGroups; i++) {
        WaterStripGroup *group = &m_data->m_groups[i];
        for (j = 0; j < group->m_numSurfaces; j++) {
            maxRatio = 0.0f;
            StripList *surface = group->m_surfaces[j];
            for (k = 0; k < surface->m_numStrips; k++) {
                StripMesh *strip = &surface->m_strips[k];
                StripVertex *v = strip->m_vertices;
                for (n = 0; n < strip->m_numVertices - 2; n++) {
                    p0.x = v[n].x;
                    p0.y = v[n].y;
                    p0.z = v[n].z;
                    p1.x = v[n + 1].x;
                    p1.y = v[n + 1].y;
                    p1.z = v[n + 1].z;
                    p2.x = v[n + 2].x;
                    p2.y = v[n + 2].y;
                    p2.z = v[n + 2].z;
                    _D3DVECTOR uvEdge = _D3DVECTOR(0.0f, 0.0f, 0.0f);
                    _D3DVECTOR posEdge = _D3DVECTOR(0.0f, 0.0f, 0.0f);
                    uvEdge.x = v[n].u - v[n + 1].u;
                    uvEdge.y = v[n].v - v[n + 1].v;
                    posEdge = p0 - p1;
                    uvLength = Length(uvEdge);
                    if (uvLength <= 0.01f)
                        uvLength = 10000.0f;
                    maxRatio = Fmax(maxRatio, Length(posEdge) / uvLength);
                    uvEdge.x = v[n + 1].u - v[n + 2].u;
                    uvEdge.y = v[n + 1].v - v[n + 2].v;
                    posEdge = p1 - p2;
                    uvLength = Length(uvEdge);
                    if (uvLength <= 0.01f)
                        uvLength = 10000.0f;
                    maxRatio = Fmax(maxRatio, Length(posEdge) / uvLength);
                    uvEdge.x = v[n + 2].u - v[n].u;
                    uvEdge.y = v[n + 2].v - v[n].v;
                    posEdge = p2 - p0;
                    uvLength = Length(uvEdge);
                    if (uvLength <= 0.01f)
                        uvLength = 10000.0f;
                    maxRatio = Fmax(maxRatio, Length(posEdge) / uvLength);
                }
            }
            // Retail repeats the same update m_numMaterials times.
            for (int m = 0; m < m_data->m_numMaterials; m++) {
                SoultreeMaterial *material = m_data->m_materials[group->m_materialIndex[j]];
                TextureMap *texture = material->GetTextureMap();
                if (texture) {
                    float *texelSize = (float *)&material->m_xB0;
                    *texelSize = Fmax(*texelSize, maxRatio / texture->m_height);
                }
            }
        }
    }
}

// ---- src/WaterObject/BuildUniqueScissorItem.cpp

#pragma fast_fptosi on

typedef int int128_t __attribute__((mode(TI)));

static inline void AddMatrix(DmaRenderItem *item, _D3DMATRIX *matrix)
{
    *(_D3DMATRIX *)&item->m_dataList[item->m_dataListCurrentSize] = *matrix;
    DmaRenderItem::UnpackCount += 16;
    item->m_dataListCurrentSize += 16;
}

// Fills a water object's unique VU1 packet: its world matrix, the camera
// matrices, a light matrix, the colour, three GIF tags and the texture
// scroll (uv) offsets.
void WaterObject::BuildUniqueScissorItem(DmaRenderItem *item, int a, _D3DINTVECTOR *color, int textured,
                                         _D3DVECTOR *c, _D3DVECTOR *d, float e, float f, int g, int h,
                                         _D3DVECTOR *i)
{
    PSXCamera *camera = (PSXCamera *)LpRenderTarget->m_camera;
    static int ScissorRenderItemID = -1;    // unused profiler ID
    sceGifTag tag;
    _D3DMATRIX light;
    float *q;
    float u;
    float v;

    item->DmaAddVifCode(0x11000000);    // FLUSH
    item->DmaAddVifCode(0x03000000);    // BASE
    item->DmaAddVifCode(0x02000000);    // OFFSET
    item->DmaBeginUnpack(0, 12);
    ValidateWorldMatrix();
    AddMatrix(item, &m_worldMatrix);
    AddMatrix(item, &camera->m_guardBandProjection);
    AddMatrix(item, &camera->m_projection);
    AddMatrix(item, &camera->m_x140);
    AddMatrix(item, &camera->m_view);
    light = IdentityMatrix();
    light._11 = -gLightDirection.x;
    light._31 = -gLightDirection.z;
    light._21 = -gLightDirection.y;
    light._22 = 0.0f;
    light._33 = 0.0f;
    light._44 = 1.0f;
    AddMatrix(item, &light);
    q = (float *)&item->m_dataList[item->m_dataListCurrentSize];
    ((unsigned int *)q)[0] = 0;
    ((unsigned int *)q)[1] = 0;
    ((unsigned int *)q)[2] = 0;
    ((unsigned int *)q)[3] = 0;
    DmaRenderItem::UnpackCount += 4;
    item->m_dataListCurrentSize += 4;
    q = (float *)&item->m_dataList[item->m_dataListCurrentSize];
    ((unsigned int *)q)[0] = 0;
    ((unsigned int *)q)[1] = 0;
    ((unsigned int *)q)[2] = 0;
    ((unsigned int *)q)[3] = 0;
    DmaRenderItem::UnpackCount += 4;
    item->m_dataListCurrentSize += 4;
    q = (float *)&item->m_dataList[item->m_dataListCurrentSize];
    ((unsigned int *)q)[0] = 0;
    ((unsigned int *)q)[1] = 0;
    ((unsigned int *)q)[2] = 0;
    ((unsigned int *)q)[3] = 0;
    DmaRenderItem::UnpackCount += 4;
    item->m_dataListCurrentSize += 4;
    q = (float *)&item->m_dataList[item->m_dataListCurrentSize];
    // The colour channels are converted as unsigned values.
    q[0] = (unsigned int)color->x;
    q[1] = (unsigned int)color->y;
    q[2] = (unsigned int)color->z;
    q[3] = (unsigned int)color->w;
    DmaRenderItem::UnpackCount += 4;
    item->m_dataListCurrentSize += 4;
    AddMatrix(item, &m_worldMatrix);
    q = (float *)&item->m_dataList[item->m_dataListCurrentSize];
    q[0] = camera->m_position.x;
    q[1] = camera->m_position.y;
    q[2] = camera->m_position.z;
    q[3] = camera->m_position.w;
    DmaRenderItem::UnpackCount += 4;
    item->m_dataListCurrentSize += 4;
    q = (float *)&item->m_dataList[item->m_dataListCurrentSize];
    q[0] = 2.0e-5f;
    q[1] = -16.0f;
    q[2] = LpPSXRenderTarget->m_fogEnd;
    q[3] = LpPSXRenderTarget->m_fogScale;
    DmaRenderItem::UnpackCount += 4;
    item->m_dataListCurrentSize += 4;

    *(int128_t *)&tag = 0;
    tag.EOP = 1;
    tag.PRE = 1;
    tag.PRIM = (long)textured << 4 | 0x4D;
    tag.FLG = 0;
    tag.NREG = 3;
    tag.REGS0 = 2;
    tag.REGS1 = 1;
    tag.REGS2 = 4;
    tag.NLOOP = 5;
    item->DmaAddGifTagToUnpack(&tag);

    memset(&tag, 0, sizeof(tag));
    tag.NLOOP = 1;
    tag.EOP = 1;
    tag.PRE = 1;
    tag.FLG = 0;
    tag.NREG = 1;
    tag.REGS0 = 0xF;
    item->DmaAddGifTagToUnpack(&tag);
    q = (float *)&item->m_dataList[item->m_dataListCurrentSize];
    ((unsigned int *)q)[0] = 0;
    ((unsigned int *)q)[1] = 0;
    ((unsigned int *)q)[2] = 0;
    ((unsigned int *)q)[3] = 0x7F;
    DmaRenderItem::UnpackCount += 4;
    item->m_dataListCurrentSize += 4;

    memset(&tag, 0, sizeof(tag));
    tag.NLOOP = 1;
    tag.EOP = 1;
    tag.PRE = 1;
    tag.FLG = 0;
    tag.NREG = 1;
    tag.REGS0 = 0xF;
    item->DmaAddGifTagToUnpack(&tag);
    q = (float *)&item->m_dataList[item->m_dataListCurrentSize];
    ((unsigned int *)q)[0] = 0;
    ((unsigned int *)q)[1] = 0;
    ((unsigned int *)q)[2] = 0;
    ((unsigned int *)q)[3] = 0x7F;
    DmaRenderItem::UnpackCount += 4;
    item->m_dataListCurrentSize += 4;

    q = (float *)&item->m_dataList[item->m_dataListCurrentSize];
    ((unsigned int *)q)[0] = 0;
    ((unsigned int *)q)[1] = 0;
    ((unsigned int *)q)[2] = 0;
    ((unsigned int *)q)[3] = 0;
    DmaRenderItem::UnpackCount += 4;
    item->m_dataListCurrentSize += 4;
    q = (float *)&item->m_dataList[item->m_dataListCurrentSize];
    ((unsigned int *)q)[0] = 0;
    ((unsigned int *)q)[1] = 0;
    ((unsigned int *)q)[2] = 0;
    ((unsigned int *)q)[3] = 0;
    DmaRenderItem::UnpackCount += 4;
    item->m_dataListCurrentSize += 4;
    q = (float *)&item->m_dataList[item->m_dataListCurrentSize];
    ((unsigned int *)q)[0] = a ? 1 : 0;
    ((unsigned int *)q)[1] = 0;
    ((unsigned int *)q)[2] = 0;
    ((unsigned int *)q)[3] = 0;
    DmaRenderItem::UnpackCount += 4;
    item->m_dataListCurrentSize += 4;
    q = (float *)&item->m_dataList[item->m_dataListCurrentSize];
    q[0] = c->x;
    q[1] = c->y;
    ((unsigned int *)q)[2] = 0;
    q[3] = PolygonScale;
    DmaRenderItem::UnpackCount += 4;
    item->m_dataListCurrentSize += 4;

    if (g) {
        if (h) {
            u = e * ((m_localMatrix._41 - d->x) - 128.0f);
            v = f * (128.0f + (m_localMatrix._43 - d->y));
            // A named second term keeps retail's add operand order.
            float u2 = e * (128.0f + (i->x - m_localMatrix._41));
            u -= (int)(u2 + u);
            float v2 = f * ((i->z - m_localMatrix._43) - 128.0f);
            v -= (int)(v2 + v);
        } else {
            u = e * ((m_localMatrix._41 - 2.0f * d->x) - 128.0f);
            v = f * (128.0f + (m_localMatrix._43 - 2.0f * d->y));
        }
    } else {
        u = e * ((d->x + m_localMatrix._41) - 128.0f);
        v = f * (128.0f + (d->y + m_localMatrix._43));
    }
    q = (float *)&item->m_dataList[item->m_dataListCurrentSize];
    q[0] = u;
    q[1] = v;
    ((unsigned int *)q)[2] = 0;
    q[3] = 1.0f;
    DmaRenderItem::UnpackCount += 4;
    item->m_dataListCurrentSize += 4;
    item->DmaEndUnpack();
    item->DmaAddVifCodeMSCAL();
    item->DmaAddVifCode(0x10000000);    // FLUSHE
    item->DmaAddVifCode(0x030000B5);    // BASE
    item->DmaAddVifCode(0x02000190);    // OFFSET
}

// ---- src/WaterObject/BuildRenderLists.cpp

// Sends the vertices gathered so far as one GIF packet.
#define FLUSH()                                                         \
    tag.NLOOP = count;                                                  \
    item->DmaBeginUnpack(0, 12);                                        \
    item->DmaAddGifTagToUnpack(&tag);                                   \
    item->DmaEndUnpack();                                               \
    item->DmaBeginUnpack(-1, 12);                                       \
    for (i = 0; i < count; i++)                                         \
        item->DmaAddQwordToUnpack(&positions[i]);                       \
    item->DmaEndUnpack();                                               \
    item->DmaBeginUnpack(-1, 8);                                        \
    for (i = 0; i < count; i++)                                         \
        item->DmaAddDataToUnpack(&normals[i], 3);                       \
    item->DmaEndUnpack();                                               \
    item->DmaBeginUnpack(-1, 4);                                        \
    for (i = 0; i < count; i++)                                         \
        item->DmaAddDataToUnpack(&uvs[i], 2);                           \
    item->DmaEndUnpack();                                               \
    item->DmaBeginUnpack(-1, 12);                                       \
    for (i = 0; i < count; i++)                                         \
        item->DmaAddIntVectorToUnpack(&gColorBlack);                    \
    item->DmaEndUnpack();                                               \
    item->DmaAddVifCode(0x17000000);    /* MSCNT */                     \
    m_data->m_groups[g].m_numVertices += count;

// Builds each surface's render item: its triangle strips cut into packets of
// at most 16 vertices (strips that don't fit restart two vertices back).
void WaterObject::BuildRenderLists()
{
    sceGifTag tag;
    QWORD positions[32];
    QWORD normals[32];
    QWORD uvs[32];
    int i;
    StripVertex *vertices;
    int remaining;
    int n;
    _D3DVECTOR *stripNormals;
    int vertex;
    int count;
    int space;
    int g;
    int s;
    int pending;
    int strip;
    int numSurfaces;
    StripList *surface;
    DmaRenderItem *item;

    *(int128_t *)&tag = 0;
    tag.EOP = 1;
    tag.PRE = 1;
    tag.PRIM = 0x5C;
    tag.FLG = 0;
    tag.NREG = 3;
    tag.REGS0 = 2;
    tag.REGS1 = 1;
    tag.REGS2 = 4;
    vertex = 0;
    for (g = 0; g < m_data->m_numGroups; g++) {
        // groups[g] rather than a WaterStripGroup *group local: the address then
        // stays a codegen temporary, which sets retail's spill-slot order.
        WaterStripGroup *groups = m_data->m_groups;
        numSurfaces = groups[g].m_numSurfaces;
        groups[g].m_numVertices = 0;
        for (s = 0; s < numSurfaces; s++) {
            surface = groups[g].m_surfaces[s];
            item = &groups[g].m_renderItems[s];
            space = 16;
            count = 0;
            item->Start((DmaRenderDestination)1, 0x80);
            remaining = 0;
            pending = 0;
            for (strip = 0; strip < surface->m_numStrips; strip++) {
                if (remaining == 0) {
                    StripMesh *st = &surface->m_strips[strip];
                    vertex = 0;
                    vertices = st->m_vertices;
                    remaining = st->m_numVertices;
                    stripNormals = st->m_normals;
                }
                if (space < remaining) {
                    if (pending) {
                        FLUSH()
                    }
                    space = 16;
                    count = 0;
                }
                if (space < remaining)
                    n = 16;
                else
                    n = remaining;
                int left = remaining;
                if (n)
                    pending = 1;
                for (i = 0; i < n; i++) {
                    positions[count].f[0] = vertices[vertex].x;
                    positions[count].f[1] = vertices[vertex].y;
                    positions[count].f[2] = vertices[vertex].z;
                    positions[count].ui[3] = i < 2 ? -14 : 14;
                    normals[count].f[0] = stripNormals[vertex].x;
                    normals[count].f[1] = stripNormals[vertex].y;
                    normals[count].f[2] = stripNormals[vertex].z;
                    uvs[count].f[0] = vertices[vertex].u;
                    uvs[count].f[1] = vertices[vertex].v;
                    count++;
                    vertex++;
                }
                left -= n;
                space -= n;
                remaining -= n;
                if (left) {
                    vertex -= 2;
                    strip--;
                    remaining += 2;
                }
            }
            if (pending) {
                FLUSH()
            }
            item->End();
        }
    }
}

// ---- src/sinit/WaterObject.cpp
// Static initializer of WaterObject.cpp: the axis vectors, black and the
// light direction (defined at the top).
