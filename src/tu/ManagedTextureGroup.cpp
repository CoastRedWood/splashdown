// ManagedTextureGroup.cpp  @ 0x003933B0 (text 0x3933B0-0x393650)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's ManagedTextureGroup.cpp as one translation unit: a TU without a
// static initializer inside JetskiTraining.cpp's tu_map row (its ASSERTs say
// "ManagedTextureGroup.cpp" and its RTTI restarts at BaseObject).



#include "soultree_assert.h"
#include "TextureMap/TextureMap.h"
#include "ManagedTextureGroup/ManagedTextureGroup.h"

// ---- src/ManagedTextureGroup/ctor.cpp
// Retail keeps `this` in a stack slot and reloads it for every use: the
// signature of a function containing inline asm (the global optimizer is
// off). The empty asm statement reproduces that.
ManagedTextureGroup::ManagedTextureGroup() : m_id(-1)
{
    asm("");
}

// ---- src/ManagedTextureGroup/dtor.cpp
ManagedTextureGroup::~ManagedTextureGroup()
{
    TextureMap *texture = m_textures.Last();
    while (texture) {
        m_textures.Remove(texture);
        texture->Release();
        texture = m_textures.Last();
    }
}

// ---- src/ManagedTextureGroup/AddManagedTexture.cpp
// ASSERT contains inline asm; match retail's label alignment for asm
// functions (Add/RemoveManagedTexture only).
#pragma alignlabel off
void ManagedTextureGroup::AddManagedTexture(TextureMap *texture)
{
#line 33 "ManagedTextureGroup.cpp"
    ASSERT(texture, "AddManagedTexture: texture doesn't exist");
    m_textures.Add(texture);
}

// ---- src/ManagedTextureGroup/RemoveManagedTexture.cpp
void ManagedTextureGroup::RemoveManagedTexture(TextureMap *texture)
{
#line 39 "ManagedTextureGroup.cpp"
    ASSERT(texture, "RemoveManagedTexture: texture doesn't exist");
    m_textures.Remove(texture);
}

#pragma alignlabel reset

// ---- src/ManagedTextureGroup/ClearUsage.cpp
void ManagedTextureGroup::ClearUsage()
{
    for (TextureMap *texture = m_textures.First(); texture; texture = m_textures.Next())
        texture->m_usage = 0;
}
