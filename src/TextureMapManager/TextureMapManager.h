// TextureMapManager: the platform-independent texture manager (0x44 bytes of
// data, 0x48 with GameObject's 8-byte alignment). PSXTextureMapManager
// implements the pure virtuals.
#ifndef TEXTUREMAPMANAGER_TEXTUREMAPMANAGER_H
#define TEXTUREMAPMANAGER_TEXTUREMAPMANAGER_H

#include "GameObject/GameObject.h"

class TextureMap;
class ManagedTextureGroup;

class TextureMapManager : public GameObject {
public:
    TextureMapManager();
    void DisplayLoadedTextures();

    ManagedTextureGroup *m_store;       // +0x30, new'd by Create (0x28 bytes)
    void *m_tempFileBuffer;             // +0x34
    void *m_tempImageBuffer;            // +0x38
    unsigned int m_tempFileBufferSize;  // +0x3C
    unsigned int m_tempImageBufferSize; // +0x40

    // --- virtuals in retail slot order ---
    virtual ~TextureMapManager();
    virtual GameObject *Create();
    virtual int Tick(float dt);
    virtual int PrepareGeometry();
    virtual int RenderPre3D();
    virtual int Render3D();
    virtual int RenderPost3D();
    virtual int Restore() = 0;
    virtual int KeyDownHandler(ProcessNode *node, KeyInfo *key);
    virtual void RemoveFromVideoMemory() = 0;                   // slot 27
    virtual void AddTextureMap(TextureMap *map) = 0;            // vtable +0x78
    virtual void RemoveTextureMap(TextureMap *map) = 0;         // vtable +0x7C
    virtual void *AllocTempFileBuffer(unsigned int size);
    virtual void *AllocTempImageBuffer(unsigned int size);
    virtual int SetupCache() = 0;
    virtual void DisplayTexInRAM() = 0;
    virtual void ShowUsage() = 0;
};
// LpTextureMapManager is left to the includers: TextureMap's files declare it
// TextureMapManager *, the PSX layer PSXTextureMapManager *.

#endif
