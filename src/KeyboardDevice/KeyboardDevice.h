// KeyboardDevice: the keyboard input device (no PS2 implementation; only the
// members and vtable slots ControlInterface.cpp and InputDeviceType.cpp use).
// InputDevice's data comes first, so the vptr sits after it at +0x18. Merged
// from those files' local views (IsKeyDown, Update, KEYDOWN_HANDLER_TEST,
// IsModsDown).
#ifndef KEYBOARDDEVICE_KEYBOARDDEVICE_H
#define KEYBOARDDEVICE_KEYBOARDDEVICE_H

#include "ControlInterface/ControlTypes.h"
#include "KeyInfo/KeyInfo.h"

class KeyboardDevice {
public:
    bool IsModsDown(unsigned int mods);

    char pad0[0x18];
    virtual void Slot0();               // vptr +0x18
    virtual void Slot1();
    virtual void Slot2();
    virtual void Slot3();
    virtual int IsKeyDown(unsigned int key, unsigned int mask, KeyInfo *info);
    virtual void Update(float dt);
    virtual int IsKeyDown(AssignableActionIndex action, unsigned int mask, KeyInfo *info);
    char pad1C[0x20 - 0x1C];
    KeyInfo m_keys[256];                // +0x20 (IsModsDown tests key 0x35's m_down, +0x518)
    char pad1820[0x1898 - 0x1820];
    int m_modifiers;                    // +0x1898
};

#endif
