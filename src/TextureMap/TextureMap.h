// TextureMap (0xE8 bytes) and its base BaseTextureMap (0x1C of data, 0x20
// with BaseObject's 8-byte alignment). PSXTextureMap implements the pure
// virtuals; new TextureMaps register with LpTextureMapManager.
#ifndef TEXTUREMAP_TEXTUREMAP_H
#define TEXTUREMAP_TEXTUREMAP_H

#include "GameObject/GameObject.h"
#include "TextureMapManager/TextureMapManager.h"
#include "RenderTarget/D3DEnums.h"

class ColorMapper;
struct TextureReportData;
struct _RECT;

enum ImageDataFormat {};

class BaseTextureMap : public BaseObject {
public:
    BaseTextureMap();
    void AddUsageViaProjectedSize(float size);

    float m_usage;                      // +0x10, highest mip level needed
    int m_width;                        // +0x14
    int m_height;                       // +0x18

    // Out of line in retail (0x29AC10) but defined in TextureMap's file, which inlines it.
    virtual ~BaseTextureMap() {}
    virtual void *GetData(int level) = 0;
    virtual void GetReportData(TextureReportData *report) = 0;
};

class TextureMap : public BaseTextureMap {
public:
    TextureMap(int addToManager);

    char m_name[0x80];                  // +0x20
    TextureMap *m_next;                 // +0xA0
    TextureMap *m_prev;                 // +0xA4
    int m_minSize;                      // +0xA8, smallest mip width
    int m_format;                       // +0xAC
    int m_numMips;                      // +0xB0
    ColorMapper *m_colorMapper;         // +0xB4
    int m_mipShift;                     // +0xB8
    int m_xBC;                          // +0xBC
    int m_xC0;                          // +0xC0
    char padC4[0xE8 - 0xC4];

    // --- virtuals in retail slot order ---
    virtual ~TextureMap();
    virtual void *GetData(int level);
    virtual void GetReportData(TextureReportData *report) = 0;
    virtual int Load(long file, int width, int height, int levels, ImageDataFormat format, int size,
                     int pixelFormat, ColorMapper *mapper, unsigned int flags, int, _D3DBLEND srcBlend,
                     _D3DBLEND destBlend, int colorKeyMode, unsigned int colorKey) = 0;
    virtual int Load(unsigned char *data, int width, int height, int pitch, int minSize, int srcFormat,
                     int destFormat, ColorMapper *mapper, unsigned int flags, int, int, unsigned char *,
                     _D3DBLEND srcBlend, _D3DBLEND destBlend, int colorKeyMode, unsigned int colorKey) = 0;
    virtual TextureMap *Clone() = 0;
    virtual int InVideoRAM() = 0;
    virtual int PlaceInVideoMemory(int, int, int) = 0;
    virtual int UpdateVideoMemory(_RECT *rect, int) = 0;
    virtual void RemoveFromVideoMemory() = 0;
    virtual int SetTextureStage() = 0;
    virtual void Restore() = 0;
    virtual void *LockSys(_RECT *rect, long *pitch, unsigned int flags) = 0;
    virtual int UnlockSys(_RECT *rect) = 0;
    virtual int BuildMipMapChain(int) = 0;
    virtual void *LockMipMapSurface(int level) = 0;
    virtual int UnlockMipMapSurface(int level) = 0;
    virtual void SetRenderStates() = 0;
    virtual int DumpChain() = 0;
};

#endif
