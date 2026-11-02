// UIGlobalManager.cpp  @ 0x00315F30 (text 0x315F30-0x316640)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// UIGlobalManager as one translation unit without a static initializer,
// after JetskiUIGlobalManager.cpp in its tu_map row: its .rodata repeats
// the BaseObject/GameObject RTTI records.



#include "SurfaceShaderList/SurfaceShaderList.h"
#include "UIGlobalManager/UIGlobalManager.h"
#include "GameObject/GameObject.h"
#include "PSXSurfaceShaderList/PSXSurfaceShaderList.h"
#include "UIManager/UIManager.h"
#include "ScreenTextManager/ScreenTextManager.h"
#include "ParameterBlock/ParameterBlock.h"
#include "UIWidget/UIWidget.h"
#include "UIWidgetSoultree/UIWidgetSoultree.h"
#include "UIWidgetSoultreeLightweight/UIWidgetSoultreeLightweight.h"
#include "UIWidgetSoultreeCharacter/UIWidgetSoultreeCharacter.h"

extern "C" void free(void *p);
extern "C" void *malloc(unsigned int size);
extern "C" int sprintf(char *buf, const char *fmt, ...);
extern "C" int strcmp(const char *a, const char *b);
void PSXprintf(const char *fmt, ...);

extern SurfaceShaderList *LpSurfaceShaderList;
extern char AnimContentDirectory[];

// ---- src/UIGlobalManager/ctor.cpp
UIGlobalManager::UIGlobalManager(int enabled)
    : GameObject(enabled)
{
    m_screenTextManager = 0;
    m_widgets = 0;
    m_numWidgets = 0;
    m_uiManager = 0;
}


// ---- src/UIGlobalManager/dtor.cpp


UIGlobalManager::~UIGlobalManager()
{
    if (m_widgets) {
        free(m_widgets);
        m_widgets = 0;
    }
    if (LpSurfaceShaderList) {
        delete LpSurfaceShaderList;
        LpSurfaceShaderList = 0;
    }
}


// ---- src/UIGlobalManager/Create.cpp


UIGlobalManager *UIGlobalManager::Create(int a, int b, int language)
{
    new PSXSurfaceShaderList;   // registers itself; the pointer isn't kept
    if (!LoadScreenTextManager())
        return 0;
    if (!LoadGlobalWidgets())
        return 0;
    if (language > -1)
        Localize(language);
    CreateUIManager(a, b);
    return this;
}


// ---- src/UIGlobalManager/Localize.cpp


int UIGlobalManager::Localize(int language)
{
    if (m_uiManager)
        m_uiManager->Localize();
    return 1;
}


// ---- src/UIGlobalManager/LoadScreenTextManager.cpp


int UIGlobalManager::LoadScreenTextManager()
{
    if (!(m_screenTextManager = (new ScreenTextManager(0, 1))->Create(0x80))) {
        char msg[400];
        sprintf(msg, "ScreenTextManager not created.\n");
        PSXprintf(msg);
        return 0;
    }
    return 1;
}


// ---- src/UIGlobalManager/RemoveUIManager.cpp

void UIGlobalManager::RemoveUIManager()
{
    m_screenTextManager->RemoveFromChain();
    for (int i = 0; i < m_numWidgets; i++)
        m_widgets[i]->DisableAll();
    if (m_uiManager) {
        m_uiManager->RemoveFromChain();
        m_uiManager->Release();
        m_uiManager = 0;
    }
}


// ---- src/UIGlobalManager/LoadGlobalWidgets.cpp


// Loads the widgets listed in gwidgets.dat ("Widget 1".."Widget N"), each
// with its per-player joystick flags, and starts them disabled.
int UIGlobalManager::LoadGlobalWidgets()
{
    UIWidget *widget;
    char name[0x100];
    int ok = 1;
    ParameterBlock params;
    int i;
    if (params.OpenFile("gwidgets.dat", 0)) {
        params.ReadParameterBlock("General");
        params.GetParameter("NumberOfWidgets", 0, &m_numWidgets);
        m_widgets = (GameObject **)malloc(m_numWidgets * sizeof(GameObject *));
        for (i = 0; i < m_numWidgets; i++) {
            sprintf(name, "Widget %d", i + 1);
            widget = (UIWidget *)GetWidget(&params, name);
            if (widget) {
                for (int j = 0; j < 2; j++) {
                    int wants;
                    sprintf(name, "WantsJoystickInput%d", j + 1);
                    params.GetParameter(name, 0, &wants);
                    widget->m_wantsJoystickInput[j] = wants;
                }
                widget->DisableAll();
                m_widgets[i] = widget;
            } else {
                PSXprintf("Failed to load widget in UIGlobalManager::LoadGlobalWidgets\n");
                m_widgets[i] = 0;
                ok = 0;
            }
        }
    } else {
        PSXprintf("Failed to find global widget list in UIGlobalManager::LoadGlobalWidgets\n");
    }
    return ok;
}


// ---- src/UIGlobalManager/GetWidget.cpp


// Loads a 3D widget described by a parameter block file.
GameObject *UIGlobalManager::GetWidget(ParameterBlock *params, char *name)
{
    char path[0x110];
    char value[0x104];
    GameObject *widget;
    params->ReadParameterBlock(name);
    params->GetParameter("Type", value, sizeof(value));
    if (!strcmp(value, "Soultree")) {
        params->GetParameter("SLT", value, sizeof(value));
        widget = AddChild((new UIWidgetSoultree(0, 1))->Create(1, value, params), -1);
    } else if (!strcmp(value, "Soultree_Lightweight")) {
        params->GetParameter("SLT", value, sizeof(value));
        widget = AddChild((new UIWidgetSoultreeLightweight(0, 1))->Create(2, value, params), -1);
    } else if (!strcmp(value, "Soultree_Character")) {
        params->GetParameter("MCF", value, sizeof(value));
        sprintf(path, "%s%s", AnimContentDirectory, value);
        widget = AddChild((new UIWidgetSoultreeCharacter(0, 1))->Create(3, path, params), -1);
    } else {
        widget = 0;
        PSXprintf("Failed to load %s in UIGlobalManager::GetWidget", name);
    }
    return widget;
}

