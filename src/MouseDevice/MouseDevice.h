// MouseDevice: the mouse input device (no PS2 implementation; only the
// members and vtable slots ControlInterface.cpp uses). InputDevice's data
// comes first, so the vptr sits after it at +0x18.
#ifndef MOUSEDEVICE_MOUSEDEVICE_H
#define MOUSEDEVICE_MOUSEDEVICE_H

#include "ControlInterface/ControlTypes.h"
#include "KeyInfo/KeyInfo.h"

class MouseDevice {
public:
    char pad0[0x18];
    virtual void Slot0();               // vptr +0x18
    virtual void Slot1();
    virtual void Slot2();
    virtual void Slot3();
    virtual int IsKeyDown(unsigned int key, unsigned int mask, KeyInfo *info);
    virtual int IsKeyDown(AssignableActionIndex action, unsigned int mask, KeyInfo *info);
    virtual void Update(float dt);
    KeyInfo m_keys[8];                  // +0x1C (count not known)
};

#endif
