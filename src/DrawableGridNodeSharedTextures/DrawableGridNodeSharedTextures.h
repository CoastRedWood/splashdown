// DrawableGridNodeSharedTextures: a terrain grid node whose texture maps
// come from the Terrain's shared texture table and texture cache (0xF0 bytes
// from its New, all DrawableGridNode's). Its virtuals are DrawableGridNode's texture hooks; its data
// is DrawableGridNode's m_textures (GridNodeTextures, DrawableGridNode.h).
#ifndef DRAWABLEGRIDNODESHAREDTEXTURES_H
#define DRAWABLEGRIDNODESHAREDTEXTURES_H

#include "DrawableGridNode/DrawableGridNode.h"

class DrawableGridNodeSharedTextures : public DrawableGridNode {
public:
    DrawableGridNodeSharedTextures(long id, int level, DrawableGridNode *parent, Terrain *terrain,
                                   int a, int b, int c, int d, int e, _D3DVECTOR *min, _D3DVECTOR *max);

    // --- virtuals in retail slot order ---
    virtual ~DrawableGridNodeSharedTextures();
    virtual DrawableGridNode *New(long id, int level, DrawableGridNode *node, int a, int b, int c, int d, int e,
                                  _D3DVECTOR *min, _D3DVECTOR *max);
    virtual void SelectMipMapLevel(float distance);
    virtual void PlaceMipMapInTextureCache(int detail);
    virtual void LoadTexture(long file);
    virtual void SetTextureMap();
    virtual void SetDetailTextureMap(int i);
    virtual TextureMap *GetTextureMap();
    virtual int GetAuthoredDetailTextureMapSize(int i);
};

#endif
