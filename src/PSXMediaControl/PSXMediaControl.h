// PSXMediaControl: plays an MPEG stream through the IPU (0x1F8 bytes; UIPage
// and JetskiScreenVideo new it).
#ifndef PSXMEDIACONTROL_PSXMEDIACONTROL_H
#define PSXMEDIACONTROL_PSXMEDIACONTROL_H

#include "MediaControl/MediaControl.h"

class GsTextureTransfer;
class PSXMpegStreamFile;
class PSXMpegInputBuffer;
class PSXMpegVideoDecoder;
class PSXMpegAudioDecoder;
struct PSXMpegVideoOutputBufferData;

class PSXMediaControl : public MediaControl {
public:
    PSXMediaControl(int enabled);
    PSXMediaControl *Create(char *name, int a, int b, int width, int height, void (*callback)(void *), void *data);
    void SetColor(unsigned char e4, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
    {
        m_xE4 = e4;
        m_red = r;
        m_green = g;
        m_blue = b;
        m_alpha = a;
    }
    int Initialize(char *name);
    void Terminate();
    void SendAudioData();
    void BltCurrentDecodedFrame();
    void LoopVideo();

    PSXMpegStreamFile *m_streamFile;        // +0x48
    PSXMpegInputBuffer *m_inputBuffer;      // +0x4C
    PSXMpegVideoDecoder *m_videoDecoder;    // +0x50
    PSXMpegAudioDecoder *m_audioDecoder;    // +0x54
    int m_initialized;                      // +0x58
    int m_hasAudio;                         // +0x5C
    int m_x60;                              // +0x60
    int m_paused;                           // +0x64
    int m_stopped;                          // +0x68
    char m_name[0x34];                      // +0x6C
    int m_width;                            // +0xA0
    int m_height;                           // +0xA4
    GsTextureTransfer *m_textureTransfer;   // +0xA8
    int m_xAC;                              // +0xAC
    int m_xB0;                              // +0xB0
    int m_finished;                         // +0xB4
    int m_decodeDone;                       // +0xB8
    int m_xBC;                              // +0xBC
    PSXMpegVideoOutputBufferData *m_currentFrame;   // +0xC0
    int m_newFrame;                         // +0xC4
    int m_loop;                             // +0xC8
    char padCC[0xD0 - 0xCC];
    int m_xD0;                              // +0xD0
    int m_xD4;                              // +0xD4
    int m_xD8;                              // +0xD8
    int m_xDC;                              // +0xDC
    int m_xE0;                              // +0xE0
    unsigned char m_xE4;                    // +0xE4
    unsigned char m_red;                    // +0xE5
    unsigned char m_green;                  // +0xE6
    unsigned char m_blue;                   // +0xE7
    unsigned char m_alpha;                  // +0xE8
    int m_updateDmaPackets;                 // +0xEC
    int m_decodeError;                      // +0xF0
    char m_errorText[0x40];                 // +0xF4
    char pad134[0x1F8 - 0x134];

    // --- virtuals in retail slot order ---
    virtual ~PSXMediaControl();
    virtual int Tick(float dt);
    virtual int PrepareGeometry();
    virtual int RenderPre3D();
    virtual int Pause(int pause);
    virtual int Start();
    // void in retail (Stop leaves $v0 alone); int because MediaControl.h says so.
    virtual int Stop();
    virtual int Seek(int time);
    virtual int IsPlaying();
    virtual int GetTime(int *time);
    virtual int GetDurationTime(int *time);
};

#endif
