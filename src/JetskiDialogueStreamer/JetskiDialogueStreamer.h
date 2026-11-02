// JetskiDialogueStreamer: plays numbered speech streams (the training
// instructor's dialogue) through PSXSoundInterface's RainbowPS2Snd. A
// GameObject, 0x60 bytes (data ends at +0x5C). Names come from the class's
// own methods.
#ifndef JETSKIDIALOGUESTREAMER_JETSKIDIALOGUESTREAMER_H
#define JETSKIDIALOGUESTREAMER_JETSKIDIALOGUESTREAMER_H

#include "GameObject/GameObject.h"

class PSXSoundInterface;
struct DialogueLine;

class JetskiDialogueStreamer : public GameObject {
public:
    JetskiDialogueStreamer(PSXSoundInterface *soundInterface, int enabled);
    JetskiDialogueStreamer *Create(int numLines, char *directory);
    int Play(int track, int interrupt);
    int Stop();

    // --- virtuals in retail slot order ---
    virtual ~JetskiDialogueStreamer();
    virtual int Tick(float dt);
    virtual int Pause(int pause);

    int m_voice;                        // +0x30
    int m_numLines;                     // +0x34
    int m_track;                        // +0x38
    int m_active;                       // +0x3C
    int m_playing;                      // +0x40
    int m_paused;                       // +0x44
    unsigned int m_x48;                 // +0x48
    float m_time;                       // +0x4C
    int m_x50;                          // +0x50
    DialogueLine *m_lines;              // +0x54, malloc'd
    PSXSoundInterface *m_system;        // +0x58
};

#endif
