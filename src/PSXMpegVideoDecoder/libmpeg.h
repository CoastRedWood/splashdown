// libmpeg's (Sony's MPEG decoder library) types and calls, as the movie
// player uses them. sceMpeg is 0x48 bytes (PSXMpegVideoDecoder::Initialize
// news one).
#ifndef PSXMPEGVIDEODECODER_LIBMPEG_H
#define PSXMPEGVIDEODECODER_LIBMPEG_H

struct sceIpuRGB32;

struct sceMpeg {
    int width;                          // +0x00
    int height;                         // +0x04
    int frameCount;                     // +0x08
    long pts;                           // +0x10
    long dts;                           // +0x18
    unsigned long flags;                // +0x20
    long pts2nd;                        // +0x28
    long dts2nd;                        // +0x30
    unsigned long flags2nd;             // +0x38
    void *sys;                          // +0x40
};

struct sceMpegCbData {
    int type;                           // +0x00
};

struct sceMpegCbDataStr {
    int type;                           // +0x00
    unsigned char *header;              // +0x04
    unsigned char *data;                // +0x08
    unsigned int len;                   // +0x0C
};

struct sceMpegCbDataError {
    int type;                           // +0x00
    char *errMessage;                   // +0x04
};

typedef int (*sceMpegCallback)(sceMpeg *mp, sceMpegCbData *cbdata, void *anyData);
typedef int (*sceMpegStrCallback)(sceMpeg *mp, sceMpegCbDataStr *cbdata, void *anyData);

extern "C" int sceMpegInit(void);
extern "C" int sceMpegCreate(sceMpeg *mp, unsigned char *work_area, int work_area_size);
extern "C" int sceMpegDelete(sceMpeg *mp);
extern "C" int sceMpegReset(sceMpeg *mp);
extern "C" int sceMpegIsEnd(sceMpeg *mp);
extern "C" int sceMpegGetPicture(sceMpeg *mp, sceIpuRGB32 *rgb32, int mbcount);
extern "C" int sceMpegDemuxPss(sceMpeg *mp, unsigned char *data, int size);
extern "C" int sceMpegAddCallback(sceMpeg *mp, int type, sceMpegCallback callback, void *anyData);
extern "C" int sceMpegAddStrCallback(sceMpeg *mp, int strType, int ch, sceMpegStrCallback callback, void *anyData);

#endif
