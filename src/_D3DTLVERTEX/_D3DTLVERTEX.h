// _D3DTLVERTEX: Direct3D's transformed-and-lit vertex (0x20 bytes). Its
// default ctor is out of line in retail, at 0x2565D0 in LensFlare.cpp's
// range (src/tu/LensFlare.cpp). d3dtypes.h has no copy of it.
#ifndef _D3DTLVERTEX__D3DTLVERTEX_H
#define _D3DTLVERTEX__D3DTLVERTEX_H

struct _D3DTLVERTEX {
    _D3DTLVERTEX();
    float sx, sy, sz, rhw;              // +0x00
    unsigned int color;                 // +0x10
    unsigned int specular;              // +0x14
    float tu, tv;                       // +0x18
};

#endif
