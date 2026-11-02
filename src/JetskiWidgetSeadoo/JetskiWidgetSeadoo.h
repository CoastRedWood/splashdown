// JetskiWidgetSeadoo: the UI widget that shows a craft model, a UIWidget.
// 0xE0 bytes (new in JetskiUIManager's widget factory); its data starts at
// UIWidget's 0x58. Members from the ctor, Create and CreateSeadoo: +0x58 is
// the JetskiUIManager the ctor is given (Create copies it from
// UIWidget::m_manager), +0x5C the PSXRenderableObject CreateSeadoo news,
// +0xA0 Create's "AutoRotate" parameter, +0xA8 the model file name (0x38
// bytes to the class's end; copies declare 0x40).
#ifndef JETSKIWIDGETSEADOO_JETSKIWIDGETSEADOO_H
#define JETSKIWIDGETSEADOO_JETSKIWIDGETSEADOO_H

#include "UIWidget/UIWidget.h"
#include "d3dtypes.h"

class JetskiUIManager;
class PSXRenderableObject;

class JetskiWidgetSeadoo : public UIWidget {
public:
    JetskiWidgetSeadoo(JetskiUIManager *manager, int id);
    JetskiWidgetSeadoo *Create(int id, char *name, ParameterBlock *params);
    void CreateSeadoo(int model);
    void DeleteSeadoo();
    void ChangeSkin(int skin);

    // --- overrides in retail slot order ---
    virtual ~JetskiWidgetSeadoo();
    virtual int Tick(float dt);

    JetskiUIManager *m_manager;         // +0x58, hides UIWidget::m_manager
    PSXRenderableObject *m_seadoo;      // +0x5C
    int m_model;                        // +0x60
    int m_skin;                         // +0x64
    char pad68[0x70 - 0x68];
    _D3DVECTOR m_position;              // +0x70
    _D3DVECTOR m_lookVector;            // +0x80
    _D3DVECTOR m_upVector;              // +0x90
    float m_autoRotate;                 // +0xA0
    int m_xA4;                          // +0xA4
    char m_fileName[0xE0 - 0xA8];       // +0xA8
};

#endif
