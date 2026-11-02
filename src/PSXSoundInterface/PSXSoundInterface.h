// PSXSoundInterface: the PS2 sound interface (LpGame->m_auralScape's
// m_soundInterface). SoundInterface (0x30 bytes, vptr +0x2C), then the
// RainbowPS2Snd driver at +0x40; 0x9F00 bytes (`new` in AuralScape::Create; data seen to +0x9EE0). Names come from the class's
// own methods.
#ifndef PSXSOUNDINTERFACE_PSXSOUNDINTERFACE_H
#define PSXSOUNDINTERFACE_PSXSOUNDINTERFACE_H

#include "SoundInterface/SoundInterface.h"
#include "RainbowPS2Snd/RainbowPS2Snd.h"

class Listener;
class PSXSound;
class PSXSoundGroup;
class StringTable;

class PSXSoundInterface : public SoundInterface {
public:
    PSXSoundInterface();
    int Create(unsigned int flags);
    int SetSpeakerConfiguration(unsigned int config);
    void SetDolby(int dolby);
    void SetMono(int mono);
    int SetSFXVolume(long volume);
    int SetStreamVolume(long volume);
    int GetAvailable3DHardwareVoices();
    Listener *CreateListener();
    int CreateMusic(int a, int track, int c);
    int LoadSoundStringTable(char *filename);
    void ClearSoundStringTable();
    void LoadRequestedSounds();
    PSXSound *GetSoundFromResourceName(PSXSoundGroup *group, char *name, unsigned int a, unsigned int b);

    virtual ~PSXSoundInterface();

    char pad30[0x40 - 0x30];
    RainbowPS2Snd m_snd;                // +0x40
    char pad9EB4[0x9EC0 - 0x9EB4];
    unsigned int m_speakerConfig;       // +0x9EC0
    int m_sfxVolume;                    // +0x9EC4
    int m_streamVolume;                 // +0x9EC8
    char pad9ECC[0x9ED8 - 0x9ECC];
    int m_surround;                     // +0x9ED8, reverses stereo
    StringTable *m_soundStringTable;    // +0x9EDC
    char pad9EE0[0x9F00 - 0x9EE0];      // `new PSXSoundInterface` is 0x9F00 (AuralScape::Create)
};

#endif
