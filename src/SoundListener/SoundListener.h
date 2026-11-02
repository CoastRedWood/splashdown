// SoundListener: AuralScape's handle on one PSXSoundInterface Listener
// (position/orientation/velocity live in the Listener). 8 bytes, no vtable.
#ifndef SOUNDLISTENER_SOUNDLISTENER_H
#define SOUNDLISTENER_SOUNDLISTENER_H

struct _D3DVECTOR;
class AuralScape;
class Listener;

class SoundListener {
public:
    SoundListener();
    ~SoundListener();
    int Create(AuralScape *scape, float doppler, float rolloff);
    int SetDopplerFactor(float factor);
    int SetRolloffFactor(float factor);
    int SetDistanceFactor(float factor);
    int CommitDeferredSettings();
    int GetPosition(_D3DVECTOR *position);
    int SetPosition(_D3DVECTOR *position);
    int GetOrientation(_D3DVECTOR *front, _D3DVECTOR *top);
    int SetOrientation(_D3DVECTOR *front, _D3DVECTOR *top);
    int GetVelocity(_D3DVECTOR *velocity);
    int SetVelocity(_D3DVECTOR *velocity);

    AuralScape *m_scape;                // +0x0
    Listener *m_listener;               // +0x4
};

#endif
