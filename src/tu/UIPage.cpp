// UIPage.cpp  @ 0x0031D4F0 (text 0x31D4F0-0x31E890)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's UIPage.cpp as one translation unit: UIPage's functions from its
// ctor on, then the static initializer. Its .rodata runs @1006 .. the
// UIPage RTTI. UIPage's four one-line virtuals (ResetPage,
// ProcessTransition, Transition, ProcessUserInput) sit earlier, in
// UIManager.cpp and UIWidgets.cpp, and CameraController.cpp follows.



#include "UIPage/UIPage.h"
#include "TextureMap/TextureMap.h"
#include "GameObject/GameObject.h"
#include "ParameterBlock/ParameterBlock.h"
#include "UIManager/UIManager.h"
#include "RenderTarget/RenderTarget.h"
#include "PSXMediaControl/PSXMediaControl.h"
#include "UIGroup/UIGroup.h"
#include "soultree_assert_block.h"
#include "MediaControl/MediaControl.h"
#include "UIControl/UIControl.h"
#include "UIWidget/UIWidget.h"
#include "UIWidgetEventHandler/UIWidgetEventHandler.h"
#include "sinit/AxisVectors.h"

// ---- src/UIPage/ctor.cpp

UIPage::UIPage(int enabled)
    : GameObject(enabled)
{
    m_numStaticWidgets = 0;
    m_textManager = 0;
    m_numControls = 0;
    m_controls = 0;
    m_eventHandler = 0;
    m_numGroups = 0;
    m_groups = 0;
    m_pageNo = 0;
    m_states = 0;
    m_background = 0;
    m_videoPlayer = 0;
    m_videoPlayer = 0;  // (retail stores it twice)
}


// ---- src/UIPage/dtor.cpp

extern "C" void free(void *p);


UIPage::~UIPage()
{
    int i;
    if (m_background)
        m_background->Release();
    for (i = 0; i <= m_numControls; i++) {
        if (m_controls[i])
            m_controls[i]->Release();
        m_controls[i] = 0;
    }
    if (m_controls)
        free(m_controls);
    for (i = 0; i <= m_numGroups; i++) {
        if (m_groups[i])
            m_groups[i]->Release();
    }
    if (m_groups)
        free(m_groups);
    for (i = 0; i < 2; i++) {
        if (m_states[i])
            delete m_states[i];
    }
    if (m_states)
        delete[] m_states;
}


// ---- src/UIPage/Create.cpp

extern "C" int sprintf(char *buf, const char *fmt, ...);
extern "C" char *strcpy(char *dst, const char *src);
extern "C" void *malloc(unsigned int size);
void PSXprintf(const char *fmt, ...);

class ScreenTextManager;
class UIWidget;


extern RenderTarget *LpRenderTarget;

UIPage *UIPage::Create(char *name, char *fileName, UIManager *manager, ScreenTextManager *textManager)
{
    char blockName[0x80];
    char background[0x110];
    int id;
    int useVideo;
    int numWidgets;
    int numControls;
    int numAssociations;
    UIControl *control;
    UIGroup *group;
    int i;
    int j;

    m_manager = manager;
    m_textManager = textManager;
    LpRenderTarget->m_hasDepth = 0;
    strcpy(m_name, name);
    ParameterBlock block;
    if (!block.OpenFile(fileName, 0)) {
        PSXprintf("Failed to load ui page file (.dat) UIPage::Create\n");
        return 0;
    }
    block.ReadParameterBlock("PageInfo");
    block.GetParameter("Background", "*INVALID*", background, 0x104);
    useVideo = 0;
    block.GetParameter("UseVideoAsTexture", 0, &useVideo);
    SetBackground(background, useVideo);
    block.GetParameter("PageNo", 0, &m_pageNo);
    m_eventHandler = (new UIWidgetEventHandler(1))->Create(this, m_manager);
    AddChild(m_eventHandler, -1);
    block.GetParameter("NumberOfStaticWidgets", 0, &m_numStaticWidgets);
    block.GetParameter("NumberOfControls", 0, &m_numControls);
    m_controls = (UIControl **)malloc((m_numControls + 1) * PS2_PTR_SIZE);
    block.GetParameter("NumberOfGroups", 0, &m_numGroups);
    m_groups = (UIGroup **)malloc((m_numGroups + 1) * PS2_PTR_SIZE);
    m_states = new UIPageState *[2];
    for (i = 0; i < 2; i++) {
        m_states[i] = new UIPageState;
        m_states[i]->m_group = 0;
        m_states[i]->m_groupNumber = 0;
    }
    for (i = 0; i <= m_numControls; i++) {
        m_controls[i] = new UIControl(1);
        control = m_controls[i];
        sprintf(blockName, "Control %d", i);
        control->m_on = block.ReadParameterBlock(blockName);
        if (control->m_on) {
            block.GetParameter("NumberOfWidgets", 0, &numWidgets);
            control->m_numWidgets = numWidgets;
            if (numWidgets > 0) {
                control->m_widgets = (UIWidget **)malloc(numWidgets * PS2_PTR_SIZE);
                for (j = 0; j < numWidgets; j++) {
                    sprintf(blockName, "WidgetID%d", j + 1);
                    block.GetParameter(blockName, 0, &id);
                    control->m_widgets[j] = m_manager->m_widgets[id - 1];
                    if (control->m_widgets[j])
                        m_eventHandler->AddToJoystickEventList(control->m_widgets[j]);
                }
            }
        }
    }
    for (i = 0; i <= m_numGroups; i++) {
        m_groups[i] = new UIGroup(1);
        group = m_groups[i];
        group->m_on = 1;
        sprintf(blockName, "Group %d", i);
        block.ReadParameterBlock(blockName);
        block.GetParameter("NumberOfControls", 0, &numControls);
        group->m_numControls = numControls;
        if (numControls > 0) {
            group->m_controls = (UIControl **)malloc(numControls * PS2_PTR_SIZE);
            for (j = 0; j < numControls; j++) {
                sprintf(blockName, "ControlID%d", j + 1);
                block.GetParameter(blockName, -1, &id);
                if (id != -1)
                    group->m_controls[j] = m_controls[id];
                else
                    group->m_controls[j] = 0;
            }
            group->m_selected = 1;
            group->m_selectedControl = group->m_controls[group->m_selected - 1];
        } else {
            group->m_controls = 0;
            group->m_selected = 0;
            group->m_selectedControl = 0;
        }
        block.GetParameter("NumberOfAssociations", 0, &numAssociations);
        group->m_numOthers = numAssociations;
        if (numAssociations > 0) {
            group->m_others = (UIControl **)malloc(numAssociations * PS2_PTR_SIZE);
            for (j = 0; j < numAssociations; j++) {
                sprintf(blockName, "AssociationID%d", j + 1);
                block.GetParameter(blockName, 0, &id);
                group->m_others[j] = m_controls[id];
            }
        } else {
            group->m_others = 0;
        }
        block.GetParameter("KeepEnabled", 1, &group->m_keepEnabled);
    }
    return this;
}


// ---- src/UIPage/Initialize.cpp

void UIPage::Initialize()
{
}


// ---- src/UIPage/SetBackground.cpp

extern "C" int sprintf(char *buf, const char *fmt, ...);
extern "C" char *strstr(const char *s, const char *find);

class Palette8;
TextureMap *GetTextureMapFromResourceName(char *name, int flags, Palette8 *palette, unsigned int a,
                                          _D3DBLEND src, _D3DBLEND dst, int alpha, unsigned int colorKey, int b);

extern char UITextureContentDirectory[];


extern RenderTarget *LpRenderTarget;


// A ".tga" name loads a still background, a ".pss" name plays a movie and
// "*" means none. Without a background the render target clears the screen.
int UIPage::SetBackground(char *name, int loop)
{
    char path[0x110];
    char video[0x40];
    sprintf(path, "%s%s", UITextureContentDirectory, name);
    if (m_background) {
        m_background->Release();
        m_background = 0;
    }
    if (m_videoPlayer) {
        m_videoPlayer->Stop();
        m_videoPlayer->RemoveFromChain();
        m_videoPlayer->Release();
        m_videoPlayer = 0;
    }
    if (*name != '*') {
        if (strstr(path, ".tga")) {
            m_background = GetTextureMapFromResourceName(path, 0x22B8, 0, 0, (_D3DBLEND)5, (_D3DBLEND)6, 0x80,
                                                         0xFF00FF, 1);
        } else if (strstr(path, ".pss")) {
            sprintf(video, "%s", path);
            CreateVideoPlayer();
            if (m_videoPlayer)
                m_videoPlayer->Create(video, loop, 0, 0, 0, 0, 0);
        }
        if (m_background || m_videoPlayer) {
            LpRenderTarget->m_hasDepth = 0;
            return 1;
        }
    } else {
        m_background = 0;
        m_videoPlayer = 0;
    }
    LpRenderTarget->m_hasDepth = 1;
    return 0;
}


// ---- src/UIPage/PageSelect.cpp


extern RenderTarget *LpRenderTarget;

void UIPage::PageSelect()
{
    int i;
    EnableAll();
    for (i = 0; i < 2; i++) {
        m_states[i]->m_groupNumber = 0;
        m_states[i]->m_group = 0;
    }
    for (i = 0; i <= m_numGroups; i++) {
        if (m_groups[i])
            m_groups[i]->Initialize();
    }
    if (!m_background && !m_videoPlayer)
        LpRenderTarget->m_hasDepth = 1;
    else
        LpRenderTarget->m_hasDepth = 0;
}


// ---- src/UIPage/PageDeSelect.cpp

extern RenderTarget *LpRenderTarget;


void UIPage::PageDeSelect()
{
    DisableAll();
    for (int i = 0; i <= m_numGroups; i++) {
        if (m_groups[i])
            m_groups[i]->DisableWidgets();
    }
    LpRenderTarget->m_hasDepth = 0;
}


// ---- src/UIPage/NextGroup.cpp


void UIPage::NextGroup(int state)
{
    int groupNumber = m_states[state]->m_groupNumber;
    UIGroup *group = m_states[state]->m_group;
    if (group) {
        group->DeSelect();
        do {
            groupNumber++;
            if (groupNumber > m_numGroups)
                groupNumber = 1;
            group = m_groups[groupNumber];
        } while (!group->m_on);
        group->Select();
        group->SetFocus(group->m_selected);
        m_states[state]->m_groupNumber = groupNumber;
        m_states[state]->m_group = group;
    }
}


// ---- src/UIPage/PreviousGroup.cpp


void UIPage::PreviousGroup(int state)
{
    int groupNumber = m_states[state]->m_groupNumber;
    UIGroup *group = m_states[state]->m_group;
    if (group) {
        group->DeSelect();
        do {
            groupNumber--;
            if (groupNumber < 1)
                groupNumber = m_numGroups;
            group = m_groups[groupNumber];
        } while (!group->m_on);
        group->Select();
        group->SetFocus(group->m_selected);
        m_states[state]->m_groupNumber = groupNumber;
        m_states[state]->m_group = group;
    }
}


// ---- src/UIPage/SelectGroup.cpp


void UIPage::SelectGroup(int state, int newGroupNumber)
{
#line 424 "UIPage.cpp"
    ASSERT(newGroupNumber >= 0, "newGroupNumber cannot be negative");
    if (m_states[state]->m_group) {
        UIGroup *group = m_groups[newGroupNumber];
        if (group && group->m_on) {
            m_states[state]->m_group->DeSelect();
            group->Select();
            group->SetFocus(group->m_selected);
            m_states[state]->m_groupNumber = newGroupNumber;
            m_states[state]->m_group = group;
        }
    }
}


// ---- src/UIPage/Tick.cpp


int UIPage::Tick(float dt)
{
    if (m_videoPlayer && !m_videoPlayer->IsPlaying())
        m_videoPlayer->Start();
    return GameObject::Tick(dt);
}


// ---- src/UIPage/SetGroupFocus.cpp


void UIPage::SetGroupFocus(int group, int focus)
{
    m_groups[group]->SetFocus(focus);
}


// ---- src/UIPage/SetPlayerFocus.cpp


void UIPage::SetPlayerFocus(int group, int control, int player)
{
    // Deselect groups no player is on. (Retail compares against i + 1.)
    int i, j;
    for (i = 1; i <= m_numGroups; i++) {
        int unused = 1;
        for (j = 0; j < 2; j++) {
            if (m_states[j]->m_groupNumber == i + 1)
                unused = 0;
        }
        if (unused)
            m_groups[i]->DeSelect();
    }
    UIGroup *newGroup = m_groups[group];
    newGroup->Select();
    newGroup->SetFocus(control);
    m_states[player]->m_group = m_groups[group];
    m_states[player]->m_groupNumber = group;
}


// ---- src/UIPage/EnableGroup.cpp


void UIPage::EnableGroup(int group)
{
    m_groups[group]->TurnOnOff(1);
    m_groups[group]->EnableWidgets();
}


// ---- src/UIPage/EnableGroupControl.cpp


// control is 1-based.
void UIPage::EnableGroupControl(int group, int control)
{
    m_groups[group]->m_controls[control - 1]->TurnOnOff(1);
    m_groups[group]->m_controls[control - 1]->EnableWidgets();
}


// ---- src/UIPage/EnableGroupControlWidget.cpp


// control and widget are 1-based.
void UIPage::EnableGroupControlWidget(int group, int control, int widget)
{
    m_groups[group]->m_controls[control - 1]->m_widgets[widget - 1]->TurnOnOff(1);
}


// ---- src/UIPage/DisableGroup.cpp


void UIPage::DisableGroup(int group)
{
    m_groups[group]->TurnOnOff(0);
    m_groups[group]->DisableWidgets();
}


// ---- src/UIPage/DisableGroupControl.cpp


// control is 1-based.
void UIPage::DisableGroupControl(int group, int control)
{
    m_groups[group]->m_controls[control - 1]->TurnOnOff(0);
    m_groups[group]->m_controls[control - 1]->DisableWidgets();
}


// ---- src/UIPage/DisableGroupControlWidget.cpp


// control and widget are 1-based.
void UIPage::DisableGroupControlWidget(int group, int control, int widget)
{
    m_groups[group]->m_controls[control - 1]->m_widgets[widget - 1]->TurnOnOff(0);
}


// ---- src/UIPage/TurnGroupOnOff.cpp
int UIPage::TurnGroupOnOff(int on, int group)
{
    m_groups[group]->TurnOnOff(on);
    return 1;
}


// ---- src/UIPage/TurnGroupControlOnOff.cpp

// control is 1-based.
int UIPage::TurnGroupControlOnOff(int on, int group, int control)
{
    m_groups[group]->m_controls[control - 1]->TurnOnOff(on);
    return 1;
}


// ---- src/UIPage/RenderPre3D.cpp


int UIPage::RenderPre3D()
{
    if (m_background)
        LpRenderTarget->Blt(0, m_background, 0, 0);
    return GameObject::RenderPre3D();
}


// ---- src/UIPage/CreateVideoPlayer.cpp


int UIPage::CreateVideoPlayer()
{
    if (!m_videoPlayer) {
        m_videoPlayer = new PSXMediaControl(1);
        if (m_videoPlayer) {
            AddChild(m_videoPlayer, -1);
            return 1;
        }
        return 0;
    }
    return 0;
}


// ---- src/UIPage/GetJoystickGroupID.cpp

int UIPage::GetJoystickGroupID(int joystick)
{
    return m_states[joystick]->m_groupNumber;
}


// ---- src/UIPage/GetGroupControlID.cpp


int UIPage::GetGroupControlID(int group)
{
    return m_groups[group]->m_selected;
}


// ---- src/UIPage/GetWidgetPtr.cpp

// group and widget are 1-based.
UIWidget *UIPage::GetWidgetPtr(int group, int control, int widget)
{
    return m_groups[group]->m_controls[control - 1]->m_widgets[widget - 1];
}



// ---- src/sinit/UIPage.cpp
// Static initializer of UIPage.cpp: only the axis vectors (included at the
// top: retail's .bss has them first).
