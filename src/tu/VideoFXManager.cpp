// VideoFXManager.cpp  @ 0x00397340 (text 0x397340-0x397970)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// VideoFXManager as one translation unit without a static initializer, in
// TextureMapStringTable.cpp's tu_map row: its .rodata is @573 (RenderPre3D's
// jump table) and the VideoFXManager RTTI (@742-@746).


#include "GameObject/GameObject.h"
#include "VideoFXManager/VideoFXManager.h"
#include "PSXVideoCard/PSXVideoCard.h"
#include "JetskiGame/JetskiGame.h"
#include "Game/Game.h"

extern JetskiGame *LpGame;
extern PSXVideoCard *LpVideoCard;

// ---- src/VideoFXManager/ctor.cpp
// Per-viewport (4) full-screen video effects.

VideoFXManager::VideoFXManager(int enabled) : GameObject(enabled)
{
    for (int i = 0; i < 4; i++) {
        m_overlayR[i] = 0;
        m_overlayG[i] = 0;
        m_overlayB[i] = 0;
        m_overlayA[i] = 0;
        m_overlayMode[i] = 0x8000000041;
        m_overlayState[i] = 0;
        m_blurAmount[i] = 0;
        m_blurState[i] = 0;
        m_duration[i] = 0;
        m_state[i] = 0;
        m_dynamicViewportFade[i] = 0;
    }
    m_dynamicScreenFade = 0;
}

// ---- src/VideoFXManager/dtor.cpp
VideoFXManager::~VideoFXManager()
{
}

// ---- src/VideoFXManager/Create.cpp
GameObject *VideoFXManager::Create()
{
    return GameObject::Create();
}

// ---- src/VideoFXManager/RenderPre3D.cpp
// Runs the requested fade for the current viewport.
int VideoFXManager::RenderPre3D()
{
    int vp = LpGame->m_currentPlayer;
    switch (m_state[vp]) {
    case 0:
        break;
    case 1:
        LpVideoCard->FadeOutViewport(m_duration[vp], 0);
        m_state[vp] = 3;
        break;
    case 2:
        LpVideoCard->FadeInViewport(m_duration[vp], 0);
        m_state[vp] = 0;
        break;
    case 3:
        if (LpVideoCard->IsViewportFaded(-1))
            m_state[vp] = 0;
        break;
    case 4:
        if (LpVideoCard->IsScreenFaded())
            m_state[vp] = 0;
        break;
    case 5:
        LpVideoCard->FadeOutScreen(m_duration[vp], 0);
        m_state[vp] = 0;
        break;
    case 6:
        LpVideoCard->FadeInScreen(m_duration[vp], 0);
        m_state[vp] = 0;
        break;
    case 7:
        LpVideoCard->FadeOutScreen(m_duration[vp], 1);
        m_state[vp] = 4;
        break;
    case 8:
        LpVideoCard->FadeInScreen(m_duration[vp], 1);
        m_state[vp] = 0;
        break;
    }
    return GameObject::RenderPre3D();
}

// ---- src/VideoFXManager/Render3D.cpp
// Applies the current viewport's pending color overlay and motion blur, then any
// dynamic fades.
int VideoFXManager::Render3D()
{
    int vp = LpGame->m_currentPlayer;
    if (m_overlayState[vp] == 1) {
        LpVideoCard->ApplyColorOverlay(m_overlayR[vp], m_overlayG[vp], m_overlayB[vp], m_overlayA[vp], m_overlayMode[vp], 0);
        LpVideoCard->ExecuteColorOverlay();
        LpVideoCard->RemoveColorOverlay();
    } else if (m_overlayState[vp] == 2) {
        LpVideoCard->ApplyColorOverlay(m_overlayR[vp], m_overlayG[vp], m_overlayB[vp], m_overlayA[vp], m_overlayMode[vp], 0);
    } else {
        LpVideoCard->RemoveColorOverlay();
    }
    m_overlayState[vp] = 0;
    if (m_blurState[vp] == 1) {
        LpVideoCard->MotionBlur(m_blurAmount[vp], 0, -1);
        LpVideoCard->ExecuteMotionBlur();
    } else if (m_blurState[vp] == 2) {
        LpVideoCard->MotionBlur(m_blurAmount[vp], 0, -1);
    }
    m_blurState[vp] = 0;
    if (m_dynamicViewportFade[vp])
        LpVideoCard->ExecuteDynamicViewportFading();
    if (m_dynamicScreenFade)
        LpVideoCard->ExecuteDynamicScreenFading();
    return GameObject::Render3D();
}

// ---- src/VideoFXManager/FadeOut.cpp
int VideoFXManager::FadeOut(int time, int viewport)
{
    if (viewport == -1)
        viewport = LpGame->m_currentPlayer;
    if (!m_state[viewport] && !LpVideoCard->IsViewportFaded(viewport)) {
        m_state[viewport] = 1;
        m_duration[viewport] = time;
        return 1;
    }
    return 0;
}

// ---- src/VideoFXManager/FadeIn.cpp
int VideoFXManager::FadeIn(int duration, int viewport)
{
    int vp = viewport;
    if (viewport == -1)
        vp = LpGame->m_currentPlayer;
    // retail checks the caller's viewport here (-1 when defaulted), not vp
    if (m_state[vp] == 3 && LpVideoCard->IsViewportFaded(viewport))
        m_state[vp] = 0;
    if (m_state[vp] == 0 && LpVideoCard->IsViewportFaded(vp)) {
        m_state[vp] = 2;
        m_duration[vp] = duration;
        return 1;
    }
    return 0;
}

