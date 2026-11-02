// JetskiWidgetTextMove: a JetskiWidgetText whose lines slide between
// (XStart, YStart) and the text position (Transition/ProcessTransition).
// 0x148 bytes (new in JetskiUIManager's widget factory). Members from
// Create's parameter keys and the ctor (clears +0x130..+0x144).
#ifndef JETSKIWIDGETTEXTMOVE_JETSKIWIDGETTEXTMOVE_H
#define JETSKIWIDGETTEXTMOVE_JETSKIWIDGETTEXTMOVE_H

#include "JetskiWidgetText/JetskiWidgetText.h"

class LightManager;

class JetskiWidgetTextMove : public JetskiWidgetText {
public:
    JetskiWidgetTextMove(UIManager *manager, int enabled);
    JetskiWidgetTextMove *Create(int id, ParameterBlock *params, LightManager *lights, ScreenTextManager *textManager);

    // --- overrides in retail slot order ---
    virtual ~JetskiWidgetTextMove();
    virtual int Tick(float dt);
    virtual void Transition(int direction);
    virtual int ProcessTransition(float dt);

    int m_xStart;                       // +0x130, "XStart"
    int m_yStart;                       // +0x134, "YStart"
    int m_x;                            // +0x138, current position
    int m_y;                            // +0x13C
    float m_speed;                      // +0x140, "Speed" (pixels per second)
    float m_progress;                   // +0x144, fractional pixels
};

#endif
