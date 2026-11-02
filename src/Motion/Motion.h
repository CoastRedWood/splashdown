// Motion — keyframe tracks; partial layout from matched methods.
#ifndef MOTION_H
#define MOTION_H

extern "C" void *malloc(unsigned int size);

extern "C" void free(void *p);

#include "LinearPositionController/LinearPositionController.h"
#include "BezierPositionController/BezierPositionController.h"
#include "ByteAxisRotationController/ByteAxisRotationController.h"
#include "ShortAxisRotationController/ShortAxisRotationController.h"

class Motion {
public:
    Motion();
    ~Motion();
    void AllocatePositions(int count);
    void AllocateRotations(int count);
    int PastEndOfMotion(float time);

    char m_name[0x51];                  // +0x00
    char m_vueName[0x51];               // +0x51
    char padA2[0xA4 - 0xA2];
    PositionController **m_positions;   // +0xA4
    RotationController **m_rotations;   // +0xA8
    int m_numPositions;                 // +0xAC
    int m_numRotations;                 // +0xB0
    int m_looping;                      // +0xB4
    int m_interpolatePosition;          // +0xB8
    int m_interpolateRotation;          // +0xBC
    int m_rotationType;                 // +0xC0: 0 byte axis, 1 short axis
    int m_positionType;                 // +0xC4: 0 linear, 1 bezier
    int m_fps;                          // +0xC8
    int m_frameCount;                   // +0xCC
    float m_duration;                   // +0xD0
    float m_transitionTime;             // +0xD4
    int m_refCount;                     // +0xD8 (`new Motion` is 0xDC)
};

#endif
