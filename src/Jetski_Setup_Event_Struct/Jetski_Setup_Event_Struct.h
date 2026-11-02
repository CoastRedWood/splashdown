#ifndef JETSKI_SETUP_EVENT_STRUCT_H
#define JETSKI_SETUP_EVENT_STRUCT_H

// One event's setup choices on the setup page (0x10 bytes); -1 = unset.
struct Jetski_Setup_Event_Struct {
    Jetski_Setup_Event_Struct();

    int m_x0;                           // +0x0
    int m_x4;                           // +0x4
    int m_x8;                           // +0x8
    int m_xC;                           // +0xC
};

#endif
