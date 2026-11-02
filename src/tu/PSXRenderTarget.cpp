// PSXRenderTarget.cpp  @ 0x00272FB0 (text 0x272FB0-0x276620)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's PSXRenderTarget.cpp as one translation unit: SyncDmaPath and
// GsSyncPath, PSXRenderTarget, then its ContainerList<_sceDmaTag *> members
// (instantiated at the end, CONTAINERLIST_MEMBERS_ALL) and its static
// initializer. Its .rodata runs from the "DMA channel does not exist." ASSERT
// to the RenderTarget/ PSXRenderTarget RTTI (@1770).




#define PSX_RENDER_TARGET_GLOBAL    // LpRenderTarget is a PSXRenderTarget * here (RenderTarget.h)
#include "ContainerList/ContainerList.h"
#define PSXRENDERTARGET_TYPED_TAGS  // m_tags is a ContainerList<_sceDmaTag *> here (PSXRenderTarget.h)
#include "sinit/AxisVectors.h"
#include "GsTextureTransfer/GsSyncPath.h"
#include "DmaRenderItem/DmaRenderItem.h"
#include "RenderTarget/RenderTarget.h"
#include "DmaPacket/DmaPacket.h"
#include "sceGifTag/sceGifTag.h"
#include "PSXVideoCard/PSXVideoCard.h"
#include "PSXRenderTarget/PSXRenderTarget.h"
#include "DebugOverlay/DebugOverlay.h"
#include "Game/Game.h"
#include "JetskiGame/JetskiGame.h"
#include "_RECT/_RECT.h"
#include "RenderTarget/D3DEnums.h"
#include "PSXTextureMap/PSXTextureMap.h"
#include "PSXTextureMapManager/PSXTextureMapManager.h"
#include "_sceDmaTag/_sceDmaTag.h"
#include "d3dtypes.h"
#include "PSXCamera/PSXCamera.h"
#include "HiResMeter/HiResMeter.h"
#include "soultree_assert_block.h"
#define CONTAINERLIST_MEMBERS_ALL
#include "ContainerList/ContainerListMembers.h"

extern "C" int FlushCache(int op);


// ---- src/global/SyncDmaPath.cpp

typedef int int128_t __attribute__((mode(TI)));

#define D_STAT ((volatile unsigned int *)0x1000E010)
#define D_PCR  ((volatile unsigned int *)0x1000E020)
#define D0_CHCR ((volatile unsigned int *)0x10008000)
#define D1_CHCR ((volatile unsigned int *)0x10009000)
#define D2_CHCR ((volatile unsigned int *)0x1000A000)
#define D3_CHCR ((volatile unsigned int *)0x1000B000)
#define D4_CHCR ((volatile unsigned int *)0x1000B400)
#define D5_CHCR ((volatile unsigned int *)0x1000C000)
#define D6_CHCR ((volatile unsigned int *)0x1000C400)
#define D7_CHCR ((volatile unsigned int *)0x1000C800)
#define D8_CHCR ((volatile unsigned int *)0x1000D000)
#define D9_CHCR ((volatile unsigned int *)0x1000D400)

// Waits for DMA channel ch to finish. Returns 0 when done, 1 when still busy
// and not waiting, 2 on timeout.
int SyncDmaPath(int ch, int wait, int quiet)
{
    union {
        int128_t q;
        int w[4];
    } count;
    count.q = 1;
    int busy = 0;

#ifdef SPLASHDOWN_PC
    if (!wait)
        port_dma_poll(ch);
#endif
    *D_STAT = 1 << ch;
    *D_PCR = 1 << ch;
    asm("sync.p; sync");
    switch (ch) {
    case 0: busy = *D0_CHCR & 0x100; break;
    case 1: busy = *D1_CHCR & 0x100; break;
    case 2: busy = *D2_CHCR & 0x100; break;
    case 3: busy = *D3_CHCR & 0x100; break;
    case 4: busy = *D4_CHCR & 0x100; break;
    case 5: busy = *D5_CHCR & 0x100; break;
    case 6: busy = *D6_CHCR & 0x100; break;
    case 7: busy = *D7_CHCR & 0x100; break;
    case 8: busy = *D8_CHCR & 0x100; break;
    case 9: busy = *D9_CHCR & 0x100; break;
    default:
#line 103 "PSXRenderTarget.cpp"
        ASSERT(0, "DMA channel does not exist.");
    }
    if (!busy || !wait) {
        *D_PCR = 0;
        // Two returns, not `return busy ? 1 : 0;`: mwcc turns either into a
        // movz, but the ?: join leaves a `b` to the next block that the block
        // optimizer deletes, which rebuilds the predecessor lists and lets the
        // delay-slot filler fill slots retail leaves empty.
        if (busy)
            return 1;
        return 0;
    }
    // Spin on the COP0 condition (DMA done) for up to 300M iterations. The
    // nop after SyncDmaDone doubles as bc0f's delay slot, as in retail.
    asm("sync.p; sync; lui $t0, 0x11E1; ori $t0, $t0, 0xA300; SyncDmaLoop: addi $t0, $t0, -1; blez $t0, SyncDmaDone; nop; bc0f SyncDmaLoop; SyncDmaDone: nop; sq $t0, 0(%0)" : : "r"(&count.q) : "$8", "memory");
    *D_PCR = 0;
    if (count.w[0] == 0) {
#line 144 "PSXRenderTarget.cpp"
        ASSERTF(quiet == 0, ("DMA channel %d sync timeout error.", ch));
        return 2;
    }
    return 0;
}


// ---- src/global/GsSyncPath.cpp
// Compiler session state: retail compiled
// this file in an mwcc session that had already generated a two-argument runtime
// helper call (64-bit / or %, double arithmetic or compare). mwcc then models
// every later helper call (__floatdisf, fptoui, fptodp, ...) as reading $a0-$a1,
// so temporaries live before such a call skip $a0/$a1. This unused function
// recreates that state; it is not part of the retail file.
static void MWCC_SHIM_TEXT MwccSessionState(long *a) { a[0] = a[0] / a[1]; }



extern int TimerOverflowCounterT0;

// Win32-style timeGetTime: EE timer 0 (T0_COUNT at 0x10000000) ticks plus the
// overflows counted by TimerHandlerT0, accumulated and scaled to milliseconds.
inline unsigned int timeGetTime()
{
    static long Time = 0;
    static int lastCount = 0;
    int count, elapsed, overflows;
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

// The parts of PSXRenderTarget and PSXVideoCard used here.

extern PSXVideoCard *LpVideoCard;

extern volatile int LockRenderQueueState;
// Retail's .data has it just before this TU's other globals, after
// PSXInputDeviceType.cpp's: this TU's first, or PSXMain.cpp's.
unsigned int SyncTimeoutResetTime = 6;

extern "C" {
void sceDmaReset(int mode);
void sceDevVif1Reset();
void sceGsResetPath();
void sceGsResetGraph(int mode, int inter, int omode, int ffmode);
}

int SyncDmaPath(int ch, int wait, int quiet);

#define REG(a) (*(volatile unsigned int *)(a))
#define REG16(a) (*(volatile unsigned short *)(a))
#define D1_MADR 0x10009010
#define D1_QWC 0x10009020
#define D1_TADR 0x10009030
#define D2_MADR 0x1000A010
#define D2_QWC 0x1000A020
#define D2_TADR 0x1000A030
#define GIF_STAT 0x10003020
#define VIF1_STAT 0x10003C00
#define D_STAT 0x1000E010

// Waits for the paths in `path` (bit 0 VIF1, bit 1 GIF) to drain. Returns
// 1/2/3 for the paths still busy when not waiting; on a timeout dumps the DMA
// state and resets the DMAC, VIF1 and GS.
int GsSyncPath(int wait, GS_PATH path)
{
    static int DmaErrorFrameCnt = 0;
    // Declaration order chosen for retail's register allocation.
    int failed;
    int busy;
    int result;
    unsigned int start;
    PSXVideoCard *card;
    unsigned int timeout;
    unsigned int frames;
    PSXRenderTarget *target;

    if (LockRenderQueueState) {
        if (wait) {
            static int WaitForGet = -1; // unused profiler ID
            while (LockRenderQueueState) {
                if (LpRenderTarget->m_finishCallback)
                    LpRenderTarget->m_finishCallback();
            }
        } else {
            return 3;
        }
    }
    failed = 0;
    busy = 0;
    if (path & 1) {
        result = SyncDmaPath(1, wait, 0);
        if (result == 1) {
            busy = 1;
        } else if (result == 0) {
            if (REG(VIF1_STAT) & 0xF) {
                start = timeGetTime();
                while (REG(VIF1_STAT) & 0xF) {
                    if (timeGetTime() - start > 1000) {
                        result = 2;
                        break;
                    }
                }
            }
        }
        if (result == 2) {
            PSXprintf("\n\n**** GS Sync Timeout DMA Channel 1 (VIF1) ****\n\n");
            PSXprintf("DMA channel 1 sync timeout.\n");
            PSXprintf("D1_MADR: (Memory Address Register)\n");
            PSXprintf("  ADDR = %x\n", REG(D1_MADR) & 0x3FFFFFFF);
            PSXprintf("  SPR = %d\n", REG(D1_MADR) >> 30 & 1);
            PSXprintf("D1_TADR: (Tag Address Register)\n");
            PSXprintf("  ADDR = %x\n", REG(D1_TADR) & 0x3FFFFFFF);
            PSXprintf("  SPR = %d\n", REG(D1_TADR) >> 30 & 1);
            PSXprintf("D1_QWC: (Quadword Count)\n");
            PSXprintf("  QWC = %d\n", REG16(D1_QWC));
            PSXprintf("VIF1_STAT: (VIF1 Status Register)\n");
            PSXprintf("  VPS = %d\n", REG(VIF1_STAT) & 3);
            PSXprintf("  VEW = %d\n", REG(VIF1_STAT) >> 2 & 1);
            PSXprintf("  VGW = %d\n", REG(VIF1_STAT) >> 3 & 1);
            PSXprintf("  MRK = %d\n", REG(VIF1_STAT) >> 6 & 1);
            PSXprintf("  DBF = %d\n", REG(VIF1_STAT) >> 7 & 1);
            PSXprintf("  VSS = %d\n", REG(VIF1_STAT) >> 8 & 1);
            PSXprintf("  VFS = %d\n", REG(VIF1_STAT) >> 9 & 1);
            PSXprintf("  VIS = %d\n", REG(VIF1_STAT) >> 10 & 1);
            PSXprintf("  INT = %d\n", REG(VIF1_STAT) >> 11 & 1);
            PSXprintf("  ER0 = %d\n", REG(VIF1_STAT) >> 12 & 1);
            PSXprintf("  ER1 = %d\n", REG(VIF1_STAT) >> 13 & 1);
            PSXprintf("  FDR = %d\n", REG(VIF1_STAT) >> 23 & 1);
            PSXprintf("  RQC = %d\n", REG(VIF1_STAT) >> 24 & 0xF);
            failed = 1;
        }
    }
    if (path & 2) {
        result = SyncDmaPath(2, wait, 0);
        if (result == 1) {
            if (busy == 1)
                busy = 3;
            else
                busy = 2;
        } else if (result == 0) {
            if (REG(GIF_STAT) & 0xFC0) {
                start = timeGetTime();
                while (REG(GIF_STAT) & 0xFC0) {
                    if (timeGetTime() - start > 1000) {
                        result = 2;
                        break;
                    }
                }
            }
        }
        if (result == 2) {
            PSXprintf("\n\n**** GS Sync Timeout DMA Channel 2 (GIF) ****\n\n");
            PSXprintf("DMA channel 2 sync timeout.\n");
            PSXprintf("D2_MADR: (Memory Address Register)\n");
            PSXprintf("  ADDR = %x\n", REG(D2_MADR) & 0x3FFFFFFF);
            PSXprintf("  SPR  = %d\n", REG(D2_MADR) >> 30 & 1);
            PSXprintf("D2_TADR: (Tag Address Register)\n");
            PSXprintf("  ADDR = %x\n", REG(D2_TADR) & 0x3FFFFFFF);
            PSXprintf("  SPR  = %d\n", REG(D2_TADR) >> 30 & 1);
            PSXprintf("D2_QWC: (Quadword Count)\n");
            PSXprintf("  QWC  = %d\n", REG16(D2_QWC));
            PSXprintf("GIF_STAT: (GIF Status Register)\n");
            PSXprintf("  M3R = %d\n", REG(GIF_STAT) & 1);
            PSXprintf("  M3P = %d\n", REG(GIF_STAT) >> 1 & 1);
            PSXprintf("  IMT = %d\n", REG(GIF_STAT) >> 2 & 1);
            PSXprintf("  PSE = %d\n", REG(GIF_STAT) >> 3 & 1);
            PSXprintf("  IP3 = %d\n", REG(GIF_STAT) >> 5 & 1);
            PSXprintf("  P3Q = %d\n", REG(GIF_STAT) >> 6 & 1);
            PSXprintf("  P2Q = %d\n", REG(GIF_STAT) >> 7 & 1);
            PSXprintf("  P1Q = %d\n", REG(GIF_STAT) >> 8 & 1);
            PSXprintf("  OPH = %d\n", REG(GIF_STAT) >> 9 & 1);
            PSXprintf("  APATH = %d\n", REG(GIF_STAT) >> 10 & 1);
            PSXprintf("  DIR = %d\n", REG(GIF_STAT) >> 12 & 1);
            PSXprintf("  FDR = %d\n", REG(GIF_STAT) >> 24 & 0xF);
            failed |= 2;
        }
    }
    if (failed) {
        PSXprintf("\nD_STAT: (DMAC Interrupt Status Register)\n");
        PSXprintf("  CIS1: %d\n", REG(D_STAT) >> 1 & 1);
        PSXprintf("  CIS2: %d\n", REG(D_STAT) >> 2 & 1);
        PSXprintf("  SIS:  %d\n", REG(D_STAT) >> 13 & 1);
        PSXprintf("  MEIS: %d\n", REG(D_STAT) >> 14 & 1);
        PSXprintf("  BEIS: %d\n", REG(D_STAT) >> 15 & 1);
        PSXprintf("  CIM1: %d\n", REG(D_STAT) >> 17 & 1);
        PSXprintf("  CIM2: %d\n", REG(D_STAT) >> 18 & 1);
        PSXprintf("  SIM:  %d\n", REG(D_STAT) >> 29 & 1);
        PSXprintf("  MEIM: %d\n", REG(D_STAT) >> 30 & 1);
#line 441 "PSXRenderTarget.cpp"
        ASSERT(0, "\n********** GS Sync Timeout *********\n");
        sceDmaReset(1);
        sceDevVif1Reset();
        sceGsResetPath();
        sceGsResetGraph(0, 1, 2, 0);
        LpRenderTarget->m_xC4 = failed;
        DmaErrorFrameCnt = LpVideoCard->m_frameCount;
    } else {
        if ((target = LpRenderTarget) != 0) {
            card = LpVideoCard;
            timeout = SyncTimeoutResetTime * 30;
            frames = card->m_frameCount;
            if (DmaErrorFrameCnt + timeout < frames)
                target->m_xC4 = 0;
        }
    }
    return busy;
}



// Defined here for its retail .bss slot.
static int nTotalQueueSize;
static int nTotalQueueSubmit;
int TimeGPU;

// Set by the static initializer; unused. Retail's .bss has it after
// GsSyncPath's statics and before Blt's.
static int RenderLockOverlapID = LpHiResMeter->GetMeterID("OverlapLockQueue");

// ---- src/PSXRenderTarget/ctor.cpp




#define GIF_MODE ((volatile int *)0x10003010)


PSXRenderTarget::PSXRenderTarget()
{
    m_dmaVif0 = 0;
    m_dmaVif1 = 0;
    m_dmaGif = 0;
    m_buildQueue = 2;
    m_sendQueue = 1;
    m_queue1.m_start = 0;
    m_queue1.m_cur = 0;
    m_queue1.m_size = 0;
    m_queue2.m_start = 0;
    m_queue2.m_cur = 0;
    m_queue2.m_size = 0;
    m_x7C = 0;
    m_lastMPG = 0;
    m_packet = 0;
    m_queueRunning = 1;
    m_renderedStrips = 0;
    m_renderedVertices = 0;
    m_fogStart = 50.0f;
    m_fogEnd = 300.0f;
    m_fogScale = 0;
    m_finishCallback = 0;
    m_xC4 = 0;
    *GIF_MODE = 4;
}


// ---- src/PSXRenderTarget/dtor.cpp
extern "C" void free(void *p);

struct _sceDmaTag;








PSXRenderTarget::~PSXRenderTarget()
{
    LpRenderTarget = 0;
    if (m_packet)
        delete m_packet;
    if (m_queue1.m_start)
        free(m_queue1.m_start);
    if (m_queue2.m_start)
        free(m_queue2.m_start);
}


// ---- src/PSXRenderTarget/Create.cpp
typedef unsigned long u_long;

extern "C" void *memalign(unsigned int align, unsigned int size);
extern "C" void *memset(void *dst, int c, unsigned int n);


struct _sceDmaTag;
int GsSyncPath(int mode, GS_PATH path);

// VU microcode and DMA chains in the retail image.
extern char Vu0TransformNoLightNoClipCode[];
extern char Vu0MiscCode[];
extern char KickGSCode[];
extern char ScissorCoreCode[];









PSXRenderTarget *PSXRenderTarget::Create(PSXVideoCard *videoCard, int a)
{
    RenderTarget::Create(videoCard);
    LpRenderTarget = this;
    m_screenType = 0x22B;
    m_frameCount = 2;
    m_queue1.m_start = (_sceDmaTag *)memalign(0x40, 0x20000);
    m_queue1.m_cur = m_queue1.m_start;
    m_queue2.m_start = (_sceDmaTag *)memalign(0x40, 0x20000);
    m_queue2.m_cur = m_queue1.m_start;

    // A packet that flushes the VIF and kicks a one-register GIF tag.
    m_renderItem.Start((DmaRenderDestination)1, 0x80);
    m_renderItem.DmaAddVifCode(0x11000000);   // FLUSH
    m_renderItem.DmaAddVifCode(0x03000000);   // BASE 0
    m_renderItem.DmaAddVifCode(0x02000000);   // OFFSET 0
    m_renderItem.DmaBeginUnpack(0x320, 0xC);
    sceGifTag tag;
    memset(&tag, 0, sizeof(tag));
    tag.NLOOP = 1;
    tag.EOP = 1;
    tag.PRE = 1;
    tag.FLG = 0;
    tag.NREG = 1;
    tag.REGS0 = 0xF;
    m_renderItem.DmaAddGifTagToUnpack(&tag);
    m_renderItem.DmaEndUnpack();
    m_renderItem.DmaAddVifCode(0x14000000);   // MSCAL
    m_renderItem.DmaAddVifCode(0x10000000);
    m_renderItem.End();

    m_tags.Create(0x10, 0x10);
    m_width = videoCard->m_width;
    m_height = videoCard->m_height;
    m_packet = new DmaPacket(0xC, 1);
    m_x24 = 1.0f;
    Reset();
    DmaSend((DmaChannelType)1, Vu0TransformNoLightNoClipCode, 0, 0);
    DmaSend((DmaChannelType)1, Vu0MiscCode, 0, 0);
    DmaSend((DmaChannelType)2, KickGSCode, 0, 0);
    GsSyncPath(1, (GS_PATH)1);
    DmaSend((DmaChannelType)2, ScissorCoreCode, 0, 0);
    GsSyncPath(1, (GS_PATH)1);
    return this;
}


// ---- src/PSXRenderTarget/Reset.cpp

struct sceDmaChcr {
    unsigned char m_bits0_5 : 6;
    unsigned char m_bit6 : 1;
};
struct sceDmaChan {
    sceDmaChcr chcr;
};
extern "C" sceDmaChan *sceDmaGetChan(int id);

void PSXRenderTarget::Reset()
{
    m_dmaVif0 = sceDmaGetChan(0);
    m_dmaVif0->chcr.m_bit6 = 1;
    m_dmaVif1 = sceDmaGetChan(1);
    m_dmaVif1->chcr.m_bit6 = 1;
    m_dmaGif = sceDmaGetChan(2);
    m_dmaGif->chcr.m_bit6 = 1;
}


// ---- src/PSXRenderTarget/Clear.cpp

int PSXRenderTarget::Clear(_RECT *rect, unsigned int flags)
{
    m_x30 = 0;
    m_x34 = 0;
    m_x38 = 0;
    m_x3C = 0;
    if (!flags)
        flags = (m_hasDepth ? 1 : 0) | 2;
    if (rect)
        LpVideoCard->ClearFrameBuffer(rect, m_clearColor, flags);
    else
        LpVideoCard->ClearFrameBuffer(0, m_clearColor, flags);
    return 1;
}


// ---- src/PSXRenderTarget/BeginScene.cpp

extern Game *LpGame;
extern int GPUOverlayId;

struct _sceDmaTag;



int PSXRenderTarget::BeginScene()
{
    m_queueRunning = 1;
    ResetRenderQueue(1);
    ResetRenderQueue(2);
    m_tags.Clear();
    if (LpGame->m_debugOverlay) {
        if (GPUOverlayId < 0)
            GPUOverlayId = LpGame->m_debugOverlay->m_numPages++;
        LpGame->m_debugOverlay->PrintfHeading(GPUOverlayId, "GPU timing");
    }
    m_lastMPG = 0;
    return 1;
}


// ---- src/PSXRenderTarget/EndScene.cpp
extern PSXVideoCard *LpVideoCard;




// This file's page in the debug overlay, claimed on first use.
static int DebugOverlayID = -1;
int GPUOverlayId = -1;



int PSXRenderTarget::EndScene()
{
    LpVideoCard->AddVideoCardEffects();
    if (LpGame->m_debugOverlay) {
        if (DebugOverlayID < 0)
            DebugOverlayID = LpGame->m_debugOverlay->m_numPages++;
        LpGame->m_debugOverlay->PrintfHeading(DebugOverlayID, "PSX RT");
        LpGame->m_debugOverlay->PrintfDetail(DebugOverlayID, "nTotalQueueSize: %d", nTotalQueueSize);
        LpGame->m_debugOverlay->PrintfDetail(DebugOverlayID, "nTotalQueueSubmit: %d", nTotalQueueSubmit);
        LpGame->m_debugOverlay->PrintfDetail(DebugOverlayID, "");
        LpGame->m_debugOverlay->PrintfDetail(DebugOverlayID, "TotalRenderedStrips: %d", m_renderedStrips);
        LpGame->m_debugOverlay->PrintfDetail(DebugOverlayID, "TotalRenderedVertices: %d", m_renderedVertices);
    }
    nTotalQueueSize = 0;
    nTotalQueueSubmit = 0;
    m_renderedStrips = 0;
    m_renderedVertices = 0;
    if (IsRenderingComplete() == 1)
        TrySendRenderQueue(1);
    return 1;
}


// ---- src/PSXRenderTarget/Blt.cpp
// Blt__15PSXRenderTargetFP5_RECTP10TextureMapP5_RECTUi  @ 0x00274700
// Codegen notes: the passes rounding keeps retail's shape (`t = 0; if (...) t = passes;`
// then `t *= passes`, so the mult precedes the % div); `out = buffer` is set once per
// pass before the format tests and idx is built up in place (`idx += ...; idx *= ...`),
// both for retail's operand order and buffer's spill cost; buffer is declared before
// pass so their callee-saved registers come out as retail's.
#pragma fast_fptosi on


extern "C" void *malloc(unsigned int size);
extern "C" void free(void *p);

extern PSXVideoCard *LpVideoCard;
extern PSXTextureMapManager *LpTextureMapManager;

struct _D3DVIEWPORT8;
struct _D3DLIGHT8;

// Local copies of RenderTarget/PSXRenderTarget: the shared headers declare Blt
// void, but retail's returns int (1, or 0 for an unsupported pixel format).


static inline int FloatToInt(float f)
{
    int i;
    asm("cvt.w.s $f5, %1; mfc1 %0, $f5" : "=r"(i) : "f"(f) : "$f5");
    return i;
}

int PSXRenderTarget::Blt(_RECT *dst_, TextureMap *texture, _RECT *src, unsigned int flags)
{
    unsigned short *pixels;
    int rows;
    int y;
    int end;
    int dstWidth;
    int srcWidth;
    int srcHeight;
    _RECT *dst;
    PSXTextureMap *tex;
    int dstHeight;
    unsigned char *buffer;
    int pass;
    int useHardware;
    _RECT srcRect;
    _RECT dstRect;
    float xStep;
    float yStep;
    int passes;

    dst = dst_;
    useHardware = 1;
    tex = (PSXTextureMap *)texture;

    if (src) {
        srcWidth = src->right - src->left;
        srcHeight = src->bottom - src->top;
        srcRect.left = src->left;
        srcRect.right = src->right;
        srcRect.top = src->top;
        srcRect.bottom = src->bottom;
#line 771 "PSXRenderTarget.cpp"
        ASSERT(srcWidth <= tex->m_width, "Blt: dest rectangle is wider than the image");
        ASSERT(srcHeight <= tex->m_height, "Blt: dest rectangle is taller than the image");
    } else {
        srcWidth = tex->m_width;
        srcHeight = tex->m_height;
        srcRect.left = 0;
        srcRect.right = srcWidth;
        srcRect.top = 0;
        srcRect.bottom = srcHeight;
    }

    if (dst) {
        dstWidth = (int)(float)(dst->right - dst->left);
        dstHeight = (int)(float)(dst->bottom - dst->top);
        dstRect.left = (int)(float)dst->left;
        dstRect.right = (int)(float)dst->right;
        dstRect.top = (int)(float)dst->top;
        dstRect.bottom = (int)(float)dst->bottom;
    } else {
        dstWidth = LpVideoCard->m_width;
        dstHeight = LpVideoCard->m_height;
        dstRect.left = 0;
        dstRect.right = dstWidth;
        dstRect.top = 0;
        dstRect.bottom = dstHeight;
    }

    xStep = (float)srcWidth / (float)dstWidth;
    yStep = (float)srcHeight / (float)dstHeight;

    if (useHardware) {
        LpVideoCard->HardwareBlt(&dstRect, tex, &srcRect);
        return 1;
    }

    if (tex->m_format == 8888) {
        buffer = (unsigned char *)tex->GetTopMipLevel()->m_surface;
        LpTextureMapManager->m_gsTransfer->PrepareTransferTextureToFrameBuffer(buffer, srcWidth, srcHeight,
                                                                            dstRect.left, dstRect.top, 0);
        LpTextureMapManager->m_gsTransfer->SendTexture(1);
        buffer = 0;
    } else {
        pixels = (unsigned short *)tex->GetTopMipLevel()->m_surface;
        buffer = (unsigned char *)malloc(0x40000);
        passes = 1;
        rows = dstHeight;
        y = 0;
        end = dstHeight;
        if (dstWidth * dstHeight > 0x10000) {
            rows = 0x10000 / dstWidth;
            int t = 0;
            if (0x10000 % dstHeight)
                t = passes;
            passes = dstHeight / rows;
            t *= passes;
            passes += (t + dstHeight % rows) ? 1 : 0;
            end = rows;
        }
        for (pass = 0; pass < passes; pass++) {
            int row;
            int col;
            unsigned int *out;
            int idx;
            float fx;
            out = (unsigned int *)buffer;
            if (tex->m_format == 1555) {
                for (row = y; row < end; row++) {
                    idx = FloatToInt(row * yStep);
                    idx += srcRect.top;
                    idx *= tex->m_width;
                    idx += srcRect.left;
                    fx = idx;
                    for (col = 0; col < dstWidth; col++) {
                        unsigned short pixel;
                        fx += xStep;
                        pixel = pixels[idx];
                        unsigned int alpha;
                        idx = FloatToInt(fx);
                        alpha = (pixel & 0x8000) ? 0xFF000000 : 0;
                        *out = ((pixel << 3) & 0xF8) | (((pixel << 6) & 0xF800) | (((pixel << 9) & 0xF80000) | alpha));
                        out++;
                    }
                }
            } else if (tex->m_format == 555) {
                for (row = y; row < end; row++) {
                    idx = FloatToInt(row * yStep);
                    idx += srcRect.top;
                    idx *= tex->m_width;
                    idx += srcRect.left;
                    fx = idx;
                    for (col = 0; col < dstWidth; col++) {
                        unsigned short pixel;
                        fx += xStep;
                        pixel = pixels[idx];
                        idx = FloatToInt(fx);
                        *out = ((pixel << 3) & 0xF8) | (((pixel << 9) & 0xF80000) | ((pixel << 6) & 0xF800));
                        out++;
                    }
                }
            } else if (tex->m_format == 565) {
                for (row = y; row < end; row++) {
                    idx = FloatToInt(row * yStep);
                    idx += srcRect.top;
                    idx *= tex->m_width;
                    idx += srcRect.left;
                    fx = idx;
                    for (col = 0; col < dstWidth; col++) {
                        unsigned short pixel;
                        fx += xStep;
                        pixel = pixels[idx];
                        idx = FloatToInt(fx);
                        *out = ((pixel << 3) & 0xF8) | (((pixel << 8) & 0xF80000) | ((pixel << 5) & 0xF800));
                        out++;
                    }
                }
            } else {
#line 947 "PSXRenderTarget.cpp"
                ASSERTF(0, ("Error in PSXRenderTarget -- BLT does not support pixel format %d\n", tex->m_format));
                return 0;
            }
            FlushCache(0);
            LpTextureMapManager->m_gsTransfer->PrepareTransferTextureToFrameBuffer(buffer, dstWidth, rows,
                                                                                dstRect.left, dstRect.top + y, 0);
            LpTextureMapManager->m_gsTransfer->SendTexture(1);
            y += rows;
            if (end + rows > dstHeight)
                rows = dstHeight - end;
            end += rows;
        }
    }
    if (buffer)
        free(buffer);
    WaitForRenderingToComplete();
    return 1;
}
#pragma fast_fptosi reset


// ---- src/PSXRenderTarget/Lock.cpp
struct _RECT;


int PSXRenderTarget::Lock(_RECT *rect, long *pitch, unsigned int flags)
{
    return 0;
}


// ---- src/PSXRenderTarget/Unlock.cpp
struct _RECT;


int PSXRenderTarget::Unlock(_RECT *rect)
{
    return 0;
}


// ---- src/PSXRenderTarget/DmaSend.cpp
// ASSERT contains inline asm; match retail's label alignment for asm functions.
#pragma alignlabel off



int GsSyncPath(int mode, GS_PATH path);

#define D0_CHCR ((volatile unsigned int *)0x10008000)
#define D0_QWC  ((volatile unsigned int *)0x10008020)
#define D0_TADR ((volatile unsigned int *)0x10008030)
#define D1_CHCR ((volatile unsigned int *)0x10009000)
#define D1_QWC  ((volatile unsigned int *)0x10009020)
#define D1_TADR ((volatile unsigned int *)0x10009030)
#define D2_CHCR ((volatile unsigned int *)0x1000A000)
#define D2_QWC  ((volatile unsigned int *)0x1000A020)
#define D2_TADR ((volatile unsigned int *)0x1000A030)

// Start a source-chain DMA transfer on the given channel.
void PSXRenderTarget::DmaSend(DmaChannelType channel, void *chain, int flush, int)
{
    if (flush)
        FlushCache(0);
    switch (channel) {
    case DMA_CHANNEL_GIF:
        GsSyncPath(1, (GS_PATH)2);
        *D2_TADR = (unsigned int)chain;
        *D2_QWC = 0;
        *D2_CHCR = 0x105;
        break;
    case DMA_CHANNEL_VIF0:
        while (*D0_CHCR & 0x100)
            ;
        *D0_TADR = (unsigned int)chain & 0x0FFFFFFF;
        *D0_QWC = 0;
        *D0_CHCR = 0x1C5;
        break;
    case DMA_CHANNEL_VIF1:
        GsSyncPath(1, (GS_PATH)1);
        *D1_TADR = (unsigned int)chain & 0x0FFFFFFF;
        *D1_QWC = 0;
        *D1_CHCR = 0x1C5;
        break;
    default:
#line 1049 "PSXRenderTarget.cpp"
        ASSERT(0, "DmaSend: Invalid channel");
        break;
    }
}
#pragma alignlabel reset


// ---- src/PSXRenderTarget/AddToRenderQueue.cpp
// ASSERT contains inline asm; match retail's label alignment for asm functions.
#pragma alignlabel off




// Copies one quadword (the tag) with lq/sq.
static inline void CopyQword(void *dst, void *src)
{
    asm("lq $8, 0(%1); sq $8, 0(%0)" : : "r"(dst), "r"(src) : "$8", "memory");
}

class DmaRenderItem;

int SyncDmaPath(int path, int a, int b);

// Adds a render item's DMA tag (its first quadword) to the queue being built.
_sceDmaTag *PSXRenderTarget::AddToRenderQueue(DmaRenderItem *item, char *)
{
    if (LockRenderQueueState && m_finishCallback) {
        if (!SyncDmaPath(1, 0, 0))
            m_finishCallback();
    }
    if (m_buildQueue == 1) {
        if (m_queue2.m_size + 1 >= 0x2000) {
#line 1126 "PSXRenderTarget.cpp"
            ASSERT(0, "AddToRenderQueue: RenderQueueCurSize2 too large for render queue");
            return 0;
        }
        CopyQword(m_queue2.m_cur, item);
        m_queue2.m_cur++;
        m_queue2.m_size++;
        nTotalQueueSize++;
        return m_queue2.m_cur - 1;
    } else {
        if (m_queue1.m_size + 1 >= 0x2000) {
#line 1149 "PSXRenderTarget.cpp"
            ASSERT(0, "AddToRenderQueue: RenderQueueCurSize1 too large for render queue");
            return 0;
        }
        CopyQword(m_queue1.m_cur, item);
        m_queue1.m_cur++;
        m_queue1.m_size++;
        nTotalQueueSize++;
        return m_queue1.m_cur - 1;
    }
}
#pragma alignlabel reset


// ---- src/PSXRenderTarget/AddIndexedItemToRenderQueue.cpp
extern "C" void *memcpy(void *dst, const void *src, unsigned int n);

struct _sceDmaTag;
struct DmaRenderItem;


void PSXRenderTarget::AddIndexedItemToRenderQueue(_sceDmaTag *tag, DmaRenderItem *item)
{
    memcpy(tag, item, 16);
}


// ---- src/PSXRenderTarget/AddToRenderQueue_00275210.cpp
// ASSERT contains inline asm; match retail's label alignment for asm functions.
#pragma alignlabel off




// Copies one quadword (the tag) with lq/sq.

// Reserves count tags in the queue being built, copies the first one there
// and returns where it went.
_sceDmaTag *PSXRenderTarget::AddToRenderQueue(_sceDmaTag *tags, int count, char *)
{
    if (m_buildQueue == 1) {
        if (m_queue2.m_size + count >= 0x2000) {
#line 1188 "PSXRenderTarget.cpp"
            ASSERT(0, "AddToRenderQueue: RenderQueueCurSize2+numberOfTags too large for render queue");
            return 0;
        }
        CopyQword(m_queue2.m_cur, tags);
        m_queue2.m_cur += count;
        m_queue2.m_size += count;
        return m_queue2.m_cur - count;
    } else {
        if (m_queue1.m_size + count >= 0x2000) {
#line 1210 "PSXRenderTarget.cpp"
            ASSERT(0, "AddToRenderQueue: RenderQueueCurSize1+numberOfTags too large for render queue");
            return 0;
        }
        CopyQword(m_queue1.m_cur, tags);
        m_queue1.m_cur += count;
        m_queue1.m_size += count;
        return m_queue1.m_cur - count;
    }
}
#pragma alignlabel reset


// ---- src/PSXRenderTarget/AddMPGToRenderQueue.cpp
// ASSERT contains inline asm; match retail's label alignment for asm functions.
#pragma alignlabel off




// Copies one quadword (the tag) with lq/sq.

// Adds the tag for a movie frame's upload, once per frame packet.
_sceDmaTag *PSXRenderTarget::AddMPGToRenderQueue(unsigned int *packet)
{
    if (m_lastMPG == packet)
        return 0;
    m_lastMPG = packet;
    if (m_buildQueue == 1) {
        if (m_queue2.m_size + 1 >= 0x2000) {
#line 1244 "PSXRenderTarget.cpp"
            ASSERT(0, "AddMPGToRenderQueue: RenderQueueCurSize2 too large for render queue");
            return 0;
        }
        CopyQword(m_queue2.m_cur, packet);
        m_queue2.m_cur++;
        m_queue2.m_size++;
        return m_queue2.m_cur - 1;
    } else {
        if (m_queue1.m_size + 1 >= 0x2000) {
#line 1258 "PSXRenderTarget.cpp"
            ASSERT(0, "AddMPGToRenderQueue: RenderQueueCurSize1 too large for render queue");
            return 0;
        }
        CopyQword(m_queue1.m_cur, packet);
        m_queue1.m_cur++;
        m_queue1.m_size++;
        return m_queue1.m_cur - 1;
    }
}
#pragma alignlabel reset


// ---- src/PSXRenderTarget/ResetRenderQueue.cpp

void PSXRenderTarget::ResetRenderQueue(int queue)
{
    if (queue == 1) {
        m_queue1.m_size = 0;
        m_queue1.m_cur = m_queue1.m_start;
    } else {
        m_queue2.m_size = 0;
        m_queue2.m_cur = m_queue2.m_start;
    }
}


// ---- src/PSXRenderTarget/LockTrySendRenderQueue.cpp

void PSXRenderTarget::LockTrySendRenderQueue(int (*fn)())
{
    m_finishCallback = fn;
}


// ---- src/PSXRenderTarget/TrySendRenderQueue.cpp
extern PSXTextureMapManager *LpTextureMapManager;


int PSXRenderTarget::TrySendRenderQueue(int wait)
{
    if (!m_queueRunning)
        return 0;
    static int test = 0;
    if (test && !wait)
        return 0;
    static int TrySendID = -1;  // profiling timer id (unused)
    if (wait) {
        if (m_finishCallback) {
            while (m_finishCallback())
                ;
            m_finishCallback = 0;
        }
        WaitForRenderingToComplete();
    }
    if (IsRenderingComplete() == 1) {
        if (m_finishCallback) {
            if (m_finishCallback())
                return 0;
            m_finishCallback = 0;
        }
        ResetRenderQueue(m_buildQueue);
        m_sendQueue = m_buildQueue;
        m_buildQueue = (m_buildQueue == 1) ? 2 : 1;
        static int GIF1ID = -1;  // profiling timer ids (unused)
        LpTextureMapManager->ResetSendList();
        static int GIF2ID = -1;
        LpTextureMapManager->BeginTransfer();
        static int VIFID = -1;
        SendRenderQueue(m_buildQueue);
        LpTextureMapManager->ResetBuildList(m_sendQueue);
        return 1;
    }
    return 0;
}


// ---- src/PSXRenderTarget/SendRenderQueue.cpp




// Terminate the queue's DMA chain with an END tag and send it down PATH3.
void PSXRenderTarget::SendRenderQueue(int queue)
{
    if (!m_queueRunning)
        return;
    if (queue == 1) {
        if (m_queue1.m_size > 0) {
            m_x7C = 0;
            nTotalQueueSubmit += m_queue1.m_size;
            _sceDmaTag *tag = m_queue1.m_cur;
            tag->qwc = 0;
            tag->mark = 0;
            tag->id = 0x70;     // END
            tag->next = 0;
            tag->p[0] = 0;
            tag->p[1] = 0;
            FlushCache(0);
            DmaSend((DmaChannelType)2, m_queue1.m_start, 0, 0);
        }
    } else {
        if (m_queue2.m_size > 0) {
            m_x7C = 0;
            nTotalQueueSubmit += m_queue2.m_size;
            _sceDmaTag *tag = m_queue2.m_cur;
            tag->qwc = 0;
            tag->mark = 0;
            tag->id = 0x70;     // END
            tag->next = 0;
            tag->p[0] = 0;
            tag->p[1] = 0;
            FlushCache(0);
            DmaSend((DmaChannelType)2, m_queue2.m_start, 0, 0);
        }
    }
}


// ---- src/PSXRenderTarget/IsRenderingComplete.cpp

int PSXRenderTarget::IsRenderingComplete()
{
    static int IsRenderingCompleteID = -1;  // unused in retail too (a compiled-out profiling meter id?)
    if (GsSyncPath(0, (GS_PATH)3))
        return 0;
    return 1;
}


// ---- src/PSXRenderTarget/WaitForRenderingToComplete.cpp

void PSXRenderTarget::WaitForRenderingToComplete()
{
    GsSyncPath(1, (GS_PATH)3);
}


// ---- src/PSXRenderTarget/StopRenderQueue.cpp

void PSXRenderTarget::StopRenderQueue()
{
    WaitForRenderingToComplete();
    m_queueRunning = 0;
}


// ---- src/PSXRenderTarget/DrawIndexedPrimitive.cpp

int PSXRenderTarget::DrawIndexedPrimitive(int type, unsigned int fvf, void *verts, unsigned int numVerts,
                                          unsigned short *indices, unsigned int numIndices, unsigned int flags)
{
    return 0;
}


// ---- src/PSXRenderTarget/DrawPrimitive.cpp

int PSXRenderTarget::DrawPrimitive(int type, unsigned int fvf, void *verts, unsigned int numVerts, unsigned int flags)
{
    return 0;
}


// ---- src/PSXRenderTarget/GetTextureStageState.cpp

void PSXRenderTarget::GetTextureStageState(unsigned int stage, _D3DTEXTURESTAGESTATETYPE type, unsigned int *value)
{
    if (value)
        *value = 0;
}


// ---- src/PSXRenderTarget/SetTextureStageState.cpp

void PSXRenderTarget::SetTextureStageState(unsigned int stage, _D3DTEXTURESTAGESTATETYPE type, unsigned int value)
{
}


// ---- src/PSXRenderTarget/SetRenderState.cpp

void PSXRenderTarget::SetRenderState(_D3DRENDERSTATETYPE state, unsigned int value, int)
{
    switch (state) {
    case 0x24:  // D3DRS_FOGSTART
        m_fogStart = *(float *)&value;
        if (m_fogStart != m_fogEnd)
            m_fogScale = 255.0f / (m_fogStart - m_fogEnd);
        else
            m_fogScale = 0;
        break;
    case 0x25:  // D3DRS_FOGEND
        m_fogEnd = *(float *)&value;
        if (m_fogStart != m_fogEnd)
            m_fogScale = 255.0f / (m_fogStart - m_fogEnd);
        else
            m_fogScale = 0;
        break;
    }
}


// ---- src/PSXRenderTarget/GetRenderState.cpp

void PSXRenderTarget::GetRenderState(_D3DRENDERSTATETYPE state, unsigned int *value)
{
    if (value)
        *value = 0;
}


// ---- src/PSXRenderTarget/SetTextureState.cpp

void PSXRenderTarget::SetTextureState(_D3DTEXTUREBLEND blend, int value)
{
}


// ---- src/PSXRenderTarget/DisableTextureStage.cpp

void PSXRenderTarget::DisableTextureStage(unsigned int stage)
{
}


// ---- src/PSXRenderTarget/EnableAlphaTest.cpp

void PSXRenderTarget::EnableAlphaTest(unsigned char ref)
{
}


// ---- src/PSXRenderTarget/DisableAlphaTest.cpp

void PSXRenderTarget::DisableAlphaTest()
{
}


// ---- src/PSXRenderTarget/SetLight.cpp
struct _D3DLIGHT8;


void PSXRenderTarget::SetLight(unsigned int index, _D3DLIGHT8 *light)
{
}


// ---- src/PSXRenderTarget/LightEnable.cpp

void PSXRenderTarget::LightEnable(unsigned int index, int enable)
{
}


// ---- src/PSXRenderTarget/SetViewport.cpp
#pragma fast_fptosi on
typedef unsigned long u_long;
typedef int int128_t __attribute__((mode(TI)));

#define SCE_GS_SET_XYOFFSET(ofx, ofy) ((u_long)(ofx) | ((u_long)(ofy) << 32))
#define SCE_GS_SET_SCISSOR(scax0, scax1, scay0, scay1) \
    ((u_long)(scax0) | ((u_long)(scax1) << 16) | ((u_long)(scay0) << 32) | ((u_long)(scay1) << 48))
#define SCE_GS_SET_XYZ(x, y, z) ((u_long)(x) | ((u_long)(y) << 16) | ((u_long)(z) << 32))


// The queued form of SetViewport(_D3DVIEWPORT8 *): saves the camera's viewport
// in *previous, rescales the camera, and appends the same GS register writes to
// item as a VU1 A+D packet instead of sending them at once.
int PSXRenderTarget::SetViewport(_D3DVIEWPORT8 *viewport, DmaRenderItem *item, _D3DVIEWPORT8 *previous)
{
    PSXCamera *camera = (PSXCamera *)m_camera;
    if (previous)
        *previous = camera->m_viewport;
    if (camera) {
        int ofx = (int)(2048.0f - 0.5f * viewport->Width - viewport->X) << 4;
        int ofy = (int)(2048.0f - 0.5f * viewport->Height - viewport->Y) << 4;
        float yScale = viewport->Height / (float)(int)(camera->m_bottom - camera->m_top);
        float xScale = viewport->Width / (float)(int)(camera->m_right - camera->m_left);
        camera->m_x338 = xScale;
        camera->m_x33C = yScale;
        camera->m_viewport.X = viewport->X;
        camera->m_viewport.Y = viewport->Y;
        camera->m_viewport.Width = viewport->Width;
        camera->m_viewport.Height = viewport->Height;
        camera->UpdateMatrices();
        int left = viewport->X;
        int width = viewport->Width;
        int top = viewport->Y;
        int height = viewport->Height;
        int right = viewport->Width + viewport->X - 1;
        int bottom = viewport->Height + viewport->Y - 1;
        int x1 = ofx + (width << 4);
        int y1 = ofy + (height << 4);
        sceGifTag tag;
        *(int128_t *)&tag = 0;
        tag.FLG = 0;
        tag.EOP = 1;
        tag.NREG = 8;
        tag.NLOOP = 1;
        tag.REGS0 = 0xE;    // A+D
        tag.REGS1 = 0xE;
        tag.REGS2 = 0xE;
        tag.REGS3 = 0xE;
        tag.REGS4 = 0xE;
        tag.REGS5 = 0xE;
        tag.REGS6 = 0xE;
        tag.REGS7 = 0xE;
        item->DmaAddVifCode(0x11000000);    // FLUSH
        item->DmaAddVifCode(0x03000000);    // BASE
        item->DmaAddVifCode(0x02000000);    // OFFSET
        if ((item->m_dataListCurrentSize + 5) % 2)
            item->DmaAddVifCode(0);         // NOP: align the unpacked data
        item->DmaBeginUnpack(0x320, 12);
        item->DmaAddGifTagToUnpack(&tag);
        u_long *d = (u_long *)&item->m_dataList[item->m_dataListCurrentSize];
        d[0] = SCE_GS_SET_XYOFFSET(ofx, ofy);
        d[1] = 0x18;                        // XYOFFSET_1
        d[2] = SCE_GS_SET_SCISSOR(left, right, top, bottom);
        d[3] = 0x40;                        // SCISSOR_1
        d[4] = 0x32001;
        d[5] = 0x47;                        // TEST_1
        d[6] = 6;
        d[7] = 0;                           // PRIM
        d[8] = 0x3F80000080404000;          // 0x00,0x40,0x40,0x80, Q 1.0f
        d[9] = 1;                           // RGBAQ
        d[10] = SCE_GS_SET_XYZ(ofx, ofy, 0);
        d[11] = 4;                          // XYZ2
        d[12] = SCE_GS_SET_XYZ(x1, y1, 0);
        d[13] = 4;
        d[14] = 0x7000D;
        d[15] = 0x47;
        DmaRenderItem::UnpackCount += 0x20;
        item->m_dataListCurrentSize += 0x20;
        item->DmaEndUnpack();
        item->DmaAddVifCode(0x14000000);    // MSCAL
    }
    return 1;
}
#pragma fast_fptosi reset


// ---- src/PSXRenderTarget/SetViewport_00276050.cpp
#pragma fast_fptosi on
typedef unsigned long u_long;

#define SCE_GS_SET_XYOFFSET(ofx, ofy) ((u_long)(ofx) | ((u_long)(ofy) << 32))
#define SCE_GS_SET_SCISSOR(scax0, scax1, scay0, scay1) \
    ((u_long)(scax0) | ((u_long)(scax1) << 16) | ((u_long)(scay0) << 32) | ((u_long)(scay1) << 48))
#define SCE_GS_SET_XYZ(x, y, z) ((u_long)(x) | ((u_long)(y) << 16) | ((u_long)(z) << 32))


// Flushes the render queue, rescales the camera to the viewport and sends a
// GS packet: XYOFFSET/SCISSOR for the viewport and a sprite (RGBAQ 0x80404000,
// one 1.0f Q) covering it, then restores TEST_1.
// right/bottom read the viewport again rather than left/width (and x1/y1 are
// computed up front): that keeps them out of mwcc's forward substitution, so
// they are computed before the packet calls and spilled, as in retail.
int PSXRenderTarget::SetViewport(_D3DVIEWPORT8 *viewport)
{
    TrySendRenderQueue(1);
    WaitForRenderingToComplete();
    PSXCamera *camera = (PSXCamera *)m_camera;
    if (camera) {
        int ofx = (int)(2048.0f - 0.5f * viewport->Width - viewport->X) << 4;
        int ofy = (int)(2048.0f - 0.5f * viewport->Height - viewport->Y) << 4;
        float yScale = viewport->Height / (float)(int)(camera->m_bottom - camera->m_top);
        float xScale = viewport->Width / (float)(int)(camera->m_right - camera->m_left);
        camera->m_x338 = xScale;
        camera->m_x33C = yScale;
        int left = viewport->X;
        int width = viewport->Width;
        int top = viewport->Y;
        int height = viewport->Height;
        int right = viewport->Width + viewport->X - 1;
        int bottom = viewport->Height + viewport->Y - 1;
        int x1 = ofx + (width << 4);
        int y1 = ofy + (height << 4);
        m_packet->ResetPacket();
        m_packet->AddSourceDmaTag();
        m_packet->AddGifTag(1);
        m_packet->AddGifData(0x18, SCE_GS_SET_XYOFFSET(ofx, ofy));
        m_packet->AddGifData(0x40, SCE_GS_SET_SCISSOR(left, right, top, bottom));
        m_packet->AddGifData(0x47, 0x32001);
        m_packet->AddGifData(0x00, 0x6);
        m_packet->AddGifData(0x01, 0x3F80000080404000);   // RGBAQ: 0x00,0x40,0x40,0x80, Q 1.0f
        m_packet->AddGifData(0x04, SCE_GS_SET_XYZ(ofx, ofy, 0));
        m_packet->AddGifData(0x04, SCE_GS_SET_XYZ(x1, y1, 0));
        m_packet->AddGifData(0x47, 0x7000D);
        m_packet->AddDmaEndTag();
        m_packet->SendGifPacket(1, 1);
    }
    return 1;
}
#pragma fast_fptosi reset


// ---- src/PSXRenderTarget/PixelFormatAvailable.cpp

int PSXRenderTarget::PixelFormatAvailable(int format)
{
    if (format == 8)
        return 1;
    if (format == 0x613)
        return 1;
    if (format == 0x22B)
        return 1;
    return format == 0x22B8;
}
