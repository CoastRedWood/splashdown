// What the JetskiWidgetCharacter files share beyond the class header.
#ifndef JETSKIWIDGETCHARACTERLOCAL_H
#define JETSKIWIDGETCHARACTERLOCAL_H

extern "C" char *strcpy(char *dst, const char *src);

#include "d3dtypes.h"
#include "ParameterBlock/ParameterBlock.h"
#include "SoultreeObject/SoultreeObject.h"
#include "Character/Character.h"

// RenderableObject.h lacks ChangeMaterial (ChangeMaterial__16RenderableObjectFii).
#include "RenderableObject/RenderableObject.h"
#include "UIPage/UIPage.h"
#include "JetskiUIManager/JetskiUIManager.h"
#include "JetskiWidgetCharacter/JetskiWidgetCharacter.h"

// The page's craft widget (a JetskiWidgetSeadoo): its model at +0x5C.
class UIWidgetModel : public UIWidget {
public:
    char pad58[PS2_PTR_SIZE];           // JetskiWidgetSeadoo::m_manager, a pointer
    SoultreeObject *m_model;            // +0x5C
};

#endif
