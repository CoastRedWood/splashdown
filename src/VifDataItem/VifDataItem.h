// VifDataItem: the head of a render-queue item (0x10 bytes): its size in
// quadwords and a pointer to its VIF packet, which usually follows the head
// in the same allocation (Terrain's scissor and common items derive from it
// with their packet as a member). Many files still keep local views that
// include their packet.
#ifndef VIFDATAITEM_VIFDATAITEM_H
#define VIFDATAITEM_VIFDATAITEM_H

#include "ps2_port.h"

struct VifDataItem {
    short m_numQwords;                  // +0x00
    unsigned char m_02;                 // +0x02
    unsigned char m_03;                 // +0x03
    PS2_PTR(void) m_data;                       // +0x04, the packet
    int m_08;                           // +0x08
    int m_0C;                           // +0x0C
};

#endif
