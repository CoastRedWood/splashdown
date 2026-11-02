// DrawableGridNodeSharedTextures.cpp  @ 0x00235D60 (text 0x235D60-0x2364A0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// A retail translation unit without a static initializer (the name is a guess):
// DrawableGridNodeSharedTextures. Split from Fog.cpp: mwcc writes RTTI only at
// the end of a TU, and retail's .rodata has Fog's RTTI (@709-@723), then this
// file's GridNode/DrawableGridNode/DrawableGridNodeSharedTextures RTTI
// (@859-@863), then GridNodeGeometry.cpp's (labels restart at @696). Where
// RotateAroundAxis and Parametric2DLineIntersection end (Fog.cpp's tail or this
// file's head) nothing decides.
#include "GameObject/GameObject.h"
#include "DmaRenderItem/DmaRenderItem.h"
#include "Fog/Fog.h"
#include "RenderTarget/RenderTarget.h"
#include "PSXRenderTarget/PSXRenderTarget.h"
#include "Camera/Camera.h"
#include "sceGifTag/sceGifTag.h"
#include "d3dtypes.h"
#include "TextureMap/TextureMap.h"
#include "PSXTextureMap/PSXTextureMap.h"
#include "Terrain/Terrain.h"
#include "AgeManager/AgeManager.h"
#include "DrawableGridNodeSharedTextures/DrawableGridNodeSharedTextures.h"
#include "DrawableGridNode/DrawableGridNode.h"
#include "GridNodeGeometryBase/GridNodeGeometryBase.h"
#include "GridNodeGeometryLOD/GridNodeGeometryLOD.h"
#include "DataAccess/DataAccess.h"
#include "ControlInterface/ControlTypes.h"
#include "GridNodeGeometryPlain/GridNodeGeometryPlain.h"

extern "C" void *malloc(unsigned int size);
extern "C" void free(void *p);

#define D3DTSS_COLOROP ((_D3DTEXTURESTAGESTATETYPE)1)
#define D3DTSS_ALPHAOP ((_D3DTEXTURESTAGESTATETYPE)4)
#define D3DTOP_DISABLE 1


// ---- src/DrawableGridNodeSharedTextures/ctor.cpp
struct _D3DVECTOR;
class Terrain;



DrawableGridNodeSharedTextures::DrawableGridNodeSharedTextures(long id, int level, DrawableGridNode *parent,
                                                               Terrain *terrain, int a, int b, int c, int d,
                                                               int e, _D3DVECTOR *min, _D3DVECTOR *max)
    : DrawableGridNode(id, level, parent, terrain, a, b, c, d, e, min, max)
{
    LoadNode(id, level, parent, a, b, c, d, e, min, max);
}


// ---- src/DrawableGridNodeSharedTextures/New.cpp
struct _D3DVECTOR;
class Terrain;



DrawableGridNode *DrawableGridNodeSharedTextures::New(long id, int level, DrawableGridNode *node,
                                                                    int a, int b, int c, int d, int e,
                                                                    _D3DVECTOR *min, _D3DVECTOR *max)
{
    return new DrawableGridNodeSharedTextures(id, level, this, node->m_terrain, a, b, c, d, e, min, max);
}


// ---- (LoadTexture)
void DrawableGridNodeSharedTextures::LoadTexture(long file)
{
}


// ---- src/DrawableGridNodeSharedTextures/SelectMipMapLevel.cpp
// Note `quarter` is only set inside the if, as in retail: the final divide
// reads whatever $f2 holds when there is no detail texture (retail bug).
class TextureMap;


// One authored detail texture slot (44 bytes).


class DrawableGridNodeSharedTextures;



void DrawableGridNodeSharedTextures::SelectMipMapLevel(float distance)
{
    float level;
    float quarter;
    if (distance != 0.0f)
        level = m_terrain->m_gridScale * (float)(m_terrain->m_xCC * (16 << m_shift)) / distance;
    else
        level = 10000.0f;
    if (m_parent && m_parent->m_textures->m_detail) {
        DetailTextureEntry *detail = m_parent->m_textures->m_detail;
        int i = ((m_position >> 4) & 0xC) | ((m_position >> 2) & 3);
        m_textures->m_texture = m_terrain->m_textures[detail[i].m_index];
        quarter = 0.25f;
        int high = m_position >> 4;   // retail shifts before masking the low bits
        m_uOffset = quarter * (m_position & 3);
        m_vOffset = quarter * (high & 3);
        m_uvScale = 0.015625f;
    }
    m_textures->m_mipLevel = level / quarter;
}



// ---- src/DrawableGridNodeSharedTextures/PlaceMipMapInTextureCache.cpp




// One authored detail texture slot (44 bytes).




// detail == -1 places the node's own texture, otherwise detail texture `detail`.
void DrawableGridNodeSharedTextures::PlaceMipMapInTextureCache(int detail)
{
    TextureMap *texture;
    if (detail == -1)
        texture = m_textures->m_texture;
    else
        texture = m_textures->m_detail[detail].m_texture;
    if (texture) {
        float size;
        if (detail != -1) {
            float distance = CalculateDistanceTo4x4Child(detail);
            if (distance != 0.0f)
                m_textures->m_detail[detail].m_projectedSize =
                    (float)(m_terrain->m_xCC * (16 << m_shift)) / (4.0f * distance);
            else
                m_textures->m_detail[detail].m_projectedSize = 10000.0f;
            size = m_textures->m_detail[detail].m_projectedSize;
        } else {
            size = m_textures->m_mipLevel;
        }
        texture->AddUsageViaProjectedSize(size);
    }
}


// ---- src/DrawableGridNodeSharedTextures/SetTextureMap.cpp

void DrawableGridNodeSharedTextures::SetTextureMap()
{
    GridNodeTextures *textures = m_textures;
    if (textures->m_texture) {
        if (textures->m_ageItem.m_size)
            m_terrain->m_ageManager->TouchItem(&textures->m_ageItem);
    } else {
        LpRenderTarget->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DISABLE);
        LpRenderTarget->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
    }
}


// ---- (GetTextureMap)
TextureMap *DrawableGridNodeSharedTextures::GetTextureMap()
{
    return m_textures->m_texture;
}


// ---- src/DrawableGridNodeSharedTextures/SetDetailTextureMap.cpp
void DrawableGridNodeSharedTextures::SetDetailTextureMap(int i)
{
    GridNodeTextures *textures = m_textures;
    int index = textures->m_detail[i].m_index;
    if (index == -1) {
        textures->m_texture = 0;
    } else if (textures->m_detail[i].m_ownTexture) {
        textures->m_texture = textures->m_detail[i].m_texture;
        if (m_textures->m_detail[i].m_ageItem.m_size)
            m_terrain->m_ageManager->TouchItem(&m_textures->m_detail[i].m_ageItem);
    } else {
        textures->m_texture = m_terrain->m_textures[index];
    }
    if (m_textures->m_texture) {
        m_textures->m_texture->SetRenderStates();
    } else {
        LpRenderTarget->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DISABLE);
        LpRenderTarget->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
    }
}


// ---- src/DrawableGridNodeSharedTextures/GetAuthoredDetailTextureMapSize.cpp


// One authored detail texture slot (44 bytes).



int DrawableGridNodeSharedTextures::GetAuthoredDetailTextureMapSize(int i)
{
    TextureMap *texture;
    int index = m_textures->m_detail[i].m_index;
    if (index == -1)
        texture = 0;
    else if (m_textures->m_detail[i].m_ownTexture)
        texture = m_textures->m_detail[i].m_texture;
    else
        texture = m_terrain->m_textures[index];
    if (texture)
        return texture->m_width << texture->m_mipShift;
    return 0;
}


// ---- src/DrawableGridNodeSharedTextures/dtor.cpp


DrawableGridNodeSharedTextures::~DrawableGridNodeSharedTextures()
{
}

