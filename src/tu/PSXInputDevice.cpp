// PSXInputDevice.cpp  @ 0x00270850 (text 0x270850-0x2708F0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp)
//
// Retail's PSXInputDevice.cpp as one translation unit, a TU without an
// initializer inside PSXCamera.cpp's tu_map row. Its .rodata is
// InputDevice's and PSXInputDevice's RTTI (0x416C78-0x416CC0, labels
// restarting at @436 after PSXGame.cpp's); PSXInputDeviceType.cpp's strings
// follow (@544) and repeat the RTTI with its own copies. `make link` places
// its data as well as its code.


#include "ControlInterface/ControlTypes.h"
#include "JoystickDevice/JoystickDevice.h"

// ---- src/PSXInputDevice/ctor.cpp

PSXInputDevice::PSXInputDevice(DeviceType type) : InputDevice(type)
{
}

// ---- src/PSXInputDevice/dtor.cpp

PSXInputDevice::~PSXInputDevice()
{
}
