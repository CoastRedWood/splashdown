// RainbowPS2Snd.cpp  @ 0x003308A0 (text 0x3308A0-0x3367C0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp)
//
// Retail's RainbowPS2Snd.cpp as one translation unit: a TU without a static
// initializer inside WaterWrecker.cpp's tu_map row, between WaterWrecker.cpp
// and MicroProgram.cpp (its .rodata labels run @517..@2482; MicroProgram.cpp
// restarts them at @345). The EE side of the IOP sound driver, then the
// ContainerList<_tStreamData *> and <_tLoadedSound *> members it
// instantiates (0x335EE0-0x3367C0). `make link` places its data as well as
// its code.


#define CONTAINERLIST_MEMBERS_ALL
#include "soultree_assert_block.h"
#include "ContainerList/ContainerListMembers.h"
#include "RainbowPS2Snd/RainbowPS2Snd.h"
#include "FILE_HANDLE_STRUCT/FILE_HANDLE_STRUCT.h"
#include "ParameterBlock/ParameterBlock.h"

#pragma fast_fptosi on

extern "C" char *strcpy(char *dst, const char *src);
extern "C" char *strcat(char *dst, const char *src);
extern "C" int sprintf(char *buf, const char *fmt, ...);
extern "C" int strcasecmp(const char *a, const char *b);
extern "C" int sceSifCallRpc(sceSifClientData *client, unsigned int func, unsigned int mode,
                             void *send, int ssize, void *recv, int rsize,
                             void (*end)(void *), void *end_param);
extern "C" int sceSifCheckStatRpc(sceSifClientData *client);
extern "C" int sceSifBindRpc(sceSifClientData *client, unsigned int request, unsigned int mode);

FILE_HANDLE_STRUCT *FOPEN(char *name, const char *mode);
int FCLOSE(FILE_HANDLE_STRUCT *file);
int FREAD(void *buffer, unsigned int size, FILE_HANDLE_STRUCT *file);

extern char AudioContentDirectory[];

#define MAX_SOUNDS 200

// A preloaded (or stream-library) stream: its file name, pitch and duration.
struct _tStreamData {
    char m_fileName[0x104];             // +0x000
    int m_pitch;                        // +0x104
    int m_duration;                     // +0x108
};

// A loaded sound file and its handle (0x108 bytes).
struct _tLoadedSound {
    int m_handle;                       // +0x000
    char m_name[0x104];                 // +0x004
};

// VAG file header.
struct VagHeader {
    char m_magic[4];                    // +0x00
    int m_version;                      // +0x04
    int m_x08;                          // +0x08
    int m_dataSize;                     // +0x0C, big-endian
    int m_sampleRate;                   // +0x10, big-endian
    char pad14[0x44 - 0x14];
};

// The driver's sound/stream-load arguments (RPC 5, batch command 7).
struct SoundLoadArgs {
    int m_handle;                       // +0x00
    char m_stream;                      // +0x04
    unsigned int m_flags;               // +0x08
    char m_stereo;                      // +0x0C
    char m_x0D;                         // +0x0D
    int m_start[2];                     // +0x10, left and right
    int m_end[2];                       // +0x18
    int m_size[2];                      // +0x20
    int m_x28[2];                       // +0x28
};

// Arguments of the duration and pitch queries (RPC 0xF and 0x10).
struct SoundInfoArgs {
    int m_handle;
    int m_stream;
    int m_stereo;
    int m_x0C;
    int m_x10;
};

// A voice command's arguments (RPC 9, batch commands 9 and 0x11-0x15).
struct RainbowVoiceArgs {
    int m_args[9];
};

// StopVoice's arguments (RPC 9, batch command 9): copied whole into the
// command, so named fields (an int array copies through FPRs).
struct RainbowStopArgs {
    int m_handle;                       // +0x00
    int m_x04, m_x08, m_x0C, m_x10, m_x14, m_x18, m_x1C, m_x20;
};

// PlaySound's arguments (RPC 8, batch command 8).
struct RainbowSoundArgs {
    int m_args[7];
};

// IsSoundPlaying's query (RPC 0x25).
struct RainbowSoundQuery {
    int m_handle;                       // +0x00
    int m_x04;
    int m_x08;
    int m_x0C;
    int m_x10;                          // +0x10
    int m_pad14[4];
};

// An inlined strrchr(name, '.').
static inline char *FindExtension(char *s)
{
    char *start = s;
    while (*s++)
        ;
    do {
        s--;
    } while (s != start && *s != '.');
    return *s == '.' ? s : 0;
}

// ---- src/RainbowPS2Snd/ctor.cpp

RainbowPS2Snd::RainbowPS2Snd()
{
    m_batchId = -1;
    m_numSounds = 0;
    m_x400 = 100;
    m_x404 = 100;
    m_x408 = 100;
    m_surround = 0;
    m_mono = 0;
    m_streamLibraryLoaded = 0;
    m_currentBatch = 1;
    m_batchCount[0] = 1;
    m_batchCount[1] = 1;
    m_batchData = &m_batchBuffer1;
    m_maxBatchCommands = 0;
    m_streamUsed = 0;
    m_movieStream = -1;
    m_streamPitch = 0;
    m_streamSlots = 0;
    m_numStreams = 0;
    m_fileName[0] = 0;
    m_preloadedStreams.Create(1, 16);
    m_loadedSounds.Create(1, 16);
    for (int bank = 0; bank < 2; bank++) {
        for (int voice = 0; voice < 24; voice++) {
            m_voices[bank][voice] = 0;
            m_sequence[bank][voice] = 0;
            m_voiceInfo[bank][voice].m_stream = 0;
            m_voiceInfo[bank][voice].m_x04 = 0;
            m_voiceInfo[bank][voice].m_fading = 0;
            m_voiceInfo[bank][voice].m_paused = 0;
            m_voiceInfo[bank][voice].m_index = -1;
            m_voiceInfo[bank][voice].m_group = -1;
            m_voiceInfo[bank][voice].m_fadeRate = 0;
            m_voiceInfo[bank][voice].m_idleTime = 180.0f;
        }
    }
    memset(&m_batchBuffer1, 0, sizeof(m_batchBuffer1));
    memset(&m_batchBuffer2, 0, sizeof(m_batchBuffer2));
    memset(&m_client, 0, sizeof(m_client));
    memset(&m_batchClients[0], 0, sizeof(m_batchClients[0]));
    memset(&m_batchClients[1], 0, sizeof(m_batchClients[1]));
    m_batchData->m_numCommands = 0;
}

// ---- src/RainbowPS2Snd/dtor.cpp

RainbowPS2Snd::~RainbowPS2Snd()
{
    if (m_streamUsed)
        delete[] m_streamUsed;
    if (m_streamPitch)
        delete[] m_streamPitch;
    if (m_streamSlots)
        delete[] m_streamSlots;
    while (m_loadedSounds.m_numItems > 0) {
        _tLoadedSound *sound = m_loadedSounds.GetItem(0);
        m_loadedSounds.RemoveItem(sound);
        delete sound;
    }
    while (m_preloadedStreams.m_numItems > 0) {
        _tStreamData *stream = m_preloadedStreams.GetItem(0);
        m_preloadedStreams.RemoveItem(stream);
        delete stream;
    }
}

// ---- src/RainbowPS2Snd/Rpc.cpp

int RainbowPS2Snd::Rpc(int func, char *data, unsigned int size)
{
    memset(m_sendBuffer, 0, 0x134);
    memcpy(m_sendBuffer, data, size);
    sceSifCallRpc(&m_client, func, 0, m_sendBuffer, (size + 15) & ~15, m_recvBuffer, 0x20, 0, 0);
    return m_recvBuffer[0];
}

// ---- src/RainbowPS2Snd/RpcBatch.cpp

// Send the batch being filled and start filling the other buffer. (data is unused.)
int RainbowPS2Snd::RpcBatch(int func, char *, unsigned int size)
{
    unsigned int blocks = (size + 15) / 16;
    if (m_currentBatch == 1) {
        m_batchData->m_x04 = m_batchCount[0];
        m_batchCount[0]++;
        while (sceSifCheckStatRpc(&m_batchClients[0]))
            ;
        sceSifCallRpc(&m_batchClients[0], func, 1, &m_batchBuffer1, blocks * 16, m_recvBuffer, 0x20, 0, 0);
        m_currentBatch = 2;
        m_batchData = &m_batchBuffer2;
    } else if (m_currentBatch == 2) {
        m_batchData->m_x04 = m_batchCount[1];
        m_batchCount[1]++;
        while (sceSifCheckStatRpc(&m_batchClients[1]))
            ;
        sceSifCallRpc(&m_batchClients[1], func, 1, &m_batchBuffer2, blocks * 16, m_recvBuffer, 0x20, 0, 0);
        m_currentBatch = 1;
        m_batchData = &m_batchBuffer1;
    }
    return 0;
}

// ---- src/RainbowPS2Snd/GetAvailableSound.cpp

short RainbowPS2Snd::GetAvailableSound()
{
    if (m_numSounds + 1 > MAX_SOUNDS) {
        PSXprintf("RainbowPS2Snd: MAX_SOUNDS reached!\n");
        return -1;
    }
    m_numSounds++;
    return m_numSounds;
}

// ---- src/RainbowPS2Snd/GetFreeStreams.cpp

int RainbowPS2Snd::GetFreeStreams()
{
    unsigned int i;
    short numFree = 0;
    for (i = 0; i < m_numStreams; i++) {
        if (m_streamUsed[i] == 0)
            numFree++;
    }
    return numFree;
}

// ---- src/RainbowPS2Snd/GetAvailableStream.cpp

int RainbowPS2Snd::GetAvailableStream()
{
    float best = 0.0f;
    int found = -1;
    for (unsigned int i = 0; i < m_numStreams; i++) {
        if (m_streamUsed[i] == 0) {
            if (best == 0.0f) {
                best = m_streamPitch[i];
                found = i;
            } else if (m_streamPitch[i] > best) {
                best = m_streamPitch[i];
                found = i;
            }
        }
    }
    if (found != -1) {
        m_streamUsed[found] = 1;
        m_streamPitch[found] = 0;
        return found;
    }
    PSXprintf("RainbowPS2Snd: No Streams Available!\n");
    return -1;
}

// ---- src/RainbowPS2Snd/GetPreLoadedStreamIndex.cpp

// With a stream library loaded, names are stored without the directory.
int RainbowPS2Snd::GetPreLoadedStreamIndex(char *name)
{
    char streamName[0x110];
    char path[0x110];
    int len = strlen(name);
    if (name[len - 1] == '\n' || name[len - 1] == '\r')
        name[len - 1] = 0;
    if (m_streamLibraryLoaded) {
        for (int i = 0; i < m_preloadedStreams.m_numItems; i++) {
            strcpy(streamName, m_preloadedStreams.GetItem(i)->m_fileName);
            if (strcasecmp(streamName, name) == 0)
                return i;
        }
        return -1;
    }
    sprintf(path, "%s%s", AudioContentDirectory, name);
    for (int i = 0; i < m_preloadedStreams.m_numItems; i++) {
        strcpy(streamName, m_preloadedStreams.GetItem(i)->m_fileName);
        if (strcasecmp(streamName, path) == 0)
            return i;
    }
    return -1;
}

// ---- src/RainbowPS2Snd/FreeStream.cpp

void RainbowPS2Snd::FreeStream(int stream)
{
#line 401 "RainbowPS2Snd.cpp"
    ASSERTF(stream >= 0 && stream < m_numStreams, ("FreeStream: StreamIndex out of range! (%d)\n", stream));
    if (stream >= 0 && stream < m_numStreams)
        m_streamUsed[stream] = 0;
}

// ---- src/RainbowPS2Snd/GetAvailableVoice.cpp

// Picks the free voice idle the longest (bank 0 up, or bank 1 down when reverse);
// returns its handle: bank, voice and the voice's new sequence number.
int RainbowPS2Snd::GetAvailableVoice(int a, int b, int reverse, int c)
{
    float best;
    unsigned short bestBank;
    unsigned short bestVoice;
    int found;
    short bank;
    short voice;
    best = 0.0f;
    found = 0;

    if (!reverse) {
        for (bank = 0; bank < 2; bank++) {
            for (voice = 0; voice < 24; voice++) {
                if (!m_voices[bank][voice]) {
                    float age = m_voiceInfo[bank][voice].m_idleTime;
                    if (age > 2.5f) {
                        if (best == 0.0f) {
                            found = 1;
                            best = age;
                            bestBank = bank;
                            bestVoice = voice;
                        } else if (age > best) {
                            found = 1;
                            best = age;
                            bestBank = bank;
                            bestVoice = voice;
                        }
                    }
                }
                if (found == 1 && best >= 180.0f)
                    break;
            }
            if (found == 1 && best >= 180.0f)
                break;
        }
    } else {
        for (bank = 1; bank >= 0; bank--) {
            for (voice = 23; voice >= 0; voice--) {
                if (!m_voices[bank][voice]) {
                    float age = m_voiceInfo[bank][voice].m_idleTime;
                    if (age > 2.5f) {
                        if (best == 0.0f) {
                            found = 1;
                            best = age;
                            bestBank = bank;
                            bestVoice = voice;
                        } else if (age > best) {
                            found = 1;
                            best = age;
                            bestBank = bank;
                            bestVoice = voice;
                        }
                    }
                }
                if (found == 1 && best >= 180.0f)
                    break;
            }
            if (found == 1 && best >= 180.0f)
                break;
        }
    }
    if (!found) {
        PSXprintf("RainbowPS2Snd: No Available Voices!!\n");
        return -1;
    }
    m_voices[bestBank][bestVoice] = 1;
    m_sequence[bestBank][bestVoice]++;
    if (m_sequence[bestBank][bestVoice] > 500)
        m_sequence[bestBank][bestVoice] = 0;
    short sequence = m_sequence[bestBank][bestVoice];
    int handle = bestBank | (sequence << 6 | bestVoice << 1);
    m_voiceInfo[bestBank][bestVoice].m_stream = reverse;
    m_voiceInfo[bestBank][bestVoice].m_x04 = c;
    m_voiceInfo[bestBank][bestVoice].m_fading = 0;
    m_voiceInfo[bestBank][bestVoice].m_paused = 0;
    m_voiceInfo[bestBank][bestVoice].m_index = a;
    m_voiceInfo[bestBank][bestVoice].m_group = b;
    m_voiceInfo[bestBank][bestVoice].m_fadeRate = 0;
    m_voiceInfo[bestBank][bestVoice].m_idleTime = 0.0f;
    return handle;
}

// ---- src/RainbowPS2Snd/GetAvailableVoicePair.cpp

// Considers one voice: the two longest-idle free voices of a bank become the pair.
#define CONSIDER(b)                                                     \
    if (!m_voices[b][voice]) {                                          \
        float age = m_voiceInfo[b][voice].m_idleTime;                   \
        if (age > 2.5f) {                                               \
            if (!found) {                                               \
                if (best == 0.0f) {                                     \
                    found = 1;                                          \
                    best = age;                                         \
                    bank = b;                                           \
                    voice1 = voice;                                     \
                } else if (age > best) {                                \
                    found = 1;                                          \
                    best = age;                                         \
                    bank = b;                                           \
                    voice1 = voice;                                     \
                }                                                       \
            } else if (!found2) {                                       \
                if (best2 == 0.0f) {                                    \
                    found2 = 1;                                         \
                    best2 = age;                                        \
                    bank2 = b;                                          \
                    voice2 = voice;                                     \
                } else if (age > best2) {                               \
                    found2 = 1;                                         \
                    best2 = age;                                        \
                    bank2 = b;                                          \
                    voice2 = voice;                                     \
                }                                                       \
            }                                                           \
        }                                                               \
    }                                                                   \
    if (found == 1 && found2 == 1 && best >= 180.0f && best2 >= 180.0f) \
        break;

// Finds two free voices in one bank (bank 0 first, or bank 1 first for streams),
// preferring the longest idle; returns 0 when there are none.
int RainbowPS2Snd::GetAvailableVoicePair(int index, int index2, int handle, int stream, int c, int *left, int *right)
{
    float best;
    float best2;
    unsigned short bank;
    unsigned short bank2;
    unsigned short voice1;
    unsigned short voice2;
    int found;
    int found2;
    short voice;

    best = 0.0f;
    best2 = 0.0f;
    found = 0;
    found2 = 0;
    *left = -1;
    *right = -1;
    if (!stream) {
        for (voice = 0; voice < 24; voice++) {
            CONSIDER(0)
        }
        if (!found || !found2) {
            found = 0;
            found2 = 0;
            best = 0.0f;
            best2 = 0.0f;
            for (voice = 0; voice < 24; voice++) {
                CONSIDER(1)
            }
        }
    } else {
        for (voice = 23; voice >= 0; voice--) {
            CONSIDER(1)
        }
        if (!found || !found2) {
            best = 0.0f;
            best2 = 0.0f;
            found = 0;
            found2 = 0;
            for (voice = 23; voice >= 0; voice--) {
                CONSIDER(0)
            }
        }
    }
    if (!found || !found2) {
        PSXprintf("RainbowPS2Snd: No Available Voice Pairs!!\n");
        return 0;
    }
    m_voices[bank][voice1] = 1;
    m_voices[bank2][voice2] = 1;
    m_sequence[bank][voice1]++;
    if (m_sequence[bank][voice1] > 500)
        m_sequence[bank][voice1] = 0;
    m_sequence[bank2][voice2]++;
    if (m_sequence[bank2][voice2] > 500)
        m_sequence[bank2][voice2] = 0;
    short sequence = m_sequence[bank][voice1];
    short sequence2 = m_sequence[bank2][voice2];
    *left = bank | (sequence << 6 | voice1 << 1);
    *right = bank2 | (sequence2 << 6 | voice2 << 1);
    m_voiceInfo[bank][voice1].m_stream = stream;
    m_voiceInfo[bank][voice1].m_x04 = c;
    m_voiceInfo[bank][voice1].m_fading = 0;
    m_voiceInfo[bank][voice1].m_paused = 0;
    m_voiceInfo[bank][voice1].m_index = index;
    m_voiceInfo[bank][voice1].m_group = handle;
    m_voiceInfo[bank][voice1].m_fadeRate = 0;
    m_voiceInfo[bank][voice1].m_idleTime = 0.0f;
    m_voiceInfo[bank2][voice2].m_stream = stream;
    m_voiceInfo[bank2][voice2].m_x04 = c;
    m_voiceInfo[bank2][voice2].m_fading = 0;
    m_voiceInfo[bank2][voice2].m_paused = 0;
    m_voiceInfo[bank2][voice2].m_index = index2;
    m_voiceInfo[bank2][voice2].m_group = handle;
    m_voiceInfo[bank2][voice2].m_fadeRate = 0;
    m_voiceInfo[bank2][voice2].m_idleTime = 0.0f;
    return 1;
}

#undef CONSIDER

// ---- src/RainbowPS2Snd/FreeVoice.cpp

// A voice handle packs the bank (bit 0), the voice (bits 1-5) and the sequence number it was issued with (bits 6+).
void RainbowPS2Snd::FreeVoice(int handle)
{
    unsigned short sequence = handle >> 6;
    unsigned char voice = (handle >> 1) & 0x1F;
    unsigned char bank = handle & 1;
    if (sequence == m_sequence[bank][voice]) {
        m_voices[bank][voice] = 0;
        if (m_voiceInfo[bank][voice].m_stream)
            m_voiceInfo[bank][voice].m_idleTime = 0.0f;
        else
            m_voiceInfo[bank][voice].m_idleTime = 3.0f;
    }
}

// ---- src/RainbowPS2Snd/Init.cpp

int RainbowPS2Snd::Init(unsigned int numVoices)
{
    int ret;
    m_numStreams = numVoices;
    m_streamUsed = new int[m_numStreams];
    m_streamPitch = new float[m_numStreams];
    m_streamSlots = new SoundSlot[m_numStreams];
    for (unsigned int i = 0; i < m_numStreams; i++) {
        m_streamUsed[i] = 0;
        m_streamPitch[i] = 180.0f;
    }
    PSXprintf("RainbowPS2Snd::Init(): entered\n");
    do {
        if ((ret = sceSifBindRpc(&m_batchClients[0], 0x12346, 1)) < 0) {
            PSXprintf("ERROR: sceSifBindRpc\n");
            return ret;
        }
        if ((ret = sceSifBindRpc(&m_batchClients[1], 0x23456, 1)) < 0) {
            PSXprintf("ERROR: sceSifBindRpc\n");
            return ret;
        }
        if ((ret = sceSifBindRpc(&m_client, 0x34567, 0)) < 0) {
            PSXprintf("ERROR: sceSifBindRpc\n");
            return ret;
        }
    } while (!m_batchClients[0].serve || !m_batchClients[1].serve || !m_client.serve);
    unsigned int version = Rpc(1, (char *)&m_numStreams, 4);
    PSXprintf("**** SOUND LIBRARY VERSION %X.%02X\n", (version & 0xFF00) >> 8, version & 0xFF);
    if (version != 0x207) {
#line 984 "RainbowPS2Snd.cpp"
        ASSERTF(0, ("RainbowPS2Snd::Init(): wrong sound version.  Expecting %X.%02X, got %X.%02X\n", 2, 7, version >> 8, version & 0xFF));
        return -110;
    }
    PSXprintf("RainbowPS2Snd::Init(): done\n");
    return 0;
}

// ---- src/RainbowPS2Snd/ShowDriverMemoryUsed.cpp

void RainbowPS2Snd::ShowDriverMemoryUsed()
{
    Rpc(0x24, 0, 0);
}

// ---- src/RainbowPS2Snd/SendBatch.cpp

int RainbowPS2Snd::SendBatch()
{
    if (m_batchId == -1)
        return 0;
    if (m_batchData->m_numCommands > m_maxBatchCommands)
        m_maxBatchCommands = m_batchData->m_numCommands;
    int func = (m_currentBatch == 1) ? 0x20 : 0x21;
    int result = RpcBatch(func, (char *)&m_batchData, 0x4508);
    m_batchId = -1;
    m_batchData->m_numCommands = 0;
    memset(m_batchData, 0, 0x4508);
    return result;
}

// ---- src/RainbowPS2Snd/GetPlayStatus.cpp

// Advances voice and stream timers, stops voices that have played out and frees
// streams no voice uses any more.
void RainbowPS2Snd::GetPlayStatus(float dt)
{
    int bank;
    int voice;
    for (bank = 0; bank < 2; bank++) {
        for (voice = 0; voice < 24; voice++) {
            if (m_voices[bank][voice] && !m_voiceInfo[bank][voice].m_x04 && !m_voiceInfo[bank][voice].m_paused) {
                float duration;
                m_voiceInfo[bank][voice].m_fadeRate += dt;
                if (m_voiceInfo[bank][voice].m_stream)
                    duration = m_streamSlots[m_voiceInfo[bank][voice].m_index].m_duration;
                else
                    duration = m_sounds[m_voiceInfo[bank][voice].m_index].m_duration / 1000.0f;
                if (!m_voiceInfo[bank][voice].m_stream) {
                    if (m_voiceInfo[bank][voice].m_fadeRate >= 0.25f + duration) {
                        m_voices[bank][voice] = 0;
                        m_voiceInfo[bank][voice].m_idleTime = 3.0f;
                    }
                } else {
                    if (m_voiceInfo[bank][voice].m_fadeRate >= 0.5f + duration)
                        m_voices[bank][voice] = 0;
                }
            } else if (!m_voices[bank][voice]) {
                if (m_streamUsed[m_voiceInfo[bank][voice].m_index] && m_voiceInfo[bank][voice].m_stream &&
                    m_voiceInfo[bank][voice].m_idleTime > 1.0f) {
                    FreeStream(m_voiceInfo[bank][voice].m_index);
                    m_voiceInfo[bank][voice].m_stream = 0;
                }
                if (m_voiceInfo[bank][voice].m_idleTime < 180.0f)
                    m_voiceInfo[bank][voice].m_idleTime += dt;
            } else if (m_voices[bank][voice] && m_voiceInfo[bank][voice].m_x04) {
                if (m_voiceInfo[bank][voice].m_fading)
                    m_voiceInfo[bank][voice].m_fadeRate += dt;
                if (m_voiceInfo[bank][voice].m_fading && m_voiceInfo[bank][voice].m_fadeRate >= 0.5f) {
                    m_voiceInfo[bank][voice].m_fading = 0;
                    m_voices[bank][voice] = 0;
                    if (!m_voiceInfo[bank][voice].m_stream)
                        m_voiceInfo[bank][voice].m_idleTime = 3.0f;
                }
            }
        }
    }
    for (unsigned int i = 0; i < m_numStreams; i++) {
        if (!m_streamUsed[i]) {
            if (m_streamPitch[i] < 180.0f)
                m_streamPitch[i] += dt;
        } else if (i != m_movieStream) {
            int used = 0;
            for (bank = 0; bank < 2; bank++) {
                for (voice = 0; voice < 24; voice++) {
                    if (m_voiceInfo[bank][voice].m_stream && m_voiceInfo[bank][voice].m_index == i) {
                        used = 1;
                        break;
                    }
                }
            }
            if (!used)
                FreeStream(i);
        }
    }
}

// ---- src/RainbowPS2Snd/ClearMemory.cpp

void RainbowPS2Snd::ClearMemory()
{
    m_numSounds = 0;
    memset(m_sounds, 0, sizeof(m_sounds));
    memset(m_voices, 0, sizeof(m_voices));
    memset(m_sequence, 0, sizeof(m_sequence));
    for (int bank = 0; bank < 2; bank++) {
        for (int voice = 0; voice < 24; voice++) {
            m_voiceInfo[bank][voice].m_stream = 0;
            m_voiceInfo[bank][voice].m_x04 = 0;
            m_voiceInfo[bank][voice].m_fading = 0;
            m_voiceInfo[bank][voice].m_paused = 0;
            m_voiceInfo[bank][voice].m_index = -1;
            m_voiceInfo[bank][voice].m_group = -1;
            m_voiceInfo[bank][voice].m_fadeRate = 0;
            m_voiceInfo[bank][voice].m_idleTime = 180.0f;
        }
    }
    for (unsigned int i = 0; i < m_numStreams; i++) {
        m_streamUsed[i] = 0;
        m_streamPitch[i] = 180.0f;
    }
    for (int i = 0; i < m_loadedSounds.m_numItems; i++)
        delete m_loadedSounds.GetItem(i);
    m_loadedSounds.Clear();
    Rpc(0x23, 0, 0);
}

// ---- src/RainbowPS2Snd/PreLoadStream.cpp

int RainbowPS2Snd::PreLoadStream(char *name, int *preloadIndex)
{
    char path[0x110];
    VagHeader header;
    int sampleRate;
    int dataSize;

    if (m_streamLibraryLoaded) {
        if (preloadIndex) {
            *preloadIndex = GetPreLoadedStreamIndex(name);
#line 1236 "RainbowPS2Snd.cpp"
            ASSERTF(*preloadIndex != -1, ("Error! Streaming file %s not found in Stream Library\n", name));
            return 1;
        }
#line 1239 "RainbowPS2Snd.cpp"
        ASSERT(0, "Error! Must provide preloadIndex parameter to PreLoadStream if using Stream Library File\n");
    } else {
        int index = GetPreLoadedStreamIndex(name);
        if (preloadIndex)
            *preloadIndex = index;
        if (index >= 0)
            return 1;
    }
    memset(&header, 0, sizeof(header));
    sprintf(m_fileName, "%s%s", AudioContentDirectory, name);
    char *extension = FindExtension(m_fileName);
    if (extension)
        strcpy(extension, ".vag");
    else
        strcat(m_fileName, (char *)".vag");
    sprintf(path, "%s%s", AudioContentDirectory, name);
    extension = FindExtension(path);
    if (extension)
        strcpy(extension, ".vag");
    else
        strcat(path, (char *)".vag");
    FILE_HANDLE_STRUCT *file = FOPEN(path, "r");
    if (!file) {
        PSXprintf("RainbowPS2Sound::LoadSound Cannot find '%s'\n", m_fileName);
        return -0x72;
    }
    if (FREAD(&header, 0x40, file) <= 0)
        return 0;
    FCLOSE(file);
    EndianIntSwap(&header.m_sampleRate, &sampleRate);
    EndianIntSwap(&header.m_dataSize, &dataSize);
    _tStreamData *data = new _tStreamData;
    strcpy(data->m_fileName, m_fileName);
    float seconds = dataSize / (sampleRate / 28 * 16);
    data->m_duration = seconds;
    if (!data->m_duration)
        data->m_duration = 1;
    data->m_pitch = (sampleRate << 12) / 48000;
    if (!m_preloadedStreams.AddItem(data)) {
        delete data;
#line 1349 "RainbowPS2Snd.cpp"
        ASSERT(0, "RainbowPS2Snd::PreloadStream(): couldn't add tStreamData item to list.\n");
        return 0;
    }
    if (preloadIndex)
        *preloadIndex = m_preloadedStreams.m_numItems - 1;
    return 1;
}

// ---- src/RainbowPS2Snd/LoadStreamLibrary.cpp

int RainbowPS2Snd::LoadStreamLibrary(char *name)
{
    char block[0x40];
    ParameterBlock params;
    int numSongs;
    int value;

    if (!params.OpenFile(name, 0))
        return 0;
    if (params.ReadParameterBlock("Songs")) {
        params.GetParameter("NumberOfSongs", 0, &numSongs);
        for (int i = 1; i <= numSongs; i++) {
            _tStreamData *data = new _tStreamData;
            sprintf(block, "Song %d", i);
            params.ReadParameterBlock(block);
            params.GetParameter("FileName", "", data->m_fileName, -1);
            params.GetParameter("Duration", 0, &value);
            data->m_duration = value;
            params.GetParameter("Pitch", 0, &value);
            data->m_pitch = value;
            if (!m_preloadedStreams.AddItem(data)) {
                delete data;
#line 1443 "RainbowPS2Snd.cpp"
                ASSERT(0, "RainbowPS2Snd::PreloadStream(): couldn't add tStreamData item to list.\n");
                return 0;
            }
        }
    } else {
        return 0;
    }
    m_streamLibraryLoaded = 1;
    return 1;
}

// ---- src/RainbowPS2Snd/LoadSound.cpp

int RainbowPS2Snd::LoadSound(char *name_, unsigned int flags_, int b, int stereo_, int isStream_)
{
    int stereo;
    char *name;
    RainbowPS2Snd *self;
    _tLoadedSound *loaded;
    short sound;
    int i;
    short sound2;
    char compare[0x110];
    SoundInfoArgs info;
    char path[0x110];
    SoundLoadArgs args;
    char base[0x110];
    int index;
    int result;
    int isStream;
    unsigned int flags;
    // Retail copies `this` and the parameters into locals first. mwcc colours
    // named locals before parameters, so these copies are what give retail's
    // $s registers (stereo $s0, name $s1, this $s2, ...).
    self = this;
    flags = flags_;
    name = name_;
    stereo = stereo_;
    isStream = isStream_;
    sound2 = 0;
    memset(&args, 0, sizeof(args));
    sprintf(path, "%s", name);
    if (!isStream) {
        if (name[strlen(name) - 1] == '\n' || name[strlen(name) - 1] == '\r')
            name[strlen(name) - 1] = 0;
        for (i = 0; i < self->m_loadedSounds.m_numItems; i++) {
            strcpy(compare, self->m_loadedSounds.GetItem(i)->m_name);
            if (!strcasecmp(compare, name))
                return self->m_loadedSounds.GetItem(i)->m_handle;
        }
    } else {
        index = self->GetPreLoadedStreamIndex(name);
        if (index < 0) {
            PSXprintf("RainbowPS2Snd: Load called for a Stream which was NOT PreLoaded (%s)!\n", name);
            return -0x6A;
        }
    }
    if (!isStream) {
        sound = self->GetAvailableSound();
        if (sound == -1)
            return -0x65;
        if (stereo) {
            sound2 = self->GetAvailableSound();
            if (sound2 == -1)
                return -0x65;
        }
    } else {
        sound = self->GetAvailableStream();
        if (sound == -1)
            return -0x6B;
        if (stereo) {
            sound2 = self->GetAvailableStream();
            if (sound2 == -1) {
                self->FreeStream(sound);
                return -0x6B;
            }
        }
    }
    if (!isStream && stereo) {
        char *extension;
        FILE_HANDLE_STRUCT *file;
        FILE_HANDLE_STRUCT *file2;
        strcpy(base, path);
        extension = FindExtension(base);
        if (extension)
            memset(extension, 0, strlen(extension));
        sprintf(self->m_fileName, "%sL.vag", base);
        file = FOPEN(self->m_fileName, "r");
        if (!file) {
            PSXprintf("RainbowPS2Sound::LoadSound Cannot find '%s'\n", self->m_fileName);
            self->m_numSounds--;
            if (stereo)
                self->m_numSounds--;
            return -0x72;
        }
        strcpy(base, path);
        extension = FindExtension(base);
        if (extension)
            memset(extension, 0, strlen(extension));
        sprintf(self->m_fileName, "%sR.vag", base);
        file2 = FOPEN(self->m_fileName, "r");
        if (!file2) {
            PSXprintf("RainbowPS2Sound::LoadSound Cannot find '%s'\n", self->m_fileName);
            self->m_numSounds--;
            if (stereo)
                self->m_numSounds--;
            return -0x72;
        }
        args.m_handle = sound << 16 | sound2;
        args.m_stream = isStream;
        args.m_flags = flags;
        args.m_stereo = stereo;
        args.m_x0D = b;
        args.m_start[0] = file->file.lsn;
        args.m_end[0] = file->file.lsn + file->file.size - 1;
        args.m_size[0] = file->file.size;
        args.m_x28[0] = file->size;
        args.m_start[1] = file2->file.lsn;
        args.m_end[1] = file2->file.lsn + file2->file.size - 1;
        args.m_size[1] = file2->file.size;
        args.m_x28[1] = file2->size;
        FCLOSE(file);
        FCLOSE(file2);
    } else {
        strcpy(self->m_fileName, path);
        char *extension = FindExtension(self->m_fileName);
        if (extension)
            strcpy(extension, ".vag");
        else
            strcat(
#ifdef SPLASHDOWN_PC
                   self->m_fileName,
#else
                   (char *)self + 0x84,
#endif
                   (char *)".vag");
        FILE_HANDLE_STRUCT *file = FOPEN(self->m_fileName, "r");
        if (!file) {
            PSXprintf("RainbowPS2Sound::LoadSound Cannot find '%s'\n", self->m_fileName);
            self->m_numSounds--;
            if (stereo)
                self->m_numSounds--;
            return -0x72;
        }
        args.m_handle = sound << 16 | sound2;
        args.m_stream = isStream;
        args.m_flags = flags;
        args.m_stereo = stereo;
        args.m_x0D = b;
        args.m_start[0] = file->file.lsn;
        args.m_end[0] = file->file.lsn + file->file.size - 1;
        args.m_size[0] = file->file.size;
        args.m_x28[0] = file->size;
        FCLOSE(file);
    }
    if (isStream) {
        if (self->m_batchId + 1 == 64)
            self->SendBatch();
        self->m_batchId++;
        self->m_batchData->m_numCommands++;
        self->m_batchData->m_commands[self->m_batchId].m_command = 7;
        *(SoundLoadArgs *)self->m_batchData->m_commands[self->m_batchId].m_args = args;
    } else {
        result = self->Rpc(5, (char *)&args, sizeof(args));
        if (result < 0) {
            if (result == -0x75) {
#line 1721 "RainbowPS2Snd.cpp"
                ASSERT(0, "RainbowPS2Snd::LoadSound(): 2 MEG memory limit exceeded!\n");
            }
            PSXprintf("RainbowPS2Snd::LoadSound() Failed!\n");
            self->m_numSounds--;
            if (stereo)
                self->m_numSounds--;
            return result;
        }
    }
    info.m_handle = sound << 16 | sound2;
    info.m_stream = isStream;
    info.m_stereo = stereo;
    info.m_x0C = 0;
    info.m_x10 = 0;
    if (!isStream) {
        self->m_sounds[sound].m_duration = self->Rpc(0xF, (char *)&info, sizeof(info));
        self->m_sounds[sound].m_pitch = self->Rpc(0x10, (char *)&info, sizeof(info));
        self->m_sounds[sound].m_handle = flags;
        if (stereo) {
            self->m_sounds[sound2].m_duration = self->m_sounds[sound].m_duration;
            self->m_sounds[sound2].m_pitch = self->m_sounds[sound].m_pitch;
            self->m_sounds[sound2].m_handle = flags;
        }
    } else {
        self->m_streamSlots[sound].m_duration = self->m_preloadedStreams.GetItem(index)->m_duration;
        self->m_streamSlots[sound].m_pitch = self->m_preloadedStreams.GetItem(index)->m_pitch;
        self->m_streamSlots[sound].m_handle = flags;
        if (stereo) {
            self->m_streamSlots[sound2].m_duration = self->m_streamSlots[sound].m_duration;
            self->m_streamSlots[sound2].m_pitch = self->m_streamSlots[sound].m_pitch;
            self->m_streamSlots[sound2].m_handle = flags;
        }
    }
    static int verbose = 0;
    if (verbose) {
        if (!isStream) {
            PSXprintf("SoundL Duration : %d\n", self->m_sounds[sound].m_duration);
            PSXprintf("SoundL Pitch    : %d\n", self->m_sounds[sound].m_pitch);
            if (stereo) {
                PSXprintf("SoundR Duration : %d\n", self->m_sounds[sound2].m_duration);
                PSXprintf("SoundR Pitch    : %d\n", self->m_sounds[sound2].m_pitch);
            }
        } else {
            PSXprintf("StreamL Duration : %d\n", self->m_streamSlots[sound].m_duration);
            PSXprintf("StreamL Pitch    : %d\n", self->m_streamSlots[sound].m_pitch);
            if (stereo) {
                PSXprintf("StreamR Duration : %d\n", self->m_streamSlots[sound2].m_duration);
                PSXprintf("StreamR Pitch    : %d\n", self->m_streamSlots[sound2].m_pitch);
            }
        }
    }
    if (!isStream) {
        loaded = new _tLoadedSound;
        strcpy(loaded->m_name, name);
        loaded->m_handle = sound << 16 | sound2;
        self->m_loadedSounds.AddItem(loaded);
    }
    return sound << 16 | sound2;
}

// ---- src/RainbowPS2Snd/LoadAndPlayStream.cpp

// Loads a preloaded stream (and a second one for stereo) and starts it on a
// free voice or voice pair; returns the voice handle(s), first in the high half.
int RainbowPS2Snd::LoadAndPlayStream(char *name, unsigned int *duration, unsigned int flags, int stereo, int volume,
                                     int pan, int pitch, int preloadIndex)
{
    char path[0x110];
    SoundLoadArgs args;
    int voice;
    int voice2;
    short stream2;
    voice = 0;
    voice2 = 0;
    stream2 = 0;
    memset(&args, 0, sizeof(args));
    sprintf(path, "%s%s", AudioContentDirectory, name);
    int index;
    if (preloadIndex >= 0) {
        index = preloadIndex;
    } else {
        index = GetPreLoadedStreamIndex(name);
        if (index < 0) {
            PSXprintf("RainbowPS2Snd: Load called for a Stream which was NOT PreLoaded (%s)!\n", path);
            return -0x6A;
        }
    }
    short stream = GetAvailableStream();
    if (stream == -1)
        return -0x6B;
    if (stereo) {
        stream2 = GetAvailableStream();
        if (stream2 == -1) {
            FreeStream(stream);
            return -0x6B;
        }
    }
    *duration = GetStreamDuration(index);
    strcpy(m_fileName, path);
    char *extension = FindExtension(m_fileName);
    if (extension)
        strcpy(extension, ".vag");
    else
        // Spelled differently from the other m_fileName uses: this form loads
        // the literal before the buffer address, as retail does.
        strcat(
#ifdef SPLASHDOWN_PC
                   this->m_fileName,
#else
                   (char *)this + 0x84,
#endif
                   (char *)".vag");
    FILE_HANDLE_STRUCT *file = FOPEN(m_fileName, "r");
    if (!file) {
        PSXprintf("RainbowPS2Sound::LoadSound Cannot find '%s'\n", m_fileName);
        return -0x72;
    }
    int handle = stream << 16 | stream2;
    args.m_handle = handle;
    args.m_stream = 1;
    args.m_flags = flags;
    args.m_stereo = stereo;
    args.m_x0D = 0;
    args.m_start[0] = file->file.lsn;
    args.m_end[0] = file->file.lsn + file->file.size - 1;
    args.m_size[0] = file->file.size;
    args.m_x28[0] = file->size;
    FCLOSE(file);
    m_streamSlots[stream].m_duration = m_preloadedStreams.GetItem(index)->m_duration;
    m_streamSlots[stream].m_pitch = m_preloadedStreams.GetItem(index)->m_pitch;
    m_streamSlots[stream].m_handle = flags;
    if (stereo) {
        m_streamSlots[stream2].m_duration = m_streamSlots[stream].m_duration;
        m_streamSlots[stream2].m_pitch = m_streamSlots[stream].m_pitch;
        m_streamSlots[stream2].m_handle = flags;
    }
    int slot = m_streamSlots[stream].m_handle;
    int right = 0;
    if (stereo) {
        if (!GetAvailableVoicePair(stream, stream2, slot, 1, 0, &voice, &voice2)) {
            FreeStream(stream);
            FreeStream(stream2);
            return -0x64;
        }
    } else {
        voice = GetAvailableVoice(stream, slot, 1, 0);
        if (voice == -1) {
            FreeStream(stream);
            return -0x64;
        }
    }
    int left = voice;
    if (stereo)
        right = voice2;
    if (m_mono || pan == 0 || pan == 360 || pan == 180)
        pan = -1;
    if (volume < -1 || volume > 100) {
        FreeVoice(voice);
        FreeStream(stream);
        if (stereo) {
            FreeVoice(voice2);
            FreeStream(stream2);
        }
        return -0x6F;
    }
    if (pan != -1) {
        if (pan < 180) {
            pan = volume * pan / 180;
        } else {
            int r = volume * (pan - 180);
            pan = volume;
            volume = r / 180;
        }
    } else {
        pan = volume;
    }
    if (pitch < -1 || pitch > 0x3FFF) {
        FreeVoice(voice);
        FreeStream(stream);
        if (stereo) {
            FreeVoice(voice2);
            FreeStream(stream2);
        }
        return -0x71;
    }
    if (m_batchId + 1 == 64)
        SendBatch();
    m_batchId++;
    m_batchData->m_numCommands++;
    m_batchData->m_commands[m_batchId].m_command = 7;
    *(SoundLoadArgs *)m_batchData->m_commands[m_batchId].m_args = args;
    if (m_batchId + 1 == 64)
        SendBatch();
    m_batchId++;
    m_batchData->m_numCommands++;
    m_batchData->m_commands[m_batchId].m_command = 8;
    RainbowBatchCommand *command = &m_batchData->m_commands[m_batchId];
    command->m_args[0] = handle;
    command->m_args[1] = 1;
    command->m_args[2] = left;
    command->m_args[3] = right;
    command->m_args[4] = pan;
    command->m_args[5] = volume;
    command->m_args[6] = pitch;
    return voice << 16 | voice2;
}

// ---- src/RainbowPS2Snd/PauseVoice.cpp

// The handle's top half packs the bank (bit 0) and the voice (bits 1-5).
int RainbowPS2Snd::PauseVoice(int handle)
{
    if (handle == -1)
        return -0x69;
    if (!MatchSequence(handle))
        return -0x70;
    if (m_batchId + 1 == 64)
        SendBatch();
    int voice = handle >> 16;
    if (m_voiceInfo[voice & 1][(voice >> 1) & 0x1F].m_paused)
        m_voiceInfo[voice & 1][(voice >> 1) & 0x1F].m_paused = 0;
    else
        m_voiceInfo[voice & 1][(voice >> 1) & 0x1F].m_paused = 1;
    m_batchId++;
    m_batchData->m_numCommands++;
    m_batchData->m_commands[m_batchId].m_command = 0xA;
    m_batchData->m_commands[m_batchId].m_args[0] = handle;
    return 0;
}

// ---- src/RainbowPS2Snd/StopVoice.cpp

// Frees one voice slot (bank, voice) unpacked from a voice id.
#define CLEAR(bank, voice)                                  \
    m_voices[bank][voice] = 0;                              \
    if (m_voiceInfo[bank][voice].m_stream)                  \
        m_voiceInfo[bank][voice].m_idleTime = 0.0f;         \
    else                                                    \
        m_voiceInfo[bank][voice].m_idleTime = 3.0f;

int RainbowPS2Snd::StopVoice(int handle, int stereo, int immediate)
{
    RainbowStopArgs args;
    memset(&args, 0, sizeof(args));
    if (handle == -1)
        return -0x69;
    if (!MatchSequence(handle))
        return -0x70;
    int left = handle >> 16;
    int right = handle & 0xFFFF;
    {
        unsigned char voice = (left >> 1) & 0x1F;
        unsigned char bank = left & 1;
        CLEAR(bank, voice);
    }
    if (stereo) {
        unsigned char voice = (right >> 1) & 0x1F;
        unsigned char bank = right & 1;
        CLEAR(bank, voice);
    }
    args.m_handle = handle;
    if (immediate) {
        Rpc(9, (char *)&args, sizeof(args));
    } else {
        if (m_batchId + 1 == 64)
            SendBatch();
        m_batchId++;
        m_batchData->m_numCommands++;
        m_batchData->m_commands[m_batchId].m_command = 9;
        *(RainbowStopArgs *)m_batchData->m_commands[m_batchId].m_args = args;
    }
    return 0;
}

#undef CLEAR

// ---- src/RainbowPS2Snd/InitMovieSound.cpp

int RainbowPS2Snd::InitMovieSound()
{
    int stream = GetAvailableStream();
    if (stream < 0 || stream >= m_numStreams)
        return -0x6B;
#line 2370 "RainbowPS2Snd.cpp"
    ASSERTF(stream >= 0 && stream < m_numStreams, ("InitMovieSound: StreamIndex out of range! (%d)\n", stream));
    m_movieStream = stream;
    int data = stream;
    Rpc(0x1A, (char *)&data, 4);
    return stream << 16;
}

// ---- src/RainbowPS2Snd/MovieSound.cpp

#define VALID_MOVIE_STREAM(s) ((s == 0 || s >= 0x10000) && (unsigned)s < (unsigned)(m_numStreams << 16))
int RainbowPS2Snd::GetMovieSoundBuffer(int stream)
{
#line 2397 "RainbowPS2Snd.cpp"
    ASSERTF(VALID_MOVIE_STREAM(stream), ("GetMovieSoundBuffer: Stream out of range! (%d)\n", stream));
    if (stream < 0)
        return -0x6A;
    int data = stream;
    return Rpc(0x1B, (char *)&data, 4);
}
int RainbowPS2Snd::StartMovieSound(int stream)
{
#line 2411 "RainbowPS2Snd.cpp"
    ASSERTF(VALID_MOVIE_STREAM(stream), ("StartMovieSound: Stream out of range! (%d)\n", stream));
    if (stream < 0)
        return -0x6A;
    int data = stream;
    Rpc(0x1C, (char *)&data, 4);
    return 0;
}
int RainbowPS2Snd::StopMovieSound(int stream)
{
#line 2457 "RainbowPS2Snd.cpp"
    ASSERTF(VALID_MOVIE_STREAM(stream), ("StopMovieSound: Stream out of range! (%d)\n", stream));
    if (stream < 0)
        return -0x6A;
    int data = stream;
    Rpc(0x1E, (char *)&data, 4);
    FreeStream(stream >> 16);
    m_movieStream = -1;
    return 0;
}
int RainbowPS2Snd::GetMovieSoundStatus(int stream)
{
#line 2481 "RainbowPS2Snd.cpp"
    ASSERTF(VALID_MOVIE_STREAM(stream), ("GetMovieSoundStatus: Stream out of range! (%d)\n", stream));
    if (stream < 0)
        return -0x6A;
    return Rpc(0x1F, (char *)&stream, 4);
}

// ---- src/RainbowPS2Snd/PlaySound.cpp

// Starts a sound (or a stream) on a free voice, or a voice pair for stereo; returns
// the voice handle(s), first voice in the high half.
int RainbowPS2Snd::PlaySound(int handle, int stereo, int stream, int c, int volume, int pan, int pitch, int immediate)
{
    RainbowSoundArgs args;
    int voice;
    int voice2;
    voice = 0;
    voice2 = 0;
    if (handle == -1)
        return -0x6A;
    args.m_args[0] = handle;
    args.m_args[1] = stream;
    args.m_args[2] = 0;
    args.m_args[3] = 0;
    args.m_args[4] = -1;
    args.m_args[5] = -1;
    args.m_args[6] = -1;
    int index2;
    int index;
    index = handle >> 16;
    index2 = handle & 0xFFFF;
    int slot;
    if (stream)
        slot = m_streamSlots[index].m_handle;
    else
        slot = m_sounds[index].m_handle;
    if (stereo) {
        if (!GetAvailableVoicePair(index, index2, slot, stream, c, &voice, &voice2)) {
            if (stream) {
                FreeStream(index);
                FreeStream(index2);
            }
            return -0x64;
        }
    } else {
        voice = GetAvailableVoice(index, slot, stream, c);
        if (voice == -1) {
            if (stream)
                FreeStream(index);
            return -0x64;
        }
    }
    args.m_args[2] = voice;
    if (stereo)
        args.m_args[3] = voice2;
    if (m_mono || pan == 0 || pan == 360 || pan == 180)
        pan = -1;
    if (volume < -1 || volume > 100) {
        FreeVoice(voice);
        if (stereo)
            FreeVoice(voice2);
        if (stream) {
            FreeStream(index);
            if (stereo)
                FreeStream(index2);
        }
        return -0x6F;
    }
    if (pan != -1) {
        if (pan < 180) {
            args.m_args[4] = volume * pan / 180;
            args.m_args[5] = volume;
        } else {
            args.m_args[4] = volume;
            args.m_args[5] = volume * (pan - 180) / 180;
        }
    } else {
        args.m_args[4] = volume;
        args.m_args[5] = volume;
    }
    if (pitch < -1 || pitch > 0x3FFF) {
        FreeVoice(voice);
        if (stereo)
            FreeVoice(voice2);
        if (stream) {
            FreeStream(index);
            if (stereo)
                FreeStream(index2);
        }
        return -0x71;
    }
    args.m_args[6] = pitch;
    if (immediate) {
        Rpc(8, (char *)&args, sizeof(args));
    } else {
        if (m_batchId + 1 == 64)
            SendBatch();
        m_batchId++;
        m_batchData->m_numCommands++;
        m_batchData->m_commands[m_batchId].m_command = 8;
        RainbowBatchCommand *command = &m_batchData->m_commands[m_batchId];
        command->m_args[0] = args.m_args[0];
        command->m_args[1] = args.m_args[1];
        command->m_args[2] = args.m_args[2];
        command->m_args[3] = args.m_args[3];
        command->m_args[4] = args.m_args[4];
        command->m_args[5] = args.m_args[5];
        command->m_args[6] = args.m_args[6];
    }
    return voice << 16 | voice2;
}

// ---- src/RainbowPS2Snd/IsSoundPlaying.cpp

// The handle's top 16 bits hold the bank (bit 0), voice (bits 1-5) and
// the sequence number it was issued with (bits 6+).
int RainbowPS2Snd::IsSoundPlaying(int handle, int ask)
{
    if (handle == -1)
        return -0x69;
    int h = handle >> 16;
    unsigned short sequence = (unsigned int)h >> 6;
    unsigned char bank = h & 1;
    unsigned char voice = ((unsigned int)h >> 1) & 0x1F;
    if (m_voices[bank][voice]) {
        if (m_sequence[bank][voice] == sequence) {
            if (ask) {
                RainbowSoundQuery query;
                memset(&query, 0, sizeof(query));
                query.m_handle = handle;
                query.m_x10 = 1;
                return Rpc(0x25, (char *)&query, sizeof(query)) == 1;
            }
            return 1;
        }
    }
    return 0;
}

// ---- src/RainbowPS2Snd/GetSoundDuration.cpp

int RainbowPS2Snd::GetSoundDuration(int handle)
{
    if (handle == -1)
        return -0x6A;
    return m_sounds[handle >> 16].m_duration;
}

// ---- src/RainbowPS2Snd/GetSoundPitch.cpp

int RainbowPS2Snd::GetSoundPitch(int handle)
{
    if (handle == -1)
        return -0x6A;
    return m_sounds[handle >> 16].m_pitch;
}

// ---- src/RainbowPS2Snd/GetStreamDuration.cpp

int RainbowPS2Snd::GetStreamDuration(int stream)
{
    if (stream < 0)
        return -0x6A;
    return m_preloadedStreams.GetItem(stream)->m_duration;
}

// ---- src/RainbowPS2Snd/GetStreamPitch.cpp

int RainbowPS2Snd::GetStreamPitch(int stream)
{
    if (stream < 0)
        return -0x6A;
    return m_preloadedStreams.GetItem(stream)->m_pitch;
}

// ---- src/RainbowPS2Snd/PanVoice.cpp

// Pans a voice: pan is an angle (1..359, 180 = centre) turned into left/right
// volumes; with surround on, the rear arcs also set a surround value.
int RainbowPS2Snd::PanVoice(int handle, int pan)
{
    RainbowVoiceArgs args;  // only some fields set: the rest go out uninitialized
    int left;
    int surround;
    if (handle == -1)
        return -0x69;
    if (!MatchSequence(handle))
        return -0x70;
    if (m_mono)
        return 0;
    surround = 0;
    if (pan == 0 || pan == 360)
        return -0x6D;
    if (m_surround)
        surround = 1;
    if (pan < 180) {
        if (pan < 91) {
            left = (90 - pan) * 100 / 90;
            pan = 100;
        } else {
            if (surround == 1) {
                left = (pan - 90) * 100 / 90;
                pan = 100;
                if (surround == 1)
                    surround = 76;
            } else {
                left = (pan - 91) * 100 / 89;
                pan = 100;
            }
        }
    } else if (pan > 180) {
        if (pan >= 270) {
            pan = (pan - 270) * 100 / 90;
            left = 100;
        } else {
            if (surround == 1) {
                pan = (270 - pan) * 100 / 90;
                left = 100;
                if (surround == 1)
                    surround = 82;
            } else {
                pan = (269 - pan) * 100 / 89;
                left = 100;
            }
        }
    } else {
        pan = left = 100;
        if (surround == 1)
            surround = 82;
    }
    args.m_args[0] = handle;
    args.m_args[3] = left;
    args.m_args[4] = pan;
    args.m_args[5] = surround;
    if (m_batchId + 1 == 64)
        SendBatch();
    m_batchId++;
    m_batchData->m_numCommands++;
    m_batchData->m_commands[m_batchId].m_command = 0x11;
    RainbowBatchCommand *command = &m_batchData->m_commands[m_batchId];
    command->m_args[0] = args.m_args[0];
    command->m_args[1] = args.m_args[1];
    command->m_args[2] = args.m_args[2];
    command->m_args[3] = args.m_args[3];
    command->m_args[4] = args.m_args[4];
    command->m_args[5] = args.m_args[5];
    command->m_args[6] = args.m_args[6];
    command->m_args[7] = args.m_args[7];
    command->m_args[8] = args.m_args[8];
    return 0;
}

// ---- src/RainbowPS2Snd/PitchVoice.cpp

int RainbowPS2Snd::PitchVoice(int handle, int pitch, int a, int b)
{
    RainbowVoiceArgs args;  // only some fields set: the rest go out uninitialized
    if (handle == -1)
        return -0x69;
    if (!MatchSequence(handle))
        return -0x70;
    if (pitch < 0 || pitch > 0x3FFF)
        return -0x71;
    b = b ? b : 0;
    args.m_args[0] = handle;
    args.m_args[6] = a;
    args.m_args[7] = b;
    args.m_args[8] = pitch;
    if (m_batchId + 1 == 64)
        SendBatch();
    m_batchId++;
    m_batchData->m_numCommands++;
    m_batchData->m_commands[m_batchId].m_command = 0x12;
    RainbowBatchCommand *command = &m_batchData->m_commands[m_batchId];
    command->m_args[0] = args.m_args[0];
    command->m_args[1] = args.m_args[1];
    command->m_args[2] = args.m_args[2];
    command->m_args[3] = args.m_args[3];
    command->m_args[4] = args.m_args[4];
    command->m_args[5] = args.m_args[5];
    command->m_args[6] = args.m_args[6];
    command->m_args[7] = args.m_args[7];
    command->m_args[8] = args.m_args[8];
    return 0;
}

// ---- src/RainbowPS2Snd/VolumeVoice.cpp

int RainbowPS2Snd::VolumeVoice(int handle, int volume, int a, int fadeTime)
{
    RainbowVoiceArgs args;
    if (handle == -1)
        return -0x69;
    if (!MatchSequence(handle))
        return -0x70;
    memset(&args, 0, sizeof(args));
    unsigned int id = handle >> 16;
    unsigned short voice = (id >> 1) & 0x1F;
    unsigned short bank = id & 1;
    if (volume < 0 || volume > 100)
        return -0x6F;
    args.m_args[0] = handle;
    if (fadeTime) {
        args.m_args[1] = a;
        args.m_args[2] = fadeTime;
        args.m_args[3] = volume;
        args.m_args[4] = volume;
        if (!volume) {
            m_voiceInfo[bank][voice].m_fading = 1;
            m_voiceInfo[bank][voice].m_fadeRate = -(fadeTime / 1000.0f);
        }
    } else {
        args.m_args[1] = a;
        args.m_args[2] = 0;
        args.m_args[3] = volume;
        args.m_args[4] = volume;
    }
    if (m_batchId + 1 == 64)
        SendBatch();
    m_batchId++;
    m_batchData->m_numCommands++;
    m_batchData->m_commands[m_batchId].m_command = 0x13;
    RainbowBatchCommand *command = &m_batchData->m_commands[m_batchId];
    command->m_args[0] = args.m_args[0];
    command->m_args[1] = args.m_args[1];
    command->m_args[2] = args.m_args[2];
    command->m_args[3] = args.m_args[3];
    command->m_args[4] = args.m_args[4];
    command->m_args[5] = args.m_args[5];
    command->m_args[6] = args.m_args[6];
    command->m_args[7] = args.m_args[7];
    command->m_args[8] = args.m_args[8];
    return 0;
}

// ---- src/RainbowPS2Snd/SettingsVoice.cpp

// Sets a voice's volume, pan (as in PanVoice, scaled by volume) and pitch in one
// batch command; -1 leaves a setting unchanged.
int RainbowPS2Snd::SettingsVoice(int handle, int volume, int pan, int pitch, int a, int b, int c, int fade, int fadeTime)
{
    RainbowVoiceArgs args;
    if (handle == -1)
        return -0x69;
    memset(&args, 0, sizeof(args));
    if (!MatchSequence(handle))
        return -0x70;
    if (m_mono || pan == 0 || pan == 360)
        pan = -1;
    if (pitch > 0x3FFF)
        return -0x71;
    if (volume > 100)
        return -0x6F;
    args.m_args[0] = handle;
    args.m_args[5] = 0;
    args.m_args[3] = -1;
    args.m_args[4] = -1;
    if (pan != -1) {
        if (m_surround)
            args.m_args[5] = 1;
        if (pan < 180) {
            if (pan < 91) {
                args.m_args[3] = volume * (90 - pan) / 90;
                args.m_args[4] = volume;
            } else {
                if (args.m_args[5] == 1) {
                    args.m_args[3] = volume * (pan - 90) / 90;
                    args.m_args[4] = volume;
                    args.m_args[5] = 76;
                } else {
                    args.m_args[3] = volume * (pan - 91) / 89;
                    args.m_args[4] = volume;
                }
            }
        } else if (pan > 180) {
            if (pan >= 270) {
                args.m_args[3] = volume;
                args.m_args[4] = volume * (pan - 270) / 90;
            } else {
                if (args.m_args[5] == 1) {
                    args.m_args[3] = volume;
                    args.m_args[4] = volume * (270 - pan) / 90;
                    args.m_args[5] = 82;
                } else {
                    args.m_args[3] = volume;
                    args.m_args[4] = volume * (269 - pan) / 89;
                }
            }
        } else {
            args.m_args[3] = volume;
            args.m_args[4] = volume;
            if (args.m_args[5] == 1)
                args.m_args[5] = 82;
        }
    } else if (volume != -1) {
        args.m_args[3] = volume;
        args.m_args[4] = volume;
    }
    args.m_args[8] = -1;
    if (pitch != -1) {
        if (fade) {
            args.m_args[6] = 1;
            args.m_args[8] = pitch;
        } else {
            args.m_args[6] = 0;
            args.m_args[8] = pitch;
        }
        if (fadeTime) {
            args.m_args[7] = fadeTime;
            args.m_args[8] = pitch;
        } else {
            args.m_args[7] = 0;
        }
    }
    if (m_batchId + 1 == 64)
        SendBatch();
    m_batchId++;
    m_batchData->m_numCommands++;
    m_batchData->m_commands[m_batchId].m_command = 0x15;
    RainbowBatchCommand *command = &m_batchData->m_commands[m_batchId];
    command->m_args[0] = args.m_args[0];
    command->m_args[1] = args.m_args[1];
    command->m_args[2] = args.m_args[2];
    command->m_args[3] = args.m_args[3];
    command->m_args[4] = args.m_args[4];
    command->m_args[5] = args.m_args[5];
    command->m_args[6] = args.m_args[6];
    command->m_args[7] = args.m_args[7];
    command->m_args[8] = args.m_args[8];
    return 0;
}

// ---- src/RainbowPS2Snd/StopAllVoices.cpp

int RainbowPS2Snd::StopAllVoices()
{
    for (int bank = 0; bank < 2; bank++) {
        for (int voice = 0; voice < 24; voice++) {
            m_voices[bank][voice] = 0;
            if (m_voiceInfo[bank][voice].m_stream)
                m_voiceInfo[bank][voice].m_idleTime = 0.0f;
            else
                m_voiceInfo[bank][voice].m_idleTime = 3.0f;
        }
    }
    if (m_batchId + 1 == 64)
        SendBatch();
    m_batchId++;
    m_batchData->m_numCommands++;
    m_batchData->m_commands[m_batchId].m_command = 0xB;
    return 0;
}

// ---- src/RainbowPS2Snd/VolumeGroup.cpp

int RainbowPS2Snd::VolumeGroup(unsigned int group, int volume)
{
    unsigned int arg0, arg1, arg2;  // unsigned: the int volume is copied, not propagated
    if (volume < 0 || volume > 100)
        return -111;
    arg0 = group;
    arg1 = 0;
    arg2 = volume;
    if (m_batchId + 1 == 64)
        SendBatch();
    m_batchId++;
    m_batchData->m_numCommands++;
    m_batchData->m_commands[m_batchId].m_command = 0x16;
    RainbowBatchCommand *command = &m_batchData->m_commands[m_batchId];
    command->m_args[0] = arg0;
    command->m_args[1] = arg1;
    command->m_args[2] = arg2;
    return 0;
}

// ---- src/RainbowPS2Snd/PauseGroup.cpp

int RainbowPS2Snd::PauseGroup(unsigned int group)
{
    int unused;  // retail sends two uninitialized argument words
    if (m_batchId + 1 == 64)
        SendBatch();
    m_batchId++;
    m_batchData->m_numCommands++;
    m_batchData->m_commands[m_batchId].m_command = 0x17;
    RainbowBatchCommand *command = &m_batchData->m_commands[m_batchId];
    command->m_args[0] = group;
    command->m_args[1] = unused;
    command->m_args[2] = unused;
    return 0;
}

// ---- src/RainbowPS2Snd/StopGroup.cpp

int RainbowPS2Snd::StopGroup(unsigned int group)
{
    int unused;  // retail sends two uninitialized argument words
    for (int bank = 0; bank < 2; bank++) {
        for (int voice = 0; voice < 24; voice++) {
            if (m_voiceInfo[bank][voice].m_group == group) {
                m_voices[bank][voice] = 0;
                if (m_voiceInfo[bank][voice].m_stream)
                    m_voiceInfo[bank][voice].m_idleTime = 0.0f;
                else
                    m_voiceInfo[bank][voice].m_idleTime = 3.0f;
            }
        }
    }
    if (m_batchId + 1 == 64)
        SendBatch();
    m_batchId++;
    m_batchData->m_numCommands++;
    m_batchData->m_commands[m_batchId].m_command = 0x18;
    RainbowBatchCommand *command = &m_batchData->m_commands[m_batchId];
    command->m_args[0] = group;
    command->m_args[1] = unused;
    command->m_args[2] = unused;
    return 0;
}

// ---- src/RainbowPS2Snd/MatchSequence.cpp

// A sound handle carries, in its top 16 bits, the bank (bit 0), the slot
// (bits 1-5) and the sequence number it was issued with (bits 6+).
int RainbowPS2Snd::MatchSequence(int handle)
{
    if (handle == -1)
        return 0;
    unsigned int id = handle >> 16;
    return ((id >> 6) & 0xFFFF) == m_sequence[id & 1][(id >> 1) & 0x1F];
}

// ---- src/RainbowPS2Snd/EndianIntSwap.cpp

// A 4-byte endian reversal.
void RainbowPS2Snd::EndianIntSwap(void *src, void *dst)
{
    ((char *)dst)[0] = ((char *)src)[3];
    ((char *)dst)[1] = ((char *)src)[2];
    ((char *)dst)[2] = ((char *)src)[1];
    ((char *)dst)[3] = ((char *)src)[0];
}

// The ContainerList<_tStreamData *> and <_tLoadedSound *> members follow,
// instantiated from ContainerListMembers.h (CONTAINERLIST_MEMBERS_ALL).
