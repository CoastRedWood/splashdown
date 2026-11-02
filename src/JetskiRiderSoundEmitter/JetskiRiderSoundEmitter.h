// JetskiRiderSoundEmitter: plays a racing rider's speech streams (wrecks,
// stunts, passing, ...). A GameObject, 0x548 bytes (`new` in JetskiRider);
// data +0x30..+0x544. Names follow its own methods (src/JetskiRiderSoundEmitter/).
// +0x528 is the JetskiVehicle Create stores (SwitchSounds' copy calls it a
// JetskiRider). The lists are ContainerList<Rider_Stream_Info *> by value
// (some copies spell it StreamList); ContainerList's dtor is not in
// ContainerList.h, so the dtor's file keeps its own copy.
#ifndef JETSKIRIDERSOUNDEMITTER_JETSKIRIDERSOUNDEMITTER_H
#define JETSKIRIDERSOUNDEMITTER_JETSKIRIDERSOUNDEMITTER_H

#include "GameObject/GameObject.h"
#include "ContainerList/ContainerList.h"

class JetskiVehicle;
class PSXSoundInterface;
struct _D3DVECTOR;

class JetskiRiderSoundEmitter : public GameObject {
public:
    JetskiRiderSoundEmitter(PSXSoundInterface *soundInterface, unsigned int flags, int id);
    JetskiRiderSoundEmitter *Create(char *file, JetskiVehicle *jetski, float a, int b);
    int LoadSounds(char *fileName);
    int UnloadSounds();
    int SwitchSounds(Rider_Stream_Info *stream);
    int Stop();
    int PlayJoinSounds();
    int PlayPassSounds();
    int PlayPassedSounds();
    int PlaySplashSounds(float speed);
    int PlayStuntSounds();
    int PlayWinSounds();
    int PlayWreckSounds(_D3DVECTOR *position, int wrecked, int big, int force);

    // --- virtuals in retail slot order ---
    virtual ~JetskiRiderSoundEmitter();
    virtual int Tick(float dt);

    int m_voice;                                        // +0x30
    int m_playing;                                      // +0x34
    unsigned int m_flags;                               // +0x38
    unsigned int m_streamHandle;                        // +0x3C
    float m_playTime;                                   // +0x40
    PSXSoundInterface *m_soundInterface;                // +0x44
    ContainerList<Rider_Stream_Info *> m_wreckSounds;   // +0x48
    ContainerList<Rider_Stream_Info *> m_crashSounds;   // +0x5C
    ContainerList<Rider_Stream_Info *> m_splashSounds;  // +0x70
    ContainerList<Rider_Stream_Info *> m_stuntSounds;   // +0x84
    ContainerList<Rider_Stream_Info *> m_smallSplashSounds; // +0x98
    ContainerList<Rider_Stream_Info *> m_bigSplashSounds;   // +0xAC
    ContainerList<Rider_Stream_Info *> m_passSounds;    // +0xC0
    ContainerList<Rider_Stream_Info *> m_passedSounds;  // +0xD4
    ContainerList<Rider_Stream_Info *> m_winSounds;     // +0xE8
    ContainerList<Rider_Stream_Info *> m_joinSounds;    // +0xFC
    int m_lastWreckSound;                               // +0x110
    char m_lastCrashSound[0x104];                       // +0x114
    int m_lastSplashSound;                              // +0x218
    char m_lastStuntSound[0x104];                       // +0x21C
    char m_lastPassSound[0x104];                        // +0x320
    char m_lastPassedSound[0x104];                      // +0x424
    JetskiVehicle *m_jetski;                            // +0x528 (Create)
    float m_passChance;                                 // +0x52C
    float m_passedChance;                               // +0x530
    float m_crashChance;                                // +0x534
    float m_stuntChance;                                // +0x538
    float m_splashChance;                               // +0x53C
    int m_540;                                          // +0x540
};

#endif
