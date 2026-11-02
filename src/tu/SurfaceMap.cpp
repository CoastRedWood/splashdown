// SurfaceMap.cpp  @ 0x002976D0 (text 0x2976D0-0x297900)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp)
//
// Retail's SurfaceMap.cpp as one translation unit: a TU without a static
// initializer, the last of SelectiveGravityModel.cpp's tu_map row. Its
// .rodata (0x41AAA0-0x41AAE0, labels restarting at @571 and ending with
// SurfaceMap's RTTI) follows StripMesh.cpp's. `make link` places its data
// as well as its code.


#include "d3dtypes.h"
#include "PSXTextureMap/PSXTextureMap.h"
#include "SurfaceMap/SurfaceMap.h"

extern "C" void *memcpy(void *dst, const void *src, unsigned int n);

// ---- src/SurfaceMap/ctor.cpp

SurfaceMap::SurfaceMap(PSXTextureMap *map, _D3DMATERIAL8 *material)
{
    m_textureMap = map;
    if (m_textureMap)
        m_textureMap->AddRef();
    if (material) {
        memcpy(&m_material, material, sizeof(_D3DMATERIAL8));
    } else {
        m_material.Diffuse.r = 1.0f;
        m_material.Diffuse.g = 1.0f;
        m_material.Diffuse.b = 1.0f;
        m_material.Diffuse.a = 1.0f;
        m_material.Ambient.r = 1.0f;
        m_material.Ambient.g = 1.0f;
        m_material.Ambient.b = 1.0f;
        m_material.Ambient.a = 1.0f;
        m_material.Specular.r = 0.0f;
        m_material.Specular.g = 0.0f;
        m_material.Specular.b = 0.0f;
        m_material.Specular.a = 0.0f;
        m_material.Emissive.r = 0.0f;
        m_material.Emissive.g = 0.0f;
        m_material.Emissive.b = 0.0f;
        m_material.Emissive.a = 0.0f;
        m_material.Power = 0.0f;
    }
    Changed();
}

// ---- src/SurfaceMap/SetTextureMap.cpp

int SurfaceMap::SetTextureMap(PSXTextureMap *map)
{
    if (m_textureMap)
        m_textureMap->Release();
    m_textureMap = map;
    if (m_textureMap)
        m_textureMap->AddRef();
    return Changed();
}

// ---- src/SurfaceMap/Changed.cpp

int SurfaceMap::Changed()
{
    return 1;
}

// ---- src/SurfaceMap/dtor.cpp

SurfaceMap::~SurfaceMap()
{
    if (m_textureMap)
        m_textureMap->Release();
}
