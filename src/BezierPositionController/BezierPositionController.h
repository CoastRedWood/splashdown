// BezierPositionController: a PositionController whose keys are Bezier
// segments (BezierKey). 0x18 bytes (`new` in Motion::AllocatePositions); the
// key array is at +0x14.
#ifndef BEZIERPOSITIONCONTROLLER_H
#define BEZIERPOSITIONCONTROLLER_H

#include "d3dtypes.h"
#include "PositionController/PositionController.h"
#include "BezierKey/BezierKey.h"

class BezierPositionController : public PositionController {
public:
    BezierPositionController();

    // --- virtuals in retail slot order ---
    virtual ~BezierPositionController();
    virtual void SetPosition(_D3DVECTOR *position, float time);
    virtual void SetPosition(_D3DVECTOR *position, float time, _D3DVECTOR *inTangent, _D3DVECTOR *outTangent);
    virtual void GetPosition(float time, _D3DVECTOR *position);
    virtual void SetKey(int i, _D3DVECTOR *position, float *time, _D3DVECTOR *inTangent, _D3DVECTOR *outTangent);
    virtual void GetKey(int i, _D3DVECTOR *position, float *time, _D3DVECTOR *inTangent, _D3DVECTOR *outTangent);
    virtual int ValidTime(float t);
    virtual void AllocateKeys(int count);

    BezierKey *m_keys;                  // +0x14
};

void GenerateTangents(BezierPositionController *controller);

#endif
