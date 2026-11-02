// JetskiInGameOverlay.cpp  @ 0x002A1660 (text 0x2A1660-0x2AF930)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's JetskiInGameOverlay.cpp as one translation unit: the in-game HUD.
// JetskiInGameOverlayContainer, then JetskiPerformanceMeter, JetskiInGameStats,
// ScreenPrimitiveVertex, JetskiHeadingArrows, JetskiCountDown,
// SplitScreenSeparator, JetskiRadar, JetskiInfoText, GetVolumeAndPan and
// DistanceCompareDescend, then the static initializer (only the axis vectors).
// Its .rodata labels run without a restart to @4528 (RTTI JetskiInfoText ...
// JetskiInGameOverlayContainer).


#include "GameObject/GameObject.h"
#include "RenderTarget/RenderTarget.h"
#include "JetskiPerformanceMeter/JetskiPerformanceMeter.h"
#include "ScreenText/ScreenText.h"
#include "ScreenIcon/ScreenIcon.h"
#include "soultree_assert_block.h"
#include "ViewPortCode/ViewPortCode.h"
#include "ScreenTextFontItemList/ScreenTextFontItem.h"
#include "ScreenTextFontItemList/ScreenTextFontItemList.h"
#include "JetskiGame/JetskiGame.h"
#include "ScreenTextFont/ScreenTextFont.h"
#include "PSXTextureMap/PSXTextureMap.h"
#include "ScreenIconManager/ScreenIconManager.h"
#include "JetskiCameraController/JetskiCameraController.h"
#include "ScreenTextManager/ScreenTextManager.h"
#include "LocalizedString/LocalizedString.h"
#include "GameText/GameText.h"
#include "JetskiGameOptions/JetskiGameOptions.h"
#include "JetskiInGameStats/JetskiInGameStats.h"
#include "Game/Game.h"
#include "RaceStatusStruct_Tag/RaceStatusStruct_Tag.h"
#include "JetskiInfoText/JetskiInfoText.h"
#include "d3dtypes.h"
#include "JetskiVehicle/JetskiVehicle.h"
#include "JetskiHeadingArrows/JetskiHeadingArrows.h"
#include "SoultreeObject/SoultreeObject.h"
#include "SearchableObject/SearchableObject.h"
#include "PSXRenderableObject/PSXRenderableObject.h"
#include "sinit/AxisVectors.h"
#include "Camera/Camera.h"
#include "JetskiCompetition/JetskiCompetition.h"
#include "JetskiCountDown/JetskiCountDown.h"
#include "Scene/Scene.h"
#include "RaceStatusClass/RaceStatusClass.h"
#include "SplitScreenSeparator/SplitScreenSeparator.h"
#include "ScreenPrimitiveVertex/ScreenPrimitiveVertex.h"
#include "_RECT/_RECT.h"
#include "ScreenObject/ScreenObject.h"
#include "ScreenPrimitive/ScreenPrimitive.h"
#include "DmaRenderItem/DmaRenderItem.h"
#include "ScreenPrimitiveManager/ScreenPrimitiveManager.h"
#include "JetskiRadar/JetskiRadar.h"
#include "TextureMap/TextureMap.h"
#include "PhysicsRigidBody/PhysicsRigidBody.h"
#include "PSXCamera/PSXCamera.h"
#include "SceneManagerData/SceneManagerData.h"
#include "PSXRenderTarget/PSXRenderTarget.h"
#include "JetskiRider/JetskiRider.h"
#include "JetskiCareerCompetition/JetskiCareerCompetition.h"
#include "JetskiVCRController/JetskiVCRController.h"

// ---- src/JetskiInGameOverlayContainer/ctor.cpp

class ProcessNode;
class KeyInfo;

// The GameObject that holds a viewport's in-game overlays (no header).
class JetskiInGameOverlayContainer : public GameObject {
public:
    JetskiInGameOverlayContainer(int id);

    // --- virtuals in retail slot order ---
    virtual ~JetskiInGameOverlayContainer();
    virtual int PrepareGeometry();
    virtual int Render3D();
    virtual int KeyDownHandler(ProcessNode *node, KeyInfo *info);
};

JetskiInGameOverlayContainer::JetskiInGameOverlayContainer(int id) : GameObject(id)
{
}


// ---- src/JetskiInGameOverlayContainer/dtor.cpp


JetskiInGameOverlayContainer::~JetskiInGameOverlayContainer()
{
}


// ---- src/JetskiInGameOverlayContainer/PrepareGeometry.cpp

// Profiling timer registration, compiled out in retail: the timer id is
// still a guarded function-local static.
inline int RegisterTimer() { return -1; }

int RenderOverlays = 1;


int JetskiInGameOverlayContainer::PrepareGeometry()
{
    if (!RenderOverlays)
        return 1;
    static int timerID = RegisterTimer();
    GameObject::PrepareGeometry();
    return 1;
}


// ---- src/JetskiInGameOverlayContainer/Render3D.cpp

extern RenderTarget *LpRenderTarget;
extern int RenderOverlays;
extern int TimeGPU;



// The timer IDs are leftovers of profiling code compiled out of retail.
int JetskiInGameOverlayContainer::Render3D()
{
    if (!RenderOverlays)
        return 1;
    if (TimeGPU) {
        LpRenderTarget->TrySendRenderQueue(1);
        LpRenderTarget->WaitForRenderingToComplete();
    }
    static int CPUTimeID = -1;
    GameObject::Render3D();
    if (TimeGPU) {
        static int GPUTimeID = -1;
        LpRenderTarget->TrySendRenderQueue(1);
        LpRenderTarget->WaitForRenderingToComplete();
        return 1;
    }
    LpRenderTarget->TrySendRenderQueue(0);
    return 1;
}


// ---- src/JetskiInGameOverlayContainer/KeyDownHandler.cpp
enum DeviceType {};
int KEYDOWN_HANDLER_TEST(unsigned int key, DeviceType device, ProcessNode *node, unsigned int flags);

extern int RenderOverlays;


int JetskiInGameOverlayContainer::KeyDownHandler(ProcessNode *node, KeyInfo *)
{
    if (KEYDOWN_HANDLER_TEST(0x25, (DeviceType)0, node, 0x80)) {
        RenderOverlays = 1 - RenderOverlays;
        return 1;
    }
    return 0;
}


// ---- src/JetskiPerformanceMeter/ctor.cpp

JetskiPerformanceMeter::JetskiPerformanceMeter(int enabled) : GameObject(enabled)
{
    m_iconManager = 0;
    m_items = 0;
    m_textManager = 0;
    m_labels = 0;
    m_text48 = 0;
    m_text44 = 0;
    m_font4C = 0;
    m_font50 = 0;
    m_font54 = 0;
    m_enabled[0] = 0;
    m_enabled[1] = 0;
    m_showSpeed = 0;
    m_showPerformance = 0;
    m_maxSpeed = 90.0f;
    m_speed = -9999.0f;
    m_x5C = 1;
    m_x6C = 6.28f;
    m_x70 = 4.85f;
    m_x74 = m_x6C - m_x70;
    m_x78 = 0.0f;
    m_x7C = 2.8f;
    m_x80 = m_x7C - m_x78;
}


// ---- src/JetskiPerformanceMeter/dtor.cpp
extern "C" void free(void *p);


// Meter elements (real class unknown): a polymorphic class whose vptr is at +0.

JetskiPerformanceMeter::~JetskiPerformanceMeter()
{
    int i;
    for (i = 0; i < 11; i++)
        delete m_items[i];
    free(m_items);
    m_items = 0;
    for (i = 0; i < 2; i++)
        delete m_labels[i];
    free(m_labels);
    m_labels = 0;
    delete m_text44;
    delete m_text48;
}


// ---- src/JetskiPerformanceMeter/Create.cpp
#pragma inline_depth(5)


extern "C" char *strcat(char *dst, const char *src);
extern "C" void *malloc(unsigned int size);

extern char UITextureContentDirectory[];


extern JetskiGame *LpGame;

class JetskiCompetition;
class ScreenIconManager;
class ScreenTextManager;

// Local copy of JetskiPerformanceMeter.h's class: the shared header leaves
// Create undeclared (no shared ViewPortCode there).

// Texture coordinates already in 0..1.
static inline void SetUV(ScreenIcon *icon, float u0, float u1, float v0, float v1)
{
    icon->m_u0 = u0;
    icon->m_u1 = u1;
    icon->m_v0 = v0;
    icon->m_v1 = v1;
    icon->m_uvDirty = 1;
}

// Colours go through a block-local copy at each call (ScreenText::SetColor
// copies again): that puts retail's colour constants in the right
// registers.
#define SET_COLOR(text, value)          \
    {                                   \
        unsigned int color = value;     \
        (text)->SetColor(color);        \
    }

JetskiPerformanceMeter *JetskiPerformanceMeter::Create(ScreenIconManager *iconManager, ScreenTextManager *textManager,
                                                       ViewPortCode viewport)
{
    char path[0x104];
    ScreenTextFontItem *item;
    int i;

    m_viewport = viewport;
    m_iconManager = iconManager;
    m_items = (ScreenIcon **)malloc(11 * sizeof(ScreenIcon *));
    m_font4C = (item = LpGame->m_fonts->GetScreenTextFont("45LCD")) ? item->m_font : 0;
    m_font50 = (item = LpGame->m_fonts->GetScreenTextFont("title_b")) ? item->m_font : 0;
    m_font54 = (item = LpGame->m_fonts->GetScreenTextFont("info_c")) ? item->m_font : 0;
    m_textManager = textManager;
    m_labels = (ScreenText **)malloc(0x54);
    for (i = 0; i < 11; i++) {
#line 194 "JetskiInGameOverlay.cpp"
        STRCPY(path, UITextureContentDirectory);
        switch (i) {
        case 0:
            strcat(path, "GUImeterframe.tga");
            break;
        case 2:
        case 3:
            strcat(path, "GUImetercolor.tga");
            break;
        case 1:
            strcat(path, "GUISpeedometer.tga");
            break;
        case 4:
        case 5:
        case 9:
        case 10:
            strcat(path, "GUISpeedColor.tga");
            break;
        case 6:
        case 7:
        case 8:
            strcat(path, "GUIHoldMeter.tga");
            break;
        }
        m_items[i] = (new ScreenIcon)->Create(path, 0.0f, 0.0f);
        switch (i) {
        case 1:
            m_items[i]->SetWidth(104.0f);
            m_items[i]->SetHeight(104.0f);
            m_items[i]->SetUVRect(10.0f, 114.0f, 6.0f, 110.0f);
            m_items[i]->SetDepth(8.0f);
            m_items[i]->m_zBuffer = 1;
            break;
        case 5:
            m_items[i]->SetUVRect(0.0f, 1.0f, 23.0f, 24.0f);
            m_items[i]->SetDepth(11.0f);
            m_items[i]->m_zBuffer = 1;
            break;
        case 4:
            m_items[i]->SetWidth(130.0f);
            m_items[i]->SetHeight(59.0f);
            m_items[i]->SetDepth(12.0f);
            m_items[i]->m_zBuffer = 1;
            break;
        case 0:
            m_items[i]->SetWidth(75.0f);
            m_items[i]->SetHeight(86.0f);
            m_items[i]->SetUVRect(27.0f, 102.0f, 21.0f, 107.0f);
            m_items[i]->SetDepth(9.0f);
            m_items[i]->m_zBuffer = 1;
            break;
        case 3:
            m_items[i]->SetWidth(93.0f);
            m_items[i]->SetHeight(110.0f);
            m_items[i]->SetUVRect(29.0f, 30.0f, 21.0f, 22.0f);
            m_items[i]->SetDepth(10.0f);
            m_items[i]->m_zBuffer = 1;
            break;
        case 2:
            m_items[i]->SetWidth(69.0f);
            m_items[i]->SetHeight(80.0f);
            m_items[i]->SetUVRect(29.0f, 98.0f, 21.0f, 101.0f);
            m_items[i]->SetDepth(11.0f);
            m_items[i]->m_zBuffer = 1;
            break;
        case 6:
            m_items[i]->SetWidth(78.0f);
            m_items[i]->SetHeight(80.0f);
            SetUV(m_items[i], 0.0f, 1.0f, 0.0f, 1.0f);
            m_items[i]->SetDepth(6.0f);
            SET_COLOR(m_items[i], 0xFF919191)
            m_items[i]->m_zBuffer = 1;
            break;
        case 7:
            m_items[i]->SetWidth(39.0f);
            m_items[i]->SetHeight(80.0f);
            SetUV(m_items[i], 0.5f, 1.0f, 0.0f, 1.0f);
            m_items[i]->SetDepth(5.0f);
            SET_COLOR(m_items[i], 0xFFC80000)
            m_items[i]->m_zBuffer = 1;
            break;
        case 8:
            m_items[i]->SetWidth(39.0f);
            m_items[i]->SetHeight(80.0f);
            SetUV(m_items[i], 0.0f, 0.5f, 0.0f, 1.0f);
            m_items[i]->SetDepth(3.0f);
            SET_COLOR(m_items[i], 0xFFC80000)
            m_items[i]->m_zBuffer = 1;
            break;
        case 9:
            m_items[i]->SetUVRect(0.0f, 1.0f, 23.0f, 24.0f);
            m_items[i]->SetWidth(43.0f);
            m_items[i]->SetHeight(85.0f);
            m_items[i]->SetDepth(4.0f);
            m_items[i]->m_zBuffer = 1;
            break;
        case 10:
            m_items[i]->SetUVRect(0.0f, 1.0f, 23.0f, 24.0f);
            m_items[i]->SetWidth(43.0f);
            m_items[i]->SetHeight(85.0f);
            m_items[i]->SetDepth(2.0f);
            m_items[i]->m_zBuffer = 1;
            break;
        }
    }
    for (i = 0; i < 2; i++) {
        float x;
        float y;
        m_labels[i] = new ScreenText(0x28, 0);
        switch (i) {
        case 0:
            x = m_viewport == 0 ? 593.0f : 273.0f;
            y = 325.0f;
            m_labels[i]->SetScale(1.111f);
            m_labels[i]->SetPosition(x, y);
            SET_COLOR(m_labels[i], 0xFF6EF4FF)
            break;
        case 1:
            if (m_viewport == 0) {
                x = 522.0f;
                y = 345.0f;
            } else {
                x = m_viewport == 1 ? 522.0f : 202.0f;
                y = m_viewport == 2 ? 345.0f : 122.0f;
            }
            m_labels[i]->SetPosition(x, y);
            m_labels[i]->SetScale(1.0f);
            SET_COLOR(m_labels[i], 0xFF6EF4FF)
            break;
        }
    }
    float x;
    float y;
    m_text44 = new ScreenText(0x28, 0);
    m_text44->SetString("");
    SET_COLOR(m_text44, 0xFF6EF4FF)
    if (m_viewport == 0) {
        x = 557.0f;
        y = 378.0f;
    } else if (m_viewport == 1) {
        x = 557.0f;
        y = 154.0f;
    } else {
        x = 237.0f;
        y = 378.0f;
    }
    m_text44->SetPosition(x, y);
    m_text44->SetScale(0.67f);
    m_text48 = new ScreenText(0x28, 0);
    m_text48->SetString("");
    SET_COLOR(m_text48, 0xFF6EF4FF)
    m_x58 = -1;
    SetupInfo(1);
    return this;
}


// ---- src/JetskiPerformanceMeter/Setters.cpp
// (Split from SmogParticleEmitter/ReloadFromFile.cpp, now archived: that
// function is Smoker.cpp's; these are JetskiInGameOverlay.cpp's.)

int JetskiPerformanceMeter::SetCompetition(JetskiCompetition *competition)
{
    m_competition = competition;
    return 1;
}

// A negative argument means "use the player's option".
void JetskiPerformanceMeter::SetShowing(int player0, int player1)
{
    if (player0 < 0)
        player0 = LpGame->m_gameOptions->m_hudFlags[0] & 2;
    m_enabled[0] = player0;
    if (player1 < 0)
        player1 = LpGame->m_gameOptions->m_hudFlags[1] & 2;
    m_enabled[1] = player1;
    SetPerformance(1);
    SetSpeedometer(m_viewport == 0);
}


void JetskiPerformanceMeter::SetSpeedometer(int show)
{
    m_showSpeed = show;
}

void JetskiPerformanceMeter::SetPerformance(int show)
{
    m_showPerformance = show;
}

// ---- src/JetskiPerformanceMeter/SetShowing.cpp


extern JetskiGame *LpGame;




// ---- src/JetskiPerformanceMeter/PrepareGeometry.cpp
extern JetskiGame *LpGame;

// Keep every meter element at least 10x scale.
int JetskiPerformanceMeter::PrepareGeometry()
{
    TextureMap *image;
    for (int i = 0; i < 11; i++) {
        if (m_items[i]) {
            image = m_items[i]->m_texture;
            if (10.0f > image->m_usage)
                image->m_usage = 10.0f;
        }
    }
    if (m_font4C) {
        image = m_font4C->m_texture;
        if (10.0f > image->m_usage)
            image->m_usage = 10.0f;
    }
    if (m_font50) {
        image = m_font50->m_texture;
        if (10.0f > image->m_usage)
            image->m_usage = 10.0f;
    }
    SetupInfo(LpGame->m_currentPlayer + 1);
    return GameObject::PrepareGeometry();
}


// ---- src/JetskiPerformanceMeter/Render3D.cpp
extern JetskiGame *LpGame;

int JetskiPerformanceMeter::Render3D()
{
    if (!LpGame->m_competition)
        return GameObject::Render3D();
    if (m_enabled[LpGame->m_currentPlayer]) {
        SetupHoldMeter();
        if (m_showSpeed)
            RenderSpeed();
        if (m_showPerformance)
            RenderPerformance();
        if (m_showPerformance)
            m_iconManager->AddIcon(m_items[0], 0);
        if (m_showSpeed)
            m_iconManager->AddIcon(m_items[1], 0);
        RenderHoldMeter();
    }
    return GameObject::Render3D();
}


// ---- src/JetskiPerformanceMeter/SetupHoldMeter.cpp
#pragma fast_fptosi on








extern JetskiGame *LpGame;

// Airborne (bit 0 clear), in a trick (bits 1, 4) and holding it (bit 5).
static inline bool IsHoldingTrick(RaceStatusStruct_Tag *status)
{
    unsigned int flags = status->m_flags;
    return !(flags & 1) && (flags & 2) && (flags & 0x10) && (flags & 0x20);
}



void JetskiPerformanceMeter::SetupHoldMeter()
{
    int vehicle = LpGame->m_competition->m_cameraControllers[LpGame->m_currentPlayer]->m_x64;
    RaceStatusStruct_Tag *rider = LpGame->m_competition->m_vehicles[vehicle]->m_ownStatus;
    m_holding = IsHoldingTrick(rider);
    if (m_holding) {
        m_multiplier = (int)(rider->m_stunts->m_x1C / 0.5f) + 1.0f;
        m_holdFraction = (rider->m_stunts->m_x1C - 0.5f * (m_multiplier - 1.0f)) / 0.5f;
        if (LpGame->m_gameOptions->m_raceType != 5
            && LpGame->m_competition->m_vehicles[vehicle]->m_rider->m_x404[rider->m_stunts->m_type]
            && rider->m_numStunts < 2)
            m_multiplier *= 0.5f;
        m_xA0 = LpGame->m_competition->m_vehicles[vehicle]->m_rider->m_stunt;
        if (rider->m_flags & 0x100)
            m_x58 = m_xA0;
    }
}
#pragma fast_fptosi reset


// ---- src/JetskiPerformanceMeter/RenderHoldMeter.cpp
extern "C" int sprintf(char *buf, const char *fmt, ...);
extern "C" char *strncpy(char *dst, const char *src, unsigned int n);

class ScreenTextFont;






class JetskiCompetition;
class ScreenTextFont;
class ScreenIconManager;


// The hold meter is two half-dials, each sweeping up to pi.
void JetskiPerformanceMeter::RenderHoldMeter()
{
    char buf[0x40];
    if (m_holding) {
        sprintf(buf, "%1.1fx", m_multiplier);
        ScreenText *text = m_labels[1];
        text->m_positionDirty = 1;
        strncpy(text->m_text, buf, text->m_maxChars);
        text->m_text[text->m_maxChars] = 0;
        text->m_x5D_bit4 = 1;
        text->m_dirty = 1;
        text->m_cellsDirty = 1;
        m_textManager->PositionText(m_labels[1], ScreenTextManager::LeftJustification, m_font50);
        m_textManager->AddText(m_labels[1], m_font50);
        m_iconManager->AddIcon(m_items[6], 0);
        float angle = 6.28318548f * m_holdFraction;
        if (angle > 3.14159274f)
            m_items[9]->SetAngle(3.14159274f);
        else
            m_items[9]->SetAngle(angle);
        m_iconManager->AddIcon(m_items[9], 0);
        m_iconManager->AddIcon(m_items[7], 0);
        if (angle > 3.14159274f) {
            angle -= 3.14159274f;
            m_items[10]->SetAngle(angle);
            m_iconManager->AddIcon(m_items[10], 0);
            m_iconManager->AddIcon(m_items[8], 0);
        }
    }
}


// ---- src/JetskiPerformanceMeter/StuntsName.cpp
// GetString is only inlined at a deeper inline depth than the default.
#pragma inline_depth(5)



extern JetskiGame *LpGame;


// Localized name of a stunt; stunt 27 is each character's signature stunt.
char *JetskiPerformanceMeter::StuntsName(int stunt, int character)
{
    switch (stunt) {
    case 0:
        return LpGame->m_text->GetString(23);
    case 1:
        return LpGame->m_text->GetString(24);
    case 2:
        return LpGame->m_text->GetString(25);
    case 3:
        return LpGame->m_text->GetString(26);
    case 4:
        return LpGame->m_text->GetString(27);
    case 5:
        return LpGame->m_text->GetString(28);
    case 6:
        return LpGame->m_text->GetString(29);
    case 7:
        return LpGame->m_text->GetString(30);
    case 8:
        return LpGame->m_text->GetString(31);
    case 9:
        return LpGame->m_text->GetString(32);
    case 10:
        return LpGame->m_text->GetString(33);
    case 11:
        return LpGame->m_text->GetString(34);
    case 12:
        return LpGame->m_text->GetString(35);
    case 13:
        return LpGame->m_text->GetString(36);
    case 14:
        return LpGame->m_text->GetString(37);
    case 15:
        return LpGame->m_text->GetString(38);
    case 16:
        return LpGame->m_text->GetString(39);
    case 17:
        return LpGame->m_text->GetString(40);
    case 18:
        return LpGame->m_text->GetString(41);
    case 19:
        return LpGame->m_text->GetString(42);
    case 20:
        return LpGame->m_text->GetString(43);
    case 21:
        return LpGame->m_text->GetString(44);
    case 22:
        return LpGame->m_text->GetString(45);
    case 23:
        return LpGame->m_text->GetString(46);
    case 24:
        return LpGame->m_text->GetString(47);
    case 25:
        return LpGame->m_text->GetString(48);
    case 26:
        return LpGame->m_text->GetString(49);
    case 27:
        switch (character) {
        case 0:
            return LpGame->m_text->GetString(51);
        case 4:
            return LpGame->m_text->GetString(52);
        case 2:
            return LpGame->m_text->GetString(53);
        case 3:
            return LpGame->m_text->GetString(54);
        case 1:
            return LpGame->m_text->GetString(55);
        case 5:
            return LpGame->m_text->GetString(56);
        case 6:
            return LpGame->m_text->GetString(57);
        case 7:
            return LpGame->m_text->GetString(58);
        case 8:
            return LpGame->m_text->GetString(59);
        default:
            return LpGame->m_text->GetString(50);
        }
    }
    return 0;
}


// ---- src/JetskiPerformanceMeter/RenderSpeed.cpp
// GetString is only inlined at a deeper inline depth than the default.
#pragma inline_depth(5)
#pragma fast_fptosi on

extern "C" int sprintf(char *buf, const char *fmt, ...);

class ScreenTextFont;












extern JetskiGame *LpGame;

class JetskiCompetition;
class ScreenTextFont;
class ScreenIconManager;


// Speedometer needle and digits (mph or km/h).
void JetskiPerformanceMeter::RenderSpeed()
{
    char buf[0x10];
    JetskiVehicle *vehicle = LpGame->m_competition->m_vehicles[LpGame->m_competition->m_cameraControllers[LpGame->m_currentPlayer]->m_x64];
    float speed;
    if (LpGame->m_gameOptions->m_x4A4C) {
        speed = (int)(0.68182f * vehicle->m_body->m_speed);
        m_maxSpeed = 90.0f;
        if (speed != m_speed) {
            m_text44->SetString(LpGame->m_text->GetString(61));
            m_text48->SetString(LpGame->m_text->GetString(61));
            m_speed = speed;
        }
    } else {
        speed = (int)(1.08f * vehicle->m_body->m_speed);
        m_maxSpeed = 144.0f;
        if (speed != m_speed) {
            m_text44->SetString(LpGame->m_text->GetString(62));
            m_text48->SetString(LpGame->m_text->GetString(62));
            m_speed = speed;
        }
    }
    float angle;
    if (vehicle->m_x550)
        angle = 1.5707964f + m_x78;
    else
        angle = 1.5707964f + (m_x78 + m_x80 * (speed / m_maxSpeed));
    m_items[5]->SetAngle(angle);
    m_iconManager->AddIcon(m_items[5], 0);
    m_iconManager->AddIcon(m_items[4], 0);
    if (!m_holding) {
        m_textManager->PositionText(m_text44, ScreenTextManager::CenterHorizontalJustification, m_font50);
        m_textManager->AddText(m_text44, m_font50);
        sprintf(buf, "%.0f", speed);
        m_text48->SetString(buf);
        m_labels[0]->SetString(buf);
        m_textManager->PositionText(m_labels[0], ScreenTextManager::RightJustification, m_font4C);
        m_textManager->AddText(m_labels[0], m_font4C);
    }
}
#pragma fast_fptosi reset


// ---- src/JetskiPerformanceMeter/RenderPerformance.cpp
// Compiler session state: retail compiled
// this file in an mwcc session that had already generated a two-argument runtime
// helper call (64-bit / or %, double arithmetic or compare). mwcc then models
// every later helper call (__floatdisf, fptoui, fptodp, ...) as reading $a0-$a1,
// so temporaries live before such a call skip $a0/$a1. This unused function
// recreates that state; it is not part of the retail file.
static void MWCC_SHIM_TEXT MwccSessionState(long *a) { a[0] = a[0] / a[1]; }





extern JetskiGame *LpGame;




class JetskiCompetition;
class ScreenTextFont;
class ScreenIconManager;


void JetskiPerformanceMeter::RenderPerformance()
{
    float perf = LpGame->m_competition->m_vehicles[LpGame->m_competition->m_cameraControllers[LpGame->m_currentPlayer]->m_x64]->m_performance;
    if (perf > 1.2f)
        perf = 1.2f;
    else if (perf < 0.0f)
        perf = 0.0f;
    if (perf > 1.0f)
        perf = (perf - 1.0f) * 1.5 + 0.7f;
    else
        perf *= 0.7f;
    m_items[3]->SetAngle(m_x6C - perf * m_x74);
    m_iconManager->AddIcon(m_items[3], 0);
    m_iconManager->AddIcon(m_items[2], 0);
}


// ---- src/JetskiPerformanceMeter/Tick.cpp


int JetskiPerformanceMeter::Tick(float dt)
{
    return GameObject::Tick(dt);
}


// ---- src/JetskiPerformanceMeter/SetupInfo.cpp


extern JetskiGame *LpGame;


class JetskiCompetition;
class ScreenTextFont;
class ScreenIconManager;


void JetskiPerformanceMeter::SetupInfo(int player)
{
    int dx = 0;
    int dy = 0;
    if (LpGame->m_gameOptions->m_splitScreen) {
        switch (m_viewport) {
        case 1:
            dy = -224;
            break;
        case 2:
            dx = -320;
            break;
        }
    }
    m_items[0]->SetPosition(dx + 545, dy + 342);
    m_items[2]->SetPosition(dx + 548, dy + 343);
    m_items[3]->SetPosition(dx + 549, dy + 339);
    m_items[3]->SetPivot(dx + 549, dy + 339);
    m_items[1]->SetPosition(dx + 499, dy + 307);
    m_items[4]->SetPosition(dx + 360, dy + 298);
    m_items[4]->SetPivot(dx + 488, dy + 298);
    m_items[4]->SetAngle(-1.5707964f);
    m_items[5]->SetPosition(dx + 473, dy + 360);
    m_items[5]->SetPivot(dx + 537, dy + 360);
    m_items[6]->SetPosition(dx + 517, dy + 320);
    m_items[7]->SetPosition(dx + 556, dy + 320);
    m_items[8]->SetPosition(dx + 517, dy + 320);
    m_items[9]->SetPosition(dx + 556, dy + 318);
    m_items[9]->SetPivot(dx + 556, dy + 360);
    m_items[10]->SetPosition(dx + 514, dy + 318);
    m_items[10]->SetPivot(dx + 556, dy + 360);
}


// ---- src/JetskiInGameStats/ctor.cpp

class JetskiCompetition;


JetskiInGameStats::JetskiInGameStats(int enabled) : GameObject(enabled)
{
    m_competition = 0;
    m_iconManager = 0;
    m_icons = 0;
    m_textManager = 0;
    m_texts = 0;
    m_font = 0;
    m_font70 = 0;
    m_showing[0] = 0;
    m_showing[1] = 0;
    m_showTotalRaceTime = 0;
    m_showRecordLapTime = 0;
    m_showBestLap = 0;
    m_showLapTime = 0;
    m_showTimeBehindLeader = 0;
    m_showPosition = 0;
    m_showLap = 0;
    m_showScoreText = 0;
    m_showCountdownText = 0;
    m_showWrongWayText = 0;
    m_showLeftWing = 1;
    m_showRightWing = 1;
    m_showNewRecordText = 0;
    m_showReplayText = 0;
    m_showRaceTypeText = 0;
    m_secretTime[0] = -1.0f;
    m_secretTime[1] = -1.0f;
    m_secretTime[2] = -1.0f;
    m_secretTime[3] = -1.0f;
    // Retail clears five entries: the last one is m_font again.
    for (int i = 0; i < 5; i++)
        m_rankTexts[i] = 0;
    m_lastLap[0] = 1;
    m_lastLap[1] = 1;
    m_lastCountdown = -9999.0f;
    m_lastPosition = -99;
}


// ---- src/JetskiInGameStats/dtor.cpp
extern "C" void free(void *p);





JetskiInGameStats::~JetskiInGameStats()
{
    int i;
    if (m_icons) {
        for (i = 0; i < 7; i++) {
            if (m_icons[i]) {
                delete m_icons[i];
                m_icons[i] = 0;
            }
        }
        free(m_icons);
        m_icons = 0;
    }
    if (m_texts) {
        for (i = 0; i < 21; i++) {
            if (m_texts[i]) {
                delete m_texts[i];
                m_texts[i] = 0;
            }
        }
        free(m_texts);
        m_texts = 0;
    }
    delete m_recordLabel;
    delete m_bestLapLabel;
    delete m_lapTimeLabel;
    delete m_lapLabel;
    delete m_timeBehindLabel;
    delete m_positionSuffix;
    for (i = 0; i < 4; i++) {
        if (m_rankTexts[i])
            delete m_rankTexts[i];
    }
}


// ---- src/JetskiInGameStats/Create.cpp
// Create__17JetskiInGameStatsFP17ScreenIconManagerP17ScreenTextManager12ViewPortCodeiii  @ 0x002A5770
#pragma inline_depth(5)

extern "C" char *strncpy(char *dst, const char *src, unsigned int n);
extern "C" char *strcpy(char *dst, const char *src);
extern "C" char *strcat(char *dst, const char *src);
extern "C" int sprintf(char *buf, const char *fmt, ...);
extern "C" void *malloc(unsigned int size);

class ScreenTextFont;
extern JetskiGame *LpGame;

extern char UITextureContentDirectory[];


// A track's records in the hall of fame (0xA0 bytes; 21 tracks per difficulty).
struct TrackRecords {
    char pad0[0x38];
    float m_lapRecord;                  // +0x38
    char pad3C[0xA0 - 0x3C];
};

// JetskiGameOptions::m_hallOfFame as records: easy, then hard.
struct HallOfFame {
    char pad0[0x8A8];
    TrackRecords m_records[2 * 21];     // +0x8A8
};

// Taking the record through a helper gives retail's `addu index, base`.
static inline TrackRecords *GetRecord(TrackRecords *records, int track) { return &records[track]; }

#define SET_COLOR(text, value)              \
    {                                       \
        unsigned int color = value;         \
        ScreenText *colored = (text);       \
        colored->SetColor(color);           \
    }

JetskiInGameStats *JetskiInGameStats::Create(ScreenIconManager *icons, ScreenTextManager *texts,
                                             ViewPortCode viewport, int x78, int leftWing, int rightWing)
{
    char name[0x110];
    char label[0x20];
    char time[0x80];
    char laps[0x10];
    int i;
    unsigned int color;
    int length;
    float x, y, w, h, u0, u1, v0, v1;
    float scale;

    m_twoPlayers = viewport;
    m_showLeftWing = leftWing;
    m_showRightWing = rightWing;
    *(int *)pad78 = x78;                // +0x78 (see JetskiInGameStats.h)
    y = x = 0.0f;
    ScreenTextFontItem *item = LpGame->m_fonts->GetScreenTextFont("info_c");
    m_font = item ? item->m_font : 0;
    item = LpGame->m_fonts->GetScreenTextFont("title_b");
    m_font70 = item ? item->m_font : 0;
    m_textManager = texts;
    m_texts = (ScreenText **)malloc(21 * sizeof(ScreenText *));
    for (i = 0; i < 21; i++)
        m_texts[i] = 0;
    m_iconManager = icons;
    m_icons = (ScreenIcon **)malloc(7 * sizeof(ScreenIcon *));
    for (i = 0; i < 7; i++)
        m_icons[i] = 0;

    for (i = leftWing ? 0 : 3; i < 7; i++) {
        switch (i) {
        case 0:
            sprintf(name, "%sGUIBarLeft.tga", UITextureContentDirectory);
            if (m_twoPlayers == 0 || LpGame->m_gameOptions->m_raceType == 4)
                x = 113.0f;
            else
                x = 47.0f;
            y = 21.0f;
            w = 22.0f;
            h = 56.0f;
            u0 = 30.0f;
            u1 = 52.0f;
            v0 = 4.0f;
            v1 = 60.0f;
            break;
        case 1:
            sprintf(name, "%sGUIBarLeft.tga", UITextureContentDirectory);
            if (m_twoPlayers == 0 || LpGame->m_gameOptions->m_raceType == 4)
                w = 93.0f;
            else
                w = 27.0f;
            x = 20.0f;
            y = 21.0f;
            h = 56.0f;
            u0 = 29.0f;
            u1 = 30.0f;
            v0 = 4.0f;
            v1 = 60.0f;
            break;
        case 2:
            sprintf(name, "%sGUIBarLeft.tga", UITextureContentDirectory);
            y = 21.0f;
            x = 0.0f;
            w = 20.0f;
            h = 56.0f;
            u0 = 9.0f;
            u1 = 29.0f;
            v0 = 4.0f;
            v1 = 60.0f;
            break;
        case 3:
            sprintf(name, "%sGUIBarRight.tga", UITextureContentDirectory);
            if (m_twoPlayers == 0)
                x = 451.0f;
            else if (m_twoPlayers == 1)
                x = 466.0f;
            else
                x = 146.0f;
            y = 21.0f;
            w = 26.0f;
            h = 72.0f;
            u0 = 15.0f;
            u1 = 41.0f;
            v0 = 28.0f;
            v1 = 100.0f;
            break;
        case 4:
            sprintf(name, "%sGUIBarRight.tga", UITextureContentDirectory);
            if (m_twoPlayers == 0) {
                x = 477.0f;
                w = 143.0f;
            } else {
                if (m_twoPlayers == 1)
                    x = 492.0f;
                else
                    x = 172.0f;
                w = 128.0f;
            }
            y = 21.0f;
            h = 72.0f;
            u0 = 41.0f;
            u1 = 42.0f;
            v0 = 28.0f;
            v1 = 100.0f;
            break;
        case 5:
            sprintf(name, "%sGUIBarRight.tga", UITextureContentDirectory);
            if (m_twoPlayers == 0)
                x = 620.0f;
            else if (m_twoPlayers == 1)
                x = 620.0f;
            else
                x = 300.0f;
            y = 21.0f;
            w = 22.0f;
            h = 72.0f;
            u0 = 42.0f;
            u1 = 64.0f;
            v0 = 28.0f;
            v1 = 100.0f;
            break;
        case 6:
            sprintf(name, "%sCLogo.tga", UITextureContentDirectory);
            if (m_twoPlayers == 0)
                x = 500.0f;
            else if (m_twoPlayers == 1)
                x = 512.0f;
            else
                x = 187.0f;
            y = 77.0f;
            w = 91.0f;
            h = 25.0f;
            u0 = 18.0f;
            u1 = 109.0f;
            v0 = 24.0f;
            v1 = 60.0f;
            break;
        }
        m_icons[i] = (new ScreenIcon)->Create(name, 0.0f, 0.0f);
        m_icons[i]->SetWidth(w);
        m_icons[i]->SetHeight(h);
        m_icons[i]->SetPosition(x, y);
        m_icons[i]->SetUVRect(u0, u1, v0, v1);
    }

    for (i = 0; i < 21; i++) {
        scale = 0.9f;
        length = 20;
        color = 0xFF6EF4FF;
        switch (i) {
        case 0:
            if (m_twoPlayers == 0) {
                x = 618.0f;
                y = 19.0f;
            } else {
                if (m_twoPlayers == 1)
                    x = 623.0f;
                else
                    x = 300.0f;
                y = 20.0f;
            }
            length = 40;
            break;
        case 1:
            if (m_twoPlayers == 0) {
                x = 623.0f;
                y = 20.0f;
            } else {
                if (m_twoPlayers == 1)
                    x = 623.0f;
                else
                    x = 300.0f;
                y = 18.0f;
            }
            length = 10;
            break;
        case 2:
            if (LpGame->m_gameOptions->m_raceType == 4) {
                x = 120.0f;
                y = 46.0f;
                scale = 1.2857f;
            } else {
                if (m_twoPlayers == 0)
                    x = 623.0f;
                else if (m_twoPlayers == 1)
                    x = 623.0f;
                else
                    x = 300.0f;
                y = 39.0f;
            }
            length = 10;
            break;
        case 3:
        case 18:
            if (m_twoPlayers == 0) {
                x = 623.0f;
                y = 58.0f;
            } else {
                if (m_twoPlayers == 1)
                    x = 623.0f;
                else
                    x = 300.0f;
                y = 58.0f;
            }
            length = 10;
            break;
        case 4:
            if (LpGame->m_gameOptions->m_raceType == 4) {
                x = 130.0f;
                y = 22.0f;
                color = 0xFFFFDE00;
            } else {
                if (m_twoPlayers == 0) {
                    if (LpGame->m_gameOptions->m_raceType == 2 || LpGame->m_gameOptions->m_raceType == 3)
                        x = 104.0f;
                    else
                        x = 104.0f;
                } else {
                    x = 38.0f;
                }
                y = 22.0f;
            }
            length = 8;
            break;
        case 5:
            if (LpGame->m_gameOptions->m_raceType == 4) {
                x = 98.0f;
                y = 22.0f;
                color = 0xFFFFDE00;
            } else {
                if (m_twoPlayers == 0) {
                    if (LpGame->m_gameOptions->m_raceType == 2)
                        x = 35.0f;
                    else
                        x = 104.0f;
                } else {
                    x = 38.0f;
                }
                y = 22.0f;
            }
            length = 8;
            break;
        case 6:
            if (m_twoPlayers == 0) {
                x = 42.0f;
                scale = 1.059f;
            } else {
                x = 22.0f;
                scale = 0.9531f;
            }
            y = 39.0f;
            length = 8;
            color = 0xFFBCF7FF;
            break;
        case 7:
            x = 70.0f;
            length = 8;
            y = 39.0f;
            scale = 1.29f;
            color = 0xFFBCF7FF;
            break;
        case 8:
            if (m_twoPlayers == 0) {
                x = 535.0f;
                y = 19.0f;
            } else {
                if (m_twoPlayers == 1)
                    x = 542.0f;
                else
                    x = 215.0f;
                y = 20.0f;
            }
            break;
        case 9:
            if (m_twoPlayers == 0) {
                x = 535.0f;
                y = 38.0f;
            } else {
                if (m_twoPlayers == 1)
                    x = 542.0f;
                else
                    x = 215.0f;
                y = 39.0f;
            }
            break;
        case 10:
            if (m_twoPlayers == 0) {
                x = 535.0f;
                y = 57.0f;
            } else {
                if (m_twoPlayers == 1)
                    x = 542.0f;
                else
                    x = 215.0f;
                y = 58.0f;
            }
            break;
        case 11:
            if (m_twoPlayers == 0) {
                x = 535.0f;
                y = 75.0f;
            } else {
                if (m_twoPlayers == 1)
                    x = 542.0f;
                else
                    x = 215.0f;
                y = 76.0f;
            }
            break;
        case 12:
            if (LpGame->m_gameOptions->m_raceType == 5) {
                x = 37.0f;
                y = 21.0f;
                color = 0xFFFFDE00;
            } else {
                x = 20.0f;
                y = 22.0f;
            }
            break;
        case 13:
            if (m_twoPlayers == 0) {
                x = 320.0f;
                y = 340.0f;
            } else if (m_twoPlayers == 1) {
                x = 320.0f;
                y = 126.0f;
            } else {
                x = 160.0f;
                y = 350.0f;
            }
            scale = 1.4f;
            color = 0xFFFFDE00;
            length = 40;
            break;
        case 14:
            if (m_twoPlayers == 0) {
                x = 320.0f;
                y = 360.0f;
            } else if (m_twoPlayers == 1) {
                x = 320.0f;
                y = 146.0f;
            } else {
                x = 160.0f;
                y = 370.0f;
            }
            scale = 1.4f;
            color = 0xFFFFDE00;
            length = 40;
            break;
        case 16:
            if (m_twoPlayers == 0) {
                x = 320.0f;
                y = 145.0f;
            } else if (m_twoPlayers == 1) {
                x = 320.0f;
                y = 70.0f;
            } else {
                x = 160.0f;
                y = 145.0f;
            }
            scale = 1.5f;
            length = 30;
            color = 0xFFE14646;
            break;
        case 15:
            if (m_twoPlayers == 0) {
                x = 320.0f;
                y = 220.0f;
            } else if (m_twoPlayers == 1) {
                x = 320.0f;
                y = 110.0f;
            } else {
                x = 160.0f;
                y = 220.0f;
            }
            scale = 1.5f;
            color = 0xFFFFDE00;
            length = 30;
            break;
        case 17:
            x = 20.0f;
            y = 410.0f;
            color = 0xFFD76464;
            scale = 1.5f;
            break;
        case 19:
            x = 20.0f;
            y = 400.0f;
            color = 0xFFFFDE00;
            break;
        case 20:
            x = 320.0f;
            color = 0xFFFFDE00;
            y = 32.0f;
            break;
        }
        m_texts[i] = new ScreenText(length, 0);
        m_texts[i]->SetPosition(x, y);
        m_texts[i]->SetScale(scale);
        {
            // SetColor written out: through the inline's parameter copy, color would become
            // an optimizer temp and swap $s1/$s2 with length.
            ScreenText *t = m_texts[i];
            t->m_color = color;
            t->m_x5D_bit4 = 1;
        }
    }

    m_texts[13]->SetString(LpGame->m_text->GetString(80));
    m_textManager->PositionText(m_texts[13], ScreenTextManager::CenterHorizontalJustification, m_font);
    m_texts[16]->SetString(LpGame->m_text->GetString(66));
    m_textManager->PositionText(m_texts[16], ScreenTextManager::CenterHorizontalJustification, m_font);
    strncpy(label, LpGame->m_text->GetString(65), 0x17);
    label[0x18] = 0;    // (retail terminates one past the copied length)
    strcat(label, "!");
    m_texts[15]->SetString(label);
    m_textManager->PositionText(m_texts[15], ScreenTextManager::CenterHorizontalJustification, m_font);

    m_recordLabel = new ScreenText(40, 0);
    m_recordLabel->SetString(LpGame->m_text->GetString(69));
    SET_COLOR(m_recordLabel, 0xFF6EF4FF)
    if (m_twoPlayers == 0) {
        m_recordLabel->SetPosition(535.0f, 57.0f);
    } else {
        if (m_twoPlayers == 1)
            x = 535.0f;
        else
            x = 212.0f;
        m_recordLabel->SetPosition(x, 58.0f);
        m_recordLabel->SetScale(0.9f);
    }
    JetskiGameOptions *options = LpGame->m_gameOptions;
    m_textManager->PositionText(m_recordLabel, ScreenTextManager::RightJustification, m_font);
    {
        float record;
        if (options->m_difficulty == 3)
            record = GetRecord(((HallOfFame *)options)->m_records + 21, options->m_x2C)->m_lapRecord;
        else
            record = GetRecord(((HallOfFame *)options)->m_records, options->m_x2C)->m_lapRecord;
        if (record == 0.0f || record == 3.40282347e+38f) {
            strcpy(time, "--:--.--");
        } else {
            int minutes;
            if (record < 0.0f)
                record = -1.0f * record;
            if (!(record < 5999.99f)) {
                record = 59.99f;
                minutes = 99;
            } else {
                minutes = 0;
                while (!(record < 60.0f)) {
                    record -= 60.0f;
                    minutes++;
                }
            }
            sprintf(time, "%2d:%05.2f", minutes, record);
        }
    }
    m_texts[18]->SetString(time);
    m_textManager->PositionText(m_texts[18], ScreenTextManager::RightJustification, m_font);

    m_bestLapLabel = new ScreenText(40, 0);
    m_bestLapLabel->SetString(LpGame->m_text->GetString(70));
    SET_COLOR(m_bestLapLabel, 0xFF6EF4FF)
    if (m_twoPlayers == 0) {
        m_bestLapLabel->SetPosition(537.0f, 18.0f);
    } else {
        if (m_twoPlayers == 1)
            x = 541.0f;
        else
            x = 217.0f;
        m_bestLapLabel->SetPosition(x, 18.0f);
        m_bestLapLabel->SetScale(0.9f);
    }
    m_textManager->PositionText(m_bestLapLabel, ScreenTextManager::RightJustification, m_font);

    m_lapTimeLabel = new ScreenText(40, 0);
    SET_COLOR(m_lapTimeLabel, 0xFF6EF4FF)
    if (m_twoPlayers == 0) {
        { float px = 537.0f; float py = 37.0f; m_lapTimeLabel->SetPosition(px, py); }
    } else {
        if (m_twoPlayers == 1)
            x = 541.0f;
        else
            x = 217.0f;
        m_lapTimeLabel->SetPosition(x, 37.0f);
        m_lapTimeLabel->SetScale(0.9f);
    }
    m_lapTimeLabel->SetString(LpGame->m_text->GetString(71));

    m_lapLabel = new ScreenText(40, 0);
    SET_COLOR(m_lapLabel, 0xFF6EF4FF)
    if (m_twoPlayers == 0) {
        { float px = 78.0f; float py = 21.0f; m_lapLabel->SetPosition(px, py); }
    } else {
        if (m_twoPlayers == 1)
            x = 35.0f;
        else
            x = 35.0f;
        m_lapLabel->SetPosition(x, 16.0f);
        m_lapLabel->SetScale(0.9f);
    }
    m_lapLabel->SetString(LpGame->m_text->GetString(71));
    if (LpGame->m_gameOptions->m_raceType == 2 || LpGame->m_gameOptions->m_raceType == 3) {
        if (m_twoPlayers == 0)
            m_texts[5]->SetString("");
    } else {
        sprintf(laps, "/%d", LpGame->m_gameOptions->m_x30);
        m_texts[5]->SetString(laps);
    }
    m_textManager->PositionText(m_texts[5], ScreenTextManager::LeftJustification, m_font);
    if (m_twoPlayers == 0)
        m_textManager->PositionText(m_lapLabel, ScreenTextManager::RightJustification, m_font);

    m_timeBehindLabel = new ScreenText(40, 0);
    m_timeBehindLabel->SetString(LpGame->m_text->GetString(73));
    if (m_twoPlayers == 0) {
        m_timeBehindLabel->SetPosition(537.0f, 56.0f);
    } else {
        if (m_twoPlayers == 1)
            x = 541.0f;
        else
            x = 217.0f;
        m_timeBehindLabel->SetPosition(x, 56.0f);
        m_timeBehindLabel->SetScale(0.9f);
    }
    m_textManager->PositionText(m_timeBehindLabel, ScreenTextManager::RightJustification, m_font);
    {
        unsigned int yellow = 0xFFFFDE00;
        SET_COLOR(m_timeBehindLabel, yellow)
        SET_COLOR(m_texts[3], yellow)
    }
    m_wasAhead = 0;

    m_positionSuffix = new ScreenText(40, 0);
    SET_COLOR(m_positionSuffix, 0xFFBCF7FF)
    m_textManager->PositionText(m_texts[6], ScreenTextManager::LeftJustification, m_font70);

    m_rankTexts[0] = new ScreenText(40, 0);
    m_rankTexts[0]->SetString("");
    if (m_twoPlayers == 0) {
        m_rankTexts[0]->SetPosition(525.0f, 19.0f);
    } else {
        if (m_twoPlayers == 1)
            x = 535.0f;
        else
            x = 212.0f;
        m_rankTexts[0]->SetPosition(x, 20.0f);
        m_rankTexts[0]->SetScale(0.9f);
    }
    SET_COLOR(m_rankTexts[0], 0xFF6EF4FF)
    m_rankTexts[1] = new ScreenText(40, 0);
    m_rankTexts[1]->SetString("");
    if (m_twoPlayers == 0) {
        m_rankTexts[1]->SetPosition(525.0f, 38.0f);
    } else {
        if (m_twoPlayers == 1)
            x = 535.0f;
        else
            x = 212.0f;
        m_rankTexts[1]->SetPosition(x, 39.0f);
        m_rankTexts[1]->SetScale(0.9f);
    }
    SET_COLOR(m_rankTexts[1], 0xFF6EF4FF)
    m_rankTexts[2] = new ScreenText(40, 0);
    m_rankTexts[2]->SetString("");
    if (m_twoPlayers == 0) {
        m_rankTexts[2]->SetPosition(525.0f, 57.0f);
    } else {
        if (m_twoPlayers == 1)
            x = 535.0f;
        else
            x = 212.0f;
        m_rankTexts[2]->SetPosition(x, 58.0f);
        m_rankTexts[2]->SetScale(0.9f);
    }
    SET_COLOR(m_rankTexts[2], 0xFF6EF4FF)
    m_rankTexts[3] = new ScreenText(40, 0);
    m_rankTexts[3]->SetString("");
    if (m_twoPlayers == 0) {
        m_rankTexts[3]->SetPosition(525.0f, 75.0f);
    } else {
        if (m_twoPlayers == 1)
            x = 535.0f;
        else
            x = 212.0f;
        m_rankTexts[3]->SetPosition(x, 76.0f);
        m_rankTexts[3]->SetScale(0.9f);
    }
    SET_COLOR(m_rankTexts[3], 0xFF6EF4FF)

    m_texts[19]->SetString(LpGame->m_text->GetString(64));
    m_textManager->PositionText(m_texts[19], ScreenTextManager::LeftJustification, m_font);

    m_texts[2]->SetScale(1.143f);
    m_texts[3]->SetScale(1.143f);
    m_texts[1]->SetScale(1.143f);
    m_texts[6]->SetScale(1.34f);
    m_positionSuffix->SetScale(0.834f);
    m_texts[19]->SetScale(1.5f);
    m_texts[20]->SetScale(1.34f);
    if (LpGame->m_gameOptions->m_raceType == 4)
        m_texts[2]->SetScale(1.29f);
    m_bestLapLabel->SetScale(1.143f);
    m_timeBehindLabel->SetScale(1.143f);
    m_lapTimeLabel->SetScale(1.143f);
    if (LpGame->m_gameOptions->m_raceType != 4) {
        m_texts[4]->SetScale(1.143f);
    } else {
        m_texts[4]->SetScale(1.0f);
        m_texts[12]->SetScale(1.0f);
    }
    m_texts[5]->SetScale(1.143f);
    m_lapLabel->SetScale(1.143f);
    return this;
}


// ---- src/JetskiInGameStats/Setters.cpp
// Overlay toggles: each Set* turns one element of the in-game HUD on or off.


int JetskiInGameStats::SetCompetition(JetskiCompetition *competition)
{
    m_competition = competition;
    return 1;
}

// A negative argument means "use the game options".
void JetskiInGameStats::SetShowing(int lapTimes, int positions)
{
    if (lapTimes < 0)
        lapTimes = LpGame->m_gameOptions->m_hudFlags[0] & 1;
    m_showing[0] = lapTimes;
    if (positions < 0)
        positions = LpGame->m_gameOptions->m_hudFlags[1] & 1;
    m_showing[1] = positions;
}


void JetskiInGameStats::SetTotalRaceTime(int on) { m_showTotalRaceTime = on; }
void JetskiInGameStats::SetRecordLapTime(int on) { m_showRecordLapTime = on; }
void JetskiInGameStats::SetBestLap(int on) { m_showBestLap = on; }
void JetskiInGameStats::SetLapTime(int on) { m_showLapTime = on; }
void JetskiInGameStats::SetTimeBehindLeader(int on) { m_showTimeBehindLeader = on; }
void JetskiInGameStats::SetPosition(int on) { m_showPosition = on; }
void JetskiInGameStats::SetLap(int on) { m_showLap = on; }
void JetskiInGameStats::SetCountdownText(int on) { m_showCountdownText = on; }
void JetskiInGameStats::SetScoreText(int on) { m_showScoreText = on; }
void JetskiInGameStats::SetNewRecordText(int on) { m_showNewRecordText = on; }
void JetskiInGameStats::SetReplayText(int on) { m_showReplayText = on; }
void JetskiInGameStats::SetRaceTypeText(int on) { m_showRaceTypeText = on; }
void JetskiInGameStats::SetWrongWayText(int on) { m_showWrongWayText = on; }

void JetskiInGameStats::SetWings(int left, int right)
{
    m_showLeftWing = left;
    m_showRightWing = right;
}


// ---- src/JetskiInGameStats/SetShowing.cpp


extern JetskiGame *LpGame;




// ---- src/JetskiInGameStats/PrepareGeometry.cpp





int JetskiInGameStats::PrepareGeometry()
{
    for (int i = 0; i < 7; i++) {
        if (m_icons[i]) {
            TextureMap *t = m_icons[i]->m_texture;
            if (10.0f > t->m_usage)
                t->m_usage = 10.0f;
        }
    }
    if (m_font) {
        TextureMap *t = m_font->m_texture;
        if (10.0f > t->m_usage)
            t->m_usage = 10.0f;
    }
    if (m_font70) {
        TextureMap *t = m_font70->m_texture;
        if (10.0f > t->m_usage)
            t->m_usage = 10.0f;
    }
    return GameObject::PrepareGeometry();
}


// ---- src/JetskiInGameStats/Render3D.cpp


class JetskiCompetition;


int JetskiInGameStats::Render3D()
{
    if (!m_competition)
        return GameObject::Render3D();
    if (m_showing[LpGame->m_currentPlayer]) {
        RenderIcons();
        static int scoreIDall = -1;     // unused profiler ID
        if (m_showRecordLapTime)
            RenderRecordLapTime();
        if (m_showBestLap)
            RenderBestLap();
        if (m_showLapTime)
            RenderLapTime();
        if (m_showTimeBehindLeader)
            RenderTimeBehindLeader();
        if (m_showPosition)
            RenderPosition();
        if (m_showLap)
            RenderLap();
        if (m_showCountdownText)
            RenderCountdownText();
        if (m_showScoreText)
            RenderScoreText();
        if (m_showWrongWayText)
            RenderWrongWayText();
        if (m_showReplayText)
            RenderReplayText();
        if (m_showRaceTypeText)
            RenderRaceTypeText();
    }
    RenderSecretFoundText();
    return GameObject::Render3D();
}


// ---- src/JetskiInGameStats/Tick.cpp


int JetskiInGameStats::Tick(float dt)
{
    for (int i = 0; i < 2; i++) {
        if (i > 0 && !m_twoPlayers)
            break;
        if (m_secretTime[i] >= 0.0f)
            m_secretTime[i] -= dt;
        if (m_wrongWayTime[i] >= 0.0f)
            m_wrongWayTime[i] += dt;
    }
    return GameObject::Tick(dt);
}


// ---- src/JetskiInGameStats/RenderIcons.cpp
class ScreenIcon;



void JetskiInGameStats::RenderIcons()
{
    if (m_showLeftWing) {
        m_iconManager->AddIcon(m_icons[0], 0);
        m_iconManager->AddIcon(m_icons[1], 0);
        m_iconManager->AddIcon(m_icons[2], 0);
    }
    if (m_showRightWing) {
        m_iconManager->AddIcon(m_icons[3], 0);
        m_iconManager->AddIcon(m_icons[4], 0);
        m_iconManager->AddIcon(m_icons[5], 0);
        m_iconManager->AddIcon(m_icons[6], 0);
    }
}


// ---- src/JetskiInGameStats/RenderRecordLapTime.cpp
class ScreenText;
class ScreenTextFont;




void JetskiInGameStats::RenderRecordLapTime()
{
    m_textManager->AddText(m_recordLabel, m_font);
    m_textManager->AddText(m_texts[18], m_font);
}


// ---- src/JetskiInGameStats/RenderBestLap.cpp
extern "C" int sprintf(char *buf, const char *fmt, ...);
extern "C" char *strncpy(char *dst, const char *src, unsigned int n);
extern "C" char *strcpy(char *dst, const char *src);

class ScreenTextFont;











void JetskiInGameStats::RenderBestLap()
{
    char buf[0x80];
    m_textManager->AddText(m_bestLapLabel, m_font);
    float time = m_competition->m_vehicles[m_competition->m_cameraControllers[LpGame->m_currentPlayer]->m_x64]
                     ->m_ownStatus->m_bestLap;
    if (time == 0.0f || time == 3.40282347e+38f) {
        strcpy(buf, "--:--.--");
    } else {
        int minutes;
        if (time < 0.0f)
            time = -1.0f * time;
        if (!(time < 5999.99f)) {
            time = 59.99f;
            minutes = 99;
        } else {
            minutes = 0;
            while (!(time < 60.0f)) {
                time -= 60.0f;
                minutes++;
            }
        }
        sprintf(buf, "%2d:%05.2f", minutes, time);
    }
    ScreenText *text = m_texts[1];
    text->m_positionDirty = 1;
    strncpy(text->m_text, buf, text->m_maxChars);
    text->m_text[text->m_maxChars] = 0;
    text->m_x5D_bit4 = 1;
    text->m_dirty = 1;
    text->m_cellsDirty = 1;
    m_textManager->PositionText(m_texts[1], ScreenTextManager::RightJustification, m_font);
    m_textManager->AddText(m_texts[1], m_font);
}


// ---- src/JetskiInGameStats/RenderLapTime.cpp


extern "C" char *strncpy(char *dst, const char *src, unsigned int n);
extern "C" char *strcpy(char *dst, const char *src);
extern "C" int sprintf(char *buf, const char *fmt, ...);



class ScreenTextFont;








extern JetskiGame *LpGame;







#define SET_TEXT(text, str)                                     \
    text->m_positionDirty = 1;                                       \
    strncpy(text->m_text, str, text->m_maxChars);            \
    text->m_text[text->m_maxChars] = 0;                      \
    text->m_x5D_bit4 = 1;                                       \
    text->m_dirty = 1;                                       \
    text->m_cellsDirty = 1;

static inline void FormatTime(char *buf, float time)
{
    if (time == 0.0f || time == 3.40282347e+38f) {
        strcpy(buf, "--:--.--");
    } else {
        int minutes;
        if (time < 0.0f)
            time = -1.0f * time;
        if (!(time < 5999.99f)) {
            time = 59.99f;
            minutes = 99;
        } else {
            minutes = 0;
            while (!(time < 60.0f)) {
                time -= 60.0f;
                minutes++;
            }
        }
        sprintf(buf, "%2d:%05.2f", minutes, time);
    }
}

// The current lap time; when a lap completes, also posts the lap time as
// info text, flagged when it is a new lap record.
void JetskiInGameStats::RenderLapTime()
{
    char buf[0x80];
    char message[0x80];
    m_textManager->PositionText(m_lapTimeLabel, ScreenTextManager::RightJustification, m_font);
    m_textManager->AddText(m_lapTimeLabel, m_font);
    int player = LpGame->m_currentPlayer;
    int vehicle = m_competition->m_cameraControllers[player]->m_x64;
    int lap = m_competition->m_vehicles[vehicle]->m_ownStatus->m_lap + 1;
    if (m_competition->m_vehicles[vehicle]->m_newLap) {
        if (lap < 101) {
            int other = m_competition->m_vehicles[vehicle + 5]->m_player;
            FormatTime(buf, m_competition->m_vehicles[vehicle]->m_ownStatus->m_lastLap);
            JetskiGameOptions *options = LpGame->m_gameOptions;
            float record;
            if (options->m_difficulty == 3)
                record = GetRecord(((HallOfFame *)options)->m_records + 21, options->m_x2C)->m_lapRecord;
            else
                record = GetRecord(((HallOfFame *)options)->m_records, options->m_x2C)->m_lapRecord;
            float time = m_competition->m_vehicles[other + 5]->m_ownStatus->m_lastLap;
            if (!(record < time) && !(m_competition->m_vehicles[vehicle]->m_ownStatus->m_bestLap < time)) {
                char *newRecord = LpGame->m_text->GetString(0x41);
                sprintf(message, "%s %s  %s", newRecord, LpGame->m_text->GetString(0x47), buf);
            } else {
                sprintf(message, "%s  %s", LpGame->m_text->GetString(0x47), buf);
            }
            float scale = 1.0f;
            LpGame->m_competition->m_infoText->AddInfoText(message, player, 5.0f, 0, scale);
        }
        m_lastLap[player] = lap;
        m_competition->m_vehicles[vehicle]->m_newLap = 0;
    }
    FormatTime(buf, m_competition->m_vehicles[vehicle]->m_ownStatus->m_x6D8);
    ScreenText *text = m_texts[2];
    SET_TEXT(text, buf)
    m_textManager->PositionText(m_texts[2], ScreenTextManager::RightJustification, m_font);
    m_textManager->AddText(m_texts[2], m_font);
}


// ---- src/JetskiInGameStats/RenderCountdownText.cpp
#pragma fast_fptosi on

extern "C" int sprintf(char *buf, const char *fmt, ...);
extern "C" char *strncpy(char *dst, const char *src, unsigned int n);

class ScreenTextFont;











void JetskiInGameStats::RenderCountdownText()
{
    float countdown = m_competition->m_vehicles[m_competition->m_cameraControllers[LpGame->m_currentPlayer]->m_x64]
                          ->m_ownStatus->m_timeRemaining;
    if (countdown != m_lastCountdown) {
        char buf[16];
        sprintf(buf, "%d", (int)countdown);
        ScreenText *text = m_texts[7];
        text->m_positionDirty = 1;
        strncpy(text->m_text, buf, text->m_maxChars);
        text->m_text[text->m_maxChars] = 0;
        text->m_x5D_bit4 = 1;
        text->m_dirty = 1;
        text->m_cellsDirty = 1;
        m_textManager->PositionText(m_texts[7], ScreenTextManager::CenterHorizontalJustification,
                                    m_font70);
        m_lastCountdown = countdown;
    }
    m_textManager->AddText(m_texts[7], m_font70);
}
#pragma fast_fptosi reset


// ---- src/JetskiInGameStats/RenderScoreText.cpp

extern "C" char *strncpy(char *dst, const char *src, unsigned int n);
extern "C" char *strcpy(char *dst, const char *src);
extern "C" int sprintf(char *buf, const char *fmt, ...);



class ScreenTextFont;






extern JetskiGame *LpGame;







#define SET_TEXT(text, str)                                     \
    text->m_positionDirty = 1;                                       \
    strncpy(text->m_text, str, text->m_maxChars);            \
    text->m_text[text->m_maxChars] = 0;                      \
    text->m_x5D_bit4 = 1;                                       \
    text->m_dirty = 1;                                       \
    text->m_cellsDirty = 1;

// The score panel: in mode 5 the racer list with names and ranks; otherwise
// the score, count and a timer coloured by how much time is left.
void JetskiInGameStats::RenderScoreText()
{
    char buf[0x100];
    char name[0x40];
    char timeText[0x80];
    RaceStatusStruct_Tag *status = m_competition->m_raceStatus->m_vehicles;
    if (LpGame->m_gameOptions->m_raceType == 5) {
        int i = 0;
        int text = 8;
        for (; status; status = status->m_next, i++, text++) {
            if (status->m_flags & 0x80) {
                sprintf(buf, "%s %d", LpGame->m_text->GetString(0x4A), i + 1);
                ScreenText *t = m_texts[12];
                SET_TEXT(t, buf)
                m_textManager->PositionText(m_texts[12], ScreenTextManager::LeftJustification, m_font);
                m_textManager->AddText(m_texts[12], m_font);
            }
            sprintf(buf, "%d:", i + 1);
            ScreenText *t = m_rankTexts[i];
            SET_TEXT(t, buf)
            m_textManager->PositionText(m_rankTexts[i], ScreenTextManager::RightJustification, m_font);
            m_textManager->AddText(m_rankTexts[i], m_font);
            sprintf(buf, "%s", LpGame->m_text->GetString(0x4B));
            t = m_texts[text];
            SET_TEXT(t, buf)
            strcpy(name, m_texts[text]->m_text);
            name[status->m_x30] = 0;
            t = m_texts[text];
            SET_TEXT(t, name)
            m_textManager->PositionText(m_texts[text], ScreenTextManager::LeftJustification, m_font);
            m_textManager->AddText(m_texts[text], m_font);
        }
    } else {
        char *str = LpGame->m_text->GetString(0x4C);
        ScreenText *t = m_texts[12];
        SET_TEXT(t, str)
        m_textManager->PositionText(m_texts[12], ScreenTextManager::LeftJustification, m_font);
        m_textManager->AddText(m_texts[12], m_font);
        sprintf(buf, "%2d", LpGame->m_competition->m_timeElimTime);
        t = m_texts[4];
        SET_TEXT(t, buf)
        m_textManager->PositionText(m_texts[4], ScreenTextManager::RightJustification, m_font);
        m_textManager->AddText(m_texts[4], m_font);
        float time = m_competition->m_vehicles[m_competition->m_cameraControllers[LpGame->m_currentPlayer]->m_x64]
                         ->m_ownStatus->m_timeRemaining;
        FormatTime(timeText, time);
        // Colour by the time left: plenty, some, little.
        ScreenText *timer;
        unsigned int color;
        if (time > 30.0f) {
            color = 0xFF6EF4FF;
            timer = m_texts[2];
            timer->m_color = color;
            timer->m_x5D_bit4 = 1;
        } else if (time > 15.0f) {
            color = 0xFFFFDE00;
            timer = m_texts[2];
            timer->m_color = color;
            timer->m_x5D_bit4 = 1;
        } else {
            color = 0xFFD76464;
            timer = m_texts[2];
            timer->m_color = color;
            timer->m_x5D_bit4 = 1;
        }
        t = m_texts[2];
        SET_TEXT(t, timeText)
        m_textManager->PositionText(m_texts[2], ScreenTextManager::RightJustification, m_font);
        m_textManager->AddText(m_texts[2], m_font);
    }
}


// ---- src/JetskiInGameStats/RenderTimeBehindLeader.cpp
extern "C" int sprintf(char *buf, const char *fmt, ...);
extern "C" char *strncpy(char *dst, const char *src, unsigned int n);
extern "C" char *strcpy(char *dst, const char *src);

class ScreenTextFont;











#define SET_TEXT(text, str)                                     \
    text->m_positionDirty = 1;                                       \
    strncpy(text->m_text, str, text->m_maxChars);            \
    text->m_text[text->m_maxChars] = 0;                      \
    text->m_x5D_bit4 = 1;                                       \
    text->m_dirty = 1;                                       \
    text->m_cellsDirty = 1;

void JetskiInGameStats::RenderTimeBehindLeader()
{
    char text[0x80];
    char time[0x80];
    RaceStatusStruct_Tag *state = m_competition->m_vehicles[m_competition->m_cameraControllers[LpGame->m_currentPlayer]->m_x64]
                                  ->m_ownStatus;
    if (state->m_flags & 1) {
        strcpy(text, "--:--.--");
    } else {
        float behind = state->m_x6DC;
        if (behind > 599.99f)
            behind = 599.99f;
        else if (behind < -599.99f)
            behind = -599.99f;
        float t = behind;
        if (behind == 0.0f || behind == 3.40282347e+38f) {
            strcpy(time, "--:--.--");
        } else {
            int minutes;
            if (t < 0.0f)
                t = -1.0f * t;
            if (!(t < 5999.99f)) {
                t = 59.99f;
                minutes = 99;
            } else {
                minutes = 0;
                while (!(t < 60.0f)) {
                    t -= 60.0f;
                    minutes++;
                }
            }
            sprintf(time, "%2d:%05.2f", minutes, t);
        }
        int ahead = behind > 0.0f;
        // Skip the leading digit space of "%2d".
        if (ahead)
            sprintf(text, "-%s", time + 1);
        else
            sprintf(text, "+%s", time + 1);
        if (ahead != m_wasAhead) {
            unsigned int color;
            if (ahead)
                color = 0xFFD76464;
            else
                color = 0xFFFFDE00;
            m_timeBehindLabel->SetColor(color);
            m_texts[3]->SetColor(color);
            m_wasAhead = ahead;
        }
    }
    m_textManager->AddText(m_timeBehindLabel, m_font);
    ScreenText *line = m_texts[3];
    SET_TEXT(line, text)
    m_textManager->PositionText(m_texts[3], ScreenTextManager::RightJustification, m_font);
    m_textManager->AddText(m_texts[3], m_font);
}


// ---- src/JetskiInGameStats/RenderLap.cpp
extern "C" int sprintf(char *buf, const char *fmt, ...);
extern "C" char *strncpy(char *dst, const char *src, unsigned int n);
extern "C" char *strcpy(char *dst, const char *src);

class ScreenTextFont;









extern JetskiGame *LpGame;



#define SET_TEXT(text, buf)                                     \
    text->m_positionDirty = 1;                                       \
    strncpy(text->m_text, buf, text->m_maxChars);            \
    text->m_text[text->m_maxChars] = 0;                      \
    text->m_x5D_bit4 = 1;                                       \
    text->m_dirty = 1;                                       \
    text->m_cellsDirty = 1;

void JetskiInGameStats::RenderLap()
{
    char buf[0x80];
    int lap = m_competition->m_vehicles[m_competition->m_cameraControllers[LpGame->m_currentPlayer]->m_x64]
                  ->m_ownStatus->m_lap + 1;
    if (LpGame->m_gameOptions->m_raceType == 2 || LpGame->m_gameOptions->m_raceType == 3) {
        if (m_twoPlayers)
            return;
        sprintf(buf, "%d", lap);
        ScreenText *text = m_texts[4];
        SET_TEXT(text, buf)
    } else {
        if (lap > LpGame->m_gameOptions->m_x30)
            lap = LpGame->m_gameOptions->m_x30;
        sprintf(buf, "%d", lap);
        ScreenText *text = m_texts[4];
        SET_TEXT(text, buf)
        m_textManager->AddText(m_texts[5], m_font);
    }
    m_textManager->PositionText(m_texts[4], ScreenTextManager::RightJustification, m_font);
    m_textManager->AddText(m_texts[4], m_font);
    if (!m_twoPlayers)
        m_textManager->AddText(m_lapLabel, m_font);
}


// ---- src/JetskiInGameStats/RenderPosition.cpp
// RenderPosition__17JetskiInGameStatsFv  @ 0x002AA1E0
// NEAR-MISS: 0x3A0 vs 0x3A4, prologue scheduling only: retail copies this to $s0 before the
// position chain (player index in $a0) and leaves the position==0 bnez delay slot empty.

#pragma alignlabel off

extern "C" char *strncpy(char *dst, const char *src, unsigned int n);
extern "C" int sprintf(char *buf, const char *fmt, ...);



class ScreenTextFont;









extern JetskiGame *LpGame;



#define SET_TEXT(text, str)                                     \
    text->m_positionDirty = 1;                                       \
    strncpy(text->m_text, str, text->m_maxChars);            \
    text->m_text[text->m_maxChars] = 0;                      \
    text->m_x5D_bit4 = 1;                                       \
    text->m_dirty = 1;                                       \
    text->m_cellsDirty = 1;

void JetskiInGameStats::RenderPosition()
{
    char buf[0x10];
    int player = LpGame->m_currentPlayer;
    JetskiGameOptions *options = LpGame->m_gameOptions;
    int position = m_competition->m_vehicles[m_competition->m_cameraControllers[player]->m_x64]->m_ownStatus->m_place;
    if (!position)
        position = 1;
    if (position != m_lastPosition) {
        sprintf(buf, "%d", position);
        ScreenText *text = m_texts[6];
        SET_TEXT(text, buf)
        if (!m_twoPlayers) {
            // "st", "nd", "rd", ...
            char *suffix = LpGame->m_text->GetString(position - 1);
            ScreenText *line = m_positionSuffix;
            SET_TEXT(line, suffix)
            _D3DVECTOR at = m_texts[6]->GetPosition();
            if (options->m_language == 2)
                m_positionSuffix->SetPosition(32.0f + at.x, 12.0f + at.y);
            else
                m_positionSuffix->SetPosition(32.0f + at.x, 3.0f + at.y);
            m_textManager->PositionText(m_positionSuffix, ScreenTextManager::LeftJustification, m_font70);
        }
        m_lastPosition = position;
    }
    m_textManager->AddText(m_texts[6], m_font70);
    if (!m_twoPlayers)
        m_textManager->AddText(m_positionSuffix, m_font70);
}
#pragma alignlabel reset


// ---- src/JetskiInGameStats/RenderWrongWayText.cpp
#pragma fast_fptosi on

class ScreenText;
class ScreenTextFont;









// After three seconds going the wrong way, blink the warning once a second.
void JetskiInGameStats::RenderWrongWayText()
{
    int player = LpGame->m_currentPlayer;
    if (m_competition->m_vehicles[m_competition->m_cameraControllers[player]->m_x64]->m_wrongWay) {
        if (m_wrongWayTime[player] < 0.0f) {
            m_wrongWayTime[player] = 0.0f;
        } else if (m_wrongWayTime[player] >= 3.0f) {
            if ((int)m_wrongWayTime[player] % 2)
                m_textManager->AddText(m_texts[16], m_font);
        }
    } else {
        m_wrongWayTime[player] = -1.0f;
    }
}
#pragma fast_fptosi reset


// ---- src/JetskiInGameStats/RenderReplayText.cpp
class ScreenText;
class ScreenTextFont;




void JetskiInGameStats::RenderReplayText()
{
    m_textManager->AddText(m_texts[19], m_font);
}


// ---- src/JetskiInGameStats/RenderRaceTypeText.cpp
#pragma alignlabel off

extern "C" char *strncpy(char *dst, const char *src, unsigned int n);



class ScreenTextFont;




extern JetskiGame *LpGame;



#define SET_TEXT(text, str)                                     \
    text->m_positionDirty = 1;                                       \
    strncpy(text->m_text, str, text->m_maxChars);            \
    text->m_text[text->m_maxChars] = 0;                      \
    text->m_x5D_bit4 = 1;                                       \
    text->m_dirty = 1;                                       \
    text->m_cellsDirty = 1;

void JetskiInGameStats::RenderRaceTypeText()
{
    if (((JetskiCareerCompetition *)LpGame->m_competition)->m_raceType == 1) {
        char *str = LpGame->m_text->GetString(0x5E);
        ScreenText *text = m_texts[20];
        SET_TEXT(text, str)
    } else {
        char *str = LpGame->m_text->GetString(0xC4);
        ScreenText *text = m_texts[20];
        SET_TEXT(text, str)
    }
    m_textManager->PositionText(m_texts[20], ScreenTextManager::CenterHorizontalJustification, m_font);
    m_textManager->AddText(m_texts[20], m_font);
}
#pragma alignlabel reset


// ---- src/JetskiInGameStats/RenderSecretFoundText.cpp
extern "C" char *strncpy(char *dst, const char *src, unsigned int n);

class ScreenTextFont;









void JetskiInGameStats::RenderSecretFoundText()
{
    int player = LpGame->m_currentPlayer;
    JetskiCompetition *competition = m_competition;
    int vehicle = competition->m_cameraControllers[player]->m_x64;
    if (m_competition->m_x2D8 && competition->m_x2DC == vehicle) {
        if (m_secretTime[player] == -1.0f) {
            m_secretTime[player] = 3.5f;
        } else if (m_secretTime[player] < 0.0f) {
            m_competition->m_x2D8 = 0;
            m_secretTime[player] = -1.0f;
        }
        ScreenText *text = m_texts[14];
        JetskiCompetition *c = m_competition;
        text->m_positionDirty = 1;
        strncpy(text->m_text, &c->m_x2E0, text->m_maxChars);
        text->m_text[text->m_maxChars] = 0;
        text->m_x5D_bit4 = 1;
        text->m_dirty = 1;
        text->m_cellsDirty = 1;
        m_textManager->PositionText(m_texts[14], ScreenTextManager::CenterHorizontalJustification, m_font);
        m_textManager->AddText(m_texts[13], m_font);
        m_textManager->AddText(m_texts[14], m_font);
    }
}


// ---- src/JetskiHeadingArrows/ctor.cpp
// (Same as __ct__17TextureMapManagerFv, different init/vtable.)
//
// __ct__10GameObjectFi(a0);
// *(a0+8) = D_00448550;  // vtable
// *(a0+0x30) = 0; *(a0+0x4C) = 0; *(a0+0x50) = 0; *(a0+0x3C) = 0;
// *(a0+0x44) = 0; *(a0+0x40) = 0; *(a0+0x48) = 0;
// return a0;

JetskiHeadingArrows::JetskiHeadingArrows(int enabled) : GameObject(enabled)
{
    m_competition = 0;
    m_show[0] = 0;
    m_show[1] = 0;
    m_cameras[0] = 0;
    m_vehicles[0] = 0;
    m_cameras[1] = 0;
    m_vehicles[1] = 0;
}


// ---- src/JetskiHeadingArrows/dtor.cpp

// The arrows' GameObject part is a second base at +8, behind 8 bytes of
// something else (as for JetskiCountDown's sprites).

JetskiHeadingArrows::~JetskiHeadingArrows()
{
    for (int i = 0; i < 2; i++) {
        if (m_arrows[i])
            m_arrows[i]->Release();
    }
}


// ---- src/JetskiHeadingArrows/Create.cpp


// SoultreeObject's first base: 8 bytes in front of its GameObject subobject.



// Retail's min: min.s into $f8, stored through a stack temporary.
static inline float MinS(float a, float b)
{
    float r;
    asm("min.s $f8, %1, %2; swc1 $f8, 0(%0)" : : "r"(&r), "f"(a), "f"(b) : "$f8", "memory");
    return r;
}


JetskiHeadingArrows *JetskiHeadingArrows::Create()
{
    for (int i = 0; i < 2; i++) {
        m_arrows[i] = new PSXRenderableObject(0);
        m_arrows[i]->Create("arrow.slt", 0, 0);
        m_arrows[i]->m_x190_bit4 = 1;
    }
    _D3DVECTOR min, max;
    m_arrows[0]->GetHierarchyBoundingBox(&min, &max);
    float size = MinS(max.x, max.y);
    size = MinS(size, max.z);
    m_scale = 2.0f * size;
    m_x58 = 240.0f;
    return this;
}


// ---- src/JetskiHeadingArrows/SetCompetition.cpp
class SearchNode;


class PSXCamera;
class JetskiVehicle;




void JetskiHeadingArrows::SetCompetition(JetskiCompetition *competition, PSXCamera **cameras, JetskiVehicle **vehicles)
{
    if (m_competition)
        return;
    m_competition = competition;
    if (m_competition) {
        for (int i = 0; i < 2; i++) {
            m_arrows[i]->AddSearchPlacement(*m_competition->m_scene->m_staticNodes, 2);
            m_cameras[i] = cameras[i];
            m_vehicles[i] = vehicles[i];
        }
    }
    if (!m_competition->m_x134)
        m_x58 = 120.0f;
    else
        m_x58 = 80.0f;
}


// ---- src/JetskiHeadingArrows/Tick.cpp
// Tick__19JetskiHeadingArrowsFf  @ 0x002AADC0
// Shows each player's arrow pointing back to the track while the vehicle is off it. Normalize is
// a VU0 asm helper, so the optimizer is off: ends[2] gives retail's constructor store order, and
// the named fov and the across/upward/scale order give its FP schedule and registers.



float FastTan(float x);

// libvu0 sceVu0Normalize.
static inline void Normalize(_D3DVECTOR *out, _D3DVECTOR *in)
{
    asm("lqc2 vf4, 0x0(%1); vmul.xyz vf5, vf4, vf4; vaddy.x vf5, vf5, vf5y; vaddz.x vf5, vf5, vf5z; vrsqrt Q, vf0w, vf5x; vsub.xyzw vf6, vf0, vf0; vaddw.xyzw vf6, vf6, vf4w; vwaitq; vmulq.xyz vf6, vf4, Q; sqc2 vf6, 0x0(%0)"
        : : "r"(out), "r"(in) : "memory");
}

int JetskiHeadingArrows::Tick(float dt)
{
    if (m_competition) {
        _D3DVECTOR direction, above, ends[2];
        for (int i = 0; i < 2; i++) {
            if (!m_cameras[i])
                continue;
            JetskiVehicle *vehicle = m_vehicles[i];
            if (!vehicle->m_ownStatus->m_onTrack && !vehicle->m_x530) {
                m_show[i] = 1;
                m_arrows[i]->EnableAll();
                above = vehicle->m_resetPosition;
                above.y += 2.5f;
                ends[1] = above;
                Camera *camera = m_cameras[i];
                _D3DVECTOR position = camera->m_position;
                _D3DVECTOR forward = _D3DVECTOR(camera->m_view._13, camera->m_view._23, camera->m_view._33);
                _D3DVECTOR up = _D3DVECTOR(camera->m_view._12, camera->m_view._22, camera->m_view._32);
                _D3DVECTOR right = _D3DVECTOR(camera->m_view._11, camera->m_view._21, camera->m_view._31);
                float side;
                if (m_competition->m_x134 == 2)
                    side = 0.34f;
                else
                    side = 0.5f;
                float fov = camera->m_fov;
                float tanHalfFov = FastTan(0.01745329f * (0.5f * fov));
                float height = 3.0f * tanHalfFov;
                float across = 2.0f * (side - 0.5f) * height;
                float upward = 0.8f * height * camera->m_pixelAspect;
                float scale = 3.0f * m_x58 / (camera->m_focalLength * m_scale);
                position += 3.0f * forward;
                position += across * right;
                position += upward * up;
                ends[0] = position;
                direction = ends[1] - ends[0];
                Normalize(&direction, &direction);
                direction.y = -0.5f;
                m_arrows[i]->SetPosition(position);
                m_arrows[i]->SetOrientation(&direction, (_D3DVECTOR *)&YVector, 1, 1, 1);
                m_arrows[i]->AddScale(scale, scale, scale);
                m_arrows[i]->UpdateSearchPlacement();
            } else {
                m_show[i] = 0;
                m_arrows[i]->DisableAll();
            }
        }
    }
    return GameObject::Tick(dt);
}


// ---- src/JetskiHeadingArrows/PrepareGeometry.cpp

class Camera;

extern RenderTarget *LpRenderTarget;


// Each arrow shows only in its own player's view.
int JetskiHeadingArrows::PrepareGeometry()
{
    if (m_competition && m_competition->m_x134) {
        if (LpRenderTarget->m_camera == m_cameras[1]) {
            if (m_show[0])
                m_arrows[0]->Unhide();
            if (m_show[1])
                m_arrows[1]->Hide();
        } else {
            if (m_show[1])
                m_arrows[1]->Unhide();
            if (m_show[0])
                m_arrows[0]->Hide();
        }
    }
    return 1;
}


// ---- src/JetskiCountDown/ctor.cpp
//
// __ct__10GameObjectFi(a0);
// *(a0+8) = D_004484D0;  // vtable
// *(a0+0x30) = 0;
// for (i = 0; i < 3; i++) { *(a0+0x34+i*4) = 0; *(a0+0x40+i*4) = 0; }
// *(a0+0x4C) = 0; *(a0+0x50) = 0;
// return a0;

JetskiCountDown::JetskiCountDown(int enabled) : GameObject(enabled)
{
    m_competition = 0;
    for (int i = 0; i < 3; i++) {
        m_sprites[0][i] = 0;
        m_sprites[1][i] = 0;
    }
    m_cameras[0] = 0;
    m_cameras[1] = 0;
}


// ---- src/JetskiCountDown/dtor.cpp

JetskiCountDown::~JetskiCountDown()
{
    for (int i = 0; i < 3; i++) {
        if (m_sprites[0][i])
            m_sprites[0][i]->Release();
        if (m_sprites[1][i])
            m_sprites[1][i]->Release();
    }
}


// ---- src/JetskiCountDown/Create.cpp
extern "C" int sprintf(char *buf, const char *fmt, ...);


static inline float Fmin(float a, float b)
{
    float r;
    asm("min.s $f8, %1, %2; swc1 $f8, 0(%0)" : : "r"(&r), "f"(a), "f"(b) : "$f8", "memory");
    return r;
}

class LightManager;






JetskiCountDown *JetskiCountDown::Create(LightManager *, ViewPortCode viewport)
{
    char name[0x20];
    int i;
    m_viewport = viewport;
    for (i = 0; i < 3; i++) {
        m_sprites[0][i] = new PSXRenderableObject(0);
        sprintf(name, "%d.slt", i + 1);
        m_sprites[0][i]->Create(name, 0, 0);
        m_sprites[0][i]->m_x190_bit4 = 1;
    }
    if (viewport) {
        for (i = 0; i < 3; i++) {
            m_sprites[1][i] = new PSXRenderableObject(0);
            sprintf(name, "%d.slt", i + 1);
            m_sprites[1][i]->Create(name, 0, 0);
            m_sprites[1][i]->m_x190_bit4 = 1;
        }
    }
    _D3DVECTOR min;
    _D3DVECTOR max;
    m_sprites[0][2]->GetHierarchyBoundingBox(&min, &max);
    float size = Fmin(max.x, max.y);
    size = Fmin(size, max.z);
    m_scale = 2.0f * size;
    if (!m_viewport)
        m_x58 = 60.0f;
    else
        m_x58 = 50.0f;
    return this;
}


// ---- src/JetskiCountDown/SetCompetition.cpp
class SearchNode;
class PSXCamera;





int JetskiCountDown::SetCompetition(JetskiCompetition *competition, PSXCamera **cameras)
{
    if (m_competition)
        return 1;
    m_competition = competition;
    if (m_competition) {
        for (int i = 0; i < 3; i++) {
            m_sprites[0][i]->AddSearchPlacement(m_competition->m_scene->m_staticNodes[0], 2);
            if (m_viewport)
                m_sprites[1][i]->AddSearchPlacement(m_competition->m_scene->m_staticNodes[0], 2);
        }
        for (int i = 0; i < 2; i++)
            m_cameras[i] = cameras[i];
    }
    return 1;
}


// ---- src/JetskiCountDown/Tick.cpp
// Tick__15JetskiCountDownFf  @ 0x002AB800
// Places the 3-2-1 countdown sprites in front of each player's camera. Normalize is a VU0 asm
// helper, so the optimizer is off: the named fov/side/h locals and the single `t` reproduce
// retail's FP registers and schedule, and the redundant `else digit = 2;` is the block-optimizer
// event that gives retail's delay-slot fills.


float FastTan(float x);

// libvu0 sceVu0Normalize.

// 2: count the race start down; 1: count the competition's own timer down; 0: none.
static inline int CountDownMode(JetskiCompetition *competition)
{
    if (competition->m_raceStatus->m_flags & 1)
        return 2;
    return competition->m_status == 3 ? 1 : 0;
}

int JetskiCountDown::Tick(float dt)
{
    if (m_competition) {
        _D3DVECTOR look, target, ends[2];
        for (int i = 0; i < 2; i++) {
            if (!m_cameras[i])
                continue;
            int mode = CountDownMode(m_competition);
            if (mode) {
                int j;
                int digit = 2;
                float scale;
                float t;
                if (mode == 2) {
                    scale = 1.0f;
                    RaceStatusStruct_Tag *status = m_competition->m_vehicles[i + 5]->m_ownStatus;
                    if (!status)
                        continue;
                    t = status->m_x6E0;
                    if (t <= 0.0f)
                        continue;
                    if (!(t <= 2.0f))
                        digit = 0;
                    else if (!(t <= 1.0f))
                        digit = 1;
                    else
                        digit = 2; // redundant (digit starts at 2), kept for retail's codegen
                } else {
                    t = m_competition->m_x138;
                    if (!(t <= 3.0f)) {
                        scale = 0.0f;
                    } else if (!(t <= 2.0f)) {
                        scale = 1.5f - (t - 2.0f) / 2.0f;
                    } else if (!(t <= 1.0f)) {
                        digit = 1;
                        scale = 1.5f - (t - 1.0f) / 2.0f;
                    } else {
                        digit = 0;
                        scale = 1.5f - t / 2.0f;
                    }
                }
                for (j = 0; j < 3; j++) {
                    if (digit == j)
                        m_sprites[i][j]->EnableAll();
                    else
                        m_sprites[i][j]->DisableAll();
                }
                Camera *camera = m_cameras[i];
                _D3DVECTOR position = camera->m_position;
                _D3DVECTOR forward = _D3DVECTOR(camera->m_view._13, camera->m_view._23, camera->m_view._33);
                _D3DVECTOR up = _D3DVECTOR(camera->m_view._12, camera->m_view._22, camera->m_view._32);
                _D3DVECTOR right = _D3DVECTOR(camera->m_view._11, camera->m_view._21, camera->m_view._31);
                float fov = camera->m_fov;
                float tan = FastTan(0.01745329f * (0.5f * fov));
                float side = 0.0f;
                float h = 3.0f * tan;
                float height = 0.5f * h * camera->m_pixelAspect;
                scale *= 3.0f * m_x58 / (camera->m_focalLength * m_scale);
                ends[0] = position;
                position += 3.0f * forward;
                position += side * right;
                position += height * up;
                ends[1] = position;
                look = ends[0] - ends[1];
                Normalize(&look, &look);
                m_sprites[i][digit]->SetPosition(position);
                m_sprites[i][digit]->SetOrientation(&look, (_D3DVECTOR *)&YVector, 1, 1, 1);
                m_sprites[i][digit]->AddScale(scale, scale, scale);
                m_sprites[i][digit]->UpdateSearchPlacement();
            } else {
                m_sprites[i][0]->DisableAll();
                m_sprites[i][1]->DisableAll();
                m_sprites[i][2]->DisableAll();
            }
        }
    }
    return GameObject::Tick(dt);
}


// ---- src/JetskiCountDown/Hide.cpp

int JetskiCountDown::Hide()
{
    for (int i = 0; i < 2; i++) {
        if (m_cameras[i]) {
            m_sprites[i][0]->DisableAll();
            m_sprites[i][1]->DisableAll();
            m_sprites[i][2]->DisableAll();
        }
    }
    return 1;
}


// ---- src/JetskiCountDown/PrepareGeometry.cpp
extern RenderTarget *LpRenderTarget;

int JetskiCountDown::PrepareGeometry()
{
    if (m_viewport) {
        if (LpRenderTarget->m_camera == m_cameras[1]) {
            m_sprites[1][0]->DisableAll();
            m_sprites[1][1]->DisableAll();
            m_sprites[1][2]->DisableAll();
        } else {
            m_sprites[0][0]->DisableAll();
            m_sprites[0][1]->DisableAll();
            m_sprites[0][2]->DisableAll();
        }
    }
    return 1;
}


// ---- src/SplitScreenSeparator/ctor.cpp
// (Same as __ct__19JetskiHeadingArrowsFi, different init/vtable.)
//
// __ct__12ScreenObjectFi(a0);
// *(a0+8) = D_00448450;  // vtable
// *(a0+0x78) = 0; *(a0+0x58) = 0; *(a0+0x5C) = 0; *(a0+0x60) = 0;
// *(a0+0x64) = 0;
// return a0;

SplitScreenSeparator::SplitScreenSeparator(int enabled) : ScreenObject(enabled)
{
    m_orientation = 0;
    m_primitives[0] = 0;
    m_primitives[1] = 0;
    m_primitives[2] = 0;
    m_primitives[3] = 0;
}


// ---- src/SplitScreenSeparator/dtor.cpp

SplitScreenSeparator::~SplitScreenSeparator()
{
    for (int i = 0; i < 4; i++) {
        if (m_primitives[i]) {
            if (m_vertices[i])
                delete[] m_vertices[i];
            delete m_primitives[i];
        }
    }
}


// ---- src/ScreenPrimitiveVertex/dtor.cpp

ScreenPrimitiveVertex::~ScreenPrimitiveVertex()
{
}


// ---- src/SplitScreenSeparator/Create.cpp
#pragma inline_depth(4)






SplitScreenSeparator *SplitScreenSeparator::Create(ViewPortCode orientation)
{
    ScreenObject::Create(50, 50, 50);
    m_orientation = orientation;
    for (int i = 0; i < 4; i++) {
        m_vertices[i] = new ScreenPrimitiveVertex[4];
        m_primitives[i] = (new ScreenPrimitive)->Create(m_vertices[i], 4, (ScreenPrimitiveType)1, 0);
        for (int j = 0; j < 4; j++) {
            m_primitives[i]->m_vertices[j].SetColor(0);
            m_primitives[i]->m_vertices[j].SetAlpha(0xFF);
        }
        m_primitives[i]->m_alphaTest = 0;
        m_primitives[i]->m_dirty = 1;
    }
    if (m_orientation == 1) {
        m_viewportRects[0].left = 0;
        m_viewportRects[0].right = 640;
        m_viewportRects[0].top = 0;
        m_viewportRects[0].bottom = 224;
        m_viewportRects[1].left = 0;
        m_viewportRects[1].right = 640;
        m_viewportRects[1].top = 224;
        m_viewportRects[1].bottom = 448;
    } else {
        m_viewportRects[0].top = 0;
        m_viewportRects[0].bottom = 448;
        m_viewportRects[0].left = 0;
        m_viewportRects[0].right = 320;
        m_viewportRects[1].top = 0;
        m_viewportRects[1].bottom = 448;
        m_viewportRects[1].left = 320;
        m_viewportRects[1].right = 640;
    }
    return this;
}


// ---- src/ScreenPrimitiveVertex/ctor.cpp

ScreenPrimitiveVertex::ScreenPrimitiveVertex()
{
    m_position.x = 0.0f;
    m_position.y = 0.0f;
    m_position.z = 1.0f;
    m_position.w = 1.0f;
    m_alpha = 0xFF;
    m_color = -1;
    m_x18 = m_x1C = 0;
    m_vifPosition = 0;
    m_vifColor = 0;
    m_x28 = 0;
}


// ---- src/SplitScreenSeparator/SetupRects.cpp






// One-pixel bars along the four edges of rect.
void SplitScreenSeparator::SetupRects(_RECT rect)
{
    float left = rect.left;
    float right = rect.right;
    float top = rect.top;
    float bottom = rect.bottom;

    m_primitives[0]->m_vertices[0].SetPosition(left, bottom - 1.0f);
    m_primitives[0]->m_vertices[1].SetPosition(right, bottom - 1.0f);
    m_primitives[0]->m_vertices[2].SetPosition(left, bottom);
    m_primitives[0]->m_vertices[3].SetPosition(right, bottom);
    m_primitives[0]->m_dirty = 1;

    m_primitives[1]->m_vertices[0].SetPosition(left, top);
    m_primitives[1]->m_vertices[1].SetPosition(right, top);
    m_primitives[1]->m_vertices[2].SetPosition(left, 1.0f + top);
    m_primitives[1]->m_vertices[3].SetPosition(right, 1.0f + top);
    m_primitives[1]->m_dirty = 1;

    m_primitives[2]->m_vertices[0].SetPosition(right - 1.0f, top);
    m_primitives[2]->m_vertices[1].SetPosition(right, top);
    m_primitives[2]->m_vertices[2].SetPosition(right - 1.0f, bottom);
    m_primitives[2]->m_vertices[3].SetPosition(right, bottom);
    m_primitives[2]->m_dirty = 1;

    m_primitives[3]->m_vertices[0].SetPosition(left, top);
    m_primitives[3]->m_vertices[1].SetPosition(1.0f + left, top);
    m_primitives[3]->m_vertices[2].SetPosition(left, bottom);
    m_primitives[3]->m_vertices[3].SetPosition(1.0f + left, bottom);
    m_primitives[3]->m_dirty = 1;
}


// ---- src/SplitScreenSeparator/PrepareGeometry.cpp

int SplitScreenSeparator::PrepareGeometry()
{
    return 1;
}


// ---- src/SplitScreenSeparator/Render3D.cpp


int SplitScreenSeparator::Render3D()
{
    int viewport = LpGame->m_currentPlayer;
    SetupRects(m_viewportRects[viewport]);
    if (m_orientation == 1) {
        if (viewport == 0)
            m_primitiveManager->AddPrimitive(m_primitives[0]);
        else
            m_primitiveManager->AddPrimitive(m_primitives[1]);
    } else {
        if (viewport == 0)
            m_primitiveManager->AddPrimitive(m_primitives[2]);
        else
            m_primitiveManager->AddPrimitive(m_primitives[3]);
    }
    ScreenObject::Render3D();
}


// ---- src/JetskiRadar/ctor.cpp
//
// __ct__10GameObjectFi(a0);
// *(a0+8) = D_004483D0;
// *(a0+0x30) = 0; *(a0+0xB8) = 0; *(a0+0x108) = 0;
// *(a0+0xBC) = 1.0f; *(a0+0xC0) = 0; *(a0+0xC4) = 0; *(a0+0xC8) = 0;
// for (i = 0; i < 5; i++) {
//     *(a0+4i+0x34) = 0; *(a0+4i+0x48) = 1; *(a0+4i+0x5C) = 0;
//     *(a0+4i+0x7C) = 0; *(a0+4i+0x90) = 0;
// }
// *(a0+0xCC) = 0; *(a0+0x70) = 0; *(a0+0x74) = 0; *(a0+0x78) = 0;
// *(a0+0xD0) = 0; *(a0+0xD4) = 0;

JetskiRadar::JetskiRadar(int enabled) : GameObject(enabled)
{
    m_competition = 0;
    m_numVehicles = 0;
    m_showing = 0;
    m_scale = 1.0f;
    m_centerX = 0;
    m_centerZ = 0;
    m_vOffset = 0;
    for (int i = 0; i < 5; i++) {
        m_vehicles[i] = 0;
        m_blink[i] = 1;
        m_blinkTimer[i] = 0;
        m_vertices[i] = 0;
        m_blips[i] = 0;
    }
    m_numViewports = 0;
    m_iconManager = 0;
    m_map = 0;
    m_primitiveManager = 0;
    m_radarViewportItem = 0;
    m_restoreViewportItem = 0;
}


// ---- src/JetskiRadar/dtor.cpp





JetskiRadar::~JetskiRadar()
{
    if (m_map)
        delete m_map;
    if (m_radarViewportItem)
        delete m_radarViewportItem;
    if (m_restoreViewportItem)
        delete m_restoreViewportItem;
    for (int i = 0; i < m_numVehicles; i++) {
        if (m_blips[i])
            delete m_blips[i];
        if (m_vertices[i])
            delete[] m_vertices[i];
    }
    // With split screen the radar owns its own icon and text managers.
    if (m_numViewports > 1) {
        if (m_iconManager)
            delete m_iconManager;
        if (m_primitiveManager)
            delete m_primitiveManager;
    }
}


// ---- src/JetskiRadar/Create.cpp
extern "C" int sprintf(char *buf, const char *fmt, ...);
void PSXprintf(const char *fmt, ...);

extern char SceneContentDirectory[];

class ScreenTextManager;






extern JetskiGame *LpGame;



JetskiRadar *JetskiRadar::Create(ScreenTextManager *textManager, ViewPortCode viewport, JetskiCompetition *competition)
{
    char name[0x110];
    m_numViewports = 1;
    if (viewport)
        m_numViewports = 2;
    if (m_numViewports == 1) {
        m_left = 20;
        m_top = 300;
        m_right = m_left + 128;
        m_bottom = m_top + 128;
    } else if (viewport == 1) {
        m_left = 80;
        m_top = 160;
        m_right = m_left + 128;
        m_bottom = m_top + 128;
    } else {
        m_left = 256;
        m_top = 160;
        m_right = m_left + 128;
        m_bottom = m_top + 128;
    }
    m_radarViewportItem = new DmaRenderItem;
    m_restoreViewportItem = new DmaRenderItem;
    if (LpGame->m_gameOptions->m_difficulty == 3)
        sprintf(name, "%s%sHR.tga", SceneContentDirectory, competition->m_sceneFolder);
    else
        sprintf(name, "%s%sER.tga", SceneContentDirectory, competition->m_sceneFolder);
    m_map = (new ScreenIcon)->Create(name, 0.0f, 0.0f);
    if (!m_map)
        PSXprintf("icon %s for JetskiRadar could not be loaded!\n", name);
    if (m_numViewports > 1) {
        m_primitiveManager = (new ScreenPrimitiveManager(1))->Create(10);
        m_iconManager = (new ScreenIconManager(1))->Create(5);
    }
    return this;
}


// ---- src/JetskiRadar/SetCompetition.cpp



class JetskiVehicle;
class ScreenIconManager;
class ScreenPrimitiveManager;
class DmaRenderItem;

// The radar's view of the vertex colour setter: the colour goes to the VIF
// data as one four-word block.
struct VifColor {
    int r, g, b, a;
};

static inline void RadarUpdateColor(ScreenPrimitiveVertex *v)
{
    VifColor c;
    c.r = ((v->m_color >> 16) & 0xFF) >> 1;
    c.g = ((v->m_color >> 8) & 0xFF) >> 1;
    c.b = (v->m_color & 0xFF) >> 1;
    c.a = v->m_alpha >> 2;
    if (v->m_vifColor)
        *(VifColor *)v->m_vifColor = c;
}
static inline void RadarSetColor(ScreenPrimitiveVertex *v, unsigned int color)
{
    v->m_color = color;
    RadarUpdateColor(v);
}
static inline void RadarSetAlpha(ScreenPrimitiveVertex *v, unsigned char alpha)
{
    v->m_alpha = alpha;
    RadarUpdateColor(v);
}




// The scene's track extents, in 256-unit map cells.




extern JetskiGame *LpGame;

static inline float Fmax(float a, float b)
{
    float r;
    asm("max.s $f8, %1, %2; swc1 $f8, 0(%0)" : : "r"(&r), "f"(a), "f"(b) : "$f8", "memory");
    return r;
}


// Sizes the radar map to the scene's track extents (with per-track zoom and
// offset tweaks) and builds one coloured blip per vehicle. The Fmax asm turns
// the global optimizer off; the declaration order reproduces retail's
// registers.
int JetskiRadar::SetCompetition(JetskiCompetition *competition)
{
    float rangeZ;
    float sizeZ;
    float zoom;
    SceneManagerData *data;
    float originX;
    int width;
    float sizeX;
    int height;
    float cellX;
    float originZ;
    float rangeX;
    float offsetZ;
    float cellZ;
    float scaleX;
    float scaleZ;
    int i;
    if (m_competition)
        return 1;
    m_competition = competition;
    if (m_numViewports == 1) {
        m_primitiveManager = (ScreenPrimitiveManager *)m_competition->m_x34;
        m_iconManager = m_competition->m_iconManager;
    }
    width = ((TextureMap *)m_map->m_texture)->m_width;
    height = ((TextureMap *)m_map->m_texture)->m_height;
    m_map->SetPosition(m_left, m_top);
    m_map->SetWidth(width);
    m_map->SetHeight(height);

    zoom = 1.0f;
    data = m_competition->m_scene->m_manager;
    originX = data->m_terrainWidthScale;
    originZ = data->m_terrainBreadthScale;
    sizeX = data->m_terrainWidth;
    sizeZ = data->m_terrainBreadth;
    cellX = 256.0f * originX;
    cellZ = 256.0f * originZ;
    if (sizeX > 2.0f) {
        rangeX = sizeX - 2.0f;
        sizeX = sizeX - 1.0f;
    } else {
        rangeX = sizeX;
    }
    offsetZ = 1.0f;
    if (sizeZ > 1.0f) {
        if (sizeZ > 2.0f) {
            rangeZ = 1.0f;
        } else {
            offsetZ = 0.0f;
            rangeZ = sizeZ;
        }
    } else {
        rangeZ = 1.0f;
        offsetZ = sizeZ - 1.0f;
    }

    m_numVehicles = m_competition->m_numVehicles;
    for (i = 0; i < m_numVehicles; i++)
        m_vehicles[i] = m_competition->m_vehicles[i];
    if (LpGame->m_gameOptions->m_raceType == 3)
        m_numVehicles++;

    m_centerX = sizeX * cellX;
    m_centerZ = offsetZ * cellZ;
    m_vOffset = 0.0f;
    scaleX = cellX * rangeX / (float)width;
    scaleZ = cellZ * rangeZ / (float)height;
    switch (LpGame->m_gameOptions->m_x2C + 1) {
    case 1:
        zoom = 128.0f / 113.0f;
        zoom = zoom / 3.0f;
        m_centerX = m_centerX - cellX;
        m_centerZ = m_centerZ + cellZ;
        break;
    case 7:
        m_vOffset = 6.0f;
        break;
    case 11:
        zoom = 128.0f / 118.0f;
        break;
    case 14:
        m_centerX = m_centerX + cellX;
        break;
    case 10:
        m_centerX = m_centerX + 0.0625f * cellX;
        break;
    case 4:
    case 12:
        zoom = 0.5f;
        m_centerX = m_centerX - 0.703125f * cellX;
        m_centerZ = m_centerZ + 0.296875f * cellX;
        break;
    case 8:
    case 16:
        zoom = 0.5f;
        m_centerX = m_centerX - 0.8828125f * cellX;
        m_centerZ = m_centerZ + 0.1171875f * cellX;
        break;
    case 13:
        m_centerX = m_centerX + cellX;
        scaleZ *= 2.0f;
        m_vOffset = 7.0f;
        break;
    case 3:
        zoom = 64.0f / 117.0f;
        m_centerX = m_centerX - 0.5f * cellX;
        break;
    case 18:
        m_vOffset = 35.0f;
        break;
    }
    m_scale = zoom * Fmax(scaleX, scaleZ);

    for (i = 0; i < m_numVehicles;) {
        m_vertices[i] = new ScreenPrimitiveVertex[4];
        m_blips[i] = (new ScreenPrimitive)->Create(m_vertices[i], 4, (ScreenPrimitiveType)1, 0);
        for (int j = 0; j < 4; j++)
            RadarSetAlpha(&m_blips[i]->m_vertices[j], 0xFF);
        if (m_numViewports == 1) {
            if (i == 0) {
                RadarSetColor(&m_blips[i]->m_vertices[0], 0x19F52D);
                RadarSetColor(&m_blips[i]->m_vertices[1], 0x19F52D);
                RadarSetColor(&m_blips[i]->m_vertices[2], 0x19F52D);
                RadarSetColor(&m_blips[i]->m_vertices[3], 0x19F52D);
            } else {
                RadarSetColor(&m_blips[i]->m_vertices[0], 0xFF2900);
                RadarSetColor(&m_blips[i]->m_vertices[1], 0xFF2900);
                RadarSetColor(&m_blips[i]->m_vertices[2], 0xFF2900);
                RadarSetColor(&m_blips[i]->m_vertices[3], 0xFF2900);
            }
        } else {
            switch (i) {
            case 0:
                RadarSetColor(&m_blips[i]->m_vertices[0], 0x19F52D);
                RadarSetColor(&m_blips[i]->m_vertices[1], 0x19F52D);
                RadarSetColor(&m_blips[i]->m_vertices[2], 0x19F52D);
                RadarSetColor(&m_blips[i]->m_vertices[3], 0x19F52D);
                break;
            case 1:
                RadarSetColor(&m_blips[i]->m_vertices[0], 0x5932F2);
                RadarSetColor(&m_blips[i]->m_vertices[1], 0x5932F2);
                RadarSetColor(&m_blips[i]->m_vertices[2], 0x5932F2);
                RadarSetColor(&m_blips[i]->m_vertices[3], 0x5932F2);
                break;
            default:
                RadarSetColor(&m_blips[i]->m_vertices[0], 0xFF2900);
                RadarSetColor(&m_blips[i]->m_vertices[1], 0xFF2900);
                RadarSetColor(&m_blips[i]->m_vertices[2], 0xFF2900);
                RadarSetColor(&m_blips[i]->m_vertices[3], 0xFF2900);
                break;
            }
        }
        ScreenPrimitive *blip = m_blips[i++];
        blip->m_dirty = 1;
    }
    return 1;
}


// ---- src/JetskiRadar/SetShowing.cpp

void JetskiRadar::SetShowing(int showing)
{
    m_showing = showing;
}


// ---- src/JetskiRadar/PrepareGeometry.cpp

int JetskiRadar::PrepareGeometry()
{
    TextureMap *texture = m_map->m_texture;
    if (10.0f > texture->m_usage)
        texture->m_usage = 10.0f;
    return 1;
}


// ---- src/JetskiRadar/Tick.cpp


extern JetskiGame *LpGame;




int JetskiRadar::Tick(float dt)
{
    int count;
    if (LpGame->m_gameOptions->m_raceType == 3)
        count = m_numVehicles - 1;
    else if (LpGame->m_gameOptions->m_raceType == 6 && ((JetskiCareerCompetition *)m_competition)->m_raceType == 1)
        count = 1;
    else
        count = m_numVehicles;
    for (int i = 0; i < count; i++) {
        if (m_vehicles[i] && m_vehicles[i]->m_ownStatus->m_onTrack == 1) {
            m_blink[i] = 1;
            m_blinkTimer[i] = 0.0f;
        } else if (m_blinkTimer[i] > 0.0f) {
            m_blinkTimer[i] -= dt;
        } else {
            m_blink[i] = !m_blink[i];
            m_blinkTimer[i] = 0.25f;
        }
    }
    return 1;
}


// ---- src/JetskiRadar/SetViewport.cpp








// Switch to a full-screen (640x448) viewport for the radar, remembering the
// camera's viewport for RestoreViewport().
void JetskiRadar::SetViewport()
{
    m_radarViewport = LpRenderTarget->m_camera->m_viewport;
    m_savedViewport = m_radarViewport;
    m_radarViewport.X = 0;
    m_radarViewport.Y = 0;
    m_radarViewport.Width = 640;
    m_radarViewport.Height = 448;
    m_radarViewportItem->Start((DmaRenderDestination)1, 0x80);
    ((PSXRenderTarget *)LpRenderTarget)->SetViewport(&m_radarViewport, m_radarViewportItem, &m_savedViewport);
    m_radarViewportItem->End();
    ((PSXRenderTarget *)LpRenderTarget)->AddToRenderQueue(m_radarViewportItem, "");
}


// ---- src/JetskiRadar/RestoreViewport.cpp



void JetskiRadar::RestoreViewport()
{
    m_restoreViewportItem->Start((DmaRenderDestination)1, 0x80);
    ((PSXRenderTarget *)LpRenderTarget)->SetViewport(&m_savedViewport, m_restoreViewportItem, &m_radarViewport);
    m_restoreViewportItem->End();
    ((PSXRenderTarget *)LpRenderTarget)->AddToRenderQueue(m_restoreViewportItem, "");
}


// ---- src/JetskiRadar/Render3D.cpp


int JetskiRadar::Render3D()
{
    if (LpGame->m_viewport == LpGame->m_numCameras - 1) {
        if (!m_competition)
            return GameObject::Render3D();
        if (m_showing) {
            if (m_numViewports > 1)
                SetViewport();
            m_iconManager->AddIcon(m_map, 0);
            RenderRadar();
            if (m_numViewports > 1) {
                m_iconManager->Render3D();
                m_primitiveManager->Render3D();
                RestoreViewport();
            }
        }
    }
    return 1;
}


// ---- src/JetskiRadar/RenderRadar.cpp






extern JetskiGame *LpGame;

struct RiderModel {
    char pad0[0x60];
    _D3DVECTOR m_position;              // +0x60
};

struct RiderParts {
#ifdef SPLASHDOWN_PC
    char pad0[sizeof(RaceStatusVehicleInterface)];  // x86-64: JetskiGhostVehicle's m_object
#else
    char pad0[0x10];
#endif
    SoultreeObject *m_body;             // +0x10
    char pad14[0x80 - 0x14];
    struct { char pad[0x288]; SoultreeObject *m_object; } *m_x80;  // +0x80
};

struct RaceInfo {
    char pad0[0xD4];
    int m_xD4;                          // +0xD4
};



void JetskiRadar::RenderRadar()
{
    int count;
    if (LpGame->m_gameOptions->m_raceType == 6 && ((JetskiCareerCompetition *)m_competition)->m_raceType == 1)
        count = 1;
    else
        count = m_numVehicles;
    for (int i = count - 1; i >= 0; i--) {
        if (!m_blink[i])
            continue;
        float u, v;
        if (LpGame->m_gameOptions->m_raceType == 3 && i == m_numVehicles - 1) {
            if (!m_competition->m_vcr->m_loaded)
                continue;
            _D3DVECTOR pos;
            if (LpGame->m_gameOptions->m_x54 == 2)
                ((RiderParts *)m_competition->m_ghost)->m_x80->m_object->GetPosition(&pos);
            else
                ((RiderParts *)m_competition->m_ghost)->m_body->GetPosition(&pos);
            CalculateTextureCoordinates(pos, u, v);
        } else {
            CalculateTextureCoordinates(m_vehicles[i]->m_body->m_x60, u, v);
        }
        u += m_left;
        v += m_top;
        m_blips[i]->m_vertices[0].SetPosition(u - 3.0f, v - 3.0f);
        m_blips[i]->m_vertices[1].SetPosition(u + 3.0f, v - 3.0f);
        m_blips[i]->m_vertices[2].SetPosition(u - 3.0f, v + 3.0f);
        m_blips[i]->m_vertices[3].SetPosition(u + 3.0f, v + 3.0f);
        m_primitiveManager->AddPrimitive(m_blips[i]);
    }
}


// ---- src/JetskiRadar/CalculateTextureCoordinates.cpp

static inline float Fmin2(float a, float b)
{
    float r;
    asm("min.s $f8, %1, %2; swc1 $f8, 0(%0)" : : "r"(&r), "f"(a), "f"(b) : "$f8", "memory");
    return r;
}


// Radar texture coordinates of a world position; clamps to the radar's edge
// and returns 0 when the position is off the radar.
int JetskiRadar::CalculateTextureCoordinates(_D3DVECTOR position, float &u, float &v)
{
    u = 128.0f - (m_centerX - position.x) / m_scale;
    v = 128.0f - (position.z - m_centerZ) / m_scale - m_vOffset;
    if (u > 125.0f || u < 2.0f || v > 125.0f || v < 2.0f) {
        u = Fmax(Fmin2(125.0f, u), 2.0f);
        v = Fmax(Fmin2(125.0f, v), 2.0f);
        return 0;
    }
    return 1;
}


// ---- src/JetskiInfoText/ctor.cpp

JetskiInfoText::JetskiInfoText(int enabled) : GameObject(enabled)
{
    m_textManager = 0;
    m_font = 0;
    for (int i = 0; i < 2; i++) {
        m_texts[i] = 0;
        m_x44[i] = 0;
        m_x4C[i] = 0;
    }
    m_messages[0] = m_messages[1] = 0;
    m_current[0] = m_current[1] = 0;
    m_numQueued[0] = m_numQueued[1] = 0;
    m_viewport = 0;
}


// ---- src/JetskiInfoText/dtor.cpp


JetskiInfoText::~JetskiInfoText()
{
    int i;
    if (m_texts[0])
        delete m_texts[0];
    if (m_texts[1])
        delete m_texts[1];
    m_current[0] = m_messages[0];
    m_current[1] = m_messages[1];
    for (i = 0; i < 2; i++) {
        while (m_current[i]) {
            InfoMessage *entry = m_current[i];
            m_current[i] = entry->m_next;
            delete entry;
            // stop when the ring wraps back to the head
            if (m_current[i] && m_current[i]->m_next == m_messages[i])
                m_current[i]->m_next = 0;
        }
    }
    m_current[1] = 0;
    m_current[0] = 0;
    m_messages[1] = 0;
    m_messages[0] = 0;
}


// ---- src/JetskiInfoText/Create.cpp

extern "C" char *strncpy(char *dst, const char *src, unsigned int n);

class ScreenTextFont;


class ScreenTextManager;







JetskiInfoText *JetskiInfoText::Create(ScreenTextManager *textManager, ViewPortCode viewport)
{
    int i;
    int j;

    m_viewport = viewport;
    ScreenTextFontItem *item = LpGame->m_fonts->GetScreenTextFont("info_b");
    m_font = item ? item->m_font : 0;
    m_textManager = textManager;
    if (m_viewport == 0) {
        m_texts[0] = new ScreenText(0x3C, 0);
        m_texts[0]->SetString("");
        m_texts[0]->SetColor(0xFFFFDE00);
        m_texts[0]->SetPosition(320.0f, 400.0f);
    } else {
        for (i = 0; i < 2; i++) {
            m_texts[i] = new ScreenText(0x3C, 0);
            m_texts[i]->SetString("");
            m_texts[i]->SetColor(0xFFFFDE00);
            if (m_viewport == 1)
                m_texts[i]->SetPosition(320.0f, 185.0f);
            else
                m_texts[i]->SetPosition(160.0f, 400.0f);
        }
    }
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 3; j++) {
            if (j == 0) {
                if (!(m_messages[i] = new InfoMessage))
                    return 0;
                m_current[i] = m_messages[i];
                m_current[i]->m_text[0] = 0;
                m_current[i]->m_duration = 0.0f;
                m_current[i]->m_time = 0.0f;
            } else {
                if (!(m_current[i]->m_next = new InfoMessage))
                    return 0;
                m_current[i] = m_current[i]->m_next;
                m_current[i]->m_text[0] = 0;
                m_current[i]->m_duration = 0.0f;
                m_current[i]->m_time = 0.0f;
            }
        }
        m_current[i]->m_next = m_messages[i];
    }
    return this;
}


// ---- src/JetskiInfoText/PrepareGeometry.cpp

int JetskiInfoText::PrepareGeometry()
{
    if (m_font && (m_x44[0] || m_x44[1])) {
        TextureMap *texture = m_font->m_texture;
        if (10.0f > texture->m_usage)
            texture->m_usage = 10.0f;
    }
    return GameObject::PrepareGeometry();
}


// ---- src/JetskiInfoText/Render3D.cpp




static inline void ShowText(JetskiInfoText *info, int player)
{
    info->m_textManager->PositionText(info->m_texts[player], ScreenTextManager::CenterHorizontalJustification,
                                      info->m_font);
    ScreenText *text = info->m_texts[player];
    text->m_scale = info->m_x6C;
    text->m_dirty = 1;
    text->m_positionDirty = 1;
    info->m_textManager->AddText(info->m_texts[player], info->m_font);
}

int JetskiInfoText::Render3D()
{
    if (m_viewport) {
        for (int i = 0; i < 2; i++) {
            if (m_numQueued[i] && i == LpGame->m_currentPlayer) {
                ShowText(this, i);
            }
        }
    } else if (m_numQueued[0]) {
        ShowText(this, 0);
    }
    return GameObject::Render3D();
}


// ---- src/JetskiInfoText/Tick.cpp




int JetskiInfoText::Tick(float dt)
{
    int i;
    if (m_viewport) {
        for (i = 0; i < 2; i++) {
            if (m_numQueued[i]) {
                m_current[i]->m_time += dt;
                bool expired = m_current[i]->m_time >= m_current[i]->m_duration;
                if (expired)
                    NextMessage(i);
            }
        }
    } else {
        for (i = 0; i < 1; i++) {
            if (m_numQueued[i]) {
                m_current[i]->m_time += dt;
                bool expired = m_current[i]->m_time >= m_current[i]->m_duration;
                if (expired)
                    NextMessage(i);
            }
        }
    }
    return GameObject::Tick(dt);
}


// ---- src/JetskiInfoText/AddInfoText.cpp







void JetskiInfoText::AddInfoText(char *text, int player, float duration, int flush, float x6C)
{
    InfoMessage *msg;

    m_x6C = x6C;
    if (player >= 2) {
#line 4182 "JetskiInGameOverlay.cpp"
        ASSERT(0, "INVALID INFOTEXT CAMERA NUMBER");
        return;
    }
    if (LpGame->m_gameOptions->m_raceType == 5)
        player = 0;
    if (flush)
        FlushQueue(player);
    msg = m_current[player]->m_next;
    if (duration <= 0.0f)
        return;
    while (!(msg->m_duration <= 0.0f)) {
        if (msg == m_current[player])
            break;
        msg = msg->m_next;
    }
#line 4201 "JetskiInGameOverlay.cpp"
    STRCPY(msg->m_text, text);
    msg->m_duration = duration;
    msg->m_time = 0.0f;
    m_numQueued[player]++;
    if (msg == m_current[player]) {
        m_texts[player]->SetString(m_current[player]->m_text);
        m_texts[player]->SetScale(1.25f);
    }
    if (m_numQueued[player] > 3) {
        m_current[player] = m_current[player]->m_next;
        m_numQueued[player] = 3;
    }
    if (m_numQueued[player] > 1 && !(m_current[player]->m_duration <= 1.0f))
        m_current[player]->m_duration = 1.0f;
}


// ---- src/JetskiInfoText/NextMessage.cpp

extern "C" char *strncpy(char *dst, const char *src, unsigned int n);





void JetskiInfoText::NextMessage(int player)
{
    if (m_current[player]->m_duration > 0.0f)
        m_numQueued[player]--;
    m_current[player]->m_text[0] = 0;
    m_current[player]->m_duration = 0.0f;
    m_current[player]->m_time = 0.0f;
    m_current[player] = m_current[player]->m_next;
    if (m_numQueued[player]) {
        while (m_current[player]->m_duration == 0.0f)
            m_current[player] = m_current[player]->m_next;
        m_texts[player]->SetString(m_current[player]->m_text);
    }
    // With more messages waiting, show this one for at most a second.
    if (m_numQueued[player] > 1 && m_current[player]->m_duration > 1.0f)
        m_current[player]->m_duration = 1.0f;
}


// ---- src/JetskiInfoText/FlushQueue.cpp


void JetskiInfoText::FlushQueue(int player)
{
    if (player >= 2) {
#line 4258 "JetskiInGameOverlay.cpp"
        ASSERT(0, "INVALID INFOTEXT QUEUE NUMBER");
        return;
    }
    InfoMessage *msg = m_messages[player];
    for (int i = 0; i < 3; i++) {
        msg->m_text[0] = 0;
        msg->m_duration = 0.0f;
        msg->m_time = 0.0f;
        msg = msg->m_next;
    }
    m_numQueued[player] = 0;
}


// ---- src/global/GetVolumeAndPan.cpp
#pragma fast_fptosi on


float FastAtan2(float y, float x);
float FastCos(float x);
float FastSin(float x);

static inline float RSqrt(float x)
{
    float r;
    asm("rsqrt.s $f8, %0, %1; swc1 $f8, 0(%2)" : : "f"(1.0f), "f"(x), "r"(&r) : "$f8", "memory");
    return r;
}

static inline float Length(const _D3DVECTOR &v)
{
    float len2 = v.x * v.x + v.y * v.y + v.z * v.z;
    if (len2 == 0.0f)
        return 0.0f;
    return 1.0f / RSqrt(len2);
}

// Volume (0..100, by distance), pan (angle between the listener's facing
// and the sound, in degrees) and Doppler term for a sound.
int GetVolumeAndPan(_D3DVECTOR &front, _D3DVECTOR &relative, float distance, float minDistance,
                    float maxDistance, _D3DVECTOR velocity, _D3DVECTOR sourceVelocity,
                    float *volume, long *pan, int *doppler)
{
    *volume = -2500.0f;
    *pan = 0;
    *doppler = 0;
    if (!(distance < maxDistance))
        return 0;
    if (distance <= minDistance)
        *volume = 100.0f;
    else
        *volume = 100.0f - 100.0f * ((distance - minDistance) / (maxDistance - minDistance));

    int facing = 57.29578f * FastAtan2(front.z, front.x);
    int toSound = 57.29578f * FastAtan2(relative.z, relative.x);
    int angle = toSound - facing;
    if (angle > 180)
        angle = -(359 - angle);
    else if (angle < -180)
        angle += 359;
    *pan = angle;

    _D3DVECTOR v1;
    _D3DVECTOR v2;
    _D3DVECTOR dv;
    float speed1 = Length(velocity);
    float speed2 = Length(sourceVelocity);
    float heading1 = 57.29578f * FastAtan2(velocity.z, velocity.x);
    float heading2 = 57.29578f * FastAtan2(sourceVelocity.z, sourceVelocity.x);
    // Retail wraps this difference but never uses it (the compares remain).
    float diff = heading1 - heading2;
    if (diff > 180.0f)
        diff -= 360.0f;
    else if (diff < -180.0f)
        diff += 360.0f;
    v1.x = speed1 * FastCos(0.01745329f * heading1);
    v1.z = speed1 * FastSin(0.01745329f * heading1);
    v2.x = speed2 * FastCos(0.01745329f * heading2);
    v2.z = speed2 * FastSin(0.01745329f * heading2);
    dv.x = v1.x - v2.x;
    dv.y = 0.0f;
    dv.z = v1.z - v2.z;
    *doppler = Length(dv);
    return 1;
}
#pragma fast_fptosi reset


// ---- src/global/DistanceCompareDescend.cpp
#ifdef SPLASHDOWN_PC
#include "SoundEmitter/SoundEmitter.h"
#endif
class SortObject {
public:
#ifdef SPLASHDOWN_PC
    char pad0[__builtin_offsetof(SoundEmitter, m_closestDistance)];  // x86-64: the SoundEmitter field
#else
    char pad0[0x1D0];
#endif
    float m_distance;  // +0x1D0
};

// qsort comparator over SortObject pointers: farthest first.
int DistanceCompareDescend(const void *a, const void *b)
{
    float distA = (*(SortObject **)a)->m_distance;
    float distB = (*(SortObject **)b)->m_distance;
    if (distA > distB)
        return -1;
    return 1;
}

