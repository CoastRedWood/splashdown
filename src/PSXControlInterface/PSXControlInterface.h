// PSXControlInterface: the PS2 joypad manager, a ControlInterface (vptr at
// +0xCC0) adding m_numJoysticks at +0xCC4. 0xCC8 bytes (`new` in
// Game::PreGameInitialize).
#ifndef PSXCONTROLINTERFACE_H
#define PSXCONTROLINTERFACE_H

#include "ControlInterface/ControlInterface.h"
#include "PSXJoystickDevice/PSXJoystickDevice.h"

// One scePad DMA area (scePadDmaBufferMax quadwords) per ports/pc/slot opened
// (.bss 0x474080, defined in src/tu/PSXControlInterface.cpp).
extern unsigned long PadDmaBuffer[5][32];

extern "C" int scePadPortOpen(int port, int slot, unsigned long *addr);

class PSXControlInterface : public ControlInterface {
public:
    PSXControlInterface();
    virtual ~PSXControlInterface();
    int Create(int numJoysticks);

    int m_numJoysticks;                 // +0xCC4
};

#endif
