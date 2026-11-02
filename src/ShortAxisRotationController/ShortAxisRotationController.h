#ifndef SHORTAXISROTATIONCONTROLLER_H
#define SHORTAXISROTATIONCONTROLLER_H

extern "C" void *realloc(void *p, unsigned int size);
extern "C" void free(void *p);

struct Quaternion;

#include "RotationController/RotationController.h"

// A packed rotation: angle in 4096 steps per turn (AngleAxis.cpp), unit axis * 32767.
struct ShortAngleAxis {
    unsigned short angle;
    short x, y, z;
};

struct ShortAxisRotationKey {       // 12 bytes
    float m_time;
    ShortAngleAxis m_rotation;
};

class ShortAxisRotationController : public RotationController {
public:
    ShortAxisRotationController();
    virtual ~ShortAxisRotationController();
    virtual void SetQuaternion(Quaternion *, float);
    virtual void GetQuaternion(float, Quaternion *);
    virtual void GetKey(int, float *, Quaternion *);
    virtual void SetKey(int, float *, Quaternion *);
    virtual int ValidTime(float);
    virtual void AllocateKeys(int);

    ShortAxisRotationKey *m_keys;   // +0x14
};

#endif
