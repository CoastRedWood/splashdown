// PSXMpegVideoOutputBuffer: two decoded movie frames, each with the DMA
// packets that upload it and draw it (0x20 bytes, PSXMpegVideoDecoder's ctor
// news it).
#ifndef PSXMPEGVIDEOOUTPUTBUFFER_H
#define PSXMPEGVIDEOOUTPUTBUFFER_H

#include "PSXMpegVideoDecoder/PSXMpegDefs.h"

class DmaPacket;
struct sceIpuRGB32;

// One decoded frame (0x20 bytes): its pixels and the packets that upload it
// (to the Z buffer area) and draw it, with the GIF data words that
// GsTextureTransfer::TransferVideoFrameToFrameBuffer patches.
struct PSXMpegVideoOutputBufferData {
    sceIpuRGB32 *m_pixels;              // +0x00 (memalign'd, width * height * 4)
    DmaPacket *m_texturePacket;         // +0x04
    unsigned int *m_bitbltbuf;          // +0x08 (the texture packet's BITBLTBUF data)
    DmaPacket *m_spritePacket;          // +0x0C
    unsigned int *m_alpha;              // +0x10 (the sprite packet's ALPHA_1 data)
    unsigned int *m_topLeft;            // +0x14 (its first XYZ2)
    unsigned int *m_bottomRight;        // +0x18 (its second XYZ2)
    int m_frame;                        // +0x1C (sceMpeg frame count)
};

class PSXMpegVideoOutputBuffer {
public:
    PSXMpegVideoOutputBuffer(int width, int height);
    ~PSXMpegVideoOutputBuffer();
    void Initialize();
    void Reset();
    PSXMpegVideoOutputBufferData *Read();
    PSXMpegVideoOutputBufferData *Write();
    void UpdateDmaPackets(int x, int y, int width, int height, int format);
    static void InitializeTextureDmaPacket(PSXMpegVideoOutputBufferData *data, int width, int height, int format);
    static void InitializeSpriteDmaPacket(PSXMpegVideoOutputBufferData *data, int x, int y, int width, int height);
    int IsEmpty() { return m_count == 0; }
    int IsFull() { return m_count == PSXMpegDefs::VideoOutputBufferSize; }

    PSXMpegVideoOutputBufferData *m_data;   // +0x00 (two entries)
    int m_readPos;                      // +0x04
    int m_writePos;                     // +0x08
    int m_count;                        // +0x0C
    int m_readIndex;                    // +0x10
    int m_writeIndex;                   // +0x14
    int m_width;                        // +0x18
    int m_height;                       // +0x1C
};

#endif
