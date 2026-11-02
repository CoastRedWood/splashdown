#ifndef POSITIONCONTROLLER_POSITIONCONTROLLER_H
#define POSITIONCONTROLLER_POSITIONCONTROLLER_H

#include "d3dtypes.h"

class PositionController {
public:
    PositionController();
    int m_bone;                     // +0x00 (MotionLibrary::LoadMotionFromVUE)
    int m_interpolate;              // +0x04, Motion::m_interpolatePosition
    int m_numKeys;                  // +0x08
    int m_allocatedKeys;            // +0x0C
    virtual ~PositionController();  // vptr +0x10
    // Slots 1-7 are pure (0 in retail's vtable, 0x44BB70).
    virtual void SetPosition(_D3DVECTOR *, float) = 0;
    virtual void SetPosition(_D3DVECTOR *, float, _D3DVECTOR *, _D3DVECTOR *) = 0;
    virtual void GetPosition(float, _D3DVECTOR *) = 0;
    virtual void SetKey(int, _D3DVECTOR *, float *, _D3DVECTOR *, _D3DVECTOR *) = 0;
    virtual void GetKey(int, _D3DVECTOR *, float *, _D3DVECTOR *, _D3DVECTOR *) = 0;
    virtual int ValidTime(float) = 0;
    virtual void AllocateKeys(int) = 0;
};

#endif
