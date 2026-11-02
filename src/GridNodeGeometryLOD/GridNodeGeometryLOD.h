// GridNodeGeometryLOD — per-vertex terrain grid data, one table per LOD
// (0xD54 bytes). Layout from the matched accessors, ctor and WriteToFile.
#ifndef GRIDNODEGEOMETRYLOD_H
#define GRIDNODEGEOMETRYLOD_H

#include "GridNodeGeometryBase/GridNodeGeometryBase.h"

// One grid vertex (retail's name, from the ctor's __ct__12HeightVertexFv;
// other files call it GridVertex).
struct HeightVertex {
    HeightVertex();
    unsigned char userFlags;   // +0
    unsigned char m_delta;     // +1, index into Decompress
    unsigned short height;     // +2
    ByteVector normal;         // +4
    unsigned char tessFlip : 1;  // +7 bit 0: tesselation flip flag
};

#include "d3dtypes.h"

// Each TU that uses SizeVerticesBy (the start index of each row of the 17x17
// vertex grid) has its own static copy (Fog.cpp, Griddraw.cpp, GridNode.cpp).

class GridNodeGeometryLOD : public GridNodeGeometryBase {
public:
    GridNodeGeometryLOD();
    GridNodeGeometryLOD(long file);

    // --- virtuals in retail slot order ---
    virtual void WriteToFile(FILE_HANDLE_STRUCT *file);  // the key function: retail's vtable order
    virtual ~GridNodeGeometryLOD();                     //
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

    HeightVertex verts[17 * 17];        // +0x008 (all LODs: SizeVerticesBy[lod] is each one's start)
    char m_x910[0x200];                 // +0x910 (short per-delta start table)
    char m_xB10[0x244];                 // +0xB10 (short per-vertex delta table, padded)
};

#endif
