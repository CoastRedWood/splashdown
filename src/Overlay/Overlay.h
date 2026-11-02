// Overlay: a textured 2D quad drawn over the scene (TextOverlay derives from
// it), a GameObject. 0x210 bytes (TextOverlay's data starts there). Members
// from the ctor (it constructs the 5 vertices and the 4 + 1 DmaRenderItems,
// clears the rects, alpha 0xFF, depth 1) and Create/SetUpRect.
#ifndef OVERLAY_OVERLAY_H
#define OVERLAY_OVERLAY_H

#include "GameObject/GameObject.h"
#include "DmaRenderItem/DmaRenderItem.h"
#include "_RECT/_RECT.h"
#include "_D3DTLVERTEX/_D3DTLVERTEX.h"

class TextureMap;

class Overlay : public GameObject {
public:
    Overlay(int enabled, int layer);
    Overlay *Create(TextureMap *texture, _RECT *srcRect, int mode, _RECT *dstRect, float depth);
    void SetUpRect();
    void SetVertexColor(int r, int g, int b);
    void Build2DQuad();

    // --- overrides in retail slot order ---
    virtual ~Overlay();
    virtual int Tick(float dt);
    virtual int PrepareGeometry();
    virtual int Render3D();

    _D3DTLVERTEX m_vertices[5];         // +0x30, [4] closes the strip
    TextureMap *m_texture;              // +0xD0
    int m_screenType;                   // +0xD4
    int m_mode;                         // +0xD8, 1: scale to the screen
    _RECT m_srcRect;                    // +0xDC
    _RECT m_dstRect;                    // +0xEC
    int m_hasDstRect;                   // +0xFC
    int m_alpha;                        // +0x100, 0xFF
    int m_useAlpha;                     // +0x104
    float m_depth;                      // +0x108
    int m_x10C;                         // +0x10C
    int m_layer;                        // +0x110
    int m_buffer;                       // +0x114, the m_renderItems slot in use
    char pad118[0x120 - 0x118];
    DmaRenderItem m_renderItems[4];     // +0x120
    DmaRenderItem m_renderItem;         // +0x1E0, the render state
};

#endif
