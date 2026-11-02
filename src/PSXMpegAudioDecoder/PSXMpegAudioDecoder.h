// PSXMpegAudioDecoder: the movie audio stream's ring buffer and its trip to
// the IOP (Sony ezmovie's AudioDec; 0x80 bytes, PSXMediaControl::Create
// news it).
#ifndef PSXMPEGAUDIODECODER_H
#define PSXMPEGAUDIODECODER_H

class PSXMediaControl;

// Sony's ezmovie audio stream headers.
struct SpuStreamHeader {
    char id[4];
    int size;
    int type;
    int rate;
    int ch;
    int interSize;
    int loopStart;
    int loopEnd;
};

struct SpuStreamBody {
    char id[4];
    int size;
};

class PSXMpegAudioDecoder {
public:
    PSXMpegAudioDecoder(PSXMediaControl *control);
    ~PSXMpegAudioDecoder();
    void Initialize();
    void Terminate();
    int Reset();
    int Start();
    int Stop();
    int IsPreset();
    void BeginRingBufferPut(unsigned char **ptr1, int *size1, unsigned char **ptr2, int *size2);
    void EndRingBufferPut(int size);
    int SendToIOP();                      // bytes sent
    int SendDataToIOP(int iopAddress, unsigned char *data, int size);
    void IopGetArea(int *pd0, int *d0, int *pd1, int *d1, int pos);
    int SendAreaToIop(int pd0, int d0, int pd1, int d1, unsigned char *ps0, int s0, unsigned char *ps1, int s1);

    int m_state;                          // +0x00 (0 = reading the header, 1 = preset, 2 = playing, 4 = stopped)
    SpuStreamHeader m_header;             // +0x04
    SpuStreamBody m_body;                 // +0x24
    int m_hdrCount;                       // +0x2C (header bytes gathered)
    unsigned char *m_buffer;              // +0x30 (ring buffer, memalign'd 0xC000)
    int m_writePos;                       // +0x34
    int m_used;                           // +0x38
    int m_bufferSize;                     // +0x3C (0xC000)
    int m_40;                             // +0x40
    int m_iopBuff;                        // +0x44 (IOP address)
    char pad48[0x4C - 0x48];
    int m_iopBuffSize;                    // +0x4C (0x6000)
    int m_50;                             // +0x50 (0x800)
    int m_iopLastPos;                     // +0x54
    int m_58;                             // +0x58
    int m_bytesRead;                      // +0x5C
    float m_playTime;                     // +0x60, m_bytesRead / 96 (SendToIOP)
    int m_64;                             // +0x64
    float m_68;                           // +0x68 (Start stores timeGetTime())
    float m_6C;                           // +0x6C (Reset stores timeGetTime())
    int m_stream;                         // +0x70 (-1)
    int m_soundBuffer;                    // +0x74 (IOP address, RainbowPS2Snd::GetMovieSoundBuffer)
    int m_dataSize;                       // +0x78
    PSXMediaControl *m_control;           // +0x7C
};

#endif
