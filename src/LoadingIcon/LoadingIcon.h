// LoadingIcon: the animated icon shown while loading (JetskiLoadManager::
// m_loadIcon). Its frames sit at the top of video memory, and the vblank
// interrupt LoadingIconInterrupt sends a sprite packet pointing at the next
// one. 0x14 bytes. Names follow Create.
#ifndef LOADINGICON_LOADINGICON_H
#define LOADINGICON_LOADINGICON_H

#include "_RECT/_RECT.h"

class DmaPacket;

class LoadingIcon {
public:
    LoadingIcon();
    ~LoadingIcon();
    void Create(char *name, int numFrames, _RECT rect, unsigned char alpha, int copyDisplay);
    void Enable();
    void Disable();
    void SetSpeed(int speed);

    int m_numFrames;                    // +0x0
    DmaPacket *m_packet;                // +0x4
    unsigned int *m_xyz0;               // +0x8, the sprite's first XYZ2 in the packet
    unsigned int *m_xyz1;               // +0xC
    int m_psm;                          // +0x10
};

#endif
