// GridNodeGeometryBase: a terrain grid node's 17x17 heights, normals and
// flags, behind accessors (8 bytes: m_baseHeight, then the vptr). The ctor
// is inline; the dtor is out of line (GridNodeGeometryBase/dtor.cpp), though
// the subclasses' own dtor files inline an empty one.
#ifndef GRIDNODEGEOMETRYBASE_H
#define GRIDNODEGEOMETRYBASE_H

struct _D3DVECTOR;
struct FILE_HANDLE_STRUCT;

#include "ByteVector/ByteVector.h"

class GridNodeGeometryBase {
public:
    GridNodeGeometryBase() {}

    int m_baseHeight;                   // +0x00

    // --- virtuals in retail slot order ---
    virtual ~GridNodeGeometryBase();    // vptr +0x04
    virtual void WriteToFile(FILE_HANDLE_STRUCT *file) = 0;
    virtual int GetHeight(int i, int lod) = 0;
    virtual int GetHeight(int i) = 0;
    virtual void SetHeight(int i, int lod, int height) = 0;
    virtual void SetHeight(int i, int height) = 0;
    virtual ByteVector *GetNormal(int i, int lod) = 0;
    virtual ByteVector *GetNormal(int i) = 0;
    virtual void SetNormal(int i, int lod, _D3DVECTOR *n) = 0;
    virtual void SetNormal(int i, _D3DVECTOR *n) = 0;
    virtual void SetNormal(int i, ByteVector *n) = 0;
    virtual int GetTesselationFlipFlag(int i, int lod) = 0;
    virtual int GetTesselationFlipFlag(int i) = 0;
    virtual void SetTesselationFlipFlag(int i, int flag) = 0;
    virtual void SetTesselationFlipFlag(int i, int lod, int flag) = 0;
    virtual int GetUserFlagData(int i, int lod) = 0;
    virtual void SetUserFlagData(int i, int lod, unsigned char flags) = 0;
    virtual unsigned char GetUserFlagData(int i) = 0;   // unsigned char: callers pass it on without masking
    virtual void SetUserFlagData(int i, unsigned char flags) = 0;
};

#endif
