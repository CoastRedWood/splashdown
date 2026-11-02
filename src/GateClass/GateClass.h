// GateClass: a race gate (RaceStatusClass::AddGate news it, 0x90 bytes,
// and chains it on RaceStatusClass::m_objects through m_next). A
// GameObject. Names come from the class's own methods (Crossed).
#ifndef GATECLASS_GATECLASS_H
#define GATECLASS_GATECLASS_H

#include "d3dtypes.h"
#include "GameObject/GameObject.h"

class GateClass : public GameObject {
public:
    GateClass(int id);
    GateClass *Create(int type, _D3DVECTOR *center, _D3DVECTOR *normal, _D3DVECTOR *halfSize, float *width,
                      float *height, float *depth);
    int Crossed(_D3DVECTOR *from, _D3DVECTOR *to);

    // --- virtuals in retail slot order ---
    virtual ~GateClass();

    unsigned int m_flags;               // +0x30 (1 active, 4 either direction, 8 corner + edge form)
    char pad34[0x40 - 0x34];
    _D3DVECTOR m_center;                // +0x40
    _D3DVECTOR m_normal;                // +0x50
    _D3DVECTOR m_halfSize;              // +0x60
    float m_x70;                        // +0x70
    float m_height;                     // +0x74
    float m_depth;                      // +0x78 (Create)
    int m_x7C;                          // +0x7C
    GateClass *m_next;                  // +0x80, RaceStatusClass's gate list
    char pad84[0x90 - 0x84];
};

#endif
