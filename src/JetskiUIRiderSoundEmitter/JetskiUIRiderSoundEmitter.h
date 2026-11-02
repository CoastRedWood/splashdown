// JetskiUIRiderSoundEmitter: plays the riders' speech streams in the menus
// (character select). A GameObject, 0x60 bytes (8-aligned, data ends at
// +0x5C). Names come from the class's own methods.
#ifndef JETSKIUIRIDERSOUNDEMITTER_JETSKIUIRIDERSOUNDEMITTER_H
#define JETSKIUIRIDERSOUNDEMITTER_JETSKIUIRIDERSOUNDEMITTER_H

#include "GameObject/GameObject.h"

template <class T> class ContainerList;
class PSXSoundInterface;
struct Rider_Stream_Info;
typedef ContainerList<Rider_Stream_Info *> StreamList;

class JetskiUIRiderSoundEmitter : public GameObject {
public:
    JetskiUIRiderSoundEmitter(PSXSoundInterface *soundInterface, unsigned int flags, int id);
    JetskiUIRiderSoundEmitter *Create(char *file);
    int LoadSounds(char *file);
    int PlayDialogue(int rider, int type);
    int SwitchSounds(Rider_Stream_Info *info);
    int Stop();

    // --- virtuals in retail slot order ---
    virtual ~JetskiUIRiderSoundEmitter();
    virtual int Tick(float dt);

    StreamList *m_sounds;               // +0x30, m_numRiders of them
    StreamList *m_dialogue;             // +0x34
    int m_numRiders;                    // +0x38
    int m_lastSound;                    // +0x3C
    int m_lastDialogue;                 // +0x40
    int m_voice;                        // +0x44
    int m_playing;                      // +0x48
    unsigned int m_flags;               // +0x4C
    unsigned int m_stream;              // +0x50
    float m_playTime;                   // +0x54
    PSXSoundInterface *m_soundInterface;    // +0x58 (its RainbowPS2Snd plays the streams)
};

#endif
