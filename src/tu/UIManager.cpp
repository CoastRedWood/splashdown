// UIManager.cpp  @ 0x00317AB0 (text 0x317AB0-0x318C00)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's UIManager.cpp as one translation unit: UIManager's functions and
// UICameraInfo's ctor, then the static initializer. Its .rodata runs @964 ..
// the UIManager RTTI (the LoadPages/LoadWidgets/GetWidget strings included),
// so the UIManager functions after SetCurrentPage, up to
// LoadRequestedMaterials (0x318BD0), belong here too; with them come
// UIPage::ResetPage, ProcessTransition and Transition and
// UIWidget::WidgetLocalize, defined here in retail's order. UIWidgets.cpp
// follows. `make link` places the object's data as well as its code
// (tools/link_c.py).


#include "PSXVideoCard/PSXVideoCard.h"
#include "UIManager/UIManager.h"
#include "MaterialStringTable/MaterialStringTable.h"
#include "TextureMapStringTable/TextureMapStringTable.h"
#include "GameObject/GameObject.h"
#include "RenderTarget/RenderTarget.h"
#include "TextureMapManager/TextureMapManager.h"
#include "Game/Game.h"
#include "UIGlobalManager/UIGlobalManager.h"
#include "UIPage/UIPage.h"
#include "ParameterBlock/ParameterBlock.h"
#include "UIWidget/UIWidget.h"
#include "UIWidgetEventHandler/UIWidgetEventHandler.h"
#include "UIWidgetSoultree/UIWidgetSoultree.h"
#include "UIWidgetSoultreeLightweight/UIWidgetSoultreeLightweight.h"
#include "UIWidgetSoultreeCharacter/UIWidgetSoultreeCharacter.h"
#include "UIWidgetOverlayText/UIWidgetOverlayText.h"
#include "UIWidgetIcon/UIWidgetIcon.h"
#include "UIWidgetMovementControl/UIWidgetMovementControl.h"
#include "UIWidgetMovementGroup/UIWidgetMovementGroup.h"
#include "UIWidgetStaticUserRotateDL/UIWidgetStaticUserRotateDL.h"
#include "GameObject/GameObjectGlobals.h"
#include "sinit/AxisVectors.h"

extern "C" int sprintf(char *buf, const char *fmt, ...);
extern "C" int strcmp(const char *a, const char *b);
extern "C" void *malloc(unsigned int size);
void PSXprintf(const char *fmt, ...);
extern char UIContentDirectory[];

// ---- src/UIManager/ctor.cpp
extern PSXVideoCard *LpVideoCard;


UIManager::UIManager(int enabled)
    : GameObject(enabled)
{
    m_x180 = 1;
    m_x38 = 0;
    m_textManager = 0;
    m_iconManager = 0;
    m_x144 = 0;
    m_numPages = 0;
    m_currentPage = 0;
    m_x178 = 0;
    m_closing = 0;
    m_x164 = 0;
    m_state = 0;
    m_direction = 0;
    LpVideoCard->m_x44 = 1;
}


// ---- src/UICameraInfo/ctor.cpp
// Out of line (0x317B80); UIManager's ctor runs it through __construct_array.

UICameraInfo::UICameraInfo()
{
    m_position[2] = 0;
    m_position[1] = 0;
    m_position[0] = 0;
    m_position[3] = 1.0f;
    m_target[2] = 0;
    m_target[1] = 0;
    m_target[0] = 0;
    m_target[3] = 1.0f;
    m_up[2] = 0;
    m_up[1] = 0;
    m_up[0] = 0;
    m_up[3] = 1.0f;
}


// ---- src/UIManager/dtor.cpp
extern MaterialStringTable *const LpMaterialStringTable;

extern TextureMapStringTable *const LpTextureMapStringTable;

extern "C" void free(void *p);


UIManager::~UIManager()
{
    if (m_pages)
        free(m_pages);
    if (m_widgets)
        free(m_widgets);
    LpVideoCard->m_x44 = 0;
    LpMaterialStringTable->ReleaseMaterials();
    LpTextureMapStringTable->ReleaseTextureMaps();
}


// ---- src/UIManager/Create.cpp
extern TextureMapManager *LpTextureMapManager;


extern Game *LpGame;


UIManager *UIManager::Create(UIGlobalManager *global)
{
    m_global = global;
    m_textManager = global->m_screenTextManager;
    LpRenderTarget->SetTextureStageState(0, (_D3DTEXTURESTAGESTATETYPE)13, 0);
    LpRenderTarget->SetTextureStageState(0, (_D3DTEXTURESTAGESTATETYPE)14, 0);
    if (LpGame->m_textureCache)
        LpTextureMapManager->SetupCache();
    return this;
}


// ---- src/UIManager/SetCurrentPage.cpp


void UIManager::SetCurrentPage(int page)
{
    m_currentIndex = page;
    m_currentPage = m_pages[page - 1];
    m_currentPage->PageSelect();
    m_currentPage->ResetPage();
}


// ---- src/UIPage/ResetPage.cpp

void UIPage::ResetPage()
{
}


// ---- src/UIManager/Tick.cpp


int UIManager::Tick(float dt)
{
    if (!m_x180) {
        switch (m_state) {
        case 1:
            SwitchPage(m_nextPage);
            m_currentPage->ResetPage();
            PlaySound(m_state, 1, 0, 0);
            m_state = 4;
            m_direction = 1;
            m_currentPage->Transition(m_direction);
            m_currentPage->m_eventHandler->DisableAll();
            break;
        case 2:
            PlaySound(m_state, 1, 0, 0);
            m_state = 4;
            m_direction = 1;
            m_currentPage->Transition(m_direction);
            m_currentPage->m_eventHandler->DisableAll();
            break;
        case 7:
            m_state = 4;
            m_direction = 2;
            m_currentPage->Transition(m_direction);
            m_currentPage->m_eventHandler->DisableAll();
            break;
        case 6:
            m_state = 3;
            m_direction = 2;
            m_currentPage->Transition(m_direction);
            m_currentPage->m_eventHandler->DisableAll();
            break;
        case 8:
            m_state = 5;
            m_direction = 2;
            m_currentPage->Transition(m_direction);
            m_currentPage->m_eventHandler->DisableAll();
            break;
        case 3:
            if (m_currentPage->ProcessTransition(dt)) {
                if (m_direction == 2)
                    m_state = 9;
                m_direction = 0;
            }
            break;
        case 4:
            if (m_currentPage->ProcessTransition(dt)) {
                if (m_direction == 1) {
                    m_currentPage->m_eventHandler->EnableAll();
                    m_state = 0;
                } else if (m_direction == 2) {
                    m_state = 1;
                }
                m_direction = 0;
            }
            break;
        case 5:
            if (m_currentPage->ProcessTransition(dt)) {
                if (m_direction == 2)
                    m_state = 10;
                m_direction = 0;
            }
            break;
        case 9:
            m_closing = 1;
            m_x178 = 0;
            break;
        case 10:
            m_x178 = 1;
            m_closing = 1;
            break;
        }
    } else {
        m_state = 2;
        m_x180 = 0;
    }
    if (m_closing)
        UIPrepareClose();
    return GameObject::Tick(dt);
}


// ---- src/UIPage/ProcessTransition.cpp

int UIPage::ProcessTransition(float dt)
{
    return 1;
}


// ---- src/UIPage/Transition.cpp

int UIPage::Transition(int direction)
{
    return 1;
}


// ---- src/UIManager/SwitchPage.cpp


int UIManager::SwitchPage(int page)
{
    m_pages[m_currentIndex - 1]->PageDeSelect();
    m_pages[page - 1]->PageSelect();
    m_currentIndex = page;
    m_currentPage = m_pages[m_currentIndex - 1];
    return 1;
}


// ---- src/UIManager/LoadPages.cpp


int UIManager::LoadPages()
{
    char file[0x100];
    char pageFile[0x100];
    char pageName[0x100];
    ParameterBlock params;
    int i;

    sprintf(file, "%sui.bpb", UIContentDirectory);
    if (!params.OpenFile(file, 0)) {
        PSXprintf("Failed to load ui.bpb in UIManager::LoadPages, try ui.dat\n");
        sprintf(file, "%sui.dat", UIContentDirectory);
        if (!params.OpenFile(file, 0)) {
            PSXprintf("Failed to load ui.dat in UIManager::LoadPages\n");
            return 0;
        }
    }
    params.ReadParameterBlock("UIBase");
    params.GetParameter("TotalPages", 0, &m_numPages);
    m_pages = (UIPage **)malloc(m_numPages * sizeof(UIPage *));
    for (i = 0; i < m_numPages; i++) {
        sprintf(pageName, "Page %d", i + 1);
        params.ReadParameterBlock(pageName);
        params.GetParameter("PageFile", pageFile, 0x100);
        sprintf(file, "%s%s", UIContentDirectory, pageFile);
        m_pages[i] = GetPage(i + 1, file);
        m_pages[i]->DisableAll();
        PSXprintf("%d PAGE\n", i);
    }
    return 1;
}


// ---- src/UIManager/AttachPages.cpp


int UIManager::AttachPages()
{
    for (int i = 0; i < m_numPages; i++) {
        if (m_pages[i])
            AddChild(m_pages[i], -1);
        else
            return 0;
    }
    return 1;
}


// ---- src/UIManager/UIPrepareClose.cpp

void UIManager::UIPrepareClose() {}


// ---- src/UIManager/PlaySound.cpp

void UIManager::PlaySound(int, int, unsigned int, int) {}


// ---- src/UIManager/LoadWidgets.cpp


int UIManager::LoadWidgets()
{
    char file[0x100];
    UIWidget *widget;
    int ok;
    int i;
    int j;
    ok = 1;
    sprintf(file, "%sWidgets.Bpb", UIContentDirectory);
    ParameterBlock params;
    int value;
    if (!params.OpenFile(file, 0)) {
        PSXprintf("Failed to load widget list (.bpb) in UIManager::LoadWidgets, try (.dat)\n");
        sprintf(file, "%sWidgets.Dat", UIContentDirectory);
        if (!params.OpenFile(file, 0))
            return 0;
    }
    params.ReadParameterBlock("General");
    params.GetParameter("NumberOfWidgets", 0, &m_numWidgets);
    m_widgets = (UIWidget **)malloc(m_numWidgets * sizeof(UIWidget *));
    for (i = 0; i < m_numWidgets; i++) {
        sprintf(file, "Widget %d", i + 1);
        params.ReadParameterBlock(file);
        params.GetParameter("Type", file, 0x100);
        widget = GetWidget(&params, file);
        if (widget) {
            for (j = 0; j < 2; j++) {
                sprintf(file, "WantsJoystickInput%d", j + 1);
                params.GetParameter(file, 0, &value);
                widget->m_wantsJoystickInput[j] = value;
            }
            widget->DisableAll();
            m_widgets[i] = widget;
        } else {
            ok = 0;
            m_widgets[i] = 0;
        }
    }
    return ok;
}


// ---- src/UIManager/GetWidget.cpp


extern char AnimContentDirectory[];


UIWidget *UIManager::GetWidget(ParameterBlock *block, char *type)
{
    char path[0x110];
    char name[0x108];
    int index;
    int dynamicLoad;
    UIWidget *widget;
    if (!strcmp(type, "Global")) {
        if (block->GetParameter("GlobalWidgetNumber", 0, &index))
            widget = (UIWidget *)m_global->m_widgets[index - 1];
        else
            widget = 0;
    } else if (!strcmp(type, "Soultree")) {
        block->GetParameter("SLT", name, 0x104);
        widget = (new UIWidgetSoultree(this, 1))->Create(1, name, block);
    } else if (!strcmp(type, "Soultree_Lightweight")) {
        block->GetParameter("SLT", name, 0x104);
        widget = (new UIWidgetSoultreeLightweight(this, 1))->Create(2, name, block);
    } else if (!strcmp(type, "Soultree_Character")) {
        block->GetParameter("MCF", name, 0x104);
        sprintf(path, "%s%s", AnimContentDirectory, name);
        widget = (new UIWidgetSoultreeCharacter(this, 1))->Create(3, path, block);
    } else if (!strcmp(type, "Overlay_Text")) {
        widget = (new UIWidgetOverlayText(this, 1))->Create(4, block, m_textManager);
    } else if (!strcmp(type, "Icon")) {
        widget = (new UIWidgetIcon(this, 1))->Create(5, block, this);
    } else if (!strcmp(type, "Movement_Control")) {
        widget = (new UIWidgetMovementControl(this, 1))->Create(9, block, this);
    } else if (!strcmp(type, "Movement_Group")) {
        widget = (new UIWidgetMovementGroup(this, 1))->Create(0xA, block, this);
    } else if (!strcmp(type, "User_Rotate")) {
        block->GetParameter("DynamicLoad", 0, &dynamicLoad);
        block->GetParameter("SLT", name, 0x104);
        if (dynamicLoad)
            widget = (new UIWidgetStaticUserRotateDL(this, 1))->Create(0xB, name, block);
        else
            widget = (new UIWidgetStaticUserRotate(this, 1))->Create(0xB, path, block);  // retail passes path, never set here
    } else {
        widget = 0;
        PSXprintf("Failed to load %s in UIManager::GetWidget", type);
    }
    return widget;
}


// ---- src/UIManager/AttachWidgets.cpp


int UIManager::AttachWidgets()
{
    for (int i = 0; i < m_numWidgets; i++) {
        if (m_widgets[i] && !m_widgets[i]->m_parent)
            AddChild(m_widgets[i], -1);
    }
    return 1;
}


// ---- src/UIManager/Localize.cpp

int UIManager::Localize()
{
    LpRenderTarget->TrySendRenderQueue(1);
    LpRenderTarget->WaitForRenderingToComplete();
    for (int i = 0; i < m_numWidgets; i++)
        m_widgets[i]->WidgetLocalize();
    return 1;
}


// ---- src/UIWidget/WidgetLocalize.cpp

int UIWidget::WidgetLocalize()
{
    return 1;
}


// ---- src/UIManager/LoadRequestedMaterials.cpp


void UIManager::LoadRequestedMaterials()
{
    LpMaterialStringTable->LoadRequested();
    LpTextureMapStringTable->LoadRequested();
}


// ---- src/sinit/UIManager.cpp
// Static initializer of UIManager.cpp: only the axis vectors from the shared header.

