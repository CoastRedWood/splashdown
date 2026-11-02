// PSXMpegInputBuffer: reads the movie off the disc in 16 KB chunks, demuxes
// them with libmpeg, and keeps the video ring buffer that feeds the IPU
// (0x24 bytes, PSXMediaControl::Create news it).
#ifndef PSXMPEGINPUTBUFFER_H
#define PSXMPEGINPUTBUFFER_H

struct sceMpeg;
struct sceMpegCbData;
struct sceMpegCbDataStr;
class PSXMpegStreamFile;
class PSXMpegVideoDecoder;
class PSXMpegAudioDecoder;

class PSXMpegInputBuffer {
public:
    PSXMpegInputBuffer(PSXMpegStreamFile *file, PSXMpegVideoDecoder *video, PSXMpegAudioDecoder *audio);
    ~PSXMpegInputBuffer();
    void Initialize();
    void Reset();
    int PreBuffer();
    int FillInputBuffer(sceMpeg *mp, int wait);
    int DemuxVideoData(sceMpeg *mp, sceMpegCbDataStr *data, void *context);
    int DemuxAudioData(sceMpeg *mp, sceMpegCbDataStr *data, void *context);
    int BackgroundProcessing(sceMpeg *mp, sceMpegCbData *data, void *context);
    int NoDataAvailable(sceMpeg *mp, sceMpegCbData *data, void *context);

    unsigned char *m_readBuffer;        // +0x00 (0x4000 bytes read from the disc)
    char *m_buffer;                     // +0x04 (video ring buffer, 0x60000 bytes)
    char *m_writePtr;                   // +0x08
    char *m_readPtr;                    // +0x0C
    char *m_10;                         // +0x10 (end of the data sent to the IPU)
    int m_bytesLeft;                    // +0x14 (unconsumed bytes at the end of m_readBuffer)
    PSXMpegStreamFile *m_file;          // +0x18
    PSXMpegVideoDecoder *m_video;       // +0x1C
    PSXMpegAudioDecoder *m_audioDecoder;    // +0x20
};

#endif
