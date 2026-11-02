// UIWidgetSoultreeCharacter — a UI widget showing an animated character
// (UIWidgets.cpp). 0x3A0 bytes (`new` in UIManager::GetWidget).
#ifndef UIWIDGETSOULTREECHARACTER_H
#define UIWIDGETSOULTREECHARACTER_H

extern "C" int strcmp(const char *a, const char *b);
extern "C" void free(void *p);

#include "d3dtypes.h"
#include "D3DIMSoultreeCharacter/D3DIMSoultreeCharacter.h"
#include "SoultreeObject/SoultreeObject.h"
#include "UIWidget/UIWidget.h"

class UIManager;

class UIWidgetSoultreeCharacter : public UIWidget {
public:
    UIWidgetSoultreeCharacter(UIManager *manager, int id);

    // --- virtuals in retail slot order ---
    virtual ~UIWidgetSoultreeCharacter();
    virtual int Tick(float dt);
    virtual void WidgetSelect();
    virtual void WidgetDeSelect();
    virtual void WidgetKeepEnabled();
    virtual void Initialize();
    virtual UIWidgetSoultreeCharacter *Create(int id, char *name, ParameterBlock *params);

    D3DIMSoultreeCharacter *m_character;    // +0x58
    char pad5C[0x60 - 0x5C];
    _D3DVECTOR m_position;          // +0x60
    _D3DVECTOR m_forward;           // +0x70
    _D3DVECTOR m_up;                // +0x80
    char m_initialAction[0x100];    // +0x90
    char m_selectAction[0x100];     // +0x190
    char m_deselectAction[0x100];   // +0x290
    int m_numActionNames;           // +0x390
    char **m_actionNames;           // +0x394
    char pad398[0x3A0 - 0x398];
};

#endif
