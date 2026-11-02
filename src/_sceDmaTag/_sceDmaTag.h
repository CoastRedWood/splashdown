#ifndef _SCEDMATAG_H
#define _SCEDMATAG_H

#include "ps2_port.h"

// libdma's DMA tag (sceDmaTag), 0x10 bytes.
struct _sceDmaTag {
    unsigned short qwc;                 // +0x0
    unsigned char mark;                 // +0x2
    unsigned char id;                   // +0x3
    PS2_PTR(_sceDmaTag) next;                   // +0x4
    unsigned int p[2];                  // +0x8
};

#endif
