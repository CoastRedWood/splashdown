// TextureMapManager.cpp  @ 0x0029AD20 (text 0x29AD20-0x29B0A0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's TextureMapManager.cpp as one translation unit, a TU without an
// initializer inside Terrain.cpp's tu_map row: its .rodata ("Loaded Textures",
// "'%s'", then RTTI TextureMapManager, BaseObject, GameObject) restarts
// BaseObject at 0x41AD58, after TextureMap.cpp's. `make link` places its data
// too.


#include "GameObject/GameObject.h"
#include "TextureMapManager/TextureMapManager.h"
#include "ManagedTextureGroup/ManagedTextureGroup.h"
#include "TextureMap/TextureMap.h"
#include "ControlInterface/ControlTypes.h"

// Defined here for its retail .bss slot.
class PSXTextureMapManager;
PSXTextureMapManager *LpTextureMapManager;


extern "C" void *malloc(unsigned int size);
extern "C" void free(void *p);
void PSXprintf(const char *fmt, ...);
int KEYDOWN_HANDLER_TEST(unsigned int key, DeviceType device, ProcessNode *node, unsigned int mods);

// ---- src/TextureMapManager/ctor.cpp

TextureMapManager::TextureMapManager() : GameObject(1)
{
    m_store = 0;
    m_tempFileBuffer = 0;
    m_tempFileBufferSize = 0;
    m_tempImageBuffer = 0;
    m_tempImageBufferSize = 0;
}


// ---- src/TextureMapManager/dtor.cpp

TextureMapManager::~TextureMapManager()
{
    if (m_store) {
        m_store->Release();
        m_store = 0;
    }
    if (m_tempFileBuffer)
        free(m_tempFileBuffer);
    if (m_tempImageBuffer)
        free(m_tempImageBuffer);
}


// ---- src/TextureMapManager/Create.cpp

GameObject *TextureMapManager::Create()
{
    m_store = new ManagedTextureGroup;
    return GameObject::Create();
}


// ---- src/TextureMapManager/AllocTempFileBuffer.cpp
void *TextureMapManager::AllocTempFileBuffer(unsigned int size)
{
    if (m_tempFileBufferSize < size) {
        if (m_tempFileBuffer) {
            free(m_tempFileBuffer);
            m_tempFileBuffer = 0;
        }
        if ((m_tempFileBuffer = malloc(size)) == 0)
            return 0;
        m_tempFileBufferSize = size;
    }
    return m_tempFileBuffer;
}


// ---- src/TextureMapManager/AllocTempImageBuffer.cpp
void *TextureMapManager::AllocTempImageBuffer(unsigned int size)
{
    if (m_tempImageBufferSize < size) {
        if (m_tempImageBuffer) {
            free(m_tempImageBuffer);
            m_tempImageBuffer = 0;
        }
        if ((m_tempImageBuffer = malloc(size)) == 0)
            return 0;
        m_tempImageBufferSize = size;
    }
    return m_tempImageBuffer;
}


// ---- src/TextureMapManager/Tick.cpp


int TextureMapManager::Tick(float dt)
{
    return GameObject::Tick(dt);
}


// ---- src/TextureMapManager/RenderPre3D.cpp


int TextureMapManager::RenderPre3D()
{
    return GameObject::RenderPre3D();
}


// ---- src/TextureMapManager/PrepareGeometry.cpp


int TextureMapManager::PrepareGeometry()
{
    return GameObject::PrepareGeometry();
}


// ---- src/TextureMapManager/Render3D.cpp


int TextureMapManager::Render3D()
{
    return GameObject::Render3D();
}


// ---- src/TextureMapManager/RenderPost3D.cpp


int TextureMapManager::RenderPost3D()
{
    return GameObject::RenderPost3D();
}


// ---- src/TextureMapManager/KeyDownHandler.cpp



int TextureMapManager::KeyDownHandler(ProcessNode *node, KeyInfo *key)
{
    if (KEYDOWN_HANDLER_TEST(0x25, DEVICE_KEYBOARD, node, 0x80))
        DisplayLoadedTextures();
    return GameObject::KeyDownHandler(node, key);
}


// ---- src/TextureMapManager/DisplayLoadedTextures.cpp
void TextureMapManager::DisplayLoadedTextures()
{
    PSXprintf("Loaded Textures\n\n");
    for (TextureMap *t = m_store->m_textures.First(); t; t = m_store->m_textures.Next())
        PSXprintf("'%s'\n", t->m_name);
}

