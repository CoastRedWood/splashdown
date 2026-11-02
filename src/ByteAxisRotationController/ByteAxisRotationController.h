// ByteAxisRotationController: a RotationController whose keys store the
// rotation as a byte angle/axis (8-byte ByteAxisRotationKey: time, then the
// ByteAngleAxis). 0x18 bytes (`new` in Motion::AllocateRotations); the key
// array is at +0x14. The key types stay with the files (their ByteAngleAxis
// copies disagree: angle + ByteVector, angle + 3 chars, or 4 bytes).
#ifndef BYTEAXISROTATIONCONTROLLER_BYTEAXISROTATIONCONTROLLER_H
#define BYTEAXISROTATIONCONTROLLER_BYTEAXISROTATIONCONTROLLER_H

#include "RotationController/RotationController.h"

struct ByteAxisRotationKey;

class ByteAxisRotationController : public RotationController {
public:
    ByteAxisRotationController();

    // --- virtuals in retail slot order ---
    virtual ~ByteAxisRotationController();
    virtual void SetQuaternion(Quaternion *q, float time);
    virtual void GetQuaternion(float time, Quaternion *q);
    virtual void GetKey(int index, float *time, Quaternion *q);
    virtual void SetKey(int index, float *time, Quaternion *q);
    virtual int ValidTime(float time);
    virtual void AllocateKeys(int count);

    ByteAxisRotationKey *m_keys;        // +0x14
};

#endif
