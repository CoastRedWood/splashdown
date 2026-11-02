// LightFlare.cpp  @ 0x0034F6F0 (text 0x34F6F0-0x34FA00)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's LightFlare.cpp as one translation unit: LightFlare::SetColor,
// SearchableGameObject's ctor and BaseTextureMap's ctor and
// AddUsageViaProjectedSize, then the static initializer. Its only .rodata is
// the "SearchableGameObject" type name (@439, right after PSXMesh.cpp's
// RTTI); KeyFrameController.cpp's strings (@1668...) follow. Nothing in the
// data separates the BaseTextureMap functions from KeyFrameController.cpp
// (they use no data of their own); they are taken with this TU, before
// KeyFrameController.cpp's PositionController. The rest of the tu_map row
// (0x34FA00-0x351970) is KeyFrameController.cpp
// (src/tu/KeyFrameController.cpp). `make link` places the object's data as
// well as its code (tools/link_c.py).


#include "GameObject/GameObject.h"
#include "Mesh/Mesh.h"
#include "TypeRegistry/TypeRegistry.h"
#include "SearchableObject/SearchableObject.h"
#include "SearchableGameObject/SearchableGameObject.h"
#include "TextureMap/TextureMap.h"
#include "sinit/AxisVectors.h"

// ---- src/LightFlare/SetColor.cpp

// Only SetColor is out of line here; no header declares LightFlare.
class LightFlare {
public:
    void SetColor(const int &color);

    char m_pad0[0x90];
    Mesh *m_mesh;   // +0x90, a single 4-vertex quad
};

void LightFlare::SetColor(const int &color)
{
    m_mesh->SetRGBA(0, 0, color);
    m_mesh->SetRGBA(0, 1, color);
    m_mesh->SetRGBA(0, 2, color);
    m_mesh->SetRGBA(0, 3, color);
}



// ---- src/SearchableGameObject/ctor.cpp

extern TypeRegistry *LpTypeRegistry;

unsigned char SearchableGameObjectTypeID = 0xFF;   // until registered



SearchableGameObject::SearchableGameObject(int enabled) : GameObject(enabled)
{
    if (SearchableGameObjectTypeID == 0xFF)
        SearchableGameObjectTypeID = LpTypeRegistry->GetTypeID("SearchableGameObject");
    m_typeID = SearchableGameObjectTypeID;
    m_searchVisible = 1;
    m_addToList = 0;
    SearchableObject::m_hidden = 0;
}


// ---- src/BaseTextureMap/ctor.cpp

BaseTextureMap::BaseTextureMap()
{
    m_usage = 0;
    m_width = 0;
    m_height = 0;
}


// ---- src/BaseTextureMap/AddUsageViaProjectedSize.cpp
#pragma fast_fptosi on  // retail never calls fptosi: float->int is an inline cvt.w.s


void BaseTextureMap::AddUsageViaProjectedSize(float size)
{
    if (size > 2.0f * m_width)
        size = 2.0f * m_width;
    int pixels = (int)size;
    int level;
    int base;
    if (pixels >= 256) {
        base = 256;
        level = 8;
    } else if (pixels >= 128) {
        base = 128;
        level = 7;
    } else if (pixels >= 64) {
        base = 64;
        level = 6;
    } else if (pixels >= 32) {
        base = 32;
        level = 5;
    } else {
        base = 16;
        level = 4;
    }
    float frac = (size - base) / base;
    if (frac < 0.0f)
        frac = 0.0f;
    float usage = level + frac;
    if (usage > m_usage)
        m_usage = usage;
}

#pragma fast_fptosi reset


// ---- src/sinit/LightFlare.cpp
// Static initializer of LightFlare.cpp: only the axis vectors from the shared header.
