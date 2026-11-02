// MediaControl.cpp  @ 0x002CC710 (text 0x2CC710-0x2CC9B0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp)
//
// Retail's MediaControl.cpp as one translation unit: a TU without a
// static initializer inside PSXWater.cpp's tu_map row, after
// TextOverlay.cpp. Its .rodata is MediaControl's RTTI block alone
// (0x41F8B8, restarting at BaseObject after TextOverlay's "Overlay"); no
// function has a literal. `make link` places its data as well as its
// code.

#include "MediaControl/MediaControl.h"
#include "GameObject/GameObject.h"

// ---- src/MediaControl/ctor.cpp

MediaControl::MediaControl(int enabled)
    : GameObject(enabled)
{
    m_30 = 0;
    m_34 = 0;
    m_callback = 0;
    m_context = 0;
    m_40 = 0;
}


// ---- src/MediaControl/dtor.cpp

MediaControl::~MediaControl()
{
}


// ---- src/MediaControl/Create.cpp

MediaControl *MediaControl::Create(char *, void (*callback)(void *), void *context)
{
    GameObject::Create();
    m_callback = callback;
    m_context = context;
    return this;
}


// ---- src/MediaControl/Tick.cpp


int MediaControl::Tick(float dt)
{
    return GameObject::Tick(dt);
}


// ---- src/MediaControl/Disable.cpp

void MediaControl::Disable()
{
    GameObject::Disable();
    Pause(1);
}


// ---- src/MediaControl/Enable.cpp

void MediaControl::Enable()
{
    GameObject::Enable();
    Pause(0);
}


// ---- src/MediaControl/AncestorsDisabled.cpp

void MediaControl::AncestorsDisabled()
{
    GameObject::AncestorsDisabled();
    Pause(1);
}


// ---- src/MediaControl/AncestorsEnabled.cpp

void MediaControl::AncestorsEnabled()
{
    GameObject::AncestorsEnabled();
    Pause(0);
}


// ---- src/MediaControl/LostFocus.cpp

int MediaControl::LostFocus()
{
    Pause(1);
    return GameObject::LostFocus();
}


// ---- src/MediaControl/Restore.cpp

int MediaControl::Restore()
{
    Pause(0);
    return GameObject::Restore();
}

