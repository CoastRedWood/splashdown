// PSXTextureMap: a TextureMap with up to five PS2 mip surfaces (0x20C bytes
// of data, 0x210 with BaseObject's 8-byte alignment; Clone news 0x210).
#ifndef PSXTEXTUREMAP_PSXTEXTUREMAP_H
#define PSXTEXTUREMAP_PSXTEXTUREMAP_H

#include "TextureMap/TextureMap.h"
#include "GsTextureTransfer/GsTextureTransfer.h"

class Clut4;
class Clut8;
struct PSXTextureFileSurfaceHeader;

enum TexFormat {
    TEXFORMAT_4BIT = 0,
    TEXFORMAT_8BIT = 1,
    TEXFORMAT_COMPRESSED = 2
};

// One mip level (0x34 bytes).
struct PSXMipSurface {
    void *m_surface;                    // +0x00, also tested as "present"
    int m_width;                        // +0x04
    int m_blocks;                       // +0x08
    int m_widthLog2;                    // +0x0C
    int m_height;                       // +0x10
    int m_heightLog2;                   // +0x14
    void *m_data;                       // +0x18
    int m_compressedSize;               // +0x1C
    int m_slot;                         // +0x20, -1 when not resident
    unsigned short m_x24;               // +0x24, 0xFFFF until placed
    char pad26[0x28 - 0x26];
    int m_x28;                          // +0x28
    int m_x2C;                          // +0x2C
    unsigned char m_borrowed : 1;       // +0x30 bit 0, surface belongs to another map (Clone);
                                        // PrepareTransferTexture reads it as "compressed"
    unsigned char m_tiled : 1;          // +0x30 bit 1
    unsigned char m_half : 1;           // +0x30 bit 2
};

class PSXTextureMap : public TextureMap {
public:
    PSXTextureMap(int addToManager);
    int AllocateAllSurfaces();
    int AllocateCompressedSurface(PSXMipSurface *surface, int size, PSXTextureFileSurfaceHeader *header);
    int AllocateUncompressedSurface(TexFormat format, PSXMipSurface *surface, PSXTextureFileSurfaceHeader *header);
    int ReadSurfaceFromFile(long file, TexFormat format, PSXMipSurface *surface, PSXTextureFileSurfaceHeader *header);
    void SkipSurfaceInFile(long file, TexFormat format, int width, int height);
    int LoadFromContainer(long file, unsigned int flags);
    void CalculateReqdMipLevel(float distance);
    PSXMipSurface *GetReqdMipLevel();
    PSXMipSurface *GetTopMipLevel();
    int MapUsageToLevel(int usage);
    GsTextureTransfer *PrepareTransferTexture(GsTextureTransfer *transfer, int x, int y, PSXMipSurface *mip,
                                              unsigned char *data);

    int m_bitsPerPixel;                 // +0xE8
    int m_xEC;                          // +0xEC
    int m_xF0;                          // +0xF0
    int m_reqdMipLevel;                 // +0xF4
    unsigned char m_compressed : 1;     // +0xF8 bit 0
    unsigned char m_noMipChain : 1;     // +0xF8 bit 1
    unsigned char m_x4 : 1;             // +0xF8 bit 2
    unsigned char m_prepared : 1;       // +0xF8 bit 3
    GsTextureTransfer m_transfer;       // +0xFC
    PSXMipSurface m_mips[5];            // +0x100
    Clut4 *m_clut4;                     // +0x204
    Clut8 *m_clut8;                     // +0x208

    // --- virtuals in retail slot order ---
    virtual ~PSXTextureMap();
    virtual void GetReportData(TextureReportData *report);
    virtual int Load(long file, int width, int height, int levels, ImageDataFormat format, int size,
                     int pixelFormat, ColorMapper *mapper, unsigned int flags, int, _D3DBLEND srcBlend,
                     _D3DBLEND destBlend, int colorKeyMode, unsigned int colorKey);
    virtual int Load(unsigned char *data, int width, int height, int pitch, int minSize, int srcFormat,
                     int destFormat, ColorMapper *mapper, unsigned int flags, int, int, unsigned char *,
                     _D3DBLEND srcBlend, _D3DBLEND destBlend, int colorKeyMode, unsigned int colorKey);
    virtual PSXTextureMap *Clone();
    virtual int InVideoRAM();
    virtual int PlaceInVideoMemory(int, int, int);
    virtual int UpdateVideoMemory(_RECT *rect, int);
    virtual void RemoveFromVideoMemory();
    virtual int SetTextureStage();
    virtual void Restore();
    virtual void *LockSys(_RECT *rect, long *pitch, unsigned int flags);
    virtual int UnlockSys(_RECT *rect);
    virtual int BuildMipMapChain(int);
    virtual void *LockMipMapSurface(int level);
    virtual int UnlockMipMapSurface(int level);
    virtual void SetRenderStates();
    virtual int DumpChain();
};

#endif
