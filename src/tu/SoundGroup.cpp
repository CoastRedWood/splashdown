// SoundGroup.cpp  @ 0x00203440 (text 0x203440-0x203740)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// A retail translation unit without a static initializer: the sound base
// classes SoundGroup, SoundInterface, Sound and Music. Its .rodata is @574-@585
// (labels restart after Game.cpp's @1121; RTTI Music ... SoundGroup);
// BaseObject.cpp restarts at @365.


#include "SoundGroup/SoundGroup.h"
#include "SoundInterface/SoundInterface.h"
#include "Sound/Sound.h"
#include "GameObject/GameObject.h"
#include "Music/Music.h"

// ---- src/SoundGroup/ctor.cpp
// (This draft also held a copy of __dt__10SoundGroupFv; see dtor.cpp.)


SoundGroup::SoundGroup(int enabled)
    : GameObject(enabled)
{
    m_30 = 1;
    m_38 = 0;
}


// ---- src/SoundGroup/dtor.cpp

SoundGroup::~SoundGroup()
{
}


// ---- src/SoundInterface/ctor.cpp

SoundInterface::SoundInterface()
{
    m_flag = 0;
}


// ---- src/SoundInterface/dtor.cpp

SoundInterface::~SoundInterface()
{
}


// ---- src/Sound/ctor_002035D0.cpp

Sound::Sound()
{
    m_state = 0;
    m_x14 = 0;
    m_x18 = 0;
}


// ---- src/Sound/dtor.cpp

Sound::~Sound()
{
}


// ---- src/Music/ctor.cpp

Music::Music(int enabled) : GameObject(enabled)
{
    m_track = 1;
    m_numTracks = 0;
}


// ---- src/Music/dtor.cpp

Music::~Music()
{
}


// ---- ContainerList<Sound *>'s ctor (SoundInterface's lists): mwcc
// instantiates it at the TU's end, where retail has it (0x203710).
#define CONTAINERLIST_MEMBERS_ALL
#include "ContainerList/ContainerListMembers.h"
