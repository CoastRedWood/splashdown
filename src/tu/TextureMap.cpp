// TextureMap.cpp  @ 0x0029AB40 (text 0x29AB40-0x29AD20)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's TextureMap.cpp as one translation unit, a TU without an
// initializer inside Terrain.cpp's tu_map row: its .rodata (RTTI
// TextureMap, BaseObject, BaseTextureMap) restarts BaseObject at 0x41ACB8,
// right after Terrain.cpp's. TextureMap's ctor, dtor and GetData and
// BaseTextureMap's dtor. `make link` places its data too.


#include "TextureMap/TextureMap.h"
#include "GameObject/GameObject.h"
#include "DataAccess/DataAccess.h"

// ---- src/TextureMap/GetData.cpp

// Retail returns the object itself, whatever the level.
void *TextureMap::GetData(int)
{
    return this;
}


// ---- src/TextureMap/ctor.cpp

extern TextureMapManager *LpTextureMapManager;

TextureMap::TextureMap(int addToManager)
{
    m_next = 0;
    m_prev = 0;
    m_minSize = 1;
    m_format = -1;
    m_numMips = 0;
    m_colorMapper = 0;
    m_mipShift = 0;
    m_xBC = 0;
    m_xC0 = 0;
    m_name[0] = 0;
    if (addToManager && LpTextureMapManager)
        LpTextureMapManager->AddTextureMap(this);
}


// ---- src/BaseTextureMap/dtor.cpp

// BaseTextureMap's dtor (0x29AC10) is TextureMap.h's inline one: the vtables
// this file emits need an out-of-line copy, which lands here.


// ---- src/TextureMap/dtor.cpp

extern DataAccess *LpDataAccess;

TextureMap::~TextureMap()
{
    if (LpTextureMapManager)
        LpTextureMapManager->RemoveTextureMap(this);
    LpDataAccess->DetachObject(this);
}

