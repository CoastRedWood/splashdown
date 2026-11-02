// CollisionCharacter: a D3DIMSoultreeCharacter with a collision object (the
// wrecked rider). Its data runs +0x210..+0x277, padded to +0x280; the
// virtual GameObject base follows after one more word, at +0x288 (retail's
// thunks are @640@48@). The class is 16-byte aligned: `new
// CollisionCharacter` is 0x2C0, 0x2B8 rounded up (its _D3DVECTORs were
// probably the aligned(16) kind in this TU). Tick/Render3D override
// GameObject's (the virtual base), so they get primary slots 8 and 9.
#ifndef COLLISIONCHARACTER_COLLISIONCHARACTER_H
#define COLLISIONCHARACTER_COLLISIONCHARACTER_H

#include "D3DIMSoultreeCharacter/D3DIMSoultreeCharacter.h"
#include "d3dtypes.h"

class CollisionObject;
class SoultreeObject;

class CollisionCharacter : public D3DIMSoultreeCharacter {
public:
    CollisionCharacter(int construct);
    GameObject *Create(char *name, char *collisionName, int a, int b, int hasCollision);

    // --- virtuals in retail slot order ---
    virtual ~CollisionCharacter();
    virtual int Tick(float dt);
    virtual int Render3D();

    _D3DVECTOR m_lastPosition;          // +0x210
    _D3DVECTOR m_velocity;              // +0x220
    _D3DVECTOR m_x230;                  // +0x230
    _D3DVECTOR m_x240;                  // +0x240
    _D3DVECTOR m_x250;                  // +0x250
    _D3DVECTOR m_x260;                  // +0x260
    CollisionObject *m_collision;       // +0x270
    SoultreeObject *m_x274;             // +0x274
    char pad278[0x280 - 0x278];
} __attribute__((aligned(16)));

#endif
