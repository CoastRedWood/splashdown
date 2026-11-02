// Cube: a six-faced panel-textured cube read from a file (sky box);
// 0x660 bytes (new Cube in DrawableCube::Create).
#ifndef CUBE_H
#define CUBE_H

#include "TextureMap/TextureMap.h"
#include "CubeFace/CubeFace.h"

class ColorMapper;

class Cube : public BaseObject {
public:
    Cube();
    void Initialize();
    Cube *Create(long file, int fail);
    int SetCamera(_D3DVECTOR *position, _D3DVECTOR *target, _D3DVECTOR *up);
    int LoadFaces(ColorMapper *mapper);
    int LoadPanel(int face, int panel, ColorMapper *mapper);

    // --- virtuals in retail slot order ---
    virtual ~Cube();

    _D3DVECTOR m_position;              // +0x10
    _D3DVECTOR m_target;                // +0x20
    _D3DVECTOR m_up;                    // +0x30
    long m_file;                        // +0x40 (64-bit, -1)
    ImageDataFormat m_format;           // +0x48
    unsigned short m_flags;             // +0x4C (bit 1: panels padded, bit 2: panel ids stored)
    unsigned short m_x4E;               // +0x4E (0xFFFF)
    CubeFace m_faces[6];                // +0x50
    int m_fileOffset;                   // +0x650 (where LoadFaces seeks to)
    char pad654[0x660 - 0x654];
};

#endif
