// PSXSurfaceGeometry.cpp  @ 0x00349DD0 (text 0x349DD0-0x34A5D0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's PSXSurfaceGeometry.cpp as one translation unit, a TU without an
// initializer inside PSXSurfaceMaterial.cpp's tu_map row:
// PSXSurfaceGeometry's methods. Its only .rodata is the PSXSurfaceGeometry
// RTTI (0x436760-0x4367B0, labels restarting at @518).


#include "d3dtypes.h"
#include "ParameterBlock/ParameterBlock.h"
#include "PSXShaderDescriptor/PSXShaderDescriptor.h"
#include "MaterialAnimationData/MaterialAnimationData.h"
#include "TextureMapStringTable/TextureMapStringTable.h"
#include "soultree_assert_block.h"
#include "QWORD/QWORD.h"
#include "StringTable/StringTable.h"
#include "sceGifTag/sceGifTag.h"
#include "PSXVideoCard/PSXVideoCard.h"
#include "SurfaceMaterial/SurfaceMaterial.h"
#include "PSXSurfaceMaterial/PSXSurfaceMaterial.h"
#include "TextureMap/TextureMap.h"
#include "ShaderDescriptor/ShaderDescriptor.h"
#include "PSXSurfaceShaderInstance/PSXSurfaceShaderInstance.h"
#include "SurfaceShaderInstance/SurfaceShaderInstance.h"
#include "PSXTextureMapManager/PSXTextureMapManager.h"
#include "PSXSurfaceGeometry/PSXSurfaceGeometry.h"
#include "PSXSurfaceShaderList/PSXSurfaceShaderList.h"
#include "SurfaceShaderList/SurfaceShaderList.h"
#include "DmaRenderItem/DmaRenderItem.h"
#include "_sceDmaTag/_sceDmaTag.h"
#include "DataAccess/DataAccess.h"
#include "SurfaceGeometry/SurfaceGeometry.h"
#include "MaterialStringTable/MaterialStringTable.h"
#include "PSXTextureMap/PSXTextureMap.h"
#include "Game/Game.h"
#include "Water/Water.h"
#include "PSXWater/PSXWater.h"
#include "PSXRenderTarget/PSXRenderTarget.h"
#include "PSXCamera/PSXCamera.h"
#include "LightManager/LightManager.h"
#include "VifDataItem/VifDataItem.h"


extern TextureMapStringTable *const LpTextureMapStringTable;

// ---- src/PSXSurfaceGeometry/ctor.cpp

PSXSurfaceGeometry::PSXSurfaceGeometry()
{
    m_flags = 0;
    m_numLods = 0;
    m_xA = 0xFF;
    m_xB = 0;
    m_xC = 1.0f;
    m_x10 = 1.0f;
    m_lods[0] = 1.0f;
    m_lods[1] = 1.0f;
    m_packet = 0;
    m_xBbits.bit0 = 0;
    m_instanceTags = 0;
}


// ---- src/PSXSurfaceGeometry/dtor.cpp
extern "C" void free(void *p);


PSXSurfaceGeometry::~PSXSurfaceGeometry()
{
    if (m_xBbits.bit0 && m_packet)
        free(m_packet);
    if (m_instanceTags)
        delete[] m_instanceTags;
}


// ---- src/PSXSurfaceGeometry/ReadFromFile.cpp
extern "C" void *malloc(unsigned int size);
extern "C" void free(void *p);
extern "C" void *memalign(unsigned int align, unsigned int size);

extern DataAccess *LpDataAccess;


enum {
    DMA_REF = 0x30,
    DMA_CALL = 0x50,
    DMA_RET = 0x60
};

static inline void SetTag(_sceDmaTag *tag, int qwc, int id, _sceDmaTag *next)
{
    tag->qwc = qwc;
    tag->mark = 0;
    tag->id = id;
    tag->next = next;
    tag->p[0] = 0;
    tag->p[1] = 0;
}

class InstanceMaster;



int PSXSurfaceGeometry::ReadFromFile(long file, InstanceMaster *master, int instance)
{
    unsigned int size;
    int startsSize;
    unsigned char pad;

    SurfaceGeometry::ReadFromFile(file, master, instance);
    LpDataAccess->FRead(file, &m_flags, 1, 1);
    LpDataAccess->FRead(file, &m_numLods, 1, 1);
    LpDataAccess->FRead(file, &m_xA, 1, 1);
    LpDataAccess->FRead(file, &pad, 1, 1);
    if ((m_flags & 1) ? 1 : 0)
        LpDataAccess->FRead(file, &m_xC, 4, 1);
    if ((m_flags & 2) ? 1 : 0)
        LpDataAccess->FRead(file, &m_x10, 4, 1);
    for (int i = 0; i < m_numLods; i++)
        LpDataAccess->FRead(file, &m_lods[i], 4, 1);
    LpDataAccess->FRead(file, &size, 4, 1);
    int instanced = 0;
    int *starts;
    if (master) {
        instanced = 1;
        LpDataAccess->FRead(file, &startsSize, 4, 1);
        starts = (int *)malloc(startsSize);
        LpDataAccess->FRead(file, starts, startsSize, 1);
        LpDataAccess->FRead(file, &size, 4, 1);
    }
    if (size) {
        unsigned int qwc = (size + 15) >> 4;
        size = qwc * 16;
        m_packet = (_sceDmaTag *)memalign(64, size + 16);
        m_xBbits.bit0 = 1;
        LpDataAccess->FRead(file, m_packet + 1, size, 1);
        if (instanced) {
            SetupInstanceTags(starts, qwc, master, instance);
            free(starts);
            SetTag(m_packet, 0, DMA_CALL, m_instanceTags);
        } else {
            SetTag(m_packet, qwc, DMA_REF, m_packet + 1);
        }
    }
    return 1;
}


// ---- src/PSXSurfaceGeometry/Create.cpp

int PSXSurfaceGeometry::Create(char *name, unsigned char flags)
{
    SurfaceGeometry::Create(name, flags);
    m_flags = flags;
    return 1;
}


// ---- src/PSXSurfaceGeometry/SetupInstanceTags.cpp

struct InstanceGeometry {
    char pad0[0x30];
    _sceDmaTag *m_packets;          // +0x30
    int m_numStrips;                // +0x34
    int *m_stripPackets;            // +0x38, per strip, index into m_packets
    char pad3C[0x40 - 0x3C];
};

struct InstanceData {
    char pad0[0x8];
    InstanceGeometry *m_geometry;   // +0x08
};

class InstanceMaster {
public:
    char pad0[0x30];
    InstanceData *m_data;           // +0x30
};


// Build a DMA chain that, per strip, calls the instance's packet and then
// refs this geometry's strip data, ending with a ret.
void PSXSurfaceGeometry::SetupInstanceTags(int *stripStarts, int end, InstanceMaster *master, int instance)
{
    InstanceGeometry *geometry = &master->m_data->m_geometry[instance];
    int numStrips = geometry->m_numStrips;
    int tag = 0;
    int *stripPackets = geometry->m_stripPackets;

    m_instanceTags = new _sceDmaTag[numStrips * 2 + 1];
    SetTag(&m_instanceTags[numStrips * 2], 0, DMA_RET, 0);
    int i;
    for (i = 0; i < numStrips; i++, tag += 2, stripPackets++, stripStarts++) {
        SetTag(&m_instanceTags[i * 2], 0, DMA_CALL, &geometry->m_packets[*stripPackets]);
        _sceDmaTag *data = &m_packet[*stripStarts] + 1;
        int qwc;
        if (i == numStrips - 1)
            qwc = end - *stripStarts;
        else
            qwc = stripStarts[1] - *stripStarts;
        SetTag(&m_instanceTags[tag + 1], qwc, DMA_REF, data);
    }
}


// ---- src/PSXSurfaceGeometry/AddToRenderQueue.cpp
extern MaterialStringTable *LpMaterialStringTable;
struct _sceDmaTag;





void PSXSurfaceGeometry::AddToRenderQueue(_sceDmaTag *start, _sceDmaTag *end, int flags)
{
    ((PSXSurfaceMaterial *)LpMaterialStringTable->GetPointer(m_materialIndex))->AddGeometry(this, start, end, flags);
}


// ---- src/PSXSurfaceGeometry/UpdateLOD.cpp




void PSXSurfaceGeometry::UpdateLOD(float a, float b)
{
    ((PSXSurfaceMaterial *)LpMaterialStringTable->GetPointer(m_materialIndex))->UpdateLOD(m_lods, (float)m_numLods, a, b);
}


// ---- src/PSXSurfaceGeometry/ChangeMaterial.cpp



extern PSXTextureMapManager *LpTextureMapManager;




int PSXSurfaceGeometry::ChangeMaterial(int oldMaterial, int newMaterial)
{
    if (m_materialIndex == oldMaterial) {
        m_materialIndex = newMaterial;
        SurfaceMaterial *material = (SurfaceMaterial *)LpMaterialStringTable->GetPointer(oldMaterial);
        if (material) {
            for (int i = 0; i < material->m_numShaders; i++) {
                ShaderDescriptor *shader = material->m_shaders[i];
                if (shader->m_textures[0] != -1) {
                    PSXTextureMap *texture = (PSXTextureMap *)LpTextureMapStringTable->GetPointer(shader->m_textures[0]);
                    if (texture->m_compressed)
                        LpTextureMapManager->RemoveTextureFromIpuCache(texture);
                }
                if (shader->m_textures[1] != -1) {
                    // Retail looks up the first texture again here.
                    PSXTextureMap *texture = (PSXTextureMap *)LpTextureMapStringTable->GetPointer(shader->m_textures[0]);
                    if (texture->m_compressed)
                        LpTextureMapManager->RemoveTextureFromIpuCache(texture);
                }
            }
        }
        LpMaterialStringTable->Request(newMaterial);
        LpMaterialStringTable->RevokeRequest(oldMaterial);
    }
    return m_materialIndex;
}

