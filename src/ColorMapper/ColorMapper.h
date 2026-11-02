// ColorMapper: an 8-bit palette with its 15/16-bit versions and the
// reverse lookups from a 15/16-bit colour to a palette index (0x18B18
// bytes, DrawableCube::Create news it).
#ifndef COLORMAPPER_H
#define COLORMAPPER_H

#include "GameObject/GameObject.h"
#include "RGBTripple/RGBTripple.h"

class ColorMapper : public BaseObject {
public:
    ColorMapper(long handle);
    void Make15and16BitPalettes();
    unsigned char *GetPalette();
    unsigned char *GetLookup15();
    unsigned char *GetLookup16();

    // --- virtuals in retail slot order ---
    virtual ~ColorMapper();

    int m_firstColor;                   // +0x10
    int m_numColors;                    // +0x14
    RGBTripple m_palette[256];          // +0x18
    unsigned short m_palette15[256];    // +0x318, x555
    unsigned short m_palette16[256];    // +0x518, 565
    unsigned char m_lookup15[0x8000];   // +0x718, x555 colour -> palette index
    unsigned char m_lookup16[0x10000];  // +0x8718, 565 colour -> palette index
    char pad18718[0x18B18 - 0x18718];
};

#endif
