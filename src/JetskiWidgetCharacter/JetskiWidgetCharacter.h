// JetskiWidgetCharacter: the UI widget that shows a rider model, a UIWidget.
// 0x1E0 bytes (new in JetskiUIManager's widget factory); its data starts at
// UIWidget's 0x58. Members from the ctor and Create/CreateCharacter: +0x58
// is the JetskiUIManager the ctor is given (Create copies it from
// UIWidget::m_manager), +0x5C the D3DIMSoultreeCharacter CreateCharacter
// news, +0xA4 the motion name CreateCharacter reads (GetParameter max 0x100),
// +0x1A4 the name Create copies (0x3C bytes to the class's end).
#ifndef JETSKIWIDGETCHARACTER_JETSKIWIDGETCHARACTER_H
#define JETSKIWIDGETCHARACTER_JETSKIWIDGETCHARACTER_H

#include "UIWidget/UIWidget.h"
#include "d3dtypes.h"

class Character;
class JetskiUIManager;

class JetskiWidgetCharacter : public UIWidget {
public:
    JetskiWidgetCharacter(JetskiUIManager *manager, int id);
    JetskiWidgetCharacter *Create(int id, char *name, ParameterBlock *params);
    void CreateCharacter(int rider, int mode);
    void DeleteCharacter();
    void ChangeSkin(int skin);

    // --- overrides in retail slot order ---
    virtual ~JetskiWidgetCharacter();
    virtual int Tick(float dt);

    JetskiUIManager *m_manager;         // +0x58, hides UIWidget::m_manager
    Character *m_character;             // +0x5C
    int m_mode;                         // +0x60
    int m_rider;                        // +0x64
    int m_skin;                         // +0x68
    char pad6C[0x70 - 0x6C];
    _D3DVECTOR m_position;              // +0x70
    _D3DVECTOR m_lookVector;            // +0x80
    _D3DVECTOR m_upVector;              // +0x90
    int m_initialMotion;                // +0xA0
    char m_motionName[0x100];           // +0xA4
    char m_name[0x1E0 - 0x1A4];         // +0x1A4
};

#endif
