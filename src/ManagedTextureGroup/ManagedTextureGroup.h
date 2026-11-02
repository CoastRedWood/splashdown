// ManagedTextureGroup — a named set of textures loaded/unloaded together
// (0x28 bytes; TextureMapManager::Create news one as its store).
#ifndef MANAGEDTEXTUREGROUP_H
#define MANAGEDTEXTUREGROUP_H

#include "GameObject/GameObject.h"

class TextureMap;

// Intrusive list of texture maps, walked with First/Next (0x10 bytes).
class TextureMapList {
public:
    TextureMapList();
    ~TextureMapList();
    void Add(TextureMap *texture);
    void Remove(TextureMap *texture);
    TextureMap *First();
    TextureMap *Next();
    TextureMap *Last();

    TextureMap *m_head;     // +0x00
    TextureMap *m_tail;     // +0x04
    TextureMap *m_cur;      // +0x08
    int m_count;            // +0x0C
};

class ManagedTextureGroup : public BaseObject {
public:
    ManagedTextureGroup();
    void AddManagedTexture(TextureMap *texture);
    void RemoveManagedTexture(TextureMap *texture);
    void ClearUsage();

    // --- virtuals in retail slot order ---
    virtual ~ManagedTextureGroup();

    TextureMapList m_textures;  // +0x10
    int m_id;                   // +0x20 (-1)
};

#endif
