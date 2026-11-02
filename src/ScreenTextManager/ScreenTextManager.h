// ScreenTextManager: queues ScreenTexts each frame (AddText) and sends them as
// VU1 packets in Render3D. Data from the ctor (src/ScreenTextManager/ctor.cpp)
// and its own methods; 0x50 bytes.
#ifndef SCREENTEXTMANAGER_SCREENTEXTMANAGER_H
#define SCREENTEXTMANAGER_SCREENTEXTMANAGER_H

#include "GameObject/GameObject.h"
#include "d3dtypes.h"

class DmaRenderItem;
class ScreenText;
class ScreenTextFont;

class ScreenTextManager : public GameObject {
public:
    ScreenTextManager(ScreenTextFont *font, int enabled);
    ScreenTextManager *Create(int maxTexts);
    void AddText(ScreenText *text, ScreenTextFont *font);
    void SetCharacterCells(ScreenText *text);
    void RenderQueuedText();
    void AddTextToRenderItem(DmaRenderItem *item, ScreenText *text);
    int ComposeRenderItem(ScreenText *text);
    int GetStringWidthInPixels(ScreenText *text);
    int GetStringWidthInPixels(ScreenText *text, ScreenTextFont *font, float scale);
    void PositionText(ScreenText *text);
    void PositionText(ScreenText *text, int justification, ScreenTextFont *font);

    static int LeftJustification;
    static int CenterHorizontalJustification;
    static int CenterVerticalJustification;
    static int CenterBothJustification;
    static int RightJustification;
    // The vertex buffer ComposeRenderItem builds a text in: 5 quadwords per
    // character (4 corners, then the UVs); __sinit constructs it.
    static _D3DVECTOR TempVertices[100][5];

    // --- virtuals in retail slot order ---
    virtual ~ScreenTextManager();
    virtual int Render3D();

    int m_maxTexts;                     // +0x30, at most 128
    ScreenText **m_texts;               // +0x34, this frame's queue
    int m_numTexts;                     // +0x38, cleared by Render3D
    ScreenTextFont *m_defaultFont;      // +0x3C
    int m_x40;                          // +0x40, 1 from the ctor: Render3D renders only if set
    DmaRenderItem *m_renderItems;       // +0x44, one per text
    DmaRenderItem *m_startItem;         // +0x48
    DmaRenderItem *m_endItem;           // +0x4C
};

#endif
