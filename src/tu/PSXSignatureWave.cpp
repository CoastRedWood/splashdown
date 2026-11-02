// PSXSignatureWave.cpp  @ 0x002F4C80 (text 0x2F4C80-0x2F6760)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's PSXSignatureWave.cpp as one translation unit: its functions in
// retail order, then its static initializer. `make link` places the object's
// data as well as its code (tools/link_c.py).



#include "d3dtypes.h"
#include "sceGifTag/sceGifTag.h"
#include "PSXSignatureWave/PSXSignatureWave.h"
#include "PSXTextureMap/PSXTextureMap.h"
#include "soultree_assert_block.h"
#include "_sceDmaTag/_sceDmaTag.h"
#include "DmaRenderItem/DmaRenderItem.h"
#include "PSXRenderStateBlock/PSXRenderStateBlock.h"
#include "PSXRenderTarget/PSXRenderTarget.h"
#include "Camera/Camera.h"
#include "PSXVideoCard/PSXVideoCard.h"
#include "PSXWaterDisplacement/PSXWaterDisplacement.h"
#include "WaterObject/WaterObject.h"
#include "PSXTextureMapManager/PSXTextureMapManager.h"
#include "PSXCamera/PSXCamera.h"
#include "sinit/AxisVectors.h"

typedef int int128_t __attribute__((mode(TI)));
typedef unsigned long u_long;   // 64-bit under mwccps2
typedef unsigned char u_char;

class Palette8;

struct _D3DINTVECTOR {
    int x, y, z, w;
} __attribute__((aligned(16)));

extern "C" void *memalign(unsigned int alignment, unsigned int size);
extern "C" void free(void *p);
extern "C" int sprintf(char *buf, const char *fmt, ...);
extern "C" double fmod(double x, double y);

extern int gRenderFroth;
extern int gConvertNativeTo32Bit;
unsigned char gbOffsetToWorld = 1;
extern int giFadeAlpha[][4];
extern PSXVideoCard *LpVideoCard;
extern PSXTextureMapManager *LpTextureMapManager;

// Per-mode wave colours and viewports.
extern _D3DINTVECTOR colorRed;
extern _D3DINTVECTOR colorGreen;
extern _D3DINTVECTOR colorBlue;
int gpiChannelViewport1[] = {0, 1, 1, 2, 2};
int gpiChannelViewport2[] = {0, 1, 2, 2, 2};
_D3DINTVECTOR *gpChannelColor1[] = {&colorGreen, &colorRed, &colorBlue, &colorRed, &colorGreen};
_D3DINTVECTOR *gpChannelColor2[] = {&colorBlue, &colorGreen, &colorRed, &colorBlue, &colorBlue};
int giFadeAlpha[][4] = {
    {0x40, 0x40, 0x40, 0x40},
    {0, 0x40, 0, 0x40},
    {0x40, 0, 0x40, 0},
};


// ---- src/PSXSignatureWave/ctor.cpp


// Render-state packet: a GIF tag and eight A+D register writes.
PSXSignatureWave::PSXSignatureWave(_D3DVECTOR &position, _D3DVECTOR &direction, unsigned int numWaves, float width,
                                   float length, float widthSpeed, float lengthSpeed, float unused, float xC8,
                                   float xCC, int xD4, int mode, int x110)
{
    m_maps = 0;
    m_surface = 0;
    m_nextSurface = 0;
    m_statePackets = 0;
    m_geometryPackets = 0;
    if (!gRenderFroth && mode == 4) {
        m_mode = mode;
        return;
    }
    m_gifTag.EOP = 1;
    m_gifTag.PRE = 1;
    m_gifTag.FLG = 0;
    m_gifTag.NREG = 3;
    m_gifTag.REGS0 = 2;
    m_gifTag.REGS1 = 1;
    m_gifTag.REGS2 = 4;
    m_gifTag.NLOOP = 4;
    m_gifTag.PRIM = 0x5C;
    m_numSegments = 1;
    m_bytesPerPixel = 1;
    m_pitch = 0;
    SetParams(position, direction, numWaves, width, length, widthSpeed, lengthSpeed, unused, xC8, xCC, xD4, mode,
              x110);
    unsigned int i;
    m_statePackets = (WaveStatePacket *)memalign(0x80, 6 * sizeof(WaveStatePacket));
    for (i = 0; i < 6; i++) {
        WaveStatePacket *p = &m_statePackets[i];
        p->m_vif[0] = 0x11000000;       // FLUSH
        p->m_vif[1] = 0x03000000;       // BASE
        p->m_vif[2] = 0x02000000;       // OFFSET
        p->m_vif[3] = 0x6C09C320;       // UNPACK
        *(int128_t *)&p->m_gifTag = 0;
        p->m_gifTag.FLG = 0;
        p->m_gifTag.EOP = 1;
        p->m_gifTag.NREG = 8;
        p->m_gifTag.NLOOP = 1;
        p->m_gifTag.REGS0 = p->m_gifTag.REGS1 = p->m_gifTag.REGS2 = p->m_gifTag.REGS3 = p->m_gifTag.REGS4 =
            p->m_gifTag.REGS5 = p->m_gifTag.REGS6 = p->m_gifTag.REGS7 = 0xE;   // A+D
        p->m_ad[1] = 0x06;              // TEX0_1
        p->m_ad[11] = 0x4E;             // ZBUF_1
        p->m_ad[3] = 0x3F;              // TEXFLUSH
        p->m_ad[2] = 0;
        p->m_ad[13] = 0x08;             // CLAMP_1
        if (xD4)
            p->m_ad[12] = 0;
        else
            p->m_ad[12] = 5;
        p->m_ad[9] = 0x47;              // TEST_1
        p->m_ad[8] = 0x3000C;
        p->m_ad[5] = 0x14;              // TEX1_1
        p->m_ad[4] = 0xFFFFFFB500000060;
        p->m_ad[7] = 0x3B;              // TEXA
        p->m_ad[6] = 0x7F8000;
        p->m_ad[15] = 0x42;             // ALPHA_1
        p->m_ad[14] = 0x8000000048;
        p->m_vifEnd[0] = 0x14000000;    // MSCAL
        p->m_vifEnd[1] = 0x10000000;    // FLUSHE
        p->m_vifEnd[3] = 0;
        p->m_vifEnd[2] = 0;
    }
    m_geometryPackets = (WaveGeometryPacket *)memalign(0x80, 6 * sizeof(WaveGeometryPacket));
    for (i = 0; i < 6; i++) {
        WaveGeometryPacket *p = &m_geometryPackets[i];
        p->m_vif[0] = 0x11000000;       // FLUSH
        p->m_vif[1] = 0x03000000;       // BASE
        p->m_vif[2] = 0x02000000;       // OFFSET
        p->m_vif[3] = 0x6C0DC320;       // UNPACK
        *(int128_t *)&p->m_gifTag = 0;
        p->m_gifTag.EOP = 1;
        p->m_gifTag.PRE = 1;
        p->m_gifTag.FLG = 0;
        p->m_gifTag.NREG = 3;
        p->m_gifTag.REGS0 = 2;
        p->m_gifTag.REGS1 = 1;
        p->m_gifTag.REGS2 = 4;
        p->m_gifTag.NLOOP = 4;
        p->m_gifTag.PRIM = 0x5C;
        p->m_vifEnd[0] = 0x14000000;    // MSCAL
        p->m_vifEnd[1] = 0x10000000;    // FLUSHE
        p->m_vifEnd[3] = 0;
        p->m_vifEnd[2] = 0;
    }
}


// ---- src/PSXSignatureWave/dtor.cpp

PSXSignatureWave::~PSXSignatureWave()
{
    if (m_maps) {
        for (unsigned int i = 0; i < m_numWaves; i++) {
            if (m_maps[i])
                m_maps[i]->Release();
        }
        delete[] m_maps;
    }
    if (m_statePackets) {
        free(m_statePackets);
        m_statePackets = 0;
    }
    if (m_geometryPackets) {
        free(m_geometryPackets);
        m_geometryPackets = 0;
    }
}


// ---- src/PSXSignatureWave/setOneOverNumWaves.cpp
#pragma fast_fptosi on


void PSXSignatureWave::setOneOverNumWaves(float oneOverNumWaves)
{
    m_oneOverNumWaves = oneOverNumWaves;
    if (m_mode == 4) {
        if (gRenderFroth) {
            for (unsigned int i = 0; i < 6; i++)
                m_statePackets[i].m_ad[14] = ((u_long)0xFF << 32) | 0x68;
        }
    } else {
        int alpha = (int)(128.0f * m_oneOverNumWaves);
        for (unsigned int i = 0; i < 6; i++)
            m_statePackets[i].m_ad[14] = ((u_long)alpha << 32) | 0x48;
    }
}


// ---- src/PSXSignatureWave/SetParams.cpp
#pragma fast_fptosi reset

void PSXSignatureWave::SetParams(_D3DVECTOR &position, _D3DVECTOR &direction, unsigned int numWaves, float width,
                                 float length, float widthSpeed, float lengthSpeed, float, float xC8, float xCC,
                                 int xD4, int mode, int x110)
{
    m_position = position;
    m_direction = direction;
    m_numWaves = numWaves;
    m_width = width;
    m_length = length;
    m_oneOverWidth = 1.0f / width;
    m_oneOverLength = 1.0f / length;
    m_xB0 = 0;
    m_xB4 = 0;
    m_xC0 = 0;
    m_xC4 = 0;
    m_xB8 = widthSpeed * m_width / m_numSegments;
    m_xBC = lengthSpeed * m_length / m_numSegments;
    m_xC8 = xC8;
    m_xCC = xCC;
    m_xD4 = xD4;
    m_mode = mode;
    m_scale.x = 256.0f / m_width;
    m_scale.y = -256.0f / m_length;
    m_scale.z = m_scale.w = 1.0f;
    m_xD0 = m_xD4 ? 0 : 1;
    m_x110 = x110;
    m_currentWave = 0;
}


// ---- src/PSXSignatureWave/loadAnimation.cpp


PSXTextureMap *GetTextureMapFromResourceName(char *name, int flags, Palette8 *palette, unsigned int a,
                                             _D3DBLEND src, _D3DBLEND dst, int alpha, unsigned int colorKey, int b);
int BytesPerPixel(int format);


int PSXSignatureWave::loadAnimation(char *name)
{
    char frameName[0x88];
    long pitch;
    if (!gRenderFroth && m_mode == 4)
        return 1;
    int convert = gConvertNativeTo32Bit;
    gConvertNativeTo32Bit = 0;
    m_maps = new PSXTextureMap *[m_numWaves];
    for (unsigned int i = 0; i < m_numWaves; i++) {
        m_maps[i] = 0;
        sprintf(frameName, "%s%04d", name, i);
        m_maps[i] = GetTextureMapFromResourceName(frameName, 0x22B8, 0, 0, (_D3DBLEND)5, (_D3DBLEND)6, 0x80,
                                                    0xFF00FF, 1);
    }
    void *bits = m_maps[0]->LockSys(0, &pitch, 0);
    m_bytesPerPixel = BytesPerPixel(m_maps[0]->m_format);
    m_numSegments = pitch / m_bytesPerPixel;
    m_xB8 /= m_numSegments;
    m_xBC /= m_numSegments;
    m_surface = m_nextSurface = bits;
    m_maps[0]->UnlockSys(0);
    gConvertNativeTo32Bit = convert;
    return 1;
}


// ---- src/PSXSignatureWave/addVerticesToRenderItem.cpp
#pragma fast_fptosi on


#define SCE_GS_SET_TEX0(tbp, tbw, psm, tw, th, tcc, tfx, cbp, cpsm, csm, csa, cld) \
    ((u_long)(tbp) | ((u_long)(tbw) << 14) | ((u_long)(psm) << 20) | ((u_long)(tw) << 26) | \
     ((u_long)(th) << 30) | ((u_long)(tcc) << 34) | ((u_long)(tfx) << 35) | ((u_long)(cbp) << 37) | \
     ((u_long)(cpsm) << 51) | ((u_long)(csm) << 55) | ((u_long)(csa) << 56) | ((u_long)(cld) << 61))


static inline void SetDmaTag(_sceDmaTag *tag, int qwc, int id, _sceDmaTag *next)
{
    tag->qwc = qwc;
    tag->mark = 0;
    tag->id = id;
    tag->next = next;
    tag->p[0] = 0;
    tag->p[1] = 0;
}


// One vertex of a wave quad in the geometry packet: 0x30 bytes from +0x20.
// A colour as a quadword or as its four components.
union WaveColor {
    int128_t q;
    _D3DINTVECTOR v;
};

struct WaveVertex {
    float m_uv[4];                      // +0x00, s, t, q
    WaveColor m_color;                  // +0x10
    int m_xyz[4];                       // +0x20
};

// WaveGeometryPacket (PSXSignatureWave.h) with its four vertices.
struct WaveQuadPacket {                 // 0xF0 bytes
    unsigned int m_vif[4];              // +0x00
    sceGifTag m_gifTag;                 // +0x10
    WaveVertex m_verts[4];              // +0x20
    unsigned int m_vifEnd[4];           // +0xE0
};


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

// libvu0 sceVu0ApplyMatrix: out = m * v (xyzw).
static inline void ApplyMatrix(_D3DVECTOR *out, _D3DMATRIX *m, _D3DVECTOR *v)
{
    asm("lqc2 vf4, 0x0(%1); lqc2 vf5, 0x10(%1); lqc2 vf6, 0x20(%1); lqc2 vf7, 0x30(%1); lqc2 vf8, 0x0(%2); vmulax.xyzw ACC, vf4, vf8x; vmadday.xyzw ACC, vf5, vf8y; vmaddaz.xyzw ACC, vf6, vf8z; vmaddw.xyzw vf9, vf7, vf8w; sqc2 vf9, 0x0(%0)"
        : : "r"(out), "r"(m), "r"(v) : "memory");
}

// PSXRenderStateBlock::SetTextureMap's TEX0 setup, written into a packet,
// skipped for froth when froth is off. An inline member, so its statics
// are $localstatics shared across TUs, as in retail.
inline void PSXSignatureWave::SetTextureMap(PSXTextureMap *texture, unsigned long *tex0)
{
    if (!gRenderFroth && m_mode == 4)
        return;
    PSXMipSurface *mip;
    u_char psm;
    short cbp;
    u_char cpsm;
    u_char csm;
    u_char csa;
    u_char cld;
#line 467 "PSXSignatureWave.cpp"
    ASSERT(texture, "SetTextureMap: lpTextureMap == NULL");
    LpTextureMapManager->AddTextureToGifList(texture);
    LpTextureMapManager->AddTextureInterruptToRenderQueue();
    mip = texture->GetReqdMipLevel();
    csm = 0;
    csa = 0;
    switch (texture->m_format) {
    case 0x22B8:
    case 0x115C:
        psm = 0;
        cbp = 0;
        cpsm = cld = 0;
        break;
    case 0x613:
    case 0x22B:
    case 0x235:
        psm = 2;
        cbp = 0;
        cpsm = cld = 0;
        break;
    case 8:
        psm = 0x13;
        cbp = *(short *)&texture->m_xF0;
        cpsm = 0;
        cld = 1;
        break;
    case 4:
        psm = 0x14;
        cbp = *(short *)&texture->m_xF0;
        cpsm = 0;
        cld = 1;
        break;
    default:
        PSXprintf("Invalid pixel format (%d) in PSXRenderStateBlock::SetTextureFunction()\n", texture->m_format);
#line 517 "PSXSignatureWave.cpp"
        ASSERTF(0, ("Invalid pixel format (%d) in PSXRenderStateBlock::SetTextureFunction()", texture->m_format));
    }
    *tex0 = SCE_GS_SET_TEX0(texture->m_xEC, mip->m_blocks, psm, mip->m_widthLog2, mip->m_heightLog2, 1, 0, cbp, cpsm, csm, csa, cld);
}

void PSXSignatureWave::addVerticesToRenderItem(PSXWaterDisplacement *displacement, _D3DINTVECTOR *color,
                                               PSXTextureMap *map, int viewport, int pass)
{
    _D3DMATRIX *matrix = &((PSXCamera *)LpRenderTarget->m_camera)->m_x250;
    displacement->SetViewport(viewport);
    sceGifTag *tag = &m_geometryPackets[m_x114].m_gifTag;
    WaveColor rgba = *(WaveColor *)color;
    if (map) {
        SetTextureMap(map, &m_statePackets[m_x114].m_ad[0]);
        tag->PRIM = 0x5C;
    } else {
        tag->PRIM = 0x4C;
        rgba.v.x <<= 1;
        rgba.v.y <<= 1;
        rgba.v.z <<= 1;
    }
    m_statePackets[m_x114].m_ad[10] = (long)LpVideoCard->GetZBufferAddress() | 0x31000000;
    _sceDmaTag stateTag;
    SetDmaTag(&stateTag, 0xB, 0x30, (_sceDmaTag *)&m_statePackets[m_x114]);
    ((PSXRenderTarget *)LpRenderTarget)->AddToRenderQueue(&stateTag, 1, "");
    _D3DVECTOR offset;
    if (m_xD4) {
        offset.x = (float)(int)(m_oneOverWidth * (m_corners[1]->x - m_xC0));
        offset.y = (float)(int)(m_oneOverLength * (m_corners[1]->z - m_xC4));
    }
    _D3DVECTOR p;
    float *uv;
    unsigned int i;
    WaveQuadPacket *packet;
    _D3DVECTOR *corner;
    for (i = 0; i < 4; i++) {
        packet = (WaveQuadPacket *)&m_geometryPackets[m_x114];
        uv = packet->m_verts[i].m_uv;
        corner = m_corners[i];
        ApplyMatrix(&p, matrix, corner);
        p.z = Fmin(p.z, 16776960.0f);
        p.z = Fmax(p.z, 0.0f);
        p.w = Fmax(p.w, 1.0f);
        float q = 1.0f / p.w;
        float scale = 16.0f * q;
        packet->m_verts[i].m_xyz[0] = p.x * scale;
        packet->m_verts[i].m_xyz[1] = p.y * scale;
        packet->m_verts[i].m_xyz[2] = p.z * scale;
        packet->m_verts[i].m_xyz[3] = 0;
        packet->m_verts[i].m_uv[0] = m_oneOverWidth * (corner->x - m_xC0);
        packet->m_verts[i].m_uv[1] = m_oneOverLength * (corner->z - m_xC4);
        packet->m_verts[i].m_uv[2] = q;
        if (m_xD4) {
            uv[0] -= offset.x;
            uv[1] -= offset.y;
        }
        uv[0] *= q;
        uv[1] *= q;
        rgba.v.w = m_oneOverNumWaves * giFadeAlpha[pass][i];
        packet->m_verts[i].m_color.q = rgba.q;
    }
    _sceDmaTag geometryTag;
    SetDmaTag(&geometryTag, 0xF, 0x30, (_sceDmaTag *)&m_geometryPackets[m_x114]);
    ((PSXRenderTarget *)LpRenderTarget)->AddToRenderQueue(&geometryTag, 1, "");
    m_x114++;
}


// ---- src/PSXSignatureWave/BuildRenderItem.cpp
#pragma fast_fptosi reset


void PSXSignatureWave::BuildRenderItem(PSXWaterDisplacement *displacement)
{
    if (!gRenderFroth && m_mode == 4)
        return;
    static int PSXSignatureWaveBuildRenderItemID = -1;     // profiler ID, unused
    unsigned int next = m_currentWave + 1;
    if (!(next < m_numWaves))
        next = 0;
    m_xC0 = m_position.x + m_xB0;
    m_xC4 = m_position.z + m_xB4;
    m_x114 = 0;
    if (displacement->m_x5B4 && displacement->m_x5B8 && m_mode != 4) {
        m_corners[0] = &displacement->m_corners[0];
        m_corners[1] = &displacement->m_corners[5];
        m_corners[2] = &displacement->m_corners[2];
        m_corners[3] = &displacement->m_corners[4];
        addVerticesToRenderItem(displacement, gpChannelColor1[m_mode], 0, gpiChannelViewport1[m_mode], 2);
        if (m_mode != 2 && m_mode != 3)
            addVerticesToRenderItem(displacement, gpChannelColor2[m_mode], 0, gpiChannelViewport2[m_mode], 2);
        addVerticesToRenderItem(displacement, gpChannelColor1[m_mode], m_maps[m_currentWave], gpiChannelViewport1[m_mode], 1);
        if (m_mode != 2 && m_mode != 3)
            addVerticesToRenderItem(displacement, gpChannelColor2[m_mode], m_maps[next], gpiChannelViewport2[m_mode], 1);
        m_corners[0] = &displacement->m_corners[5];
        m_corners[1] = &displacement->m_corners[1];
        m_corners[2] = &displacement->m_corners[4];
        m_corners[3] = &displacement->m_corners[3];
        addVerticesToRenderItem(displacement, gpChannelColor1[m_mode], m_maps[m_currentWave], gpiChannelViewport1[m_mode], 0);
        if (m_mode != 2 && m_mode != 3)
            addVerticesToRenderItem(displacement, gpChannelColor2[m_mode], m_maps[next], gpiChannelViewport2[m_mode], 0);
    } else {
        m_corners[0] = &displacement->m_corners[0];
        m_corners[1] = &displacement->m_corners[1];
        m_corners[2] = &displacement->m_corners[2];
        m_corners[3] = &displacement->m_corners[3];
        addVerticesToRenderItem(displacement, gpChannelColor1[m_mode], m_maps[m_currentWave], gpiChannelViewport1[m_mode], 0);
        if (m_mode != 2 && m_mode != 3)
            addVerticesToRenderItem(displacement, gpChannelColor2[m_mode], m_maps[next], gpiChannelViewport2[m_mode], 0);
    }
}


// ---- src/PSXSignatureWave/Tick.cpp


// Advances the animated wave surfaces and scrolls the wave offsets.
int PSXSignatureWave::Tick(float dt, unsigned int *frames)
{
    if (!gRenderFroth && m_mode == 4)
        return 1;
    m_currentWave = frames[m_mode] % m_numWaves;
    PSXTextureMap *map = m_maps[m_currentWave];
    long pitch;
    m_surface = map->LockSys(0, &pitch, 0);
    m_pitch = pitch;
#line 806 "PSXSignatureWave.cpp"
    ASSERT(m_surface != 0, "Signature wave Surface NULL");
    map->UnlockSys(0);
    unsigned int next = m_currentWave + 1;
    if (!(next < m_numWaves))
        next = 0;
    map = m_maps[next];
    m_nextSurface = map->LockSys(0, 0, 0);
    map->UnlockSys(0);
    m_xB0 += m_xB8 * dt;
    m_xB0 = fmod(m_xB0, m_width);
    m_xB4 += m_xBC * dt;
    m_xB4 = fmod(m_xB4, m_length);
    return 1;
}


// ---- src/PSXSignatureWave/RenderWaterObjects.cpp


// The texture coordinate offset, set by the static initializer (with the
// axis vectors); its .bss precedes RenderWaterObjects' static in retail.
_D3DVECTOR lTexCoordOffset(0.0f, 0.0f, 0.0f, 0.0f);


void PSXSignatureWave::RenderWaterObjects(PSXWaterDisplacement *displacement)
{
    if (!gRenderFroth && m_mode == 4)
        return;
    static int SignatureWaveRenderWaterObjectsID = -1;     // profiler ID, unused
    m_wave = m_currentWave;
    m_nextWave = m_wave + 1;
    if (!(m_nextWave < m_numWaves))
        m_nextWave = 0;
    m_oneOverWidth = 1.0f / m_width;
    m_oneOverLength = 1.0f / m_length;
    lTexCoordOffset.x = 0.5f * m_position.x + m_xB0;
    lTexCoordOffset.y = 0.5f * m_position.z + m_xB4;
    int viewport;
    int mode;
    WaterObject **object;
    float alpha;
    _D3DINTVECTOR *color;
    PSXTextureMap *map;
    viewport = gpiChannelViewport1[m_mode];
    displacement->SetViewport(viewport);
    mode = (&displacement->m_x374)[viewport] ? 5 : 1;
    (&displacement->m_x374)[viewport] = 0;
    color = gpChannelColor1[m_mode];
    map = m_maps[m_wave];
    object = displacement->m_waterObjects;
    if (m_mode == 4)
        alpha = 1.0f;
    else
        alpha = m_oneOverNumWaves;
    for (; *object; object++)
        (*object)->Render3D(mode, color, map, &m_scale, &lTexCoordOffset, m_oneOverWidth, m_oneOverLength, gbOffsetToWorld,
                            m_xD4, alpha, &displacement->m_corners[1]);
    if (m_mode != 2 && m_mode != 3) {
        viewport = gpiChannelViewport2[m_mode];
        displacement->SetViewport(viewport);
        mode = (&displacement->m_x374)[viewport] ? 5 : 1;
        (&displacement->m_x374)[viewport] = 0;
        color = gpChannelColor2[m_mode];
        map = m_maps[m_nextWave];
        for (object = displacement->m_waterObjects; *object; object++)
            (*object)->Render3D(mode, color, map, &m_scale, &lTexCoordOffset, m_oneOverWidth, m_oneOverLength,
                                gbOffsetToWorld, m_xD4, alpha, &displacement->m_corners[1]);
    }
}
