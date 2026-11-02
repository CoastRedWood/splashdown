// MediaControl — GameObject that pauses its media when disabled or unfocused.
#ifndef MEDIACONTROL_H
#define MEDIACONTROL_H

#include "GameObject/GameObject.h"

class MediaControl : public GameObject {
public:
    MediaControl(int enabled);
    virtual ~MediaControl();
    void Disable();
    void Enable();
    void AncestorsDisabled();
    void AncestorsEnabled();
    int Tick(float dt);
    virtual int Pause(int pause) = 0;
    int LostFocus();
    int Restore();

    virtual MediaControl *Create(char *filename, void (*callback)(void *), void *context);
    virtual int Start() = 0;
    virtual int Stop() = 0;
    virtual int Seek(int time) = 0;
    virtual int IsPlaying() = 0;
    virtual int GetTime(int *time) = 0;
    virtual int GetDurationTime(int *time) = 0;

    int m_30;
    int m_34;
    void (*m_callback)(void *);         // +0x38
    void *m_context;                    // +0x3C
    unsigned char m_40 : 1;     // +0x40 bit 0
};

#endif
