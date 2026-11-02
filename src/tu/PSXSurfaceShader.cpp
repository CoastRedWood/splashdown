// PSXSurfaceShader.cpp  @ 0x00347ED0 (text 0x347ED0-0x349DD0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's PSXSurfaceShader.cpp as one translation unit, inside
// PSXSurfaceMaterial.cpp's tu_map row: PSXSurfaceShaderInstance,
// PSXSurfaceShaderList and the Build*RenderItem packet builders, then the
// static initializer (the axis vectors). Its .rodata (0x436520-0x43675C) is
// Render's strings, then the PSXSurfaceShaderList and
// PSXSurfaceShaderInstance RTTI. The preamble and PRMODE restore items
// derive from VifDataItem. PSX_RENDER_TARGET_GLOBAL declares LpRenderTarget
// as the PSXRenderTarget it is: a cast of RenderTarget.h's declaration moves
// PSXSurfaceShaderInstance::Render's loads.



// inline_depth is TU-wide: without 8, BuildGenericRenderItem's _D3DMATRIX
// copy is an out-of-line operator=.
#pragma inline_depth(8)
#pragma fast_fptosi on

#define PSX_RENDER_TARGET_GLOBAL
#include "d3dtypes.h"
#include "ParameterBlock/ParameterBlock.h"
#include "PSXShaderDescriptor/PSXShaderDescriptor.h"
#include "MaterialAnimationData/MaterialAnimationData.h"
#include "TextureMapStringTable/TextureMapStringTable.h"
#include "soultree_assert_block.h"
#include "QWORD/QWORD.h"
#include "StringTable/StringTable.h"
#include "sceGifTag/sceGifTag.h"
#include "PSXVideoCard/PSXVideoCard.h"
#include "SurfaceMaterial/SurfaceMaterial.h"
#include "PSXSurfaceMaterial/PSXSurfaceMaterial.h"
#include "TextureMap/TextureMap.h"
#include "ShaderDescriptor/ShaderDescriptor.h"
#include "PSXSurfaceShaderInstance/PSXSurfaceShaderInstance.h"
#include "SurfaceShaderInstance/SurfaceShaderInstance.h"
#include "PSXTextureMapManager/PSXTextureMapManager.h"
#include "PSXSurfaceGeometry/PSXSurfaceGeometry.h"
#include "PSXSurfaceShaderList/PSXSurfaceShaderList.h"
#include "SurfaceShaderList/SurfaceShaderList.h"
#include "DmaRenderItem/DmaRenderItem.h"
#include "_sceDmaTag/_sceDmaTag.h"
#include "DataAccess/DataAccess.h"
#include "SurfaceGeometry/SurfaceGeometry.h"
#include "MaterialStringTable/MaterialStringTable.h"
#include "PSXTextureMap/PSXTextureMap.h"
#include "Game/Game.h"
#include "Water/Water.h"
#include "PSXWater/PSXWater.h"
#include "PSXRenderTarget/PSXRenderTarget.h"
#include "PSXCamera/PSXCamera.h"
#include "LightManager/LightManager.h"
#include "VifDataItem/VifDataItem.h"
#include "sinit/AxisVectors.h"


extern TextureMapStringTable *const LpTextureMapStringTable;

// ---- src/PSXSurfaceShaderInstance/ctor.cpp
PSXSurfaceShaderInstance::PSXSurfaceShaderInstance()
{
    m_x1C = 0;
}

// ---- src/PSXSurfaceShaderInstance/dtor.cpp


PSXSurfaceShaderInstance::~PSXSurfaceShaderInstance()
{
}


// ---- src/PSXSurfaceShaderInstance/Render.cpp
extern PSXSurfaceShaderList *LpSurfaceShaderList;
void PSXprintf(const char *fmt, ...);
extern StringTable *LpMicroProgramStringTable;
extern PSXTextureMapManager *LpTextureMapManager;
extern unsigned int StripPreambleCode[];
extern unsigned int ScissorCoreCode[];
static int DebugDump;

// Defined here for its retail .bss slot.
int Wireframe;
int NoKick;
int NopLoopFlag;
int TotalSurfaceGeometrySizeThisFrame;
int TotalTransformRenderSizeThisFrame;
int TotalRenderStatesSizeThisFrame;
int TotalTextureTransferSizeThisFrame;


void PSXSurfaceShaderInstance::Render()
{
    static int useTextures = 1;
    static int NoKickIndex = LpMicroProgramStringTable->GetIndex("StripOptFogCodeNoKick");
    static int NopLoopIndex = LpMicroProgramStringTable->GetIndex("NopLoop");
    static int WireframeIndex = LpMicroProgramStringTable->GetIndex("StripWireframe");
    static int BackfaceCullIndex = LpMicroProgramStringTable->GetIndex("StripFogCullCode");
    static int DiffuseCode = LpMicroProgramStringTable->GetIndex("Diffuse");
    static int NoFogCode = LpMicroProgramStringTable->GetIndex("StripOptCode");
    static int Eco2DCode = LpMicroProgramStringTable->GetIndex("EcoSystemCode2D");
    PSXShaderDescriptor *desc = (PSXShaderDescriptor *)m_descriptor;
    _sceDmaTag *address;
    int i;
    if (desc) {
        if (desc->m_enabled) {
            address = LpRenderTarget->AddMPGToRenderQueue((unsigned int *)LpMicroProgramStringTable->GetPointer(desc->m_microProgram));
            if (DebugDump && address)
                PSXprintf("\n   (0x%lx,         ) send MPG %s\n", address, LpMicroProgramStringTable->GetString(desc->m_microProgram));
            if (desc->m_numTextures && useTextures) {
                for (i = 0; i < desc->m_numTextures; i++) {
                    if (desc->m_textures[i] != -1) {
                        PSXTextureMap *map = (PSXTextureMap *)LpTextureMapStringTable->GetPointer(desc->m_textures[i]);
                        if (map) {
                            PSXTextureMap *texture = (PSXTextureMap *)map->GetData(desc->m_animation ? desc->m_animation->m_frame : 0.0f);
                            LpTextureMapManager->AddTextureToGifList(texture);
                            LpTextureMapManager->AddTextureInterruptToRenderQueue();
                            int textureAddress = desc->SetUpTextureMapAddress(texture);
                            if (DebugDump)
                                PSXprintf("%5d bytes texture sent to 0x%lx %s\n", textureAddress, texture->m_xEC,
                                          LpTextureMapStringTable->GetString(desc->m_textures[0]));  // (sic: always texture 0)
                        }
                    }
                }
            } else if (desc->m_numTextures == 0 && desc->m_fillTexture) {
                LpTextureMapManager->AddFillTextureToGifList(desc->m_fillColor);
                LpTextureMapManager->AddTextureInterruptToRenderQueue();
            }
            address = LpRenderTarget->AddToRenderQueue(desc->m_packetTag, 1, "");
            if (DebugDump)
                PSXprintf("   send (0x%lx, 0x%lx) LpRenderStatesRenderItem 0x%lx\n", address, desc->m_packetTag + 1,
                          desc->m_packetTag);
            for (SurfaceGeometryNode *node = m_head; node; node = node->next) {
                PSXSurfaceGeometry *geometry = (PSXSurfaceGeometry *)node->geometry;
                if (node->end) {
                    address = LpRenderTarget->AddToRenderQueue(node->end, 1, "");
                    if (DebugDump)
                        PSXprintf("   send (0x%lx, 0x%lx) LpTransformRenderItem 0x%lx\n", address, node->end + 1, node->end);
                }
                address = LpRenderTarget->AddToRenderQueue(node->start, 1, "");
                if (DebugDump)
                    PSXprintf("   send (0x%lx, 0x%lx) LpTransformRenderItem 0x%lx\n", address, node->start + 1,
                              node->start);
                address = LpRenderTarget->AddToRenderQueue(geometry->m_packet, 1, "");
                if (DebugDump)
                    PSXprintf("   send (0x%lx, 0x%lx) Surface data = 0x%lx\n", address, (unsigned int *)geometry->m_packet + 1, geometry->m_packet);
            }
            if (m_bits.m_flush)
                LpRenderTarget->TrySendRenderQueue(0);
            LpRenderTarget->AddMPGToRenderQueue(StripPreambleCode);
            LpRenderTarget->AddMPGToRenderQueue(ScissorCoreCode);
            static int RESTORE_LIGHTS = 1;
            if (RESTORE_LIGHTS)
                LpRenderTarget->AddToRenderQueue((_sceDmaTag *)LpSurfaceShaderList->m_preamble, 1, "");
        }
    } else if (m_list) {
        m_list->Render();
    }
}


// ---- src/PSXSurfaceShaderList/ctor.cpp
PSXSurfaceShaderList::PSXSurfaceShaderList()
{
    m_preamble = 0;
    m_restorePRMODE = 0;
}


// ---- src/PSXSurfaceShaderList/dtor.cpp
extern "C" void free(void *p);



PSXSurfaceShaderList::~PSXSurfaceShaderList()
{
    if (m_preamble)
        free(m_preamble);
    if (m_restorePRMODE)
        free(m_restorePRMODE);
}


// ---- src/PSXSurfaceShaderList/AddShaderInstance.cpp
class ShaderDescriptor;



int PSXSurfaceShaderList::AddShaderInstance(ShaderDescriptor *desc, int flags)
{
    return SurfaceShaderList::AddShaderInstance(desc, flags);
}


// ---- src/PSXSurfaceShaderList/Render.cpp
extern int TotalSurfaceGeometrySizeThisFrame;
extern int TotalTransformRenderSizeThisFrame;
extern int TotalRenderStatesSizeThisFrame;
extern int TotalTextureTransferSizeThisFrame;

VifDataItem *BuildPreambleRenderItem(VifDataItem *item, PSXCamera *camera);
VifDataItem *BuildRestorePRMODERenderItem(VifDataItem *item);

void PSXSurfaceShaderList::Render()
{
    DebugDump = 0;
    static int timerID = -1;    // unused
    if (LpSurfaceShaderList == this) {
        LpRenderTarget->AddMPGToRenderQueue(StripPreambleCode);
        m_preamble = BuildPreambleRenderItem(m_preamble, (PSXCamera *)LpRenderTarget->m_camera);
    }
    if (m_preamble)
        LpRenderTarget->AddToRenderQueue((_sceDmaTag *)m_preamble, 1, "");
    TotalSurfaceGeometrySizeThisFrame = 0;
    TotalTransformRenderSizeThisFrame = 0;
    TotalRenderStatesSizeThisFrame = 0;
    TotalTextureTransferSizeThisFrame = 0;
    SurfaceShaderList::Render();
    if (LpSurfaceShaderList == this)
        m_restorePRMODE = BuildRestorePRMODERenderItem(m_restorePRMODE);
    if (m_restorePRMODE)
        LpRenderTarget->AddToRenderQueue((_sceDmaTag *)m_restorePRMODE, 1, "");
}


// ---- src/PSXSurfaceShaderInstance/AddGeometry.cpp
#ifdef SPLASHDOWN_PC
extern "C" void port_dma_unaligned(const void *addr, const char *where);
#endif
void PSXSurfaceShaderInstance::AddGeometry(SurfaceGeometry *geometry, _sceDmaTag *start, _sceDmaTag *end)
{
#ifdef SPLASHDOWN_PC
    if (((unsigned long)start | (unsigned long)end) & 0xF)
        port_dma_unaligned(start, (const char *)__builtin_return_address(0));
#endif
    SurfaceGeometryNode *node = SurfaceShaderInstance::AddGeometry(geometry);
    node->start = start;
    node->end = end;
}


// ---- src/global/BuildGenericRenderItem.cpp




_D3DMATRIX IdentityMatrix();

static inline void AddMatrix(DmaRenderItem *item, _D3DMATRIX *matrix)
{
    *(_D3DMATRIX *)&item->m_dataList[item->m_dataListCurrentSize] = *matrix;
    DmaRenderItem::UnpackCount += 16;
    item->m_dataListCurrentSize += 16;
}

// The VU1 packet every generic (non-unique) object shares: the camera
// matrices, screen and fog constants, the default lights and the GIF tags.
void BuildGenericRenderItem(DmaRenderItem *item)
{
    PSXCamera *camera = (PSXCamera *)LpRenderTarget->m_camera;
    QWORD directions[4];
    QWORD colors[4];
    sceGifTag tag;
    _D3DMATRIX identity;
    float *q;

    item->DmaAddVifCode(0x11000000);    // FLUSH
    item->DmaAddVifCode(0x03000000);    // BASE
    item->DmaAddVifCode(0x02000000);    // OFFSET
    item->DmaBeginUnpack(4, 12);
    AddMatrix(item, &camera->m_guardBandProjection);
    item->DmaEndUnpack();
    item->DmaBeginUnpack(8, 12);
    AddMatrix(item, &camera->m_projection);
    item->DmaEndUnpack();
    item->DmaBeginUnpack(12, 12);
    AddMatrix(item, &camera->m_x140);
    item->DmaEndUnpack();
    item->DmaBeginUnpack(16, 12);
    AddMatrix(item, &camera->m_view);
    item->DmaEndUnpack();
    item->DmaBeginUnpack(0x2B, 12);
    q = (float *)&item->m_dataList[item->m_dataListCurrentSize];
    q[0] = camera->m_x140._11;
    q[1] = camera->m_x140._22;
    q[2] = camera->m_x140._33;
    q[3] = 2.0f * camera->m_x140._11 / 4096.0f;
    DmaRenderItem::UnpackCount += 4;
    item->m_dataListCurrentSize += 4;
    item->DmaEndUnpack();
    item->DmaBeginUnpack(0x2C, 12);
    q = (float *)&item->m_dataList[item->m_dataListCurrentSize];
    q[0] = camera->m_x140._41;
    q[1] = camera->m_x140._42;
    q[2] = camera->m_x140._43;
    q[3] = 2.0f * camera->m_x140._22 / 4096.0f;
    DmaRenderItem::UnpackCount += 4;
    item->m_dataListCurrentSize += 4;
    item->DmaEndUnpack();
    item->DmaBeginUnpack(0x2E, 12);
    q = (float *)&item->m_dataList[item->m_dataListCurrentSize];
    q[0] = 1.0f / camera->m_guardBandProjection._11;
    q[1] = 1.0f / camera->m_guardBandProjection._22;
    q[3] = q[2] = 1.0f;
    DmaRenderItem::UnpackCount += 4;
    item->m_dataListCurrentSize += 4;
    item->DmaEndUnpack();
    if (LpLightManager) {
        LpLightManager->GetDefaultLights(directions, colors, 0);
        item->DmaBeginUnpack(0x14, 12);
        item->DmaAddQwordToUnpack(&directions[0]);
        item->DmaAddQwordToUnpack(&directions[1]);
        item->DmaAddQwordToUnpack(&directions[2]);
        item->DmaAddQwordToUnpack(&directions[3]);
        item->DmaEndUnpack();
        item->DmaBeginUnpack(0x18, 12);
        item->DmaAddQwordToUnpack(&colors[0]);
        item->DmaAddQwordToUnpack(&colors[1]);
        item->DmaAddQwordToUnpack(&colors[2]);
        item->DmaAddQwordToUnpack(&colors[3]);
        item->DmaEndUnpack();
    }
    identity = IdentityMatrix();
    item->DmaBeginUnpack(0x1C, 12);
    AddMatrix(item, &identity);
    item->DmaEndUnpack();
    item->DmaBeginUnpack(0x20, 12);
    q = (float *)&item->m_dataList[item->m_dataListCurrentSize];
    q[0] = camera->m_position.x;
    q[1] = camera->m_position.y;
    q[2] = camera->m_position.z;
    q[3] = camera->m_position.w;
    DmaRenderItem::UnpackCount += 4;
    item->m_dataListCurrentSize += 4;
    item->DmaEndUnpack();
    item->DmaBeginUnpack(0x21, 12);
    q = (float *)&item->m_dataList[item->m_dataListCurrentSize];
    q[0] = 2.0e-5f;
    q[1] = -16.0f;
    q[2] = LpRenderTarget->m_fogEnd;
    q[3] = LpRenderTarget->m_fogScale;
    DmaRenderItem::UnpackCount += 4;
    item->m_dataListCurrentSize += 4;
    item->DmaEndUnpack();

    *(int128_t *)&tag = 0;
    tag.EOP = 1;
    tag.PRE = 1;
    tag.PRIM = 5;
    tag.FLG = 0;
    tag.NREG = 3;
    tag.REGS0 = 2;
    tag.REGS1 = 1;
    tag.REGS2 = 4;
    tag.NLOOP = 5;                      // set last, unlike the other tags
    item->DmaBeginUnpack(0x22, 12);
    item->DmaAddGifTagToUnpack(&tag);
    item->DmaEndUnpack();

    *(int128_t *)&tag = 0;
    tag.NLOOP = 0;
    tag.EOP = 1;
    tag.PRE = 1;
    tag.PRIM = 0x19;
    tag.FLG = 0;
    tag.NREG = 3;
    tag.REGS0 = 2;
    tag.REGS1 = 1;
    tag.REGS2 = 4;
    item->DmaBeginUnpack(0x2D, 12);
    item->DmaAddGifTagToUnpack(&tag);
    item->DmaEndUnpack();

    *(int128_t *)&tag = 0;
    tag.NLOOP = 1;
    tag.EOP = 1;
    tag.PRE = 1;
    tag.FLG = 0;
    tag.NREG = 1;
    tag.REGS0 = 0xF;
    item->DmaBeginUnpack(0x23, 12);
    item->DmaAddGifTagToUnpack(&tag);
    q = (float *)&item->m_dataList[item->m_dataListCurrentSize];
    ((unsigned int *)q)[0] = 0;
    ((unsigned int *)q)[1] = 0;
    ((unsigned int *)q)[2] = 0;
    ((unsigned int *)q)[3] = 0x7F;
    DmaRenderItem::UnpackCount += 4;
    item->m_dataListCurrentSize += 4;
    item->DmaEndUnpack();

    *(int128_t *)&tag = 0;
    tag.NLOOP = 1;
    tag.EOP = 1;
    tag.PRE = 1;
    tag.FLG = 0;
    tag.NREG = 1;
    tag.REGS0 = 0xF;
    item->DmaBeginUnpack(0x25, 12);
    item->DmaAddGifTagToUnpack(&tag);
    q = (float *)&item->m_dataList[item->m_dataListCurrentSize];
    ((unsigned int *)q)[0] = 0;
    ((unsigned int *)q)[1] = 0;
    ((unsigned int *)q)[2] = 0;
    ((unsigned int *)q)[3] = 0x7F;
    DmaRenderItem::UnpackCount += 4;
    item->m_dataListCurrentSize += 4;
    item->DmaEndUnpack();

    item->DmaBeginUnpack(0x27, 12);
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
    item->DmaEndUnpack();
    item->DmaEndUnpack();               // twice, as retail
}


// ---- src/global/BuildUniqueRenderItem.cpp


// The unique (per-object) packet: the object's matrix and a flag quadword.
void BuildUniqueRenderItem(DmaRenderItem *item, _D3DMATRIX *matrix, unsigned int flag)
{
    item->DmaAddVifCode(0x11000000);    // FLUSH
    item->DmaAddVifCode(0x03000000);    // BASE
    item->DmaAddVifCode(0x02000000);    // OFFSET
    item->DmaBeginUnpack(0, 12);
    *(_D3DMATRIX *)&item->m_dataList[item->m_dataListCurrentSize] = *matrix;
    DmaRenderItem::UnpackCount += 16;
    item->m_dataListCurrentSize += 16;
    item->DmaEndUnpack();
    item->DmaBeginUnpack(0x29, 12);
    unsigned int *q = &item->m_dataList[item->m_dataListCurrentSize];
    q[0] = flag ? 1 : 0;
    q[1] = 0;
    q[2] = 0;
    q[3] = 0;
    DmaRenderItem::UnpackCount += 4;
    item->m_dataListCurrentSize += 4;
    item->DmaEndUnpack();
    item->DmaAddVifCodeMSCAL();
    item->DmaAddVifCode(0x10000000);    // FLUSHE
    item->DmaAddVifCode(0x030000B5);    // BASE
    item->DmaAddVifCode(0x02000190);    // OFFSET
}


// ---- src/global/BuildPreambleRenderItem.cpp
extern "C" void *memalign(unsigned int alignment, unsigned int size);
extern "C" void *memset(void *dst, int c, unsigned int n);



_D3DMATRIX IdentityMatrix();

// The preamble item: its VIF packet follows the head (0x3B0 bytes in all).
struct PreambleDataItem : VifDataItem {
    unsigned int m_code[4];             // +0x10
};

union Qword {
    unsigned int w[4];
    float f[4];
};

// 64-byte copy through $at, $v0, $v1, $a0 (hand-written).
#define COPY_MATRIX(dst, src)                                                                                 \
    asm("lq $1, 0x0(%1); lq $2, 0x10(%1); lq $3, 0x20(%1); lq $4, 0x30(%1); sq $1, 0x0(%0); sq $2, 0x10(%0); sq $3, 0x20(%0); sq $4, 0x30(%0)" \
        : : "r"(dst), "r"(src) : "$1", "$2", "$3", "$4", "memory")

#define GIF(q) ((sceGifTag *)(q))

// Builds (or refills) the per-frame VU1 preamble packet: the camera matrices,
// screen and fog constants, the default lights and the GIF tags the
// microcode uses, each behind its UNPACK VIF code.
VifDataItem *BuildPreambleRenderItem(VifDataItem *base, PSXCamera *camera)
{
    PreambleDataItem *item = (PreambleDataItem *)base;
    Qword *p;
    if (!item) {
        unsigned int size = 0x3B0;
        item = (PreambleDataItem *)memalign(0x40, size);
        memset(item, 0, size);
        item->m_numQwords = (size - 0x10) >> 4;
        item->m_02 = 0;
        item->m_03 = 0x30;
        item->m_data = item->m_code;
        item->m_08 = 0;
        item->m_0C = 0;
    }
    item->m_code[0] = 0x11000000;       // FLUSH
    item->m_code[1] = 0x03000000;       // BASE
    item->m_code[2] = 0x02000000;       // OFFSET
    p = (Qword *)item->m_code;
    p->w[3] = 0x6C048004;
    p++;
    COPY_MATRIX(p, &camera->m_guardBandProjection);
    p += 4;
    p->w[3] = 0x6C048008;
    p++;
    COPY_MATRIX(p, &camera->m_projection);
    p += 4;
    p->w[3] = 0x6C04800C;
    p++;
    COPY_MATRIX(p, &camera->m_x140);
    p += 4;
    p->w[3] = 0x6C048010;
    p++;
    COPY_MATRIX(p, &camera->m_view);
    p += 4;
    p->w[3] = 0x6C01802B;
    p++;
    p->f[0] = camera->m_x140._11;
    p->f[1] = camera->m_x140._22;
    p->f[2] = camera->m_x140._33;
    p->f[3] = 2.0f * camera->m_x140._11 / 4096.0f;
    p++;
    p->w[3] = 0x6C01802C;
    p++;
    p->f[0] = camera->m_x140._41;
    p->f[1] = camera->m_x140._42;
    p->f[2] = camera->m_x140._43;
    p->f[3] = 2.0f * camera->m_x140._22 / 4096.0f;
    p++;
    _D3DMATRIX identity;
    identity = IdentityMatrix();
    p->w[3] = 0x6C04801C;
    p++;
    COPY_MATRIX(p, &identity);
    p += 4;
    p->w[3] = 0x6C01802E;
    p++;
    p->f[0] = 1.0f / camera->m_guardBandProjection._11;
    p->f[1] = 1.0f / camera->m_guardBandProjection._22;
    p->f[3] = p->f[2] = 1.0f;
    p++;
    if (LpLightManager) {
        p->w[3] = 0x6C048014;
        p++;
        LpLightManager->GetDefaultLights((QWORD *)p, (QWORD *)(p + 5), 0);
        p += 4;
        p->w[3] = 0x6C048018;
        p += 5;
    }
    p->w[3] = 0x6C018020;
    p++;
    p->f[0] = camera->m_position.x;
    p->f[1] = camera->m_position.y;
    p->f[2] = camera->m_position.z;
    p->f[3] = camera->m_position.w;
    p++;
    p->w[3] = 0x6C018021;
    p++;
    p->f[0] = 2.0e-5f;
    p->f[1] = -16.0f;
    p->f[2] = LpRenderTarget->m_fogEnd;
    p->f[3] = LpRenderTarget->m_fogScale;
    p++;
    p->w[3] = 0x6C018022;
    p++;
    *(int128_t *)p = 0;
    GIF(p)->NLOOP = 5;
    GIF(p)->EOP = 1;
    GIF(p)->PRE = 1;
    GIF(p)->PRIM = 5;
    GIF(p)->FLG = 0;
    GIF(p)->NREG = 3;
    GIF(p)->REGS0 = 2;
    GIF(p)->REGS1 = 1;
    GIF(p)->REGS2 = 4;
    p++;
    p->w[3] = 0x6C01802D;
    p++;
    *(int128_t *)p = 0;
    GIF(p)->NLOOP = 0;
    GIF(p)->EOP = 1;
    GIF(p)->PRE = 1;
    GIF(p)->PRIM = 1;
    GIF(p)->FLG = 0;
    GIF(p)->NREG = 3;
    GIF(p)->REGS0 = 2;
    GIF(p)->REGS1 = 1;
    GIF(p)->REGS2 = 4;
    p++;
    p->w[3] = 0x6C028023;
    p++;
    *(int128_t *)p = 0;
    GIF(p)->NLOOP = 1;
    GIF(p)->EOP = 1;
    GIF(p)->PRE = 1;
    GIF(p)->FLG = 0;
    GIF(p)->NREG = 1;
    GIF(p)->REGS0 = 0xF;
    p++;
    p->w[0] = 0;
    p->w[1] = 0;
    p->w[2] = 0;
    p->w[3] = 0x7F;
    p++;
    p->w[3] = 0x6C028025;
    p++;
    *(int128_t *)p = 0;
    GIF(p)->NLOOP = 1;
    GIF(p)->EOP = 1;
    GIF(p)->PRE = 1;
    GIF(p)->FLG = 0;
    GIF(p)->NREG = 1;
    GIF(p)->REGS0 = 0xF;
    p++;
    p->w[0] = 0;
    p->w[1] = 0;
    p->w[2] = 0;
    p->w[3] = 0x7F;
    p++;
    p->w[3] = 0x6C028027;
    p++;
    p->w[0] = 0;
    p->w[1] = 0;
    p->w[2] = 0;
    p->w[3] = 0;
    p++;
    p->w[0] = 0;
    p->w[1] = 0;
    p->w[2] = 0;
    p->w[3] = 0;
    return item;
}


// ---- src/global/BuildRestorePRMODERenderItem.cpp
extern "C" void *memalign(unsigned int alignment, unsigned int size);
extern "C" void *memset(void *dst, int c, unsigned int n);



// The PRMODE restore item, packet included (0x50 bytes).
struct RestorePRMODEDataItem : VifDataItem {
    unsigned int m_vif[4];      // +0x10
    sceGifTag m_gifTag;         // +0x20
    long m_prmodecont[2];       // +0x30 (A+D data, register)
    unsigned int m_vifEnd[4];   // +0x40
};

// A VIF packet that unpacks a GIFtag plus an A+D write of PRMODECONT = 1
// into VU1 memory.
VifDataItem *BuildRestorePRMODERenderItem(VifDataItem *base)
{
    RestorePRMODEDataItem *item = (RestorePRMODEDataItem *)base;
    if (!item) {
        item = (RestorePRMODEDataItem *)memalign(0x40, sizeof(RestorePRMODEDataItem));
        memset(item, 0, sizeof(RestorePRMODEDataItem));
        item->m_numQwords = 4;
        item->m_02 = 0;
        item->m_03 = 0x30;
        item->m_data = &item->m_vif;
        item->m_08 = 0;
        item->m_0C = 0;
    }
    item->m_vif[0] = 0x11000000;    // FLUSH
    item->m_vif[1] = 0x03000000;    // BASE
    item->m_vif[2] = 0x02000000;    // OFFSET
    item->m_vif[3] = 0x6C028320;    // UNPACK
    *(int128_t *)&item->m_gifTag = 0;
    item->m_gifTag.FLG = 0;
    item->m_gifTag.EOP = 1;
    item->m_gifTag.NREG = 1;
    item->m_gifTag.NLOOP = 1;
    item->m_gifTag.REGS0 = 0xE;     // A+D
    item->m_prmodecont[0] = 1;
    item->m_prmodecont[1] = 0x1A;   // PRMODECONT
    item->m_vifEnd[0] = 0x14000000;  // MSCAL
    item->m_vifEnd[1] = 0x10000000;  // FLUSHE
    item->m_vifEnd[2] = 0x03000000;
    item->m_vifEnd[3] = 0x02000000;
    return item;
}


