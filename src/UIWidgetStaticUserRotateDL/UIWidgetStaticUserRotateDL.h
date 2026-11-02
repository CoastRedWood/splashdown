// UIWidgetStaticUserRotateDL: UIWidgetStaticUserRotate whose model loads on
// selection ("DynamicLoad", UIWidgets.cpp). 0x250 bytes (`new` in
// UIManager::GetWidget).
#ifndef UIWIDGETSTATICUSERROTATEDL_H
#define UIWIDGETSTATICUSERROTATEDL_H

#include "UIWidgetStaticUserRotate/UIWidgetStaticUserRotate.h"

class UIWidgetStaticUserRotateDL : public UIWidgetStaticUserRotate {
public:
    UIWidgetStaticUserRotateDL(UIManager *manager, int enabled);
    UIWidgetStaticUserRotateDL *Create(int id, char *name, ParameterBlock *block);
    int LoadResources();
    int CleanResources();

    // --- virtuals in retail slot order ---
    virtual ~UIWidgetStaticUserRotateDL() {}   // inline in retail, so Tick is the key function
    virtual int Tick(float dt);
    virtual void WidgetSelect();
    virtual void WidgetDeSelect();

    int m_isLit;                    // +0xB0
    char padB4[0xC0 - 0xB4];
    _D3DVECTOR m_forward;           // +0xC0
    _D3DVECTOR m_up;                // +0xD0
    char m_fileName[0x104];         // +0xE0
    int m_resourcesLoaded;          // +0x1E4
    int m_timing;                   // +0x1E8, waiting out m_loadDelay
    int m_cleanFromMemory;          // +0x1EC
    int m_restoreMatrix;            // +0x1F0
    int m_setPosition;              // +0x1F4
    int m_setOrientation;           // +0x1F8
    int m_x1FC;                     // +0x1FC
    _D3DMATRIX m_savedMatrix;       // +0x200
    float m_loadDelay;              // +0x240
    char pad244[0x248 - 0x244];
    long m_selectTime;              // +0x248
};

#endif
