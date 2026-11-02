// Vegetation: one placed plant of an EcoSystem. A SearchableObject (no
// vtable): the ctor is SearchableObject's inline one (+0x00 = 0, +0x02 = 0xFF,
// the three bits cleared) before it sets +0x02 to the EcoSystem's type ID.
// 0x1C bytes. The ctor stores +0x14 as one word; Render and PrepareGeometry
// use it as two shorts (cos, sin). Names from EcoSystem.cpp.
#ifndef VEGETATION_VEGETATION_H
#define VEGETATION_VEGETATION_H

#include "SearchableObject/SearchableObject.h"

class CollisionObject;
struct VifDataItem;
struct WordVector;
struct _D3DVECTOR;

class Vegetation : public SearchableObject {
public:
    Vegetation();
    void Create(int type, _D3DVECTOR *position, char height, char radius);
    void Create(int type, WordVector *position, char height, char radius);
    CollisionObject *GetCollisionObject(int index);
    float GetRadius();
    int NumCollisionObjects();
    int Render(VifDataItem *item);

    unsigned short m_depth;             // +0x04, sort key (0..0xFFFF)
    unsigned short m_next;              // +0x06, next in its depth bucket, then the frame stamp
    unsigned short m_x;                 // +0x08, grid position
    unsigned short m_y;                 // +0x0A
    unsigned short m_z;                 // +0x0C
    unsigned char m_type;               // +0x0E
    unsigned char m_alpha;              // +0x0F, 64 = 2D, 0 = 3D
    char m_height;                      // +0x10
    char m_radius;                      // +0x11 (GetRadius: the LOD)
    unsigned char m_is2D : 1;           // +0x12 bit 0 (Create sets it)
    union {
        int m_x14;                      // +0x14 (the ctor's one word)
        struct {
            short m_cos;                // +0x14
            short m_sin;                // +0x16
        } m_rot;
    };
    unsigned int m_clipCodes;           // +0x18
};

#endif
