// DebugOverlay.cpp  @ 0x0022CBF0 (text 0x22CBF0-0x22D0F0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp)
//
// DebugOverlay's four functions as one translation unit, a TU without a
// static initializer in Cube.cpp's tu_map row, between Debug.cpp and
// DmaRenderItem.cpp. It has no strings or globals of its own (only
// RenderDelayed's static), so it could also be Debug.cpp's tail.


#include "DebugOverlay/DebugOverlay.h"
#include "_RECT/_RECT.h"
#include "ScreenTextManager/ScreenTextManager.h"
#include "Camera/Camera.h"
#include "RenderTarget/RenderTarget.h"
#include "ScreenText/ScreenText.h"
#include "Game/Game.h"
#include "PSXVideoCard/PSXVideoCard.h"
#include "soultree_assert_block.h"
#include "ScreenTextFont/ScreenTextFont.h"

// ScreenTextFontManager.h's inline glyph lookup (as in ScreenTextManager.cpp),
// unused here: an inline's statics are emitted where it is defined, and
// retail's copies, the first in link order, are this TU's (its assertion flag
// at 0x451D28, before GRender3DID).
inline ScreenTextGlyph *ScreenTextFont::GetCell(int ascii) const
{
#line 123 "ScreenTextFontManager.h"
    ASSERTF(ascii >= 0 && ascii < 256, ("Value out of range : ascii=%d\n", ascii));
    return m_glyphs[ascii];
}

#include "mwcc_va.h"




extern "C" int strcmp(const char *a, const char *b);
extern "C" int vsprintf(char *buf, const char *fmt, va_list ap);

extern VideoCard *LpVideoCard;
extern Game *LpGame;
int GRender3DID;                         // .bss 0x451D30

// ---- src/DebugOverlay/SetLine.cpp

void DebugOverlay::SetLine(int line, const char *text, int color)
{
    if (line < m_numLines && strcmp(m_lines[line].m_text, text)) {
        strncpy(m_lines[line].m_text, text, 79);
        m_lines[line].m_text[79] = 0;
        m_lines[line].m_color = color;
    }
}

// ---- src/DebugOverlay/PrintfDetail.cpp

void DebugOverlay::PrintfDetail(int page, const char *fmt, ...)
{
    if (!m_enabled || page != m_page)
        return;
    if (m_line + 1 < m_numLines) {
        va_list ap;
        va_start(ap, fmt);
        vsprintf(m_buffer, fmt, ap);
        SetLine(++m_line, m_buffer, 0);
    }
}

// ---- src/DebugOverlay/PrintfHeading.cpp

void DebugOverlay::PrintfHeading(int page, const char *fmt, ...)
{
    if (!m_enabled || page != m_page)
        return;
    if (!m_enabled || m_line + 1 >= m_numLines)  // retail tests m_enabled again
        return;
    va_list ap;
        va_start(ap, fmt);
        vsprintf(m_buffer, fmt, ap);
        SetLine(++m_line, m_buffer, 1);
}

// ---- src/DebugOverlay/RenderDelayed.cpp

int DebugOverlay::RenderDelayed()
{
    if (m_line == -1 || !m_font)
        return 1;
    static int Render3DID = -1;
    GRender3DID = Render3DID;
    _RECT viewport;
    int i = 0;
    if (LpGame->m_cameras[1]) {
        LpRenderTarget->WaitForRenderingToComplete();
        LpRenderTarget->m_camera->GetViewportBoundsPhysical(&viewport);
        LpRenderTarget->m_camera->ResizeViewport(0, 0, LpVideoCard->m_width, (unsigned int &)LpVideoCard->m_height);   // read as unsigned: retail's load order
    }
    for (i = 0; i <= m_line; i++) {
        m_texts[i].SetString(m_lines[i].m_text);
        m_texts[i].SetPosition(m_lines[i].m_x, m_lines[i].m_y);
        if (m_lines[i].m_color == 1)
            m_texts[i].SetColor(0xFFFF0000);
        else
            m_texts[i].SetColor(0xFFFFFFFF);
        m_textManager->PositionText(&m_texts[i], ScreenTextManager::LeftJustification, m_font);
        m_textManager->AddText(&m_texts[i], m_font);
    }
    m_line = -1;
    if (LpGame->m_cameras[1]) {
        LpRenderTarget->WaitForRenderingToComplete();
        LpRenderTarget->m_camera->ResizeViewport(viewport.left, viewport.top, viewport.right - viewport.left,
                                                 viewport.bottom - viewport.top);
    }
    return 1;
}

