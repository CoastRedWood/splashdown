#ifndef SINIT_D3DINTVECTOR_H
#define SINIT_D3DINTVECTOR_H

// An integer 4-vector with a constructor (the colours the static initializers
// build: GS colour components, 0x80 = full, alpha 0x40 = opaque). The name
// follows PSXWater/PSXWater.h's note on _D3DINTVECTOR; only the stores matter.
struct _D3DINTVECTOR {
    _D3DINTVECTOR();                    // out of line: src/tu/PSXWater.cpp
    _D3DINTVECTOR(int ax, int ay, int az, int aw) { x = ax; y = ay; z = az; w = aw; }
    int x, y, z, w;
};

#endif
