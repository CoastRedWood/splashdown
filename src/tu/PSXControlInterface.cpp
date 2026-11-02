// PSXControlInterface.cpp  @ 0x00270160 (text 0x270160-0x2703B0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp)
//
// Retail's PSXControlInterface.cpp as one translation unit, a TU without an
// initializer inside PSXCamera.cpp's tu_map row. Its .rodata is
// PSXControlInterface's RTTI (0x416B80-0x416BE0): the labels restart (@534)
// after PSXCamera.cpp's, and PSXGame.cpp's strings follow (@543). `make
// link` places its data as well as its code.


#include "ControlInterface/ControlInterface.h"
#include "KeyboardDevice/KeyboardDevice.h"
#include "JoystickDevice/JoystickDevice.h"
#include "PSXJoystickDevice/PSXJoystickDevice.h"
#include "PSXControlInterface/PSXControlInterface.h"

// libpad's DMA areas (.bss 0x474080, 64-byte aligned as scePadPortOpen
// requires), before PSXInputDeviceType.cpp's MultiTapDevice.
unsigned long PadDmaBuffer[5][32] __attribute__((aligned(64)));

// ---- src/PSXControlInterface/ctor.cpp

PSXControlInterface::PSXControlInterface()
{
    m_numJoysticks = 0;
}

// ---- src/PSXControlInterface/dtor.cpp

PSXControlInterface::~PSXControlInterface()
{
    // The keyboard is an InputDevice (KeyboardDevice.h only has the slots
    // its users call): deleted through InputDevice's virtual dtor.
    if (m_keyboard)
        delete (InputDevice *)m_keyboard;
    for (int i = 0; i < m_numJoysticks; i++) {
        if (m_joysticks[i])
            delete m_joysticks[i];
    }
}

// ---- src/PSXControlInterface/Create.cpp

int PSXControlInterface::Create(int numJoysticks)
{
    m_numJoysticks = numJoysticks;
    for (int i = 0; i < numJoysticks; i++) {
        PSXJoystickDevice *joystick = new PSXJoystickDevice(i);
        joystick->Create(i);
        m_joysticks[i] = joystick;
    }
    scePadPortOpen(0, 0, PadDmaBuffer[0]);
    scePadPortOpen(0, 1, PadDmaBuffer[1]);
    scePadPortOpen(0, 2, PadDmaBuffer[2]);
    scePadPortOpen(0, 3, PadDmaBuffer[3]);
    scePadPortOpen(1, 0, PadDmaBuffer[4]);
    m_currentDevice = m_joysticks[0];
    m_keyboard = 0;
    return ControlInterface::Create();
}
