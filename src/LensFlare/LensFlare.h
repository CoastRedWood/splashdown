// LensFlare: a light's lens flare (LightEmitter::m_lensFlare). Only its two
// setters are out of line (LensFlare.cpp, a TU without an initializer:
// src/tu/LensFlare.cpp); the size and the rest of the layout are unknown.
#ifndef LENSFLARE_LENSFLARE_H
#define LENSFLARE_LENSFLARE_H

#include "d3dtypes.h"

class LensFlare {
public:
    void SetColor(unsigned int color);
    void SetPosition(_D3DVECTOR &v);

    char pad0[0x30];
    _D3DVECTOR m_position;              // +0x30
    char pad40[0xD8 - 0x40];
    unsigned int m_color;               // +0xD8, 0xRRGGBB
};

#endif
