// BezierKey: one key of a BezierPositionController (time, position and the
// in/out tangents), 0x28 bytes.
#ifndef BEZIERKEY_BEZIERKEY_H
#define BEZIERKEY_BEZIERKEY_H

struct BezierKey {
    float time;             // +0x00
    float position[3];      // +0x04
    float inTangent[3];     // +0x10
    float outTangent[3];    // +0x1C
};                          // size 0x28

#endif
