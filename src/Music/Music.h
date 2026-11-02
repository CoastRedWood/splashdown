// Music: the music player base, a GameObject (0x38 bytes: the ctor sets
// m_track = 1, m_numTracks = 0). PSXMusic derives from it.
#ifndef MUSIC_MUSIC_H
#define MUSIC_MUSIC_H

#include "GameObject/GameObject.h"

class Music : public GameObject {
public:
    Music(int enabled);
    virtual ~Music();

    int m_track;                        // +0x30, 1-based
    int m_numTracks;                    // +0x34
};

#endif
