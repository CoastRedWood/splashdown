// SimpleVCR.cpp  @ 0x003899A0 (text 0x3899A0-0x38A860)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// SimpleVCR as one translation unit, a TU without an initializer at the end
// of JetskiLoadManager.cpp's tu_map row, after JetskiRaceOverMenus.cpp. Its
// .rodata restarts the labels at @618 and ends with the SimpleVCR RTTI; the
// name is a guess.




#include "GameObject/GameObject.h"
#include "SimpleVCR/SimpleVCR.h"
#include "JetskiGame/JetskiGame.h"
#include "DataAccess/DataAccess.h"

extern "C" void *memcpy(void *dst, const void *src, unsigned int n);
void PSXprintf(const char *fmt, ...);
extern JetskiGame *LpGame;

// ---- src/SimpleVCR/ctor.cpp

SimpleVCR::SimpleVCR() : GameObject(1)
{
    m_numPointers = 0;
    m_maxPointers = 0;
    m_numRequested = 0;
    m_bufferPos = 0;
    m_bufferSize = 0;
    m_buffer = 0;
    m_bufferCopy = 0;
    m_values = 0;
    m_sources = 0;
    m_writePtr = 0;
    m_readPos = 0;
    m_x70 = 0;
    m_recording = 0;
    m_playing = 0;
    m_rewind = 0;
    m_time = 0;
    m_playTime = 0;
    m_lastPlayTime = 0;
    m_frameTime = 0;
    m_majorTimer = 0;
    m_minorTimer = 0;
    m_playRate = 1.0f;
    m_frame.m_time = 0;
    m_frame.m_major = 1;
    m_frame.m_numMinor = 0;
    m_x84 = 0;
    m_x88 = 0;
    m_numMinorFrames = 0;
    m_xAC = 0;
    m_xB0 = 0;
    m_rates = 0;
    m_invRates = 0;
    m_lastValues = 0;
    m_current = 0;
    m_previous = 0;
    m_atEnd = 0;
    m_x48 = 1.0f;
    m_minorInterval = 1.0f / 15.0f;
}


// ---- src/SimpleVCR/dtor.cpp


SimpleVCR::~SimpleVCR()
{
    if (m_buffer)
        delete[] m_buffer;
    if (m_values)
        delete[] m_values;
    if (m_bufferCopy)
        delete[] m_bufferCopy;
    if (m_sources)
        delete[] m_sources;
    if (m_xAC)
        delete[] m_xAC;
    if (m_xB0)
        delete[] m_xB0;
    if (m_rates)
        delete[] m_rates;
    if (m_invRates)
        delete[] m_invRates;
    if (m_lastValues)
        delete[] m_lastValues;
    if (m_current)
        delete[] m_current;
    if (m_previous)
        delete[] m_previous;
}


// ---- src/SimpleVCR/Create.cpp
extern "C" void *memset(void *dst, int c, unsigned int n);


SimpleVCR *SimpleVCR::Create(unsigned int maxPointers, unsigned int bufferSize, int keepCopy)
{
    m_keepCopy = keepCopy;
    m_bufferSize = bufferSize;
    m_maxPointers = maxPointers;
    m_buffer = new char[m_bufferSize];
    m_values = new float *[m_maxPointers];
    m_sources = new float *[m_maxPointers];
    memset(m_buffer, 0, m_bufferSize);
    if (m_keepCopy) {
        m_bufferCopy = new char[m_bufferSize];
        memset(m_bufferCopy, 0, m_bufferSize);
    }
    m_xAC = new int[m_maxPointers];
    m_xB0 = new int[m_maxPointers];
    m_rates = new float[m_maxPointers];
    m_invRates = new float[m_maxPointers];
    m_lastValues = new float[m_maxPointers];
    m_current = new float[m_maxPointers];
    m_previous = new float[m_maxPointers];
    for (unsigned int i = 0; i < m_maxPointers; i++) {
        m_values[i] = 0;
        m_sources[i] = 0;
        m_xAC[i] = 1;
        m_xB0[i] = 1;
        m_rates[i] = 0.0f;
        m_invRates[i] = 0.0f;
        m_lastValues[i] = 0.0f;
        m_current[i] = 0.0f;
        m_previous[i] = 0.0f;
    }
    Rewind();
    return this;
}


// ---- src/SimpleVCR/AddPointer.cpp

int SimpleVCR::AddPointer(float rate, float *value, float *source, int a, int b)
{
    m_numRequested++;
    if (rate > 0.0f && m_numPointers < m_maxPointers && !m_x70) {
        m_values[m_numPointers] = value;
        if (source)
            m_sources[m_numPointers] = source;
        else
            m_sources[m_numPointers] = value;
        m_xAC[m_numPointers] = b;
        m_xB0[m_numPointers] = a;
        m_rates[m_numPointers] = rate;
        m_invRates[m_numPointers] = 1.0f / rate;
        m_numPointers++;
        return m_numPointers - 1;
    }
    PSXprintf("SimpleVCR: Failed to add pointer to VCR list. Require %d pointers.\n", m_numRequested);
    return -1;
}


// ---- src/SimpleVCR/Reset.cpp

void SimpleVCR::Reset()
{
    m_atEnd = 0;
    m_recording = 0;
    m_playing = 0;
    m_rewind = 0;
    Rewind();
}


// ---- src/SimpleVCR/Rewind.cpp
void SimpleVCR::Rewind()
{
    m_atEnd = 0;
    m_bufferPos = 0;
    m_time = 0;
    m_majorTimer = 0;
    m_minorTimer = 0;
    m_playTime = 0;
    m_frame.m_time = 0;
    m_frame.m_major = 1;
    m_frame.m_numMinor = 0;
    m_x84 = m_maxPointers * 4 + 12;
    m_x88 = m_maxPointers * 5 + 12;
    m_numMinorFrames = 0;
    m_writePtr = m_buffer;
    if (m_keepCopy)
        m_readPos = m_bufferCopy;
    else
        m_readPos = m_buffer;
}


// ---- src/SimpleVCR/WriteBuffer.cpp
extern "C" void *memcpy(void *dst, const void *src, unsigned int n);

void SimpleVCR::WriteBuffer(void *src, unsigned int size)
{
    memcpy(m_writePtr, src, size);
    m_writePtr += size;
    m_bufferPos += size;
    m_numMinorFrames += size;
}


// ---- src/SimpleVCR/ReadBuffer.cpp
extern "C" void *memcpy(void *dst, const void *src, unsigned int n);


void SimpleVCR::ReadBuffer(void *dst, unsigned int size)
{
    memcpy(dst, m_readPos, size);
    m_readPos += size;
}


// ---- src/SimpleVCR/Tick.cpp

int SimpleVCR::Tick(float dt)
{
    static int TickID = -1;     // left over from a compiled-out profiling meter
    if (LpGame->m_paused)
        return 1;
    m_x70 = 1;
    if (m_recording) {
        m_time += dt;
        if (m_majorTimer == 0.0f) {
            WriteMajorFrame();
            m_minorTimer = m_minorInterval;
        } else if (m_minorTimer <= 0.0f) {
            WriteMinorFrame();
            m_minorTimer = m_minorInterval;
        }
        m_majorTimer -= dt;
        m_minorTimer -= dt;
    }
    if (m_playing)
        Playback(dt);
    GameObject::Tick(dt);
    return 1;
}


// ---- src/SimpleVCR/WriteMajorFrame.cpp

void SimpleVCR::WriteMajorFrame()
{
    if (m_bufferPos + m_x84 + sizeof(VCRFrameHeader) < m_bufferSize) {
        VCRFrameHeader header;
        header.m_time = m_time;
        header.m_major = 1;
        header.m_numMinor = m_numMinorFrames;
        m_numMinorFrames = 0;
        WriteBuffer(&header, sizeof(header));
        for (unsigned int i = 0; i < m_numPointers; i++) {
            WriteBuffer(m_values[i], 4);
            m_lastValues[i] = *m_values[i];
        }
        VCRFrameHeader end;
        end.m_time = -999.0f;
        end.m_numMinor = m_numMinorFrames;
        end.m_major = 1;
        memcpy(m_writePtr, &end, sizeof(end));
    }
}


// ---- src/SimpleVCR/WriteMinorFrame.cpp
#pragma fast_fptosi on
extern "C" void *memcpy(void *dst, const void *src, unsigned int n);
void PSXprintf(const char *fmt, ...);


extern JetskiGame *LpGame;

// Writes a minor frame: per value, the change since the last written value
// as a signed byte in units of the inverse rate, or 0x7F and the full value
// when the change doesn't fit.
void SimpleVCR::WriteMinorFrame()
{
    if (m_bufferPos + m_x88 + sizeof(VCRFrameHeader) < m_bufferSize) {
        VCRFrameHeader header;
        VCRFrameHeader end;
        float value;
        char escape;
        char delta;
        unsigned int i;
        header.m_time = m_time;
        header.m_major = 0;
        header.m_numMinor = m_numMinorFrames;
        m_numMinorFrames = 0;
        WriteBuffer(&header, sizeof(header));
        for (i = 0; i < m_numPointers; i++) {
            value = *m_values[i];
            float diff = value - m_lastValues[i];
            int d = (int)(diff * m_rates[i] + 0.5f * (float)(diff < 0.0f ? -1 : 1));
            if (d >= 127 || d < -127) {
                escape = 0x7F;
                WriteBuffer(&escape, 1);
                WriteBuffer(&value, 4);
                m_lastValues[i] = value;
            } else {
                delta = d;
                WriteBuffer(&delta, 1);
                m_lastValues[i] = value + delta * m_invRates[i] - diff;
            }
        }
        end.m_time = -999.0f;
        end.m_numMinor = m_numMinorFrames;
        end.m_major = 1;
        memcpy(m_writePtr, &end, sizeof(end));
    }
}


// ---- src/SimpleVCR/ReadFrame.cpp
extern "C" void *memcpy(void *dst, const void *src, unsigned int n);
void PSXprintf(const char *fmt, ...);


extern JetskiGame *LpGame;

// Reads the next recorded frame: a major frame stores every value, a minor
// frame stores per-value deltas scaled by the inverse rate (0x7F escapes to a
// full value).
int SimpleVCR::ReadFrame()
{
    VCRFrameHeader header;
    unsigned int i;
    memcpy(&header, m_readPos, sizeof(header));
    if (header.m_time == -999.0f)
        return 1;
    m_frameTime = m_frame.m_time;
    ReadBuffer(&m_frame, sizeof(m_frame));
    if (m_frame.m_major == 1) {
        for (i = 0; i < m_numPointers; i++) {
            m_previous[i] = m_current[i];
            ReadBuffer(&m_current[i], 4);
            if (!m_frame.m_numMinor)
                m_previous[i] = m_current[i];
        }
    } else {
        for (i = 0; i < m_numPointers; i++) {
            char delta;
            m_previous[i] = m_current[i];
            ReadBuffer(&delta, 1);
            if (delta == 0x7F)
                ReadBuffer(&m_current[i], 4);
            else
                m_current[i] += delta * m_invRates[i];
        }
    }
    return 0;
}


// ---- src/SimpleVCR/SaveRecording.cpp
extern "C" void *memcpy(void *dst, const void *src, unsigned int n);


void SimpleVCR::SaveRecording()
{
    if (m_keepCopy)
        memcpy(m_bufferCopy, m_buffer, m_bufferSize);
}


// ---- src/SimpleVCR/LoadFromDisk.cpp
void PSXprintf(const char *fmt, ...);

extern DataAccess *LpDataAccess;


int SimpleVCR::LoadFromDisk(char *filename)
{
    if (m_keepCopy) {
        long handle = LpDataAccess->FOpen(filename, "rb");
        if (handle == -1) {
            PSXprintf("recorded vcrfile '%s' not found\n", filename);
            return 0;
        }
        LpDataAccess->FRead(handle, m_bufferCopy, m_bufferSize, 1);
        LpDataAccess->FClose(handle);
        return 1;
    }
    return 0;
}


// ---- src/SimpleVCR/Playback.cpp
extern "C" void *memcpy(void *dst, const void *src, unsigned int n);
void PSXprintf(const char *fmt, ...);


extern JetskiGame *LpGame;

static inline float Fmax(float a, float b)
{
    float r;
    asm("max.s $f8, %1, %2; swc1 $f8, 0(%0)" : : "r"(&r), "f"(a), "f"(b) : "$f8", "memory");
    return r;
}

// Advances the playback clock, reads frames up to it and interpolates every
// recorded value between the last two frames.
void SimpleVCR::Playback(float dt)
{
    if (m_rewind) {
        float time = m_playTime;
        Rewind();
        m_playTime = time - dt * m_playRate;
        m_playTime = Fmax(m_playTime, 0.0f);
    } else {
        m_playTime += dt * m_playRate;
    }
    if (m_atEnd || m_lastPlayTime == m_playTime)
        return;
    m_lastPlayTime = m_playTime;
    while (!m_atEnd && !(m_playTime < m_frame.m_time))
        m_atEnd = ReadFrame();
    if (m_frame.m_time == m_frameTime)
        return;
    float t = (m_playTime - m_frameTime) / (m_frame.m_time - m_frameTime);
    for (unsigned int i = 0; i < m_numPointers; i++) {
        float value;
        if (m_xB0[i])
            value = m_previous[i] + t * (m_current[i] - m_previous[i]);
        else
            value = m_current[i];
        if (m_sources[i] && m_xAC[i])
            *m_sources[i] = value;
    }
}

