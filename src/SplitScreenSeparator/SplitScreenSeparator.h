// SplitScreenSeparator — the bars drawn between split-screen viewports.
#ifndef SPLITSCREENSEPARATOR_H
#define SPLITSCREENSEPARATOR_H

#include "_RECT/_RECT.h"

#include "ScreenPrimitiveVertex/ScreenPrimitiveVertex.h"

#include "ScreenPrimitive/ScreenPrimitive.h"

#include "ScreenPrimitiveManager/ScreenPrimitiveManager.h"

#include "ScreenObject/ScreenObject.h"
#include "ViewPortCode/ViewPortCode.h"

class SplitScreenSeparator : public ScreenObject {
public:
    SplitScreenSeparator(int enabled);
    SplitScreenSeparator *Create(ViewPortCode viewport);
    virtual ~SplitScreenSeparator();
    int PrepareGeometry();
    int Render3D();
    void SetupRects(_RECT rect);

    ScreenPrimitive *m_primitives[4];           // +0x58
    ScreenPrimitiveVertex *m_vertices[4];       // +0x68
    int m_orientation;                          // +0x78 (1 = horizontal split)
    _RECT m_viewportRects[2];                   // +0x7C, per viewport (new is 0xA0)
};

#endif
