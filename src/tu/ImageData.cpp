// ImageData.cpp  @ 0x00242150 (text 0x242150-0x243720)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's ImageData.cpp as one translation unit: a TU without an initializer
// in HiResMeter.cpp's tu_map row (its strings and RTTI follow
// HiResMeter.cpp's; link order is alphabetical: HiResMeter < ImageData <
// InputDevice < InputDeviceType). `make link` places the object's data as
// well as its code (tools/link_c.py).



#include "ImageData/ImageData.h"
#include "TextureMap/TextureMap.h"
#include "mwcc_shim.h"
// ---- src/ImageData/ctor.cpp

ImageData::ImageData()
{
    m_buffer = 0;
    m_bufferSize = 0;
    Reset();
    m_xEC = 1;
    m_weights[0][0] = 0.05;
    m_weights[0][1] = 0.05;
    m_weights[0][2] = 0.05;
    m_weights[0][3] = 0.05;
    m_weights[0][4] = 0.05;
    m_weights[1][0] = 0.05;
    m_weights[1][1] = 0.25;
    m_weights[1][2] = 0.55;
    m_weights[1][3] = 0.25;
    m_weights[1][4] = 0.05;
    m_weights[2][0] = 0.05;
    m_weights[2][1] = 0.55;
    m_weights[2][2] = 0.95;
    m_weights[2][3] = 0.55;
    m_weights[2][4] = 0.05;
    m_weights[3][0] = 0.05;
    m_weights[3][1] = 0.25;
    m_weights[3][2] = 0.55;
    m_weights[3][3] = 0.25;
    m_weights[3][4] = 0.05;
    m_weights[4][0] = 0.05;
    m_weights[4][1] = 0.05;
    m_weights[4][2] = 0.05;
    m_weights[4][3] = 0.05;
    m_weights[4][4] = 0.05;
}


// ---- src/ImageData/Reset.cpp


void ImageData::Reset()
{
    m_bytesPerPixel = 0;
    m_currentLevel = 0;
    m_size = 0;
    m_colorMapper = 0;
    for (short i = 0; i < 15; i++) {
        m_entries[i].m_width = 0;
        m_entries[i].m_c = 0;
        m_entries[i].m_height = 0;
        m_entries[i].m_d = 0;
        m_entries[i].m_offset = 0;
    }
}


// ---- src/ImageData/LoadFromArray.cpp
extern "C" void *memcpy(void *dst, const void *src, unsigned int n);

int BytesPerPixel(ImageDataFormat format);


int ImageData::LoadFromArray(unsigned char *data, ImageDataFormat format, int width, int height, int d, int c)
{
    if (!data)
        return 0;
    Reset();
    m_format = format;
    m_currentLevel = 0;
    m_entries[m_currentLevel].m_width = width;
    m_entries[m_currentLevel].m_height = height;
    m_entries[m_currentLevel].m_d = d;
    m_entries[m_currentLevel].m_c = c;
    m_bytesPerPixel = BytesPerPixel(m_format);
    m_size = m_bytesPerPixel * (m_entries[m_currentLevel].m_width * m_entries[m_currentLevel].m_height);
    if (!EnsureBufferSpace(m_size))
        return 0;
    memcpy(m_buffer, data, m_size);
    return 1;
}


// ---- src/ImageData/CreateMipMapChain.cpp
extern "C" void *memcpy(void *dst, const void *src, unsigned int n);
extern "C" void *memset(void *dst, int c, unsigned int n);
extern "C" void *malloc(unsigned int size);
extern "C" void free(void *p);

ImageDataFormat MipmappedFormatFromImageDataFormat(ImageDataFormat format);


int ImageData::CreateMipMapChain()
{
    if (m_size <= 0)
        return 0;
    if (m_currentLevel > 0)
        return 0;
    long total;
    unsigned char *original = (unsigned char *)malloc(m_size);
    if (!original)
        return 0;
    memcpy(original, m_buffer, m_size);
    // Count the levels down to 1 pixel wide.
    int size = m_entries[0].m_width;
    m_currentLevel = 0;
    do {
        size /= 2;
        m_currentLevel++;
    } while (size > 1);
    m_entries[m_currentLevel].m_width = m_entries[0].m_width;
    m_entries[m_currentLevel].m_height = m_entries[0].m_height;
    m_entries[m_currentLevel].m_c = m_entries[0].m_c;
    m_entries[m_currentLevel].m_d = m_entries[0].m_d;
    // do-whiles: retail has no entry tests (there is at least one level).
    int i = m_currentLevel - 1;
    do {
        m_entries[i].m_width = m_entries[i + 1].m_width / 2;
        m_entries[i].m_height = m_entries[i + 1].m_height / 2;
        m_entries[i].m_c = m_entries[i + 1].m_c / 2;
        m_entries[i].m_d = m_entries[i + 1].m_d / 2;
        i--;
    } while (i >= 0);
    m_entries[0].m_offset = 0;
    i = 1;
    do {
        m_entries[i].m_offset = m_entries[i - 1].m_offset +
                                m_bytesPerPixel * (m_entries[i - 1].m_width * m_entries[i - 1].m_height);
        i++;
    } while (i <= m_currentLevel);
    total = m_entries[m_currentLevel].m_offset +
                 m_bytesPerPixel * (m_entries[m_currentLevel].m_width * m_entries[m_currentLevel].m_height);
    if (!EnsureBufferSpace(total))
        return 0;
    memset(m_buffer, 0, total);
    memcpy(m_buffer + m_entries[m_currentLevel].m_offset, original, m_size);
    m_size = total;
    m_format = MipmappedFormatFromImageDataFormat(m_format);
    i = 0;
    do {
        Scale(m_entries[i].m_width, m_entries[i].m_height, m_entries[m_currentLevel].m_width,
              m_entries[m_currentLevel].m_height, m_bytesPerPixel, m_buffer + m_entries[i].m_offset,
              m_buffer + m_entries[m_currentLevel].m_offset, m_format);
        i++;
    } while (i < m_currentLevel);
    if (original)
        free(original);
    return 1;
}


// ---- src/ImageData/DropMipMapChain.cpp
extern "C" void *memcpy(void *dst, const void *src, unsigned int n);

ImageDataFormat NonMipmappedFormatFromImageDataFormat(ImageDataFormat format);


int ImageData::DropMipMapChain()
{
    if (m_size <= 0)
        return 0;
    if (m_currentLevel == 0)
        return 0;
    // Keep only the current (smallest) level: move it to the buffer start.
    memcpy(m_buffer, m_buffer + m_entries[m_currentLevel].m_offset,
           m_bytesPerPixel * (m_entries[m_currentLevel].m_width * m_entries[m_currentLevel].m_height));
    m_entries[0].m_width = m_entries[m_currentLevel].m_width;
    m_entries[0].m_c = m_entries[m_currentLevel].m_c;
    m_entries[0].m_height = m_entries[m_currentLevel].m_height;
    m_entries[0].m_d = m_entries[m_currentLevel].m_d;
    m_entries[0].m_offset = 0;
    // A do-while: retail has no entry test (m_currentLevel is known non-zero).
    int i = 1;
    do {
        m_entries[i].m_width = 0;
        m_entries[i].m_c = 0;
        m_entries[i].m_height = 0;
        m_entries[i].m_d = 0;
        m_entries[i].m_offset = 0;
        i++;
    } while (i <= m_currentLevel);
    m_currentLevel = 0;
    m_size = m_entries[m_currentLevel].m_offset +
             m_bytesPerPixel * (m_entries[m_currentLevel].m_width * m_entries[m_currentLevel].m_height);
    m_format = NonMipmappedFormatFromImageDataFormat(m_format);
    return 1;
}


// ---- src/ImageData/Scale.cpp
// Scale__9ImageDataFiiiisPUcPUc15ImageDataFormat  @ 0x00242A60
// Register colouring notes: the kernel weight is read through `double *wp`, so the
// twice-used `*wp` load becomes an IroCSE temp, numbered below the IroLoop-hoisted
// source-row pointer (a named `double w` always numbers above every temp and took the
// pointer's $s5). The pixel is a named `unsigned char px` and the product is written
// `px * *wp`, which gives dpmul(pixel, weight) as in retail. sx++/sy++ (not += 1.0) and
// the int bpp assigned inside the || put 1.0 in $a1 and bpp's home at 0x170.

// Compiler session state: retail compiled
// this file in an mwcc session that had already generated a two-argument runtime
// helper call (64-bit / or %, double arithmetic or compare). mwcc then models
// every later helper call (__floatdisf, fptoui, fptodp, ...) as reading $a0-$a1,
// so temporaries live before such a call skip $a0/$a1. This unused function
// recreates that state; it is not part of the retail file.
// (Still needed in the whole TU: Scale is 7 words off without it.)
static void MWCC_SHIM_TEXT MwccSessionState(long *a) { a[0] = a[0] / a[1]; }


class ColorMapper;

int PixelFormatFromImageDataFormat(ImageDataFormat format);
int PixTransScale(void *dst, void *src, int dstWidth, int dstHeight, int dstPitch, int srcPitch, int level, int format,
                  ColorMapper *mapper, int mode);

// Box-filters src down to dst one byte plane at a time, weighting each
// source texel with the 5x5 kernel.
int ImageData::Scale(int dstWidth, int dstHeight, int srcWidth, int srcHeight, short bytesPerPixel,
                     unsigned char *dst, unsigned char *src, ImageDataFormat format)
{
    unsigned char *dstLine;
    unsigned char *out;
    double xRatio;
    double yRatio;
    double yLow;
    double yHigh;
    int x;
    int y;
    long srcRow;
    long dstRow;
    short c;
    double yLow0;
    double yHigh0;
    double xLow0;
    double xHigh0;
    double ySpan;
    double xSpan;
    int bpp;
    unsigned char *srcPlane;
    double xLow;
    short ky;
    double sx;
    double xHigh;
    double sy;
    double start;
    double sum;
    double weight;
    short kx;
    if (m_xEC || (bpp = bytesPerPixel) < 3) {
        short level = 0;
        int w = srcWidth;
        while (w > dstWidth) {
            level++;
            w >>= 1;
        }
        return PixTransScale(dst, src, dstWidth, dstHeight, dstWidth, srcWidth, level,
                             PixelFormatFromImageDataFormat(format), m_colorMapper, 2) != 0;
    }

    xRatio = (double)srcWidth / (double)dstWidth;
    yRatio = (double)srcHeight / (double)dstHeight;
    srcRow = srcWidth * bpp;
    dstRow = dstWidth * bpp;
    c = 0;
    start = 0.0;
    yLow0 = start - yRatio;
    yHigh0 = start + yRatio;
    xLow0 = start - xRatio;
    xHigh0 = start + xRatio;
    do {
        y = 0;
        srcPlane = src + c;
        dstLine = dst + c;
        yLow = yLow0;
        yHigh = yHigh0;
        do {
            out = dstLine;
            xLow = xLow0;
            xHigh = xHigh0;
            x = 0;
            ySpan = yHigh - yLow;
            do {
                weight = 0.0;
                sum = 0.0;
                sy = yLow;
                xSpan = xHigh - xLow;
                do {
                    ky = (int)((sy - yLow) / ySpan * 4.0);
                    sx = xLow;
                    do {
                        if (sx >= 0.0 && sx < srcWidth && sy >= 0.0 && sy < srcHeight) {
                            kx = (int)((sx - xLow) / xSpan * 4.0);
                            double *wp = &m_weights[ky][kx];
                            unsigned char px = (srcPlane + (int)sy * srcRow)[(int)sx * bpp];
                            sum = sum + px * *wp;
                            weight = weight + *wp;
                        }
                        sx++;
                    } while (sx < xHigh);
                    sy++;
                } while (sy < yHigh);
                *out = (unsigned int)(sum / weight);
                x++;
                xLow = xHigh - xRatio;
                xHigh = xHigh + xRatio;
                out += bpp;
            } while (x < dstWidth);
            y++;
            yLow = yHigh - yRatio;
            yHigh = yHigh + yRatio;
            dstLine += dstRow;
        } while (y < dstHeight);
        c++;
    } while (c < bpp);
    return 1;
}


// ---- src/ImageData/Save.cpp
struct FILE_HANDLE_STRUCT;


int ImageData::Save(FILE_HANDLE_STRUCT *file)
{
    return Save(file, m_format);
}


// ---- src/ImageData/Save_00242F60.cpp
// Save__9ImageDataFP18FILE_HANDLE_STRUCT15ImageDataFormat  @ 0x00242F60
// Register colouring notes: the level loop reuses `i` (the counter of the offset-table loop)
// and the converted width reuses `alpha`. Each reused variable's later live range is split off
// by mwcc's web splitting into a new temp, and those temps are numbered BELOW the IroLoop
// pointer temp (this + i*0x18), which a separate named counter/width cannot be. That vreg
// order (counter < pointer < size) is what gives retail's counter $s0 / pointer $s6.

extern "C" void *malloc(unsigned int size);
extern "C" void free(void *p);

struct FILE_HANDLE_STRUCT;
long FTELL(FILE_HANDLE_STRUCT *file);
int FSEEK(FILE_HANDLE_STRUCT *file, long offset, int whence);
unsigned int FWRITE(void *data, unsigned int size, unsigned int count, FILE_HANDLE_STRUCT *file);


int IsAlphaFormat(int bitFormat);
int IsMipMapped(ImageDataFormat format);
int IsCompressedFormat(ImageDataFormat format);
int CompressionType(ImageDataFormat format);
int BytesPerPixel(ImageDataFormat format);
int PixelFormatFromImageDataFormat(ImageDataFormat format);
int LZWCompress(void *input, void *output, int size);
void PixTransColorConvert(void *dst, void *src, int width, int height, int srcPitch, int dstPitch, int dstFormat,
                          int srcFormat, int, ColorMapper *mapper, int, unsigned int colorKey);


// Writes the image (every mip level for a mipmapped format) in the given
// format, converting and LZW-compressing as needed.
int ImageData::Save(FILE_HANDLE_STRUCT *file, ImageDataFormat format)
{
    int i;
    
    int alpha;
    int start;
    int offset;
    unsigned char *pixels;
    int createdChain;
    int bpp;
    short bytesPerPixel;
    unsigned char *compressed;
    int converted;
    start = -1;
    offset = 0;
    pixels = 0;
    alpha = IsAlphaFormat(format);
    if (alpha != IsAlphaFormat(m_format))
        return 0;
    FWRITE(&m_entries[m_currentLevel].m_width, 4, 1, file);
    FWRITE(&m_entries[m_currentLevel].m_height, 4, 1, file);
    FWRITE(&format, 4, 1, file);
    if (IsMipMapped(format)) {
        start = FTELL(file);
        i = 0;
        do {
            FWRITE(&offset, 4, 1, file);
            i++;
        } while (i <= m_currentLevel);
        FWRITE(&offset, 4, 1, file);
    }
    bytesPerPixel = BytesPerPixel(format);
    if (format != m_format) {
        pixels = (unsigned char *)malloc(bytesPerPixel * (m_entries[m_currentLevel].m_width * m_entries[m_currentLevel].m_height));
        if (!pixels)
            return 0;
    }
    bpp = bytesPerPixel;
    compressed = (unsigned char *)malloc(bpp * (m_entries[m_currentLevel].m_width * m_entries[m_currentLevel].m_height));
    if (!compressed)
        return 0;
    i = 0;
    createdChain = 0;
    if (IsMipMapped(format)) {
        if (!m_currentLevel) {
            CreateMipMapChain();
            createdChain = 1;
        }
    } else {
        i = m_currentLevel;
    }
    do {
        if (format != m_format) {
            alpha = m_entries[i].m_width;
            PixTransColorConvert(pixels, m_buffer + m_entries[i].m_offset, alpha, m_entries[i].m_height, alpha, alpha,
                                 PixelFormatFromImageDataFormat(format), PixelFormatFromImageDataFormat(m_format), 1,
                                 m_colorMapper, 0x80, 0xFF00FF);
            converted = 1;
        } else {
            pixels = m_buffer + m_entries[i].m_offset;
            converted = 0;
        }
        unsigned long size = bpp * (m_entries[i].m_width * m_entries[i].m_height);
        unsigned long compressedSize = size;
        if (IsCompressedFormat(format)) {
            switch (CompressionType(format)) {
            case 3:
                compressedSize = LZWCompress(pixels, compressed, size);
                break;
            }
        }
        if (IsMipMapped(format)) {
            offset = FTELL(file) - start;
            FSEEK(file, start, 0);
            FWRITE(&offset, 4, 1, file);
            start = FTELL(file);
            FSEEK(file, 0, 2);
        } else if (compressedSize < size) {
            FWRITE(&compressedSize, 8, 1, file);
        }
        if (compressedSize < size)
            FWRITE(compressed, compressedSize, 1, file);
        else
            FWRITE(pixels, size, 1, file);
        i++;
    } while (i <= m_currentLevel);
    if (IsMipMapped(format)) {
        offset = FTELL(file) - start;
        FSEEK(file, start, 0);
        FWRITE(&offset, 4, 1, file);
        start = FTELL(file);
        FSEEK(file, 0, 2);
    }
    if (createdChain)
        DropMipMapChain();
    if (pixels && converted)
        free(pixels);
    if (compressed)
        free(compressed);
    return 1;
}


// ---- src/ImageData/Dump.cpp
// Retail built this file with dead-assignment elimination off (it orders the
// loads in the export block).
#pragma opt_dead_assignments off

extern "C" int sprintf(char *buf, const char *fmt, ...);
extern "C" char *strcpy(char *dst, const char *src);

struct FILE_HANDLE_STRUCT;
FILE_HANDLE_STRUCT *FOPEN(char *name, const char *mode);
int FCLOSE(FILE_HANDLE_STRUCT *file);

struct RGBTripple;
struct RGBQuad;
void ExportImage16(unsigned short *pixels, int width, int height, int stride, int format, char *name, unsigned char a);
void ExportImage24(RGBTripple *pixels, int width, int height, int stride, char *name, unsigned char a);
void ExportImage32(RGBQuad *pixels, int width, int height, int stride, char *name, unsigned char a);


// Writes every mip level at least minSize across to TGA files.
void ImageData::Dump(const char *name, int minSize)
{
    char ext[5] = ".tga";
    char base[0x110];
    char file[0x108];
    int i = 0;
    if (!name) {
        name = "C:\\temp\\texture";
        FILE_HANDLE_STRUCT *f = 0;
        do {
            if (f)
                FCLOSE(f);
            sprintf(base, "%s%03d", name, i);
            f = FOPEN(base, "r");
            i++;
        } while (f);
    } else {
        strcpy(base, name);
    }
    int j = 0;
    do {
        sprintf(file, "%s_%02d%s", name, j, ext);
        int width = m_entries[j].m_width;
        int height = m_entries[j].m_height;
        if (!(width < minSize && height < minSize)) {
            int offset = m_entries[j].m_offset;
            unsigned char *pixels = m_buffer;
            pixels += offset;
            if (m_bytesPerPixel == 2)
                ExportImage16((unsigned short *)pixels, width, height, 0, m_format == 0x22B ? 0xE5 : 0x7E0, file,
                              0x20);
            else if (m_bytesPerPixel == 3)
                ExportImage24((RGBTripple *)pixels, width, height, 0, file, 0x20);
            else if (m_bytesPerPixel == 4)
                ExportImage32((RGBQuad *)pixels, width, height, 0, file, 0x20);
        }
        j++;
    } while (j <= m_currentLevel);
}
#pragma opt_dead_assignments reset


// ---- src/ImageData/dtor.cpp
extern "C" void free(void *p);


ImageData::~ImageData()
{
    if (m_buffer)
        free(m_buffer);
    m_bufferSize = 0;
}


// ---- src/ImageData/EnsureBufferSpace.cpp
extern "C" void *malloc(unsigned int size);
extern "C" void *realloc(void *p, unsigned int size);


int ImageData::EnsureBufferSpace(long size)
{
    if (!m_buffer) {
        m_buffer = (unsigned char *)malloc(size);
        m_bufferSize = size;
    } else if (m_bufferSize < size) {
        m_buffer = (unsigned char *)realloc(m_buffer, size);
        m_bufferSize = size;
    }
    if (!m_buffer) {
        m_bufferSize = 0;
        return 0;
    }
    return 1;
}

