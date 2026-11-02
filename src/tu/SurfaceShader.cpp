// SurfaceShader.cpp  @ 0x0034AA90 (text 0x34AA90-0x34B050)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's SurfaceShader.cpp (the name its ASSERT gives) as one translation
// unit, a TU without an initializer inside PSXSurfaceMaterial.cpp's tu_map
// row: SurfaceShaderInstance and SurfaceShaderList. Its .rodata
// (0x436820-0x4368BC, labels @539-562) is the ASSERT strings and the
// SurfaceShaderList and SurfaceShaderInstance RTTI.



#include "SurfaceShaderInstance/SurfaceShaderInstance.h"
#include "SurfaceShaderList/SurfaceShaderList.h"
#include "BlockAllocator/BlockAllocator.h"
#include "soultree_assert_block.h"
#include "ShaderDescriptor/ShaderDescriptor.h"
#include "PSXSurfaceShaderInstance/PSXSurfaceShaderInstance.h"

// Defined here for its retail .bss slot.
SurfaceShaderList *LpSurfaceShaderList;


extern SurfaceShaderList *LpSurfaceShaderList;

// ---- src/SurfaceShaderInstance/ctor.cpp

SurfaceShaderInstance::SurfaceShaderInstance()
{
    m_descriptor = 0;
    m_list = 0;
    m_head = 0;
    m_tail = 0;
    m_frame = 0;
    m_next = 0;
}


// ---- src/SurfaceShaderInstance/dtor.cpp

SurfaceShaderInstance::~SurfaceShaderInstance()
{
    if (LpSurfaceShaderList)
        delete m_list;
}


// ---- src/SurfaceShaderInstance/AddGeometry.cpp




SurfaceGeometryNode *SurfaceShaderInstance::AddGeometry(SurfaceGeometry *geometry)
{
    if (m_next) {
        return m_next->AddGeometry(geometry);
    }
    SurfaceGeometryNode *node = (SurfaceGeometryNode *)LpSurfaceShaderList->m_allocator->AllocBlock();
    node->next = 0;
    node->geometry = geometry;
    if (m_frame != LpSurfaceShaderList->m_frame) {
        m_frame = LpSurfaceShaderList->m_frame;
        m_head = m_tail = node;
    } else {
        m_tail->next = node;
        m_tail = node;
    }
    return node;
}


// ---- src/SurfaceShaderList/ctor.cpp
extern "C" void *memset(void *dst, int c, unsigned int n);



SurfaceShaderList::SurfaceShaderList()
{
    memset(m_shaders, 0, sizeof(m_shaders));
    m_numShaders = 0;
    m_allocator = new BlockAllocator(sizeof(SurfaceGeometryNode), 0x1000);
    m_frame = 1;
    if (!LpSurfaceShaderList)
        LpSurfaceShaderList = this;
}


// ---- src/SurfaceShaderList/dtor.cpp


SurfaceShaderList::~SurfaceShaderList()
{
    if (m_allocator)
        m_allocator->Release();
    if (LpSurfaceShaderList == this)
        LpSurfaceShaderList = 0;
    for (int i = 0; i < m_numShaders; i++) {
        if (m_shaders[i])
            delete m_shaders[i];
    }
}


// ---- src/SurfaceShaderList/Purge.cpp



void SurfaceShaderList::Purge()
{
    if (m_allocator)
        m_allocator->Purge();
    for (int i = 0; i < m_numShaders; i++) {
        if (m_shaders[i])
            delete m_shaders[i];
        m_shaders[i] = 0;
    }
    m_numShaders = 0;
}


// ---- src/SurfaceShaderList/AddShaderInstance.cpp





int SurfaceShaderList::AddShaderInstance(ShaderDescriptor *descriptor, int index)
{
    if (index == -1) {
#line 134 "SurfaceShader.cpp"
        ASSERT(m_numShaders != 0x1000, "AddShaderInstance: too many shader instances");
        index = m_numShaders++;
    }
    if (!m_shaders[index])
        m_shaders[index] = new PSXSurfaceShaderInstance;
    m_shaders[index]->m_descriptor = descriptor;
    for (int i = 0; i < m_numShaders - 1; i++) {
        if (m_shaders[i]) {
            if (descriptor->IsIdentical(m_shaders[i]->m_descriptor))
                m_shaders[i]->m_next = m_shaders[index];
        }
    }
    return index;
}


// ---- src/SurfaceShaderList/Render.cpp



void SurfaceShaderList::Render()
{
    static int RenderShaders = 1;   // debug switch
    if (RenderShaders) {
        for (int i = 0; i < m_numShaders; i++) {
            if (m_shaders[i] && m_shaders[i]->m_frame == m_frame)
                ((PSXSurfaceShaderInstance *)m_shaders[i])->Render();
        }
    }
    m_allocator->Purge();
    m_frame++;
}

