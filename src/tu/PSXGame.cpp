// PSXGame.cpp  @ 0x002703B0 (text 0x2703B0-0x270850)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp)
//
// Retail's PSXGame.cpp as one translation unit, a TU without an initializer
// inside PSXCamera.cpp's tu_map row. Its .rodata (0x416BE0-0x416C78) is
// Initialize's and SetDefaultVideoMode's strings (@543, @608, after
// PSXControlInterface.cpp's RTTI) and PSXGame's RTTI; PSXInputDevice.cpp's
// RTTI follows with a label restart (@436). `make link` places its data as
// well as its code.


#include "Game/Game.h"
#include "ControlInterface/ControlTypes.h"
#include "Camera/Camera.h"
#include "RenderTarget/RenderTarget.h"
#include "PSXRenderTarget/PSXRenderTarget.h"
#include "PSXVideoCard/PSXVideoCard.h"
#include "PSXGame/PSXGame.h"

extern "C" char *strcpy(char *dst, const char *src);
void PSXprintf(const char *fmt, ...);
int KEYDOWN_HANDLER_TEST(unsigned int key, DeviceType device, ProcessNode *node, unsigned int mods);

extern int Wireframe;

// ---- src/PSXGame/ctor.cpp

PSXGame::PSXGame()
{
}

// ---- src/PSXGame/dtor.cpp

PSXGame::~PSXGame()
{
}

// ---- src/PSXGame/PreGameInitialize.cpp

int PSXGame::PreGameInitialize()
{
    return Game::PreGameInitialize();
}

// ---- src/PSXGame/Initialize.cpp

int PSXGame::Initialize(char *errorMessage)
{
    GetDisplayModes();
    if (!FilterDisplayModes(0)) {
        if (errorMessage)
            strcpy(errorMessage, "PSXGame::Initialize() didn't find a suitable mode");
        return 0;
    }
    LpVideoCard = new PSXVideoCard;
    LpVideoCard->Create();
    return Game::Initialize(errorMessage);
}

// ---- src/PSXGame/KeyDownHandler.cpp

int PSXGame::KeyDownHandler(ProcessNode *node, KeyInfo *key)
{
    if (Game::KeyDownHandler(node, key))
        return 1;
    if (m_debugKeys && KEYDOWN_HANDLER_TEST(0x40, (DeviceType)0, node, 3)) {
        Wireframe = 1 - Wireframe;
        return 1;
    }
    return 0;
}

// ---- src/PSXGame/ChangeVideoMode.cpp

int PSXGame::ChangeVideoMode(int mode)
{
    if (mode == -1)
        return 0;
    Camera *camera = 0;
    if (LpRenderTarget)
        camera = LpRenderTarget->m_camera;
    if (LpRenderTarget) {
        delete LpRenderTarget;
        LpRenderTarget = 0;
    }
    LpVideoCard->SetVideoMode(mode);
    if (!PostVideoModeChange())
        return 0;
    if (LpRenderTarget && camera) {
        LpRenderTarget->SetCamera(camera);
        LpRenderTarget->SetViewport(&camera->m_viewport);
    }
    return 1;
}

// ---- src/PSXGame/FilterDisplayModes.cpp

int PSXGame::FilterDisplayModes(PSXVideoCard *)
{
    return 1;
}

// ---- src/PSXGame/SetBasicRenderStates.cpp

int PSXGame::SetBasicRenderStates()
{
    return 1;
}

// ---- src/PSXGame/PrepareFrame.cpp

int PSXGame::PrepareFrame()
{
    return Game::PrepareFrame();
}

// ---- src/PSXGame/CreateRenderTarget.cpp

PSXRenderTarget *PSXGame::CreateRenderTarget()
{
    return (new PSXRenderTarget)->Create(LpVideoCard, 1);
}

// ---- src/PSXGame/SetDefaultVideoMode.cpp

int PSXGame::SetDefaultVideoMode()
{
    if (!((VideoCard *)LpVideoCard)->SetVideoMode(640, 448, 32, 2, 0, 1, 1)) {
        PSXprintf("PSXGame::SetDefaultVideoMode() failure\n");
        return 0;
    }
    return PostVideoModeChange() != 0;
}

// (GetDisplayModes and GetRegistryInt were in SetBasicRenderStates.cpp.)

int PSXGame::GetDisplayModes()
{
    return 1;
}

// The PS2 has no registry: getters return the default, setters fail.

int PSXGame::GetRegistryInt(const char *, int def)
{
    return def;
}

// ---- src/PSXGame/GetRegistryFloat.cpp

float PSXGame::GetRegistryFloat(const char *, float def)
{
    return def;
}

// ---- src/PSXGame/GetRegistryBOOL.cpp

int PSXGame::GetRegistryBOOL(const char *, int def)
{
    return def;
}

// ---- src/PSXGame/GetRegistryString.cpp

int PSXGame::GetRegistryString(const char *, const char *def, char *out, unsigned int *)
{
    strcpy(out, def);
    return 0;
}

// ---- src/PSXGame/GetRegistryData.cpp

int PSXGame::GetRegistryData(const char *, void *, unsigned int *)
{
    return 0;
}

// ---- src/PSXGame/SetRegistryInt.cpp

int PSXGame::SetRegistryInt(const char *, int)
{
    return 0;
}

// ---- src/PSXGame/SetRegistryFloat.cpp

int PSXGame::SetRegistryFloat(const char *, float)
{
    return 0;
}

// ---- src/PSXGame/SetRegistryBOOL.cpp

int PSXGame::SetRegistryBOOL(const char *, int)
{
    return 0;
}

// ---- src/PSXGame/SetRegistryString.cpp

int PSXGame::SetRegistryString(const char *, const char *)
{
    return 0;
}

// ---- src/PSXGame/SetRegistryData.cpp

int PSXGame::SetRegistryData(const char *, void *, int)
{
    return 0;
}
