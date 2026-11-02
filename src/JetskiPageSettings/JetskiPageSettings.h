#ifndef JETSKIPAGESETTINGS_H
#define JETSKIPAGESETTINGS_H

class JetskiManagerSettingsMain;
class JetskiPageSettingsCharSelect;
class JetskiPageSettingsSetup;
class JetskiPageSettingsMain;
class JetskiPageSettingsOptions;
class JetskiPageSettingsGOptions;
class JetskiPageSettingsLoadSave;
class JetskiPageSettingsVideos;

// Per-page UI settings, owned by JetskiGameOptions (+0x5FD8). 0x20 bytes.
class JetskiPageSettings {
public:
    JetskiPageSettings();
    ~JetskiPageSettings();
    void Create();
    void ResetOptions();
    void ResetCareer();

    JetskiManagerSettingsMain *m_managerMain;   // +0x0
    JetskiPageSettingsCharSelect *m_charSelect; // +0x4
    JetskiPageSettingsSetup *m_setup;           // +0x8
    JetskiPageSettingsMain *m_main;             // +0xC
    JetskiPageSettingsOptions *m_options;       // +0x10
    JetskiPageSettingsGOptions *m_gameOptions;  // +0x14
    JetskiPageSettingsLoadSave *m_loadSave;     // +0x18
    JetskiPageSettingsVideos *m_videos;         // +0x1C
};

#endif
