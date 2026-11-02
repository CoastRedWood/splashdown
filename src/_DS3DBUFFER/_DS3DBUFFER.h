// _DS3DBUFFER: DirectSound's 3D buffer parameters as the PS2 build has them,
// 0x70 bytes. The game's _D3DVECTOR is 16 bytes (x, y, z, w) and LONG is a
// 64-bit `long`, so lConeOutsideVolume takes 8 bytes and the floats after it
// start at +0x58 (SoundEmitter's own methods use these offsets). The
// vectors sit on 16-byte boundaries in retail; d3dtypes.h's _D3DVECTOR is
// only 4-aligned, so the gaps are explicit pads.
#ifndef _DS3DBUFFER_DS3DBUFFER_H
#define _DS3DBUFFER_DS3DBUFFER_H

#include "d3dtypes.h"

struct _DS3DBUFFER {
    unsigned int dwSize;                // +0x00
    _D3DVECTOR vPosition __attribute__((aligned(16)));  // +0x10
    _D3DVECTOR vVelocity;               // +0x20
    unsigned int dwInsideConeAngle;     // +0x30
    unsigned int dwOutsideConeAngle;    // +0x34
    _D3DVECTOR vConeOrientation __attribute__((aligned(16)));  // +0x40
    long lConeOutsideVolume;            // +0x50 (64-bit)
    float flMinDistance;                // +0x58
    float flMaxDistance;                // +0x5C
    unsigned int dwMode;                // +0x60
} __attribute__((aligned(16)));

#endif
