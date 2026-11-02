// RenderTarget: the platform-independent render target (0x40 bytes of data,
// vptr at +0x40, 0x44 in all). PSXRenderTarget implements the pure virtuals.
#ifndef RENDERTARGET_RENDERTARGET_H
#define RENDERTARGET_RENDERTARGET_H

#include "_RECT/_RECT.h"
#include "RenderTarget/D3DEnums.h"

class Camera;
class PSXVideoCard;
class TextureMap;
struct _D3DVIEWPORT8;
struct _D3DLIGHT8;

class RenderTarget {
public:
    RenderTarget();
    void Create(PSXVideoCard *card);
    void SetCamera(Camera *camera);
    void NextCyclingBufferId();

    Camera *m_camera;                   // +0x00
    int m_width;                        // +0x04
    int m_height;                       // +0x08
    int m_frameCount;                   // +0x0C (NextCyclingBufferId uses it as the buffer count)
    int m_bufferId;                     // +0x10, cycles 0..m_frameCount-1
    int m_cycleCount;                   // +0x14
    char pad18[0x20 - 0x18];
    int m_screenType;                   // +0x20
    float m_x24;                        // +0x24
    unsigned int m_clearColor;          // +0x28
    int m_hasDepth;                     // +0x2C
    int m_x30;                          // +0x30
    int m_x34;                          // +0x34
    int m_x38;                          // +0x38
    int m_x3C;                          // +0x3C

    // --- virtuals in retail slot order ---
    virtual ~RenderTarget();            // vptr +0x40
    virtual int BeginScene() = 0;
    virtual int EndScene() = 0;
    virtual int Blt(_RECT *dst, TextureMap *texture, _RECT *src, unsigned int flags) = 0;
    virtual int Lock(_RECT *rect, long *pitch, unsigned int flags) = 0;
    virtual int Unlock(_RECT *rect) = 0;
    virtual int TrySendRenderQueue(int wait);
    virtual void WaitForRenderingToComplete() = 0;
    virtual void GetTextureStageState(unsigned int stage, _D3DTEXTURESTAGESTATETYPE type, unsigned int *value) = 0;
    virtual void SetTextureStageState(unsigned int stage, _D3DTEXTURESTAGESTATETYPE type, unsigned int value) = 0;
    virtual void SetRenderState(_D3DRENDERSTATETYPE state, unsigned int value, int) = 0;
    virtual void GetRenderState(_D3DRENDERSTATETYPE state, unsigned int *value) = 0;
    virtual void SetTextureState(_D3DTEXTUREBLEND blend, int value) = 0;
    virtual void DisableTextureStage(unsigned int stage) = 0;
    virtual void SetLight(unsigned int index, _D3DLIGHT8 *light) = 0;
    virtual void LightEnable(unsigned int index, int enable) = 0;
    virtual int Clear(_RECT *rect, unsigned int flags) = 0;
    virtual int PixelFormatAvailable(int format) = 0;
    virtual int SetViewport(_D3DVIEWPORT8 *viewport) = 0;
    virtual int DrawPrimitive(int type, unsigned int fvf, void *verts, unsigned int numVerts,
                              unsigned int flags) = 0;
    virtual int DrawIndexedPrimitive(int type, unsigned int fvf, void *verts, unsigned int numVerts,
                                     unsigned short *indices, unsigned int numIndices, unsigned int flags) = 0;
    virtual void EnableAlphaTest(unsigned char ref);
    virtual void DisableAlphaTest();
};
// Game's view; PSX-layer files declare it PSXRenderTarget * themselves. A
// TU that defines PSX_RENDER_TARGET_GLOBAL before its includes gets
// PSXRenderTarget.h's PSXRenderTarget * declaration instead (a cast of this
// one can change the load order: PSXSurfaceShaderInstance::Render).
#ifndef PSX_RENDER_TARGET_GLOBAL
extern RenderTarget *LpRenderTarget;
#endif

#endif
