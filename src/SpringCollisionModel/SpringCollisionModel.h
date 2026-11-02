// SpringCollisionModel: a GameObject that models a PhysicsBody's contacts as
// springs (SpringCollisionDetectionModel fills it). 0x130 bytes (`new
// SpringCollisionModel`); the known data ends at +0x124.
// Names follow SpringCollisionModel's own methods (src/SpringCollisionModel/),
// otherwise the most common name among the files that declared it locally.
#ifndef SPRINGCOLLISIONMODEL_SPRINGCOLLISIONMODEL_H
#define SPRINGCOLLISIONMODEL_SPRINGCOLLISIONMODEL_H

#include "GameObject/GameObject.h"
#include "d3dtypes.h"

struct BoundingBoxTree;
class CollisionObject;
class PhysicsBody;
class SoultreeObject;

// One spring (the +0x78 array, m_numSprings of m_maxContacts used). 0x70 bytes.
struct SpringCollisionPoint {
    float m_stiffness;                  // +0x00
    char pad04[0x20 - 0x04];
    _D3DVECTOR m_point;                 // +0x20, local position
    _D3DVECTOR m_groundNormal;          // +0x30
    _D3DVECTOR m_worldPosition;         // +0x40
    SoultreeObject *m_object;           // +0x50
    char pad54[0x5C - 0x54];
    int m_id;                           // +0x5C
    int m_colliding;                    // +0x60 (SimulationTick)
    int m_surfaceType;                  // +0x64
    char pad68[0x70 - 0x68];
};

// One contact with another object (the +0x84 array). 0x70 bytes.
struct ObjectCollisionPoint {
    char pad00[0x10];
    _D3DVECTOR m_point;                 // +0x10
    _D3DVECTOR m_normal;                // +0x20
    _D3DVECTOR m_x30;                   // +0x30
    _D3DVECTOR m_x40;                   // +0x40
    SoultreeObject *m_object;           // +0x50
    PhysicsBody *m_body;                // +0x54
    CollisionObject *m_collision;       // +0x58
    char pad5C[0x60 - 0x5C];
    int m_colliding;                    // +0x60 (SimulationTick)
    int m_x64;                          // +0x64
    char pad68[0x70 - 0x68];
};

class SpringCollisionModel : public GameObject {
public:
    SpringCollisionModel(int enabled);
    void Create(int maxContacts);
    void SetBody(PhysicsBody *body);
    void SetCollisionParameters(float friction, float stiffness, float damping, float stiffness2,
                                float damping2, float b0, float b4, float b8);
    void SetFriction(int i, float friction);
    float ComputeStiffness(float a, float b);
    float ComputeDamping(float ratio, float stiffness, float mass);
    void ResetObjectCollisions();
    int AddSpringCollisionPoint(BoundingBoxTree *tree, int flags, SoultreeObject *obj);
    void AddSpringCollisionPoint(CollisionObject *object, int flags, SoultreeObject *obj);
    int AddSpringCollisionPoint(_D3DVECTOR point, int id, SoultreeObject *object, float stiffness); // the tree overload returns it
    void AddObjectCollisionPoint(_D3DVECTOR point, _D3DVECTOR normal, _D3DVECTOR a, _D3DVECTOR b, PhysicsBody *body,
                                 SoultreeObject *object, CollisionObject *collision, int id);
    void ApplyForceToBody(PhysicsBody *body, _D3DVECTOR force, _D3DVECTOR point, float scale);
    void RemoveNormalVelocity(_D3DVECTOR &normal, _D3DVECTOR &velocity);
    int CalculateImpulseResponse(float depth, float dt, _D3DVECTOR normal, _D3DVECTOR point, PhysicsBody *other);
    void CalculateCollisionResponse(int type, float dt, float friction, float depth, _D3DVECTOR normal,
                                    _D3DVECTOR point, PhysicsBody *other);

    // --- virtuals in retail slot order ---
    virtual ~SpringCollisionModel();
    virtual int SimulationTick(float dt);
    virtual int Render3D();

    PhysicsBody *m_body;                // +0x30 (SetBody)
    int m_x34;                          // +0x34
    int m_x38;                          // +0x38
    int m_x3C;                          // +0x3C
    int m_x40;                          // +0x40
    int m_x44;                          // +0x44
    int m_groundCollision;              // +0x48 (1 from the ctor)
    int m_x4C;                          // +0x4C
    int m_x50;                          // +0x50
    int m_x54;                          // +0x54
    int m_x58;                          // +0x58
    int m_x5C;                          // +0x5C
    int m_x60;                          // +0x60
    int m_yawOnly;                      // +0x64
    int m_useCenter;                    // +0x68
    int m_x6C;                          // +0x6C
    int m_x70;                          // +0x70
    int m_numSprings;                   // +0x74
    SpringCollisionPoint *m_springs;    // +0x78, m_maxContacts entries
    int m_numContacts;                  // +0x7C
    int m_maxContacts;                  // +0x80 (Create)
    ObjectCollisionPoint *m_contacts;   // +0x84, m_maxContacts entries
    float m_friction;                   // +0x88 (SetCollisionParameters)
    float m_x8C;                        // +0x8C, 10000 from the ctor
    float m_x90;                        // +0x90, 0.15 from the ctor
    float m_stiffness;                  // +0x94
    float m_damping;                    // +0x98
    float m_stiffness2;                 // +0x9C
    float m_damping2;                   // +0xA0
    float m_xA4;                        // +0xA4
    float m_xA8;                        // +0xA8
    float m_xAC;                        // +0xAC
    float m_xB0;                        // +0xB0
    float m_xB4;                        // +0xB4
    float m_xB8;                        // +0xB8
    float m_xBC;                        // +0xBC
    char padC0[0xC4 - 0xC0];
    float m_xC4;                        // +0xC4, 0.07 from the ctor
    float m_xC8;                        // +0xC8, 0.07 from the ctor
    int m_surfaceType;                  // +0xCC, last spring's
    float m_frictions[10];              // +0xD0, per surface type (SetFriction)
    char padF8[0x100 - 0xF8];
    _D3DVECTOR m_center;                // +0x100
    _D3DVECTOR m_x110;                  // +0x110
    CollisionObject *m_hitObject;       // +0x120, what SpringCollisionDetectionIHitSomethingCallback last hit
    char pad124[0x130 - 0x124];
};

#endif
