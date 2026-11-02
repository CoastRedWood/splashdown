// ProcessNode: one queued input event, handed to the KeyDown/KeyUp
// handlers (ControlInterface keeps an array of them), 0x14 bytes.
#ifndef PROCESSNODE_PROCESSNODE_H
#define PROCESSNODE_PROCESSNODE_H

#include "ControlInterface/ControlTypes.h"

struct ProcessNode {
    unsigned int m_key;                 // +0x00
    DeviceType m_device;                // +0x04
    int m_controller;                   // +0x08, joystick index
    int m_event;                        // +0x0C: 0 key up, 1 key down
    int m_modifiers;                    // +0x10
};

#endif
