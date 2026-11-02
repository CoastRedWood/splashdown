// PSXMediaControl.cpp  @ 0x002CC9B0 (text 0x2CC9B0-0x2CD830)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp)
//
// Retail's PSXMediaControl.cpp as one translation unit: a TU without a
// static initializer inside PSXWater.cpp's tu_map row, after
// MediaControl.cpp. Its .rodata restarts the labels at @660 after
// MediaControl's RTTI ("PSXMediaControl.cpp" ASSERT, then the
// PSXMediaControl/MediaControl RTTI). `make link` places its data as well
// as its code.


#include "PSXMediaControl/PSXMediaControl.h"
#include "GsTextureTransfer/GsTextureTransfer.h"
#include "PSXMpegVideoDecoder/PSXMpegVideoDecoder.h"
#include "PSXMpegVideoDecoder/PSXMpegDefs.h"
#include "PSXMpegVideoDecoder/libmpeg.h"
#include "PSXMpegAudioDecoder/PSXMpegAudioDecoder.h"
#include "PSXMpegStreamFile/PSXMpegStreamFile.h"
#include "PSXMpegInputBuffer/PSXMpegInputBuffer.h"
#include "PSXVideoCard/PSXVideoCard.h"
#include "GameObject/GameObject.h"
#include "PSXMpegVideoOutputBuffer/PSXMpegVideoOutputBuffer.h"
#include "soultree_assert_block.h"

// Defined here for its retail .bss slot.
int TotalNumberOfTickSeconds;


extern "C" char *strcpy(char *dst, const char *src);
int GsSyncPath(int mode, GS_PATH path);

extern int AllowIPUInterrupts;
extern int TotalNumberOfTickSeconds;
extern PSXVideoCard *LpVideoCard;

#define MIN(a, b) ((a) < (b) ? (a) : (b))

// ---- src/PSXMediaControl/ctor.cpp


PSXMediaControl::PSXMediaControl(int id) : MediaControl(id)
{
    m_videoDecoder = 0;
    m_audioDecoder = 0;
    m_streamFile = 0;
    m_inputBuffer = 0;
    m_hasAudio = 1;
    m_x60 = 0;
    m_stopped = 1;
    m_paused = 0;
    m_initialized = 1;
    AllowIPUInterrupts = 0;
    m_textureTransfer = (new GsTextureTransfer)->Create(0x609);
    m_xAC = 0;
    m_xB0 = 0;
    m_finished = 0;
    m_decodeDone = 0;
    m_currentFrame = 0;
    m_xD0 = 0;
    m_xD4 = 0;
    m_xD8 = 0;
    m_xDC = 0;
    m_xE0 = 0;
    m_xE4 = 0;
    m_red = 1;
    m_green = 0;
    m_blue = 1;
    m_alpha = 0x80;
    m_updateDmaPackets = 1;
    m_decodeError = 0;
    m_errorText[0] = 0;
    m_width = 0;
    m_height = 0;
    m_xBC = 0;
    m_newFrame = 0;
    m_loop = 0;
}


// ---- src/PSXMediaControl/dtor.cpp
PSXMediaControl::~PSXMediaControl()
{
    if (m_initialized)
        Terminate();
    if (m_videoDecoder) {
        delete m_videoDecoder;
        m_videoDecoder = 0;
    }
    if (m_audioDecoder) {
        delete m_audioDecoder;
        m_audioDecoder = 0;
    }
    if (m_streamFile) {
        delete m_streamFile;
        m_streamFile = 0;
    }
    if (m_inputBuffer) {
        delete m_inputBuffer;
        m_inputBuffer = 0;
    }
    AllowIPUInterrupts = 0;
    if (m_textureTransfer) {
        delete m_textureTransfer;
        m_textureTransfer = 0;
    }
    m_currentFrame = 0;
}


// ---- src/PSXMediaControl/Create.cpp
// Sets up the decoders for a movie (callback and data are unused).
PSXMediaControl *PSXMediaControl::Create(char *name, int a, int b, int width, int height, void (*callback)(void *), void *data)
{
    strcpy(m_name, name);
    m_initialized = 1;
    int screenWidth = LpVideoCard->m_width;
    int screenHeight = LpVideoCard->m_height;
    m_width = MIN(width, screenWidth);
    m_height = MIN(height, screenHeight);
    if (!m_width)
        m_width = screenWidth;
    if (!m_height)
        m_height = screenHeight;
    m_xD0 = a;
    m_xD4 = b;
    m_xE0 = 0;
    m_xE4 = 0;
    m_red = 2;
    m_green = 2;
    m_blue = 2;
    m_alpha = 0x80;
    m_updateDmaPackets = 1;
    m_decodeError = 0;
    m_errorText[0] = 0;
    if (m_videoDecoder) {
        delete m_videoDecoder;
        m_videoDecoder = 0;
    }
    m_videoDecoder = new PSXMpegVideoDecoder(m_width, m_height, this);
    if (m_audioDecoder) {
        delete m_audioDecoder;
        m_audioDecoder = 0;
    }
    m_audioDecoder = new PSXMpegAudioDecoder(this);
    if (m_streamFile) {
        delete m_streamFile;
        m_streamFile = 0;
    }
    m_streamFile = new PSXMpegStreamFile;
    m_inputBuffer = new PSXMpegInputBuffer(m_streamFile, m_videoDecoder, m_audioDecoder);
    m_currentFrame = 0;
    m_newFrame = 0;
    m_loop = 0;
    return this;
}


// ---- src/PSXMediaControl/Tick.cpp

int PSXMediaControl::Tick(float dt)
{
    GameObject::Tick(dt);
    if (m_stopped && !m_xBC)
        return 1;
    if (m_finished) {
        if (m_hasAudio)
            m_audioDecoder->Reset();
        if (m_xBC)
            Terminate();
        m_stopped = 1;
        m_xAC = 0;
        m_xBC = 0;
    } else {
        SendAudioData();
    }
    return 1;
}


// ---- src/PSXMediaControl/BltCurrentDecodedFrame.cpp
void PSXMediaControl::BltCurrentDecodedFrame()
{
    if (m_xE0 && m_currentFrame) {
        m_textureTransfer->TransferVideoFrameToFrameBuffer(
            m_currentFrame->m_pixels, m_xD8, m_xDC, m_xD0, m_xD4, m_xDC,
            m_currentFrame->m_texturePacket, m_currentFrame->m_bitbltbuf, m_currentFrame->m_spritePacket,
            m_currentFrame->m_alpha, m_currentFrame->m_topLeft, m_currentFrame->m_bottomRight,
            m_xE4, m_red, m_green, m_blue, m_alpha);
    }
}


// Retail creates "%s\n" (@660) just before LoopVideo's literal (@665), long
// before RenderPre3D prints the error text with it: a function here that
// nothing called, which retail's linker stripped. link_c leaves its section
// out.
static void StrippedPrintText(char *text)
{
    PSXprintf("%s\n", text);
}


// ---- src/PSXMediaControl/LoopVideo.cpp

void PSXMediaControl::LoopVideo()
{
    PSXprintf("Looping mpeg video\n");
    m_streamFile->Reset();
    m_videoDecoder->Reset();
    m_audioDecoder->Reset();
    m_xB0 = 0;
    m_finished = 0;
    m_decodeDone = 0;
    m_decodeError = 0;
    m_xBC = 0;
}


// ---- src/PSXMediaControl/PrepareGeometry.cpp


int PSXMediaControl::PrepareGeometry()
{
    if (m_stopped || m_paused)
        return 1;
    if (!m_xAC) {
        m_videoDecoder->m_output->Reset();
        m_inputBuffer->PreBuffer();
        m_xAC = 1;
        m_xB0 = 0;
        while (!m_videoDecoder->m_output->IsFull()) {
            m_videoDecoder->DecodeBitstream();
            SendAudioData();
        }
    }
    return 1;
}


// ---- src/PSXMediaControl/RenderPre3D.cpp

#define BLT_FRAME(frame)                                                                   \
    m_textureTransfer->TransferVideoFrameToFrameBuffer(                                     \
        (frame)->m_pixels, m_xD8, m_xDC, m_xD0, m_xD4, m_xDC,                                \
        (frame)->m_texturePacket, (frame)->m_bitbltbuf, (frame)->m_spritePacket,             \
        (frame)->m_alpha, (frame)->m_topLeft, (frame)->m_bottomRight,                        \
        m_xE4, m_red, m_green, m_blue, m_alpha)

int PSXMediaControl::RenderPre3D()
{
    static int printFrameNumber = 0;
    GameObject::RenderPre3D();
    if (m_stopped)
        return 1;
    if (!GsSyncPath(0, (GS_PATH)3)) {
        m_xD8 = m_videoDecoder->m_mpeg->width;
        m_xDC = m_videoDecoder->m_mpeg->height;
        PSXMpegVideoOutputBufferData *frame = m_videoDecoder->m_output->Read();
        if (frame) {
#line 347 "PSXMediaControl.cpp"
            ASSERT(frame->m_pixels, "RenderPre3D: VideoOutputBuffer read failed");
            if (m_updateDmaPackets) {
                m_videoDecoder->m_output->UpdateDmaPackets(m_xD0, m_xD4, m_xD8, m_xDC, m_xDC);
                m_updateDmaPackets = 0;
            }
            if (!m_xE0) {
                if (printFrameNumber)
                    PSXprintf("Blting Frame Number %d at address 0x%x\n", frame->m_frame, frame);
                if (!m_decodeError)     // (the redundant cast gives retail's load order)
                    m_textureTransfer->TransferVideoFrameToFrameBuffer(
                        (sceIpuRGB32 *)frame->m_pixels, m_xD8, m_xDC, m_xD0, m_xD4, m_xDC,
                        frame->m_texturePacket, frame->m_bitbltbuf, frame->m_spritePacket,
                        frame->m_alpha, frame->m_topLeft, frame->m_bottomRight,
                        m_xE4, m_red, m_green, m_blue, m_alpha);
                else if (m_currentFrame)
                    BLT_FRAME(m_currentFrame);
            }
            if (m_decodeError) {
                if (m_decodeError) {
                    PSXprintf("PSXMediaControl::RenderPre3D Video Decode Error!!  FRAME NUMBER %d\n",
                              m_currentFrame->m_frame);
                    PSXprintf("%s\n", m_errorText);
                }
            }
            m_currentFrame = frame;
            m_decodeError = 0;
            m_errorText[0] = 0;
            if (!m_paused && !m_decodeDone) {
                int state = m_videoDecoder->DecodeBitstream();
                if (state == PSXMpegDefs::VideoDecoderState_END || state == -1) {
                    if (m_loop) {
                        LoopVideo();
                    } else {
                        m_decodeDone = 1;
                        m_finished = 1;
                        m_stopped = 1;
                        m_xBC = 1;
                        m_newFrame = 0;
                        PSXprintf("PSXMediaControl::RenderPre3D : reached end of movie! FRAME # %d\n",
                                  m_currentFrame->m_frame);
                    }
                }
                SendAudioData();
            }
        } else {
            if (m_loop) {
                m_videoDecoder->DecodeBitstream();
            } else {
                m_finished = 1;
                m_stopped = 1;
                m_xBC = 1;
                PSXprintf("PSXMediaControl::RenderPre3D : reached end of movie! FRAME #%d\n",
                          m_currentFrame->m_frame);
            }
            m_newFrame = 1;
            SendAudioData();
        }
        if (m_newFrame) {
            if (!m_xE0 && m_currentFrame)
                BLT_FRAME(m_currentFrame);
            m_newFrame = 0;
        }
        return 1;
    }
    return 1;
}


// ---- src/PSXMediaControl/Start.cpp
int PSXMediaControl::Start()
{
    if (!m_initialized)
        return 0;
    if (Initialize(m_name) == 1) {
        m_stopped = 0;
        m_initialized = 1;
    } else {
        m_stopped = 1;
        m_initialized = 0;
        return 0;
    }
    return 1;
}


// ---- src/PSXMediaControl/Stop.cpp
int PSXMediaControl::Stop()    // void in retail: no return, $v0 left as is
{
    if (!m_stopped) {
        m_videoDecoder->m_state = PSXMpegDefs::VideoDecoderState_ABORT;
        m_audioDecoder->Reset();
        m_videoDecoder->Reset();
        Terminate();
        m_stopped = 1;
    }
}


// ---- src/PSXMediaControl/IsPlaying.cpp

int PSXMediaControl::IsPlaying()
{
    int playing = 1;
    if (m_stopped)
        playing = 0;
    else if (m_paused)
        playing = 0;
    return playing;
}


// ---- src/PSXMediaControl/Pause.cpp

int PSXMediaControl::Pause(int pause)
{
    if (pause)
        m_paused = 1;
    else
        m_paused = 0;
    return 0;
}


// ---- src/PSXMediaControl/Initialize.cpp
int PSXMediaControl::Initialize(char *name)
{
    if (m_finished && m_xBC) {
        if (m_hasAudio)
            m_audioDecoder->Reset();
        Terminate();
        m_stopped = 1;
        m_xAC = 0;
        m_xBC = 0;
    }
    if (!m_streamFile->Open(name))
        return 0;
    sceMpegInit();
    m_videoDecoder->Initialize();
    m_audioDecoder->Initialize();
    sceMpegAddStrCallback(m_videoDecoder->m_mpeg, 0, 0, PSXMpegVideoDecoder::VideoStreamCallback, this);
    // The (redundant) cast is in retail: it orders the argument setup.
    if (m_hasAudio)
        sceMpegAddStrCallback(m_videoDecoder->m_mpeg, 2, 0, (sceMpegStrCallback)PSXMpegVideoDecoder::AudioStreamCallback, this);
    m_xAC = 0;
    m_xB0 = 0;
    m_finished = 0;
    m_decodeDone = 0;
    LpVideoCard->EnableVSyncTo30Hz(0);
    TotalNumberOfTickSeconds = 0;
    m_currentFrame = 0;
    m_xD8 = 0;
    m_xDC = 0;
    m_updateDmaPackets = 1;
    m_decodeError = 0;
    m_xBC = 0;
    return 1;
}


// ---- src/PSXMediaControl/Terminate.cpp

void PSXMediaControl::Terminate()
{
    if (m_videoDecoder)
        m_videoDecoder->Terminate();
    if (m_audioDecoder)
        m_audioDecoder->Terminate();
    if (m_streamFile)
        m_streamFile->Close();
    m_currentFrame = 0;
    if (LpVideoCard)
        LpVideoCard->ResetVSyncToDefault();
}


// ---- src/PSXMediaControl/SendAudioData.cpp


void PSXMediaControl::SendAudioData()
{
    if (m_hasAudio) {
        m_audioDecoder->SendToIOP();
        if (!m_xB0)
            m_xB0 = m_audioDecoder->Start();
    }
}


// ---- src/PSXMediaControl/Seek.cpp

int PSXMediaControl::Seek(int)
{
    return 0;
}


// ---- src/PSXMediaControl/GetTime.cpp

int PSXMediaControl::GetTime(int *)
{
    return 0;
}


// ---- src/PSXMediaControl/GetDurationTime.cpp

int PSXMediaControl::GetDurationTime(int *)
{
    return 0;
}

