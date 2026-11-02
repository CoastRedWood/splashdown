// Where the sound driver's mix goes (ports/pc/audio/driver.cpp mixes):
//
// - normal play: an SDL3 audio stream (48 kHz, 16-bit stereo) whose
//   callback mixes what the device asks for, on SDL's audio thread; with no
//   audio device, a thread of our own mixes in real time (so voices still
//   end and the music still moves on).
// - PORT_DETERMINISTIC: no device; the game thread mixes 800.8 frames at
//   each virtual vblank (ports/pc/hw.cpp), so the driver's state, and the RPC
//   replies that read it (IsPlaying), depend only on the game's commands.
// - PORT_AUDIO_WAV=path.wav records the mix (16-bit stereo 48 kHz; the
//   header is rewritten every second and at exit).
// - PORT_NO_AUDIO=1: no driver, no output (audio/audio.h).
#include "audio/audio.h"
#include "port.h"

#include <SDL3/SDL.h>

#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <mutex>
#include <thread>
#include <vector>

namespace {

FILE *g_wav;
uint64_t g_wavFrames;
uint64_t g_wavHeaderAt; // frames when the header was last written
std::mutex g_wavLock;
bool g_started;
uint64_t g_vblankFrames; // deterministic: frames mixed, in 1/1000ths

void WriteHeader()
{
    uint32_t data = (uint32_t)(g_wavFrames * 4);
    uint8_t h[44];
    auto u32 = [&](int at, uint32_t v) { memcpy(h + at, &v, 4); };
    auto u16 = [&](int at, uint16_t v) { memcpy(h + at, &v, 2); };
    memcpy(h, "RIFF", 4);
    u32(4, 36 + data);
    memcpy(h + 8, "WAVEfmt ", 8);
    u32(16, 16);
    u16(20, 1);
    u16(22, 2);
    u32(24, audio::kRate);
    u32(28, audio::kRate * 4);
    u16(32, 4);
    u16(34, 16);
    memcpy(h + 36, "data", 4);
    u32(40, data);
    long at = ftell(g_wav);
    fseek(g_wav, 0, SEEK_SET);
    fwrite(h, 1, sizeof(h), g_wav);
    fseek(g_wav, at, SEEK_SET);
    fflush(g_wav);
    g_wavHeaderAt = g_wavFrames;
}

// Mix frames and record them; returns the samples in buf.
void MixOut(std::vector<int16_t> &buf, int frames)
{
    buf.resize((size_t)frames * 2);
    audio::Mix(buf.data(), frames);
    if (g_wav) {
        std::lock_guard<std::mutex> lock(g_wavLock);
        fwrite(buf.data(), 4, frames, g_wav);
        g_wavFrames += frames;
        if (g_wavFrames - g_wavHeaderAt >= (uint64_t)audio::kRate)
            WriteHeader();
    }
}

void SDLCALL Callback(void *, SDL_AudioStream *stream, int additional, int)
{
    static std::vector<int16_t> buf;
    int frames = additional / 4;
    while (frames > 0) {
        int n = frames < 1024 ? frames : 1024;
        MixOut(buf, n);
        SDL_PutAudioStreamData(stream, buf.data(), n * 4);
        frames -= n;
    }
}

// No audio device: mix in real time anyway.
void MixThread()
{
    port_thread_name("audio-mix");
    std::vector<int16_t> buf;
    auto start = std::chrono::steady_clock::now();
    uint64_t done = 0;
    for (;;) {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        auto us = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start)
                      .count();
        uint64_t want = (uint64_t)us * audio::kRate / 1000000;
        while (done < want) {
            int n = want - done < 1024 ? (int)(want - done) : 1024;
            MixOut(buf, n);
            done += n;
        }
    }
}

} // namespace

void port_audio_start(void)
{
    if (audio::g_off) {
        port_log("audio: off (PORT_NO_AUDIO)");
        return;
    }
    g_started = true;
    if (const char *path = getenv("PORT_AUDIO_WAV")) {
        g_wav = fopen(path, "wb");
        if (!g_wav)
            port_log("audio: can't write %s", path);
        else
            WriteHeader();
    }
    if (port_deterministic()) {
        port_log("audio: deterministic, mixed at each vblank%s", g_wav ? " into the WAV" : "");
        return;
    }
    if (SDL_InitSubSystem(SDL_INIT_AUDIO)) {
        SDL_AudioSpec spec = {SDL_AUDIO_S16, 2, audio::kRate};
        SDL_AudioStream *stream =
            SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, Callback, nullptr);
        if (stream) {
            SDL_ResumeAudioStreamDevice(stream);
            port_log("audio: SDL %s, 48 kHz stereo", SDL_GetCurrentAudioDriver());
            return;
        }
        port_log("audio: no audio device (%s): mixing without output", SDL_GetError());
    } else {
        port_log("audio: SDL audio failed (%s): mixing without output", SDL_GetError());
    }
    std::thread(MixThread).detach();
}

void port_audio_vblank(void)
{
    if (!g_started)
        return;
    static std::vector<int16_t> buf;
    // 48000 / 59.94 = 800.8 frames a vblank
    uint64_t before = g_vblankFrames / 1000;
    g_vblankFrames += 800800;
    int frames = (int)(g_vblankFrames / 1000 - before);
    MixOut(buf, frames);
}

void port_audio_shutdown(void)
{
    if (!g_wav)
        return;
    std::lock_guard<std::mutex> lock(g_wavLock);
    WriteHeader();
    port_log("audio: %llu frames (%.1f s) written", (unsigned long long)g_wavFrames,
             g_wavFrames / (double)audio::kRate);
}
