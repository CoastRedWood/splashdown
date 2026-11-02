// PSXMpegVideoDecoder: the libmpeg decoder and its output frames (0x18
// bytes, PSXMediaControl::Create news it). The sceMpeg callbacks are
// static; their context is the PSXMediaControl.
#ifndef PSXMPEGVIDEODECODER_H
#define PSXMPEGVIDEODECODER_H

struct sceMpeg;
struct sceMpegCbData;
struct sceMpegCbDataStr;
struct sceMpegCbDataError;
class PSXMpegVideoOutputBuffer;
class PSXMediaControl;

class PSXMpegVideoDecoder {
public:
    PSXMpegVideoDecoder(int width, int height, PSXMediaControl *control);
    ~PSXMpegVideoDecoder();
    void Initialize();
    void Terminate();
    void Reset();
    int DecodeBitstream();
    static int VideoStreamCallback(sceMpeg *mp, sceMpegCbDataStr *data, void *context);
    static int AudioStreamCallback(sceMpeg *mp, sceMpegCbDataStr *data, void *context);
    static int MpegError(sceMpeg *mp, sceMpegCbDataError *data, void *context);
    static int MpegNodata(sceMpeg *mp, sceMpegCbData *data, void *context);
    static int MpegBackground(sceMpeg *mp, sceMpegCbData *data, void *context);

    unsigned char *m_workArea;          // +0x00 (memalign'd, width * height * 4.5 + 2 KB; unsigned char: Initialize's load order needs it)
    sceMpeg *m_mpeg;                    // +0x04
    PSXMpegVideoOutputBuffer *m_output; // +0x08
    int m_state;                        // +0x0C (PSXMpegDefs::VideoDecoderState_*: 1 aborted, 3 ended)
    int m_10;                           // +0x10
    PSXMediaControl *m_control;         // +0x14
};

#endif
