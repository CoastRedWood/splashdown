// UIWidgetStaticUserRotate: a UI widget showing a Soultree model the player
// can turn (UIWidgets.cpp). 0xB0 bytes (`new` in UIManager::GetWidget).
#ifndef UIWIDGETSTATICUSERROTATE_H
#define UIWIDGETSTATICUSERROTATE_H

#include "d3dtypes.h"
#include "UIWidget/UIWidget.h"

class PSXRenderableObject;

class UIWidgetStaticUserRotate : public UIWidget {
public:
    UIWidgetStaticUserRotate(UIManager *manager, int enabled);
    UIWidgetStaticUserRotate *Create(int id, char *name, ParameterBlock *block);
    void UpdatePosition(float dt);
    void RotateXAxis(float dt);
    void RotateYAxis(float dt);
    void RotateZAxis(float dt);

    // --- virtuals in retail slot order ---
    virtual ~UIWidgetStaticUserRotate();
    virtual int Tick(float dt);
    virtual void WidgetSelect();
    virtual void WidgetDeSelect();

    PSXRenderableObject *m_object;  // +0x58
    char pad5C[0x60 - 0x5C];
    _D3DVECTOR m_position;          // +0x60
    _D3DVECTOR m_startPosition;     // +0x70
    float m_rotateSpeedX;           // +0x80
    float m_rotateSpeedY;           // +0x84
    float m_rotateSpeedZ;           // +0x88
    int m_rotateX;                  // +0x8C
    int m_rotateY;                  // +0x90
    int m_rotateZ;                  // +0x94
    int m_deselected;               // +0x98, "StartPaused"
    int m_x9C;                      // +0x9C
    int m_xA0;                      // +0xA0
    int m_xA4;                      // +0xA4
    float m_rotateScale;            // +0xA8, 2 pi
    char padAC[0xB0 - 0xAC];
};

#endif
