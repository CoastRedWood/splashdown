// PSXMpegDefs: the movie player's shared constants (static members defined
// in retail's data, so they load from memory; src/tu/PSXMpegVideo.cpp
// defines them).
#ifndef PSXMPEGDEFS_H
#define PSXMPEGDEFS_H

class PSXMpegDefs {
public:
    static const int VideoOutputBufferSize;     // const: absolute load, not $gp
    static const int VideoDecoderState_ABORT;   // const: absolute load, not $gp
    static const int VideoDecoderState_END;     // const: absolute load, not $gp
};

#endif
