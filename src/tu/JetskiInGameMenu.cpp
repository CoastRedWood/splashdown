// JetskiInGameMenu.cpp: the in-race menus, 0x2D0210-0x2E6630: JetskiMenu (their
// base), JetskiInGameMenu, JetskiTrainingInGameMenu, JetskiNameMenu and
// JetskiPromptMenu; "JetskiInGameMenu.cpp" in the ASSERTs.

// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp)

#include "GameObject/GameObject.h"
#include "ViewPortCode/ViewPortCode.h"
#include "Camera/Camera.h"
#include "ScreenIcon/ScreenIcon.h"
#include "DmaRenderItem/DmaRenderItem.h"
#include "JetskiMenu/JetskiMenu.h"
#include "ScreenIconManager/ScreenIconManager.h"
#include "soultree_assert_block.h"
#include "PSXRenderTarget/PSXRenderTarget.h"
#include "JetskiInfoText/JetskiInfoText.h"
#include "JetskiCompetition/JetskiCompetition.h"
#include "JetskiGame/JetskiGame.h"
#include "AuralScape/AuralScape.h"
#include "ResponseEmitter/ResponseEmitter.h"
#include "JetskiInGameMenu/JetskiInGameMenu.h"
#include "ScreenText/ScreenText.h"
#include "LocalizedString/LocalizedString.h"
#include "GameText/GameText.h"
#include "ScreenTextManager/ScreenTextManager.h"
#include "ScreenTextFontItemList/ScreenTextFontItem.h"
#include "ScreenTextFontItemList/ScreenTextFontItemList.h"
#include "ScreenTextFont/ScreenTextFont.h"
#include "TextureMap/TextureMap.h"
#include "JetskiScreenText/JetskiScreenText.h"
#include "RenderTarget/RenderTarget.h"
#include "RainbowPS2Snd/RainbowPS2Snd.h"
#include "PSXSoundInterface/PSXSoundInterface.h"
#include "PSXMusic/PSXMusic.h"
#include "JetskiGameOptions/JetskiGameOptions.h"
#include "ProcessNode/ProcessNode.h"
#include "PSXSoundGroup/PSXSoundGroup.h"
#include "PSXSound/PSXSound.h"
#include "Scene/Scene.h"
#include "JetskiRadar/JetskiRadar.h"
#include "JetskiInGameStats/JetskiInGameStats.h"
#include "JetskiPerformanceMeter/JetskiPerformanceMeter.h"
#include "JetskiTrainingInGameMenu/JetskiTrainingInGameMenu.h"
#include "JetskiTraining/JetskiTraining.h"
#include "JetskiNameMenu/JetskiNameMenu.h"
#include "CareerRacer/CareerRacer.h"
#include "RaceStatusEventInterface/RaceStatusEventInterface.h"
#include "JetskiPromptMenu/JetskiPromptMenu.h"
#include "RaceStatusStruct_Tag/RaceStatusStruct_Tag.h"
#include "JetskiDialogueStreamer/JetskiDialogueStreamer.h"
#include "JetskiCareerCompetition/JetskiCareerCompetition.h"
#include "JetskiGameOptions/JetskiGameOptions.h"
#include "HallOfFameListStruct/HallOfFameListStruct.h"
#include "JetskiCareer/JetskiCareer.h"
#include "RaceStatusClass/RaceStatusClass.h"
#include "ControlInterface/ControlTypes.h"
#include "TextureMap/TextureMap.h"
#include "PSXTextureMap/PSXTextureMap.h"
#include "sinit/AxisVectors.h"

// GameText::GetString (with its ASSERTF) is inlined inside SetString.
#pragma inline_depth(5)

extern "C" void *malloc(unsigned int size);
extern "C" void free(void *p);
extern "C" char *strcat(char *dst, const char *src);
extern "C" char *strcpy(char *dst, const char *src);
extern "C" int strcmp(const char *a, const char *b);
extern "C" char *strstr(const char *s, const char *find);
extern "C" int sprintf(char *buf, const char *fmt, ...);

extern char UITextureContentDirectory[0x104];
extern JetskiGame *LpGame;

int KEYDOWN_HANDLER_TEST(AssignableActionIndex action, DeviceType device, ProcessNode *node, unsigned int mods);

// Waits for the render queue before a menu draws (one copy for the TU).
static int WaitForRenderingToFinish = 1;

// ---- src/JetskiMenu/ctor.cpp
// GameObject as in GameObject/GameObject.h, plus its constructor and size.

JetskiMenu::JetskiMenu(int enabled) : GameObject(enabled)
{
    m_iconManager = 0;
    m_x34 = 0;
    m_x38 = 0;
    m_x3C = 0;
    m_x40 = 0;
    m_x44 = 0;
    m_x48 = 0;
    m_x4C = 0;
    m_x50 = 0;
    m_x54 = 0;
    m_x58 = 0;
    m_x5C = 0;
    m_x60 = 0;
    m_x68 = 0;
    m_x6C = 0;
    m_x70 = 0;
    m_viewport = (ViewPortCode)0;
    m_infoDirty = 1;
    m_menuViewportItem = 0;
    m_viewportItem = 0;
    m_xB8 = 0;
    m_savedViewport = LpRenderTarget->m_camera->m_viewport;
    m_menuViewport = m_savedViewport;
    for (int i = 0; i < 7; i++) {
        m_timers[i].m_active = 0;
        m_timers[i].m_time = 0.0f;
    }
    m_closing = 0;
    m_closeAction = -1;
}

// ---- src/JetskiMenu/dtor.cpp

#define DELETE_ICONS(icons, count)              \
    if (icons) {                                \
        for (i = 0; i < count; i++)             \
            delete icons[i];                    \
        free(icons);                            \
        icons = 0;                              \
    }

JetskiMenu::~JetskiMenu()
{
    int i;
    DELETE_ICONS(m_x34, 4)
    DELETE_ICONS(m_x38, 7)
    DELETE_ICONS(m_x3C, 7)
    DELETE_ICONS(m_x40, 4)
    DELETE_ICONS(m_x44, 1)
    DELETE_ICONS(m_x4C, 2)
    DELETE_ICONS(m_x50, 7)
    DELETE_ICONS(m_x54, 7)
    DELETE_ICONS(m_x58, 6)
    DELETE_ICONS(m_x5C, 4)
    if (m_menuViewportItem) {
        delete m_menuViewportItem;
        m_menuViewportItem = 0;
    }
    if (m_viewportItem) {
        delete m_viewportItem;
        m_viewportItem = 0;
    }
}

// ---- src/JetskiMenu/Create.cpp

JetskiMenu *JetskiMenu::Create(ScreenIconManager *, ViewPortCode viewport)
{
    m_viewport = viewport;
    m_iconManager = (new ScreenIconManager(1))->Create(0x80);
    AddChild(m_iconManager, -1);
    m_infoDirty = 1;
    m_menuViewportItem = new DmaRenderItem;
    m_viewportItem = new DmaRenderItem;
    return this;
}

// ---- src/JetskiMenu/BuildMenuFull.cpp

// A zeroed 0x30000 parser-heap image: after JetskiMenu's dtor, BuildMenuFull
// and BuildMenuSmall each put one float constant in another register without
// it.
struct MwccHeapImage {
    char tail[0x30000];
};
#pragma section mwccshim begin
inline void MwccHeapImageShim() { const MwccHeapImage image = { {0} }; }
#pragma section mwccshim end

// Builds the full-screen menu frame: four corners, two columns of seven
// tiles, the four button icons and the logo.
void JetskiMenu::BuildMenuFull(int numItems)
{
    char path[0x104];
    int i;

    m_x48 = 1;
    m_x6C = numItems;
    m_x68 = 0;
    m_x34 = (ScreenIcon **)malloc(4 * sizeof(ScreenIcon *));
    m_x38 = (ScreenIcon **)malloc(7 * sizeof(ScreenIcon *));
    m_x3C = (ScreenIcon **)malloc(7 * sizeof(ScreenIcon *));
    m_x40 = (ScreenIcon **)malloc(4 * sizeof(ScreenIcon *));
    m_x44 = (ScreenIcon **)malloc(1 * sizeof(ScreenIcon *));
    for (i = 0; i < 4; i++) {
#line 246 "JetskiInGameMenu.cpp"
        STRCPY(path, UITextureContentDirectory);
        switch (i) {
        case 0:
            strcat(path, "FullUL.tga");
            break;
        case 1:
            strcat(path, "FullUR.tga");
            break;
        case 2:
            strcat(path, "FullBL.tga");
            break;
        case 3:
            strcat(path, "FullBR.tga");
            break;
        }
        m_x34[i] = (new ScreenIcon)->Create(path, 0.0f, 0.0f);
        m_x34[i]->SetWidth(256.0f);
        m_x34[i]->SetHeight(256.0f);
    }
#line 271 "JetskiInGameMenu.cpp"
    STRCPY(path, UITextureContentDirectory);
    strcat(path, "BlueB.tga");
    for (i = 0; i < 7; i++) {
        m_x38[i] = (new ScreenIcon)->Create(path, 0.0f, 0.0f);
        switch (i) {
        case 0:
            m_x38[i]->SetWidth(127.0f);
            m_x38[i]->SetHeight(20.0f);
            m_x38[i]->SetUVRect(0.0f, 127.0f, 1.0f, 21.0f);
            break;
        case 1:
            m_x38[i]->SetWidth(127.0f);
            m_x38[i]->SetHeight(20.0f);
            m_x38[i]->SetUVRect(0.0f, 127.0f, 21.0f, 41.0f);
            break;
        case 2:
            m_x38[i]->SetWidth(40.0f);
            m_x38[i]->SetHeight(22.0f);
            m_x38[i]->SetUVRect(0.0f, 39.0f, 42.0f, 64.0f);
            break;
        case 3:
            m_x38[i]->SetWidth(40.0f);
            m_x38[i]->SetHeight(36.0f);
            m_x38[i]->SetUVRect(41.0f, 81.0f, 42.0f, 78.0f);
            break;
        case 4:
            m_x38[i]->SetWidth(41.0f);
            m_x38[i]->SetHeight(42.0f);
            m_x38[i]->SetUVRect(41.0f, 82.0f, 82.0f, 124.0f);
            break;
        case 5:
            m_x38[i]->SetWidth(36.0f);
            m_x38[i]->SetHeight(56.0f);
            m_x38[i]->SetUVRect(2.0f, 38.0f, 71.0f, 127.0f);
            break;
        case 6:
            m_x38[i]->SetWidth(20.0f);
            m_x38[i]->SetHeight(56.0f);
            m_x38[i]->SetUVRect(107.0f, 127.0f, 42.0f, 98.0f);
            break;
        }
    }
    for (i = 0; i < 7; i++) {
        m_x3C[i] = (new ScreenIcon)->Create(path, 0.0f, 0.0f);
        switch (i) {
        case 0:
            m_x3C[i]->SetWidth(127.0f);
            m_x3C[i]->SetHeight(20.0f);
            m_x3C[i]->SetUVRect(0.0f, 127.0f, 1.0f, 21.0f);
            break;
        case 1:
            m_x3C[i]->SetWidth(127.0f);
            m_x3C[i]->SetHeight(20.0f);
            m_x3C[i]->SetUVRect(0.0f, 127.0f, 21.0f, 41.0f);
            break;
        case 2:
            m_x3C[i]->SetWidth(40.0f);
            m_x3C[i]->SetHeight(22.0f);
            m_x3C[i]->SetUVRect(0.0f, 39.0f, 42.0f, 64.0f);
            break;
        case 3:
            m_x3C[i]->SetWidth(40.0f);
            m_x3C[i]->SetHeight(36.0f);
            m_x3C[i]->SetUVRect(41.0f, 81.0f, 42.0f, 78.0f);
            break;
        case 4:
            m_x3C[i]->SetWidth(41.0f);
            m_x3C[i]->SetHeight(42.0f);
            m_x3C[i]->SetUVRect(41.0f, 82.0f, 82.0f, 124.0f);
            break;
        case 5:
            m_x3C[i]->SetWidth(36.0f);
            m_x3C[i]->SetHeight(56.0f);
            m_x3C[i]->SetUVRect(2.0f, 38.0f, 71.0f, 127.0f);
            break;
        case 6:
            m_x3C[i]->SetWidth(20.0f);
            m_x3C[i]->SetHeight(56.0f);
            m_x3C[i]->SetUVRect(107.0f, 127.0f, 42.0f, 98.0f);
            break;
        }
    }
#line 371 "JetskiInGameMenu.cpp"
    STRCPY(path, UITextureContentDirectory);
    strcat(path, "SquareD.tga");
    m_x40[0] = (new ScreenIcon)->Create(path, 0.0f, 0.0f);
    m_x40[0]->SetWidth(22.0f);
    m_x40[0]->SetHeight(23.0f);
    m_x40[0]->SetUVRect(5.0f, 27.0f, 3.0f, 26.0f);
#line 380 "JetskiInGameMenu.cpp"
    STRCPY(path, UITextureContentDirectory);
    strcat(path, "SquareS.tga");
    m_x40[2] = (new ScreenIcon)->Create(path, 0.0f, 0.0f);
    m_x40[2]->SetWidth(22.0f);
    m_x40[2]->SetHeight(23.0f);
    m_x40[2]->SetUVRect(5.0f, 27.0f, 3.0f, 26.0f);
#line 389 "JetskiInGameMenu.cpp"
    STRCPY(path, UITextureContentDirectory);
    strcat(path, "XD.tga");
    m_x40[1] = (new ScreenIcon)->Create(path, 0.0f, 0.0f);
    m_x40[1]->SetWidth(22.0f);
    m_x40[1]->SetHeight(23.0f);
    m_x40[1]->SetUVRect(4.0f, 26.0f, 5.0f, 28.0f);
#line 398 "JetskiInGameMenu.cpp"
    STRCPY(path, UITextureContentDirectory);
    strcat(path, "XS.tga");
    m_x40[3] = (new ScreenIcon)->Create(path, 0.0f, 0.0f);
    m_x40[3]->SetWidth(22.0f);
    m_x40[3]->SetHeight(23.0f);
    m_x40[3]->SetUVRect(4.0f, 26.0f, 5.0f, 28.0f);
#line 407 "JetskiInGameMenu.cpp"
    STRCPY(path, UITextureContentDirectory);
    strcat(path, "CLogoC.tga");
    m_x44[0] = (new ScreenIcon)->Create(path, 0.0f, 0.0f);
    m_x44[0]->SetWidth(128.0f);
    m_x44[0]->SetHeight(52.0f);
    m_x44[0]->SetUVRect(0.0f, 128.0f, 9.0f, 61.0f);
}

// ---- src/JetskiMenu/BuildMenuSmall.cpp

// Builds the small (split-screen) menu frame: two halves, two columns of
// seven tiles, the six button icons and four little markers.
void JetskiMenu::BuildMenuSmall(int numItems)
{
    char path[0x104];
    int i;

    m_x60 = 1;
    m_x70 = numItems;
    m_x68 = 0;
    m_x4C = (ScreenIcon **)malloc(2 * sizeof(ScreenIcon *));
    m_x50 = (ScreenIcon **)malloc(7 * sizeof(ScreenIcon *));
    m_x54 = (ScreenIcon **)malloc(7 * sizeof(ScreenIcon *));
    m_x58 = (ScreenIcon **)malloc(6 * sizeof(ScreenIcon *));
    m_x5C = (ScreenIcon **)malloc(4 * sizeof(ScreenIcon *));
    for (i = 0; i < 2; i++) {
#line 440 "JetskiInGameMenu.cpp"
        STRCPY(path, UITextureContentDirectory);
        switch (i) {
        case 0:
            strcat(path, "SmallWL.tga");
            break;
        case 1:
            strcat(path, "SmallWR.tga");
            break;
        }
        m_x4C[i] = (new ScreenIcon)->Create(path, 0.0f, 0.0f);
        m_x4C[i]->SetWidth(256.0f);
        m_x4C[i]->SetHeight(256.0f);
    }
#line 455 "JetskiInGameMenu.cpp"
    STRCPY(path, UITextureContentDirectory);
    strcat(path, "BlueB.tga");
    for (i = 0; i < 7; i++) {
        // Retail loads the y coordinate first here and copies it to x (not in
        // the other loops); the chained assignment reproduces that.
        float x, y;
        m_x50[i] = (new ScreenIcon)->Create(path, x = y = 0.0f, y);
        switch (i) {
        case 0:
            m_x50[i]->SetWidth(127.0f);
            m_x50[i]->SetHeight(20.0f);
            m_x50[i]->SetUVRect(0.0f, 127.0f, 1.0f, 21.0f);
            break;
        case 1:
            m_x50[i]->SetWidth(127.0f);
            m_x50[i]->SetHeight(20.0f);
            m_x50[i]->SetUVRect(0.0f, 127.0f, 21.0f, 41.0f);
            break;
        case 2:
            m_x50[i]->SetWidth(40.0f);
            m_x50[i]->SetHeight(22.0f);
            m_x50[i]->SetUVRect(0.0f, 39.0f, 42.0f, 64.0f);
            break;
        case 3:
            m_x50[i]->SetWidth(40.0f);
            m_x50[i]->SetHeight(36.0f);
            m_x50[i]->SetUVRect(41.0f, 81.0f, 42.0f, 78.0f);
            break;
        case 4:
            m_x50[i]->SetWidth(41.0f);
            m_x50[i]->SetHeight(42.0f);
            m_x50[i]->SetUVRect(41.0f, 82.0f, 82.0f, 124.0f);
            break;
        case 5:
            m_x50[i]->SetWidth(36.0f);
            m_x50[i]->SetHeight(56.0f);
            m_x50[i]->SetUVRect(2.0f, 38.0f, 71.0f, 127.0f);
            break;
        case 6:
            m_x50[i]->SetWidth(20.0f);
            m_x50[i]->SetHeight(56.0f);
            m_x50[i]->SetUVRect(107.0f, 127.0f, 42.0f, 98.0f);
            break;
        }
    }
    for (i = 0; i < 7; i++) {
        m_x54[i] = (new ScreenIcon)->Create(path, 0.0f, 0.0f);
        switch (i) {
        case 0:
            m_x54[i]->SetWidth(127.0f);
            m_x54[i]->SetHeight(20.0f);
            m_x54[i]->SetUVRect(0.0f, 127.0f, 1.0f, 21.0f);
            break;
        case 1:
            m_x54[i]->SetWidth(127.0f);
            m_x54[i]->SetHeight(20.0f);
            m_x54[i]->SetUVRect(0.0f, 127.0f, 21.0f, 41.0f);
            break;
        case 2:
            m_x54[i]->SetWidth(40.0f);
            m_x54[i]->SetHeight(22.0f);
            m_x54[i]->SetUVRect(0.0f, 39.0f, 42.0f, 64.0f);
            break;
        case 3:
            m_x54[i]->SetWidth(40.0f);
            m_x54[i]->SetHeight(36.0f);
            m_x54[i]->SetUVRect(41.0f, 81.0f, 42.0f, 78.0f);
            break;
        case 4:
            m_x54[i]->SetWidth(41.0f);
            m_x54[i]->SetHeight(42.0f);
            m_x54[i]->SetUVRect(41.0f, 82.0f, 82.0f, 124.0f);
            break;
        case 5:
            m_x54[i]->SetWidth(36.0f);
            m_x54[i]->SetHeight(56.0f);
            m_x54[i]->SetUVRect(2.0f, 38.0f, 71.0f, 127.0f);
            break;
        case 6:
            m_x54[i]->SetWidth(20.0f);
            m_x54[i]->SetHeight(56.0f);
            m_x54[i]->SetUVRect(107.0f, 127.0f, 42.0f, 98.0f);
            break;
        }
    }
#line 555 "JetskiInGameMenu.cpp"
    STRCPY(path, UITextureContentDirectory);
    strcat(path, "TriangleD.tga");
    m_x58[0] = (new ScreenIcon)->Create(path, 0.0f, 0.0f);
    m_x58[0]->SetWidth(22.0f);
    m_x58[0]->SetHeight(23.0f);
    m_x58[0]->SetUVRect(5.0f, 27.0f, 3.0f, 26.0f);
#line 564 "JetskiInGameMenu.cpp"
    STRCPY(path, UITextureContentDirectory);
    strcat(path, "TriangleS.tga");
    m_x58[3] = (new ScreenIcon)->Create(path, 0.0f, 0.0f);
    m_x58[3]->SetWidth(22.0f);
    m_x58[3]->SetHeight(23.0f);
    m_x58[3]->SetUVRect(5.0f, 27.0f, 3.0f, 26.0f);
#line 573 "JetskiInGameMenu.cpp"
    STRCPY(path, UITextureContentDirectory);
    strcat(path, "XD.tga");
    m_x58[1] = (new ScreenIcon)->Create(path, 0.0f, 0.0f);
    m_x58[1]->SetWidth(22.0f);
    m_x58[1]->SetHeight(23.0f);
    m_x58[1]->SetUVRect(4.0f, 26.0f, 5.0f, 28.0f);
#line 582 "JetskiInGameMenu.cpp"
    STRCPY(path, UITextureContentDirectory);
    strcat(path, "XS.tga");
    m_x58[4] = (new ScreenIcon)->Create(path, 0.0f, 0.0f);
    m_x58[4]->SetWidth(22.0f);
    m_x58[4]->SetHeight(23.0f);
    m_x58[4]->SetUVRect(4.0f, 26.0f, 5.0f, 28.0f);
#line 591 "JetskiInGameMenu.cpp"
    STRCPY(path, UITextureContentDirectory);
    strcat(path, "SquareD.tga");
    m_x58[2] = (new ScreenIcon)->Create(path, 0.0f, 0.0f);
    m_x58[2]->SetWidth(22.0f);
    m_x58[2]->SetHeight(23.0f);
    m_x58[2]->SetUVRect(5.0f, 27.0f, 3.0f, 26.0f);
#line 600 "JetskiInGameMenu.cpp"
    STRCPY(path, UITextureContentDirectory);
    strcat(path, "SquareS.tga");
    m_x58[5] = (new ScreenIcon)->Create(path, 0.0f, 0.0f);
    m_x58[5]->SetWidth(22.0f);
    m_x58[5]->SetHeight(23.0f);
    m_x58[5]->SetUVRect(5.0f, 27.0f, 3.0f, 26.0f);
#line 609 "JetskiInGameMenu.cpp"
    STRCPY(path, UITextureContentDirectory);
    strcat(path, "LittleWht.tga");
    m_x5C[0] = (new ScreenIcon)->Create(path, 0.0f, 0.0f);
    m_x5C[0]->SetWidth(12.0f);
    m_x5C[0]->SetHeight(12.0f);
    m_x5C[0]->SetUVRect(1.0f, 13.0f, 1.0f, 13.0f);
#line 621 "JetskiInGameMenu.cpp"
    STRCPY(path, UITextureContentDirectory);
    strcat(path, "LittleWhtR.tga");
    m_x5C[1] = (new ScreenIcon)->Create(path, 0.0f, 0.0f);
    m_x5C[1]->SetWidth(12.0f);
    m_x5C[1]->SetHeight(12.0f);
    m_x5C[1]->SetUVRect(1.0f, 13.0f, 1.0f, 13.0f);
#line 632 "JetskiInGameMenu.cpp"
    STRCPY(path, UITextureContentDirectory);
    strcat(path, "LittleYlw.tga");
    m_x5C[2] = (new ScreenIcon)->Create(path, 0.0f, 0.0f);
    m_x5C[2]->SetWidth(12.0f);
    m_x5C[2]->SetHeight(12.0f);
    m_x5C[2]->SetUVRect(1.0f, 13.0f, 1.0f, 13.0f);
#line 642 "JetskiInGameMenu.cpp"
    STRCPY(path, UITextureContentDirectory);
    strcat(path, "LittleYlwR.tga");
    m_x5C[3] = (new ScreenIcon)->Create(path, 0.0f, 0.0f);
    m_x5C[3]->SetWidth(12.0f);
    m_x5C[3]->SetHeight(12.0f);
    m_x5C[3]->SetUVRect(1.0f, 13.0f, 1.0f, 13.0f);
}

// ---- src/JetskiMenu/PrepareGeometry.cpp

// Keeps every icon texture's usage at least 10.
#define CLAMP_TEXTS(list, count)                                    \
    if (list) {                                                     \
        for (i = 0; i < count; i++) {                               \
            if (list[i]) {                                          \
                BaseTextureMap *texture = list[i]->m_texture;      \
                if (10.0f > texture->m_usage)                       \
                    texture->m_usage = 10.0f;                         \
            }                                                       \
        }                                                           \
    }

int JetskiMenu::PrepareGeometry()
{
    int i;

    if (m_x48) {
        CLAMP_TEXTS(m_x34, 4)
        CLAMP_TEXTS(m_x38, 7)
        CLAMP_TEXTS(m_x3C, 7)
        CLAMP_TEXTS(m_x40, 4)
        CLAMP_TEXTS(m_x44, 1)
    }
    if (m_x60) {
        CLAMP_TEXTS(m_x4C, 2)
        CLAMP_TEXTS(m_x50, 7)
        CLAMP_TEXTS(m_x54, 7)
        CLAMP_TEXTS(m_x58, 6)
        CLAMP_TEXTS(m_x5C, 4)
    }
    return GameObject::PrepareGeometry();
}

// ---- src/JetskiMenu/SetViewport.cpp

// Switch to a full-screen (640x448) viewport for the menu, remembering the
// camera's viewport for RestoreViewport().
void JetskiMenu::SetViewport()
{
    m_menuViewport = LpRenderTarget->m_camera->m_viewport;
    m_savedViewport = m_menuViewport;
    m_menuViewport.X = 0;
    m_menuViewport.Y = 0;
    m_menuViewport.Width = 640;
    m_menuViewport.Height = 448;
    m_menuViewportItem->Start((DmaRenderDestination)1, 0x80);
    ((PSXRenderTarget *)LpRenderTarget)->SetViewport(&m_menuViewport, m_menuViewportItem, &m_savedViewport);
    m_menuViewportItem->End();
    ((PSXRenderTarget *)LpRenderTarget)->AddToRenderQueue(m_menuViewportItem, "");
}

// ---- src/JetskiMenu/RestoreViewport.cpp

void JetskiMenu::RestoreViewport()
{
    m_viewportItem->Start((DmaRenderDestination)1, 0x80);
    ((PSXRenderTarget *)LpRenderTarget)->SetViewport(&m_savedViewport, m_viewportItem, &m_menuViewport);
    m_viewportItem->End();
    ((PSXRenderTarget *)LpRenderTarget)->AddToRenderQueue(m_viewportItem, "");
}

// ---- src/JetskiMenu/Render3D.cpp

#define ADD_ICONS(list, count)                                      \
    i = 0;                                                          \
    if (list) {                                                     \
        for (; i < count; i++) {                                    \
            if (list[i])                                            \
                m_iconManager->AddIcon(list[i], 0);                 \
        }                                                           \
    }

#define ADD_SEVEN(list)                                             \
    if (list) {                                                     \
        m_iconManager->AddIcon(list[6], 0);                         \
        m_iconManager->AddIcon(list[5], 0);                         \
        m_iconManager->AddIcon(list[3], 0);                         \
        m_iconManager->AddIcon(list[4], 0);                         \
        m_iconManager->AddIcon(list[2], 0);                         \
        m_iconManager->AddIcon(list[1], 0);                         \
        m_iconManager->AddIcon(list[0], 0);                         \
    }

int JetskiMenu::Render3D()
{
    int i;

    if (m_x48) {
        ADD_ICONS(m_x34, 4)
        ADD_SEVEN(m_x38)
        ADD_ICONS(m_x3C, 7)
        i = 0;
        if (m_x40) {
            for (; i < 2; i++) {
                if (!m_x40[i])
                    continue;
                if (i == 0 || i == 2) {
                    switch (m_x6C) {
                    case 12:
                    case 8:
                        break;
                    default:
                        if (m_timers[6].m_active)
                            m_iconManager->AddIcon(m_x40[2], 0);
                        else
                            m_iconManager->AddIcon(m_x40[0], 0);
                        break;
                    }
                } else if (i == 1 || i == 3) {
                    switch (m_x6C) {
                    case 8:
                        break;
                    default:
                        if (m_timers[5].m_active)
                            m_iconManager->AddIcon(m_x40[3], 0);
                        else
                            m_iconManager->AddIcon(m_x40[1], 0);
                        break;
                    }
                }
            }
        }
        if (m_x44) {
            for (i = 0; i < 1; i++) {
                if (!m_x44[i])
                    continue;
                switch (m_x6C) {
                case 1:
                case 2:
                case 4:
                    m_iconManager->AddIcon(m_x44[i], 0);
                    break;
                }
            }
        }
        LpGame->m_competition->m_infoText->AddInfoText("", 0, 0.0f, 1, 1.0f);
    }
    if (m_x60) {
        ADD_ICONS(m_x4C, 2)
        ADD_SEVEN(m_x50)
        ADD_ICONS(m_x54, 7)
        i = 0;
        if (m_x58) {
            for (; i < 3; i++) {
                if (!m_x58[i])
                    continue;
                if (i == 0 || i == 3) {
                    if (m_x68 > 0) {
                        if (m_timers[3].m_active)
                            m_iconManager->AddIcon(m_x58[3], 0);
                        else
                            m_iconManager->AddIcon(m_x58[0], 0);
                    }
                } else if (i == 1 || i == 4) {
                    if (m_x68 <= 0 || m_x70 == 9 || m_x70 == 11) {
                        if (m_timers[2].m_active)
                            m_iconManager->AddIcon(m_x58[4], 0);
                        else
                            m_iconManager->AddIcon(m_x58[1], 0);
                    }
                } else if (i == 2 || i == 5) {
                    switch (m_x70) {
                    case 6:
                        if (m_timers[4].m_active)
                            m_iconManager->AddIcon(m_x58[5], 0);
                        else
                            m_iconManager->AddIcon(m_x58[2], 0);
                        break;
                    }
                }
            }
        }
        if (m_x5C && m_x70 == 0 && m_x68 == 1) {
            if (m_timers[0].m_active) {
                if (m_x5C[2])
                    m_iconManager->AddIcon(m_x5C[2], 0);
            } else {
                if (m_x5C[0])
                    m_iconManager->AddIcon(m_x5C[0], 0);
            }
            if (m_timers[1].m_active) {
                if (m_x5C[3])
                    m_iconManager->AddIcon(m_x5C[3], 0);
            } else {
                if (m_x5C[1])
                    m_iconManager->AddIcon(m_x5C[1], 0);
            }
        }
        switch (m_x70) {
        case 1: case 2: case 3: case 4: case 5: case 6:
        case 7: case 8: case 9: case 10: case 11: case 12:
            LpGame->m_competition->m_infoText->AddInfoText("", 0, 0.0f, 1, 1.0f);
            break;
        }
    }
    return 1;
}

// ---- src/JetskiMenu/EnableMenu.cpp

void JetskiMenu::EnableMenu()
{
    LpGame->m_auralScape->StopAllAuralScapeSounds();
    LpGame->m_auralScape->Disable();
    EnableAll();
}

// ---- src/JetskiMenu/DisableMenu.cpp

void JetskiMenu::DisableMenu()
{
    LpGame->m_auralScape->Enable();
    LpGame->m_competition->ResetAllEngineSounds();
}

// ---- src/JetskiMenu/SetupInfo.cpp

// Lays out the menu's icons.
void JetskiMenu::SetupInfo()
{
    int i;

    if (m_x48) {
        if (m_x34) {
            m_x34[0]->SetPosition(64.0f, -32.0f);
            m_x34[1]->SetPosition(320.0f, -32.0f);
            m_x34[2]->SetPosition(64.0f, 224.0f);
            m_x34[3]->SetPosition(320.0f, 224.0f);
        }
        if (m_x38) {
            m_x38[0]->SetPosition(327.0f, 15.0f);
            m_x38[1]->SetPosition(201.0f, 15.0f);
            m_x38[2]->SetPosition(162.0f, 15.0f);
            m_x38[3]->SetPosition(122.0f, 16.0f);
            m_x38[4]->SetPosition(82.0f, 27.0f);
            m_x38[5]->SetPosition(69.0f, 68.0f);
            m_x38[6]->SetPosition(69.0f, 123.0f);
        }
        if (m_x3C) {
            m_x3C[0]->SetPosition(327.0f, 15.0f);
            m_x3C[1]->SetPosition(201.0f, 15.0f);
            m_x3C[2]->SetPosition(162.0f, 15.0f);
            m_x3C[3]->SetPosition(122.0f, 16.0f);
            m_x3C[4]->SetPosition(82.0f, 27.0f);
            m_x3C[5]->SetPosition(69.0f, 68.0f);
            m_x3C[6]->SetPosition(69.0f, 123.0f);
            for (i = 0; i < 7; i++) {
                m_x3C[i]->SetAngle(3.14f);
                m_x3C[i]->SetPivot(320.0f, 224.0f);
            }
        }
        if (m_x40) {
            m_x40[0]->SetPosition(275.0f, 386.0f);
            m_x40[2]->SetPosition(275.0f, 386.0f);
            m_x40[1]->SetPosition(389.0f, 386.0f);
            m_x40[3]->SetPosition(389.0f, 386.0f);
        }
        if (m_x44) {
            switch (m_x6C) {
            case 1:
            case 2:
            case 4:
            case 5:
            case 6:
                m_x44[0]->SetPosition(455.0f, 24.0f);
                break;
            }
        }
    }
    if (m_x60) {
        if (m_x4C) {
            m_x4C[0]->SetPosition(64.0f, 100.0f);
            m_x4C[1]->SetPosition(320.0f, 100.0f);
        }
        if (m_x50) {
            m_x50[0]->SetPosition(343.0f, 98.0f);
            m_x50[1]->SetPosition(217.0f, 98.0f);
            m_x50[2]->SetPosition(178.0f, 98.0f);
            m_x50[3]->SetPosition(138.0f, 99.0f);
            m_x50[4]->SetPosition(98.0f, 109.0f);
            m_x50[5]->SetPosition(85.0f, 150.0f);
            m_x50[6]->SetPosition(85.0f, 205.0f);
        }
        if (m_x54) {
            m_x54[0]->SetPosition(343.0f, 98.0f);
            m_x54[1]->SetPosition(217.0f, 98.0f);
            m_x54[2]->SetPosition(178.0f, 98.0f);
            m_x54[3]->SetPosition(138.0f, 99.0f);
            m_x54[4]->SetPosition(98.0f, 109.0f);
            m_x54[5]->SetPosition(85.0f, 150.0f);
            m_x54[6]->SetPosition(85.0f, 205.0f);
            for (i = 0; i < 7; i++) {
                m_x54[i]->SetAngle(3.14f);
                m_x54[i]->SetPivot(313.0f, 224.0f);
            }
        }
        if (m_x58) {
            m_x58[0]->SetPosition(245.0f, 305.0f);
            m_x58[3]->SetPosition(245.0f, 305.0f);
            m_x58[1]->SetPosition(359.0f, 305.0f);
            m_x58[4]->SetPosition(359.0f, 305.0f);
            m_x58[2]->SetPosition(245.0f, 305.0f);
            m_x58[5]->SetPosition(245.0f, 305.0f);
        }
    }
}

// ---- src/JetskiMenu/PreparePlayback.cpp

void JetskiMenu::PreparePlayback(PlaybackType type)
{
    LpGame->m_competition->SetPlayback(type);
    LpGame->m_competition->Pause(0);
    LpGame->m_auralScape->Enable();
    LpGame->m_competition->ResetAllEngineSounds();
    DisableAll();
}

// ---- src/JetskiMenu/KeyDownHandler.cpp

#define ACTION(n) ((AssignableActionIndex)(n))
#define TEST(n) KEYDOWN_HANDLER_TEST(ACTION(n), DEVICE_JOYSTICK, node, 0x80000000)
#define PLAY(n) LpGame->m_competition->m_responses->TriggerResponse(n, 0, 0)

int JetskiMenu::KeyDownHandler(ProcessNode *node, KeyInfo *key)
{
    GameObject::KeyDownHandler(node, key);
    if (TEST(2) || TEST(3)) {
        if (m_x48) {
            switch (m_x6C) {
            case 8:
                PLAY(1);
                break;
            }
        } else if (m_x60) {
            switch (m_x70) {
            case 0:
            case 9:
                PLAY(2);
                break;
            }
        }
    } else if (TEST(4)) {
        if (m_x48) {
            switch (m_x6C) {
            case 8:
                break;
            default:
                m_timers[5].m_active = 1;
                m_timers[5].m_time = 0.12f;
                PLAY(3);
                break;
            }
        } else if (m_x60) {
            if (m_x68 <= 0 || m_x70 != 0) {
                m_timers[2].m_active = 1;
                m_timers[2].m_time = 0.12f;
                PLAY(3);
            }
        }
    } else if (TEST(5)) {
        if (m_x48) {
            switch (m_x6C) {
            case 12:
            case 8:
                break;
            default:
                m_timers[6].m_active = 1;
                m_timers[6].m_time = 0.12f;
                PLAY(3);
                break;
            }
        } else if (m_x60) {
            switch (m_x70) {
            case 6:
                m_timers[4].m_active = 1;
                m_timers[4].m_time = 0.12f;
                PLAY(3);
                break;
            }
        }
    } else if (TEST(6)) {
        if (m_x60) {
            m_timers[3].m_active = 1;
            m_timers[3].m_time = 0.12f;
        }
    } else if (!TEST(0)) {
        TEST(1);
    }
    return 1;
}

// ---- src/JetskiMenu/PostTextPositioning.cpp

void JetskiMenu::PostTextPositioning()
{
    int x, y;   // left unset for other modes, as in retail
    switch (m_viewport) {
    case 0:
    case 1:
    case 2:
        x = 0;
        y = 0;
        break;
    }
    if (m_x5C && !m_x70) {
        ScreenText *box = ((JetskiInGameMenu *)LpGame->m_competition->m_inGameMenu)->m_infoTexts[3];
        float left = box->m_position.x;
        float width = 10.0f + (left - box->m_x20.x);
        m_x5C[0]->SetPosition((x + (left - width)) - 14.0f, y + 0xEE);
        m_x5C[1]->SetPosition(x + (left + width), y + 0xEE);
        m_x5C[2]->SetPosition((x + (left - width)) - 14.0f, y + 0xEE);
        m_x5C[3]->SetPosition(x + (left + width), y + 0xEE);
    }
}

// ---- src/JetskiMenu/Tick.cpp

int JetskiMenu::Tick(float dt)
{
    for (int i = 0; i < 7; i++) {
        if (m_timers[i].m_active == 1) {
            m_timers[i].m_time -= dt;
            if (m_timers[i].m_time <= 0.0f)
                m_timers[i].m_active = 0;
        }
    }
    return GameObject::Tick(dt);
}

// ---- src/JetskiInGameMenu/ctor.cpp

JetskiInGameMenu::JetskiInGameMenu(int enabled) : JetskiMenu(enabled)
{
    m_textManager = 0;
    m_texts = 0;
    m_menuTexts = 0;
    m_infoTexts = 0;
    m_font = 0;
    m_titleFont = 0;
    m_headerFont = 0;
    m_selection = 0;
    m_x118 = 0;
    m_x11C = 0;
    m_x68 = 0;
    m_numItems[0] = 7;
    m_numTexts[0] = 12;
    m_numItems[1] = 4;
    m_numTexts[1] = 15;
    m_track = -1;
    m_musicChanged = 0;
}

// ---- src/JetskiInGameMenu/dtor.cpp

JetskiInGameMenu::~JetskiInGameMenu()
{
    int i;
    for (i = 0; i < 12; i++)
        delete m_menuTexts[i];
    for (i = 0; i < 15; i++)
        delete m_infoTexts[i];
    free(m_menuTexts);
    free(m_infoTexts);
    m_texts = 0;
    m_menuTexts = 0;
    m_infoTexts = 0;
}

// ---- src/JetskiInGameMenu/Create.cpp

// Colours go through a block-local copy at each call (and SetColor copies
// again, in ScreenText.h): that puts retail's colour constants in the right
// registers.
#define SET_COLOR(text, value)          \
    {                                   \
        unsigned int color = value;     \
        (text)->SetColor(color);        \
    }

// Bar of 'I's whose length shows a volume setting (in steps of 12).
#define SET_METER(meter)                                                   \
    {                                                                      \
        int length = (&LpGame->m_gameOptions->m_musicVolume)[meter] / 12;               \
        strcpy(bar, m_infoTexts[i]->m_text);                                  \
        for (int j = length + 1; j < 10; j++)                              \
            bar[j] = 0;                                                    \
        m_infoTexts[i]->SetString(bar);                                         \
        SET_COLOR(m_infoTexts[i], 0xFFD76464)                                   \
    }

JetskiInGameMenu *JetskiInGameMenu::Create(ScreenIconManager *icons, ScreenTextManager *texts, ViewPortCode viewport)
{
    char bar[64];
    int i;
    int id;

    JetskiMenu::Create(icons, viewport);
    BuildMenuSmall(0);
    ScreenTextFontItem *item = LpGame->m_fonts->GetScreenTextFont("info_b");
    m_font = item ? item->m_font : 0;
    item = LpGame->m_fonts->GetScreenTextFont("info_c");
    m_titleFont = item ? item->m_font : 0;
    item = LpGame->m_fonts->GetScreenTextFont("title_b");
    m_headerFont = item ? item->m_font : 0;
    m_textManager = (new ScreenTextManager(0, 1))->Create(64);
    AddChild(m_textManager, -1);
    m_menuTexts = (ScreenText **)malloc(12 * sizeof(ScreenText *));
    m_infoTexts = (ScreenText **)malloc(15 * sizeof(ScreenText *));
    id = 0;
    for (i = 0; i < 12; i++) {
        float scale = 1.125f;
        switch (i) {
        case 0:
            id = 128;
            break;
        case 1:
            id = 129;
            break;
        case 2:
            id = 133;
            break;
        case 9:
            id = 134;
            break;
        case 3:
            id = 136;
            break;
        case 10:
            id = 134;
            break;
        case 4:
            id = 137;
            break;
        case 11:
            id = 134;
            break;
        case 5:
            id = 135;
            break;
        case 6:
            id = 139;
            break;
        case 8:
            scale = 1.0f;
            id = 11;
            break;
        case 7:
            id = 250;
            scale = 1.0f;
            break;
        }
        m_menuTexts[i] = new ScreenText(LpGame->m_text->GetMaxStringLength(id), LpGame->m_text->GetString(id));
        m_menuTexts[i]->SetScale(scale);
    }
    for (i = 0; i < 15; i++) {
        m_infoTexts[i] = new ScreenText(40, 0);
        m_infoTexts[i]->SetScale(1.125f);
        switch (i) {
        case 0:
            m_infoTexts[i]->SetString(LpGame->m_text->GetString(130));
            break;
        case 7:
        case 8:
            m_infoTexts[i]->SetString("IIIIIIIII");
            if (i == 8)
                SET_METER(0)
            else
                SET_COLOR(m_infoTexts[i], 0xFFFFFFFF)
            break;
        case 1:
            m_infoTexts[i]->SetString(LpGame->m_text->GetString(131));
            break;
        case 9:
        case 10:
            m_infoTexts[i]->SetString("IIIIIIIII");
            if (i == 10)
                SET_METER(1)
            else
                SET_COLOR(m_infoTexts[i], 0xFFFFFFFF)
            break;
        case 2:
            m_infoTexts[i]->SetString(LpGame->m_text->GetString(132));
            break;
        case 11:
        case 12:
            m_infoTexts[i]->SetString("IIIIIIIII");
            if (i == 12)
                SET_METER(2)
            else
                SET_COLOR(m_infoTexts[i], 0xFFFFFFFF)
            break;
        case 3:
            m_infoTexts[i]->SetString(LpGame->m_text->GetString(138));
            break;
        case 5:
            m_infoTexts[i]->SetString(LpGame->m_text->GetString(11));
            m_infoTexts[i]->SetScale(1.0f);
            break;
        case 6:
            m_infoTexts[i]->SetString(LpGame->m_text->GetString(12));
            m_infoTexts[i]->SetScale(1.0f);
            break;
        case 4:
            m_infoTexts[i]->SetString(LpGame->m_text->GetString(135));
            m_infoTexts[i]->SetScale(1.0f);
            break;
        case 13:
        case 14:
            break;
        }
    }
    m_texts = m_menuTexts;
    m_x68 = 0;                          // the selection group, not m_selection
    SelectText();
    return this;
}

// ---- src/JetskiInGameMenu/PrepareGeometry.cpp

// Clamp the items' minimum widths to 10, rebuild the info panel if it was
// invalidated, then let the base menu prepare its geometry.
int JetskiInGameMenu::PrepareGeometry()
{
    if (m_font) {
        TextureMap *texture = m_font->m_texture;
        if (10.0f > texture->m_usage)
            texture->m_usage = 10.0f;
    }
    if (m_titleFont) {
        TextureMap *texture = m_titleFont->m_texture;
        if (10.0f > texture->m_usage)
            texture->m_usage = 10.0f;
    }
    if (m_headerFont) {
        TextureMap *texture = m_headerFont->m_texture;
        if (10.0f > texture->m_usage)
            texture->m_usage = 10.0f;
    }
    if (m_infoDirty) {
        SetupInfo();
        m_infoDirty = 0;
    }
    return JetskiMenu::PrepareGeometry();
}

// ---- src/JetskiInGameMenu/Render3D.cpp
// GameObject per the retail vtable (Render3D/PrepareGeometry return int here).

// Draw once per frame, on the last player's pass.
int JetskiInGameMenu::Render3D()
{
    if (LpGame->m_viewport == LpGame->m_numCameras - 1) {
        if (WaitForRenderingToFinish && m_viewport) {
            LpRenderTarget->TrySendRenderQueue(1);
            LpRenderTarget->WaitForRenderingToComplete();
        }
        SetViewport();
        JetskiMenu::Render3D();
        for (int i = 0; i < m_numTexts[m_x68]; i++) {
            switch (m_x68) {
            case 0:
                switch (i) {
                case 0: case 1: case 2: case 3: case 5: case 6: case 9: case 10:
                    m_textManager->AddText(m_texts[i], m_font);
                    break;
                case 4: case 11:
                    if (LpGame->m_competition->m_radar)
                        m_textManager->AddText(m_texts[i], m_font);
                    break;
                case 8:
                    m_textManager->AddText(m_texts[i], m_titleFont);
                    break;
                case 7:
                    m_textManager->AddText(m_texts[i], m_headerFont);
                    break;
                }
                break;
            case 1:
                switch (i) {
                case 0: case 1: case 2: case 3: case 7: case 8: case 9: case 10:
                case 11: case 12: case 13: case 14:
                    m_textManager->AddText(m_texts[i], m_font);
                    break;
                case 6:
                    m_textManager->AddText(m_texts[i], m_titleFont);
                    break;
                case 4:
                    m_textManager->AddText(m_texts[i], m_headerFont);
                    break;
                case 5:
                    break;
                }
                break;
            }
        }
        GameObject::Render3D();
        RestoreViewport();
    }
    return 1;
}

// ---- src/JetskiInGameMenu/Tick.cpp

int JetskiInGameMenu::Tick(float dt)
{
    int busy = 0;
    JetskiGameOptions *options = LpGame->m_gameOptions;
    JetskiCompetition *competition = LpGame->m_competition;
    LpGame->m_auralScape->m_soundInterface->m_snd.GetPlayStatus(dt);
    if (competition->m_music)
        competition->m_music->Tick(dt);
    if (m_timers[0].m_active == 1 || m_timers[1].m_active == 1)
        busy = 1;
    if (m_musicChanged && !busy) {
        if (competition->m_music->m_playing)
            competition->m_music->Stop(0.0f);
        competition->m_music->ShuffleStartingAt(m_track + 1, 1);
        m_musicChanged = 0;
    }
    if (competition->m_music->m_playing && !m_musicChanged) {
        int track = competition->m_music->m_track - 1;
        if (track != m_track) {
            m_track = track;
            m_infoTexts[13]->SetString(LpGame->m_competition->m_trackNames[m_track]);
            m_infoTexts[14]->SetString(LpGame->m_competition->m_trackNames2[m_track]);
        }
    }
    int i;
    int moving = 0;
    if (m_closing) {
        for (i = 0; i < 7; i++) {
            if (m_timers[i].m_active)
                moving = 1;
        }
        if (!moving) {
            switch (m_closeAction) {
            case 0:
                DisableMenu();
                competition->InGamePause(0, m_xB8);
                SelectText();
                m_closing = 0;
                m_closeAction = -1;
                break;
            case 1:
                DisableMenu();
                competition->RestartRace((GamePlayStatus)3);
                m_closing = 0;
                m_closeAction = -1;
                SelectText();
                break;
            case 2:
                m_selection = 0;
                m_x68 = 1;
                m_texts = m_infoTexts;
                SelectText();
                m_closing = 0;
                m_closeAction = -1;
                if (m_x68 == 1 && (m_selection == 0 || m_selection == 3)) {
                    if (competition->m_music && competition->m_x1F0)
                        competition->m_x1F0->SetVolume(LpGame->m_gameOptions->m_musicVolume);
                } else {
                    if (competition->m_music && competition->m_x1F0)
                        competition->m_x1F0->SetVolume(0);
                }
                break;
            case 3:
                m_selection = 5;
                m_x68 = 0;
                m_texts = m_menuTexts;
                SelectText();
                m_closing = 0;
                m_closeAction = -1;
                if (m_x68 == 1 && (m_selection == 0 || m_selection == 3)) {
                    if (competition->m_music && competition->m_x1F0)
                        competition->m_x1F0->SetVolume(LpGame->m_gameOptions->m_musicVolume);
                } else {
                    if (competition->m_music && competition->m_x1F0)
                        competition->m_x1F0->SetVolume(0);
                }
                break;
            case 4:
                DisableMenu();
                if (options->m_raceType == 3) {
                    competition->Pause(1);
                    options->m_careerMode = 0;
                    competition->m_nameMenu->Initialize(0);
                } else {
                    competition->EventComplete();
                    SelectText();
                }
                m_closing = 0;
                m_closeAction = -1;
                break;
            }
        }
    }
    return JetskiMenu::Tick(dt);
}

// ---- src/JetskiInGameMenu/KeyDownHandler.cpp
// Optimizer-off function (KEYDOWN_HANDLER_TEST's asm): $s registers are assigned greedily
// in declaration order, so `string`, a copy of `node` and a copy of `this` are declared
// first (s0, s1, s2) and every member access goes through `self`. The copies are assigned
// this-first so the prologue copies them in retail's order.


// Per-player HUD switches: m_hudFlags in JetskiGameOptions.h.
#define HUD_FLAGS(player) (LpGame->m_gameOptions->m_hudFlags[player])
#define ACTION(n) ((AssignableActionIndex)(n))

int JetskiInGameMenu::KeyDownHandler(ProcessNode *node_, KeyInfo *key)
{
    char shorter[11];
    char longer[100];
    char *string;
    ProcessNode *node;          // copy of node_
    JetskiInGameMenu *self;     // copy of this
    int volume;
    int toggle;
    int item;

    self = this;
    node = node_;

    if (node->m_controller != self->m_xB8)
        return 0;
    self->DeSelectText();
    if (KEYDOWN_HANDLER_TEST(ACTION(2), DEVICE_JOYSTICK, node, 0x80000000)) {
        self->m_selection--;
        if (!LpGame->m_competition->m_radar && self->m_selection == 4)
            self->m_selection--;
        if (self->m_selection < 0)
            self->m_selection = self->m_numItems[self->m_x68] - 1;
        if (self->m_x68 == 1 && (self->m_selection == 0 || self->m_selection == 3)) {
            if (LpGame->m_competition->m_x1F0)
                LpGame->m_competition->m_x1F0->SetVolume(LpGame->m_gameOptions->m_musicVolume);
        } else {
            if (LpGame->m_competition->m_x1F0)
                LpGame->m_competition->m_x1F0->SetVolume(0);
        }
    } else if (KEYDOWN_HANDLER_TEST(ACTION(3), DEVICE_JOYSTICK, node, 0x80000000)) {
        self->m_selection++;
        if (!LpGame->m_competition->m_radar && self->m_selection == 4)
            self->m_selection++;
        if (self->m_selection >= self->m_numItems[self->m_x68])
            self->m_selection = 0;
        if (self->m_x68 == 1 && (self->m_selection == 0 || self->m_selection == 3)) {
            if (LpGame->m_competition->m_x1F0)
                LpGame->m_competition->m_x1F0->SetVolume(LpGame->m_gameOptions->m_musicVolume);
        } else {
            if (LpGame->m_competition->m_x1F0)
                LpGame->m_competition->m_x1F0->SetVolume(0);
        }
    } else if (KEYDOWN_HANDLER_TEST(ACTION(4), DEVICE_JOYSTICK, node, 0x80000000)) {
        switch (self->m_x68) {
        case 0:
            switch (self->m_selection) {
            case 0:
                self->m_closing = 1;
                self->m_closeAction = 0;
                break;
            case 1:
                self->m_closing = 1;
                self->m_closeAction = 1;
                break;
            case 5:
                self->m_closing = 1;
                self->m_closeAction = 2;
                break;
            case 6:
                self->m_closing = 1;
                self->m_closeAction = 4;
                break;
            }
            break;
        case 1:
            break;
        }
    } else if (KEYDOWN_HANDLER_TEST(ACTION(0), DEVICE_JOYSTICK, node, 0x80000000)) {
        item = -1;
        volume = 0;
        toggle = 0;
        string = 0;
        switch (self->m_x68) {
        case 1:
            switch (self->m_selection) {
            case 0:
                volume = 1;
                item = 8;
                LpGame->m_competition->m_responses->TriggerResponse(1, 0, 0);
                break;
            case 1:
                volume = 1;
                item = 10;
                LpGame->m_competition->m_responses->TriggerResponse(1, 0, 0);
                break;
            case 2:
                volume = 1;
                item = 12;
                LpGame->m_competition->m_responses->TriggerResponse(1, 0, 0);
                break;
            case 3:
                self->m_timers[1].m_active = 0;
                self->m_timers[0].m_active = 1;
                self->m_timers[0].m_time = 0.5f;
                self->m_track--;
                if (self->m_track < 0)
                    self->m_track = 11;
                self->m_infoTexts[13]->SetString(LpGame->m_competition->m_trackNames[self->m_track]);
                self->m_infoTexts[14]->SetString(LpGame->m_competition->m_trackNames2[self->m_track]);
                self->m_musicChanged = 1;
                LpGame->m_competition->m_responses->TriggerResponse(1, 0, 0);
                break;
            }
            break;
        case 0:
            switch (self->m_selection) {
            case 2:
                toggle = 1;
                item = 9;
                LpGame->m_competition->m_responses->TriggerResponse(1, 0, 0);
                break;
            case 3:
                toggle = 1;
                item = 10;
                LpGame->m_competition->m_responses->TriggerResponse(1, 0, 0);
                break;
            case 4:
                toggle = 1;
                item = 11;
                LpGame->m_competition->m_responses->TriggerResponse(1, 0, 0);
                break;
            }
            break;
        }
        if (item != -1)
            string = self->m_texts[item]->m_text;
        switch (self->m_x68) {
        case 1:
            if (volume) {
                if (strlen(string) > 1) {
#line 1949 "JetskiInGameMenu.cpp"
                    STRCPY(shorter, string);
                    self->m_texts[item]->SetString(shorter + 1);
                    if (item == 8) {
                        LpGame->m_gameOptions->m_musicVolume -= 12;
                        if (LpGame->m_gameOptions->m_musicVolume < 0)
                            LpGame->m_gameOptions->m_musicVolume = 0;
                        LpGame->m_competition->m_x1F0->SetVolume(LpGame->m_gameOptions->m_musicVolume);
                    } else if (item == 10) {
                        LpGame->m_gameOptions->m_x828 -= 12;
                        if (LpGame->m_gameOptions->m_x828 < 0)
                            LpGame->m_gameOptions->m_x828 = 0;
                        LpGame->m_competition->m_scene->m_soundGroup->SetVolume(LpGame->m_gameOptions->m_x828);
                    } else if (item == 12) {
                        LpGame->m_gameOptions->m_x82C -= 12;
                        if (LpGame->m_gameOptions->m_x82C < 0)
                            LpGame->m_gameOptions->m_x82C = 0;
                        LpGame->m_competition->m_x1EC->SetVolume(LpGame->m_gameOptions->m_x82C);
                        LpGame->m_competition->m_x1E8->Play(0, 0);
                    }
                }
            }
            break;
        case 0:
            if (toggle) {
                if (!strcmp(string, LpGame->m_text->GetString(134))) {
                    self->m_texts[item]->SetString(LpGame->m_text->GetString(140));
                    switch (self->m_selection) {
                    case 2:
                        HUD_FLAGS(self->m_xB8) &= ~1;
                        LpGame->m_competition->m_inGameStats->SetShowing(-1, -1);
                        break;
                    case 3:
                        HUD_FLAGS(self->m_xB8) &= ~2;
                        LpGame->m_competition->m_performanceMeter->SetShowing(-1, -1);
                        break;
                    case 4:
                        if (LpGame->m_competition->m_radar) {
                            LpGame->m_gameOptions->m_x858 = 0;
                            LpGame->m_competition->m_radar->SetShowing(0);
                        }
                        break;
                    }
                } else {
                    self->m_texts[item]->SetString(LpGame->m_text->GetString(134));
                    switch (self->m_selection) {
                    case 2:
                        HUD_FLAGS(self->m_xB8) |= 1;
                        LpGame->m_competition->m_inGameStats->SetShowing(-1, -1);
                        break;
                    case 3:
                        HUD_FLAGS(self->m_xB8) |= 2;
                        LpGame->m_competition->m_performanceMeter->SetShowing(-1, -1);
                        break;
                    case 4:
                        if (LpGame->m_competition->m_radar) {
                            LpGame->m_gameOptions->m_x858 = 1;
                            LpGame->m_competition->m_radar->SetShowing(1);
                        }
                        break;
                    }
                }
            }
            break;
        }
    } else if (KEYDOWN_HANDLER_TEST(ACTION(1), DEVICE_JOYSTICK, node, 0x80000000)) {
        item = -1;
        volume = 0;
        toggle = 0;
        string = 0;
        switch (self->m_x68) {
        case 1:
            switch (self->m_selection) {
            case 0:
                volume = 1;
                item = 8;
                LpGame->m_competition->m_responses->TriggerResponse(1, 0, 0);
                break;
            case 1:
                volume = 1;
                item = 10;
                LpGame->m_competition->m_responses->TriggerResponse(1, 0, 0);
                break;
            case 2:
                volume = 1;
                item = 12;
                LpGame->m_competition->m_responses->TriggerResponse(1, 0, 0);
                break;
            case 3:
                self->m_timers[0].m_active = 0;
                self->m_timers[1].m_active = 1;
                self->m_timers[1].m_time = 0.5f;
                self->m_track++;
                if (self->m_track >= 12)
                    self->m_track = 0;
                self->m_infoTexts[13]->SetString(LpGame->m_competition->m_trackNames[self->m_track]);
                self->m_infoTexts[14]->SetString(LpGame->m_competition->m_trackNames2[self->m_track]);
                self->m_musicChanged = 1;
                LpGame->m_competition->m_responses->TriggerResponse(1, 0, 0);
                break;
            }
            break;
        case 0:
            switch (self->m_selection) {
            case 2:
                toggle = 1;
                item = 9;
                LpGame->m_competition->m_responses->TriggerResponse(1, 0, 0);
                break;
            case 3:
                toggle = 1;
                item = 10;
                LpGame->m_competition->m_responses->TriggerResponse(1, 0, 0);
                break;
            case 4:
                toggle = 1;
                item = 11;
                LpGame->m_competition->m_responses->TriggerResponse(1, 0, 0);
                break;
            }
            break;
        }
        if (item != -1)
            string = self->m_texts[item]->m_text;
        switch (self->m_x68) {
        case 1:
            if (volume) {
                if (strlen(string) < 9) {
#line 2099 "JetskiInGameMenu.cpp"
                    STRCPY(longer, string);
                    strcat(longer, "I");
                    self->m_texts[item]->SetString(longer);
                    if (item == 8) {
                        LpGame->m_gameOptions->m_musicVolume += 12;
                        if (LpGame->m_gameOptions->m_musicVolume > 96)
                            LpGame->m_gameOptions->m_musicVolume = 96;
                        LpGame->m_competition->m_x1F0->SetVolume(LpGame->m_gameOptions->m_musicVolume);
                    } else if (item == 10) {
                        LpGame->m_gameOptions->m_x828 += 12;
                        if (LpGame->m_gameOptions->m_x828 > 96)
                            LpGame->m_gameOptions->m_x828 = 96;
                        LpGame->m_competition->m_scene->m_soundGroup->SetVolume(LpGame->m_gameOptions->m_x828);
                    } else if (item == 12) {
                        LpGame->m_gameOptions->m_x82C += 12;
                        if (LpGame->m_gameOptions->m_x82C > 96)
                            LpGame->m_gameOptions->m_x82C = 96;
                        LpGame->m_competition->m_x1EC->SetVolume(LpGame->m_gameOptions->m_x82C);
                        LpGame->m_competition->m_x1E8->Play(0, 0);
                    }
                }
            }
            break;
        case 0:
            if (toggle) {
                if (!strcmp(string, LpGame->m_text->GetString(134))) {
                    self->m_texts[item]->SetString(LpGame->m_text->GetString(140));
                    switch (self->m_selection) {
                    case 2:
                        HUD_FLAGS(self->m_xB8) &= ~1;
                        LpGame->m_competition->m_inGameStats->SetShowing(-1, -1);
                        break;
                    case 3:
                        HUD_FLAGS(self->m_xB8) &= ~2;
                        LpGame->m_competition->m_performanceMeter->SetShowing(-1, -1);
                        break;
                    case 4:
                        if (LpGame->m_competition->m_radar) {
                            LpGame->m_gameOptions->m_x858 = 0;
                            LpGame->m_competition->m_radar->SetShowing(0);
                        }
                        break;
                    }
                } else {
                    self->m_texts[item]->SetString(LpGame->m_text->GetString(134));
                    switch (self->m_selection) {
                    case 2:
                        HUD_FLAGS(self->m_xB8) |= 1;
                        LpGame->m_competition->m_inGameStats->SetShowing(-1, -1);
                        break;
                    case 3:
                        HUD_FLAGS(self->m_xB8) |= 2;
                        LpGame->m_competition->m_performanceMeter->SetShowing(-1, -1);
                        break;
                    case 4:
                        if (LpGame->m_competition->m_radar) {
                            LpGame->m_gameOptions->m_x858 = 1;
                            LpGame->m_competition->m_radar->SetShowing(1);
                        }
                        break;
                    }
                }
            }
            break;
        }
    } else if (KEYDOWN_HANDLER_TEST(ACTION(6), DEVICE_JOYSTICK, node, 0x80000000) && self->m_x68 == 1) {
        self->m_closing = 1;
        self->m_closeAction = 3;
        LpGame->m_competition->m_responses->TriggerResponse(3, 0, 0);
    } else if (KEYDOWN_HANDLER_TEST(ACTION(8), DEVICE_JOYSTICK, node, 0x80000000)) {
        self->DisableMenu();
        LpGame->m_competition->InGamePause(0, self->m_xB8);
        self->m_closing = 0;
        self->m_closeAction = -1;
        if (self->m_x68 == 1) {
            self->m_selection = 0;
            self->m_x68 = 0;
            self->m_texts = self->m_menuTexts;
        }
    }
    if (!self->m_closing)
        self->SelectText();
    self->JetskiMenu::KeyDownHandler(node, key);
    return 1;
}

// ---- src/JetskiInGameMenu/EnableMenu.cpp

void JetskiInGameMenu::EnableMenu()
{
    JetskiCompetition *data = LpGame->m_competition;
    GameText *text;
    int title;
    char *str;
    m_selection = 0;
    if (data->m_x118 == 1)
        title = 125;
    else if (!m_xB8)
        title = 249;
    else
        title = 250;
    text = LpGame->m_text;
    str = text->GetString(title);
    m_menuTexts[7]->SetString(str);
    if (LpGame->m_gameOptions->m_hudFlags[m_xB8] & 1)
        str = LpGame->m_text->GetString(134);
    else
        str = LpGame->m_text->GetString(140);
    m_texts[9]->SetString(str);
    m_textManager->AddText(m_texts[9], m_font);
    if (LpGame->m_gameOptions->m_hudFlags[m_xB8] & 2)
        str = LpGame->m_text->GetString(134);
    else
        str = LpGame->m_text->GetString(140);
    m_texts[10]->SetString(str);
    m_textManager->AddText(m_texts[10], m_font);
    if (LpGame->m_competition->m_radar) {
        if (LpGame->m_gameOptions->m_x858)
            str = LpGame->m_text->GetString(134);
        else
            str = LpGame->m_text->GetString(140);
        m_texts[11]->SetString(str);
        m_textManager->AddText(m_texts[11], m_font);
    }
    LpGame->m_auralScape->StopAllAuralScapeSounds();
    LpGame->m_auralScape->Disable();
    if (data->m_music && data->m_music->m_playing) {
        m_track = data->m_music->m_track - 1;
        m_infoTexts[13]->SetString(data->m_trackNames[m_track]);
        m_infoTexts[14]->SetString(data->m_trackNames2[m_track]);
        if (LpGame->m_competition->m_x1F0)
            LpGame->m_competition->m_x1F0->SetVolume(0);
    }
    EnableAll();
}

// ---- src/JetskiInGameMenu/DisableMenu.cpp

void JetskiInGameMenu::DisableMenu()
{
    m_selection = 0;
    if (m_musicChanged) {
        PSXMusic *music = LpGame->m_competition->m_music;
        if (music->m_playing)
            music->Stop(0.0f);
        LpGame->m_competition->m_music->ShuffleStartingAt(m_track + 1, 1);
        m_musicChanged = 0;
    }
    LpGame->m_competition->Pause(0);
    LpGame->m_auralScape->Enable();
    LpGame->m_competition->ResetAllEngineSounds();
    if (LpGame->m_competition->m_x1F0)
        LpGame->m_competition->m_x1F0->SetVolume(LpGame->m_gameOptions->m_musicVolume);
    DisableAll();
}

// ---- src/JetskiInGameMenu/SetupInfo.cpp

// Lays out the option and info columns in screen pixels, offset per viewport
// (every viewport's offset is 0 in this build, but the switch survives).
void JetskiInGameMenu::SetupInfo()
{
    int x;
    int y;
    int i;
    int justification;
    int hasThird;

    JetskiMenu::SetupInfo();
    x = 0;
    y = 0;
    switch (m_viewport) {
    case 0:
    case 1:
    case 2:
        x = 0;
        y = 0;
        break;
    }
    hasThird = LpGame->m_competition->m_radar ? 1 : 0;
    justification = 0;
    for (i = 0; i < m_numTexts[0]; i++) {
        switch (i) {
        case 0:
            justification = ScreenTextManager::RightJustification;
            m_menuTexts[i]->SetPosition(x + 372, y + 157);
            break;
        case 1:
            justification = ScreenTextManager::RightJustification;
            m_menuTexts[i]->SetPosition(x + 372, y + 177);
            break;
        case 2:
            justification = ScreenTextManager::RightJustification;
            m_menuTexts[i]->SetPosition(x + 372, y + 197);
            break;
        case 9:
            justification = ScreenTextManager::LeftJustification;
            m_menuTexts[i]->SetPosition(x + 390, y + 197);
            break;
        case 3:
            justification = ScreenTextManager::RightJustification;
            m_menuTexts[i]->SetPosition(x + 372, y + 217);
            break;
        case 10:
            justification = ScreenTextManager::LeftJustification;
            m_menuTexts[i]->SetPosition(x + 390, y + 217);
            break;
        case 4:
            justification = ScreenTextManager::RightJustification;
            m_menuTexts[i]->SetPosition(x + 372, y + 237);
            break;
        case 11:
            justification = ScreenTextManager::LeftJustification;
            m_menuTexts[i]->SetPosition(x + 390, y + 237);
            break;
        case 5:
            justification = ScreenTextManager::RightJustification;
            m_menuTexts[i]->SetPosition(x + 372, y + ((hasThird ? 5 : 4) * 20 + 157));
            break;
        case 6:
            justification = ScreenTextManager::RightJustification;
            m_menuTexts[i]->SetPosition(x + 372, y + ((hasThird ? 6 : 5) * 20 + 157));
            break;
        case 8:
            justification = ScreenTextManager::LeftJustification;
            m_menuTexts[i]->SetPosition(x + 382, y + 307);
            break;
        case 7:
            justification = ScreenTextManager::CenterHorizontalJustification;
            m_menuTexts[i]->SetPosition(x + 320, y + 122);
            break;
        }
        if (i == 8)
            m_textManager->PositionText(m_texts[i], justification, m_titleFont);
        else if (i == 7)
            m_textManager->PositionText(m_texts[i], justification, m_headerFont);
        else
            m_textManager->PositionText(m_texts[i], justification, m_font);
    }
    m_texts = m_infoTexts;
    for (i = 0; i < m_numTexts[1]; i++) {
        switch (i) {
        case 0:
            justification = ScreenTextManager::RightJustification;
            m_infoTexts[i]->SetPosition(x + 372, y + 162);
            break;
        case 7:
        case 8:
            justification = ScreenTextManager::LeftJustification;
            m_infoTexts[i]->SetPosition(x + 390, y + 162);
            break;
        case 1:
            justification = ScreenTextManager::RightJustification;
            m_infoTexts[i]->SetPosition(x + 372, y + 182);
            break;
        case 9:
        case 10:
            justification = ScreenTextManager::LeftJustification;
            m_infoTexts[i]->SetPosition(x + 390, y + 182);
            break;
        case 2:
            justification = ScreenTextManager::RightJustification;
            m_infoTexts[i]->SetPosition(x + 372, y + 202);
            break;
        case 11:
        case 12:
            justification = ScreenTextManager::LeftJustification;
            m_infoTexts[i]->SetPosition(x + 390, y + 202);
            break;
        case 3:
            justification = ScreenTextManager::CenterHorizontalJustification;
            m_infoTexts[i]->SetPosition(x + 320, y + 232);
            break;
        case 13:
            justification = ScreenTextManager::CenterHorizontalJustification;
            m_infoTexts[i]->SetPosition(x + 320, y + 252);
            break;
        case 14:
            justification = ScreenTextManager::CenterHorizontalJustification;
            m_infoTexts[i]->SetPosition(x + 320, y + 272);
            break;
        case 5:
            justification = ScreenTextManager::LeftJustification;
            m_infoTexts[i]->SetPosition(x + 382, y + 307);
            break;
        case 6:
            justification = ScreenTextManager::LeftJustification;
            m_infoTexts[i]->SetPosition(x + 268, y + 307);
            break;
        case 4:
            justification = ScreenTextManager::LeftJustification;
            m_infoTexts[i]->SetPosition(x + 137, y + 128);
            break;
        }
        if (i == 5 || i == 6)
            m_textManager->PositionText(m_texts[i], justification, m_titleFont);
        else if (i == 4)
            m_textManager->PositionText(m_texts[i], justification, m_headerFont);
        else
            m_textManager->PositionText(m_texts[i], justification, m_font);
    }
    m_texts = m_menuTexts;
    PostTextPositioning();
}

// ---- src/JetskiInGameMenu/DeSelectText.cpp

// Restore the colors of the selected line and the value shown next to it.
void JetskiInGameMenu::DeSelectText()
{
    int valueType = 0;
    int value = -1;

    switch (m_x68) {
    case 0:
        switch (m_selection) {
        case 2:
            value = 9;
            valueType = 2;
            break;
        case 3:
            valueType = 2;
            value = 10;
            break;
        case 4:
            valueType = 2;
            value = 11;
            break;
        }
        break;
    case 1:
        switch (m_selection) {
        case 0:
            value = 8;
            valueType = 1;
            break;
        case 1:
            value = 10;
            valueType = 1;
            break;
        case 2:
            value = 12;
            valueType = 1;
            break;
        }
        break;
    }
    if (valueType == 1)
        m_texts[value]->SetColor(0xFFD76464);
    else if (valueType == 2)
        m_texts[value]->SetColor(0xFFFFFFFF);
    m_texts[m_selection]->SetColor(0xFFFFFFFF);
}

// ---- src/JetskiInGameMenu/SelectText.cpp

// Highlight the selected line (and the value shown next to it, if any).
void JetskiInGameMenu::SelectText()
{
    int hasValue = 0;
    int value = -1;

    switch (m_x68) {
    case 0:
        switch (m_selection) {
        case 2:
            value = 9;
            hasValue = 1;
            break;
        case 3:
            value = 10;
            hasValue = 1;
            break;
        case 4:
            value = 11;
            hasValue = 1;
            break;
        }
        break;
    case 1:
        switch (m_selection) {
        case 0:
            value = 8;
            hasValue = 1;
            break;
        case 1:
            value = 10;
            hasValue = 1;
            break;
        case 2:
            value = 12;
            hasValue = 1;
            break;
        }
        break;
    }
    if (hasValue) {
        m_texts[value]->SetColor(0xFFFFDE00);
    }
    m_texts[m_selection]->SetColor(0xFFFFDE00);
}

// ---- src/JetskiTrainingInGameMenu/ctor.cpp

JetskiTrainingInGameMenu::JetskiTrainingInGameMenu(int enabled) : JetskiMenu(enabled)
{
    m_textManager = 0;
    m_texts = 0;
    m_menuTexts = 0;
    m_infoTexts = 0;
    m_font = 0;
    m_titleFont = 0;
    m_headerFont = 0;
    m_x68 = 0;
    m_selection = 0;
    m_x118 = 0;
    m_x11C = 0;
    m_numItems[0] = 4;
    m_numItems[1] = m_numTexts[0] = 6;
    m_numTexts[1] = 9;
}

// ---- src/JetskiTrainingInGameMenu/dtor.cpp

JetskiTrainingInGameMenu::~JetskiTrainingInGameMenu()
{
    int i;
    for (i = 0; i < 6; i++) {
        if (m_menuTexts[i])
            delete m_menuTexts[i];
    }
    if (m_menuTexts) {
        free(m_menuTexts);
        m_menuTexts = 0;
    }
    for (i = 0; i < 9; i++) {
        if (m_infoTexts[i])
            delete m_infoTexts[i];
    }
    if (m_infoTexts) {
        free(m_infoTexts);
        m_infoTexts = 0;
    }
}

// ---- src/JetskiTrainingInGameMenu/Create.cpp

// SetPosition's arguments go through block-scoped float copies (as SET_COLOR does for
// colours in the other UI Creates): that gives retail's setter temporaries there.
#define SET_POS(text, x, y)              \
    {                                    \
        float px = x;                    \
        float py = y;                    \
        (text)->SetPosition(px, py);     \
    }

// Where a text is also scaled, the position copies are made before the SetScale call (that
// order sets the setter temporaries' registers as in retail).
#define SET_SCALE_POS(text, scale, x, y) \
    {                                    \
        float px = x;                    \
        float py = y;                    \
        (text)->SetScale(scale);         \
        (text)->SetPosition(px, py);     \
    }

JetskiTrainingInGameMenu *JetskiTrainingInGameMenu::Create(ScreenIconManager *icons, ScreenTextManager *texts,
                                                           ViewPortCode viewport)
{
    int i;

    JetskiMenu::Create(icons, viewport);
    BuildMenuSmall(9);
    ScreenTextFontItem *item = LpGame->m_fonts->GetScreenTextFont("info_c");
    m_font = item ? item->m_font : 0;
    item = LpGame->m_fonts->GetScreenTextFont("info_c");
    m_titleFont = item ? item->m_font : 0;
    item = LpGame->m_fonts->GetScreenTextFont("title_b");
    m_headerFont = item ? item->m_font : 0;
    m_textManager = (new ScreenTextManager(0, 1))->Create(64);
    AddChild(m_textManager, -1);
    m_menuTexts = (ScreenText **)malloc(6 * sizeof(ScreenText *));
    m_infoTexts = (ScreenText **)malloc(9 * sizeof(ScreenText *));
    for (i = 0; i < 6; i++) {
        m_menuTexts[i] = new ScreenText(40, 0);
        switch (i) {
        case 0:
            m_menuTexts[i]->SetString(LpGame->m_text->GetString(128));
            SET_SCALE_POS(m_menuTexts[i], 1.286f, 320.0f, 175.0f)
            m_textManager->PositionText(m_menuTexts[i], ScreenTextManager::CenterHorizontalJustification, m_font);
            break;
        case 1:
            m_menuTexts[i]->SetString(LpGame->m_text->GetString(141));
            SET_SCALE_POS(m_menuTexts[i], 1.286f, 320.0f, 205.0f)
            m_textManager->PositionText(m_menuTexts[i], ScreenTextManager::CenterHorizontalJustification, m_font);
            break;
        case 2:
            m_menuTexts[i]->SetString(LpGame->m_text->GetString(142));
            SET_SCALE_POS(m_menuTexts[i], 1.286f, 320.0f, 235.0f)
            m_textManager->PositionText(m_menuTexts[i], ScreenTextManager::CenterHorizontalJustification, m_font);
            break;
        case 3:
            m_menuTexts[i]->SetString(LpGame->m_text->GetString(143));
            SET_SCALE_POS(m_menuTexts[i], 1.286f, 320.0f, 265.0f)
            m_textManager->PositionText(m_menuTexts[i], ScreenTextManager::CenterHorizontalJustification, m_font);
            break;
        case 4:
            m_menuTexts[i]->SetString(LpGame->m_text->GetString(144));
            SET_POS(m_menuTexts[i], 320.0f, 122.0f)
            m_textManager->PositionText(m_menuTexts[i], ScreenTextManager::CenterHorizontalJustification, m_headerFont);
            break;
        case 5:
            m_menuTexts[i]->SetString(LpGame->m_text->GetString(11));
            SET_POS(m_menuTexts[i], 382.0f, 307.0f)
            m_textManager->PositionText(m_menuTexts[i], ScreenTextManager::LeftJustification, m_titleFont);
            break;
        }
    }
    for (i = 0; i < 9; i++) {
        m_infoTexts[i] = new ScreenText(40, 0);
        switch (i) {
        case 0:
            m_infoTexts[i]->SetString(LpGame->m_text->GetString(165));
            SET_SCALE_POS(m_infoTexts[i], 1.236f, 320.0f, 157.0f)
            m_textManager->PositionText(m_infoTexts[i], ScreenTextManager::CenterHorizontalJustification, m_font);
            break;
        case 1:
            m_infoTexts[i]->SetString(LpGame->m_text->GetString(166));
            SET_SCALE_POS(m_infoTexts[i], 1.236f, 320.0f, 181.0f)
            m_textManager->PositionText(m_infoTexts[i], ScreenTextManager::CenterHorizontalJustification, m_font);
            break;
        case 2:
            m_infoTexts[i]->SetString(LpGame->m_text->GetString(167));
            SET_SCALE_POS(m_infoTexts[i], 1.236f, 320.0f, 205.0f)
            m_textManager->PositionText(m_infoTexts[i], ScreenTextManager::CenterHorizontalJustification, m_font);
            break;
        case 3:
            m_infoTexts[i]->SetString(LpGame->m_text->GetString(168));
            SET_SCALE_POS(m_infoTexts[i], 1.236f, 320.0f, 229.0f)
            m_textManager->PositionText(m_infoTexts[i], ScreenTextManager::CenterHorizontalJustification, m_font);
            break;
        case 4:
            m_infoTexts[i]->SetString(LpGame->m_text->GetString(169));
            SET_SCALE_POS(m_infoTexts[i], 1.236f, 320.0f, 253.0f)
            m_textManager->PositionText(m_infoTexts[i], ScreenTextManager::CenterHorizontalJustification, m_font);
            break;
        case 5:
            m_infoTexts[i]->SetString(LpGame->m_text->GetString(170));
            SET_SCALE_POS(m_infoTexts[i], 1.236f, 320.0f, 277.0f)
            m_textManager->PositionText(m_infoTexts[i], ScreenTextManager::CenterHorizontalJustification, m_font);
            break;
        case 6:
            m_infoTexts[i]->SetString(LpGame->m_text->GetString(144));
            SET_POS(m_infoTexts[i], 320.0f, 122.0f)
            m_textManager->PositionText(m_infoTexts[i], ScreenTextManager::CenterHorizontalJustification, m_headerFont);
            break;
        case 7:
            m_infoTexts[i]->SetString(LpGame->m_text->GetString(12));
            SET_POS(m_infoTexts[i], 268.0f, 307.0f)
            m_textManager->PositionText(m_infoTexts[i], ScreenTextManager::LeftJustification, m_titleFont);
            break;
        case 8:
            m_infoTexts[i]->SetString(LpGame->m_text->GetString(11));
            SET_POS(m_infoTexts[i], 382.0f, 307.0f)
            m_textManager->PositionText(m_infoTexts[i], ScreenTextManager::LeftJustification, m_titleFont);
            break;
        }
    }
    m_texts = m_menuTexts;
    m_x68 = 0;
    SelectText();
    return this;
}

// ---- src/JetskiTrainingInGameMenu/PrepareGeometry.cpp

// Clamp the items' minimum widths to 10, rebuild the info panel if it was
// invalidated, then let the base menu prepare its geometry.
int JetskiTrainingInGameMenu::PrepareGeometry()
{
    if (m_font) {
        TextureMap *texture = m_font->m_texture;
        if (10.0f > texture->m_usage)
            texture->m_usage = 10.0f;
    }
    if (m_titleFont) {  // retail tests m_titleFont but clamps m_font
        TextureMap *texture = m_font->m_texture;
        if (10.0f > texture->m_usage)
            texture->m_usage = 10.0f;
    }
    if (m_headerFont) {  // retail tests m_headerFont but clamps m_font
        TextureMap *texture = m_font->m_texture;
        if (10.0f > texture->m_usage)
            texture->m_usage = 10.0f;
    }
    if (m_infoDirty) {
        SetupInfo();
        m_infoDirty = 0;
    }
    return JetskiMenu::PrepareGeometry();
}

// ---- src/JetskiTrainingInGameMenu/Render3D.cpp
// GameObject per the retail vtable (Render3D/PrepareGeometry return int here).

// Draw once per frame, on the last player's pass.
int JetskiTrainingInGameMenu::Render3D()
{
    if (LpGame->m_viewport == LpGame->m_numCameras - 1) {
        if (WaitForRenderingToFinish && m_viewport) {
            LpRenderTarget->TrySendRenderQueue(1);
            LpRenderTarget->WaitForRenderingToComplete();
        }
        SetViewport();
        JetskiMenu::Render3D();
        int i;
        switch (m_x68) {
        case 0:
            for (i = 0; i < 4; i++)
                m_textManager->AddText(m_texts[i], m_font);
            m_textManager->AddText(m_texts[4], m_headerFont);
            m_textManager->AddText(m_texts[5], m_titleFont);
            break;
        case 1:
            for (i = 0; i < 6; i++)
                m_textManager->AddText(m_texts[i], m_font);
            m_textManager->AddText(m_texts[6], m_headerFont);
            m_textManager->AddText(m_texts[7], m_titleFont);
            m_textManager->AddText(m_texts[8], m_titleFont);
            break;
        }
        GameObject::Render3D();
        RestoreViewport();
    }
    return 1;
}

// ---- src/JetskiTrainingInGameMenu/Tick.cpp

// Only the slot used here is named; the vptr sits at +0x8.

int JetskiTrainingInGameMenu::Tick(float dt)
{
    int i;
    int busy = 0;
    if (m_closing) {
        for (i = 0; i < 7; i++) {
            if (m_timers[i].m_active)
                busy = 1;
        }
        if (!busy) {
            switch (m_closeAction) {
            case -2:
                DisableMenu();
                ((JetskiTraining *)LpGame->m_competition)->InGamePause(0, m_xB8);
                break;
            case -4:
                m_selection = 0;
                m_x68 = 1;
                m_texts = m_infoTexts;
                DeSelectText();
                SelectText();
                break;
            case -3:
                m_selection = 2;
                m_x68 = 0;
                m_texts = m_menuTexts;
                DeSelectText();
                SelectText();
                break;
            case 0:
                DisableMenu();
                ((JetskiTraining *)LpGame->m_competition)->m_status = 5;
                LpGame->m_paused = 0;
                ((JetskiTraining *)LpGame->m_competition)->ResetTest(0, 1);
                break;
            case -1:
                DisableMenu();
                ((JetskiTraining *)LpGame->m_competition)->EventComplete();
                break;
            case 2:
                DisableMenu();
                ((JetskiTraining *)LpGame->m_competition)->m_status = 5;
                LpGame->m_paused = 0;
                ((JetskiTraining *)LpGame->m_competition)->ResetTest(2, 1);
                break;
            case 3:
                DisableMenu();
                ((JetskiTraining *)LpGame->m_competition)->m_status = 5;
                LpGame->m_paused = 0;
                ((JetskiTraining *)LpGame->m_competition)->ResetTest(3, 1);
                break;
            case 4:
                DisableMenu();
                ((JetskiTraining *)LpGame->m_competition)->m_status = 5;
                LpGame->m_paused = 0;
                ((JetskiTraining *)LpGame->m_competition)->ResetTest(4, 1);
                break;
            case 5:
                DisableMenu();
                ((JetskiTraining *)LpGame->m_competition)->m_status = 5;
                LpGame->m_paused = 0;
                ((JetskiTraining *)LpGame->m_competition)->ResetTest(5, 1);
                break;
            case 7:
                DisableMenu();
                ((JetskiTraining *)LpGame->m_competition)->m_status = 5;
                LpGame->m_paused = 0;
                ((JetskiTraining *)LpGame->m_competition)->ResetTest(7, 1);
                break;
            case 1:
            case 6:
                break;
            }
            m_closing = 0;
            m_closeAction = -5;
        }
    }
    return JetskiMenu::Tick(dt);
}

// ---- src/JetskiTrainingInGameMenu/KeyDownHandler.cpp

#define ACTION(n) ((AssignableActionIndex)(n))

int JetskiTrainingInGameMenu::KeyDownHandler(ProcessNode *node, KeyInfo *key)
{
    if (node->m_controller != m_xB8)
        return 0;
    DeSelectText();
    if (KEYDOWN_HANDLER_TEST(ACTION(2), DEVICE_JOYSTICK, node, 0x80000000)) {
        m_selection--;
        if (m_selection < 0)
            m_selection = m_numItems[m_x68] - 1;
        if ((((JetskiTraining *)LpGame->m_competition)->m_section == 3 || ((JetskiTraining *)LpGame->m_competition)->m_section == 4) && !m_x68 && !m_selection)
            m_selection = 3;
    } else if (KEYDOWN_HANDLER_TEST(ACTION(3), DEVICE_JOYSTICK, node, 0x80000000)) {
        m_selection++;
        if (m_selection >= m_numItems[m_x68])
            m_selection = 0;
        if ((((JetskiTraining *)LpGame->m_competition)->m_section == 3 || ((JetskiTraining *)LpGame->m_competition)->m_section == 4) && !m_x68 && !m_selection)
            m_selection++;
    } else if (KEYDOWN_HANDLER_TEST(ACTION(4), DEVICE_JOYSTICK, node, 0x80000000)) {
        switch (m_x68) {
        case 0:
            switch (m_selection) {
            case 0:
                m_closing = 1;
                m_closeAction = -2;
                break;
            case 1:
                m_closing = 1;
                m_closeAction = 0;
                ((JetskiTraining *)LpGame->m_competition)->m_trainingComplete = 1;
                break;
            case 2:
                m_closing = 1;
                m_closeAction = -4;
                break;
            case 3:
                m_closing = 1;
                m_closeAction = -1;
                break;
            }
            break;
        case 1:
            switch (m_selection) {
            case 0:
                m_closing = 1;
                m_closeAction = 0;
                ((JetskiTraining *)LpGame->m_competition)->m_trainingComplete = 0;
                break;
            case 1:
                m_closing = 1;
                m_closeAction = 2;
                ((JetskiTraining *)LpGame->m_competition)->m_trainingComplete = 0;
                break;
            case 2:
                m_closing = 1;
                m_closeAction = 3;
                ((JetskiTraining *)LpGame->m_competition)->m_trainingComplete = 0;
                break;
            case 3:
                m_closing = 1;
                m_closeAction = 4;
                ((JetskiTraining *)LpGame->m_competition)->m_trainingComplete = 0;
                break;
            case 4:
                m_closing = 1;
                m_closeAction = 5;
                ((JetskiTraining *)LpGame->m_competition)->m_trainingComplete = 0;
                break;
            case 5:
                m_closing = 1;
                m_closeAction = 7;
                ((JetskiTraining *)LpGame->m_competition)->m_trainingComplete = 0;
                break;
            }
            break;
        }
    } else if (KEYDOWN_HANDLER_TEST(ACTION(6), DEVICE_JOYSTICK, node, 0x80000000) && m_x68 == 1) {
        LpGame->m_competition->m_responses->TriggerResponse(3, 0, 0);
        m_closing = 1;
        m_closeAction = -3;
    } else if (KEYDOWN_HANDLER_TEST(ACTION(8), DEVICE_JOYSTICK, node, 0x80000000)) {
        DisableMenu();
        LpGame->m_competition->InGamePause(0, m_xB8);
        m_closing = 0;
        m_closeAction = -5;
        if (m_x68 == 1) {
            m_selection = 0;
            m_x68 = 0;
            m_texts = m_menuTexts;
        }
    }
    if (!m_closing)
        SelectText();
    JetskiMenu::KeyDownHandler(node, key);
    return 1;
}

// ---- src/JetskiTrainingInGameMenu/EnableMenu.cpp

void JetskiTrainingInGameMenu::EnableMenu()
{
    switch (m_x68) {
    case 0:
        m_texts = m_menuTexts;
        break;
    case 1:
        m_texts = m_infoTexts;
        break;
    }
    if (((JetskiTraining *)LpGame->m_competition)->m_section == 2 || ((JetskiTraining *)LpGame->m_competition)->m_section == 1) {
        m_texts = m_menuTexts;
        m_x68 = 0;
        m_selection = 0;
    }
    if (LpGame->m_competition->m_radar)
        LpGame->m_competition->m_radar->SetShowing(0);
    JetskiTraining *competition = (JetskiTraining *)LpGame->m_competition;
    if (competition->m_section == 1) {
        JetskiDialogueStreamer *dialog = competition->m_dialogue;
        if (dialog->m_playing)
            dialog->Pause(1);
    }
    LpGame->m_auralScape->StopAllAuralScapeSounds();
    LpGame->m_auralScape->Disable();
    DeSelectText();
    SelectText();
    EnableAll();
}

// ---- src/JetskiTrainingInGameMenu/DisableMenu.cpp

void JetskiTrainingInGameMenu::DisableMenu()
{
    m_selection = 0;
    LpGame->m_competition->Pause(0);
    JetskiTraining *competition = (JetskiTraining *)LpGame->m_competition;
    if (competition->m_section == 1) {
        JetskiDialogueStreamer *dialog = competition->m_dialogue;
        if (dialog->m_playing)
            dialog->Pause(0);
    }
    if (m_x68 == 0)
        m_texts = m_menuTexts;
    else if (m_x68 == 1)
        m_texts = m_infoTexts;
    LpGame->m_auralScape->Enable();
    LpGame->m_competition->ResetAllEngineSounds();
    DisableAll();
}

// ---- src/JetskiTrainingInGameMenu/SetupInfo.cpp

void JetskiTrainingInGameMenu::SetupInfo()
{
    JetskiMenu::SetupInfo();
}

// ---- src/JetskiTrainingInGameMenu/DeSelectText.cpp

void JetskiTrainingInGameMenu::DeSelectText()
{
    int i;
    switch (m_x68) {
    case 0:
        for (i = 0; i < 4; i++) {
            ScreenText *text = m_texts[i];
            text->m_color = 0xFFFFFFFF;
            text->m_x5D_bit4 = 1;
        }
        if (((JetskiTraining *)LpGame->m_competition)->m_section == 3 || ((JetskiTraining *)LpGame->m_competition)->m_section == 4) {
            ScreenText *text = m_texts[0];
            text->m_color = 0xFF646464;     // greyed out
            text->m_x5D_bit4 = 1;
        }
        break;
    case 1:
        for (i = 0; i < 6; i++) {
            ScreenText *text = m_texts[i];
            text->m_color = 0xFFFFFFFF;
            text->m_x5D_bit4 = 1;
        }
        break;
    }
}

// ---- src/JetskiTrainingInGameMenu/SelectText.cpp

void JetskiTrainingInGameMenu::SelectText()
{
    ScreenText *item = m_texts[m_selection];
    item->m_color = 0xFFFFDE00;
    item->m_x5D_bit4 = 1;
}

// ---- src/JetskiNameMenu/ctor.cpp

JetskiNameMenu::JetskiNameMenu(int enabled) : JetskiMenu(enabled)
{
    for (int i = 0; i < 2; i++) {
        (&m_text114)[i] = 0;
        (&m_text120)[i] = 0;
        m_recordCounts[i] = 0;
    }
    m_textManager = 0;
    m_letters = 0;
    m_texts = 0;
    m_nameTexts = 0;
    m_cursor = 0;
    m_selection = 1;
    m_column = 1;
    m_font = 0;
    m_titleFont = 0;
    m_smallFont = 0;
    m_cursorOn = 0;
    m_mode = 1;
    m_player = 0;
}

// ---- src/JetskiNameMenu/dtor.cpp

JetskiNameMenu::~JetskiNameMenu()
{
    int i;
    for (i = 0; i < 78; i++)
        delete m_letters[i];
    free(m_letters);
    for (i = 0; i < 2; i++)
        delete m_nameTexts[i];
    free(m_nameTexts);
    for (i = 0; i < 6; i++)
        delete m_texts[i];
    free(m_texts);
    delete m_text114;
    delete m_text118;
    delete m_cursor;
    delete m_text120;
    delete m_text124;
}

// ---- src/JetskiNameMenu/Create.cpp

// Colours go through a block-local copy: in the record-line switches that is
// what loads the colour before the m_texts[i] reload, as retail does.
#define SET_COLOR(text, value)          \
    {                                   \
        unsigned int color = value;     \
        (text)->SetColor(color);        \
    }

// Creates the name menu's fonts and texts: the two name lines, the six
// record lines, the prompts, the keyboard and the cursor.
JetskiNameMenu *JetskiNameMenu::Create(ScreenIconManager *icons, ScreenTextManager *texts, ViewPortCode viewport)
{
    int i;
    JetskiMenu::Create(icons, viewport);
    BuildMenuFull(8);
    BuildMenuSmall(8);
    ScreenTextFontItem *item = LpGame->m_fonts->GetScreenTextFont("info_b");
    m_font = item ? item->m_font : 0;
    item = LpGame->m_fonts->GetScreenTextFont("title_b");
    m_titleFont = item ? item->m_font : 0;
    item = LpGame->m_fonts->GetScreenTextFont("info_c");
    m_smallFont = item ? item->m_font : 0;
    m_textManager = (new ScreenTextManager(0, 1))->Create(83);
    AddChild(m_textManager, -1);
    m_letters = (ScreenText **)malloc(78 * sizeof(ScreenText *));
    m_nameTexts = (ScreenText **)malloc(2 * sizeof(ScreenText *));
    m_texts = (ScreenText **)malloc(6 * sizeof(ScreenText *));
    for (i = 0; i < 2; i++) {
        m_nameTexts[i] = new ScreenText(40, 0);
        m_nameTexts[i]->SetScale(1.286f);
        switch (i) {
        case 0:
            m_nameTexts[i]->SetString(LpGame->m_text->GetString(145));
            SET_COLOR(m_nameTexts[i], 0xFFFFFFFF)
            break;
        case 1:
            m_nameTexts[i]->SetString("");
            SET_COLOR(m_nameTexts[i], 0xFFFFFFFF)
            break;
        }
    }
    for (i = 0; i < 6; i++) {
        m_texts[i] = new ScreenText(40, 0);
        m_texts[i]->SetScale(1.059f);
        switch (i) {
        case 0:
            m_texts[i]->SetString("");
            SET_COLOR(m_texts[i], 0xFFFFFFFF)
            break;
        case 1:
            m_texts[i]->SetString("");
            SET_COLOR(m_texts[i], 0xFFFFFFFF)
            break;
        case 2:
            m_texts[i]->SetString("");
            SET_COLOR(m_texts[i], 0xFFFFFFFF)
            break;
        case 3:
            m_texts[i]->SetString("");
            SET_COLOR(m_texts[i], 0xFFFFDE00)
            break;
        case 4:
            m_texts[i]->SetString("");
            SET_COLOR(m_texts[i], 0xFFFFDE00)
            break;
        case 5:
            m_texts[i]->SetString("");
            SET_COLOR(m_texts[i], 0xFFFFFFFF)
            break;
        }
    }
    m_text114 = new ScreenText(40, 0);
    m_text114->SetScale(1.0f);
    m_text114->SetString(LpGame->m_text->GetString(146));
    SET_COLOR(m_text114, 0xFF000000)
    m_text118 = new ScreenText(40, 0);
    m_text118->SetScale(1.0f);
    m_text118->SetString(LpGame->m_text->GetString(147));
    SET_COLOR(m_text118, 0xFFFFFFFF)
    SetupKeyBoard();
    m_cursor = new ScreenText(40, 0);
    m_cursor->SetString("_");
    SET_COLOR(m_cursor, 0xFFFFDE00)
    m_text120 = new ScreenText(40, 0);
    m_text120->SetScale(1.059f);
    m_text120->SetString(LpGame->m_text->GetString(148));
    SET_COLOR(m_text120, 0xFF000000)
    m_text124 = new ScreenText(40, 0);
    m_text124->SetScale(1.059f);
    m_text124->SetString(LpGame->m_text->GetString(149));
    SET_COLOR(m_text124, 0xFFFFFFFF)
    return this;
}

// ---- src/JetskiNameMenu/Initialize.cpp

void JetskiNameMenu::Initialize(int mode)
{
    JetskiGameOptions *options;
    JetskiCompetition *competition;
    competition = LpGame->m_competition;
    options = LpGame->m_gameOptions;
    if (competition->m_radar)
        competition->m_radar->SetShowing(0);
    competition->m_inGameStats->SetShowing(0, 0);
    competition->m_performanceMeter->SetShowing(0, 0);
    EnableAll();
    m_mode = mode;
    if (!m_player)
        m_player = 1;
    switch (m_mode) {
    case 0:
        if (competition->CheckForNewHighScores(m_player)) {
            options->m_careerMode = 1;
            InitializeListRecords();
        } else {
            DisableMenu();
        }
        break;
    case 1:
        InitializeTypeWriter();
        break;
    }
}

// ---- src/JetskiNameMenu/InitializeListRecords.cpp

void JetskiNameMenu::InitializeListRecords()
{
    char title[0x20];
    int id;

    if (m_player == 1)
        id = 0xF9;
    else
        id = 0xFA;
    sprintf(title, "%s:", LpGame->m_text->GetString(id));
    m_texts[0]->SetString(title);
    m_texts[5]->SetString(LpGame->m_text->GetString(0x80));
    m_x60 = 1;
    m_x48 = 0;
    m_infoDirty = 1;
    BuildRecordLists();
}

// ---- src/JetskiNameMenu/SetSelectionYellow.cpp

void JetskiNameMenu::SetSelectionYellow()
{
    ScreenText *text = m_letters[m_selection - 1];
    text->m_scale = 1.4f;
    text->m_dirty = 1;
    text->m_positionDirty = 1;
    text = m_letters[m_selection - 1];
    text->m_color = 0xFFFFDE00;
    text->m_x5D_bit4 = 1;
}

// ---- src/JetskiNameMenu/SetSelectionWhite.cpp

void JetskiNameMenu::SetSelectionWhite()
{
    ScreenText *text = m_letters[m_selection - 1];
    text->m_scale = 1.286f;
    text->m_dirty = 1;
    text->m_positionDirty = 1;
    text = m_letters[m_selection - 1];
    text->m_color = 0xFFFFFFFF;
    text->m_x5D_bit4 = 1;
}

// ---- src/JetskiNameMenu/InitializeTypeWriter.cpp

static inline void SetText(ScreenText *text, char *string)
{
    text->m_positionDirty = 1;
    strncpy(text->m_text, string, text->m_maxChars);
    text->m_text[text->m_maxChars] = 0;
    text->m_x5D_bit4 = 1;
    text->m_dirty = 1;
    text->m_cellsDirty = 1;
}

void JetskiNameMenu::InitializeTypeWriter()
{
    SetSelectionWhite();
    m_x60 = 0;
    m_x48 = 1;
    m_infoDirty = 1;
    m_column = 1;
    m_cursorTimer = 0;
    m_cursorOn = 1;
    JetskiGameOptions *options = LpGame->m_gameOptions;
    switch (options->m_raceType) {
    case 6:
    case 7:
        SetText(m_nameTexts[1], options->m_team->m_name);
        m_selection = 0x4E;
        break;
    default:
        SetText(m_nameTexts[1], "");
        m_selection = 1;
        break;
    }
    SetSelectionYellow();
}

// ---- src/JetskiNameMenu/KeyDownHandler.cpp

int JetskiNameMenu::KeyDownHandler(ProcessNode *node, KeyInfo *key)
{
    if (node->m_controller == m_player - 1) {
        JetskiMenu::KeyDownHandler(node, key);
        switch (m_mode) {
        case 0:
            KeyListRecords(node, key);
            break;
        case 1:
            KeyTypeWriter(node, key);
            break;
        }
    }
    return 1;
}

// ---- src/JetskiNameMenu/KeyListRecords.cpp

void JetskiNameMenu::KeyListRecords(ProcessNode *node, KeyInfo *key)
{
    if (KEYDOWN_HANDLER_TEST((AssignableActionIndex)4, DEVICE_JOYSTICK, node, 0x80000000))
        m_closing = 1;
}

// ---- src/JetskiNameMenu/KeyTypeWriter.cpp

#define KEY_UP      (AssignableActionIndex)3
#define KEY_DOWN    (AssignableActionIndex)2
#define KEY_LEFT    (AssignableActionIndex)0
#define KEY_RIGHT   (AssignableActionIndex)1
#define KEY_SELECT  (AssignableActionIndex)4

// Moves the typewriter cursor. Keys 70, 74 and 78 are the bottom row's
// space/back/end keys.
void JetskiNameMenu::KeyTypeWriter(ProcessNode *node, KeyInfo *key)
{
    asm("");    // retail compiled this without the global optimizer (no CSE, unfilled slots)
    if (node->m_controller == m_player - 1) {
        SetSelectionWhite();
        if (KEYDOWN_HANDLER_TEST(KEY_UP, (DeviceType)2, node, 0x80000000)) {
            LpGame->m_competition->m_responses->TriggerResponse(1, 0, 0);
            if (m_selection == 0x4E || m_selection == 0x4A || m_selection == 0x46) {
                m_selection = m_column;
            } else if (m_selection >= 0x35 && m_selection < 0x42) {
                m_selection = 0x4E;
            } else {
                m_selection += 13;
                if (m_selection > 0x4E)
                    m_selection -= 0x4E;
            }
        } else if (KEYDOWN_HANDLER_TEST(KEY_DOWN, (DeviceType)2, node, 0x80000000)) {
            LpGame->m_competition->m_responses->TriggerResponse(1, 0, 0);
            if (m_selection == 0x4E || m_selection == 0x4A || m_selection == 0x46) {
                m_selection = m_column + 0x34;
            } else if (m_selection > 0 && m_selection < 0xE) {
                m_selection = 0x4E;
            } else {
                m_selection -= 13;
                if (m_selection <= 0)
                    m_selection += 0x4E;
            }
        } else if (KEYDOWN_HANDLER_TEST(KEY_LEFT, (DeviceType)2, node, 0x80000000)) {
            LpGame->m_competition->m_responses->TriggerResponse(1, 0, 0);
            switch (m_selection) {
            case 0x4E:
                m_selection = 0x4A;
                break;
            case 0x4A:
                m_selection = 0x46;
                break;
            case 0x46:
                m_selection = 0x4E;
                break;
            default:
                m_column--;
                if (m_column <= 0)
                    m_column = 13;
                m_selection--;
                if (m_selection % 13 == 0)
                    m_selection += 13;
                break;
            }
        } else if (KEYDOWN_HANDLER_TEST(KEY_RIGHT, (DeviceType)2, node, 0x80000000)) {
            LpGame->m_competition->m_responses->TriggerResponse(1, 0, 0);
            switch (m_selection) {
            case 0x4E:
                m_selection = 0x46;
                break;
            case 0x4A:
                m_selection = 0x4E;
                break;
            case 0x46:
                m_selection = 0x4A;
                break;
            default:
                m_column++;
                if (m_column > 13)
                    m_column = 1;
                m_selection++;
                if (m_selection % 13 == 1)
                    m_selection -= 13;
                break;
            }
        } else if (KEYDOWN_HANDLER_TEST(KEY_SELECT, (DeviceType)2, node, 0x80000000)) {
            ProcessLetter();
        }
        SetSelectionYellow();
    }
}

// ---- src/JetskiNameMenu/PrepareGeometry.cpp

// Raise the fonts' texture usage to at least 10, rebuild the info panel if it
// was invalidated, then let the base menu prepare its geometry.
int JetskiNameMenu::PrepareGeometry()
{
    if (m_font) {
        TextureMap *texture = m_font->m_texture;
        if (10.0f > texture->m_usage)
            texture->m_usage = 10.0f;
    }
    if (m_titleFont) {
        TextureMap *texture = m_titleFont->m_texture;
        if (10.0f > texture->m_usage)
            texture->m_usage = 10.0f;
    }
    if (m_font) {  // retail tests m_font but raises m_smallFont's
        TextureMap *texture = m_smallFont->m_texture;
        if (10.0f > texture->m_usage)
            texture->m_usage = 10.0f;
    }
    if (m_infoDirty) {
        SetupInfo();
        m_infoDirty = 0;
    }
    return JetskiMenu::PrepareGeometry();
}

// ---- src/JetskiNameMenu/SetupInfo.cpp

void JetskiNameMenu::SetupInfo()
{
    int x;
    int y;
    int i;
    JetskiMenu::SetupInfo();
    switch (m_viewport) {
    case 0:
    case 1:
    case 2:
        x = 0;
        y = 0;
        break;
    }
    for (i = 0; i < 2; i++) {
        switch (i) {
        case 0:
            m_nameTexts[i]->SetPosition(x + 0xE6, y + 0x69);
            m_textManager->PositionText(m_nameTexts[i], ScreenTextManager::RightJustification, m_font);
            break;
        case 1:
            m_nameTexts[i]->SetPosition(x + 0xFF, y + 0x69);
            m_nameTexts[i]->SetJustification(ScreenTextManager::LeftJustification);
            m_textManager->PositionText(m_nameTexts[i], ScreenTextManager::LeftJustification, m_font);
            break;
        }
    }
    for (i = 0; i < 6; i++) {
        switch (i) {
        case 0:
            m_texts[i]->SetPosition(x + 0x140, y + 0xAF);
            m_textManager->PositionText(m_texts[i], ScreenTextManager::CenterHorizontalJustification, m_font);
            break;
        case 1:
            m_texts[i]->SetPosition(x + 0x140, y + 0xCD);
            m_textManager->PositionText(m_texts[i], ScreenTextManager::CenterHorizontalJustification, m_font);
            break;
        case 2:
            m_texts[i]->SetPosition(x + 0x140, y + 0xF5);
            m_textManager->PositionText(m_texts[i], ScreenTextManager::CenterHorizontalJustification, m_font);
            break;
        case 3:
            m_texts[i]->SetPosition(x + 0x140, y + 0xE1);
            m_textManager->PositionText(m_texts[i], ScreenTextManager::CenterHorizontalJustification, m_font);
            break;
        case 4:
            m_texts[i]->SetPosition(x + 0x140, y + 0x109);
            m_textManager->PositionText(m_texts[i], ScreenTextManager::CenterHorizontalJustification, m_font);
            break;
        case 5:
            m_texts[i]->SetPosition(x + 0x17E, y + 0x12F);
            m_textManager->PositionText(m_texts[i], ScreenTextManager::LeftJustification, m_smallFont);
            break;
        }
    }
    m_text114->SetPosition(x + 0x140, y + 0x7A);
    m_textManager->PositionText(m_text114, ScreenTextManager::CenterHorizontalJustification, m_titleFont);
    m_text118->SetPosition(x + 0x13D, y + 0x77);
    m_textManager->PositionText(m_text118, ScreenTextManager::CenterHorizontalJustification, m_titleFont);
    m_text120->SetPosition(x + 0x140, y + 0x27);
    m_textManager->PositionText(m_text120, ScreenTextManager::CenterHorizontalJustification, m_titleFont);
    m_text124->SetPosition(x + 0x13D, y + 0x24);
    m_textManager->PositionText(m_text124, ScreenTextManager::CenterHorizontalJustification, m_titleFont);
    // The 78 typewriter keys: 13 columns, 6 rows; keys 70, 74 and 78 are
    // right-justified.
    for (i = 0; i < 78; i++) {
        m_letters[i]->SetPosition(x + (i % 13 * 35 + 0x6E), y + (i / 13 * 30 + 0x8C));
        int justification = ScreenTextManager::CenterHorizontalJustification;
        if (i + 1 == 78 || i + 1 == 74 || i + 1 == 70)
            justification = ScreenTextManager::RightJustification;
        m_textManager->PositionText(m_letters[i], justification, m_font);
    }
}

// ---- src/JetskiNameMenu/Render3D.cpp

// Draw once per frame, on the last player's pass.
int JetskiNameMenu::Render3D()
{
    if (LpGame->m_viewport == LpGame->m_numCameras - 1) {
        if (WaitForRenderingToFinish && m_viewport) {
            LpRenderTarget->TrySendRenderQueue(1);
            LpRenderTarget->WaitForRenderingToComplete();
        }
        SetViewport();
        JetskiMenu::Render3D();
        switch (m_mode) {
        case 0:
            RenderListRecords();
            break;
        case 1:
            RenderTypeWriter();
            break;
        }
        GameObject::Render3D();
        RestoreViewport();
    }
    return 1;
}

// ---- src/JetskiNameMenu/RenderListRecords.cpp

void JetskiNameMenu::RenderListRecords()
{
    m_textManager->AddText(m_text114, m_titleFont);
    m_textManager->AddText(m_text118, m_titleFont);
    m_textManager->AddText(m_texts[0], m_font);
    int i;
    for (i = 0; i < m_recordCounts[m_player - 1]; i++)
        m_textManager->AddText(m_texts[1 + i], m_font);
    for (i = 0; i < m_recordCounts[m_player - 1]; i++)
        m_textManager->AddText(m_texts[3 + i], m_font);
    m_textManager->AddText(m_texts[5], m_smallFont);
}

// ---- src/JetskiNameMenu/RenderTypeWriter.cpp

// Draws the typed name, the on-screen keyboard and a blinking cursor after the
// name while it is shorter than 8 characters.
#pragma fast_fptosi on
void JetskiNameMenu::RenderTypeWriter()
{
    m_textManager->AddText(m_nameTexts[0], m_font);
    m_textManager->AddText(m_nameTexts[1], m_font);
    for (int i = 0; i < 78; i++)
        m_textManager->AddText(m_letters[i], m_font);
    if (m_cursorOn && (int)strlen(m_nameTexts[1]->m_text) < 8) {
        int width = m_textManager->GetStringWidthInPixels(m_nameTexts[1], m_font, 1.0f);
        float x = m_nameTexts[1]->m_position.x;
        float y = m_nameTexts[1]->m_position.y;
        float scale = m_nameTexts[1]->m_scale;
        m_cursor->SetScale(scale);
        m_cursor->SetPosition(x + (int)(width * scale), y);
        m_cursor->SetFont(m_font);
        m_textManager->PositionText(m_cursor);
        m_textManager->AddText(m_cursor, m_font);
    }
    m_textManager->AddText(m_text120, m_titleFont);
    m_textManager->AddText(m_text124, m_titleFont);
}
#pragma fast_fptosi reset

// ---- src/JetskiNameMenu/Tick.cpp

int JetskiNameMenu::Tick(float dt)
{
    switch (m_mode) {
    case 0:
        TickListRecords(dt);
        break;
    case 1:
        TickTypeWriter(dt);
        break;
    }
    return JetskiMenu::Tick(dt);
}

// ---- src/JetskiNameMenu/TickListRecords.cpp

void JetskiNameMenu::TickListRecords(float)
{
    int busy = 0;
    if (m_closing) {
        for (int i = 0; i < 7; i++) {
            if (m_timers[i].m_active)
                busy = 1;
        }
        if (!busy) {
            m_mode = 1;
            InitializeTypeWriter();
            m_closing = 0;
        }
    }
}

// ---- src/JetskiNameMenu/TickTypeWriter.cpp

void JetskiNameMenu::TickTypeWriter(float dt)
{
    m_cursorTimer += dt;
    if (m_cursorTimer >= 0.5f) {
        m_cursorTimer -= 0.5f;
        m_cursorOn = !m_cursorOn;
    }
}

// ---- src/JetskiNameMenu/SetupKeyBoard.cpp

// The typewriter: letters, punctuation, digits and the space/back/end keys.
void JetskiNameMenu::SetupKeyBoard()
{
    char *space = LpGame->m_text->GetString(150);
    char *back = LpGame->m_text->GetString(151);
    char *end = LpGame->m_text->GetString(152);
    char *keys[78] = {
        "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M",
        "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z",
        "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m",
        "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z",
        "!", ".", "#", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0",
        "", "", "", "", space,
        "", "", "", back,
        "", "", "", end,
    };
    for (int i = 0; i < 78; i++) {
        m_letters[i] = new ScreenText(0x28, 0);
        m_letters[i]->Create(strlen(keys[i]) + 2, keys[i]);
        m_letters[i]->SetX(1.286f);
        m_letters[i]->SetColor(0xFFFFFFFF);
    }
    m_selection = 1;
    SetSelectionYellow();
}

// ---- src/JetskiNameMenu/ProcessLetter.cpp

// Apply the selected on-screen key to the name being typed: a letter,
// space (0x46), backspace (0x4A) or done (0x4E). A full name (8 letters)
// moves the selection to done.
void JetskiNameMenu::ProcessLetter()
{
    char name[9];
    int len;

#line 4102 "JetskiInGameMenu.cpp"
    STRCPY(name, m_nameTexts[1]->m_text);
    len = strlen(name);
    switch (m_selection) {
    case 0x46:
        if (len >= 8) {
            SetSelectionWhite();
            m_selection = 0x4E;
            SetSelectionYellow();
            return;
        }
        strcat(name, " ");
        break;
    case 0x4A:
        if (len > 0)
            name[len - 1] = 0;
        break;
    case 0x4E:
        ReplaceName();
        DisableMenu();
        return;
    default:
        if (len >= 8) {
            SetSelectionWhite();
            m_selection = 0x4E;
            SetSelectionYellow();
            return;
        }
        strcat(name, m_letters[m_selection - 1]->m_text);
        break;
    }
    m_nameTexts[1]->SetString(name);
    m_cursorOn = 1;
    m_cursorTimer = 0.0f;
}

// ---- src/JetskiNameMenu/ReplaceName.cpp

// JetskiGameOptions::m_hallOfFame as the record lists index it: the six
// names of a track's entry with their times (or points), in pairs per track.
struct NameList {
    char m_names[6][9];                 // +0x00
    char pad36[0x38 - 0x36];
    float m_times[6];                   // +0x38
};                                      // 0x50 bytes

struct NameListPair {
    NameList m_first;                   // +0x00
    NameList m_second;                  // +0x50
};                                      // 0xA0 bytes

struct ScoreList {
    char m_names[6][9];                 // +0x00
    char pad36[0x38 - 0x36];
    int m_scores[6];                    // +0x38
};

struct TimeScoreListPair {
    NameList m_times;                   // +0x00
    ScoreList m_scores;                 // +0x50
};

// A view of the whole JetskiGameOptions: indexing m_hallOfFame through a
// pointer to it (not through &options->m_hallOfFame) gives retail's code.
struct HallOfFameRecords {
    char pad0[0x8A8];
    NameListPair m_records[2][21];      // +0x8A8, m_hallOfFame
    NameList m_singleRecords[2][21];    // +0x22E8
    TimeScoreListPair m_x3008[2][21];   // +0x3008
};

#define RECORDS(options) ((HallOfFameRecords *)(options))

void JetskiNameMenu::ReplaceName()
{
    char placeholder[0x10];
    JetskiGameOptions *options = LpGame->m_gameOptions;
    int i;
    sprintf(placeholder, "%^&*%d", m_player);
    switch (LpGame->m_gameOptions->m_raceType) {
    case 6:
        if (((JetskiCareerCompetition *)LpGame->m_competition)->m_raceType == 1) {
            NameList *list = &RECORDS(options)->m_singleRecords[options->m_difficulty == 3 ? 1 : 0][options->m_x2C];
            for (i = 0; i < 6; i++) {
                if (strstr(list->m_names[i], placeholder))
                    strcpy(list->m_names[i], m_nameTexts[1]->m_text);
            }
            return;
        } else {
            NameListPair *pair = &RECORDS(options)->m_records[options->m_difficulty == 3 ? 1 : 0][options->m_x2C];
            for (i = 0; i < 6; i++) {
                if (strstr(pair->m_first.m_names[i], placeholder))
                    strcpy(pair->m_first.m_names[i], m_nameTexts[1]->m_text);
            }
            for (i = 0; i < 6; i++) {
                if (strstr(pair->m_second.m_names[i], placeholder))
                    strcpy(pair->m_second.m_names[i], m_nameTexts[1]->m_text);
            }
        }
        break;
    case 1:
    case 2:
    case 7: {
        NameListPair *pair = &RECORDS(options)->m_records[options->m_difficulty == 3 ? 1 : 0][options->m_x2C];
        for (i = 0; i < 6; i++) {
            if (strstr(pair->m_first.m_names[i], placeholder))
                strcpy(pair->m_first.m_names[i], m_nameTexts[1]->m_text);
        }
        for (i = 0; i < 6; i++) {
            if (strstr(pair->m_second.m_names[i], placeholder))
                strcpy(pair->m_second.m_names[i], m_nameTexts[1]->m_text);
        }
        break;
    }
    case 4: {
        TimeScoreListPair *pair = &RECORDS(options)->m_x3008[options->m_difficulty == 3 ? 1 : 0][options->m_x2C];
        for (i = 0; i < 6; i++) {
            if (strstr(pair->m_times.m_names[i], placeholder))
                strcpy(pair->m_times.m_names[i], m_nameTexts[1]->m_text);
        }
        for (i = 0; i < 6; i++) {
            if (strstr(pair->m_scores.m_names[i], placeholder))
                strcpy(pair->m_scores.m_names[i], m_nameTexts[1]->m_text);
        }
        break;
    }
    case 8:
        break;
    case 3: {
        NameList *list = &RECORDS(options)->m_singleRecords[options->m_difficulty == 3 ? 1 : 0][options->m_x2C];
        for (i = 0; i < 6; i++) {
            if (strstr(list->m_names[i], placeholder))
                strcpy(list->m_names[i], m_nameTexts[1]->m_text);
        }
        break;
    }
    }
}

// ---- src/JetskiNameMenu/BuildRecordLists.cpp

static inline void FormatTime(char *buffer, float t)
{
    if (t == 0.0f || t == 3.4028235e38f) {
        strcpy(buffer, "--:--.--");
    } else {
        int minutes;
        if (t < 0.0f)
            t = -1.0f * t;
        if (t >= 5999.99f) {
            t = 59.99f;
            minutes = 99;
        } else {
            for (minutes = 0; !(t < 60.0f); minutes++)
                t -= 60.0f;
        }
        sprintf(buffer, "%2d:%05.2f", minutes, t);
    }
}

// Lists this player's places in one record table: each entry holding the
// player's placeholder name gets a "place, name" text (string id) and a
// value text. Retail positions the name text twice and never the value.
#define LIST_RECORDS(list, id, FORMAT_VALUE)                                                        \
    for (i = 0; i < 6; i++) {                                                                       \
        if (strstr((list)->m_names[i], placeholder)) {                                              \
            if (i)                                                                                  \
                sprintf(text, "%d%s %s", i + 1, places[i], LpGame->m_text->GetString(id));          \
            else                                                                                    \
                sprintf(text, LpGame->m_text->GetString(id));                                       \
            m_texts[m_recordCounts[m_player - 1] + 1]->SetString(text);                             \
            m_textManager->PositionText(m_texts[m_recordCounts[m_player - 1] + 1],                  \
                                        ScreenTextManager::LeftJustification, m_font);              \
            FORMAT_VALUE;                                                                           \
            m_texts[m_recordCounts[m_player - 1] + 3]->SetString(text);                             \
            m_textManager->PositionText(m_texts[m_recordCounts[m_player - 1] + 1],                  \
                                        ScreenTextManager::LeftJustification, m_font);              \
            m_recordCounts[m_player - 1]++;                                                         \
        }                                                                                           \
    }

void JetskiNameMenu::BuildRecordLists()
{
    char text[0x40];
    JetskiGameOptions *options = LpGame->m_gameOptions;
    char *places[6] = {
        LpGame->m_text->GetString(0), LpGame->m_text->GetString(1), LpGame->m_text->GetString(2),
        LpGame->m_text->GetString(3), LpGame->m_text->GetString(4), LpGame->m_text->GetString(5),
    };
    char placeholder[0x10];
    int i;

    m_recordCounts[m_player - 1] = 0;
    sprintf(placeholder, "%^&*%d", m_player);
    switch (LpGame->m_gameOptions->m_raceType) {
    case 6:
        if (((JetskiCareerCompetition *)LpGame->m_competition)->m_raceType == 1) {
            NameList *list = &RECORDS(options)->m_singleRecords[options->m_difficulty == 3 ? 1 : 0][options->m_x2C];
            LIST_RECORDS(list, 0x99, FormatTime(text, list->m_times[i]));
        } else {
            NameListPair *pair = &RECORDS(options)->m_records[options->m_difficulty == 3 ? 1 : 0][options->m_x2C];
            LIST_RECORDS(&pair->m_first, 0x99, FormatTime(text, pair->m_first.m_times[i]));
        }
        break;
    case 1:
    case 2:
    case 7: {
        NameListPair *pair = &RECORDS(options)->m_records[options->m_difficulty == 3 ? 1 : 0][options->m_x2C];
        LIST_RECORDS(&pair->m_first, 0x99, FormatTime(text, pair->m_first.m_times[i]));
        LIST_RECORDS(&pair->m_second, 0x9A, FormatTime(text, pair->m_second.m_times[i]));
        break;
    }
    case 4: {
        TimeScoreListPair *pair = &RECORDS(options)->m_x3008[options->m_difficulty == 3 ? 1 : 0][options->m_x2C];
        LIST_RECORDS(&pair->m_times, 0x4E, FormatTime(text, pair->m_times.m_times[i]));
        LIST_RECORDS(&pair->m_scores, 0x4F, sprintf(text, "%d", pair->m_scores.m_scores[i]));
        break;
    }
    case 8:
        break;
    case 3: {
        NameList *list = &RECORDS(options)->m_singleRecords[options->m_difficulty == 3 ? 1 : 0][options->m_x2C];
        LIST_RECORDS(list, 0x99, FormatTime(text, list->m_times[i]));
        break;
    }
    }
    if (!m_recordCounts[m_player - 1])
        m_texts[1]->SetString(LpGame->m_text->GetString(0x9B));
}

// ---- src/JetskiNameMenu/DisableMenu.cpp

void JetskiNameMenu::DisableMenu()
{
    JetskiGameOptions *options = LpGame->m_gameOptions;
    DisableAll();
    LpGame->m_auralScape->Enable();
    LpGame->m_competition->ResetAllEngineSounds();
    if (options->m_raceType == 6) {
        JetskiCareerCompetition *competition = (JetskiCareerCompetition *)LpGame->m_competition;
        if (competition->m_circuitOver) {
            if (competition->m_playerWon)
                competition->m_promptMenu->SetCareerOverPrompt(0);
            else
                competition->m_promptMenu->SetCareerOverPrompt(1);
            LpGame->m_competition->m_promptMenu->EnableMenu();
        } else {
            competition->EventComplete();
        }
    } else {
        if (m_player == 1 && LpGame->m_competition->m_x118 == 2) {
            m_player = 2;
            Initialize(0);
        } else {
            LpGame->m_competition->EventComplete();
        }
    }
}

// ---- src/JetskiPromptMenu/ctor.cpp

JetskiPromptMenu::JetskiPromptMenu(int enabled) : JetskiMenu(enabled)
{
    m_textManager = 0;
    m_texts = 0;
    m_prompts = 0;
    m_font104 = 0;
    m_font108 = 0;
    m_font10C = 0;
    m_font110 = 0;
    m_x68 = 0;
    m_promptType = 1;
    m_controller = 0;
    m_numTexts[0] = 10;
    m_numTexts[1] = 12;
}

// ---- src/JetskiPromptMenu/dtor.cpp

JetskiPromptMenu::~JetskiPromptMenu()
{
    int i;
    for (i = 0; i < m_numTexts[m_promptType]; i++)
        delete m_texts[i];
    free(m_texts);
    m_texts = 0;
    if (m_prompts) {
        for (i = 0; i < 2; i++)
            delete m_prompts[i];
        free(m_prompts);
        m_prompts = 0;
    }
}

// ---- src/JetskiPromptMenu/Create.cpp

// Colours go through a block-local copy at each call: that puts retail's
// colour constants in the right registers.
#define SET_COLOR(text, value)          \
    {                                   \
        unsigned int color = value;     \
        (text)->SetColor(color);        \
    }

// Builds the prompt menu's texts: the continue prompt (type 0) or the copycat
// screen (type 1, with its stunt list and two prompt lines).
JetskiPromptMenu *JetskiPromptMenu::Create(ScreenIconManager *icons, ScreenTextManager *texts, ViewPortCode viewport, int type)
{
    int i;
    m_promptType = type;
    JetskiMenu::Create(icons, viewport);
    switch (m_promptType) {
    case 0:
        BuildMenuSmall(10);
        break;
    case 1:
        BuildMenuFull(12);
        break;
    }
    ScreenTextFontItem *item = LpGame->m_fonts->GetScreenTextFont("info_c");
    m_font104 = item ? item->m_font : 0;
    item = LpGame->m_fonts->GetScreenTextFont("info_c");
    m_font108 = item ? item->m_font : 0;
    item = LpGame->m_fonts->GetScreenTextFont("title_b");
    m_font10C = item ? item->m_font : 0;
    item = LpGame->m_fonts->GetScreenTextFont("title_b");
    m_font110 = item ? item->m_font : 0;
    m_textManager = (new ScreenTextManager(0, 1))->Create(64);
    AddChild(m_textManager, -1);
    m_texts = (ScreenText **)malloc(m_numTexts[m_promptType] * sizeof(ScreenText *));
    switch (m_promptType) {
    case 0:
        for (i = 0; i < m_numTexts[m_promptType]; i++) {
            m_texts[i] = new ScreenText(40, 0);
            switch (i) {
            case 0:
                m_texts[i]->SetPosition(320.0f, 122.0f);
                m_texts[i]->SetFont(m_font10C);
                m_texts[i]->SetJustification(ScreenTextManager::CenterHorizontalJustification);
                break;
            case 1:
                m_texts[i]->SetScale(1.286f);
                m_texts[i]->SetPosition(320.0f, 160.0f);
                m_texts[i]->SetFont(m_font104);
                m_texts[i]->SetJustification(ScreenTextManager::CenterHorizontalJustification);
                break;
            case 2:
                m_texts[i]->SetScale(1.286f);
                m_texts[i]->SetPosition(320.0f, 180.0f);
                SET_COLOR(m_texts[i], 0xFFFFDE00)
                m_texts[i]->SetFont(m_font104);
                m_texts[i]->SetJustification(ScreenTextManager::CenterHorizontalJustification);
                break;
            case 3:
                m_texts[i]->SetScale(1.286f);
                m_texts[i]->SetPosition(320.0f, 210.0f);
                SET_COLOR(m_texts[i], 0xFFFFDE00)
                m_texts[i]->SetFont(m_font104);
                m_texts[i]->SetJustification(ScreenTextManager::CenterHorizontalJustification);
                break;
            case 4:
                m_texts[i]->SetScale(1.286f);
                m_texts[i]->SetPosition(320.0f, 240.0f);
                SET_COLOR(m_texts[i], 0xFFFFDE00)
                m_texts[i]->SetFont(m_font104);
                m_texts[i]->SetJustification(ScreenTextManager::CenterHorizontalJustification);
                break;
            case 5:
                m_texts[i]->SetScale(1.286f);
                m_texts[i]->SetPosition(320.0f, 270.0f);
                SET_COLOR(m_texts[i], 0xFFFFDE00)
                m_texts[i]->SetFont(m_font104);
                m_texts[i]->SetJustification(ScreenTextManager::CenterHorizontalJustification);
                break;
            case 6:
                m_texts[i]->SetScale(1.286f);
                m_texts[i]->SetPosition(320.0f, 195.0f);
                SET_COLOR(m_texts[i], 0xFFFFDE00)
                m_texts[i]->SetFont(m_font104);
                m_texts[i]->SetJustification(ScreenTextManager::CenterHorizontalJustification);
                break;
            case 7:
                m_texts[i]->SetScale(1.286f);
                m_texts[i]->SetPosition(320.0f, 225.0f);
                SET_COLOR(m_texts[i], 0xFFFFDE00)
                m_texts[i]->SetFont(m_font104);
                m_texts[i]->SetJustification(ScreenTextManager::CenterHorizontalJustification);
                break;
            case 8:
                m_texts[i]->SetScale(1.286f);
                m_texts[i]->SetPosition(320.0f, 255.0f);
                SET_COLOR(m_texts[i], 0xFFFFDE00)
                m_texts[i]->SetFont(m_font104);
                m_texts[i]->SetJustification(ScreenTextManager::CenterHorizontalJustification);
                break;
            case 9:
                m_texts[i]->SetString(LpGame->m_text->GetString(128));
                m_texts[i]->SetPosition(382.0f, 307.0f);
                m_texts[i]->SetScale(0.857f);
                m_texts[i]->SetFont(m_font108);
                m_texts[i]->SetJustification(ScreenTextManager::LeftJustification);
                m_textManager->PositionText(m_texts[i]);
                break;
            }
        }
        break;
    case 1:
        for (i = 0; i < m_numTexts[m_promptType]; i++) {
            m_texts[i] = new ScreenText(40, 0);
            switch (i) {
            case 0:
                m_texts[i]->SetString(LpGame->m_text->GetString(92));
                m_texts[i]->SetScale(1.059f);
                m_texts[i]->SetPosition(320.0f, 39.0f);
                m_texts[i]->SetFont(m_font10C);
                m_texts[i]->SetJustification(ScreenTextManager::CenterHorizontalJustification);
                m_textManager->PositionText(m_texts[i]);
                break;
            case 1:
                m_texts[i]->SetScale(1.429f);
                m_texts[i]->SetPosition(120.0f, 97.0f);
                m_texts[i]->SetFont(m_font104);
                m_texts[i]->SetJustification(ScreenTextManager::LeftJustification);
                break;
            case 6:
                m_texts[i]->SetScale(1.429f);
                m_texts[i]->SetPosition(470.0f, 97.0f);
                m_texts[i]->SetFont(m_font104);
                m_texts[i]->SetJustification(ScreenTextManager::RightJustification);
                break;
            case 2:
                m_texts[i]->SetScale(1.429f);
                m_texts[i]->SetPosition(120.0f, 127.0f);
                m_texts[i]->SetFont(m_font104);
                m_texts[i]->SetJustification(ScreenTextManager::LeftJustification);
                break;
            case 7:
                m_texts[i]->SetScale(1.429f);
                m_texts[i]->SetPosition(470.0f, 127.0f);
                m_texts[i]->SetFont(m_font104);
                m_texts[i]->SetJustification(ScreenTextManager::RightJustification);
                break;
            case 3:
                m_texts[i]->SetScale(1.429f);
                m_texts[i]->SetPosition(120.0f, 157.0f);
                m_texts[i]->SetFont(m_font104);
                m_texts[i]->SetJustification(ScreenTextManager::LeftJustification);
                break;
            case 8:
                m_texts[i]->SetScale(1.429f);
                m_texts[i]->SetPosition(470.0f, 157.0f);
                m_texts[i]->SetFont(m_font104);
                m_texts[i]->SetJustification(ScreenTextManager::RightJustification);
                break;
            case 4:
                m_texts[i]->SetScale(1.429f);
                m_texts[i]->SetPosition(120.0f, 187.0f);
                m_texts[i]->SetFont(m_font104);
                m_texts[i]->SetJustification(ScreenTextManager::LeftJustification);
                break;
            case 9:
                m_texts[i]->SetScale(1.429f);
                m_texts[i]->SetPosition(470.0f, 187.0f);
                m_texts[i]->SetFont(m_font104);
                m_texts[i]->SetJustification(ScreenTextManager::RightJustification);
                break;
            case 5:
                m_texts[i]->SetScale(1.429f);
                m_texts[i]->SetPosition(120.0f, 217.0f);
                m_texts[i]->SetFont(m_font104);
                m_texts[i]->SetJustification(ScreenTextManager::LeftJustification);
                break;
            case 10:
                m_texts[i]->SetScale(1.429f);
                m_texts[i]->SetPosition(470.0f, 217.0f);
                m_texts[i]->SetFont(m_font104);
                m_texts[i]->SetJustification(ScreenTextManager::RightJustification);
                break;
            case 11:
                m_texts[i]->SetString(LpGame->m_text->GetString(128));
                m_texts[i]->SetPosition(412.0f, 388.0f);
                m_texts[i]->SetScale(0.857f);
                m_texts[i]->SetFont(m_font108);
                m_texts[i]->SetJustification(ScreenTextManager::LeftJustification);
                m_textManager->PositionText(m_texts[i]);
                break;
            }
        }
        m_prompts = (JetskiScreenText **)malloc(2 * sizeof(JetskiScreenText *));
        for (i = 0; i < 2; i++) {
            m_prompts[i] = new JetskiScreenText;
            m_prompts[i]->m_params.m_color = -1;
            m_prompts[i]->m_params.m_font = m_font104;
            m_prompts[i]->m_params.m_justification = ScreenTextManager::CenterHorizontalJustification;
            m_prompts[i]->m_params.m_lineSpacing = 18;
            m_prompts[i]->m_params.m_scale = 1.143f;
            switch (i) {
            case 0:
                m_prompts[i]->m_params.m_x = 320;
                m_prompts[i]->m_params.m_y = 250;
                break;
            case 1:
                m_prompts[i]->m_params.m_x = 320;
                m_prompts[i]->m_params.m_y = 300;
                break;
            }
        }
        break;
    }
    m_controller = -1;
    return this;
}

// ---- src/JetskiPromptMenu/PrepareGeometry.cpp

// Clamp the items' minimum widths to 10, rebuild the info panel if it was
// invalidated, then let the base menu prepare its geometry.
int JetskiPromptMenu::PrepareGeometry()
{
    if (m_font104) {
        TextureMap *texture = m_font104->m_texture;
        if (10.0f > texture->m_usage)
            texture->m_usage = 10.0f;
    }
    if (m_font108) {  // retail tests m_font108 but clamps m_font104
        TextureMap *texture = m_font104->m_texture;
        if (10.0f > texture->m_usage)
            texture->m_usage = 10.0f;
    }
    if (m_font10C) {  // retail tests m_font10C but clamps m_font104
        TextureMap *texture = m_font104->m_texture;
        if (10.0f > texture->m_usage)
            texture->m_usage = 10.0f;
    }
    if (m_infoDirty) {
        SetupInfo();
        m_infoDirty = 0;
    }
    return JetskiMenu::PrepareGeometry();
}

// ---- src/JetskiPromptMenu/Render3D.cpp
// GameObject per the retail vtable (Render3D/PrepareGeometry return int here).

// Draw once per frame, on the last player's pass.
int JetskiPromptMenu::Render3D()
{
    JetskiGameOptions *multiplayer = LpGame->m_gameOptions;
    int i;
    if (LpGame->m_viewport == LpGame->m_numCameras - 1) {
        if (WaitForRenderingToFinish && m_viewport) {
            LpRenderTarget->TrySendRenderQueue(1);
            LpRenderTarget->WaitForRenderingToComplete();
        }
        SetViewport();
        JetskiMenu::Render3D();
        switch (m_promptType) {
        case 0: {
            for (i = 0; i < m_numTexts[m_promptType]; i++)
                m_textManager->AddText(m_texts[i], 0);
            break;
        }
        case 1: {
            JetskiCareerCompetition *competition = (JetskiCareerCompetition *)LpGame->m_competition;
            for (i = 0; i < m_numTexts[m_promptType]; i++)
                m_textManager->AddText(m_texts[i], 0);
            m_prompts[0]->AddText(m_textManager);
            if (competition->m_playerWon && (multiplayer->m_difficulty == 1 || competition->m_x408))
                m_prompts[1]->AddText(m_textManager);
            break;
        }
        }
        GameObject::Render3D();
        RestoreViewport();
    }
    return 1;
}

// ---- src/JetskiPromptMenu/Tick.cpp

int JetskiPromptMenu::Tick(float dt)
{
    int busy = 0;
    if (m_closing) {
        for (int i = 0; i < 7; i++) {
            if (m_timers[i].m_active)
                busy = 1;
        }
        if (!busy) {
            DisableMenu();
            m_closing = 0;
        }
    }
    return JetskiMenu::Tick(dt);
}

// ---- src/JetskiPromptMenu/KeyDownHandler.cpp

int JetskiPromptMenu::KeyDownHandler(ProcessNode *node, KeyInfo *key)
{
    if (KEYDOWN_HANDLER_TEST((AssignableActionIndex)4, (DeviceType)2, node, 0x80000000)) {
        if (!m_promptType) {
            if (m_controller != node->m_controller)
                return 0;
        }
        m_closing = 1;
    }
    JetskiMenu::KeyDownHandler(node, key);
    return 1;
}

// ---- src/JetskiPromptMenu/EnableMenu.cpp

void JetskiPromptMenu::EnableMenu()
{
    LpGame->m_competition->Pause(1);
    if (LpGame->m_competition->m_radar)
        LpGame->m_competition->m_radar->SetShowing(0);
    LpGame->m_auralScape->StopAllAuralScapeSounds();
    LpGame->m_auralScape->Disable();
    EnableAll();
}

// ---- src/JetskiPromptMenu/DisableMenu.cpp

void JetskiPromptMenu::DisableMenu()
{
    switch (m_promptType) {
    case 1:
        LpGame->m_competition->EventComplete();
        break;
    default:
        LpGame->m_competition->Pause(0);
        break;
    }
    LpGame->m_auralScape->Enable();
    LpGame->m_competition->ResetAllEngineSounds();
    DisableAll();
}

// ---- src/JetskiPromptMenu/SetupInfo.cpp

void JetskiPromptMenu::SetupInfo()
{
    JetskiMenu::SetupInfo();
}

// ---- src/JetskiPromptMenu/SetCopyCatPrompt.cpp

// The copycat prompt: whose turn it is and, while copying, the stunts to copy.
#pragma fast_fptosi on
void JetskiPromptMenu::SetCopyCatPrompt(int a, int player)
{
    char buf[0x30];
    m_controller = player - 1;
    if (player == 1)
        strcpy(buf, LpGame->m_text->GetString(249));
    else
        strcpy(buf, LpGame->m_text->GetString(250));
    m_texts[0]->SetString(buf);
    m_textManager->PositionText(m_texts[0]);
    LpGame->m_competition->m_infoText->AddInfoText("", 0, 0.0f, 1, 1.0f);
    switch (a) {
    case 1: {
        m_texts[1]->SetString(LpGame->m_text->GetString(242));
        float step = 0.5f;
        int n = 0;
        RaceStatusStruct_Tag *s = LpGame->m_competition->m_raceStatus->m_vehicles;
        char multiplier[10] = "";
        while (s && !(s->m_flags & 0x100))
            s = s->m_next;
        if (!s)
            return;
        RaceStatusStunt *stunt = s->m_stunts;
        if (stunt) {    // (redundant guard: retail tests stunt twice)
            while (stunt && n < 5) {
                if (stunt->m_type >= 0)
                    strcpy(m_stunts[n], LpGame->m_competition->m_performanceMeter->StuntsName(stunt->m_type, -1));
                else
                    strcpy(m_stunts[n], LpGame->m_text->GetString(170));
                if (!(stunt->m_x1C / step < 1.0f)) {
                    strcpy(multiplier, "");
                    sprintf(multiplier, " %1.1fx", 1.0f + (int)(stunt->m_x1C / step));
                    strcat(m_stunts[n], multiplier);
                }
                stunt = stunt->m_next;
                n++;
            }
        }
        m_texts[2]->SetString(m_stunts[n - 1] ? m_stunts[n - 1] : (char *)"??? ??? ???");
        m_texts[3]->SetString(n - 2 >= 0 ? m_stunts[n - 2] : (char *)"");
        m_texts[6]->SetString(n - 2 >= 0 ? (char *)"+" : (char *)"");
        m_texts[4]->SetString(n - 3 >= 0 ? m_stunts[n - 3] : (char *)"");
        m_texts[7]->SetString(n - 3 >= 0 ? (char *)"+" : (char *)"");
        m_texts[5]->SetString(n - 4 >= 0 ? m_stunts[n - 4] : (char *)"");
        m_texts[8]->SetString(n - 4 >= 0 ? (char *)"+" : (char *)"");
        break;
    }
    case 0:
        m_texts[1]->SetString(LpGame->m_text->GetString(241));
        m_texts[2]->SetString("");
        m_texts[3]->SetString("");
        m_texts[4]->SetString("");
        m_texts[5]->SetString("");
        m_texts[6]->SetString("");
        m_texts[7]->SetString("");
        m_texts[8]->SetString("");
        break;
    }
    m_textManager->PositionText(m_texts[1]);
    m_textManager->PositionText(m_texts[2]);
    m_textManager->PositionText(m_texts[3]);
    m_textManager->PositionText(m_texts[4]);
    m_textManager->PositionText(m_texts[5]);
    m_textManager->PositionText(m_texts[6]);
    m_textManager->PositionText(m_texts[7]);
    m_textManager->PositionText(m_texts[8]);
}
#pragma fast_fptosi reset

// ---- src/JetskiPromptMenu/SetCareerOverPrompt.cpp

static inline void SetTextString(ScreenText *text, char *buf)
{
    text->m_positionDirty = 1;
    strncpy(text->m_text, buf, text->m_maxChars);
    text->m_text[text->m_maxChars] = 0;
    text->m_x5D_bit4 = 1;
    text->m_dirty = 1;
    text->m_cellsDirty = 1;
}
// Lists the final career standings (names and points, highlighting the
// computer players) and the end-of-career prompts.
void JetskiPromptMenu::SetCareerOverPrompt(int mode)
{
    JetskiCareerCompetition *competition;
    JetskiGameOptions *data;
    JetskiCareer *career;
    int line;
    int place;
    int i;
    char buf[0x100];

    data = LpGame->m_gameOptions;
    competition = (JetskiCareerCompetition *)LpGame->m_competition;
    career = data->m_career;
    line = 1;
    for (place = 0; place < 5; place++) {
        for (i = 0; i < 5; i++) {
            if (career->m_racers[i].m_place != place + 1)
                continue;
            if (!career->m_racers[i].m_enabled) {
                ScreenText *t = m_texts[line];
                int color;
                t->m_color = color = 0xFFFFDE00;   // chained: one constant register
                t->m_x5D_bit4 = 1;
                t = m_texts[line + 5];
                t->m_color = color;
                t->m_x5D_bit4 = 1;
            }
            sprintf(buf, "%d. %s", place + 1, career->m_racers[i].m_name);
            SetTextString(m_texts[line], buf);
            m_textManager->PositionText(m_texts[line]);
            sprintf(buf, "%d", career->m_racers[i].m_points);
            SetTextString(m_texts[line + 5], buf);
            m_textManager->PositionText(m_texts[line + 5]);
            line++;
        }
    }
    if (mode == 0) {
        m_prompts[0]->SetText(0x100, 0);
        if (data->m_difficulty == 1)
            m_prompts[1]->SetText(0x101, 0);
        else if (competition->m_x408)
            m_prompts[1]->SetText(0x9D, 0);
    } else if (mode == 1) {
        m_prompts[0]->SetText(0x102, 0);
    }
}

// ---- src/sinit/JetskiInGameMenu.cpp
// Static initializer of JetskiInGameMenu.cpp: only the axis vectors from the shared header.

