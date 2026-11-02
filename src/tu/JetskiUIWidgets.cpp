// JetskiUIWidgets.cpp  @ 0x00324670 (text 0x324670-0x3285B0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 2 -str readonly, mode: cpp)
//
// Retail's JetskiUIWidgets.cpp as one translation unit: the JetskiWidget*
// classes, unanchored inside WaterObject.cpp's tu_map row
// (__sinit_JetskiUIWidgets.cpp, the .ctor slot after WaterObject.cpp's,
// builds only the axis vectors). Built with -sdatathreshold 2, as
// ParseMultiLine's per-function file was: the 2-byte initializer of its
// `char delim[] = "@"` sits in retail's .sdata. `make link` places its data
// as well as its code.


#pragma inline_depth(5)
#pragma fast_fptosi on

#include "d3dtypes.h"
#include "UIWidget/UIWidget.h"
#include "UIManager/UIManager.h"
#include "JetskiWidgetButton/JetskiWidgetButton.h"
#include "JetskiWidgetButton/JetskiWidgetButtonLocal.h"
#include "JetskiWidgetLap/JetskiWidgetLap.h"
#include "JetskiWidgetCharacter/JetskiWidgetCharacter.h"
#include "JetskiWidgetCharacter/JetskiWidgetCharacterLocal.h"
#include "JetskiWidgetSeadoo/JetskiWidgetSeadoo.h"
#include "JetskiWidgetText/JetskiWidgetText.h"
#include "JetskiWidgetTextMove/JetskiWidgetTextMove.h"
#include "JetskiWidgetHighlight/JetskiWidgetHighlight.h"
#include "ScreenIcon/ScreenIcon.h"
#include "ScreenText/ScreenText.h"
#include "_RECT/_RECT.h"
#include "ParameterBlock/ParameterBlock.h"
#include "ScreenIconManager/ScreenIconManager.h"
#include "ScreenTextManager/ScreenTextManager.h"
#include "ScreenTextFontItemList/ScreenTextFontItem.h"
#include "ScreenTextFontItemList/ScreenTextFontItemList.h"
#include "ScreenTextFont/ScreenTextFont.h"
#include "JetskiGame/JetskiGame.h"
#include "JetskiGameOptions/JetskiGameOptions.h"
#include "GameText/GameText.h"
#include "LocalizedString/LocalizedString.h"
#include "MotionLibrary/MotionLibrary.h"
#include "D3DIMSoultreeCharacter/D3DIMSoultreeCharacter.h"
#include "Character/Character.h"
#include "GameObject/GameObjectGlobals.h"
#include "PSXRenderableObject/PSXRenderableObject.h"
#include "RenderableObject/RenderableObject.h"
#include "JetskiUIManager/JetskiUIManager.h"
#include "JetskiUIManMain/JetskiUIManMain.h"
#include "PSXVideoCard/PSXVideoCard.h"
#include "UIWaterEffect/UIWaterEffect.h"
#include "RenderTarget/RenderTarget.h"
#include "sinit/AxisVectors.h"

extern "C" int sprintf(char *buf, const char *fmt, ...);
extern "C" char *strcpy(char *dst, const char *src);
extern "C" unsigned int strlen(const char *s);
extern "C" int strcmp(const char *a, const char *b);
extern "C" void *malloc(unsigned int size);
extern "C" void *realloc(void *p, unsigned int size);
extern "C" void free(void *p);
extern "C" char *strtok(char *s, const char *delim);
void PSXprintf(const char *fmt, ...);

extern JetskiGame *LpGame;
extern MotionLibrary *LpMotionLibrary;
extern PSXVideoCard *LpVideoCard;
extern char UITextureContentDirectory[0x104];

// ---- src/JetskiWidgetButton/ctor.cpp

JetskiWidgetButton::JetskiWidgetButton(UIManager *manager, int enabled) : UIWidget(manager, enabled)
{
    m_iconManager = 0;
    m_icon = 0;
    m_rect.left = 0;
    m_rect.right = 0;
    m_rect.top = 0;
    m_rect.bottom = 0;
    m_selectTimer = 0;
    m_selected = 0;
    m_manager = 0;
    m_textManager = 0;
    m_text = 0;
    m_font = 0;
    m_textID = 0;
    m_scale = 1.0f;
    m_color = 0;
    m_textX = 0;
    m_textY = 0;
}

// ---- src/JetskiWidgetButton/dtor.cpp

JetskiWidgetButton::~JetskiWidgetButton()
{
    if (m_icon) {
        delete m_icon;
        m_icon = 0;
    }
    if (m_text) {
        delete m_text;
        m_text = 0;
    }
}

// ---- src/JetskiWidgetButton/Create.cpp

JetskiWidgetButton *JetskiWidgetButton::Create(int id, ParameterBlock *params, UIManager *manager,
                                               ScreenTextManager *textManager)
{
    char path[0x110];
    char texture[0x104];
    _RECT partial;
    _RECT none;
    int hasPartial;

    UIWidget::Create(params);
    m_manager = manager;
    m_iconManager = m_manager->m_iconManager;
    none.left = 0;
    none.right = 0;
    none.top = 0;
    none.bottom = 0;
    hasPartial = 0;
    if (!m_iconManager)
        m_manager->m_iconManager = m_iconManager = (new ScreenIconManager(1))->Create(128);
    if (params->GetParameter("PartialRect", &none, &partial))
        hasPartial = 1;
    if (!params->GetParameter("MapRect", &none, &m_rect)) {
        // (The redundant casts reproduce retail's argument evaluation order.)
        params->GetParameter("XLocation", 0, (int *)&m_rect.left);
        params->GetParameter("YLocation", 0, (int *)&m_rect.top);
        m_rect.right = m_rect.left + (partial.right - partial.left);
        m_rect.bottom = m_rect.top + (partial.bottom - partial.top);
    }
    params->GetParameter("Texture", "*INVALID*", texture, sizeof(texture));
    sprintf(path, "%s%s", UITextureContentDirectory, texture);
    m_icon = (new ScreenIcon)->Create(path, 0.0f, 0.0f);
    m_icon->SetPosition(m_rect.left, m_rect.top);
    m_icon->SetWidth(m_rect.right - m_rect.left);
    m_icon->SetHeight(m_rect.bottom - m_rect.top);
    if (hasPartial)
        m_icon->SetUVRect(partial.left, partial.right, partial.top, partial.bottom);
    _D3DVECTOR color;
    m_textManager = textManager;
    params->GetParameter("FontName", m_fontName, sizeof(m_fontName));
    params->GetParameter("XPosition", 0, &m_textX);
    params->GetParameter("YPosition", 0, &m_textY);
    params->GetParameter("JustificationID", ScreenTextManager::LeftJustification, &m_justification);
    params->GetParameter("Scale", 1.0f, &m_scale);
    params->GetParameter("TextID", -1, &m_textID);
    _D3DVECTOR defaultColor(40.0f, 40.0f, 40.0f);
    params->GetParameter("Color", &defaultColor, &color);
    ScreenTextFontItem *font = LpGame->m_fonts->GetScreenTextFont(m_fontName);
    m_font = font ? font->m_font : 0;
    m_color = (unsigned char)(int)color.x | (((unsigned char)(int)color.y << 8) | (((unsigned char)(int)color.z << 16) | 0xFF000000));
    m_id = id;
    return this;
}

// ---- src/JetskiWidgetButton/Tick.cpp

// Parser heap state: Create leaves the heap so that Tick's ApplyExternalForce arguments are
// loaded in another order (11 words). A zeroed image bigger than any heap block restores
// retail's; it is not part of the retail file.

struct MwccHeapImage {
    char tail[0x30000];
};
#pragma section mwccshim begin
inline void MwccHeapImageShim() { const MwccHeapImage image = { {0} }; }
#pragma section mwccshim end

int JetskiWidgetButton::Tick(float dt)
{
    if (m_selectTimer > 0.0f) {
        m_selectTimer -= dt;
    } else if (m_selected) {
        m_selected = 0;
        m_icon->SetPosition(m_rect.left, m_rect.top);
        m_text->SetPosition(m_textX, m_textY);
        SetColor(0, 0xFF, 0xFF, 0xFF);
        if (((JetskiUIManager *)m_manager)->m_waterEffect)
            ((JetskiUIManager *)m_manager)->m_waterEffect->ApplyExternalForce(10.0f, 0.5f * (m_rect.left + m_rect.right), 0.5f * (m_rect.top + m_rect.bottom));
    }
    return 1;
}

// ---- src/JetskiWidgetButton/Render3D.cpp

int JetskiWidgetButton::Render3D()
{
    m_iconManager->AddIcon(m_icon, 0);
    m_textManager->PositionText(m_text);
    m_textManager->AddText(m_text, 0);
    return GameObject::Render3D();
}

// ---- src/JetskiWidgetButton/WidgetLocalize.cpp

// ScreenText::SetJustification with the value passed by value: the
// WidgetLocalize functions load the value before the text pointer.
static inline void SetJustificationByValue(ScreenText *text, int justification)
{
    text->m_justification = justification;
    text->m_positionDirty = 1;
}

#pragma alignlabel off
int JetskiWidgetButton::WidgetLocalize()
{
    char buf[0x1F0];
    if (m_text) {
        delete m_text;
        m_text = 0;
    }
    strcpy(buf, LpGame->m_uiText->GetString(m_textID));
    m_text = new ScreenText(0x28, 0);
    m_text->Create(strlen(buf), buf);
    m_text->SetScale(m_scale);
    SetJustificationByValue(m_text, m_justification);
    m_text->SetFont(m_font);
    m_text->SetColor(m_color);
    m_text->SetPosition(m_textX, m_textY);
    m_textManager->PositionText(m_text);
    return 1;
}
#pragma alignlabel reset

// ---- src/JetskiWidgetButton/WidgetSelect.cpp

// Nudge the icon and text 3 pixels down-right while the button is pressed.
void JetskiWidgetButton::WidgetSelect()
{
    UIWidget::WidgetSelect();
    m_selected = 1;
    m_selectTimer = 0.08f;
    m_icon->SetPosition(m_rect.left + 3, m_rect.top + 3);
    m_text->SetPosition(m_textX + 3, m_textY + 3);
}

// ---- src/JetskiWidgetButton/WidgetDeSelect.cpp

void JetskiWidgetButton::WidgetDeSelect()
{
    UIWidget::WidgetDeSelect();
    SetColor(0xEC, 0x85, 0x31, 0xFF);
    m_selected = 0;
    m_selectTimer = 0.0f;
    m_icon->SetPosition(m_rect.left, m_rect.top);
    m_text->SetPosition(m_textX, m_textY);
}

// ---- src/JetskiWidgetButton/SetColor.cpp

void JetskiWidgetButton::SetColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    ScreenIcon *text = m_icon;
    text->m_color = r | (g << 8 | (a << 24 | b << 16));
    text->m_colorDirty = 1;
}

// ---- src/JetskiWidgetLap/ctor.cpp

JetskiWidgetLap::JetskiWidgetLap(UIManager *manager, int id)
    : UIWidget(manager, id)
{
    // Retail sets m_lap twice (the lap count, then 1) and m_font twice.
    m_lap = LpGame->m_gameOptions->m_x30;
    m_font = 0;
    m_xE8 = 0;
    m_lap = 1;
    m_textManager = 0;
    m_text = 0;
    m_font = 0;
    m_fontName[0] = 0;
}

// ---- src/JetskiWidgetLap/dtor.cpp

JetskiWidgetLap::~JetskiWidgetLap()
{
    delete m_text;
}

// ---- src/JetskiWidgetLap/Create.cpp

// Packs a 0-255 colour vector as opaque ARGB. The alpha is the signed int
// (0xFF << 24), not 0xFF000000: that is what gives retail's register order.
#define PACK_COLOR_ARGB(v) ((0xFF << 24) | ((int)(v).x << 16) | ((int)(v).y << 8) | (int)(v).z)

JetskiWidgetLap *JetskiWidgetLap::Create(int id, ParameterBlock *params, ScreenTextManager *manager)
{
    char justification[0x20];
    m_id = id;
    m_textManager = manager;
    _D3DVECTOR color;
    _D3DVECTOR highlight;
    params->GetParameter("FontName", m_fontName, 0x80);
    params->GetParameter("XPosition", 0, &m_textX);
    params->GetParameter("YPosition", 0, &m_textY);
    params->GetParameter("Justification", "Left", justification, 0x14);
    params->GetParameter("Color", &_D3DVECTOR(40.0f, 40.0f, 40.0f), &color);
    m_color = PACK_COLOR_ARGB(color);
    params->GetParameter("HighlightColor", &_D3DVECTOR(255.0f, 255.0f, 255.0f), &highlight);
    m_highlightColor = PACK_COLOR_ARGB(highlight);
    if (!strcmp(justification, "Left"))
        m_justification = ScreenTextManager::LeftJustification;
    else if (!strcmp(justification, "Right"))
        m_justification = ScreenTextManager::RightJustification;
    else if (!strcmp(justification, "CenterBoth"))
        m_justification = ScreenTextManager::CenterBothJustification;
    else if (!strcmp(justification, "CenterVertical"))
        m_justification = ScreenTextManager::CenterVerticalJustification;
    else if (!strcmp(justification, "CenterHorizontal"))
        m_justification = ScreenTextManager::CenterHorizontalJustification;
    ScreenTextFontItem *item = LpGame->m_fonts->GetScreenTextFont(m_fontName);
    m_font = item ? item->m_font : 0;
    m_text = new ScreenText(0x28, 0);
    m_text->SetPosition(m_textX, m_textY);
    m_text->SetColor(m_color);
    return this;
}

// ---- src/JetskiWidgetLap/Render3D.cpp

int JetskiWidgetLap::Render3D()
{
    char buf[256];
    sprintf(buf, "%d", m_lap);
    m_text->SetText(buf);
    m_textManager->PositionText(m_text, m_justification, m_font);
    m_textManager->AddText(m_text, m_font);
    return GameObject::Render3D();
}

// ---- src/JetskiWidgetLap/WidgetSelect.cpp

void JetskiWidgetLap::WidgetSelect()
{
    UIWidget::WidgetSelect();
    ScreenText *item = m_text;
    item->m_color = m_highlightColor;
    item->m_x5D_bit4 = 1;
}

// ---- src/JetskiWidgetLap/WidgetDeSelect.cpp

void JetskiWidgetLap::WidgetDeSelect()
{
    UIWidget::WidgetDeSelect();
    ScreenText *item = m_text;
    item->m_color = m_color;
    item->m_x5D_bit4 = 1;
}

// ---- src/JetskiWidgetLap/WidgetKeepEnabled.cpp

void JetskiWidgetLap::WidgetKeepEnabled()
{
    m_selected = 0;
    ScreenText *item = m_text;
    item->m_color = m_color;
    item->m_x5D_bit4 = 1;
}

// ---- src/JetskiWidgetLap/WidgetKeyDownHandler.cpp

void JetskiWidgetLap::WidgetKeyDownHandler(ProcessNode *, KeyInfo *, int, int direction)
{
    switch (direction) {
    case 0:
        m_lap--;
        if (m_lap <= 0)
            m_lap = 99;
        break;
    case 1:
        m_lap++;
        if (m_lap > 99)
            m_lap = 1;
        break;
    }
}

// ---- src/JetskiWidgetCharacter/ctor.cpp

JetskiWidgetCharacter::JetskiWidgetCharacter(JetskiUIManager *manager, int id)
    : UIWidget((UIManager *)manager, id)
{
    m_character = 0;
    strcpy(m_name, "");
    m_mode = 0;
    m_rider = 0;
    m_skin = 0;
    m_position = _D3DVECTOR(1.0f, 1.0f, 1.0f);
    m_lookVector = _D3DVECTOR(0.0f, 0.0f, 0.0f);
    m_upVector = _D3DVECTOR(0.0f, 0.0f, 0.0f);
    m_initialMotion = 0;
    m_manager = manager;
    m_motionName[0] = 0;
}

// ---- src/JetskiWidgetCharacter/dtor.cpp

JetskiWidgetCharacter::~JetskiWidgetCharacter()
{
}

// ---- src/JetskiWidgetCharacter/Create.cpp

JetskiWidgetCharacter *JetskiWidgetCharacter::Create(int id, char *name, ParameterBlock *params)
{
    UIWidget::Create(params);
    m_manager = (JetskiUIManager *)UIWidget::m_manager;
    _D3DVECTOR one(1.0f, 1.0f, 1.0f);
    params->GetParameter("Position", &one, &m_position);
    _D3DVECTOR look(0.0f, 0.0f, 1.0f);
    params->GetParameter("LookVector", &look, &m_lookVector);
    _D3DVECTOR up(0.0f, 0.0f, 1.0f);
    params->GetParameter("UpVector", &up, &m_upVector);
    params->GetParameter("InitialMotion", 0, &m_initialMotion);
    strcpy(m_name, name);
    m_id = id;
    return this;
}

// ---- src/JetskiWidgetCharacter/CreateCharacter.cpp

void JetskiWidgetCharacter::CreateCharacter(int rider, int mode)
{
    JetskiGameOptions *data = LpGame->m_gameOptions;
    char block[0xD0];
    LpRenderTarget->TrySendRenderQueue(1);
    LpRenderTarget->WaitForRenderingToComplete();
    ParameterBlock *params = new ParameterBlock;
    params->OpenFile(m_name, 0);
    strcpy(m_motionName, "NONE");
    sprintf(block, "Motion %d", m_initialMotion);
    params->ReadParameterBlock(block);
    params->GetParameter("NAME", m_motionName, 0x100);
    delete params;
    m_character = new D3DIMSoultreeCharacter(1);
    ((D3DIMSoultreeCharacter *)m_character)->Create(m_name, 0, 1, 1);  // m_character is a Character *
    m_character->TickActionByPercentage(LpMotionLibrary->MotionOfName(m_motionName, 1), 0.0f, 0);
    m_character->m_root->SetPosition(m_position);
    m_character->m_root->SetOrientation(&m_lookVector, &m_upVector, 1, 0, 0);
    m_character->m_root->AddSearchPlacement(m_manager->m_searchNode, 2);
    if (!AddChild(m_character, 0))
        PSXprintf("UI dynamic object %s not created\n", m_name);
    m_mode = mode;
    m_rider = rider;
    m_skin = 0;
    ChangeSkin(data->m_skins[m_rider]);
}

// ---- src/JetskiWidgetCharacter/DeleteCharacter.cpp

void JetskiWidgetCharacter::DeleteCharacter()
{
    LpRenderTarget->TrySendRenderQueue(1);
    LpRenderTarget->WaitForRenderingToComplete();
    m_character->m_root->RemoveFromSearch();
    m_character->RemoveFromChain();
    m_character->Release();
    m_character = 0;
}

// ---- src/JetskiWidgetCharacter/ChangeSkin.cpp

void JetskiWidgetCharacter::ChangeSkin(int skin)
{
    if (m_skin == skin)
        return;
    JetskiGameOptions *options = LpGame->m_gameOptions;
    LpRenderTarget->TrySendRenderQueue(1);
    LpRenderTarget->WaitForRenderingToComplete();
    ((RenderableObject *)m_character->m_root)->ChangeMaterial(options->m_riderSkins[m_rider][0][m_skin],
                                        options->m_riderSkins[m_rider][0][skin]);
    ((RenderableObject *)m_character->m_root)->ChangeMaterial(options->m_riderSkins[m_rider][1][m_skin],
                                        options->m_riderSkins[m_rider][1][skin]);
    m_skin = skin;
    options->m_skins[m_rider] = m_skin;
    LpRenderTarget->TrySendRenderQueue(1);
    LpRenderTarget->WaitForRenderingToComplete();
}

// ---- src/JetskiWidgetCharacter/Tick.cpp

int JetskiWidgetCharacter::Tick(float dt)
{
    UIWidgetModel *widget = 0;
    UIPage *page;
    switch (m_mode) {
    case 2:
        page = m_manager->m_pages[1];
        widget = (UIWidgetModel *)page->GetWidgetPtr(2, page->GetGroupControlID(2), 1);
        break;
    case 0x1B:
        page = m_manager->m_pages[0x1A];
        widget = (UIWidgetModel *)page->GetWidgetPtr(2, page->GetGroupControlID(2), 1);
        break;
    }
    if (widget && m_character) {
        _D3DMATRIX matrix;
        widget->m_model->GetMatrix(0, &matrix);
        m_character->m_root->SetMatrix(0, &matrix);
    }
    return GameObject::Tick(dt);
}

// ---- src/JetskiWidgetSeadoo/ctor.cpp

JetskiWidgetSeadoo::JetskiWidgetSeadoo(JetskiUIManager *manager, int id)
    : UIWidget((UIManager *)manager, id)
{
    m_seadoo = 0;
    strcpy(m_fileName, "");
    m_position.x = 1.0f;
    m_position.y = 1.0f;
    m_position.z = 1.0f;
    m_position.w = 1.0f;
    m_lookVector.x = 0.0f;
    m_lookVector.y = 0.0f;
    m_lookVector.z = 0.0f;
    m_lookVector.w = 1.0f;
    m_upVector.x = 0.0f;
    m_upVector.y = 0.0f;
    m_upVector.z = 0.0f;
    m_upVector.w = 1.0f;
    m_xA4 = 0;
    m_autoRotate = 0.0f;
    m_manager = manager;
    m_model = 0;
    m_skin = 0;
}

// ---- src/JetskiWidgetSeadoo/dtor.cpp

JetskiWidgetSeadoo::~JetskiWidgetSeadoo()
{
}

// ---- src/JetskiWidgetSeadoo/Create.cpp

JetskiWidgetSeadoo *JetskiWidgetSeadoo::Create(int id, char *name, ParameterBlock *params)
{
    m_id = id;
    UIWidget::Create(params);
    m_manager = (JetskiUIManager *)UIWidget::m_manager;
    _D3DVECTOR one(1.0f, 1.0f, 1.0f);
    params->GetParameter("Position", &one, &m_position);
    _D3DVECTOR look(0.0f, 0.0f, 1.0f);
    params->GetParameter("LookVector", &look, &m_lookVector);
    _D3DVECTOR up(0.0f, 0.0f, 1.0f);
    params->GetParameter("UpVector", &up, &m_upVector);
    strcpy(m_fileName, name);
    params->GetParameter("AutoRotate", 0.0f, &m_autoRotate);
    return this;
}

// ---- src/JetskiWidgetSeadoo/CreateSeadoo.cpp

// SoultreeObject's first base: 8 bytes in front of its GameObject subobject.

// The page manager is a JetskiUIManMain (+0x2F4 is past JetskiUIManager).

void JetskiWidgetSeadoo::CreateSeadoo(int model)
{
    JetskiGameOptions *options = LpGame->m_gameOptions;
    ((JetskiUIManMain *)UIWidget::m_manager)->m_x2F4 = 0;
    LpRenderTarget->TrySendRenderQueue(1);
    LpRenderTarget->WaitForRenderingToComplete();
    m_seadoo = new PSXRenderableObject(1);
    m_seadoo->Create(m_fileName, 0, 0);
    m_seadoo->SetPosition(m_position);
    m_seadoo->SetOrientation(&m_lookVector, &m_upVector, 1, 0, 0);
    m_seadoo->AddSearchPlacement(m_manager->m_searchNode, 2);
    if (!AddChild(m_seadoo, 0))
        PSXprintf("UI static object %s not created\n", m_fileName);
    m_model = model;
    m_skin = 0;
    ChangeSkin(options->m_skins[m_model]);
}

// ---- src/JetskiWidgetSeadoo/DeleteSeadoo.cpp

void JetskiWidgetSeadoo::DeleteSeadoo()
{
    LpRenderTarget->TrySendRenderQueue(1);
    LpRenderTarget->WaitForRenderingToComplete();
    m_seadoo->RemoveFromSearch();
    m_seadoo->RemoveFromChain();
    m_seadoo->Release();
    m_seadoo = 0;
}

// ---- src/JetskiWidgetSeadoo/ChangeSkin.cpp

void JetskiWidgetSeadoo::ChangeSkin(int skin)
{
    if (m_skin != skin) {
        JetskiGameOptions *options = LpGame->m_gameOptions;
        LpRenderTarget->TrySendRenderQueue(1);
        LpRenderTarget->WaitForRenderingToComplete();
        m_seadoo->ChangeMaterial(options->m_jetskiSkins[m_model][0][m_skin], options->m_jetskiSkins[m_model][0][skin]);
        m_seadoo->ChangeMaterial(options->m_jetskiSkins[m_model][1][m_skin], options->m_jetskiSkins[m_model][1][skin]);
        m_skin = skin;
        LpRenderTarget->TrySendRenderQueue(1);
        LpRenderTarget->WaitForRenderingToComplete();
    }
}

// ---- src/JetskiWidgetSeadoo/Tick.cpp

// The page manager is a JetskiUIManMain (+0x2F4 is past JetskiUIManager).

int JetskiWidgetSeadoo::Tick(float dt)
{
    ((JetskiUIManMain *)UIWidget::m_manager)->m_x2F4 = 1;
    return GameObject::Tick(dt);
}

// ---- src/JetskiWidgetText/ctor.cpp

JetskiWidgetText::JetskiWidgetText(UIManager *manager, int enabled) : UIWidget(manager, enabled)
{
    m_textManager = 0;
    m_lines = 0;
    m_font = 0;
    m_stringID = 0;
    m_numLines = 0;
    m_scale = 1.0f;
    m_color = 0;
    m_textX = 0;
    m_textY = 0;
    m_hasHighlight = 0;
    m_highlightColor = 0;
    m_hasHighlightScale = 0;
    m_highlightScale = 1.0f;
    m_height = 0;
    m_maxChars = 0;
    m_x118 = 0;
    m_x12C = 0;
}

// ---- src/JetskiWidgetText/dtor.cpp

JetskiWidgetText::~JetskiWidgetText()
{
    if (m_lines) {
        for (int i = 0; i < m_numLines; i++)
            delete m_lines[i];
        free(m_lines);
    }
}

// ---- src/JetskiWidgetText/Create.cpp

// Packs a 0-255 colour vector as opaque ARGB.
#define PACK_COLOR(v) (0xFF000000 | ((int)(v).x << 16) | ((int)(v).y << 8) | (int)(v).z)

JetskiWidgetText *JetskiWidgetText::Create(int id, ParameterBlock *params, ScreenTextManager *manager)
{
    UIWidget::Create(params);
    _D3DVECTOR color;
    _D3DVECTOR highlight;
    m_textManager = manager;
    params->GetParameter("FontName", m_fontName, 0x80);
    params->GetParameter("XPosition", 0, &m_textX);
    params->GetParameter("YPosition", 0, &m_textY);
    params->GetParameter("MaxCharacters", 0, &m_maxChars);
    params->GetParameter("JustificationID", ScreenTextManager::LeftJustification, &m_justification);
    params->GetParameter("TextID", -1, &m_stringID);
    params->GetParameter("Height", 0, &m_height);
    params->GetParameter("Scale", 1.0f, &m_scale);
    params->GetParameter("Color", &_D3DVECTOR(40.0f, 40.0f, 40.0f), &color);
    params->GetParameter("AlphaEnabled", 0, &m_alphaEnabled);
    params->GetParameter("Alpha", 0xFF, &m_alpha);
    params->GetParameter("DepthEnabled", 0, &m_depthEnabled);
    params->GetParameter("Depth", 0.0f, &m_depth);
    if (params->GetParameter("HighlightColor", &_D3DVECTOR(255.0f, 255.0f, 255.0f), &highlight)) {
        m_hasHighlight = 1;
        m_highlightColor = PACK_COLOR(highlight);
        if (params->GetParameter("HighlightScale", 1.0f, &m_highlightScale))
            m_hasHighlightScale = 1;
    }
    ScreenTextFontItem *item = LpGame->m_fonts->GetScreenTextFont(m_fontName);
    m_font = item ? item->m_font : 0;
    m_color = PACK_COLOR(color);
    m_id = id;
    return this;
}

// ---- src/JetskiWidgetText/Render3D.cpp

int JetskiWidgetText::Render3D()
{
    if (m_x12C)
        RenderText();
    return GameObject::Render3D();
}

// ---- src/JetskiWidgetText/WidgetLocalize.cpp

// Sets ScreenText +0x4C (ParseMultiLine's depth) and marks the text dirty.
static inline void SetTextDepth(ScreenText *text, float depth)
{
    text->m_x4C = depth;
    text->m_dirty = 1;
}

int JetskiWidgetText::WidgetLocalize()
{
    char text[0x1F0];
    if (m_lines) {
        for (int i = 0; i < m_numLines; i++)
            delete m_lines[i];
        free(m_lines);
        m_lines = 0;
    }
    strcpy(text, LpGame->m_uiText->GetString(m_stringID));
    if (m_height) {
        ParseMultiLine(text, m_textManager);
    } else {
        m_numLines = 1;
        m_lines = (ScreenText **)malloc(m_numLines * sizeof(ScreenText *));
        m_lines[0] = new ScreenText(0x28, 0);
        if (m_maxChars)
            m_lines[0]->Create(m_maxChars, text);
        else
            m_lines[0]->Create(strlen(text), text);
        m_lines[0]->SetScale(m_scale);
        SetJustificationByValue(m_lines[0], m_justification);
        m_lines[0]->SetFont(m_font);
        m_lines[0]->SetColor(m_color);
        m_lines[0]->SetPosition(m_textX, m_textY);
        if (m_alphaEnabled)
            m_lines[0]->SetAlpha(m_alpha);
        if (m_depthEnabled)
            SetTextDepth(m_lines[0], m_depth);
        m_x12C = 1;
    }
    return 1;
}

// ---- src/JetskiWidgetText/ParseMultiLine.cpp

// One ScreenText per '@'-separated line.
void JetskiWidgetText::ParseMultiLine(char *string, ScreenTextManager *manager)
{
    char delim[] = "@";
    char *token;
    int line = -1;
    m_lines = 0;
    m_numLines = 0;
    token = strtok(string, delim);
    while (token) {
        m_numLines++;
        line++;
        m_lines = (ScreenText **)realloc(m_lines, m_numLines * sizeof(ScreenText *));
        m_lines[line] = new ScreenText(40, 0);
        if (m_maxChars)
            m_lines[line]->Create(m_maxChars, token);
        else
            m_lines[line]->Create(strlen(token), token);
        m_lines[line]->SetScale(m_scale);
        m_lines[line]->SetJustification(m_justification);
        m_lines[line]->SetFont(m_font);
        m_lines[line]->SetColor(m_color);
        m_lines[line]->SetPosition(m_textX, m_textY + m_height * line);
        if (m_alphaEnabled)
            m_lines[line]->SetAlpha(m_alpha);
        if (m_depthEnabled) {
            ScreenText *text = m_lines[line];
            text->m_x4C = m_depth;
            text->m_dirty = 1;
        }
        m_x12C = 1;
        token = strtok(0, delim);
    }
}

// ---- src/JetskiWidgetText/WidgetSelect.cpp

void JetskiWidgetText::WidgetSelect()
{
    UIWidget::WidgetSelect();
    m_x12C = 1;
    if (m_hasHighlight) {
        SetColor(m_highlightColor);
        if (m_hasHighlightScale) {
            for (int i = 0; i < m_numLines; i++) {
                ScreenText *line = m_lines[i];
                line->m_scale = m_highlightScale;
                line->m_dirty = 1;
                line->m_positionDirty = 1;
            }
        }
    }
}

// ---- src/JetskiWidgetText/WidgetDeSelect.cpp

void JetskiWidgetText::WidgetDeSelect()
{
    UIWidget::WidgetDeSelect();
    if (m_hasHighlight) {
        if (m_hasHighlightScale) {
            for (int i = 0; i < m_numLines; i++) {
                ScreenText *line = m_lines[i];
                line->m_scale = m_scale;
                line->m_dirty = 1;
                line->m_positionDirty = 1;
            }
        }
        SetColor(m_color);
    } else {
        m_x12C = 0;
    }
}

// ---- src/JetskiWidgetText/WidgetKeepEnabled.cpp

void JetskiWidgetText::WidgetKeepEnabled()
{
    UIWidget::WidgetKeepEnabled();
    SetColor(m_color);
}

// ---- src/JetskiWidgetText/RenderText.cpp

int JetskiWidgetText::RenderText()
{
    for (int i = 0; i < m_numLines; i++)
        m_textManager->AddText(m_lines[i], 0);
    return 1;
}

// ---- src/JetskiWidgetText/SetColor.cpp

void JetskiWidgetText::SetColor(unsigned int color)
{
    for (int i = 0; i < m_numLines; i++) {
        ScreenText *item = m_lines[i];
        item->m_color = color;
        item->m_x5D_bit4 = 1;
    }
}

// ---- src/JetskiWidgetTextMove/ctor.cpp

JetskiWidgetTextMove::JetskiWidgetTextMove(UIManager *manager, int enabled) : JetskiWidgetText(manager, enabled)
{
    m_xStart = 0;
    m_yStart = 0;
    m_x = 0;
    m_y = 0;
    m_speed = 0;
    m_progress = 0;
}

// ---- src/JetskiWidgetTextMove/dtor.cpp

JetskiWidgetTextMove::~JetskiWidgetTextMove()
{
}

// ---- src/JetskiWidgetTextMove/Create.cpp

JetskiWidgetTextMove *JetskiWidgetTextMove::Create(int id, ParameterBlock *params, LightManager *lights,
                                                   ScreenTextManager *textManager)
{
    JetskiWidgetText::Create(id, params, textManager);
    params->GetParameter("XStart", 0, &m_xStart);
    params->GetParameter("YStart", 0, &m_yStart);
    params->GetParameter("Speed", 0.0f, &m_speed);
    for (int i = 0; i < m_numLines; i++) {
        float y = m_yStart + i * m_font->GetHeight();
        ScreenText *text = m_lines[i];
        text->m_position.x = m_xStart;
        text->m_position.y = y;
        text->m_positionDirty = 1;
        text->m_dirty = 1;
    }
    return this;
}

// ---- src/JetskiWidgetTextMove/Tick.cpp

int JetskiWidgetTextMove::Tick(float dt)
{
    return GameObject::Tick(dt);
}

// ---- src/JetskiWidgetTextMove/Transition.cpp

void JetskiWidgetTextMove::Transition(int direction)
{
    m_x54 = direction;
    if (m_x54 == 1) {
        m_x = m_xStart;
        m_y = m_yStart;
    } else if (m_x54 == 2) {
        m_x = m_textX;
        m_y = m_textY;
    }
}

// ---- src/JetskiWidgetTextMove/ProcessTransition.cpp

// Step value toward target by step; sets done once it is there.
#define MOVE_TOWARD(value, target, step, done) \
    if (value == target) {                     \
        done = 1;                              \
    } else if (value > target) {               \
        value -= step;                         \
        if (value < target)                    \
            value = target;                    \
    } else if (value < target) {              \
        value += step;                         \
        if (value > target)                    \
            value = target;                    \
    }

int JetskiWidgetTextMove::ProcessTransition(float dt)
{
    int doneX = 0;
    int doneY = 0;
    if (m_x54) {
        m_progress += m_speed * dt;
        int step = (int)m_progress;
        if (step) {
            m_progress -= step;
            if (m_x54 == 1) {
                MOVE_TOWARD(m_x, m_textX, step, doneX);
                MOVE_TOWARD(m_y, m_textY, step, doneY);
                if (doneX && doneY)
                    m_x54 = 0;
            } else if (m_x54 == 2) {
                MOVE_TOWARD(m_x, m_xStart, step, doneX);
                MOVE_TOWARD(m_y, m_yStart, step, doneY);
                if (doneX && doneY)
                    m_x54 = 0;
            }
            for (int i = 0; i < m_numLines; i++) {
                m_lines[i]->SetPosition(m_x, m_y + i * m_font->GetHeight());
            }
        }
    }
    return m_x54 == 0;
}

// ---- src/JetskiWidgetHighlight/ctor.cpp

JetskiWidgetHighlight::JetskiWidgetHighlight(UIManager *manager, int enabled) : UIWidget(manager, enabled)
{
    m_iconManager = 0;
    m_icon = 0;
    m_deselectIcon = 0;
    m_x64 = 0;
    m_flashing = 0;
    m_flashTime = 0;
    m_timer = 0;
}

// ---- src/JetskiWidgetHighlight/dtor.cpp

JetskiWidgetHighlight::~JetskiWidgetHighlight()
{
    delete m_icon;
    delete m_deselectIcon;
}

// ---- src/JetskiWidgetHighlight/Create.cpp

JetskiWidgetHighlight *JetskiWidgetHighlight::Create(int id, ParameterBlock *params, UIManager *manager)
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
    m_id = id;
    m_manager = manager;
    m_iconManager = m_manager->m_iconManager;
    m_flashing = 0;
    params->GetParameter("HighlightTime", 0.0f, &m_flashTime);
    m_timer = 0;
    params->GetParameter("MAPKEY_LEFT", 0, &m_keyMap[0]);
    params->GetParameter("MAPKEY_RIGHT", 0, &m_keyMap[1]);
    params->GetParameter("MAPKEY_UP", 0, &m_keyMap[2]);
    params->GetParameter("MAPKEY_DOWN", 0, &m_keyMap[3]);
    params->GetParameter("MAPKEY_X", 0, &m_keyMap[4]);
    params->GetParameter("MAPKEY_SQUARE", 0, &m_keyMap[5]);
    params->GetParameter("MAPKEY_TRIANGLE", 0, &m_keyMap[6]);
    params->GetParameter("MAPKEY_CIRCLE", 0, &m_keyMap[7]);
    params->GetParameter("MAPKEY_START", 0, &m_keyMap[8]);
    params->GetParameter("MAPKEY_SELECT", 0, &m_keyMap[9]);
    params->GetParameter("MAPKEY_L1", 0, &m_keyMap[10]);
    params->GetParameter("MAPKEY_R1", 0, &m_keyMap[11]);
    params->GetParameter("MAPKEY_L2", 0, &m_keyMap[12]);
    params->GetParameter("MAPKEY_R2", 0, &m_keyMap[13]);
    params->GetParameter("MAPKEY_L3", 0, &m_keyMap[14]);
    params->GetParameter("MAPKEY_R3", 0, &m_keyMap[15]);
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
    params->GetParameter("AlphaEnabled", 0, &alphaEnabled);
    params->GetParameter("Alpha", 0xFF, &alpha);
    params->GetParameter("DepthEnabled", 0, &depthEnabled);
    params->GetParameter("Depth", 0.0f, &depth);
    if (depth < 0.0f)
        depth = LpVideoCard->m_maxDepth - 300;

    if (params->GetParameter("PartialRectSelect", &none, &partial))
        hasPartial = 1;
    if (!params->GetParameter("MapRectSelect", &none, &map)) {
        params->GetParameter("XLocationSelect", 0, (int *)&map.left);  // (cast: retail argument order)
        params->GetParameter("YLocationSelect", 0, &map.top);
        map.right = map.left + (partial.right - partial.left);
        map.bottom = map.top + (partial.bottom - partial.top);
    }
    params->GetParameter("SelectTexture", "*INVALID*", texture, sizeof(texture));
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

    if (params->GetParameter("PartialRectDeselect", &none, &partial))
        hasPartial = 1;
    if (!params->GetParameter("MapRectDeselect", &none, &map)) {
        params->GetParameter("XLocationDeselect", 0, (int *)&map.left);  // (cast: retail argument order)
        params->GetParameter("YLocationDeselect", 0, &map.top);
        map.right = map.left + (partial.right - partial.left);
        map.bottom = map.top + (partial.bottom - partial.top);
    }
    m_deselectIcon = m_icon;            // (overwritten below; as in retail)
    params->GetParameter("DeselectTexture", "*INVALID*", texture, sizeof(texture));
    sprintf(path, "%s%s", UITextureContentDirectory, texture);
    m_deselectIcon = (new ScreenIcon)->Create(path, 0.0f, 0.0f);
    m_deselectIcon->SetPosition(map.left, map.top);
    m_deselectIcon->SetWidth(map.right - map.left);
    m_deselectIcon->SetHeight(map.bottom - map.top);
    m_deselectIcon->m_alphaTest = alphaEnabled;
    m_deselectIcon->SetAlpha(alpha);
    m_deselectIcon->m_zBuffer = depthEnabled;
    m_deselectIcon->SetDepth(depth);
    if (angle > 0.0f) {
        m_deselectIcon->SetAngle(angle);
        m_deselectIcon->SetPivot(pivotX, pivotY);
    }
    if (hasPartial)
        m_deselectIcon->SetUVRect(partial.left, partial.right, partial.top, partial.bottom);

    m_selected = 1;
    m_xC0.x = map.left;
    m_xD0.x = map.right;
    m_xC0.y = m_xD0.y = map.top + (map.bottom - map.top) / 2.0f;
    return this;
}

// ---- src/JetskiWidgetHighlight/Render3D.cpp

int JetskiWidgetHighlight::Render3D()
{
    if (m_flashing)
        m_iconManager->AddIcon(m_icon, 0);
    else
        m_iconManager->AddIcon(m_deselectIcon, 0);
    return GameObject::Render3D();
}

// ---- src/JetskiWidgetHighlight/WidgetSelect.cpp

void JetskiWidgetHighlight::WidgetSelect()
{
    UIWidget::WidgetSelect();
}

// ---- src/JetskiWidgetHighlight/WidgetDeSelect.cpp

void JetskiWidgetHighlight::WidgetDeSelect()
{
    UIWidget::WidgetDeSelect();
}

// ---- src/JetskiWidgetHighlight/Initialize.cpp

void JetskiWidgetHighlight::Initialize()
{
    UIWidget::Initialize();
    m_flashing = 0;
    m_timer = 0;
}

// ---- src/JetskiWidgetHighlight/Tick.cpp

int JetskiWidgetHighlight::Tick(float dt)
{
    if (m_flashing) {
        if (m_timer < m_flashTime) {
            m_timer += dt;
        } else {
            m_flashing = !m_flashing;
            m_timer = 0;
        }
    }
    return GameObject::Tick(dt);
}

// ---- src/JetskiWidgetHighlight/WidgetKeyDownHandler.cpp
// WidgetKeyDownHandler__21JetskiWidgetHighlightFP11ProcessNodeP7KeyInfoii  @ 0x00328470
// Retail's unrotated loop and its single padding nop before the epilogue label come from
// an asm nop after the loop (it also turns the global optimizer off) plus
// `#pragma alignlabel off`. The player flag is read through `(int *)this + player`
// (retail: `addu a2, a0, v1; lw 0x6C(a2)`): a plain `m_x6C[player]` gives
// `addu idx, this`, and `&m_x6C[player]` adds the 0x6C separately. Declaring the
// counter before that pointer puts them in $a1/$a2 as retail has them.

#pragma alignlabel off
void JetskiWidgetHighlight::WidgetKeyDownHandler(ProcessNode *node, KeyInfo *key, int a, int player)
{
    unsigned int i;
    int *flags = (int *)this + player;   // flags[0x6C / 4] is m_keyMap[player]
    for (i = 0; i < 64; i++) {
        if (flags[0x6C / 4]) {
            m_flashing = 1;
            m_timer = 0;
            JetskiUIManager *holder = (JetskiUIManager *)m_manager;
            if (!holder->m_waterEffect)
                return;
            static float x = 0.0f;
            static float z = 0.0f;
            static float ftps = 12.0f;
            static float forceScale = 1.0f;
            x = (m_xC0.x + m_xD0.x) / 2.0f;
            z = (m_xC0.y + m_xD0.y) / 2.0f;
            holder->m_waterEffect->ApplyExternalForce(forceScale, x, z);
            return;
        }
    }
    asm("nop");     // (retail has a lone nop here)
}
#pragma alignlabel reset

// ---- src/sinit/JetskiUIWidgets.cpp
// __sinit_JetskiUIWidgets.cpp builds only the axis vectors (sinit/AxisVectors.h, included above).
