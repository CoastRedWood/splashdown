// TgaFile: a Targa image being read or written (the header fields, then
// the pixel and row buffers). 0x128 bytes: ReadTgaFileHeader mallocs
// sizeof(TgaFile); the export helpers only use it on the stack.
#ifndef TGAFILE_H
#define TGAFILE_H

struct TgaFile {
    unsigned char idLength;      // +0x00
    unsigned char colorMapType;  // +0x01
    unsigned char imageType;     // +0x02
    short colorMapStart;         // +0x04
    short colorMapLength;        // +0x06
    unsigned char colorMapDepth; // +0x08
    short xOrigin;               // +0x0A
    short yOrigin;               // +0x0C
    short width;                 // +0x0E
    short height;                // +0x10
    unsigned char bitsPerPixel;  // +0x12
    unsigned char descriptor;    // +0x13
    unsigned char *pixels;       // +0x14
    unsigned int pixelsSize;     // +0x18
    char name[0x104];            // +0x1C
    void *rowBuffer;             // +0x120 (FreeTgaFile frees it)
    unsigned int rowBufferSize;  // +0x124
};

#endif
