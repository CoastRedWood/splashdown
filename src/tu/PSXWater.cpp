// PSXWater.cpp  @ 0x002C9050 (text 0x2C9050-0x2CB630)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp)
//
// Retail's PSXWater.cpp as one translation unit: the first TU of its
// tu_map row (TextOverlay.cpp, MediaControl.cpp, PSXMediaControl.cpp and
// PSXMpegVideo.cpp follow, each its own src/tu file). The zlib helpers
// ZLibCompress/ZLibDecompress, PSXWater and the out-of-line _D3DINTVECTOR
// default ctor; .rodata @860..@1181 up to Water's RTTI. `make link`
// places its data as well as its code.




#include "PSXWaterDisplacement/PSXWaterDisplacement.h"
#include "PSXWater/PSXWater.h"
#include "GameObject/GameObject.h"
#include "d3dtypes.h"
#include "Water/Water.h"
#include "DmaRenderItem/DmaRenderItem.h"
#include "soultree_assert_block.h"
#include "sceGifTag/sceGifTag.h"
#include "QuadTree/QuadTree.h"
#include "SearchableObject/SearchableObject.h"
#include "_sceDmaTag/_sceDmaTag.h"
#include "RenderTarget/RenderTarget.h"
#include "PSXCamera/PSXCamera.h"
#include "PSXTextureMap/PSXTextureMap.h"
#include "PSXRenderStateBlock/PSXRenderStateBlock.h"
#include "StringTable/StringTable.h"
#include "TextureMap/TextureMap.h"
#include "ParameterBlock/ParameterBlock.h"
#include "PSXSurfaceGeometry/PSXSurfaceGeometry.h"
#include "PSXSurfaceShaderInstance/PSXSurfaceShaderInstance.h"
#include "SurfaceShaderList/SurfaceShaderList.h"
#include "PSXSignatureWave/PSXSignatureWave.h"
#include "SoultreeObject/SoultreeObject.h"
#include "WaterObject/WaterObject.h"
#include "sinit/AxisVectors.h"
#include "sinit/D3DIntVector.h"

#pragma fast_fptosi on

extern "C" void *memalign(unsigned int align, unsigned int size);
extern "C" void *memset(void *dst, int c, unsigned int n);
extern "C" void free(void *p);
extern "C" int sprintf(char *buf, const char *fmt, ...);
extern "C" char *strcpy(char *dst, const char *src);

typedef unsigned long u_long;

// ZLibCompress/ZLibDecompress (0x2C9050-0x2C9160) open this TU: their
// "1.0" (@341) comes after JetskiGameOptions.cpp's RTTI and before
// PSXWater's literals (@898), with no label restart between.
typedef unsigned long uLong;  // 64-bit under mwccps2

// zlib 1.0.x z_stream.
struct z_stream {
    unsigned char *next_in;     // +0x00
    unsigned int avail_in;      // +0x04
    uLong total_in;             // +0x08
    unsigned char *next_out;    // +0x10
    unsigned int avail_out;     // +0x14
    uLong total_out;            // +0x18
    char *msg;                  // +0x20
    void *state;                // +0x24
    void *zalloc;               // +0x28
    void *zfree;                // +0x2C
    void *opaque;               // +0x30
    int data_type;              // +0x34
    uLong adler;                // +0x38
    uLong reserved;             // +0x40
};

// ---- src/global/ZLibCompress.cpp
extern "C" int deflateInit_(z_stream *strm, int level, const char *version, int stream_size);
extern "C" int deflate(z_stream *strm, int flush);
extern "C" int deflateEnd(z_stream *strm);

// Deflates `size` bytes of src into dst in one call (Z_FINISH, default
// level); returns the compressed size.
int ZLibCompress(void *src, void *dst, int size)
{
    z_stream stream;
    memset(&stream, 0, sizeof(stream));
    stream.next_in = (unsigned char *)src;
    stream.avail_in = size;
    stream.next_out = (unsigned char *)dst;
    stream.avail_out = size;
    deflateInit_(&stream, -1, "1.0", sizeof(stream));
    deflate(&stream, 4);
    int compressed = stream.total_out;
    deflateEnd(&stream);
    return compressed;
}


// ---- src/global/ZLibDecompress.cpp
extern "C" int inflateInit_(z_stream *strm, const char *version, int stream_size);
extern "C" int inflate(z_stream *strm, int flush);
extern "C" int inflateEnd(z_stream *strm);

// Inflates `size` bytes of src into dst in one call (Z_FINISH).
void ZLibDecompress(void *dst, void *src, int size)
{
    z_stream stream;
    memset(&stream, 0, sizeof(stream));
    stream.next_in = (unsigned char *)src;
    stream.avail_in = size;
    stream.next_out = (unsigned char *)dst;
    stream.avail_out = size;
    inflateInit_(&stream, "1.0", sizeof(stream));
    inflate(&stream, 4);
    inflateEnd(&stream);
}


#define SCE_GS_SET_TEX0(tbp, tbw, psm, tw, th, tcc, tfx, cbp, cpsm, csm, csa, cld) \
    ((u_long)(tbp) | ((u_long)(tbw) << 14) | ((u_long)(psm) << 20) | ((u_long)(tw) << 26) | \
     ((u_long)(th) << 30) | ((u_long)(tcc) << 34) | ((u_long)(tfx) << 35) | ((u_long)(cbp) << 37) | \
     ((u_long)(cpsm) << 51) | ((u_long)(csm) << 55) | ((u_long)(csa) << 56) | ((u_long)(cld) << 61))

// A material's texture reference and shader, as PrepareGeometry reads them
// (LpMaterialStringTable's entries; no shared header yet).
#ifdef SPLASHDOWN_PC
// x86-64: SurfaceMaterial's m_shaders[0] and m_instances[0], ShaderDescriptor's m_textures[0]
#include "SurfaceMaterial/SurfaceMaterial.h"
#include "ShaderDescriptor/ShaderDescriptor.h"
struct MaterialTexture {
    char pad0[__builtin_offsetof(ShaderDescriptor, m_textures)];
    short m_textureIndex;
};

struct Material {
    char pad0[__builtin_offsetof(SurfaceMaterial, m_shaders)];
    MaterialTexture *m_texture;
    char pad4C[__builtin_offsetof(SurfaceMaterial, m_instances) - __builtin_offsetof(SurfaceMaterial, m_shaders) - sizeof(void *)];
    int m_shader;
};
#else
struct MaterialTexture {
    char pad0[0x8];
    short m_textureIndex;               // +0x08
};

struct Material {
    char pad0[0x48];
    MaterialTexture *m_texture;         // +0x48
    char pad4C[0x58 - 0x4C];
    int m_shader;                       // +0x58
};
#endif

struct WaterPacket {
    char pad[0x50];
};

extern QuadTree *LpVisibilityQuadTree;
extern void *gWaterDmaData;
extern _D3DVECTOR gwp;
extern int TimeGPU;
extern float gWakeScale;
extern float gWaterScale;
extern _D3DVECTOR gNearClipTopLeft;
extern StringTable *LpMaterialStringTable;
extern StringTable *LpTextureMapStringTable;
extern SurfaceShaderList *LpSurfaceShaderList;
extern char WaterContentDirectory[];
extern float gBaseScale;
extern float PolygonScale;
extern float gGuardBandAdditionalFov;
extern float gPoolDepth;
extern int gbRenderInfinitePlane;
extern int giFadeAlpha[];
extern float gfMinForce;
extern float gfStartScale;
extern float gfBaseLineScale;

// The TU's globals (.data, then .bss in the static initializer's order).
int gRenderWater = 1;
int gOffset = 0x100;
float gTexCoordScale = 0.1f;
int gAref2 = 50;
float gfWaterBaseSwellHeight;
float gfWaterBaseSwellPeriod;
float gfOriginalBaseHeight;
_D3DINTVECTOR gWaterColor1(33, 44, 55, 32);
_D3DINTVECTOR gWaterColor2(127, 127, 127, 32);
_D3DVECTOR constants1(0.0f, 0.0f, 1111.0f, -2.0f);
_D3DVECTOR constants2(0.0f, 32.0f, 32.0f, 0.5f);
_D3DVECTOR gFrothFrameBlend(0.1f, 0.0f, 0.0f, 0.0f);
float gfBaseTextureConstant;

static inline void SetDmaTag(_sceDmaTag *tag, int qwc, int id, _sceDmaTag *next)
{
    tag->qwc = qwc;
    tag->mark = 0;
    tag->id = id;
    tag->next = next;
    tag->p[0] = 0;
    tag->p[1] = 0;
}

// ---- src/PSXWater/ApplyWakeFroth.cpp

void PSXWater::ApplyWakeFroth(_D3DVECTOR &pos, float strength, _D3DVECTOR &dir)
{
    m_displacement->ApplyWakeFroth(pos, strength, dir);
}


// ---- src/PSXWater/KeyDownHandler.cpp


int PSXWater::KeyDownHandler(ProcessNode *node, KeyInfo *key)
{
    return GameObject::KeyDownHandler(node, key);
}


// ---- src/PSXWater/ctor.cpp

PSXWater::PSXWater(int enabled, int resolution, int numTiles) : Water(enabled)
{
    m_resolution = resolution;
    m_numVertices = m_resolution * 3 + 1;
    m_numTiles = numTiles;
    m_x388 = m_numVertices * 23;
    m_xE0 = m_numTiles * 20;
    m_gridSize = m_numVertices;
    m_x38C = -2.0f / (m_numVertices - 1);
    m_x390 = 2.0f / (m_numVertices - 2);
    constants1.x = -m_x390;
    constants2.x = m_numVertices + 2;
    m_x498 = (float)(m_numTiles * 17 + 2) / (m_numTiles * 17);
    m_dmaData = 0;
}


// ---- src/_D3DINTVECTOR/ctor.cpp

_D3DINTVECTOR::_D3DINTVECTOR()
{
    x = y = z = w = 0;
}


// ---- src/PSXWater/dtor.cpp

PSXWater::~PSXWater()
{
    if (m_dmaData)
        free(m_dmaData);
    if (m_geometry)
        delete m_geometry;
}


// ---- src/PSXWater/Create.cpp

// Sets up the water's DMA buffer, GIFtags and A+D register block, and puts
// it in the visibility quadtree.
PSXWater *PSXWater::Create(char *name, LightManager *lights, int a, int b, float f)
{
#line 163 "PSXWater.cpp"
    ASSERTF(sizeof(WaterPacket) == 0x50, ("Create: wrong qword length (%d)", sizeof(WaterPacket) / 16));
    Water::Create(name, lights, a, b, f);
    m_dmaData = memalign(0x80, (m_numVertices + 1) * m_numTiles * 23 * 16);
    gWaterDmaData = m_dmaData;
    memset(&m_stripTag, 0, sizeof(sceGifTag));
    m_stripTag.EOP = 1;
    m_stripTag.PRE = 1;
    m_stripTag.FLG = 0;
    m_stripTag.NREG = 3;
    m_stripTag.REGS0 = 2;
    m_stripTag.REGS1 = 1;
    m_stripTag.REGS2 = 4;
    m_stripTag.NLOOP = 0x22;
    m_stripTag.PRIM = 0x7C;
    m_vuConstants.i[0] = m_vuConstants.i[1] = m_vuConstants.i[2] = m_vuConstants.i[3] = m_vuConstants.i[4]
        = m_vuConstants.i[5] = m_vuConstants.i[6] = m_vuConstants.i[7] = m_vuConstants.i[8] = m_vuConstants.i[9]
        = m_vuConstants.i[10] = m_vuConstants.i[11] = m_vuConstants.i[12] = 0;
    gwp.y = m_baseHeight;
    memset(&m_adTag, 0, sizeof(sceGifTag));
    m_adTag.NLOOP = 1;
    m_adTag.EOP = 0;
    m_adTag.PRE = 1;
    m_adTag.FLG = 0;
    m_adTag.NREG = 4;
    m_adTag.REGS0 = 0xE;
    m_adTag.REGS1 = 0xE;
    m_adTag.REGS2 = 0xE;
    m_adTag.REGS3 = 0xE;
    _D3DVECTOR min;
    _D3DVECTOR max;
    min.x = LpVisibilityQuadTree->m_sizeX / 2.0f;
    min.y = 0.0f;
    min.z = LpVisibilityQuadTree->m_sizeZ / 2.0f;
    max.x = min.x / 2.0f;
    max.y = 1000.0f;
    max.z = min.z / 2.0f;
    unsigned int code = LpVisibilityQuadTree->CalculateBoxPlacementCode(&min, &max, 0);
    LpVisibilityQuadTree->InsertObject(this, code, &min, &max);
    m_vif[0] = 0x11000000;      // FLUSH
    m_vif[1] = 0x03000000;      // BASE
    m_vif[2] = 0x02000000;      // OFFSET
    m_vif[3] = 0x6C1CC000;      // UNPACK
    m_x390 = 2.0f / m_numVertices;
    constants1.x = -m_x390;
    constants2.x = m_numVertices;
    m_constants[0] = constants1;
    m_constants[1] = constants2;
    m_stripTagCopy = m_stripTag;
    m_adTagCopy = m_adTag;
    m_tex0aReg = 6;             // TEX0_1
    m_tex0bReg = 6;
    m_texflush = 0;
    m_texflushReg = 0x3F;       // TEXFLUSH
    m_alphaA = 0x44;
    m_alphaAReg = 0x42;         // ALPHA_1
    m_alphaB = 0x44;
    m_alphaBReg = 0x42;
    m_testA = 0x5000D;
    m_testAReg = 0x47;          // TEST_1
    m_testB = ((long)gAref2 << 4) | 0x5000D;
    m_testBReg = 0x47;
    m_colors[0] = gWaterColor1;
    m_colors[1] = gWaterColor2;
    return this;
}


// ---- src/PSXWater/Render3D.cpp

// What closed the 130-word gap:
// - the m_x2C0/m_x310/m_x300 arithmetic is computed into vector locals (a, c, b) BEFORE the
//   m_constants1 stores; it then runs before the matrix copy in the pre-RA schedule;
// - @temp numbering of a struct local follows the order its pieces first appear: `a` declared
//   first (its default ctor writes z, y, x, w at function entry, so its pieces number above
//   offset's), `offset` copy-constructed (pieces x, y, z, w), `c` default-constructed in place;
// - sz2 kept with the no-code `sz2 = sz2;`;
// - m_geometry is a void * cast at the call: the cast argument is evaluated before `this`.
int PSXWater::Render3D()
{
    _D3DVECTOR a;
    float depth;
    PSXCamera *camera;
    float tiles;
    float u;
    float v;
    float texScale;
    float sy;
    PSXWaterDisplacement *d;
    float sx;
    WaterDisplacementParams *params;
    float scale;
    float sz;
    float farClip;
    float camNear;
    float sz2;
    float nearClip;
    float w;
    float inv;
    if (!gRenderWater)
        return GameObject::Render3D();
    if (TimeGPU) {
        LpRenderTarget->WaitForRenderingToComplete();
        LpRenderTarget->TrySendRenderQueue(0);
        LpRenderTarget->WaitForRenderingToComplete();
    }
    static int CPUTimeID = -1;          // unused profiler ID
    m_vuConstants.f[0] = gWakeScale;
    m_vuConstants.f[1] = m_weights[0] * (1.0f - m_displacement->m_frac[0]);
    m_vuConstants.f[2] = m_weights[0] * m_displacement->m_frac[0];
    m_vuConstants.f[4] = m_weights[1] * (1.0f - m_displacement->m_frac[1]);
    m_vuConstants.f[5] = m_weights[1] * m_displacement->m_frac[1];
    m_vuConstants.f[6] = m_weights[2];
    m_vuConstants.f[8] = m_weights[3];
    gFrothFrameBlend.y = 1.0f - m_displacement->m_frac[4];
    gFrothFrameBlend.z = m_displacement->m_frac[4];
    m_vuConstants.i[13] = m_numTiles;
    m_vuConstants.f[12] = -0.0310058594f * gWaterScale * (gWakeScale + (m_weights[3] + (m_weights[2] + (m_weights[0] + m_weights[1]))));
    d = m_displacement;
    sx = d->m_x3F0.x;
    sz = d->m_x3F0.z;
    sz2 = 2.0f * sz;
    u = d->m_corners[1].x;
    sy = d->m_x3F0.y;
    camera = (PSXCamera *)LpRenderTarget->m_camera;
    scale = m_x498;
    tiles = m_numTiles;
    texScale = gTexCoordScale;
    params = d->m_params;
    v = d->m_corners[1].z;
    sz2 = sz2;
    inv = 1.0f / tiles;
    a.y = scale * (2.0f * sy * inv);
    a.x = scale * (2.0f * sx * inv);
    _D3DVECTOR offset(d->m_x3C0);
    a.z = scale * (sz2 * inv);
    camNear = camera->m_nearClip;
    farClip = params->m_x34;
    _D3DVECTOR c;
    c.x = (int)(u * texScale);
    c.y = (int)(v * texScale);
    c.z = 0.0f;
    c.w = texScale;
    depth = 1.0f - camNear / camera->m_farClip;
    nearClip = params->m_farClip;
    w = 255.0f / (farClip - nearClip);
    _D3DVECTOR b(0.0f, 1.0f - depth, 5.96046519e-08f * depth);
    m_constants[0].w = w;
    m_constants[0].z = -nearClip * w;
    m_x160 = camera->m_x250;
    m_x1A0 = m_vuConstants;
    m_x1E0 = camera->m_position;
    m_x210 = gFrothFrameBlend;
    m_x2B0 = offset;
    m_x2C0 = a;
    m_x2D0 = gNearClipTopLeft;
    m_x300 = b;
    m_x310 = c;
    SetDmaTag(&m_vifTag, 0x1D, 0x30, (_sceDmaTag *)m_vif);
    SetDmaTag(&m_dataTag, m_x388 * m_numTiles, 0x30, (_sceDmaTag *)m_dmaData);
    m_shader->AddGeometry((SurfaceGeometry *)(void *)m_geometry, &m_vifTag, 0);
    return GameObject::Render3D();
}


// ---- src/PSXWater/UpdateTex0.cpp

// TEX0 for the water texture, plain and at gOffset. (Retail passes tw for th.)
void PSXWater::UpdateTex0()
{
    PSXRenderStateBlock state;
    PSXTextureMap *texture = m_texture;
    texture->CalculateReqdMipLevel(0.0f);
    state.SetTextureMap(texture);
    texture->GetReqdMipLevel();
    m_tex0a = SCE_GS_SET_TEX0(state.m_tbp, state.m_tbw, state.m_psm, state.m_tw, state.m_tw, state.m_tcc,
                             state.m_tfx, state.m_cbp, state.m_cpsm, state.m_csm, state.m_csa, state.m_cld);
    m_tex0b = SCE_GS_SET_TEX0(state.m_tbp + gOffset, state.m_tbw, state.m_psm, state.m_tw, state.m_tw,
                                   state.m_tcc, state.m_tfx, state.m_cbp, state.m_cpsm, state.m_csm, state.m_csa,
                                   state.m_cld);
}


// ---- src/PSXWater/PrepareGeometry.cpp

int PSXWater::PrepareGeometry()
{
    static int PSXWtrPrpGeoID = -1;   // profiler id, unused in retail
    Material *material = (Material *)LpMaterialStringTable->GetPointer(m_geometry->m_materialIndex);
    TextureMap *texture = (TextureMap *)LpTextureMapStringTable->GetPointer(material->m_texture->m_textureIndex);
    if (texture) {
        m_texture = (PSXTextureMap *)texture;
        long pitch;
        m_texture->LockSys(0, &pitch, 0);
        gOffset = (int)pitch / 2;
        if (!(10.0f <= texture->m_usage))
            texture->m_usage = 10.0f;
    }
    m_shader = (PSXSurfaceShaderInstance *)LpSurfaceShaderList->m_shaders[material->m_shader];
    return GameObject::PrepareGeometry();
}


// ---- src/PSXWater/LoadWaterFromParameterBlockFile.cpp

// Reads the water's parameter file and creates its signature waves, pools
// and surface geometry.
int PSXWater::LoadWaterFromParameterBlockFile(char *name)
{
    char shaderName[0x100];
    strcpy(m_fileName, name);
    ParameterBlock block;
    char section[0x80];
    char fileName[0x90];
    float maxOpacity;
    int lodBias;
    float widthX;
    float widthZ;
    float tileX;
    float tileZ;
    float velocityX;
    float velocityZ;
    float rotation;
    int numFrames;
    int channel;
    int physicsType;
    int tile;
    float poolX;
    float poolY;
    float poolZ;
    unsigned int i;
    if (!block.OpenFile(name, 0))
        return 0;
    block.ReadParameterBlock("General Information");
    block.GetParameter("WaterBaseHeight", 20.0f, &m_baseHeight);
    gfOriginalBaseHeight = m_baseHeight;
    block.GetParameter("WorldWidthHeightInFeet", 2304.0f, &gfBaseTextureConstant);
    block.GetParameter("WaterHeightRange", 40.0f, &gBaseScale);
    block.GetParameter("BaseOpacity", 32.0f, &m_baseOpacity);
    block.GetParameter("MaxOpacity", 64.0f, &maxOpacity);
    constants2.y = m_baseOpacity;
    constants2.z = maxOpacity - m_baseOpacity;
    block.GetParameter("ChannelScale0", 0.0f, &m_weights[0]);
    block.GetParameter("ChannelScale1", 0.0f, &m_weights[1]);
    block.GetParameter("ChannelScale2", 0.0f, &m_weights[2]);
    block.GetParameter("ChannelScale3", 0.0f, &m_weights[3]);
    block.GetParameter("FramesPerSecond0", 0.0f, &m_scrollRate[0]);
    block.GetParameter("FramesPerSecond1", 0.0f, &m_scrollRate[1]);
    block.GetParameter("FramesPerSecond2", 0.0f, &m_scrollRate[2]);
    block.GetParameter("FramesPerSecond3", 0.0f, &m_scrollRate[3]);
    block.GetParameter("FramesPerSecond4", 0.0f, &m_scrollRate[4]);
    block.GetParameter("WakeScale", 10.0f, &gWakeScale);
    block.GetParameter("NumSignatureWaves", 0, &m_numWaves);
    block.GetParameter("NumPools", 0, &m_numPools);
    block.GetParameter("EnvColorRed", 0x50, (int *)&gWaterColor1.x);
    block.GetParameter("EnvColorGreen", 0x50, (int *)&gWaterColor1.y);
    block.GetParameter("EnvColorBlue", 0x50, (int *)&gWaterColor1.z);
    block.GetParameter("FrothColorRed", 0x50, (int *)&gWaterColor2.x);
    block.GetParameter("FrothColorGreen", 0x50, (int *)&gWaterColor2.y);
    block.GetParameter("FrothColorBlue", 0x50, (int *)&gWaterColor2.z);
    block.GetParameter("ShaderName", "WaterDefault", shaderName, -1);
    block.GetParameter("PolygonScale", 0.9999f, &PolygonScale);
    block.GetParameter("AdditionalFov", 4.0f, &gGuardBandAdditionalFov);
    block.GetParameter("PoolDepth", 100.0f, &gPoolDepth);
    block.GetParameter("EnableBaseDisplacement", 1, &m_enableBaseDisplacement);
    m_colors[0] = gWaterColor1;
    m_colors[1] = gWaterColor2;
    block.GetParameter("WaterBaseSwellHeight", 0.0f, &gfWaterBaseSwellHeight);
    block.GetParameter("WaterBaseSwellPeriod", 5.0f, &gfWaterBaseSwellPeriod);
    block.GetParameter("RenderInfinitePlane", 1, &gbRenderInfinitePlane);
    block.GetParameter("LodBias", 3, &lodBias);
    giFadeAlpha[4] = giFadeAlpha[6] = giFadeAlpha[9] = giFadeAlpha[11] = lodBias;
    block.ReadParameterBlock("Wake Froth");
    block.GetParameter("MinForce", 0.01f, &gfMinForce);
    block.GetParameter("AlphaReference", 50, &gAref2);
    block.GetParameter("TexCoordScale", 0.1f, &gTexCoordScale);
    block.GetParameter("FrothScale", 0.0003f, &gfStartScale);
    block.GetParameter("FrothStartSize", 2.0f, &gfBaseLineScale);
    m_testB = ((long)gAref2 << 4) | 0x5000D;
    gBaseScale *= 16.0f;
    if (m_numWaves) {
        _D3DVECTOR position(0.0f, 0.0f, 0.0f);
        _D3DVECTOR size(0.0f, 0.0f, 1.0f);
        m_waves = new PSXSignatureWave *[m_numWaves + 1];
        for (i = 0; i < m_numWaves; i++) {
            sprintf(section, "Signature Wave %d", i + 1);
            block.ReadParameterBlock(section);
            block.GetParameter("filename", "", fileName, -1);
            block.GetParameter("positionX", 0.0f, &position.x);
            block.GetParameter("positionY", 0.0f, &position.y);
            block.GetParameter("positionZ", 0.0f, &position.z);
            block.GetParameter("widthX", 0.0f, &widthX);
            block.GetParameter("widthZ", 0.0f, &widthZ);
            block.GetParameter("numFrames", 0, &numFrames);
            block.GetParameter("velocityX", 0.0f, &velocityX);
            block.GetParameter("velocityZ", 0.0f, &velocityZ);
            block.GetParameter("rotation", 0.0f, &rotation);
            block.GetParameter("tileX", 1.0f, &tileX);
            block.GetParameter("tileZ", 1.0f, &tileZ);
            block.GetParameter("tile", 1, &tile);
            block.GetParameter("channel", 0, &channel);
            block.GetParameter("PhysicsType", 0, &physicsType);
            size.x = 0.5f * widthX;
            size.z = 0.5f * widthZ;
#line 567 "PSXWater.cpp"
            ASSERTF(channel < 5, ("ReloadWaterFromParameterBlockFile: channel number (%d) too high", channel));
            m_x70[channel]++;
            m_waves[i] = new PSXSignatureWave(position, size, numFrames, widthX, widthZ, velocityX, velocityZ,
                                              rotation, tileX, tileZ, tile, channel, physicsType);
            char wavePath[0x100];
            sprintf(wavePath, "%sSigs\\%s", WaterContentDirectory, fileName);
            m_waves[i]->loadAnimation(wavePath);
        }
        m_waves[i] = 0;
        for (unsigned int j = 0; j < i; j++) {
            PSXSignatureWave *wave = m_waves[j];
            wave->setOneOverNumWaves(1.0f / m_x70[wave->m_mode]);
        }
    }
    if (m_numPools) {
        m_objects = new SoultreeObject *[m_numPools + 1];
        for (i = 0; i < m_numPools; i++) {
            sprintf(section, "Pool %d", i + 1);
            block.ReadParameterBlock(section);
            block.GetParameter("filename", "", fileName, -1);
            block.GetParameter("positionX", 0.0f, &poolX);
            block.GetParameter("positionY", 0.0f, &poolY);
            block.GetParameter("positionZ", 0.0f, &poolZ);
            char poolPath[0x100];
            sprintf(poolPath, "%s%s", WaterContentDirectory, fileName, i + 4);
            m_objects[i] = (SoultreeObject *)(new WaterObject(1))->Create(poolPath, 1, 1);
            m_objects[i]->SetPosition(poolX, poolY, poolZ);
        }
        m_objects[i] = 0;
    }
    m_geometry = new PSXSurfaceGeometry;
    m_geometry->Create(shaderName, 0);
    m_geometry->m_packet = &m_dataTag;
    return 1;
}


// ---- src/PSXWater/ReloadWaterFromParameterBlockFile.cpp

// Re-reads the water's parameter file: general settings, wake froth, the
// signature waves and the pool positions.
int PSXWater::ReloadWaterFromParameterBlockFile()
{
    ParameterBlock block;
    char section[0x80];
    char fileName[0x90];
    float maxOpacity;
    int lodBias;
    float widthX;
    float widthZ;
    float tileX;
    float tileZ;
    float velocityX;
    float velocityZ;
    float rotation;
    int numFrames;
    int channel;
    int physicsType;
    int tile;
    float poolX;
    float poolY;
    float poolZ;
    unsigned int i;
    if (!block.OpenFile(m_fileName, 0))
        return 0;
    block.ReadParameterBlock("General Information");
    block.GetParameter("WaterBaseHeight", 20.0f, &m_baseHeight);
    gfOriginalBaseHeight = m_baseHeight;
    block.GetParameter("WorldWidthHeightInFeet", 2304.0f, &gfBaseTextureConstant);
    block.GetParameter("WaterHeightRange", 40.0f, &gBaseScale);
    block.GetParameter("BaseOpacity", 32.0f, &m_baseOpacity);
    block.GetParameter("MaxOpacity", 64.0f, &maxOpacity);
    constants2.y = m_baseOpacity;
    constants2.z = maxOpacity - m_baseOpacity;
    block.GetParameter("ChannelScale0", 0.0f, &m_weights[0]);
    block.GetParameter("ChannelScale1", 0.0f, &m_weights[1]);
    block.GetParameter("ChannelScale2", 0.0f, &m_weights[2]);
    block.GetParameter("ChannelScale3", 0.0f, &m_weights[3]);
    block.GetParameter("FramesPerSecond0", 0.0f, &m_scrollRate[0]);
    block.GetParameter("FramesPerSecond1", 0.0f, &m_scrollRate[1]);
    block.GetParameter("FramesPerSecond2", 0.0f, &m_scrollRate[2]);
    block.GetParameter("FramesPerSecond3", 0.0f, &m_scrollRate[3]);
    block.GetParameter("FramesPerSecond4", 0.0f, &m_scrollRate[4]);
    block.GetParameter("WakeScale", 10.0f, &gWakeScale);
    block.GetParameter("EnvColorRed", 0x50, (int *)&gWaterColor1.x);
    block.GetParameter("EnvColorGreen", 0x50, (int *)&gWaterColor1.y);
    block.GetParameter("EnvColorBlue", 0x50, (int *)&gWaterColor1.z);
    block.GetParameter("FrothColorRed", 0x50, (int *)&gWaterColor2.x);
    block.GetParameter("FrothColorGreen", 0x50, (int *)&gWaterColor2.y);
    block.GetParameter("FrothColorBlue", 0x50, (int *)&gWaterColor2.z);
    block.GetParameter("PolygonScale", 0.9999f, &PolygonScale);
    block.GetParameter("PolygonScale", 0.9999f, &PolygonScale);
    block.GetParameter("AdditionalFov", 4.0f, &gGuardBandAdditionalFov);
    block.GetParameter("PoolDepth", 100.0f, &gPoolDepth);
    block.GetParameter("WaterBaseSwellHeight", 0.0f, &gfWaterBaseSwellHeight);
    block.GetParameter("WaterBaseSwellPeriod", 5.0f, &gfWaterBaseSwellPeriod);
    block.GetParameter("RenderInfinitePlane", 1, &gbRenderInfinitePlane);
    block.GetParameter("EnableBaseDisplacement", 1, &m_enableBaseDisplacement);
    block.GetParameter("LodBias", 3, &lodBias);
    giFadeAlpha[4] = giFadeAlpha[6] = giFadeAlpha[9] = giFadeAlpha[11] = lodBias;
    m_colors[0] = gWaterColor1;
    m_colors[1] = gWaterColor2;
    m_constants[0] = constants1;
    m_constants[1] = constants2;
    block.ReadParameterBlock("Wake Froth");
    block.GetParameter("MinForce", 0.01f, &gfMinForce);
    block.GetParameter("AlphaReference", 50, &gAref2);
    block.GetParameter("TexCoordScale", 0.1f, &gTexCoordScale);
    block.GetParameter("FrothScale", 0.0003f, &gfStartScale);
    block.GetParameter("FrothStartSize", 2.0f, &gfBaseLineScale);
    m_testB = ((long)gAref2 << 4) | 0x5000D;
    gBaseScale *= 16.0f;
    if (m_numWaves) {
        _D3DVECTOR position(0.0f, 0.0f, 0.0f);
        _D3DVECTOR size(0.0f, 0.0f, 1.0f);
        for (i = 0; i < m_numWaves; i++) {
            sprintf(section, "Signature Wave %d", i + 1);
            block.ReadParameterBlock(section);
            block.GetParameter("filename", "", fileName, -1);
            block.GetParameter("positionX", 0.0f, &position.x);
            block.GetParameter("positionY", 0.0f, &position.y);
            block.GetParameter("positionZ", 0.0f, &position.z);
            block.GetParameter("widthX", 0.0f, &widthX);
            block.GetParameter("widthZ", 0.0f, &widthZ);
            block.GetParameter("numFrames", 0, &numFrames);
            block.GetParameter("velocityX", 0.0f, &velocityX);
            block.GetParameter("velocityZ", 0.0f, &velocityZ);
            block.GetParameter("rotation", 0.0f, &rotation);
            block.GetParameter("tileX", 1.0f, &tileX);
            block.GetParameter("tileZ", 1.0f, &tileZ);
            block.GetParameter("tile", 1, &tile);
            block.GetParameter("channel", 0, &channel);
            block.GetParameter("PhysicsType", 0, &physicsType);
            size.x = 0.5f * widthX;
            size.z = 0.5f * widthZ;
#line 755 "PSXWater.cpp"
            ASSERTF(channel < 5, ("LoadWaterFromParameterBlockFile: channel number (%d) too high", channel));
            m_waves[i]->SetParams(position, size, numFrames, widthX, widthZ, velocityX, velocityZ, rotation, tileX,
                                  tileZ, tile, channel, physicsType);
        }
        for (unsigned int j = 0; j < i; j++) {
            PSXSignatureWave *wave = m_waves[j];
            wave->setOneOverNumWaves(1.0f / m_x70[wave->m_mode]);
        }
    }
    if (m_numPools) {
        for (i = 0; i < m_numPools; i++) {
            sprintf(section, "Pool %d", i + 1);
            block.ReadParameterBlock(section);
            block.GetParameter("positionX", 0.0f, &poolX);
            block.GetParameter("positionY", 0.0f, &poolY);
            block.GetParameter("positionZ", 0.0f, &poolZ);
            m_objects[i]->SetPosition(poolX, poolY, poolZ);
        }
    }
    return 1;
}


// ---- src/PSXWater/LoadWaterFromBinaryFile.cpp

int PSXWater::LoadWaterFromBinaryFile(char *filename)
{
    return 0;
}

