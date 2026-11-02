// The IOP sound driver, PS2SOUND.IRX ("PS2Sound\IOP\OptomizedDriver",
// sound library 2.07), on the PC. The disc's IRX has its symbols and its
// MW debug section (function, struct and field names); the commands below
// follow its disassembly, played on
// an SPU2-like mixer instead of libsd:
//
// - 48 voices (2 cores x 24), PS-ADPCM decoded block by block as the SPU
//   does (loop start/end/repeat flags), pitch 0x1000 = 48 kHz with 4-point
//   interpolation, linear volumes 0..0x3FFF per side (the driver's
//   percent * 0x3FFF / 100, times the group volumes), a short attack and
//   release instead of ADSR (the driver sets ADSR1 0x9EEE: fast attack).
// - Sounds (LoadSoundCD) are read from the disc image by the IOP, by LSN,
//   VAG header and all, as the driver's sceCdRead did; a looping load sets
//   the loop flags of the first and last blocks as LoadCDFile does.
// - Streams (LoadStreamCD + PlaySound with Stream=1) read the disc as they
//   play: a mono stream is a VAG (data after the 0x40-byte header), a stereo
//   one interleaves L and R by 2048-byte sector after a header sector (the
//   songs). No SPU double buffer: blocks come straight from the file.
// - Volume ramps (VolVelocity: the fade time in ms, as VolumeVoice turns it
//   into units per second), IsFading voices stop at 0; pitch ramps the same.
//
// Everything is under one lock: the game thread's commands, the mixer
// (SDL's audio thread, the fallback thread, or the game thread at each
// virtual vblank in PORT_DETERMINISTIC mode: ports/pc/audio/output.cpp).
#include "audio/audio.h"
#include "port.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <mutex>
#include <vector>

namespace audio {

bool g_log = getenv("PORT_AUDIO_LOG") != nullptr;
bool g_off = getenv("PORT_NO_AUDIO") && strcmp(getenv("PORT_NO_AUDIO"), "0");

namespace {

// PORT_AUDIO_MUTE=g,g,...: leave the voices of those groups (or `streams`,
// every stream) out of the mix (analysis: an engine without the music).
struct Mute {
    uint32_t groups = 0;
    bool streams = false;
    bool movie = false;        // =movie: the movie sound
    Mute()
    {
        const char *v = getenv("PORT_AUDIO_MUTE");
        for (const char *p = v; p && *p;) {
            if (!strncmp(p, "streams", 7))
                streams = true;
            else if (!strncmp(p, "movie", 5))
                movie = true;
            else if (*p >= '0' && *p <= '9')
                groups |= 1u << (atoi(p) & 31);
            p = strchr(p, ',');
            if (p)
                p++;
        }
    }
    // PORT_AUDIO_SOLO=n: only the voices playing sound slot n; =vN: only
    // voice N (core * 24 + voice); =streams: only streams; =movie: only the
    // movie sound
    const char *soloEnv = getenv("PORT_AUDIO_SOLO");
    int solo = soloEnv && *soloEnv != 'v' && *soloEnv != 's' ? atoi(soloEnv) : -1;
    bool soloStreams = soloEnv && *soloEnv == 's'; // =streams
    bool soloMovie = soloEnv && *soloEnv == 'm';   // =movie
    bool MovieMuted() const
    {
        return movie || (soloEnv && *soloEnv && !soloMovie);
    }
    int soloVoice = soloEnv && *soloEnv == 'v' ? atoi(soloEnv + 1) : -1;
    bool operator()(bool isStream, int group, int index, int voice) const
    {
        if (soloMovie)
            return true;
        if (soloVoice >= 0)
            return voice != soloVoice;
        if (soloStreams)
            return !isStream;
        if (solo >= 0)
            return isStream || index != solo;
        return (isStream && streams) || (group >= 0 && group < 32 && (groups >> group & 1));
    }
} g_mute;

const int kVoices = 48;    // 2 cores x 24
const int kSounds = 256;   // the EE's sound slots (MAX_SOUNDS 200, from 1)
const int kStreams = 16;   // RainbowPS2Snd::Init's stream count is smaller
const int kGroups = 32;    // gGroupVolume: 0 = sounds, 1 = streams, 2-24 groups
const int kSector = 2048;

std::mutex g_lock;

struct Sound {
    bool loaded = false;
    std::vector<uint8_t> data; // ADPCM, after the VAG header
    int group = 0;
    int duration = 0;          // ms
    int pitch = 0x1000;
};

struct Stream {
    bool loaded = false;
    bool stereo = false;       // one file, L and R sectors interleaved
    int channel = 0;           // 0 = L, 1 = R (stereo)
    bool loop = false;
    int group = 1;
    int pitch = 0x1000;
    int dryL = 100, dryR = 100;
    uint64_t base = 0;         // the file's byte offset in the image
    uint32_t bytes = 0;        // the file's size
    uint32_t chanBytes = 0;    // this channel's ADPCM bytes
    // A cache of this channel's data: kCache bytes from cacheAt
    static const uint32_t kCache = 16 * kSector;
    uint8_t cache[kCache];
    uint32_t cacheAt = ~0u;
};

struct Voice {
    bool active = false;
    int seq = 0;
    bool isStream = false;
    int index = 0;             // sound or stream
    int other = -1;            // the stereo pair's voice
    int group = 0;
    // ADPCM
    uint32_t addr = 0;         // the current block (byte offset)
    uint32_t loopAddr = 0;
    uint8_t flags = 0;         // the current block's flags
    int pos = 28;              // next sample in dec
    int16_t dec[28] = {};
    int32_t h1 = 0, h2 = 0;
    int16_t hist[4] = {};      // the last 4 samples, newest last
    uint32_t counter = 0;      // 12-bit fraction, as the SPU's pitch counter
    bool ended = false;        // the data ran out: release
    // pitch
    int pitch = 0x1000;
    int pitchTarget = 0x1000;
    float pitchVel = 0;        // per sample, 0 = jump
    bool paused = false;
    // volume, 0..1 = 0..0x3FFF
    float volL = 0, volR = 0, tgtL = 0, tgtR = 0;
    float velL = 0, velR = 0;  // per sample, 0 = jump (smoothed)
    bool fading = false;
    // envelope
    int env = 0;               // 0 attack, 1 sustain, 2 release
    float level = 0;
};

Sound g_sounds[kSounds];
Stream g_streams[kStreams];
Voice g_voices[kVoices];
int g_groupVolume[kGroups];
int g_maxStreams;
uint64_t g_mixed; // frames mixed so far (log time stamps)

// The movie sound (InitMovieSound .. GetMovieStatus, 0x1A-0x1F). The
// driver hands the EE a 0x6000-byte IOP buffer (GetMovieSoundBuffer: the
// stream's gStreamBuffers slot), which StartMovieSound plays in a loop by
// SPU2 core 0's sound data input (sceSdBlockTrans(0, WRITE | LOOP, buf,
// 0x6000), BVOLL/R 0x3FFF): 16-bit PCM, 48 kHz, 512 bytes of L then 512 of
// R per 1 KB block (the PSS header's "interleave size 512"). The EE
// (PSXMpegAudioDecoder::SendToIOP, ezmpeg's audioDec) writes the free part
// behind the play position with SIF DMA (ports/pc/sdk/stubs.cpp ->
// audio::IopWrite) and asks for the position with GetMovieStatus.
const uint32_t kMovieBase = 0x100000;  // stream n's buffer: kMovieBase + n * kMovieBytes
const uint32_t kMovieBytes = 0x6000;
const uint32_t kMovieFrames = kMovieBytes / 4;
struct MovieSound {
    uint8_t ring[kMovieBytes] = {};
    uint32_t addr = kMovieBase;        // the buffer's IOP address
    bool playing = false;
    bool paused = false;
    uint32_t frame = 0;                // play position, stereo frames
    uint64_t played = 0;               // frames played since Start (log)
    bool fresh[kMovieBytes / 1024] = {};  // written since last played
    int underruns = 0;                 // blocks played stale
} g_movie;

const float kAttack = 1.0f / (0.002f * kRate);   // 2 ms
const float kRelease = 1.0f / (0.008f * kRate);  // 8 ms
const float kVolStep = 1.0f / (0.010f * kRate);  // an immediate change, over 10 ms

const int kFilter[5][2] = {{0, 0}, {60, 0}, {115, -52}, {98, -55}, {122, -60}};

void DecodeBlock(const uint8_t *b, int16_t *out, int32_t &h1, int32_t &h2)
{
    int shift = b[0] & 15, filter = b[0] >> 4;
    if (filter > 4)
        filter = 0;
    if (shift > 12)
        shift = 9; // as the SPU does
    int f0 = kFilter[filter][0], f1 = kFilter[filter][1];
    for (int i = 0; i < 28; i++) {
        int n = (b[2 + i / 2] >> ((i & 1) * 4)) & 15;
        int32_t s = (int32_t)((int16_t)(n << 12)) >> shift;
        s += (h1 * f0 + h2 * f1 + 32) >> 6;
        s = std::clamp(s, -32768, 32767);
        out[i] = (int16_t)s;
        h2 = h1;
        h1 = s;
    }
}

int Clamp100(int v)
{
    return std::clamp(v, 0, 100);
}

// DecodeVoiceInst: core in bit 0, voice in bits 1-5, sequence above.
int VoiceIndex(int inst)
{
    return (inst & 1) * 24 + ((inst >> 1) & 31) % 24;
}

int VoiceSeq(int inst)
{
    return (int16_t)inst >> 6;
}

// The voices a handle names (L in the top half, its pair if it has one),
// if L's sequence matches.
int HandleVoices(int32_t handle, int out[2])
{
    int inst = handle >> 16;
    Voice &v = g_voices[VoiceIndex(inst)];
    if (v.seq != VoiceSeq(inst))
        return 0;
    out[0] = VoiceIndex(inst);
    if (v.other >= 0) {
        out[1] = v.other;
        return 2;
    }
    return 1;
}

// Stream data: this channel's 16 bytes at off (block-aligned), or nullptr
// past the end.
const uint8_t *StreamBlock(Stream &s, uint32_t off)
{
    if (off + 16 > s.chanBytes)
        return nullptr;
    if (s.cacheAt == ~0u || off < s.cacheAt || off + 16 > s.cacheAt + Stream::kCache) {
        uint32_t at = off & ~(uint32_t)(kSector - 1);
        if (!s.stereo) {
            port_disc_read(s.base + 0x40 + at, s.cache, Stream::kCache);
        } else {
            // sector k of channel c is file sector 1 + 2k + c
            for (uint32_t k = 0; k < Stream::kCache / kSector; k++) {
                uint32_t sector = at / kSector + k;
                port_disc_read(s.base + (uint64_t)(1 + 2 * sector + s.channel) * kSector, s.cache + k * kSector,
                               kSector);
            }
        }
        s.cacheAt = at;
    }
    return s.cache + (off - s.cacheAt);
}

// The block at v.addr, or nullptr if the data ends there.
const uint8_t *VoiceBlock(Voice &v)
{
    if (v.isStream)
        return StreamBlock(g_streams[v.index], v.addr);
    Sound &s = g_sounds[v.index];
    if (!s.loaded || v.addr + 16 > s.data.size())
        return nullptr;
    return s.data.data() + v.addr;
}

void StartBlock(Voice &v)
{
    const uint8_t *b = VoiceBlock(v);
    if (!b) {
        v.ended = true;
        v.flags = 0;
        memset(v.dec, 0, sizeof(v.dec));
        v.pos = 0;
        return;
    }
    v.flags = b[1];
    if ((v.flags & 4) && !v.isStream)
        v.loopAddr = v.addr;
    DecodeBlock(b, v.dec, v.h1, v.h2);
    v.pos = 0;
}

// The voice's next sample at its own rate, the SPU's way: a block's flags
// act once its 28 samples have played (end: jump to the loop address if
// repeat is set, else the voice ends).
int16_t NextSample(Voice &v)
{
    if (v.ended)
        return 0;
    if (v.pos >= 28) {
        uint8_t f = v.flags;
        if (v.isStream) {
            // streams end where the data does or at an end flag without
            // repeat (each song channel's last block), and restart if looped
            if ((f & 3) == 1) {
                v.addr = g_streams[v.index].chanBytes; // past the end
            } else {
                v.addr += 16;
            }
            if (v.addr + 16 > g_streams[v.index].chanBytes && g_streams[v.index].loop)
                v.addr = 0;
        } else if (f & 1) {
            if (f & 2) {
                v.addr = v.loopAddr;
            } else {
                v.ended = true;
                return 0;
            }
        } else {
            v.addr += 16;
        }
        StartBlock(v);
        if (v.ended)
            return 0;
    }
    return v.dec[v.pos++];
}

void KeyOff(Voice &v)
{
    if (v.active && v.env != 2)
        v.env = 2;
}

void StopNow(Voice &v)
{
    v.active = false;
    v.level = 0;
}

float GroupFactor(const Voice &v)
{
    float f = g_groupVolume[v.isStream ? 1 : 0] / 100.0f;
    if (v.group >= 2 && v.group < kGroups)
        f *= g_groupVolume[v.group] / 100.0f;
    return f;
}

// ---- commands

int Init(const int32_t *a)
{
    g_maxStreams = a[0];
    port_log("audio: sound driver up, %d streams", g_maxStreams);
    return 0x207; // sound library 2.07: RainbowPS2Snd::Init checks it
}

// LoadCDFile: read the VAG, take its rate and size, keep the ADPCM.
int LoadCDFile(int index, uint32_t lsn, uint32_t bytes, int group, int looping)
{
    if (index <= 0 || index >= kSounds || bytes < 0x50)
        return -114;
    Sound &s = g_sounds[index];
    std::vector<uint8_t> file(bytes);
    port_disc_read((uint64_t)lsn * kSector, file.data(), bytes);
    uint32_t size = (uint32_t)file[12] << 24 | file[13] << 16 | file[14] << 8 | file[15];
    uint32_t rate = (uint32_t)file[16] << 24 | file[17] << 16 | file[18] << 8 | file[19];
    s.data.assign(file.begin() + 0x40, file.end());
    s.data.resize(s.data.size() & ~15u);
    if (looping && s.data.size() >= 16) {
        s.data[1] |= 6;
        s.data[s.data.size() - 15] |= 3;
    }
    s.group = group;
    // the driver's integer formulas (Duration in ms)
    uint32_t blocks = rate / 28 * 16;
    s.duration = blocks ? (int)((uint32_t)(size * 1000u) / blocks) : 0;
    s.pitch = (int)((rate << 12) / 48000);
    s.loaded = true;
    for (Voice &v : g_voices) // a reload under a playing voice restarts it cleanly
        if (v.active && !v.isStream && v.index == index)
            StopNow(v);
    return 0;
}

// _PS2CDLoadRequest: SoundIndex (sound << 16 | sound2), Stream (byte 4),
// Group, Stereo (byte 12), Looping (byte 13), StartSector[2], EndSector[2],
// NumSectors[2], NumBytes[2]. The second file loads only for Stereo == 'S',
// which the EE never sends (it sends 1): stereo sounds play their L file.
int LoadSoundCD(const int32_t *a)
{
    const uint8_t *b = (const uint8_t *)a;
    uint32_t handle = (uint32_t)a[0];
    if (b[4] == 1)
        return 0;
    LoadCDFile(handle >> 16, a[4], a[10], a[2], b[13]);
    if ((int8_t)b[12] == 'S')
        LoadCDFile(handle & 0xFFFF, a[5], a[11], a[2], b[13]);
    return 0; // the driver always answers 0
}

// _PS2SoundRequest: Sound, Stream, Stereo, DataL, DataR.
int SoundDuration(const int32_t *a)
{
    int i = (uint32_t)a[0] >> 16;
    return i < kSounds ? g_sounds[i].duration : 0;
}

int SoundPitch(const int32_t *a)
{
    int i = (uint32_t)a[0] >> 16;
    return i < kSounds ? g_sounds[i].pitch : 0;
}

void LoadStreamOne(int index, int channel, const int32_t *a, bool stereo)
{
    if (index < 0 || index >= kStreams)
        return;
    Stream &s = g_streams[index];
    const uint8_t *b = (const uint8_t *)a;
    s.loaded = true;
    s.stereo = stereo;
    s.channel = channel;
    s.loop = b[13] != 0;
    s.group = a[2];
    s.base = (uint64_t)(uint32_t)a[4] * kSector;
    s.bytes = (uint32_t)a[10];
    s.cacheAt = ~0u;
    s.dryL = s.dryR = 100;
    uint8_t h[0x40];
    port_disc_read(s.base, h, sizeof(h));
    uint32_t rate = (uint32_t)h[16] << 24 | h[17] << 16 | h[18] << 8 | h[19];
    s.pitch = rate ? (int)((rate << 12) / 48000) : 0x1000;
    if (stereo)
        s.chanBytes = s.bytes > kSector ? (s.bytes - kSector) / 2 / kSector * kSector : 0;
    else
        s.chanBytes = s.bytes > 0x40 ? (s.bytes - 0x40) & ~15u : 0;
}

// LoadStreamCD: the same request; a stereo stream is one interleaved file
// (StartSector[0] for both halves).
void LoadStreamCD(const int32_t *a)
{
    const uint8_t *b = (const uint8_t *)a;
    uint32_t handle = (uint32_t)a[0];
    if (b[12]) {
        LoadStreamOne(handle >> 16, 0, a, true);
        LoadStreamOne(handle & 0xFFFF, 1, a, true);
    } else {
        LoadStreamOne(handle >> 16, 0, a, false);
    }
}

void StartVoice(int vi, int seq, bool isStream, int index, int group, float l, float r, int pitch)
{
    Voice &v = g_voices[vi];
    v = Voice();
    v.active = true;
    v.seq = seq;
    v.isStream = isStream;
    v.index = index;
    v.group = group;
    v.volL = v.tgtL = std::clamp(l, 0.0f, 1.0f);
    v.volR = v.tgtR = std::clamp(r, 0.0f, 1.0f);
    v.pitch = v.pitchTarget = std::clamp(pitch, 0, 0x3FFF);
    v.addr = 0;
    v.loopAddr = 0;
    v.env = 0;
    v.level = 0;
    StartBlock(v);
    if (v.ended) // nothing to play (a stereo sound's R half: never loaded)
        v.active = false;
}

// _PS2PlayRequest: Sound, Stream, VoiceL, VoiceR, VolumeL, VolumeR, Pitch
// (-1: the sound's own volume 100 and pitch).
void PlaySound(const int32_t *a)
{
    uint32_t handle = (uint32_t)a[0];
    bool isStream = a[1] == 1;
    int instL = a[2], instR = a[3];
    int volL = a[4], volR = a[5], pitch = a[6];
    if (!isStream) {
        int i = handle >> 16, i2 = handle & 0xFFFF;
        if (i >= kSounds)
            return;
        bool stereo = i2 != 0; // sound slots start at 1
        Sound &s = g_sounds[i];
        // InitSoundForPlaying: -1 keeps 100; the sound's Stereo byte is
        // never set, so both voices of a pair play both sides
        float l = (volL == -1 ? 100 : volL) / 100.0f, r = (volR == -1 ? 100 : volR) / 100.0f;
        int p = pitch == -1 ? s.pitch : pitch;
        int vl = VoiceIndex(instL);
        StartVoice(vl, VoiceSeq(instL), false, i, s.group, l, r, p);
        if (stereo && i2 < kSounds) {
            int vr = VoiceIndex(instR);
            Sound &s2 = g_sounds[i2];
            StartVoice(vr, VoiceSeq(instR), false, i2, s2.group, l, r, pitch == -1 ? s2.pitch : pitch);
            g_voices[vl].other = vr;
            g_voices[vr].other = vl;
        }
    } else {
        int i = handle >> 16, i2 = handle & 0xFFFF;
        if (i >= kStreams || !g_streams[i].loaded)
            return;
        Stream &s = g_streams[i];
        // DryL/DryR take the volumes (unsigned <= 100, so -1 is 100); a
        // stereo pair's L stream is silent on the right and R on the left
        unsigned ul = (unsigned)volL, ur = (unsigned)volR;
        s.dryL = ul > 100 ? 100 : ul;
        s.dryR = ur > 100 ? 100 : ur;
        int p = pitch == -1 ? s.pitch : pitch;
        int vl = VoiceIndex(instL);
        if (s.stereo && i2 < kStreams) {
            Stream &s2 = g_streams[i2];
            s2.dryL = 0;
            s2.dryR = s.dryR;
            s.dryR = 0;
            int vr = VoiceIndex(instR);
            StartVoice(vl, VoiceSeq(instL), true, i, s.group, s.dryL / 100.0f, 0, p);
            StartVoice(vr, VoiceSeq(instR), true, i2, s2.group, 0, s2.dryR / 100.0f, pitch == -1 ? s2.pitch : pitch);
            g_voices[vl].other = vr;
            g_voices[vr].other = vl;
        } else {
            StartVoice(vl, VoiceSeq(instL), true, i, s.group, s.dryL / 100.0f, s.dryR / 100.0f, p);
        }
    }
}

// StopVoice (PrivateStopVoice): key off the voice and its pair.
void StopVoice(const int32_t *a)
{
    int v[2];
    int n = HandleVoices(a[0], v);
    for (int i = 0; i < n; i++)
        KeyOff(g_voices[v[i]]);
}

// PauseVoice: toggles (pitch 0 and back).
void PauseVoice(const int32_t *a)
{
    int v[2];
    int n = HandleVoices(a[0], v);
    for (int i = 0; i < n; i++)
        if (g_voices[v[i]].active)
            g_voices[v[i]].paused = !g_voices[v[i]].paused;
}

void StopAllVoices()
{
    for (Voice &v : g_voices)
        KeyOff(v);
}

void FadeOutAllVoices()
{
    for (Voice &v : g_voices) {
        if (!v.active)
            continue;
        v.velL = v.volL / (0.5f * kRate);
        v.velR = v.volR / (0.5f * kRate);
        v.tgtL = v.tgtR = 0;
        v.fading = true;
    }
}

// Set a voice's target volumes (percent), keeping a stereo stream's sides.
void SetTarget(Voice &v, float l, float r)
{
    if (v.isStream && g_streams[v.index].stereo) {
        if (g_streams[v.index].channel == 0)
            r = 0;
        else
            l = 0;
    }
    v.tgtL = std::clamp(l, 0.0f, 1.0f);
    v.tgtR = std::clamp(r, 0.0f, 1.0f);
}

// _PS2VoiceRequest: VoiceInst, RelativeVol, VolVelocity (ms), VolL, VolR,
// Dolby, RelativePitch, PitchVelocity (ms), Pitch.
void VolumeVoice(const int32_t *a)
{
    int v[2];
    int n = HandleVoices(a[0], v);
    for (int i = 0; i < n; i++) {
        Voice &x = g_voices[v[i]];
        float l = Clamp100(a[3]) / 100.0f, r = Clamp100(a[4]) / 100.0f;
        if (a[1]) { // relative: a percentage of the current volume
            l *= x.volL;
            r *= x.volR;
        }
        SetTarget(x, l, r);
        if (a[2] > 0) {
            float secs = a[2] / 1000.0f;
            x.velL = std::fabs(x.volL - x.tgtL) / (secs * kRate);
            x.velR = std::fabs(x.volR - x.tgtR) / (secs * kRate);
            if (a[3] == 0)
                x.fading = true; // stops at 0
        } else {
            x.velL = x.velR = 0;
        }
    }
}

void SetPitch(Voice &x, int pitch, int velocityMs)
{
    pitch = std::clamp(pitch, 0, 0x3FFF);
    x.pitchTarget = pitch;
    if (velocityMs > 0) {
        x.pitchVel = std::abs(pitch - x.pitch) / (velocityMs / 1000.0f * kRate);
    } else {
        x.pitchVel = 0;
        x.pitch = pitch;
    }
}

void PitchVoice(const int32_t *a)
{
    int v[2];
    int n = HandleVoices(a[0], v);
    for (int i = 0; i < n; i++)
        if (a[8] >= 0)
            SetPitch(g_voices[v[i]], a[8], a[7]);
}

// PanVoice: VolL/VolR are the sides' percentages (absolute).
void PanVoice(const int32_t *a)
{
    int v[2];
    int n = HandleVoices(a[0], v);
    for (int i = 0; i < n; i++) {
        Voice &x = g_voices[v[i]];
        SetTarget(x, Clamp100(a[3]) / 100.0f, Clamp100(a[4]) / 100.0f);
        x.velL = x.velR = 0;
    }
}

// SettingsVoice: volumes and pitch at once, -1 keeps one.
void SettingsVoice(const int32_t *a)
{
    int v[2];
    int n = HandleVoices(a[0], v);
    for (int i = 0; i < n; i++) {
        Voice &x = g_voices[v[i]];
        if (a[3] != -1 || a[4] != -1) {
            float l = a[3] == -1 ? x.tgtL : Clamp100(a[3]) / 100.0f;
            float r = a[4] == -1 ? x.tgtR : Clamp100(a[4]) / 100.0f;
            if (a[1]) {
                l *= x.volL;
                r *= x.volR;
            }
            SetTarget(x, l, r);
            if (a[2] > 0) {
                float secs = a[2] / 1000.0f;
                x.velL = std::fabs(x.volL - x.tgtL) / (secs * kRate);
                x.velR = std::fabs(x.volR - x.tgtR) / (secs * kRate);
            } else {
                x.velL = x.velR = 0;
            }
        }
        if (a[8] != -1)
            SetPitch(x, a[8], a[7]);
    }
}

// _PS2GroupRequest: Group, Relative, Data.
void VolumeGroup(const int32_t *a)
{
    int g = a[0];
    if (g < 0 || g >= kGroups)
        return;
    g_groupVolume[g] = a[1] ? g_groupVolume[g] * Clamp100(a[2]) / 100 : Clamp100(a[2]);
}

// PauseGroup/StopGroup: the active voices whose sound or stream is in the
// group (by its GroupId); pause toggles.
void PauseGroup(const int32_t *a)
{
    for (Voice &v : g_voices)
        if (v.active && v.group == a[0])
            v.paused = !v.paused;
}

void StopGroup(const int32_t *a)
{
    for (Voice &v : g_voices)
        if (v.active && v.group == a[0])
            KeyOff(v);
}

// IsPlaying (_PS2SoundRequest-shaped: the handle, Stereo at +0x10): the
// voice's IsActive, and its pair's (the low half) for stereo. No sequence
// check, as in the driver.
int IsPlaying(const int32_t *a)
{
    int l = VoiceIndex(a[0] >> 16);
    if (a[4] == 1) {
        int r = VoiceIndex(a[0] & 0xFFFF);
        return g_voices[l].active && g_voices[r].active;
    }
    return g_voices[l].active ? 1 : 0;
}

void ClearMemory()
{
    for (Voice &v : g_voices)
        if (v.active && !v.isStream)
            StopNow(v);
    for (Sound &s : g_sounds)
        s = Sound();
}

struct InitGroups {
    InitGroups()
    {
        for (int &g : g_groupVolume)
            g = 100;
    }
} g_initGroups;

const char *BatchName(int cmd)
{
    switch (cmd) {
    case 2: return "SetMasterReverb";
    case 3: return "SetMasterVolume";
    case 6: return "LoadStream";
    case 7: return "LoadStreamCD";
    case 8: return "PlaySound";
    case 9: return "StopVoice";
    case 10: return "PauseVoice";
    case 11: return "StopAllVoices";
    case 12: return "FadeOutAllVoices";
    case 14: return "ReverbSound";
    case 17: return "PanVoice";
    case 18: return "PitchVoice";
    case 19: return "VolumeVoice";
    case 20: return "ReverbVoice";
    case 21: return "SettingsVoice";
    case 22: return "VolumeGroup";
    case 23: return "PauseGroup";
    case 24: return "StopGroup";
    case 25: return "ReverbGroup";
    }
    return "?";
}

void LogArgs(const char *kind, unsigned id, const char *name, const int32_t *a, int n)
{
    char buf[256];
    int len = snprintf(buf, sizeof(buf), "audio %.2f: %s 0x%X %s", g_mixed / (double)kRate, kind, id, name);
    for (int i = 0; i < n && len < (int)sizeof(buf) - 12; i++)
        len += snprintf(buf + len, sizeof(buf) - len, " %X", a[i]);
    port_log("%s", buf);
}

void LogUnknown(const char *kind, unsigned id)
{
    static bool seen[2][64];
    bool &s = seen[kind[0] == 'b'][id & 63];
    if (!s) {
        s = true;
        port_log("audio: %s command 0x%X not implemented", kind, id);
    }
}

} // namespace

int Command(unsigned func, const int32_t *a)
{
    if (g_off) {
        // no driver: what the stage-3 stub answered (golden.py's runs)
        return func == 1 ? 0x207 : func == 0x1B ? 0x100000 : 0;
    }
    std::lock_guard<std::mutex> lock(g_lock);
    if (g_log)
        LogArgs("rpc", func, "", a, 12);
    switch (func) {
    case 1: return Init(a);
    case 5: return LoadSoundCD(a);
    case 8: PlaySound(a); return 0;
    case 9: StopVoice(a); return 0;
    case 0xF: return SoundDuration(a);
    case 0x10: return SoundPitch(a);
    case 0x1A: return 0;        // InitMovieSound
    case 0x1B:                  // GetMovieSoundBuffer
        g_movie.addr = kMovieBase + (uint32_t)((a[0] >> 16) & 15) * kMovieBytes;
        return (int)g_movie.addr;
    case 0x1C:                  // StartMovieSound: from the buffer's start
        g_movie.playing = true;
        g_movie.paused = false;
        g_movie.frame = 0;
        g_movie.played = 0;
        g_movie.underruns = 0;
        if (g_log)
            port_log("audio: movie sound starts at vblank %llu", (unsigned long long)port_vblanks());
        return 0;
    case 0x1D:                  // PauseMovieSound: toggles
        g_movie.paused = !g_movie.paused;
        return 0;
    case 0x1E:                  // StopMovieSound
        if (g_log && g_movie.playing)
            port_log("audio: movie sound stopped after %.3f s, %d stale 1 KB blocks played",
                     g_movie.played / (double)kRate, g_movie.underruns);
        g_movie.playing = false;
        return 0;
    case 0x1F:                  // GetMovieStatus
        // The IRX answers sceSdBlockTransStatus(0, 0) & 0xFFFFFF, the IOP
        // address being played, but the EE adds the buffer's address to it
        // (SendToIOP: IopGetArea(..., status + m_iopBuff); ezmpeg subtracts),
        // so the PC answers the play offset (whole 1 KB blocks) minus the
        // address: the EE then sees the offset and its ring arithmetic
        // (the "same block" test, the free length) works as ezmpeg meant.
        return (int)((g_movie.frame / 256 * 1024) - g_movie.addr);
    case 0x23: ClearMemory(); return 0;
    case 0x24: return 0;        // ShowMemoryUsed
    case 0x25: return IsPlaying(a);
    }
    LogUnknown("rpc", func);
    return -104; // the driver's "Unknown Normal Rpc command"
}

void BatchCommand(int cmd, const int32_t *a)
{
    if (g_off)
        return;
    std::lock_guard<std::mutex> lock(g_lock);
    if (g_log)
        LogArgs("batch", cmd, BatchName(cmd), a, cmd == 7 ? 12 : 9);
    switch (cmd) {
    case 7: LoadStreamCD(a); return;
    case 8: PlaySound(a); return;
    case 9: StopVoice(a); return;
    case 10: PauseVoice(a); return;
    case 11: StopAllVoices(); return;
    case 12: FadeOutAllVoices(); return;
    case 17: PanVoice(a); return;
    case 18: PitchVoice(a); return;
    case 19: VolumeVoice(a); return;
    case 21: SettingsVoice(a); return;
    case 22: VolumeGroup(a); return;
    case 23: PauseGroup(a); return;
    case 24: StopGroup(a); return;
    case 2: case 3: case 14: case 20: case 25: return; // reverb, master volume: not modelled
    }
    LogUnknown("batch", cmd);
}

void IopWrite(uint32_t addr, const void *data, uint32_t bytes)
{
    if (g_off)
        return;
    std::lock_guard<std::mutex> lock(g_lock);
    if (addr < g_movie.addr || addr >= g_movie.addr + kMovieBytes)
        return;
    uint32_t off = addr - g_movie.addr, n = std::min(bytes, kMovieBytes - off);
    memcpy(&g_movie.ring[off], data, n);
    for (uint32_t b = off / 1024; b * 1024 < off + n; b++)
        g_movie.fresh[b] = true;
}

void Mix(int16_t *out, int frames)
{
    std::lock_guard<std::mutex> lock(g_lock);
    static std::vector<float> acc;
    acc.assign((size_t)frames * 2, 0.0f);
    for (Voice &v : g_voices) {
        if (!v.active)
            continue;
        if (v.paused)
            continue;
        float g = GroupFactor(v);
        if (g_mute(v.isStream, v.group, v.index, (int)(&v - g_voices)))
            g = 0;
        for (int i = 0; i < frames; i++) {
            // pitch ramp
            if (v.pitch != v.pitchTarget) {
                if (v.pitchVel > 0) {
                    float d = v.pitchTarget - v.pitch;
                    int step = std::max(1, (int)v.pitchVel);
                    v.pitch += std::abs(d) <= step ? (int)d : (d > 0 ? step : -step);
                } else {
                    v.pitch = v.pitchTarget;
                }
            }
            v.counter += v.pitch;
            while (v.counter >= 0x1000) {
                v.counter -= 0x1000;
                v.hist[0] = v.hist[1];
                v.hist[1] = v.hist[2];
                v.hist[2] = v.hist[3];
                v.hist[3] = NextSample(v);
            }
            if (v.ended && v.env != 2)
                v.env = 2;
            // envelope
            if (v.env == 0) {
                v.level += kAttack;
                if (v.level >= 1) {
                    v.level = 1;
                    v.env = 1;
                }
            } else if (v.env == 2) {
                v.level -= kRelease;
                if (v.level <= 0) {
                    StopNow(v);
                    break;
                }
            }
            // volume ramps
            float sl = v.velL > 0 ? v.velL : kVolStep, sr = v.velR > 0 ? v.velR : kVolStep;
            if (v.volL != v.tgtL)
                v.volL = std::fabs(v.tgtL - v.volL) <= sl ? v.tgtL : v.volL + (v.tgtL > v.volL ? sl : -sl);
            if (v.volR != v.tgtR)
                v.volR = std::fabs(v.tgtR - v.volR) <= sr ? v.tgtR : v.volR + (v.tgtR > v.volR ? sr : -sr);
            if (v.fading && v.volL <= 0 && v.volR <= 0) {
                StopNow(v);
                break;
            }
            // 4-point (Catmull-Rom) interpolation between hist[1] and hist[2]
            float t = v.counter * (1.0f / 4096.0f);
            float p0 = v.hist[0], p1 = v.hist[1], p2 = v.hist[2], p3 = v.hist[3];
            float s = p1 + 0.5f * t * (p2 - p0 + t * (2 * p0 - 5 * p1 + 4 * p2 - p3 + t * (3 * (p1 - p2) + p3 - p0)));
            s *= v.level * g;
            acc[2 * i] += s * v.volL;
            acc[2 * i + 1] += s * v.volR;
        }
    }
    if (g_movie.playing && !g_movie.paused) {
        float g = g_mute.MovieMuted() ? 0.0f : 1.0f;
        for (int i = 0; i < frames; i++) {
            uint32_t f = g_movie.frame, at = f / 256 * 1024 + f % 256 * 2;
            if (f % 256 == 0) {
                // a new block: the EE should have refilled it since it last played
                if (!g_movie.fresh[f / 256] && g_movie.played) {
                    if (g_log && g_movie.underruns < 20)
                        port_log("audio: movie sound underrun at %.3f s (block %u)",
                                 (g_movie.played + i) / (double)kRate, f / 256);
                    g_movie.underruns++;
                }
                g_movie.fresh[f / 256] = false;
            }
            int16_t l, r;
            memcpy(&l, &g_movie.ring[at], 2);
            memcpy(&r, &g_movie.ring[at + 512], 2);
            acc[2 * i] += l * g;
            acc[2 * i + 1] += r * g;
            g_movie.frame = (f + 1) % kMovieFrames;
        }
        g_movie.played += frames;
    }
    g_mixed += frames;
    // The SPU2 saturates; a soft knee above -2 dBFS instead of hard clips
    const float knee = 0.8f * 32767;
    for (int i = 0; i < frames * 2; i++) {
        float x = acc[i], m = std::fabs(x);
        if (m > knee)
            x = std::copysign(knee + (32767 - knee) * std::tanh((m - knee) / (32767 - knee)), x);
        out[i] = (int16_t)std::clamp((int)std::lrint(x), -32768, 32767);
    }
}

} // namespace audio
