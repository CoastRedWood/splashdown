// InputDeviceType.cpp  @ 0x00243850 (text 0x243850-0x244810)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's InputDeviceType.cpp as one translation unit: JoystickDevice,
// KeyboardDevice::IsModsDown and the ContainerList<AnalogAxis *> members it
// uses, a TU without an initializer at the end of HiResMeter.cpp's tu_map row
// (the second InputDevice RTTI, at 0x412370; the assert says
// "InputDeviceType.cpp"). `make link` places the object's data as well as its
// code (tools/link_c.py). The ContainerList members are instantiated
// implicitly at the end of the TU, as in retail (the inline destructor right
// after the constructor that needs it).




#include "ContainerList/ContainerList.h"
#include "AnalogAxis/AnalogAxis.h"
#include "JoystickDevice/JoystickDevice.h"
#include "KeyboardDevice/KeyboardDevice.h"
#include "PSXButton/PSXButton.h"
#include "ControlInterface/ControlTypes.h"
#include "MappableKeyLookupTable/MappableKeyLookupTable.h"
#include "ControlInterface/ControlInterface.h"
#include "Game/Game.h"
#include "soultree_assert_block.h"
#define CONTAINERLIST_MEMBERS_ALL
#include "ContainerList/ContainerListMembers.h"

extern Game *LpGame;

// ---- src/JoystickDevice/ctor.cpp

JoystickDevice::JoystickDevice(int port) : PSXInputDevice((DeviceType)2)
{
    int i;
    m_port = port;
    m_x31C = 0;
    for (i = 0; i < 6; i++) {
        for (int j = 0; j < 16; j++) {
            m_buttonMap[i][j].m_x0 = 0;
            m_buttonMap[i][j].m_x4 = 0;
        }
    }
    for (i = 0; i < 6; i++) {
        m_axis[i] = 0;
        m_axes[i].Create(1, 1);
    }
    for (i = 0; i < 32; i++) {
        m_keys[i].m_down = 0;
        m_keys[i].m_value = 0.0f;
    }
    m_directionFlipped = LpGame->GetRegistryBOOL("JoyDirectionFlipped", 0);
}


// ---- src/JoystickDevice/dtor.cpp
JoystickDevice::~JoystickDevice()
{
}


// ---- src/JoystickDevice/IsKeyDown.cpp

// Retail forms the key's address through a helper (it gives `addu index, this`)
// before the state test, and copies the first word through that pointer.
static inline KeyInfo *GetKey(KeyInfo *keys, unsigned int key) { return &keys[key]; }

int JoystickDevice::IsKeyDown(unsigned int key, unsigned int mods, KeyInfo *info)
{
    KeyboardDevice *keyboard = LpGame->m_controls->m_keyboard;
    KeyInfo *k;
    if (keyboard) {
        if (!keyboard->IsModsDown(mods))
            return 0;
    } else if (mods && mods != 0x80000000 && mods != 0x3F) {
        return 0;
    }
    k = GetKey(m_keys, key);
    if (m_keys[key].m_down == 1) {
        if (info) {
            info->m_down = k->m_down;
            info->m_value = m_keys[key].m_value;
            info->m_x08 = m_keys[key].m_x08;
            info->m_x0C = m_keys[key].m_x0C;
            info->m_x10 = m_keys[key].m_x10;
            info->m_x14 = m_keys[key].m_x14;
        }
        return 1;
    }
    return 0;
}


// ---- src/JoystickDevice/AttachAxis.cpp

int JoystickDevice::AttachAxis(AnalogAxis *axis, float a, float b)
{
    axis->m_device = this;
    axis->m_x1C = a;
    axis->m_x20 = b;
    m_axes[axis->m_type].AddItem(axis);
    switch (axis->m_type) {
    case 0:
        axis->SetActionKey(-2, -3);
        break;
    case 1:
        axis->SetActionKey(-5, -4);
        break;
    case 2:
        axis->SetActionKey(-7, -6);
        break;
    case 3:
        axis->SetActionKey(-9, -8);
        break;
    case 4:
        axis->SetActionKey(-11, -10);
        break;
    case 5:
        axis->SetActionKey(-13, -12);
        break;
    }
    return 1;
}


// ---- src/JoystickDevice/RemoveAxis.cpp


void JoystickDevice::RemoveAxis(int id)
{
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < m_axes[i].m_numItems; j++) {
            AnalogAxis *axis = m_axes[i].GetItem(j);
            if (axis->m_index == id)
                m_axes[i].RemoveItem(axis);
        }
    }
}


// ---- src/JoystickDevice/UpdateAttachedAxes.cpp

void JoystickDevice::UpdateAttachedAxes(AxisType type, float a, float b)
{
    for (int i = 0; i < m_axes[type].m_numItems; i++) {
        AnalogAxis *axis = m_axes[type].GetItem(i);
        axis->SetValueAbsolute(a, b);
#line 128 "InputDeviceType.cpp"
        ASSERT(axis->m_type == type, "UpdateAttachedAxes: pAxis->AnalogAxisType != axisType");
    }
}


// ---- src/JoystickDevice/IsKeyDown_00243E90.cpp

// Negative actions are analog axes pushed past a quarter of their range.
int JoystickDevice::IsKeyDown(AssignableActionIndex action, unsigned int mods, KeyInfo *info)
{
    if (action < 0) {
        switch (action) {
        case -2:
            if (m_axisPresent0 && m_axis[0] < 16384.0f)
                return 1;
            break;
        case -3:
            if (m_axisPresent0 && m_axis[0] > 49152.0f)
                return 1;
            break;
        case -4:
            if (m_axisPresent1 && m_axis[1] < 16384.0f)
                return 1;
            break;
        case -5:
            if (m_axisPresent1 && m_axis[1] > 49152.0f)
                return 1;
            break;
        case -6:
            if (m_axisPresent2 && m_axis[2] < 16384.0f)
                return 1;
            break;
        case -7:
            if (m_axisPresent2 && m_axis[2] > 49152.0f)
                return 1;
            break;
        case -8:
            if (m_axisPresent3 && m_axis[3] < 16384.0f)
                return 1;
            break;
        case -9:
            if (m_axisPresent3 && m_axis[3] > 49152.0f)
                return 1;
            break;
        case -10:
            if (m_axisPresent4 && m_axis[4] < 16384.0f)
                return 1;
            break;
        case -11:
            if (m_axisPresent4 && m_axis[4] > 49152.0f)
                return 1;
            break;
        case -12:
            if (m_axisPresent5 && m_axis[5] < 16384.0f)
                return 1;
            break;
        case -13:
            if (m_axisPresent5 && m_axis[5] > 49152.0f)
                return 1;
            break;
        }
        return 0;
    }
    MappableKeyLookupTable *table = LpGame->m_controls->m_keyLookupTable;
    if (!table)
        return 0;
    unsigned int key;
    table->GetJoy(action, key, m_port);
    if (key == -1)
        return 0;
    return IsKeyDown(key, mods, info);
}


// ---- src/JoystickDevice/IsKeyAnalog.cpp
int JoystickDevice::IsKeyAnalog(unsigned int key)
{
    if (key >= m_numButtons) return 0; if (m_buttons) return m_buttons[key].m_flags & 2; return 0;
}


// ---- src/JoystickDevice/IsKeyAnalog_00244280.cpp

int JoystickDevice::IsKeyAnalog(AssignableActionIndex action)
{
    unsigned int key;
    LpGame->m_controls->m_keyLookupTable->GetJoy(action, key, m_port);
    if (key == (unsigned int)-1)
        return 0;
    return IsKeyAnalog(key);
}


// ---- src/KeyboardDevice/IsModsDown.cpp
bool KeyboardDevice::IsModsDown(unsigned int mods)
{
    unsigned char debugKeys = LpGame->m_debugKeys;
    if (!debugKeys && (mods & 0x80))
        return 0;
    if (mods == 0x3F)
        return 1;
    if (debugKeys && mods == 0x80 && m_keys[0x35].m_down == 1)
        return 1;
    if (mods == 0x80000000)
        return !m_modifiers;
    if (mods & 0x40)
        return (m_modifiers | 0x40) == mods;
    return (m_modifiers & mods) != 0;
}


// Retail's RTTI block has InputDevice's and PSXInputDevice's records before
// JoystickDevice's name (@975-@977, then @986): mwcc built them as
// the bases of a class whose vtable it wrote first, pushed last on its LIFO
// list (key function defined after JoystickDevice's). Nothing constructed that
// class, so retail's linker stripped its vtable, RTTI and functions; link_c
// leaves them out ("Section
// order").
class StrippedKeyboardDevice : public PSXInputDevice {
public:
    virtual ~StrippedKeyboardDevice();
};

StrippedKeyboardDevice::~StrippedKeyboardDevice()
{
}
