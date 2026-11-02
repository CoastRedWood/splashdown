// AnimTextureMap.cpp  @ 0x00305980 (text 0x305980-0x305F60)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// AnimTextureMap as one translation unit, a TU without an initializer
// inside Portal.cpp's tu_map row, after JetskiFollowCam.cpp. Its .rodata
// (0x430890-0x430950) restarts the labels at @510 and ends with the
// AnimTextureMap RTTI. It emits a weak copy of BaseTextureMap's inline dtor
// (0x29AC10), which retail's linker dropped.



#include "AnimTextureMap/AnimTextureMap.h"
#include "DataAccess/DataAccess.h"
#include "ParameterBlock/ParameterBlock.h"
#include "d3dtypes.h"

// ---- src/AnimTextureMap/GetData.cpp


void *AnimTextureMap::GetData(int index)
{
    BaseTextureMap *frame = m_frames[index];
    if (m_usage > frame->m_usage)
        frame->m_usage = m_usage;
    return frame;
}


// ---- src/AnimTextureMap/ctor.cpp
extern "C" char *strcpy(char *dst, const char *src);





AnimTextureMap::AnimTextureMap()
{
    m_numFrames = 0;
    m_frames = 0;
    strcpy(m_name, "");
}


// ---- src/AnimTextureMap/dtor.cpp

extern DataAccess *LpDataAccess;




AnimTextureMap::~AnimTextureMap()
{
    ReleaseList();
    LpDataAccess->DetachObject(this);
    LpDataAccess->DropDevice(m_name, 1);
}


// ---- src/AnimTextureMap/ReleaseList.cpp


void AnimTextureMap::ReleaseList()
{
    if (m_frames) {
        for (int i = 0; i < m_numFrames; i++) {
            if (m_frames[i])
                m_frames[i]->Release();
        }
        delete[] m_frames;
        m_frames = 0;
    }
    m_numFrames = 0;
}


// ---- src/AnimTextureMap/Create.cpp
extern "C" char *strcpy(char *dst, const char *src);
extern "C" char *strcat(char *dst, const char *src);
extern "C" int sprintf(char *buf, const char *fmt, ...);
extern "C" int tolower(char c);
void PSXprintf(const char *fmt, ...);

class Palette8;


// An inlined strrchr.
static inline char *FindLast(char *s, char c)
{
    char *start = s;
    while (*s++)
        ;
    do {
        s--;
    } while (s != start && *s != c);
    if (*s == c)
        return s;
    return 0;
}


TextureMap *GetTextureMapFromResourceName(char *name, int a, Palette8 *palette, unsigned int b, _D3DBLEND src,
                                          _D3DBLEND dst, int c, unsigned int d, int e);


AnimTextureMap *AnimTextureMap::Create(char *name, int a, int c, unsigned int d)
{
    char path[0x110];
    char base[0x110];
    char *p;
    char *q;
    char *ext;
    int i;
    int failed;
    failed = 0;
    strcpy(m_name, name);
    strcpy(path, name);
    for (p = path; *p; p++)
        *p = tolower(*p);
    ext = FindLast(path, '.');
    if (ext)
        strcpy(ext, ".atd");
    else
        strcat(path, (const char *)".atd");    // the casts load the literal first, as in retail
    ParameterBlock params;
    if (!params.OpenFile(path, 0)) {
        PSXprintf("'%s' does not exist\n", path);
        goto fail;
    }
    params.ReadParameterBlock("TextureMap");
    params.GetParameter("NumAnims", 0, &m_numFrames);
    m_frames = new BaseTextureMap *[m_numFrames];
    strcpy(base, name);
    for (q = base; *q; q++)
        *q = tolower(*q);
    ext = FindLast(base, '.');
    if (ext)
        strcpy(ext, "");
    else
        strcat(base, (const char *)"");
    for (i = 0; i < m_numFrames; i++) {
        sprintf(path, "%s%02d.tga", base, i + 1);
        m_frames[i] = GetTextureMapFromResourceName(path, a, 0, 2, (_D3DBLEND)5, (_D3DBLEND)7, c, d, 1);
        if (!m_frames[i]) {
            failed = 1;
        } else {
            m_width = m_frames[i]->m_width;
            m_height = m_frames[i]->m_height;
        }
    }
    if (failed)
        ReleaseList();
    return this;
fail:
    Release();
    return 0;
}


// ---- src/AnimTextureMap/GetReportData.cpp
struct TextureReportData {
    int m_x0;                           // +0x0
    int m_x4;                           // +0x4
    int m_x8;                           // +0x8
};



void AnimTextureMap::GetReportData(TextureReportData *data)
{
    for (int i = 0; i < m_numFrames; i++) {
        if (m_frames[i]) {
            TextureReportData frame;
            m_frames[i]->GetReportData(&frame);
            data->m_x0 = frame.m_x0;
            data->m_x4 += frame.m_x4;
            data->m_x8 += frame.m_x8;
        }
    }
}

