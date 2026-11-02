// AnalogAxis: one analog input axis attached to an input device, mapped
// to [m_min, m_max] around m_center with a dead zone. 0x3C bytes, no
// vtable. Names come from the class's own methods (SetValueAbsolute
// writes m_value at +0x24 and reads m_center at +0x28).
#ifndef ANALOGAXIS_ANALOGAXIS_H
#define ANALOGAXIS_ANALOGAXIS_H

#include "ControlInterface/ControlTypes.h"

class InputDevice;

class AnalogAxis {
public:
    AnalogAxis(float min, float max, float center, int index, AxisType type, float deadZone);
    ~AnalogAxis();
    void Init();
    void RemoveAxis();
    void SetActionKey(unsigned int key, unsigned int altKey);
    void SetValueAbsolute(float value, float range);

    InputDevice *m_device;              // +0x00 (JoystickDevice's copy: JoystickDevice *)
    int m_index;                        // +0x04
    AxisType m_type;                    // +0x08, -1 = UNDEFINED_AXISTYPE
    unsigned int m_actionKey;           // +0x0C
    unsigned int m_altActionKey;        // +0x10
    int m_x14;                          // +0x14
    int m_x18;                          // +0x18
    float m_x1C;                        // +0x1C, set by JoystickDevice::AttachAxis
    float m_x20;                        // +0x20
    float m_value;                      // +0x24
    float m_center;                     // +0x28
    float m_min;                        // +0x2C
    float m_max;                        // +0x30
    float m_deadZoneHigh;               // +0x34
    float m_deadZoneLow;                // +0x38
};

#endif
