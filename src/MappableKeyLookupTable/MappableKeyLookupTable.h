// MappableKeyLookupTable: the action -> key/button map for the keyboard,
// eight joysticks and the mouse (0x17 actions each), 0x398 bytes. Names
// come from the class's own methods.
#ifndef MAPPABLEKEYLOOKUPTABLE_MAPPABLEKEYLOOKUPTABLE_H
#define MAPPABLEKEYLOOKUPTABLE_MAPPABLEKEYLOOKUPTABLE_H

#include "ControlInterface/ControlTypes.h"

class MappableKeyLookupTable {
public:
    MappableKeyLookupTable();
    void Reset();
    void GetKey(int action, unsigned int &key);
    void GetJoy(int action, unsigned int &key, int joystick);
    void GetMouse(int action, unsigned int &key);
    void MapKeys(DeviceType type, int pad, ...);

    unsigned int m_keys[0x17];          // +0x000
    unsigned int m_joy[8][0x17];        // +0x05C
    unsigned int m_mouse[0x17];         // +0x33C
};

#endif
