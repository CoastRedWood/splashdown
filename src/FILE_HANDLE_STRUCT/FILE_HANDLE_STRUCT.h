// FILE_HANDLE_STRUCT: an open disc file (FOPEN/FREAD/FSEEK/...): libcdvd's
// file entry plus a read position; 0x28 bytes.
#ifndef FILE_HANDLE_STRUCT_FILE_HANDLE_STRUCT_H
#define FILE_HANDLE_STRUCT_FILE_HANDLE_STRUCT_H

// libcdvd's file entry (PSXMpegStreamFile.h includes it from here).
struct sceCdlFILE {
    unsigned int lsn;                   // +0x00
    unsigned int size;                  // +0x04
    char name[16];                      // +0x08
    unsigned char date[8];              // +0x18
};

struct FILE_HANDLE_STRUCT {
    FILE_HANDLE_STRUCT();
    ~FILE_HANDLE_STRUCT();

    sceCdlFILE file;                    // +0x00
    unsigned int position;              // +0x20
    unsigned int size;                  // +0x24
};

#endif
