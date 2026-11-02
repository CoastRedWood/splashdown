// Cubedraw.cpp  @ 0x00228850 (text 0x228850-0x22BEE0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp)
//
// Retail's Cubedraw.cpp as one translation unit: MakePanelMappingFromString
// and DrawableCube, after Cube.cpp in Cube.cpp's tu_map row. Its .rodata
// (0x410EC8-0x411220) starts with the helper's "," right after Cube's RTTI,
// holds "Cubedraw.cpp" (the ASSERTs) and ends with DrawableCube's RTTI;
// __sinit_Cubedraw.cpp is the row's second .ctor slot. Its globals are
// defined here: .data 0x3F9080-0x3F90A0 and .bss 0x450BD0-0x451BC8. `make
// link` places its data as well as its code.


#include "GameObject/GameObject.h"
#include "DrawableCube/DrawableCube.h"
#include "DmaRenderItem/DmaRenderItem.h"
#include "SearchPlacement/SearchPlacement.h"
#include "DataAccess/DataAccess.h"
#include "ColorMapper/ColorMapper.h"
#include "soultree_assert_block.h"
#include "ParameterBlock/ParameterBlock.h"
#include "Cube/Cube.h"
#include "sceGifTag/sceGifTag.h"
#include "PSXVideoCard/PSXVideoCard.h"
#include "_D3DLVERTEX/_D3DLVERTEX.h"
#include "d3dtypes.h"
#include "Camera/Camera.h"
#include "GameObject/GameObjectGlobals.h"
#include "PSXTextureMap/PSXTextureMap.h"
#include "VisibleTest/VisibleTest.h"
#include "global/Vu0Inline.h"
#include "StringTable/StringTable.h"
#include "TextureMap/TextureMap.h"
#include "PSXTextureMapManager/PSXTextureMapManager.h"
#include "PSXRenderTarget/PSXRenderTarget.h"
#include "RenderTarget/RenderTarget.h"
#include "PSXCamera/PSXCamera.h"
#include "TokenString/TokenString.h"
#include "QWORD/QWORD.h"
#include "ControlInterface/ControlTypes.h"
#include "sinit/AxisVectors.h"

#pragma inline_depth(4)

extern "C" void *memset(void *dst, int c, unsigned int n);
extern "C" void *memcpy(void *dst, const void *src, unsigned int n);
extern "C" void *memalign(unsigned int alignment, unsigned int size);
extern "C" void free(void *p);
extern "C" int atoi(const char *s);
extern "C" int sprintf(char *buf, const char *fmt, ...);
extern "C" char *strcpy(char *dst, const char *src);
extern "C" char *strcat(char *dst, const char *src);
extern "C" int strcmp(const char *a, const char *b);
extern "C" int rand(void);
extern "C" void srand(unsigned int seed);

typedef int int128_t __attribute__((mode(TI)));

extern DataAccess *LpDataAccess;
extern PSXVideoCard *LpVideoCard;

int KEYDOWN_HANDLER_TEST(unsigned int key, DeviceType device, ProcessNode *node, unsigned int flags);

// Per-panel UV scale (the ctor resets it).
struct UVInfo {
    float m_scale;          // +0x0
    float m_x4;             // +0x4
    float m_x8;             // +0x8
};

// One animated panel sequence (m_sequences' element), 0x28 bytes.
struct PanelSequence {
    float m_startFrame;                 // +0x00
    float m_endFrame;                   // +0x04
    int m_numFrames;                    // +0x08
    float m_framesPerSecond;            // +0x0C
    float m_idleTime;                   // +0x10
    int m_randomStartChance;            // +0x14, percent
    float m_idleElapsed;                // +0x18, BeginTime from the .acb
    float m_retryTime;                  // +0x1C
    float m_frame;                      // +0x20
    unsigned char m_playing : 1;        // +0x24 bit 0
};

// A vertex as BuildRenderList unpacks it: float x/y/z, int w.
struct CubeQword {
    float x, y, z;
    int w;
};

// One QWORD lane as float or int.
#define QF(q, f) (*(float *)&(q).f)
#define QI(q, f) (*(int *)&(q).f)

// .data
int gCubeSubdivColumns = 3;
int gCubeSubdivRows = 2;
int gUseCubeTexture = 1;
int gEnableVUSubdivision = 1;
int RenderCube = 1;

// .bss
int gCubeRecreateGeom;
int DebugFill;
int gEnableCubeZWrite;
UVInfo uvinfo[96];
// The cube drawing tables (the static initializer constructs the vectors);
// retail's .bss has them after uvinfo, before the functions' statics.
_D3DVECTOR FaceNormals[6];
_D3DVECTOR CubePoints[150];
_D3DVECTOR Face2DPoints[25];
void *TempBuffer;
int TempBufferSize;

// ---- src/global/MakePanelMappingFromString.cpp

// "a,b,c,..." -> one mapping per panel.
void MakePanelMappingFromString(char *s, AnimatedPanel *panels)
{
    TokenString tokens(s);
    int panel = 0;
    for (int i = 0; i < 16; i++) {
        char *token = tokens.NextToken(",");
        while (token) {
            panels[panel].m_mapping = atoi(token);
            token = tokens.NextToken(",");
            panel++;
        }
    }
}

// ---- src/DrawableCube/ctor.cpp

DrawableCube::DrawableCube(int enabled) : SearchableGameObject(enabled)
{
    m_searchVisible = 1;
    m_addToList = 1;
    m_cube = 0;
    m_cubes = 0;
    m_currentCube = 0;
    m_animation = 0;
    m_sequences = 0;
    m_numSequences = 0;
    m_numCubes = 0;
    m_colorMapper = 0;
    m_searchPlacement = 0;
    memset(uvinfo, 0, sizeof(uvinfo));
    for (int i = 0; i < 96; i++)
        uvinfo[i].m_scale = 1.0f;
}

// ---- src/DrawableCube/dtor.cpp

DrawableCube::~DrawableCube()
{
    int i;
    for (i = 0; i < 96; i++) {
        if (m_renderItems[i])
            delete m_renderItems[i];
        if (m_uniqueItems[i])
            free(m_uniqueItems[i]);
    }
    if (m_generalItem)
        free(m_generalItem);
    if (m_restoreItem)
        free(m_restoreItem);
    if (TempBuffer)
        free(TempBuffer);
    TempBuffer = 0;
    TempBufferSize = 0;
    if (m_colorMapper)
        m_colorMapper->Release();
    for (i = 0; i < m_numCubes; i++) {
        if (m_cubes[i])
            m_cubes[i]->Release();
    }
    if (m_searchPlacement)
        delete m_searchPlacement;
    if (m_cubes)
        delete[] m_cubes;
    if (m_sequences)
        delete[] m_sequences;
}

// ---- src/DrawableCube/Create.cpp

// Loads the colour mappers and cubes from the file, then builds the render
// items and geometry.
DrawableCube *DrawableCube::Create(char *name)
{
    int i;
    GameObject::Create();
    long file = LpDataAccess->FOpen(name, "rb");
    if (file == -1) {
        Release();
        return 0;
    }
    int size;
    int numMappers;
    int pos;
    if (LpDataAccess->FRead(file, &numMappers, 4, 1) != 1)
        goto fail;
    if (LpDataAccess->FRead(file, &m_numCubes, 4, 1) != 1)
        goto fail;
    for (i = 0; i < numMappers; i++) {
        if (LpDataAccess->FRead(file, &size, 4, 1) != 1)
            goto fail;
        pos = LpDataAccess->FTell(file);
        if (LpDataAccess->FSeek(file, size, 0))
            goto fail;
        if (m_colorMapper)
            m_colorMapper->Release();
        m_colorMapper = new ColorMapper(file);
        if (LpDataAccess->FSeek(file, pos, 0))
            goto fail;
    }
    m_currentCube = 0;
    m_cubes = new Cube *[m_numCubes];
    for (i = 0; i < m_numCubes; i++) {
        if (LpDataAccess->FRead(file, &size, 4, 1) != 1)
            goto fail;
        pos = LpDataAccess->FTell(file);
        if (LpDataAccess->FSeek(file, size, 0))
            goto fail;
        m_cubes[i] = (new Cube)->Create(file, 0);
        if (!m_cubes[i])
            goto fail;
        if (LpDataAccess->FSeek(file, pos, 0))
            goto fail;
    }
    for (i = 0; i < m_numCubes; i++)
        LoadCube(m_cubes[i]);
    m_currentCube = 0;
    m_cube = m_cubes[m_currentCube];
    m_generalItem = CreateGeneralDataItem();
    for (i = 0; i < 96; i++) {
        m_renderItems[i] = new DmaRenderItem;
        if (!m_renderItems[i])
            goto fail;
        m_uniqueItems[i] = CreateUniqueDataItem();
        if (!m_uniqueItems[i])
            goto fail;
    }
    m_restoreItem = CreateRestoreDataItem();
    CreateGeometry();
    m_colorMapper->Release();
    m_colorMapper = 0;
    LpDataAccess->FClose(file);
    CreateAnimatedPanels(name);
    return this;
fail:
    Release();
    return 0;
}

// ---- src/DrawableCube/CreateAnimatedPanels.cpp

// An inlined strrchr(name, '.').
static inline char *FindExtension(char *s)
{
    char *start = s;
    while (*s++)
        ;
    do {
        s--;
    } while (s != start && *s != '.');
    return *s == '.' ? s : 0;
}

#pragma fast_fptosi on
// Reads the .acb file next to the cube: the animation sequences, then which
// sequences each mapped panel cycles through.
void DrawableCube::CreateAnimatedPanels(char *name)
{
    char fileName[0x110];
    char section[0x110];
    // Declaration order sets the registers (an asserting function: each local
    // takes the lowest $s register no earlier-declared, overlapping local holds).
    int frame;
    PanelSequence *seq;
    AnimatedPanel *p;
    int face;
    int panel;
    int i;
    int s;
    char *token;
    strcpy(fileName, name);
    char *ext = FindExtension(fileName);
    if (ext)
        strcpy(ext, ".acb");
    else
        strcat(fileName, (char *)".acb");
    ParameterBlock block;
    char mapping[0x110];
    char key[0x110];
    char list[0x110];
    if (!block.OpenFile(fileName, 0))
        return;
    block.ReadParameterBlock("Sequences");
    block.GetParameter("NumSequences", 0, &m_numSequences);
    if (m_numSequences <= 0)
        return;
    m_sequences = new PanelSequence[m_numSequences];
    for (i = 0; i < m_numSequences; i++) {
        seq = &m_sequences[i];
        sprintf(section, "Sequence%d", i + 1);
        block.ReadParameterBlock(section);
        block.GetParameter("StartFrame", 0.0f, &seq->m_startFrame);
        seq->m_startFrame -= 1.0f;
        block.GetParameter("EndFrame", 0.0f, &seq->m_endFrame);
        seq->m_endFrame -= 1.0f;
        seq->m_numFrames = (int)(seq->m_endFrame - seq->m_startFrame);
        block.GetParameter("FramesPerSecond", 0.0f, &seq->m_framesPerSecond);
        block.GetParameter("IdleTime", 0.0f, &seq->m_idleTime);
        block.GetParameter("RandomStartChance", 100, &seq->m_randomStartChance);
        float beginTime;
        block.GetParameter("BeginTime", 0.0f, &beginTime);
        seq->m_idleElapsed = beginTime;
        seq->m_retryTime = 1.0f;
        seq->m_frame = 0;
        seq->m_playing = 0;
    }
    block.ReadParameterBlock("CubeMapping");
    for (face = 0; face < 6; face++) {
        sprintf(key, "Face%dMapping", face + 1);
        block.GetParameter(key, "", mapping, -1);
        MakePanelMappingFromString(mapping, m_panels[face]);
    }
    for (face = 0; face < 6; face++) {
        sprintf(key, "Face%dMapping", face + 1);
        block.GetParameter(key, "", mapping, -1);
        MakePanelMappingFromString(mapping, m_panels[face]);
    }
    for (face = 0; face < 6; face++) {
        for (panel = 0; panel < 16; panel++) {
            p = &m_panels[face][panel];
            if (!p->m_mapping)
                continue;
            sprintf(section, "Panel%d", p->m_mapping);
            block.ReadParameterBlock(section);
            block.GetParameter("SequenceList", "", list, -1);
            p->m_numSequences = 0;
            if (!strcmp(list, ""))
                continue;
            TokenString tokens(list);
            token = tokens.NextToken(",");
            while (token) {
#line 331 "Cubedraw.cpp"
                ASSERTF(p->m_numSequences < 3, ("Panel%d - SequenceList too high", p->m_mapping));
                p->m_sequences[p->m_numSequences] = atoi(token) - 1;
                s = p->m_sequences[p->m_numSequences];
#line 335 "Cubedraw.cpp"
                ASSERTF(s < m_numSequences, ("PanelID %d sequence number is greater than the number of sequences", p->m_mapping));
                seq = &m_sequences[s];
                for (frame = (int)seq->m_startFrame; frame < seq->m_endFrame; frame++) {
#line 341 "Cubedraw.cpp"
                    ASSERTF(m_cubes[frame]->m_faces[face].m_panels[panel], ("Missing panel (face %d panel %d frame %d sequence %d)", face + 1, panel + 1, frame + 1, p->m_sequences[p->m_numSequences] + 1));
                }
                token = tokens.NextToken(",");
                p->m_numSequences++;
            }
        }
    }
}
#pragma fast_fptosi reset

// ---- src/DrawableCube/EnableCubeZWrite.cpp

void DrawableCube::EnableCubeZWrite(int enable)
{
    gEnableCubeZWrite = enable;
}

// ---- src/DrawableCube/EnableVUSubdivision.cpp

void DrawableCube::EnableVUSubdivision(int enable)
{
    gEnableVUSubdivision = enable;
}

// ---- src/DrawableCube/CreateGeneralDataItem.cpp

struct GeneralPacket {
    unsigned int m_vif[4];      // +0x000
    sceGifTag m_gifTag;         // +0x010
    long m_test;                // +0x020
    long m_testReg;             // +0x028
    unsigned int m_vifMid[4];   // +0x030
    float m_matrix[4][4];       // +0x040
    char pad80[0x180 - 0x80];
    float m_matrix2[4][4];      // +0x180
    float m_x1C0[4];            // +0x1C0
    float m_scale[4][4];        // +0x1D0
    char pad210[0x260 - 0x210];
    sceGifTag m_primTag;        // +0x260
    sceGifTag m_tag2;           // +0x270
    int m_tag2Data[4];          // +0x280
    sceGifTag m_tag3;           // +0x290
    int m_tag3Data[4];          // +0x2A0
    float m_x2B0[4];            // +0x2B0
    float m_x2C0[4];            // +0x2C0
    int m_x2D0[4];              // +0x2D0
    unsigned int m_vifEnd[4];   // +0x2E0
    char pad2F0[0x10];
};

// The VifDataItem this function builds, with its packet.
struct CubeGeneralItem {
    short m_numQwords;          // +0x00
    unsigned char m_02;         // +0x02
    unsigned char m_03;         // +0x03
    PS2_PTR(GeneralPacket) m_data;      // +0x04
    int m_08;                   // +0x08
    int m_0C;                   // +0x0C
    GeneralPacket m_packet;     // +0x10
};

// The shared cube packet: TEST, identity matrices, a PRIM GIFtag and the
// microprogram kick.
VifDataItem *DrawableCube::CreateGeneralDataItem()
{
    CubeGeneralItem *item = (CubeGeneralItem *)memalign(0x40, sizeof(CubeGeneralItem));
    memset(item, 0, sizeof(CubeGeneralItem));
    item->m_packet.m_vif[0] = 0x11000000;   // FLUSH
    item->m_packet.m_vif[1] = 0x03000000;   // BASE
    item->m_packet.m_vif[2] = 0x02000000;   // OFFSET
    item->m_packet.m_vif[3] = 0x6C028320;   // UNPACK
    *(int128_t *)&item->m_packet.m_gifTag = 0;
    item->m_packet.m_gifTag.FLG = 0;
    item->m_packet.m_gifTag.EOP = 1;
    item->m_packet.m_gifTag.NREG = 1;
    item->m_packet.m_gifTag.NLOOP = 1;
    item->m_packet.m_gifTag.REGS0 = 0xE;    // A+D
    item->m_packet.m_test = 0x5000C;
    item->m_packet.m_testReg = 0x47;               // TEST_1
    item->m_packet.m_vifMid[0] = 0x14000000;       // MSCAL
    item->m_packet.m_vifMid[3] = 0x6C2A8000;       // UNPACK
    item->m_packet.m_matrix[0][0] = 1.0f;
    item->m_packet.m_matrix[1][1] = 1.0f;
    item->m_packet.m_matrix[2][2] = 1.0f;
    item->m_packet.m_matrix[3][0] = m_cube->m_position.x;
    item->m_packet.m_matrix[3][1] = m_cube->m_position.y;
    item->m_packet.m_matrix[3][2] = m_cube->m_position.z;
    item->m_packet.m_matrix[3][3] = 1.0f;
    item->m_packet.m_matrix2[0][0] = 1.0f;
    item->m_packet.m_matrix2[1][1] = 1.0f;
    item->m_packet.m_matrix2[2][2] = 1.0f;
    item->m_packet.m_matrix2[3][3] = 1.0f;
    item->m_packet.m_scale[0][0] = item->m_packet.m_scale[0][1] = item->m_packet.m_scale[0][2] = 127.0f;
    item->m_packet.m_scale[1][0] = item->m_packet.m_scale[1][1] = item->m_packet.m_scale[1][2] = 127.0f;
    item->m_packet.m_scale[2][0] = item->m_packet.m_scale[2][1] = item->m_packet.m_scale[2][2] = 127.0f;
    item->m_packet.m_scale[3][0] = item->m_packet.m_scale[3][1] = item->m_packet.m_scale[3][2] = 127.0f;
    *(int128_t *)&item->m_packet.m_primTag = 0;
    item->m_packet.m_primTag.EOP = 1;
    item->m_packet.m_primTag.PRE = 1;
    item->m_packet.m_primTag.PRIM = 0x1D;
    item->m_packet.m_primTag.FLG = 0;
    item->m_packet.m_primTag.NREG = 3;
    item->m_packet.m_primTag.REGS0 = 2;
    item->m_packet.m_primTag.REGS1 = 1;
    item->m_packet.m_primTag.REGS2 = 4;
    item->m_packet.m_primTag.NLOOP = 5;
    *(int128_t *)&item->m_packet.m_tag2 = 0;
    item->m_packet.m_tag2.NLOOP = 1;
    item->m_packet.m_tag2.EOP = 1;
    item->m_packet.m_tag2.PRE = 1;
    item->m_packet.m_tag2.FLG = 0;
    item->m_packet.m_tag2.NREG = 1;
    item->m_packet.m_tag2.REGS0 = 0xF;
    item->m_packet.m_tag2Data[3] = 0x7F;
    *(int128_t *)&item->m_packet.m_tag3 = 0;
    item->m_packet.m_tag3.NLOOP = 1;
    item->m_packet.m_tag3.EOP = 1;
    item->m_packet.m_tag3.PRE = 1;
    item->m_packet.m_tag3.FLG = 0;
    item->m_packet.m_tag3.NREG = 1;
    item->m_packet.m_tag3.REGS0 = 0xF;
    item->m_packet.m_tag3Data[3] = 0x7F;
    item->m_packet.m_x2B0[0] = item->m_packet.m_x2B0[1] = item->m_packet.m_x2B0[2] = item->m_packet.m_x2B0[3] = 0.0f;
    item->m_packet.m_x2C0[0] = item->m_packet.m_x2C0[1] = item->m_packet.m_x2C0[2] = item->m_packet.m_x2C0[3] = 0.0f;
    item->m_packet.m_x2D0[0] = 6;
    item->m_packet.m_x2D0[3] = 0;
    item->m_packet.m_vifEnd[0] = 0x14000019;       // MSCAL
    item->m_packet.m_vifEnd[1] = 0x10000000;       // FLUSHE
    item->m_packet.m_vifEnd[2] = 0x030000B5;       // BASE
    item->m_packet.m_vifEnd[3] = 0x02000190;       // OFFSET
    item->m_numQwords = 0x30;
    item->m_02 = 0;
    item->m_03 = 0x30;
    item->m_data = &item->m_packet;
    item->m_08 = 0;
    item->m_0C = 0;
    return (VifDataItem *)item;
}

// ---- src/DrawableCube/CreateUniqueDataItem.cpp

struct UniquePacket {
    unsigned int m_vif[4];      // +0x00
    sceGifTag m_gifTag;         // +0x10
    long m_tex0;                // +0x20 (set by UpdateUniqueDataItem)
    long m_tex0Reg;             // +0x28
    long m_zbuf;                // +0x30
    long m_zbufReg;             // +0x38
    unsigned int m_vifMid[4];   // +0x40
    float m_matrix[3][4];       // +0x50
    float m_color[4];           // +0x80
    unsigned int m_vifEnd[4];   // +0x90
    char padA0[0x10];
};

// The VifDataItem this function builds, with its packet.
struct CubeUniqueItem {
    short m_numQwords;          // +0x00
    unsigned char m_02;         // +0x02
    unsigned char m_03;         // +0x03
    PS2_PTR(UniquePacket) m_data;       // +0x04
    int m_08;                   // +0x08
    int m_0C;                   // +0x0C
    UniquePacket m_packet;      // +0x10
};

// The per-cube packet: TEX0 and ZBUF writes, an identity rotation and the
// face colour for the microprogram.
VifDataItem *DrawableCube::CreateUniqueDataItem()
{
    CubeUniqueItem *item = (CubeUniqueItem *)memalign(0x40, sizeof(CubeUniqueItem));
    memset(item, 0, sizeof(CubeUniqueItem));
    item->m_packet.m_vif[0] = 0x11000000;   // FLUSH
    item->m_packet.m_vif[1] = 0x03000000;   // BASE
    item->m_packet.m_vif[2] = 0x02000000;   // OFFSET
    item->m_packet.m_vif[3] = 0x6C038320;   // UNPACK
    *(int128_t *)&item->m_packet.m_gifTag = 0;
    item->m_packet.m_gifTag.FLG = 0;
    item->m_packet.m_gifTag.EOP = 1;
    item->m_packet.m_gifTag.NREG = 2;
    item->m_packet.m_gifTag.NLOOP = 1;
    item->m_packet.m_gifTag.REGS0 = 0xE;    // A+D
    item->m_packet.m_gifTag.REGS1 = 0xE;    // A+D
    item->m_packet.m_tex0Reg = 6;           // TEX0_1
    UniquePacket *packet = &item->m_packet;
    packet->m_zbuf = (long)LpVideoCard->GetZBufferAddress() | ((long)LpVideoCard->GetZBufferFormat() << 24)
                   | ((long)(gEnableCubeZWrite ? 0 : 1) << 32);
    packet->m_zbufReg = 0x4E;               // ZBUF_1
    packet->m_vifMid[0] = 0x14000000;       // MSCAL
    packet->m_vifMid[3] = 0x6C048000;       // UNPACK
    packet->m_matrix[0][0] = 1.0f;
    packet->m_matrix[1][1] = 1.0f;
    packet->m_matrix[2][2] = 1.0f;
    packet->m_color[0] = m_cube->m_position.x;
    packet->m_color[1] = m_cube->m_position.y;
    packet->m_color[2] = m_cube->m_position.z;
    packet->m_color[3] = 1.0f;
    packet->m_vifEnd[0] = 0x14000019;       // MSCAL
    packet->m_vifEnd[1] = 0x10000000;       // FLUSHE
    packet->m_vifEnd[2] = 0x030000B5;       // BASE
    packet->m_vifEnd[3] = 0x02000190;       // OFFSET
    item->m_numQwords = 11;
    item->m_02 = 0;
    item->m_03 = 0x30;
    item->m_data = &item->m_packet;
    item->m_08 = 0;
    item->m_0C = 0;
    return (VifDataItem *)item;
}

// ---- src/DrawableCube/CreateRestoreDataItem.cpp

struct RestorePacket {
    unsigned int m_vif[4];      // +0x00
    sceGifTag m_gifTag;         // +0x10
    long m_zbuf;                // +0x20
    long m_zbufReg;             // +0x28
    unsigned int m_vifEnd[4];   // +0x30
};

// The VifDataItem this function builds, with its packet.
struct CubeRestoreItem {
    short m_numQwords;          // +0x00
    unsigned char m_02;         // +0x02
    unsigned char m_03;         // +0x03
    PS2_PTR(RestorePacket) m_data;      // +0x04
    int m_08;                   // +0x08
    int m_0C;                   // +0x0C
    RestorePacket m_packet;     // +0x10
};

// A VIF packet that unpacks a GIFtag plus an A+D write restoring ZBUF to the
// video card's z-buffer.
VifDataItem *DrawableCube::CreateRestoreDataItem()
{
    CubeRestoreItem *item = (CubeRestoreItem *)memalign(0x40, sizeof(CubeRestoreItem));
    memset(item, 0, sizeof(CubeRestoreItem));
    item->m_packet.m_vif[0] = 0x11000000;   // FLUSH
    item->m_packet.m_vif[1] = 0x03000000;   // BASE
    item->m_packet.m_vif[2] = 0x02000000;   // OFFSET
    item->m_packet.m_vif[3] = 0x6C028320;   // UNPACK
    *(int128_t *)&item->m_packet.m_gifTag = 0;
    item->m_packet.m_gifTag.FLG = 0;
    item->m_packet.m_gifTag.EOP = 1;
    item->m_packet.m_gifTag.NREG = 1;
    item->m_packet.m_gifTag.NLOOP = 1;
    item->m_packet.m_gifTag.REGS0 = 0xE;    // A+D
    RestorePacket *packet = &item->m_packet;
    packet->m_zbuf = (long)LpVideoCard->GetZBufferAddress() | ((long)LpVideoCard->GetZBufferFormat() << 24);
    packet->m_zbufReg = 0x4E;               // ZBUF_1
    packet->m_vifEnd[0] = 0x14000000;       // MSCAL
    packet->m_vifEnd[1] = 0x10000000;       // FLUSHE
    item->m_numQwords = 4;
    item->m_02 = 0;
    item->m_03 = 0x30;
    item->m_data = &item->m_packet;
    item->m_08 = 0;
    item->m_0C = 0;
    return (VifDataItem *)item;
}

// ---- src/DrawableCube/CreateGeometry.cpp
// The memcpy loop steps p again (`p = v; ... p++`): that second live range of p becomes
// a web-split temp, the copy p = v is coalesced (v dies there), and the merged node
// takes the split temp's number, which sits between the loop-2 row counter's split temp
// and the face*25 strength-reduced temp, as retail's slots 0xC0/0xB0/0xA0 need. Split
// temps are created in order of each variable's first appearance, so p must appear
// before row: the dead `p = 0;` does that (it is removed later). `row = 0;` before `v =
// verts;` puts row's def ahead of the spill temp for v, which then takes $v1 like
// retail.


// Builds the six faces of the sky cube: a 5x5 vertex grid per face, rendered
// as 4x4 cells.
int DrawableCube::CreateGeometry()
{
    _D3DLVERTEX verts[25];
    int face;
    int n;
    int k;
    _D3DLVERTEX *v;
    int row;
    int col;
    int cx;
    _D3DLVERTEX *q;
    _D3DLVERTEX *p;
    int ry;
    DmaRenderItem *item;
    int fog;
    p = 0;
    n = 0;
    for (face = 0; face < 6; face++) {
        row = 0;
        v = verts;
        p = v;
        for (ry = 0; row < 5; row++, ry += 128) {
            for (col = 0, cx = 0, q = p; col < 5; col++, cx += 128, p++, q++) {
                switch (face) {
                case 0:
                    q->x = 100.0f * (cx - 256);
                    q->y = 100.0f * (256 - ry);
                    q->z = 25600.0f;
                    FaceNormals[face].y = 0.0f;
                    FaceNormals[face].x = 0.0f;
                    FaceNormals[face].z = -1.0f;
                    break;
                case 1:
                    q->x = -25600.0f;
                    q->y = 100.0f * (256 - ry);
                    q->z = 100.0f * (cx - 256);
                    FaceNormals[face].y = 0.0f;
                    FaceNormals[face].z = 0.0f;
                    FaceNormals[face].x = 1.0f;
                    break;
                case 2:
                    q->x = 100.0f * (256 - cx);
                    q->y = 100.0f * (256 - ry);
                    q->z = -25600.0f;
                    FaceNormals[face].y = 0.0f;
                    FaceNormals[face].x = 0.0f;
                    FaceNormals[face].z = 1.0f;
                    break;
                case 3:
                    q->x = 25600.0f;
                    q->y = 100.0f * (256 - ry);
                    q->z = 100.0f * (256 - cx);
                    FaceNormals[face].y = 0.0f;
                    FaceNormals[face].z = 0.0f;
                    FaceNormals[face].x = -1.0f;
                    break;
                case 4:
                    q->x = 100.0f * (cx - 256);
                    q->y = 25600.0f;
                    q->z = 100.0f * (ry - 256);
                    FaceNormals[face].z = 0.0f;
                    FaceNormals[face].x = 0.0f;
                    FaceNormals[face].y = -1.0f;
                    break;
                case 5:
                    q->x = 100.0f * (cx - 256);
                    q->y = -25600.0f;
                    q->z = 100.0f * (256 - ry);
                    FaceNormals[face].z = 0.0f;
                    FaceNormals[face].x = 0.0f;
                    FaceNormals[face].y = 1.0f;
                    break;
                }
                q->color = 0xFFFFFFFF;
                q->tu = col / 4.0f;
                q->tv = row / 4.0f;
            }
        }
        k = 0;
        for (row = 0; row < 4; row++) {
            for (col = 0; col < 4; col++) {
                item = m_renderItems[n];
                fog = face < 4 && row >= 2;
                if (gEnableVUSubdivision)
                    BuildRenderList(item, &verts[k], fog, face);
                else
                    BuildRenderListCPU(item, &verts[k], fog, face);
                k++;
                n++;
            }
            k++;
        }
        p = v;
        for (row = 0; row < 25; row++) {
            memcpy(&CubePoints[face * 25 + row], p, 16);
            CubePoints[face * 25 + row].w = 1.0f;
            p++;
        }
    }
    return 1;
}

// ---- src/_D3DLVERTEX/ctor.cpp

_D3DLVERTEX::_D3DLVERTEX() {}

// ---- src/DrawableCube/BuildRenderList.cpp
// BuildRenderList__12DrawableCubeFP13DmaRenderItemP11_D3DLVERTEXii  @ 0x0022A2A0
// NEAR-MISS: 13/255 words: cols and axis swap $s3/$s4 (retail: cols $s3, axis $s4). Tried
// all declaration orders, setup statement orders, case statement orders, an axis default.

// Read as a volatile lvalue: that is a codegen spelling, not a claim about the data: without it mwcc
// copy-propagates the load into an optimizer temp (numbered below every named
// local) and `cols` takes $s4 instead of $s3; with it `cols` stays a named local
// whose declaration position (after corner, before axis) sets its register.

void DrawableCube::BuildRenderList(DmaRenderItem *item, _D3DLVERTEX *verts, int fog, int face)
{
    sceGifTag tag;
    CubeQword q;
    int rows;
    float dx, dy, dz;
    int nloop;
    _D3DLVERTEX *origin;
    _D3DLVERTEX *corner;
    int cols;
    int axis;
    *(int128_t *)&tag = 0;
    tag.EOP = 1;
    tag.PRE = 1;
    tag.PRIM = (unsigned long)fog << 5 | 0x1C;
    tag.FLG = 0;
    tag.NREG = 3;
    tag.REGS0 = 2;
    tag.REGS1 = 1;
    tag.REGS2 = 4;
    item->Start((DmaRenderDestination)1, 0x80);
    rows = gCubeSubdivRows;
    cols = (volatile int &)gCubeSubdivColumns;
    dx = dy = dz = 0.0f;
    nloop = (cols + 1) * 2;
    origin = verts;
    switch (face) {
    case 0:
    case 2:
        axis = 0;
        dx = (verts[1].x - verts[0].x) / cols;
        verts += 5;
        dy = (verts->y - origin->y) / rows;
        break;
    case 1:
    case 3:
        axis = 1;
        dz = (verts[1].z - verts[0].z) / cols;
        verts += 5;
        dy = (verts->y - origin->y) / rows;
        break;
    default:
        axis = 2;
        dx = (verts[1].x - verts[0].x) / cols;
        verts += 5;
        dz = (verts->z - origin->z) / rows;
        break;
    }
    corner = verts + 1;
    item->DmaAddVifCode(0x11000000);
    item->DmaBeginUnpack(0, 12);
    tag.NLOOP = nloop;
    item->DmaAddGifTagToUnpack(&tag);
    item->DmaEndUnpack();
    item->DmaBeginUnpack(-1, 12);
    q.x = origin->x;
    QF(q, y) = origin->y;
    QF(q, z) = origin->z;
    QI(q, w) = axis;
    item->DmaAddQwordToUnpack((QWORD *)&q);
    q.x = corner->x;
    QF(q, y) = corner->y;
    QF(q, z) = corner->z;
    QI(q, w) = axis;
    item->DmaAddQwordToUnpack((QWORD *)&q);
    q.x = 0.03125f;
    QF(q, y) = 0.03125f;
    QI(q, z) = 0;
    QI(q, w) = 0;
    item->DmaAddQwordToUnpack((QWORD *)&q);
    q.x = dx;
    QF(q, y) = dy;
    QF(q, z) = dz;
    QI(q, w) = 0;
    item->DmaAddQwordToUnpack((QWORD *)&q);
    q.x = 0.9375f / cols;
    QF(q, y) = 0.9375f / rows;
    QI(q, z) = cols;
    QI(q, w) = rows;
    item->DmaAddQwordToUnpack((QWORD *)&q);
    item->DmaEndUnpack();
    item->DmaAddVifCode(0x17000000);
    item->End();
}

// ---- src/DrawableCube/BuildRenderListCPU.cpp

// CPU fallback for BuildRenderList: unpacks every vertex of the face's grid,
// a row of triangle-strip pairs at a time, followed by its UVs. The vertex
// lanes are written through a float/int union (array elements go through
// address registers, as in retail).
void DrawableCube::BuildRenderListCPU(DmaRenderItem *item, _D3DLVERTEX *verts, int fog, int face)
{
    sceGifTag tag;
    union {
        float f[4];
        int i[4];
    } q;
    float uv[2];
    int rows;
    int cols;
    float dx, dy, dz;
    int nloop;
    int col, row;
    *(int128_t *)&tag = 0;
    tag.EOP = 1;
    tag.PRE = 1;
    tag.PRIM = (unsigned long)fog << 5 | 0x1C;
    tag.FLG = 0;
    tag.NREG = 3;
    tag.REGS0 = 2;
    tag.REGS1 = 1;
    tag.REGS2 = 4;
    item->Start((DmaRenderDestination)1, 0x80);
    rows = gCubeSubdivRows;
    cols = gCubeSubdivColumns;
    dx = dy = dz = 0.0f;
    nloop = (cols + 1) * 2;
    switch (face) {
    case 0:
    case 2:
        dx = (verts[1].x - verts[0].x) / cols;
        dy = (verts[5].y - verts[0].y) / rows;
        break;
    case 1:
    case 3:
        dz = (verts[1].z - verts[0].z) / cols;
        dy = (verts[5].y - verts[0].y) / rows;
        break;
    default:
        dx = (verts[1].x - verts[0].x) / cols;
        dz = (verts[5].z - verts[0].z) / rows;
        break;
    }
    float du = 0.9375f / cols;
    float dv = 0.9375f / rows;
    for (row = 0; row < rows; row++) {
        item->DmaBeginUnpack(0, 12);
        tag.NLOOP = nloop;
        item->DmaAddGifTagToUnpack(&tag);
        item->DmaEndUnpack();
        item->DmaBeginUnpack(-1, 12);
        for (col = 0; col < cols + 1; col++) {
            switch (face) {
            case 0:
            case 2:
                q.f[0] = verts->x + dx * col;
                q.f[1] = verts->y + dy * row;
                q.f[2] = verts->z;
                q.i[3] = 14;
                item->DmaAddQwordToUnpack((QWORD *)&q);
                q.f[0] = verts->x + dx * col;
                q.f[1] = verts->y + dy * (row + 1);
                q.f[2] = verts->z;
                q.i[3] = 14;
                item->DmaAddQwordToUnpack((QWORD *)&q);
                break;
            case 1:
            case 3:
                q.f[0] = verts->x;
                q.f[1] = verts->y + dy * row;
                q.f[2] = verts->z + dz * col;
                q.i[3] = 14;
                item->DmaAddQwordToUnpack((QWORD *)&q);
                q.f[0] = verts->x;
                q.f[1] = verts->y + dy * (row + 1);
                q.f[2] = verts->z + dz * col;
                q.i[3] = 14;
                item->DmaAddQwordToUnpack((QWORD *)&q);
                break;
            default:
                q.f[0] = verts->x + dx * col;
                q.f[1] = verts->y;
                q.f[2] = verts->z + dz * row;
                q.i[3] = 14;
                item->DmaAddQwordToUnpack((QWORD *)&q);
                q.f[0] = verts->x + dx * col;
                q.f[1] = verts->y;
                q.f[2] = verts->z + dz * (row + 1);
                q.i[3] = 14;
                item->DmaAddQwordToUnpack((QWORD *)&q);
                break;
            }
        }
        item->DmaEndUnpack();
        item->DmaBeginUnpack(nloop * 2 + 1, 4);
        for (col = 0; col < cols + 1; col++) {
            uv[0] = 0.03125f + du * col;
            uv[1] = 0.03125f + dv * row;
            item->DmaAddDataToUnpack(uv, 2);
            uv[1] += dv;
            item->DmaAddDataToUnpack(uv, 2);
        }
        item->DmaEndUnpack();
        item->DmaAddVifCode(0x17000000);
    }
    item->End();
}

// ---- src/DrawableCube/LoadCube.cpp

int DrawableCube::LoadCube(Cube *cube)
{
    m_cube = cube;
    m_cube->LoadFaces(m_colorMapper);
    return 1;
}

// ---- src/DrawableCube/FaceVisible.cpp

static inline float DotProduct(_D3DVECTOR *a, _D3DVECTOR *b)
{
    float r;
    asm("lqc2 vf4, 0x0(%1); lqc2 vf5, 0x0(%2); vmul.xyz vf5, vf4, vf5; vaddy.x vf5, vf5, vf5y; vaddz.x vf5, vf5, vf5z; qmfc2 $2, vf5; mtc1 $2, %0"
        : "=f"(r) : "r"(a), "r"(b) : "$2");
    return r;
}

int DrawableCube::FaceVisible(Camera *camera, _D3DVECTOR *normal, _D3DVECTOR *point)
{
    float threshold = 0.0046153799630701542f * (camera->m_fov - 10.0f);
    if (DotProduct(normal, point) > threshold)
        return 0;
    return 1;
}

// ---- src/DrawableCube/CalculateVisibility.cpp

_D3DMATRIX IdentityMatrix();

extern VisibleTest *VisTest;

// Marks which of each face's 16 panels are on screen (clip codes of the 5x5
// grid points around each panel) and asks for at least mip level 8 of every
// visible panel's texture.
void DrawableCube::CalculateVisibility()
{
    _D3DVECTOR viewDir;
    _D3DVECTOR unused;                  // constructed, never used (as retail)
    viewDir.x = LpRenderTarget->m_camera->m_view._13;
    viewDir.y = LpRenderTarget->m_camera->m_view._23;
    viewDir.z = LpRenderTarget->m_camera->m_view._33;
    _D3DVECTOR toCube = m_cube->m_position - LpRenderTarget->m_camera->m_position;
    static _D3DMATRIX worldMatrix = IdentityMatrix();
    worldMatrix._33 = worldMatrix._22 = worldMatrix._11 = 1.0f;
    worldMatrix._41 = m_cube->m_position.x;
    worldMatrix._42 = m_cube->m_position.y;
    worldMatrix._43 = m_cube->m_position.z;
    static _D3DMATRIX objectToClipMatrix;
    Vu0MulMatrix(&objectToClipMatrix, &LpRenderTarget->m_camera->m_viewProjection, &worldMatrix);
    int i, j;
    for (i = 0; i < 6; i++) {
        m_cube->m_faces[i].m_visibleMask = 0;
        if (FaceVisible(LpRenderTarget->m_camera, &viewDir, &FaceNormals[i])) {
            unsigned int clip[25];
            _D3DVECTOR *p = &CubePoints[i * 25];
            unsigned int *c = clip;
            for (j = 0; j < 25; j++) {
                VisTest->PointVisible(LpRenderTarget->m_camera, &objectToClipMatrix, p, 0, c);
                p++;
                c++;
            }
            for (j = 0; j < 16; j++) {
                int k = j + j / 4;
                // Accumulated in steps, as retail: a ~0x10 constant in one
                // expression gets hoisted out of the loop.
                unsigned int outside = ~0x10;
                outside &= clip[k];
                outside &= clip[k + 1];
                outside &= clip[k + 6];
                outside &= clip[k + 5];
                if (outside == 0) {
                    m_cube->m_faces[i].m_visibleMask |= (unsigned short)(1 << j);
                    PSXTextureMap *panel = m_cube->m_faces[i].m_panels[j];
                    if (panel && !(8.0f <= panel->m_usage))
                        panel->m_usage = 8.0f;
                }
            }
        }
    }
}

// ---- src/DrawableCube/Tick.cpp
// mwcc session-state shim: retail was
// built in one compiler run, so an earlier 64-bit divide had raised the
// helper-argument high-water mark; this unused function reproduces it.
static void MWCC_SHIM_TEXT MwccSessionState(long *a) { a[0] = a[0] / a[1]; }

extern int TimerOverflowCounterT0;

inline unsigned int timeGetTime()
{
    static long Time = 0;
    static int lastCount = 0;
    int count;
    int elapsed;
    int overflows;
    count = *(volatile int *)0x10000000;    // T0_COUNT
    overflows = TimerOverflowCounterT0;
    TimerOverflowCounterT0 = 0;
    if (lastCount <= count)
        elapsed = count - lastCount;
    else
        elapsed = 0x10000 - lastCount + count;
    if (lastCount >= count && overflows > 0)
        overflows--;
    lastCount = count;
    Time += elapsed + (overflows << 16);
    return 0.0017066667f * Time;
}

inline int GetRandomNumber(int lo, int hi)
{
    // volatile: retail reloads seeder for srand() right after storing it
    static volatile unsigned int seeder = 0;
    if (!seeder) {
        seeder = timeGetTime();
        srand(seeder);
    }
    int r = (int)((hi - lo + 1) * (rand() * 4.656613e-10f));
    r = r > hi ? hi : r;
    r = lo > r ? lo : r;
    return r;
}

#pragma fast_fptosi on
int DrawableCube::Tick(float dt)
{
    PanelSequence *p;
    int i;
    int idle;
    PanelSequence *s;
    if (!m_sequences)
        return GameObject::Tick(dt);
    idle = 1;
    for (i = 0; i < m_numSequences; i++) {
        p = &m_sequences[i];
        if (p->m_playing == 1)
            idle = 0;
        else
            p->m_idleElapsed += dt;
    }
    if (idle) {
        for (i = 0; i < m_numSequences; i++) {
            p = &m_sequences[i];
            if (p->m_idleElapsed > p->m_idleTime) {
                if (p->m_retryTime >= 1.0f) {
                    p->m_retryTime = 0;
                    if (GetRandomNumber(0, 100) < p->m_randomStartChance) {
                        p->m_playing = 1;
                        m_animation = i;
                        break;
                    }
                } else {
                    p->m_retryTime += dt;
                }
            }
        }
    } else {
        s = &m_sequences[m_animation];
        s->m_frame += dt * s->m_framesPerSecond;
        if (s->m_frame > s->m_endFrame - s->m_startFrame) {
            s->m_frame = 0;
            s->m_idleElapsed = 0;
            s->m_playing = 0;
        }
    }
    return GameObject::Tick(dt);
}
#pragma fast_fptosi reset

// ---- src/DrawableCube/Render3D.cpp

// Quadword matrix copy (libvu0 sceVu0CopyMatrix style inline asm).
static inline void CopyMatrix(void *dst, void *src)
{
    asm("lq $8, 0x0(%1); lq $9, 0x10(%1); lq $10, 0x20(%1); lq $11, 0x30(%1); sq $8, 0x0(%0); sq $9, 0x10(%0); sq $10, 0x20(%0); sq $11, 0x30(%0)"
        : : "r"(dst), "r"(src) : "$8", "$9", "$10", "$11", "memory");
}

extern StringTable *LpMicroProgramStringTable;
extern StringTable *LpTextureMapStringTable;
extern PSXTextureMapManager *LpTextureMapManager;
extern unsigned int StripPreambleCode[];   // VU1 microcode

extern int TimeGPU;
extern int NopLoopFlag;
extern int NoKick;

#define LpPSXRenderTarget ((PSXRenderTarget *)LpRenderTarget)

// The timer IDs are leftovers of profiling code compiled out of retail.
int DrawableCube::Render3D()
{
    int face, panel, index;
    TextureMap *texture;
    VifDataItem *item;

    if (!RenderCube)
        return 1;
    if (TimeGPU) {
        LpPSXRenderTarget->TrySendRenderQueue(1);
        LpPSXRenderTarget->WaitForRenderingToComplete();
    }
    static int CPUTimeID = -1;
    static int CubeSubdivCode = LpMicroProgramStringTable->GetIndex("CubeSubdiv");
    static int DiffuseCode = LpMicroProgramStringTable->GetIndex("Diffuse");
    static int UnlitCode = LpMicroProgramStringTable->GetIndex("StripUnlitCode");
    static int NopLoopIndex = LpMicroProgramStringTable->GetIndex("NopLoop");
    static int NoKickIndex = LpMicroProgramStringTable->GetIndex("StripOptFogCodeNoKick");

    LpPSXRenderTarget->AddMPGToRenderQueue(StripPreambleCode);
    if (NopLoopFlag)
        LpPSXRenderTarget->AddMPGToRenderQueue((unsigned int *)LpMicroProgramStringTable->GetPointer(NopLoopIndex));
    else if (NoKick)
        LpPSXRenderTarget->AddMPGToRenderQueue((unsigned int *)LpMicroProgramStringTable->GetPointer(NoKickIndex));
    else if (gEnableVUSubdivision)
        LpPSXRenderTarget->AddMPGToRenderQueue((unsigned int *)LpMicroProgramStringTable->GetPointer(CubeSubdivCode));
    else
        LpPSXRenderTarget->AddMPGToRenderQueue((unsigned int *)LpMicroProgramStringTable->GetPointer(UnlitCode));

    PSXCamera *camera = (PSXCamera *)LpRenderTarget->m_camera;
    QWORD *data = (QWORD *)((char *)m_generalItem + 0x80);
    data->f[0] = m_cube->m_position.x;
    data->f[1] = m_cube->m_position.y;
    data->f[2] = m_cube->m_position.z;
    data->f[3] = 1.0f;
    data++;
    CopyMatrix(data, &camera->m_guardBandProjection);
    data += 4;
    CopyMatrix(data, &camera->m_projection);
    data += 4;
    CopyMatrix(data, &camera->m_x140);
    data += 4;
    CopyMatrix(data, &camera->m_view);
    data = (QWORD *)((char *)m_generalItem + 0x250);
    data->f[0] = camera->m_position.x;
    data->f[1] = camera->m_position.y;
    data->f[2] = camera->m_position.z;
    data->f[3] = 1.0f;
    data++;
    data->f[0] = 2e-5f;
    data->f[1] = -16.0f;
    data->f[2] = LpPSXRenderTarget->m_fogEnd;
    data->f[3] = LpPSXRenderTarget->m_fogScale;
    // Through a local: retail loads LpRenderTarget before the item here.
    PSXRenderTarget *target = LpPSXRenderTarget;
    target->AddToRenderQueue((_sceDmaTag *)m_generalItem, 1, "");

    index = 0;
    for (face = 0; face < 6; face++) {
        for (panel = 0; panel < 16; panel++, index++) {
            if (!(m_cube->m_faces[face].m_visibleMask & (1 << panel)))
                continue;
            if (!(m_cube->m_faces[face].m_panelMask & (1 << panel)))
                continue;
            texture = GetTextureMap(face, panel);
            if (!gUseCubeTexture) {
                int fill;
                // Debug fill textures by size; any other value takes the first.
                switch (DebugFill) {
                default:
                case 0x20: fill = 0; break;
                case 0x40: fill = 1; break;
                case 0x80: fill = 2; break;
                case 0x100: fill = 3; break;
                }
                texture = (TextureMap *)LpTextureMapStringTable->GetPointer(fill);
                texture->m_usage = 10.0f;
            }
            LpTextureMapManager->AddTextureToGifList((PSXTextureMap *)texture);
            LpTextureMapManager->AddTextureInterruptToRenderQueue();
            item = m_uniqueItems[index];
            UpdateUniqueDataItem(item, texture);
            LpPSXRenderTarget->AddToRenderQueue((_sceDmaTag *)item, 1, "");
            LpPSXRenderTarget->AddToRenderQueue(m_renderItems[index], "");
        }
    }
    LpPSXRenderTarget->AddToRenderQueue((_sceDmaTag *)m_restoreItem, 1, "");
    if (!TimeGPU)
        LpPSXRenderTarget->TrySendRenderQueue(0);
    if (TimeGPU) {
        static int GPUTimeID = -1;
        LpPSXRenderTarget->TrySendRenderQueue(1);
        LpPSXRenderTarget->WaitForRenderingToComplete();
    } else {
        LpPSXRenderTarget->TrySendRenderQueue(0);
    }
    return 1;
}
#undef LpPSXRenderTarget

// ---- src/DrawableCube/UpdateUniqueDataItem.cpp
typedef unsigned long u_long;

// libgraph's GS TEX0 register packing.
#define SCE_GS_SET_TEX0(tbp, tbw, psm, tw, th, tcc, tfx, cbp, cpsm, csm, csa, cld) \
    ((u_long)(tbp) | ((u_long)(tbw) << 14) | ((u_long)(psm) << 20) | ((u_long)(tw) << 26) | \
     ((u_long)(th) << 30) | ((u_long)(tcc) << 34) | ((u_long)(tfx) << 35) | ((u_long)(cbp) << 37) | \
     ((u_long)(cpsm) << 51) | ((u_long)(csm) << 55) | ((u_long)(csa) << 56) | ((u_long)(cld) << 61))

// CreateUniqueDataItem's packet from its TEX0 write (item +0x30).
struct CubeVifData {
    u_long m_tex0;                      // +0x00
    char pad08[0x60 - 0x8];
    float m_color[4];                   // +0x60
};

void DrawableCube::UpdateUniqueDataItem(VifDataItem *item, TextureMap *texture)
{
    CubeVifData *data = (CubeVifData *)((char *)item + 0x30);
    PSXTextureMap *psxTexture = (PSXTextureMap *)texture;
    PSXMipSurface *mip = psxTexture->GetReqdMipLevel();
    data->m_tex0 = SCE_GS_SET_TEX0(psxTexture->m_xEC, mip->m_blocks, 2, mip->m_widthLog2, mip->m_heightLog2, 1, 0, 0, 0, 0, 0, 0);
    data->m_color[0] = m_cube->m_position.x;
    data->m_color[1] = m_cube->m_position.y;
    data->m_color[2] = m_cube->m_position.z;
    data->m_color[3] = 1.0f;
}

// ---- src/DrawableCube/GetTextureMap.cpp

static inline int FloatToInt(float f)
{
    int i;
    asm("cvt.w.s $f5, %1; mfc1 %0, $f5" : "=r"(i) : "f"(f) : "$f5");
    return i;
}

// The texture for a face layer, taken from the animation frame's cube when
// the current animation drives that layer.
TextureMap *DrawableCube::GetTextureMap(int face, int layer)
{
    if (!m_sequences)
        return m_cube->m_faces[face].m_panels[layer];
    AnimatedPanel *use = &m_panels[face][layer];
    if (use->m_mapping) {
        PanelSequence *animation = &m_sequences[m_animation];
        for (int i = 0; i < use->m_numSequences; i++) {
            if (animation->m_playing && use->m_sequences[i] == m_animation) {
                int cube = FloatToInt(animation->m_frame + animation->m_startFrame);
#line 1697 "Cubedraw.cpp"
                ASSERT(cube < m_numCubes, "DrawableCube::GetTextureMap LpCubes index error");
                TextureMap *map = m_cubes[cube]->m_faces[face].m_panels[layer];
                if (!map)
                    return m_cube->m_faces[face].m_panels[layer];
                return map;
            }
        }
    }
    return m_cube->m_faces[face].m_panels[layer];
}

// ---- src/DrawableCube/PrepareGeometry.cpp

int DrawableCube::PrepareGeometry()
{
    if (!RenderCube)
        return 1;
    // Profiler IDs: initialized, never read.
    static int DrawableCubePrepareGeometryID = -1;
    static int CubePrprGeom = -1;
    if (gCubeRecreateGeom)
        CreateGeometry();
    CalculateVisibility();
    return 1;
}

// ---- src/DrawableCube/KeyDownHandler.cpp

int DrawableCube::KeyDownHandler(ProcessNode *node, KeyInfo *key)
{
    if (GameObject::KeyDownHandler(node, key))
        return 1;
    if (KEYDOWN_HANDLER_TEST(0x22, (DeviceType)0, node, 0x80)) {
        RenderCube = 1 - RenderCube;
        return 1;
    }
    return 0;
}

// ---- src/sinit/Cubedraw.cpp
// Static initializer: the axis vectors (sinit/AxisVectors.h) and the cube
// drawing tables (defined at the top).
