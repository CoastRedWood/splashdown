// PSXVideoCard.cpp  @ 0x0027AF00 (text 0x27AF00-0x282760)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// PSXVideoCard as one translation unit without a static initializer, in
// PSXRenderTarget.cpp's tu_map row: the VSync timer handlers, PSXVideoCard
// and ContainerList<VideoPreFlipObject *>. Its .rodata restarts at @692 after
// the PSXTextureMap RTTI and ends with the VideoCard/ PSXVideoCard RTTI. The
// file name is a guess.



#include "ContainerList/ContainerList.h"
#define PSX_RENDER_TARGET_GLOBAL    // LpRenderTarget is a PSXRenderTarget * here (RenderTarget.h)
#include "PSXVideoCard/PSXVideoCard.h"
#include "DmaPacket/DmaPacket.h"
#include "DmaRenderItem/DmaRenderItem.h"
#include "GsTextureTransfer/GsSyncPath.h"
#include "Game/Game.h"
#include "_RECT/_RECT.h"
#include "Camera/Camera.h"
#include "RenderTarget/RenderTarget.h"
#include "PSXCamera/PSXCamera.h"
#include "soultree_assert_block.h"
#include "PSXTextureMap/PSXTextureMap.h"
#include "PSXTextureMapManager/PSXTextureMapManager.h"
#include "sceGifTag/sceGifTag.h"
#include "JetskiGame/JetskiGame.h"
#include "PSXRenderTarget/PSXRenderTarget.h"
#define CONTAINERLIST_MEMBERS_ALL
#include "ContainerList/ContainerListMembers.h"

// Defined here for its retail .bss slot.
volatile int VSyncComplete;


extern volatile int VSyncComplete;
extern Game *const LpGame;          // const: the fade functions read it once

// ---- src/global/VSyncTimerHandler.cpp

// GS vsync interrupt handler: count the frame, clear the VSINT bit in
// GS CSR and leave it unmasked in IMR, then re-enable interrupts.
int VSyncTimerHandler(int)
{
    VSyncComplete;  // retail reads the counter twice
#ifdef SPLASHDOWN_PC
    // the timer thread, beside the game thread's `VSyncComplete = 0`: one
    // atomic add (a load and a store could undo the reset)
    __atomic_add_fetch(&VSyncComplete, 1, __ATOMIC_SEQ_CST);
#else
    VSyncComplete = VSyncComplete + 1;
#endif
#ifdef SPLASHDOWN_PC
    // CSR's event bits are write-1-to-clear; on the PC the register is
    // memory, and this handler runs on the timer thread: a plain store of 8
    // cleared a FINISH the GS had just set while Flip waited for it (a hang)
    port_gs_csr_ack(8);
#else
    *(volatile unsigned long *)0x12001000 = 8;           // GS CSR: clear VSINT
#endif
    *(volatile unsigned long *)0x12001010 = 0xFFFFF7FF;  // GS IMR: unmask VSMSK
    asm("sync");
    asm("ei");
    return -1;
}


// ---- src/global/WaitForVSync.cpp
// mwcc session state: retail was compiled in one long compiler run in which
// an earlier 64-bit divide raised the helper-argument high-water mark; this
// unused shim recreates it.
static void MWCC_SHIM_TEXT MwccSessionState(long *a) { a[0] = a[0] / a[1]; }

extern int TimerOverflowCounterT0;

#define T0_COUNT ((volatile int *)0x10000000)

// Milliseconds from EE timer 0 (BUSCLK / 256 = 585.9375 ticks per ms).
inline float f_timeGetTime()
{
    static long Time = 0;
    static int lastCount = 0;
    int count;
    int elapsed;
    int overflows;
    count = *T0_COUNT;
    overflows = TimerOverflowCounterT0;
    TimerOverflowCounterT0 = 0;
    // Two statements in the first arm keep this a real if/else: as one
    // assignment per arm mwcc folds it into a ?: whose join label loses
    // retail's loop alignment padding (StaticFadeOut/In).
    if (lastCount <= count) {
        elapsed = count;
        elapsed -= lastCount;
    } else
        elapsed = 0x10000 - lastCount + count;
    if (!(lastCount < count) && overflows > 0)
        overflows--;
    lastCount = count;
    Time += elapsed + (overflows << 16);
    return 0.0017066667f * Time;
}

typedef unsigned long u_long;
#define GS_CSR ((volatile u_long *)0x12001000)
#define GS_IMR ((volatile u_long *)0x12001010)


// Waits until VSyncComplete reaches count. If that takes longer than 17 ms
// per frame, the GS is reset: SIGNAL event cleared and interrupts masked.
void WaitForVSync(int count)
{
    float start = 0.0f;
    float timeout = 17.0f * (count - VSyncComplete);
    while (VSyncComplete < count) {
#ifdef SPLASHDOWN_PC
        port_vsync_wait();
#endif
        if (start == 0.0f) {
            start = f_timeGetTime();
        } else if (f_timeGetTime() - start > timeout) {
#ifdef SPLASHDOWN_PC
            port_gs_csr_ack(8);
#else
            *GS_CSR = 8;
#endif
            *GS_IMR = 0xFFFFF7FF;
        }
    }
}


// ---- src/PSXVideoCard/ctor.cpp
extern "C" void *memset(void *dst, int c, unsigned int n);


PSXVideoCard::PSXVideoCard()
{
    int i;

    m_flipPaused = 0;
    m_lastFlipTime = 0;
    m_flipTime = 0;
    m_fpsFrameCount = 0;
    m_fpsTime = 0;
    m_fps = 0;
    m_frameRate = 0;
    m_frameRateAfterVSync = 0;
    m_x2D4 = 0;
    m_packet = 0;
    m_frameCount = 0;
    m_flipPending = 0;
    m_videoModes = 0;
    m_bufferFlip = 1;
    m_width = 0;
    m_height = 0;
    memset(&m_x48, 0, sizeof(_RECT));
    m_frameBufferPages = 0;
    m_zBufferAddress = 0;
    m_depth = 32;
    m_psm = 0;
    m_zpsm = 0x31;
    m_maxDepth = 0xFFFFFF;
    m_x44 = 0;
    m_x2EF_bit0 = 1;
    m_zmask = 0;
    m_x2EF_bit2 = 0;
    m_x2EC = 0;
    m_x2ED = 0;
    m_x2EE = 0x7F;
    m_alpha = 0x80;
    m_crossFade = 0;
    m_x408 = 0;
    m_magh = 4;
    m_shiftX = 0;
    m_shiftY = 0;
    m_x3D4 = 0;
    m_x3D8 = 0;
    for (i = 0; i < 4; i++) {
        m_viewportFaded[i] = 0;
        m_x300[i] = 0;
        m_viewportFadeTarget[i] = 0;
        m_viewportFadeLevel[i] = 0x80;
        m_viewportFading[i] = 0;
        m_viewportFadeOut[i] = 0;
        m_viewportFadeFrames[i] = 0;
        m_viewportFadeStartColor[i] = 0;
        m_viewportFadeEndColor[i] = 0x80000000;
        m_viewportFadeStepB[i] = 0;
        m_viewportFadeStepR[i] = 0;
        m_viewportFadeStepG[i] = 0;
        m_viewportFadeStart[i] = 0;
        memset(&m_viewportFadeRect[i], 0, sizeof(_RECT));
    }
    m_fadeTarget = 0x80;
    m_fadedOut = 0;
    m_fadeStartColor = 0x80000000;
    m_fadeEndColor = 0;
    m_fadeFrames = 0;
    m_fadeStepB = 0;
    m_fadeStepG = 0;
    m_fadeStepR = 0;
    m_fadeStart = 0;
    m_fadingOut = 0;
    m_fadingInFlag = 0;
    m_fadeFlag = 0;
    m_fadeDone = 0;
    m_fadingIn = 0;
    m_x490 = 0;
    for (i = 0; i < 4; i++) {
        m_colorOverlay[i] = 0;
        m_colorOverlayColor[i] = 0;
        m_overlayAlpha[i] = 0x8000000044;
        memset(&m_colorOverlayRect[i], 0, sizeof(_RECT));
    }
    m_x4E4 = 0;
    for (i = 0; i < 4; i++) {
        m_blurAlpha[i] = 0;
        m_blurRed[i] = 0;
        m_blurGreen[i] = 0;
        m_blurBlue[i] = 0;
        memset(&m_blurRect[i], 0, sizeof(_RECT));
    }
    m_vsyncTo30Hz = 2;
    m_defaultVSyncTo30Hz = 2;
}


// ---- src/PSXVideoCard/dtor.cpp
extern "C" int DisableIntc(int cause);
extern "C" int RemoveIntcHandler(int cause, int handlerId);




PSXVideoCard::~PSXVideoCard()
{
    DisableIntc(0);
    RemoveIntcHandler(0, 0);
    if (m_packet)
        delete m_packet;
    if (m_x3D4)
        delete m_x3D4;
    if (m_x3D8)
        delete m_x3D8;
    if (m_x490)
        delete m_x490;
    if (m_x4E4)
        delete m_x4E4;
}


// ---- src/PSXVideoCard/Create.cpp
extern "C" void *malloc(unsigned int size);
extern "C" int AddIntcHandler(int cause, int (*handler)(int), int next);
extern "C" int EnableIntc(int cause);

int VSyncTimerHandler(int cause);

#define GS_CSR ((volatile unsigned long *)0x12001000)
#define GS_IMR ((volatile unsigned long *)0x12001010)




#define SET_MODE(i, w, h, d, addr)    \
    m_videoModes[i].m_width = w;             \
    m_videoModes[i].m_height = h;            \
    m_videoModes[i].m_depth = d;             \
    m_videoModes[i].m_vramAddress = addr;    \
    m_videoModes[i].m_flags = 0;

PSXVideoCard *PSXVideoCard::Create()
{
    m_numModes = 10;
    m_mode = 4;
    m_videoModes = (PSXVideoMode *)malloc(m_numModes * sizeof(PSXVideoMode));
    SET_MODE(0, 256, 448, 32, 0x2B0000)
    SET_MODE(1, 320, 448, 32, 0x25C000)
    SET_MODE(2, 384, 448, 32, 0x208000)
    SET_MODE(3, 512, 448, 32, 0x160000)
    SET_MODE(4, 640, 448, 32, 0xB8000)
    SET_MODE(5, 256, 448, 16, 0x2B0000)
    SET_MODE(6, 320, 448, 16, 0x25C000)
    SET_MODE(7, 384, 448, 16, 0x208000)
    SET_MODE(8, 512, 448, 16, 0x160000)
    SET_MODE(9, 640, 448, 16, 0xB8000)
    m_x10 = 0x400000;
    m_packet = new DmaPacket(0x12, 1);
    m_x3D4 = new DmaRenderItem;
    m_x3D8 = new DmaRenderItem;
    m_x490 = new DmaRenderItem;
    m_x4E4 = new DmaRenderItem;
    AddIntcHandler(0, VSyncTimerHandler, -1);
    EnableIntc(0);
    *GS_CSR = 8;
    *GS_IMR = 0xFFFFF7FF;
    return this;
}


// ---- src/PSXVideoCard/Release.cpp
extern "C" void free(void *ptr);


void PSXVideoCard::Release()
{
    VideoCard::Release();
    if (m_videoModes) {
        free(m_videoModes);
        m_videoModes = 0;
    }
}


// ---- src/PSXVideoCard/SetVideoMode.cpp

int PSXVideoCard::SetVideoMode(int mode)
{
    if (mode < 0 || mode >= m_numModes)
        return 0;
    if (!SetVideoMode(m_videoModes[mode].m_width, m_videoModes[mode].m_height,
                      m_videoModes[mode].m_depth, 0, 0, 1, 0))
        return 0;
    m_mode = mode;
    return 1;
}


// ---- src/PSXVideoCard/SetVideoMode_0027BB30.cpp
struct _RECT;


int PSXVideoCard::SetVideoMode(int width, int height, int depth, int a, int b, int c, int d)
{
    int mode = FindVideoMode(width, height, depth);
    if (mode == -1)
        return 0;
    m_mode = mode;
    m_depth = depth;
    switch (mode) {
    case 0:
    case 5:
        m_magh = 10;
        break;
    case 1:
    case 6:
        m_magh = 8;
        break;
    case 2:
    case 7:
        m_magh = 7;
        break;
    case 3:
    case 8:
        m_magh = 5;
        break;
    case 4:
    case 9:
        m_magh = 4;
        break;
    }
    CreateDoubleBuffer(width, height, 1, 0);
    ClearVRAM(0, 0, 0x80, 0x40);
    SetDrawEnvironment(m_bufferFlip);
    ClearFrameBuffer(0, 0, 3);
    Flip(1);
    ClearFrameBuffer(0, 0, 3);
    Flip(1);
    return 1;
}


// ---- src/PSXVideoCard/FindVideoMode.cpp


int PSXVideoCard::FindVideoMode(int width, int height, int depth)
{
    for (int i = 0; i < m_numModes; i++) {
        if (depth == m_videoModes[i].m_depth && height == m_videoModes[i].m_height
            && width == m_videoModes[i].m_width)
            return i;
    }
    return -1;
}


// ---- src/PSXVideoCard/DisplayToScreen.cpp

int PSXVideoCard::DisplayToScreen()
{
    m_flipPending = Flip(1);
    return m_flipPending;
}


// ---- src/PSXVideoCard/Flip.cpp


void WaitForVSync(int count);
extern "C" int FlushCache(int operation);

#define GS_CSR ((volatile unsigned long *)0x12001000)


extern int TimerOverflowCounterT0;

#define T0_COUNT ((volatile int *)0x10000000)

// Milliseconds from EE timer 0 (BUSCLK / 256 = 585.9375 ticks per ms).

int PSXVideoCard::Flip(int wait)
{
    if (m_flipPaused)
        return 0;
    if (wait) {
        GsSyncPath(1, (GS_PATH)3);
    } else if (GsSyncPath(0, (GS_PATH)3)) {
        return 1;
    }
    ApplyPreFlipObjects();
    if (!m_fadeFlag && m_fadingOut) {
        StaticFadeOut();
        m_fadingIn = 1;
    }
    if (!m_fadeFlag && m_fadingInFlag) {
        StaticFadeIn();
        m_fadingIn = 0;
    }
    if (m_fadingIn)
        OverlayFrameBuffer(m_fadeEndColor);
    m_fadeDone = 0;
    m_x300[0] = 0;
    m_x300[1] = 0;
    m_x300[2] = 0;
    m_x300[3] = 0;
    if (m_crossFade) {
        CrossFadeBuffers();
        m_crossFade = 0;
    }
    *GS_CSR |= 2;
    m_packet->ResetPacket();
    m_packet->AddSourceDmaTag();
    m_packet->AddGifTag(1);
    m_packet->AddGifData(0x61, 1);
    m_packet->AddDmaEndTag();
    m_packet->SendGifPacket(1, 1);
    while (!(*GS_CSR & 2))
        ;
    m_lastFlipTime = m_flipTime;
    m_flipTime = f_timeGetTime();
    m_frameRate = 1000.0f / (m_flipTime - m_lastFlipTime);
    m_fpsTime += m_flipTime - m_lastFlipTime;
    if (m_fpsTime > 1000.0f) {
        m_fps = 1000.0f * (float)(m_frameCount - m_fpsFrameCount) / m_fpsTime;
        m_fpsFrameCount = m_frameCount;
        m_fpsTime = 0;
    }
    if (m_vsyncTo30Hz == 1) {
        if (VSyncComplete < 2)
            WaitForVSync(2);
    } else if (m_vsyncTo30Hz == 2) {
        if (!VSyncComplete)
            WaitForVSync(1);
    }
    m_flipTime = f_timeGetTime();
    m_frameRateAfterVSync = 1000.0f / (m_flipTime - m_lastFlipTime);
    VSyncComplete = 0;
    m_bufferFlip = m_frameCount & 1;
    SetDisplayEnvironment(m_bufferFlip);
    SetDrawEnvironment(m_bufferFlip);
    m_frameCount++;
    FlushCache(0);
    GsSyncPath(1, (GS_PATH)3);
    return 0;
}


// ---- src/PSXVideoCard/TryFlip.cpp

void PSXVideoCard::TryFlip(int wait)
{
    if (m_flipPending) {
        if (wait)
            m_flipPending = Flip(1);
        else
            m_flipPending = Flip(0);
    }
}


// ---- src/PSXVideoCard/PauseVideoCardFlip.cpp

void PSXVideoCard::PauseVideoCardFlip(int pause)
{
    m_flipPaused = pause;
}


// ---- src/PSXVideoCard/AddVideoCardEffects.cpp

void PSXVideoCard::AddVideoCardEffects()
{
    ExecuteMotionBlur();
    ExecuteColorOverlay();
    ExecuteDynamicViewportFading();
    ExecuteDynamicScreenFading();
}


// ---- src/PSXVideoCard/ExecuteMotionBlur.cpp



void PSXVideoCard::ExecuteMotionBlur()
{
    int viewport = LpGame->m_viewport;
    if (m_blurAlpha[viewport])
        MotionBlurBuffers(m_x4E4, &m_blurRect[viewport]);
}


// ---- src/PSXVideoCard/ExecuteColorOverlay.cpp


class DmaRenderItem;


void PSXVideoCard::ExecuteColorOverlay()
{
    int viewport = LpGame->m_viewport;
    if (m_colorOverlay[viewport])
        OverlayViewport(m_x490, m_colorOverlayColor[viewport], m_overlayAlpha[viewport],
                        &m_colorOverlayRect[viewport]);
}


// ---- src/PSXVideoCard/ExecuteDynamicViewportFading.cpp
#pragma fast_fptosi on



// Read with absolute loads, so declared const.


extern int TimerOverflowCounterT0;

#define T0_COUNT ((volatile int *)0x10000000)

// Milliseconds from EE timer 0 (BUSCLK / 256 = 585.9375 ticks per ms).

// Puts an alpha level on top of a BGR colour. As a helper rather than written
// out, it gives retail's `or` operand order.
static inline unsigned int SetAlpha(int alpha, int color) { return color | alpha << 24; }

void PSXVideoCard::ExecuteDynamicViewportFading()
{
    int viewport = LpGame->m_viewport;
    if (m_x300[viewport] == 1)
        return;
    if (m_viewportFaded[viewport] && !m_viewportFadeOut[viewport] && !m_viewportFading[viewport]) {
        OverlayViewport(m_x3D4, m_viewportFadeEndColor[viewport], 0x8000000044, &m_viewportFadeRect[viewport]);
        m_x300[viewport] = 1;
    }
    int steps;
    float now = f_timeGetTime();
    float elapsed = now - m_viewportFadeStart[viewport];
    if (m_viewportFadeOut[viewport]) {
        steps = (int)(elapsed / m_viewportFadeFrames[viewport]);
        if (steps > 0) {
            m_viewportFadeTarget[viewport] += steps;
            m_viewportFadeStart[viewport] = now;
        }
        if (m_viewportFadeTarget[viewport] > m_viewportFadeLevel[viewport])
            m_viewportFadeTarget[viewport] = m_viewportFadeLevel[viewport];
        int level = m_viewportFadeTarget[viewport];
        OverlayViewport(m_x3D4,
                        SetAlpha(level, m_viewportFadeStartColor[viewport]
                                        + ((int)(m_viewportFadeStepB[viewport] * level) << 16)
                                        + ((int)(m_viewportFadeStepG[viewport] * level) << 8)
                                        + (int)(m_viewportFadeStepR[viewport] * level)),
                        0x8000000044, &m_viewportFadeRect[viewport]);
        if (m_viewportFadeTarget[viewport] == m_viewportFadeLevel[viewport]) {
            m_viewportFaded[viewport] = 1;
            m_viewportFadeOut[viewport] = 0;
        }
        m_x300[viewport] = 1;
    } else if (m_viewportFading[viewport]) {
        steps = (int)(elapsed / m_viewportFadeFrames[viewport]);
        if (steps > 0) {
            m_viewportFadeTarget[viewport] -= steps;
            m_viewportFadeStart[viewport] = now;
        }
        if (m_viewportFadeTarget[viewport] < m_viewportFadeLevel[viewport])
            m_viewportFadeTarget[viewport] = m_viewportFadeLevel[viewport];
        int level = m_viewportFadeTarget[viewport];
        OverlayViewport(m_x3D4,
                        SetAlpha(level, m_viewportFadeStartColor[viewport]
                                        + ((int)(m_viewportFadeStepB[viewport] * level) << 16)
                                        + ((int)(m_viewportFadeStepG[viewport] * level) << 8)
                                        + (int)(m_viewportFadeStepR[viewport] * level)),
                        0x8000000044, &m_viewportFadeRect[viewport]);
        if (m_viewportFadeTarget[viewport] == m_viewportFadeLevel[viewport]) {
            m_viewportFaded[viewport] = 0;
            m_viewportFading[viewport] = 0;
        }
        m_x300[viewport] = 1;
    } else {
        m_viewportFadeStart[viewport] = now;
    }
}
#pragma fast_fptosi reset


// ---- src/PSXVideoCard/ExecuteDynamicScreenFading.cpp
#pragma fast_fptosi on



// Read with absolute loads, so declared const.

class DmaRenderItem;


extern int TimerOverflowCounterT0;

#define T0_COUNT ((volatile int *)0x10000000)

// Milliseconds from EE timer 0 (BUSCLK / 256 = 585.9375 ticks per ms).

// Puts an alpha level on top of a BGR colour. As a helper rather than written
// out, it gives retail's `or` operand order.

void PSXVideoCard::ExecuteDynamicScreenFading()
{
    if (LpGame->m_viewport < LpGame->m_numCameras - 1)
        return;
    if (m_fadeDone == 1)
        return;
    if (m_fadedOut && !m_fadingInFlag && !m_fadingOut) {
        OverlayViewport(m_x3D8, m_fadeEndColor, 0x8000000044, &m_x48);
        m_fadeDone = 1;
    }
    int steps;
    float now = f_timeGetTime();
    float elapsed = now - m_fadeStart;
    if (m_fadingOut && m_fadeFlag) {
        steps = (int)(elapsed / m_fadeFrames);
        if (steps > 0) {
            m_fadeLevel += steps;
            m_fadeStart = now;
        }
        if (m_fadeLevel > m_fadeTarget)
            m_fadeLevel = m_fadeTarget;
        int level = m_fadeLevel;
        OverlayViewport(m_x3D8,
                        SetAlpha(level, m_fadeStartColor + ((int)(m_fadeStepB * level) << 16)
                                        + ((int)(m_fadeStepG * level) << 8) + (int)(m_fadeStepR * level)),
                        0x8000000044, &m_x48);
        if (m_fadeLevel == m_fadeTarget) {
            m_fadedOut = 1;
            m_fadingOut = 0;
        }
        m_fadeDone = 1;
    } else if (m_fadingInFlag && m_fadeFlag) {
        steps = (int)(elapsed / m_fadeFrames);
        if (steps > 0) {
            m_fadeLevel -= steps;
            m_fadeStart = now;
        }
        if (m_fadeLevel < m_fadeTarget)
            m_fadeLevel = m_fadeTarget;
        int level = m_fadeLevel;
        OverlayViewport(m_x3D8,
                        SetAlpha(level, m_fadeStartColor + ((int)(m_fadeStepB * level) << 16)
                                        + ((int)(m_fadeStepG * level) << 8) + (int)(m_fadeStepR * level)),
                        0x8000000044, &m_x48);
        if (m_fadeLevel == m_fadeTarget) {
            m_fadedOut = 0;
            m_fadingIn = 0;
            m_fadingInFlag = 0;
        }
        m_fadeDone = 1;
    } else {
        m_fadeStart = now;
    }
}
#pragma fast_fptosi reset


// ---- src/PSXVideoCard/CreateDoubleBuffer.cpp
struct sceGsGParam {
    short sceGsInterMode;
    short sceGsOutMode;
    short sceGsFFMode;
    short sceGsVersion;
};

extern "C" int sceGsResetGraph(short mode, short inter, short omode, short ffmode);
extern "C" sceGsGParam *sceGsGetGParam(void);


int PSXVideoCard::CreateDoubleBuffer(int width, int height, int interlace, int frameMode)
{
    m_width = width;
    m_height = height;
    m_x48.top = 0;
    m_x48.left = 0;
    m_x48.bottom = height;
    m_x48.right = width;
    if (interlace)
        m_interlace = 1;
    else
        m_interlace = 0;
    if (frameMode)
        m_ffmd = 1;
    else
        m_ffmd = 0;
    sceGsResetGraph(0, m_interlace, 2, m_ffmd);
    sceGsGetGParam()->sceGsInterMode = m_interlace;
    m_offsetX = (2048 - m_width / 2) << 4;
    m_offsetY = (2048 - m_height / 2) << 4;
    if (m_depth == 32) {
        m_psm = 0;
        m_zpsm = 0x31;
        m_maxDepth = 0xFFFFFF;
        m_frameBufferPages = m_width * m_height / 2048;
        m_zBufferAddress = m_frameBufferPages + m_width * m_height / 2048;
    } else {
        m_psm = 10;
        m_zpsm = 0x31;
        m_maxDepth = 0xFFFF;
        m_frameBufferPages = m_width * m_height / 2048 / 2;
        m_zBufferAddress = m_frameBufferPages + m_width * m_height / 2048 / 2;
    }
    return 1;
}


// ---- src/PSXVideoCard/SetDisplayEnvironment.cpp
typedef unsigned long u_long;

#define GS_PMODE    ((volatile u_long *)0x12000000)
#define GS_SMODE2   ((volatile u_long *)0x12000020)
#define GS_DISPFB1  ((volatile u_long *)0x12000070)
#define GS_DISPLAY1 ((volatile u_long *)0x12000080)
#define GS_DISPFB2  ((volatile u_long *)0x12000090)
#define GS_DISPLAY2 ((volatile u_long *)0x120000A0)
#define GS_BGCOLOR  ((volatile u_long *)0x120000E0)

#define SCE_GS_SET_DISPFB(fbp, fbw, psm, dbx, dby) \
    ((u_long)(fbp) | ((u_long)(fbw) << 9) | ((u_long)(psm) << 15) | ((u_long)(dbx) << 32) | ((u_long)(dby) << 43))
#define SCE_GS_SET_DISPLAY(dx, dy, magh, magv, dw, dh) \
    ((u_long)(dx) | ((u_long)(dy) << 12) | ((u_long)(magh) << 23) | ((u_long)(magv) << 27) | ((u_long)(dw) << 32) | ((u_long)(dh) << 44))

static inline int MIN(int a, int b) { return a < b ? a : b; }
static inline int MAX(int a, int b) { return a > b ? a : b; }


// Programs the GS display registers (PMODE, SMODE2, DISPFB/DISPLAY 1 and 2,
// BGCOLOR) for the current display buffer.
int PSXVideoCard::SetDisplayEnvironment(short)
{
    int fbp;
    int ofsX;
    int ofsY;
    int height;
    unsigned int alpha;
    fbp = GetCurrentDisplayBufferAddress();
    height = m_height;
    ofsX = 0;
    ofsY = 0;

    if (m_x2EF_bit2) {
        alpha = m_alpha;
    } else {
        alpha = 0x80;
        if (m_x44)
            ofsY = ofsX = 1;
    }
    m_shiftX = MIN(MAX(m_shiftX, -50), 50);
    m_shiftY = MIN(MAX(m_shiftY, -50), 50);
    if (m_x2EF_bit0)
        *GS_PMODE = (u_long)(int)alpha << 8 | 0x63;
    *GS_SMODE2 = (u_long)m_interlace | (u_long)m_ffmd << 1;
    if (!m_x2EF_bit2)
        *GS_DISPFB1 = SCE_GS_SET_DISPFB(fbp, m_width >> 6, m_psm, 0, 0);
    *GS_DISPFB2 = SCE_GS_SET_DISPFB(fbp, m_width >> 6, m_psm, 0, 0);
    if (!m_x2EF_bit2)
        *GS_DISPLAY1 = SCE_GS_SET_DISPLAY(m_magh * (m_shiftX + ofsX) + 636, m_shiftY + ofsY + 50,
                                          m_magh - 1, 0, (m_width - ofsX - 1) * m_magh, height - ofsY - 1);
    *GS_DISPLAY2 = SCE_GS_SET_DISPLAY(m_shiftX * m_magh + 636, m_shiftY + 50,
                                      m_magh - 1, 0, (m_width - 1) * m_magh, height - 1);
    *GS_BGCOLOR = 0;
    asm("sync");
    return 1;
}


// ---- src/PSXVideoCard/SetDrawEnvironment.cpp
typedef unsigned long u_long;

#define SCE_GS_SET_FRAME(fbp, fbw, psm, fbmask) \
    ((u_long)(fbp) | ((u_long)(fbw) << 16) | ((u_long)(psm) << 24) | ((u_long)(fbmask) << 32))
#define SCE_GS_SET_ZBUF(zbp, psm, zmsk) \
    ((u_long)(zbp) | ((u_long)(psm) << 24) | ((u_long)(zmsk) << 32))
#define SCE_GS_SET_XYOFFSET(ofx, ofy) ((u_long)(ofx) | ((u_long)(ofy) << 32))
#define SCE_GS_SET_SCISSOR(scax0, scax1, scay0, scay1) \
    ((u_long)(scax0) | ((u_long)(scax1) << 16) | ((u_long)(scay0) << 32) | ((u_long)(scay1) << 48))



int PSXVideoCard::SetDrawEnvironment(short)
{
    int fbp = GetCurrentFrameBufferAddress();
    int offsetY = m_offsetY;
    m_packet->ResetPacket();
    m_packet->AddSourceDmaTag();
    m_packet->AddGifTag(1);
    m_packet->AddGifData(0x4C, SCE_GS_SET_FRAME(fbp, m_width >> 6, m_psm, 0));
    m_packet->AddGifData(0x4E, SCE_GS_SET_ZBUF(m_zBufferAddress, m_zpsm, m_zmask));
    m_packet->AddGifData(0x18, SCE_GS_SET_XYOFFSET(m_offsetX, offsetY));
    m_packet->AddGifData(0x40, SCE_GS_SET_SCISSOR(0, m_width - 1, 0, m_height - 1));
    m_packet->AddGifData(0x1A, 1);                          // PRMODECONT
    if (m_psm == 0)
        m_packet->AddGifData(0x45, 0);                      // DTHE off for PSMCT32
    else
        m_packet->AddGifData(0x45, 1);
    m_packet->AddGifData(0x46, 1);                          // COLCLAMP
    m_packet->AddGifData(0x47, 0x7000D);                    // TEST_1
    m_packet->AddDmaEndTag();
    m_packet->SendGifPacket(1, 1);
    return 1;
}


// ---- src/PSXVideoCard/ClearFrameBuffer.cpp
// Clears `rect` (or the whole frame buffer) to color with a sprite; mode picks the
// TEST register value (2 and 1 also write Z). Retail built this function without the global optimizer (each u_long
// conversion is redone, nothing is CSE'd across the packet calls).
#pragma global_optimizer off
typedef unsigned long u_long;

#define SCE_GS_SET_XYOFFSET(ofx, ofy) ((u_long)(ofx) | ((u_long)(ofy) << 32))
#define SCE_GS_SET_SCISSOR(scax0, scax1, scay0, scay1) \
    ((u_long)(scax0) | ((u_long)(scax1) << 16) | ((u_long)(scay0) << 32) | ((u_long)(scay1) << 48))
#define SCE_GS_SET_RGBAQ(r, g, b, a, q) \
    ((u_long)(r) | ((u_long)(g) << 8) | ((u_long)(b) << 16) | ((u_long)(a) << 24) | ((u_long)(q) << 32))
#define SCE_GS_SET_XYZ(x, y, z) ((u_long)(x) | ((u_long)(y) << 16) | ((u_long)(z) << 32))
#define SCE_GS_SET_ZBUF(zbp, psm, zmsk) ((u_long)(zbp) | ((u_long)(psm) << 24) | ((u_long)(zmsk) << 32))






int PSXVideoCard::ClearFrameBuffer(_RECT *rect, unsigned int color, unsigned int mode)
{
    unsigned char r;
    unsigned char g;
    int ofsX;
    int ofsY;
    int left;
    int top;
    int x0;
    int y0;
    int x1;
    int y1;
    int right;
    int bottom;

    _RECT viewport = {0, 0, 640, 448};
    if (LpRenderTarget) {
        if (LpRenderTarget->m_camera)
            LpRenderTarget->m_camera->GetViewportBoundsPhysical(&viewport);
        int vbottom, center, vleft, vright, vtop;
        center = 0x800;
        vleft = viewport.left;
        vright = viewport.right;
        vtop = viewport.top;
        vbottom = viewport.bottom;
        ofsX = (center - ((vright - vleft) >> 1) - vleft) << 4;
        ofsY = (center - ((vbottom - vtop) >> 1) - vtop) << 4;
    }
    if (rect) {
        int center;
        int rright;
        int rbottom;
        int mask;
        left = rect->left;
        center = 0x800;
        rright = rect->right;
        top = rect->top;
        mask = ~0xF;
        rbottom = rect->bottom;
        x0 = (center - ((rright - left) >> 1) - left) << 4;
        y0 = (center - ((rbottom - top) >> 1) - top) << 4;
        x1 = x0 + ((rright << 4) & mask);
        y1 = y0 + ((rbottom << 4) & mask);
        right = rright - 1;
        bottom = rbottom - 1;
    } else {
        int width;
        int height;
        width = m_width;
        x0 = m_offsetX;
        height = m_height;
        y0 = m_offsetY;
        left = 0;
        top = 0;
        x1 = x0 + (width << 4);
        y1 = y0 + (height << 4);
        right = width - 1;
        bottom = height - 1;
    }
    r = color >> 16;
    g = color >> 8;
    unsigned char b = color;
    m_packet->ResetPacket();
    m_packet->AddSourceDmaTag();
    m_packet->AddGifTag(1);
    if (mode == 2)
        m_packet->AddGifData(0x47, 0x32001);
    else if (mode == 1)
        m_packet->AddGifData(0x47, 0x31001);
    else
        m_packet->AddGifData(0x47, 0x3000D);
    if (m_zpsm == 0x31)
        m_packet->AddGifData(0x4E, SCE_GS_SET_ZBUF(m_zBufferAddress, 0x30, 0));
    m_packet->AddGifData(0x18, SCE_GS_SET_XYOFFSET(x0, y0));
    m_packet->AddGifData(0x40, SCE_GS_SET_SCISSOR(left, right, top, bottom));
    m_packet->AddGifData(0x00, 0x6);
    // A and Q written as one folded constant (0x80 << 24 | 1.0f << 32).
    m_packet->AddGifData(0x01, (u_long)r | ((u_long)g << 8) | ((u_long)b << 16) | (((u_long)0x80 << 24) | ((u_long)0x3F800000 << 32)));
    m_packet->AddGifData(0x04, SCE_GS_SET_XYZ(x0, y0, 0));
    m_packet->AddGifData(0x04, SCE_GS_SET_XYZ(x1, y1, 0));
    m_packet->AddGifData(0x47, 0x7000D);
    m_packet->AddGifData(0x18, SCE_GS_SET_XYOFFSET(ofsX, ofsY));
    m_packet->AddGifData(0x40, SCE_GS_SET_SCISSOR(viewport.left, viewport.right - 1, viewport.top, viewport.bottom - 1));
    int psm = m_zpsm;
    if (psm == 0x31)
        m_packet->AddGifData(0x4E, SCE_GS_SET_ZBUF(m_zBufferAddress, psm, 0));

    m_packet->AddDmaEndTag();
    m_packet->SendGifPacket(1, 1);
    return 1;
}
#pragma global_optimizer reset


// ---- src/PSXVideoCard/ClearFrameBuffer_0027D5F0.cpp
// ClearFrameBuffer__12PSXVideoCardFP5_RECTP9PSXCameraUiUi  @ 0x0027D5F0
// Same body as ClearFrameBuffer(_RECT *, Ui, Ui) with the viewport taken from `camera`.
// Retail reads viewport.right/top/bottom through computed addresses (addiu sp+N; lw 0):
// int& locals reproduce that. Where each reference is bound matters in this optimizer-off
// function: binding `vt` and `vb` right before their first use keeps the bottom load after
// the ofsX store (bound early, it is hoisted), and the temporaries vright/vleft/vtop take
// $a0/$a1/$a2 in declaration order.
// Clears `rect` (or the whole frame buffer) to color with a sprite; mode picks the
// TEST register value (2 and 1 also write Z). Retail built this function without the global optimizer (each u_long
// conversion is redone, nothing is CSE'd across the packet calls).
#pragma global_optimizer off
typedef unsigned long u_long;

#define SCE_GS_SET_XYOFFSET(ofx, ofy) ((u_long)(ofx) | ((u_long)(ofy) << 32))
#define SCE_GS_SET_SCISSOR(scax0, scax1, scay0, scay1) \
    ((u_long)(scax0) | ((u_long)(scax1) << 16) | ((u_long)(scay0) << 32) | ((u_long)(scay1) << 48))
#define SCE_GS_SET_RGBAQ(r, g, b, a, q) \
    ((u_long)(r) | ((u_long)(g) << 8) | ((u_long)(b) << 16) | ((u_long)(a) << 24) | ((u_long)(q) << 32))
#define SCE_GS_SET_XYZ(x, y, z) ((u_long)(x) | ((u_long)(y) << 16) | ((u_long)(z) << 32))
#define SCE_GS_SET_ZBUF(zbp, psm, zmsk) ((u_long)(zbp) | ((u_long)(psm) << 24) | ((u_long)(zmsk) << 32))






int PSXVideoCard::ClearFrameBuffer(_RECT *rect, PSXCamera *camera, unsigned int color, unsigned int mode)
{
    unsigned char r;
    unsigned char g;
    int ofsX;
    int ofsY;
    int left;
    int top;
    int x0;
    int y0;
    int x1;
    int y1;
    int right;
    int bottom;
    int center;

    _RECT viewport = {0, 0, 640, 448};
    int &vr = viewport.right;
    if (camera)
        camera->GetViewportBoundsPhysical(&viewport);
    int vright;
    int vleft;
    int vtop;
    vright = vr;
    vleft = viewport.left;
    center = 0x800;
    int &vt = viewport.top;
    vtop = vt;
    ofsX = (center - ((vright - vleft) >> 1) - vleft) << 4;
    int &vb = viewport.bottom;
    ofsY = (center - ((vb - vtop) >> 1) - vtop) << 4;
    if (rect) {
        int rright;
        int rbottom;
        int mask;
        left = rect->left;
        rright = rect->right;
        top = rect->top;
        mask = ~0xF;
        rbottom = rect->bottom;
        x0 = (center - ((rright - left) >> 1) - left) << 4;
        y0 = (center - ((rbottom - top) >> 1) - top) << 4;
        x1 = x0 + ((rright << 4) & mask);
        y1 = y0 + ((rbottom << 4) & mask);
        right = rright - 1;
        bottom = rbottom - 1;
    } else {
        int width;
        int height;
        width = m_width;
        x0 = m_offsetX;
        height = m_height;
        y0 = m_offsetY;
        left = 0;
        top = 0;
        x1 = x0 + (width << 4);
        y1 = y0 + (height << 4);
        right = width - 1;
        bottom = height - 1;
    }
    r = color >> 16;
    g = color >> 8;
    unsigned char b = color;
    m_packet->ResetPacket();
    m_packet->AddSourceDmaTag();
    m_packet->AddGifTag(1);
    if (mode == 2)
        m_packet->AddGifData(0x47, 0x32001);
    else if (mode == 1)
        m_packet->AddGifData(0x47, 0x31001);
    else
        m_packet->AddGifData(0x47, 0x3000D);
    if (m_zpsm == 0x31)
        m_packet->AddGifData(0x4E, SCE_GS_SET_ZBUF(m_zBufferAddress, 0x30, 0));
    m_packet->AddGifData(0x18, SCE_GS_SET_XYOFFSET(x0, y0));
    m_packet->AddGifData(0x40, SCE_GS_SET_SCISSOR(left, right, top, bottom));
    m_packet->AddGifData(0x00, 0x6);
    // A and Q written as one folded constant (0x80 << 24 | 1.0f << 32).
    m_packet->AddGifData(0x01, (u_long)r | ((u_long)g << 8) | ((u_long)b << 16) | (((u_long)0x80 << 24) | ((u_long)0x3F800000 << 32)));
    m_packet->AddGifData(0x04, SCE_GS_SET_XYZ(x0, y0, 0));
    m_packet->AddGifData(0x04, SCE_GS_SET_XYZ(x1, y1, 0));
    m_packet->AddGifData(0x47, 0x7000D);
    m_packet->AddGifData(0x18, SCE_GS_SET_XYOFFSET(ofsX, ofsY));
    m_packet->AddGifData(0x40, SCE_GS_SET_SCISSOR(viewport.left, vr - 1, vt, vb - 1));
    int psm = m_zpsm;
    if (psm == 0x31)
        m_packet->AddGifData(0x4E, SCE_GS_SET_ZBUF(m_zBufferAddress, psm, 0));

    m_packet->AddDmaEndTag();
    m_packet->SendGifPacket(1, 1);
    return 1;
}
#pragma global_optimizer reset


// ---- src/PSXVideoCard/ClearVRAM.cpp
typedef unsigned long u_long;

#define SCE_GS_SET_RGBAQ(r, g, b, a, q) \
    ((u_long)(r) | ((u_long)(g) << 8) | ((u_long)(b) << 16) | \
    ((u_long)(a) << 24) | ((u_long)(q) << 32))



// Fills the whole of VRAM (a 1024x1024 sprite) with one colour.
void PSXVideoCard::ClearVRAM(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    m_packet->ResetPacket();
    m_packet->AddSourceDmaTag();
    m_packet->AddGifTag(1);
    m_packet->AddGifData(0x1A, 1);                          // PRMODECONT
    m_packet->AddGifData(0x4E, 0x130000000UL);              // ZBUF_1: Z writes masked
    m_packet->AddGifData(0x4C, 0x100000);                   // FRAME_1: FBW 16
    m_packet->AddGifData(0x18, 0x600000006000UL);           // XYOFFSET_1
    m_packet->AddGifData(0x40, 0x03FF000003FF0000UL);       // SCISSOR_1
    m_packet->AddGifData(0x47, 0x30013);                    // TEST_1
    m_packet->AddGifData(0x00, 6);                          // PRIM: sprite
    m_packet->AddGifData(0x01, SCE_GS_SET_RGBAQ(r, g, b, a, 0x3F800000));
    m_packet->AddGifData(0x04, 0x60006000);                 // XYZ2
    m_packet->AddGifData(0x04, 0xA000A000);                 // XYZ2
    m_packet->AddDmaEndTag();
    m_packet->SendGifPacket(1, 1);
}


// ---- src/PSXVideoCard/HardwareBlt.cpp
// Retail compiled this without the global optimizer (#pragma below): that keeps
// the 0x3800 CLUT address as an int that TEX0 sign-extends, and the PRIM bits
// folded apart from abe.
#pragma global_optimizer off
#pragma fast_fptosi on
typedef unsigned long u_long;

#define SCE_GS_SET_PRIM(prim, iip, tme, fge, abe, aa1, fst, ctxt, fix) \
    ((u_long)(prim) | ((u_long)(iip) << 3) | ((u_long)(tme) << 4) | ((u_long)(fge) << 5) | \
     ((u_long)(abe) << 6) | ((u_long)(aa1) << 7) | ((u_long)(fst) << 8) | ((u_long)(ctxt) << 9) | \
     ((u_long)(fix) << 10))
#define SCE_GS_SET_ALPHA(a, b, c, d, fix) \
    ((u_long)(a) | ((u_long)(b) << 2) | ((u_long)(c) << 4) | ((u_long)(d) << 6) | ((u_long)(fix) << 32))
#define SCE_GS_SET_TEX0(tbp, tbw, psm, tw, th, tcc, tfx, cbp, cpsm, csm, csa, cld) \
    ((u_long)(tbp) | ((u_long)(tbw) << 14) | ((u_long)(psm) << 20) | ((u_long)(tw) << 26) | \
     ((u_long)(th) << 30) | ((u_long)(tcc) << 34) | ((u_long)(tfx) << 35) | ((u_long)(cbp) << 37) | \
     ((u_long)(cpsm) << 51) | ((u_long)(csm) << 55) | ((u_long)(csa) << 56) | ((u_long)(cld) << 61))
#define SCE_GS_SET_TEX1(lcm, mxl, mmag, mmin, mtba, l, k) \
    ((u_long)(lcm) | ((u_long)(mxl) << 2) | ((u_long)(mmag) << 5) | ((u_long)(mmin) << 6) | \
     ((u_long)(mtba) << 9) | ((u_long)(l) << 19) | ((u_long)(k) << 32))
#define SCE_GS_SET_UV(u, v) ((u_long)(u) | ((u_long)(v) << 16))
#define SCE_GS_SET_XYZ(x, y, z) ((u_long)(x) | ((u_long)(y) << 16) | ((u_long)(z) << 32))
#define SCE_GS_SET_RGBAQ(r, g, b, a, q) \
    ((u_long)(r) | ((u_long)(g) << 8) | ((u_long)(b) << 16) | ((u_long)(a) << 24) | ((u_long)(q) << 32))

extern "C" double ldexp(double x, int exp);
extern "C" double log(double x);

extern PSXTextureMapManager *LpTextureMapManager;

// Draws `src` of a texture into `dst` of the frame buffer: uploads the
// texture just past the Z buffer and draws it with a sprite.
void PSXVideoCard::HardwareBlt(_RECT *dst, PSXTextureMap *texture, _RECT *src)
{
    _RECT rect;
    _RECT viewport;
    unsigned char *data;
    int bpp;
    int psm;
    int abe;
    int dbp;
    PSXMipSurface *top;
    int x0;
    int y0;
    int tw;
    int tbw;
    int th;
    int cbp;

    data = (unsigned char *)texture->GetTopMipLevel()->m_surface;
    bpp = 16;
    psm = 2;
    abe = 0;
    switch (texture->m_format) {
    case 0x22B8:
        bpp = 32;
        abe = 1;
        psm = 0;
        break;
    case 0x613:
        // Redundant (bpp is already 16), as in retail: the dead store gives mwcc's
        // block optimizer the change that makes the delay-slot filler move
        // instructions instead of leaving dead copies.
        bpp = 16;
        break;
    case 8:
        psm = 0x13;
        abe = 1;
        bpp = 8;
        break;
    case 4:
        psm = 0x14;
        abe = 1;
        bpp = 4;
        break;
    default:
#line 1263 "PSXVideoCard.cpp"
        ASSERT(0, "PSXVideoCard::HardwareBlt Invalid PixelFormat");
        break;
    }
    dbp = m_zBufferAddress << 5;
    cbp = 0x3800;                       // CLUT just past the texture
    LpRenderTarget->WaitForRenderingToComplete();
    top = texture->GetTopMipLevel();
    if (texture->m_format == 8 || texture->m_format == 4) {
        texture->m_prepared = 0;
        texture->PrepareTransferTexture(LpTextureMapManager->m_gsTransfer, dbp, cbp, top,
                                        (unsigned char *)top->m_surface)->SendTexture(1);
    } else {
        LpTextureMapManager->m_gsTransfer->PrepareTransferOversizedTexture(data, dbp, texture->m_width,
                                                                          texture->m_height, 0, 0, psm, bpp);
        LpTextureMapManager->m_gsTransfer->SendTexture(1);
    }
    LpRenderTarget->WaitForRenderingToComplete();
    if (LpRenderTarget->m_camera) {
        LpRenderTarget->m_camera->GetViewportBoundsPhysical(&viewport);
        x0 = (int)(2048.0f - 0.5f * (viewport.right - viewport.left) - viewport.left) << 4;
        y0 = (int)(2048.0f - 0.5f * (viewport.bottom - viewport.top) - viewport.top) << 4;
    } else {
        x0 = m_offsetX;
        y0 = m_offsetY;
    }
    rect.left = x0 + (dst->left << 4);
    rect.top = y0 + (dst->top << 4);
    rect.right = x0 + (dst->right << 4);
    rect.bottom = y0 + (dst->bottom << 4);
    tw = top->m_widthLog2;
    tbw = top->m_blocks;
    if (texture->m_width % (int)ldexp(1.0, tw))
        tw++;
    th = log(texture->m_height) / 0.6931471805599453;
    if (texture->m_height % (int)ldexp(1.0, th))
        th++;
    m_packet->ResetPacket();
    m_packet->AddSourceDmaTag();
    m_packet->AddGifTag(1);
    m_packet->AddGifData(0x47, 0x1001);
    m_packet->AddGifData(0x00, SCE_GS_SET_PRIM(6, 0, 1, 0, 0, 0, 1, 0, 0) | (u_long)abe << 6);
    m_packet->AddGifData(0x42, SCE_GS_SET_ALPHA(0, 1, 0, 1, 0x80));
    if (texture->m_format == 8 || texture->m_format == 4)
        m_packet->AddGifData(0x06, SCE_GS_SET_TEX0(dbp, tbw, psm, tw, th, 1, 0, cbp, 0, 0, 0, 1));
    else
        m_packet->AddGifData(0x06, SCE_GS_SET_TEX0(dbp, tbw, psm, tw, th, 1, 0, 0, 0, 0, 0, 0));
    m_packet->AddGifData(0x3F, 0);
    m_packet->AddGifData(0x14, SCE_GS_SET_TEX1(0, 0, 1, 5, 0, 0, -75));
    m_packet->AddGifData(0x03, SCE_GS_SET_UV((src->left + 1) << 4, (src->top + 1) << 4));
    m_packet->AddGifData(0x01, SCE_GS_SET_RGBAQ(0x80, 0x80, 0x80, 0x40, 0x3F800000));
    m_packet->AddGifData(0x05, SCE_GS_SET_XYZ(rect.left, rect.top, 0));
    m_packet->AddGifData(0x03, SCE_GS_SET_UV(src->right << 4, src->bottom << 4));
    m_packet->AddGifData(0x01, SCE_GS_SET_RGBAQ(0x80, 0x80, 0x80, 0x40, 0x3F800000));
    m_packet->AddGifData(0x05, SCE_GS_SET_XYZ(rect.right, rect.bottom, 0));
    m_packet->AddGifData(0x47, 0x7000D);
    m_packet->AddDmaEndTag();
    m_packet->SendGifPacket(1, 1);
    if (LpRenderTarget->m_camera)
        ClearFrameBuffer(0, 0, 2);
}
#pragma fast_fptosi reset
#pragma global_optimizer reset


// ---- src/PSXVideoCard/CrossFadeBuffers.cpp
#pragma fast_fptosi on


typedef unsigned long u_long;

#define SCE_GS_SET_FRAME(fbp, fbw, psm, fbmask) \
    ((u_long)(fbp) | ((u_long)(fbw) << 16) | ((u_long)(psm) << 24) | ((u_long)(fbmask) << 32))
#define SCE_GS_SET_XYOFFSET(ofx, ofy) ((u_long)(ofx) | ((u_long)(ofy) << 32))
#define SCE_GS_SET_SCISSOR(scax0, scax1, scay0, scay1) \
    ((u_long)(scax0) | ((u_long)(scax1) << 16) | ((u_long)(scay0) << 32) | ((u_long)(scay1) << 48))
#define SCE_GS_SET_TEX0(tbp, tbw, psm, tw, th, tcc, tfx, cbp, cpsm, csm, csa, cld) \
    ((u_long)(tbp) | ((u_long)(tbw) << 14) | ((u_long)(psm) << 20) | ((u_long)(tw) << 26) | \
     ((u_long)(th) << 30) | ((u_long)(tcc) << 34) | ((u_long)(tfx) << 35) | ((u_long)(cbp) << 37) | \
     ((u_long)(cpsm) << 51) | ((u_long)(csm) << 55) | ((u_long)(csa) << 56) | ((u_long)(cld) << 61))
#define SCE_GS_SET_RGBAQ(r, g, b, a, q) \
    ((u_long)(r) | ((u_long)(g) << 8) | ((u_long)(b) << 16) | ((u_long)(a) << 24) | ((u_long)(q) << 32))
#define SCE_GS_SET_UV(u, v) ((u_long)(u) | ((u_long)(v) << 16))
#define SCE_GS_SET_XYZ(x, y, z) ((u_long)(x) | ((u_long)(y) << 16) | ((u_long)(z) << 32))






void WaitForVSync(int count);
extern "C" double log(double x);
extern "C" double ldexp(double x, int exp);

extern int TimerOverflowCounterT0;

// Win32-style timeGetTime: EE timer 0 (T0_COUNT at 0x10000000) ticks plus the
// overflows counted by TimerHandlerT0, accumulated and scaled to milliseconds.
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

// Cross-fades from the displayed buffer to the new frame: the old picture is
// kept as a texture and drawn over the new one with rising alpha.
void PSXVideoCard::CrossFadeBuffers()
{
    int display;
    int frame;
    int ofsX;
    int ofsY;
    int tw;
    int th;
    int x0;
    int y0;
    int width;
    int height;
    unsigned char alpha;
    int step;
    int tbw;
    int x1;
    int y1;

    display = GetCurrentDisplayBufferAddress();
    frame = GetCurrentFrameBufferAddress();
    _RECT rect = {0, 0, 640, 448};
    step = 1;
    if (LpRenderTarget) {
        if (LpRenderTarget->m_camera)
            LpRenderTarget->m_camera->GetViewportBoundsPhysical(&rect);
        ofsX = (0x800 - ((rect.right - rect.left) >> 1) - rect.left) << 4;
        ofsY = (0x800 - ((rect.bottom - rect.top) >> 1) - rect.top) << 4;
    }
    alpha = 0;
    width = m_width - 1;
    height = m_height - 1;
    x0 = m_offsetX;
    y0 = m_offsetY;
    x1 = x0 + (width << 4);
    y1 = y0 + (height << 4);
    tbw = m_width >> 6;
    tw = (int)(log((double)m_width) / 0.6931471805599453);
    if (m_width % (int)ldexp(1.0, tw))
        tw++;
    th = (int)(log((double)m_height) / 0.6931471805599453);
    if (m_height % (int)ldexp(1.0, th))
        th++;
    m_packet->ResetPacket();
    m_packet->AddSourceDmaTag();
    m_packet->AddGifTag(1);
    m_packet->AddGifData(0x4C, SCE_GS_SET_FRAME(display, m_width >> 6, m_psm, 0));
    m_packet->AddGifData(0x47, 0xC);
    m_packet->AddGifData(0x18, SCE_GS_SET_XYOFFSET(x0, y0));
    m_packet->AddGifData(0x40, (u_long)height << 48 | (u_long)width << 16);
    m_packet->AddDmaEndTag();
    m_packet->SendGifPacket(1, 1);
    BltDisplayToZ(0);
    m_zmask = 0;

    while (alpha <= 0x80) {
        unsigned int start = timeGetTime();
        VSyncComplete = 0;
        WaitForVSync(1);
        BltZToDisplay();
        m_packet->ResetPacket();
        m_packet->AddSourceDmaTag();
        m_packet->AddGifTag(1);
        m_packet->AddGifData(0x00, 0x156);
        m_packet->AddGifData(0x06, SCE_GS_SET_TEX0(frame << 5, tbw, 0, tw, th, 0, 0, 0, 0, 0, 0, 0));
        m_packet->AddGifData(0x3F, 0);
        m_packet->AddGifData(0x14, 0);
        m_packet->AddGifData(0x03, SCE_GS_SET_UV(1, 1));
        m_packet->AddGifData(0x01, SCE_GS_SET_RGBAQ(0x80, 0x80, 0x80, alpha, 0x3F800000));
        // XYZ2 written with (long) rather than the macro's (u_long) casts: retail
        // converts x0/y0 afresh here instead of reusing the XYOFFSET conversions.
        m_packet->AddGifData(0x05, ((long)x0 | ((long)y0 << 16)));
        m_packet->AddGifData(0x03, SCE_GS_SET_UV(m_width << 4, m_height << 4));
        m_packet->AddGifData(0x01, SCE_GS_SET_RGBAQ(0x80, 0x80, 0x80, alpha, 0x3F800000));
        m_packet->AddGifData(0x05, SCE_GS_SET_XYZ(x1, y1, 0));
        m_packet->AddDmaEndTag();
        m_packet->SendGifPacket(1, 1);
        alpha += (unsigned char)step;
        if (alpha > 0x80 && alpha - step != 0x80)
            alpha = 0x80;
        int elapsed = timeGetTime() - start;
        if (elapsed > m_x408)
            step++;
        while (elapsed < m_x408)
            elapsed = timeGetTime() - start;
    }

    m_packet->ResetPacket();
    m_packet->AddSourceDmaTag();
    m_packet->AddGifTag(1);
    m_packet->AddGifData(0x47, 0x7000D);
    m_packet->AddGifData(0x18, SCE_GS_SET_XYOFFSET(ofsX, ofsY));
    m_packet->AddGifData(0x40, SCE_GS_SET_SCISSOR(rect.left, rect.right - 1, rect.top, rect.bottom - 1));
    m_packet->AddGifData(0x4C, SCE_GS_SET_FRAME(frame, m_width >> 6, m_psm, 0));
    m_packet->AddDmaEndTag();
    m_packet->SendGifPacket(1, 1);
}
#pragma fast_fptosi reset


// ---- src/PSXVideoCard/MotionBlurBuffers.cpp

#pragma optimization_level 3
typedef unsigned long u_long;
typedef int int128_t __attribute__((mode(TI)));

#define SCE_GS_SET_XYOFFSET(ofx, ofy) ((u_long)(ofx) | ((u_long)(ofy) << 32))
#define SCE_GS_SET_SCISSOR(scax0, scax1, scay0, scay1) \
    ((u_long)(scax0) | ((u_long)(scax1) << 16) | ((u_long)(scay0) << 32) | ((u_long)(scay1) << 48))
#define SCE_GS_SET_XYZ(x, y, z) ((u_long)(x) | ((u_long)(y) << 16) | ((u_long)(z) << 32))
#define SCE_GS_SET_UV(u, v) ((u_long)(u) | ((u_long)(v) << 16))
#define SCE_GS_SET_TEX0(tbp, tbw, psm, tw, th) \
    ((u_long)(tbp) | ((u_long)(tbw) << 14) | ((u_long)(psm) << 20) | ((u_long)(tw) << 26) | ((u_long)(th) << 30))


extern "C" double log(double x);
extern "C" double ldexp(double x, int exp);



#define LN2 0.69314718055994530942

void PSXVideoCard::MotionBlurBuffers(DmaRenderItem *item, _RECT *rect)
{
    int offX;
    int display;
    unsigned char r;
    unsigned char g;
    unsigned char b;
    int bottom;
    unsigned char a;
    int ofsX;
    int ofsY;
    int fbw;
    int offY;
    int x0;
    int y0;
    int x1;
    int right;
    int y1;
    int right1;
    int tw;
    int th;
    int center;
    int left;
    int top;
    int bottom1;
    int vp;

    display = GetCurrentDisplayBufferAddress();
    GetCurrentFrameBufferAddress();
    _RECT viewport = {0, 0, 640, 448};
    vp = LpGame->m_viewport;
    if (LpRenderTarget) {
        if (LpRenderTarget->m_camera)
            LpRenderTarget->m_camera->GetViewportBoundsPhysical(&viewport);
        ofsX = (0x800 - ((viewport.right - viewport.left) >> 1) - viewport.left) << 4;
        ofsY = (0x800 - ((viewport.bottom - viewport.top) >> 1) - viewport.top) << 4;
    }
    if (!rect)
        rect = &viewport;
    left = rect->left;
    fbw = m_width >> 6;
    center = 0x800;
    right = rect->right;
    top = rect->top;
    offX = (center - ((right - left) >> 1) - left) << 4;
    bottom = rect->bottom;
    offY = (center - ((bottom - top) >> 1) - top) << 4;
    x0 = offX + (left << 4);
    a = m_blurAlpha[vp];
    y0 = offY + (top << 4);
    r = m_blurRed[vp];
    x1 = offX + (right << 4);
    g = m_blurGreen[vp];
    b = m_blurBlue[vp];
    y1 = offY + (bottom << 4);
    bottom1 = bottom - 1;
    right1 = right - 1;
    tw = (int)(log((double)rect->right) / LN2);
    if (rect->right % (int)ldexp(1.0, tw))
        tw++;
    th = (int)(log((double)rect->bottom) / LN2);
    if (rect->bottom % (int)ldexp(1.0, th))
        th++;
    sceGifTag tag;
    *(int128_t *)&tag = 0;
    tag.FLG = 0;
    tag.EOP = 1;
    tag.NREG = 16;     // truncated to 0 (16 registers)
    tag.NLOOP = 1;
    tag.REGS0 = 0xE;    // A+D
    tag.REGS1 = 0xE;
    tag.REGS2 = 0xE;
    tag.REGS3 = 0xE;
    tag.REGS4 = 0xE;
    tag.REGS5 = 0xE;
    tag.REGS6 = 0xE;
    tag.REGS7 = 0xE;
    tag.REGS8 = 0xE;
    tag.REGS9 = 0xE;
    tag.REGS10 = 0xE;
    tag.REGS11 = 0xE;
    tag.REGS12 = 0xE;
    tag.REGS13 = 0xE;
    tag.REGS14 = 0xE;
    tag.REGS15 = 0xE;
    item->Start((DmaRenderDestination)1, 0x48);
    item->DmaAddVifCode(0x11000000);    // FLUSH
    item->DmaAddVifCode(0x03000000);    // BASE
    item->DmaAddVifCode(0x02000000);    // OFFSET
    if ((item->m_dataListCurrentSize + 5) % 2)
        item->DmaAddVifCode(0);         // NOP: align the unpacked data
    item->DmaBeginUnpack(0x320, 12);
    item->DmaAddGifTagToUnpack(&tag);
    u_long *d = (u_long *)&item->m_dataList[item->m_dataListCurrentSize];
    d[0] = 0xD;
    d[1] = 0x47;                        // TEST_1
    d[2] = SCE_GS_SET_XYOFFSET(offX, offY);
    d[3] = 0x18;                        // XYOFFSET_1
    d[4] = SCE_GS_SET_SCISSOR(left, right1, top, bottom1);
    d[5] = 0x40;                        // SCISSOR_1
    d[6] = 0x156;                       // sprite, textured, blended, UV
    d[7] = 0;                           // PRIM
    d[8] = 0x44 | ((u_long)0x80 << 32);
    d[9] = 0x42;                        // ALPHA_1
    d[10] = SCE_GS_SET_TEX0(display << 5, fbw, 0, tw, th);
    d[11] = 6;                          // TEX0_1
    d[12] = 0;
    d[13] = 0x3F;                       // TEXFLUSH
    d[14] = 0;
    d[15] = 0x14;                       // TEX1_1
    d[16] = SCE_GS_SET_UV((rect->left + 1) << 4, (rect->top + 1) << 4);
    d[17] = 3;                          // UV
    d[18] = (u_long)r | ((u_long)g << 8) | ((u_long)b << 16) | ((u_long)a << 24) | ((u_long)0x3F800000 << 32);
    d[19] = 1;                          // RGBAQ
    d[20] = SCE_GS_SET_XYZ(x0, y0, 0);
    d[21] = 4;                          // XYZ2
    d[22] = SCE_GS_SET_UV(rect->right << 4, rect->bottom << 4);
    d[23] = 3;
    d[24] = SCE_GS_SET_XYZ(x1, y1, 0);
    d[25] = 4;
    d[26] = 0x7000D;
    d[27] = 0x47;
    d[28] = SCE_GS_SET_XYOFFSET(ofsX, ofsY);
    d[29] = 0x18;
    d[30] = SCE_GS_SET_SCISSOR(viewport.left, viewport.right - 1, viewport.top, viewport.bottom - 1);
    d[31] = 0x40;
    DmaRenderItem::UnpackCount += 0x40;
    item->m_dataListCurrentSize += 0x40;
    item->DmaEndUnpack();
    item->DmaAddVifCode(0x14000000);    // MSCAL
    item->End();
    LpRenderTarget->AddToRenderQueue(item, "");
}
#pragma optimization_level reset


// ---- src/PSXVideoCard/OverlayFrameBuffer.cpp
// Retail built this function without the global optimizer (each u_long
// conversion is redone, nothing is CSE'd across the packet calls).
#pragma global_optimizer off
typedef unsigned long u_long;

#define SCE_GS_SET_XYOFFSET(ofx, ofy) ((u_long)(ofx) | ((u_long)(ofy) << 32))
#define SCE_GS_SET_SCISSOR(scax0, scax1, scay0, scay1) \
    ((u_long)(scax0) | ((u_long)(scax1) << 16) | ((u_long)(scay0) << 32) | ((u_long)(scay1) << 48))
#define SCE_GS_SET_ALPHA(a, b, c, d, fix) \
    ((u_long)(a) | ((u_long)(b) << 2) | ((u_long)(c) << 4) | ((u_long)(d) << 6) | ((u_long)(fix) << 32))
#define SCE_GS_SET_RGBAQ(r, g, b, a, q) \
    ((u_long)(r) | ((u_long)(g) << 8) | ((u_long)(b) << 16) | ((u_long)(a) << 24) | ((u_long)(q) << 32))
#define SCE_GS_SET_XYZ(x, y, z) ((u_long)(x) | ((u_long)(y) << 16) | ((u_long)(z) << 32))






// Draws a sprite of the given color (0xAARRGGBB) over the whole frame
// buffer, blended with ALPHA (Cs - Cd) * FIX + Cd.
void PSXVideoCard::OverlayFrameBuffer(unsigned int color)
{
    int ofsX;
    int ofsY;
    int x1;
    int y1;
    int x0;
    int y0;
    int width;
    int height;

    GetCurrentDisplayBufferAddress();
    GetCurrentFrameBufferAddress();
    _RECT rect = {0, 0, 640, 448};
    // ofsX/ofsY stay uninitialized without a render target, as in retail.
    if (LpRenderTarget) {
        if (LpRenderTarget->m_camera)
            LpRenderTarget->m_camera->GetViewportBoundsPhysical(&rect);
        int bottom, center, left, right, top;
        center = 0x800;
        left = rect.left;
        right = rect.right;
        top = rect.top;
        bottom = rect.bottom;
        ofsX = (center - ((right - left) >> 1) - left) << 4;
        ofsY = (center - ((bottom - top) >> 1) - top) << 4;
    }
    width = m_width;
    x0 = m_offsetX;
    height = m_height;
    y0 = m_offsetY;
    x1 = x0 + (width << 4);
    y1 = y0 + (height << 4);
    m_packet->ResetPacket();
    m_packet->AddSourceDmaTag();
    m_packet->AddGifTag(1);
    m_packet->AddGifData(0x47, 0xD);
    m_packet->AddGifData(0x18, SCE_GS_SET_XYOFFSET(x0, y0));
    // Written high field first: retail computes scay1 before scax1.
    m_packet->AddGifData(0x40, (u_long)(height - 1) << 48 | (u_long)(width - 1) << 16);
    m_packet->AddGifData(0x00, 0x46);
    m_packet->AddGifData(0x42, SCE_GS_SET_ALPHA(0, 1, 0, 1, 0x80));
    m_packet->AddGifData(0x01, SCE_GS_SET_RGBAQ((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF,
                                                (color >> 24) & 0xFF, 0x3F800000));
    m_packet->AddGifData(0x04, SCE_GS_SET_XYZ(x0, y0, 0));
    m_packet->AddGifData(0x04, SCE_GS_SET_XYZ(x1, y1, 0));
    m_packet->AddGifData(0x47, 0x7000D);
    m_packet->AddGifData(0x18, SCE_GS_SET_XYOFFSET(ofsX, ofsY));
    m_packet->AddGifData(0x40, SCE_GS_SET_SCISSOR(rect.left, rect.right - 1, rect.top, rect.bottom - 1));
    m_packet->AddDmaEndTag();
    m_packet->SendGifPacket(1, 1);
}
#pragma global_optimizer reset


// ---- src/PSXVideoCard/OverlayViewport.cpp
// Retail built this function at optimization level 3: no loop optimizations, but
// addresses used more than once in a block (the GIFtag bytes) are CSE'd into
// registers, which neither -O4 nor `global_optimizer off` reproduces.
#pragma optimization_level 3
typedef unsigned long u_long;
typedef int int128_t __attribute__((mode(TI)));

#define SCE_GS_SET_XYOFFSET(ofx, ofy) ((u_long)(ofx) | ((u_long)(ofy) << 32))
#define SCE_GS_SET_SCISSOR(scax0, scax1, scay0, scay1) \
    ((u_long)(scax0) | ((u_long)(scax1) << 16) | ((u_long)(scay0) << 32) | ((u_long)(scay1) << 48))
#define SCE_GS_SET_XYZ(x, y, z) ((u_long)(x) | ((u_long)(y) << 16) | ((u_long)(z) << 32))








// Blends a sprite of the given color (0xAARRGGBB) over rect (default: the
// viewport) with the given ALPHA register, through a VU1 A+D packet.
void PSXVideoCard::OverlayViewport(DmaRenderItem *item, unsigned int color, u_long alpha, _RECT *rect)
{
    unsigned char r;
    unsigned char g;
    int offX;
    unsigned char a;
    int ofsX;
    int offY;
    int ofsY;
    int x0;
    int y0;
    int left;
    int x1;
    int top;
    int y1;
    int right;
    int bottom;
    int center;

    GetCurrentDisplayBufferAddress();
    GetCurrentFrameBufferAddress();
    // Note retail's default is 639x447 (the final SCISSOR subtracts 1 again).
    _RECT viewport = {0, 0, 639, 447};
    if (LpRenderTarget) {
        if (LpRenderTarget->m_camera)
            LpRenderTarget->m_camera->GetViewportBoundsPhysical(&viewport);
        ofsX = (0x800 - ((viewport.right - viewport.left) >> 1) - viewport.left) << 4;
        ofsY = (0x800 - ((viewport.bottom - viewport.top) >> 1) - viewport.top) << 4;
    }
    if (!rect)
        rect = &viewport;
    left = rect->left;
    right = rect->right;
    a = color >> 24;
    top = rect->top;
    bottom = rect->bottom;
    center = 0x800;
    offX = (center - ((right - left) >> 1) - left) << 4;
    offY = (center - ((bottom - top) >> 1) - top) << 4;
    x0 = offX + (left << 4);
    y0 = offY + (top << 4);
    x1 = offX + (right << 4);
    y1 = offY + (bottom << 4);
    r = color >> 16;
    g = color >> 8;
    sceGifTag tag;
    *(int128_t *)&tag = 0;
    tag.FLG = 0;
    tag.EOP = 1;
    tag.NREG = 11;
    tag.NLOOP = 1;
    tag.REGS0 = 0xE;    // A+D
    tag.REGS1 = 0xE;
    tag.REGS2 = 0xE;
    tag.REGS3 = 0xE;
    tag.REGS4 = 0xE;
    tag.REGS5 = 0xE;
    tag.REGS6 = 0xE;
    tag.REGS7 = 0xE;
    tag.REGS8 = 0xE;
    tag.REGS9 = 0xE;
    tag.REGS10 = 0xE;
    unsigned char b = color;
    item->Start((DmaRenderDestination)1, 0x34);
    item->DmaAddVifCode(0x11000000);    // FLUSH
    item->DmaAddVifCode(0x03000000);    // BASE
    item->DmaAddVifCode(0x02000000);    // OFFSET
    if ((item->m_dataListCurrentSize + 5) % 2)
        item->DmaAddVifCode(0);         // NOP: align the unpacked data
    item->DmaBeginUnpack(0x320, 12);
    item->DmaAddGifTagToUnpack(&tag);
    u_long *d = (u_long *)&item->m_dataList[item->m_dataListCurrentSize];
    d[0] = 0xD;
    d[1] = 0x47;                        // TEST_1
    d[2] = SCE_GS_SET_XYOFFSET(offX, offY);
    d[3] = 0x18;                        // XYOFFSET_1
    d[4] = SCE_GS_SET_SCISSOR(left, right, top, bottom);
    d[5] = 0x40;                        // SCISSOR_1
    d[6] = 0x46;
    d[7] = 0;                           // PRIM
    d[8] = alpha;
    d[9] = 0x42;                        // ALPHA_1
    d[10] = (u_long)r | ((u_long)g << 8) | ((u_long)b << 16) | ((u_long)a << 24) | ((u_long)0x3F800000 << 32);
    d[11] = 1;                          // RGBAQ
    d[12] = SCE_GS_SET_XYZ(x0, y0, 0);
    d[13] = 4;                          // XYZ2
    d[14] = SCE_GS_SET_XYZ(x1, y1, 0);
    d[15] = 4;
    d[16] = 0x7000D;
    d[17] = 0x47;
    d[18] = SCE_GS_SET_XYOFFSET(ofsX, ofsY);
    d[19] = 0x18;
    d[20] = SCE_GS_SET_SCISSOR(viewport.left, viewport.right - 1, viewport.top, viewport.bottom - 1);
    d[21] = 0x40;
    DmaRenderItem::UnpackCount += 0x2C;
    item->m_dataListCurrentSize += 0x2C;
    item->DmaEndUnpack();
    item->DmaAddVifCode(0x14000000);    // MSCAL
    item->End();
    LpRenderTarget->AddToRenderQueue(item, "");
}
#pragma optimization_level reset


// ---- src/PSXVideoCard/SetViewportFadeColors.cpp
// ASSERT contains inline asm; match retail's label alignment for asm functions.
#pragma alignlabel off




// Colors are packed ABGR; the fade is parameterised by alpha.
void PSXVideoCard::SetViewportFadeColors(int startColor, int endColor)
{
    int viewport;
    int alphaRange;
    alphaRange = ((endColor >> 24) & 0xFF) - ((startColor >> 24) & 0xFF);
    viewport = ((JetskiGame *)LpGame)->m_viewport;
#line 1880 "PSXVideoCard.cpp"
    ASSERT(alphaRange > 0, "Start color alpha must be less than end color's alpha");
    m_viewportFadeStartColor[viewport] = startColor;
    m_viewportFadeEndColor[viewport] = endColor;
    m_viewportFadeStepB[viewport] = (float)(((endColor >> 16) & 0xFF) - ((startColor >> 16) & 0xFF)) / (float)alphaRange;
    m_viewportFadeStepG[viewport] = (float)(((endColor >> 8) & 0xFF) - ((startColor >> 8) & 0xFF)) / (float)alphaRange;
    m_viewportFadeStepR[viewport] = (float)((endColor & 0xFF) - (startColor & 0xFF)) / (float)alphaRange;
}
#pragma alignlabel reset


// ---- src/PSXVideoCard/SetScreenFadeColors.cpp
// ASSERT contains inline asm; match retail's label alignment for asm functions.
#pragma alignlabel off



// Colors are packed ABGR; the fade is parameterised by alpha.
void PSXVideoCard::SetScreenFadeColors(int startColor, int endColor)
{
    int alphaRange = ((endColor >> 24) & 0xFF) - ((startColor >> 24) & 0xFF);
#line 1893 "PSXVideoCard.cpp"
    ASSERT(alphaRange > 0, "Start color alpha must be less than end color's alpha");
    m_fadeStartColor = startColor;
    m_fadeEndColor = endColor;
    m_fadeStepB = (float)(((endColor >> 16) & 0xFF) - ((startColor >> 16) & 0xFF)) / (float)alphaRange;
    m_fadeStepG = (float)(((endColor >> 8) & 0xFF) - ((startColor >> 8) & 0xFF)) / (float)alphaRange;
    m_fadeStepR = (float)((endColor & 0xFF) - (startColor & 0xFF)) / (float)alphaRange;
}
#pragma alignlabel reset


// ---- src/PSXVideoCard/FadeOutViewport.cpp
// ASSERT contains inline asm; match retail's label alignment for asm functions.
#pragma alignlabel off






// Read with absolute loads, so declared const.


extern int TimerOverflowCounterT0;

#define T0_COUNT ((volatile int *)0x10000000)

// Milliseconds from EE timer 0 (BUSCLK / 256 = 585.9375 ticks per ms).

static inline int FloatToInt(float f)
{
    int i;
    asm("cvt.w.s $f5, %1; mfc1 %0, $f5" : "=r"(i) : "f"(f) : "$f5");
    return i;
}

void PSXVideoCard::FadeOutViewport(int duration, _RECT *rect)
{
    if (LpGame->m_renderPhase != 3) {
#line 1908 "PSXVideoCard.cpp"
        ASSERT(0, "This function may only be called in RenderPre3D!");
    }
    int viewport = LpGame->m_viewport;
#line 1912 "PSXVideoCard.cpp"
    ASSERT(viewport < 4, "Only 4 viewports are supported (0-3)!");
    if (m_viewportFaded[viewport] == 1 || m_viewportFadeOut[viewport] == 1)
        return;
    m_viewportFadeLevel[viewport] = (m_viewportFadeEndColor[viewport] >> 24) & 0xFF;
    m_viewportFadeFrames[viewport] = FloatToInt(0.5f + duration / ((float)m_viewportFadeLevel[viewport] - (float)m_viewportFadeTarget[viewport]));
    m_viewportFadeOut[viewport] = 1;
    m_viewportFading[viewport] = 0;
    m_viewportFadeTarget[viewport] = 0;
    m_viewportFadeStart[viewport] = f_timeGetTime();
    if (rect) {
        m_viewportFadeRect[viewport] = *rect;
    } else {
        if (!LpRenderTarget->m_camera) {
            PSXprintf("DON'T CALL FadeOutViewport WITH NO CAMERA!!\n");
            return;
        }
        LpRenderTarget->m_camera->GetViewportBoundsPhysical(&m_viewportFadeRect[viewport]);
    }
}
#pragma alignlabel reset


// ---- src/PSXVideoCard/FadeInViewport.cpp
// ASSERT contains inline asm; match retail's label alignment for asm functions.
#pragma alignlabel off






// Read with absolute loads, so declared const.


extern int TimerOverflowCounterT0;

#define T0_COUNT ((volatile int *)0x10000000)

// Milliseconds from EE timer 0 (BUSCLK / 256 = 585.9375 ticks per ms).

void PSXVideoCard::FadeInViewport(int duration, _RECT *rect)
{
    if (LpGame->m_renderPhase != 3) {
#line 1945 "PSXVideoCard.cpp"
        ASSERT(0, "This function may only be called in RenderPre3D!");
    }
    int viewport = LpGame->m_viewport;
#line 1949 "PSXVideoCard.cpp"
    ASSERT(viewport < 4, "Only 4 viewports are supported (0-3)!");
    if (m_viewportFading[viewport] != 1) {
        m_viewportFadeLevel[viewport] = (m_viewportFadeStartColor[viewport] >> 24) & 0xFF;
        m_viewportFadeFrames[viewport] = FloatToInt(0.5f + duration / ((float)m_viewportFadeTarget[viewport] - (float)m_viewportFadeLevel[viewport]));
        m_viewportFadeOut[viewport] = 0;
        m_viewportFading[viewport] = 1;
        m_viewportFadeStart[viewport] = f_timeGetTime();
        if (rect) {
            m_viewportFadeRect[viewport] = *rect;
        } else {
            if (!LpRenderTarget->m_camera) {
                PSXprintf("DON'T CALL FadeInViewport WITH NO CAMERA!!\n");
                return;
            }
            LpRenderTarget->m_camera->GetViewportBoundsPhysical(&m_viewportFadeRect[viewport]);
        }
    }
}
#pragma alignlabel reset


// ---- src/PSXVideoCard/IsViewportFaded.cpp
// ASSERT contains inline asm; match retail's label alignment for asm functions.
#pragma alignlabel off




// viewport -1 means "the frame being rendered", valid only in RenderPre3D.
int PSXVideoCard::IsViewportFaded(int viewport)
{
#line 1978 "PSXVideoCard.cpp"
    ASSERT(viewport < 4, "Only 4 viewports are supported (0-3)!");
    if (viewport == -1) {
        if (LpGame->m_renderPhase != 3) {
#line 1982 "PSXVideoCard.cpp"
            ASSERT(0, "This function may only be called in RenderPre3D!");
        }
        return m_viewportFaded[LpGame->m_viewport];
    }
    return m_viewportFaded[viewport];
}
#pragma alignlabel reset


// ---- src/PSXVideoCard/IsScreenFaded.cpp

int PSXVideoCard::IsScreenFaded()
{
    return m_fadedOut | m_fadingIn;
}


// ---- src/PSXVideoCard/FadeOutScreen.cpp
// ASSERT contains inline asm; match retail's label alignment for asm functions.
#pragma alignlabel off




extern int TimerOverflowCounterT0;

#define T0_COUNT ((volatile int *)0x10000000)

// Milliseconds from EE timer 0 (BUSCLK / 256 = 585.9375 ticks per ms).

// Read with absolute loads, so declared const.


static inline void SetFading(PSXVideoCard *card, int out, int in)
{
    card->m_fadingOut = out;
    card->m_fadingInFlag = in;
}

void PSXVideoCard::FadeOutScreen(int duration, int flag)
{
    if (LpGame->m_renderPhase == 5) {
#line 2003 "PSXVideoCard.cpp"
        ASSERT(0, "This function may not be called in RenderPost3D!");
    }
    if (m_fadedOut == 1 || m_fadingOut == 1)
        return;
    m_fadeTarget = (m_fadeEndColor >> 24) & 0xFF;
    SetFading(this, 1, 0);
    m_fadeStart = f_timeGetTime();
    m_fadeFrames = FloatToInt(0.5f + duration / ((float)m_fadeTarget - (float)m_fadeLevel));
    m_fadeFlag = flag;
}
#pragma alignlabel reset


// ---- src/PSXVideoCard/FadeInScreen.cpp
// ASSERT contains inline asm; match retail's label alignment for asm functions.
#pragma alignlabel off




extern int TimerOverflowCounterT0;

#define T0_COUNT ((volatile int *)0x10000000)

// Milliseconds from EE timer 0 (BUSCLK / 256 = 585.9375 ticks per ms).

// Read with absolute loads, so declared const.

void PSXVideoCard::FadeInScreen(int duration, int flag)
{
    if (LpGame->m_renderPhase == 5) {
#line 2025 "PSXVideoCard.cpp"
        ASSERT(0, "This function may not be called in RenderPost3D!");
    }
    if (m_fadingInFlag != 1) {
        m_fadeTarget = (m_fadeStartColor >> 24) & 0xFF;
        SetFading(this, 0, 1);
        m_fadeStart = f_timeGetTime();
        m_fadeFrames = FloatToInt(0.5f + duration / ((float)m_fadeLevel - (float)m_fadeTarget));
        m_fadeFlag = flag;
    }
}
#pragma alignlabel reset


// ---- src/PSXVideoCard/StaticFadeOut.cpp

#pragma fast_fptosi on
typedef unsigned long u_long;

#define SCE_GS_SET_FRAME(fbp, fbw, psm, fbmask) \
    ((u_long)(fbp) | ((u_long)(fbw) << 16) | ((u_long)(psm) << 24) | ((u_long)(fbmask) << 32))
#define SCE_GS_SET_XYOFFSET(ofx, ofy) ((u_long)(ofx) | ((u_long)(ofy) << 32))
#define SCE_GS_SET_SCISSOR(scax0, scax1, scay0, scay1) \
    ((u_long)(scax0) | ((u_long)(scax1) << 16) | ((u_long)(scay0) << 32) | ((u_long)(scay1) << 48))
#define SCE_GS_SET_RGBAQ(r, g, b, a, q) \
    ((u_long)(r) | ((u_long)(g) << 8) | ((u_long)(b) << 16) | ((u_long)(a) << 24) | ((u_long)(q) << 32))
#define SCE_GS_SET_XYZ(x, y, z) ((u_long)(x) | ((u_long)(y) << 16) | ((u_long)(z) << 32))

#define GS_CSR ((volatile u_long *)0x12001000)





void WaitForVSync(int count);

// Local copy of PSXVideoCard's members: the shared header has no member at
// +0x3E8.

extern int TimerOverflowCounterT0;

#define T0_COUNT ((volatile int *)0x10000000)

// Milliseconds from EE timer 0 (BUSCLK / 256 = 585.9375 ticks per ms).

// Fades the screen out in a blocking loop: each step blends a sprite of the
// fade colour over the frame (kept in the Z buffer) and shows it.
void PSXVideoCard::StaticFadeOut()
{
    int width;
    int x1;
    int frame;
    int ofsX;
    int height;
    int display;
    int x0;
    int ofsY;
    int y1;
    int y0;
    display = GetCurrentDisplayBufferAddress();
    frame = GetCurrentFrameBufferAddress();
    _RECT rect = {0, 0, 640, 448};
    int step = 1;
    if (LpRenderTarget) {
        if (LpRenderTarget->m_camera)
            LpRenderTarget->m_camera->GetViewportBoundsPhysical(&rect);
        ofsX = (0x800 - ((rect.right - rect.left) >> 1) - rect.left) << 4;
        ofsY = (0x800 - ((rect.bottom - rect.top) >> 1) - rect.top) << 4;
    }
    width = m_width;
    x0 = m_offsetX;
    height = m_height;
    y0 = m_offsetY;
    x1 = x0 + (width << 4);
    y1 = y0 + (height << 4);
    m_packet->ResetPacket();
    m_packet->AddSourceDmaTag();
    m_packet->AddGifTag(1);
    m_packet->AddGifData(0x4C, SCE_GS_SET_FRAME(display, m_width >> 6, m_psm, 0));
    m_packet->AddGifData(0x47, 0xD);
    m_packet->AddGifData(0x18, SCE_GS_SET_XYOFFSET(x0, y0));
    m_packet->AddGifData(0x40, (u_long)(height - 1) << 48 | (u_long)(width - 1) << 16);
    m_packet->AddDmaEndTag();
    m_packet->SendGifPacket(1, 1);
    BltDisplayToZ(0);

    int alpha;
    int startAlpha = (m_fadeStartColor >> 24) & 0xFF;
    alpha = startAlpha;
    float start = f_timeGetTime();
    while (alpha <= m_fadeTarget) {
        VSyncComplete = 0;
        WaitForVSync(1);
        BltZToDisplay();
        float a = alpha - startAlpha;
        float db = m_fadeStepB * a;
        float dg = m_fadeStepG * a;
        float dr = m_fadeStepR * a;
        float b = ((m_fadeStartColor >> 16) & 0xFF) + db;
        float g = ((m_fadeStartColor >> 8) & 0xFF) + dg;
        float r = (m_fadeStartColor & 0xFF) + dr;
        *GS_CSR |= 2;
        m_packet->ResetPacket();
        m_packet->AddSourceDmaTag();
        m_packet->AddGifTag(1);
        m_packet->AddGifData(0x00, 0x46);
        m_packet->AddGifData(0x01, SCE_GS_SET_RGBAQ((int)b, (int)g, (int)r, alpha, 0x3F800000));
        m_packet->AddGifData(0x04, ((long)x0 | ((long)y0 << 16)));
        m_packet->AddGifData(0x04, ((long)x1 | ((long)y1 << 16)));
        m_packet->AddGifData(0x61, 1);
        m_packet->AddDmaEndTag();
        m_packet->SendGifPacket(1, 1);
        while (!(*GS_CSR & 2))
            ;
        alpha += step;
        if (alpha > m_fadeTarget && alpha - step != m_fadeTarget)
            alpha = m_fadeTarget;
        float elapsed = f_timeGetTime() - start;
        if (elapsed > m_fadeFrames * (alpha - startAlpha))
            step++;
        int done = alpha - startAlpha;
        while (elapsed < m_fadeFrames * done)
            elapsed = f_timeGetTime() - start;
    }

    m_packet->ResetPacket();
    m_packet->AddSourceDmaTag();
    m_packet->AddGifTag(1);
    m_packet->AddGifData(0x47, 0x7000D);
    m_packet->AddGifData(0x18, SCE_GS_SET_XYOFFSET(ofsX, ofsY));
    m_packet->AddGifData(0x40, SCE_GS_SET_SCISSOR(rect.left, rect.right - 1, rect.top, rect.bottom - 1));
    m_packet->AddGifData(0x4C, SCE_GS_SET_FRAME(frame, m_width >> 6, m_psm, 0));
    m_packet->AddDmaEndTag();
    m_packet->SendGifPacket(1, 1);
    m_fadingOut = 0;
    m_fadeLevel = alpha;
    m_fadeDone = 0;
}
#pragma fast_fptosi reset


// ---- src/PSXVideoCard/StaticFadeIn.cpp

#pragma fast_fptosi on
typedef unsigned long u_long;

#define SCE_GS_SET_FRAME(fbp, fbw, psm, fbmask) \
    ((u_long)(fbp) | ((u_long)(fbw) << 16) | ((u_long)(psm) << 24) | ((u_long)(fbmask) << 32))
#define SCE_GS_SET_XYOFFSET(ofx, ofy) ((u_long)(ofx) | ((u_long)(ofy) << 32))
#define SCE_GS_SET_SCISSOR(scax0, scax1, scay0, scay1) \
    ((u_long)(scax0) | ((u_long)(scax1) << 16) | ((u_long)(scay0) << 32) | ((u_long)(scay1) << 48))
#define SCE_GS_SET_RGBAQ(r, g, b, a, q) \
    ((u_long)(r) | ((u_long)(g) << 8) | ((u_long)(b) << 16) | ((u_long)(a) << 24) | ((u_long)(q) << 32))
#define SCE_GS_SET_XYZ(x, y, z) ((u_long)(x) | ((u_long)(y) << 16) | ((u_long)(z) << 32))

#define GS_CSR ((volatile u_long *)0x12001000)





void WaitForVSync(int count);

// Local copy of PSXVideoCard's members: the shared header has no member at
// +0x3E8.

extern int TimerOverflowCounterT0;

#define T0_COUNT ((volatile int *)0x10000000)

// Milliseconds from EE timer 0 (BUSCLK / 256 = 585.9375 ticks per ms).

// Fades the screen out in a blocking loop: each step blends a sprite of the
// fade colour over the frame (kept in the Z buffer) and shows it.
void PSXVideoCard::StaticFadeIn()
{
    int width;
    int x1;
    int frame;
    int ofsX;
    int height;
    int display;
    int x0;
    int ofsY;
    int y1;
    int y0;
    display = GetCurrentDisplayBufferAddress();
    frame = GetCurrentFrameBufferAddress();
    _RECT rect = {0, 0, 640, 448};
    int step = 1;
    if (LpRenderTarget) {
        if (LpRenderTarget->m_camera)
            LpRenderTarget->m_camera->GetViewportBoundsPhysical(&rect);
        ofsX = (0x800 - ((rect.right - rect.left) >> 1) - rect.left) << 4;
        ofsY = (0x800 - ((rect.bottom - rect.top) >> 1) - rect.top) << 4;
    }
    width = m_width;
    x0 = m_offsetX;
    height = m_height;
    y0 = m_offsetY;
    x1 = x0 + (width << 4);
    y1 = y0 + (height << 4);
    m_packet->ResetPacket();
    m_packet->AddSourceDmaTag();
    m_packet->AddGifTag(1);
    m_packet->AddGifData(0x4C, SCE_GS_SET_FRAME(display, m_width >> 6, m_psm, 0));
    m_packet->AddGifData(0x47, 0xD);
    m_packet->AddGifData(0x18, SCE_GS_SET_XYOFFSET(x0, y0));
    m_packet->AddGifData(0x40, (u_long)(height - 1) << 48 | (u_long)(width - 1) << 16);
    m_packet->AddDmaEndTag();
    m_packet->SendGifPacket(1, 1);

    int alpha;
    int startAlpha = (m_fadeEndColor >> 24) & 0xFF;
    alpha = startAlpha;
    float start = f_timeGetTime();
    while (alpha >= m_fadeTarget) {
        VSyncComplete = 0;
        WaitForVSync(1);
        BltFrameToDisplay();
        float a = alpha - startAlpha;
        float db = m_fadeStepB * a;
        float dg = m_fadeStepG * a;
        float dr = m_fadeStepR * a;
        float b = ((m_fadeStartColor >> 16) & 0xFF) + db;
        float g = ((m_fadeStartColor >> 8) & 0xFF) + dg;
        float r = (m_fadeStartColor & 0xFF) + dr;
        *GS_CSR |= 2;
        m_packet->ResetPacket();
        m_packet->AddSourceDmaTag();
        m_packet->AddGifTag(1);
        m_packet->AddGifData(0x00, 0x46);
        m_packet->AddGifData(0x01, SCE_GS_SET_RGBAQ((int)b, (int)g, (int)r, alpha, 0x3F800000));
        m_packet->AddGifData(0x04, ((long)x0 | ((long)y0 << 16)));
        m_packet->AddGifData(0x04, ((long)x1 | ((long)y1 << 16)));
        m_packet->AddGifData(0x61, 1);
        m_packet->AddDmaEndTag();
        m_packet->SendGifPacket(1, 1);
        while (!(*GS_CSR & 2))
            ;
        alpha -= step;
        if (alpha < m_fadeTarget && alpha + step != m_fadeTarget)
            alpha = m_fadeTarget;
        float elapsed = f_timeGetTime() - start;
        if (elapsed > m_fadeFrames * (startAlpha - alpha))
            step++;
        int done = startAlpha - alpha;
        while (elapsed < m_fadeFrames * done)
            elapsed = f_timeGetTime() - start;
    }

    m_packet->ResetPacket();
    m_packet->AddSourceDmaTag();
    m_packet->AddGifTag(1);
    m_packet->AddGifData(0x47, 0x7000D);
    m_packet->AddGifData(0x18, SCE_GS_SET_XYOFFSET(ofsX, ofsY));
    m_packet->AddGifData(0x40, SCE_GS_SET_SCISSOR(rect.left, rect.right - 1, rect.top, rect.bottom - 1));
    m_packet->AddGifData(0x4C, SCE_GS_SET_FRAME(frame, m_width >> 6, m_psm, 0));
    m_packet->AddDmaEndTag();
    m_packet->SendGifPacket(1, 1);
    m_fadedOut = 0;
    m_fadingInFlag = 0;
    m_fadeLevel = alpha;
}
#pragma fast_fptosi reset


// ---- src/PSXVideoCard/ApplyPreFlipObjects.cpp

class VideoPreFlipObject {
public:
    virtual ~VideoPreFlipObject();
    virtual void Apply();
};


void PSXVideoCard::ApplyPreFlipObjects()
{
    for (int i = 0; i < m_preFlipObjects.m_numItems; i++)
        m_preFlipObjects.GetItem(i)->Apply();
}


// ---- src/PSXVideoCard/ApplyColorOverlay.cpp
// ASSERT contains inline asm; match retail's label alignment for asm functions.
#pragma alignlabel off





// Read with absolute loads, so declared const.


void PSXVideoCard::ApplyColorOverlay(unsigned char r, unsigned char g, unsigned char b, unsigned char a,
                                     unsigned long alphaBlend, _RECT *rect)
{
    if (LpGame->m_renderPhase != 3 && LpGame->m_renderPhase != 4) {
#line 2321 "PSXVideoCard.cpp"
        ASSERT(0, "This function may only be called in RenderPre3D!");
    }
    int viewport = LpGame->m_viewport;
#line 2325 "PSXVideoCard.cpp"
    ASSERT(viewport < 4, "Only 4 viewports are supported (0-3)!");
    m_colorOverlay[viewport] = 1;
    m_colorOverlayColor[viewport] = a | ((b << 8) | ((r << 24) | (g << 16)));
    m_overlayAlpha[viewport] = alphaBlend;
    if (rect) {
        m_colorOverlayRect[viewport] = *rect;
    } else {
        if (!LpRenderTarget->m_camera) {
            PSXprintf("DON'T CALL ApplyColorOverlay WITH NO CAMERA!!\n");
            return;
        }
        LpRenderTarget->m_camera->GetViewportBoundsPhysical(&m_colorOverlayRect[viewport]);
    }
}
#pragma alignlabel reset


// ---- src/PSXVideoCard/ApplyColorOverlay_00281B50.cpp
// ASSERT contains inline asm; match retail's label alignment for asm functions.
#pragma alignlabel off





// Read with absolute loads, so declared const.


void PSXVideoCard::ApplyColorOverlay(_RECT *rect)
{
    if (LpGame->m_renderPhase != 3 && LpGame->m_renderPhase != 4) {
#line 2350 "PSXVideoCard.cpp"
        ASSERT(0, "This function may only be called in RenderPre3D!");
    }
    int viewport = LpGame->m_viewport;
#line 2354 "PSXVideoCard.cpp"
    ASSERT(viewport < 4, "Only 4 viewports are supported (0-3)!");
    m_colorOverlay[viewport] = 1;
    if (rect) {
        m_colorOverlayRect[viewport] = *rect;
    } else {
        if (!LpRenderTarget->m_camera) {
            PSXprintf("DON'T CALL ApplyColorOverlay WITH NO CAMERA!!\n");
            return;
        }
        LpRenderTarget->m_camera->GetViewportBoundsPhysical(&m_colorOverlayRect[viewport]);
    }
}
#pragma alignlabel reset


// ---- src/PSXVideoCard/RemoveColorOverlay.cpp
// ASSERT contains inline asm; match retail's label alignment for asm functions.
#pragma alignlabel off


// Read with absolute loads, so declared const.


void PSXVideoCard::RemoveColorOverlay()
{
    if (LpGame->m_renderPhase != 3 && LpGame->m_renderPhase != 4) {
#line 2375 "PSXVideoCard.cpp"
        ASSERT(0, "This function may only be called in RenderPre3D!");
    }
    m_colorOverlay[LpGame->m_viewport] = 0;
}
#pragma alignlabel reset


// ---- src/PSXVideoCard/MotionBlur.cpp

// ASSERT contains inline asm; match retail's label alignment for asm functions.
#pragma alignlabel off





// Read with absolute loads, so declared const.


// Turns on motion blur for the current viewport: amount is a percentage,
// color 0xRRGGBB or -1 for neutral grey.
void PSXVideoCard::MotionBlur(int amount, _RECT *rect, int color)
{
    if (LpGame->m_renderPhase != 3 && LpGame->m_renderPhase != 4) {
#line 2388 "PSXVideoCard.cpp"
        ASSERT(0, "This function may only be called in RenderPre3D!");
    }
    int viewport = LpGame->m_viewport;
#line 2392 "PSXVideoCard.cpp"
    ASSERT(viewport < 4, "Only 4 viewports are supported (0-3)!");
    if (amount > 100)
        amount = 100;
    if (amount < 0)
        amount = 0;
    m_blurAlpha[viewport] = 128.0f * (amount / 100.0f);
    if (color == -1) {
        m_blurRed[viewport] = 0x80;
        m_blurGreen[viewport] = 0x80;
        m_blurBlue[viewport] = 0x80;
    } else {
        m_blurRed[viewport] = color >> 16;
        m_blurGreen[viewport] = color >> 8;
        m_blurBlue[viewport] = color;
    }
    if (LpRenderTarget->m_camera && !rect) {
        LpRenderTarget->m_camera->GetViewportBoundsPhysical(&m_blurRect[viewport]);
        return;
    }
    if (rect) {
        m_blurRect[viewport] = *rect;
    } else {
        m_blurRect[viewport].left = 0;
        m_blurRect[viewport].right = m_width;
        m_blurRect[viewport].top = 0;
        m_blurRect[viewport].bottom = m_height;
    }
}
#pragma alignlabel reset


// ---- src/PSXVideoCard/BltDisplayToZ.cpp
typedef unsigned long u_long;

// libgraph's GS register packing.
#define SCE_GS_SET_BITBLTBUF(sbp, sbw, spsm, dbp, dbw, dpsm) \
    ((u_long)(sbp) | ((u_long)(sbw) << 16) | ((u_long)(spsm) << 24) | \
     ((u_long)(dbp) << 32) | ((u_long)(dbw) << 48) | ((u_long)(dpsm) << 56))
#define SCE_GS_SET_TRXREG(rrw, rrh) ((u_long)(rrw) | ((u_long)(rrh) << 32))
#define SCE_GS_SET_ZBUF(zbp, psm, zmsk) ((u_long)(zbp) | ((u_long)(psm) << 24) | ((u_long)(zmsk) << 32))
#define SCE_GS_SET_DISPFB(fbp, fbw, psm, dbx, dby) \
    ((u_long)(fbp) | ((u_long)(fbw) << 9) | ((u_long)(psm) << 15) | \
     ((u_long)(dbx) << 32) | ((u_long)(dby) << 43))
#define SCE_GS_SET_DISPLAY(dx, dy, magh, magv, dw, dh) \
    ((u_long)(dx) | ((u_long)(dy) << 12) | ((u_long)(magh) << 23) | \
     ((u_long)(magv) << 27) | ((u_long)(dw) << 32) | ((u_long)(dh) << 44))
#define SCE_GS_BITBLTBUF 0x50
#define SCE_GS_TRXPOS 0x51
#define SCE_GS_TRXREG 0x52
#define SCE_GS_TRXDIR 0x53
#define SCE_GS_ZBUF_1 0x4E

#define GS_DISPFB1 ((volatile u_long *)0x12000070)
#define GS_DISPLAY1 ((volatile u_long *)0x12000080)

void WaitForVSync(int count);



// Copies the display buffer into the Z buffer and, if display is set,
// shows the Z buffer.
void PSXVideoCard::BltDisplayToZ(int display)
{
    int displayBuffer = GetCurrentDisplayBufferAddress();
    GetCurrentFrameBufferAddress();  // result unused
    VSyncComplete = 0;
    WaitForVSync(1);
    m_x2EC = 0;
    m_x2ED = 0;
    m_x2EE = 0;
    m_packet->ResetPacket();
    m_packet->AddSourceDmaTag();
    m_packet->AddGifTag(1);
    m_packet->AddGifData(SCE_GS_BITBLTBUF, SCE_GS_SET_BITBLTBUF(displayBuffer << 5, m_width >> 6, m_psm,
                                                                 m_zBufferAddress << 5, m_width >> 6, m_psm));
    m_packet->AddGifData(SCE_GS_TRXPOS, 0);
    m_packet->AddGifData(SCE_GS_TRXREG, SCE_GS_SET_TRXREG(m_width, m_height));
    m_packet->AddGifData(SCE_GS_TRXDIR, 2);
    m_packet->AddGifData(SCE_GS_ZBUF_1, SCE_GS_SET_ZBUF(m_zBufferAddress, m_zpsm, 1));
    m_packet->AddDmaEndTag();
    m_packet->SendGifPacket(1, 1);
    m_zmask = 1;
    if (display) {
        VSyncComplete = 0;
        WaitForVSync(1);
        *GS_DISPFB1 = SCE_GS_SET_DISPFB(m_zBufferAddress, m_width >> 6, m_psm, 0, 0);
        *GS_DISPLAY1 = SCE_GS_SET_DISPLAY(636, 50, (m_width + 2559) / m_width - 1, 0, (m_width - 1) * m_magh,
                                          m_height - 1);
        asm("sync");
        m_x2EF_bit2 = 1;
        m_alpha = 0xFF;
        VSyncComplete = 0;
        WaitForVSync(1);
    }
}


// ---- src/PSXVideoCard/BltZToDisplay.cpp
typedef unsigned long u_long;

// libgraph's GS register packing.
#define SCE_GS_SET_BITBLTBUF(sbp, sbw, spsm, dbp, dbw, dpsm) \
    ((u_long)(sbp) | ((u_long)(sbw) << 16) | ((u_long)(spsm) << 24) | \
     ((u_long)(dbp) << 32) | ((u_long)(dbw) << 48) | ((u_long)(dpsm) << 56))
#define SCE_GS_SET_TRXREG(rrw, rrh) ((u_long)(rrw) | ((u_long)(rrh) << 32))
#define SCE_GS_BITBLTBUF 0x50
#define SCE_GS_TRXPOS 0x51
#define SCE_GS_TRXREG 0x52
#define SCE_GS_TRXDIR 0x53



void PSXVideoCard::BltZToDisplay()
{
    int display = GetCurrentDisplayBufferAddress();
    GetCurrentFrameBufferAddress();  // result unused
    m_x2EC = 0;
    m_x2ED = 0;
    m_x2EE = 0;
    m_packet->ResetPacket();
    m_packet->AddSourceDmaTag();
    m_packet->AddGifTag(1);
    m_packet->AddGifData(SCE_GS_BITBLTBUF, SCE_GS_SET_BITBLTBUF(m_zBufferAddress << 5, m_width >> 6, m_psm,
                                                                 display << 5, m_width >> 6, m_psm));
    m_packet->AddGifData(SCE_GS_TRXPOS, 0);
    m_packet->AddGifData(SCE_GS_TRXREG, SCE_GS_SET_TRXREG(m_width, m_height));
    m_packet->AddGifData(SCE_GS_TRXDIR, 2);
    m_packet->AddDmaEndTag();
    m_packet->SendGifPacket(1, 1);
}


// ---- src/PSXVideoCard/BltFrameToDisplay.cpp
typedef unsigned long u_long;

// libgraph's GS register packing.
#define SCE_GS_SET_BITBLTBUF(sbp, sbw, spsm, dbp, dbw, dpsm) \
    ((u_long)(sbp) | ((u_long)(sbw) << 16) | ((u_long)(spsm) << 24) | \
     ((u_long)(dbp) << 32) | ((u_long)(dbw) << 48) | ((u_long)(dpsm) << 56))
#define SCE_GS_SET_TRXREG(rrw, rrh) ((u_long)(rrw) | ((u_long)(rrh) << 32))
#define SCE_GS_BITBLTBUF 0x50
#define SCE_GS_TRXPOS 0x51
#define SCE_GS_TRXREG 0x52
#define SCE_GS_TRXDIR 0x53



void PSXVideoCard::BltFrameToDisplay()
{
    int display = GetCurrentDisplayBufferAddress();
    int frame = GetCurrentFrameBufferAddress();
    m_x2EC = 0;
    m_x2ED = 0;
    m_x2EE = 0;
    m_packet->ResetPacket();
    m_packet->AddSourceDmaTag();
    m_packet->AddGifTag(1);
    m_packet->AddGifData(SCE_GS_BITBLTBUF, SCE_GS_SET_BITBLTBUF(frame << 5, m_width >> 6, m_psm,
                                                                 display << 5, m_width >> 6, m_psm));
    m_packet->AddGifData(SCE_GS_TRXPOS, 0);
    m_packet->AddGifData(SCE_GS_TRXREG, SCE_GS_SET_TRXREG(m_width, m_height));
    m_packet->AddGifData(SCE_GS_TRXDIR, 2);
    m_packet->AddDmaEndTag();
    m_packet->SendGifPacket(1, 1);
}


// ---- src/PSXVideoCard/GetCurrentFrameBufferAddress.cpp

int PSXVideoCard::GetCurrentFrameBufferAddress()
{
    if (m_bufferFlip)
        return 0;
    return m_frameBufferPages;
}


// ---- src/PSXVideoCard/GetCurrentDisplayBufferAddress.cpp

int PSXVideoCard::GetCurrentDisplayBufferAddress()
{
    return m_bufferFlip ? m_frameBufferPages : 0;
}


// ---- src/PSXVideoCard/GetZBufferAddress.cpp

int PSXVideoCard::GetZBufferAddress()
{
    return m_zBufferAddress;
}

int PSXVideoCard::GetZBufferFormat()
{
    return m_zpsm;
}

int PSXVideoCard::GetXOffset()
{
    return m_offsetX;
}

int PSXVideoCard::GetYOffset()
{
    return m_offsetY;
}


// ---- src/PSXVideoCard/EnableVSyncTo30Hz.cpp

void PSXVideoCard::EnableVSyncTo30Hz(int permanent)
{
    m_vsyncTo30Hz = 1;
    if (permanent)
        m_defaultVSyncTo30Hz = 1;
}


// ---- (ResetVSyncToDefault)
void PSXVideoCard::ResetVSyncToDefault()
{
    m_vsyncTo30Hz = m_defaultVSyncTo30Hz;
}


// ---- src/PSXVideoCard/DisableVSync.cpp

void PSXVideoCard::DisableVSync(int permanent)
{
    m_vsyncTo30Hz = 0;
    if (permanent)
        m_defaultVSyncTo30Hz = 0;
}


// ---- src/PSXVideoCard/SetShiftX.cpp
// ASSERT contains inline asm; match retail's label alignment for asm functions.
#pragma alignlabel off



void PSXVideoCard::SetShiftX(int shift)
{
    if ((shift < 0 ? -shift : shift) > 50) {
        // Retail passes only one argument for the two %d's.
#line 3133 "PSXVideoCard.cpp"
        ASSERTF(0, ("Shift must be less than %d and greater than -%d.\n", 50));
    }
    m_shiftX = shift;
}
#pragma alignlabel reset


// ---- src/PSXVideoCard/SetShiftY.cpp
// ASSERT contains inline asm; match retail's label alignment for asm functions.
#pragma alignlabel off



void PSXVideoCard::SetShiftY(int shift)
{
    if ((shift < 0 ? -shift : shift) > 50) {
        // Retail passes only one argument for the two %d's.
#line 3142 "PSXVideoCard.cpp"
        ASSERTF(0, ("Shift must be less than %d and greater than -%d.\n", 50));
    }
    m_shiftY = shift;
}
#pragma alignlabel reset


// ---- src/PSXVideoCard/GetShiftX.cpp

int PSXVideoCard::GetShiftX()
{
    return m_shiftX;
}


// ---- src/PSXVideoCard/GetShiftY.cpp

int PSXVideoCard::GetShiftY()
{
    return m_shiftY;
}

