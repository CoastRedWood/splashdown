#ifndef JETSKIPAGESETTINGSSETUP_H
#define JETSKIPAGESETTINGSSETUP_H

#include "Jetski_Setup_Event_Struct/Jetski_Setup_Event_Struct.h"

// Race-setup page settings (0xA8 bytes, no vtable).
class JetskiPageSettingsSetup {
public:
    JetskiPageSettingsSetup();
    ~JetskiPageSettingsSetup();
    void Reset();

    Jetski_Setup_Event_Struct m_events[5];   // +0x0
    Jetski_Setup_Event_Struct m_events2[5];  // +0x50
    int m_xA0;                          // +0xA0
    int m_xA4;                          // +0xA4
};

#endif
