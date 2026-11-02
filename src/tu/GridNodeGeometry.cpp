// GridNodeGeometry.cpp  @ 0x002364A0 (text 0x2364A0-0x237C00)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// A retail translation unit without a static initializer (the name is a guess): the
// delta compression tables and the GridNodeGeometryBase/LOD/Plain accessors. Split
// from Fog.cpp: its .rodata labels restart (@696-@719, the GridNodeGeometry RTTI)
// after DrawableGridNodeSharedTextures.cpp's @863. InitializeDeltaCompressionTables
// and its tables could also be that file's tail; nothing decides.

#include "GameObject/GameObject.h"
#include "DmaRenderItem/DmaRenderItem.h"
#include "Fog/Fog.h"
#include "RenderTarget/RenderTarget.h"
#include "PSXRenderTarget/PSXRenderTarget.h"
#include "Camera/Camera.h"
#include "sceGifTag/sceGifTag.h"
#include "d3dtypes.h"
#include "TextureMap/TextureMap.h"
#include "PSXTextureMap/PSXTextureMap.h"
#include "Terrain/Terrain.h"
#include "AgeManager/AgeManager.h"
#include "DrawableGridNodeSharedTextures/DrawableGridNodeSharedTextures.h"
#include "DrawableGridNode/DrawableGridNode.h"
#include "GridNodeGeometryBase/GridNodeGeometryBase.h"
#include "GridNodeGeometryLOD/GridNodeGeometryLOD.h"
#include "DataAccess/DataAccess.h"
#include "ControlInterface/ControlTypes.h"
#include "GridNodeGeometryPlain/GridNodeGeometryPlain.h"

extern "C" void *malloc(unsigned int size);
extern "C" void free(void *p);

#define D3DTSS_COLOROP ((_D3DTEXTURESTAGESTATETYPE)1)
#define D3DTSS_ALPHAOP ((_D3DTEXTURESTAGESTATETYPE)4)
#define D3DTOP_DISABLE 1

// The grid geometry's globals, in retail's .data/.bss order.
static int SizeVerticesBy[18] = {
    0, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x110, 0x121,
};
int TablesInitialized;
unsigned short Decompress[256];
unsigned char Compress[0x10000];
static unsigned char *TempBuffer;   // scratch buffer shared by all grid geometry


// ---- src/global/InitializeDeltaCompressionTables.cpp
extern "C" double pow(double x, double y);


// Builds the 8-bit <-> 16-bit companding tables used for delta compression.
void InitializeDeltaCompressionTables()
{
    int i, j;

    if (TablesInitialized)
        return;
    for (i = 0, j = 0; i < 256; i++) {
        Decompress[i] = (int)pow(1.0 + i, 1.0 + (double)i * (double)i / 65025.0) - 1;
        while (j <= Decompress[i])
            Compress[j++] = i;
    }
    TablesInitialized = 1;
}


// ---- (HeightVertex)
inline HeightVertex::HeightVertex()
{
}


// ---- src/GridNodeGeometryBase/dtor.cpp

inline GridNodeGeometryBase::~GridNodeGeometryBase()
{
}


// Retail writes the HeightVertex ctor and GridNodeGeometryBase dtor copies
// before GridNodeGeometryLOD's dtor: mwcc writes an inline function's
// out-of-line copy after the function that first needs it, and a ctor here
// needed both (as GridNodeGeometryLOD(long) does in ours). Nothing called
// it, so retail's linker stripped it; link_c leaves it out
//.
GridNodeGeometryLOD::GridNodeGeometryLOD()
{
}


// ---- src/GridNodeGeometryLOD/dtor.cpp
extern "C" void free(void *p);




GridNodeGeometryLOD::~GridNodeGeometryLOD()
{
    if (TempBuffer) {
        free(TempBuffer);
        TempBuffer = 0;
    }
}


// ---- src/GridNodeGeometryLOD/WriteToFile.cpp
extern "C" void *malloc(unsigned int size);

struct FILE_HANDLE_STRUCT;
void FWRITE(void *data, unsigned int size, unsigned int count, FILE_HANDLE_STRUCT *file);
int LZWCompress(void *src, void *dst, int size);



// Each block is written LZW-compressed when that makes it smaller:
// compressed size, raw size, then the data.
void GridNodeGeometryLOD::WriteToFile(FILE_HANDLE_STRUCT *file)
{
    int compressed;
    int size;
    if (!TempBuffer)
        TempBuffer = (unsigned char *)malloc(sizeof(verts));
    size = sizeof(m_x910);
    compressed = LZWCompress(m_x910, TempBuffer, size);
    FWRITE(&compressed, 4, 1, file);
    FWRITE(&size, 4, 1, file);
    if (compressed < size)
        FWRITE(TempBuffer, 1, compressed, file);
    else
        FWRITE(m_x910, 1, size, file);
    size = sizeof(m_xB10);
    compressed = LZWCompress(m_xB10, TempBuffer, size);
    FWRITE(&compressed, 4, 1, file);
    FWRITE(&size, 4, 1, file);
    if (compressed < size)
        FWRITE(TempBuffer, 1, compressed, file);
    else
        FWRITE(m_xB10, 1, size, file);
    size = sizeof(verts);
    compressed = LZWCompress(verts, TempBuffer, size);
    FWRITE(&compressed, 4, 1, file);
    FWRITE(&size, 4, 1, file);
    if (compressed < size)
        FWRITE(TempBuffer, 1, compressed, file);
    else
        FWRITE(verts, 1, size, file);
    FWRITE(&m_baseHeight, 4, 1, file);
}


// ---- src/GridNodeGeometryLOD/ctor.cpp
extern "C" void *malloc(unsigned int size);

extern DataAccess *LpDataAccess;

int LZWDecompress(void *dst, void *src, int size);


struct _D3DVECTOR;
struct FILE_HANDLE_STRUCT;







// Each block was written by WriteToFile: compressed size, raw size, then the
// data (LZW-compressed when that was smaller).
GridNodeGeometryLOD::GridNodeGeometryLOD(long file)
{
    int compressed;
    int size;
    if (!TempBuffer)
        TempBuffer = (unsigned char *)malloc(sizeof(verts));
    LpDataAccess->FRead(file, &compressed, 4, 1);
    LpDataAccess->FRead(file, &size, 4, 1);
    if (compressed < size) {
        LpDataAccess->FRead(file, TempBuffer, 1, compressed);
        LZWDecompress(m_x910, TempBuffer, sizeof(m_x910));
    } else {
        LpDataAccess->FRead(file, m_x910, 1, size);
    }
    LpDataAccess->FRead(file, &compressed, 4, 1);
    LpDataAccess->FRead(file, &size, 4, 1);
    if (compressed < size) {
        LpDataAccess->FRead(file, TempBuffer, 1, compressed);
        LZWDecompress(m_xB10, TempBuffer, sizeof(m_xB10));
    } else {
        LpDataAccess->FRead(file, m_xB10, 1, size);
    }
    LpDataAccess->FRead(file, &compressed, 4, 1);
    LpDataAccess->FRead(file, &size, 4, 1);
    if (compressed < size) {
        LpDataAccess->FRead(file, TempBuffer, 1, compressed);
        LZWDecompress(verts, TempBuffer, sizeof(verts));
    } else {
        LpDataAccess->FRead(file, verts, 1, size);
    }
    LpDataAccess->FRead(file, &m_baseHeight, 4, 1);
}


// ---- (GridNormal)
inline GridNormal::GridNormal()
{
}


// Retail writes the GridNormal ctor copy before
// GridNodeGeometryPlain(GridNodeGeometryLOD *): a stripped ctor here needed it
// first, as with GridNodeGeometryLOD() above.
GridNodeGeometryPlain::GridNodeGeometryPlain()
{
}

// ---- src/GridNodeGeometryPlain/ctor.cpp
struct _D3DVECTOR;
struct FILE_HANDLE_STRUCT;



// Copy the full-detail data out of a multi-LOD geometry.
GridNodeGeometryPlain::GridNodeGeometryPlain(GridNodeGeometryLOD *lod)
{
    m_baseHeight = lod->m_baseHeight;
    for (int i = 0; i < SizeVerticesBy[17]; i++) {
        SetHeight(i, lod->GetHeight(i));
        SetNormal(i, lod->GetNormal(i));
        SetTesselationFlipFlag(i, lod->GetTesselationFlipFlag(i));
        SetUserFlagData(i, lod->GetUserFlagData(i));
    }
}


// ---- src/GridNodeGeometryPlain/SetUserFlagData.cpp

void GridNodeGeometryPlain::SetUserFlagData(int i, unsigned char flags)
{
    normals[i].userFlags = flags & 0x7F;
}


// ---- src/GridNodeGeometryLOD/GetUserFlagData_00236D00.cpp

unsigned char GridNodeGeometryLOD::GetUserFlagData(int i)
{
    return verts[i].userFlags;
}


// ---- src/GridNodeGeometryPlain/SetTesselationFlipFlag.cpp

void GridNodeGeometryPlain::SetTesselationFlipFlag(int i, int flag)
{
    normals[i].tessFlip = flag;
}


// ---- src/GridNodeGeometryLOD/GetTesselationFlipFlag_00236D40.cpp

int GridNodeGeometryLOD::GetTesselationFlipFlag(int i)
{
    HeightVertex *v = &verts[i];  // binding v first gives retail's addu order
    return v->tessFlip;
}


// ---- src/GridNodeGeometryPlain/SetNormal.cpp

void GridNodeGeometryPlain::SetNormal(int i, ByteVector *n)
{
    normals[i].normal.x = n->x;
    normals[i].normal.y = n->y;
    normals[i].normal.z = n->z;
}


// ---- src/GridNodeGeometryLOD/GetNormal_00236D90.cpp

ByteVector *GridNodeGeometryLOD::GetNormal(int i)
{
    return &verts[i].normal;
}


// ---- src/GridNodeGeometryPlain/SetHeight.cpp

void GridNodeGeometryPlain::SetHeight(int i, int height)
{
    i += (i / 17) * 3;  // skip the 3 pad entries at the end of each row
    heights[0][i] = height;
}


// ---- src/GridNodeGeometryLOD/GetHeight_00236DE0.cpp

int GridNodeGeometryLOD::GetHeight(int i)
{
    return verts[i].height;
}


// ---- src/GridNodeGeometryPlain/dtor.cpp
extern "C" void free(void *p);




GridNodeGeometryPlain::~GridNodeGeometryPlain()
{
    if (TempBuffer) {
        free(TempBuffer);
        TempBuffer = 0;
    }
}


// ---- src/GridNodeGeometryPlain/WriteToFile.cpp
extern "C" void *malloc(unsigned int size);

struct FILE_HANDLE_STRUCT;
void FWRITE(void *data, unsigned int size, unsigned int count, FILE_HANDLE_STRUCT *file);
int LZWCompress(void *src, void *dst, int size);





// Each block is written LZW-compressed when that makes it smaller:
// compressed size, raw size, then the data.
void GridNodeGeometryPlain::WriteToFile(FILE_HANDLE_STRUCT *file)
{
    int compressed;
    int size;
    FWRITE(&m_baseHeight, 4, 1, file);
    if (!TempBuffer)
        TempBuffer = (unsigned char *)malloc(sizeof(normals));
    size = sizeof(heights);
    compressed = LZWCompress(heights, TempBuffer, size);
    FWRITE(&compressed, 4, 1, file);
    FWRITE(&size, 4, 1, file);
    if (compressed < size)
        FWRITE(TempBuffer, 1, compressed, file);
    else
        FWRITE(heights, 1, size, file);
    size = sizeof(normals);
    compressed = LZWCompress(normals, TempBuffer, size);
    FWRITE(&compressed, 4, 1, file);
    FWRITE(&size, 4, 1, file);
    if (compressed < size)
        FWRITE(TempBuffer, 1, compressed, file);
    else
        FWRITE(normals, 1, size, file);
}


// ---- src/GridNodeGeometryPlain/ctor_00236FF0.cpp
// Retail fully unrolls the row-spreading loop (3 zero pads + 17 copies per row) with
// (d - k) << 1 index math and d*2 / s*2 kept as induction variables. The unroll pragmas
// unroll the inner loops; the first pad and the first copy are written out (their bare
// d / s indices are what get the induction variables), the first copy's index goes
// through j so it stays unfolded like the unrolled ones, and the step order `s -= 17,
// d -= 20` sets the order of the induction-variable updates.

#pragma opt_unroll_count 17
#pragma opt_unroll_instr_count 100
extern "C" void *malloc(unsigned int size);

extern DataAccess *LpDataAccess;

int LZWDecompress(void *dst, void *src, int size);



// Each block was written by WriteToFile: compressed size, raw size, then the
// data (LZW-compressed when that was smaller).
GridNodeGeometryPlain::GridNodeGeometryPlain(long file)
{
    int compressed;
    int size;
    LpDataAccess->FRead(file, &m_baseHeight, 4, 1);
    if (!TempBuffer)
        TempBuffer = (unsigned char *)malloc(sizeof(normals));
    LpDataAccess->FRead(file, &compressed, 4, 1);
    LpDataAccess->FRead(file, &size, 4, 1);
    if (compressed < size) {
        LpDataAccess->FRead(file, TempBuffer, 1, compressed);
        LZWDecompress(heights, TempBuffer, sizeof(heights));
    } else {
        LpDataAccess->FRead(file, heights, 1, size);
    }
    LpDataAccess->FRead(file, &compressed, 4, 1);
    LpDataAccess->FRead(file, &size, 4, 1);
    if (compressed < size) {
        LpDataAccess->FRead(file, TempBuffer, 1, compressed);
        LZWDecompress(normals, TempBuffer, sizeof(normals));
    } else {
        LpDataAccess->FRead(file, normals, 1, size);
    }
    m_qwc = 0x2D;
    m_x492 = 0;
    m_x493 = 0x30;
    m_packet = &m_stcycl;
    m_x498 = 0;
    m_x49C = 0;
    m_stcycl = 0x01000404;
    m_unpack = 0x6D558019;
    m_unpack |= 0x4000;
    m_tail[0] = m_tail[1] = 0;
    m_tail[2] = 0;
    m_tail[3] = 0x14000019;
    m_tail[4] = 0x11000000;
    m_tail[5] = m_tail[6] = m_tail[7] = 0;
    // Spread the 17x17 heights read above out to rows of 20, from the end.
    for (int d = 17 * 20 - 1, s = 17 * 17 - 1; d > 0; s -= 17, d -= 20) {
        heights[0][d] = 0;
        for (int j = 1; j < 3; j++)
            heights[0][d - j] = 0;
        int j = 3;
        heights[0][d - j] = heights[0][s];
        for (j = 4; j < 20; j++)
            heights[0][d - j] = heights[0][s + 3 - j];
    }
}


// ---- src/GridNodeGeometryPlain/GetHeight.cpp

int GridNodeGeometryPlain::GetHeight(int i)
{
    i += (i / 17) * 3;  // skip the 3 pad entries at the end of each row
    return heights[0][i];
}


// ---- src/GridNodeGeometryPlain/GetHeight_002375D0.cpp

int GridNodeGeometryPlain::GetHeight(int i, int lod)
{
    // Retail indexes the height table flat (row * 20 + column).
    return heights[0][i + lod * 20];
}


// ---- src/GridNodeGeometryPlain/SetHeight_002375F0.cpp

void GridNodeGeometryPlain::SetHeight(int i, int lod, int height)
{
    // Retail indexes the height table flat (row * 20 + column).
    heights[0][i + lod * 20] = height;
}


// ---- src/GridNodeGeometryPlain/GetNormal_00237610.cpp

ByteVector *GridNodeGeometryPlain::GetNormal(int i)
{
    return &normals[i].normal;
}


// ---- src/GridNodeGeometryPlain/GetNormal.cpp

ByteVector *GridNodeGeometryPlain::GetNormal(int i, int lod)
{
    return &normals[i + SizeVerticesBy[lod]].normal;
}


// ---- src/GridNodeGeometryPlain/SetNormal_00237650.cpp


// Quantize a unit normal to bytes (x127).
void GridNodeGeometryPlain::SetNormal(int index, _D3DVECTOR *normal)
{
    normals[index].normal.x = (unsigned char)(127.0f * normal->x);
    normals[index].normal.y = (unsigned char)(127.0f * normal->y);
    normals[index].normal.z = (unsigned char)(127.0f * normal->z);
}


// ---- src/GridNodeGeometryPlain/SetNormal_002376E0.cpp


void GridNodeGeometryPlain::SetNormal(int i, int lod, _D3DVECTOR *n)
{
    int k = i + SizeVerticesBy[lod];
    normals[k].normal.x = (unsigned char)(127.0f * n->x);
    normals[k].normal.y = (unsigned char)(127.0f * n->y);
    normals[k].normal.z = (unsigned char)(127.0f * n->z);
}


// ---- src/GridNodeGeometryPlain/GetTesselationFlipFlag_002377A0.cpp

int GridNodeGeometryPlain::GetTesselationFlipFlag(int i)
{
    return normals[i].tessFlip;
}


// ---- src/GridNodeGeometryPlain/GetTesselationFlipFlag.cpp

int GridNodeGeometryPlain::GetTesselationFlipFlag(int i, int lod)
{
    return normals[i + SizeVerticesBy[lod]].tessFlip;
}


// ---- src/GridNodeGeometryPlain/SetTesselationFlipFlag_002377F0.cpp

void GridNodeGeometryPlain::SetTesselationFlipFlag(int i, int lod, int flag)
{
    normals[i + SizeVerticesBy[lod]].tessFlip = flag;
}


// ---- src/GridNodeGeometryPlain/GetUserFlagData_00237830.cpp

unsigned char GridNodeGeometryPlain::GetUserFlagData(int i)
{
    return normals[i].userFlags;
}


// ---- src/GridNodeGeometryPlain/GetUserFlagData.cpp

int GridNodeGeometryPlain::GetUserFlagData(int i, int lod)
{
    return normals[i + SizeVerticesBy[lod]].userFlags;
}


// ---- src/GridNodeGeometryPlain/SetUserFlagData_00237880.cpp

void GridNodeGeometryPlain::SetUserFlagData(int i, int lod, unsigned char flags)
{
    normals[i + SizeVerticesBy[lod]].userFlags = flags & 0x7F;
}


// ---- src/GridNodeGeometryLOD/GetHeight.cpp

int GridNodeGeometryLOD::GetHeight(int i, int lod)
{
    return verts[i + SizeVerticesBy[lod]].height;
}


// ---- src/GridNodeGeometryLOD/SetHeight_00237900.cpp

void GridNodeGeometryLOD::SetHeight(int i, int height)
{
    verts[i].height = height;
}


// ---- src/GridNodeGeometryLOD/SetHeight.cpp

void GridNodeGeometryLOD::SetHeight(int i, int lod, int height)
{
    verts[i + SizeVerticesBy[lod]].height = height;
}


// ---- src/GridNodeGeometryLOD/GetNormal.cpp

ByteVector *GridNodeGeometryLOD::GetNormal(int i, int lod)
{
    return &verts[i + SizeVerticesBy[lod]].normal;
}


// ---- src/GridNodeGeometryLOD/SetNormal.cpp

void GridNodeGeometryLOD::SetNormal(int i, ByteVector *n)
{
    verts[i].normal.x = n->x;
    verts[i].normal.y = n->y;
    verts[i].normal.z = n->z;
}


// ---- src/GridNodeGeometryLOD/SetNormal_002379A0.cpp

void GridNodeGeometryLOD::SetNormal(int i, _D3DVECTOR *n)
{
    verts[i].normal.x = (unsigned char)(127.0f * n->x);
    verts[i].normal.y = (unsigned char)(127.0f * n->y);
    verts[i].normal.z = (unsigned char)(127.0f * n->z);
}


// ---- src/GridNodeGeometryLOD/SetNormal_00237A30.cpp


void GridNodeGeometryLOD::SetNormal(int i, int lod, _D3DVECTOR *n)
{
    int k = i + SizeVerticesBy[lod];
    verts[k].normal.x = (unsigned char)(127.0f * n->x);
    verts[k].normal.y = (unsigned char)(127.0f * n->y);
    verts[k].normal.z = (unsigned char)(127.0f * n->z);
}


// ---- src/GridNodeGeometryLOD/GetTesselationFlipFlag.cpp

int GridNodeGeometryLOD::GetTesselationFlipFlag(int i, int lod)
{
    return verts[i + SizeVerticesBy[lod]].tessFlip;
}


// ---- src/GridNodeGeometryLOD/SetTesselationFlipFlag.cpp

void GridNodeGeometryLOD::SetTesselationFlipFlag(int i, int flag)
{
    verts[i].tessFlip = flag;
}


// ---- src/GridNodeGeometryLOD/SetTesselationFlipFlag_00237B50.cpp

void GridNodeGeometryLOD::SetTesselationFlipFlag(int i, int lod, int flag)
{
    verts[i + SizeVerticesBy[lod]].tessFlip = flag;
}


// ---- src/GridNodeGeometryLOD/GetUserFlagData.cpp

int GridNodeGeometryLOD::GetUserFlagData(int i, int lod)
{
    return verts[i + SizeVerticesBy[lod]].userFlags;
}


// ---- src/GridNodeGeometryLOD/SetUserFlagData_00237BC0.cpp

void GridNodeGeometryLOD::SetUserFlagData(int i, unsigned char flags)
{
    verts[i].userFlags = flags;
}


// ---- src/GridNodeGeometryLOD/SetUserFlagData.cpp

void GridNodeGeometryLOD::SetUserFlagData(int i, int lod, unsigned char flags)
{
    verts[i + SizeVerticesBy[lod]].userFlags = flags;
}

