// RainbowPS2Snd: the EE side of the IOP sound driver (SIF RPC clients,
// batched voice commands, voice/stream bookkeeping). No vtable, 0x9E74
// bytes; PSXSoundInterface holds one at +0x40. Names come from the class's
// own methods (most common among them where they differ).
#ifndef RAINBOWPS2SND_RAINBOWPS2SND_H
#define RAINBOWPS2SND_RAINBOWPS2SND_H

#include "ContainerList/ContainerList.h"

// The SIF RPC client block (sifrpc.h's layout; Init polls serve).
struct sceSifClientData {
    char pad0[0x24];
    void *serve;                        // +0x24
};

// One queued driver command (0x114 bytes).
struct RainbowBatchCommand {
    int m_command;                      // +0x00
    int m_args[(0x114 - 0x4) / 4];      // +0x04
};

// One batch of driver commands (0x4508 bytes).
struct RainbowBatchData {
    int m_numCommands;                  // +0x00
    int m_x04;                          // +0x04, batch number
    RainbowBatchCommand m_commands[64]; // +0x08
};

// A loaded sound's driver handle, pitch and duration (12 bytes).
struct SoundSlot {
    int m_handle;                       // +0x0
    int m_pitch;                        // +0x4
    unsigned int m_duration;            // +0x8
};

// One hardware voice (0x20 bytes).
struct VoiceInfo {
    int m_stream;                       // +0x00
    int m_x04;                          // +0x04
    int m_fading;                       // +0x08
    int m_paused;                       // +0x0C
    int m_index;                        // +0x10, sound or stream slot
    unsigned int m_group;               // +0x14
    float m_fadeRate;                   // +0x18
    float m_idleTime;                   // +0x1C
};

class RainbowPS2Snd {
public:
    RainbowPS2Snd();
    ~RainbowPS2Snd();
    int Init(unsigned int numVoices);
    void ClearMemory();
    void ShowDriverMemoryUsed();
    int Rpc(int func, char *data, unsigned int size);
    int RpcBatch(int func, char *data, unsigned int size);
    int SendBatch();
    void EndianIntSwap(void *src, void *dst);
    int LoadSound(char *name, unsigned int a, int b, int c, int d);
    short GetAvailableSound();
    int GetSoundDuration(int handle);
    int GetSoundPitch(int handle);
    int MatchSequence(int handle);
    int GetAvailableVoice(int a, int b, int reverse, int c);
    int GetAvailableVoicePair(int index, int index2, int handle, int stream, int c, int *left, int *right);
    void FreeVoice(int handle);
    int PlaySound(int handle, int stereo, int stream, int c, int volume, int pan, int pitch, int immediate);
    int IsSoundPlaying(int handle, int ask);
    int StopVoice(int handle, int stereo, int immediate);
    int StopAllVoices();
    int PauseVoice(int handle);
    int PanVoice(int handle, int pan);
    int PitchVoice(int handle, int pitch, int a, int b);
    int VolumeVoice(int handle, int volume, int a, int fadeTime);
    int SettingsVoice(int handle, int volume, int pan, int pitch, int a, int b, int c, int fade, int fadeTime);
    int PauseGroup(unsigned int group);
    int StopGroup(unsigned int group);
    int VolumeGroup(unsigned int group, int volume);
    void GetPlayStatus(float dt);
    int LoadStreamLibrary(char *name);
    int PreLoadStream(char *name, int *preloadIndex);
    int GetPreLoadedStreamIndex(char *name);
    int LoadAndPlayStream(char *name, unsigned int *handle, unsigned int flags, int a, int b, int c, int d, int e);
    int GetAvailableStream();
    int GetFreeStreams();
    void FreeStream(int stream);
    int GetStreamDuration(int stream);
    int GetStreamPitch(int stream);
    int InitMovieSound();
    int StartMovieSound(int stream);
    int StopMovieSound(int stream);
    int GetMovieSoundStatus(int stream);
    int GetMovieSoundBuffer(int stream);

    sceSifClientData m_batchClients[2]; // +0x000
    sceSifClientData m_client;          // +0x050
    int m_currentBatch;                 // +0x078
    int m_batchCount[2];                // +0x07C
    char m_fileName[0x1C0 - 0x84];      // +0x084, the file being opened
    unsigned int m_sendBuffer[0x50];    // +0x1C0
    int m_recvBuffer[8];                // +0x300
    char pad320[0x340 - 0x320];
    int m_voices[2][24];                // +0x340
    int m_x400;                         // +0x400
    int m_x404;                         // +0x404
    int m_x408;                         // +0x408
    unsigned int m_numStreams;          // +0x40C
    SoundSlot m_sounds[200];            // +0x410
    int *m_streamUsed;                  // +0xD70
    int m_movieStream;                  // +0xD74
    float *m_streamPitch;               // +0xD78
    SoundSlot *m_streamSlots;           // +0xD7C
    unsigned short m_sequence[2][24];   // +0xD80
    VoiceInfo m_voiceInfo[2][24];       // +0xDE0
    unsigned int m_numSounds;           // +0x13E0
    int m_surround;                     // +0x13E4
    int m_mono;                         // +0x13E8
    int m_streamLibraryLoaded;          // +0x13EC
    int m_batchId;                      // +0x13F0
    int m_maxBatchCommands;             // +0x13F4
    char pad13F8[0x1400 - 0x13F8];
    RainbowBatchData m_batchBuffer1;    // +0x1400
    char pad5908[0x5940 - 0x5908];
    RainbowBatchData m_batchBuffer2;    // +0x5940
    RainbowBatchData *m_batchData;      // +0x9E48, the batch being filled
    ContainerList<_tLoadedSound *> m_loadedSounds;     // +0x9E4C
    ContainerList<_tStreamData *> m_preloadedStreams;  // +0x9E60
};

#endif
