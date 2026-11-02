// Terrain.cpp  @ 0x00297900 (text 0x297900-0x29AB40)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's Terrain.cpp as one translation unit: Terrain's methods,
// PatchCacheItem's ctor, then the static initializer. `make link` places the
// object's data as well as its code (tools/link_c.py). The rest of the tu_map
// row is TUs without an initializer, left per-function: TextureMap.cpp
// (TextureMap, BaseTextureMap; BaseObject RTTI restart at 0x41ACB8),
// TextureMapManager.cpp (restart at 0x41AD58), Texutil.cpp (from BytesPerPixel
// at 0x29B0A0; its jump tables precede "Texutil.cpp"), the TGA functions and
// TokenString. Create's and the other local Terrain views are on Terrain.h;
// the scissor and common render items derive from VifDataItem; CalculateKappa
// and PatchCacheItem's ctor are C++ again. DrawableGridNodeSharedTextures
// comes from its header.




// inline_depth and fast_fptosi are TU-wide (GetGridPointIntersection's).
#pragma inline_depth(6)
#pragma fast_fptosi on

#include "d3dtypes.h"
#include "GameObject/GameObject.h"
#include "Terrain/Terrain.h"
#include "soultree_assert_block.h"
#include "DataAccess/DataAccess.h"
#include "ColorMapper/ColorMapper.h"
#include "PSXTextureMap/PSXTextureMap.h"
#include "GridNodeGeometryLOD/GridNodeGeometryLOD.h"
#include "GridNodeGeometryPlain/GridNodeGeometryPlain.h"
#include "DrawableGridNodeSharedTextures/DrawableGridNodeSharedTextures.h"
#include "DrawableGridNode/DrawableGridNode.h"
#include "DebugOverlay/DebugOverlay.h"
#include "Game/Game.h"
#include "sceGifTag/sceGifTag.h"
#include "Camera/Camera.h"
#include "RenderTarget/RenderTarget.h"
#include "AgeManager/AgeManager.h"
#include "GridNodeGeometryBase/GridNodeGeometryBase.h"
#include "ImageData/ImageData.h"
#include "PSXCamera/PSXCamera.h"
#include "PSXRenderTarget/PSXRenderTarget.h"
#include "TextureMap/TextureMap.h"
#include "PeakInt/PeakInt.h"
#include "TerrainQuality/TerrainQuality.h"
#include "PatchCacheItem/PatchCacheItem.h"
#include "VifDataItem/VifDataItem.h"
#include "QWORD/QWORD.h"
#include "ControlInterface/ControlTypes.h"
#include "sinit/AxisVectors.h"


// Defined here for its retail .bss slot.
int TotalBoxVisibility;
int TotalSphereVisibility;
int HighWaterMark;
int WaterMark;

// The static initializer's objects, after the axis vectors (__sinit_Terrain.cpp
// constructs them in this order): the cache size peak here and the patch
// cache before GetGridPointIntersection (retail's .bss order).
static PeakInt PeakCacheSize(5000);

// Defined here for its retail .bss slot.
static int CacheSize;
Terrain *GameTerrain;


// ---- src/Terrain/ctor.cpp
extern "C" void *memset(void *dst, int c, unsigned int n);




_D3DMATRIX IdentityMatrix();





Terrain::Terrain(int enabled) : GameObject(enabled)
{
    m_gridScale = 1.0f;
    m_file = 0;
    m_checkDeltas = 0;
    m_xE4 = 0;
    m_evaluations = 0;
    m_xF0 = 0;
    m_xF4 = 0;
    m_vertexNotifications = 0;
    m_parentNotifications = 0;
    m_x100 = 0;
    m_x104 = 0;
    m_x108 = 0;
    m_vertexEvaluations = 0;
    m_frameCount = 0;
    m_numTextures = 0;
    m_x598 = 0;
    m_root = 0;
    m_renderTolerance = 1000;
    m_dirty = 0;
    m_xAC = 0;
    m_xC54 = 0;
    m_xE8 = 0;
    m_xC8 = 0;
    m_xCC = 0;
    m_threshold = 0;
    m_xD0 = 0;
    m_xD8 = 0;
    m_xDC = 0;
    m_numGeometries = 0;
    m_geometries = 0;
    m_xC60 = 1.0f;
    m_xC64 = 0;
    m_detailTexture = 0;
    m_xC70 = IdentityMatrix();
    m_xE4C = 0;
    m_xE50 = 0;
    m_xE54 = 0;
    m_xCD8 = 0;
    m_xCDC = 0;
    m_xCE0 = 0;
    m_meshesBuilt = 0;
    m_visible = 0;
    m_numVisibleNodes = 0;
    m_debugPage = -1;
    memset(m_filename, 0, sizeof(m_filename));
    m_xCE4 = 0;
    m_xDFC = 0;
    m_halfRes = 0;
    m_xE04 = 0;
    m_xE58 = 0;
    m_xE5C = 0;
    m_ageManager = 0;
    m_meshAgeManager = 0;
}


// Retail's .rodata opens with "Terrain.cpp" and "\n" (@857, @859; @858
// never written) before Create's literals (@1170-): a function with an
// ASSERT came here that nothing called, so retail's linker stripped it with
// its message and statics. link_c
// leaves its sections out.
#line 400 "Terrain.cpp"
static void StrippedAssert(int ok)
{
    ASSERT(ok, "stripped");
}


// ---- src/Terrain/Create.cpp
// Four dead ints read in an if (0) keep retail's unused 16 bytes at the top of the frame.

extern "C" char *strcpy(char *dst, const char *src);
extern "C" char *strcat(char *dst, const char *src);
extern "C" int strcmp(const char *a, const char *b);
extern "C" int strcasecmp(const char *a, const char *b);
extern "C" void *malloc(unsigned int size);
extern "C" void *calloc(unsigned int n, unsigned int size);



extern DataAccess *LpDataAccess;
extern Game *LpGame;
extern Terrain *GameTerrain;
extern char TerrainTextureContentDirectory[0x104];
extern int MAX_MIP_LEVELS;
extern int MIP_TOP_SIZE;

class Palette8;
BaseObject *GetTextureMapFromResourceName(char *name, int format, Palette8 *palette, unsigned int flags,
                                          _D3DBLEND srcBlend, _D3DBLEND destBlend, int colorKeyMode,
                                          unsigned int colorKey, int addRef);
void SPLIT_PATH(const char *path, char *drive, char *dir, char *fname, char *ext);
void ReviseIpuImageDataFormat(ImageDataFormat &format);

#define FLT_MAX 3.40282347e+38f

static inline int GetDebugPage(DebugOverlay *overlay) { return overlay->m_numPages++; }

// Loads a .trn/.tri terrain: header, geometries, textures and the node tree,
// then the detail texture.

int Terrain::Create(char *name, int a, int b, char *textureFolder, int c, int detailX, int detailZ)
{
    char texturePath[0x104];
    int hasColorMapper;
    int width;
    int height;
    ImageDataFormat format;
    int gridSize;
    int x21C;
    int numGeometryRecords;
    char ext[4];
    char magic[4];
    int version;
    int x230;
    int numTextures;
    int x238;
    int x23C;
    int i;
    unsigned int flags;
    int dbg0;
    int dbg1;
    int dbg2;
    int dbg3;

    GameObject::Create();
    flags = c ? 8 : 0;
    strcpy(m_filename, name);
    m_halfRes = detailX;
    m_xE04 = detailZ;
    SPLIT_PATH(name, 0, 0, 0, ext);
    m_xCE4 = strcmp(ext, ".tri") == 0 ? 1 : 0;
    LpTerrainQuality = (TerrainQuality *)TerrainQualityHardware;
    SetGraphicQuality(b);
    m_file = LpDataAccess->FOpen(name, "rb");
    if (m_file == -1) {
        PSXprintf("Terrain::Create() fail\n");
        Release();
        return 0;
    }
    m_xE4C = (BaseObject **)calloc(200, PS2_PTR_SIZE);
    LpDataAccess->FRead(m_file, magic, 4, 1);
    LpDataAccess->FRead(m_file, &version, 4, 1);
    if (!strcmp(magic, "TRN"))
        m_isPlain = 0;
    else if (!strcmp(magic, "TR2"))
        m_isPlain = 1;
    else
        goto fail;
    if (version != 5)
        goto fail;
    if (!m_isPlain) {
        LpDataAccess->FRead(m_file, &hasColorMapper, 4, 1);
        if (hasColorMapper)
            m_xAC = new ColorMapper(m_file);
    } else {
        m_xAC = 0;
    }
    LpDataAccess->FRead(m_file, &x230, 4, 1);
    LpDataAccess->FRead(m_file, &m_gridScale, 4, 1);
    LpDataAccess->FRead(m_file, &gridSize, 4, 1);
    LpDataAccess->FRead(m_file, &x21C, 4, 1);
    LpDataAccess->FRead(m_file, &m_numGeometries, 4, 1);
    LpDataAccess->FRead(m_file, &numGeometryRecords, 4, 1);
    m_geometries = (GridNodeGeometryBase **)malloc(m_numGeometries * sizeof(GridNodeGeometryBase *));
    LpDataAccess->FSeek(m_file, m_numGeometries * PS2_SIZEOF_PTR(GridNodeGeometryBase), 1);
    LpDataAccess->FRead(m_file, &m_numTextures, 4, 1);
    LpDataAccess->FRead(m_file, &numTextures, 4, 1);
    LpDataAccess->FSeek(m_file, m_numTextures * PS2_SIZEOF_PTR(TextureMap), 1);
    for (i = 0; i < numGeometryRecords; i++) {
        if (!m_isPlain)
            m_geometries[i] = new GridNodeGeometryLOD(m_file);
        else
            m_geometries[i] = new GridNodeGeometryPlain(m_file);
    }
    if (numTextures) {
        x238 = 0;
        x23C = 0;
        for (i = 0; i < numTextures; i++) {
            if (m_xCE4) {
                LpDataAccess->FTell(m_file);
                LpDataAccess->FRead(m_file, &format, 4, 1);
                LpDataAccess->FRead(m_file, &x238, 4, 1);
                LpDataAccess->FRead(m_file, &width, 4, 1);
                LpDataAccess->FRead(m_file, &height, 4, 1);
                LpDataAccess->FRead(m_file, &x23C, 4, 1);
                LpDataAccess->FTell(m_file);
                if (height != 0x1000)
                    ReviseIpuImageDataFormat(format);
                height = width;
            } else {
                LpDataAccess->FRead(m_file, &width, 4, 1);
                LpDataAccess->FRead(m_file, &height, 4, 1);
                LpDataAccess->FRead(m_file, &format, 4, 1);
            }
            PSXTextureMap *texture = new PSXTextureMap(1);
            texture->Load(m_file, width, height, 1, format, 0, 0, (ColorMapper *)m_xAC, flags | 2, m_xB0,
                          (_D3DBLEND)2, (_D3DBLEND)1, 0x80, 0xFF00FF);
            m_textures[i] = texture;
        }
        i = width;
        MAX_MIP_LEVELS = 0;
        MIP_TOP_SIZE = i;
        while (i > 0) {
            i >>= 1;
            MAX_MIP_LEVELS++;
        }
    }
    m_x40 = _D3DVECTOR(FLT_MAX, FLT_MAX, FLT_MAX);
    m_x50 = _D3DVECTOR(-FLT_MAX, -FLT_MAX, -FLT_MAX);
    m_root = new DrawableGridNodeSharedTextures(m_file, a, 0, this, gridSize, gridSize * 4, 0, 0, 0, &m_x40, &m_x50);
    m_x50.x += 16.0f;
    m_x50.z += 16.0f;
    m_x50 *= m_gridScale;
    m_x40 *= m_gridScale;
    if (m_root) {
        int shift = m_root->m_shift;
        for (int level = 0; level < m_root->m_level; level++)
            shift -= 4;
        m_xC60 = 1.0f / ((1 << shift) * m_gridScale);
    }
    if (m_isPlain == 1)
        m_visible = (DrawableGridNode **)malloc(4000 * PS2_PTR_SIZE);
    if (LpGame->m_debugOverlay && m_debugPage < 0)
        m_debugPage = GetDebugPage(LpGame->m_debugOverlay);
    GameTerrain = this;
    LpDataAccess->FClose(m_file);
    m_restoreScissor = (_sceDmaTag *)CreateCommonDataItem();
    for (i = 0; i < 8; i++) {
        m_scissorItems[i] = CreateScissorDataItem(0);
        m_scissorItems2[i] = CreateScissorDataItem(1);
    }
    m_detailSrcBlend = (_D3DBLEND)5;
    m_detailDestBlend = (_D3DBLEND)6;
    if (!textureFolder || !strcasecmp(textureFolder, "none")) {
        m_detailTexture = 0;
    } else {
        if (textureFolder) {
#line 502 "Terrain.cpp"
            STRCPY(texturePath, TerrainTextureContentDirectory);
            strcat(texturePath, textureFolder);
        } else {
#line 506 "Terrain.cpp"
            STRCPY(texturePath, TerrainTextureContentDirectory);
            strcat(texturePath, "GrayNois.tga");
        }
        m_detailTexture = (TextureMap *)GetTextureMapFromResourceName(texturePath, 0x115C, 0, 0, m_detailSrcBlend,
                                                                     m_detailDestBlend, 0x80, 0xFF00FF, 1);
    }
    if (m_detailTexture) {
        ((PSXTextureMap *)m_detailTexture)->PlaceInVideoMemory(1, 0, 1);
        m_detailColor = m_detailTexture->m_xBC;
    } else {
        m_detailColor = -1;
    }
    unsigned int inverse = 0xFF - ((m_detailColor & 0xFF000000) >> 24);
    m_detailInverse = 0xFF000000 | inverse << 16 | inverse << 8 | inverse;
    // Each channel scaled by the texture's alpha (255 - inverse), one named
    // result per channel: that gives retail's multiply and OR order.
    unsigned int red = (0xFF - inverse) * ((m_detailColor & 0xFF0000) >> 16) / 0xFF;
    unsigned int green = (0xFF - inverse) * ((m_detailColor & 0xFF00) >> 8) / 0xFF;
    unsigned int blue = (0xFF - inverse) * (m_detailColor & 0xFF) / 0xFF;
    m_detailModulate = 0xFF000000 | red << 16 | green << 8 | blue;
    m_lodDistance = 0x100;
    if (0) {
        LpDataAccess->FRead(m_file, &dbg0, 4, 1);
        LpDataAccess->FRead(m_file, &dbg1, 4, 1);
        LpDataAccess->FRead(m_file, &dbg2, 4, 1);
        LpDataAccess->FRead(m_file, &dbg3, 4, 1);
    }
    return (int)this;

fail:
    LpDataAccess->FClose(m_file);
    m_root = 0;
    PSXprintf("trn file failed to load");
    Release();
    return 0;
}


// ---- src/Terrain/CreateScissorDataItem.cpp
extern "C" void *memalign(unsigned int alignment, unsigned int size);
extern "C" void *memset(void *dst, int c, unsigned int n);



typedef unsigned long u_long;

#define SCE_GS_SET_PRIM(prim, iip, tme, fge, abe, aa1, fst, ctxt, fix) \
    ((u_long)(prim) | ((u_long)(iip) << 3) | ((u_long)(tme) << 4) | \
    ((u_long)(fge) << 5) | ((u_long)(abe) << 6) | ((u_long)(aa1) << 7) | \
    ((u_long)(fst) << 8) | ((u_long)(ctxt) << 9) | ((u_long)(fix) << 10))

// A terrain node's packet: the scale matrix, the per-node constants and the
// strip/fan GIFtags (alpha blending per the argument).
struct ScissorPacket {
    unsigned int m_vif[4];      // +0x000
    float m_matrix[4][4];       // +0x010
    char pad050[0x150 - 0x50];
    float m_x150[3];            // +0x150
    int m_alpha;                // +0x15C
    char pad160[0x230 - 0x160];
    sceGifTag m_stripTag;       // +0x230
    sceGifTag m_tag2;           // +0x240
    int m_tag2Data[4];          // +0x250
    sceGifTag m_tag3;           // +0x260
    int m_tag3Data[4];          // +0x270
    sceGifTag m_tag4;           // +0x280
    int m_tag4Data[4];          // +0x290
    int m_x2A0[4];              // +0x2A0
    unsigned int m_vifMid[4];   // +0x2B0
    sceGifTag m_fanTag;         // +0x2C0
    unsigned int m_vifEnd[4];   // +0x2D0
};

struct ScissorDataItem : VifDataItem {
    ScissorPacket m_packet;     // +0x10
};


// Builds the per-node packet: the scale matrix and the strip and fan GIFtags,
// alpha blended when alpha is set.
VifDataItem *Terrain::CreateScissorDataItem(int alpha)
{
    ScissorDataItem *item = (ScissorDataItem *)memalign(0x40, sizeof(ScissorDataItem));
    memset(item, 0, sizeof(ScissorDataItem));
    item->m_packet.m_vif[0] = 0x11000000;   // FLUSH
    item->m_packet.m_vif[1] = 0x03000000;   // BASE
    item->m_packet.m_vif[2] = 0x02000000;   // OFFSET
    item->m_packet.m_vif[3] = 0x6C2A8000;   // UNPACK
    item->m_packet.m_matrix[0][0] = m_gridScale;
    item->m_packet.m_matrix[0][1] = 0;
    item->m_packet.m_matrix[0][2] = 0;
    item->m_packet.m_matrix[0][3] = 0;
    item->m_packet.m_matrix[1][0] = 0;
    item->m_packet.m_matrix[1][1] = m_gridScale;
    item->m_packet.m_matrix[1][2] = 0;
    item->m_packet.m_matrix[1][3] = 0;
    item->m_packet.m_matrix[2][0] = 0;
    item->m_packet.m_matrix[2][1] = 0;
    item->m_packet.m_matrix[2][2] = m_gridScale;
    item->m_packet.m_matrix[2][3] = 0;
    item->m_packet.m_matrix[3][0] = 0;
    item->m_packet.m_matrix[3][1] = 0;
    item->m_packet.m_matrix[3][2] = 0;
    item->m_packet.m_matrix[3][3] = 1.0f;
    item->m_packet.m_x150[0] = item->m_packet.m_x150[1] = 0.0146484375f;
    item->m_packet.m_x150[2] = 6.0f;
    item->m_packet.m_alpha = alpha;
    *(int128_t *)&item->m_packet.m_stripTag = 0;
    item->m_packet.m_stripTag.EOP = 1;
    item->m_packet.m_stripTag.PRE = 1;
    item->m_packet.m_stripTag.PRIM = SCE_GS_SET_PRIM(5, 1, 1, 1, alpha, 0, 0, 0, 0);
    item->m_packet.m_stripTag.FLG = 0;
    item->m_packet.m_stripTag.NREG = 3;
    item->m_packet.m_stripTag.REGS0 = 2;
    item->m_packet.m_stripTag.REGS1 = 1;
    item->m_packet.m_stripTag.REGS2 = 4;
    item->m_packet.m_stripTag.NLOOP = 5;
    *(int128_t *)&item->m_packet.m_tag2 = 0;
    item->m_packet.m_tag2.NLOOP = 1;
    item->m_packet.m_tag2.EOP = 1;
    item->m_packet.m_tag2.PRE = 1;
    item->m_packet.m_tag2.FLG = 0;
    item->m_packet.m_tag2.NREG = 1;
    item->m_packet.m_tag2.REGS0 = 0xF;
    item->m_packet.m_tag2Data[3] = 0x7F;
    *(int128_t *)&item->m_packet.m_tag3 = 0;
    item->m_packet.m_tag3.NLOOP = 1;
    item->m_packet.m_tag3.EOP = 1;
    item->m_packet.m_tag3.PRE = 1;
    item->m_packet.m_tag3.FLG = 0;
    item->m_packet.m_tag3.NREG = 1;
    item->m_packet.m_tag3.REGS0 = 0xE;
    *(int128_t *)&item->m_packet.m_tag4 = 0;
    item->m_packet.m_tag4.NLOOP = 1;
    item->m_packet.m_tag4.EOP = 1;
    item->m_packet.m_tag4.PRE = 1;
    item->m_packet.m_tag4.FLG = 0;
    item->m_packet.m_tag4.NREG = 1;
    item->m_packet.m_tag4.REGS0 = 0xF;
    item->m_packet.m_tag4Data[3] = 0x7F;
    item->m_packet.m_x2A0[0] = 1;
    item->m_packet.m_vifMid[3] = 0x6C0180B5;
    *(int128_t *)&item->m_packet.m_fanTag = 0;
    item->m_packet.m_fanTag.NLOOP = m_halfRes ? 0x12 : 0x22;
    item->m_packet.m_fanTag.EOP = 1;
    item->m_packet.m_fanTag.PRE = 1;
    item->m_packet.m_fanTag.PRIM = SCE_GS_SET_PRIM(4, 1, 1, 1, alpha, 0, 0, 0, 0);
    item->m_packet.m_fanTag.FLG = 0;
    item->m_packet.m_fanTag.NREG = 3;
    item->m_packet.m_fanTag.REGS0 = 2;
    item->m_packet.m_fanTag.REGS1 = 1;
    item->m_packet.m_fanTag.REGS2 = 4;
    item->m_packet.m_vifEnd[0] = 0x030000B5;    // BASE
    item->m_packet.m_vifEnd[1] = 0x02000190;    // OFFSET
    item->m_packet.m_vifEnd[2] = 0;
    item->m_packet.m_vifEnd[3] = 0;
    item->m_numQwords = 0x2E;
    item->m_02 = 0;
    item->m_03 = 0x30;
    item->m_data = &item->m_packet;
    item->m_08 = 0;
    item->m_0C = 0;
    return item;
}


// ---- src/Terrain/CreateCommonDataItem.cpp
// CreateCommonDataItem__7TerrainFv  @ 0x00298CB0

extern "C" void *memalign(unsigned int alignment, unsigned int size);
extern "C" void *memset(void *dst, int c, unsigned int n);



// The terrain's common packet: TEST/CLAMP/ALPHA/TEXA, TEXFLUSH, the strip PRIM
// GIFtags and the per-terrain scale constants.
struct CommonPacket {
    unsigned int m_vif[4];      // +0x000
    sceGifTag m_gifTag;         // +0x010
    long m_test;                // +0x020
    long m_testReg;             // +0x028
    long m_clamp;               // +0x030
    long m_clampReg;            // +0x038
    long m_alpha;               // +0x040
    long m_alphaReg;            // +0x048
    long m_texa;                // +0x050
    long m_texaReg;             // +0x058
    unsigned int m_vifMid[4];   // +0x060
    sceGifTag m_flushTag;       // +0x070
    long m_texflush;            // +0x080
    long m_texflushReg;         // +0x088
    sceGifTag m_stripTag;       // +0x090
    sceGifTag m_fanTag;         // +0x0A0
    float m_x0B0[4];            // +0x0B0
    int m_x0C0[4];              // +0x0C0
    int m_x0D0[4];              // +0x0D0
    float m_scale[4][4];        // +0x0E0
    char pad120[0x1C0 - 0x120];
};

struct CommonDataItem : VifDataItem {
    CommonPacket m_packet;      // +0x10
};


VifDataItem *Terrain::CreateCommonDataItem()
{
    CommonDataItem *item = (CommonDataItem *)memalign(0x40, sizeof(CommonDataItem));
    memset(item, 0, sizeof(CommonDataItem));
    // Every packet field is written through this pointer (a pointer local, not
    // item->m_packet each time, gives retail's register pressure and its
    // rematerialized MSCAL constant).
    CommonPacket *packet = &item->m_packet;
    packet->m_vif[0] = 0x11000000;   // FLUSH
    packet->m_vif[1] = 0x03000000;   // BASE
    packet->m_vif[2] = 0x02000000;   // OFFSET
    packet->m_vif[3] = 0x6C058320;   // UNPACK
    *(int128_t *)&packet->m_gifTag = 0;
    packet->m_gifTag.FLG = 0;
    packet->m_gifTag.EOP = 1;
    packet->m_gifTag.NREG = 4;
    packet->m_gifTag.NLOOP = 1;
    packet->m_gifTag.REGS0 = 0xE;    // A+D
    packet->m_gifTag.REGS1 = 0xE;
    packet->m_gifTag.REGS2 = 0xE;
    packet->m_gifTag.REGS3 = 0xE;
    packet->m_test = 0x5000C;
    packet->m_testReg = 0x47;        // TEST_1
    packet->m_clamp = 0;
    packet->m_clampReg = 8;          // CLAMP_1
    packet->m_alpha = 0x44;
    packet->m_alphaReg = 0x42;       // ALPHA_1
    packet->m_texa = 0xFF000000FFL;
    packet->m_texaReg = 0x3B;        // TEXA
    packet->m_vifMid[0] = 0x14000000;    // MSCAL
    packet->m_vifMid[3] = 0x6C158000;    // UNPACK
    packet->m_flushTag.FLG = 0;
    packet->m_flushTag.EOP = 1;
    packet->m_flushTag.NREG = 2;
    packet->m_flushTag.NLOOP = 1;
    packet->m_flushTag.REGS0 = 0xE;
    packet->m_flushTag.REGS1 = 0xE;
    packet->m_texflush = 0;
    packet->m_texflushReg = 0x3F;    // TEXFLUSH
    packet->m_stripTag.NLOOP = m_halfRes ? 0x12 : 0x22;
    packet->m_stripTag.EOP = 1;
    packet->m_stripTag.PRE = 1;
    packet->m_stripTag.PRIM = 0x3C;
    packet->m_stripTag.FLG = 0;
    packet->m_stripTag.NREG = 3;
    packet->m_stripTag.REGS0 = 2;
    packet->m_stripTag.REGS1 = 1;
    packet->m_stripTag.REGS2 = 4;
    packet->m_fanTag.NLOOP = m_halfRes ? 0x12 : 0x22;
    packet->m_fanTag.EOP = 1;
    packet->m_fanTag.PRE = 1;
    packet->m_fanTag.PRIM = 0x7C;
    packet->m_fanTag.FLG = 0;
    packet->m_fanTag.NREG = 3;
    packet->m_fanTag.REGS0 = 2;
    packet->m_fanTag.REGS1 = 1;
    packet->m_fanTag.REGS2 = 4;
    float scale = 0.0146484375f;
    if (m_halfRes)
        scale *= 2.0f;
    packet->m_x0B0[0] = packet->m_x0B0[1] = scale;
    packet->m_x0B0[2] = 6.0f;
    packet->m_x0B0[3] = 1.0f;
    packet->m_x0C0[0] = packet->m_x0C0[1] = packet->m_x0C0[2] = 0x7F;
    packet->m_x0C0[3] = 0x40;
    packet->m_x0D0[3] = 0x8FF0;
    packet->m_scale[0][0] = m_gridScale;
    packet->m_scale[1][1] = m_gridScale;
    packet->m_scale[2][2] = m_gridScale;
    packet->m_scale[3][3] = 1.0f;
    item->m_numQwords = 0x1C;
    item->m_02 = 0;
    item->m_03 = 0x30;
    item->m_data = &item->m_packet;
    item->m_08 = 0;
    item->m_0C = 0;
    return item;
}


// ---- src/Terrain/Render3D.cpp




// Quadword matrix copy (libvu0 sceVu0CopyMatrix style inline asm).
static inline void CopyMatrix(void *dst, void *src)
{
    asm("lq $8, 0x0(%1); lq $9, 0x10(%1); lq $10, 0x20(%1); lq $11, 0x30(%1); sq $8, 0x0(%0); sq $9, 0x10(%0); sq $10, 0x20(%0); sq $11, 0x30(%0)"
        : : "r"(dst), "r"(src) : "$8", "$9", "$10", "$11", "memory");
}

extern Game *LpGame;
int RenderTerrain = 1;
extern int TimeGPU;

int Terrain::Render3D()
{
    if (!RenderTerrain)
        return 1;
    if (TimeGPU) {
        LpRenderTarget->TrySendRenderQueue(1);
        LpRenderTarget->WaitForRenderingToComplete();
    }
    static int CPUTimeID = -1;  // profiler ids, unused in retail
    static int count = 0;
    PSXCamera *camera = (PSXCamera *)LpRenderTarget->m_camera;
    QWORD *data = (QWORD *)m_restoreScissor + 19;  // the common item's per-frame constants
    CopyMatrix(data, &camera->m_x250);
    data += 4;
    CopyMatrix(data, &camera->m_guardBandViewProjection);
    data += 4;
    float invScale = 1.0f / m_gridScale;
    data->f[0] = camera->m_position.x * invScale;
    data->f[1] = camera->m_position.y * invScale;
    data->f[2] = camera->m_position.z * invScale;
    data->ui[3] = 0;
    data++;
    data->f[0] = 2e-5f * m_gridScale;
    data->f[1] = -(16.0f * invScale);
    data->f[2] = ((PSXRenderTarget *)LpRenderTarget)->m_fogEnd * invScale;
    data->f[3] = ((PSXRenderTarget *)LpRenderTarget)->m_fogScale * m_gridScale;
    m_xE4 = 0;
    m_xE8 = 0;
    if (!m_root->RenderDetail(m_detailTexture))
        return 0;
    if (TimeGPU) {
        static int GPUTimeID = -1;
        LpRenderTarget->TrySendRenderQueue(1);
        LpRenderTarget->WaitForRenderingToComplete();
    }
    if (LpGame->m_debugKeys && LpGame->m_debugOverlay) {
        LpGame->m_debugOverlay->PrintfHeading(m_debugPage, "Terrain");
        LpGame->m_debugOverlay->PrintfDetail(m_debugPage, "Total visible nodes:%d", m_numVisibleNodes);
        LpGame->m_debugOverlay->PrintfDetail(m_debugPage, "Memory %d", 0);
        LpGame->m_debugOverlay->PrintfDetail(m_debugPage, "Total Transforms %d", m_numVisibleNodes * (17 * 17));
        LpGame->m_debugOverlay->PrintfDetail(m_debugPage, "Total Triangles  %d", m_numVisibleNodes * 512);
    }
    return 1;
}


// ---- src/Terrain/PrepareGeometry.cpp


static inline int FloatToInt(float f)
{
    int i;
    asm("cvt.w.s $f5, %1; mfc1 %0, $f5" : "=r"(i) : "f"(f) : "$f5");
    return i;
}


extern int RenderTerrain;
extern int TotalBoxVisibility;
extern int TotalSphereVisibility;

int Terrain::PrepareGeometry()
{
    if (!RenderTerrain)
        return 1;
    static int TerrainPrepareGeometryID = -1;   // profiler ids, unused in retail
    static int TerrainPrprGeom = -1;
    if (m_ageManager) {
        int total;
        int size = CacheSize = m_ageManager->GetSize(&total);
        PeakCacheSize.Record(CacheSize);
        if (total > 0x100000)
            m_ageManager->Purge(size - total + 0x100000);
        m_ageManager->Tick();
    }
    if (m_meshAgeManager) {
        int total;
        int size = m_meshAgeManager->GetSize(&total);
        if (total > 0x100000)
            m_meshAgeManager->Purge(size - total + 0x80000);
        m_meshAgeManager->Tick();
    }
    m_camera = LpRenderTarget->m_camera;
    TotalBoxVisibility = 0;
    TotalSphereVisibility = 0;
    m_evaluations = 0;
    m_xF0 = 0;
    m_xF4 = 0;
    m_vertexNotifications = 0;
    m_parentNotifications = 0;
    m_x100 = 0;
    m_x104 = 0;
    m_x108 = 0;
    m_vertexEvaluations = 0;
    int changed = 0;
    if (m_renderTolerance != m_xC8)
        changed = 1;
    m_cameraPosition = m_camera->m_position;
    m_cameraGrid = m_camera->m_position;
    m_cameraGrid.x /= m_gridScale;
    m_cameraGrid.y /= m_gridScale;
    m_cameraGrid.z /= m_gridScale;
    m_xCC = FloatToInt(0.5f + m_camera->m_focalLength);
    m_xC8 = m_renderTolerance;
    CalculateKappa();
    m_numVisibleNodes = 0;
    m_xDFC++;
    m_root->SelectVisibleBlocks(m_file, &m_camera->m_viewProjection, changed);
    m_dirty = 0;
    if (m_detailTexture && 10.0f > m_detailTexture->m_usage)
        m_detailTexture->m_usage = 10.0f;
    LpRenderTarget->TrySendRenderQueue(0);
    return 1;
}


// ---- src/Terrain/EventHandler.cpp

int Terrain::EventHandler(int event)
{
    return 0;
}


// ---- src/Terrain/KeyUpHandler.cpp
int Terrain::KeyUpHandler(ProcessNode *node, KeyInfo *key)
{
    return 0;
}


// ---- src/Terrain/SetRenderTolerance.cpp
void Terrain::SetRenderTolerance(int tolerance)
{
    if (tolerance < 0)
        tolerance = 0;
    if (m_renderTolerance != tolerance) {
        m_renderTolerance = tolerance;
        m_dirty = 1;
    }
}


// ---- src/Terrain/SetGraphicQuality.cpp
int RenderDetail = 1;

void Terrain::SetGraphicQuality(int quality)
{
    m_quality = quality;
    RenderDetail = LpTerrainQuality[m_quality].m_renderDetail;
    SetRenderTolerance(LpTerrainQuality[m_quality].m_tolerance);
    if (m_xCDC)
        m_lodDistance = LpTerrainQuality[9].m_lodDistance;
    else
        m_lodDistance = LpTerrainQuality[m_quality].m_lodDistance;
    m_xCD4 = LpTerrainQuality[m_quality].m_8;
}


// ---- src/Terrain/dtor.cpp

extern "C" void free(void *p);

class Terrain;
extern Terrain *GameTerrain;




Terrain::~Terrain()
{
    int i;
    if (m_xE4C) {
        for (i = 0; i < m_xE50; i++)
            m_xE4C[i]->Release();
        free(m_xE4C);
    }
    if (m_detailTexture)
        m_detailTexture->Release();
    if (m_xAC)
        m_xAC->Release();
    if (m_numTextures) {
        for (i = 0; i < m_numTextures; i++) {
            if (m_textures[i])
                m_textures[i]->Release();
        }
    }
    if (m_root) {
        m_root->DestroyMesh(1);
        delete m_root;
    }
    if (m_geometries) {
        for (i = 0; i < m_numGeometries; i++) {
            if (m_geometries[i])
                delete m_geometries[i];
        }
        free(m_geometries);
    }
    if (m_ageManager)
        delete m_ageManager;
    if (m_meshAgeManager)
        delete m_meshAgeManager;
    if (m_visible)
        free(m_visible);
    if (m_restoreScissor)
        free(m_restoreScissor);
    for (i = 0; i < 8; i++) {
        if (m_scissorItems[i])
            free(m_scissorItems[i]);
        if (m_scissorItems2[i])
            free(m_scissorItems2[i]);
    }
    GameTerrain = 0;
}


// ---- src/Terrain/CalculateKappa.cpp

void Terrain::CalculateKappa()
{
    int a1 = m_xC8;
    int a2 = m_xCC;
    m_threshold = (float)(a1 * a1) / (float)(a2 * a2);
    m_xD0 = (float)m_xC8 / (float)m_xCC;
    m_xD8 = 1.0f / m_threshold;
    m_xDC = 1.0f / m_xD0;
}


static PatchCacheItem PatchCache[32];

// ---- src/Terrain/GetGridPointIntersection.cpp


static inline float Fmax(float a, float b)
{
    float r;
    asm("max.s $f8, %1, %2; swc1 $f8, 0(%0)" : : "r"(&r), "f"(a), "f"(b) : "$f8", "memory");
    return r;
}

// libvu0 sceVu0Normalize.
static inline void Normalize(_D3DVECTOR *out, const _D3DVECTOR &in)
{
    asm("lqc2 vf4, 0x0(%1); vmul.xyz vf5, vf4, vf4; vaddy.x vf5, vf5, vf5y; vaddz.x vf5, vf5, vf5z; vrsqrt Q, vf0w, vf5x; vsub.xyzw vf6, vf0, vf0; vaddw.xyzw vf6, vf6, vf4w; vwaitq; vmulq.xyz vf6, vf4, Q; sqc2 vf6, 0x0(%0)"
        : : "r"(out), "r"(&in) : "memory");
}


int TotalCacheItems = 10;

// The hardware quality table, one row per graphic-quality level.
TerrainQuality TerrainQualityHardware[] = {
    {7, 0x1000, 0, 1},
    {6, 0x800, 0, 1},
    {5, 0x400, 0, 1},
    {4, 0x400, 0, 1},
    {4, 0x400, 0, 1},
    {3, 0x200, 0, 1},
    {3, 0x200, 0, 1},
    {2, 0x200, 0, 1},
    {2, 0x100, 0, 1},
    {1, 0x100, 0, 1},
};


// The normal blends divide through these two helpers rather than d3dtypes.h's
// operator/: the extra inline level (MakeVec's parameters) makes mwcc number
// the x product before the y product, which picks retail's registers.
static inline _D3DVECTOR MakeVec(float x, float y, float z)
{
    return _D3DVECTOR(x, y, z);
}

// v / s, as one reciprocal and three multiplies.
static inline _D3DVECTOR DivV(const _D3DVECTOR &v, float s)
{
    float r = 1.0f / s;
    return MakeVec(v.x * r, v.y * r, v.z * r);
}

// Height, normal and material of the terrain under point (x, z); the cell's
// heights and normals are cached.
int Terrain::GetGridPointIntersection(_D3DVECTOR *point, _D3DVECTOR *normal, int flags, unsigned char *material)
{
    static int GetGridPointIntersectionID = -1;     // unused profiler ID
    float d[4];
    int step = 1;
    float size = 1.0f;
    if (m_xE04) {
        step = 2;
        size = 2.0f;
    }
    if (!m_xE58) {
        for (int i = 0; i < 32; i++) {
            PatchCache[i].m_key = -1;
            m_xE58 = 1;
        }
    }
    if (m_root) {
        float fx = point->x * m_xC60;
        float fz = point->z * m_xC60;
        if (fx < 0.0f)
            fx = 0.0f;
        if (fz < 0.0f)
            fz = 0.0f;
        int x = fx;
        int z = fz;
        if (step == 2) {
            if (x & 1)
                x--;
            if (z & 1)
                z--;
        }
        if (x < 0)
            return;
        if (z < 0)
            return;
        PatchCacheItem *item;
        int key = (x << 16) | z;
        item = 0;
        for (int i = 0; i < TotalCacheItems; i++) {
            if (key == PatchCache[i].m_key) {
                item = &PatchCache[i];
                break;
            }
        }
        if (!item) {
            item = &PatchCache[m_xE5C];
            m_xE5C++;
            if (m_xE5C >= TotalCacheItems)
                m_xE5C = 0;
            float heights[4];
            if (m_root->GetPatch(x, z, heights, item->m_normals, item->m_materials, step)) {
                item->m_heights[0] = heights[0] * m_gridScale;
                item->m_heights[1] = heights[1] * m_gridScale;
                item->m_heights[2] = heights[2] * m_gridScale;
                item->m_heights[3] = heights[3] * m_gridScale;
                item->m_key = key;
                item->m_x = x;
                item->m_z = z;
                item->m_fx = fx;
                item->m_fz = fz;
            } else {
                if (normal) {
                    normal->x = 0.0f;
                    normal->y = 1.0f;
                    normal->z = 0.0f;
                }
                item->m_key = 0x7FFFFFFF;
                return;
            }
        }
        float u;
        float uu;
        float iuiu;
        float iu;
        float vv;
        float iv;
        float iviv;
        float v;
        u = (fx - x) / size;
        v = (fz - z) / size;
        vv = v * v;
        iu = 1.0f - u;
        iuiu = iu * iu;
        iv = 1.0f - v;
        iviv = iv * iv;
        uu = u * u;
        d[0] = uu + vv;
        d[1] = iuiu + vv;
        d[2] = uu + iviv;
        d[3] = iuiu + iviv;
        d[0] = Fmax(0.001f, d[0]);
        d[1] = Fmax(0.001f, d[1]);
        d[2] = Fmax(0.001f, d[2]);
        d[3] = Fmax(0.001f, d[3]);
        if (material) {
            float best = 3.4028235e38f;
            int nearest;
            for (int i = 0; i < 4; i++) {
                if (d[i] < best) {
                    best = d[i];
                    nearest = i;
                }
            }
            *material = item->m_materials[nearest];
        }
        if (m_isPlain == 1 || (x & 1) != (z & 1)) {
            if (d[0] < d[3]) {
                point->y = item->m_heights[0] + u * (item->m_heights[1] - item->m_heights[0]) + v * (item->m_heights[2] - item->m_heights[0]);
                if (normal)
                    Normalize(normal, DivV(item->m_normals[0], d[0]) + DivV(item->m_normals[1], d[1]) + DivV(item->m_normals[2], d[2]));
            } else {
                point->y = item->m_heights[2] + u * (item->m_heights[3] - item->m_heights[2]) + (1.0f - v) * (item->m_heights[1] - item->m_heights[3]);
                if (normal)
                    Normalize(normal, DivV(item->m_normals[1], d[1]) + DivV(item->m_normals[3], d[3]) + DivV(item->m_normals[2], d[2]));
            }
        } else {
            if (d[1] < d[2]) {
                point->y = item->m_heights[0] + u * (item->m_heights[1] - item->m_heights[0]) + v * (item->m_heights[3] - item->m_heights[1]);
                if (normal)
                    Normalize(normal, DivV(item->m_normals[0], d[0]) + DivV(item->m_normals[1], d[1]) + DivV(item->m_normals[2], d[2]));
            } else {
                point->y = item->m_heights[2] + u * (item->m_heights[3] - item->m_heights[2]) + (1.0f - v) * (item->m_heights[0] - item->m_heights[2]);
                if (normal)
                    Normalize(normal, DivV(item->m_normals[0], d[0]) + DivV(item->m_normals[3], d[3]) + DivV(item->m_normals[2], d[2]));
            }
        }
    }
}


// ---- src/Terrain/KeyDownHandler.cpp
extern "C" char *strcpy(char *dst, const char *src);

int KEYDOWN_HANDLER_TEST(unsigned int key, DeviceType device, ProcessNode *node, unsigned int mods);

// An inlined strrchr.
static inline char *FindLast(char *s, char c)
{
    char *start = s;
    while (*s++)
        ;
    do {
        s--;
    } while (s != start && *s != c);
    if (*s == c)
        return s;
    return 0;
}


int Terrain::KeyDownHandler(ProcessNode *node, KeyInfo *key)
{
    if (KEYDOWN_HANDLER_TEST(0x1E, DEVICE_KEYBOARD, node, 0x80)) {
        if (RenderDetail && m_xCD8) {
            m_xCDC = 1 - m_xCDC;
            if (m_xCDC) {
                m_lodDistance = LpTerrainQuality[9].m_lodDistance;
                return 1;
            }
            m_lodDistance = LpTerrainQuality[m_quality].m_lodDistance;
        }
        RenderDetail = 1 - RenderDetail;
        return 1;
    }
    if (KEYDOWN_HANDLER_TEST(0x1F, DEVICE_KEYBOARD, node, 0x80)) {
        RenderTerrain = 1 - RenderTerrain;
        return 1;
    }
    if (KEYDOWN_HANDLER_TEST(0x13, DEVICE_KEYBOARD, node, 0x80)) {
        ConvertToPlain();
        return 1;
    }
    if (KEYDOWN_HANDLER_TEST(0x16, DEVICE_KEYBOARD, node, 0x80)) {
        WriteTexturesToFile(0, 0);
        return 1;
    }
    if (KEYDOWN_HANDLER_TEST(0x1A, DEVICE_KEYBOARD, node, 0x80)) {
        char filename[0x104];
        strcpy(filename, m_filename);
        if (m_isPlain == 1)
            strcpy(FindLast(filename, '.'), ".tr2");
        ExportTerrain(filename);
        return 1;
    }
    return 0;
}


// ---- src/Terrain/GetMinMaxHeight.cpp



void Terrain::GetMinMaxHeight(float *minHeight, float *maxHeight)
{
    if (minHeight)
        *minHeight = m_root->m_minHeight * m_gridScale;
    if (maxHeight)
        *maxHeight = m_root->m_maxHeight * m_gridScale;
}


// ---- src/Terrain/ExportTerrain.cpp

int Terrain::ExportTerrain(char *filename)
{
    return 0;
}


// ---- src/Terrain/ConvertToPlain.cpp
extern "C" void *malloc(unsigned int size);









// Replaces every LOD geometry with a plain (single-level) copy.
void Terrain::ConvertToPlain()
{
    GridNodeGeometryBase *plain;
    int i;

    if (m_isPlain == 1)
        return;
    for (i = 0; i < m_numGeometries; i++) {
        plain = new GridNodeGeometryPlain((GridNodeGeometryLOD *)m_geometries[i]);
        m_root->SubstituteGeometry(m_geometries[i], plain);
        delete m_geometries[i];
        m_geometries[i] = plain;
    }
    m_isPlain = 1;
    m_visible = (DrawableGridNode **)malloc(4000 * PS2_PTR_SIZE);
    m_numVisibleNodes = 0;
}


// ---- src/Terrain/WriteTexturesToFile.cpp
extern "C" int sprintf(char *buf, const char *fmt, ...);
extern "C" char *strcpy(char *dst, const char *src);
extern "C" void *malloc(unsigned int size);
extern "C" void free(void *p);

struct FILE_HANDLE_STRUCT;
struct _RECT;
class ColorMapper;
long FTELL(FILE_HANDLE_STRUCT *file);
int BytesPerPixel(int format);
int BytesPerPixel(ImageDataFormat format);
void PixTransColorConvert(void *dst, void *src, int width, int height, int dstPitch, int srcPitch, int dstFormat,
                          int srcFormat, int a, ColorMapper *mapper, int alpha, unsigned int color);

// Writes each texture into the open terrain file, or dumps it to its own
// numbered file when there is none.
void Terrain::WriteTexturesToFile(FILE_HANDLE_STRUCT *file, int *offsets)
{
    char name[0x110];
    char suffix[0x100];
    long pitch;
    ImageData *image = new ImageData;
    unsigned char *buffer = (unsigned char *)malloc(BytesPerPixel(0x378) << 16);
    ImageDataFormat format = (ImageDataFormat)(file ? 7 : 1);
    for (int i = 0; i < m_numTextures; i++) {
        if (file) {
            *offsets = FTELL(file);
        } else {
            sprintf(suffix, "%03d", i);
            strcpy(name, m_filename);
            strcpy(FindLast(name, '.'), suffix);
        }
        void *bits = m_textures[i]->LockSys(0, &pitch, 0);
        if (!bits)
            break;
        int bytesPerPixel = BytesPerPixel(m_textures[i]->m_format);
        int dstFormat = BytesPerPixel(format) == 3 ? 0x378 : 0x22B;
        PixTransColorConvert(buffer, bits, m_textures[i]->m_width, m_textures[i]->m_height, 0x100,
                             pitch / bytesPerPixel, dstFormat, m_textures[i]->m_format, 0, 0, 0x80, 0xFF00FF);
        image->LoadFromArray(buffer, format, m_textures[i]->m_width, m_textures[i]->m_height, 8, 8);
        if (!m_textures[i]->UnlockSys(0))
            break;
        image->CreateMipMapChain();
        if (file)
            image->Save(file);
        else
            image->Dump(name, 16);
        offsets++;
    }
    free(buffer);
    delete image;
}


// ---- src/PatchCacheItem/ctor.cpp

PatchCacheItem::PatchCacheItem()
{
}


// ---- src/sinit/Terrain.cpp
// Static initializer of Terrain.cpp: the axis vectors, PeakCacheSize and
// PatchCache (defined above).


// Defined here for its retail .bss slot.
TerrainQuality *LpTerrainQuality;
