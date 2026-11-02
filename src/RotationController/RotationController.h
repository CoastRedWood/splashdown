// RotationController: the abstract per-bone rotation track of a Motion
// (ByteAxisRotationController implements it). 0x14 bytes: four words
// (the ctor stores 0, 1, 0, 0), then the vptr at +0x10. Slots 1-6 are pure
// (0 in retail's vtable); their signatures are ByteAxisRotationController's.
#ifndef ROTATIONCONTROLLER_H
#define ROTATIONCONTROLLER_H

struct Quaternion;

class RotationController {
public:
    RotationController();
    int m_bone;                         // +0x00, the bone index (Motnctrl.cpp, MotionLibrary.cpp)
    int m_4;                            // +0x04, 1 from the ctor
    int m_numKeys;                      // +0x08
    int m_allocatedKeys;                // +0x0C

    // --- virtuals in retail slot order (retail vtable); vptr at +0x10 ---
    virtual ~RotationController();
    virtual void SetQuaternion(Quaternion *q, float time) = 0;
    virtual void GetQuaternion(float time, Quaternion *q) = 0;
    virtual void GetKey(int index, float *time, Quaternion *q) = 0;
    virtual void SetKey(int index, float *time, Quaternion *q) = 0;
    virtual int ValidTime(float time) = 0;
    virtual void AllocateKeys(int count) = 0;
};

#endif
