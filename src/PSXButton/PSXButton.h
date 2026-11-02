// PSXButton: one entry of an input device's per-key table
// (InputDevice::m_buttons, m_numButtons of them), 0xC bytes.
// Names follow PSXJoystickDevice::PolledUpdate's copy.
#ifndef PSXBUTTON_PSXBUTTON_H
#define PSXBUTTON_PSXBUTTON_H

struct PSXButton {
    unsigned char m_flags;              // +0x00, bit 1: reads pressure (analog)
    char pad01[0x4 - 0x1];
    int m_pressureIndex;                // +0x04, byte of the pad data
    unsigned short m_mask;              // +0x08, bit in the digital button word
};

#endif
