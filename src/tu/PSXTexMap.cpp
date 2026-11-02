// PSXTexMap.cpp  @ 0x00276620 (text 0x276620-0x27AF00)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// PSXTextureMap as one translation unit without a static initializer, in
// PSXRenderTarget.cpp's tu_map row: the 8-to-32-bit block converters, PSXTextureMap,
// Clut8/Clut4::Create. Its ASSERTs name "PSXTexMap.cpp"; its .rodata restarts at @720 after
// PSXRenderTarget.cpp's RTTI and ends with the Clut/Clut4/Clut8/BaseObject/BaseTextureMap/
// TextureMap/PSXTextureMap RTTI.





extern "C" void *memcpy(void *dst, void *src, unsigned int n);
#define MEMCPY_DECLARED
#include "soultree_assert_block.h"
#include "PSXTextureMap/PSXTextureMap.h"
#include "AnimTextureMap/AnimTextureMap.h"
#include "DataAccess/DataAccess.h"
#include "TgaFile/TgaFile.h"
#include "TextureMap/TextureMap.h"
#include "PSXTextureMapManager/PSXTextureMapManager.h"
#include "Clut4/Clut4.h"
#include "Clut8/Clut8.h"
#include "mwcc_shim.h"

// Defined here for its retail .bss slot.
unsigned int gTotalFormatConvTime;
int MemoryAllocatedForUnCompressedTextureMaps;
unsigned int MemoryAllocatedForCompressedTextureMaps;
// Retail's .data has it first, before BlockConv8to32's table and the block
// tables (after PSXRenderTarget.cpp's GPUOverlayId).
int gConvertNativeTo32Bit = 1;

// An inline with external linkage: retail's statics are
// allowThisAssertion$localstatic3$BitsPerPixelToPS2PixelFormat__Fi ...,
// shared across TUs; defined here: retail's sit after these globals in .bss.
inline int BitsPerPixelToPS2PixelFormat(int bpp)
{
    switch (bpp) {
    case 4:
        return 0x14;
    case 8:
        return 0x13;
    case 16:
        return 2;
    case 32:
        return 0;
    default:
#line 42 "PSXTexMap.cpp"
        ASSERTF(0, ("BitsPerPixelToPS2PixelFormat: invalid BitsPP (%d)", bpp));
        return -1;
    }
}



// ---- src/global/BlockConv8to32.cpp
// Unswizzles one 64x4 block of 8-bit texels from the GS's PSMT8 layout.
// Odd rows use the second half of the table.
int BlockConv8to32(unsigned char *src, unsigned char *dst)
{
    static unsigned int lut[128] = {   // not const: retail's is in .data
    0x00, 0x24, 0x08, 0x2C, 0x01, 0x25, 0x09, 0x2D,
    0x02, 0x26, 0x0A, 0x2E, 0x03, 0x27, 0x0B, 0x2F,
    0x04, 0x20, 0x0C, 0x28, 0x05, 0x21, 0x0D, 0x29,
    0x06, 0x22, 0x0E, 0x2A, 0x07, 0x23, 0x0F, 0x2B,
    0x10, 0x34, 0x18, 0x3C, 0x11, 0x35, 0x19, 0x3D,
    0x12, 0x36, 0x1A, 0x3E, 0x13, 0x37, 0x1B, 0x3F,
    0x14, 0x30, 0x1C, 0x38, 0x15, 0x31, 0x1D, 0x39,
    0x16, 0x32, 0x1E, 0x3A, 0x17, 0x33, 0x1F, 0x3B,
    0x04, 0x20, 0x0C, 0x28, 0x05, 0x21, 0x0D, 0x29,
    0x06, 0x22, 0x0E, 0x2A, 0x07, 0x23, 0x0F, 0x2B,
    0x00, 0x24, 0x08, 0x2C, 0x01, 0x25, 0x09, 0x2D,
    0x02, 0x26, 0x0A, 0x2E, 0x03, 0x27, 0x0B, 0x2F,
    0x14, 0x30, 0x1C, 0x38, 0x15, 0x31, 0x1D, 0x39,
    0x16, 0x32, 0x1E, 0x3A, 0x17, 0x33, 0x1F, 0x3B,
    0x10, 0x34, 0x18, 0x3C, 0x11, 0x35, 0x19, 0x3D,
    0x12, 0x36, 0x1A, 0x3E, 0x13, 0x37, 0x1B, 0x3F,
    };
    unsigned int i, j, y, k, n;

    n = 0;
    for (y = 0; y < 4; y++) {
        k = (y & 1) * 64;
        for (i = 0; i < 16; i++) {
            for (j = 0; j < 4; j++) {
                dst[n] = src[lut[k]];
                k++;
                n++;
            }
        }
        src += 0x40;
    }
    return 0;
}


// ---- src/global/PageConv8to32.cpp
// PageConv8to32__FiiPUcPUc — PSMT8 page -> PSMCT32 page swizzle  @ 0x002766B0

extern "C" {
void *malloc(unsigned int);
void free(void *);
void *memset(void *, int, unsigned int);
}
int BlockConv8to32(unsigned char *src, unsigned char *dst);

// Converts a 128-byte-wide page of PSMT8 texels to the PSMCT32 layout, one
// 16x16 block at a time (based on Sony's sample swizzle code).
int PageConv8to32(int width, int height, unsigned char *p_input, unsigned char *p_output)
{
    static int block_table8[32] = {
        0, 1, 4, 5, 16, 17, 20, 21, 2, 3, 6, 7, 18, 19, 22, 23,
        8, 9, 12, 13, 24, 25, 28, 29, 10, 11, 14, 15, 26, 27, 30, 31,
    };
    static int block_table32[32] = {
        0, 1, 4, 5, 16, 17, 20, 21, 2, 3, 6, 7, 18, 19, 22, 23,
        8, 9, 12, 13, 24, 25, 28, 29, 10, 11, 14, 15, 26, 27, 30, 31,
    };
    int *index32_h;
    int *index32_v;
    int j;
    int *p;     // named, declared after j: retail colours it before i (a0/a1)
    unsigned char input_block[16 * 16];
    unsigned char output_block[16 * 16];
    unsigned char *pi0, *pi1, *po0, *po1;
    int index0;
    int index1;
    int i;
    int in_block_nb;
    int k;
    int i_width, i_height;
    int in_block_x, in_block_y;

    index32_h = (int *)malloc(8 * 4 * 4);
    index32_v = (int *)malloc(8 * 4 * 4);
    index0 = 0;
    for (i = 0; i < 4; i++) {
        for (j = 0, p = &block_table32[index0]; j < 8; j++) {
            index1 = *p;
            p++;
            index32_h[index1] = j;
            index32_v[index1] = i;
            index0++;
        }
    }

    i_width = width / 16;
    i_height = height / 16;

    memset(input_block, 0, 16 * 16);
    memset(output_block, 0, 16 * 16);

    for (i = 0; i < i_height; i++) {
        // Recomputed every row (not a running += 16): keeps p_input itself from
        // becoming the outer induction pointer, as in retail.
        in_block_y = i * 16;
        for (j = 0; j < i_width; j++) {
            pi0 = input_block;
            pi1 = p_input + in_block_y * 128 + j * 16;
            in_block_nb = i * i_width + j;
            in_block_x = block_table8[in_block_nb];
            for (k = 0; k < 16; k++) {
                memcpy(pi0, pi1, 16);
                pi0 += 16;
                pi1 += 128;
            }

            BlockConv8to32(input_block, output_block);

            po0 = output_block;
            po1 = p_output + index32_v[in_block_x] * 2048 + index32_h[in_block_x] * 32;
            for (k = 0; k < 8; k++) {
                memcpy(po1, po0, 32);
                po0 += 32;
                po1 += 256;
            }
        }
    }

    free(index32_h);
    free(index32_v);
    return 0;
}


// ---- src/global/Conv8to32.cpp
struct _reent {
    int _errno;
    void *_stdin;
    void *_stdout;
    void *_stderr;
};
extern struct _reent *_impure_ptr;
#define stderr (_impure_ptr->_stderr)

extern "C" {
int fprintf(void *stream, const char *format, ...);
void *memset(void *, int, unsigned int);
}
int PageConv8to32(int width, int height, unsigned char *p_input, unsigned char *p_output);

// Converts a PSMT8 image to the PSMCT32 layout one 128x64 page at a time
// (Sony's sample swizzle code).
int Conv8to32(int width, int height, unsigned char *p_input, unsigned char *p_output)
{
    // Declaration order chosen for retail register/spill-slot allocation.
    int k;
    int i;
    int in_line;
    int j;
    int out_height;
    unsigned char *pi1;
    int in_height;
    int n_page_h;
    unsigned char *po1;
    int n_page_w;
    unsigned char *pi0;
    unsigned char *po0;
    int out_line;
    unsigned char page_in[128 * 64];
    unsigned char page_out[128 * 64];

    for (i = 0; i < 11; i++)
        if (width == (1024 >> i))
            break;
    if (i == 11) {
        fprintf(stderr, "Error : width is not 2^n\n");
        return -1;
    }
    for (i = 0; i < 11; i++)
        if (height == (1024 >> i))
            break;
    if (i == 11) {
        // Retail prints the width message here too.
        fprintf(stderr, "Error : width is not 2^n\n");
        return -1;
    }

    memset(page_in, 0, sizeof(page_in));
    memset(page_out, 0, sizeof(page_out));

    n_page_w = (width - 1) / 128 + 1;
    n_page_h = (height - 1) / 64 + 1;

    if (n_page_w == 1) {
        in_line = width;
        out_line = width * 2;
    } else {
        in_line = 128;
        out_line = 256;
    }
    if (n_page_h == 1) {
        in_height = height;
        out_height = height / 2;
    } else {
        in_height = 64;
        out_height = 32;
    }

    for (i = 0; i < n_page_h; i++) {
        for (j = 0; j < n_page_w; j++) {
            pi0 = page_in;
            pi1 = p_input + i * (n_page_w * (in_line * 64)) + j * in_line;
            for (k = 0; k < in_height; k++) {
                memcpy(pi0, pi1, in_line);
                pi1 += in_line * n_page_w;
                pi0 += 128;
            }
            PageConv8to32(128, 64, page_in, page_out);
            po0 = page_out;
            po1 = p_output + i * (n_page_w * (out_line * out_height)) + j * out_line;
            for (k = 0; k < out_height; k++) {
                memcpy(po1, po0, out_line);
                po1 += out_line * n_page_w;
                po0 += 256;
            }
        }
    }
    return 0;
}


// ---- src/global/GetTextureMapFromResourceName.cpp
// FindExtension is inlined inside SetExtension.
#pragma inline_depth(5)

extern "C" char *strcpy(char *dst, const char *src);
extern "C" char *strcat(char *dst, const char *src);



extern DataAccess *LpDataAccess;

class Palette8;

void ReviseIpuImageDataFormat(ImageDataFormat &format);
TgaFile *ImportTgaFile(long file, TgaFile *tga);
void FreeTgaFile(TgaFile *tga);

// An inlined strrchr(name, '.').
static inline char *FindExtension(char *s)
{
    char *start = s;
    while (*s++)
        ;
    do {
        s--;
    } while (s != start && *s != '.');
    if (*s == '.')
        return s;
    return 0;
}

// Replaces name's extension, or appends one.
static inline void SetExtension(char *name, char *extension)
{
    char *ext;
    if ((ext = FindExtension(name)))
        strcpy(ext, extension);
    else
        strcat(name, extension);
}

// Finds a texture by resource name, trying .pst (already loaded or a
// container), .ipu, .tga, .atm and .atd files in turn.
BaseObject *GetTextureMapFromResourceName(char *name, int format, Palette8 *palette, unsigned int flags,
                                          _D3DBLEND srcBlend, _D3DBLEND destBlend, int colorKeyMode,
                                          unsigned int colorKey, int addRef)
{
    char path[0x110];
    long file = -1;
    ImageDataFormat ipuFormat;
    int size;
    int width;
    int height;
    BaseObject *object = 0;
    int x10;
    TgaFile *tga = 0;
    ColorMapper *mapper = 0;
    int anim;
    int container = 0;  // set but never read; retail's branch layout needs it
    int targa;
    int ipu;

    strcpy(path, name);
    targa = 0;
    anim = 0;
    ipu = 0;
    SetExtension(path, ".pst");
    if (LpDataAccess->GetDataOrFileHandle(path, "rb", &file, (void **)&object, 0)) {
        container = 1;
        goto found;
    }
    SetExtension(path, ".ipu");
    if (LpDataAccess->GetDataOrFileHandle(path, "rb", &file, (void **)&object, 0)) {
        ipu = 1;
        goto found;
    }
    SetExtension(path, ".tga");
    if (LpDataAccess->GetDataOrFileHandle(path, "rb", &file, (void **)&object, 0)) {
        targa = 1;
        goto found;
    }
    SetExtension(path, ".atm");
    if (LpDataAccess->FileExists(path)) {
        anim = 1;
        goto found;
    }
    SetExtension(path, ".atd");
    if (LpDataAccess->GetDataOrFileHandle(path, "rb", &file, (void **)&object, 0)) {
        anim = 2;
        goto found;
    }
#line 355 "PSXTexMap.cpp"
    ASSERTF(0, ("ERROR : Could not open TextureMap '%s' in any format!\n", path));
    return 0;

found:
    if (object) {
        if (addRef)
            object->AddRef();
        return object;
    }
    if (anim == 1) {
        LpDataAccess->LoadResourceFile(path, 0, 0, 0, 0, 0, 0, 0);
        AnimTextureMap *map = new AnimTextureMap;
        map->Create(path, format, colorKeyMode, colorKey);
        return map;
    }
    if (anim == 2) {
        LpDataAccess->FClose(file);
        AnimTextureMap *map = new AnimTextureMap;
        map->Create(path, format, colorKeyMode, colorKey);
        LpDataAccess->AttachObject(file, map);
        return map;
    }
    if (ipu == 1) {
        if (LpDataAccess->FRead(file, &ipuFormat, 4, 1) != 1)
            return 0;
        format = 0x613;
        x10 = 0;
        LpDataAccess->FRead(file, &size, 4, 1);
        LpDataAccess->FRead(file, &width, 4, 1);
        LpDataAccess->FRead(file, &height, 4, 1);
        LpDataAccess->FRead(file, &x10, 4, 1);
        // The fourth header word is a version (0x1000 = current); IPU
        // images are square.
        if (height != 0x1000)
            ReviseIpuImageDataFormat(ipuFormat);
        height = width;
    } else if (targa == 1) {
        if (!(tga = ImportTgaFile(file, 0))) {
#line 414 "PSXTexMap.cpp"
            ASSERTF(0, ("Unable to open texture '%s'.  Invalid format.\n", name));
            return 0;
        }
        width = tga->width;
        height = tga->height;
        // (16 and 24 bit both map to 0x613, as in retail.)
        format = tga->bitsPerPixel == 16 ? 0x613 : tga->bitsPerPixel == 24 ? 0x613 : 0x22B8;
    }
    PSXTextureMap *map = new PSXTextureMap(1);
    strncpy(map->m_name, path, 0x7F);
    map->m_name[0x7F] = 0;
    int levels = (flags & 2) ? 16 : (width > height ? height : width);
    if (targa == 1) {
        if (map->Load((unsigned char *)tga->pixels, width, height, width, levels,
                      tga->bitsPerPixel == 16 ? 0x22B : tga->bitsPerPixel == 24 ? 0x378 : 0x22B8, format, mapper, flags,
                      0, 1, 0, srcBlend, destBlend, colorKeyMode, colorKey))
            LpDataAccess->AttachObject(file, map);
        FreeTgaFile(tga);
    } else if (ipu == 1) {
        if (map->Load(file, width, height, levels, ipuFormat, size, format, mapper, flags, 0, srcBlend, destBlend,
                      colorKeyMode, colorKey))
            LpDataAccess->AttachObject(file, map);
    } else {
        if (map->LoadFromContainer(file, flags))
            LpDataAccess->AttachObject(file, map);
    }
    LpDataAccess->FClose(file);
    return map;
}


// ---- src/global/PC2PSXTextureFormat.cpp
// Converts PC pixel data to the PS2 layout in place: 16-bit 555/565 pixels
// get their red and blue fields swapped, 24-bit pixels their R and B bytes.
void PC2PSXTextureFormat(unsigned char *data, int format, int width, int height)
{
    int i;
    if (format == 0x22B || format == 0x613) {
        if (data) {
            unsigned short *p = (unsigned short *)data;
            for (i = 0; i < width * height; i++) {
                *p = ((*p & 0x7C00) >> 10) | (*p & 0x3E0) | ((*p & 0x1F) << 10);
                p++;
            }
        }
    } else if (format == 0x378) {
        if (data) {
            for (i = 0; i < width * height; i++) {
                unsigned char r = data[0];
                unsigned char g = data[1];
                unsigned char b = data[2];
                data[0] = b;
                data[1] = g;
                data[2] = r;
                data += 3;
            }
        }
    }
}


// ---- src/global/PCToPSXColorKey.cpp
// Swaps red and blue for PSX texture modes 0x22B and 0x613.
void PCToPSXColorKey(unsigned int *key, int mode)
{
    if (mode == 0x22B || mode == 0x613) {
        unsigned int v = *key;
        *key = ((v << 16) & 0xFF0000) | (v & 0xFF00) | ((v >> 16) & 0xFF);
    }
}


// ---- src/PSXTextureMap/ctor.cpp


PSXTextureMap::PSXTextureMap(int id) : TextureMap(id)
{
    m_bitsPerPixel = -1;
    for (int i = 0; i < 5; i++) {
        m_mips[i].m_surface = 0;
        m_mips[i].m_width = -1;
        m_mips[i].m_height = -1;
        m_mips[i].m_x24 = 0xFFFF;
        m_mips[i].m_blocks = -1;
        m_mips[i].m_widthLog2 = -1;
        m_mips[i].m_heightLog2 = -1;
        m_mips[i].m_compressedSize = -1;
        m_mips[i].m_data = 0;
        m_mips[i].m_slot = -1;
        m_mips[i].m_tiled = 0;
        m_mips[i].m_borrowed = 0;
        m_mips[i].m_x2C = -1;
        m_mips[i].m_x28 = -1;
    }
    m_clut4 = 0;
    m_clut8 = 0;
    m_reqdMipLevel = -1;
    m_xEC = -1;
    m_xF0 = -1;
    m_compressed = 0;
    m_noMipChain = 0;
    m_x4 = 0;
    m_prepared = 0;
}


// ---- src/PSXTextureMap/dtor.cpp
extern "C" void free(void *p);

extern PSXTextureMapManager *LpTextureMapManager;


// Clut4 and Clut8 have no shared header yet; only their virtual dtor is used.

PSXTextureMap::~PSXTextureMap()
{
    for (int i = 0; i < 5; i++) {
        if (m_mips[i].m_surface && !m_noMipChain)
            free(m_mips[i].m_surface);
    }
    if (m_clut4)
        delete m_clut4;
    if (m_clut8)
        delete m_clut8;
    if (m_compressed)
        LpTextureMapManager->InvalidateIpuCache();
}


// ---- src/Clut8/dtor.cpp



Clut8::~Clut8()
{
}


// ---- src/Clut4/dtor.cpp



Clut4::~Clut4()
{
}


// ---- src/PSXTextureMap/MapUsageToLevel.cpp


int PSXTextureMap::MapUsageToLevel(int usage)
{
    int level = 0;
    int unused;
    // The out-of-range arm is dead code, but retail keeps its branch shape
    // (`bnez -> switch; b skip`), which an empty `if (usage < 10)` loses.
    if (usage > 9) {
        unused = 1;
    } else {
        switch (usage) {
        case 0:
            level = 4;
            break;
        case 1:
        case 2:
            level = 4;
            break;
        case 3:
            level = 4;
            break;
        case 4:
            level = 4;
            break;
        case 5:
            level = 3;
            break;
        case 6:
            level = 2;
            break;
        case 7:
            level = 1;
            break;
        case 8:
            level = 0;
            break;
        case 9:
            level = 0;
            break;
        default:
#line 691 "PSXTexMap.cpp"
            ASSERT(0, "MapUsageToLevel: Invalid usage");
            break;
        }
    }
    if (level >= m_numMips) {
        level = m_numMips - 1;
        while (!m_mips[level].m_surface) {
            level--;
            if (level < 0) {
#line 704 "PSXTexMap.cpp"
                ASSERT(0, "MapUsageToLevel: there are no defined levels");
            }
        }
    }
    return level;
}


// ---- src/PSXTextureMap/CalculateReqdMipLevel.cpp

// Retail converts through $f5 here, not with the compiler's own cvt.w.s:
// an inline-asm helper like its fabs/sqrt ones (33 retail sites use $f5).
static inline int FloatToInt(float f)
{
    int i;
    asm("cvt.w.s $f5, %1; mfc1 %0, $f5" : "=r"(i) : "f"(f) : "$f5");
    return i;
}

void PSXTextureMap::CalculateReqdMipLevel(float distance)
{
    m_reqdMipLevel = 0;
    float usage = m_usage;
    if (usage >= 6.0f)
        usage += distance;
    m_reqdMipLevel = MapUsageToLevel(FloatToInt(usage + 0.5f));
}


// ---- src/PSXTextureMap/GetTopMipLevel.cpp
#pragma alignlabel off



PSXMipSurface *PSXTextureMap::GetTopMipLevel()
{
    PSXMipSurface *mipSurface = &m_mips[0];
#line 727 "PSXTexMap.cpp"
    ASSERT(mipSurface->m_width != -1 && mipSurface->m_height != -1, "GetTopMipLevel: mipSurface->TextureWidth or TextureHeight == -1");
    return mipSurface;
}
#pragma alignlabel reset


// ---- src/PSXTextureMap/GetReqdMipLevel.cpp
#pragma alignlabel off



PSXMipSurface *PSXTextureMap::GetReqdMipLevel()
{
    if (m_reqdMipLevel == -1)
        return 0;
    PSXMipSurface *mipSurface = &m_mips[m_reqdMipLevel];
#line 736 "PSXTexMap.cpp"
    ASSERT(mipSurface->m_width != -1 && mipSurface->m_height != -1, "GetReqdMipLevel: mipSurface->TextureWidth or TextureHeight == -1");
    return mipSurface;
}
#pragma alignlabel reset


// ---- src/PSXTextureMap/Restore.cpp

void PSXTextureMap::Restore()
{
    
}


// ---- src/PSXTextureMap/LoadFromContainer.cpp


extern DataAccess *LpDataAccess;

int PixelFormatFromTexFormat(TexFormat format);
int BitsPerPixel(TexFormat format);



struct PSXTextureFileSurfaceHeader {
    int m_height;                   // +0x00
    int m_width;                    // +0x04
    int m_blocks;                      // +0x08
    int m_widthLog2;                      // +0x0C
};

int PSXTextureMap::LoadFromContainer(long file, unsigned int flags)
{
    int version;
    TexFormat format;
    int numLevels;
    LpDataAccess->FRead(file, &version, 4, 1);
#line 764 "PSXTexMap.cpp"
    ASSERT(version == 1, "Incorrect TextureFileVersion");
    if (version != 1)
        return 0;
    LpDataAccess->FRead(file, &format, 4, 1);
    m_format = PixelFormatFromTexFormat(format);
    m_bitsPerPixel = BitsPerPixel(format);
    LpDataAccess->FRead(file, &numLevels, 4, 1);
    switch (format) {
    case TEXFORMAT_4BIT:
        m_clut4 = new Clut4;
        m_clut4->Read(file);
        m_transfer.Create(0x12);
        break;
    case TEXFORMAT_8BIT:
        m_clut8 = new Clut8;
        m_clut8->Read(file);
        m_transfer.Create(0x12);
        break;
    case TEXFORMAT_COMPRESSED:
        m_compressed = 1;
        m_transfer.Create(10);
        break;
    default:
#line 801 "PSXTexMap.cpp"
        ASSERT(0, "SkipSurfaceInFile bad format");
        break;
    }
    // Levels are stored smallest first; only the full chain (flags & 2)
    // keeps more than the top level.
    for (int level = numLevels - 1; level >= 0; level--) {
        PSXTextureFileSurfaceHeader header;
        if ((flags & 2) || level == 0) {
            PSXMipSurface *surface = &m_mips[level];
            LpDataAccess->FRead(file, &header, sizeof(header), 1);
            if (!ReadSurfaceFromFile(file, format, surface, &header))
                goto done;  // (sic) retail still reports success
        } else {
            LpDataAccess->FRead(file, &header, sizeof(header), 1);
            SkipSurfaceInFile(file, format, header.m_height, header.m_width);
        }
        if (level == 0) {
            m_width = header.m_width;
            m_height = header.m_height;
        }
    }
    m_numMips = (flags & 2) ? numLevels : 1;
    return 1;
done:
    return 1;
}


// ---- src/PSXTextureMap/SkipSurfaceInFile.cpp
// ASSERT contains inline asm; match retail's label alignment for asm functions.
#pragma alignlabel off
#pragma fast_fptosi on

// Format names inferred from the sizes skipped.


extern DataAccess *LpDataAccess;


void PSXTextureMap::SkipSurfaceInFile(long file, TexFormat format, int width, int height)
{
    switch (format) {
    case TEXFORMAT_4BIT:
        LpDataAccess->FSeek(file, (int)(0.5f * (height * width)), 1);
        break;
    case TEXFORMAT_8BIT:
        LpDataAccess->FSeek(file, height * width, 1);
        break;
    case TEXFORMAT_COMPRESSED: {
        int size;
        LpDataAccess->FRead(file, &size, 4, 1);
        LpDataAccess->FSeek(file, size, 1);
        break;
    }
    default:
#line 867 "PSXTexMap.cpp"
        ASSERT(0, "SkipSurfaceInFile bad format");
        break;
    }
}
#pragma alignlabel reset
#pragma fast_fptosi reset


// ---- src/PSXTextureMap/ReadSurfaceFromFile.cpp
// Compiler session state: retail compiled
// this file in an mwcc session that had already generated a two-argument runtime
// helper call (64-bit / or %, double arithmetic or compare). mwcc then models
// every later helper call (__floatdisf, fptoui, fptodp, ...) as reading $a0-$a1,
// so temporaries live before such a call skip $a0/$a1. This unused function
// recreates that state; it is not part of the retail file.
static void MWCC_SHIM_TEXT MwccSessionState(long *a) { a[0] = a[0] / a[1]; }


extern DataAccess *LpDataAccess;


extern "C" void *malloc(unsigned int size);
extern "C" void free(void *p);

extern int TimerOverflowCounterT0;
extern int gConvertNativeTo32Bit;
extern unsigned int gTotalFormatConvTime;

int Conv8to32(int width, int height, unsigned char *p_input, unsigned char *p_output);


// Win32-style timeGetTime: EE timer 0 (T0_COUNT at 0x10000000) ticks plus the
// overflows counted by TimerHandlerT0, accumulated and scaled to milliseconds.
inline unsigned int timeGetTime()
{
    static long Time = 0;
    static int lastCount = 0;
    int count;
    int elapsed;
    int overflows;
    count = *(volatile int *)0x10000000;
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

static inline bool IsConvertibleWidth(int width)
{
    return width == 0x400 || width == 0x200 || width == 0x100 || width == 0x80 || width == 0x40 || width == 0x20
        || width == 0x10 || width == 8;
}

int PSXTextureMap::ReadSurfaceFromFile(long file, TexFormat format, PSXMipSurface *surface, PSXTextureFileSurfaceHeader *header)
{
    int size;
    switch (format) {
    case TEXFORMAT_4BIT:
        if (!AllocateUncompressedSurface(format, surface, header))
            return 0;
        LpDataAccess->FRead(file, *(unsigned char **)&surface->m_surface, m_bitsPerPixel * (header->m_width * header->m_height) >> 3);
        break;
    case TEXFORMAT_8BIT:
        if (!AllocateUncompressedSurface(format, surface, header))
            return 0;
        LpDataAccess->FRead(file, *(unsigned char **)&surface->m_surface, m_bitsPerPixel * (header->m_width * header->m_height) >> 3);
        if (gConvertNativeTo32Bit && IsConvertibleWidth(header->m_width) != 0) {
            unsigned int start = timeGetTime();
            unsigned char *converted = (unsigned char *)malloc(header->m_width * header->m_height);
            Conv8to32(header->m_width, header->m_height, *(unsigned char **)&surface->m_surface, converted);
            free(surface->m_surface);
            surface->m_surface = converted;
            surface->m_half = 1;
            gTotalFormatConvTime += timeGetTime() - start;
        } else {
            surface->m_half = 0;
        }
        break;
    case TEXFORMAT_COMPRESSED:
        LpDataAccess->FRead(file, &size, 4, 1);
        if (!AllocateCompressedSurface(surface, size, header))
            return 0;
        LpDataAccess->FRead(file, *(unsigned char **)&surface->m_data, 1, surface->m_compressedSize);
        surface->m_compressedSize -= 0x10;
        surface->m_surface = surface->m_data;
        surface->m_data = (char *)surface->m_data + 0x10;
        break;
    default:
#line 922 "PSXTexMap.cpp"
        ASSERT(0, "ReadSurfaceFromFile bad format");
        break;
    }
#line 924 "PSXTexMap.cpp"
    ASSERT(surface->m_width >= 16, "");
    return 1;
}


// ---- src/PSXTextureMap/Load.cpp

extern DataAccess *LpDataAccess;


extern "C" void *memset(void *dst, int c, unsigned int n);
extern "C" void *memalign(unsigned int align, unsigned int size);
extern "C" double log(double x);

extern unsigned int MemoryAllocatedForCompressedTextureMaps;

int BitsPerPixel(int format);
int BitsPerPixel(ImageDataFormat format);
int PixelFormatFromImageDataFormat(ImageDataFormat format);
int IsIpuCompressed(ImageDataFormat format);
int IsMipMapped(ImageDataFormat format);
int IsCompressedFormat(ImageDataFormat format);
void PC2PSXTextureFormat(unsigned char *data, int format, int width, int height);
void PCToPSXColorKey(unsigned int *colorKey, int format);
void PixTransColorConvert(void *dst, void *src, int width, int height, int srcPitch, int dstPitch, int dstFormat,
                          int srcFormat, int, ColorMapper *mapper, int, unsigned int colorKey);

static inline int Max(int a, int b) { return a > b ? a : b; }
static inline int Min(int a, int b) { return a > b ? b : a; }

// Loads a texture (or an IPU-compressed mip chain) from an open file. The
// uncompressed case reads into the manager's temp buffer and hands it to the
// other Load overload.
int PSXTextureMap::Load(long file, int width, int height, int levels, ImageDataFormat format, int size,
                        int pixelFormat, ColorMapper *mapper, unsigned int flags, int x, _D3DBLEND srcBlend,
                        _D3DBLEND destBlend, int colorKeyMode, unsigned int colorKey)
{
    // Declaration order chosen for retail's register allocation.
    int mipWidth;
    unsigned char *fileBuffer;
    int mipHeight;
    int offsets[12];
    unsigned char *imageBuffer;
    int srcPixelFormat;
    int i;
    void *dst;
    int pos;
    int k;
    PSXMipSurface *mip;
#line 951 "PSXTexMap.cpp"
    ASSERT(format != 1, "Uncompressed888 unsupported at this time");
    fileBuffer = 0;
    imageBuffer = 0;
    srcPixelFormat = PixelFormatFromImageDataFormat(format);

    m_compressed = IsIpuCompressed(format);
    if (m_compressed) {
        pixelFormat = 0x613;
        if (format == 0x20)
            format = (ImageDataFormat)0x22;
        else
            format = (ImageDataFormat)0x21;
    } else {
        m_x4 = 1;
    }
    memset(offsets, 0, sizeof(offsets));
    m_bitsPerPixel = BitsPerPixel(pixelFormat);
    if (!m_compressed) {
        fileBuffer = (unsigned char *)LpTextureMapManager->AllocTempFileBuffer(
            width * height * Max(BitsPerPixel(srcPixelFormat), BitsPerPixel(pixelFormat)) >> 3);
        if (!fileBuffer)
            return 0;
        imageBuffer = (unsigned char *)LpTextureMapManager->AllocTempImageBuffer(
            (width * height * Max(BitsPerPixel(srcPixelFormat), BitsPerPixel(pixelFormat)) >> 3) + 4);
        if (!imageBuffer)
            return 0;
    }
    if (IsMipMapped(format)) {
        if (!m_compressed) {
            if (!Load(0, width, height, width, levels, srcPixelFormat, pixelFormat, mapper, flags, x, 1, 0,
                      srcBlend, destBlend, colorKeyMode, colorKey))
                return 0;
        } else {
            m_width = width;
            m_height = height;
            m_minSize = 16;
        }
        m_numMips = 0;
        mipWidth = 1;
        i = 0;
        if (m_compressed)
            mipWidth = 16;
        LpDataAccess->FTell(file);
        do {
            pos = LpDataAccess->FTell(file);
            if (LpDataAccess->FRead(file, &offsets[i], 4, 1) != 1)
                return 0;
            if (m_compressed)
                offsets[i] = 0;
            else
                offsets[i] += pos;
            m_numMips++;
            mipWidth <<= 1;
            i++;
        } while (mipWidth <= width);
        pos = LpDataAccess->FTell(file);
        if (LpDataAccess->FRead(file, &offsets[i], 4, 1) != 1)
            return 0;
        if (m_compressed)
            offsets[i] = 0;
        else
            offsets[i] += pos;
        if (flags & 2)
            m_numMips = m_numMips < 5 ? m_numMips : 5;
        else
            m_numMips = 1;
        mipHeight = mipWidth = 1;
        int last = i - 1;
        i = 0;
        if (m_compressed)
            mipWidth = mipHeight = 16;
        do {
            if (m_compressed)
                size = offsets[last - i + 1] - offsets[last - i];
            else
                size = offsets[i + 1] - offsets[i];
            if (m_compressed) {
                LpDataAccess->FRead(file, &size, 4, 1);
                m_mips[last - i].m_compressedSize = size;
                m_mips[last - i].m_data = memalign(0x40, size);
                MemoryAllocatedForCompressedTextureMaps += size;
                if (LpDataAccess->FRead(file, m_mips[last - i].m_data, 1, m_mips[last - i].m_compressedSize) !=
                    m_mips[last - i].m_compressedSize)
                    return 0;
                mip = &m_mips[last - i];
                mip->m_width = mipWidth;
                mip->m_height = mipHeight;
                mip->m_x24 = mip->m_width;
                mip->m_blocks = Max(mip->m_width / 64, 1);
                mip->m_widthLog2 = log(mip->m_width) / log(2.0);
                mip->m_heightLog2 = log(mip->m_height) / log(2.0);
                mip->m_x2C = mip->m_width;
                mip->m_x28 = mip->m_height;
                mip->m_compressedSize -= 16;
                mip->m_surface = mip->m_data;
                mip->m_data = (char *)mip->m_data + 16;
            } else {
                if (LpDataAccess->FRead(file, imageBuffer, mipWidth * BitsPerPixel(format) >> 3, mipHeight) !=
                    mipHeight)
                    return 0;
            }
            if (!m_compressed) {
                dst = m_mips[0].m_surface;
                if (mipWidth >= levels) {
                    for (k = 0; k < Min(Max(m_numMips - 2, 1), 5); k++) {
                        if (m_mips[k].m_width == mipWidth)
                            dst = m_mips[k].m_surface;
                    }
                }
                PixTransColorConvert(dst, imageBuffer, mipWidth, mipHeight, mipWidth, mipWidth, m_format,
                                     srcPixelFormat, 0, mapper, colorKeyMode, colorKey);
                PC2PSXTextureFormat((unsigned char *)dst, srcPixelFormat, mipWidth, mipHeight);
                PCToPSXColorKey(&colorKey, m_format);
            }
            mipWidth <<= 1;
            i++;
            mipHeight <<= 1;
        } while (mipWidth <= width);
        // Retail compares width with itself here (always false), so this loop
        // never runs; `(int)width` keeps mwcc from folding it away.
        if (!m_compressed && (int)width < width) {
            do {
                size = offsets[i + 1] - offsets[i];
                if (IsCompressedFormat(format) && size != mipWidth * mipHeight * BitsPerPixel(format) >> 3) {
                    if (!size) {
                        if (LpDataAccess->FRead(file, &size, 4, 1) != 1)
                            return 0;
                    }
                    if (LpDataAccess->FRead(file, fileBuffer, 1, size) != size)
                        return 0;
                } else {
                    if (LpDataAccess->FRead(file, imageBuffer, mipWidth * BitsPerPixel(format) >> 3, mipHeight) !=
                        mipHeight)
                        return 0;
                }
                mipWidth <<= 1;
                i++;
                mipHeight <<= 1;
            } while (mipWidth <= width);
        }
        m_numMips = m_numMips < 5 ? m_numMips : 5;
        if (m_compressed) {
            m_format = PixelFormatFromImageDataFormat(format);
            LpDataAccess->FTell(file);
        }
        m_transfer.Create(10);
        return 1;
    }
    if (m_compressed) {
        m_width = width;
        m_height = height;
        m_minSize = m_width;
        m_numMips = 0;
        i = 0;
        mipWidth = width;
        LpDataAccess->FTell(file);
        do {
            LpDataAccess->FTell(file);
            if (LpDataAccess->FRead(file, &offsets[i], 4, 1) != 1)
                return 0;
            offsets[i] = 0;
            m_numMips++;
            mipWidth <<= 1;
            i++;
        } while (mipWidth <= width);
        LpDataAccess->FTell(file);
        if (LpDataAccess->FRead(file, &offsets[i], 4, 1) != 1)
            return 0;
        offsets[i] = 0;
        m_numMips = 1;
        mipWidth = m_width;
        mipHeight = m_height;
        int last = i - 1;
        i = 0;
        do {
            size = offsets[last - i + 1] - offsets[last - i];
            LpDataAccess->FRead(file, &size, 4, 1);
            m_mips[last - i].m_compressedSize = size;
            m_mips[last - i].m_data = memalign(0x40, size);
            MemoryAllocatedForCompressedTextureMaps += size;
            if (LpDataAccess->FRead(file, m_mips[last - i].m_data, 1, m_mips[last - i].m_compressedSize) !=
                m_mips[last - i].m_compressedSize)
                return 0;
            mip = &m_mips[last - i];
            mip->m_width = mipWidth;
            mip->m_height = mipHeight;
            mip->m_x24 = mip->m_width;
            mip->m_blocks = Max(mip->m_width / 64, 1);
            mip->m_widthLog2 = log(mip->m_width) / log(2.0);
            mip->m_heightLog2 = log(mip->m_height) / log(2.0);
            mip->m_x2C = mip->m_width;
            mip->m_x28 = mip->m_height;
            mip->m_compressedSize -= 16;
            mip->m_surface = mip->m_data;
            mip->m_data = (char *)mip->m_data + 16;
            mipWidth <<= 1;
            i++;
            mipHeight <<= 1;
        } while (mipWidth <= width);
        m_format = PixelFormatFromImageDataFormat(format);
        LpDataAccess->FTell(file);
        m_transfer.Create(10);
        return 1;
    }
    size = width * height * BitsPerPixel(format) >> 3;
    if (LpDataAccess->FRead(file, imageBuffer, 1, size) != size)
        return 0;
    return Load(imageBuffer, width, height, width, levels, srcPixelFormat, pixelFormat, mapper, flags, x, 1, 0, srcBlend,
                destBlend, colorKeyMode, colorKey);
}


// ---- src/PSXTextureMap/AllocateAllSurfaces.cpp

extern "C" void *memalign(unsigned int align, unsigned int size);
extern "C" double log(double x);

extern int MemoryAllocatedForUnCompressedTextureMaps;

#define MAX(a, b) ((a) > (b) ? (a) : (b))

class PSXRenderTarget {
public:
    char pad[0x40];   // data before the vptr
    virtual ~PSXRenderTarget();
    virtual void BeginScene();
    virtual void EndScene();
    virtual void Blt();
    virtual void Lock();
    virtual void Unlock();
    virtual void TrySendRenderQueue(int);
    virtual void WaitForRenderingToComplete();
    virtual void GetTextureStageState();
    virtual void SetTextureStageState();
    virtual void SetRenderState();
    virtual void GetRenderState();
    virtual void SetTextureState();
    virtual void DisableTextureStage();
    virtual void SetLight();
    virtual void LightEnable();
    virtual void Clear();
    virtual int PixelFormatAvailable(int format);
};
extern PSXRenderTarget *LpRenderTarget;


// Allocates the top surface and every mip level below it (each half the size,
// down to 8x8). The top level's block count goes through a local; the mip levels
// spell MAX(width / 64, n) out, so retail recomputes the quotient.
int PSXTextureMap::AllocateAllSurfaces()
{
    int i;
    int size;
    int width;
    int height;
    int blocks;

    if (LpRenderTarget->PixelFormatAvailable(m_format)) {
        size = m_bitsPerPixel * (m_height * m_width) >> 3;
        m_mips[0].m_surface = memalign(0x40, size);
        MemoryAllocatedForUnCompressedTextureMaps += size;
        m_mips[0].m_width = m_width;
        m_mips[0].m_height = m_height;
        m_mips[0].m_x24 = m_width;
        m_mips[0].m_widthLog2 = log(m_width) / log(2.0);
        m_mips[0].m_heightLog2 = log(m_height) / log(2.0);
        if (m_bitsPerPixel == 8 || m_bitsPerPixel == 4) {
            blocks = m_width / 64;
            m_mips[0].m_blocks = MAX(blocks, 2);
            m_mips[0].m_blocks += m_mips[0].m_blocks % 2;
        } else {
            blocks = m_width / 64;
            m_mips[0].m_blocks = MAX(blocks, 1);
        }
        m_mips[0].m_x2C = m_width;
        m_mips[0].m_x28 = m_height;
        if (!m_mips[0].m_surface) {
#line 1422 "PSXTexMap.cpp"
            ASSERT(0, "CreateSurfaces: LpSurfaceMipMapChain[0].LpSurface == NULL");
            goto fail;
        }
        if (m_numMips > 1) {
            width = m_width;
            height = m_height;
            for (i = 1; i < m_numMips; i++) {
                width = (width >> 1 > 8) ? width >> 1 : 8;
                height = (height >> 1 > 8) ? height >> 1 : 8;
                size = m_bitsPerPixel * (width * height) >> 3;
                m_mips[i].m_surface = memalign(0x40, size);
                MemoryAllocatedForUnCompressedTextureMaps += size;
                if (!m_mips[i].m_surface) {
#line 1446 "PSXTexMap.cpp"
                    ASSERT(0, "CreateSurfaces: LpSurfaceMipMapChain[i].LpSurface == NULL");
                    goto fail;
                }
                m_mips[i].m_width = width;
                m_mips[i].m_height = height;
                m_mips[i].m_x24 = m_mips[i].m_width;
                if (m_bitsPerPixel == 8 || m_bitsPerPixel == 4) {
                    m_mips[i].m_blocks = MAX(width / 64, 2);
                    m_mips[i].m_blocks += m_mips[i].m_blocks % 2;
                } else {
                    m_mips[i].m_blocks = MAX(width / 64, 1);
                }
                m_mips[i].m_widthLog2 = log(m_mips[i].m_width) / log(2.0);
                m_mips[i].m_heightLog2 = log(m_mips[i].m_height) / log(2.0);
                m_mips[i].m_x2C = m_mips[i].m_width;
                m_mips[i].m_x28 = m_mips[i].m_height;
            }
        }
        return 1;
    }
fail:
    return 0;
}


// ---- src/PSXTextureMap/AllocateUncompressedSurface.cpp

extern "C" void *memalign(unsigned int align, unsigned int size);
extern "C" double log(double x);

extern int MemoryAllocatedForUnCompressedTextureMaps;

#define MAX(a, b) ((a) > (b) ? (a) : (b))

int BitsPerPixel(TexFormat format);


int PSXTextureMap::AllocateUncompressedSurface(TexFormat format, PSXMipSurface *surface, PSXTextureFileSurfaceHeader *header)
{
    int bits = BitsPerPixel(format);
    int height = header->m_height;
    int width = header->m_width;
    int size = bits * (height * width) >> 3;
    surface->m_surface = memalign(0x40, size);
    MemoryAllocatedForUnCompressedTextureMaps += size;
    surface->m_width = width;
    surface->m_height = height;
    surface->m_x24 = width;
    surface->m_widthLog2 = log(width) / log(2.0);
    surface->m_heightLog2 = log(height) / log(2.0);
    if (m_bitsPerPixel == 8 || m_bitsPerPixel == 4) {
        surface->m_blocks = MAX(width / 64, 2);
        surface->m_blocks += surface->m_blocks % 2;
    } else {
        surface->m_blocks = MAX(width / 64, 1);
    }
    surface->m_x2C = width;
    surface->m_x28 = height;
    if (!surface->m_surface) {
#line 1520 "PSXTexMap.cpp"
        ASSERT(0, "AllocateSurface: mipSurface->LpSurface == NULL");
    } else {
        return 1;
    }
    return 0;
}


// ---- src/PSXTextureMap/AllocateCompressedSurface.cpp
extern "C" void *memalign(unsigned int align, unsigned int size);
extern "C" double log(double x);

// unsigned: an int global reverses the size/header s-register colouring.
extern unsigned int MemoryAllocatedForCompressedTextureMaps;

#define MAX(a, b) ((a) > (b) ? (a) : (b))



int PSXTextureMap::AllocateCompressedSurface(PSXMipSurface *surface, int size, PSXTextureFileSurfaceHeader *header)
{
    surface->m_compressedSize = size;
    surface->m_data = memalign(0x40, size);
    if (surface->m_data) {
        MemoryAllocatedForCompressedTextureMaps += size;
        surface->m_width = header->m_width;
        surface->m_height = header->m_height;
        surface->m_x24 = surface->m_width;
        surface->m_blocks = MAX(surface->m_width / 64, 1);
        surface->m_widthLog2 = log(surface->m_width) / log(2.0);
        surface->m_heightLog2 = log(surface->m_height) / log(2.0);
        surface->m_x2C = surface->m_width;
        surface->m_x28 = surface->m_height;
        return 1;
    }
    return 0;
}


// ---- src/PSXTextureMap/PrepareTransferTexture.cpp

struct sceIpuRGB16;
struct sceIpuRGB32;



extern int gConvertNativeTo32Bit;


static inline bool IsTextureSize(int size)
{
    bool ok = size == 1024 || size == 512 || size == 256 || size == 128 || size == 64 || size == 32 || size == 16 || size == 8;
    return ok;
}

GsTextureTransfer *PSXTextureMap::PrepareTransferTexture(GsTextureTransfer *transfer, int x, int y, PSXMipSurface *mip, unsigned char *data)
{
    if (mip->m_borrowed) {
        if (!mip->m_tiled) {
            int psm = BitsPerPixelToPS2PixelFormat(m_bitsPerPixel);
            transfer->PrepareTransferTexture(data, x, mip->m_x2C, mip->m_x28, 0, 0, psm, m_bitsPerPixel, mip->m_x24);
        } else if (m_bitsPerPixel == 32) {
            transfer->PrepareTransferTiledTexture((sceIpuRGB32 *)data, x, mip->m_x2C, mip->m_x28, 0, 0, mip->m_x24);
        } else {
            transfer->PrepareTransferTiledTexture((sceIpuRGB16 *)data, x, mip->m_x2C, mip->m_x28, 0, 0, mip->m_x24);
        }
        return transfer;
    }
    switch (m_bitsPerPixel) {
    case 32:
    case 16: {
        int psm;
        if (m_bitsPerPixel == 32)
            psm = 0;
        else
            psm = 2;
        if (m_prepared) {
            m_transfer.UpdateTransferTexture(data, x, mip->m_x2C, mip->m_x28, psm, m_bitsPerPixel);
        } else {
            m_transfer.PrepareTransferTexture(data, x, mip->m_x2C, mip->m_x28, 0, 0, psm, m_bitsPerPixel, 0);
            m_prepared = 1;
        }
        break;
    }
    case 8: {
        int width, height, size, bpp;
        if (gConvertNativeTo32Bit && IsTextureSize(mip->m_x2C) && mip->m_half) {
            width = mip->m_x2C / 2;
            height = mip->m_x28 / 2;
            size = mip->m_blocks / 2;
            bpp = 32;
        } else {
            width = mip->m_x2C;
            height = mip->m_x28;
            size = mip->m_blocks;
            bpp = 8;
        }
        if (m_prepared) {
            m_transfer.UpdateTransferTextureAndClut(data, (unsigned char *)m_clut8->m_colors, x, y, width, height, 0x13, 0, bpp, size);
        } else {
            m_transfer.PrepareTransferTextureAndClut(data, (unsigned char *)m_clut8->m_colors, x, y, width, height, 0, 0, 0x13, 0, bpp, size);
            m_prepared = 1;
        }
        break;
    }
    case 4:
        if (m_prepared) {
            m_transfer.UpdateTransferTextureAndClut(data, (unsigned char *)m_clut4->m_colors, x, y, mip->m_x2C, mip->m_x28, 0x14, 0, m_bitsPerPixel, mip->m_blocks);
        } else {
            m_transfer.PrepareTransferTextureAndClut(data, (unsigned char *)m_clut4->m_colors, x, y, mip->m_x2C, mip->m_x28, 0, 0, 0x14, 0, m_bitsPerPixel, mip->m_blocks);
            m_prepared = 1;
        }
        break;
    default:
#line 1747 "PSXTexMap.cpp"
        ASSERT(0, "");
        break;
    }
    return &m_transfer;
}


// ---- src/PSXTextureMap/Load_0027A410.cpp


class ColorMapper;
struct TextureReportData;
struct _RECT;

int BitsPerPixel(int format);
void PC2PSXTextureFormat(unsigned char *data, int format, int width, int height);
void PCToPSXColorKey(unsigned int *colorKey, int format);
void PixTransColorConvert(void *dst, void *src, int width, int height, int srcPitch, int dstPitch, int dstFormat,
                          int srcFormat, int, ColorMapper *mapper, int, unsigned int colorKey);

int PSXTextureMap::Load(unsigned char *data, int width, int height, int pitch, int minSize, int srcFormat,
                        int destFormat, ColorMapper *mapper, unsigned int flags, int, int, unsigned char *,
                        _D3DBLEND, _D3DBLEND, int colorKeyMode, unsigned int colorKey)
{
    m_width = width;
    m_height = height;
    m_minSize = minSize;
    m_colorMapper = mapper;
    // (mwcc tests the case values bottom-up, so their order here is retail's.)
    switch (destFormat) {
    case 0x22B:
    case 0x235:
    case 0x613:
        destFormat = 0x613;
        break;
    case 0x115C:
    case 0x22B8:
        m_x4 = 1;
        destFormat = 0x22B8;
        break;
    default:
#line 1802 "PSXTexMap.cpp"
        ASSERT(0, "Load: Invalid destPixelFormat");
        break;
    }
    m_transfer.Create(10);
    m_bitsPerPixel = BitsPerPixel(destFormat);
    m_format = destFormat;
    m_numMips = 1;
    if (flags & 2) {
        for (int w = m_width; m_minSize < w; w >>= 1)
            m_numMips++;
        if (m_numMips > 5)
            m_numMips = 5;
    }
    if (!m_mips[0].m_surface && !AllocateAllSurfaces()) {
        PSXprintf("Unable to create suitable surface! (PSXTextureMap::Load)\n");
#line 1830 "PSXTexMap.cpp"
        ASSERT(0, "Load: Unable to create suitable surface!");
    }
    if (data) {
        if (!m_mips[0].m_surface) {
#line 1845 "PSXTexMap.cpp"
            ASSERT(0, "Load: LpSurfaceMipMapChain[0].LpSurface == NULL");
        }
        PC2PSXTextureFormat(data, srcFormat, width, height);
        PCToPSXColorKey(&colorKey, m_format);
        PixTransColorConvert(m_mips[0].m_surface, data, width, height, pitch, pitch, m_format, srcFormat, 0, mapper,
                             colorKeyMode, colorKey);
        if (flags & 2) {
            if (!BuildMipMapChain(2)) {
#line 1862 "PSXTexMap.cpp"
                ASSERT(0, "Load: Unable to build mipmap chain.");
            }
        }
    }
    return 1;
}



extern "C" void *memalign(unsigned int align, unsigned int size);
extern "C" double log(double x);

extern int MemoryAllocatedForUnCompressedTextureMaps;

int BitsPerPixel(int format);


static inline void BorrowSurface(PSXMipSurface *dst, PSXMipSurface *src)
{
    dst->m_surface = src->m_surface;
    dst->m_tiled = src->m_tiled;
    dst->m_borrowed = 1;
}

PSXTextureMap *PSXTextureMap::Clone()
{
    PSXTextureMap *clone = new PSXTextureMap(1);
    clone->m_width = m_width;
    clone->m_height = m_height;
    clone->m_numMips = m_numMips;
    clone->m_format = m_format;
    clone->m_bitsPerPixel = m_bitsPerPixel;
    clone->m_noMipChain = m_noMipChain;
    int bpp = BitsPerPixel(clone->m_format);
    int width = clone->m_width;
    int height = clone->m_height;
    int size = bpp * (m_height * m_width) >> 3;
    if (!m_noMipChain) {
        clone->m_mips[0].m_surface = memalign(0x40, size);
        MemoryAllocatedForUnCompressedTextureMaps += size;
        memcpy(clone->m_mips[0].m_surface, m_mips[0].m_surface, size);
    } else {
        BorrowSurface(&clone->m_mips[0], &m_mips[0]);
    }
    clone->m_mips[0].m_width = m_width;
    clone->m_mips[0].m_height = m_height;
    clone->m_mips[0].m_x24 = clone->m_mips[0].m_width;
    clone->m_mips[0].m_blocks = Max(m_mips[0].m_width / 64, 1);
    clone->m_mips[0].m_x2C = clone->m_mips[0].m_width;
    clone->m_mips[0].m_x28 = clone->m_mips[0].m_height;
    for (int i = 1; i < Min(Max(m_numMips - 1, 1), 5); i++) {
        width = Max(width >> 1, 1);
        height = Max(height >> 1, 1);
        size = bpp * (width * height) >> 3;
        if (!m_noMipChain) {
            clone->m_mips[i].m_surface = memalign(0x40, size);
            MemoryAllocatedForUnCompressedTextureMaps += size;
            memcpy(clone->m_mips[i].m_surface, m_mips[i].m_surface, size);
        } else {
            BorrowSurface(&clone->m_mips[i], &m_mips[i]);
        }
        clone->m_mips[i].m_width = width;
        clone->m_mips[i].m_height = height;
        clone->m_mips[i].m_x24 = clone->m_mips[i].m_width;
        clone->m_mips[i].m_blocks = Max(m_mips[i].m_width / 64, 1);
        clone->m_mips[i].m_widthLog2 = log(m_mips[i].m_width) / log(2.0);
        clone->m_mips[i].m_heightLog2 = log(m_mips[i].m_height) / log(2.0);
        clone->m_mips[i].m_x2C = clone->m_mips[i].m_width;
        clone->m_mips[i].m_x28 = clone->m_mips[i].m_height;
    }
    return clone;
}


// ---- src/PSXTextureMap/InVideoRAM.cpp

int PSXTextureMap::InVideoRAM()
{
    return 1;
}


// ---- src/PSXTextureMap/PlaceInVideoMemory.cpp

int PSXTextureMap::PlaceInVideoMemory(int, int, int)
{
    return 1;
}


// ---- src/PSXTextureMap/RemoveFromVideoMemory.cpp

void PSXTextureMap::RemoveFromVideoMemory()
{
    
}


// ---- src/PSXTextureMap/UpdateVideoMemory.cpp
struct _RECT;

int PSXTextureMap::UpdateVideoMemory(_RECT *, int)
{
    return 1;
}


// ---- src/PSXTextureMap/BuildMipMapChain.cpp
// Block-form ASSERT (no do/while(0)) gives retail's loop-only label padding.
// The goto shape gives retail's beqz-to-epilogue and the dead li v0,0.


class ColorMapper;

void PixTransScale(void *dst, void *src, int dstWidth, int dstHeight, int dstPitch, int srcPitch, int a, int format,
                   ColorMapper *mapper, int arg);


int PSXTextureMap::BuildMipMapChain(int arg)
{
    if (!m_noMipChain) {
#line 1989 "PSXTexMap.cpp"
        ASSERT(m_mips[0].m_surface, "BuildMipMapChain: LpSurfaceMipMapChain[0].LpSurface==NULL");
        void *src = m_mips[0].m_surface;
        if (!src)
            goto fail;
        int srcWidth = m_width;
        int height = m_height;
        int width = srcWidth;
        for (int i = 1; i < m_numMips; i++) {
            width = (width >> 1 > 8) ? width >> 1 : 8;
            height = (height >> 1 > 8) ? height >> 1 : 8;
            PixTransScale(m_mips[i].m_surface, src, width, height, width, srcWidth, 1, m_format, m_colorMapper, arg);
            src = m_mips[i].m_surface;
            srcWidth = width;
        }
    }
    return 1;
fail:
    return 0;
}


// ---- src/PSXTextureMap/SetTextureStage.cpp

int PSXTextureMap::SetTextureStage()
{
    return 1;
}


// ---- src/PSXTextureMap/SetRenderStates.cpp

void PSXTextureMap::SetRenderStates()
{
    
}


// ---- src/PSXTextureMap/LockSys.cpp
struct _RECT;

int BitsPerPixel(int format);


void *PSXTextureMap::LockSys(_RECT *, long *pitch, unsigned int)
{
    if (pitch)
        *pitch = (m_width * BitsPerPixel(m_format)) >> 3;
    return m_mips[0].m_surface;
}


// ---- src/PSXTextureMap/UnlockSys.cpp
struct _RECT;

int PSXTextureMap::UnlockSys(_RECT *)
{
    return 1;
}


// ---- src/PSXTextureMap/LockMipMapSurface.cpp

void * PSXTextureMap::LockMipMapSurface(int)
{
    return 0;
}


// ---- src/PSXTextureMap/UnlockMipMapSurface.cpp

int PSXTextureMap::UnlockMipMapSurface(int)
{
    return 0;
}


// ---- src/PSXTextureMap/DumpChain.cpp

int PSXTextureMap::DumpChain()
{
    return 0;
}


// ---- src/PSXTextureMap/GetReportData.cpp

struct TextureReportData {
    int m_format;                   // +0x00
    int m_compressedBytes;          // +0x04
    int m_bytes;                    // +0x08
};

void PSXTextureMap::GetReportData(TextureReportData *report)
{
    report->m_compressedBytes = 0;
    report->m_bytes = 0;
    int i;
    int total = 0;
    if (m_compressed) {
        for (i = 0; i < 5; i++) {
            PSXMipSurface *mip = &m_mips[i];
            if (mip->m_surface)
                total += mip->m_compressedSize;
        }
        report->m_compressedBytes = total;
    } else {
        for (i = 0; i < 5; i++) {
            PSXMipSurface *mip = &m_mips[i];
            if (mip->m_surface)
                total += (m_bitsPerPixel * (mip->m_width * mip->m_height)) >> 3;
        }
        report->m_bytes = total;
    }
    report->m_format = m_format;
}

