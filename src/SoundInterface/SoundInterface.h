// SoundInterface — owns the sound and stream lists.
#ifndef SOUNDINTERFACE_H
#define SOUNDINTERFACE_H

class Sound;

#include "ContainerList/ContainerList.h"

class SoundInterface {
public:
    SoundInterface();

    ContainerList<Sound *> m_sounds;        // +0x00
    ContainerList<Sound *> m_streams;       // +0x14
    unsigned char m_flag : 1;               // +0x28

    virtual ~SoundInterface();              // vptr at +0x2C, after the data
};

#endif
