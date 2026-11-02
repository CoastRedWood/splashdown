// PSXTextureMapManager: the PS2 texture manager (0x120 bytes; Game news it).
// It streams textures into four VRAM slots through GIF transfers and IPU
// decompression.
#ifndef PSXTEXTUREMAPMANAGER_H
#define PSXTEXTUREMAPMANAGER_H

#include "TextureMapManager/TextureMapManager.h"
#include "DmaRenderItem/DmaRenderItem.h"

class GsTextureTransfer;
class IpuTextureTransfer;
class IpuCache;
class PSXTextureMap;
struct TransferSlotAssignment;
struct IpuCacheHandle;
struct _sceDmaTag;

class PSXTextureMapManager : public TextureMapManager {
public:
    PSXTextureMapManager();
    void LockTextureSlot(PSXTextureMap *map);
    void UnlockTextureSlot(PSXTextureMap *map);
    int GetSlotAddress(int slot);
    int GetClutAddress(int slot);
    void CalculateVideoMemoryAddresses(int slot, PSXTextureMap *map, TransferSlotAssignment *assign);
    void AddTextureToGifList(PSXTextureMap *map);
    int AddFillTextureToGifList(int color);
    void *AddTextureInterruptToRenderQueue();
    void BeginTransfer();
    int SendCurrentTransfer(int wait);
    void ResetBuildList(int list);
    void ResetSendList();
    void ResetFrame();
    IpuCacheHandle *AddTextureToDecompressionQueue(PSXTextureMap *texture);
    void BeginDecompression();
    TextureMap *DecompressCurrent(int wait);
    void IPUDecompress(unsigned char *dst, unsigned char *src, int size, int width, int height, int unused,
                       int wait);
    void AllocateIpuCache(int memInBytes);
    void FreeIpuCache();
    void InvalidateIpuCache();
    void RemoveTextureFromIpuCache(PSXTextureMap *texture);
    void ViewImageData(unsigned char *data, int size);

    char pad48[0x50 - 0x48];
    DmaRenderItem m_x50;                // +0x50
    GsTextureTransfer *m_gsTransfer;    // +0x80
    IpuTextureTransfer *m_ipuTransfer;  // +0x84
    char pad88[0x90 - 0x88];
    DmaRenderItem m_x90;                // +0x90, the texture-interrupt item
    DmaRenderItem m_xC0;                // +0xC0
    int m_currentSlot;                  // +0xF0
    int m_slotLocked[4];                // +0xF4
    _sceDmaTag *m_x104;                 // +0x104
    int m_x108;                         // +0x108
    IpuCache *m_ipuCache;               // +0x10C
    int m_x110;                         // +0x110
    unsigned int *m_x114;               // +0x114, fill texture data (2x2 32-bit texels)
    GsTextureTransfer *m_x118;          // +0x118, fill texture transfer
    int m_x11C;                         // +0x11C, fill colour

    // --- virtuals in retail slot order ---
    virtual ~PSXTextureMapManager();
    virtual PSXTextureMapManager *Create();
    virtual int Tick(float dt);
    virtual int PrepareGeometry();
    virtual int RenderPre3D();
    virtual int Render3D();
    virtual int RenderPost3D();
    virtual int Restore();
    virtual int KeyDownHandler(ProcessNode *node, KeyInfo *key);
    virtual void RemoveFromVideoMemory();
    virtual void AddTextureMap(TextureMap *map);
    virtual void RemoveTextureMap(TextureMap *map);
    virtual int SetupCache();
    virtual void DisplayTexInRAM();
    virtual void ShowUsage();
};

#endif
