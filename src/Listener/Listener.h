// Listener: PSXSoundInterface's listener (a BaseObject; position,
// velocity and orientation), 0x50 bytes.
#ifndef LISTENER_LISTENER_H
#define LISTENER_LISTENER_H

#include "d3dtypes.h"
#include "GameObject/GameObject.h"

class Listener : public BaseObject {
public:
    Listener();
    int SetDopplerFactor(float factor, int deferred);
    int SetRolloffFactor(float factor, int deferred);
    int SetDistanceFactor(float factor, int deferred);
    int CommitDeferredSettings();

    // --- virtuals in retail slot order ---
    virtual ~Listener();

    _D3DVECTOR m_position;              // +0x10
    _D3DVECTOR m_front;                 // +0x20 (SoundListener::GetOrientation's front)
    _D3DVECTOR m_top;                   // +0x30
    _D3DVECTOR m_velocity;              // +0x40 (SoundListener::GetVelocity)
};

#endif
