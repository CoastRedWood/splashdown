// InputDevice.cpp  @ 0x00243720 (text 0x243720-0x243850)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's InputDevice.cpp as one translation unit: InputDevice's
// constructor and destructor, a TU without an initializer in
// HiResMeter.cpp's tu_map row (its own InputDevice RTTI at 0x412288, after
// ImageData.cpp's; InputDeviceType.cpp has the second copy). InputDevice is
// JoystickDevice.h's declaration (RemoveAxis pure in slot 0).


#include "JoystickDevice/JoystickDevice.h"

extern "C" void free(void *p);

// ---- src/InputDevice/ctor.cpp

InputDevice::InputDevice(DeviceType type)
{
    m_axisPresent0 = 0;
    m_axisPresent1 = 0;
    m_axisPresent2 = 0;
    m_axisPresent3 = 0;
    m_axisPresent4 = 0;
    m_axisPresent5 = 0;
    m_buttons = 0;
    m_x00 = 0;
    m_numButtons = 0;
    m_type = type;
    m_subType = -1;
}


// ---- src/InputDevice/dtor.cpp

InputDevice::~InputDevice()
{
    if (m_buttons)
        free(m_buttons);
    m_buttons = 0;
}
