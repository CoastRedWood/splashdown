// PSXMpegStreamFile: the movie file streamed off the disc with sceCdSt*
// (0x12C bytes, PSXMediaControl::Create news it).
#ifndef PSXMPEGSTREAMFILE_H
#define PSXMPEGSTREAMFILE_H

#include "FILE_HANDLE_STRUCT/FILE_HANDLE_STRUCT.h"   // libcdvd's sceCdlFILE

class PSXMpegStreamFile {
public:
    PSXMpegStreamFile();
    int Open(char *name);
    int Close();
    int Reset();
    int Read(void *buffer, int size);

    char m_name[0x100];                 // +0x000
    int m_file;                         // +0x100 (-1)
    int m_x104;                         // +0x104 (FOPEN handle's +0x24)
    sceCdlFILE m_cdFile;                // +0x108
    void *m_iopBuffer;                  // +0x128 (sceSifAllocIopHeap)
};

#endif
