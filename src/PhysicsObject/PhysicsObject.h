// PhysicsObject: a free-floating physics prop (a GameObject owning a
// PhysicsRigidBody, gravity and spring collision models). 0xC0 bytes
// (`new` in Scene loading); data +0x30..+0xB7. ResetPositionAndOrientation
// is virtual slot 27.
#ifndef PHYSICSOBJECT_PHYSICSOBJECT_H
#define PHYSICSOBJECT_PHYSICSOBJECT_H

#include "GameObject/GameObject.h"
#include "d3dtypes.h"

class CollisionObject;
class PhysicsRigidBody;
class SearchNode;
class SelectiveGravityModel;
class SoultreeObject;
class SpringCollisionDetectionModel;
class SpringCollisionModel;
class Terrain;

class PhysicsObject : public GameObject {
public:
    PhysicsObject(int enabled);
    PhysicsObject *Create(_D3DVECTOR position, _D3DVECTOR forward, _D3DVECTOR up, Terrain *terrain, char *name,
                          SearchNode *objectNode, SearchNode *collisionNode);
    void SetPhysicalProperties(float mass, float inertiaScale, float a, float b, float c, float d, float e,
                               float f, float g, float h, float i);

    // --- virtuals in retail slot order ---
    virtual ~PhysicsObject();
    virtual int Tick(float dt);
    virtual void ResetPositionAndOrientation();

    int m_unk30;                        // +0x30
    char pad34[0x40 - 0x34];
    _D3DVECTOR m_initialPosition;       // +0x40
    _D3DVECTOR m_initialForward;        // +0x50
    _D3DVECTOR m_initialUp;             // +0x60
    SearchNode *m_objectSearchNode;     // +0x70
    SearchNode *m_collisionSearchNode;  // +0x74
    char pad78[0x80 - 0x78];
    _D3DVECTOR m_unk80;                 // +0x80
    PhysicsRigidBody *m_body;           // +0x90
    SelectiveGravityModel *m_gravity;   // +0x94
    SpringCollisionDetectionModel *m_springDetection; // +0x98
    SpringCollisionModel *m_springs;    // +0x9C
    CollisionObject *m_rayCollision;    // +0xA0
    CollisionObject *m_faceCollision;   // +0xA4
    float m_xA8;                        // +0xA8
    float m_xAC;                        // +0xAC
    SoultreeObject *m_object;           // +0xB0
    int m_unkB4;                        // +0xB4
    char padB8[0xC0 - 0xB8];            // (`new PhysicsObject` is 0xC0)
};

#endif
