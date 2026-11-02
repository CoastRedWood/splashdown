// SimpleVCR: records and plays back a set of float channels (AddPointer)
// into a frame buffer. A GameObject, 0xD0 bytes (JetskiVCRController's
// members start at +0xD0). Names come from the class's own methods.
#ifndef SIMPLEVCR_SIMPLEVCR_H
#define SIMPLEVCR_SIMPLEVCR_H

#include "GameObject/GameObject.h"

// Frame header written into the recording buffer.
struct VCRFrameHeader {
    float m_time;                       // +0x0
    unsigned char m_major;              // +0x4
    int m_numMinor;                     // +0x8
};

class SimpleVCR : public GameObject {
public:
    SimpleVCR();
    SimpleVCR *Create(unsigned int maxPointers, unsigned int bufferSize, int keepCopy);
    int AddPointer(float rate, float *value, float *source, int a, int b);
    void Reset();
    void Rewind();
    void WriteBuffer(void *data, unsigned int size);
    void ReadBuffer(void *data, unsigned int size);
    void WriteMajorFrame();
    void WriteMinorFrame();
    int ReadFrame();
    void SaveRecording();
    int LoadFromDisk(char *filename);
    void Playback(float dt);
    // Bounds-checked setter, inlined in retail (JetskiVCRController's
    // REPLAYREC/REWforPLAY); the name is a guess.
    void EnableChannel(int i)
    {
        if (i >= 0 && (unsigned int)i < (unsigned int)m_numPointers)
            m_xAC[i] = 1;
    }

    // --- virtuals in retail slot order ---
    virtual ~SimpleVCR();
    virtual int Tick(float dt);

    float m_time;                       // +0x30
    float m_playTime;                   // +0x34
    float m_lastPlayTime;               // +0x38
    float m_frameTime;                  // +0x3C
    float m_majorTimer;                 // +0x40
    float m_minorTimer;                 // +0x44
    float m_x48;                        // +0x48
    float m_minorInterval;              // +0x4C
    float m_playRate;                   // +0x50
    VCRFrameHeader m_frame;             // +0x54
    int m_keepCopy;                     // +0x60
    int m_recording;                    // +0x64
    int m_playing;                      // +0x68
    int m_rewind;                       // +0x6C
    int m_x70;                          // +0x70
    int m_atEnd;                        // +0x74
    char *m_buffer;                     // +0x78
    char *m_writePtr;                   // +0x7C, WriteBuffer() copies here
    char *m_readPos;                    // +0x80
    unsigned int m_x84;                 // +0x84
    unsigned int m_x88;                 // +0x88
    int m_numMinorFrames;               // +0x8C
    unsigned int m_bufferPos;           // +0x90, bytes written so far
    unsigned int m_maxPointers;         // +0x94
    int m_numRequested;                 // +0x98
    int m_numPointers;                  // +0x9C
    float **m_values;                   // +0xA0
    float **m_sources;                  // +0xA4
    char padA8[0xAC - 0xA8];
    int *m_xAC;                         // +0xAC
    int *m_xB0;                         // +0xB0
    float *m_rates;                     // +0xB4
    float *m_invRates;                  // +0xB8
    float *m_lastValues;                // +0xBC
    float *m_previous;                  // +0xC0
    float *m_current;                   // +0xC4
    // Create() stores its bufferSize argument here; WriteMajorFrame/
    // WriteMinorFrame check m_bufferPos + frame size against it.
    unsigned int m_bufferSize;          // +0xC8
    char *m_bufferCopy;                 // +0xCC, filled when m_keepCopy
};

#endif
