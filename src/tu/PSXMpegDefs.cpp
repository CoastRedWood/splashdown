// PSXMpegDefs.cpp  @ 0x0041FB78 (no text; data only)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// The movie player's shared constants as one translation unit: a data-only
// TU. Their .rodata (0x41FB78-0x41FB8C) follows PSXMediaControl.cpp's RTTI
// (the end of that TU) and precedes PSXMpegVideo.cpp's literals, and
// PSXMpegVideo.cpp loads them (initializers it could see would be folded),
// so they are taken to be defined in a file of their own. The file name is
// a guess.
#include "PSXMpegVideoDecoder/PSXMpegDefs.h"

const int PSXMpegDefs::VideoOutputBufferSize = 2;
const int PSXMpegDefs::VideoDecoderState_ABORT = 1;
const int PSXMpegDefs::VideoDecoderState_END = 3;
