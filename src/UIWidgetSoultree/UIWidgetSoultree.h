// UIWidgetSoultree: a UI widget showing a static Soultree model that can spin
// (UIWidgets.cpp). 0x160 bytes (`new` in UIManager::GetWidget).
#ifndef UIWIDGETSOULTREE_H
#define UIWIDGETSOULTREE_H

#include "d3dtypes.h"
#include "UIWidget/UIWidget.h"

class RenderableObject;

class UIWidgetSoultree : public UIWidget {
public:
    UIWidgetSoultree(UIManager *manager, int enabled);
    UIWidgetSoultree *Create(int id, char *name, ParameterBlock *block);
    void Rotate(float dt);
    int SelectedTextureChange();
    int DeSelectedTextureChange();

    // --- virtuals in retail slot order ---
    virtual ~UIWidgetSoultree();
    virtual int Tick(float dt);
    virtual void WidgetSelect();
    virtual void WidgetDeSelect();
    virtual void WidgetKeepEnabled();
    virtual void Initialize();

    RenderableObject *m_object;     // +0x58
    char pad5C[0x60 - 0x5C];
    _D3DVECTOR m_position;          // +0x60
    _D3DVECTOR m_forward;           // +0x70
    _D3DVECTOR m_up;                // +0x80
    float m_rotationPeriod;         // +0x90, 0 = no spin
    float m_angle;                  // +0x94
    char m_selectedTexture[100];    // +0x98
    char m_deselectedTexture[100];  // +0xFC
};

#endif
