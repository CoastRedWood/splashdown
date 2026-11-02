// TextOverlay.cpp  @ 0x002CB630 (text 0x2CB630-0x2CC710)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp)
//
// Retail's TextOverlay.cpp as one translation unit: a TU without a static
// initializer inside PSXWater.cpp's tu_map row, after PSXWater.cpp (out of
// alphabetical order) and before MediaControl.cpp. Its .rodata restarts the
// labels at @595 after PSXWater's RTTI ("TextOverlay.cpp" ASSERT, then the
// TextOverlay/Overlay RTTI). `make link` places its data as well as its code.



#include "ScreenTextManager/ScreenTextManager.h"
#include "ScreenText/ScreenText.h"
#include "TextOverlay/TextOverlay.h"
#include "GameObject/GameObject.h"
#include "TextureMap/TextureMap.h"
#include "soultree_assert_block.h"
#include "_RECT/_RECT.h"
#include "Camera/Camera.h"
#include "RenderTarget/RenderTarget.h"
#include "PSXVideoCard/PSXVideoCard.h"
#include "ScreenTextFontItemList/ScreenTextFontItem.h"
#include "ScreenTextFontItemList/ScreenTextFontItemList.h"
#include "ScreenTextFontManager/ScreenTextFontManager.h"
#include "JetskiGame/JetskiGame.h"

#pragma fast_fptosi on

extern "C" char *strncpy(char *dst, const char *src, unsigned int n);
extern "C" void *memcpy(void *dst, const void *src, unsigned int n);

extern PSXVideoCard *LpVideoCard;
extern JetskiGame *LpGame;

// ---- src/TextOverlay/ctor.cpp


TextOverlay::TextOverlay(int enabled, int maxLength) : Overlay(enabled, 1)
{
    m_texture = 0;
    if (maxLength)
        m_screenText = new ScreenText(maxLength, 0);
    else
        m_screenText = new ScreenText(0x28, 0);
    m_screenText->SetText("");
    m_justification = ScreenTextManager::CenterBothJustification;
    m_x250 = 0;
    m_screenText->SetColor(0xFFFFFFFF);
    m_x25C = 0.0f;
    m_x258 = 0.0f;
    m_x260 = 1;
    m_x264 = -1.0f;
    m_x268 = 0;
    m_camera = 0;
    m_screenText->SetAlpha(0xFF);
    m_endAlpha = m_startAlpha = m_targetAlpha = m_screenText->m_alphaFix;
    m_screenText->SetX(1.0f);
    m_endX = m_startX = m_x = m_screenText->m_scale;
    m_screenText->SetY(0.0f);
    m_endY = m_startY = m_y = m_screenText->m_x48;
    m_moveElapsed = 0;
    m_fadeElapsed = 0.0f;
    m_xElapsed = 0.0f;
    m_yElapsed = 0.0f;
    m_moveTime = 0;
    m_fadeTime = 0.0f;
    m_xTime = 0.0f;
    m_yTime = 0.0f;
    m_scaleX = 1.0f;
    m_scaleY = 1.0f;
    m_busy = 0;
    m_x2AC = -1.0f;
    m_textManager = 0;
}


// ---- src/TextOverlay/dtor.cpp



TextOverlay::~TextOverlay()
{
    if (m_texture)
        m_texture->Release();
    if (m_screenText)
        delete m_screenText;
}


// ---- src/TextOverlay/Create.cpp

TextOverlay *TextOverlay::Create(ScreenTextManager *manager, char **fontNames, Camera *camera, int numFonts)
{
    _RECT viewport;
#line 73 "TextOverlay.cpp"
    ASSERT(manager != 0, "lpScreenTextManager == NULL");
    m_camera = camera;
    if (m_camera)
        m_camera->GetViewportBoundsLogical(&viewport);
    else
        LpRenderTarget->m_camera->GetViewportBoundsLogical(&viewport);
    m_scaleX = (float)LpVideoCard->m_width / (float)(viewport.right - viewport.left);
    m_scaleY = (float)LpVideoCard->m_height / (float)(viewport.bottom - viewport.top);
    m_rect.left = (int)(502.0f / m_scaleX);
    m_rect.right = m_rect.left + 128;
    m_rect.top = (int)(315.0f / m_scaleY);
    m_rect.bottom = m_rect.top + 16;
    ((ScreenTextFontManager *)LpGame->m_fonts)->CreateFonts(numFonts, fontNames, 0);
    ScreenTextFontItem *item = LpGame->m_fonts->GetScreenTextFont(fontNames[0]);
    m_font = item ? item->m_font : 0;
    m_textManager = manager;
    Overlay::Create(0, &m_rect, 1, 0, 0.00001f);
    return this;
}


// ---- src/TextOverlay/Tick.cpp
// Runs the display timer and blink, then steps the position, alpha and x/y
// fades towards their targets.
int TextOverlay::Tick(float dt)
{
    if (m_x250) {
        if (m_x264 >= 0.0f) {
            m_x264 -= dt;
            m_x258 -= dt;
            if (m_x258 <= 0.0f && m_x25C > 0.0f) {
                m_x260 = !m_x260;
                m_x258 = m_x25C;
            }
        } else if (!m_x268) {
            RemoveText();
        } else {
            m_x258 -= dt;
            if (m_x258 <= 0.0f && m_x25C > 0.0f) {
                m_x260 = !m_x260;
                m_x258 = m_x25C;
            }
        }
    } else {
        m_busy = 0;
    }
    if (m_targetRect.left != m_rect.left || m_targetRect.top != m_rect.top) {
        _RECT rect;
        if (m_camera)
            m_camera->GetViewportBoundsLogical(&rect);
        else
            LpRenderTarget->m_camera->GetViewportBoundsLogical(&rect);
        float sx = (float)LpVideoCard->m_width / (rect.right - rect.left);
        float sy = (float)LpVideoCard->m_height / (rect.bottom - rect.top);
        if (sx != m_scaleX) {
            m_scaleX = sx;
            m_targetRect.left = (int)(m_targetRect.left / m_scaleX);
        }
        if (sy != m_scaleY) {
            m_scaleY = sy;
            m_targetRect.top = (int)(m_targetRect.top / m_scaleY);
        }
        if (m_moveTime == 0.0f || !(m_moveElapsed < m_moveTime)) {
            memcpy(&m_rect, &m_targetRect, sizeof(_RECT));
        } else {
            float dy = m_targetRect.top - m_startRect.top;
            float t = m_moveElapsed / m_moveTime;
            float dx = m_targetRect.left - m_startRect.left;
            m_rect.left = m_startRect.left + (int)(dx * t);
            m_rect.top = m_startRect.top + (int)(dy * t);
            m_moveElapsed += dt;
        }
    }
    ScreenText *item = m_screenText;
    if (m_targetAlpha != item->m_alphaFix) {
        if (!(m_fadeElapsed < m_fadeTime)) {
            item->m_alphaFix = m_targetAlpha;
            item->m_x5D_bit4 = 1;
        } else {
            item->m_alphaFix = m_startAlpha + (int)((m_targetAlpha - m_startAlpha) * (m_fadeElapsed / m_fadeTime));
            item->m_x5D_bit4 = 1;
            m_fadeElapsed += dt;
        }
    }
    item = m_screenText;
    if (m_x != item->m_scale) {
        if (!(m_xElapsed < m_xTime)) {
            item->m_scale = m_x;
            item->m_dirty = 1;
            item->m_positionDirty = 1;
        } else {
            item->m_scale = m_startX + (m_x - m_startX) * (m_xElapsed / m_xTime);
            item->m_dirty = 1;
            item->m_positionDirty = 1;
            m_xElapsed += dt;
        }
    }
    item = m_screenText;
    if (m_y != item->m_x48) {
        if (!(m_yElapsed < m_yTime)) {
            item->m_x48 = m_y;
            item->m_dirty = 1;
        } else {
            item->m_x48 = m_startY + (m_y - m_startY) * (m_yElapsed / m_yTime);
            item->m_dirty = 1;
            m_yElapsed += dt;
        }
    }
    if (m_x2AC >= 0.0f) {
        m_x2AC -= dt;
    } else if (m_x2AC != -1.0f) {
        m_x2AC = -1.0f;
        m_targetAlpha = m_endAlpha;
        if (m_targetAlpha > 255)
            m_targetAlpha = 255;
        else if (m_targetAlpha < 0)
            m_targetAlpha = 0;
        m_startAlpha = m_screenText->m_alphaFix;
        m_fadeElapsed = 0.0f;
        float x = m_endX;
        m_x = x;
        if (x < 0.0f)
            m_x = 0.0f;
        m_startX = m_screenText->m_scale;
        m_xElapsed = 0.0f;
        m_y = m_endY;
        m_startY = m_screenText->m_x48;
        m_yElapsed = 0.0f;
    }
    return 1;
}


// ---- src/TextOverlay/Render3D.cpp
static inline void Draw(TextOverlay *o)
{
    if (o->m_x250 && o->m_x260) {
        o->m_screenText->SetPosition(o->m_rect.left, o->m_rect.top);
        o->m_textManager->PositionText(o->m_screenText, o->m_justification, o->m_font);
        o->m_textManager->AddText(o->m_screenText, o->m_font);
        LpRenderTarget->SetRenderState((_D3DRENDERSTATETYPE)0xE, 1, 0);
        LpRenderTarget->SetRenderState((_D3DRENDERSTATETYPE)7, 1, 0);
    }
}

int TextOverlay::Render3D()
{
    static int meterID = -1;  // profiler ID, unused
    if (m_camera) {
        if (m_camera == LpRenderTarget->m_camera)
            Draw(this);
    } else {
        Draw(this);
    }
    GameObject::Render3D();
    return 1;
}


// ---- src/TextOverlay/SetText.cpp



void TextOverlay::SetText(char *text)
{
    if (!m_busy) {
        ScreenText *screenText = m_screenText;
        screenText->m_positionDirty = 1;
        strncpy(screenText->m_text, text, screenText->m_maxChars);
        screenText->m_text[screenText->m_maxChars] = 0;
        screenText->m_x5D_bit4 = 1;
        screenText->m_dirty = 1;
        screenText->m_cellsDirty = 1;
    }
}


// ---- src/TextOverlay/SetColor.cpp

void TextOverlay::SetColor(unsigned int color)
{
    if (m_busy == 0) {
        ScreenText *item = m_screenText;
        item->m_color = color;
        item->m_x5D_bit4 = 1;
    }
}


// ---- src/TextOverlay/DisplayText.cpp


void TextOverlay::DisplayText(float a, float b, int alpha, float x, float y, int busy, float c,
                              int alpha2, float x2, float y2, float d, float e, float f)
{
    if (m_busy)
        return;
    m_busy = busy;
    m_x250 = 1;
    m_x258 = a;
    m_x25C = a;
    m_x260 = 1;
    if (!(b < 0.0f)) {
        m_x264 = b;
        m_x268 = 0;
    } else {
        m_x268 = 1;
    }
    m_targetAlpha = alpha;
    // m_screenText is re-read for each property, as if through inline setters
    ScreenText *item = m_screenText;
    item->m_alphaFix = alpha;
    item->m_x5D_bit4 = 1;
    m_x = x;
    item = m_screenText;
    item->m_scale = x;
    item->m_dirty = 1;
    item->m_positionDirty = 1;
    m_y = y;
    item = m_screenText;
    item->m_x48 = y;
    item->m_dirty = 1;
    m_x2AC = c;
    if (!(c <= -1.0f)) {
        m_endAlpha = alpha2;
        m_fadeTime = d;
        m_endX = x2;
        m_xTime = e;
        m_endY = y2;
        m_yTime = f;
        m_busy = 1;
    } else {
        m_endAlpha = alpha;
        m_endX = x;
        m_endY = y;
    }
}


// ---- src/TextOverlay/RemoveText.cpp


void TextOverlay::RemoveText()
{
    m_x250 = 0;
    m_x268 = 0;
    m_x264 = -1.0f;
    m_busy = 0;
}


// ---- src/TextOverlay/SetAlignment.cpp

void TextOverlay::SetAlignment(int justification)
{
    if (m_busy == 0)
        m_justification = justification;
}


// ---- src/TextOverlay/SetPosition.cpp

void TextOverlay::SetPosition(int x, int y, float moveTime)
{
    if (m_busy == 0) {
        m_moveTime = moveTime;
        m_moveElapsed = 0;
        m_targetRect.left = x;
        m_targetRect.top = y;
        m_startRect.left = m_rect.left;
        m_startRect.top = m_rect.top;
    }
}


// ---- src/TextOverlay/SetAlpha.cpp

void TextOverlay::SetAlpha(int alpha, float fadeTime)
{
    if (m_busy == 0) {
        m_fadeTime = fadeTime;
        m_fadeElapsed = 0;
        m_targetAlpha = alpha;
        if (m_targetAlpha > 255)
            m_targetAlpha = 255;
        else if (m_targetAlpha < 0)
            m_targetAlpha = 0;
        m_startAlpha = m_screenText->m_alphaFix;
    }
}


// ---- src/TextOverlay/SetAlphaEnabled.cpp


void TextOverlay::SetAlphaEnabled(int enabled)
{
    m_screenText->m_alphaTest = enabled;
}


// ---- src/TextOverlay/SetDepthEnabled.cpp

void TextOverlay::SetDepthEnabled(int enabled)
{
    m_screenText->m_zBuffer = enabled;
}


// ---- src/TextOverlay/SetDepth.cpp

void TextOverlay::SetDepth(float depth)
{
    ScreenText *item = m_screenText;
    item->m_x4C = depth;
    item->m_dirty = 1;
}

