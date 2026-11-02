// JetskiProgressBar.cpp  @ 0x00397970 (text 0x397970-0x397C10)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// JetskiProgressBar as one translation unit without a static initializer, in
// TextureMapStringTable.cpp's tu_map row: its .rodata restarts at @554 after
// the VideoFXManager RTTI and is its own RTTI block; zlib follows.



#include "JetskiProgressBar/JetskiProgressBar.h"
#include "TextureMap/TextureMap.h"
#include "RenderTarget/RenderTarget.h"
#include "d3dtypes.h"

class Palette8;

TextureMap *GetTextureMapFromResourceName(char *name, int flags, Palette8 *palette, unsigned int a,
                                          _D3DBLEND src, _D3DBLEND dst, int alpha, unsigned int colorKey, int b);

JetskiProgressBar::JetskiProgressBar(int enabled)
    : GameObject(enabled)
{
    m_texture = 0;
    m_progress = 0;
}

JetskiProgressBar::~JetskiProgressBar()
{
    LpRenderTarget->TrySendRenderQueue(1);
    LpRenderTarget->WaitForRenderingToComplete();
    if (m_texture)
        m_texture->Release();
}

JetskiProgressBar *JetskiProgressBar::Create(char *textureName)
{
    m_texture = GetTextureMapFromResourceName(textureName, 0x22B8, 0, 0, (_D3DBLEND)5, (_D3DBLEND)6, 0x80, 0xFF00FF, 1);
    m_srcRect.left = 4;
    m_srcRect.right = 11;
    m_srcRect.top = 4;
    m_srcRect.bottom = 5;
    m_dstRect.left = 603;
    m_dstRect.right = 609;
    m_dstRect.top = 411;
    m_dstRect.bottom = 412;
    GameObject::Create();
    return this;
}

#pragma fast_fptosi on
// Draw the bar filled to m_progress (it grows upwards from y = 412).
int JetskiProgressBar::AddProgress(float amount)
{
    LpRenderTarget->TrySendRenderQueue(1);
    LpRenderTarget->WaitForRenderingToComplete();
    float progress = m_progress += amount;
    if (progress < 1.0f) {
        m_dstRect.top = (int)(412.0f - 392.0f * progress);
        LpRenderTarget->Blt(&m_dstRect, m_texture, &m_srcRect, 0);
        return 0;
    }
    m_progress = 1.0f;
    m_dstRect.top = 20;
    LpRenderTarget->Blt(&m_dstRect, m_texture, &m_srcRect, 0);
    return 1;
}
#pragma fast_fptosi reset
