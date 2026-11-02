// GridNodeGeometryPlain — single-LOD terrain grid geometry (17x17 vertices,
// 0x770 bytes). Layout from the matched accessors and ctors.
#ifndef GRIDNODEGEOMETRYPLAIN_H
#define GRIDNODEGEOMETRYPLAIN_H

#include "GridNodeGeometryLOD/GridNodeGeometryLOD.h"

struct GridNormal {
    GridNormal();
    ByteVector normal;              // +0
    unsigned char tessFlip : 1;     // +3 bit 0
    unsigned char userFlags : 7;    // +3 bits 1-7
};

class GridNodeGeometryPlain : public GridNodeGeometryBase {
public:
    GridNodeGeometryPlain();
    GridNodeGeometryPlain(GridNodeGeometryLOD *lod);
    GridNodeGeometryPlain(long file);

    // --- virtuals in retail slot order ---
    virtual ~GridNodeGeometryPlain();
    virtual void WriteToFile(FILE_HANDLE_STRUCT *file);
    virtual int GetHeight(int i, int lod);
    virtual int GetHeight(int i);
    virtual void SetHeight(int i, int lod, int height);
    virtual void SetHeight(int i, int height);
    virtual ByteVector *GetNormal(int i, int lod);
    virtual ByteVector *GetNormal(int i);
    virtual void SetNormal(int i, int lod, _D3DVECTOR *n);
    virtual void SetNormal(int i, _D3DVECTOR *n);
    virtual void SetNormal(int i, ByteVector *n);
    virtual int GetTesselationFlipFlag(int i, int lod);
    virtual int GetTesselationFlipFlag(int i);
    virtual void SetTesselationFlipFlag(int i, int flag);
    virtual void SetTesselationFlipFlag(int i, int lod, int flag);
    virtual int GetUserFlagData(int i, int lod);
    virtual void SetUserFlagData(int i, int lod, unsigned char flags);
    virtual unsigned char GetUserFlagData(int i);
    virtual void SetUserFlagData(int i, unsigned char flags);

    GridNormal normals[17 * 17];    // +0x008
    char pad48C[0x490 - 0x48C];
    // VIF packet sending the heights (DmaRenderItem-style header).
    short m_qwc;                    // +0x490
    unsigned char m_x492;           // +0x492
    unsigned char m_x493;           // +0x493
    unsigned int *m_packet;         // +0x494
    int m_x498;                     // +0x498
    int m_x49C;                     // +0x49C
#ifdef SPLASHDOWN_PC
    // x86-64: the packet from here (0x2D qwords, REF'd as it lies) must be
    // qword-aligned, as on the PS2
    unsigned int m_stcycl __attribute__((aligned(16)));
#else
    unsigned int m_stcycl;          // +0x4A0
#endif
    unsigned int m_unpack;          // +0x4A4
    unsigned short heights[17][20]; // +0x4A8 (17 rows padded to 20)
    unsigned int m_tail[8];         // +0x750
};

#endif
