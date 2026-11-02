#ifndef JETSKIPAGESETTINGSCHARSELECT_H
#define JETSKIPAGESETTINGSCHARSELECT_H

// Character-select page settings (0x18 bytes, no vtable).
class JetskiPageSettingsCharSelect {
public:
    JetskiPageSettingsCharSelect();
    ~JetskiPageSettingsCharSelect();
    void Reset();

    int m_x0;                           // +0x0
    int m_x4;                           // +0x4
    int m_x8;                           // +0x8
    int m_xC;                           // +0xC
    int m_x10;                          // +0x10
    int m_x14;                          // +0x14
};

#endif
