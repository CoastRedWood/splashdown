// JetskiLoadManager.cpp  @ 0x00376DD0 (text 0x376DD0-0x37C300)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's JetskiLoadManager.cpp as one translation unit: JetskiLoadManager,
// the boot screens (JetskiScreenLanguage, Legal, Title, LoadUI, LoadEvent,
// Video) and GetGameTextIDFromCourse, then the static initializer (only the
// axis vectors). Its .rodata ends with the JetskiLoadManager RTTI (0x43B4BC);
// JetskiRaceOverMenus.cpp follows (labels restart at @1215).



#include "GameObject/GameObject.h"
#include "JetskiLoadManager/JetskiLoadManager.h"
#include "d3dtypes.h"
#include "Game/Game.h"
#include "RenderTarget/RenderTarget.h"
#include "PSXGame/PSXGame.h"
#include "PSXTextureMap/PSXTextureMap.h"
#include "DataAccess/DataAccess.h"
#include "PSXVideoCard/PSXVideoCard.h"
#include "soultree_string.h"
#include "_RECT/_RECT.h"
#include "JetskiScreenLanguage/JetskiScreenLanguage.h"
#include "ControlInterface/ControlInterface.h"
#include "JetskiGameOptions/JetskiGameOptions.h"
#include "JetskiGame/JetskiGame.h"
#include "GameObject/GameObjectGlobals.h"
#include "JetskiScreenLoadUI/JetskiScreenLoadUI.h"
#include "PSXRenderTarget/PSXRenderTarget.h"
#include "ScreenText/ScreenText.h"
#include "JetskiScreenText/JetskiScreenText.h"
#include "TextureMap/TextureMap.h"
#include "soultree_assert_block.h"
#include "ScreenTextManager/ScreenTextManager.h"
#include "ScreenTextFontItemList/ScreenTextFontItem.h"
#include "ScreenTextFontItemList/ScreenTextFontItemList.h"
#include "GameText/GameText.h"
#include "JetskiCareer/JetskiCareer.h"
#include "CareerRacer/CareerRacer.h"
#include "MediaControl/MediaControl.h"
#include "PSXMediaControl/PSXMediaControl.h"
#include "sinit/AxisVectors.h"
#include "UICamera/UICamera.h"
#include "LoadingIcon/LoadingIcon.h"
#include "ProcessNode/ProcessNode.h"
#include "ScreenIcon/ScreenIcon.h"
#include "ScreenIconManager/ScreenIconManager.h"

class ProcessNode;
class KeyInfo;
class ScreenTextFont;

// inline_depth is TU-wide (JetskiScreenLoadEvent::Create's).
#pragma inline_depth(5)

extern JetskiGame *LpGame;
extern char LocalizationContentDirectory[0x104];

// The load bar (its code is in TextureMapStringTable.cpp's row). 0x58 bytes.
class JetskiProgressBar : public GameObject {
public:
    JetskiProgressBar(int enabled);
    int Create(char *filename);
    void AddProgress(float amount);
    void *m_texture;                    // +0x30
    char pad34[0x58 - 0x34];
};

// The boot screens JetskiLoadManager::Tick steps through (no headers; only
// this TU uses them).
class JetskiScreenLegal : public GameObject {
public:
    JetskiScreenLegal(int enabled);
    JetskiScreenLegal *Create(JetskiLoadManager *loadManager);

    // --- virtuals in retail slot order ---
    virtual ~JetskiScreenLegal();
    virtual int Tick(float dt);
    virtual int RenderPre3D();

    JetskiLoadManager *m_loadManager;   // +0x30
    PSXTextureMap *m_background;        // +0x34
    float m_time;                       // +0x38
    char pad3C[0x40 - 0x3C];
};

class JetskiScreenTitle : public GameObject {
public:
    JetskiScreenTitle(int enabled);
    JetskiScreenTitle *Create(JetskiLoadManager *loadManager);

    // --- virtuals in retail slot order ---
    virtual ~JetskiScreenTitle();
    virtual int Tick(float dt);
    virtual int RenderPre3D();

    JetskiLoadManager *m_loadManager;   // +0x30
    PSXTextureMap *m_background;        // +0x34
    float m_time;                       // +0x38
    char pad3C[0x40 - 0x3C];
};

class JetskiScreenLoadEvent : public GameObject {
public:
    JetskiScreenLoadEvent(int enabled);
    JetskiScreenLoadEvent *Create(JetskiLoadManager *loadManager);

    // --- virtuals in retail slot order ---
    virtual ~JetskiScreenLoadEvent();
    virtual int Tick(float dt);
    virtual int RenderPre3D();
    virtual int Render3D();

    JetskiLoadManager *m_loadManager;   // +0x30
    PSXTextureMap *m_background;        // +0x34
    int m_x38;                          // +0x38
    int m_event;                        // +0x3C, -1 for none
    int m_numTexts[3];                  // +0x40, per event
    ScreenText **m_texts;               // +0x4C (calloc'd)
    JetskiScreenText *m_x50;            // +0x50
    JetskiScreenText *m_hint;           // +0x54
    ScreenTextFont *m_titleA;           // +0x58
    ScreenTextFont *m_titleB;           // +0x5C
    ScreenTextFont *m_infoA;            // +0x60
    ScreenTextFont *m_infoB;            // +0x64
    ScreenTextManager *m_textManager;   // +0x68
    char pad6C[0x70 - 0x6C];
};

class JetskiScreenVideo : public GameObject {
public:
    JetskiScreenVideo(int enabled);
    JetskiScreenVideo *Create(JetskiLoadManager *loadManager);
    void ExitVideo();

    // --- virtuals in retail slot order ---
    virtual ~JetskiScreenVideo();
    virtual int Tick(float dt);
    virtual int KeyDownHandler(ProcessNode *node, KeyInfo *key);

    JetskiLoadManager *m_loadManager;   // +0x30
    PSXMediaControl *m_video;           // +0x34
};


// ---- src/JetskiLoadManager/ctor.cpp

JetskiLoadManager::JetskiLoadManager(int enabled) : GameObject(enabled)
{
    m_screen = 0;
    m_loadScreen = 0;
    m_status = 0;
    m_current = 0;
    m_loadIcon = 0;
    m_loadBar = 0;
    m_progress = 9;
    m_total = 3;
}

// ---- src/JetskiLoadManager/dtor.cpp


JetskiLoadManager::~JetskiLoadManager()
{
}


// ---- src/JetskiLoadManager/Create.cpp

void PSXprintf(const char *fmt, ...);






JetskiLoadManager *JetskiLoadManager::Create()
{
    GameObject::Create();
    LpGame->m_textureCache = 1;
    if (!(m_camera = new UICamera(1))->Create())
        PSXprintf("Failed to create LpUICameras in UIManager::LoadCamera\n");
    _D3DVECTOR position(384.0f, 1056.0f, 256.0f);
    _D3DVECTOR direction(0.0f, -1.0f, 0.0f);
    _D3DVECTOR up(0.0f, 0.0f, 1.0f);
    float fov = 35.0f;
    float roll = 0.0f;
    m_camera->SetViewParameters(&position, &direction, &up, &roll, &fov);
    m_camera->Enable();
    if (!AddChild(m_camera, -1))
        PSXprintf("Camera not created");
    LpGame->SetCameras(m_camera, 0, 0, 0);
    return this;
}


// ---- src/JetskiLoadManager/Tick.cpp
// Compiler session state: an unused
// helper that recreates the mwcc session retail compiled this file in; it is
// not part of the retail file.
static void MWCC_SHIM_TEXT MwccSessionState(long *a) { a[0] = a[0] / a[1]; }

extern "C" double sqrt(double x);

extern int TimerOverflowCounterT0;

inline unsigned int timeGetTime()
{
    static long Time = 0;
    static int lastCount = 0;
    int count, elapsed, overflows;
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


class JetskiLoadManager;


extern JetskiGame *LpGame;







// UICamera is incomplete here, so m_camera is cast to the GameObject it is.

int JetskiLoadManager::Tick(float dt)
{
    JetskiGameOptions *options = LpGame->m_gameOptions;
    int start = 0;
    if (m_status) {
        m_current = m_status;
        m_status = 0;
        LpRenderTarget->TrySendRenderQueue(1);
        LpRenderTarget->WaitForRenderingToComplete();
        if (m_screen) {
            m_screen->RemoveFromChain();
            m_screen->Release();
            m_screen = 0;
        }
        switch (m_current) {
        case 0:
            break;
        case 1:
            ((GameObject *)m_camera)->DisableAll();
            RemoveLoad();
            switch (LpGame->m_gameOptions->m_uiMode) {
            case 1:
                if (!options->m_x5FDC) {
                    CreateLoadBar("UIStart\\");
                    CreateLoadIcon("UIStart\\");
                } else {
                    start = timeGetTime();
                }
                break;
            }
            LpGame->StartUI();
            switch (LpGame->m_gameOptions->m_uiMode) {
            case 1:
                if (options->m_x5FDC) {
                    double busy = start * 12.34;
                    while (timeGetTime() - start <= 5000)
#ifdef SPLASHDOWN_PC
                        port_time_spin(),
#endif
                        sqrt(busy);
                    options->m_x5FDC = 0;
                } else {
                    RemoveLoadBar();
                    RemoveLoadIcon();
                }
                break;
            }
            RemoveFromChain();
            break;
        case 2:
            ((GameObject *)m_camera)->DisableAll();
            RemoveLoad();
            CreateLoadIcon("GamStart\\");
            CreateLoadBar("GamStart\\");
            LpGame->StartEvent();
            RemoveLoadBar();
            RemoveLoadIcon();
            RemoveFromChain();
            break;
        case 3:
            if (LpGame->m_competition)
                LpGame->EndEvent();
            if (LpGame->m_uiManager)
                LpGame->EndUI();
            ((GameObject *)m_camera)->Enable();
            LpGame->SetCameras((Camera *)m_camera, 0, 0, 0);
            m_loadScreen = new JetskiScreenLoadUI(1);
            ((JetskiScreenLoadUI *)m_loadScreen)->Create(this);
            AddChild(m_loadScreen, -1);
            break;
        case 4:
            if (LpGame->m_competition)
                LpGame->EndEvent();
            if (LpGame->m_uiManager)
                LpGame->EndUI();
            ((GameObject *)m_camera)->Enable();
            LpGame->SetCameras((Camera *)m_camera, 0, 0, 0);
            m_loadScreen = new JetskiScreenLoadEvent(1);
            ((JetskiScreenLoadEvent *)m_loadScreen)->Create(this);
            AddChild(m_loadScreen, -1);
            break;
        case 8:
            m_screen = new JetskiScreenLanguage(1);
            ((JetskiScreenLanguage *)m_screen)->Create(this);
            AddChild(m_screen, -1);
            break;
        case 5:
            m_screen = new JetskiScreenLegal(1);
            ((JetskiScreenLegal *)m_screen)->Create(this);
            AddChild(m_screen, -1);
            break;
        case 6:
            m_screen = new JetskiScreenTitle(1);
            ((JetskiScreenTitle *)m_screen)->Create(this);
            AddChild(m_screen, -1);
            break;
        case 7:
            if (LpGame->m_competition)
                LpGame->EndEvent();
            if (LpGame->m_uiManager)
                LpGame->EndUI();
            ((GameObject *)m_camera)->Enable();
            LpGame->SetCameras((Camera *)m_camera, 0, 0, 0);
            m_screen = new JetskiScreenVideo(1);
            ((JetskiScreenVideo *)m_screen)->Create(this);
            AddChild(m_screen, -1);
            break;
        }
    }
    return GameObject::Tick(dt);
}


// ---- src/JetskiLoadManager/LoadBackground.cpp
extern "C" char *strcpy(char *dst, const char *src);
extern "C" char *strcat(char *dst, const char *src);

class Palette8;
struct _RECT;


PSXTextureMap *GetTextureMapFromResourceName(char *name, int flags, Palette8 *palette, unsigned int a,
                                             _D3DBLEND src, _D3DBLEND dst, int alpha, unsigned int colorKey, int b);

extern DataAccess *LpDataAccess;

extern PSXVideoCard *LpVideoCard;

// The last '.' in the name, or 0.
static inline char *FindExtension(char *s)
{
    char *start = s;
    while (*s++)
        ;
    do {
        s--;
    } while (s != start && *s != '.');
    if (*s == '.')
        return s;
    return 0;
}


int JetskiLoadManager::LoadBackground(char *name, PSXTextureMap **texture)
{
    char resource[0x110];

    if (*texture)
        (*texture)->Release();
    strcpy(resource, name);
    char *extension = FindExtension(resource);
    if (extension)
        strcpy(extension, ".res");
    else
        strcat(resource, (char *)".res");   // the cast moves the literal's load first, as in retail
    LpDataAccess->LoadResourceFile(resource, 0, 0, 0, 0, 0, 0, 0);
    *texture = GetTextureMapFromResourceName(name, 0x22B8, 0, 0, (_D3DBLEND)5, (_D3DBLEND)6, 0x80, 0xFF00FF, 1);
    LpDataAccess->DropDevice(resource, 1);
    if (*texture) {
        LpVideoCard->ClearFrameBuffer(0, 0, 3);
        return 1;
    }
    return 0;
}


// ---- src/JetskiLoadManager/RemoveLoad.cpp

extern RenderTarget *LpRenderTarget;



void JetskiLoadManager::RemoveLoad()
{
    LpRenderTarget->TrySendRenderQueue(1);
    LpRenderTarget->WaitForRenderingToComplete();
    if (m_loadScreen) {
        m_loadScreen->RemoveFromChain();
        m_loadScreen->Release();
        m_loadScreen = 0;
    }
}


// ---- src/JetskiLoadManager/CreateLoadIcon.cpp
#pragma alignlabel off


#pragma fast_fptosi on

extern char UIContentDirectory[0x104];




void JetskiLoadManager::CreateLoadIcon(char *directory)
{
    char savedDirectory[0xC8];
    _RECT rect;

#line 337 "JetskiLoadManager.cpp"
    STRCPY(savedDirectory, UIContentDirectory);
    STRCPY(UIContentDirectory, directory);
    rect.left = 538;
    rect.right = (int)(64.0f + rect.left);
    rect.top = 28;
    rect.bottom = (int)(64.0f + rect.top);
    m_loadIcon = new LoadingIcon();
    m_loadIcon->Create("LoadBuoy", 0x20, rect, 0xFF, 1);
    m_loadIcon->SetSpeed(99);
#line 347 "JetskiLoadManager.cpp"
    STRCPY(UIContentDirectory, savedDirectory);
    m_loadIcon->Enable();
}


#pragma fast_fptosi reset
#pragma alignlabel reset

// ---- src/JetskiLoadManager/RemoveLoadIcon.cpp

extern RenderTarget *LpRenderTarget;



void JetskiLoadManager::RemoveLoadIcon()
{
    LpRenderTarget->TrySendRenderQueue(1);
    LpRenderTarget->WaitForRenderingToComplete();
    if (m_loadIcon) {
        m_loadIcon->Disable();
        delete m_loadIcon;
        m_loadIcon = 0;
    }
}


// ---- src/JetskiLoadManager/CreateLoadBar.cpp
extern "C" int sprintf(char *buf, const char *fmt, ...);



void JetskiLoadManager::CreateLoadBar(char *folder)
{
    char filename[128];
    if (m_loadBar)
        RemoveLoadBar();
    sprintf(filename, "%sLoadBar.tga", folder);
    m_loadBar = new JetskiProgressBar(1);
    m_loadBar->Create(filename);
}


// ---- src/JetskiLoadManager/RemoveLoadBar.cpp

extern RenderTarget *LpRenderTarget;



// Only its GameObject base matters here.

void JetskiLoadManager::RemoveLoadBar()
{
    LpRenderTarget->TrySendRenderQueue(1);
    LpRenderTarget->WaitForRenderingToComplete();
    if (m_loadBar) {
        m_loadBar->Release();
        m_loadBar = 0;
    }
}


// ---- src/JetskiLoadManager/SetStatus.cpp




void JetskiLoadManager::SetStatus(int status, int progress, int total)
{
    m_status = status;
    switch (status) {
    case 7:
        m_total = total;
        m_progress = progress;
        break;
    }
    if (!m_parent)
        LpGame->m_root->AddChild(this, -1);
}


// ---- src/JetskiLoadManager/AddProgress.cpp

void JetskiLoadManager::AddProgress(float amount)
{
    if (m_loadBar)
        m_loadBar->AddProgress(amount);
}

// ---- src/JetskiScreenLanguage/ctor.cpp

JetskiScreenLanguage::JetskiScreenLanguage(int id) : GameObject(id)
{
    m_loadManager = 0;
    m_background = 0;
    m_iconManager = 0;
    m_icon = 0;
    m_language = LpGame->m_gameOptions->m_language;
    m_selected = 0;
}


// ---- src/JetskiScreenLanguage/dtor.cpp

extern RenderTarget *LpRenderTarget;


extern DataAccess *LpDataAccess;



JetskiScreenLanguage::~JetskiScreenLanguage()
{
    LpRenderTarget->TrySendRenderQueue(1);
    LpRenderTarget->WaitForRenderingToComplete();
    if (m_background)
        m_background->Release();
    LpDataAccess->DropDevice(m_flagsPath, 1);
}


// ---- src/JetskiScreenLanguage/Create.cpp

JetskiScreenLanguage *JetskiScreenLanguage::Create(JetskiLoadManager *loadManager)
{
    char path[0x104];
    LpRenderTarget->m_hasDepth = 0;
    m_loadManager = loadManager;
    sprintf(path, "AppStart\\%slangscr.tga", LocalizationContentDirectory);
    m_loadManager->LoadBackground(path, &m_background);
    m_iconManager = (new ScreenIconManager(1))->Create(0x80);
    if (m_iconManager)
        AddChild(m_iconManager, -1);
    sprintf(m_flagsPath, "AppStart\\%suiflags.res", LocalizationContentDirectory);
    LpDataAccess->LoadResourceFile(m_flagsPath, 0, 0, 0, 0, 0, 0, 0);
    ChangeFlag();
    return this;
}


// ---- src/JetskiScreenLanguage/ChangeFlag.cpp

// Show the flag of the selected language.
int JetskiScreenLanguage::ChangeFlag()
{
    char path[0x104];
    int y;
    switch (m_language) {
    case 2:
        sprintf(path, "AppStart\\%sflag_G.tga", LocalizationContentDirectory);
        y = 190;
        break;
    case 3:
        sprintf(path, "AppStart\\%sflag_I.tga", LocalizationContentDirectory);
        y = 254;
        break;
    case 4:
        sprintf(path, "AppStart\\%sflag_S.tga", LocalizationContentDirectory);
        y = 326;
        break;
    case 1:
        sprintf(path, "AppStart\\%sflag_F.tga", LocalizationContentDirectory);
        y = 118;
        break;
    default:
        sprintf(path, "AppStart\\%sflag_E.tga", LocalizationContentDirectory);
        y = 54;
        break;
    }
    if (m_icon) {
        delete m_icon;
        m_icon = 0;
    }
    m_icon = (new ScreenIcon)->Create(path, 58.0f, y);
    return 1;
}


// ---- src/JetskiScreenLanguage/KeyDownHandler.cpp
class KeyInfo;





// Left/right cycle through the five languages; select confirms.
int JetskiScreenLanguage::KeyDownHandler(ProcessNode *node, KeyInfo *info)
{
    if (!node->m_controller) {
        if (node->m_key == LpGame->m_controls->GetKeyFromMappedAction((AssignableActionIndex)2, (DeviceType)2, 0)) {
            m_language = m_language == 1 ? 5 : m_language == 2 ? 1 : m_language == 3 ? 2 : m_language == 4 ? 3 : 4;
            ChangeFlag();
        } else if (node->m_key == LpGame->m_controls->GetKeyFromMappedAction((AssignableActionIndex)3, (DeviceType)2, 0)) {
            m_language = m_language == 1 ? 2 : m_language == 2 ? 3 : m_language == 3 ? 4 : m_language == 4 ? 5 : 1;
            ChangeFlag();
        } else if (node->m_key == LpGame->m_controls->GetKeyFromMappedAction((AssignableActionIndex)4, (DeviceType)2, 0)) {
            m_selected = 1;
        }
    }
    return 1;
}


// ---- src/JetskiScreenLanguage/Tick.cpp


extern JetskiGame *LpGame;



int JetskiScreenLanguage::Tick(float dt)
{
    if (m_selected) {
        LpGame->SetLanguage(m_language);
        LpGame->m_gameOptions->SetUI(2, 5);
        m_loadManager->SetStatus(1, 0, 0);
    }
    return GameObject::Tick(dt);
}


// ---- src/JetskiScreenLanguage/RenderPre3D.cpp

int JetskiScreenLanguage::RenderPre3D()
{
    if (m_background)
        LpRenderTarget->Blt(0, m_background, 0, 0);
    m_iconManager->AddIcon(m_icon, 0);
    return GameObject::RenderPre3D();
}


// ---- src/JetskiScreenLegal/ctor.cpp

JetskiScreenLegal::JetskiScreenLegal(int enabled) : GameObject(enabled)
{
    m_loadManager = 0;
    m_background = 0;
    m_time = 0;
}

// ---- src/JetskiScreenLegal/dtor.cpp

extern RenderTarget *LpRenderTarget;


JetskiScreenLegal::~JetskiScreenLegal()
{
    LpRenderTarget->TrySendRenderQueue(1);
    LpRenderTarget->WaitForRenderingToComplete();
    if (m_background) {
        m_background->Release();
        m_background = 0;
    }
}


// ---- src/JetskiScreenLegal/Create.cpp
extern "C" int sprintf(char *buf, const char *fmt, ...);

class PSXTextureMap;


extern RenderTarget *LpRenderTarget;



extern JetskiGame *LpGame;



JetskiScreenLegal *JetskiScreenLegal::Create(JetskiLoadManager *loadManager)
{
    char name[0x104];
    char language;
    LpRenderTarget->m_hasDepth = 0;
    m_loadManager = loadManager;
    int lang = LpGame->m_gameOptions->m_language;
    if (lang == 1)
        language = 'F';
    else if (lang == 2)
        language = 'G';
    else if (lang == 3)
        language = 'I';
    else if (lang == 5)
        language = 'U';
    else if (lang == 4)
        language = 'S';
    else
        language = 'E';
    sprintf(name, "AppStart\\Legal_%c.tga", language);
    m_loadManager->LoadBackground(name, &m_background);
    return this;
}


// ---- src/JetskiScreenLegal/Tick.cpp



int JetskiScreenLegal::Tick(float dt)
{
    m_time += dt;
    if (m_time > 5.0f)
        m_loadManager->SetStatus(6, 0, 0);
    return GameObject::Tick(dt);
}


// ---- src/JetskiScreenTitle/RenderPre3D.cpp

struct _RECT;
class TextureMap;


extern RenderTarget *LpRenderTarget;


int JetskiScreenLegal::RenderPre3D()
{
    if (m_background)
        LpRenderTarget->Blt(0, m_background, 0, 0);
    return GameObject::RenderPre3D();
}


// ---- src/JetskiScreenTitle/ctor.cpp

JetskiScreenTitle::JetskiScreenTitle(int enabled) : GameObject(enabled)
{
    m_loadManager = 0;
    m_background = 0;
    m_time = 0;
}

// ---- src/JetskiScreenTitle/dtor.cpp


JetskiScreenTitle::~JetskiScreenTitle()
{
    LpRenderTarget->TrySendRenderQueue(1);
    LpRenderTarget->WaitForRenderingToComplete();
    if (m_background) {
        m_background->Release();
        m_background = 0;
    }
}


// ---- src/JetskiScreenTitle/Create.cpp

class PSXTextureMap;

extern RenderTarget *LpRenderTarget;


extern JetskiGame *LpGame;



// The title screen in the selected language.
JetskiScreenTitle *JetskiScreenTitle::Create(JetskiLoadManager *loadManager)
{
    JetskiGameOptions *options = LpGame->m_gameOptions;
    LpRenderTarget->m_hasDepth = 0;
    m_loadManager = loadManager;
    switch (options->m_language) {
    case 1:
        m_loadManager->LoadBackground("AppStart\\title_f.tga", &m_background);
        break;
    case 2:
        m_loadManager->LoadBackground("AppStart\\title_g.tga", &m_background);
        break;
    case 3:
        m_loadManager->LoadBackground("AppStart\\title_i.tga", &m_background);
        break;
    case 4:
        m_loadManager->LoadBackground("AppStart\\title_s.tga", &m_background);
        break;
    case 5:
        m_loadManager->LoadBackground("AppStart\\title_u.tga", &m_background);
        break;
    default:
        m_loadManager->LoadBackground("AppStart\\title_e.tga", &m_background);
        break;
    }
    return this;
}


// ---- src/JetskiScreenTitle/Tick.cpp
extern "C" char *strcpy(char *dst, const char *src);





extern JetskiGame *LpGame;


int JetskiScreenTitle::Tick(float dt)
{
    JetskiGameOptions *options = LpGame->m_gameOptions;
    m_time += dt;
    if (m_time > 0.1f) {
        strcpy(options->m_creditsVideo, "Videos\\Intro.pss");
        options->m_x89C = 31;
        options->SetUI(1, 10);
        m_loadManager->SetStatus(1, 0, 0);
    }
    return GameObject::Tick(dt);
}


// ---- src/JetskiScreenLegal/RenderPre3D.cpp
// (This file holds JetskiScreenTitle's RenderPre3D; the draft file names were swapped with src/JetskiScreenTitle/RenderPre3D.cpp.)



struct _RECT;
class TextureMap;


extern RenderTarget *LpRenderTarget;


int JetskiScreenTitle::RenderPre3D()
{
    if (m_background)
        LpRenderTarget->Blt(0, m_background, 0, 0);
    return GameObject::RenderPre3D();
}


// ---- src/JetskiScreenLoadUI/ctor.cpp

JetskiScreenLoadUI::JetskiScreenLoadUI(int enabled) : GameObject(enabled)
{
    m_loadManager = 0;
    m_background = 0;
    m_frames = 0;
}

// ---- src/JetskiScreenLoadUI/dtor.cpp

JetskiScreenLoadUI::~JetskiScreenLoadUI()
{
    LpRenderTarget->TrySendRenderQueue(1);
    LpRenderTarget->WaitForRenderingToComplete();
    if (m_background) {
        m_background->Release();
        m_background = 0;
    }
}


// ---- src/JetskiScreenLoadUI/Create.cpp
class PSXTextureMap;

extern RenderTarget *LpRenderTarget;



JetskiScreenLoadUI *JetskiScreenLoadUI::Create(JetskiLoadManager *manager)
{
    LpRenderTarget->TrySendRenderQueue(1);
    LpRenderTarget->WaitForRenderingToComplete();
    m_loadManager = manager;
    LpRenderTarget->m_hasDepth = 0;
    m_loadManager->LoadBackground("UIStart\\LoadUI.tga", &m_background);
    m_frames = 0;
    return this;
}


// ---- src/JetskiScreenLoadUI/Tick.cpp

int JetskiScreenLoadUI::Tick(float dt)
{
    if (m_frames > 3)  // "> 3", not ">= 4": mwcc tests via $at
        m_loadManager->SetStatus(1, 0, 0);
    m_frames++;
    return GameObject::Tick(dt);
}


// ---- src/JetskiScreenLoadUI/RenderPre3D.cpp

int JetskiScreenLoadUI::RenderPre3D()
{
    if (m_background) {
        LpRenderTarget->TrySendRenderQueue(1);
        LpRenderTarget->WaitForRenderingToComplete();
        LpRenderTarget->Blt(0, m_background, 0, 0);
        LpRenderTarget->TrySendRenderQueue(1);
        LpRenderTarget->WaitForRenderingToComplete();
    }
    return GameObject::RenderPre3D();
}


// ---- src/JetskiScreenLoadEvent/ctor.cpp


JetskiScreenLoadEvent::JetskiScreenLoadEvent(int enabled) : GameObject(enabled)
{
    m_loadManager = 0;
    m_background = 0;
    m_texts = 0;
    m_x50 = 0;
    m_hint = 0;
    m_titleA = 0;
    m_titleB = 0;
    m_infoA = 0;
    m_infoB = 0;
    m_textManager = 0;
    m_event = -1;
    m_numTexts[0] = 3;
    m_numTexts[1] = 3;
    m_numTexts[2] = 4;
    m_x38 = 0;
}


// ---- src/JetskiScreenLoadEvent/dtor.cpp

extern "C" void free(void *);





extern JetskiGame *LpGame;


JetskiScreenLoadEvent::~JetskiScreenLoadEvent()
{
    JetskiGameOptions *options = LpGame->m_gameOptions;
    LpRenderTarget->TrySendRenderQueue(1);
    LpRenderTarget->WaitForRenderingToComplete();
    options->m_x86C = 0;
    if (m_background) {
        m_background->Release();
        m_background = 0;
    }
    if (m_event != -1) {
        for (int i = 0; i < m_numTexts[m_event]; i++) {
            if (m_texts[i])
                delete m_texts[i];
        }
        free(m_texts);
        m_texts = 0;
    }
    if (m_x50) {
        delete m_x50;
        m_x50 = 0;
    }
    if (m_hint) {
        delete m_hint;
        m_hint = 0;
    }
}


// ---- src/JetskiScreenLoadEvent/Create.cpp
// Notes from matching:
// - Every GetString result and SetColor argument here is an inline-function temporary; the named
//   locals assigned from them are coalesced away, so the $s colours follow the temporaries.
// - Case 1's sprintf branches each declare their own laps/a/b: one shared set of variables made
//   the 0xC4 string (raceType 6) and the 0xC7/0xC6 strings one register node; retail keeps them apart.
// - case 5/4 write SetColor out through a local pointer and declare color before font: through
//   the inline SetColor the color value became the inline's temporary (numbered below font).
// - `string` is one variable shared by the case 5 and case 4 loops (retail clears it up front).
// - i is declared before options, so options numbers below i and retail's i $s0, options $s1.
// - The loading hint is picked with 28.0f * (rand() / 2^31): one of 28 hints from 0xCE.
#pragma fast_fptosi on


extern "C" int sprintf(char *buf, const char *fmt, ...);
extern "C" char *strcpy(char *dst, const char *src);
extern "C" char *strncpy(char *dst, const char *src, unsigned int n);
extern "C" unsigned int strlen(const char *s);
extern "C" void *calloc(unsigned int n, unsigned int size);
extern "C" int rand(void);

extern JetskiGame *LpGame;


int GetGameTextIDFromCourse(int course);
int GetGameTextIDFromSkillLevel(int level);

class PSXTextureMap;


#define SET_COLOR(text, value)                  \
    {                                           \
        unsigned int color = value;             \
        ScreenText *colored = (text);           \
        colored->SetColor(color);               \
    }

JetskiScreenLoadEvent *JetskiScreenLoadEvent::Create(JetskiLoadManager *loadManager)
{
    char path[0x104];
    char text[0x104];
    char suffix[0xC];
    int track;
    int i;
    JetskiGameOptions *options;
    ScreenTextFontItem *item;
    char *string;

    LpRenderTarget->TrySendRenderQueue(1);
    LpRenderTarget->WaitForRenderingToComplete();
    options = LpGame->m_gameOptions;
    m_loadManager = loadManager;
    switch (options->m_raceType) {
    case 4:
    case 5:
    case 8:
        m_event = 0;
        break;
    case 1:
    case 2:
    case 3:
    case 6:
        m_event = 1;
        break;
    case 7:
        m_event = 2;
        break;
    }
    LpRenderTarget->m_hasDepth = 0;
    m_textManager = (new ScreenTextManager(0, 1))->Create(0x50);
    AddChild(m_textManager, -1);
    if (m_event != -1)
        m_texts = (ScreenText **)calloc(m_numTexts[m_event] + 1, PS2_PTR_SIZE);
    m_infoA = (item = LpGame->m_fonts->GetScreenTextFont("info_a")) ? item->m_font : 0;
    m_infoB = (item = LpGame->m_fonts->GetScreenTextFont("info_b")) ? item->m_font : 0;
    m_titleB = (item = LpGame->m_fonts->GetScreenTextFont("title_b")) ? item->m_font : 0;
    m_titleA = (item = LpGame->m_fonts->GetScreenTextFont("title_a")) ? item->m_font : 0;
    track = options->m_x2C + 1;
    strcpy(suffix, options->m_difficulty == 3 ? "H.tga" : "E.tga");
    string = 0;
    if (options->m_x86C) {
        switch (options->m_raceType) {
        case 1:
        case 2:
        case 3:
        case 6:
            switch (options->m_language) {
            case 1:
                strcpy(path, "GamStart\\load_f.tga");
                break;
            case 2:
                strcpy(path, "GamStart\\load_g.tga");
                break;
            case 3:
                strcpy(path, "GamStart\\load_i.tga");
                break;
            case 4:
                strcpy(path, "GamStart\\load_s.tga");
                break;
            case 0:
                strcpy(path, "GamStart\\load_e.tga");
                break;
            case 5:
                strcpy(path, "GamStart\\load_u.tga");
                break;
            }
            break;
        case 7: {
            char *riders[9] = { "ap", "hh", "rm", "kt", "jl", "sk", "jw", "ar", "s" };
            sprintf(path, "GamStart\\Chal\\%sv%s", riders[options->m_team->m_character], riders[options->m_challengerRider]);
            break;
        }
        case 4:
            strcpy(path, "GamStart\\Countdwn.tga");
            break;
        case 5:
            strcpy(path, "GamStart\\Copycat.tga");
            break;
        case 8:
            strcpy(path, "GamStart\\Training.tga");
            break;
        }
    } else {
        switch (options->m_raceType) {
        case 1:
            sprintf(path, "GamStart\\TrackYes\\Track%d%s", track, suffix);
            break;
        case 2:
            sprintf(path, "GamStart\\TrackYes\\Track%d%s", track, suffix);
            break;
        case 3:
            sprintf(path, "GamStart\\TrackYes\\Track%d%s", track, suffix);
            break;
        case 6:
            sprintf(path, "GamStart\\TrackYes\\Track%d%s", track, suffix);
            break;
        case 7: {
            char *riders[9] = { "ap", "hh", "rm", "kt", "jl", "sk", "jw", "ar", "s" };
            sprintf(path, "GamStart\\Chal\\%sv%s", riders[options->m_team->m_character], riders[options->m_challengerRider]);
            break;
        }
        case 4:
            strcpy(path, "GamStart\\Countdwn.tga");
            break;
        case 5:
            strcpy(path, "GamStart\\Copycat.tga");
            break;
        case 8:
            strcpy(path, "GamStart\\Training.tga");
            break;
        }
    }
    m_loadManager->LoadBackground(path, &m_background);
    float yOffset = 30.0f;
    switch (options->m_raceType) {
    case 5: {
        _D3DVECTOR position;
        float scale;
        int justification;
        unsigned int color;
        ScreenTextFont *font;
        for (i = 0; i < m_numTexts[m_event]; i++) {
            position = _D3DVECTOR(0.0f, 0.0f, 0.0f);
            scale = 1.0f;
            justification = ScreenTextManager::LeftJustification;
            font = m_titleB;
            color = 0xFFFFFFFF;
            switch (i) {
            case 0:
                string = LpGame->m_text->GetString(GetGameTextIDFromCourse(LpGame->m_gameOptions->m_x2C + 1));
                position = _D3DVECTOR(32.0f, 58.0f, 0.0f);
                font = m_titleB;
                break;
            case 1:
                string = LpGame->m_text->GetString(0xEF);
                position = _D3DVECTOR(50.0f, 60.0f + yOffset, 0.0f);
                font = m_infoA;
                break;
            case 2:
                string = LpGame->m_text->GetString(0xC1);
                color = 0xFFFFDE00;
                position = _D3DVECTOR(32.0f, 20.0f, 0.0f);
                font = m_titleA;
                break;
            }
            m_texts[i] = new ScreenText(strlen(string) + 1, string);
            m_texts[i]->SetPosition(position.x, position.y);
            m_texts[i]->SetScale(scale);
            m_texts[i]->SetFont(font);
            m_texts[i]->SetJustification(justification);
            {
                ScreenText *colored = m_texts[i];
                colored->m_color = color;
                colored->m_x5D_bit4 = 1;
            }
            m_textManager->PositionText(m_texts[i]);
            if (i == 0)
                SET_COLOR(m_texts[i], 0xFFFFDE00)
        }
        m_x50 = new JetskiScreenText;
        m_x50->m_params.m_color = 0xFFFFFFFF;
        m_x50->m_params.m_font = m_infoB;
        m_x50->m_params.m_justification = ScreenTextManager::LeftJustification;
        m_x50->m_params.m_lineSpacing = 20;
        m_x50->m_params.m_x = 50;
        m_x50->m_params.m_y = 130;
        m_x50->SetText(0xF0, 0);
        break;
    }
    case 4: {
        _D3DVECTOR position;
        for (i = 0; i < m_numTexts[m_event]; i++) {
            position = _D3DVECTOR(0.0f, 0.0f, 0.0f);
            float scale = 1.0f;
            int justification = ScreenTextManager::LeftJustification;
            unsigned int color;
            ScreenTextFont *font = m_infoB;
            color = 0xFFFFFFFF;
            switch (i) {
            case 0:
                string = LpGame->m_text->GetString(GetGameTextIDFromCourse(LpGame->m_gameOptions->m_x2C + 1));
                position = _D3DVECTOR(32.0f, 58.0f, 0.0f);
                font = m_titleB;
                break;
            case 1:
                string = LpGame->m_text->GetString(0xED);
                position = _D3DVECTOR(50.0f, 63.0f + yOffset, 0.0f);
                font = m_infoA;
                break;
            case 2:
                string = LpGame->m_text->GetString(0xC1);
                color = 0xFFFFDE00;
                position = _D3DVECTOR(32.0f, 20.0f, 0.0f);
                font = m_titleA;
                break;
            }
            m_texts[i] = new ScreenText(strlen(string) + 1, string);
            m_texts[i]->SetPosition(position.x, position.y);
            m_texts[i]->SetScale(scale);
            m_texts[i]->SetFont(font);
            m_texts[i]->SetJustification(justification);
            {
                ScreenText *colored = m_texts[i];
                colored->m_color = color;
                colored->m_x5D_bit4 = 1;
            }
            m_textManager->PositionText(m_texts[i]);
            if (i == 0)
                SET_COLOR(m_texts[i], 0xFFFFDE00)
        }
        m_x50 = new JetskiScreenText;
        m_x50->m_params.m_color = 0xFFFFFFFF;
        m_x50->m_params.m_font = m_infoB;
        m_x50->m_params.m_justification = ScreenTextManager::LeftJustification;
        m_x50->m_params.m_lineSpacing = 20;
        m_x50->m_params.m_x = 50;
        m_x50->m_params.m_y = 130;
        m_x50->SetText(0xEE, 0);
        break;
    }
    case 8:
        for (i = 0; i < m_numTexts[m_event]; i++) {
            switch (i) {
            case 0:
                m_texts[i] = new ScreenText(0x3D, "");
                m_texts[i]->SetString(LpGame->m_text->GetString(0xC0));
                m_texts[i]->SetPosition(32.0f, 58.0f);
                SET_COLOR(m_texts[i], 0xFFFFDE00)
                m_texts[i]->SetFont(m_titleB);
                m_texts[i]->SetJustification(ScreenTextManager::LeftJustification);
                m_textManager->PositionText(m_texts[i]);
                break;
            case 2:
                m_texts[i] = new ScreenText(0x3D, "");
                m_texts[i]->SetString(LpGame->m_text->GetString(0xC1));
                m_texts[i]->SetPosition(32.0f, 20.0f);
                m_texts[i]->SetFont(m_titleA);
                SET_COLOR(m_texts[i], 0xFFFFDE00)
                m_texts[i]->SetJustification(ScreenTextManager::LeftJustification);
                m_textManager->PositionText(m_texts[i]);
                break;
            }
        }
        break;
    case 1:
    case 2:
    case 3:
    case 6:
    case 7:
        m_hint = new JetskiScreenText;
        m_hint->m_params.m_color = 0xFFFFFFFF;
        m_hint->m_params.m_font = m_infoA;
        m_hint->m_params.m_justification = ScreenTextManager::LeftJustification;
        m_hint->m_params.m_scale = 0.8f;
        m_hint->m_params.m_lineSpacing = 20;
        m_hint->m_params.m_x = 29;
        m_hint->m_params.m_y = 360;
        m_hint->SetText((int)(28.0f * (rand() * 4.656613e-10f)) + 0xCE, 0);
        for (i = 0; i < m_numTexts[m_event]; i++) {
            m_texts[i] = new ScreenText(0x3D, "");
            m_texts[i]->SetJustification(ScreenTextManager::LeftJustification);
            switch (i) {
            case 0:
                m_texts[i]->SetString(LpGame->m_text->GetString(GetGameTextIDFromCourse(LpGame->m_gameOptions->m_x2C + 1)));
                m_texts[i]->SetPosition(32.0f, 58.0f);
                SET_COLOR(m_texts[i], 0xFFFFDE00)
                m_texts[i]->SetFont(m_titleB);
                m_textManager->PositionText(m_texts[i]);
                break;
            case 1: {
                if (options->m_raceType == 6) {
                    char *laps;
                    if (LpGame->m_gameOptions->m_x30 > 1)
                        laps = LpGame->m_text->GetString(0xC2);
                    else
                        laps = LpGame->m_text->GetString(0x47);
                    char *a = LpGame->m_text->GetString(0xC4);
                    char *b = LpGame->m_text->GetString(0xCC);
                    sprintf(text, "%s %d %s %d - %s - %d %s", a, LpGame->m_gameOptions->m_career->m_currentCircuit + 1,
                            b, LpGame->m_gameOptions->m_career->m_numEvents,
                            LpGame->m_text->GetString(GetGameTextIDFromSkillLevel(LpGame->m_gameOptions->m_difficulty)),
                            LpGame->m_gameOptions->m_x30, laps);
                } else if (options->m_raceType == 1) {
                    char *laps;
                    if (LpGame->m_gameOptions->m_x30 > 1)
                        laps = LpGame->m_text->GetString(0xC2);
                    else
                        laps = LpGame->m_text->GetString(0x47);
                    char *b = LpGame->m_text->GetString(0xC5);
                    sprintf(text, "%s - %s - %d %s", b,
                            LpGame->m_text->GetString(GetGameTextIDFromSkillLevel(LpGame->m_gameOptions->m_difficulty)),
                            LpGame->m_gameOptions->m_x30, laps);
                } else if (options->m_raceType == 3) {
                    char *a = LpGame->m_text->GetString(0xC7);
                    sprintf(text, "%s - %s", a,
                            LpGame->m_text->GetString(GetGameTextIDFromSkillLevel(LpGame->m_gameOptions->m_difficulty)));
                } else if (options->m_raceType == 2) {
                    char *a = LpGame->m_text->GetString(0xC6);
                    sprintf(text, "%s - %s", a,
                            LpGame->m_text->GetString(GetGameTextIDFromSkillLevel(LpGame->m_gameOptions->m_difficulty)));
                } else if (options->m_raceType == 7) {
                    char *laps;
                    if (LpGame->m_gameOptions->m_x30 > 1)
                        laps = LpGame->m_text->GetString(0xC2);
                    else
                        laps = LpGame->m_text->GetString(0x47);
                    char *b = LpGame->m_text->GetString(0xCA);
                    sprintf(text, "%s - %s - %d %s", b,
                            LpGame->m_text->GetString(GetGameTextIDFromSkillLevel(LpGame->m_gameOptions->m_difficulty)),
                            LpGame->m_gameOptions->m_x30, laps);
                } else {
                    strcpy(text, "");
                }
                m_texts[i]->SetString(text);
                m_texts[i]->SetPosition(32.0f, 90.0f);
                m_texts[i]->SetScale(1.0f);
                m_texts[i]->SetFont(m_infoB);
                m_textManager->PositionText(m_texts[i]);
                break;
            }
            case 2:
                m_texts[i]->SetString(LpGame->m_text->GetString(0xC1));
                m_texts[i]->SetPosition(32.0f, 20.0f);
                m_texts[i]->SetFont(m_titleA);
                SET_COLOR(m_texts[i], 0xFFFFDE00)
                m_textManager->PositionText(m_texts[i]);
                break;
            case 3:
                m_texts[i]->SetString(LpGame->m_text->GetString(0xCD));
                m_texts[i]->SetPosition(299.0f, 192.0f);
                m_texts[i]->SetFont(m_titleB);
                SET_COLOR(m_texts[i], 0xFFFFDE00)
                m_textManager->PositionText(m_texts[i]);
                break;
            }
        }
        break;
    }
    return this;
}


#pragma fast_fptosi reset

// ---- src/JetskiScreenLoadEvent/Tick.cpp

int JetskiScreenLoadEvent::Tick(float dt)
{
    if (m_x38 > 3)
        m_loadManager->SetStatus(2, 0, 0);
    m_x38++;
    return GameObject::Tick(dt);
}

// ---- src/JetskiScreenLoadEvent/RenderPre3D.cpp


int JetskiScreenLoadEvent::RenderPre3D()
{
    if (m_background) {
        LpRenderTarget->TrySendRenderQueue(1);
        LpRenderTarget->WaitForRenderingToComplete();
        LpRenderTarget->Blt(0, m_background, 0, 0);
        LpRenderTarget->TrySendRenderQueue(1);
        LpRenderTarget->WaitForRenderingToComplete();
    }
    return GameObject::RenderPre3D();
}


// ---- src/JetskiScreenLoadEvent/Render3D.cpp

class ScreenTextFont;





extern JetskiGame *LpGame;


int JetskiScreenLoadEvent::Render3D()
{
    JetskiGameOptions *options = LpGame->m_gameOptions;
    int i;
    LpRenderTarget->TrySendRenderQueue(1);
    LpRenderTarget->WaitForRenderingToComplete();
    if (options->m_x86C) {
        switch (options->m_raceType) {
        case 7:
        case 4:
        case 5:
        case 8:
            for (i = 0; i < m_numTexts[m_event]; i++) {
                if (m_texts[i])
                    m_textManager->AddText(m_texts[i], 0);
            }
            if (m_x50)
                m_x50->AddText(m_textManager);
            break;
        }
    } else {
        for (i = 0; i < m_numTexts[m_event]; i++) {
            if (m_texts[i])
                m_textManager->AddText(m_texts[i], 0);
        }
        if (m_x50)
            m_x50->AddText(m_textManager);
    }
    if (m_hint)
        m_hint->AddText(m_textManager);
    LpRenderTarget->TrySendRenderQueue(1);
    LpRenderTarget->WaitForRenderingToComplete();
    return GameObject::Render3D();
}


// ---- src/JetskiScreenVideo/ctor.cpp

JetskiScreenVideo::JetskiScreenVideo(int enabled) : GameObject(enabled)
{
    m_loadManager = 0;
    m_video = 0;
}

// ---- src/JetskiScreenVideo/dtor.cpp


JetskiScreenVideo::~JetskiScreenVideo()
{
    if (LpRenderTarget) {
        LpRenderTarget->TrySendRenderQueue(1);
        LpRenderTarget->WaitForRenderingToComplete();
    }
}


// ---- src/JetskiScreenVideo/Create.cpp
class ProcessNode;
class KeyInfo;



extern RenderTarget *LpRenderTarget;

extern "C" char *strcpy(char *dst, const char *src);
extern "C" char *strcat(char *dst, const char *src);




JetskiScreenVideo *JetskiScreenVideo::Create(JetskiLoadManager *loadManager)
{
    char path[0xA0];
    strcpy(path, "UI\\NTSC\\");
    LpRenderTarget->m_hasDepth = 0;
    m_loadManager = loadManager;
    if (!m_video) {
        m_video = new PSXMediaControl(1);
        if (m_video) {
            switch (m_loadManager->m_progress) {
            case 0:
                strcat(path, "Videos\\EndAP.pss");
                break;
            case 4:
                strcat(path, "Videos\\EndJL.pss");
                break;
            case 2:
                strcat(path, "Videos\\EndRM.pss");
                break;
            case 3:
                strcat(path, "Videos\\EndKT.pss");
                break;
            case 1:
                strcat(path, "Videos\\EndHH.pss");
                break;
            case 5:
                strcat(path, "Videos\\EndSK.pss");
                break;
            case 6:
                strcat(path, "Videos\\EndJW.pss");
                break;
            case 7:
                strcat(path, "Videos\\EndAR.pss");
                break;
            case 8:
                strcat(path, "Videos\\EndS.pss");
                break;
            case 13:
                strcat(path, "Videos\\Credits.pss");
                break;
            case 9:
                strcpy(path, "AppStart\\NTSC\\Rainbow.pss");
                break;
            case 10:
                strcpy(path, "AppStart\\NTSC\\Atari.pss");
                break;
            case 11:
                strcpy(path, "AppStart\\NTSC\\attract.pss");
                break;
            case 12:
                break;
            }
            m_video->Create(path, 0, 0, 0, 0, 0, 0);
            AddChild(m_video, -1);
            m_video->SetColor(0, 2, 2, 2, 0x80);
            m_video->Start();
            return this;
        }
    }
    return 0;
}


// ---- src/JetskiScreenVideo/Tick.cpp


int JetskiScreenVideo::Tick(float dt)
{
    if (!m_video->IsPlaying())
        ExitVideo();
    return GameObject::Tick(dt);
}


// ---- src/JetskiScreenVideo/KeyDownHandler.cpp
class KeyInfo;


int KEYDOWN_HANDLER_TEST(AssignableActionIndex action, DeviceType device, ProcessNode *node, unsigned int flags);




int JetskiScreenVideo::KeyDownHandler(ProcessNode *node, KeyInfo *key)
{
    if (KEYDOWN_HANDLER_TEST((AssignableActionIndex)4, (DeviceType)2, node, 0x80000000)) {
        if (!node->m_controller) {
            switch (m_loadManager->m_progress) {
            case 9:
            case 10:
                break;
            default:
                ExitVideo();
                break;
            }
        }
    }
    return GameObject::KeyDownHandler(node, key);
}


// ---- src/JetskiScreenVideo/ExitVideo.cpp




void JetskiScreenVideo::ExitVideo()
{
    m_video->Stop();
    m_video->RemoveFromChain();
    m_video->Release();
    m_video = 0;
    switch (m_loadManager->m_total) {
    case 7:
        switch (m_loadManager->m_progress) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
            m_loadManager->SetStatus(7, 13, 3);
            break;
        case 9:
            break;
        case 10:
            m_loadManager->SetStatus(7, 9, 5);
            break;
        }
        break;
    default:
        m_loadManager->SetStatus(m_loadManager->m_total, 0, 0);
        break;
    }
}


// ---- src/global/GetGameTextIDFromCourse.cpp
void PSXprintf(const char *fmt, ...);

// Game text ID of each course name.
int GetGameTextIDFromCourse(int course)
{
    switch (course) {
    case 1:
        return 177;
    case 2:
        return 175;
    case 3:
        return 188;
    case 4:
        return 181;
    case 5:
        return 182;
    case 9:
        return 179;
    case 7:
        return 176;
    case 8:
        return 186;
    case 6:
        return 185;
    case 11:
        return 180;
    case 10:
        return 183;
    case 12:
        return 191;
    case 15:
        return 184;
    case 14:
        return 189;
    case 13:
        return 172;
    case 16:
        return 187;
    case 18:
        return 173;
    case 17:
        return 190;
    case 19:
        return 171;
    case 20:
        return 178;
    case 21:
        return 174;
    default:
        PSXprintf("Invalid course number (%d) in GetGameTextIDFromCourse!\n", course);
        return -1;
    }
}


// ---- src/global/GetGameTextIDFromSkillLevel.cpp
void PSXprintf(const char *fmt, ...);

int GetGameTextIDFromSkillLevel(int skillLevel)
{
    switch (skillLevel) {
    case 1:
        return 234;
    case 2:
        return 235;
    case 3:
        return 236;
    default:
        PSXprintf("Invalid difficulty number (%d) in GetGameTextIDFromSkillLevel!\n", skillLevel);
        return -1;
    }
}

