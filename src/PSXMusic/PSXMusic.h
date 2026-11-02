// PSXMusic: the music player over RainbowPS2Snd streams (created by
// PSXSoundInterface::CreateMusic, 0xA0 bytes). Music (Music/Music.h) is its
// base. Names come from the class's own methods.
#ifndef PSXMUSIC_PSXMUSIC_H
#define PSXMUSIC_PSXMUSIC_H

#include "GameObject/GameObject.h"
#include "Music/Music.h"
// PSXSoundInterface.h brings ContainerList (m_playList's type).
#include "PSXSoundInterface/PSXSoundInterface.h"

struct MusicTrack;

class PSXMusic : public Music {
public:
    PSXMusic(PSXSoundInterface *soundInterface, int a, int b);
    PSXMusic *Create(int numTracks);
    int Play(int track, int restart);
    int Stop(float fadeTime);
    int Repeat(int track, int restart);
    int Shuffle(int restart);
    int ShuffleStartingAt(int track, int play);
    int RampVolume(float volume, float time, int a);

    // --- virtuals in retail slot order ---
    virtual ~PSXMusic();
    virtual int Tick(float dt);
    virtual int Pause(int pause);

    int m_voice;                        // +0x38, -1 if none
    int m_created;                      // +0x3C
    int m_playing;                      // +0x40
    int m_paused;                       // +0x44 (Pause)
    int m_mode;                         // +0x48
    unsigned int m_streamHandle;        // +0x4C
    float m_position;                   // +0x50
    int m_x54;                          // +0x54
    MusicTrack *m_tracks;               // +0x58, malloc'd
    int m_x5C;                          // +0x5C, 10000
    int m_x60;                          // +0x60
    unsigned int m_startTime;           // +0x64, timeGetTime() at construction
    int m_x68;                          // +0x68
    PSXSoundInterface *m_system;        // +0x6C
    ContainerList<int> m_playList;      // +0x70, tracks already played
    unsigned int m_volume;              // +0x84
    float m_x88;                        // +0x88
    float m_x8C;                        // +0x8C
    float m_x90;                        // +0x90
    int m_x94;                          // +0x94
    int m_x98;                          // +0x98
};

#endif
