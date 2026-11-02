// SoultreeMaterial.cpp  @ 0x00296850 (text 0x296850-0x297040)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp)
//
// Retail's SoultreeMaterial.cpp as one translation unit: a TU without a
// static initializer inside SelectiveGravityModel.cpp's tu_map row. Its
// .rodata (0x41A928-0x41AA50: "" @485, the file strings @597/@655-@657,
// then the RTTI chain ending with SoultreeMaterial's) restarts the labels
// after soultree.cpp's RTTI; StripMesh.cpp's strings (@609..) follow it.
// `make link` places its data as well as its code.


#include "d3dtypes.h"
#include "GameObject/GameObject.h"
#include "DataAccess/DataAccess.h"
#include "PSXTextureMap/PSXTextureMap.h"
#include "SurfaceMap/SurfaceMap.h"
#include "SoultreeMaterial/SoultreeMaterial.h"

extern "C" int sprintf(char *buf, const char *fmt, ...);
extern "C" char *strcpy(char *dst, const char *src);
extern "C" char *strcat(char *dst, const char *src);
extern "C" char *strncat(char *dst, const char *src, unsigned int n);
extern "C" unsigned int strlen(const char *s);
extern "C" int strcasecmp(const char *a, const char *b);
void PSXprintf(const char *fmt, ...);
int IsAlphaFormat(int format);
class Palette8;
PSXTextureMap *GetTextureMapFromResourceName(char *name, int format, Palette8 *palette, unsigned int flags,
                                             _D3DBLEND src, _D3DBLEND dst, int alpha, unsigned int color, int mip);

extern DataAccess *LpDataAccess;
extern char SltTextureContentDirectory[];

// ---- src/SoultreeMaterial/ctor.cpp


SoultreeMaterial::SoultreeMaterial(int enabled) : GameObject(enabled)
{
    strcpy(m_name, "");
    m_texture = 0;
    m_xD4 = 0;
    m_name[0] = 0;
    m_hasTexture = 0;
    m_x8C = 0;
    m_x88 = 1;
    m_hasSrcBlend = 0;
    m_hasDstBlend = 0;
    m_texture = 0;   // set twice in retail
    m_blendMode = 0;
    m_xA0 = 0;
    m_xA4 = 1.0f;
    m_xB0 = 0;
    m_xA8 = 0;
    m_x84 = 0;
    m_x98 = 0;
    m_x9C = 1;
    m_hasAlpha = 0;
    m_colorKey = 0xFF00FF;
    m_xB8 = 0;
    m_xC4 = 0;
}


// ---- src/SoultreeMaterial/Create.cpp

SoultreeMaterial *SoultreeMaterial::Create(int enabled)
{
    GameObject::Create();
    strcpy(m_name, "");
    m_texture = 0;
    return this;
}

// ---- src/SoultreeMaterial/dtor.cpp


SoultreeMaterial::~SoultreeMaterial()
{
    if (m_texture) {
        m_texture->Release();
        m_texture = 0;
    }
    if (m_xD4) {
        m_xD4->Release();
        m_xD4 = 0;
    }
}


// ---- src/SoultreeMaterial/ReadFromBinaryFile.cpp


void SoultreeMaterial::ReadFromBinaryFile(long file, int loadTexture)
{
    LpDataAccess->FRead(file, m_name, 0x40, 1);
    LpDataAccess->FRead(file, &m_format, 4, 1);
    LpDataAccess->FRead(file, &m_colorKey, 4, 1);
    LpDataAccess->FRead(file, &m_srcBlend, 4, 1);
    LpDataAccess->FRead(file, &m_dstBlend, 4, 1);
    LpDataAccess->FRead(file, &m_hasTexture, 4, 1);
    LpDataAccess->FRead(file, &m_x84, 4, 1);
    LpDataAccess->FRead(file, &m_x88, 4, 1);
    LpDataAccess->FRead(file, &m_x8C, 4, 1);
    LpDataAccess->FRead(file, &m_hasSrcBlend, 4, 1);
    LpDataAccess->FRead(file, &m_hasDstBlend, 4, 1);
    LpDataAccess->FRead(file, &m_x9C, 4, 1);
    LpDataAccess->FRead(file, &m_x98, 4, 1);
    LpDataAccess->FRead(file, &m_xA0, 4, 1);
    LpDataAccess->FRead(file, &m_xA4, 4, 1);
    LpDataAccess->FRead(file, &m_xA8, 4, 1);
    LpDataAccess->FRead(file, &m_blendMode, 2, 1);
    LpDataAccess->FRead(file, &m_xB0, 4, 1);
    if (m_hasTexture && loadTexture)
        LoadMaterialTextureMap();
    // (No case for blend mode 9.)
    switch (m_blendMode) {
    case 0:
        m_hasAlpha = 0;
        break;
    case 1:
        m_hasAlpha = 1;
        break;
    case 3:
        m_hasAlpha = 1;
        break;
    case 4:
        m_hasAlpha = 1;
        break;
    case 2:
        m_hasAlpha = 1;
        break;
    case 5:
        m_hasAlpha = 1;
        break;
    case 6:
        m_hasAlpha = 0;
        break;
    case 8:
        m_hasAlpha = 1;
        break;
    case 10:
        m_hasAlpha = 0;
        break;
    case 7:
        m_hasAlpha = 1;
        break;
    }
}


// ---- src/SoultreeMaterial/LoadMaterialTextureMap.cpp

void SoultreeMaterial::LoadMaterialTextureMap()
{

    int format;
    int procedural;
    PSXTextureMap *map;

    char path[0x110];
    char msg[0x100];

    if (!m_texture)
        m_texture = new SurfaceMap(0, 0);
    if (m_texture->m_textureMap) {
        m_texture->m_textureMap->Release();
        m_texture->m_textureMap = 0;
    }
    format = m_format;
    if (m_x8C)
        format = 0x613;
    procedural = 0;
    if (!strcasecmp(m_name, "PROCEDURAL"))
        procedural = 1;
    IsAlphaFormat(format);  // result unused in retail
    if (procedural) {
        map = new PSXTextureMap(1);
        map->Load(0, 0x40, 0x40, 0x40, 1, 0, format, 0, m_x88 ? 2 : 0, 0, 0, 0,
                  m_hasSrcBlend ? m_srcBlend : (_D3DBLEND)5, m_hasDstBlend ? m_dstBlend : (_D3DBLEND)6, 0x80, 0xFF00FF);
    } else {
        strcpy(path, SltTextureContentDirectory);
        if (SltTextureContentDirectory[0]) {
            if (SltTextureContentDirectory[strlen(SltTextureContentDirectory) - 1] != '\\')
                strncat(path, "\\", 0x7F);
        }
        strcat(path, m_name);
        map = GetTextureMapFromResourceName(path, format, 0, m_x88 ? 2 : 0,
                                            m_hasSrcBlend ? m_srcBlend : (_D3DBLEND)5,
                                            m_hasDstBlend ? m_dstBlend : (_D3DBLEND)6, 0x80, m_colorKey, 1);
    }
    if (!map) {
        sprintf(msg, "TextureMap %s Not loaded, please check resources and current directory\n", m_name);
        PSXprintf(msg);
    }
    m_texture->SetTextureMap(map);
    if (map) {
        map->Release();
        if (!m_texture->m_textureMap->InVideoRAM()) {
            if (m_texture->m_textureMap->GetRefCount() == 1)
                m_texture->m_textureMap->PlaceInVideoMemory(1, 0, 0);
        }
    }
}


// ---- src/SoultreeMaterial/GetTextureMap.cpp



TextureMap *SoultreeMaterial::GetTextureMap()
{
    if (m_texture)
        return m_texture->m_textureMap;
    return 0;
}


