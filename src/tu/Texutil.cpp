// Texutil.cpp  @ 0x0029B0A0 (text 0x29B0A0-0x29D010)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's Texutil.cpp as one translation unit, a TU without an initializer
// inside Terrain.cpp's tu_map row: the texture-format helpers. Its .rodata is
// the helpers' jump tables (0x41ADC0) and then the "Texutil.cpp" assert
// strings, after TextureMapManager.cpp's RTTI, then the TGA reader/writer's
// "rb", "" and "wb" (no label restart). The TGA functions (0x29B9B0-0x29D010)
// are this TU's tail: placed here, `make link` places their strings too.


#include "soultree_assert.h"

enum ImageDataFormat {};      // as in TextureMap.h and ImageData.h
enum TexFormat {};

// ---- src/global/BytesPerPixel.cpp
// Enumerator names are unknown, so the cases are numeric.

int BytesPerPixel(ImageDataFormat format)
{
    switch (format) {
    case 12:
    case 15:
    case 27:
    case 30:
        return 4;
    case 1:
    case 9:
    case 16:
    case 24:
        return 3;
    case 2:
    case 7:
    case 17:
    case 22:
        return 2;
    case 3:
    case 8:
    case 18:
    case 23:
        return 2;
    case 10:
    case 13:
    case 25:
    case 28:
        return 2;
    case 11:
    case 14:
    case 26:
    case 29:
        return 2;
    case 4:
    case 6:
    case 19:
    case 21:
        return 1;
    case 5:
    case 20:
        return 2;
    case 31:
    case 32:
        return 3;
    case 33:
    case 34:
        return 2;
    case 35:
    case 36:
        return 4;
    }
    return 0;
}


// ---- src/global/IsCompressedFormat.cpp

// Every format is listed (retail's jump table has 37 entries); the enumerator
// names are unknown, so the cases are numeric.
int IsCompressedFormat(ImageDataFormat format)
{
    switch (format) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    case 19:
    case 28:
    case 29:
    case 30:
        return 0;
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 20:
    case 21:
    case 22:
    case 23:
    case 24:
    case 25:
    case 26:
    case 27:
    case 31:
    case 32:
    case 33:
    case 34:
    case 35:
    case 36:
        return 1;
    }
    return 0;
}


// ---- src/global/CompressionType.cpp

int CompressionType(ImageDataFormat format)
{
    switch (format) {
    case 1:
    case 2:
    case 3:
    case 4:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    case 19:
    case 28:
    case 29:
    case 30:
        return 1;
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 21:
    case 22:
    case 23:
    case 24:
    case 25:
    case 26:
    case 27:
        return 3;
    case 5:
    case 20:
        return 2;
    case 31:
    case 32:
    case 33:
    case 34:
    case 35:
    case 36:
        return 4;
    case 0:
    default:
        return 0;
    }
}


// ---- src/global/IsMipMapped.cpp

// Every format is listed (retail's jump table has 37 entries); the enumerator
// names are unknown, so the cases are numeric.
int IsMipMapped(ImageDataFormat format)
{
    switch (format) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 31:
    case 33:
    case 35:
        return 0;
    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
    case 21:
    case 22:
    case 23:
    case 24:
    case 25:
    case 26:
    case 27:
    case 28:
    case 29:
    case 30:
    case 32:
    case 34:
    case 36:
        return 1;
    }
    return 0;
}


// ---- src/global/BitsPerPixel.cpp
// Enumerator names are unknown, so the cases are numeric.

int BitsPerPixel(ImageDataFormat format)
{
    switch (format) {
    case 12:
    case 15:
    case 27:
    case 30:
        return 32;
    case 1:
    case 9:
    case 16:
    case 24:
        return 24;
    case 2:
    case 7:
    case 17:
    case 22:
        return 16;
    case 3:
    case 8:
    case 18:
    case 23:
        return 16;
    case 10:
    case 13:
    case 25:
    case 28:
        return 16;
    case 11:
    case 14:
    case 26:
    case 29:
        return 16;
    case 4:
    case 6:
    case 19:
    case 21:
        return 8;
    case 5:
    case 20:
        return 16;
    case 31:
    case 32:
        return 24;
    case 33:
    case 34:
        return 16;
    case 35:
    case 36:
        return 32;
    }
    return 0;
}


// ---- src/global/PixelFormatFromImageDataFormat.cpp
// Enumerator names are unknown, so the cases are numeric.

int PixelFormatFromImageDataFormat(ImageDataFormat format)
{
    switch (format) {
    case 12:
    case 15:
    case 27:
    case 30:
        return 8888;
    case 1:
    case 9:
    case 16:
    case 24:
        return 888;
    case 2:
    case 7:
    case 17:
    case 22:
        return 555;
    case 3:
    case 8:
    case 18:
    case 23:
        return 565;
    case 10:
    case 13:
    case 25:
    case 28:
        return 1555;
    case 11:
    case 14:
    case 26:
    case 29:
        return 4444;
    case 4:
    case 6:
    case 19:
    case 21:
        return 8;
    case 5:
    case 20:
        return 555;
    case 31:
    case 32:
        return 888;
    case 33:
    case 34:
        return 1555;
    case 35:
    case 36:
        return 8888;
    }
    return 0;
}


// ---- src/global/BitsPerPixel_0029B350.cpp
// ASSERT contains inline asm: retail's label alignment for the two asserting functions.
#pragma alignlabel off



int BitsPerPixel(TexFormat format)
{
    switch (format) {
    case 0:
        return 4;
    case 1:
        return 8;
    case 2:
        return 16;
    default:
#line 424 "Texutil.cpp"
        ASSERT(0, "BytesPerPixel : Undefined TexFormat");
        return -1;
    }
}


// ---- src/global/PixelFormatFromTexFormat.cpp



int PixelFormatFromTexFormat(TexFormat format)
{
    switch (format) {
    case 0:
        return 4;
    case 1:
        return 8;
    case 2:
        return 0x613;
    default:
#line 439 "Texutil.cpp"
        ASSERT(0, "PixelFormatFromTexFormat : Undefined TexFormat");
        return -1;
    }
}


#pragma alignlabel reset


// ---- src/global/MipmappedFormatFromImageDataFormat.cpp

// Maps a single-level image format to its mipmapped equivalent; other
// formats are returned unchanged.
ImageDataFormat MipmappedFormatFromImageDataFormat(ImageDataFormat format)
{
    switch (format) {
    case 0xC:
        return (ImageDataFormat)0x1B;
    case 0xF:
        return (ImageDataFormat)0x1E;
    case 0x9:
        return (ImageDataFormat)0x18;
    case 0x1:
        return (ImageDataFormat)0x10;
    case 0x7:
        return (ImageDataFormat)0x16;
    case 0x2:
        return (ImageDataFormat)0x11;
    case 0x8:
        return (ImageDataFormat)0x17;
    case 0x3:
        return (ImageDataFormat)0x12;
    case 0xA:
        return (ImageDataFormat)0x19;
    case 0xD:
        return (ImageDataFormat)0x1C;
    case 0xB:
        return (ImageDataFormat)0x1A;
    case 0xE:
        return (ImageDataFormat)0x1D;
    case 0x6:
        return (ImageDataFormat)0x15;
    case 0x4:
        return (ImageDataFormat)0x13;
    case 0x5:
        return (ImageDataFormat)0x14;
    case 0x1F:
        return (ImageDataFormat)0x20;
    case 0x21:
        return (ImageDataFormat)0x22;
    case 0x23:
        return (ImageDataFormat)0x24;
    }
    return format;
}


// ---- src/global/NonMipmappedFormatFromImageDataFormat.cpp

// Maps a mipmapped image format to its single-level equivalent; other
// formats are returned unchanged.
ImageDataFormat NonMipmappedFormatFromImageDataFormat(ImageDataFormat format)
{
    switch (format) {
    case 0x1B:
        return (ImageDataFormat)0xC;
    case 0x1E:
        return (ImageDataFormat)0xF;
    case 0x18:
        return (ImageDataFormat)0x9;
    case 0x10:
        return (ImageDataFormat)0x1;
    case 0x16:
        return (ImageDataFormat)0x7;
    case 0x11:
        return (ImageDataFormat)0x2;
    case 0x17:
        return (ImageDataFormat)0x8;
    case 0x12:
        return (ImageDataFormat)0x3;
    case 0x19:
        return (ImageDataFormat)0xA;
    case 0x1C:
        return (ImageDataFormat)0xD;
    case 0x1A:
        return (ImageDataFormat)0xB;
    case 0x1D:
        return (ImageDataFormat)0xE;
    case 0x15:
        return (ImageDataFormat)0x6;
    case 0x13:
        return (ImageDataFormat)0x4;
    case 0x14:
        return (ImageDataFormat)0x5;
    case 0x20:
        return (ImageDataFormat)0x1F;
    case 0x22:
        return (ImageDataFormat)0x21;
    case 0x24:
        return (ImageDataFormat)0x23;
    }
    return format;
}


// ---- src/global/BytesPerPixel_0029B770.cpp
// Bytes per pixel of a pixel-format code (the codes encode the bit layout,
// e.g. 0x22B8 = 8888, 0x378 = 888, 0x235 = 565, 0x115C = 4444).
int BytesPerPixel(int format)
{
    switch (format) {
    case 0x22B8:
        return 4;
    case 0x378:
        return 3;
    case 0x235:
        return 2;
    case 0x22B:
        return 2;
    case 0x613:
        return 2;
    case 0x115C:
        return 2;
    case 8:
        return 1;
    }
    return 0;
}


// ---- src/global/BitsPerPixel_0029B830.cpp
// Formats are named by their bit layout (555 = 5:5:5, 8 = paletted).
int BitsPerPixel(int pixelFormat)
{
    switch (pixelFormat) {
    case 8888:
        return 32;
    case 888:
        return 24;
    case 565:
        return 16;
    case 555:
        return 16;
    case 1555:
        return 16;
    case 4444:
        return 16;
    case 8:
        return 8;
    }
    return 0;
}


// ---- src/global/IsAlphaFormat.cpp
int IsAlphaFormat(int bitFormat)
{
    switch (bitFormat) {
    case 8888:
        return 1;
    case 4444:
        return 1;
    }
    return 0;
}


// ---- src/global/ReviseIpuImageDataFormat.cpp

int ReviseIpuImageDataFormat(ImageDataFormat &format)
{
    int revised = 0;
    switch (format) {
    case 0x1F:
    case 0x20:
        format = (ImageDataFormat)0x1F;
        revised = 1;
        break;
    case 0x21:
    case 0x22:
        format = (ImageDataFormat)0x20;
        revised = 1;
        break;
    }
    return revised;
}


// ---- src/global/IsIpuCompressed.cpp

// Enumerator names are unknown, so the cases are numeric.
int IsIpuCompressed(ImageDataFormat format)
{
    switch (format) {
    case 31:
    case 32:
    case 33:
    case 34:
    case 35:
    case 36:
        return 1;
    }
    return 0;
}

// ---- The TGA reader/writer (0x29B9B0-0x29D010)
#include "DataAccess/DataAccess.h"
#include "TgaFile/TgaFile.h"
#include "RGBTripple/RGBTripple.h"
#include "RGBQuad/RGBQuad.h"

extern "C" void *malloc(unsigned int size);
extern "C" void free(void *p);
extern "C" void *memcpy(void *dst, const void *src, unsigned int n);
extern "C" char *strcpy(char *dst, const char *src);

struct FILE_HANDLE_STRUCT;
FILE_HANDLE_STRUCT *FOPEN(char *name, const char *mode);
int FWRITE(void *data, unsigned int size, unsigned int count, FILE_HANDLE_STRUCT *file);
int FCLOSE(FILE_HANDLE_STRUCT *file);

extern DataAccess *LpDataAccess;

TgaFile *ReadTgaFileHeader(long file, TgaFile *tga);
int Read16BitTgaData(TgaFile *tga, long file);
int Read24BitTgaData(TgaFile *tga, long file);
int Read32BitTgaData(TgaFile *tga, long file);
int ExportTgaFile(TgaFile *tga, int stride, int format, unsigned char descriptor);

// ---- src/global/Read24BitTgaData.cpp


static inline void SwapRedBlue(RGBTripple *p, int n)
{
    unsigned char c;
    int i;
    RGBTripple *q;
    // Written as a guarded do-while: the for form schedules the two
    // initialisations differently from retail.
    i = 0;
    if (i < n) {
        q = p;
        do {
            c = q->r;
            q->r = q->b;
            q->b = c;
            q++;
            i++;
        } while (i < n);
    }
}

// Reads 24-bit pixel data, raw (type 2) or run-length encoded, swaps BGR to
// RGB and flips it to top-left origin.
int Read24BitTgaData(TgaFile *tga, long file)
{
    unsigned char header;
    RGBTripple pixel;
    unsigned int size;
    RGBTripple *end;
    int i;
    unsigned char c;
    int count;
    int numPixels;
    RGBTripple *p;
    RGBTripple *top;
    RGBTripple *bottom;
    RGBTripple *row;
    int n;

    numPixels = tga->width * tga->height;
    size = numPixels * sizeof(RGBTripple);
    if (tga->pixelsSize < size) {
        if (tga->pixels)
            free(tga->pixels);
        tga->pixels = 0;
        tga->pixelsSize = 0;
    }
    if (!tga->pixels) {
        if (!(tga->pixels = (unsigned char *)malloc(size)))
            return 0;
        tga->pixelsSize = size;
    }
    p = (RGBTripple *)tga->pixels;
    end = p + numPixels;
    if (tga->imageType == 2) {
        if (LpDataAccess->FRead(file, p, 3, numPixels) != numPixels)
            return 0;
        for (i = 0; i < numPixels; i++) {
            c = p->r;
            p->r = p->b;
            p->b = c;
            p++;
        }
    } else {
        while (p < end) {
            LpDataAccess->FRead(file, &header, 1, 1);
            count = header;
            if (count & 0x80) {
                count &= 0x7F;
                if (LpDataAccess->FRead(file, &pixel, 3, 1) != 1)
                    return 0;
                c = pixel.r;
                pixel.r = pixel.b;
                pixel.b = c;
                do {
                    *p++ = pixel;
                } while (count--);
            } else {
                n = count + 1;
                int failed = LpDataAccess->FRead(file, p, 3, n) != n;
                if (failed)
                    return 0;
                SwapRedBlue(p, count + 1);
                p += count + 1;
            }
        }
    }
    if (!(tga->descriptor & 0x20)) {
        top = (RGBTripple *)tga->pixels;
        bottom = top + tga->width * (tga->height - 1);
        if (tga->rowBuffer && tga->rowBufferSize < tga->width * sizeof(RGBTripple)) {
            free(tga->rowBuffer);
            tga->rowBufferSize = 0;
            tga->rowBuffer = 0;
        }
        if (!tga->rowBuffer) {
            if (!(tga->rowBuffer = malloc(tga->width * sizeof(RGBTripple))))
                return 0;
            tga->rowBufferSize = tga->width * sizeof(RGBTripple);
        }
        row = (RGBTripple *)tga->rowBuffer;
        for (i = 0; i < tga->height / 2; i++) {
            memcpy(row, top, tga->width * sizeof(RGBTripple));
            memcpy(top, bottom, tga->width * sizeof(RGBTripple));
            memcpy(bottom, row, tga->width * sizeof(RGBTripple));
            top += tga->width;
            bottom -= tga->width;
        }
        tga->descriptor |= 0x20;
    }
    return 1;
}


// ---- src/global/Read32BitTgaData.cpp


// Reads 32-bit pixel data, raw (type 2) or run-length encoded, swaps BGRA
// to RGBA and flips it to top-left origin.
int Read32BitTgaData(TgaFile *tga, long file)
{
    int numPixels;
    unsigned int size;
    RGBQuad *end;
    RGBQuad *p;
    int i;
    int count;
    unsigned char header;
    RGBQuad pixel;
    unsigned char c;
    RGBQuad *top;
    RGBQuad *bottom;
    RGBQuad *row;

    numPixels = tga->width * tga->height;
    size = numPixels * sizeof(RGBQuad);
    if (tga->pixelsSize < size) {
        if (tga->pixels)
            free(tga->pixels);
        tga->pixels = 0;
        tga->pixelsSize = 0;
    }
    if (!tga->pixels) {
        if (!(tga->pixels = (unsigned char *)malloc(size)))
            return 0;
        tga->pixelsSize = size;
    }
    p = (RGBQuad *)tga->pixels;
    end = p + numPixels;
    if (tga->imageType == 2) {
        if (LpDataAccess->FRead(file, p, 4, numPixels) != numPixels)
            return 0;
        for (i = 0; i < numPixels; i++) {
            c = p->r;
            p->r = p->b;
            p->b = c;
            p++;
        }
    } else {
        if (p < end) {
            do {
                LpDataAccess->FRead(file, &header, 1, 1);
                if (header & 0x80) {
                    count = header & 0x7F;
                    if (LpDataAccess->FRead(file, &pixel, 4, 1) != 1)
                        return 0;
                    c = pixel.r;
                    pixel.r = pixel.b;
                    pixel.b = c;
                    do {
                        *p++ = pixel;
                    } while (count--);
                } else {
                    count = header + 1;
                    do {
                        if (LpDataAccess->FRead(file, p, 4, 1) != 1)
                            return 0;
                        c = p->r;
                        p->r = p->b;
                        p->b = c;
                        p++;
                    } while (--count);
                }
            } while (p < end);
        }
    }
    if (!(tga->descriptor & 0x20)) {
        top = (RGBQuad *)tga->pixels;
        bottom = top + tga->width * (tga->height - 1);
        if (tga->rowBuffer && tga->rowBufferSize < tga->width * sizeof(RGBQuad)) {
            free(tga->rowBuffer);
            tga->rowBufferSize = 0;
            tga->rowBuffer = 0;
        }
        if (!tga->rowBuffer) {
            if (!(tga->rowBuffer = malloc(tga->width * sizeof(RGBQuad))))
                return 0;
            tga->rowBufferSize = tga->width * sizeof(RGBQuad);
        }
        row = (RGBQuad *)tga->rowBuffer;
        for (i = 0; i < tga->height / 2; i++) {
            memcpy(row, top, tga->width * sizeof(RGBQuad));
            memcpy(top, bottom, tga->width * sizeof(RGBQuad));
            memcpy(bottom, row, tga->width * sizeof(RGBQuad));
            top += tga->width;
            bottom -= tga->width;
        }
        tga->descriptor |= 0x20;
    }
    return 1;
}


// ---- src/global/Read16BitTgaData.cpp
// Unlike Read24/Read32BitTgaData, retail built this one without the global
// optimizer (the short row width is re-extended, the row loop is unrotated).


// Reads 16-bit pixel data, raw (type 2) or run-length encoded, then flips
// it to top-left origin.
int Read16BitTgaData(TgaFile *tga, long file)
{
#pragma global_optimizer off  // here, not above: the pragma before a definition also reaches the previous function (Read32BitTgaData)
    short width;
    unsigned short *p;
    int i;
    unsigned short *row;
    int numPixels;
    int n;
    unsigned short pixel;
    unsigned short *end;
    unsigned int size;
    unsigned short *top;
    unsigned short *bottom;
    int count;
    int j;
    unsigned char header;

    numPixels = tga->width * tga->height;
    size = numPixels * sizeof(unsigned short);
    if (tga->pixelsSize < size) {
        if (tga->pixels)
            free(tga->pixels);
        tga->pixels = 0;
        tga->pixelsSize = 0;
    }
    if (!tga->pixels) {
        if (!(tga->pixels = (unsigned char *)malloc(size)))
            return 0;
        tga->pixelsSize = size;
    }
    end = (unsigned short *)tga->pixels + numPixels;
    p = (unsigned short *)tga->pixels;
    if (tga->imageType == 2) {
        for (i = 0; i < tga->height; i++) {
            width = tga->width;
            if (LpDataAccess->FRead(file, p, 2, width) != width)
                return 0;
            p += width;
        }
    } else {
        if (p < end) {
            do {
                LpDataAccess->FRead(file, &header, 1, 1);
                count = header;
                if (count & 0x80) {
                    count &= 0x7F;
                    if (LpDataAccess->FRead(file, &pixel, 2, 1) != 1)
                        return 0;
                    do {
                        *p = pixel;
                        p++;
                    } while (count--);
                } else {
                    n = count + 1;
                    int failed = LpDataAccess->FRead(file, p, 2, n) != n;
                    if (failed)
                        return 0;
                    p += count + 1;  // CSE'd with n
                }
            } while (p < end);
        }
    }
    if (!(tga->descriptor & 0x20)) {
        top = (unsigned short *)tga->pixels;
        bottom = top + tga->width * (tga->height - 1);
        if (tga->rowBuffer && tga->rowBufferSize < tga->width * sizeof(unsigned short)) {
            free(tga->rowBuffer);
            tga->rowBufferSize = 0;
            tga->rowBuffer = 0;
        }
        if (!tga->rowBuffer) {
            if (!(tga->rowBuffer = malloc(tga->width * sizeof(unsigned short))))
                return 0;
            tga->rowBufferSize = tga->width * sizeof(unsigned short);
        }
        row = (unsigned short *)tga->rowBuffer;
        for (j = 0; j < tga->height / 2; j++) {
            memcpy(row, top, tga->width * sizeof(unsigned short));
            memcpy(top, bottom, tga->width * sizeof(unsigned short));
            memcpy(bottom, row, tga->width * sizeof(unsigned short));
            top += tga->width;
            bottom -= tga->width;
        }
        tga->descriptor |= 0x20;
    }
    return 1;
}


// ---- src/global/ReadTgaFileHeader.cpp


// Reads a .tga header field by field into buffer (or a new TgaFile);
// only uncompressed or RLE true-colour (16/24/32-bit) images are accepted.
TgaFile *ReadTgaFileHeader(long file, TgaFile *buffer)
{
#pragma global_optimizer reset
    TgaFile *tga;

    if (buffer) {
        tga = buffer;
    } else if (!(tga = (TgaFile *)malloc(sizeof(TgaFile)))) {
        goto fail;
    }
    if (!buffer) {
        tga->pixels = 0;
        tga->pixelsSize = 0;
        tga->rowBuffer = 0;
        tga->rowBufferSize = 0;
    }
    tga->name[0] = 0;
    if (LpDataAccess->FRead(file, &tga->idLength, 1, 1) != 1)
        goto fail;
    if (LpDataAccess->FRead(file, &tga->colorMapType, 1, 1) != 1)
        goto fail;
    if (LpDataAccess->FRead(file, &tga->imageType, 1, 1) != 1)
        goto fail;
    if (LpDataAccess->FRead(file, &tga->colorMapStart, 2, 1) != 1)
        goto fail;
    if (LpDataAccess->FRead(file, &tga->colorMapLength, 2, 1) != 1)
        goto fail;
    if (LpDataAccess->FRead(file, &tga->colorMapDepth, 1, 1) != 1)
        goto fail;
    if (LpDataAccess->FRead(file, &tga->xOrigin, 2, 1) != 1)
        goto fail;
    if (LpDataAccess->FRead(file, &tga->yOrigin, 2, 1) != 1)
        goto fail;
    if (LpDataAccess->FRead(file, &tga->width, 2, 1) != 1)
        goto fail;
    if (LpDataAccess->FRead(file, &tga->height, 2, 1) != 1)
        goto fail;
    if (LpDataAccess->FRead(file, &tga->bitsPerPixel, 1, 1) != 1)
        goto fail;
    if (LpDataAccess->FRead(file, &tga->descriptor, 1, 1) != 1)
        goto fail;
    if ((tga->bitsPerPixel == 16 || tga->bitsPerPixel == 24 || tga->bitsPerPixel == 32) &&
        (tga->imageType == 2 || tga->imageType == 10))
        return tga;

fail:
    if (tga) {
        if (tga->pixels)
            free(tga->pixels);
        free(tga);
    }
    return 0;
}


// Retail creates "rb" (@839) a function's worth of labels before
// ImportTgaFile's "" (@865): a function opening a file came here that nothing
// called, which retail's linker stripped ("Section
// order"). link_c leaves its section out.
static long StrippedOpen(char *filename)
{
    return LpDataAccess->FOpen(filename, "rb");
}


// ---- src/global/ImportTgaFile.cpp


TgaFile *ImportTgaFile(long file, TgaFile *buffer)
{
    TgaFile *tga;
    if (!(tga = ReadTgaFileHeader(file, buffer)))
        return 0;
    if (tga->bitsPerPixel == 24) {
        if (!Read24BitTgaData(tga, file))
            goto fail;
    } else if (tga->bitsPerPixel == 32) {
        if (!Read32BitTgaData(tga, file))
            goto fail;
    } else {
        if (!Read16BitTgaData(tga, file))
            goto fail;
    }
    strcpy(tga->name, "");
    return tga;

fail:
    if (tga) {
        if (tga->pixels)
            free(tga->pixels);
        free(tga);
    }
    return 0;
}


// ---- src/global/ImportTgaFile_0029C780.cpp


TgaFile *ImportTgaFile(char *filename, TgaFile *buffer)
{
    long file;
    TgaFile *tga;

    file = LpDataAccess->FOpen(filename, "rb");
    if (file == -1)
        return 0;
    if (!(tga = ReadTgaFileHeader(file, buffer))) {
        LpDataAccess->FClose(file);
        return 0;
    }
    if (tga->bitsPerPixel == 24) {
        if (!Read24BitTgaData(tga, file))
            goto fail;
    } else if (tga->bitsPerPixel == 32) {
        if (!Read32BitTgaData(tga, file))
            goto fail;
    } else {
        if (!Read16BitTgaData(tga, file))
            goto fail;
    }
    strcpy(tga->name, filename);
    LpDataAccess->FClose(file);
    return tga;

fail:
    if (tga) {
        if (tga->pixels)
            free(tga->pixels);
        free(tga);
    }
    LpDataAccess->FClose(file);
    return 0;
}


// ---- src/global/ExportImage24.cpp, ExportImage32.cpp, ExportImage16.cpp
// The Prepare*BitTgaForExport helpers are static in retail's TGA export file
// and defined before the ExportImage functions, 32-bit first (retail's text
// order). ExportImage24/32/16 keep their `name` argument live across the
// call, which mwcc only does when it can see the callee leaves that register
// alone, so the helpers are defined here.

static void Prepare32BitTgaForExport(TgaFile *tga, RGBQuad *pixels, int width, int height)
{
    tga->idLength = 0;
    tga->colorMapType = 0;
    tga->imageType = 2;
    tga->colorMapStart = 0;
    tga->colorMapLength = 0;
    tga->colorMapDepth = 0;
    tga->xOrigin = 0;
    tga->yOrigin = 0;
    tga->width = width;
    tga->height = height;
    tga->bitsPerPixel = 32;
    tga->descriptor = 0;
    tga->pixels = (unsigned char *)pixels;
}

static void Prepare24BitTgaForExport(TgaFile *tga, RGBTripple *pixels, int width, int height)
{
    tga->idLength = 0;
    tga->colorMapType = 0;
    tga->imageType = 2;
    tga->colorMapStart = 0;
    tga->colorMapLength = 0;
    tga->colorMapDepth = 0;
    tga->xOrigin = 0;
    tga->yOrigin = 0;
    tga->width = width;
    tga->height = height;
    tga->bitsPerPixel = 24;
    tga->descriptor = 0;
    tga->pixels = (unsigned char *)pixels;
}

static void Prepare16BitTgaForExport(TgaFile *tga, unsigned short *pixels, int width, int height)
{
    tga->idLength = 0;
    tga->colorMapType = 0;
    tga->imageType = 2;
    tga->colorMapStart = 0;
    tga->colorMapLength = 0;
    tga->colorMapDepth = 0;
    tga->xOrigin = 0;
    tga->yOrigin = 0;
    tga->width = width;
    tga->height = height;
    tga->bitsPerPixel = 16;
    tga->descriptor = 0;
    tga->pixels = (unsigned char *)pixels;
}

void ExportImage24(RGBTripple *pixels, int width, int height, int a, char *name, unsigned char c)
{
    TgaFile tga;
    Prepare24BitTgaForExport(&tga, pixels, width, height);
    strcpy(tga.name, name);
    ExportTgaFile(&tga, a, 0, c);
}

void ExportImage32(RGBQuad *pixels, int width, int height, int a, char *name, unsigned char c)
{
    TgaFile tga;
    Prepare32BitTgaForExport(&tga, pixels, width, height);
    strcpy(tga.name, name);
    ExportTgaFile(&tga, a, 0, c);
}

void ExportImage16(unsigned short *pixels, int width, int height, int a, int b, char *name, unsigned char c)
{
    TgaFile tga;
    Prepare16BitTgaForExport(&tga, pixels, width, height);
    strcpy(tga.name, name);
    ExportTgaFile(&tga, a, b, c);
}


// ---- src/global/ExportTgaFile.cpp
// ExportTgaFile__FP7TgaFileiiUc  @ 0x0029CB00
// Writes a TgaFile's header and pixels (24-bit rows BGR-swapped per pixel,
// 32-bit rows swapped in place, 16-bit rows raw or converted) to tga->name.
// `y` and `x` are function-scope so the 32-bit branch can reuse them (as the
// width and the row start index): a variable's later live range becomes a
// low-numbered temporary, which is what puts the width in $s4 and the row
// counter in $s5 after the hoisted `width * 4`.

#pragma opt_strength_reduction off


int ExportTgaFile(TgaFile *tga, int stride, int format, unsigned char descriptor)
{
    FILE_HANDLE_STRUCT *file;
    int y;
    int x;

    if (!(file = FOPEN(tga->name, "wb")))
        return 0;

    tga->descriptor = descriptor;
    tga->imageType = 2;
    if (FWRITE(&tga->idLength, 1, 1, file) != 1) goto fail;
    if (FWRITE(&tga->colorMapType, 1, 1, file) != 1) goto fail;
    if (FWRITE(&tga->imageType, 1, 1, file) != 1) goto fail;
    if (FWRITE(&tga->colorMapStart, 2, 1, file) != 1) goto fail;
    if (FWRITE(&tga->colorMapLength, 2, 1, file) != 1) goto fail;
    if (FWRITE(&tga->colorMapDepth, 1, 1, file) != 1) goto fail;
    if (FWRITE(&tga->xOrigin, 2, 1, file) != 1) goto fail;
    if (FWRITE(&tga->yOrigin, 2, 1, file) != 1) goto fail;
    if (FWRITE(&tga->width, 2, 1, file) != 1) goto fail;
    if (FWRITE(&tga->height, 2, 1, file) != 1) goto fail;
    if (FWRITE(&tga->bitsPerPixel, 1, 1, file) != 1) goto fail;
    if (FWRITE(&tga->descriptor, 1, 1, file) != 1) goto fail;

    if (tga->bitsPerPixel == 24) {
        unsigned char *p;
        int index, step;
        unsigned char rgb[3];
        unsigned char t;

        if (!stride)
            stride = tga->width * 3;
        y = 0;
        index = 0;
        step = stride / 3;
        for (; y < tga->height; y++) {
            p = index * 3 + tga->pixels;
            for (x = 0; x < tga->width; x++) {
                rgb[0] = p[0];
                rgb[1] = p[1];
                rgb[2] = p[2];
                t = rgb[0];
                rgb[0] = rgb[2];
                rgb[2] = t;
                if (FWRITE(rgb, 3, 1, file) != 1)
                    goto fail;
                p += 3;
            }
            index += step;
        }
    } else if (tga->bitsPerPixel == 32) {
        unsigned char *row;
        int total;
        unsigned char *p;
        int i;
        y = tga->width;             // reused: width
        total = y * tga->height;
        row = tga->pixels;

        for (x = 0; x < total; x += y) {    // reused: row start index
            i = 0;
            if (i < y) {
                p = row;
                do {
                    unsigned char r = p[0];
                    unsigned char b = p[2];
                    p[0] = b;
                    p[2] = r;
                    p += 4;
                } while (++i < y);
            }
            if (FWRITE(row, 4, y, file) != y)
                goto fail;
            row += y * 4;
        }
    } else {

        if (!stride)
            stride = tga->width * 2;
        if (format == 0x3E0) {
            int count, index, step, y;

            y = 0;
            step = stride / 2;
            for (index = 0; y < tga->height; y++) {
                count = tga->width;
                if (FWRITE(tga->pixels + index * 2, 2, count, file) != count)
                    return 0;
                index += step;
            }
        } else {
            unsigned short *p;
            int index, step, y, x;
            unsigned short v;
            short out;

            y = 0;
            step = stride / 2;
            for (index = 0; y < tga->height; y++) {
                p = (unsigned short *)(tga->pixels + index * 2);
                for (x = 0; x < tga->width; x++) {
                    v = *p;
                    out = (v & 0x1F) | (((v & 0xFFE0) >> 1) & 0x7FE0);
                    if (FWRITE(&out, 2, 1, file) != 1)
                        return 0;
                    p++;
                }
                index += step;
            }
        }
    }
    FCLOSE(file);
    return 1;

fail:
    if (file)
        FCLOSE(file);
    return 0;
}


#pragma opt_strength_reduction reset

// ---- src/global/FreeTgaFile.cpp
void FreeTgaFile(TgaFile *tga)
{
    if (tga) {
        if (tga->pixels)
            free(tga->pixels);
        if (tga->rowBuffer)
            free(tga->rowBuffer);
        free(tga);
    }
}
