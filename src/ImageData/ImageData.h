// ImageData: a texture image and its mip chain in one buffer, with the 5x5
// filter used to build the chain (0x260 bytes; the vptr comes last).
#ifndef IMAGEDATA_H
#define IMAGEDATA_H

#include "TextureMap/TextureMap.h"     // ImageDataFormat

struct FILE_HANDLE_STRUCT;
class ColorMapper;

// One mip level (0x18 bytes).
struct ImageDataEntry {
    int m_width;                        // +0x00
    int m_height;                       // +0x04
    int m_c;                            // +0x08
    int m_d;                            // +0x0C
    long m_offset;                      // +0x10 (64-bit), into the buffer
};

class ImageData {
public:
    ImageData();
    void Reset();
    int EnsureBufferSpace(long size);
    int LoadFromArray(unsigned char *data, ImageDataFormat format, int width, int height, int d, int c);
    int CreateMipMapChain();
    int DropMipMapChain();
    int Scale(int dstWidth, int dstHeight, int srcWidth, int srcHeight, short bytesPerPixel, unsigned char *dst,
              unsigned char *src, ImageDataFormat format);
    int Save(FILE_HANDLE_STRUCT *file);
    int Save(FILE_HANDLE_STRUCT *file, ImageDataFormat format);
    void Dump(const char *name, int minSize);

    unsigned char *m_buffer;            // +0x000 (malloc'd)
    ImageDataFormat m_format;           // +0x004
    short m_currentLevel;               // +0x008 (last mip level)
    char pad00A[0x10 - 0xA];
    long m_size;                        // +0x010 (64-bit)
    long m_bufferSize;                  // +0x018 (64-bit)
    double m_weights[5][5];             // +0x020 (5x5 filter kernel)
    ColorMapper *m_colorMapper;         // +0x0E8
    int m_xEC;                          // +0x0EC
    ImageDataEntry m_entries[15];       // +0x0F0
    short m_bytesPerPixel;              // +0x258

    // --- virtuals in retail slot order ---
    virtual ~ImageData();               // vptr +0x25C
};

#endif
