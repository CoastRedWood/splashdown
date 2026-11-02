// SurfaceMap: a texture map plus a Direct3D material (a BaseObject, 0x78
// bytes: SoultreeMaterial::LoadMaterialTextureMap news it as its
// m_texture). Retail's SurfaceMap.cpp holds the ctor, SetTextureMap, Changed
// and the dtor.
#ifndef SURFACEMAP_SURFACEMAP_H
#define SURFACEMAP_SURFACEMAP_H

#include "GameObject/GameObject.h"     // BaseObject
#include "d3dtypes.h"                  // _D3DCOLORVALUE

class PSXTextureMap;
class TextureMap;

#ifndef D3DMATERIAL8_DEFINED
#define D3DMATERIAL8_DEFINED
struct _D3DMATERIAL8 {
    _D3DCOLORVALUE Diffuse;
    _D3DCOLORVALUE Ambient;
    _D3DCOLORVALUE Specular;
    _D3DCOLORVALUE Emissive;
    float Power;
};
#endif

class SurfaceMap : public BaseObject {
public:
    SurfaceMap(PSXTextureMap *map, _D3DMATERIAL8 *material);
    int SetTextureMap(PSXTextureMap *map);
    int Changed();

    // --- virtuals in retail slot order ---
    virtual ~SurfaceMap();

    TextureMap *m_textureMap;           // +0x10 (AddRef'd)
    _D3DMATERIAL8 m_material;           // +0x14
    char pad58[0x78 - 0x58];
};

#endif
