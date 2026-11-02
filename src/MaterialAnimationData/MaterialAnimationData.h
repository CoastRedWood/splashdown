// MaterialAnimationData: an animated texture's playback state, and the
// shader parameter rows a callback (AnimatedUVCallback) may animate
// (0x30 bytes, PSXShaderDescriptor::ReadAnimationParameters news it).
#ifndef MATERIALANIMATIONDATA_H
#define MATERIALANIMATIONDATA_H

#include "QWORD/QWORD.h"

class ParameterBlock;
class UVAnimationData;

class MaterialAnimationData {
public:
    MaterialAnimationData(ParameterBlock *params);
    ~MaterialAnimationData();
    int UpdateAnimation(int texture);
    void RegisterAnimationCallbackFunction(int (*callback)(void *data, MaterialAnimationData *animation), void *data);

    float m_framesPerSecond;            // +0x00
    union {                             // the ctor clears the byte, then sets the bits
        unsigned char m_flags;          // +0x04
        struct {
            unsigned char m_pingPong : 1;   // bit 0 ("animatedMapReverseLoopOnEnd")
            unsigned char m_looping : 1;    // bit 1 ("animatedMapLooping")
            unsigned char m_reverse : 1;    // bit 2 (playing backwards)
            unsigned char m_stopped : 1;    // bit 3
        } m_bits;
    };
    int m_loopOnEndToFrame;             // +0x08 (-1 = start over)
    float m_frame;                      // +0x0C ("animatedMapInitialFrame", then the current frame)
    int m_lastTime;                     // +0x10 (LpGame time of the last update, -1)
    int (*m_callback)(void *data, MaterialAnimationData *animation);   // +0x14
    void *m_callbackData;               // +0x18
    QWORD *m_parameters[4];             // +0x1C (shader parameter rows in the render-state packet)
    UVAnimationData *m_uvAnimation;     // +0x2C (deleted by the dtor; UVAnimationData has no dtor)
};

#endif
