// JetskiVehicle: a racing jetski, the base of JetskiPlayerVehicle,
// JetskiAIVehicle and JetskiGhostVehicle. 0x7B0 bytes (JetskiAIVehicle's
// VehicleAIInterface starts there). Bases: GameObject at +0, then
// RaceStatusVehicleInterface at +0x30 (its vptr at +0x3C); own data from
// +0x40. The component pointers are what Create `new`s and stores; vectors
// are where the ctor builds a _D3DVECTOR (w = 1). Other names are the most
// common among the files that declared it locally (mwcc's layout).
// Copies disagree on:
//   +0x6C..+0x78: the ctor stores 1.0f; JetskiCompetition/InGamePause reads
//         +0x6C/+0x70 as ints.
//   +0x80: retail `new PhysicsRigidBody`; copies call it m_physics
//         (JetskiPhysics, now PhysicsRigidBody), m_body (VehicleBody, ...).
//   +0x338: `new PSXRenderableObject` (one copy: JetskiBody *m_body).
//   +0x388: the ctor strcpy's a name here; its length is not known (+0x40C
//         is read as an int by JetskiRadar).
#ifndef JETSKIVEHICLE_JETSKIVEHICLE_H
#define JETSKIVEHICLE_JETSKIVEHICLE_H

#include "GameObject/GameObject.h"
#include "RaceStatusVehicleInterface/RaceStatusVehicleInterface.h"
#include "d3dtypes.h"

class BowSprayParticleEmitter;
class CollisionObject;
class JetskiCollisionModel;
class JetskiEdgeMonsterSoundEmitter;
class DripParticleEmitter;
class HydrodynamicsModel;
class JetParticleEmitter;
class JetskiAirControlModel;
class JetskiCompetition;
class JetskiEngineModel;
class JetskiEngineSoundEmitter;
class JetskiGroundModel;
class JetskiRider;
class JetskiRollControlModel;
class LightManager;
class ParameterBlock;
class ParticleManager;
class PhysicsRigidBody;
class PSXRenderableObject;
class PSXWater;
class ResponseEmitter;
class Scene;
class SearchableObject;
class SearchNode;
class SelectiveGravityModel;
class SoultreeObject;
class SoundEmitter;
class SpringCollisionDetectionModel;
class Terrain;
class Water;
struct VehicleAudType;

#include "WreckType/WreckType.h"

class JetskiVehicle : public GameObject, public RaceStatusVehicleInterface {
public:
    JetskiVehicle(int enabled);
    // Returns this. Declared int: a class-pointer return type changes
    // LoadGenericSounds codegen (constant order around SoundEmitter::Create).
    int Create(JetskiCompetition *competition, char *a, char *b, char *c, char *d, char *e, _D3DVECTOR f,
               _D3DVECTOR g, _D3DVECTOR h, _D3DVECTOR i, LightManager *lights, ParticleManager *particles,
               Terrain *terrain, PSXWater *water, int player, Scene *scene);
    void AnimateJetski();
    void Cannonball();
    int CheckForEdgeOfWorld(float *);
    int InitializeCollisionData(SearchNode *node, char *name);
    void InitializeStatus();
    int LoadGenericSounds(VehicleAudType *type);
    int LoadPhysicsParameters(ParameterBlock *block);
    void MakeInvulnerable();
    void MakeVulnerable();
    void ProcessInputs(float dt);
    void RunSimulation(float dt);
    void ShakeJoystick(float, int);
    void TestForImpact(float dt);
    void TestForReset(float dt);
    void TestForWreck(float dt);
    void UpdateEdgeMonster(float dt);
    void UpdateParticles(float dt);
    void UpdateStates(float dt);

    // --- virtuals in retail slot order ---
    virtual ~JetskiVehicle();
    virtual void DisableAll();
    virtual void EnableAll();
    virtual int Tick(float dt);
    virtual int Render3D();
    virtual int KeyDownHandler(ProcessNode *node, KeyInfo *key);
    virtual void ResetPosition(int reset);
    virtual void StopWreck(int);
    virtual void TriggerWreck(WreckType type);
    virtual int RSGetVehicleStatus(_D3DVECTOR *position, _D3DVECTOR *orientation, float *speed, int *flags);
    virtual int RSSetResetParameters(_D3DVECTOR *position, _D3DVECTOR *orientation);
    virtual int RSFinishedRaceCallback(RaceStatusClass *, RaceStatusStruct_Tag *);
    virtual int RSGateMissedCallback(RaceStatusClass *, GateClass *);
    virtual int RSGatePassedCallback(RaceStatusClass *, GateClass *);
    virtual int RSRaceOrderChangeCallback(RaceStatusClass *, RaceStatusStruct_Tag *, RaceStatusStruct_Tag *, int);
    virtual int RSVehicleTakesLeadCallback(RaceStatusClass *, RaceStatusStruct_Tag *);
    virtual int RSLapUpdateCallback(RaceStatusClass *, RaceStatusStruct_Tag *);

    int m_x40;                          // +0x40
    float m_steer;                      // +0x44
    float m_throttle;                   // +0x48
    float m_stickX;                     // +0x4C
    float m_stickY;                     // +0x50
    float m_heldSteer;                  // +0x54
    float m_heldThrottle;               // +0x58
    float m_boost;                      // +0x5C
    float m_x60;                        // +0x60
    float m_x64;                        // +0x64
    float m_boostLevel;                 // +0x68
    float m_x6C;                        // +0x6C, 1.0 from the ctor
    float m_x70;                        // +0x70, 1.0
    float m_x74;                        // +0x74, 1.0
    float m_x78;                        // +0x78, 1.0
    float m_x7C;                        // +0x7C
    PhysicsRigidBody *m_body;           // +0x80
    SelectiveGravityModel *m_gravity;   // +0x84
    JetskiEngineModel *m_engine;        // +0x88
    JetskiGroundModel *m_ground;        // +0x8C
    HydrodynamicsModel *m_hydrodynamics; // +0x90
    JetskiAirControlModel *m_airControl; // +0x94
    JetskiRollControlModel *m_rollControl; // +0x98
    SpringCollisionDetectionModel *m_detection; // +0x9C
    JetskiCollisionModel *m_collisionModel; // +0xA0
    CollisionObject *m_xA4;             // +0xA4 (LoadRaceVehicles adds scene colliders to its test list)
    CollisionObject *m_collision;       // +0xA8 (JetskiRider::InitializeCollisionData)
    CollisionObject *m_trigger;         // +0xAC, trigger sphere (InitializeCollisionData)
    int m_xB0;                          // +0xB0
    int m_xB4;                          // +0xB4
    CollisionObject *m_highSpeed;       // +0xB8, HighSpeedCollisionDetectionCallback
    int m_xBC;                          // +0xBC
    CollisionObject *m_reset;           // +0xC0, ResetCollisionDetectionCallback
    int m_numObjects;                   // +0xC4, m_detection's object list
    SearchableObject *m_objects[64];    // +0xC8
    float m_x1C8;                       // +0x1C8
    int m_x1CC;                         // +0x1CC
    int m_x1D0;                         // +0x1D0
    int m_x1D4;                         // +0x1D4
    int m_x1D8;                         // +0x1D8
    int m_x1DC;                         // +0x1DC
    int m_x1E0;                         // +0x1E0, LoadRaceVehicles' join flag
    float m_x1E4;                       // +0x1E4
    float m_x1E8;                       // +0x1E8
    float m_resetTimer;                 // +0x1EC
    float m_x1F0;                       // +0x1F0
    int m_x1F4;                         // +0x1F4
    char pad1F8[0x200 - 0x1F8];
    _D3DVECTOR m_x200;                  // +0x200
    _D3DVECTOR m_x210;                  // +0x210
    _D3DVECTOR m_x220;                  // +0x220
    _D3DVECTOR m_x230;                  // +0x230
    _D3DVECTOR m_x240;                  // +0x240
    _D3DVECTOR m_x250;                  // +0x250
    _D3DVECTOR m_resetPosition;         // +0x260 (RSSetResetParameters)
    _D3DVECTOR m_resetOrientation;      // +0x270
    _D3DVECTOR m_resetUp;               // +0x280
    _D3DVECTOR m_x290;                  // +0x290
    _D3DVECTOR m_x2A0;                  // +0x2A0
    _D3DVECTOR m_x2B0;                  // +0x2B0
    _D3DVECTOR m_x2C0;                  // +0x2C0
    _D3DVECTOR m_x2D0;                  // +0x2D0
    _D3DVECTOR m_x2E0;                  // +0x2E0
    _D3DVECTOR m_x2F0;                  // +0x2F0
    float m_x300;                       // +0x300
    float m_x304;                       // +0x304
    float m_x308;                       // +0x308
    float m_x30C;                       // +0x30C
    float m_x310;                       // +0x310
    float m_x314;                       // +0x314
    char pad318[0x320 - 0x318];
    _D3DVECTOR m_x320;                  // +0x320
    Terrain *m_terrain;                 // +0x330 (Create's terrain)
    Scene *m_data;                      // +0x334, Create's scene
    PSXRenderableObject *m_renderable;  // +0x338
    SoultreeObject *m_object;           // +0x33C (GetNamedObject on m_renderable)
    _D3DMATRIX m_baseMatrix;            // +0x340 (its matrix, from GetMatrix)
    int m_x380;                         // +0x380, -1 from the ctor
    PSXWater *m_water;                  // +0x384
    char m_name[0x40C - 0x388];         // +0x388 (strcpy'd by the ctor)
    int m_x40C;                         // +0x40C
    char pad410[0x48C - 0x410];
    int m_x48C;                         // +0x48C, 1 from the ctor
    int m_airborne;                     // +0x490
    char pad494[0x4A0 - 0x494];
    _D3DVECTOR m_x4A0;                  // +0x4A0
    _D3DVECTOR m_x4B0;                  // +0x4B0
    _D3DVECTOR m_x4C0;                  // +0x4C0
    _D3DVECTOR m_x4D0;                  // +0x4D0
    _D3DVECTOR m_x4E0;                  // +0x4E0
    _D3DVECTOR m_x4F0;                  // +0x4F0
    _D3DVECTOR m_x500;                  // +0x500
    _D3DVECTOR m_x510;                  // +0x510
    _D3DVECTOR m_x520;                  // +0x520
    int m_x530;                         // +0x530
    float m_x534;                       // +0x534
    float m_x538;                       // +0x538
    float m_invulnerable;               // +0x53C
    int m_x540;                         // +0x540
    int m_x544;                         // +0x544
    float m_x548;                       // +0x548
    float m_edgeAngle;                  // +0x54C, the edge monster's random heading offset (UpdateEdgeMonster)
    int m_x550;                         // +0x550
    float m_x554;                       // +0x554
    float m_x558;                       // +0x558
    float m_x55C;                       // +0x55C
    int m_cameraPreset;                 // +0x560, follow-camera preset index
    float m_x564;                       // +0x564
    float m_riderLateralRange;          // +0x568 "RiderLateralRange"
    float m_riderLongitudinalRange;     // +0x56C "RiderLongitudinalRange"
    _D3DVECTOR m_x570;                  // +0x570
    int m_x580;                         // +0x580, 1 from the ctor
    int m_x584;                         // +0x584, -1 from the ctor
    char pad588[0x590 - 0x588];
    _D3DVECTOR m_x590;                  // +0x590
    _D3DVECTOR m_x5A0;                  // +0x5A0
    int m_x5B0;                         // +0x5B0
    int m_x5B4;                         // +0x5B4
    int m_x5B8;                         // +0x5B8
    int m_x5BC;                         // +0x5BC
    float m_x5C0;                       // +0x5C0
    int m_wrongWay;                     // +0x5C4
    int m_x5C8;                         // +0x5C8
    float m_x5CC;                       // +0x5CC
    int m_x5D0;                         // +0x5D0
    float m_x5D4;                       // +0x5D4
    float m_x5D8;                       // +0x5D8
    int m_x5DC;                         // +0x5DC
    int m_joystick;                     // +0x5E0, controller index
    int m_player;                       // +0x5E4
    int m_x5E8[10];                     // +0x5E8
    float m_x610;                       // +0x610
    float m_x614;                       // +0x614
    int m_timeElimBuoy;                 // +0x618
    int m_newLap;                       // +0x61C
    float m_x620;                       // +0x620
    float m_performance;                // +0x624
    BowSprayParticleEmitter *m_bowSpray[5]; // +0x628
    DripParticleEmitter *m_drip;        // +0x63C
    JetParticleEmitter *m_jet;          // +0x640
    char pad644[0x650 - 0x644];
    // +0x650..+0x71C: hull points, each its own member (the ctor builds
    // the 13 vectors one by one, not as an array). Names are
    // LoadPhysicsParameters' parameter names.
    _D3DVECTOR m_nose;                  // +0x650 "Nose"
    _D3DVECTOR m_tail;                  // +0x660 "Tail"
    _D3DVECTOR m_leftSide;              // +0x670 "LeftSide"
    _D3DVECTOR m_rightSide;             // +0x680 LeftSide mirrored
    _D3DVECTOR m_head;                  // +0x690 "Head"
    _D3DVECTOR m_leftTail;              // +0x6A0 Tail at LeftSide's x
    _D3DVECTOR m_rightTail;             // +0x6B0 Tail at RightSide's x
    _D3DVECTOR m_x6C0;                  // +0x6C0 m_leftTail, x + 0.5
    _D3DVECTOR m_x6D0;                  // +0x6D0 m_rightTail, x - 0.5
    _D3DVECTOR m_jetPosition;           // +0x6E0 "JetPosition"
    _D3DVECTOR m_jetUp;                 // +0x6F0 "JetUp"
    _D3DVECTOR m_jetSide;               // +0x700 "JetSide"
    _D3DVECTOR m_jetDirection;          // +0x710 "JetDirection"
    float m_hullLength;                 // +0x720, m_nose.z - m_tail.z (LoadPhysicsParameters)
    float m_tailToSide;                 // +0x724, m_leftSide.z - m_tail.z
    float m_sideToNose;                 // +0x728, m_hullLength - m_tailToSide
    float m_invTailToSide;              // +0x72C, 1 / m_tailToSide
    float m_invSideToNose;              // +0x730, 1 / m_sideToNose
    float m_sideRatio;                  // +0x734, m_sideToNose / m_tailToSide
    JetskiEngineSoundEmitter *m_engineSound; // +0x738
    SoundEmitter *m_splashSound;        // +0x73C
    SoundEmitter *m_spraySound;         // +0x740
    SoundEmitter *m_terrainSound;       // +0x744
    SoundEmitter *m_x748;               // +0x748
    ResponseEmitter *m_responses;       // +0x74C
    SoundEmitter *m_x750;               // +0x750
    SoundEmitter *m_x754;               // +0x754
    int m_x758;                         // +0x758
    int m_x75C;                         // +0x75C
    float m_x760;                       // +0x760
    float m_x764;                       // +0x764
    JetskiEdgeMonsterSoundEmitter *m_edgeMonsterSound; // +0x768
    int m_x76C;                         // +0x76C
    int m_x770;                         // +0x770
    int m_x774;                         // +0x774
    int m_x778;                         // +0x778
    int m_x77C;                         // +0x77C
    float m_x780;                       // +0x780
    float m_x784;                       // +0x784
    int m_x788;                         // +0x788
    int m_x78C;                         // +0x78C
    int m_x790;                         // +0x790
    JetskiRider *m_rider;               // +0x794
    float m_gateBonus;                  // +0x798
    float m_gatePenalty;                // +0x79C
    float m_roll;                       // +0x7A0, barrel-roll angle
    int m_x7A4;                         // +0x7A4
    char pad7A8[0x7B0 - 0x7A8];
};

#endif
