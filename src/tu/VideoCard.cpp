// VideoCard.cpp  @ 0x0029D320 (text 0x29D320-0x29D5F0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// VideoCard (the platform-independent base of PSXVideoCard) as one translation
// unit without a static initializer, after TypeRegistry.cpp in its tu_map row.
// It owns ContainerList<VideoPreFlipObject *>'s ctor and Create, which mwcc
// instantiates at its end; PSXVideoCard.cpp owns the other members.



#include "PSXVideoCard/PSXVideoCard.h"
#include "soultree_assert_block.h"
#define CONTAINERLIST_MEMBERS_ALL
#include "ContainerList/ContainerListMembers.h"

// Defined here for its retail .bss slot.
PSXVideoCard *LpVideoCard;


// ---- src/VideoCard/ctor.cpp
VideoCard::VideoCard()
{
    m_width = 0;
    m_height = 0;
    m_depth = 0;
    m_flipPending = 0;
    m_x10 = 0;
    m_x14 = 0;
    m_preFlipObjects.Create(1, 1);
}


// ---- src/VideoCard/Release.cpp
void VideoCard::Release()
{
}


// ---- src/VideoCard/dtor.cpp
VideoCard::~VideoCard()
{
    Release();
}


// ---- src/VideoCard/PauseVideoCardFlip.cpp
void VideoCard::PauseVideoCardFlip(int pause)
{
}


// ---- src/VideoCard/AddVideoCardEffects.cpp
void VideoCard::AddVideoCardEffects()
{
}
