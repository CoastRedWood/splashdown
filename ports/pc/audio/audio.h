// The IOP sound driver (PS2SOUND.IRX) on the PC: ports/pc/audio/driver.cpp
// plays its commands on an SPU2-like mixer, ports/pc/audio/output.cpp sends the
// mix to SDL audio and PORT_AUDIO_WAV, ports/pc/sdk/sound_rpc.cpp decodes the
// SIF RPC calls into these.
#ifndef PORT_AUDIO_AUDIO_H
#define PORT_AUDIO_AUDIO_H

#include <cstdint>

namespace audio {

const int kRate = 48000;

// The normal client (0x34567): command `func` with the EE's argument words;
// the reply word (recv[0]).
int Command(unsigned func, const int32_t *args);
// One batch command (clients 0x12346/0x23456: RainbowBatchData).
void BatchCommand(int cmd, const int32_t *args);

// SIF DMA from the EE into IOP memory at addr (the movie sound's buffer;
// anything else is dropped).
void IopWrite(uint32_t addr, const void *data, uint32_t bytes);

// Mix `frames` stereo frames (interleaved L R) into out, advancing the
// voices. Takes the driver's lock.
void Mix(int16_t *out, int frames);

// PORT_AUDIO_LOG=1: log every command.
extern bool g_log;
// PORT_NO_AUDIO=1: no driver; the commands get the replies the stage-3
// stub gave (version 0x207, everything else 0: no durations, nothing
// playing), which the golden reference was recorded with.
extern bool g_off;

} // namespace audio

#endif
