// ParticleVUPhysics.cpp  @ 0x00393920 (text 0x393920-0x395380)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's ParticleVUPhysics.cpp as one translation unit: its functions in
// retail order, then its static initializer. `make link` places the
// object's data as well as its code (tools/link_c.py). SetPacketAlphaRange
// came from the cross-TU stub file src/JetskiTraining/UnloadIntroDialog.cpp
// (split: GameText's ctor stays there for Localization.cpp). The rest of
// its tu_map row is two TUs without an initializer: Localization.cpp
// (GameText, its ASSERTs say so) and LoadingIcon.cpp. The old files' views
// map onto the shared header: Tick's clock is m_depthBias, its frame time
// and count m_x20/m_x24, its packet count m_count; the per-frame state
// items are m_itemB0/m_itemE0.




#include "d3dtypes.h"
#include "GameObject/GameObject.h"
#include "DmaRenderItem/DmaRenderItem.h"
#include "Game/Game.h"
#include "PSXVideoCard/PSXVideoCard.h"
#include "sceGifTag/sceGifTag.h"
#include "ParticleVUPhysics/ParticleVUPhysics.h"
#include "PSXRenderStateBlock/PSXRenderStateBlock.h"
#include "Camera/Camera.h"
#include "DebugOverlay/DebugOverlay.h"
#include "PSXTextureMap/PSXTextureMap.h"
#include "PSXTextureMapManager/PSXTextureMapManager.h"
#include "RenderTarget/RenderTarget.h"
#include "PSXRenderTarget/PSXRenderTarget.h"
// The axis vectors every Soultree TU defines (and __sinit constructs).
#include "sinit/AxisVectors.h"

extern "C" void *memset(void *dst, int c, unsigned int n);
extern Game *LpGame;
extern PSXVideoCard *LpVideoCard;
extern PSXTextureMapManager *LpTextureMapManager;
extern RenderTarget *LpRenderTarget;
extern int TimeGPU;
int RenderParticles = 1;
extern unsigned int StripPreambleCode[];    // VU1 microcode
extern unsigned int ParticlePhysicsCode[];  // VU1 microcode
void BuildGenericRenderItem(DmaRenderItem *item);
void BuildUniqueRenderItem(DmaRenderItem *item, _D3DMATRIX *matrix, unsigned int flags);
_D3DMATRIX IdentityMatrix();

ParticleVUPhysics::ParticleVUPhysics()
{
    m_18 = 0;
    m_currentPacket = 0;
    m_dirty = 0;
    m_x20 = 0.0183f;
    m_x24 = 0;
    m_v30.x = 0.0f;
    m_v30.y = -20.0f;
    m_v30.z = 0.0f;
    m_v30.w = 1.0f;
    m_v40.x = 128.0f;
    m_v40.y = 128.0f;
    m_v40.z = 128.0f;
    m_v40.w = 128.0f;
    m_numUpdated = 0;
    m_lastPacket = 0;
    m_v50[0].x = 0.0f;
    m_v50[0].y = 0.0f;
    m_v50[0].z = 0.0f;
    m_v50[0].w = 1.0f;
    m_v50[1].x = 1.0f;
    m_v50[1].y = 1.0f;
    m_v50[1].z = 0.0f;
    m_v50[1].w = 1.0f;
    m_particles = 0;
    m_items = 0;
    m_packetDepths = 0;
    m_124 = 0;
    m_packetMinDepths = 0;
    m_xA0 = 0;
    m_A4 = 0;
    m_v90.x = -100000.0f;
    m_v90.y = -100000.0f;
    m_v90.z = -100000.0f;
    m_v90.w = 1.0f;
    SetVelocityRange(90.0f, 35.0f, 90.0f);
    SetPacketAlphaRange(0.0f, 190.0f);
    m_depthBias = 0.0f;
}

ParticleVUPhysics::~ParticleVUPhysics()
{
    if (m_particles)
        delete[] m_particles;
    if (m_items)
        delete[] m_items;
    if (m_packetDepths)
        delete[] m_packetDepths;
    if (m_packetMinDepths)
        delete[] m_packetMinDepths;
    if (m_124)
        delete[] m_124;
    if (m_xA0)
        delete[] m_xA0;
}

// Compiler session state: retail compiled
// this file in an mwcc session that had already generated a two-argument runtime
// helper call (64-bit / or %, double arithmetic or compare). mwcc then models
// every later helper call (__floatdisf, fptoui, fptodp, ...) as reading $a0-$a1,
// so temporaries live before such a call skip $a0/$a1. This unused function
// recreates that state; it is not part of the retail file. The whole TU
// still needs it for Tick (8 words off without it): nothing before Tick here
// makes such a call.
static void MWCC_SHIM_TEXT MwccSessionState(long *a) { a[0] = a[0] / a[1]; }

extern int TimerOverflowCounterT0;

inline unsigned int timeGetTime()
{
    static long Time = 0;
    static int lastCount = 0;
    int count;
    int elapsed;
    int overflows;
    count = *(volatile int *)0x10000000;
    overflows = TimerOverflowCounterT0;
    TimerOverflowCounterT0 = 0;
    if (lastCount <= count)
        elapsed = count - lastCount;
    else
        elapsed = 0x10000 - lastCount + count;
    if (lastCount >= count && overflows > 0)
        overflows--;
    lastCount = count;
    Time += elapsed + (overflows << 16);
    return 0.0017066667f * Time;
}

int ParticleVUPhysics::Tick(float dt)
{
    if (!m_18)
        return 1;
    if (LpGame->m_paused)
        return 1;
    m_depthBias += 1000.0f * dt;
    static unsigned int lastTime = 0;
    static unsigned int lastFrame = 0;
    static float fps = 0;
    static float CurrSumDt = 0;
    static int NumSamples = 0;
    unsigned int now = timeGetTime();
    unsigned int elapsed = now - lastTime;
    if (elapsed > 1000) {
        unsigned int frame = LpVideoCard->m_frameCount;
        fps = 1000.0f * (frame - lastFrame) / elapsed;
        lastTime = now;
        lastFrame = frame;
        if (fps < 70.0f && !(fps <= 15.0f)) {
            CurrSumDt += 0.5f / fps;
            NumSamples++;
            m_x20 = CurrSumDt / NumSamples;
            if (NumSamples > 20) {
                NumSamples = 0;
                CurrSumDt = 0;
            }
        }
    }
    m_x24 += 1.0f;
    if (!(m_x24 <= 65535.0f)) {
        m_x24 = 0;
        for (int i = 0; i < m_count; i++)
            m_packetDepths[i] = 0;
    }
    return 1;
}

static inline void SetVector(float *v, float x, float y, float z, float w)
{
    v[0] = x;
    v[1] = y;
    v[2] = z;
    v[3] = w;
}


void ParticleVUPhysics::Initialize(TextureMap *texture, int count, int numParticles, int numTotal)
{
    if (m_18)
        return;
    m_count = count;
    m_numTotal = numTotal;
    m_numParticles = numParticles;
    m_texture = texture;
    memset(&m_gifTag, 0, sizeof(m_gifTag));
    m_gifTag.EOP = 1;
    m_gifTag.PRE = 1;
    m_gifTag.PRIM = 0x7E;
    m_gifTag.FLG = 0;
    m_gifTag.NREG = 5;
    m_gifTag.REGS0 = 1;
    m_gifTag.REGS1 = 2;
    m_gifTag.REGS2 = 4;
    m_gifTag.REGS3 = 2;
    m_gifTag.REGS4 = 4;
    m_gifTag.NLOOP = m_numParticles;
    m_particles = new VUParticle[m_numTotal];
    m_items = new DmaRenderItem[m_count];
    m_packetDepths = new int[m_count];
    m_124 = new float[m_count];
    m_packetMinDepths = new float[m_count];
    m_xA0 = new _D3DVECTOR[8];
    int i;
    for (i = 0; i < m_numTotal; i++) {
        m_particles[i].m_life = 0.0f;
        m_particles[i].m_oneOverLife = 1e10f;
        SetVector(m_particles[i].m_position, 0.0f, 0.0f, 0.0f, 1.0f);
        SetVector(m_particles[i].m_velocity, 0.0f, 0.0f, 0.0f, 1.0f);
        m_particles[i].m_size = 0.0f;
        m_particles[i].m_growth = 0.0f;
    }
    for (i = 0; i < m_count; i++) {
        SetupPacket(&m_items[i]);
        m_packetDepths[i] = 0;
        m_packetMinDepths[i] = 100000.0f;
        m_124[i] = 0;
    }
    m_18 = 1;
}

void ParticleVUPhysics::ResetPacket()
{
    if (m_18) {
        for (int i = 0; i < m_count; i++) {
            m_packetDepths[i] = 0;
            m_packetMinDepths[i] = 100000.0f;
            m_124[i] = 0;
        }
    }
}

void ParticleVUPhysics::SetVelocityRange(float x, float y, float z)
{
    m_velocityScale.x = 32767.0f / x;
    m_velocityScale.y = 32767.0f / y;
    m_velocityScale.z = 32767.0f / z;
}

void ParticleVUPhysics::SetPacketAlphaRange(float a, float b)
{
    m_x28 = a;
    m_x2C = b;
}

#define VIF_MSCNT 0x17000000
#define VIF_UNPACK_USN 0x4000           // UNPACK "unsigned" bit


// Build the VU1 packet; the particle data itself is written in place later,
// so each unpack only reserves its space.
void ParticleVUPhysics::SetupPacket(DmaRenderItem *item)
{
    int n = m_numParticles;
    item->Start((DmaRenderDestination)1, n * 2 + (n * 2 + (n + 5) * 4));
    item->DmaBeginUnpack(0, 12);
    DmaRenderItem::UnpackCount = (m_numParticles + 4) * 4;
    item->m_dataListCurrentSize += (m_numParticles + 4) * 4;
    item->DmaEndUnpack();
    item->DmaBeginUnpack(m_numParticles + 4, 13);
    DmaRenderItem::UnpackCount = m_numParticles * 4;
    item->m_dataListCurrentSize += m_numParticles * 2;
    item->DmaEndUnpack();
    item->DmaBeginUnpack(m_numParticles + m_numParticles + 4, 5);
    DmaRenderItem::UnpackCount = m_numParticles * 4;
    item->m_dataListCurrentSize += m_numParticles;
    item->DmaEndUnpack();
    item->m_dataList[DmaRenderItem::UnpackCommandIndex] |= VIF_UNPACK_USN;
    item->DmaAddVifCode(VIF_MSCNT);
    item->End();
}


#pragma fast_fptosi on

static inline float Fmin(float a, float b)
{
    float r;
    asm("min.s $f8, %1, %2; swc1 $f8, 0(%0)" : : "r"(&r), "f"(a), "f"(b) : "$f8", "memory");
    return r;
}


struct ParticlePoint {
    float x, y, z, w;
};

// The per-packet constants VU1 reads, followed by one point per particle.
struct ParticlePacketHeader {
    float m_x00[16];
    ParticlePoint m_points[1];          // +0x40
};

// Fills one VU1 packet: the emitter constants, then positions, then per-particle
// short data whose layout depends on flags. The Fmin asm turns the global optimizer
// off, so the loop constants are locals (declared in reverse to get retail's FP
// registers) and each case repeats its setup.
void ParticleVUPhysics::UpdateParticlePacket(int packet, DmaRenderItem *item, int first, int count, int flags)
{
    short *s;
    int i;
    VUParticle *p;
    int offset;
    ParticlePacketHeader *header;
    offset = 1;
    p = &m_particles[first];
    header = (ParticlePacketHeader *)&item->m_dataList[offset];
    header->m_x00[0] = 1.0f / m_velocityScale.x;
    *(int *)&header->m_x00[1] = flags;
    header->m_x00[2] = m_x28;
    header->m_x00[3] = 1.0f / m_velocityScale.y;
    if ((float)m_packetDepths[m_currentPacket] > m_depthBias)
        m_packetMinDepths[packet] = Fmin(m_packetMinDepths[packet], m_v90.y);
    else
        m_packetMinDepths[packet] = m_v90.y;
    header->m_x00[4] = m_x24 - 0.001f * p->m_x20 / m_x20;
    header->m_x00[5] = m_packetMinDepths[packet];
    header->m_x00[6] = m_x20;
    header->m_x00[7] = m_x2C - m_x28;
    header->m_x00[8] = m_v50[0].x;
    header->m_x00[9] = m_v50[0].y;
    header->m_x00[10] = m_v50[1].x;
    header->m_x00[11] = m_v50[1].y;
    header->m_x00[12] = 0.5f * m_v30.x;
    header->m_x00[13] = 0.5f * m_v30.y;
    header->m_x00[14] = 0.5f * m_v30.z;
    *(int *)&header->m_x00[15] = 0;
    for (i = 0; i < m_numParticles; i++) {
        header->m_points[i].x = p->m_position[0];
        header->m_points[i].y = p->m_position[1];
        header->m_points[i].z = p->m_position[2];
        switch (flags) {
        case 0:
        case 1:
            if (i >= count)
                header->m_points[i].w = 0;
            else
                header->m_points[i].w = m_depthBias + p->m_life;
            break;
        case 2:
            if (i >= count)
                header->m_points[i].w = 0;
            else
                header->m_points[i].w = m_depthBias - p->m_x20;
            break;
        }
        p++;
    }
    offset += (m_numParticles + 4) * 4 + 1;
    s = (short *)&item->m_dataList[offset];
    p = &m_particles[first];
    for (i = 0; i < m_numParticles; i++) {
        *s = (int)(p->m_velocity[0] * m_velocityScale.x);
        s++;
        *s = (int)(p->m_velocity[1] * m_velocityScale.y);
        s++;
        *s = (int)(p->m_velocity[2] * m_velocityScale.z);
        s++;
        *s = (int)p->m_growth;
        s++;
        p++;
    }
    offset += m_numParticles * 2 + 1;
    switch (flags) {
    case 0: {
        float k2;
        float k1;
        s = (short *)&item->m_dataList[offset];
        p = &m_particles[first];
        i = 0;
        k1 = 655.34f;
        k2 = 32767.0f;
        for (; i < m_numParticles; i++) {
            *s = (int)(k1 * p->m_size);
            s++;
            *s = (int)(k2 * p->m_oneOverLife);
            s++;
            p++;
        }
        break;
    }
    case 1: {
        float k3;
        float k2;
        float k1;
        s = (short *)&item->m_dataList[offset];
        p = &m_particles[first];
        i = 0;
        k1 = 655.34f;
        k2 = 32767.0f;
        k3 = 0.001f;
        for (; i < m_numParticles; i++) {
            *s = (int)(k1 * p->m_size);
            s++;
            *s = (int)(k2 * p->m_oneOverLife);
            s++;
            *s = 0;
            s++;
            *s = (int)(m_x24 - k3 * p->m_x20 / m_x20);
            s++;
            p++;
        }
        break;
    }
    case 2:
    default: {
        float k1;
        s = (short *)&item->m_dataList[offset];
        p = &m_particles[first];
        i = 0;
        k1 = 655.34f;
        for (; i < m_numParticles; i++) {
            *s = (int)(k1 * p->m_size);
            s++;
            *s = (int)p->m_life;
            s++;
            p++;
        }
        break;
    }
    }
}


void ParticleVUPhysics::UpdateParticlePackets(int first, int count, int flags)
{
    static int UpdatePacketsCost = -1;  // profiler IDs, unused
    int i;
    int packets;
    int start;
    int remaining;
    // One `||` return, as retail: its last test keeps the branch-over-`b`
    // shape, and it gives mwcc's block optimizer the event that moves (rather
    // than copies) delay-slot fills elsewhere.
    if (!m_18 || LpGame->m_paused)
        return;
    if (count > 0) {
        static int PMTickID = -1;
        start = first;
        remaining = count;
        m_numUpdated += count;
        packets = count / m_numParticles + 1;
        for (i = 0; i < packets; i++) {
            float depth = 0.0f;
            DmaRenderItem *item = &m_items[m_currentPacket];
            for (int j = start; j < start + m_numParticles; j++) {
                if (m_particles[j].m_life > depth)
                    depth = m_particles[j].m_life;
            }
            UpdateParticlePacket(m_currentPacket, item, start, remaining, flags);
            m_packetDepths[m_currentPacket] = (int)(m_depthBias + depth);
            int n = m_numParticles;
            m_currentPacket++;
            start += n;
            remaining -= n;
            if (m_currentPacket >= m_count)
                m_currentPacket = 0;
        }
        m_dirty = 0;
    }
}

#pragma fast_fptosi reset

// The next free quadword of the open unpack, and the step past it.
static inline float *UnpackSlot(DmaRenderItem *item)
{
    return (float *)&item->m_dataList[item->m_dataListCurrentSize];
}

static inline void UnpackAdvance(DmaRenderItem *item)
{
    DmaRenderItem::UnpackCount += 4;
    item->m_dataListCurrentSize += 4;
}

void ParticleVUPhysics::UpdatePerFrameStates()
{
    m_itemB0.Start((DmaRenderDestination)1, 0xB8);
    BuildGenericRenderItem(&m_itemB0);
    m_itemB0.End();

    float *q;
    PSXRenderStateBlock state;
    DmaRenderItem *item = &m_itemE0;
    item->Start((DmaRenderDestination)1, state.GetRenderItemSize() + 0x30);
    PSXTextureMap *texture = (PSXTextureMap *)m_texture;
    state.ZWriteEnable(0);
    LpTextureMapManager->AddTextureToGifList(texture);
    LpTextureMapManager->AddTextureInterruptToRenderQueue();
    state.SetTextureMap(texture);
    static int AlphaTestEnableValue = 1;     // tuning knobs
    static int AlphaRefValue = 1;
    state.AlphaTestEnable(AlphaTestEnableValue);
    state.SetAlphaRefValue(AlphaRefValue);
    state.ComposeRenderItem(item);

    item->DmaBeginUnpack(0x2E, 0xC);
    q = UnpackSlot(item);
    q[0] = 0.0f;
    q[1] = 0.0f;
    q[2] = 0.0f;
    q[3] = m_x24;
    UnpackAdvance(item);
    item->DmaEndUnpack();

    item->DmaBeginUnpack(0x2F, 0xC);
    q = UnpackSlot(item);
    q[0] = m_v40.x;
    q[1] = m_v40.y;
    q[2] = m_v40.z;
    q[3] = m_depthBias;
    UnpackAdvance(item);
    q = UnpackSlot(item);
    q[0] = 1.0f / m_velocityScale.x;
    q[1] = 0.0015259254f;
    q[2] = 1.0f / 32767.0f;
    q[3] = 1.0f / m_velocityScale.y;
    UnpackAdvance(item);
    q = UnpackSlot(item);
    q[0] = m_v50[0].x;
    q[1] = m_v50[0].y;
    q[2] = m_v50[1].x;
    q[3] = m_v50[1].y;
    UnpackAdvance(item);
    item->DmaAddGifTagToUnpack(&m_gifTag);
    q = UnpackSlot(item);
    q[0] = m_v90.x;
    q[1] = m_v90.y;
    q[2] = m_v90.z;
    q[3] = LpRenderTarget->m_camera->m_focalLength;
    UnpackAdvance(item);
    item->DmaEndUnpack();

    _D3DMATRIX identity = IdentityMatrix();
    BuildUniqueRenderItem(item, &identity, 0);
    item->End();
}

int ParticleVUPhysics::Render3D()
{
    static int frameCnt = 0;
    if (!m_18)
        return 1;
    if (TimeGPU) {
        LpRenderTarget->TrySendRenderQueue(1);
        LpRenderTarget->WaitForRenderingToComplete();
    }
    static int particleCPUTime = -1;    // profiler ids, unused in retail
    int i;
    int rendered;
    int first;
    rendered = 0;
    if (RenderParticles) {
        first = 1;
        for (i = 0; i < m_count; i++) {
            if (m_depthBias < m_packetDepths[i]) {
                if (first) {
                    UpdatePerFrameStates();
                    ((PSXRenderTarget *)LpRenderTarget)->AddMPGToRenderQueue(StripPreambleCode);
                    ((PSXRenderTarget *)LpRenderTarget)->AddMPGToRenderQueue(ParticlePhysicsCode);
                    ((PSXRenderTarget *)LpRenderTarget)->AddToRenderQueue(&m_itemB0, "");
                    ((PSXRenderTarget *)LpRenderTarget)->AddToRenderQueue(&m_itemE0, "");
                    first = 0;
                }
                rendered++;
                ((PSXRenderTarget *)LpRenderTarget)->AddToRenderQueue(&m_items[i], "");
            }
        }
    }
    m_lastPacket = m_currentPacket;
    if (TimeGPU) {
        static int GPUTimeID = -1;
        LpRenderTarget->TrySendRenderQueue(1);
        LpRenderTarget->WaitForRenderingToComplete();
    } else if (((PSXRenderTarget *)LpRenderTarget)->IsRenderingComplete() == 1) {
        LpRenderTarget->TrySendRenderQueue(0);
    }
    static int overlayID = -1;
    if (LpGame->m_debugOverlay) {
        if (overlayID == -1)
            overlayID = LpGame->m_debugOverlay->m_numPages++;
        LpGame->m_debugOverlay->PrintfHeading(overlayID, "Particles");
        LpGame->m_debugOverlay->PrintfDetail(overlayID, "Number Of Packets Rendered %i", rendered);
        LpGame->m_debugOverlay->PrintfDetail(overlayID, "Number Of Particles Rendered %i", rendered * m_numParticles);
        LpGame->m_debugOverlay->PrintfDetail(overlayID, "Number Of packets %i", m_count);
        LpGame->m_debugOverlay->PrintfDetail(overlayID, "Number Of New Particles  %i", m_numUpdated);
    }
    m_numUpdated = 0;
    return 1;
}
