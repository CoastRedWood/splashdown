// KeyFrameController.cpp  @ 0x0034FA00 (text 0x34FA00-0x3515D0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's KeyFrameController.cpp as one translation unit: an unanchored TU
// in LightFlare.cpp's tu_map row, the Position/Rotation controllers (Linear,
// Bezier, ByteAxis, ShortAxis) and GenerateTangents, then the static
// initializer (__sinit_KeyFrameController.cpp, the .ctor slot after
// LightFlare.cpp's). Its strings (@1668...) follow LightFlare.cpp's one
// string. `make link` places the object's data as well as its code
// (tools/link_c.py).


#include "PositionController/PositionController.h"
#include "LinearPositionController/LinearPositionController.h"
#include "d3dtypes.h"
#include "BezierPositionController/BezierPositionController.h"
#include "RotationController/RotationController.h"
#include "ByteAxisRotationController/ByteAxisRotationController.h"
#include "Quaternion/Quaternion.h"
#include "ShortAxisRotationController/ShortAxisRotationController.h"
#include "sinit/AxisVectors.h"

// The controllers' interpolate flag, their second data word: +0x4 on the
// PS2 (data first, vptr at +0x10); after the vptr on x86-64.
#ifdef SPLASHDOWN_PC
#define INTERPOLATE_FLAG(type, field) ((type *)this)->field
#else
#define INTERPOLATE_FLAG(type, field) *(int *)((char *)this + 0x4)
#endif

extern "C" void free(void *p);
extern "C" void *malloc(unsigned int size);
extern "C" void *realloc(void *p, unsigned int size);
extern "C" char *strcpy(char *dst, const char *src);

// A packed rotation: angle in 256 steps per turn (AngleAxis.cpp), unit axis * 127.
struct ByteAngleAxis {
    unsigned char angle;
    signed char x, y, z;
};

struct ByteAxisRotationKey {
    float m_time;
    ByteAngleAxis m_rotation;
};

void SetByteAngleAxis(ByteAngleAxis *axis, Quaternion *q);
void GetQuaternion(ByteAngleAxis *axis, Quaternion *q);
void SetShortAngleAxis(ShortAngleAxis *axis, Quaternion *q);
void GetQuaternion(ShortAngleAxis *axis, Quaternion *q);

// ---- src/PositionController/ctor.cpp

PositionController::PositionController()
{
    m_bone = 0;
    m_interpolate = 1;
    m_numKeys = 0;
    m_allocatedKeys = 0;
}


// ---- src/PositionController/dtor.cpp

PositionController::~PositionController()
{
}


// ---- src/LinearPositionController/ctor.cpp

LinearPositionController::LinearPositionController()
{
    m_keys = 0;
}


// ---- src/LinearPositionController/dtor.cpp

LinearPositionController::~LinearPositionController()
{
    if (m_keys)
        free(m_keys);
}


// ---- src/LinearPositionController/SetPosition.cpp

// Index of the first key later than time. As an inline helper, its in-loop
// return gives retail's branch shape.
static inline int FindInsert(LinearPositionKey *keys, int count, float time)
{
    int i;
    for (i = 0; i < count; i++) {
        if (!(keys[i].time <= time))
            return i;
    }
    return i;
}

// Inserts a key in time order.
void LinearPositionController::SetPosition(_D3DVECTOR *position, float time)
{
    if (!(m_numKeys < m_allocatedKeys))
        AllocateKeys(m_allocatedKeys + 1);
    int count = m_numKeys;
    float x = position->x;
    float y = position->y;
    float z = position->z;
    LinearPositionKey *keys = m_keys;
    int i = FindInsert(keys, count, time);
    // Retail bug: the shift runs upward (k++), not down to i.
    for (int k = count; i < k; k++) {
        LinearPositionKey *dst = &keys[k];
        LinearPositionKey *src = &keys[k - 1];
        dst->time = src->time;
        dst->x = src->x;
        dst->y = src->y;
        dst->z = src->z;
    }
    keys[i].time = time;
    keys[i].x = x;
    keys[i].y = y;
    keys[i].z = z;
    m_numKeys++;
}


// ---- src/LinearPositionController/SetPosition_0034FC60.cpp

// The velocity/acceleration arguments are ignored: linear keys only hold positions.
void LinearPositionController::SetPosition(_D3DVECTOR *position, float time, _D3DVECTOR *, _D3DVECTOR *)
{
    SetPosition(position, time);
}


// ---- src/LinearPositionController/GetPosition.cpp

// Index of the first key later than time. As an inline helper, its in-loop
// return gives retail's branch shape.
static inline int FindKey(LinearPositionKey *keys, int count, float time)
{
    int i;
    for (i = 0; i < count; i++) {
        if (!(keys[i].time <= time))
            return i;
    }
    return i;
}

// Position at time: linear between the surrounding keys (extrapolating past
// the last one), or the previous key's position when not interpolating.
void LinearPositionController::GetPosition(float time, _D3DVECTOR *position)
{
    // +0x4 is an "interpolate" flag inside PositionController's padding.
    if (INTERPOLATE_FLAG(PositionController, m_interpolate)) {
        LinearPositionKey *keys;
        int count;
        int i;
        LinearPositionKey *a;
        LinearPositionKey *b;
        float t;
        int between;
        count = m_numKeys;
        keys = m_keys;
        i = FindKey(keys, count, time);
        if (i != count) {
            a = &keys[i - 1];
            b = &keys[i];
            time -= keys[i - 1].time;
            t = time / (keys[i].time - keys[i - 1].time);
            between = 1;
        } else {
            float span = keys[i - 1].time - keys[i - 2].time;
            a = &keys[count - 2];
            b = &keys[count - 1];
            time -= keys[count - 1].time;
            t = time / span;
            between = 0;
        }
        if (between) {
            position->x = a->x + t * (b->x - a->x);
            position->y = a->y + t * (b->y - a->y);
            position->z = a->z + t * (b->z - a->z);
        } else {
            position->x = b->x + t * (b->x - a->x);
            position->y = b->y + t * (b->y - a->y);
            position->z = b->z + t * (b->z - a->z);
        }
    } else {
        int i = FindKey(m_keys, m_numKeys, time);
        position->x = m_keys[i - 1].x;
        position->y = m_keys[i - 1].y;
        position->x = m_keys[i - 1].z;  // retail bug: z lands in x
    }
}


// ---- src/LinearPositionController/SetKey.cpp

void LinearPositionController::SetKey(int index, _D3DVECTOR *position, float *time, _D3DVECTOR *, _D3DVECTOR *)
{
    if (position) {
        m_keys[index].x = position->x;
        m_keys[index].y = position->y;
        m_keys[index].z = position->z;
    }
    if (time)
        m_keys[index].time = *time;
}


// ---- src/LinearPositionController/GetKey.cpp

void LinearPositionController::GetKey(int index, _D3DVECTOR *position, float *time, _D3DVECTOR *, _D3DVECTOR *)
{
    if (position) {
        position->x = m_keys[index].x;
        position->y = m_keys[index].y;
        position->z = m_keys[index].z;
    }
    if (time)
        *time = m_keys[index].time;
}


// ---- src/LinearPositionController/ValidTime.cpp

int LinearPositionController::ValidTime(float time)
{
    return (m_keys[m_numKeys - 1].time < time) ? 1 : 0;
}


// ---- src/LinearPositionController/AllocateKeys.cpp

void LinearPositionController::AllocateKeys(int count)
{
    m_allocatedKeys = count;
    m_keys = (LinearPositionKey *)realloc(m_keys, m_allocatedKeys * sizeof(LinearPositionKey));
}


// ---- src/BezierPositionController/ctor.cpp

BezierPositionController::BezierPositionController()
{
    m_numKeys = 0;
    m_keys = 0;
}


// ---- src/BezierPositionController/dtor.cpp



BezierPositionController::~BezierPositionController()
{
    if (m_keys)
        free(m_keys);
}


// ---- src/BezierPositionController/SetPosition.cpp




// Index of the first key later than time (inline: the in-loop return gives
// retail's branch shape).
// Index of the first key later than time. As an inline helper, its in-loop
// return gives retail's branch shape.
static inline int FindInsert(BezierKey *keys, int count, float time)
{
    int i;
    for (i = 0; i < count; i++) {
        if (!(keys[i].time <= time))
            return i;
    }
    return i;
}

// Inserts a key in time order.
void BezierPositionController::SetPosition(_D3DVECTOR *position, float time)
{
    if (!(m_numKeys < m_allocatedKeys))
        AllocateKeys(m_allocatedKeys + 1);
    int count = m_numKeys;
    float x = position->x;
    float y = position->y;
    float z = position->z;
    BezierKey *keys = m_keys;
    int i = FindInsert(keys, count, time);
    // Retail bug: the shift runs upward (k++), not down to i.
    for (int k = count; i < k; k++) {
        BezierKey *dst = &keys[k];
        BezierKey *src = &keys[k - 1];
        dst->time = src->time;
        dst->position[0] = src->position[0];
        dst->position[1] = src->position[1];
        dst->position[2] = src->position[2];
        dst->inTangent[0] = src->inTangent[0];
        dst->inTangent[1] = src->inTangent[1];
        dst->inTangent[2] = src->inTangent[2];
        dst->outTangent[0] = src->outTangent[0];
        dst->outTangent[1] = src->outTangent[1];
        dst->outTangent[2] = src->outTangent[2];
    }
    keys[i].time = time;
    keys[i].position[0] = x;
    keys[i].position[1] = y;
    keys[i].position[2] = z;
    keys[i].inTangent[0] = 0.0f;
    keys[i].inTangent[1] = 0.0f;
    keys[i].inTangent[2] = 0.0f;
    keys[i].outTangent[0] = 0.0f;
    keys[i].outTangent[1] = 0.0f;
    keys[i].outTangent[2] = 0.0f;
    m_numKeys++;
}


// ---- src/BezierPositionController/SetPosition_003501F0.cpp




// Index of the first key later than time (inline: the in-loop return gives
// retail's branch shape).
// Index of the first key later than time. As an inline helper, its in-loop
// return gives retail's branch shape.

// Inserts a key in time order. Retail stores the fourth argument as the in
// tangent, ignores the third, and writes an uninitialized float into all three
// out-tangent slots.
void BezierPositionController::SetPosition(_D3DVECTOR *position, float time, _D3DVECTOR *unused,
                                           _D3DVECTOR *inTangent)
{
    float unset;
    if (!(m_numKeys < m_allocatedKeys))
        AllocateKeys(m_allocatedKeys + 1);
    int count = m_numKeys;
    float x = position->x;
    float y = position->y;
    float z = position->z;
    float ix = inTangent->x;
    float iy = inTangent->y;
    float iz = inTangent->z;
    BezierKey *keys = m_keys;
    int i = FindInsert(keys, count, time);
    // Retail bug: the shift runs upward (k++), not down to i.
    for (int k = count; i < k; k++) {
        BezierKey *dst = &keys[k];
        BezierKey *src = &keys[k - 1];
        dst->time = src->time;
        dst->position[0] = src->position[0];
        dst->position[1] = src->position[1];
        dst->position[2] = src->position[2];
        dst->inTangent[0] = src->inTangent[0];
        dst->inTangent[1] = src->inTangent[1];
        dst->inTangent[2] = src->inTangent[2];
        dst->outTangent[0] = src->outTangent[0];
        dst->outTangent[1] = src->outTangent[1];
        dst->outTangent[2] = src->outTangent[2];
    }
    keys[i].time = time;
    keys[i].position[0] = x;
    keys[i].position[1] = y;
    keys[i].position[2] = z;
    keys[i].inTangent[0] = ix;
    keys[i].inTangent[1] = iy;
    keys[i].inTangent[2] = iz;
    keys[i].outTangent[0] = unset;
    keys[i].outTangent[1] = unset;
    keys[i].outTangent[2] = unset;
    m_numKeys++;
}


// ---- src/BezierPositionController/GetPosition.cpp




// Index of the first key later than time. As an inline helper, its in-loop
// return gives retail's branch shape.
static inline int FindKey(BezierKey *keys, int count, float time)
{
    int i;
    for (i = 0; i < count; i++) {
        if (!(keys[i].time <= time))
            return i;
    }
    return i;
}

// Position at time: a Hermite curve between the surrounding keys
// (extrapolating linearly past the last one), or the previous key's position
// when not interpolating.
void BezierPositionController::GetPosition(float time, _D3DVECTOR *position)
{
    // +0x4 is an "interpolate" flag inside PositionController's padding.
    if (INTERPOLATE_FLAG(PositionController, m_interpolate)) {
        int i;
        BezierKey *b;
        int between;
        BezierKey *keys;
        BezierKey *a;
        float t;
        int count;
        count = m_numKeys;
        keys = m_keys;
        i = FindKey(keys, count, time);
        if (i != count) {
            a = &keys[i - 1];
            b = &keys[i];
            time -= keys[i - 1].time;
            t = time / (keys[i].time - keys[i - 1].time);
            between = 1;
        } else {
            a = &keys[count - 2];
            b = &keys[count - 1];
            time -= keys[count - 1].time;
            t = time / (keys[i - 1].time - keys[i - 2].time);
            between = 0;
        }
        if (between) {
            float t2 = t * t;
            float t3 = t * t2;
            float h01 = 3.0f * t2 - 2.0f * t3;
            float h11 = t3 - t2;
            float h00 = 1.0f - h01;
            float h10 = t + (h11 - t2);
            position->x = a->position[0] * h00 + b->position[0] * h01 + a->outTangent[0] * h10 + b->inTangent[0] * h11;
            position->y = a->position[1] * h00 + b->position[1] * h01 + a->outTangent[1] * h10 + b->inTangent[1] * h11;
            position->z = a->position[2] * h00 + b->position[2] * h01 + a->outTangent[2] * h10 + b->inTangent[2] * h11;
        } else {
            position->x = b->position[0] + t * (b->position[0] - a->position[0]);
            position->y = b->position[1] + t * (b->position[1] - a->position[1]);
            position->z = b->position[2] + t * (b->position[2] - a->position[2]);
        }
    } else {
        int i = FindKey(m_keys, m_numKeys, time);
        position->x = m_keys[i - 1].position[0];
        position->y = m_keys[i - 1].position[1];
        position->z = m_keys[i - 1].position[2];
    }
}


// ---- src/BezierPositionController/SetKey.cpp

void BezierPositionController::SetKey(int i, _D3DVECTOR *position, float *time, _D3DVECTOR *inTangent, _D3DVECTOR *outTangent)
{
    if (position) {
        m_keys[i].position[0] = position->x;
        m_keys[i].position[1] = position->y;
        m_keys[i].position[2] = position->z;
    }
    if (time)
        m_keys[i].time = *time;
    if (inTangent) {
        m_keys[i].inTangent[0] = inTangent->x;
        m_keys[i].inTangent[1] = inTangent->y;
        m_keys[i].inTangent[2] = inTangent->z;
    }
    if (outTangent) {
        m_keys[i].outTangent[0] = outTangent->x;
        m_keys[i].outTangent[1] = outTangent->y;
        m_keys[i].outTangent[2] = outTangent->z;
    }
}


// ---- src/BezierPositionController/GetKey.cpp

void BezierPositionController::GetKey(int i, _D3DVECTOR *position, float *time, _D3DVECTOR *inTangent, _D3DVECTOR *outTangent)
{
    if (position) {
        position->x = m_keys[i].position[0];
        position->y = m_keys[i].position[1];
        position->z = m_keys[i].position[2];
    }
    if (time)
        *time = m_keys[i].time;
    if (inTangent) {
        inTangent->x = m_keys[i].inTangent[0];
        inTangent->y = m_keys[i].inTangent[1];
        inTangent->z = m_keys[i].inTangent[2];
    }
    if (outTangent) {
        outTangent->x = m_keys[i].outTangent[0];
        outTangent->y = m_keys[i].outTangent[1];
        outTangent->z = m_keys[i].outTangent[2];
    }
}


// ---- src/BezierPositionController/ValidTime.cpp

int BezierPositionController::ValidTime(float t)
{
    return (m_keys[m_numKeys - 1].time < t) ? 1 : 0;
}


// ---- src/BezierPositionController/AllocateKeys.cpp




void BezierPositionController::AllocateKeys(int count)
{
    m_allocatedKeys = count;
    m_keys = (BezierKey *)realloc(m_keys, m_allocatedKeys * sizeof(BezierKey));
}


// ---- src/RotationController/ctor.cpp
// (This draft also held copies of PastEndOfMotion__6MotionFf and __ct__13MotionLibraryFv; see src/Motion/, src/MotionLibrary/.)


RotationController::RotationController()
{
    m_bone = 0;
    m_4 = 1;
    m_numKeys = 0;
    m_allocatedKeys = 0;
}


// ---- src/RotationController/dtor.cpp

RotationController::~RotationController()
{
}


// ---- src/ByteAxisRotationController/ctor.cpp

ByteAxisRotationController::ByteAxisRotationController()
{
    m_keys = 0;
}


// ---- src/ByteAxisRotationController/dtor.cpp



ByteAxisRotationController::~ByteAxisRotationController()
{
    if (m_keys)
        free(m_keys);
}


// ---- src/ByteAxisRotationController/SetQuaternion.cpp
// SetQuaternion__26ByteAxisRotationControllerFP10Quaternionf  @ 0x003509F0
// Register colouring here depends on when each temporary is created: the search index
// (FindInsert's return) is a temp made at inlining, the shift loop's dst pointer is a
// later strength-reduction temp, and a named `src` local would be numbered above both.
// Retail needs dst < src < index < k in vreg order, so `src` comes from an inline
// helper too (its return temp is created at inlining, after the index, before the
// strength reduction). The Short/Long variants have other key sizes and colour differently.





void SetByteAngleAxis(ByteAngleAxis *axis, Quaternion *q);



// Index of the first key later than time. As an inline helper, its in-loop
// return gives retail's branch shape.
static inline int FindInsert(ByteAxisRotationKey *keys, int count, float time)
{
    int i;
    for (i = 0; i < count; i++) {
        if (!(keys[i].m_time <= time))
            return i;
    }
    return i;
}

// Address of key n (see the header comment for why this is a helper).
static inline ByteAxisRotationKey *KeyAt(ByteAxisRotationKey *keys, int n)
{
    return &keys[n];
}

// Inserts a key in time order.
void ByteAxisRotationController::SetQuaternion(Quaternion *q, float time)
{
    if (!(m_numKeys < m_allocatedKeys))
        AllocateKeys(m_allocatedKeys + 1);
    ByteAxisRotationKey key;
    SetByteAngleAxis(&key.m_rotation, q);
    key.m_time = time;
    int count = m_numKeys;
    ByteAxisRotationKey *keys = m_keys;
    int i = FindInsert(keys, count, key.m_time);
    // Retail bug: the shift runs upward (k++), not down to i.
    for (int k = count; i < k; k++) {
        ByteAxisRotationKey *src = KeyAt(keys, k - 1);
        keys[k] = *src;
    }
    keys[i] = key;
    m_numKeys++;
}


// ---- src/ByteAxisRotationController/GetQuaternion.cpp



void GetQuaternion(ByteAngleAxis *axis, Quaternion *q);
void FastInterpolateQuaternion(Quaternion *a, Quaternion *b, Quaternion *out, float t);
void MakeClosest(Quaternion *q, ByteAngleAxis *axis);



static inline int FindKey(ByteAxisRotationKey *keys, int count, float time)
{
    int i;
    for (i = 0; i < count; i++) {
        if (!(keys[i].m_time <= time))
            return i;
    }
    return i;
}

// Rotation at time: interpolated between the surrounding keys, or the
// previous key's rotation when not interpolating.
void ByteAxisRotationController::GetQuaternion(float time, Quaternion *q)
{
    // +0x4 is an "interpolate" flag inside RotationController's padding.
    ByteAxisRotationKey *a;
    ByteAxisRotationKey *keys;
    int count;
    int interp;
    count = m_numKeys;
    keys = m_keys;
    interp = INTERPOLATE_FLAG(RotationController, m_4);
    if (interp) {
        int i;
        ByteAxisRotationKey *b;
        float t;
        int between;
        i = FindKey(keys, count, time);
        if (i != count) {
            a = &keys[i - 1];
            b = &keys[i];
            time -= keys[i - 1].m_time;
            t = time / (keys[i].m_time - keys[i - 1].m_time);
            between = 1;
        } else {
            a = &keys[count - 2];
            b = &keys[count - 1];
            time -= keys[count - 1].m_time;
            t = time / (keys[i - 1].m_time - keys[i - 2].m_time);
            between = 0;
        }
        if (between) {
            Quaternion qb;
            Quaternion qa;
            ::GetQuaternion(&a->m_rotation, &qa);
            ::GetQuaternion(&b->m_rotation, &qb);
            FastInterpolateQuaternion(&qa, &qb, q, t);
            MakeClosest(q, &b->m_rotation);
        } else {
            ::GetQuaternion(&b->m_rotation, q);
        }
    } else {
        int i = FindKey(keys, count, time);
        ::GetQuaternion(&keys[i - 1].m_rotation, q);
    }
}


// ---- src/ByteAxisRotationController/GetKey.cpp

void GetQuaternion(ByteAngleAxis *axis, Quaternion *q);



void ByteAxisRotationController::GetKey(int index, float *time, Quaternion *q)
{
    if (time)
        *time = m_keys[index].m_time;
    if (q)
        ::GetQuaternion(&m_keys[index].m_rotation, q);
}


// ---- src/ByteAxisRotationController/SetKey.cpp

void SetByteAngleAxis(ByteAngleAxis *axis, Quaternion *q);



void ByteAxisRotationController::SetKey(int index, float *time, Quaternion *q)
{
    if (time)
        m_keys[index].m_time = *time;
    if (q)
        SetByteAngleAxis(&m_keys[index].m_rotation, q);
}


// ---- src/ByteAxisRotationController/ValidTime.cpp


int ByteAxisRotationController::ValidTime(float time)
{
    return (m_keys[m_numKeys - 1].m_time < time) ? 1 : 0;
}


// ---- src/ByteAxisRotationController/AllocateKeys.cpp



void ByteAxisRotationController::AllocateKeys(int count)
{
    m_allocatedKeys = count;
    m_keys = (ByteAxisRotationKey *)realloc(m_keys, m_allocatedKeys * sizeof(ByteAxisRotationKey));
}


// ---- src/ShortAxisRotationController/ctor.cpp

ShortAxisRotationController::ShortAxisRotationController()
{
    m_keys = 0;
}


// ---- src/ShortAxisRotationController/dtor.cpp

ShortAxisRotationController::~ShortAxisRotationController()
{
    if (m_keys)
        free(m_keys);
}


// ---- src/ShortAxisRotationController/SetQuaternion.cpp



void SetShortAngleAxis(ShortAngleAxis *axis, Quaternion *q);



// Index of the first key later than time. As an inline helper, its in-loop
// return gives retail's branch shape.
static inline int FindInsert(ShortAxisRotationKey *keys, int count, float time)
{
    int i;
    for (i = 0; i < count; i++) {
        if (!(keys[i].m_time <= time))
            return i;
    }
    return i;
}

// Inserts a key in time order.
void ShortAxisRotationController::SetQuaternion(Quaternion *q, float time)
{
    if (!(m_numKeys < m_allocatedKeys))
        AllocateKeys(m_allocatedKeys + 1);
    ShortAxisRotationKey key;
    SetShortAngleAxis(&key.m_rotation, q);
    key.m_time = time;
    int count = m_numKeys;
    ShortAxisRotationKey *keys = m_keys;
    int i = FindInsert(keys, count, key.m_time);
    // Retail bug: the shift runs upward (k++), not down to i.
    for (int k = count; i < k; k++) {
        ShortAxisRotationKey *dst = &keys[k];
        ShortAxisRotationKey *src = &keys[k - 1];
        *dst = *src;
    }
    keys[i] = key;
    m_numKeys++;
}


// ---- src/ShortAxisRotationController/GetQuaternion.cpp



void GetQuaternion(ShortAngleAxis *axis, Quaternion *q);
void FastInterpolateQuaternion(Quaternion *a, Quaternion *b, Quaternion *out, float t);
void MakeClosest(Quaternion *q, ShortAngleAxis *axis);



static inline int FindKey(ShortAxisRotationKey *keys, int count, float time)
{
    int i;
    for (i = 0; i < count; i++) {
        if (!(keys[i].m_time <= time))
            return i;
    }
    return i;
}

// Rotation at time: interpolated between the surrounding keys, or the
// previous key's rotation when not interpolating.
void ShortAxisRotationController::GetQuaternion(float time, Quaternion *q)
{
    // +0x4 is an "interpolate" flag inside RotationController's padding.
    ShortAxisRotationKey *a;
    int count = m_numKeys;
    ShortAxisRotationKey *keys = m_keys;
    if (INTERPOLATE_FLAG(RotationController, m_4)) {
        int i;
        ShortAxisRotationKey *b;
        float t;
        int between;
        i = FindKey(keys, count, time);
        if (i != count) {
            a = &keys[i - 1];
            b = &keys[i];
            time -= keys[i - 1].m_time;
            t = time / (keys[i].m_time - keys[i - 1].m_time);
            between = 1;
        } else {
            a = &keys[count - 2];
            b = &keys[count - 1];
            time -= keys[count - 1].m_time;
            t = time / (keys[i - 1].m_time - keys[i - 2].m_time);
            between = 0;
        }
        if (between) {
            Quaternion qb;
            Quaternion qa;
            ::GetQuaternion(&a->m_rotation, &qa);
            ::GetQuaternion(&b->m_rotation, &qb);
            FastInterpolateQuaternion(&qa, &qb, q, t);
            MakeClosest(q, &b->m_rotation);
        } else {
            ::GetQuaternion(&b->m_rotation, q);
        }
    } else {
        int i = FindKey(keys, count, time);
        ::GetQuaternion(&keys[i - 1].m_rotation, q);
    }
}


// ---- src/ShortAxisRotationController/GetKey.cpp

void ShortAxisRotationController::GetKey(int index, float *time, Quaternion *q)
{
    if (time)
        *time = m_keys[index].m_time;
    if (q)
        ::GetQuaternion(&m_keys[index].m_rotation, q);
}


// ---- src/ShortAxisRotationController/SetKey.cpp

void ShortAxisRotationController::SetKey(int index, float *time, Quaternion *q)
{
    if (time)
        m_keys[index].m_time = *time;
    if (q)
        SetShortAngleAxis(&m_keys[index].m_rotation, q);
}


// ---- src/ShortAxisRotationController/ValidTime.cpp

int ShortAxisRotationController::ValidTime(float time)
{
    return (m_keys[m_numKeys - 1].m_time < time) ? 1 : 0;
}


// ---- src/ShortAxisRotationController/AllocateKeys.cpp

void ShortAxisRotationController::AllocateKeys(int count)
{
    m_allocatedKeys = count;
    m_keys = (ShortAxisRotationKey *)realloc(m_keys, m_allocatedKeys * sizeof(ShortAxisRotationKey));
}


// ---- src/global/GenerateTangents.cpp




// Catmull-Rom style tangents, scaled by the ratio of neighbouring key spans.
void GenerateTangents(BezierPositionController *c)
{
    int i;
    // Declared z-first: that order gives retail's FP registers.
    float dz, dy, dx;
    float inZ, inY, inX;
    float outZ, outY, outX;
    float ratio;

    if (c->m_numKeys == 0)
        return;
    for (i = 0; i < c->m_numKeys - 1; i++) {
        dx = c->m_keys[i + 1].position[0] - c->m_keys[i].position[0];
        dy = c->m_keys[i + 1].position[1] - c->m_keys[i].position[1];
        dz = c->m_keys[i + 1].position[2] - c->m_keys[i].position[2];
        if (i == 0) {
            inX = dx;
            inY = dy;
            inZ = dz;
        } else {
            ratio = (c->m_keys[i + 1].time - c->m_keys[i].time) / (c->m_keys[i + 1].time - c->m_keys[i - 1].time);
            inX = ratio * (dx + (c->m_keys[i].position[0] - c->m_keys[i - 1].position[0]));
            inY = ratio * (dy + (c->m_keys[i].position[1] - c->m_keys[i - 1].position[1]));
            inZ = ratio * (dz + (c->m_keys[i].position[2] - c->m_keys[i - 1].position[2]));
        }
        if (i == c->m_numKeys - 2) {
            outX = dx;
            outY = dy;
            outZ = dz;
        } else {
            ratio = (c->m_keys[i + 1].time - c->m_keys[i].time) / (c->m_keys[i + 2].time - c->m_keys[i].time);
            outX = ratio * (dx + (c->m_keys[i + 2].position[0] - c->m_keys[i + 1].position[0]));
            outY = ratio * (dy + (c->m_keys[i + 2].position[1] - c->m_keys[i + 1].position[1]));
            outZ = ratio * (dz + (c->m_keys[i + 2].position[2] - c->m_keys[i + 1].position[2]));
        }
        c->m_keys[i].inTangent[0] = inX;
        c->m_keys[i].inTangent[1] = inY;
        c->m_keys[i].inTangent[2] = inZ;
        c->m_keys[i].outTangent[0] = outX;
        c->m_keys[i].outTangent[1] = outY;
        c->m_keys[i].outTangent[2] = outZ;
    }
    i = c->m_numKeys - 1;
    if (c->m_numKeys >= 2) {
        c->m_keys[i].inTangent[0] = c->m_keys[i].position[0] - c->m_keys[i - 1].position[0];
        c->m_keys[i].inTangent[1] = c->m_keys[i].position[1] - c->m_keys[i - 1].position[1];
        c->m_keys[i].inTangent[2] = c->m_keys[i].position[2] - c->m_keys[i - 1].position[2];
        c->m_keys[i].outTangent[0] = c->m_keys[i].inTangent[0];
        c->m_keys[i].outTangent[1] = c->m_keys[i].inTangent[1];
        c->m_keys[i].outTangent[2] = c->m_keys[i].inTangent[2];
    } else {
        c->m_keys[i].inTangent[0] = 0.0f;
        c->m_keys[i].inTangent[1] = 0.0f;
        c->m_keys[i].inTangent[2] = 0.0f;
        c->m_keys[i].outTangent[0] = 0.0f;
        c->m_keys[i].outTangent[1] = 0.0f;
        c->m_keys[i].outTangent[2] = 0.0f;
    }
}


// ---- src/sinit/KeyFrameController.cpp
// Static initializer of KeyFrameController.cpp: only the axis vectors from the shared header.
