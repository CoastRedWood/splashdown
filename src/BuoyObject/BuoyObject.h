// BuoyObject: a floating course buoy (a SearchableGameObject: GameObject +
// SearchableObject at +0x30, data from +0x38). 0xF0 bytes (`new` in
// JetskiCompetition::CreateBuoys). SetPhysicalProperties sets +0x38..+0x50
// and the three vectors at +0xA0..+0xC0; Create news the PhysicsRigidBody at
// +0xD4 (SimulationTick's copy calls it a Scene).
#ifndef BUOYOBJECT_BUOYOBJECT_H
#define BUOYOBJECT_BUOYOBJECT_H

#include "SearchableGameObject/SearchableGameObject.h"
#include "d3dtypes.h"

class CollisionObject;
class LightManager;
class PhysicsRigidBody;
class PSXRenderableObject;
class PSXWater;
class SearchNode;
class SearchPlacement;
class SoultreeObject;
class Water;
struct BuoyGroup;
struct BuoyMarker;

class BuoyObject : public SearchableGameObject {
public:
    BuoyObject(int enabled);
    BuoyObject *Create(SearchNode *a, SearchNode *b, LightManager *lights, PSXWater *water, char *name);
    void AddSearchPlacement(SearchNode *node, unsigned int flags);
    void ResetPositionAndOrientation();
    void SetPhysicalProperties(int type, float a, float b, float c, float d, _D3DVECTOR position,
                               _D3DVECTOR forward, _D3DVECTOR up);
    void UpdateSearchPlacement();

    // --- virtuals in retail slot order ---
    virtual ~BuoyObject();
    virtual int Tick(float dt);
    virtual int SimulationTick(float dt);
    virtual int PrepareGeometry();

    int m_type;                         // +0x38 (SetPhysicalProperties)
    unsigned int m_collisionType;       // +0x3C
    BuoyGroup *m_group;                 // +0x40
    float m_x44;                        // +0x44
    float m_x48;                        // +0x48
    float m_x4C;                        // +0x4C
    float m_x50;                        // +0x50
    float m_spring;                     // +0x54
    float m_damping;                    // +0x58
    float m_height;                     // +0x5C
    float m_x60;                        // +0x60
    float m_time;                       // +0x64
    float m_x68;                        // +0x68
    float m_noiseOffsetX;               // +0x6C
    float m_noiseOffsetZ;               // +0x70
    float m_tiltScale;                  // +0x74
    float m_maxDisplacement;            // +0x78
    float m_restHeight;                 // +0x7C
    float m_activeDistance;             // +0x80
    char pad84[0x90 - 0x84];
    _D3DVECTOR m_up;                    // +0x90
    _D3DVECTOR m_position;              // +0xA0
    _D3DVECTOR m_forward;               // +0xB0
    _D3DVECTOR m_xC0;                   // +0xC0
    SearchPlacement *m_placement;       // +0xD0
    PhysicsRigidBody *m_body;           // +0xD4
    CollisionObject *m_collision;       // +0xD8
    Water *m_water;                     // +0xDC
    PSXRenderableObject *m_object;      // +0xE0 (Create news it)
    BuoyMarker *m_marker;               // +0xE4
    char padE8[0xF0 - 0xE8];            // (`new BuoyObject` is 0xF0)
};

#endif
