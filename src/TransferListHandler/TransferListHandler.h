// TransferListHandler: PSXTextureMapManager.cpp's static list of texture
// transfers queued for each of the two frame buffers (0xC00C bytes; the
// static initializer constructs the one instance, TransferList).
#ifndef TRANSFERLISTHANDLER_TRANSFERLISTHANDLER_H
#define TRANSFERLISTHANDLER_TRANSFERLISTHANDLER_H

class PSXTextureMap;
struct IpuCacheHandle;

// One queued transfer (0x18 bytes): a texture, or a fill colour when m_texture is 0.
struct TransferSlotAssignment {
    PSXTextureMap *m_texture;           // +0x00, 0 for a fill
    int m_slot;                         // +0x04
    int m_address;                      // +0x08, the slot's video memory address
    int m_clutAddress;                  // +0x0C
    IpuCacheHandle *m_handle;           // +0x10, the decompressed image (compressed textures)
    unsigned int m_fillColor;           // +0x14
};

// The transfers queued for one frame buffer.
struct TransferBuffer {
    TransferSlotAssignment m_items[0x400];        // +0x0000
    int m_count;                        // +0x6000
};

class TransferListHandler {
public:
    TransferListHandler();
    void Reset();

    int m_current;                      // +0x0000, next item to send
    TransferBuffer m_buffers[2];        // +0x0004, built for frame buffer 1 and 2
};

#endif
