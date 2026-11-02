// JetskiUIManagers.cpp  @ 0x003690F0 (text 0x3690F0-0x36CC00)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// JetskiUIManager, JetskiUIManMain and JetskiUIManCard as one translation unit
// (its static initializer only builds the axis vectors), unanchored inside
// JetskiPracticeCompetition.cpp's tu_map row: its .rodata restarts at @968
// after the JetskiUIPages.cpp strings.
//
// Two session shims, each a 0x30000-byte all-zero heap image right before its
// function (mwcc puts it in a new first heap block, so the function parses in
// a fresh block): ReadFogData's and JetskiUIManMain::Create's. ReadFogData
// also needs Fog.h to be the TU's first include. mwcc materialises
// Fog::Create's constants in the order of their parser nodes' addresses: the
// 900/200 default-argument nodes are made when Fog.h is parsed, the 1200 node
// in ReadFogData's own epoch. Only with Fog.h first and the image before
// ReadFogData does retail's order come out (900 -> $f15, 200 -> $f16, then
// 1200 -> $f14). Fog.h anywhere else, or no image, gives 1200 first.




#include "Fog/Fog.h"
#include "JetskiUIManager/JetskiUIManager.h"
#include "SearchList/SearchList.h"
#include "LightManager/LightManager.h"
#include "GameObject/GameObject.h"
#include "ParameterBlock/ParameterBlock.h"
#include "JetskiLoadManager/JetskiLoadManager.h"
#include "JetskiGame/JetskiGame.h"
#include "MaterialStringTable/MaterialStringTable.h"
#include "TextureMapStringTable/TextureMapStringTable.h"
#include "ScreenPrimitiveManager/ScreenPrimitiveManager.h"
#include "SearchNode/SearchNode.h"
#include "PSXUIWaterEffect/PSXUIWaterEffect.h"
#include "JetskiGameOptions/JetskiGameOptions.h"
#include "UIPage/UIPage.h"
#include "ScreenTextManager/ScreenTextManager.h"
#include "ScreenIconManager/ScreenIconManager.h"
#include "UIWidget/UIWidget.h"
#include "UIManager/UIManager.h"
#include "soultree_assert_block.h"
#include "Game/Game.h"
#include "d3dtypes.h"
#include "LightEmitter/_D3DCOLORVALUE.h"
#include "LightEmitter/LightEmitter.h"
#include "JetskiUIManMain/JetskiUIManMain.h"
#include "RainbowPS2Snd/RainbowPS2Snd.h"
#include "PSXSoundInterface/PSXSoundInterface.h"
#include "AuralScape/AuralScape.h"
#include "JetskiCheatKey/JetskiCheatKey.h"
#include "ScreenIcon/ScreenIcon.h"
#include "_DS3DBUFFER/_DS3DBUFFER.h"
#include "DataAccess/DataAccess.h"
#include "PSXSoundGroup/PSXSoundGroup.h"
#include "PSXSound/PSXSound.h"
#include "ResponseEmitter/ResponseEmitter.h"
#include "JetskiUIRiderSoundEmitter/JetskiUIRiderSoundEmitter.h"
#include "JetskiUIManCard/JetskiUIManCard.h"
#include "UIWidgetEventHandler/UIWidgetEventHandler.h"
#include "UICamera/UICamera.h"
#include "LightData/LightData.h"
#include "JetskiWidgetButton/JetskiWidgetButton.h"
#include "JetskiWidgetTextMove/JetskiWidgetTextMove.h"
#include "JetskiWidgetLap/JetskiWidgetLap.h"
#include "JetskiWidgetCharacter/JetskiWidgetCharacter.h"
#include "JetskiWidgetSeadoo/JetskiWidgetSeadoo.h"
#include "JetskiWidgetText/JetskiWidgetText.h"
#include "JetskiWidgetHighlight/JetskiWidgetHighlight.h"
#include "UIWidgetMovementControl/UIWidgetMovementControl.h"
#include "UIWidgetIcon/UIWidgetIcon.h"
#include "UIWidgetMovementGroup/UIWidgetMovementGroup.h"
#include "JetskiUIPage/JetskiUIPages.h"
#include "JetskiUIPage/JetskiMemPages.h"
#include "sinit/AxisVectors.h"
#include "JetskiPageSettings/JetskiPageSettings.h"
#include "JetskiManagerSettingsMain/JetskiManagerSettingsMain.h"

extern LightManager *LpLightManager;
extern "C" char *strcpy(char *dst, const char *src);
extern "C" int sprintf(char *buf, const char *fmt, ...);
void PSXprintf(const char *fmt, ...);
class SearchMethod;
extern char UIContentDirectory[];
extern JetskiGame *LpGame;
class LightManager;
extern MaterialStringTable *LpMaterialStringTable;
extern TextureMapStringTable *LpTextureMapStringTable;
class Fog;
extern "C" void *malloc(unsigned int size);
class KeyInfo;
class ProcessNode;
class UIGlobalManager;
class UIPage;
class ScreenTextManager;
class SearchNode;
struct ObjectEnumerator;
extern ObjectEnumerator LightEmitterEnumerator[];
extern "C" char *strtok(char *s, const char *delim);
extern "C" int atoi(const char *s);
extern "C" void free(void *p);
extern "C" char *strcat(char *dst, const char *src);
extern char UITextureContentDirectory[];
extern DataAccess *LpDataAccess;
class UIManager;
class ScreenPrimitiveManager;
class Camera;

// ---- src/JetskiUIManager/ctor.cpp
JetskiUIManager::JetskiUIManager(int enabled)
    : UIManager(enabled)
{
    m_waterEffect = 0;
    m_lights = 0;
    m_primitives = 0;
    m_searchList = 0;
    m_searchNode = 0;
    m_params = 0;
    m_fog = 0;
    m_camera = 0;
    m_tuneSound = 0;
    m_negativeSound = 0;
    m_heliSound = 0;
    m_responses = 0;
    m_soundGroup = 0;
    m_musicGroup = 0;
    m_riderGroup = 0;
}

// ---- src/JetskiUIManager/dtor.cpp
JetskiUIManager::~JetskiUIManager()
{
    LpLightManager->ReleaseLightGraph();
    if (m_lights)
        delete[] m_lights;
    if (m_searchList)
        m_searchList->Release();
}

// ---- src/JetskiUIManager/Create.cpp
JetskiUIManager *JetskiUIManager::Create(char *name, int page, UIGlobalManager *global, int water)
{
    char path[0x70];
    strcpy(m_name, name);
    m_page = page;
    m_params = new ParameterBlock;
    sprintf(path, "%sui.dat", UIContentDirectory);
    if (!m_params->OpenFile(path, 0)) {
        PSXprintf("Failed to load ui.dat in UIManager::LoadPages\n");
        return 0;
    }
    LpGame->m_loadManager->AddProgress(0.2f);
    m_primitives = new ScreenPrimitiveManager(1);
    m_primitives = m_primitives->Create(0x80);
    m_searchList = new SearchList(0x400);
    m_searchNode = new SearchNode(m_enabled);
    m_searchNode->m_x48 = 0;
    AddChild(m_searchNode, -1);
    m_searchNode->SetStaticSearchMethod((SearchMethod *)m_searchList);
    m_searchNode->SetDynamicSearchMethod((SearchMethod *)m_searchList);
    if (!LoadScene())
        return 0;
    if (!LoadFog())
        return 0;
    if (!LoadCamera())
        return 0;
    UIManager::Create(global);
    if (!LoadWidgets())
        return 0;
    LpGame->m_loadManager->AddProgress(0.5f);
    if (!LoadPages())
        return 0;
    if (!AttachScene())
        return 0;
    if (water) {
        m_waterEffect = new PSXUIWaterEffect(1);
        m_waterEffect = m_waterEffect->Create("UIWaterGradient", 0x50);
        if (m_waterEffect) {
            m_waterEffect->SetLightManager(LpLightManager);
            if (m_child)
                m_waterEffect->InsertBefore(m_child, -1);
            else
                AddChild(m_waterEffect, -1);
            LoadRequestedMaterials();
            PSXprintf("UIWaterEffect was CREATED\n");
        } else {
            PSXprintf("UIWaterEffect was NOT CREATED\n");
        }
    }
    delete m_params;
    m_params = 0;
    AddChild(m_primitives, -1);
    LpMaterialStringTable->LoadRequested();
    LpTextureMapStringTable->LoadRequested();
    Localize();
    LpGame->m_loadManager->AddProgress(0.3f);
    return this;
}

// ---- src/JetskiUIManager/Tick.cpp
int JetskiUIManager::Tick(float elapsed)
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
            if (m_currentPage->ProcessTransition(elapsed)) {
                if (m_direction == 2)
                    m_state = 9;
                m_direction = 0;
            }
            break;
        case 4:
            if (m_currentPage->ProcessTransition(elapsed)) {
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
            if (m_currentPage->ProcessTransition(elapsed)) {
                if (m_direction == 2)
                    m_state = 10;
                m_direction = 0;
            }
            break;
        case 9:
            LpGame->m_gameOptions->SetUI(m_x168, m_nextPage);
            LpGame->m_loadManager->SetStatus(3, 0, 0);
            m_state = 0;
            m_closing = 1;
            m_x178 = 0;
            break;
        case 10:
            LpGame->m_loadManager->SetStatus(4, 0, 0);
            m_state = 0;
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
    return GameObject::Tick(elapsed);
}

// ---- src/JetskiUIManager/LoadFog.cpp
int JetskiUIManager::LoadFog()
{
    ReadFogData();
    if (!m_fog) {
        PSXprintf("Failed to create LpFog in PodUIManager::LoadFog\n");
        return 0;
    }
    return 1;
}

// ---- src/JetskiUIManager/AttachScene.cpp
int JetskiUIManager::AttachScene()
{
    if (!AddChild(m_camera, -1)) {
        PSXprintf("Failed to attach LpUICameras in UIManager::AttachScene\n");
        return 0;
    }
    if (m_fog) {
        if (!AddChild(m_fog, -1)) {
            PSXprintf("Failed to attach LpFog in UIManager::AttachScene\n");
            return 0;
        }
    } else {
        PSXprintf("Cannot attach LpFog, it was never created.\n");
    }
    AttachWidgets();
    AttachPages();
    if (m_iconManager)
        AddChild(m_iconManager, -1);
    AddChild(m_textManager, -1);
    return 1;
}

// ---- src/JetskiUIManager/LoadWidgets.cpp
int JetskiUIManager::LoadWidgets()
{
    char file[0x100];
    UIWidget *widget;
    int ok;
    int i;
    int j;
    ok = 1;
    sprintf(file, "%sWidgets.Bpb", UIContentDirectory);
    ParameterBlock params;
    int typeID;
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
        params.GetParameter("TypeID", 0, &typeID);
        widget = GetWidget(&params, typeID);
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

// ---- src/JetskiUIManager/GetWidget.cpp
UIWidget *JetskiUIManager::GetWidget(ParameterBlock *block, int type)
{
    char name[100];
    char path[0x70];
    UIWidget *widget = 0;
    switch (type) {
    case -1: {
        JetskiWidgetButton *button = new JetskiWidgetButton(this, 1);
        widget = (UIWidget *)AddChild(button->Create(type, block, this, m_textManager), -1);
        break;
    }
    case -3: {
        JetskiWidgetTextMove *text = new JetskiWidgetTextMove(this, 1);
        widget = (UIWidget *)AddChild(text->Create(type, block, LpLightManager, m_textManager), -1);
        break;
    }
    case -4: {
        JetskiWidgetLap *lap = new JetskiWidgetLap(this, 1);
        widget = (UIWidget *)AddChild(lap->Create(type, block, m_textManager), -1);
        break;
    }
    case -5: {
        block->GetParameter("MCF", name, 100);
        sprintf(path, "Riders\\%s", name);
        JetskiWidgetCharacter *character = new JetskiWidgetCharacter(this, 1);
        widget = (UIWidget *)AddChild(character->Create(type, path, block), -1);
        break;
    }
    case -6: {
        block->GetParameter("SLT", name, 100);
        JetskiWidgetSeadoo *seadoo = new JetskiWidgetSeadoo(this, 1);
        widget = seadoo->Create(type, name, block);
        break;
    }
    case -2: {
        JetskiWidgetText *text = new JetskiWidgetText(this, 1);
        widget = (UIWidget *)AddChild(text->Create(type, block, m_textManager), -1);
        break;
    }
    case -7: {
        JetskiWidgetHighlight *highlight = new JetskiWidgetHighlight(this, 1);
        widget = highlight->Create(type, block, this);
        break;
    }
    case 5: {
        UIWidgetIcon *icon = new UIWidgetIcon(this, 1);
        widget = icon->Create(type, block, this);
        break;
    }
    case 9: {
        UIWidgetMovementControl *control = new UIWidgetMovementControl(this, 1);
        widget = control->Create(type, block, this);
        break;
    }
    case 10: {
        UIWidgetMovementGroup *group = new UIWidgetMovementGroup(this, 1);
        widget = group->Create(type, block, this);
        break;
    }
    }
    if (!widget) {
#line 595 "JetskiUIManagers.cpp"
        ASSERT(0, "Failed to load Widget in JetskiUIManager::GetWidget");
    }
    return widget;
}

// ---- src/JetskiUIManager/LoadCamera.cpp
int JetskiUIManager::LoadCamera()
{
    int ok = 1;
    UICamera *camera = new UICamera(1);
    m_camera = camera;
    if (!camera->Create()) {
        PSXprintf("Failed to create LpUICameras in UIManager::LoadCamera\n");
        ok = 0;
    }
    _D3DVECTOR position(384.0f, 1056.0f, 256.0f, 1.0f);
    _D3DVECTOR lookAt(0.0f, -1.0f, 0.0f, 1.0f);
    _D3DVECTOR up(0.0f, 0.0f, 1.0f, 1.0f);
    float fov;
    float x1D4;
    x1D4 = 0.0f;
    fov = 35.0f;
    m_camera->SetViewParameters(&position, &lookAt, &up, &x1D4, &fov);
    m_camera->AddSearchPlacement(m_searchNode, 2);
    if (m_camera)
        m_camera->Enable();
    LpGame->SetCameras(m_camera, 0, 0, 0);
    return ok;
}

// ---- src/JetskiUIManager/LoadScene.cpp
// Retail has no label padding in this function, but one lone nop after the
// AddSearchPlacement call (before the loop increment): an explicit __I_nop(),
// probably left by a debug macro. With label padding on, mwcc would pad every
// label in the loop and lose the movz for the name ternary.
#pragma alignlabel off

int JetskiUIManager::LoadScene()
{
    int numLights;
    LpLightManager->CreateLightGraph(1);
    m_params->ReadParameterBlock("Lights");
    m_params->GetParameter("NumberOfLights", 2, &numLights);
    m_lights = new LightData[numLights];
    for (int i = 0; i < numLights; i++) {
        LightData *light = &m_lights[i];
        light->m_emitter = 0;
        if (!ReadLightData(i))
            return 0;
        if (light->m_emitsLight) {
            char *name = light->m_hasLensFlare ? light->m_lensFlareTexture : 0;
            light->m_emitter = (new LightEmitter(1))->Create(light->m_emitterType, light->m_color, &light->m_position,
                                                               light->m_pDirection, 0, light->m_range,
                                                               light->m_showDebugSphere, 0, name, i);
            if (!light->m_emitter)
                return 0;
            LpLightManager->AddLight(light->m_emitter, 0);
            AddChild(light->m_emitter, 0);
            light->m_emitter->AddSearchPlacement(LpLightManager->m_nodes[0], 1);
            __I_nop();      // retail's stray nop
        }
    }
    return 1;
}
#pragma alignlabel reset

// ---- src/JetskiUIManager/LoadPages.cpp
int JetskiUIManager::LoadPages()
{
    char file[0x100];
    char pageFile[0x100];
    char block[0x100];

    m_params->ReadParameterBlock("UIBase");
    m_params->GetParameter("TotalPages", 0, &m_numPages);
    m_pages = (UIPage **)malloc(m_numPages * sizeof(UIPage *));
    for (int i = 0; i < m_numPages; i++) {
        sprintf(block, "Page %d", i + 1);
        m_params->ReadParameterBlock(block);
        m_params->GetParameter("PageFile", pageFile, 0x100);
        sprintf(file, "%s%s", UIContentDirectory, pageFile);
        m_pages[i] = GetPage(i + 1, file);
        m_pages[i]->DisableAll();
    }
    return 1;
}

// ---- src/JetskiUIManager/ReadLightData.cpp
// libvu0 sceVu0Normalize.
static inline void Normalize(_D3DVECTOR *out, _D3DVECTOR *in)
{
    asm("lqc2 vf4, 0x0(%1); vmul.xyz vf5, vf4, vf4; vaddy.x vf5, vf5, vf5y; vaddz.x vf5, vf5, vf5z; vrsqrt Q, vf0w, vf5x; vsub.xyzw vf6, vf0, vf0; vaddw.xyzw vf6, vf6, vf4w; vwaitq; vmulq.xyz vf6, vf4, Q; sqc2 vf6, 0x0(%0)"
        : : "r"(out), "r"(in) : "memory");
}

int GetEnumerator(int *value, char *name, ObjectEnumerator *table);

// One front-end light as UI.DAT's [LightN] blocks describe it (0x100 bytes,
// JetskiUIManager::m_lights).

int JetskiUIManager::ReadLightData(int index)
{
    char block[0x10];
    char value[0x210];

    sprintf(block, "Light%d", index + 1);
    if (m_params->ReadParameterBlock(block)) {
        LightData *light = &m_lights[index];
        m_params->GetParameter("Type", "BLANK", value, 0x100);
        if (GetEnumerator(&light->m_type, value, LightEmitterEnumerator)) {
            ReportSuccess(block, "Type", value);
        } else {
            ReportError(block, "Type", value);
            return 0;
        }
        if (!ReadColorRGB(&light->m_color, block, "ColorRGB", ""))
            return 0;
        _D3DVECTOR look;
        light->m_pDirection = &light->m_direction;
        // The default is the address of a temporary (mwcc allows it; a named
        // local loads the arguments in another order).
        if (m_params->GetParameter("LookVector", &_D3DVECTOR(0.0f, 0.0f, 0.0f), &look)) {
            Normalize(&look, &look);
            light->m_direction = look;
        }
        light->m_pPosition = 0;
        light->m_position = _D3DVECTOR(0.0f, 0.0f, 0.0f);
        light->m_range = 10000.0f;
        light->m_atInfinity = 0;
        light->m_showDebugSphere = 0;
        light->m_castsShadows = 0;
        light->m_emitsLight = 1;
        light->m_hasLensFlare = 0;
        ReportError("");
    } else {
        PSXprintf("No light data:  Look in UI.DAT\n");
        return 0;
    }
    return 1;
}

// ---- src/JetskiUIManager/ReadFogData.cpp

struct FogData {
    unsigned int color;             // +0x0
    float visibility;               // +0x4
    float haziness;                 // +0x8
};

// Session shim (see the top of the file): a 0x30000-byte all-zero heap image.
struct MwccHeapImageFog {
    char tail[0x30000];
};
#pragma section mwccshim begin
inline void MwccHeapShimFog() { const MwccHeapImageFog image = { {0} }; }
#pragma section mwccshim end
int JetskiUIManager::ReadFogData()
{
    FogData fog;
    fog.color = 0x8080C0;
    fog.visibility = 1.0f;
    fog.haziness = 0.5f;
    if (m_params->ReadParameterBlock("Fog")) {
        float visibility = 1.0f;
        if (!m_params->GetParameter("Visibility", 1.0f, &visibility)) {
            PSXprintf("Fog visibility NOT FOUND using default 1.0f");
        } else if (visibility < 0.0f || visibility > 1.0f) {
            PSXprintf("Fog visibility has invalid value, must be between 0.0 and 1.0, using default 1.0");
            visibility = 1.0f;
        }
        fog.visibility = visibility;
        PSXprintf("Fog visibility set at %.1f\n", fog.visibility);
        float haziness;
        if (!m_params->GetParameter("Haziness", 0.5f, &haziness)) {
            PSXprintf("Fog haziness NOT FOUND using default 0.5");
        } else if (visibility < 0.0f || visibility > 1.0f) {   // retail tests visibility here
            PSXprintf("Fog haziness has invalid value, must be between 0.0 and 1.0, using default 0.5");
            haziness = 0.5f;
        }
        fog.haziness = haziness;
        PSXprintf("Fog haziness set at %.1f\n", fog.haziness);
        if (!ReadColorRGB((int *)&fog.color, "Fog", "ColorRGB", ""))
            return 0;
    } else {
        PSXprintf("Can't find fog data\n");
    }
    if (!(m_fog = (new Fog(1))->Create(fog.color, fog.visibility, fog.haziness, 1200.0f))) {
        PSXprintf("Failed to create LpFog in PodUIManager::LoadFog\n");
        return 0;
    }
    return 1;
}

// ---- src/JetskiUIManager/ReadColorRGB.cpp
// Reads "r,g,b" (0-255 each, stored unscaled); alpha is cleared.
int JetskiUIManager::ReadColorRGB(_D3DCOLORVALUE *color, char *block, char *name, char *defaultValue)
{
    char buffer[0x190];

    m_params->ReadParameterBlock(block);
    if (m_params->GetParameter(name, defaultValue, buffer, 0x80)) {
        int r = atoi(strtok(buffer, ","));
        int g = atoi(strtok(0, ","));
        int b = atoi(strtok(0, "\n"));
        color->r = r;
        color->g = g;
        color->b = b;
        color->a = 0;
        return 1;
    }
    color->r = 0;
    color->g = 0;
    color->b = 0;
    color->a = 0;
    return 0;
}

// ---- src/JetskiUIManager/ReadColorRGB_0036AAE0.cpp
// Reads "r,g,b" (0-255 each) into a packed 0xRRGGBB.
int JetskiUIManager::ReadColorRGB(int *color, char *block, char *name, char *defaultValue)
{
    char buffer[0x190];

    m_params->ReadParameterBlock(block);
    if (m_params->GetParameter(name, defaultValue, buffer, 0x80)) {
        int r = atoi(strtok(buffer, ","));
        int g = atoi(strtok(0, ","));
        int b = atoi(strtok(0, "\n"));
        *color = (r << 16) | (g << 8) | b;
        return 1;
    }
    *color = 0;
    return 0;
}

// ---- src/JetskiUIManager/ReportError.cpp
void JetskiUIManager::ReportError(char *, char *, char *) {}

// ---- src/JetskiUIManager/ReportError_0036ABE0.cpp
void JetskiUIManager::ReportError(char *) {}

// ---- src/JetskiUIManager/ReportSuccess.cpp
void JetskiUIManager::ReportSuccess(char *, char *, char *) {}

// ---- src/JetskiUIManMain/ctor.cpp
JetskiUIManMain::JetskiUIManMain(int enabled)
    : JetskiUIManager(enabled)
{
    m_cheatKey = 0;
    m_iconManager = 0;
    m_headShots = 0;
    m_portraits = 0;
    m_x2E0 = LpGame->m_gameOptions->m_pageSettings->m_managerMain->m_x0;
    m_x2E4 = LpGame->m_gameOptions->m_pageSettings->m_managerMain->m_x4;
    m_selectMode = 0;
    m_mode = 0;
    m_numPlayers = 0;
    m_x2F4 = 0;
    m_x2F8 = 0;
    m_x2FC = 1;
}

// ---- src/JetskiUIManMain/dtor.cpp
JetskiUIManMain::~JetskiUIManMain()
{
    int i;
    for (i = 0; i < 11; i++) {
        delete m_headShots[i];
        m_headShots[i] = 0;
    }
    free(m_headShots);
    m_headShots = 0;
    for (i = 0; i < 8; i++) {
        delete m_portraits[i];
        m_portraits[i] = 0;
    }
    free(m_portraits);
    m_portraits = 0;
    if (m_cheatKey) {
        delete m_cheatKey;
        m_cheatKey = 0;
    }
    LpGame->m_auralScape->RemoveListener(0);
    LpGame->m_auralScape->Disable();
    LpGame->m_auralScape->m_soundInterface->m_snd.StopAllVoices();
    LpGame->m_auralScape->m_soundInterface->m_snd.SendBatch();
    LpGame->m_auralScape->m_soundInterface->m_snd.ClearMemory();
}

// ---- src/JetskiUIManMain/Create.cpp
// Session shim (see the top of the file): a 0x30000-byte all-zero heap image.
struct MwccHeapImageMain {
    char tail[0x30000];
};
#pragma section mwccshim begin
inline void MwccHeapShimMain() { const MwccHeapImageMain image = { {0} }; }
#pragma section mwccshim end
JetskiUIManMain *JetskiUIManMain::Create(int page, UIGlobalManager *global)
{
    char resourceFile[0x40];
    char path[0x110];
    char name[0x104];
    JetskiGameOptions *options;
    int i;

    strcpy(resourceFile, "UI\\NTSC\\Main.res");
    LpDataAccess->LoadResourceFile(resourceFile, 0, 0, 0, 0, 0, 0, 0);
    options = LpGame->m_gameOptions;
    if ((m_cheatKey = new JetskiCheatKey) != 0) {
        sprintf(path, "%sCheatKey.key", UIContentDirectory);
        m_cheatKey->Initialize(path);
    }
    m_headShots = (ScreenIcon **)malloc(11 * sizeof(ScreenIcon *));
    m_portraits = (ScreenIcon **)malloc(8 * sizeof(ScreenIcon *));
    for (i = 0; i < 11; i++) {
#line 1275 "JetskiUIManagers.cpp"
        STRCPY(name, UITextureContentDirectory);
        switch (i) {
        case 0:
            strcat(name, "Jewel_Andy.tga");
            break;
        case 4:     // retail's jump table: Juana is case 4, Haily case 1
            strcat(name, "Jewel_Juana.tga");
            break;
        case 2:
            strcat(name, "Jewel_Rafael.tga");
            break;
        case 3:
            strcat(name, "Jewel_Kyoko.tga");
            break;
        case 1:
            strcat(name, "Jewel_Haily.tga");
            break;
        case 5:
            strcat(name, "Jewel_Sebastian.tga");
            break;
        case 6:
            strcat(name, "Jewel_Jeremy.tga");
            break;
        case 7:
            strcat(name, "Jewel_Amman.tga");
            break;
        case 8:
            strcat(name, "Jewel_Steve.tga");
            break;
        case 9:
            strcat(name, "Jewel.tga");
            break;
        case 10:
            strcat(name, "Jewel_Skin.tga");
            break;
        }
        m_headShots[i] = new ScreenIcon;
        m_headShots[i]->Create(name, 0.0f, 0.0f);
        m_headShots[i]->SetWidth(44.0f);
        m_headShots[i]->SetHeight(44.0f);
        m_headShots[i]->SetUVRect(11.0f, 55.0f, 10.0f, 54.0f);
    }
    for (i = 0; i < 8; i++) {
#line 1322 "JetskiUIManagers.cpp"
        STRCPY(name, UITextureContentDirectory);
        strcat(name, "VColor.tga");
        m_portraits[i] = new ScreenIcon;
        m_portraits[i]->Create(name, 0.0f, 0.0f);
        m_portraits[i]->SetWidth(44.0f);
        m_portraits[i]->SetHeight(44.0f);
        m_portraits[i]->SetUVRect(11.0f, 55.0f, 10.0f, 54.0f);
        switch (i) {
        case 0:
            m_portraits[i]->SetPosition(282.0f, 68.0f);
            break;
        case 1:
            m_portraits[i]->SetPosition(321.0f, 38.0f);
            break;
        case 2:
            m_portraits[i]->SetPosition(405.0f, 25.0f);
            break;
        case 3:
            m_portraits[i]->SetPosition(457.0f, 34.0f);
            break;
        case 4:
            m_portraits[i]->SetPosition(501.0f, 58.0f);
            break;
        case 5:
            m_portraits[i]->SetPosition(536.0f, 96.0f);
            break;
        case 6:
            m_portraits[i]->SetPosition(557.0f, 143.0f);
            break;
        case 7:
            m_portraits[i]->SetPosition(547.0f, 224.0f);
            break;
        }
    }
    JetskiUIManager::Create("Main", page, global, 1);
    LpDataAccess->DropDevice(resourceFile, 1);
    LpGame->m_auralScape->Enable();
    LpGame->m_auralScape->m_soundInterface->ClearSoundStringTable();
    m_soundGroup = new PSXSoundGroup(LpGame->m_auralScape->m_soundInterface, m_enabled);
    m_soundGroup->Create();
    m_soundGroup->SetVolume(LpGame->m_gameOptions->m_x828);
    AddChild(m_soundGroup, 2);
    m_musicGroup = new PSXSoundGroup(LpGame->m_auralScape->m_soundInterface, m_enabled);
    m_musicGroup->Create();
    m_musicGroup->SetVolume(LpGame->m_gameOptions->m_musicVolume);
    AddChild(m_musicGroup, 2);
    m_riderGroup = new PSXSoundGroup(LpGame->m_auralScape->m_soundInterface, m_enabled);
    m_riderGroup->Create();
    m_riderGroup->SetVolume(LpGame->m_gameOptions->m_x82C);
    AddChild(m_riderGroup, 2);
    m_tuneSound = new PSXSound(LpGame->m_auralScape->m_soundInterface, m_musicGroup, 1);
    if (!m_tuneSound->CreateFromFile("uitune_", 0xC1, 1)) {
        if (m_tuneSound) {
            delete m_tuneSound;
            m_tuneSound = 0;
        }
    }
    m_negativeSound = new PSXSound(LpGame->m_auralScape->m_soundInterface, m_riderGroup, 1);
    if (!m_negativeSound->CreateFromFile("Seb\\Negative\\ooff01.vag", 1, 1)) {
        if (m_negativeSound) {
            delete m_negativeSound;
            m_negativeSound = 0;
        }
    }
    m_heliSound = new PSXSound(LpGame->m_auralScape->m_soundInterface, m_soundGroup, 1);
    if (!m_heliSound->CreateFromFile("UI\\heliloop.vag", 0x81, 1)) {
        if (m_heliSound) {
            delete m_heliSound;
            m_heliSound = 0;
        }
    }
    _DS3DBUFFER buffer;
    m_responses = new ResponseEmitter(LpGame->m_auralScape, m_soundGroup, 1, 1);
    if (m_responses) {
        AddChild(m_responses, -1);
        m_responses->Create(buffer, 2, 20.0f, 100.0f, 0, 0);
        m_responses->LoadResponseList("UI\\UISounds.txt");
    }
    m_riderSounds = new JetskiUIRiderSoundEmitter(LpGame->m_auralScape->m_soundInterface, m_riderGroup->m_groupID, 1);
    m_riderSounds = m_riderSounds->Create("Riders\\RiderDlg.aud");
    if (m_riderSounds)
        AddChild(m_riderSounds, -1);
    LpGame->m_auralScape->m_soundInterface->LoadRequestedSounds();
    if (options->m_x878)
        options->m_x878 = 0;
    else if (m_tuneSound)
        m_tuneSound->Play(0, 0);
    SetCurrentPage(page);
    return this;
}

// ---- src/JetskiUIManMain/UIPrepareClose.cpp
void JetskiUIManMain::UIPrepareClose()
{
    m_nextPage = 1;
    UIManager::UIPrepareClose();
}

// ---- src/JetskiUIManMain/GetPage.cpp
UIPage *JetskiUIManMain::GetPage(int pageId, char *name)
{
    UIPage *page;   // retail returns it uninitialised for an unknown pageId

    switch (pageId) {
    case 0:
        break;
    case 1:
        page = (new JetskiUIPageMain(1))->Create(m_name, name, this, m_textManager);
        break;
    case 2:
        page = (new JetskiUIPageCharSelect(1))->Create(m_name, name, this, m_textManager);
        break;
    case 3:
        page = (new JetskiUIPageSetup(1))->Create(m_name, name, this, m_textManager);
        break;
    case 4:
        page = (new JetskiUIPageTraining(1))->Create(m_name, name, this, m_textManager);
        break;
    case 5:
        page = (new JetskiUIPageOptions(1))->Create(m_name, name, this, m_textManager);
        break;
    case 6:
        page = (new JetskiUIPageGOptions(1))->Create(m_name, name, this, m_textManager);
        break;
    case 7:
        page = (new JetskiUIPageSLevels(1))->Create(m_name, name, this, m_textManager);
        break;
    case 8:
        page = (new JetskiUIPageRecords(1))->Create(m_name, name, this, m_textManager);
        break;
    case 9:
        page = (new JetskiUIPageController(1))->Create(m_name, name, this, m_textManager);
        break;
    case 10:
        page = (new JetskiUIPageCredits(1))->Create(m_name, name, this, m_textManager);
        break;
    case 11:
        page = (new JetskiUIPageCenterScreen(1))->Create(m_name, name, this, m_textManager);
        break;
    case 12:
        page = (new JetskiMPLoadEmptyOpt(1))->Create(m_name, name, this, m_textManager);
        break;
    case 13:
        page = (new JetskiMPLoadYes(1))->Create(m_name, name, this, m_textManager);
        break;
    case 14:
        page = (new JetskiMPLoadNo(1))->Create(m_name, name, this, m_textManager);
        break;
    case 15:
        page = (new JetskiMPLoadNoGame(1))->Create(m_name, name, this, m_textManager);
        break;
    case 16:
        page = (new JetskiMPLoadNoFormat(1))->Create(m_name, name, this, m_textManager);
        break;
    case 17:
        page = (new JetskiMPSaveEmptyOpt(1))->Create(m_name, name, this, m_textManager);
        break;
    case 18:
        page = (new JetskiMPSaveYes(1))->Create(m_name, name, this, m_textManager);
        break;
    case 19:
        page = (new JetskiMPSaveNo(1))->Create(m_name, name, this, m_textManager);
        break;
    case 20:
        page = (new JetskiMPSaveNoSpace(1))->Create(m_name, name, this, m_textManager);
        break;
    case 21:
        page = (new JetskiMPSaveFormat(1))->Create(m_name, name, this, m_textManager);
        break;
    case 22:
        page = (new JetskiUIPageLoadSave(1))->Create(m_name, name, this, m_textManager);
        break;
    case 23:
        page = (new JetskiUIPageDiff(1))->Create(m_name, name, this, m_textManager);
        break;
    case 24:
        page = (new JetskiUIPageName(1))->Create(m_name, name, this, m_textManager);
        break;
    case 25:
        page = (new JetskiUIPageCareer(1))->Create(m_name, name, this, m_textManager);
        break;
    case 26:
        page = (new JetskiUIPageStandings(1))->Create(m_name, name, this, m_textManager, m_primitives);
        break;
    case 27:
        page = (new JetskiUIPageCarRacerSelect(1))->Create(m_name, name, this, m_textManager);
        break;
    case 28:
        page = (new JetskiUIPageChallenge(1))->Create(m_name, name, this, m_textManager);
        break;
    case 29:
        page = (new JetskiMPSaveEmptyChamp(1))->Create(m_name, name, this, m_textManager);
        break;
    case 30:
        page = (new JetskiUIPageVideos(1))->Create(m_name, name, this, m_textManager);
        break;
    case 31:
        page = (new JetskiUIPageCovers(1))->Create(m_name, name, this, m_textManager);
        break;
    }
    return page;
}

// ---- src/JetskiUIManMain/Localize.cpp
int JetskiUIManMain::Localize()
{
    UIManager::Localize();
    return 1;
}

// ---- src/JetskiUIManMain/GetTeamColor.cpp
// Retail biases the switch by 2 (team + 2 compared with 2..6).
void JetskiUIManMain::GetTeamColor(int team, unsigned int *color)
{
    switch (team + 2) {
    case 2:
        *color = 0xC8820000;
        break;
    case 3:
        *color = 0xFFB4AE00;
        break;
    case 4:
        *color = 0xFF009600;
        break;
    case 5:
        *color = 0xC8000096;
        break;
    case 6:
        *color = 0xFF9600C8;
        break;
    }
}

// ---- src/JetskiUIManCard/ctor.cpp
JetskiUIManCard::JetskiUIManCard(int enabled)
    : JetskiUIManager(enabled)
{
    m_x2D0 = 0;
}

// ---- src/JetskiUIManCard/dtor.cpp
JetskiUIManCard::~JetskiUIManCard()
{
    LpGame->m_auralScape->RemoveListener(0);
    LpGame->m_auralScape->Disable();
    LpGame->m_auralScape->m_soundInterface->m_snd.StopAllVoices();
    LpGame->m_auralScape->m_soundInterface->m_snd.SendBatch();
    LpGame->m_auralScape->m_soundInterface->m_snd.ClearMemory();
}

// ---- src/JetskiUIManCard/Create.cpp
// 16-byte aligned here: _DS3DBUFFER's vectors sit on 16-byte boundaries.

// DirectSound 3D buffer parameters (16-byte vectors, so 0x70 bytes).

JetskiUIManCard *JetskiUIManCard::Create(int page, UIGlobalManager *global)
{
    char resourceFile[0x40];
    char path[0x70];
    strcpy(resourceFile, "AppStart\\Ntsc\\Card.res");
    LpDataAccess->LoadResourceFile(resourceFile, 0, 0, 0, 0, 0, 0, 0);
    strcpy(m_name, "Card");
    m_page = page;
    m_params = new ParameterBlock;
    sprintf(path, "%sui.dat", UIContentDirectory);
    if (!m_params->OpenFile(path, 0)) {
        PSXprintf("Failed to load ui.dat in UIManager::LoadPages\n");
        return 0;
    }
    UICamera *camera = new UICamera(1);
    m_camera = camera;
    if (!camera->Create())
        PSXprintf("Failed to create LpUICameras in UIManager::LoadCamera\n");
    _D3DVECTOR position(384.0f, 1056.0f, 256.0f);
    _D3DVECTOR lookAt(0.0f, -1.0f, 0.0f);
    _D3DVECTOR up(0.0f, 0.0f, 1.0f);
    float fov = 35.0f;
    float x1D4 = 0.0f;
    m_camera->SetViewParameters(&position, &lookAt, &up, &x1D4, &fov);
    m_camera->Enable();
    UIManager::Create(global);
    if (!LoadWidgets())
        return 0;
    if (!LoadPages())
        return 0;
    if (!AddChild(m_camera, -1)) {
        PSXprintf("Failed to attach LpUICameras in UIManager::AttachScene\n");
        return 0;
    }
    LpGame->SetCameras(m_camera, 0, 0, 0);
    AttachWidgets();
    AttachPages();
    if (m_iconManager)
        AddChild(m_iconManager, -1);
    if (m_textManager)
        AddChild(m_textManager, -1);
    delete m_params;
    m_params = 0;
    Localize();
    LpGame->m_auralScape->Enable();
    LpGame->m_auralScape->m_soundInterface->SetStreamVolume(100);
    m_soundGroup = new PSXSoundGroup(LpGame->m_auralScape->m_soundInterface, m_enabled);
    m_soundGroup->SetVolume(LpGame->m_gameOptions->m_x828);
    AddChild(m_soundGroup, 2);
    _DS3DBUFFER buffer;
    m_responses = new ResponseEmitter(LpGame->m_auralScape, m_soundGroup, 1, 1);
    if (m_responses) {
        AddChild(m_responses, -1);
        m_responses->Create(buffer, 2, 20.0f, 100.0f, 0, 0);
        m_responses->LoadResponseList("UI\\UICardSounds.txt");
        LpGame->m_auralScape->m_soundInterface->LoadRequestedSounds();
    }
    LpDataAccess->DropDevice(resourceFile, 1);
    SetCurrentPage(page);
    return this;
}

// ---- src/JetskiUIManCard/UIPrepareClose.cpp
void JetskiUIManCard::UIPrepareClose()
{
    m_nextPage = 1;
    UIManager::UIPrepareClose();
}

// ---- src/JetskiUIManCard/GetPage.cpp
UIPage *JetskiUIManCard::GetPage(int pageId, char *name)
{
    UIPage *page;   // retail returns it uninitialised for an unknown pageId

    switch (pageId) {
    case 1:
        page = (new JetskiMPInitNo(1))->Create(m_name, name, (UIManager *)this, m_textManager);
        break;
    case 2:
        page = (new JetskiMPInitNoSpace(1))->Create(m_name, name, (UIManager *)this, m_textManager);
        break;
    case 3:
        page = (new JetskiMPInitYes(1))->Create(m_name, name, (UIManager *)this, m_textManager);
        break;
    case 4:
        page = (new JetskiMPInitFormat(1))->Create(m_name, name, (UIManager *)this, m_textManager);
        break;
    case 5:
        page = (new JetskiMPInitEmpty(1))->Create(m_name, name, (UIManager *)this, m_textManager);
        break;
    }
    return page;
}

// ---- src/JetskiUIManCard/Localize.cpp
int JetskiUIManCard::Localize()
{
    UIManager::Localize();
    return 1;
}

