// SoultreeMaterial: a Soultree model material (texture name, format,
// colour key, blend states) and its SurfaceMap (0xD8 bytes,
// WaterObjectData::ReadMaterialsFromBinaryFile news it).
#ifndef SOULTREEMATERIAL_H
#define SOULTREEMATERIAL_H

#include "GameObject/GameObject.h"
#include "RenderTarget/D3DEnums.h"     // _D3DBLEND

class SurfaceMap;
class TextureMap;

class SoultreeMaterial : public GameObject {
public:
    SoultreeMaterial(int enabled);
    SoultreeMaterial *Create(int enabled);
    void ReadFromBinaryFile(long file, int loadTexture);
    void LoadMaterialTextureMap();
    TextureMap *GetTextureMap();

    // --- virtuals in retail slot order ---
    virtual ~SoultreeMaterial();

    char m_name[0x40];                  // +0x30 (texture name, "PROCEDURAL" for a generated one)
    int m_format;                       // +0x70
    unsigned int m_colorKey;            // +0x74 (0xFF00FF)
    _D3DBLEND m_srcBlend;               // +0x78
    _D3DBLEND m_dstBlend;               // +0x7C
    int m_hasTexture;                   // +0x80
    int m_x84;                          // +0x84
    int m_x88;                          // +0x88 (1)
    int m_x8C;                          // +0x8C (forces format 0x613)
    int m_hasSrcBlend;                  // +0x90
    int m_hasDstBlend;                  // +0x94
    int m_x98;                          // +0x98
    int m_x9C;                          // +0x9C (1)
    int m_xA0;                          // +0xA0
    float m_xA4;                        // +0xA4 (1.0)
    int m_xA8;                          // +0xA8
    unsigned short m_blendMode;         // +0xAC
    int m_xB0;                          // +0xB0
    int m_hasAlpha;                     // +0xB4 (from m_blendMode)
    int m_xB8;                          // +0xB8
    char padBC[0xC4 - 0xBC];
    int m_xC4;                          // +0xC4
    char padC8[0xD0 - 0xC8];
    SurfaceMap *m_texture;              // +0xD0 (LoadMaterialTextureMap news it; the dtor releases it)
    BaseObject *m_xD4;                  // +0xD4 (the dtor releases it)
};

#endif
