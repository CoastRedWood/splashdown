#ifndef JETSKIMANAGERSETTINGSMAIN_H
#define JETSKIMANAGERSETTINGSMAIN_H

// Settings remembered by the main manager page (8 bytes, no vtable).
class JetskiManagerSettingsMain {
public:
    JetskiManagerSettingsMain();
    ~JetskiManagerSettingsMain();
    void Reset();

    int m_x0;                           // +0x0
    int m_x4;                           // +0x4
};

#endif
