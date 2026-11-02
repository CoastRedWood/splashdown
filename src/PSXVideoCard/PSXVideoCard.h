// PSXVideoCard: the PS2 display (0x4F0 bytes) over the platform-independent
// VideoCard (0x2C bytes of data, vptr at +0x2C). LpVideoCard is the one
// instance.
#ifndef PSXVIDEOCARD_PSXVIDEOCARD_H
#define PSXVIDEOCARD_PSXVIDEOCARD_H

#include "_RECT/_RECT.h"
#include "ContainerList/ContainerList.h"

class DmaPacket;
class DmaRenderItem;
class PSXCamera;
class PSXTextureMap;

// One entry of PSXVideoCard::m_videoModes; Create fills in ten.
struct PSXVideoMode {
    int m_width;                        // +0x00
    int m_height;                       // +0x04
    int m_depth;                        // +0x08
    int m_vramAddress;                  // +0x0C
    int m_flags;                        // +0x10
};

class VideoCard {
public:
    VideoCard();

    int m_width;                        // +0x00
    int m_height;                       // +0x04
    int m_depth;                        // +0x08, bits per pixel (32)
    unsigned char m_flipPending : 1;    // +0x0C bit 0
    int m_x10;                          // +0x10
    int m_x14;                          // +0x14
    ContainerList<VideoPreFlipObject *> m_preFlipObjects;   // +0x18

    // --- virtuals in retail slot order ---
    virtual ~VideoCard();               // vptr +0x2C
    virtual void Release();
    virtual int SetVideoMode(int width, int height, int depth, int a, int b, int c, int d) = 0;
    virtual int DisplayToScreen() = 0;
    virtual void TryFlip(int wait) = 0;
    virtual void PauseVideoCardFlip(int pause);
    virtual void AddVideoCardEffects();
    virtual void SetViewportFadeColors(int startColor, int endColor) = 0;
    virtual void SetScreenFadeColors(int startColor, int endColor) = 0;
    virtual void FadeOutViewport(int frames, _RECT *rect) = 0;
    virtual void FadeInViewport(int frames, _RECT *rect) = 0;
    virtual void FadeOutScreen(int frames, int viewport) = 0;
    virtual void FadeInScreen(int frames, int viewport) = 0;
    virtual int IsViewportFaded(int viewport) = 0;
    virtual int IsScreenFaded() = 0;
    virtual void ApplyColorOverlay(unsigned char r, unsigned char g, unsigned char b, unsigned char a,
                                   unsigned long alpha, _RECT *rect) = 0;
    virtual void ApplyColorOverlay(_RECT *rect) = 0;
    virtual void RemoveColorOverlay() = 0;
};

class PSXVideoCard : public VideoCard {
public:
    PSXVideoCard();
    PSXVideoCard *Create();
    int SetVideoMode(int mode);
    int FindVideoMode(int width, int height, int depth);
    int CreateDoubleBuffer(int width, int height, int interlace, int frameMode);
    int SetDisplayEnvironment(short buffer);
    int SetDrawEnvironment(short buffer);
    int Flip(int wait);
    int ClearFrameBuffer(_RECT *rect, unsigned int color, unsigned int mode);
    int ClearFrameBuffer(_RECT *rect, PSXCamera *camera, unsigned int color, unsigned int mode);
    void ClearVRAM(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
    int GetCurrentFrameBufferAddress();
    int GetCurrentDisplayBufferAddress();
    int GetZBufferAddress();
    int GetZBufferFormat();
    int GetXOffset();
    int GetYOffset();
    int GetShiftX();
    int GetShiftY();
    void SetShiftX(int shift);
    void SetShiftY(int shift);
    void EnableVSyncTo30Hz(int permanent);
    void DisableVSync(int permanent);
    void ResetVSyncToDefault();
    void ApplyPreFlipObjects();
    void BltDisplayToZ(int display);
    void BltFrameToDisplay();
    void BltZToDisplay();
    void HardwareBlt(_RECT *dst, PSXTextureMap *texture, _RECT *src);
    void OverlayFrameBuffer(unsigned int color);
    void OverlayViewport(DmaRenderItem *item, unsigned int color, unsigned long alpha, _RECT *rect);
    void ExecuteColorOverlay();
    void MotionBlur(int amount, _RECT *rect, int color);
    void MotionBlurBuffers(DmaRenderItem *item, _RECT *rect);
    void ExecuteMotionBlur();
    void ExecuteDynamicScreenFading();
    void ExecuteDynamicViewportFading();
    void StaticFadeIn();
    void StaticFadeOut();
    void CrossFadeBuffers();

    int m_numModes;                     // +0x030
    int m_mode;                         // +0x034
    PSXVideoMode *m_videoModes;         // +0x038, 0x14 bytes each
    int m_bufferFlip;                   // +0x03C, which of the two buffers is displayed
    unsigned int m_frameCount;          // +0x040, counted by Flip
    int m_x44;                          // +0x044
    _RECT m_x48;                        // +0x048, 0, 0, width, height
    int m_psm;                          // +0x058
    int m_zpsm;                         // +0x05C
    int m_maxDepth;                     // +0x060, 0xFFFFFF
    unsigned int m_fpsFrameCount;       // +0x064, m_frameCount at the last FPS update (Flip)
    float m_fpsTime;                    // +0x068, ms since the last FPS update
    float m_lastFlipTime;               // +0x06C
    float m_flipTime;                   // +0x070
    float m_fps;                        // +0x074, averaged
    float m_frameRate;                  // +0x078, from the last frame
    float m_frameRateAfterVSync;        // +0x07C
    signed char m_shiftX;               // +0x080
    signed char m_shiftY;               // +0x081
    int m_flipPaused;                   // +0x084
    DmaPacket *m_packet;                // +0x088
    char pad8C[0x2C0 - 0x8C];
    int m_magh;                         // +0x2C0, GS MAGH for the mode
    char pad2C4[0x2D4 - 0x2C4];
    int m_x2D4;                         // +0x2D4
    int m_offsetX;                      // +0x2D8
    int m_offsetY;                      // +0x2DC
    int m_zBufferAddress;               // +0x2E0
    int m_frameBufferPages;             // +0x2E4
    short m_interlace;                  // +0x2E8
    short m_ffmd;                       // +0x2EA
    char m_x2EC;                        // +0x2EC
    char m_x2ED;                        // +0x2ED
    char m_x2EE;                        // +0x2EE
    unsigned char m_x2EF_bit0 : 1;      // +0x2EF bit 0
    unsigned char m_zmask : 1;          // +0x2EF bit 1
    unsigned char m_x2EF_bit2 : 1;      // +0x2EF bit 2
    unsigned int m_alpha;               // +0x2F0
    unsigned char m_viewportFaded[4];   // +0x2F4
    unsigned char m_viewportFadeOut[4]; // +0x2F8
    unsigned char m_viewportFading[4];  // +0x2FC
    unsigned char m_x300[4];            // +0x300
    int m_viewportFadeStartColor[4];    // +0x304
    int m_viewportFadeEndColor[4];      // +0x314
    int m_viewportFadeTarget[4];        // +0x324
    int m_viewportFadeLevel[4];         // +0x334
    int m_viewportFadeFrames[4];        // +0x344
    float m_viewportFadeStepB[4];       // +0x354
    float m_viewportFadeStepG[4];       // +0x364
    float m_viewportFadeStepR[4];       // +0x374
    _RECT m_viewportFadeRect[4];        // +0x384
    float m_viewportFadeStart[4];       // +0x3C4
    DmaRenderItem *m_x3D4;              // +0x3D4
    DmaRenderItem *m_x3D8;              // +0x3D8
    unsigned char m_fadedOut;           // +0x3DC
    int m_fadeEndColor;                 // +0x3E0
    int m_fadeStartColor;               // +0x3E4
    int m_fadeLevel;                    // +0x3E8, current alpha
    int m_fadeTarget;                   // +0x3EC, alpha fading to
    int m_fadeFrames;                   // +0x3F0, duration per alpha step
    float m_fadeStepB;                  // +0x3F4
    float m_fadeStepG;                  // +0x3F8
    float m_fadeStepR;                  // +0x3FC
    float m_fadeStart;                  // +0x400, f_timeGetTime() at the fade's start
    unsigned char m_fadingOut : 1;      // +0x404 bit 0
    unsigned char m_fadingInFlag : 1;   // +0x404 bit 1
    unsigned char m_fadeFlag : 1;       // +0x404 bit 2, dynamic (timed) fade
    unsigned char m_fadeDone : 1;       // +0x404 bit 3, overlay drawn this frame
    unsigned char m_fadingIn : 1;       // +0x404 bit 4, statically faded
    unsigned char m_crossFade;          // +0x405
    int m_x408;                         // +0x408
    int m_colorOverlay[4];              // +0x40C
    unsigned int m_colorOverlayColor[4]; // +0x41C
    char pad42C[0x430 - 0x42C];
    unsigned long m_overlayAlpha[4];    // +0x430
    _RECT m_colorOverlayRect[4];        // +0x450
    DmaRenderItem *m_x490;              // +0x490
    unsigned char m_blurAlpha[4];       // +0x494
    unsigned char m_blurRed[4];         // +0x498
    unsigned char m_blurGreen[4];       // +0x49C
    unsigned char m_blurBlue[4];        // +0x4A0
    _RECT m_blurRect[4];                // +0x4A4
    DmaRenderItem *m_x4E4;              // +0x4E4
    int m_vsyncTo30Hz;                  // +0x4E8
    int m_defaultVSyncTo30Hz;           // +0x4EC

    // --- virtuals in retail slot order ---
    virtual ~PSXVideoCard();
    virtual void Release();
    virtual int SetVideoMode(int width, int height, int depth, int a, int b, int c, int d);
    virtual int DisplayToScreen();
    virtual void TryFlip(int wait);
    virtual void PauseVideoCardFlip(int pause);
    virtual void AddVideoCardEffects();
    virtual void SetViewportFadeColors(int startColor, int endColor);
    virtual void SetScreenFadeColors(int startColor, int endColor);
    virtual void FadeOutViewport(int frames, _RECT *rect);
    virtual void FadeInViewport(int frames, _RECT *rect);
    virtual void FadeOutScreen(int frames, int viewport);
    virtual void FadeInScreen(int frames, int viewport);
    virtual int IsViewportFaded(int viewport);
    virtual int IsScreenFaded();
    virtual void ApplyColorOverlay(unsigned char r, unsigned char g, unsigned char b, unsigned char a,
                                   unsigned long alpha, _RECT *rect);
    virtual void ApplyColorOverlay(_RECT *rect);
    virtual void RemoveColorOverlay();
};
// LpVideoCard is left to the includers: Game's files declare it VideoCard *,
// the PSX layer PSXVideoCard *.

#endif
