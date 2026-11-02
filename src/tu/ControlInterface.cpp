// ControlInterface.cpp  @ 0x00226BA0 (text 0x226BA0-0x227EB0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's ControlInterface.cpp as one translation unit: the
// KEYDOWN_HANDLER_TEST helpers, MappableKeyLookupTable, AnalogAxis,
// ControlInterface, GetPanelResolution and SkipPad. A TU without an
// initializer at the end of CollisionObject.cpp's tu_map row (its
// "ControlInterface.cpp" asserts and RTTI follow CollisionObject.cpp's RTTI).
// `make link` places the object's data as well as its code (tools/link_c.py).
// The keyboard and mouse views are KeyboardDevice.h/MouseDevice.h; the C-mode
// GetKey and Create stubs are members again.



#include "ControlInterface/ControlInterface.h"
#include "Game/Game.h"
#include "ProcessNode/ProcessNode.h"
#include "MappableKeyLookupTable/MappableKeyLookupTable.h"
#include "AnalogAxis/AnalogAxis.h"
#include "JoystickDevice/JoystickDevice.h"
#include "KeyboardDevice/KeyboardDevice.h"
#include "MouseDevice/MouseDevice.h"
#include "KeyInfo/KeyInfo.h"
#include "DataAccess/DataAccess.h"
#include "soultree_assert_block.h"

extern Game *LpGame;
extern DataAccess *LpDataAccess;

// The last axis ID handed out (NewAxisID).
int UsedAxisID;
// ---- src/global/KEYDOWN_HANDLER_TEST.cpp


int KEYDOWN_HANDLER_TEST(unsigned int key, DeviceType device, ProcessNode *node, unsigned int mods)
{
    // Retail built this function without the global optimizer (value-form
    // bools, no movz); an empty asm statement reproduces that.
    asm("");
    bool result = false;
    bool match = node->m_key == key && node->m_device == device;
    if (match) {
        bool modsOk = true;
        if (device == DEVICE_KEYBOARD) {
            KeyboardDevice *kbd = LpGame->m_controls->m_keyboard;
            bool down = kbd && kbd->IsModsDown(mods);
            if (!down)
                modsOk = false;
        }
        if (modsOk)
            result = true;
    }
    return result;
}


// ---- src/global/KEYDOWN_HANDLER_TEST_00226C40.cpp


int KEYDOWN_HANDLER_TEST(AssignableActionIndex action, DeviceType device, ProcessNode *node, unsigned int mods)
{
    // Retail built this function without the global optimizer (value-form
    // bools, no movz); an empty asm statement reproduces that.
    asm("");
    unsigned int key;
    ControlInterface *controls = LpGame->m_controls;
    if (!controls->m_keyLookupTable)
        return 0;
    switch (device) {
    case DEVICE_JOYSTICK:
        controls->m_keyLookupTable->GetJoy(action, key, node->m_controller);
        break;
    case DEVICE_KEYBOARD:
        if (controls->m_keyboard && !controls->m_keyboard->IsModsDown(mods))
            return 0;
        LpGame->m_controls->m_keyLookupTable->GetKey(action, key);
        break;
    case DEVICE_MOUSE:
        controls->m_keyLookupTable->GetMouse(action, key);
        break;
    default:
        key = -1;
        break;
    }
    bool result = false;
    bool match = node->m_key == key && node->m_device == device;
    if (match && key != -1)
        result = true;
    return result;
}


// ---- src/MappableKeyLookupTable/ctor_00226D80.cpp

MappableKeyLookupTable::MappableKeyLookupTable()
{
    Reset();
}


// ---- src/MappableKeyLookupTable/Reset.cpp
#define NUM_KEYS 23
#define NUM_SLOTS 8
#define UNMAPPED 0xFFFFFFFF


void MappableKeyLookupTable::Reset()
{
    // j declared before i: the declaration order sets retail's register choice.
    int j, i;
    for (i = 0; i < NUM_KEYS; i++) {
        m_keys[i] = UNMAPPED;
        for (j = 0; j < NUM_SLOTS; j++)
            m_joy[j][i] = UNMAPPED;
        m_mouse[i] = UNMAPPED;
    }
}


// ---- src/MappableKeyLookupTable/GetKey.cpp

void MappableKeyLookupTable::GetKey(int i, unsigned int &key)
{
    if (i < 0 || i >= 0x17)
        key = -1;
    else
        key = m_keys[i];
}


// ---- src/MappableKeyLookupTable/GetJoy.cpp
void MappableKeyLookupTable::GetJoy(int i, unsigned int &key, int pad)
{
    if (i < 0 || i >= 0x17 || pad >= 8)
        key = -1;
    else
        key = m_joy[pad][i];
}


// ---- src/MappableKeyLookupTable/GetMouse.cpp
void MappableKeyLookupTable::GetMouse(int i, unsigned int &key)
{
    if (i < 0 || i >= 0x17)
        key = -1;
    else
        key = m_mouse[i];
}


// ---- src/MappableKeyLookupTable/MapKeys.cpp
// mwcc's MIPS va_start (see src/global/PSXprintf.cpp).
#include "mwcc_va.h"





// Map up to 23 actions from the variable arguments; a negative key ends
// the list.
void MappableKeyLookupTable::MapKeys(DeviceType type, int pad, ...)
{
    va_list ap;
    va_start(ap, pad);
    int key;
    for (int i = 0; i < 0x17; i++) {
        MWCC_VA_NEXT(ap, int,
                     key);
        if (key < 0)
            break;
        switch (type) {
        case DEVICE_JOYSTICK:
            m_joy[pad][i] = key;
            break;
        case DEVICE_KEYBOARD:
            m_keys[i] = key;
            break;
        case DEVICE_MOUSE:
            m_mouse[i] = key;
            break;
        }
    }
}


// ---- src/AnalogAxis/ctor.cpp


AnalogAxis::AnalogAxis(float min, float max, float center, int index, AxisType type, float deadZone)
{
    Init();
    m_min = min;
    m_max = max;
    m_value = center;
    m_center = center;
    m_deadZoneHigh = center + deadZone;
    m_deadZoneLow = center - deadZone;
    m_index = index;
    m_type = type;
}


// ---- src/AnalogAxis/Init.cpp

void AnalogAxis::Init()
{
    m_value = 0;
    m_center = 0;
    m_min = 0;
    m_max = 0;
    m_deadZoneHigh = 0;
    m_deadZoneLow = 0;
    m_device = 0;
    m_index = 0;
    m_type = (AxisType)-1;
    m_actionKey = -1;
    m_altActionKey = -1;
    m_x14 = 0;
    m_x18 = 0;
    m_x1C = 0;
    m_x20 = 0;
}


// ---- src/MappableKeyLookupTable/AnalogAxis_SetActionKey.cpp

void AnalogAxis::SetActionKey(unsigned int key, unsigned int altKey)
{
    m_actionKey = key;
    m_altActionKey = altKey;
}


// ---- src/AnalogAxis/dtor.cpp

AnalogAxis::~AnalogAxis()
{
    RemoveAxis();
}


// ---- src/AnalogAxis/SetValueAbsolute.cpp

// Map value/range onto [min, max], then rescale around a centre dead zone.
void AnalogAxis::SetValueAbsolute(float value, float range)
{
#line 228 "ControlInterface.cpp"
    ASSERT(m_type != -1, "SetValueAbsolute: AnalogAxisType == UNDEFINED_AXISTYPE");
    float v = (value / range) * (m_max - m_min) + m_min;
    if (v >= m_deadZoneHigh) {
        v -= m_deadZoneHigh;
        v *= (m_max - m_center) / (m_max - m_deadZoneHigh);
        v += m_center;
    } else if (v <= m_deadZoneLow) {
        v -= m_deadZoneLow;
        v *= (m_center - m_min) / (m_deadZoneLow - m_min);
        v += m_center;
    } else
        v = m_center;
    m_value = v;
}


// ---- src/AnalogAxis/RemoveAxis.cpp


void AnalogAxis::RemoveAxis()
{
    if (!LpGame->m_controls)
        return;
    if (m_device)
        m_device->RemoveAxis(m_index);
}


// ---- src/ControlInterface/ctor.cpp
ControlInterface::ControlInterface()
{
    m_x00 = 0;
    m_x04 = 0;
    m_currentDevice = 0;
    for (int i = 0; i < 8; i++)
        m_joysticks[i] = 0;
    m_mouse = 0;
    m_keyboard = 0;
    m_keyLookupTable = 0;
    m_numProcessNodes = 0;
    m_ownsKeyLookupTable = 0;
}


// ---- src/ControlInterface/dtor.cpp
ControlInterface::~ControlInterface()
{
    if (m_ownsKeyLookupTable && m_keyLookupTable) {
        delete m_keyLookupTable;
        m_keyLookupTable = 0;
    }
}


// ---- src/ControlInterface/Create.cpp

int ControlInterface::Create()
{
    m_keyLookupTable = new MappableKeyLookupTable;
    m_ownsKeyLookupTable = 1;
    return 1;
}


// ---- src/ControlInterface/AttachKeyLookupTable.cpp


int ControlInterface::AttachKeyLookupTable(MappableKeyLookupTable *table)
{
    if (!table)
        return 0;
    if (m_ownsKeyLookupTable && m_keyLookupTable)
        delete m_keyLookupTable;
    m_keyLookupTable = table;
    m_ownsKeyLookupTable = 0;
    return 1;
}


// ---- src/MappableKeyLookupTable/ControlInterface_NewAxisID.cpp


int ControlInterface::NewAxisID()
{
    UsedAxisID++;
    return UsedAxisID;
}


// ---- src/ControlInterface/AddProcessNode_audit.cpp

void ControlInterface::AddProcessNode(unsigned int key, DeviceType device, KeyState state, int action)
{
    m_processNodes[m_numProcessNodes].m_key = key;
    m_processNodes[m_numProcessNodes].m_controller = action;
    m_processNodes[m_numProcessNodes].m_device = device;
    m_processNodes[m_numProcessNodes].m_event = state;
    m_processNodes[m_numProcessNodes].m_modifiers = 0x3F;
    m_numProcessNodes++;
}


// ---- src/ControlInterface/Update.cpp

int ControlInterface::Update(float dt)
{
    m_numProcessNodes = 0;
    if (m_keyboard)
        m_keyboard->Update(dt);
    if (m_mouse)
        m_mouse->Update(dt);
    int i;
    int commErrors = 0;
    for (i = 0; i < 8; i++) {
        if (m_joysticks[i]) {
            m_joysticks[i]->Update(dt);
            if (m_joysticks[i]->CommErrors())
                commErrors = 1;
        }
    }
    if (commErrors)
        ResetAllControllers(0);
    for (i = 0; i < m_numProcessNodes; i++) {
        m_processNodes[i].m_modifiers = m_keyboard == 0 ? 0 : m_keyboard->m_modifiers;
        switch (m_processNodes[i].m_device) {
        case 0:
            if (m_processNodes[i].m_event == 0)
                LpGame->KeyUpHandler(&m_processNodes[i], &m_keyboard->m_keys[m_processNodes[i].m_key]);
            if (m_processNodes[i].m_event == 1)
                LpGame->KeyDownHandler(&m_processNodes[i], &m_keyboard->m_keys[m_processNodes[i].m_key]);
            break;
        case 1:
            if (m_processNodes[i].m_event == 0)
                LpGame->KeyUpHandler(&m_processNodes[i], &m_mouse->m_keys[m_processNodes[i].m_key]);
            if (m_processNodes[i].m_event == 1)
                LpGame->KeyDownHandler(&m_processNodes[i], &m_mouse->m_keys[m_processNodes[i].m_key]);
            break;
        case 2:
            if (m_processNodes[i].m_event == 0)
                LpGame->KeyUpHandler(&m_processNodes[i],
                                     &m_joysticks[m_processNodes[i].m_controller]->m_keys[m_processNodes[i].m_key]);
            if (m_processNodes[i].m_event == 1)
                LpGame->KeyDownHandler(&m_processNodes[i],
                                       &m_joysticks[m_processNodes[i].m_controller]->m_keys[m_processNodes[i].m_key]);
            break;
        default:
#line 387 "ControlInterface.cpp"
            ASSERT(0, "ControlInterface::Update: INVALID device type");
            break;
        }
    }
    return 1;
}


// ---- src/ControlInterface/ResetAllControllers.cpp

void ControlInterface::ResetAllControllers(int wait)
{
    for (int i = 0; i < 8; i++) {
        if (m_joysticks[i]) {
            m_joysticks[i]->ResetController();
            if (wait) {
                // Give the device up to 50 polls to come back.
                int tries = 0;
                while (tries++ < 50 && !m_joysticks[i]->IsControllerSetup())
                    m_joysticks[i]->SetupController();
            }
        }
    }
}


// ---- src/ControlInterface/IsKeyDown.cpp

int ControlInterface::IsKeyDown(unsigned int key, DeviceType type, unsigned int mask, int joystick)
{
    switch (type) {
    case DEVICE_JOYSTICK:
        return (unsigned char)(m_joysticks[joystick] && m_joysticks[joystick]->IsKeyDown(key, mask, 0));
    case DEVICE_KEYBOARD:
        return (unsigned char)(m_keyboard && m_keyboard->IsKeyDown(key, mask, 0));
    case DEVICE_MOUSE:
        return (unsigned char)(m_mouse && m_mouse->IsKeyDown(key, mask, 0));
    }
    return 0;
}


// ---- src/ControlInterface/IsKeyDown_00227A90.cpp

int ControlInterface::IsKeyDown(AssignableActionIndex action, DeviceType type, unsigned int mask, int joystick)
{
    switch (type) {
    case DEVICE_JOYSTICK:
        return (unsigned char)(m_joysticks[joystick] && m_joysticks[joystick]->IsKeyDown(action, mask, 0));
    case DEVICE_KEYBOARD:
        return (unsigned char)(m_keyboard && m_keyboard->IsKeyDown(action, mask, 0));
    case DEVICE_MOUSE:
        return (unsigned char)(m_mouse && m_mouse->IsKeyDown(action, mask, 0));
    }
    return 0;
}


// ---- src/ControlInterface/IsKeyDown_00227B70.cpp

int ControlInterface::IsKeyDown(AssignableActionIndex action, unsigned int mask)
{
    return (unsigned char)((m_joysticks[0] && m_joysticks[0]->IsKeyDown(action, mask, 0))
                           || (m_keyboard && m_keyboard->IsKeyDown(action, mask, 0))
                           || (m_mouse && m_mouse->IsKeyDown(action, mask, 0)));
}


// ---- src/ControlInterface/IsJoyButtonDown.cpp


int ControlInterface::IsJoyButtonDown(unsigned int button, int joystick)
{
    JoystickDevice *device = m_joysticks[joystick];
    return device && device->IsKeyDown(button, 0x3F, 0);
}


// ---- src/ControlInterface/IsJoyButtonDown_00227CC0.cpp


int ControlInterface::IsJoyButtonDown(AssignableActionIndex action, int joystick)
{
    JoystickDevice *device = m_joysticks[joystick];
    return device && device->IsKeyDown(action, 0x3F, 0);
}


// ---- src/ControlInterface/GetKeyFromMappedAction.cpp


unsigned int ControlInterface::GetKeyFromMappedAction(AssignableActionIndex action, DeviceType device, int joystick)
{
    unsigned int key;
    if (!m_keyLookupTable)
        return -1;
    switch (device) {
    case 2:
        m_keyLookupTable->GetJoy(action, key, joystick);
        break;
    case 0:
        m_keyLookupTable->GetKey(action, key);
        break;
    case 1:
        m_keyLookupTable->GetMouse(action, key);
        break;
    default:
        key = -1;
        break;
    }
    return key;
}


// ---- src/global/GetPanelResolution.cpp
// Each panel's resolution is a 2-bit code; 8 bits per row of panels.
int GetPanelResolution(int codes, int column, int row)
{
    switch ((codes >> (column * 2) >> (row * 8)) & 3) {
    case 0:
        return 512;
    case 1:
        return 64;
    case 2:
        return 128;
    case 3:
        return 256;
    }
    return 0;
}


// ---- src/global/SkipPad.cpp


// Advances the stream to the next 2 KB (CD sector) boundary.
void SkipPad(long handle)
{
    int pad = 0x800 - LpDataAccess->FTell(handle) % 0x800;
    if (pad != 0x800)
        LpDataAccess->FSeek(handle, pad, 1);
}

