#ifndef LINEARPOSITIONCONTROLLER_H
#define LINEARPOSITIONCONTROLLER_H

extern "C" void *realloc(void *p, unsigned int size);
extern "C" void free(void *p);

#include "PositionController/PositionController.h"

struct LinearPositionKey {
    float time;
    float x, y, z;
};

class LinearPositionController : public PositionController {
public:
    LinearPositionController();
    virtual ~LinearPositionController();
    virtual void SetPosition(_D3DVECTOR *, float);
    virtual void SetPosition(_D3DVECTOR *, float, _D3DVECTOR *, _D3DVECTOR *);
    virtual void GetPosition(float, _D3DVECTOR *);
    virtual void SetKey(int, _D3DVECTOR *, float *, _D3DVECTOR *, _D3DVECTOR *);
    virtual void GetKey(int, _D3DVECTOR *, float *, _D3DVECTOR *, _D3DVECTOR *);
    virtual int ValidTime(float);
    virtual void AllocateKeys(int);

    LinearPositionKey *m_keys;      // +0x14
};

#endif
