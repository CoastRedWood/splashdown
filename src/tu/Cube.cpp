// Cube.cpp  @ 0x00227EB0 (text 0x227EB0-0x228850)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp)
//
// Retail's Cube.cpp as one translation unit: the first TU of its tu_map row.
// Its only .rodata is Cube's RTTI (0x410E90-0x410EC8); Cubedraw.cpp's strings
// follow (MakePanelMappingFromString's "," first). __sinit_Cube.cpp (the
// first of the row's three .ctor slots) builds only the axis vectors. `make
// link` places its data as well as its code.


#include "d3dtypes.h"
#include "Cube/Cube.h"
#include "CubeFace/CubeFace.h"
#include "DataAccess/DataAccess.h"
#include "PSXTextureMap/PSXTextureMap.h"
#include "sinit/AxisVectors.h"

class ColorMapper;

extern DataAccess *LpDataAccess;

void SkipPad(long file);
int GetPanelResolution(int faceResolution, int column, int row);
int IsCompressedFormat(ImageDataFormat format);
int BytesPerPixel(ImageDataFormat format);
int PixelFormatFromImageDataFormat(ImageDataFormat format);
void ReviseIpuImageDataFormat(ImageDataFormat &format);
extern "C" void *memcpy(void *dst, const void *src, unsigned int n);

// ---- src/Cube/Initialize.cpp

void Cube::Initialize()
{
    m_fileOffset = 0;
    m_file = -1;
    m_format = (ImageDataFormat)0;
    m_flags = 0;
    m_x4E = 0xFFFF;
    m_position = ZeroVector;
    m_target = ZeroVector;
    m_up = ZeroVector;
    for (int i = 0; i < 6; i++) {
        m_faces[i].m_resolution = 0;
        m_faces[i].m_panelMask = 0;
        m_faces[i].m_unk6 = 0;
        for (int j = 0; j < 16; j++) {
            m_faces[i].m_panels[j] = 0;
            m_faces[i].m_panelData[j] = 0;
            m_faces[i].m_panelIds[j] = 0xFFFF;
            m_faces[i].m_panelOffsets[j] = -1;
        }
        m_faces[i].m_normal.x = 0;
        m_faces[i].m_normal.y = 0;
        m_faces[i].m_normal.z = 0;
    }
}

// ---- src/Cube/ctor.cpp

Cube::Cube()
{
    Initialize();
}

// ---- src/CubeFace/ctor.cpp
CubeFace::CubeFace() {}

// ---- src/Cube/Create.cpp

// Reads the cube header and per-face tables; a nonzero `fail` releases the
// cube even when everything was read.
Cube *Cube::Create(long file, int fail)
{
    unsigned short flags;
    int i;
    m_file = file;
    if (LpDataAccess->FRead(m_file, &m_format, 4, 1) != 1)
        goto error;
    if (LpDataAccess->FRead(m_file, &flags, 2, 1) != 1)
        goto error;
    if (LpDataAccess->FRead(m_file, &m_x4E, 2, 1) != 1)
        goto error;
    for (i = 0; i < 6; i++) {
        if (LpDataAccess->FRead(m_file, &m_faces[i].m_panelMask, 2, 1) != 1)
            goto error;
    }
    for (i = 0; i < 6; i++) {
        if (LpDataAccess->FRead(m_file, &m_faces[i].m_resolution, 4, 1) != 1)
            goto error;
    }
    if (LpDataAccess->FRead(m_file, &m_position, 0xC, 1) != 1)
        goto error;
    if (LpDataAccess->FRead(m_file, &m_target, 0xC, 1) != 1)
        goto error;
    if (LpDataAccess->FRead(m_file, &m_up, 0xC, 1) != 1)
        goto error;
    if (flags & 4) {
        for (i = 0; i < 6; i++) {
            if (LpDataAccess->FRead(m_file, m_faces[i].m_panelIds, 0x20, 1) != 1)
                goto error;
        }
    }
    if (flags & 1) {
        for (i = 0; i < 6; i++) {
            for (int j = 0; j < 16; j++) {
                if (LpDataAccess->FRead(m_file, &m_faces[i].m_panelData[j], 4, 1) != 1)
                    goto error;
            }
        }
    }
    if (flags & 2)
        SkipPad(m_file);
    m_fileOffset = LpDataAccess->FTell(m_file);
    if (!fail)
        return this;
error:
    Release();
    return 0;
}

// ---- src/Cube/SetCamera.cpp

int Cube::SetCamera(_D3DVECTOR *position, _D3DVECTOR *target, _D3DVECTOR *up)
{
    if (position)
        memcpy(&m_position, position, sizeof(_D3DVECTOR));
    if (target)
        memcpy(&m_target, target, sizeof(_D3DVECTOR));
    if (up)
        memcpy(&m_up, up, sizeof(_D3DVECTOR));
    return 1;
}

// ---- src/Cube/LoadPanel.cpp

int Cube::LoadPanel(int face, int panel, ColorMapper *mapper)
{
    int size;
    int resolution = GetPanelResolution(m_faces[face].m_resolution, panel % 4, panel / 4);
    if (IsCompressedFormat(m_format))
        LpDataAccess->FRead(m_file, &size, 4, 1);
    else
        size = resolution * resolution * BytesPerPixel(m_format);
    m_faces[face].m_panels[panel] = new PSXTextureMap(1);
    if (m_faces[face].m_panels[panel]) {
        if (IsCompressedFormat(m_format)) {
            // IPU stream header (read and dropped).
            ImageDataFormat format;
            int width = 0;
            int height = 0;
            int x0C = 0;
            int x10 = 0;
            LpDataAccess->FRead(m_file, &format, 4, 1);
            LpDataAccess->FRead(m_file, &width, 4, 1);
            LpDataAccess->FRead(m_file, &height, 4, 1);
            LpDataAccess->FRead(m_file, &x0C, 4, 1);
            LpDataAccess->FRead(m_file, &x10, 4, 1);
            if (x0C != 0x1000)
                ReviseIpuImageDataFormat(format);
            x0C = height;
            m_faces[face].m_panels[panel]->Load(m_file, resolution, resolution, 1, (ImageDataFormat)0x20, size, 0x613,
                                                mapper, 0, 0, (_D3DBLEND)2, (_D3DBLEND)1, 0x80, 0xFF00FF);
        } else {
            m_faces[face].m_panels[panel]->Load(m_file, resolution, resolution, 1, m_format, size,
                                                PixelFormatFromImageDataFormat(m_format), mapper, 0, 0,
                                                (_D3DBLEND)2, (_D3DBLEND)1, 0x80, 0xFF00FF);
        }
        m_faces[face].m_panels[panel]->PlaceInVideoMemory(1, 0, 0);
    }
    if (m_flags & 2)
        SkipPad(m_file);
    return 1;
}

// ---- src/Cube/LoadFaces.cpp

int Cube::LoadFaces(ColorMapper *mapper)
{
    LpDataAccess->FSeek(m_file, m_fileOffset, 0);
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 16; j++) {
            if (m_faces[i].m_panelMask & (1 << j)) {
                if (!m_faces[i].m_panels[j])
                    LoadPanel(i, j, mapper);
            }
        }
    }
    return 1;
}

// ---- src/Cube/dtor.cpp

Cube::~Cube()
{
    for (int i = 5; i >= 0; i--) {
        for (int j = 15; j >= 0; j--) {
            if (m_faces[i].m_panels[j]) {
                m_faces[i].m_panels[j]->Release();
                m_faces[i].m_panels[j] = 0;
            }
        }
    }
}


// ---- src/sinit/Cube.cpp
// Static initializer: only the axis vectors (sinit/AxisVectors.h).
