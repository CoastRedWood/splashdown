// JetskiUIGlobalManager.cpp  @ 0x00315B20 (text 0x315B20-0x315F30)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's JetskiUIGlobalManager.cpp as one translation unit: its functions in
// retail order, then its static initializer. `make link` places the object's
// data as well as its code (tools/link_c.py). The UIGlobalManager functions
// after it in its tu_map row are UIGlobalManager.cpp, a TU of its own without
// an initializer (its .rodata repeats the BaseObject/GameObject RTTI records):
// src/tu/UIGlobalManager.cpp.



#include "UIGlobalManager/UIGlobalManager.h"
#include "SurfaceShaderList/SurfaceShaderList.h"
#include "JetskiUIGlobalManager/JetskiUIGlobalManager.h"
#include "PSXTextureMapManager/PSXTextureMapManager.h"
#include "JetskiGameOptions/JetskiGameOptions.h"
#include "JetskiGame/JetskiGame.h"
#include "PSXSurfaceShaderList/PSXSurfaceShaderList.h"
#include "JetskiUIManMain/JetskiUIManMain.h"
#include "JetskiUIManCard/JetskiUIManCard.h"
#include "GameObject/GameObject.h"
#include "UIManager/UIManager.h"
#include "sinit/AxisVectors.h"

void PSXprintf(const char *fmt, ...);
extern SurfaceShaderList *LpSurfaceShaderList;
extern PSXTextureMapManager *LpTextureMapManager;
extern JetskiGame *LpGame;

// ---- src/JetskiUIGlobalManager/ctor.cpp

JetskiUIGlobalManager::JetskiUIGlobalManager(int enabled) : UIGlobalManager(enabled)
{
}


// ---- src/JetskiUIGlobalManager/dtor.cpp


JetskiUIGlobalManager::~JetskiUIGlobalManager()
{
    if (LpSurfaceShaderList) {
        delete LpSurfaceShaderList;
        LpSurfaceShaderList = 0;
    }
}


// ---- src/JetskiUIGlobalManager/AllocateIpuCache.cpp


void JetskiUIGlobalManager::AllocateIpuCache()
{
    LpTextureMapManager->AllocateIpuCache(0x100000);
}


// ---- src/JetskiUIGlobalManager/FreeIpuCache.cpp


void JetskiUIGlobalManager::FreeIpuCache()
{
    LpTextureMapManager->FreeIpuCache();
}


// ---- src/JetskiUIGlobalManager/Create.cpp


UIGlobalManager *JetskiUIGlobalManager::Create(int mode, int a2)
{
    if (!LoadScreenTextManager())
        return 0;
    if (LpGame->m_gameOptions->m_language > -1)
        Localize(LpGame->m_gameOptions->m_language);
    switch (mode) {
    case 1:
        new PSXSurfaceShaderList;   // installs itself as LpSurfaceShaderList
        CreateUIManager(mode, a2);
        break;
    case 2:
        CreateUIManager(mode, a2);
        break;
    }
    return this;
}


// ---- src/JetskiUIGlobalManager/CreateUIManager.cpp


int JetskiUIGlobalManager::CreateUIManager(int mode, int a2)
{
    InitializeDirectories(mode);
    switch (mode) {
    case 1:
        LpGame->m_gameOptions->m_uiMode = 1;
        m_uiManager = (UIManager *)AddChild((new JetskiUIManMain(1))->Create(a2, this), -1);
        break;
    case 2:
        LpGame->m_gameOptions->m_uiMode = 2;
        m_uiManager = (UIManager *)AddChild((new JetskiUIManCard(1))->Create(a2, this), -1);
        break;
    }
    if (!m_uiManager) {
        PSXprintf("Could not create LpUIManager in UIGlobalManager::StartUI\n");
        return 0;
    }
    return 1;
}


// ---- src/JetskiUIGlobalManager/InitializeDirectories.cpp
extern "C" char *strcpy(char *dst, const char *src);

extern char UITextureContentDirectory[0x104];
extern char AnimContentDirectory[0x104];
extern char SceneContentDirectory[0x104];
extern char UIContentDirectory[0x104];


void JetskiUIGlobalManager::InitializeDirectories(int mode)
{
    strcpy(UITextureContentDirectory, "UI\\NTSC\\");
    strcpy(AnimContentDirectory, "UI\\Anim\\");
    switch (mode) {
    case 1:
        strcpy(SceneContentDirectory, "UI\\Main\\");
        strcpy(UIContentDirectory, "UI\\Main\\");
        break;
    case 2:
        strcpy(SceneContentDirectory, "UI\\Card\\");
        strcpy(UIContentDirectory, "UI\\Card\\");
        break;
    }
}


// ---- src/sinit/JetskiUIGlobalManager.cpp
// Static initializer of JetskiUIGlobalManager.cpp: only the axis vectors from the shared header.

