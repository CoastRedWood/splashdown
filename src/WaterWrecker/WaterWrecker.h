// WaterWrecker: the GameObject that runs a rider's wreck off the jetski (a
// physics simulation of the rider, then motion playback). 0x170 bytes
// (`new` in JetskiRider::CreateWrecker); the data seen so far ends at +0x164.
// Pointer types follow Create's signature (Create__12WaterWreckerFP22
// D3DIMSoultreeCharacterP7TerrainP8PSXWaterP16PhysicsRigidBodyPcP10SearchNode);
// names follow WaterWrecker's own methods (src/WaterWrecker/), otherwise the
// most common name among the files that declared it locally.
#ifndef WATERWRECKER_WATERWRECKER_H
#define WATERWRECKER_WATERWRECKER_H

#include "GameObject/GameObject.h"
#include "d3dtypes.h"

class BuoyancyModel;
class CollisionObject;
class D3DIMSoultreeCharacter;
class JetskiRider;
class Motion;
class ParticleManager;
class PhysicsRigidBody;
class PSXWater;
class SearchNode;
class SelectiveGravityModel;
class SoultreeObject;
class SpringCollisionDetectionModel;
class SpringCollisionModel;
class Terrain;

#include "WreckType/WreckType.h"

struct WreckSeparation {
    Motion *m_motion;                   // +0x0
    WreckType m_type;                   // +0x4
    int m_x8;                           // +0x8
};

struct WreckMotion {
    Motion *m_motion;                   // +0x0
    int m_x4;                           // +0x4
};

struct ParticleCollisionPoint {
    _D3DVECTOR m_point1;                // +0x00
    _D3DVECTOR m_point2;                // +0x10
    _D3DVECTOR m_velocity;              // +0x20
    int m_x30;                          // +0x30
    int m_flags;                        // +0x34
    SoultreeObject *m_object1;          // +0x38
    SoultreeObject *m_object2;          // +0x3C
};

class WaterWrecker : public GameObject {
public:
    WaterWrecker(int id);
    WaterWrecker *Create(D3DIMSoultreeCharacter *rider, Terrain *terrain, PSXWater *water, PhysicsRigidBody *vehicle,
                         char *name, SearchNode *node);
    void CreateCollisionObject(SearchNode *node);
    void CalculateInertia();
    int AddSeparationMotion(WreckType type, Motion *motion, int x);
    int AddInAirMotion(Motion *motion, int x);
    int AddBounceMotion(Motion *motion, int x);
    int AddTreadMotion(Motion *motion, int x);
    void AddBuoyancyPoint(_D3DVECTOR point, SoultreeObject *object);
    void AddCollisionPoint(_D3DVECTOR point, SoultreeObject *object);
    void AddVehicleCollisionObject(CollisionObject *object);
    void AddParticleCollisionPoint(_D3DVECTOR point1, SoultreeObject *object1, _D3DVECTOR point2,
                                   SoultreeObject *object2, int flags);
    void TriggerWreck();
    void TriggerWreck(WreckType type, int x);
    void StopWreck();
    void RunSimulation(float dt);
    void PlaybackRider(float dt);
    void GenerateParticles(float dt);
    void FixBaseObjectTransform();
    void CalculateVelocityAndAngularRate(float dt, int separated);
    void AnimateRider(float dt);

    // --- virtuals in retail slot order ---
    virtual ~WaterWrecker();
    virtual int KeyDownHandler(ProcessNode *node, KeyInfo *key);

    Terrain *m_terrain;                 // +0x30
    PSXWater *m_water;                  // +0x34
    JetskiRider *m_rider;               // +0x38 (Create's D3DIMSoultreeCharacter is the JetskiRider)
    SpringCollisionModel *m_springs;    // +0x3C
    SpringCollisionDetectionModel *m_collisionModel; // +0x40
    BuoyancyModel *m_buoyancy;          // +0x44
    char pad48[0x4C - 0x48];
    SelectiveGravityModel *m_gravity;   // +0x4C
    PhysicsRigidBody *m_body;           // +0x50, the rider's body
    PhysicsRigidBody *m_vehicle;        // +0x54
    int m_numParticlePoints;            // +0x58
    ParticleCollisionPoint *m_particlePoints; // +0x5C
    int m_x60;                          // +0x60
    int m_x64;                          // +0x64
    int m_x68;                          // +0x68
    char pad6C[0x70 - 0x6C];
    _D3DMATRIX m_baseTransform;         // +0x70, rider root relative to the vehicle
    int m_xB0;                          // +0xB0
    float m_wreckType;                  // +0xB4
    float m_lastWreckType;              // +0xB8
    float m_wreckIndex;                 // +0xBC
    WreckSeparation *m_separations;     // +0xC0
    int m_numSeparations;               // +0xC4
    int m_numByType[6];                 // +0xC8
    WreckMotion *m_inAirMotions;        // +0xE0
    int m_numInAirMotions;              // +0xE4
    WreckMotion *m_bounceMotions;       // +0xE8
    int m_numBounceMotions;             // +0xEC
    WreckMotion *m_treadMotions;        // +0xF0
    int m_numTreadMotions;              // +0xF4
    void *m_xF8;                        // +0xF8
    int m_xFC;                          // +0xFC
    char pad100[0x104 - 0x100];
    float m_x104;                       // +0x104
    float m_x108;                       // +0x108
    float m_x10C;                       // +0x10C
    _D3DVECTOR m_x110;                  // +0x110 (.y is the splash strength)
    int m_x120;                         // +0x120
    int m_x124;                         // +0x124
    SoultreeObject *m_vehicleObject;    // +0x128
    int m_x12C;                         // +0x12C
    CollisionObject *m_x130;            // +0x130
    int m_x134;                         // +0x134
    ParticleManager *m_particles;       // +0x138
    int m_x13C;                         // +0x13C
    int m_x140;                         // +0x140
    float m_transitionTime;             // +0x144
    float m_x148;                       // +0x148, the rider's last blend start
    float m_x14C;                       // +0x14C, time since the last motion change
    int m_x150;                         // +0x150
    int m_x154;                         // +0x154
    char pad158[0x15C - 0x158];
    int m_x15C;                         // +0x15C
    int m_x160;                         // +0x160
    char pad164[0x170 - 0x164];         // `new WaterWrecker` is 0x170 (JetskiRider::CreateWrecker)
};

#endif
