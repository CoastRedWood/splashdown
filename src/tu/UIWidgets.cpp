// UIWidgets.cpp  @ 0x003193B0 (text 0x3193B0-0x31D4F0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 2 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's UIWidgets.cpp as one translation unit (an unanchored TU in
// UIPage.cpp's tu_map row): UIWidget and the widget classes (Soultree,
// SoultreeLightweight, SoultreeCharacter, OverlayText, StaticUserRotate,
// MovementControl, MovementGroup, StaticUserRotateDL, Icon), then the static
// initializer. Its .rodata runs @909 (after UIWidgetEventHandler.cpp's RTTI
// block) .. the UIWidgetSoultree RTTI, one run of labels. Built with
// -sdatathreshold 2: ParseMultiLine's "@" delimiter array is in .sdata.



#include "UIWidgetEventHandler/UIWidgetEventHandler.h"
#include "PSXRenderTarget/PSXRenderTarget.h"
#include "GameObject/GameObject.h"
#include "UIWidget/UIWidget.h"
#include "UIPage/UIPage.h"
#include "UIManager/UIManager.h"
#include "ParameterBlock/ParameterBlock.h"
#include "d3dtypes.h"
#include "UIWidgetSoultree/UIWidgetSoultree.h"
#include "RenderableObject/RenderableObject.h"
#include "PSXRenderableObject/PSXRenderableObject.h"
#include "SoultreeObject/SoultreeObject.h"
#include "UIWidgetSoultreeCharacter/UIWidgetSoultreeCharacter.h"
#include "D3DIMSoultreeCharacter/D3DIMSoultreeCharacter.h"
#include "UIWidgetOverlayText/UIWidgetOverlayText.h"
#include "ScreenTextFont/ScreenTextFont.h"
#include "ScreenTextManager/ScreenTextManager.h"
#include "TextOverlay/TextOverlay.h"
#include "UIWidgetStaticUserRotate/UIWidgetStaticUserRotate.h"
#include "UIWidgetMovementControl/UIWidgetMovementControl.h"
#include "UIGroup/UIGroup.h"
#include "UIWidgetStaticUserRotateDL/UIWidgetStaticUserRotateDL.h"
#include "SearchableObject/SearchableObject.h"
#include "SearchPlacement/SearchPlacement.h"
#include "RenderableObjectData/RenderableObjectData.h"
#include "_RECT/_RECT.h"
#include "PSXVideoCard/PSXVideoCard.h"
#include "ScreenIconManager/ScreenIconManager.h"
#include "UIWidgetSoultreeLightweight/UIWidgetSoultreeLightweight.h"
#include "UIWidgetMovementGroup/UIWidgetMovementGroup.h"
#include "UIWidgetIcon/UIWidgetIcon.h"
#include "ScreenIcon/ScreenIcon.h"
#include "ProcessNode/ProcessNode.h"
#include "Character/Character.h"
#include "sinit/AxisVectors.h"

// ---- src/UIWidget/ctor.cpp

UIWidget::UIWidget(UIManager *manager, int enabled)
    : GameObject(enabled)
{
    m_manager = manager;
    m_selected = 0;
    m_id = 0;
    m_enableOnSelect = 0;
    m_disableOnSelect = 0;
    m_enableOnDeSelect = 0;
    m_disableOnDeSelect = 0;
    m_wantsJoystickInput[0] = 0;
    m_wantsJoystickInput[1] = 0;
    m_x54 = 0;
}


// ---- src/UIWidget/dtor.cpp


UIWidget::~UIWidget()
{
}


// ---- src/UIWidget/Create.cpp


UIWidget *UIWidget::Create(ParameterBlock *params)
{
    params->GetParameter("EnableOnSelect", 0, &m_enableOnSelect);
    params->GetParameter("DisableOnSelect", 0, &m_disableOnSelect);
    params->GetParameter("EnableOnDeSelect", 0, &m_enableOnDeSelect);
    params->GetParameter("DisableOnDeSelect", 0, &m_disableOnDeSelect);
    return this;
}


// ---- src/UIWidget/WidgetSelect.cpp

void UIWidget::WidgetSelect()
{
    m_selected = 1;
    if (!m_enableOnSelect || !m_disableOnSelect) {
        if (m_enableOnSelect)
            EnableAll();
        if (m_disableOnSelect)
            DisableAll();
    }
}


// ---- src/UIWidget/WidgetDeSelect.cpp


// UIWidget with the fields WidgetDeSelect uses (UIWidget.h stops at +0x40).

void UIWidget::WidgetDeSelect()
{
    m_selected = 0;
    if (!m_enableOnDeSelect || !m_disableOnDeSelect) {
        if (m_enableOnDeSelect)
            EnableAll();
        if (m_disableOnDeSelect)
            DisableAll();
    }
}


// ---- src/UIWidget/WidgetKeepEnabled.cpp

void UIWidget::WidgetKeepEnabled()
{
    m_selected = 0;
}


// ---- src/UIWidget/Initialize.cpp

void UIWidget::Initialize()
{
    EnableAll();
}


// ---- src/UIWidget/WidgetKeyDownHandler.cpp

void UIWidget::WidgetKeyDownHandler(ProcessNode *, KeyInfo *, int, int)
{
}


// ---- src/UIWidget/TurnOnOff.cpp


void UIWidget::TurnOnOff(int on)
{
    if (on) {
        m_selected = 1;
        EnableAll();
    } else {
        m_selected = 0;
        DisableAll();
    }
}


// ---- src/UIWidgetSoultree/ctor.cpp


UIWidgetSoultree::UIWidgetSoultree(UIManager *manager, int id)
    : UIWidget(manager, id)
{
    m_object = 0;
    m_position.x = 1.0f;
    m_position.y = 1.0f;
    m_position.z = 1.0f;
    m_position.w = 1.0f;
    m_forward.x = 0.0f;
    m_forward.y = 0.0f;
    m_forward.z = 0.0f;
    m_forward.w = 1.0f;
    m_up.x = 0.0f;
    m_up.y = 0.0f;
    m_up.z = 0.0f;
    m_up.w = 1.0f;
    m_rotationPeriod = 0;
    m_angle = 0;
    // Retail clears these twice.
    m_rotationPeriod = 0;
    m_angle = 0;
}


// ---- src/UIWidgetSoultree/dtor.cpp

UIWidgetSoultree::~UIWidgetSoultree()
{
}


// ---- src/UIWidgetSoultree/Create.cpp

void PSXprintf(const char *fmt, ...);


extern "C" char *strcpy(char *dst, const char *src);


UIWidgetSoultree *UIWidgetSoultree::Create(int id, char *name, ParameterBlock *block)
{
    m_id = id;
    UIWidget::Create(block);
    _D3DVECTOR defaultPosition(1.0f, 1.0f, 1.0f);
    block->GetParameter("Position", &defaultPosition, &m_position);
    _D3DVECTOR defaultLook(0.0f, 0.0f, 1.0f);
    block->GetParameter("LookVector", &defaultLook, &m_forward);
    _D3DVECTOR defaultUp(0.0f, 0.0f, 1.0f);
    block->GetParameter("UpVector", &defaultUp, &m_up);
    block->GetParameter("AutoRotate", 0.0f, &m_rotationPeriod);
    if (!block->GetParameter("SelectedTexture", m_selectedTexture, 100))
        strcpy(m_selectedTexture, "NONE");
    if (!block->GetParameter("DeSelectedTexture", m_deselectedTexture, 100))
        strcpy(m_deselectedTexture, "NONE");
    m_object = new PSXRenderableObject(1);
    m_object->Create(name, 0, 0);
    if (!AddChild(m_object, -1))
        PSXprintf("UI static object %s not created\n", name);
    m_manager->LoadRequestedMaterials();
    return this;
}


// ---- src/UIWidgetSoultree/Tick.cpp

int UIWidgetSoultree::Tick(float dt)
{
    Rotate(dt);
    return GameObject::Tick(dt);
}


// ---- src/UIWidgetSoultree/Initialize.cpp

void UIWidgetSoultree::Initialize()
{
    UIWidget::Initialize();
    m_object->SetPosition(m_position);
    m_object->SetOrientation(&m_forward, &m_up, 0, 1, 0);
}


// ---- src/UIWidgetSoultree/WidgetSelect.cpp

void UIWidgetSoultree::WidgetSelect()
{
    UIWidget::WidgetSelect();
    SelectedTextureChange();
}


// ---- src/UIWidgetSoultree/WidgetDeSelect.cpp

void UIWidgetSoultree::WidgetDeSelect()
{
    UIWidget::WidgetDeSelect();
    DeSelectedTextureChange();
}


// ---- src/UIWidgetSoultree/WidgetKeepEnabled.cpp

void UIWidgetSoultree::WidgetKeepEnabled()
{
    UIWidget::WidgetKeepEnabled();
    DeSelectedTextureChange();
}


// ---- src/UIWidgetSoultree/Rotate.cpp


void UIWidgetSoultree::Rotate(float dt)
{
    if (m_rotationPeriod == 0.0f)
        return;
    m_angle += dt / (3.14159274f * (2.0f * m_rotationPeriod));
    if (m_angle > 6.28318548f)
        m_angle -= 6.28318548f;
    // Retail tests m_object, then makes the same call on both paths.
    if (m_object)
        m_object->SetRotation(_D3DVECTOR(0.0f, 0.0f, 1.0f), m_angle);
    else
        m_object->SetRotation(_D3DVECTOR(0.0f, 0.0f, 1.0f), m_angle);
}


// ---- src/UIWidgetSoultree/SelectedTextureChange.cpp
extern "C" int strcmp(const char *a, const char *b);


int UIWidgetSoultree::SelectedTextureChange()
{
    return strcmp(m_selectedTexture, "NONE");
}


// ---- src/UIWidgetSoultree/DeSelectedTextureChange.cpp
extern "C" int strcmp(const char *a, const char *b);


int UIWidgetSoultree::DeSelectedTextureChange()
{
    return strcmp(m_deselectedTexture, "NONE");
}


// ---- src/UIWidgetSoultreeLightweight/ctor.cpp


UIWidgetSoultreeLightweight::UIWidgetSoultreeLightweight(UIManager *manager, int id)
    : UIWidget(manager, id)
{
    m_object = 0;
    m_position.x = 1.0f;
    m_position.y = 1.0f;
    m_position.z = 1.0f;
    m_position.w = 1.0f;
    m_forward.x = 0.0f;
    m_forward.y = 0.0f;
    m_forward.z = 0.0f;
    m_forward.w = 1.0f;
    m_up.x = 0.0f;
    m_up.y = 0.0f;
    m_up.z = 0.0f;
    m_up.w = 1.0f;
    m_rotationPeriod = 0;
    m_angle = 0;
    m_rotationPeriod = 0;   // (retail stores it twice)
}


// ---- src/UIWidgetSoultreeLightweight/dtor.cpp


UIWidgetSoultreeLightweight::~UIWidgetSoultreeLightweight()
{
}


// ---- src/UIWidgetSoultreeLightweight/Create.cpp

void PSXprintf(const char *fmt, ...);


extern "C" char *strcpy(char *dst, const char *src);


UIWidgetSoultreeLightweight *UIWidgetSoultreeLightweight::Create(int id, char *name, ParameterBlock *block)
{
    m_id = id;
    UIWidget::Create(block);
    _D3DVECTOR defaultPosition(1.0f, 1.0f, 1.0f);
    block->GetParameter("Position", &defaultPosition, &m_position);
    _D3DVECTOR defaultLook(0.0f, 0.0f, 1.0f);
    block->GetParameter("LookVector", &defaultLook, &m_forward);
    _D3DVECTOR defaultUp(0.0f, 0.0f, 1.0f);
    block->GetParameter("UpVector", &defaultUp, &m_up);
    block->GetParameter("AutoRotate", 0.0f, &m_rotationPeriod);
    if (!block->GetParameter("SelectedTexture", m_selectedTexture, 100))
        strcpy(m_selectedTexture, "NONE");
    if (!block->GetParameter("DeSelectedTexture", m_deselectedTexture, 100))
        strcpy(m_deselectedTexture, "NONE");
    m_object = new PSXRenderableObject(1);
    m_object->Create(name, 0, 0);
    if (!AddChild(m_object, 0))
        PSXprintf("UI static object %s not created\n", name);
    m_manager->LoadRequestedMaterials();
    return this;
}


// ---- src/UIWidgetSoultreeLightweight/Tick.cpp

int UIWidgetSoultreeLightweight::Tick(float dt)
{
    Rotate(dt);
    return GameObject::Tick(dt);
}


// ---- src/UIWidgetSoultreeLightweight/Initialize.cpp


void UIWidgetSoultreeLightweight::Initialize()
{
    UIWidget::Initialize();
    m_object->SetPosition(m_position);
    m_object->SetOrientation(&m_forward, &m_up, 1, 0, 0);
}


// ---- src/UIWidgetSoultreeLightweight/WidgetSelect.cpp

void UIWidgetSoultreeLightweight::WidgetSelect()
{
    UIWidget::WidgetSelect();
    SelectedTextureChange();
}


// ---- src/UIWidgetSoultreeLightweight/WidgetDeSelect.cpp

void UIWidgetSoultreeLightweight::WidgetDeSelect()
{
    UIWidget::WidgetDeSelect();
    DeSelectedTextureChange();
}


// ---- src/UIWidgetSoultreeLightweight/WidgetKeepEnabled.cpp

void UIWidgetSoultreeLightweight::WidgetKeepEnabled()
{
    UIWidget::WidgetKeepEnabled();
    DeSelectedTextureChange();
}


// ---- src/UIWidgetSoultreeLightweight/Rotate.cpp


void UIWidgetSoultreeLightweight::Rotate(float dt)
{
    if (m_rotationPeriod != 0.0f) {
        m_angle += dt / (3.14159274f * (2.0f * m_rotationPeriod));
        if (m_angle > 6.28318548f)
            m_angle -= 6.28318548f;
        if (m_object)
            m_object->SetRotation(_D3DVECTOR(0.0f, 0.0f, 1.0f), m_angle);
    }
}


// ---- src/UIWidgetSoultreeLightweight/SelectedTextureChange.cpp
extern "C" int strcmp(const char *a, const char *b);


// Nonzero when a selected-state texture was named ("NONE" means none).
int UIWidgetSoultreeLightweight::SelectedTextureChange()
{
    return strcmp(m_selectedTexture, "NONE");
}


// ---- src/UIWidgetSoultreeLightweight/DeSelectedTextureChange.cpp
extern "C" int strcmp(const char *a, const char *b);


// Nonzero when a deselected-state texture was named ("NONE" means none).
int UIWidgetSoultreeLightweight::DeSelectedTextureChange()
{
    return strcmp(m_deselectedTexture, "NONE");
}


// ---- src/UIWidgetSoultreeCharacter/ctor.cpp

UIWidgetSoultreeCharacter::UIWidgetSoultreeCharacter(UIManager *manager, int id)
    : UIWidget(manager, id)
{
    m_character = 0;
    m_position.x = 1.0f;
    m_position.y = 1.0f;
    m_position.z = 1.0f;
    m_position.w = 1.0f;
    m_forward.x = 0.0f;
    m_forward.y = 0.0f;
    m_forward.z = 0.0f;
    m_forward.w = 1.0f;
    m_up.x = 0.0f;
    m_up.y = 0.0f;
    m_up.z = 0.0f;
    m_up.w = 1.0f;
    m_numActionNames = 0;
    m_actionNames = 0;
}


// ---- src/UIWidgetSoultreeCharacter/dtor.cpp

UIWidgetSoultreeCharacter::~UIWidgetSoultreeCharacter()
{
    if (m_numActionNames > 0) {
        for (int i = 0; i < m_numActionNames; i++)
            free(m_actionNames[i]);
        free(m_actionNames);
    }
}


// ---- src/UIWidgetSoultreeCharacter/Create.cpp
void PSXprintf(const char *fmt, ...);
extern "C" int sprintf(char *buf, const char *fmt, ...);
extern "C" char *strcpy(char *dst, const char *src);
extern "C" void *malloc(unsigned int size);


// Reads motion parameter `param` and looks its name up in the motion file.
#define READ_MOTION(param, action)                                      \
    strcpy(action, "NONE");                                             \
    if (params->GetParameter(param, 0, &index) && index > 0) {          \
        sprintf(key, "Motion %d", index);                               \
        motions->ReadParameterBlock(key);                               \
        motions->GetParameter("NAME", action, 0x100);                   \
    }

UIWidgetSoultreeCharacter *UIWidgetSoultreeCharacter::Create(int id, char *name, ParameterBlock *params)
{
    char key[0x100];
    int index;
    UIWidget::Create(params);
    ParameterBlock *motions = new ParameterBlock;
    params->GetParameter("Position", &_D3DVECTOR(1.0f, 1.0f, 1.0f), &m_position);
    params->GetParameter("LookVector", &_D3DVECTOR(0.0f, 0.0f, 1.0f), &m_forward);
    params->GetParameter("UpVector", &_D3DVECTOR(0.0f, 0.0f, 1.0f), &m_up);
    motions->OpenFile(name, 0);
    READ_MOTION("SelectionMotion", m_selectAction)
    READ_MOTION("DeSelectionMotion", m_deselectAction)
    READ_MOTION("InitialMotion", m_initialAction)
    params->GetParameter("NumberOfRandomMotions", 0, &m_numActionNames);
    if (m_numActionNames > 0) {
        m_actionNames = (char **)malloc(m_numActionNames * sizeof(char *));
        for (int i = 0; i < m_numActionNames; i++) {
            m_actionNames[i] = (char *)malloc(0x100);
            sprintf(key, "RandomMotion%d", i + 1);
            if (params->GetParameter(key, 0, &index)) {
                sprintf(key, "Motion %d", index);
                motions->ReadParameterBlock(key);
                motions->GetParameter("NAME", m_actionNames[i], 0x100);
            }
        }
    }
    delete motions;
    m_id = id;
    m_character = new D3DIMSoultreeCharacter(1);
    m_character->Create(name, 0, 1, 1);
    if (!AddChild(m_character, -1))
        PSXprintf("UI dynamic object %s not created\n", name);
    m_manager->LoadRequestedMaterials();
    return this;
}


// ---- src/UIWidgetSoultreeCharacter/Tick.cpp

int UIWidgetSoultreeCharacter::Tick(float dt)
{
    m_character->TickBlendedFrame(dt, 0, 0);
    return GameObject::Tick(dt);
}


// ---- src/UIWidgetSoultreeCharacter/Initialize.cpp

void UIWidgetSoultreeCharacter::Initialize()
{
    UIWidget::Initialize();
    m_character->m_root->SetPosition(m_position);
    m_character->m_root->SetOrientation(&m_forward, &m_up, 1, 0, 0);
    if (strcmp(m_initialAction, "NONE")) {
        m_character->PerformActionByName(m_initialAction);
        m_character->TickBlendedFrame(0.001f, 0, 0);
    }
}


// ---- src/UIWidgetSoultreeCharacter/WidgetSelect.cpp

void UIWidgetSoultreeCharacter::WidgetSelect()
{
    UIWidget::WidgetSelect();
    if (strcmp(m_selectAction, "NONE")) {
        m_character->PerformActionByName(m_selectAction);
        m_character->TickBlendedFrame(0.001f, 0, 0);
    }
}


// ---- src/UIWidgetSoultreeCharacter/WidgetDeSelect.cpp

void UIWidgetSoultreeCharacter::WidgetDeSelect()
{
    UIWidget::WidgetDeSelect();
    if (strcmp(m_deselectAction, "NONE")) {
        m_character->PerformActionByName(m_deselectAction);
        m_character->TickBlendedFrame(0.001f, 0, 0);
    }
}


// ---- src/UIWidgetSoultreeCharacter/WidgetKeepEnabled.cpp

// Retail plays the deselect action here too (+0x290), not the select one.
void UIWidgetSoultreeCharacter::WidgetKeepEnabled()
{
    UIWidget::WidgetKeepEnabled();
    if (strcmp(m_deselectAction, "NONE")) {
        m_character->PerformActionByName(m_deselectAction);
        m_character->TickBlendedFrame(0.001f, 0, 0);
    }
}


// ---- src/UIWidgetOverlayText/ctor.cpp

UIWidgetOverlayText::UIWidgetOverlayText(UIManager *manager, int enabled)
    : UIWidget(manager, enabled)
{
    m_textManager = 0;
    m_overlays = 0;
    m_x60 = 0;
    m_numOverlays = 0;
    m_color = 0;
    m_keepVisible = 0;
    m_selectedColor = 0;
    m_lineHeight = 0;
    m_x10C = 0;
}


// ---- src/UIWidgetOverlayText/dtor.cpp

UIWidgetOverlayText::~UIWidgetOverlayText()
{
    if (m_overlays)
        free(m_overlays);
}


// ---- src/UIWidgetOverlayText/Create.cpp
// The DisplayText calls load every 0.0f and -1.0f argument before the 1.0fs
// (retail copies $f12 into $f18/$f19 before $f14 into $f17): those constants
// carry mwcc's uninitialised "has call" flag. The 0.0fs are copies of the
// 0.0f in d3dtypes.h's _D3DVECTOR body, flagged by the shim in d3dtypes.h;
// the -1.0fs by the heap image below. The colour packs match with a signed
// (0xFF << 24) alpha (was 0xFF000000).



#pragma fast_fptosi on

extern "C" void *malloc(unsigned int size);
extern "C" int strcmp(const char *a, const char *b);
extern "C" unsigned int strlen(const char *s);
extern "C" int sprintf(char *buf, const char *fmt, ...);


// Session shim: a 0x30000-byte heap image
// (new-first-block form) that writes the uninitialised "has call" byte of every constant node in
// Create's parse epoch, with retail's state: the four -1.0f nodes set. Unused inline; image object in .mwccshim.
#define MHICreate_F(a) char p##a[7]; char f##a;
#define MHICreate_F8(a) MHICreate_F(a##0) MHICreate_F(a##1) MHICreate_F(a##2) MHICreate_F(a##3) MHICreate_F(a##4) MHICreate_F(a##5) MHICreate_F(a##6) MHICreate_F(a##7)
#define MHICreate_F64(a) MHICreate_F8(a##0) MHICreate_F8(a##1) MHICreate_F8(a##2) MHICreate_F8(a##3) MHICreate_F8(a##4) MHICreate_F8(a##5) MHICreate_F8(a##6) MHICreate_F8(a##7)
#define MHICreate_F512(a) MHICreate_F64(a##0) MHICreate_F64(a##1) MHICreate_F64(a##2) MHICreate_F64(a##3) MHICreate_F64(a##4) MHICreate_F64(a##5) MHICreate_F64(a##6) MHICreate_F64(a##7)
#define MHICreate_Z1 {0}, 0,
#define MHICreate_Z8 MHICreate_Z1 MHICreate_Z1 MHICreate_Z1 MHICreate_Z1 MHICreate_Z1 MHICreate_Z1 MHICreate_Z1 MHICreate_Z1
#define MHICreate_Z64 MHICreate_Z8 MHICreate_Z8 MHICreate_Z8 MHICreate_Z8 MHICreate_Z8 MHICreate_Z8 MHICreate_Z8 MHICreate_Z8
#define MHICreate_Z512 MHICreate_Z64 MHICreate_Z64 MHICreate_Z64 MHICreate_Z64 MHICreate_Z64 MHICreate_Z64 MHICreate_Z64 MHICreate_Z64
struct MwccHeapImageCreate {
    char lead[0x4996];
    MHICreate_F512(n0_) MHICreate_F512(n1_) MHICreate_F512(n2_) MHICreate_F512(n3_) MHICreate_F512(n4_) MHICreate_F512(n5_)
    MHICreate_F512(n6_) MHICreate_F512(n7_) MHICreate_F512(n8_) MHICreate_F512(n9_) MHICreate_F512(n10_) MHICreate_F512(n11_)
    MHICreate_F512(n12_) MHICreate_F512(n13_) MHICreate_F512(n14_) MHICreate_F512(n15_) MHICreate_F512(n16_) MHICreate_F64(n17_)
    MHICreate_F64(n18_) MHICreate_F8(n19_) MHICreate_F8(n20_)
    char tail[0x1A1E3];
};
#pragma section mwccshim begin
inline void MwccHeapShimCreate()
{
    const MwccHeapImageCreate image = {
    {0},
    MHICreate_Z512 MHICreate_Z512 MHICreate_Z512 MHICreate_Z512 MHICreate_Z512 MHICreate_Z512 MHICreate_Z512 MHICreate_Z512
    MHICreate_Z512 MHICreate_Z64 MHICreate_Z64 MHICreate_Z64 MHICreate_Z8 MHICreate_Z1 MHICreate_Z1 MHICreate_Z1
    MHICreate_Z1 {0}, 1, MHICreate_Z8 MHICreate_Z8 MHICreate_Z8 MHICreate_Z8 MHICreate_Z1 MHICreate_Z1
    MHICreate_Z1 MHICreate_Z1 MHICreate_Z1 MHICreate_Z1 MHICreate_Z1 {0}, 1, MHICreate_Z512 MHICreate_Z512
    MHICreate_Z512 MHICreate_Z512 MHICreate_Z512 MHICreate_Z64 MHICreate_Z64 MHICreate_Z64 MHICreate_Z64 MHICreate_Z8
    MHICreate_Z8 MHICreate_Z8 MHICreate_Z8 MHICreate_Z1 MHICreate_Z1 MHICreate_Z1 MHICreate_Z1 MHICreate_Z1
    {0}, 1, MHICreate_Z8 MHICreate_Z8 MHICreate_Z8 MHICreate_Z8 MHICreate_Z1 MHICreate_Z1 MHICreate_Z1
    MHICreate_Z1 MHICreate_Z1 MHICreate_Z1 MHICreate_Z1 {0}, 1, MHICreate_Z1 MHICreate_Z1 MHICreate_Z1
    MHICreate_Z1 MHICreate_Z1 MHICreate_Z1 MHICreate_Z1 {0}, 108, MHICreate_Z8 MHICreate_Z8 MHICreate_Z1
    MHICreate_Z1 MHICreate_Z1 MHICreate_Z1 MHICreate_Z1 MHICreate_Z1 MHICreate_Z1 {0}, 108, MHICreate_Z512
    MHICreate_Z512 MHICreate_Z8 MHICreate_Z8 MHICreate_Z8 MHICreate_Z8 MHICreate_Z8 MHICreate_Z1 MHICreate_Z1
    MHICreate_Z1 MHICreate_Z1 MHICreate_Z1
    };
}
#pragma section mwccshim end
UIWidgetOverlayText *UIWidgetOverlayText::Create(int id, ParameterBlock *params, ScreenTextManager *manager)
{
    int alphaEnabled;
    int alpha;
    int depthEnabled;
    float depth;
    UIWidget::Create(params);
    _D3DVECTOR color;
    _D3DVECTOR highlight;
    _D3DVECTOR disabled;
    char text[0x190];
    char name[0x20];
    params->GetParameter("FontName", m_fontName, 0x80);
    params->GetParameter("XPosition", 0, &m_x);
    params->GetParameter("YPosition", 0, &m_y);
    int multiLine = params->GetParameter("NumberOfLines", 1, &m_numOverlays);
    params->GetParameter("Justification", "Left", name, 0x14);
    params->GetParameter("AlphaEnabled", 0, &alphaEnabled);
    params->GetParameter("Alpha", 0xFF, &alpha);
    params->GetParameter("DepthEnabled", 0, &depthEnabled);
    params->GetParameter("Depth", 0.0f, &depth);
    params->GetParameter("Color", &_D3DVECTOR(40.0f, 40.0f, 40.0f), &color);
    m_color = (0xFF << 24) | (int)color.x << 16 | (int)color.y << 8 | (int)color.z;
    if (params->GetParameter("HighlightColor", &_D3DVECTOR(255.0f, 255.0f, 255.0f), &highlight))
        m_keepVisible = 1;
    m_selectedColor = (0xFF << 24) | (int)highlight.x << 16 | (int)highlight.y << 8 | (int)highlight.z;
    if (params->GetParameter("DisabledColor", &_D3DVECTOR(128.0f, 128.0f, 128.0f), &disabled))
        m_visibleWhenDisabled = 1;
    m_disabledColor = (0xFF << 24) | (int)disabled.x << 16 | (int)disabled.y << 8 | (int)disabled.z;
    params->GetParameter("Height", 0, &m_lineHeight);
    m_textManager = manager;
    m_id = id;
    if (!strcmp(name, "Right"))
        m_justification = ScreenTextManager::RightJustification;
    else if (!strcmp(name, "CenterBoth"))
        m_justification = ScreenTextManager::CenterBothJustification;
    else if (!strcmp(name, "CenterVertical"))
        m_justification = ScreenTextManager::CenterVerticalJustification;
    else if (!strcmp(name, "CenterHorizontal"))
        m_justification = ScreenTextManager::CenterHorizontalJustification;
    else
        m_justification = ScreenTextManager::LeftJustification;
    char *fonts[1] = { m_fontName };
    if (multiLine) {
        m_overlays = (TextOverlay **)malloc(m_numOverlays * sizeof(TextOverlay *));
        for (int i = 0; i < m_numOverlays; i++) {
            sprintf(name, "Line%d", i + 1);
            params->GetParameter(name, text, 0x190);
            m_overlays[i] = new TextOverlay(1, strlen(text));
            m_overlays[i]->Create(manager, fonts, 0, 1);
            AddChild(m_overlays[i], -1);
            m_overlays[i]->DisplayText(0.0f, -1.0f, 0xFF, 1.0f, 0.0f, 0, -1.0f, 0xFF, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
            m_overlays[i]->SetAlignment(m_justification);
            m_overlays[i]->SetColor(m_color);
            m_overlays[i]->SetText(text);
            m_overlays[i]->SetPosition(m_x, m_y + i * m_overlays[i]->m_font->GetHeight(), 0.0f);
            m_overlays[i]->SetAlphaEnabled(alphaEnabled);
            m_overlays[i]->SetAlpha(alpha, 0.0f);
            m_overlays[i]->SetDepthEnabled(depthEnabled);
            m_overlays[i]->SetDepth(depth);
        }
    } else {
    params->GetParameter("Text", text, 0x190);
    if (m_lineHeight) {
        ParseMultiLine(text, manager);
    } else {
    m_numOverlays = 1;
    m_overlays = (TextOverlay **)malloc(m_numOverlays * sizeof(TextOverlay *));
    m_overlays[0] = new TextOverlay(1, strlen(text));
    m_overlays[0]->Create(manager, fonts, 0, 1);
    AddChild(m_overlays[0], -1);
    m_overlays[0]->DisplayText(0.0f, -1.0f, 0xFF, 1.0f, 0.0f, 0, -1.0f, 0xFF, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    m_overlays[0]->SetText(text);
    m_overlays[0]->SetAlignment(m_justification);
    m_overlays[0]->SetColor(m_color);
    m_overlays[0]->SetPosition(m_x, m_y, 0.0f);
    m_overlays[0]->SetAlphaEnabled(alphaEnabled);
    m_overlays[0]->SetAlpha(alpha, 0.0f);
    m_overlays[0]->SetDepthEnabled(depthEnabled);
    m_overlays[0]->SetDepth(depth);
    }
    }
    return this;
}


// ---- src/UIWidgetOverlayText/ParseMultiLine.cpp
#pragma fast_fptosi on

extern "C" void *realloc(void *p, unsigned int size);
extern "C" unsigned int strlen(const char *s);
extern "C" char *strtok(char *s, const char *delim);


// Session shim: a 0x30000-byte heap image
// (new-first-block form) that writes the uninitialised "has call" byte of every constant node in
// ParseMultiLine's parse epoch, with retail's state: SetPosition's 0.0f and DisplayText's second -1.0f and 1.0f set. Unused inline; image object in .mwccshim.
#define MHIPML_F(a) char p##a[7]; char f##a;
#define MHIPML_F8(a) MHIPML_F(a##0) MHIPML_F(a##1) MHIPML_F(a##2) MHIPML_F(a##3) MHIPML_F(a##4) MHIPML_F(a##5) MHIPML_F(a##6) MHIPML_F(a##7)
#define MHIPML_F64(a) MHIPML_F8(a##0) MHIPML_F8(a##1) MHIPML_F8(a##2) MHIPML_F8(a##3) MHIPML_F8(a##4) MHIPML_F8(a##5) MHIPML_F8(a##6) MHIPML_F8(a##7)
#define MHIPML_F512(a) MHIPML_F64(a##0) MHIPML_F64(a##1) MHIPML_F64(a##2) MHIPML_F64(a##3) MHIPML_F64(a##4) MHIPML_F64(a##5) MHIPML_F64(a##6) MHIPML_F64(a##7)
#define MHIPML_Z1 {0}, 0,
#define MHIPML_Z8 MHIPML_Z1 MHIPML_Z1 MHIPML_Z1 MHIPML_Z1 MHIPML_Z1 MHIPML_Z1 MHIPML_Z1 MHIPML_Z1
#define MHIPML_Z64 MHIPML_Z8 MHIPML_Z8 MHIPML_Z8 MHIPML_Z8 MHIPML_Z8 MHIPML_Z8 MHIPML_Z8 MHIPML_Z8
#define MHIPML_Z512 MHIPML_Z64 MHIPML_Z64 MHIPML_Z64 MHIPML_Z64 MHIPML_Z64 MHIPML_Z64 MHIPML_Z64 MHIPML_Z64
struct MwccHeapImagePML {
    char lead[0x4996];
    MHIPML_F512(n0_) MHIPML_F64(n1_) MHIPML_F64(n2_) MHIPML_F64(n3_) MHIPML_F8(n4_) MHIPML_F8(n5_)
    char tail[0x29FE3];
};
#pragma section mwccshim begin
inline void MwccHeapShimPML()
{
    const MwccHeapImagePML image = {
    {0},
    MHIPML_Z8 {0}, 1, MHIPML_Z512 MHIPML_Z64 MHIPML_Z8 MHIPML_Z8 MHIPML_Z1 MHIPML_Z1
    MHIPML_Z1 MHIPML_Z1 {0}, 1, MHIPML_Z8 MHIPML_Z1 MHIPML_Z1 MHIPML_Z1 MHIPML_Z1
    MHIPML_Z1 MHIPML_Z1 MHIPML_Z1 {0}, 1, MHIPML_Z1 MHIPML_Z1 MHIPML_Z1 MHIPML_Z1
    MHIPML_Z1 MHIPML_Z1 MHIPML_Z1 {0}, 108, MHIPML_Z8 MHIPML_Z8 MHIPML_Z1 MHIPML_Z1
    MHIPML_Z1 MHIPML_Z1 MHIPML_Z1 MHIPML_Z1 MHIPML_Z1 {0}, 108, MHIPML_Z64 MHIPML_Z1
    MHIPML_Z1
    };
}
#pragma section mwccshim end
void UIWidgetOverlayText::ParseMultiLine(char *text, ScreenTextManager *manager)
{
    char *fontNames[1] = { m_fontName };
    char delim[] = "@";
    char *token;
    int line = -1;
    m_overlays = 0;
    m_numOverlays = 0;
    token = strtok(text, delim);
    while (token) {
        m_numOverlays++;
        line++;
        m_overlays = (TextOverlay **)realloc(m_overlays, m_numOverlays * sizeof(TextOverlay *));
        m_overlays[line] = new TextOverlay(1, strlen(token));
        m_overlays[line]->Create(manager, fontNames, 0, 1);
        AddChild(m_overlays[line], -1);
        m_overlays[line]->SetText(token);
        m_overlays[line]->SetAlignment(m_justification);
        m_overlays[line]->SetColor(m_color);
        if (m_lineHeight)
            m_overlays[line]->SetPosition(m_x, m_y + m_lineHeight * line, 0.0f);
        else
            m_overlays[line]->SetPosition(m_x, m_y + line * m_overlays[line]->m_font->GetHeight(), 0.0f);
        // One TextOverlay per '@'-separated line.
        m_overlays[line]->DisplayText(0.0f, -1.0f, 0xFF, 1.0f, 0.0f, 0, -1.0f, 0xFF, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
        token = strtok(0, delim);
    }
}


// ---- src/UIWidgetOverlayText/WidgetSelect.cpp

// Session shim: a 0x30000-byte heap image
// (new-first-block form) that writes the uninitialised "has call" byte of every constant node in
// WidgetSelect's parse epoch, with retail's state: the 1.0f nodes (both -1.0fs and both 1.0fs) and the last 0.0f set. Unused inline; image object in .mwccshim.
#define MHIWS_F(a) char p##a[7]; char f##a;
#define MHIWS_F8(a) MHIWS_F(a##0) MHIWS_F(a##1) MHIWS_F(a##2) MHIWS_F(a##3) MHIWS_F(a##4) MHIWS_F(a##5) MHIWS_F(a##6) MHIWS_F(a##7)
#define MHIWS_F64(a) MHIWS_F8(a##0) MHIWS_F8(a##1) MHIWS_F8(a##2) MHIWS_F8(a##3) MHIWS_F8(a##4) MHIWS_F8(a##5) MHIWS_F8(a##6) MHIWS_F8(a##7)
#define MHIWS_F512(a) MHIWS_F64(a##0) MHIWS_F64(a##1) MHIWS_F64(a##2) MHIWS_F64(a##3) MHIWS_F64(a##4) MHIWS_F64(a##5) MHIWS_F64(a##6) MHIWS_F64(a##7)
#define MHIWS_Z1 {0}, 0,
#define MHIWS_Z8 MHIWS_Z1 MHIWS_Z1 MHIWS_Z1 MHIWS_Z1 MHIWS_Z1 MHIWS_Z1 MHIWS_Z1 MHIWS_Z1
#define MHIWS_Z64 MHIWS_Z8 MHIWS_Z8 MHIWS_Z8 MHIWS_Z8 MHIWS_Z8 MHIWS_Z8 MHIWS_Z8 MHIWS_Z8
#define MHIWS_Z512 MHIWS_Z64 MHIWS_Z64 MHIWS_Z64 MHIWS_Z64 MHIWS_Z64 MHIWS_Z64 MHIWS_Z64 MHIWS_Z64
struct MwccHeapImageWS {
    char lead[0x1616];
    MHIWS_F64(n0_) MHIWS_F64(n1_)
    char tail[0x2E5E3];
};
#pragma section mwccshim begin
inline void MwccHeapShimWS()
{
    const MwccHeapImageWS image = {
    {0},
    MHIWS_Z8 MHIWS_Z8 MHIWS_Z8 MHIWS_Z8 MHIWS_Z1 MHIWS_Z1 MHIWS_Z1 {0}, 1,
    MHIWS_Z8 MHIWS_Z1 MHIWS_Z1 MHIWS_Z1 MHIWS_Z1 MHIWS_Z1 MHIWS_Z1 MHIWS_Z1
    {0}, 1, MHIWS_Z8 MHIWS_Z8 MHIWS_Z1 MHIWS_Z1 MHIWS_Z1 MHIWS_Z1 MHIWS_Z1
    MHIWS_Z1 MHIWS_Z1 {0}, 1, MHIWS_Z8 MHIWS_Z1 MHIWS_Z1 MHIWS_Z1 MHIWS_Z1
    MHIWS_Z1 MHIWS_Z1 MHIWS_Z1 {0}, 1, MHIWS_Z8 MHIWS_Z8 MHIWS_Z8 MHIWS_Z1
    MHIWS_Z1 MHIWS_Z1 MHIWS_Z1 MHIWS_Z1 MHIWS_Z1 MHIWS_Z1 {0}, 1, MHIWS_Z1
    MHIWS_Z1 MHIWS_Z1 MHIWS_Z1
    };
}
#pragma section mwccshim end
void UIWidgetOverlayText::WidgetSelect()
{
    int i;
    UIWidget::WidgetSelect();
    if (m_keepVisible) {
        for (i = 0; i < m_numOverlays; i++)
            m_overlays[i]->SetColor(m_selectedColor);
    }
    for (i = 0; i < m_numOverlays; i++)
        m_overlays[i]->DisplayText(0.0f, -1.0f, 0xFF, 1.0f, 0.0f, 0, -1.0f, 0xFF, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
}


// ---- src/UIWidgetOverlayText/WidgetDeSelect.cpp

void UIWidgetOverlayText::WidgetDeSelect()
{
    int i;
    UIWidget::WidgetDeSelect();
    if (m_keepVisible) {
        for (i = 0; i < m_numOverlays; i++)
            m_overlays[i]->SetColor(m_color);
    } else {
        for (i = 0; i < m_numOverlays; i++)
            m_overlays[i]->RemoveText();
    }
}


// ---- src/UIWidgetOverlayText/WidgetDisable.cpp

void UIWidgetOverlayText::WidgetDisable()
{
    int i;
    if (m_visibleWhenDisabled) {
        for (i = 0; i < m_numOverlays; i++)
            m_overlays[i]->SetColor(m_disabledColor);
    } else {
        for (i = 0; i < m_numOverlays; i++)
            m_overlays[i]->RemoveText();
    }
}


// ---- src/UIWidgetOverlayText/WidgetKeepEnabled.cpp

void UIWidgetOverlayText::WidgetKeepEnabled()
{
    UIWidget::WidgetKeepEnabled();
    if (m_keepVisible) {
        for (int i = 0; i < m_numOverlays; i++)
            m_overlays[i]->SetColor(m_color);
    }
}


// ---- src/UIWidgetStaticUserRotate/ctor.cpp

UIWidgetStaticUserRotate::UIWidgetStaticUserRotate(UIManager *manager, int enabled)
    : UIWidget(manager, enabled)
{
    m_object = 0;
    m_x9C = 0;
    m_xA0 = 0;
    m_xA4 = 0;
    m_rotateX = 0;
    m_rotateY = 0;
    m_rotateZ = 0;
    m_rotateSpeedX = 0;
    m_rotateSpeedY = 0;
    m_rotateSpeedZ = 0;
    m_position.x = 0;
    m_position.y = 0;
    m_position.z = 0;
    m_position.w = 1.0f;
    m_deselected = 1;
    m_rotateScale = 6.28318548f;
}


// ---- src/UIWidgetStaticUserRotate/dtor.cpp

UIWidgetStaticUserRotate::~UIWidgetStaticUserRotate()
{
}


// ---- src/UIWidgetStaticUserRotate/Create.cpp

void PSXprintf(const char *fmt, ...);


UIWidgetStaticUserRotate *UIWidgetStaticUserRotate::Create(int id, char *name, ParameterBlock *block)
{
    _D3DVECTOR look;
    _D3DVECTOR up;
    int isLit;

    block->GetParameter("IsLit", 0, &isLit);
    block->GetParameter("XRotate", 0, &m_rotateX);
    block->GetParameter("YRotate", 0, &m_rotateY);
    block->GetParameter("ZRotate", 0, &m_rotateZ);
    block->GetParameter("StartPaused", 1, &m_deselected);
    block->GetParameter("XVelocity", 0.0f, &m_rotateSpeedX);
    block->GetParameter("YVelocity", 0.0f, &m_rotateSpeedY);
    block->GetParameter("ZVelocity", 0.0f, &m_rotateSpeedZ);
    m_id = id;
    m_object = new PSXRenderableObject(1);
    if (!AddChild(m_object->Create(name, 0, 0), 0)) {
        PSXprintf("UI static object %s not created\n", name);
    } else {
        if (block->GetParameter("Position", &_D3DVECTOR(1.0f, 1.0f, 1.0f), &m_position))
            m_object->SetPosition(&m_position);
        else
            m_object->GetPosition(&m_position);
        m_startPosition = m_position;
        if (block->GetParameter("LookVector", &_D3DVECTOR(0.0f, 0.0f, 1.0f), &look)) {
            if (block->GetParameter("UpVector", &_D3DVECTOR(0.0f, 0.0f, 1.0f), &up))
                m_object->SetOrientation(&look, &up, 1, 0, 0);
        }
    }
    m_manager->LoadRequestedMaterials();
    return this;
}


// ---- src/UIWidgetStaticUserRotate/WidgetSelect.cpp

void UIWidgetStaticUserRotate::WidgetSelect()
{
    UIWidget::WidgetSelect();
    m_deselected = 0;
    EnableAll();
}


// ---- src/UIWidgetStaticUserRotate/WidgetDeSelect.cpp

void UIWidgetStaticUserRotate::WidgetDeSelect()
{
    UIWidget::WidgetDeSelect();
    m_deselected = 1;
    DisableAll();
}


// ---- src/UIWidgetStaticUserRotate/Tick.cpp

int UIWidgetStaticUserRotate::Tick(float dt)
{
    if (!m_deselected) {
        if (m_rotateX)
            RotateXAxis(dt);
        if (m_rotateY)
            RotateYAxis(dt);
        if (m_rotateZ)
            RotateZAxis(dt);
        UpdatePosition(dt);
    }
    return GameObject::Tick(dt);
}


// ---- src/UIWidgetStaticUserRotate/UpdatePosition.cpp

void UIWidgetStaticUserRotate::UpdatePosition(float)
{
    if (m_object)
        m_object->SetPosition(&m_position);
}


// ---- src/UIWidgetStaticUserRotate/RotateXAxis.cpp

void UIWidgetStaticUserRotate::RotateXAxis(float dt)
{
    if (m_object)
        m_object->AddRotation(_D3DVECTOR(1.0f, 0.0f, 0.0f), m_rotateScale * (dt * m_rotateSpeedX));
}


// ---- src/UIWidgetStaticUserRotate/RotateYAxis.cpp

void UIWidgetStaticUserRotate::RotateYAxis(float dt)
{
    if (m_object)
        m_object->AddRotation(_D3DVECTOR(0.0f, 1.0f, 0.0f), m_rotateScale * (dt * m_rotateSpeedY));
}


// ---- src/UIWidgetStaticUserRotate/RotateZAxis.cpp

void UIWidgetStaticUserRotate::RotateZAxis(float dt)
{
    if (m_object)
        m_object->AddRotation(_D3DVECTOR(0.0f, 0.0f, 1.0f), m_rotateScale * (dt * m_rotateSpeedZ));
}


// ---- src/UIWidgetMovementControl/ctor.cpp

UIWidgetMovementControl::UIWidgetMovementControl(UIManager *manager, int id) : UIWidget(manager, id)
{
    m_columns = 0;
    for (int i = 0; i < 16; i++)
        m_targets[i] = 0;
}


// ---- src/UIWidgetMovementControl/dtor.cpp

UIWidgetMovementControl::~UIWidgetMovementControl()
{
}


// ---- src/UIWidgetMovementControl/Create.cpp


UIWidgetMovementControl *UIWidgetMovementControl::Create(int id, ParameterBlock *block, UIManager *manager)
{
    m_id = id;
    m_manager = manager;
    block->GetParameter("GridSize", 0, &m_columns);
    block->GetParameter("Lpad_Left", 0, &m_targets[0]);
    block->GetParameter("Lpad_Right", 0, &m_targets[1]);
    block->GetParameter("Lpad_Up", 0, &m_targets[2]);
    block->GetParameter("Lpad_Down", 0, &m_targets[3]);
    block->GetParameter("Rpad_Down", 0, &m_targets[4]);
    block->GetParameter("Rpad_Left", 0, &m_targets[5]);
    block->GetParameter("Rpad_Up", 0, &m_targets[6]);
    block->GetParameter("Rpad_Right", 0, &m_targets[7]);
    block->GetParameter("Start", 0, &m_targets[8]);
    block->GetParameter("Select", 0, &m_targets[9]);
    block->GetParameter("L1", 0, &m_targets[10]);
    block->GetParameter("R1", 0, &m_targets[11]);
    block->GetParameter("L2", 0, &m_targets[12]);
    block->GetParameter("R2", 0, &m_targets[13]);
    block->GetParameter("L3", 0, &m_targets[14]);
    block->GetParameter("R3", 0, &m_targets[15]);
    return this;
}


// ---- src/UIWidgetMovementControl/WidgetKeyDownHandler.cpp


// Each of the 16 inputs moves the focus with its own key code.
void UIWidgetMovementControl::WidgetKeyDownHandler(ProcessNode *node, KeyInfo *key, int player, int input)
{
    switch (input) {
    case 0:
        SelectionFocus(0, player, 15);
        break;
    case 1:
        SelectionFocus(1, player, 13);
        break;
    case 2:
        SelectionFocus(2, player, 12);
        break;
    case 3:
        SelectionFocus(3, player, 14);
        break;
    case 4:
        SelectionFocus(4, player, 3);
        break;
    case 5:
        SelectionFocus(5, player, 1);
        break;
    case 6:
        SelectionFocus(6, player, 0);
        break;
    case 7:
        SelectionFocus(7, player, 2);
        break;
    case 8:
        SelectionFocus(8, player, 6);
        break;
    case 9:
        SelectionFocus(9, player, 4);
        break;
    case 10:
        SelectionFocus(10, player, 11);
        break;
    case 11:
        SelectionFocus(11, player, 7);
        break;
    case 12:
        SelectionFocus(12, player, 5);
        break;
    case 13:
        SelectionFocus(13, player, 10);
        break;
    case 14:
        SelectionFocus(14, player, 8);
        break;
    case 15:
        SelectionFocus(15, player, 9);
        break;
    }
}


// ---- src/UIWidgetMovementControl/SelectionFocus.cpp


void UIWidgetMovementControl::SelectionFocus(int input, int group, int a)
{
    UIGroup *g = m_manager->m_currentPage->m_states[group]->m_group;
    if (!g)
        return;
    int target = m_targets[input];
    switch (target) {
    case -1:
        g->NextControl(0);
        break;
    case -2:
        g->PreviousControl(0);
        break;
    case -3:
        g->GridControlNext(m_columns);
        break;
    case -4:
        g->GridControlPrevious(m_columns);
        break;
    case -5:
        g->GridControlUp(m_columns);
        break;
    case -6:
        g->GridControlDown(m_columns);
        break;
    default:
        if (target > 0)
            g->SelectControl(target);
        break;
    }
    if (target)
        m_manager->PlaySound(target, 1, 0, 0);
}


// ---- src/UIWidgetMovementControl/WidgetSelect.cpp

void UIWidgetMovementControl::WidgetSelect()
{
    UIWidget::WidgetSelect();
    EnableAll();
}


// ---- src/UIWidgetMovementControl/WidgetDeSelect.cpp

void UIWidgetMovementControl::WidgetDeSelect()
{
    UIWidget::WidgetDeSelect();
    DisableAll();
}


// ---- src/UIWidgetMovementGroup/ctor.cpp


UIWidgetMovementGroup::UIWidgetMovementGroup(UIManager *manager, int id)
    : UIWidget(manager, id)
{
    for (int i = 0; i < 16; i++)
        m_moves[i] = 0;
}


// ---- src/UIWidgetMovementGroup/dtor.cpp


UIWidgetMovementGroup::~UIWidgetMovementGroup()
{
}


// ---- src/UIWidgetMovementGroup/Create.cpp


// Reads the slot to focus for each of the 16 directions.
UIWidgetMovementGroup *UIWidgetMovementGroup::Create(int id, ParameterBlock *params, UIManager *manager)
{
    m_id = id;
    m_manager = manager;
    params->GetParameter("Lpad_Left", 0, &m_moves[0]);
    params->GetParameter("Lpad_Right", 0, &m_moves[1]);
    params->GetParameter("Lpad_Up", 0, &m_moves[2]);
    params->GetParameter("Lpad_Down", 0, &m_moves[3]);
    params->GetParameter("Rpad_Down", 0, &m_moves[4]);
    params->GetParameter("Rpad_Left", 0, &m_moves[5]);
    params->GetParameter("Rpad_Up", 0, &m_moves[6]);
    params->GetParameter("Rpad_Right", 0, &m_moves[7]);
    params->GetParameter("Start", 0, &m_moves[8]);
    params->GetParameter("Select", 0, &m_moves[9]);
    params->GetParameter("L1", 0, &m_moves[10]);
    params->GetParameter("R1", 0, &m_moves[11]);
    params->GetParameter("L2", 0, &m_moves[12]);
    params->GetParameter("R2", 0, &m_moves[13]);
    params->GetParameter("L3", 0, &m_moves[14]);
    params->GetParameter("R3", 0, &m_moves[15]);
    return this;
}


// ---- src/UIWidgetMovementGroup/WidgetKeyDownHandler.cpp


// Each direction maps to a fixed slot to focus.
void UIWidgetMovementGroup::WidgetKeyDownHandler(ProcessNode *node, KeyInfo *key, int a, int direction)
{
    switch (direction) {
    case 0:
        SelectionFocus(0, a, 15);
        break;
    case 1:
        SelectionFocus(1, a, 13);
        break;
    case 2:
        SelectionFocus(2, a, 12);
        break;
    case 3:
        SelectionFocus(3, a, 14);
        break;
    case 4:
        SelectionFocus(4, a, 3);
        break;
    case 5:
        SelectionFocus(5, a, 1);
        break;
    case 6:
        SelectionFocus(6, a, 0);
        break;
    case 7:
        SelectionFocus(7, a, 2);
        break;
    case 8:
        SelectionFocus(8, a, 6);
        break;
    case 9:
        SelectionFocus(9, a, 4);
        break;
    case 10:
        SelectionFocus(10, a, 11);
        break;
    case 11:
        SelectionFocus(11, a, 7);
        break;
    case 12:
        SelectionFocus(12, a, 5);
        break;
    case 13:
        SelectionFocus(13, a, 10);
        break;
    case 14:
        SelectionFocus(14, a, 8);
        break;
    case 15:
        SelectionFocus(15, a, 9);
        break;
    }
}


// ---- src/UIWidgetMovementGroup/SelectionFocus.cpp


void UIWidgetMovementGroup::SelectionFocus(int direction, int player, int)
{
    int move = m_moves[direction];
    switch (move) {
    case -1:
        m_manager->m_currentPage->NextGroup(player);
        break;
    case -2:
        m_manager->m_currentPage->PreviousGroup(player);
        break;
    default:
        if (move > 0)
            m_manager->m_currentPage->SelectGroup(player, move);
        break;
    }
    if (move)
        m_manager->PlaySound(move, 1, 0, 0);
}


// ---- src/UIWidgetMovementGroup/WidgetSelect.cpp


void UIWidgetMovementGroup::WidgetSelect()
{
    UIWidget::WidgetSelect();
    EnableAll();
}


// ---- src/UIWidgetMovementGroup/WidgetDeSelect.cpp


void UIWidgetMovementGroup::WidgetDeSelect()
{
    UIWidget::WidgetDeSelect();
    DisableAll();
}


// ---- src/UIWidgetStaticUserRotateDL/ctor.cpp


UIWidgetStaticUserRotateDL::UIWidgetStaticUserRotateDL(UIManager *manager, int id)
    : UIWidgetStaticUserRotate(manager, id)
{
    m_isLit = 0;
    m_loadDelay = 0;
    m_resourcesLoaded = 0;
    m_timing = 0;
    m_cleanFromMemory = 0;
    m_x1FC = 0;
    m_restoreMatrix = 0;
    m_setPosition = 0;
    m_setOrientation = 0;
}


// ---- src/UIWidgetStaticUserRotateDL/Create.cpp
extern "C" int sprintf(char *buf, const char *fmt, ...);


UIWidgetStaticUserRotateDL *UIWidgetStaticUserRotateDL::Create(int id, char *name, ParameterBlock *block)
{
    _D3DVECTOR defaultPosition(1.0f, 1.0f, 1.0f);
    m_setPosition = block->GetParameter("Position", &defaultPosition, &m_position);
    _D3DVECTOR defaultLook(0.0f, 0.0f, 1.0f);
    m_setOrientation = block->GetParameter("LookVector", &defaultLook, &m_forward);
    m_setOrientation = m_setOrientation && block->GetParameter("UpVector", &_D3DVECTOR(0.0f, 0.0f, 1.0f), &m_up);
    block->GetParameter("IsLit", 0, &m_isLit);
    block->GetParameter("XRotate", 0, &m_rotateX);
    block->GetParameter("YRotate", 0, &m_rotateY);
    block->GetParameter("ZRotate", 0, &m_rotateZ);
    block->GetParameter("StartPaused", 1, &m_deselected);
    block->GetParameter("XVelocity", 0.0f, &m_rotateSpeedX);
    block->GetParameter("YVelocity", 0.0f, &m_rotateSpeedY);
    block->GetParameter("ZVelocity", 0.0f, &m_rotateSpeedZ);
    block->GetParameter("CleanFromMemory", 0, &m_cleanFromMemory);
    m_id = id;
    sprintf(m_fileName, "%s", name);
    return this;
}


// ---- src/UIWidgetStaticUserRotateDL/WidgetSelect.cpp

extern int TimerOverflowCounterT0;

// Win32-style timeGetTime: EE timer 0 (T0_COUNT at 0x10000000) ticks plus the
// overflows counted by TimerHandlerT0, accumulated and scaled to milliseconds.
inline unsigned int timeGetTime()
{
    static long Time = 0;
    static int lastCount = 0;
    int count;
    int elapsed;
    int overflows;
    count = *(volatile int *)0x10000000;
    overflows = TimerOverflowCounterT0;
    TimerOverflowCounterT0 = 0;
    if (lastCount <= count)
        elapsed = count - lastCount;
    else
        elapsed = 0x10000 - lastCount + count;
    if (lastCount >= count && overflows > 0)
        overflows--;
    lastCount = count;
    Time += elapsed + (overflows << 16);
    return 0.0017066667f * Time;
}


void UIWidgetStaticUserRotateDL::WidgetSelect()
{
    UIWidget::WidgetSelect();
    if (!m_resourcesLoaded) {
        if (m_loadDelay > 0.0f) {
            m_selectTime = timeGetTime();
            m_timing = 1;
        } else {
            m_resourcesLoaded = LoadResources();
        }
    }
    UIWidgetStaticUserRotate::WidgetSelect();
}


// ---- src/UIWidgetStaticUserRotateDL/WidgetDeSelect.cpp

void UIWidgetStaticUserRotateDL::WidgetDeSelect()
{
    UIWidget::WidgetDeSelect();
    if (m_cleanFromMemory) {
        if (m_resourcesLoaded)
            m_resourcesLoaded = !CleanResources();
    }
    UIWidgetStaticUserRotate::WidgetDeSelect();
}


// ---- src/UIWidgetStaticUserRotateDL/LoadResources.cpp
void PSXprintf(const char *fmt, ...);


int UIWidgetStaticUserRotateDL::LoadResources()
{
    int ok = 1;
    m_object = new PSXRenderableObject(1);
    if (!AddChild(m_object->Create(m_fileName, 0, 0), -1)) {
        PSXprintf("UI static object %s not created\n", m_fileName);
        ok = 0;
    } else {
        if (m_setPosition)
            m_object->SetPosition(&m_position);
        else
            m_object->GetPosition(&m_position);
        if (m_setOrientation)
            m_object->SetOrientation(&m_forward, &m_up, 1, 0, 0);
        else
            m_object->GetOrientation(&m_forward, &m_up);
        if (m_restoreMatrix)
            m_object->SetMatrix(m_savedMatrix);
        m_restoreMatrix = 0;
    }
    m_manager->LoadRequestedMaterials();
    return ok;
}


// ---- src/UIWidgetStaticUserRotateDL/CleanResources.cpp

int UIWidgetStaticUserRotateDL::CleanResources()
{
    if (m_object) {
        m_restoreMatrix = 1;
        m_object->GetMatrix(&m_savedMatrix);
        m_object->RemoveFromChain();
        m_object->Release();
        m_object = 0;
    }
    return 1;
}


// ---- src/UIWidgetStaticUserRotateDL/Tick.cpp




// Postpones loading the resources until m_loadDelay seconds after selection.
int UIWidgetStaticUserRotateDL::Tick(float dt)
{
    if (m_timing) {
        if ((float)((timeGetTime() - m_selectTime) / 1000) > m_loadDelay) {
            m_resourcesLoaded = LoadResources();
            m_timing = 0;
        }
    }
    if (m_resourcesLoaded)
        return UIWidgetStaticUserRotate::Tick(dt);
    else
        return GameObject::Tick(dt);
}


// ---- src/UIWidgetIcon/ctor.cpp

UIWidgetIcon::UIWidgetIcon(UIManager *manager, int enabled)
    : UIWidget(manager, enabled)
{
    m_iconManager = 0;
    m_icon = 0;
}


// ---- src/UIWidgetIcon/dtor.cpp


UIWidgetIcon::~UIWidgetIcon()
{
    if (m_icon)
        delete m_icon;
}


// ---- src/UIWidgetIcon/Create.cpp
extern "C" int sprintf(char *buf, const char *fmt, ...);


extern char UITextureContentDirectory[0x104];


extern PSXVideoCard *LpVideoCard;


UIWidgetIcon *UIWidgetIcon::Create(int id, ParameterBlock *params, UIManager *manager)
{
    char texture[0x104];
    char path[0x110];
    _RECT none;
    _RECT map;
    _RECT partial;
    float angle;
    float pivotX;
    float pivotY;
    int depthEnabled;
    float depth;
    int alphaEnabled;
    int alpha;
    int hasPartial;

    UIWidget::Create(params);
    m_manager = manager;
    m_iconManager = m_manager->m_iconManager;
    none.left = 0;
    none.right = 0;
    none.top = 0;
    none.bottom = 0;
    angle = 0;
    pivotX = 0;
    pivotY = 0;
    depthEnabled = 0;
    depth = 0;
    alphaEnabled = 0;
    alpha = 0x40;
    hasPartial = 0;
    if (!m_iconManager)
        m_manager->m_iconManager = m_iconManager = (new ScreenIconManager(1))->Create(128);
    if (params->GetParameter("Angle", 0.0f, &angle)) {
        params->GetParameter("PivotX", 0.0f, &pivotX);
        params->GetParameter("PivotY", 0.0f, &pivotY);
    }
    if (params->GetParameter("PartialRect", &none, &partial))
        hasPartial = 1;
    if (!params->GetParameter("MapRect", &none, &map)) {
        params->GetParameter("XLocation", 0, &map.left);
        params->GetParameter("YLocation", 0, &map.top);
        map.right = map.left + (partial.right - partial.left);
        map.bottom = map.top + (partial.bottom - partial.top);
    }
    params->GetParameter("AlphaEnabled", 0, &alphaEnabled);
    params->GetParameter("Alpha", 0xFF, &alpha);
    params->GetParameter("DepthEnabled", 0, &depthEnabled);
    params->GetParameter("Depth", 0.0f, &depth);
    if (depth < 0.0f)
        depth = LpVideoCard->m_maxDepth - 300;
    params->GetParameter("Texture", "*INVALID*", texture, sizeof(texture));
    sprintf(path, "%s%s", UITextureContentDirectory, texture);
    m_icon = (new ScreenIcon)->Create(path, 0.0f, 0.0f);
    m_icon->SetPosition(map.left, map.top);
    m_icon->SetWidth(map.right - map.left);
    m_icon->SetHeight(map.bottom - map.top);
    m_icon->m_alphaTest = alphaEnabled;
    m_icon->SetAlpha(alpha);
    m_icon->m_zBuffer = depthEnabled;
    m_icon->SetDepth(depth);
    if (angle > 0.0f) {
        m_icon->SetAngle(angle);
        m_icon->SetPivot(pivotX, pivotY);
    }
    if (hasPartial)
        m_icon->SetUVRect(partial.left, partial.right, partial.top, partial.bottom);
    return this;
}


// ---- src/UIWidgetIcon/Render3D.cpp

int UIWidgetIcon::Render3D()
{
    m_iconManager->AddIcon(m_icon, 0);
    return GameObject::Render3D();
}


// ---- src/UIWidgetIcon/WidgetSelect.cpp


void UIWidgetIcon::WidgetSelect()
{
    UIWidget::WidgetSelect();
}


// ---- src/UIWidgetIcon/WidgetDeSelect.cpp


void UIWidgetIcon::WidgetDeSelect()
{
    UIWidget::WidgetDeSelect();
}


// ---- src/UIWidgetIcon/Tick.cpp


int UIWidgetIcon::Tick(float dt)
{
    return GameObject::Tick(dt);
}


// ---- src/UIWidget/WidgetDisable.cpp

void UIWidget::WidgetDisable()
{
}


// UIWidgetStaticUserRotateDL's dtor is inline in its header; mwcc writes it
// here, with the vtable.


// ---- src/sinit/UIWidgets.cpp
// Static initializer of UIWidgets.cpp: only the axis vectors (included at the
// top: retail's .bss has them first).
