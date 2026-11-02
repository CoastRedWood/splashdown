// PSXRenderableObject.cpp  @ 0x00343480 (text 0x343480-0x344890)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's PSXRenderableObject.cpp as one translation unit:
// PSXRenderableObjectData's ctor, dtor and ReadFromBinaryFile first
// (0x343480-0x343710; the TU's .rodata holds their only RTTI copy), then
// PSXRenderableObject's functions in retail order, then its static initializer.
// `make link` places the object's data as well as its code (tools/link_c.py).
// m_data is a PSXRenderableObjectData here: its +0x28 block is read as the
// triangle strips (Create), the part count (dtor) and the hierarchy (Render3D).




#include "d3dtypes.h"
#include "GameObject/GameObject.h"
#include "SoultreeObject/SoultreeObject.h"
#include "PSXRenderableObject/PSXRenderableObject.h"
#include "PSXRenderableObjectData/PSXRenderableObjectData.h"
#include "global/Vu0Inline.h"
#include "VisibleTest/VisibleTest.h"
#include "Camera/Camera.h"
#include "PSXCamera/PSXCamera.h"
#include "RenderTarget/RenderTarget.h"
#include "PSXRenderTarget/PSXRenderTarget.h"
#include "StringTable/StringTable.h"
#include "SurfaceShaderList/SurfaceShaderList.h"
#include "PSXSurfaceGeometry/PSXSurfaceGeometry.h"
#include "PSXSurfaceShaderInstance/PSXSurfaceShaderInstance.h"
#include "LightManager/LightManager.h"
#include "DmaRenderItem/DmaRenderItem.h"
#include "SurfaceMaterial/SurfaceMaterial.h"
#include "ShaderDescriptor/ShaderDescriptor.h"
#include "soultree_assert.h"
// The axis vectors every Soultree TU defines (and __sinit constructs).
#include "sinit/AxisVectors.h"
#include "DataAccess/DataAccess.h"

// Defined here for its retail .bss slot.
int DoViewportClipping;


extern "C" void *memalign(unsigned int align, unsigned int size);
extern "C" void *memset(void *dst, int c, unsigned int n);
extern "C" void free(void *p);
extern "C" void *malloc(unsigned int size);

extern RenderTarget *LpRenderTarget;
extern DataAccess *LpDataAccess;
extern VisibleTest *VisTest;
extern StringTable *LpMaterialStringTable;
extern SurfaceShaderList *LpSurfaceShaderList;
extern LightManager *LpLightManager;
int DrawAnimatedFlag = 1;
int DoScissoring = 1;
extern int DoViewportClipping;
float MaxFalloffDistance = 10000.0f;
float MultipassCullDistance = 130.0f;
void DrawBox(_D3DVECTOR *corners);

// This TU's QWORD is the float view, 4-aligned (QWORD/QWORD.h: files that
// view it as float x/y/z + int w keep their own copy). Render3D's frame and
// the order it computes GetThreeLights' buffer addresses need it; the
// 16-aligned shared one, or float buffers cast to it, don't match.
struct QWORD {
    float x, y, z;
    int w;
};

// ---- src/PSXRenderableObjectData/ctor.cpp

PSXRenderableObjectData::PSXRenderableObjectData()
{
    m_strings = 0;
}


// ---- src/PSXRenderableObjectData/dtor.cpp

PSXRenderableObjectData::~PSXRenderableObjectData()
{
    if (m_strings) {
        if (m_strings->m_lengths)
            delete[] m_strings->m_lengths;
        if (m_strings->m_strings)
            delete[] m_strings->m_strings;
        if (m_strings->m_data)
            delete[] m_strings->m_data;
        free(m_strings);
    }
}


// ---- src/PSXRenderableObjectData/ReadFromBinaryFile.cpp

void PSXRenderableObjectData::ReadFromBinaryFile(long file, InstanceMaster *master)
{
    RenderableObjectData::ReadFromBinaryFile(file, master);
    int count;
#ifdef SPLASHDOWN_PC
    count = 0;  // some files end before it (lh_houseboatb.psg): the read fails
#endif
    LpDataAccess->FRead(file, &count, 4, 1);
    if (count) {
        m_strings = (RenderableStrings *)malloc(sizeof(RenderableStrings));
        m_strings->m_count = count;
        m_strings->m_lengths = new int[count];
        m_strings->m_strings = new unsigned char *[count];
        int size;
        LpDataAccess->FRead(file, &size, 4, 1);
        m_strings->m_data = new char[size];
        LpDataAccess->FRead(file, m_strings->m_data, 1, size);
        int offset = 0;
        for (int i = 0; i < count; i++) {
            int length;
            LpDataAccess->FRead(file, &length, 4, 1);
            m_strings->m_lengths[i] = length;
            m_strings->m_strings[i] = (unsigned char *)(m_strings->m_data + offset);
            offset += length + ((4 - (length & 3)) & 3);
        }
    }
}


// ---- src/PSXRenderableObject/Create.cpp

// Builds the VIF/DMA packet(s) the VU1 renderer kicks: one per triangle strip
// when the geometry has strips, else a single fixed packet. VIF codes are
// written as raw words.

// DMA tag at the head of each packet: DmaRenderItem.h's DmaTag (libdma's sceDmaTag).

GameObject *PSXRenderableObject::Create(char *name, int a, int flags)
{
    if (!RenderableObject::Create(name, a, flags))
        return 0;
    m_x1B1Bits.m_lit = flags;           // bit 0 of flags
    RenderableStrings *strips = ((PSXRenderableObjectData *)m_data)->m_strings;
    if (strips) {
        int count = strips->m_count;
        DmaTag **packets = new DmaTag *[count];
        m_partBuffers = (void **)packets;
        for (int i = 0; i < count; i++) {
            int n = ((PSXRenderableObjectData *)m_data)->m_strings->m_lengths[i];
            int size = ((n * 4 + 0x28) + n * 16) * 4 + 0x10;
            if (m_x1B1Bits.m_lit)
                size += 0xA0;
            ((DmaTag **)m_partBuffers)[i] = (DmaTag *)memalign(0x40, size);
            memset(((DmaTag **)m_partBuffers)[i], 0, size);
            DmaTag *tag = ((DmaTag **)m_partBuffers)[i];
            unsigned int *data = (unsigned int *)(tag + 1);
            data[0] = 0x11000000;
            data[1] = 0x03000000;
            data[2] = 0x02000000;
            data[3] = 0x6C018029;
            data[7] = n;
            data[11] = 0x6C048000;
            unsigned int *p = data + 0x1C;
            // VIF UNPACK V4-32 (num 4) per strip vertex; the code is held in a
            // local in retail (a folded constant schedules differently).
            unsigned int unpack = 0x6C040000;
            int addr = 0xB6;
            for (int j = 0; j < n; j++) {
                p[3] = (addr | 0x8000) | unpack;
                p += 0x14;
                addr += 0x10;
            }
            if (m_x1B1Bits.m_lit) {
                p[3] = 0x6C048014;
                p[0x17] = 0x6C048018;
                p += 0x28;
            }
            p[3] = 0x6C01802A;
            p[4] = 0x38000100;
            p[5] = 0x3C010204;
            p[6] = 0x3A001002;
            p[8] = 0x14000019;
            p[9] = 0x10000000;
            p[10] = 0x030002C5;
            p[11] = 0x02000088;
            tag = ((DmaTag **)m_partBuffers)[i];
            tag->qwc = (unsigned int)(size - 0x10) >> 4;
            tag->mark = 0;
            tag->id = 0x30;
            tag->addr = (unsigned int *)(tag + 1);
            tag->pad[0] = 0;
            tag->pad[1] = 0;
        }
    } else {
        unsigned int size = 0xB0;
        if (m_x1B1Bits.m_lit)
            size += 0xA0;
        m_partBuffers = (void **)memalign(0x40, size);
        memset(m_partBuffers, 0, size);
        DmaTag *tag = (DmaTag *)m_partBuffers;
        unsigned int *data = (unsigned int *)(tag + 1);
        data[0] = 0x11000000;
        data[1] = 0x03000000;
        data[2] = 0x02000000;
        data[3] = 0x6C018029;
        data[7] = 0;
        data[11] = 0x6C048000;
        unsigned int *p = data + 0x1C;
        if (m_x1B1Bits.m_lit) {
            p[3] = 0x6C048014;
            p[0x17] = 0x6C048018;
            p += 0x28;
        }
        p[3] = 0x6C01802A;
        p[4] = 0x38000100;
        p[5] = 0x3C010204;
        p[6] = 0x3A001002;
        p[8] = 0x14000019;
        p[9] = 0x10000000;
        p[10] = 0x030000B5;
        p[11] = 0x02000190;
        tag = (DmaTag *)m_partBuffers;
        tag->qwc = (size - 0x10) >> 4;
        tag->mark = 0;
        tag->id = 0x30;
        tag->addr = (unsigned int *)(tag + 1);
        tag->pad[0] = 0;
        tag->pad[1] = 0;
    }
    return this;
}


// ---- src/PSXRenderableObject/ctor.cpp





PSXRenderableObject::PSXRenderableObject(int id) : RenderableObject(id)
{
    m_partBuffers = 0;
}


// ---- src/PSXRenderableObject/dtor.cpp
PSXRenderableObject::~PSXRenderableObject()
{
    int i, n;

    if (m_data && ((PSXRenderableObjectData *)m_data)->m_strings) {
        n = ((PSXRenderableObjectData *)m_data)->m_strings->m_count;
        if (m_partBuffers) {
            for (i = 0; i < n; i++) {
                if (m_partBuffers[i])
                    free(m_partBuffers[i]);
            }
            delete[] m_partBuffers;
        }
    } else if (m_partBuffers) {
        free(m_partBuffers);
    }
}


// ---- src/PSXRenderableObject/BoundingBoxVisible.cpp
// Clip-test the bounding box; also note whether it needs clipping at all,
// and whether it stays inside the guard band.
int PSXRenderableObject::BoundingBoxVisible()
{
    static int calculateClipCodes = 1;
    static int doAdditionalGuardBandCheck = 1;
    ValidateWorldMatrix();
    _D3DMATRIX m __attribute__((aligned(16)));
    Vu0MulMatrix(&m, &LpRenderTarget->m_camera->m_viewProjection, &m_worldMatrix);
    m_x1B1Bits.m_boxVisible = 1;
    m_x1B1Bits.m_insideGuardBand = 0;
    int visible;
    if (calculateClipCodes) {
        unsigned int clipCodes = 0;
        if (doAdditionalGuardBandCheck) {
            if ((visible = VisTest->BoundingBoxVisible(&m, &m_localCenter, &m_localHalf, &clipCodes)) != 0) {
                if (!clipCodes)
                    m_x1B1Bits.m_insideGuardBand = 1;
                static int drawBoxes = 0;
                _D3DMATRIX guard __attribute__((aligned(16)));
                Vu0MulMatrix(&guard, &((PSXCamera *)LpRenderTarget->m_camera)->m_guardBandViewProjection, &m_worldMatrix);
                VisTest->BoundingBoxVisible(&guard, &m_localCenter, &m_localHalf, &clipCodes);
                if (0 && drawBoxes) {
                    // Debug drawing, compiled out in retail (the static is
                    // still initialized and the buffer still takes stack).
                    _D3DVECTOR corners[18];
                    DrawBox(corners);
                }
            }
        } else {
            visible = VisTest->BoundingBoxVisible(&m, &m_localCenter, &m_localHalf, &clipCodes);
            if (!clipCodes)
                m_x1B1Bits.m_insideGuardBand = 1;
        }
        clipCodes &= ~0x10;
        if (visible) {
            m_x1B1Bits.m_boxVisible = clipCodes ? 1 : 0;
            m_x1B1Bits.m_clip = clipCodes ? 1 : 0;
        }
    } else {
        if ((visible = VisTest->BoundingBoxVisible(&m, &m_localCenter, &m_localHalf, 0)) != 0)
            m_x1B1Bits.m_clip = 1;
    }
    return visible;
}


// ---- src/PSXRenderableObject/SetMultipassCullDistance.cpp

void PSXRenderableObject::SetMultipassCullDistance(float distance)
{
    MultipassCullDistance = distance;
}


// ---- src/PSXRenderableObject/ChangeFillColor.cpp
// Sets the colour of a shader pass (or the first fill-colour pass when pass
// is 0) on every surface. Returns that pass.
int PSXRenderableObject::ChangeFillColor(int pass, int color)
{
#line 444 "PSXRenderableObject.cpp"
    ASSERT(pass >= 0, "Invalid shader pass requested.");
    int i;
    unsigned char r = color >> 16;
    unsigned char g = color >> 8;
    int found = 0;
    unsigned char b = color;
    unsigned char a = color >> 24;
    for (i = 0; i < m_data->m_numLODs; i++) {
        SurfaceMaterial *shader = (SurfaceMaterial *)LpMaterialStringTable->GetPointer(m_data->m_lods[i].m_geometries->m_materialIndex);
        ShaderDescriptor *p;
#line 462 "PSXRenderableObject.cpp"
        ASSERT(pass <= shader->m_numShaders, "Invalid shader pass requested.");
        if (pass) {
            p = shader->m_shaders[pass - 1];
            if (p->m_fillTexture == 1)
                found = pass;
        } else {
            for (int j = 0; j < shader->m_numShaders; j++) {
                p = shader->m_shaders[j];
                if (p->m_fillTexture == 1) {
                    found = j;
                    break;
                }
            }
        }
#line 484 "PSXRenderableObject.cpp"
        ASSERT(found != 0, "No fill color pass found.");
        p->m_fillColor = (a << 24) | (b << 16) | (g << 8) | r;
    }
    return found;
}


// ---- src/PSXRenderableObject/Render3D.cpp
// Hand-written quadword copies (retail uses $at): a SoultreeObject's world
// matrix into a packet, and four quadwords.
#ifdef SPLASHDOWN_PC
// x86-64: SoultreeObject::m_worldMatrix isn't at +0x100
#define COPY_WORLD_MATRIX_40(object, dst) \
    __builtin_memcpy((char *)(dst) + 0x40, &((SoultreeObject *)(object))->m_worldMatrix, 0x40)
#define COPY_WORLD_MATRIX(object, dst) \
    __builtin_memcpy((char *)(dst), &((SoultreeObject *)(object))->m_worldMatrix, 0x40)
#else
#define COPY_WORLD_MATRIX_40(object, dst) \
    asm("lq $1, 0x100(%0); lq $2, 0x110(%0); lq $3, 0x120(%0); lq $4, 0x130(%0); sq $1, 0x40(%1); sq $2, 0x50(%1); sq $3, 0x60(%1); sq $4, 0x70(%1)" \
        : : "r"(object), "r"(dst) : "$1", "$2", "$3", "$4", "memory")
#define COPY_WORLD_MATRIX(object, dst) \
    asm("lq $1, 0x100(%0); lq $2, 0x110(%0); lq $3, 0x120(%0); lq $4, 0x130(%0); sq $1, 0x0(%1); sq $2, 0x10(%1); sq $3, 0x20(%1); sq $4, 0x30(%1)" \
        : : "r"(object), "r"(dst) : "$1", "$2", "$3", "$4", "memory")
#endif

static inline void Copy4Qwords(void *src, void *dst)
{
    asm("lq $1, 0x0(%0); lq $2, 0x10(%0); lq $3, 0x20(%0); lq %0, 0x30(%0); sq $1, 0x0(%1); sq $2, 0x10(%1); sq $3, 0x20(%1); sq %0, 0x30(%1)"
        : : "r"(src), "r"(dst) : "$1", "$2", "$3", "memory");
}

// Copies the world matrix of each listed object into a part's packet. Retail
// inlined a helper here: its locals' register numbering gives retail's $s choice.
inline char *PSXRenderableObject::CopyObjectMatrices(unsigned char *indices, int count, char *cursor)
{
    int j;
    unsigned char *index = indices;
    SoultreeObject *object;
    for (j = 0; j < count; j++) {
        object = m_objects[*index];
        if (object) {
            object->ValidateWorldMatrix();
            COPY_WORLD_MATRIX(object, cursor);
        }
        index++;
        cursor += 0x50;
    }
    return cursor;
}

int PSXRenderableObject::Render3D()
{
    QWORD ambient[4];
    int count;
    RenderableObjectLOD *lod;
    int numParts;
    char *buffer;
    int i;
    RenderableStrings *hierarchy;
    int multipass;
    float radius;
    unsigned int material;
    char *cursor;
    unsigned char *index;
    if (!m_visible)
        return 1;
    numParts = 0;
    ValidateWorldMatrix();
    _D3DVECTOR position(m_localMatrix._41, m_localMatrix._42, m_localMatrix._43);
    QWORD lights0[4];
    QWORD lights1[4];
    radius = m_numObjects == 1 ? m_boundingRadius : m_searchRadius;
    radius = m_distance + radius;
    m_x1B1Bits.m_fogged = 1;
    if (!DoScissoring)
        m_x1B1Bits.m_clip = 0;
    if (radius < ((PSXRenderTarget *)LpRenderTarget)->m_fogStart)
        m_x1B1Bits.m_fogged = 0;
    if (((PSXRenderableObjectData *)m_data)->m_strings) {
        if (m_x1B1Bits.m_lit && m_searchPlacement)
            LpLightManager->GetThreeLights(m_searchPlacement, position, MaxFalloffDistance, lights0, lights1, ambient);
        hierarchy = ((PSXRenderableObjectData *)m_data)->m_strings;
        numParts = hierarchy->m_count;
        for (i = 0; i < numParts; i++) {
            buffer = ((char **)m_partBuffers)[i];
            cursor = buffer + 0x40;
            COPY_WORLD_MATRIX_40(this, buffer);
            count = hierarchy->m_lengths[i];
            index = hierarchy->m_strings[i];
            *(int *)(((char **)m_partBuffers)[i] + 0x20) =
                DoViewportClipping << 4 | m_x1B1Bits.m_insideGuardBand << 3 | m_x1B1Bits.m_boxVisible << 2 | m_x1B1Bits.m_fogged << 1 | m_x1B1Bits.m_clip;
            cursor += 0x50;
            cursor = CopyObjectMatrices(index, count, cursor);
            if (m_x1B1Bits.m_lit) {
                Copy4Qwords(lights0, cursor);
                cursor += 0x50;
                Copy4Qwords(lights1, cursor);
            }
        }
        if (!m_data->m_shaderChecked)
            m_data->m_shaderChecked = 1;
    } else {
        *(int *)((char *)m_partBuffers + 0x20) =
            DoViewportClipping << 4 | m_x1B1Bits.m_insideGuardBand << 3 | m_x1B1Bits.m_boxVisible << 2 | m_x1B1Bits.m_fogged << 1 | m_x1B1Bits.m_clip;
        QWORD *packet = (QWORD *)m_partBuffers + 4;
        COPY_WORLD_MATRIX_40(this, m_partBuffers);
        if (m_x1B1Bits.m_lit && m_searchPlacement)
            LpLightManager->GetThreeLights(m_searchPlacement, position, MaxFalloffDistance, packet + 5, packet + 10, ambient);
        if (!m_data->m_shaderChecked) {
            m_data->m_shaderChecked = 1;
            material = m_data->m_lods[0].m_geometries->m_materialIndex;
            for (i = 0; i < m_data->m_numLODs; i++) {
                RenderableObjectLOD *l = &m_data->m_lods[i];
                if (l->m_numGeometries != 1 || l->m_geometries->m_materialIndex != material)
                    break;
            }
            // Retail compares with one more than the LOD count, so this never
            // happens.
            if (i == m_data->m_numLODs + 1) {
                SurfaceMaterial *mat = (SurfaceMaterial *)LpMaterialStringTable->GetPointer(material);
                if (mat->m_numShaders == 1)
                    m_data->m_shader = LpSurfaceShaderList->m_shaders[mat->m_instances[0]];
            }
        }
    }
    if (!DrawAnimatedFlag && ((PSXRenderableObjectData *)m_data)->m_strings)
        return GameObject::Render3D();
    lod = &m_data->m_lods[m_lod];
    LpMaterialStringTable->GetPointer(lod->m_geometries->m_materialIndex);
    i = 0;
    if (m_data->m_shader) {
        ((PSXSurfaceShaderInstance *)m_data->m_shader)->AddGeometry(lod->m_geometries, (_sceDmaTag *)m_partBuffers, 0);
    } else {
        for (i = 0; i < lod->m_numGeometries; i++) {
            if (numParts) {
                multipass = 0;
                if (m_distance > MultipassCullDistance)
                    multipass = 1;
                PSXSurfaceGeometry *g = lod->m_geometries;
                _sceDmaTag *tag = ((_sceDmaTag **)m_partBuffers)[g[i].m_xA];
                g[i].AddToRenderQueue(tag, 0, multipass);
            } else {
                lod->m_geometries[i].AddToRenderQueue((_sceDmaTag *)m_partBuffers, 0, 0);
            }
        }
    }
    return GameObject::Render3D();
}


// ---- src/sinit/PSXRenderableObject.cpp
// Static initializer of PSXRenderableObject.cpp: only the axis vectors from the shared header.

