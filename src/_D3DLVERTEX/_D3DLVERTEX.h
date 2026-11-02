#ifndef _D3DLVERTEX_H
#define _D3DLVERTEX_H

// Direct3D's lit vertex (0x20 bytes). The ctor is out of line and empty
// (_D3DLVERTEX/ctor.cpp); arrays of it are built with __construct_array.
struct _D3DLVERTEX {
    _D3DLVERTEX();
    float x, y, z;
    unsigned int dwReserved;
    unsigned int color, specular;
    float tu, tv;
};

#endif
