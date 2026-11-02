#ifndef JETSKIPAGESETTINGSLOADSAVE_H
#define JETSKIPAGESETTINGSLOADSAVE_H

// Settings for the LoadSave page (4 bytes, no vtable).
class JetskiPageSettingsLoadSave {
public:
    JetskiPageSettingsLoadSave();
    ~JetskiPageSettingsLoadSave();
    void Reset();

    int m_x0;                           // +0x0
};

#endif
