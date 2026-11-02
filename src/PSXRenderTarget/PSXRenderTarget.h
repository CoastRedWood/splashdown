// PSXRenderTarget: the PS2 render target (0xE0 bytes): two DMA render queues
// built and sent alternately, plus fog and DMA channel state.
#ifndef PSXRENDERTARGET_PSXRENDERTARGET_H
#define PSXRENDERTARGET_PSXRENDERTARGET_H

#include "RenderTarget/RenderTarget.h"
#include "DmaRenderItem/DmaRenderItem.h"
#include "GsTextureTransfer/GsSyncPath.h"
#include "PSXVideoCard/PSXVideoCard.h"

class DmaPacket;
struct sceDmaChan;
struct _sceDmaTag;

// Kept for the files that used this header's old local PSXVideoCard.
extern PSXVideoCard *LpVideoCard;

enum DmaChannelType {
    DMA_CHANNEL_GIF,
    DMA_CHANNEL_VIF0,
    DMA_CHANNEL_VIF1
};

struct RenderQueue {
    _sceDmaTag *m_start;                // +0x0
    _sceDmaTag *m_cur;                  // +0x4
    int m_size;                         // +0x8
};

class PSXRenderTarget : public RenderTarget {
public:
    PSXRenderTarget();
    PSXRenderTarget *Create(PSXVideoCard *videoCard, int a);
    _sceDmaTag *AddToRenderQueue(DmaRenderItem *item, char *name);
    _sceDmaTag *AddToRenderQueue(_sceDmaTag *tags, int count, char *name);
    void AddIndexedItemToRenderQueue(_sceDmaTag *tag, DmaRenderItem *item);
    _sceDmaTag *AddMPGToRenderQueue(unsigned int *packet);
    void ResetRenderQueue(int queue);
    void SendRenderQueue(int queue);
    void LockTrySendRenderQueue(int (*fn)());
    void StopRenderQueue();
    int IsRenderingComplete();
    void DmaSend(DmaChannelType channel, void *chain, int flush, int);
    int SetViewport(_D3DVIEWPORT8 *viewport, DmaRenderItem *item, _D3DVIEWPORT8 *previous);
    void Reset();

    int m_buildQueue;                   // +0x44, 1: build into the second queue
    int m_sendQueue;                    // +0x48
    RenderQueue m_queue1;               // +0x4C
    RenderQueue m_queue2;               // +0x58
    unsigned int *m_lastMPG;            // +0x64
    // ContainerList<_sceDmaTag *>. Kept as bytes for the other includers:
    // ContainerList.h pulls in soultree_assert.h (clashes with
    // soultree_assert_block.h users). PSXRenderTarget.cpp, which constructs
    // and destroys it, defines PSXRENDERTARGET_TYPED_TAGS after including
    // ContainerList.h.
#ifdef PSXRENDERTARGET_TYPED_TAGS
    ContainerList<_sceDmaTag *> m_tags; // +0x68
#elif defined(SPLASHDOWN_PC)
    // ContainerList's fields, for its size and alignment with 8-byte pointers.
    struct { int m_numItems; void *m_items; int m_growBy, m_maxItems; unsigned char m_created; } m_tags;
#else
    char m_tags[0x14];                  // +0x68
#endif
    int m_x7C;                          // +0x7C
    DmaRenderItem m_renderItem;         // +0x80
    float m_fogStart;                   // +0xB0
    float m_fogEnd;                     // +0xB4
    float m_fogScale;                   // +0xB8, 255 / (start - end)
    int m_renderedStrips;               // +0xBC
    int m_renderedVertices;             // +0xC0
    int m_xC4;                          // +0xC4
    sceDmaChan *m_dmaGif;               // +0xC8
    sceDmaChan *m_dmaVif0;              // +0xCC
    sceDmaChan *m_dmaVif1;              // +0xD0
    DmaPacket *m_packet;                // +0xD4
    unsigned char m_queueRunning : 1;   // +0xD8 bit 0
    int (*m_finishCallback)();          // +0xDC, set by LockTrySendRenderQueue; also polled by
                                        // DmaPacket::SendGifPacket

    // --- virtuals in retail slot order ---
    virtual ~PSXRenderTarget();
    virtual int BeginScene();
    virtual int EndScene();
    virtual int Blt(_RECT *dst, TextureMap *texture, _RECT *src, unsigned int flags);
    virtual int Lock(_RECT *rect, long *pitch, unsigned int flags);
    virtual int Unlock(_RECT *rect);
    virtual int TrySendRenderQueue(int wait);
    virtual void WaitForRenderingToComplete();
    virtual void GetTextureStageState(unsigned int stage, _D3DTEXTURESTAGESTATETYPE type, unsigned int *value);
    virtual void SetTextureStageState(unsigned int stage, _D3DTEXTURESTAGESTATETYPE type, unsigned int value);
    virtual void SetRenderState(_D3DRENDERSTATETYPE state, unsigned int value, int);
    virtual void GetRenderState(_D3DRENDERSTATETYPE state, unsigned int *value);
    virtual void SetTextureState(_D3DTEXTUREBLEND blend, int value);
    virtual void DisableTextureStage(unsigned int stage);
    virtual void SetLight(unsigned int index, _D3DLIGHT8 *light);
    virtual void LightEnable(unsigned int index, int enable);
    virtual int Clear(_RECT *rect, unsigned int flags);
    virtual int PixelFormatAvailable(int format);
    virtual int SetViewport(_D3DVIEWPORT8 *viewport);
    virtual int DrawPrimitive(int type, unsigned int fvf, void *verts, unsigned int numVerts, unsigned int flags);
    virtual int DrawIndexedPrimitive(int type, unsigned int fvf, void *verts, unsigned int numVerts,
                                     unsigned short *indices, unsigned int numIndices, unsigned int flags);
    virtual void EnableAlphaTest(unsigned char ref);
    virtual void DisableAlphaTest();
};

#ifdef PSX_RENDER_TARGET_GLOBAL
extern PSXRenderTarget *LpRenderTarget;     // see RenderTarget.h
#endif

#endif
