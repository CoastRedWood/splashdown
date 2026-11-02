// Particles.cpp  @ 0x00262670 (text 0x262670-0x264930)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's Particles.cpp as one translation unit, an unanchored TU in
// NormalDistribution.cpp's tu_map row: ParticleManager's ten functions, then
// the static initializer (__sinit_Particles.cpp, the .ctor slot after
// NormalDistribution.cpp's: the axis vectors only). Its .rodata
// (0x416290-0x416440) follows Parameterblocks.cpp's strings: "Particle
// Manager", the .prt parameter names, then the ParticleManager/BaseObject/
// GameObject RTTI records. `make link` places the object's data as well as its
// code (tools/link_c.py).


// Tick's and PrepareGeometry's per-function files set it (Vu0OrientMatrix,
// vector operators); TU-wide (read at the end of the TU).
#pragma inline_depth(4)

#include "d3dtypes.h"
#include "GameObject/GameObject.h"
#include "ParticleManager/ParticleManager.h"
#include "Particle/Particle.h"
#include "ParticleVUPhysics/ParticleVUPhysics.h"
#include "ParameterBlock/ParameterBlock.h"
#include "DmaRenderItem/DmaRenderItem.h"
#include "Game/Game.h"
#include "Water/Water.h"
#include "DebugOverlay/DebugOverlay.h"
#include "TextureMap/TextureMap.h"
#include "PSXTextureMap/PSXTextureMap.h"
#include "PSXTextureMapManager/PSXTextureMapManager.h"
#include "PSXRenderStateBlock/PSXRenderStateBlock.h"
#include "RenderTarget/RenderTarget.h"
#include "PSXRenderTarget/PSXRenderTarget.h"
#include "Camera/Camera.h"
#include "PSXCamera/PSXCamera.h"
#include "LightManager/LightManager.h"
#include "TriggerObject/Vu0Orient.h"
#include "sceGifTag/sceGifTag.h"

#include "sinit/AxisVectors.h"



extern "C" char *strcpy(char *dst, const char *src);
extern "C" char *strcat(char *dst, const char *src);
extern "C" int sprintf(char *buf, const char *fmt, ...);
void PSXprintf(const char *fmt, ...);

class Palette8;
PSXTextureMap *GetTextureMapFromResourceName(char *name, int flags, Palette8 *palette, unsigned int a,
                                             _D3DBLEND src, _D3DBLEND dst, int alpha, unsigned int colorKey, int b);

extern ParticleManager *LpParticleManager;
extern PSXTextureMapManager *LpTextureMapManager;
extern Game *LpGame;
extern Water *GameWater;
extern char ParticleTextureContentDirectory[];
extern int TotalBins;
extern int TimeGPU;
extern unsigned int ParticleCode[];    // VU1 microcode

typedef int int128_t __attribute__((mode(TI)));

// A frame's texture coordinates, four corners (m_frames).
struct ParticleFrame {
    float m_uv[8];
};

// A particle type's frame range (m_groups, from the .prt file).
struct ParticleGroup {
    int m_type;                         // +0x0
    int m_firstFrame;                   // +0x4
    int m_lastFrame;                    // +0x8
};

// A depth bin (m_lists): its particles, front to back, as PrepareGeometry
// last filled it in frame m_frame (m_114).
struct ParticleList {
    unsigned int m_frame;               // +0x0
    Particle *m_first;                  // +0x4
};

// ---- src/ParticleManager/ctor.cpp

ParticleManager::ParticleManager(int enabled) : GameObject(enabled)
{
    m_numParticles = 0;
    m_12C = 0;
    m_maxParticles = 0;
    m_gravity.x = 0.0f;
    m_gravity.y = -64.0f;
    m_gravity.z = 0.0f;
    m_frames = 0;
    m_texture = 0;
    m_particles = 0;
    m_groups = 0;
    m_numGroups = 0;
    m_144 = 1;
    m_vuPhysics = 0;
    m_114 = 0;
    m_118 = 0;
    m_lists = 0;
    m_glowScalar = 255.0f;
    LpParticleManager = this;
}

// ---- src/ParticleManager/dtor.cpp

ParticleManager::~ParticleManager()
{
    if (m_texture)
        m_texture->Release();
    if (m_particles) {
        for (int i = 0; i < m_maxParticles; i++) {
            if (m_particles[i])
                delete m_particles[i];
        }
        delete[] m_particles;
    }
    if (m_frames)
        delete[] m_frames;
    if (m_groups)
        delete[] m_groups;
    if (m_lists)
        delete[] m_lists;
    if (m_vuPhysics)
        m_vuPhysics->Release();
}

// ---- src/ParticleManager/Create.cpp

// libvu0 sceVu0Normalize, of a temporary.
static inline void Normalize(_D3DVECTOR *out, const _D3DVECTOR &in)
{
    asm("lqc2 vf4, 0x0(%1); vmul.xyz vf5, vf4, vf4; vaddy.x vf5, vf5, vf5y; vaddz.x vf5, vf5, vf5z; vrsqrt Q, vf0w, vf5x; vsub.xyzw vf6, vf0, vf0; vaddw.xyzw vf6, vf6, vf4w; vwaitq; vmulq.xyz vf6, vf4, Q; sqc2 vf6, 0x0(%0)"
        : : "r"(out), "r"(&in) : "memory");
}

// Sets up the bins, the particle pool and, when there is no descriptor file,
// a default frame layout over one texture: 14 quarter-size frames, then
// 4 eighth-size and 32 sixteenth-size ones (18..28 stay unset), then the
// quad's corners and its four lighting normals.
ParticleManager *ParticleManager::Create(char *name, int count)
{
    int i;
    GameObject::Create();
    m_vuPhysics = new ParticleVUPhysics();
    m_maxParticles = count;
    m_118 = count / 2;
    m_lists = new ParticleList[m_118];
    TotalBins = m_118;
    for (i = 0; i < m_118; i++) {
        m_lists[i].m_frame = 0;
        m_lists[i].m_first = 0;
    }
    m_particles = new Particle *[m_maxParticles];
    for (i = 0; i < m_maxParticles; i++)
        m_particles[i] = new Particle;
    if (!LoadFromDescriptorFile(name)) {
        char path[0x104];
        m_frames = new ParticleFrame[61];
        for (i = 0; i < 14; i++) {
            int row = i / 4;
            int column = i - row * 4;
            float v = 0.25f * row;
            float u = 0.25f * column;
            m_frames[i].m_uv[0] = u;
            m_frames[i].m_uv[1] = v;
            m_frames[i].m_uv[2] = 0.25f + u;
            m_frames[i].m_uv[3] = v;
            m_frames[i].m_uv[4] = 0.25f + u;
            m_frames[i].m_uv[5] = 0.25f + v;
            m_frames[i].m_uv[6] = u;
            m_frames[i].m_uv[7] = 0.25f + v;
        }
        for (i = 14; i < 18; i++) {
            int row = (i - 14) / 2;
            int column = (i - 14) - row * 2;
            float v = 0.75f + 0.25f * row / 2.0f;
            float u = 0.25f + 0.25f * column / 2.0f;
            m_frames[i].m_uv[0] = u;
            m_frames[i].m_uv[1] = v;
            m_frames[i].m_uv[2] = 0.125f + u;
            m_frames[i].m_uv[3] = v;
            m_frames[i].m_uv[4] = 0.125f + u;
            m_frames[i].m_uv[5] = 0.125f + v;
            m_frames[i].m_uv[6] = u;
            m_frames[i].m_uv[7] = 0.125f + v;
        }
        for (i = 29; i < 45; i++) {
            int row = (i - 29) / 4;
            int column = (i - 29) - row * 4;
            float v = 0.75f + 0.25f * row / 4.0f;
            float u = 0.5f + 0.25f * column / 4.0f;
            m_frames[i].m_uv[0] = u;
            m_frames[i].m_uv[1] = v;
            m_frames[i].m_uv[2] = 0.0625f + u;
            m_frames[i].m_uv[3] = v;
            m_frames[i].m_uv[4] = 0.0625f + u;
            m_frames[i].m_uv[5] = 0.0625f + v;
            m_frames[i].m_uv[6] = u;
            m_frames[i].m_uv[7] = 0.0625f + v;
        }
        for (i = 45; i < 61; i++) {
            int row = (i - 45) / 4;
            int column = (i - 45) - row * 4;
            float v = 0.75f + 0.25f * row / 4.0f;
            float u = 0.75f + 0.25f * column / 4.0f;
            m_frames[i].m_uv[0] = u;
            m_frames[i].m_uv[1] = v;
            m_frames[i].m_uv[2] = 0.0625f + u;
            m_frames[i].m_uv[3] = v;
            m_frames[i].m_uv[4] = 0.0625f + u;
            m_frames[i].m_uv[5] = 0.0625f + v;
            m_frames[i].m_uv[6] = u;
            m_frames[i].m_uv[7] = 0.0625f + v;
        }
        strcpy(path, ParticleTextureContentDirectory);
        strcat(path, name);
        m_texture = GetTextureMapFromResourceName(path, 0x115C, 0, 0, (_D3DBLEND)5, (_D3DBLEND)6, 0x80, 0xFF00FF, 1);
    }
    m_144 = 1;
    m_vertices[0].x = -1.0f;
    m_vertices[0].y = 1.0f;
    m_vertices[0].z = 0.0f;
    m_vertices[1].x = -1.0f;
    m_vertices[1].y = -1.0f;
    m_vertices[1].z = 0.0f;
    m_vertices[2].x = 1.0f;
    m_vertices[2].y = 1.0f;
    m_vertices[2].z = 0.0f;
    m_vertices[3].x = 1.0f;
    m_vertices[3].y = -1.0f;
    m_vertices[3].z = 0.0f;
    Normalize(&m_50[0], _D3DVECTOR(1.0f, 1.0f, 0.25f));
    Normalize(&m_50[1], _D3DVECTOR(1.0f, -1.0f, 0.25f));
    Normalize(&m_50[2], _D3DVECTOR(-1.0f, 1.0f, 0.25f));
    Normalize(&m_50[3], _D3DVECTOR(-1.0f, -1.0f, 0.25f));
    return this;
}

// ---- src/ParticleManager/Tick.cpp
#pragma fast_fptosi on

int g_ParticleDBGID = -1;
int TotalBins = 500;

inline void TickParticle(ParticleManager *manager, Particle *p, float dt)
{
    unsigned int flags = p->m_flags & 0x7C;
    if (flags & 8) {
        flags &= ~8;
        p->m_size += p->m_rise * dt;
        p->m_position.y += p->m_rise * dt;
    }
    if (flags && (flags & 4)) {
        flags &= ~4;
        p->m_position += dt * p->m_velocity;
    }
    if (flags && (flags & 0x10)) {
        flags &= ~0x10;
        p->m_velocity += dt * manager->m_gravity;
    }
    if (flags && (flags & 0x20)) {
        if (p->m_velocity.y > 0.0f)
            p->m_velocity.y -= 1.0f;
        flags &= ~0x20;
    }
    static float minHeight = GameWater->m_baseHeight;
    if (flags) {
        if (flags & 0x40) {
            p->m_frame = (int)(p->m_age / ((2.0f + p->m_lifetime) / 14.0f));
            if (p->m_position.y < minHeight && p->m_velocity.y <= 0.0f) {
                p->m_velocity = ZeroVector;
                p->m_position.y = GameWater->m_baseHeight;
                p->m_flags &= ~0x74;
                static float hangAroundTime = 500.0f;
                static float oneOverHangAroundTime = 1.0f / hangAroundTime;
                p->m_flags |= 2;
                if (p->m_lifetime - p->m_age < hangAroundTime) {
                    p->m_age = 0.0f;
                    p->m_lifetime = hangAroundTime;
                    p->m_oneOverLifetime = oneOverHangAroundTime;
                }
            }
        }
    }
}

// Defined after TickParticle, whose statics precede it in retail's .bss
//.
ParticleManager *LpParticleManager;

// Ages every particle, retiring the expired ones (swapped to the end of the
// list), then moves the rest.
int ParticleManager::Tick(float dt)
{
    if (LpGame->m_paused)
        return 1;
    // Retail treats +0x1E0 as a GameObject here (the ParticleVUPhysics is a BaseObject).
    if (m_vuPhysics && ((GameObject *)m_vuPhysics)->m_child)
        ((GameObject *)m_vuPhysics)->Disable();
    static int PMTickID = -1;           // profiler IDs, unused
    static int PrtcleMgrTickID = -1;
    float ms = 1000.0f * dt;
    for (int i = 0; i < m_numParticles; i++) {
        m_particles[i]->m_age += ms;
        Particle *p = m_particles[i];
        unsigned char expired = p->m_age > p->m_lifetime;
        if (expired) {
            m_particles[i] = m_particles[m_numParticles - 1];
            m_particles[m_numParticles - 1] = p;
            m_numParticles--;
            if (i == m_numParticles)
                break;
        }
        TickParticle(this, m_particles[i], dt);
    }
    if (LpGame->m_debugOverlay) {
        if (g_ParticleDBGID < 0)
            g_ParticleDBGID = LpGame->m_debugOverlay->m_numPages++;
        LpGame->m_debugOverlay->PrintfHeading(g_ParticleDBGID, "Particle Manager");
        LpGame->m_debugOverlay->PrintfDetail(g_ParticleDBGID, "NumberOfParticles: %d", m_numParticles);
    }
    return 1;
}

#pragma fast_fptosi reset

// ---- src/ParticleManager/LoadFromDescriptorFile.cpp

// An inlined strrchr(name, '.').
static inline char *FindExtension(char *s)
{
    char *start = s;
    while (*s++)
        ;
    do {
        s--;
    } while (s != start && *s != '.');
    return *s == '.' ? s : 0;
}

// Reads a particle descriptor (.prt): texture, groups and per-frame UVs.
int ParticleManager::LoadFromDescriptorFile(char *name)
{
    ParameterBlock block;
    char path[0x110];
    char texturePath[0x110];
    char textureName[0x110];
    char section[0x40];
    int numFrames;
    float left, right, top, bottom;
    int i;

    strcpy(path, ParticleTextureContentDirectory);
    strcat(path, name);
    char *extension = FindExtension(path);
    if (extension)
        strcpy(extension, ".prt");
    else
        strcat(path, (char *)".prt");
    if (block.OpenFile(path, 0)) {
        numFrames = 0;
        block.ReadParameterBlock("General Info");
        block.GetParameter("TextureMap", "", textureName, -1);
        block.GetParameter("NumFrames", 0, &numFrames);
        block.GetParameter("NumGroups", 0, &m_numGroups);
        block.GetParameter("GlowScalar", 255.0f, &m_glowScalar);
        strcpy(texturePath, ParticleTextureContentDirectory);
        strcat(texturePath, textureName);
        m_texture = GetTextureMapFromResourceName(texturePath, 0x115C, 0, 0, (_D3DBLEND)5, (_D3DBLEND)6, 0x80, 0xFF00FF, 1);
        if (!m_texture)
            return 0;
        if (m_numGroups > 0) {
            m_groups = new ParticleGroup[m_numGroups];
            for (i = 0; i < m_numGroups; i++) {
                sprintf(section, "Group%d", i);
                block.ReadParameterBlock(section);
                block.GetParameter("TypeID", &m_groups[i].m_type);
                block.GetParameter("FirstFrame", &m_groups[i].m_firstFrame);
                block.GetParameter("LastFrame", &m_groups[i].m_lastFrame);
            }
        }
        if (numFrames > 0) {
            float width = m_texture->m_width;
            float height = m_texture->m_height;
            m_frames = new ParticleFrame[numFrames];
            for (i = 0; i < numFrames; i++) {
                left = 0;
                right = 0;
                top = 0;
                bottom = 0;
                sprintf(section, "Frame%d", i);
                block.ReadParameterBlock(section);
                block.GetParameter("UVLeft", &left);
                left /= width;
                block.GetParameter("UVRight", &right);
                right /= width;
                block.GetParameter("UVTop", &top);
                top /= height;
                block.GetParameter("UVBottom", &bottom);
                bottom /= height;
                m_frames[i].m_uv[0] = left;
                m_frames[i].m_uv[1] = top;
                m_frames[i].m_uv[2] = right;
                m_frames[i].m_uv[3] = top;
                m_frames[i].m_uv[4] = right;
                m_frames[i].m_uv[5] = bottom;
                m_frames[i].m_uv[6] = left;
                m_frames[i].m_uv[7] = bottom;
            }
        }
        return 1;
    }
    PSXprintf("Error reading (or could not find) particle descriptor file %s\n", path);
    return 0;
}

// ---- src/ParticleManager/ClearParticles.cpp

void ParticleManager::ClearParticles()
{
    m_numParticles = 0;
    if (m_vuPhysics)
        m_vuPhysics->ResetPacket();
}

// ---- src/ParticleManager/UpdatePerFrameStates.cpp

struct QWORD {
    float x, y, z;
    int w;
};

// The per-frame packet: render state, the particle colours, the camera
// matrices and the fog parameters.
void ParticleManager::UpdatePerFrameStates()
{
    QWORD color;
    PSXTextureMap *texture;
    PSXCamera *camera;
    int i;
    DmaRenderItem *item;
    camera = (PSXCamera *)LpRenderTarget->m_camera;
    item = &m_renderItem1;
    item->Start((DmaRenderDestination)1, 0x80);
    texture = m_texture;
    PSXRenderStateBlock state;
    state.ZWriteEnable(0);
    LpTextureMapManager->AddTextureToGifList(texture);
    LpTextureMapManager->AddTextureInterruptToRenderQueue();
    state.SetTextureMap(texture);
    static int AlphaTestEnableValue = 1;     // tuning knobs
    static int AlphaRefValue = 1;
    state.AlphaTestEnable(AlphaTestEnableValue);
    state.SetAlphaRefValue(AlphaRefValue);
    state.ComposeRenderItem(item);
    item->DmaAddVifCode(0x11000000);
    item->DmaAddVifCode(0x03000000);
    item->DmaAddVifCode(0x02000000);
    item->DmaBeginUnpack(0, 0xC);
    for (i = 0; i < 4; i++) {
        color.x = ((int)m_vertices[i].color >> 16) & 0xFF;   // signed: retail converts with cvt.s.w
        color.y = ((int)m_vertices[i].color >> 8) & 0xFF;
        color.z = (int)m_vertices[i].color & 0xFF;
        color.w = 0x40;
        item->DmaAddQwordToUnpack(&color);
    }
    item->DmaAddMatrixToUnpack(&camera->m_x250);
    item->DmaAddMatrixToUnpack(&camera->m_guardBandViewProjection);
    item->DmaAddVectorToUnpack(&camera->m_position);
    _D3DVECTOR fog;
    fog.x = 2e-05f;
    fog.y = -16.0f;
    fog.z = ((PSXRenderTarget *)LpRenderTarget)->m_fogEnd;
    fog.w = ((PSXRenderTarget *)LpRenderTarget)->m_fogScale;
    item->DmaAddVectorToUnpack(&fog);
    item->DmaEndUnpack();
    item->DmaAddVifCode(0x11000000);
    item->DmaAddVifCodeMSCAL();
    item->DmaAddVifCode(0x0300000C);
    item->DmaAddVifCode(0x020001F2);
    item->DmaAddVifCode(0x10000000);
    item->End();
}

// ---- src/ParticleManager/UpdateParticleRenderItem.cpp
// mwcc session-state shim: retail was
// built in one compiler run, so an earlier 64-bit divide had raised the
// helper-argument high-water mark; this unused function reproduces it.
static void MWCC_SHIM_TEXT MwccSessionState(long *a) { a[0] = a[0] / a[1]; }
#pragma fast_fptosi on

void ParticleManager::UpdateParticleRenderItem()
{
    sceGifTag tag;
    int perBatch;
    int i;
    int size;
    float *d;
    int batches;
    int remainder;
    DmaRenderItem *item;
    Particle *p;
    int b;
    Particle *firstP;
    int firstList;
    int count;
    int list;
    *(int128_t *)&tag = 0;
    tag.EOP = 1;
    tag.PRE = 1;
    tag.PRIM = 0x7C;
    tag.FLG = 0;
    tag.NREG = 3;
    tag.REGS0 = 2;
    tag.REGS1 = 1;
    tag.REGS2 = 4;
    item = &m_renderItem0;
    perBatch = 25;
    count = m_12C;
    size = m_maxParticles * 4 + (m_maxParticles / 25 + 1) * 3;
    if (count < 25)
        perBatch = count;
    batches = count / perBatch;
    remainder = count - perBatch * batches;
    item->Start((DmaRenderDestination)1, size * 4);
    list = m_binMax + 1;
    p = 0;
    if (remainder)
        batches++;
    for (b = 0; b < batches; b++) {
        if (b == batches - 1 && remainder)
            perBatch = remainder;
        item->DmaBeginUnpack(0, 12);
        tag.NLOOP = perBatch * 4;
        item->DmaAddGifTagToUnpack(&tag);
        firstP = p;
        firstList = list;
        d = (float *)&item->m_dataList[item->m_dataListCurrentSize];
        ((int *)d)[0] = perBatch;
        d[1] = d[2] = d[3] = 0;
        DmaRenderItem::UnpackCount += 4;
        d += 4;
        item->m_dataListCurrentSize += 4;
        for (i = 0; i < perBatch; i++) {
            if (!p) {
                list--;
                while (m_lists[list].m_frame != m_114)
                    list--;
                p = m_lists[list].m_first;
            }
            d[0] = p->m_position.x;
            d[1] = p->m_position.y;
            d[2] = p->m_position.z;
            d[3] = 1.0f;
            unsigned int flags = p->m_flags;
            float alpha = 64.0f;
            if (flags & 0x2000)
                alpha = m_glowScalar;
            unsigned int a;
            if (flags & 2)
                a = alpha * (1.0f - p->m_age * p->m_oneOverLifetime);
            else if (flags & 0x1000)
                a = alpha * (p->m_age * p->m_oneOverLifetime * (p->m_x40 / (p->m_size * (p->m_size * p->m_size))));
            else
                a = alpha;
            d[4] = d[5] = LpRenderTarget->m_camera->m_focalLength * p->m_size;
            d[6] = p->m_depth;
            ((unsigned int *)d)[7] = a;
            p = p->m_next;
            d += 8;
        }
        DmaRenderItem::UnpackCount += perBatch * 8;
        item->m_dataListCurrentSize += perBatch * 8;
        item->DmaEndUnpack();
        p = firstP;
        list = firstList;
        item->DmaBeginUnpack(-1, 12);
        d = (float *)&item->m_dataList[item->m_dataListCurrentSize];
        for (i = 0; i < perBatch; i++) {
            if (!p) {
                list--;
                while (m_lists[list].m_frame != m_114)
                    list--;
                p = m_lists[list].m_first;
            }
            ParticleFrame *f = &m_frames[p->m_frame];
            if (p->m_flags & 0x4000) {
                d[0] = f->m_uv[2];
                d[2] = f->m_uv[0];
                d[4] = f->m_uv[6];
                d[6] = f->m_uv[4];
            } else {
                d[0] = f->m_uv[0];
                d[2] = f->m_uv[2];
                d[4] = f->m_uv[4];
                d[6] = f->m_uv[6];
            }
            if (p->m_flags & 0x8000) {
                d[1] = f->m_uv[5];
                d[3] = f->m_uv[7];
                d[5] = f->m_uv[1];
                d[7] = f->m_uv[3];
            } else {
                d[1] = f->m_uv[1];
                d[3] = f->m_uv[3];
                d[5] = f->m_uv[5];
                d[7] = f->m_uv[7];
            }
            p = p->m_next;
            d += 8;
        }
        DmaRenderItem::UnpackCount += perBatch * 8;
        item->m_dataListCurrentSize += perBatch * 8;
        item->DmaEndUnpack();
        item->DmaAddVifCode(0x17000000);
    }
    PSXRenderStateBlock block;
    block.ZWriteEnable(1);
    block.ComposeSimpleRenderItem(item);
    item->End();
}

#pragma fast_fptosi reset

// ---- src/ParticleManager/PrepareGeometry.cpp
#pragma fast_fptosi on

// Sorts the particles into depth bins (front to back lists for
// UpdateParticleRenderItem) and lights the particle quad for this camera.
int ParticleManager::PrepareGeometry()
{
    if (m_numParticles <= 0)
        return 1;
    static int PrtcleMgrPrprGeomID = -1;
    if (m_texture) {
        TextureMap *texture = m_texture;
        if (!(10.0f <= texture->m_usage))
            texture->m_usage = 10.0f;
        else
            texture = texture;          // vanishes; gives retail's block-optimizer event (docs: round 17)
    }
    static int PrepareGeometryID = -1;
    int i;
    _D3DMATRIX *view = &((PSXCamera *)LpRenderTarget->m_camera)->m_x250;
    static int ParticleLoop1 = -1;
    m_12C = 0;
    // 16777000 written out each time: a named constant loads before TotalBins.
    float binScale = (float)TotalBins / 16777000.0f;
    float binSize = 1.0f / binScale;
    m_binMin = 0x7FFFFFFF;
    m_binMax = 0;
    m_114++;
    for (i = 0; i < m_numParticles; i++) {
        Particle *p = m_particles[i];
        float z = p->m_position.x * view->_13 + p->m_position.y * view->_23 + p->m_position.z * view->_33 + view->_43;
        if (z < 16777000.0f) {
            int bin = TotalBins - (int)z * binScale;
            p->m_depth = binSize * (int)(z * binScale);
            if (bin < TotalBins) {
                if (m_lists[bin].m_frame < m_114) {
                    m_lists[bin].m_first = p;
                    m_lists[bin].m_frame = m_114;
                    p->m_next = 0;
                } else {
                    p->m_next = m_lists[bin].m_first;
                    m_lists[bin].m_first = p;
                }
                m_12C++;
                if (bin < m_binMin)
                    m_binMin = bin;
                if (m_binMax < bin)
                    m_binMax = bin;
            }
        }
    }
    if (m_12C == 0)
        return 1;
    static int LightingID = -1;
    _D3DMATRIX matrix;
    _D3DVECTOR position;
    position = LpRenderTarget->m_camera->m_position + 100.0f * LpRenderTarget->m_camera->m_lookAt;
    _D3DVECTOR *lookAt = &LpRenderTarget->m_camera->m_lookAt;
    _D3DVECTOR back = _D3DVECTOR(-lookAt->x, -lookAt->y, -lookAt->z);
    Vu0OrientMatrix(&matrix, &position, back, LpRenderTarget->m_camera->m_up, 1, 1);
    LpLightManager->LightVertices(&matrix, 4, m_50, m_vertices, 16);
    m_vertices[0].color &= 0xFFFFFF;
    m_vertices[1].color &= 0xFFFFFF;
    m_vertices[2].color &= 0xFFFFFF;
    m_vertices[3].color &= 0xFFFFFF;
    return 1;
}

#pragma fast_fptosi reset

// ---- src/ParticleManager/Render3D.cpp

// The timer IDs are leftovers of profiling code compiled out of retail.
int ParticleManager::Render3D()
{
    if (m_vuPhysics && m_vuPhysics->m_18)
        m_vuPhysics->Render3D();
    if (!m_numParticles || m_12C <= 0)
        return 1;
    if (TimeGPU) {
        LpRenderTarget->TrySendRenderQueue(1);
        LpRenderTarget->WaitForRenderingToComplete();
    }
    static int CPUTimeID = -1;
    static int UpdatePerFrameStatesID = -1;
    UpdatePerFrameStates();
    static int UpdateParticleRenderItemID = -1;
    UpdateParticleRenderItem();
    static int AddToRenderQueueID = -1;
    ((PSXRenderTarget *)LpRenderTarget)->AddMPGToRenderQueue(ParticleCode);
    ((PSXRenderTarget *)LpRenderTarget)->AddToRenderQueue(&m_renderItem1, "");
    ((PSXRenderTarget *)LpRenderTarget)->AddToRenderQueue(&m_renderItem0, "");
    if (TimeGPU) {
        static int GPUTimeID = -1;
        LpRenderTarget->TrySendRenderQueue(1);
        LpRenderTarget->WaitForRenderingToComplete();
    } else if (((PSXRenderTarget *)LpRenderTarget)->IsRenderingComplete() == 1) {
        LpRenderTarget->TrySendRenderQueue(0);
    }
    return 1;
}

// ---- src/sinit/Particles.cpp
// Static initializer of Particles.cpp: only the axis vectors from the shared header.
