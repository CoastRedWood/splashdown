// ControlInterface: the input manager (PSXControlInterface is the PS2 one,
// adding m_numJoysticks at +0xCC4). The data comes first, so the vptr sits
// after it at +0xCC0 and ControlInterface is 0xCC4 bytes. Names follow its
// own methods (src/ControlInterface/), otherwise the most common local name.
#ifndef CONTROLINTERFACE_CONTROLINTERFACE_H
#define CONTROLINTERFACE_CONTROLINTERFACE_H

#include "ControlInterface/ControlTypes.h"
#include "ProcessNode/ProcessNode.h"

class JoystickDevice;
class KeyboardDevice;
class MouseDevice;
class MappableKeyLookupTable;
enum KeyState {};


class ControlInterface {
public:
    ControlInterface();
    void AddProcessNode(unsigned int key, DeviceType device, KeyState state, int action);
    unsigned int GetKeyFromMappedAction(AssignableActionIndex action, DeviceType device, int joystick);
    int Update(float dt);
    void ResetAllControllers(int wait);
    int AttachKeyLookupTable(MappableKeyLookupTable *table);
    // Not static: its callers (JetskiPlayerVehicle/JetskiAIVehicle::Create) load
    // the object into $a0, though the body (UsedAxisID++) ignores it.
    int NewAxisID();

    int m_x00;                          // +0x00
    int m_x04;                          // +0x04
    int m_ownsKeyLookupTable;           // +0x08, set by Create
    JoystickDevice *m_currentDevice;    // +0x0C
    JoystickDevice *m_joysticks[8];     // +0x10
    MouseDevice *m_mouse;               // +0x30
    KeyboardDevice *m_keyboard;         // +0x34
    int m_numProcessNodes;              // +0x38
    ProcessNode m_processNodes[160];    // +0x03C, the key events of this frame (Update)
    MappableKeyLookupTable *m_keyLookupTable;   // +0xCBC

    // --- virtuals in retail slot order (retail vtable); vptr at +0xCC0 ---
    virtual ~ControlInterface();
    virtual int Create();
    virtual int IsKeyDown(unsigned int key, DeviceType type, unsigned int mask, int joystick);
    virtual int IsKeyDown(AssignableActionIndex action, DeviceType type, unsigned int mask, int joystick);
    virtual int IsKeyDown(AssignableActionIndex action, unsigned int mask);
    virtual int IsJoyButtonDown(unsigned int button, int joystick);
    virtual int IsJoyButtonDown(AssignableActionIndex action, int joystick);
};

#endif
