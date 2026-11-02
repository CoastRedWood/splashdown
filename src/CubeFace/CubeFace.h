// CubeFace: one face of a sky/environment cube, split into 16 texture
// panels (0x100 bytes).
#ifndef CUBEFACE_H
#define CUBEFACE_H

#include "d3dtypes.h"

class PSXTextureMap;

class CubeFace {
public:
    CubeFace();

    unsigned int m_resolution;          // +0x00 (face resolution; panels are a 4x4 grid of it; unsigned: LoadPanel needs it)
    unsigned short m_panelMask;         // +0x04 (panels present in the file)
    unsigned short m_unk6;              // +0x06
    PSXTextureMap *m_panels[16];        // +0x08 (Cube::LoadPanel news them)
    int m_panelData[16];                // +0x48
    unsigned short m_panelIds[16];      // +0x88 (0xFFFF; Create reads all 0x20 bytes when flag 4 is set)
    int m_panelOffsets[16];             // +0xA8 (-1)
    unsigned short m_visibleMask;       // +0xE8, panels on screen this frame (DrawableCube::CalculateVisibility)
    char padEA[0xF0 - 0xEA];
    _D3DVECTOR m_normal;                // +0xF0 (the ctor runs its default ctor)
};

#endif
