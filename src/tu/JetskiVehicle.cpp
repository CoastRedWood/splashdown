// JetskiVehicle.cpp  @ 0x003CC110 (text 0x3CC110-0x3D82A0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's JetskiVehicle.cpp as one translation unit: the two collision
// callbacks InitializeCollisionData installs (their owner is the
// JetskiVehicle: +0x80 body, +0xB0/+0xBC flags), then JetskiVehicle's 35
// functions; the static initializer builds only the axis vectors. `make link`
// places the object's data as well as its code (tools/link_c.py).



// UpdateParticles', UpdateEdgeMonster's and Cannonball's per-function files set
// it (UpdateStates' 5); TU-wide (read at the end of the TU).
#pragma inline_depth(6)

#include "d3dtypes.h"
#include "PhysicsBody/PhysicsBody.h"
#include "JetskiVehicle/JetskiVehicle.h"
#include "JetskiGame/JetskiGame.h"
#include "JetskiGameOptions/JetskiGameOptions.h"
#include "JetskiEngineSoundEmitter/JetskiEngineSoundEmitter.h"
#include "AuralScape/AuralScape.h"
#include "SoundEmitter/SoundEmitter.h"
#include "ResponseEmitter/ResponseEmitter.h"
#include "JetskiRider/JetskiRider.h"
#include "ParameterBlock/ParameterBlock.h"
#include "PSXRenderableObject/PSXRenderableObject.h"
#include "PhysicsRigidBody/PhysicsRigidBody.h"
#include "SelectiveGravityModel/SelectiveGravityModel.h"
#include "JetskiEngineModel/JetskiEngineModel.h"
#include "HydrodynamicsModel/HydrodynamicsModel.h"
#include "JetskiRollControlModel/JetskiRollControlModel.h"
#include "ParticleEmitter/ParticleEmitter.h"
#include "DripParticleEmitter/DripParticleEmitter.h"
#include "JetParticleEmitter/JetParticleEmitter.h"
#include "Scene/Scene.h"
#include "JetskiGroundModel/JetskiGroundModel.h"
#include "JetskiAirControlModel/JetskiAirControlModel.h"
#include "BowSprayParticleEmitter/BowSprayParticleEmitter.h"
#include "CollisionObject/CollisionObject.h"
#include "SpringCollisionDetectionModel/SpringCollisionDetectionModel.h"
#include "JetskiCollisionModel/JetskiCollisionModel.h"
#include "WaterWrecker/WaterWrecker.h"
#include "JetskiCompetition/JetskiCompetition.h"
#include "TrackType/TrackType.h"
#include "DebugOverlay/DebugOverlay.h"
#include "JetskiCameraController/JetskiCameraController.h"
#include "JetskiFollowCamera/JetskiFollowCamera.h"
#include "SoultreeObject/SoultreeObject.h"
#include "JetskiEdgeMonsterSoundEmitter/JetskiEdgeMonsterSoundEmitter.h"
#include "RaceStatusStruct_Tag/RaceStatusStruct_Tag.h"
#include "Terrain/Terrain.h"
#include "PSXWater/PSXWater.h"
#include "RaceStatusClass/RaceStatusClass.h"
#include "JetskiStuntsKey/JetskiStuntsKey.h"
#include "JetskiRiderSoundEmitter/JetskiRiderSoundEmitter.h"
#include "soultree_assert_block.h"
#include "JetskiCountDown/JetskiCountDown.h"
#include "JetskiInfoText/JetskiInfoText.h"
#include "GameText/GameText.h"
#include "SpringCollisionModel/SpringCollisionModel.h"
#include "PSXMusic/PSXMusic.h"
#include "Motion/Motion.h"
#include "ControlInterface/ControlInterface.h"
#include "JoystickDevice/JoystickDevice.h"
#include "JetskiCrowdSoundEmitter/JetskiCrowdSoundEmitter.h"
#include "BuoyancyModel/BuoyancyModel.h"
#include "ProcessNode/ProcessNode.h"
#include "JetskiCareerCompetition/JetskiCareerCompetition.h"
#include "JetskiVCRController/JetskiVCRController.h"
#include "PSXWaterDisplacement/PSXWaterDisplacement.h"
#include "Water/Water.h"
#include "_DS3DBUFFER/_DS3DBUFFER.h"
#include "PSXSoundGroup/PSXSoundGroup.h"
#include "ContainerList/ContainerList.h"
#include "JetskiTerrainSoundEmitter/JetskiTerrainSoundEmitter.h"
#include "JetskiSplashSoundEmitter/JetskiSplashSoundEmitter.h"
#include "CollisionCharacter/CollisionCharacter.h"
#include "Scene/SceneAnimation.h"
#include "SceneManagerData/SceneManagerData.h"
#include "CheatFlags/CheatFlags.h"
#include "CollisionTypeRegistry/CollisionTypeRegistry.h"

#include "sinit/AxisVectors.h"

typedef float sceVu0FVECTOR[4] __attribute__((aligned(16)));
typedef float sceVu0FMATRIX[4][4] __attribute__((aligned(16)));

float FastCos(float);
float FastSin(float);

// The rotation helpers of UpdateEdgeMonster and AnimateJetski, defined
// ahead of every function as in retail: mwcc gives each aligned
// `{x, y, z}` initializer a .bss template where the helper is defined, and
// retail's .bss has UpdateEdgeMonster's two, then AnimateJetski's, before
// the functions' statics.
// libvu0 sceVu0RotMatrix-style rotation by `angle` about (x, y, z).
static inline void RotationMatrix(_D3DMATRIX *m, float x, float y, float z, float c, float s, float angle)
{
    sceVu0FVECTOR axis = {x, y, z};
    sceVu0FVECTOR cs = {c, s, 1.0f - c, angle};
    asm("lqc2 vf1, 0x0(%1); lqc2 vf30, 0x0(%2); vmul.xyz vf2, vf1, vf1; vaddy.x vf2, vf2, vf2y; vaddz.x vf2, vf2, vf2z; vrsqrt Q, vf0w, vf2x; vsub.xyzw vf3, vf0, vf0; vaddw.xyzw vf3, vf3, vf1w; vwaitq; vmulq.xyz vf3, vf1, Q; vmuly.xyz vf4, vf3, vf30y; vmulx.xyz vf11, vf3, vf3x; vmulz.xyz vf11, vf11, vf30z; vaddx.x vf11, vf11, vf30x; vaddz.y vf11, vf11, vf4z; vsuby.z vf11, vf11, vf4y; vsub.w vf11, vf11, vf11; vmuly.xyz vf12, vf3, vf3y; vmulz.xyz vf12, vf12, vf30z; vsubz.x vf12, vf12, vf4z; vaddx.y vf12, vf12, vf30x; vaddx.z vf12, vf12, vf4x; vsub.w vf12, vf12, vf12; vmulz.xyz vf13, vf3, vf3z; vmulz.xyz vf13, vf13, vf30z; vaddy.x vf13, vf13, vf4y; vsubx.y vf13, vf13, vf4x; vaddx.z vf13, vf13, vf30x; vsub.w vf13, vf13, vf13; sqc2 vf11, 0x0(%0); sqc2 vf12, 0x10(%0); sqc2 vf13, 0x20(%0); sqc2 vf0, 0x30(%0)"
        : : "r"(m), "r"(axis), "r"(cs) : "memory");
}

// Rotate m's 3x3 part by `angle` about (x, y, z), keeping its translation:
// libvu0's sceVu0RotMatrix-style axis/angle build, then sceVu0MulMatrix's
// loop (m = rot * m), with the translation row parked in vf20 meanwhile.
static inline void RotateMatrixAxis(_D3DMATRIX *m, float x, float y, float z, float c, float s, float angle)
{
    sceVu0FMATRIX rot;
    sceVu0FVECTOR axis = {x, y, z};
    sceVu0FVECTOR cs = {c, s, 1.0f - c, angle};
    asm("lqc2 vf20, 0x30(%0); lqc2 vf1, 0x0(%2); lqc2 vf30, 0x0(%3); vmul.xyz vf2, vf1, vf1; vaddy.x vf2, vf2, vf2y; vaddz.x vf2, vf2, vf2z; vrsqrt Q, vf0w, vf2x; vsub.xyzw vf3, vf0, vf0; vaddw.xyzw vf3, vf3, vf1w; vwaitq; vmulq.xyz vf3, vf1, Q; vmuly.xyz vf4, vf3, vf30y; vmulx.xyz vf11, vf3, vf3x; vmulz.xyz vf11, vf11, vf30z; vaddx.x vf11, vf11, vf30x; vaddz.y vf11, vf11, vf4z; vsuby.z vf11, vf11, vf4y; vsub.w vf11, vf11, vf11; vmuly.xyz vf12, vf3, vf3y; vmulz.xyz vf12, vf12, vf30z; vsubz.x vf12, vf12, vf4z; vaddx.y vf12, vf12, vf30x; vaddx.z vf12, vf12, vf4x; vsub.w vf12, vf12, vf12; vmulz.xyz vf13, vf3, vf3z; vmulz.xyz vf13, vf13, vf30z; vaddy.x vf13, vf13, vf4y; vsubx.y vf13, vf13, vf4x; vaddx.z vf13, vf13, vf30x; vsub.w vf13, vf13, vf13; sqc2 vf11, 0x0(%1); sqc2 vf12, 0x10(%1); sqc2 vf13, 0x20(%1); sqc2 vf0, 0x30(%1)"
        : : "r"(m), "r"(rot), "r"(axis), "r"(cs) : "memory");
    // mwcc's assembler fills the bne delay slot itself with the addi before it.
    asm("lqc2 vf4, 0x0(%0); lqc2 vf5, 0x10(%0); lqc2 vf6, 0x20(%0); lqc2 vf7, 0x30(%0); addi $5, %0, 0; addi $6, %1, 0; addi $7, $0, 4; _loopMulMatrix: lqc2 vf8, 0x0($6); vmulax.xyzw ACC, vf4, vf8x; vmadday.xyzw ACC, vf5, vf8y; vmaddaz.xyzw ACC, vf6, vf8z; vmaddw.xyzw vf9, vf7, vf8w; sqc2 vf9, 0x0($5); addi $7, $7, -1; addi $6, $6, 0x10; addi $5, $5, 0x10; bne $0, $7, _loopMulMatrix; sqc2 vf20, 0x30(%0)"
        : : "r"(m), "r"(rot) : "$5", "$6", "$7", "memory");
}

extern unsigned int WORLDOBJECT_TRIGGER;
extern unsigned int WORLDOBJECT_PHYSICSBODY;
extern unsigned int COLLISION_BIKE;
extern unsigned int WORLDOBJECT_RAMP;
extern unsigned int WORLDOBJECT_BUOY;
extern unsigned int WORLDOBJECT_SOLIDFOLIAGE;
extern unsigned int WORLDOBJECT_SOFTFOLIAGE;
extern unsigned int WORLDOBJECT_PINKBALLOON;
extern unsigned int WORLDOBJECT_BLUEBALLOON;
extern unsigned int WORLDOBJECT_GREENBALLOON;

// ---- src/global/HighSpeedCollisionDetectionCallback.cpp

// VU0 dot product (xyz), as retail's inline-asm helper; the callbacks' copy
// uses the interlocking qmfc2 (Tick's and UpdateEdgeMonster's use qmfc2.ni).
static inline float DotProductI(_D3DVECTOR *a, _D3DVECTOR *b)
{
    float r;
    asm("lqc2 vf4, 0x0(%1); lqc2 vf5, 0x0(%2); vmul.xyz vf5, vf4, vf5; vaddy.x vf5, vf5, vf5y; vaddz.x vf5, vf5, vf5z; qmfc2 $2, vf5; mtc1 $2, %0"
        : "=f"(r) : "r"(a), "r"(b) : "$2");
    return r;
}


void HighSpeedCollisionDetectionCallback(CollisionObject *self, CollisionObject *other)
{
    unsigned int type = other->m_ownerType;
    if (type & WORLDOBJECT_TRIGGER)
        return;
    JetskiVehicle *owner = (JetskiVehicle *)self->m_owner;
    if ((type & WORLDOBJECT_PHYSICSBODY) || (type & COLLISION_BIKE)) {
        // Two pointer locals, declared in this order, give retail's registers.
        _D3DVECTOR *a = &((PhysicsBody *)other->m_ownerData)->m_velocity;
        _D3DVECTOR *b = &owner->m_body->m_velocity;
        // Only flag the hit when the two bodies are not separating.
        if (DotProductI(a, b) <= 0.0f)
            owner->m_xB0 = 1;
    } else {
        owner->m_xB0 = 1;
    }
}


// ---- src/global/ResetCollisionDetectionCallback.cpp


void ResetCollisionDetectionCallback(CollisionObject *self, CollisionObject *other)
{
    unsigned int type = other->m_ownerType;
    if (type & WORLDOBJECT_TRIGGER)
        return;
    JetskiVehicle *owner = (JetskiVehicle *)self->m_owner;
    if ((type & WORLDOBJECT_PHYSICSBODY) || (type & COLLISION_BIKE)) {
        // Two pointer locals, declared in this order, give retail's registers.
        _D3DVECTOR *a = &((PhysicsBody *)other->m_ownerData)->m_velocity;
        _D3DVECTOR *b = &owner->m_body->m_velocity;
        // Only flag the hit when the two bodies are not separating.
        if (DotProductI(a, b) <= 0.0f)
            owner->m_xBC = 1;
    } else {
        owner->m_xBC = 1;
    }
}


// ---- src/JetskiVehicle/ctor.cpp

extern "C" char *strcpy(char *dst, const char *src);

extern JetskiGame *LpGame;

_D3DMATRIX IdentityMatrix();


JetskiVehicle::JetskiVehicle(int enabled) : GameObject(enabled)
{
    int i;
    m_x40 = 0;
    m_steer = 0.0f;
    m_throttle = 0.0f;
    m_boost = 0.0f;
    m_stickX = 0.0f;
    m_stickY = 0.0f;
    m_heldSteer = 0.0f;
    m_heldThrottle = 0.0f;
    m_x60 = 0.0f;
    m_x64 = 0.0f;
    m_boostLevel = 0.0f;
    m_x6C = 1.0f;
    m_x70 = 1.0f;
    m_x78 = 1.0f;
    m_x74 = 1.0f;
    m_x380 = -1;
    m_x48C = 1;
    m_airborne = 1;
    m_x4A0 = ZeroVector;
    m_x4B0 = ZeroVector;
    m_x4C0 = ZeroVector;
    m_x4D0 = ZeroVector;
    m_x4E0 = ZeroVector;
    m_x4F0.x = 1000.0f;
    m_x4F0.y = 1000.0f;
    m_x4F0.z = 1000.0f;
    m_x500.x = 1000.0f;
    m_x500.y = 1000.0f;
    m_x500.z = 1000.0f;
    m_x510.x = 1000.0f;
    m_x510.y = 1000.0f;
    m_x510.z = 1000.0f;
    m_x520.x = 1000.0f;
    m_x520.y = 1000.0f;
    m_x520.z = 1000.0f;
    m_x534 = 0.0f;
    m_x538 = 1.5f;
    m_invulnerable = 0.0f;
    m_x530 = 0;
    m_x550 = 0;
    m_x554 = 0.0f;
    m_terrain = 0;
    m_water = 0;
    strcpy(m_name, "");
    m_x570 = ZeroVector;
    m_riderLateralRange = 1.0f;
    m_riderLongitudinalRange = 1.0f;
    m_x1CC = 0;
    m_x584 = -1;
    m_x590 = ZeroVector;
    m_x5A0 = ZeroVector;
    m_xB4 = 0;
    m_joystick = -1;
    m_player = -1;
    m_x5DC = 0;
    for (i = 0; i < 10; i++)
        m_x5E8[i] = 0;
    m_engineSound = 0;
    m_splashSound = 0;
    m_spraySound = 0;
    m_terrainSound = 0;
    m_x748 = 0;
    m_x750 = 0;
    m_x754 = 0;
    m_responses = 0;
    m_x758 = 1;
    m_x75C = 1;
    m_x760 = 0.0f;
    m_x764 = 0.0f;
    m_edgeMonsterSound = 0;
    m_x76C = 0;
    m_x770 = 0;
    m_x774 = 0;
    m_x778 = 0;
    m_x77C = 100;
    m_x780 = -1.0f;
    m_x790 = 0;
    m_x5D8 = 0.0f;
    m_rider = 0;
    m_x5D0 = 0;
    m_x5D4 = 0.0f;
    m_x5B0 = 0;
    m_x5B4 = 0;
    m_x5C0 = 0.0f;
    m_x5B8 = 0;
    m_x5BC = 0;
    m_x5C8 = 0;
    m_x7C = 0.0f;
    m_performance = 0.0f;
    m_x1F0 = 0.0f;
    m_resetTimer = 0.0f;
    m_x1F4 = 0;
    m_wrongWay = 0;
    m_baseMatrix = IdentityMatrix();
    m_x614 = 0.0f;
    m_x610 = 0.0f;
    m_x1E4 = 0.0f;
    m_x1E8 = 0.0f;
    m_x1D0 = 0;
    m_x1D4 = 0;
    m_x1D8 = 0;
    m_timeElimBuoy = 0;
    m_x1DC = 0;
    m_x1E0 = 0;
    m_gateBonus = 5.0f;
    if (LpGame->m_gameOptions->m_difficulty == 1)
        m_gatePenalty = 25.0f;
    else
        m_gatePenalty = 50.0f;
    m_x300 = 0.0f;
    m_x304 = 0.0f;
    m_x308 = 0.0f;
    m_x30C = 0.0f;
    m_x310 = 0.0f;
    m_x320 = ZeroVector;
    m_x314 = 0.0f;
    m_x548 = 0.0f;
    m_edgeAngle = 0;
    m_x580 = 1;
    m_x544 = -1;
    m_x558 = 0.0f;
    m_x55C = 3.14159274f;
    m_cameraPreset = 0;
    m_x564 = 10.0f;
    m_body = 0;
    m_engine = 0;
    m_gravity = 0;
    m_ground = 0;
    m_hydrodynamics = 0;
    m_airControl = 0;
    m_rollControl = 0;
    m_detection = 0;
    m_collisionModel = 0;
    m_xA4 = 0;
    m_collision = 0;
    m_trigger = 0;
    m_xB0 = 0;
    m_highSpeed = 0;
    m_xBC = 0;
    m_reset = 0;
    m_numObjects = 0;
    for (i = 0; i < 64; i++)
        m_objects[i] = 0;
    m_data = 0;
    m_object = 0;
    m_x5CC = 0.0f;
    m_x1C8 = 0.0f;
    m_renderable = 0;
    m_x620 = 0.0f;
    m_x784 = -1.0f;
    m_x788 = 0;
    m_x78C = 0;
    m_x540 = 0;
    m_roll = 0.0f;
    m_x7A4 = 0;
}


// ---- src/JetskiVehicle/dtor.cpp

extern JetskiGame *LpGame;

JetskiVehicle::~JetskiVehicle()
{
    if (LpGame->m_auralScape) {
        if (m_engineSound)
            LpGame->m_auralScape->RemoveEmitter(m_engineSound);
        if (m_splashSound)
            LpGame->m_auralScape->RemoveEmitter(m_splashSound);
        if (m_spraySound)
            LpGame->m_auralScape->RemoveEmitter(m_spraySound);
        if (m_terrainSound)
            LpGame->m_auralScape->RemoveEmitter(m_terrainSound);
    }
}


// ---- src/JetskiVehicle/DisableAll.cpp

void JetskiVehicle::DisableAll()
{
    if (m_engineSound)
        m_engineSound->DisableEmitter();
    if (m_splashSound)
        m_splashSound->DisableEmitter();
    if (m_spraySound)
        m_spraySound->Stop();
    if (m_terrainSound)
        m_terrainSound->DisableEmitter();
    if (m_x748)
        m_x748->Stop();
    if (m_responses)
        m_responses->StopAllResponses();
    if (m_x750)
        m_x750->Stop();
    if (m_x754)
        m_x754->Stop();
    GameObject::DisableAll();
}


// ---- src/JetskiVehicle/EnableAll.cpp

void JetskiVehicle::EnableAll()
{
    if (m_engineSound)
        m_engineSound->EnableEmitter();
    if (m_splashSound)
        m_splashSound->EnableEmitter();
    if (m_terrainSound)
        m_terrainSound->EnableEmitter();
    GameObject::EnableAll();
}


// ---- src/JetskiVehicle/Create.cpp

extern JetskiGame *LpGame;
void PSXprintf(const char *fmt, ...);
extern "C" char *strcpy(char *dst, const char *src);


int JetskiVehicle::Create(JetskiCompetition *competition, char *physicsFile, char *riderFile, char *riderKeys,
                          char *riderSounds, char *engineSounds, _D3DVECTOR position, _D3DVECTOR orientation,
                          _D3DVECTOR h, _D3DVECTOR i, LightManager *lights, ParticleManager *particles,
                          Terrain *terrain, PSXWater *water, int player, Scene *scene)
{
    m_data = scene;
    m_terrain = terrain;
    m_water = water;
    m_joystick = player;
    ParameterBlock block;
    strcpy(m_name, physicsFile);
    if (!block.OpenFile(m_name, 0)) {
        PSXprintf("Failed to load jetski physical properties (.PHY) as well in JetskiVehicle::Create\n");
        return 0;
    }
    GameObject::Create();
    m_resetPosition = position;
    m_resetOrientation = orientation;
    m_resetUp = YVector;
    m_x200 = position;
    m_x210 = orientation;
    m_x220 = position;
    m_x230 = orientation;
    block.ReadParameterBlock("Physical Properties");
    char sltFile[0x104];
    block.GetParameter("SLTFile", sltFile, 0x104);
    m_renderable = new PSXRenderableObject(1);
    m_renderable->Create(sltFile, 0, 0);
    m_renderable->AddSearchPlacement(scene->m_staticNodes[0], 2);
    AddChild(m_renderable, 2);
    m_object = m_renderable->GetNamedObject("Handlebars");
    m_object->GetMatrix(&m_baseMatrix);
    m_body = new PhysicsRigidBody(1);
    m_body->Create();
    m_body->SetSoultreeObject(m_renderable);
    m_body->m_x320 = 1;
    AddChild(m_body, -1);
    m_gravity = new SelectiveGravityModel(1);
    m_gravity->Create(1);
    m_gravity->AddBody(m_body);
    AddChild(m_gravity, 0);
    m_ground = new JetskiGroundModel(1);
    m_ground->Create(m_body);
    AddChild(m_ground, 0);
    m_engine = new JetskiEngineModel(1);
    m_engine->Create(m_body, m_water);
    m_engine->m_x30 = 1;
    AddChild(m_engine, 0);
    int numWake = 0;
    int numHull = 0;
    int numFroth = 0;
    block.ReadParameterBlock("Wake");
    block.GetParameter("NumberOfWakePoints", &numWake);
    block.ReadParameterBlock("Hull");
    block.GetParameter("NumberOfHullPoints", &numHull);
    block.ReadParameterBlock("Froth");
    block.GetParameter("NumberOfFrothPoints", &numFroth);
    m_hydrodynamics = new HydrodynamicsModel(1);
    m_hydrodynamics->Create(m_water, m_body, numHull, numWake, numFroth);
    m_hydrodynamics->m_x30 = 1;
    AddChild(m_hydrodynamics, 0);
    m_airControl = new JetskiAirControlModel(1);
    m_airControl->Create();
    m_airControl->SetBody(m_body);
    AddChild(m_airControl, 0);
    m_rollControl = new JetskiRollControlModel(1);
    m_rollControl->Create(m_body);
    AddChild(m_rollControl, 0);
    m_rider = new JetskiRider(1);
    if (!m_rider->Create(this, competition, riderFile, riderKeys, riderSounds, lights, terrain, water, player, scene,
                         particles)) {
        PSXprintf("Unable to create JetskiRider in JetskiVehicle::Create\n");
        return 0;
    }
    AddChild(m_rider, -1);
    ResetPosition(0);
    InitializeCollisionData(scene->m_dynamicNodes[0], sltFile);
    LoadPhysicsParameters(&block);
    if (particles && water) {
        m_bowSpray[0] = new BowSprayParticleEmitter(1);
        m_bowSpray[0]->Create(particles, 0, 0);
        AddChild(m_bowSpray[0], -1);
        m_bowSpray[1] = new BowSprayParticleEmitter(1);
        m_bowSpray[1]->Create(particles, 0, 0);
        AddChild(m_bowSpray[1], -1);
        m_bowSpray[2] = new BowSprayParticleEmitter(1);
        m_bowSpray[2]->Create(particles, 0, 0);
        AddChild(m_bowSpray[2], -1);
        m_bowSpray[3] = new BowSprayParticleEmitter(1);
        m_bowSpray[3]->Create(particles, 0, 0);
        AddChild(m_bowSpray[3], -1);
        m_bowSpray[4] = new BowSprayParticleEmitter(1);
        m_bowSpray[4]->Create(particles, 0, 0);
        ((BowSprayParticleEmitter *)m_bowSpray[4])->m_lastForward = ZeroVector;
        ((BowSprayParticleEmitter *)m_bowSpray[4])->m_lastSide = ZeroVector;
        ((BowSprayParticleEmitter *)m_bowSpray[4])->m_forward = ZeroVector;
        ((BowSprayParticleEmitter *)m_bowSpray[4])->m_side = ZeroVector;
        ((BowSprayParticleEmitter *)m_bowSpray[4])->m_x290 = 1;
        ((BowSprayParticleEmitter *)m_bowSpray[4])->m_lastForward.w = 0.0f;
        ((BowSprayParticleEmitter *)m_bowSpray[4])->m_lastSide.w = 0.0f;
        ((BowSprayParticleEmitter *)m_bowSpray[4])->m_forward.w = 0.0f;
        ((BowSprayParticleEmitter *)m_bowSpray[4])->m_side.w = 0.0f;
        AddChild(m_bowSpray[4], -1);
        m_drip = new DripParticleEmitter(1);
        m_drip->Create(particles, 0, 0);
        AddChild(m_drip, -1);
        m_jet = new JetParticleEmitter(1);
        m_jet->Create(particles, 0, 0);
        AddChild(m_jet, -1);
    }
    m_engineSound = new JetskiEngineSoundEmitter(LpGame->m_auralScape, m_data->m_soundGroup, 3, 1);
    if (m_engineSound) {
        if (m_engineSound->Create(this, engineSounds, 100.0f)) {
            AddChild(m_engineSound, -1);
            LpGame->m_auralScape->AddEmitter(m_engineSound);
        } else if (m_engineSound && !m_engineSound->Release()) {
            m_engineSound = 0;
        }
    }
    m_newLap = 0;
    return (int)this;   // retail returns this (a class-pointer return type in
                        // JetskiVehicle.h changes LoadGenericSounds codegen)
}


// ---- src/JetskiVehicle/InitializeCollisionData.cpp

extern "C" char *strcpy(char *dst, const char *src);
extern "C" char *strcat(char *dst, const char *src);

extern JetskiGame *LpGame;
extern char SltGeometryContentDirectory[0x104];




// The last '.' in the name, or 0.
static inline char *FindExtension(char *s)
{
    char *start = s;
    while (*s++)
        ;
    do {
        s--;
    } while (s != start && *s != '.');
    if (*s == '.')
        return s;
    return 0;
}

// The jetski's collision objects: the hull rays (+0xA4), the hull mesh
// (+0xA8), a high-speed ray test (+0xB8), the player's reset test (+0xC0)
// and a trigger sphere (+0xAC), plus the spring collision models.
int JetskiVehicle::InitializeCollisionData(SearchNode *node, char *name)
{
    char path[0x104];

    strcpy(path, SltGeometryContentDirectory);
    strcat(path, name);
    strcpy(FindExtension(path), "Male.col");
    m_xA4 = new CollisionObject(1);
    m_xA4->Create(2);
    m_xA4->m_ownerType = WORLDOBJECT_PHYSICSBODY | COLLISION_BIKE;
    m_xA4->SetTestListType((CollisionObjectTestListType)1);
    m_xA4->SetSoultreeCollisionRays((SoultreeObject *)m_renderable, path, 0);
    m_xA4->m_ownerData = m_body;
    m_xA4->InitializeTransform();
    m_xA4->AddSearchPlacement(node, 2);
    AddChild(m_xA4, -1);

    strcpy(path, SltGeometryContentDirectory);
    strcat(path, name);
    strcpy(FindExtension(path), "Female.col");
    m_collision = new CollisionObject(1);
    m_collision->Create(5);
    m_collision->m_ownerType = WORLDOBJECT_PHYSICSBODY | COLLISION_BIKE;
    m_collision->SetTestListType((CollisionObjectTestListType)1);
    m_collision->SetSoultreeCollision((SoultreeObject *)m_renderable, path);
    m_collision->m_owner = this;
    m_collision->m_ownerData = m_body;
    m_collision->InitializeTransform();
    m_collision->AddSearchPlacement(node, 2);
    AddChild(m_collision, -1);

    m_collisionModel = new JetskiCollisionModel(1);
    m_collisionModel->Create(0x3C);
    m_collisionModel->SetBody((PhysicsBody *)m_body);
    m_collisionModel->AddSpringCollisionPoint(m_xA4, 1, (SoultreeObject *)m_renderable);
    m_collisionModel->m_useCenter = 1;
    m_collisionModel->m_x5C = 0;
    m_collisionModel->m_x54 = 0;
    m_collisionModel->m_x58 = 1;
    m_collisionModel->m_x34 = 1;
    m_collisionModel->m_xAC = 1.0f;
    m_collisionModel->m_x8C = 10.0f;
    AddChild(m_collisionModel, -1);

    m_detection = new SpringCollisionDetectionModel(1);
    m_detection->Create(m_collisionModel, m_terrain, 0, 0, ZeroVector, 0);
    m_detection->m_x64 = 1;
    m_detection->SetCollisionObject(m_xA4);
    {
        SpringCollisionDetectionModel *detection = m_detection;
        detection->m_numObjects = &m_numObjects;
        detection->m_objects = m_objects;
    }
    AddChild(m_detection, -1);

    m_highSpeed = new CollisionObject(1);
    m_highSpeed->Create(2);
    m_highSpeed->m_callback = HighSpeedCollisionDetectionCallback;
    m_highSpeed->SetTestListType((CollisionObjectTestListType)1);
    m_highSpeed->m_ownerType = WORLDOBJECT_PHYSICSBODY | COLLISION_BIKE;
    m_highSpeed->SetDynamicRayCollision(1, 0, 0);
    m_highSpeed->m_owner = this;
    m_highSpeed->m_ownerData = m_body;
    m_highSpeed->InitializeTransform();
    m_highSpeed->AddSearchPlacement(node, 2);
    AddChild(m_highSpeed, -1);

    unsigned char reset = m_player >= 0 &&
                          (LpGame->m_gameOptions->m_raceType == 2 || LpGame->m_gameOptions->m_raceType == 4 ||
                           LpGame->m_gameOptions->m_raceType == 5);
    if (reset) {
        m_reset = new CollisionObject(1);
        m_reset->Create(2);
        m_reset->m_callback = ResetCollisionDetectionCallback;
        m_reset->SetTestListType((CollisionObjectTestListType)1);
        m_reset->m_ownerType = WORLDOBJECT_PHYSICSBODY | COLLISION_BIKE;
        m_reset->SetDynamicRayCollision(3, 0, 0);
        m_reset->m_owner = this;
        m_reset->m_ownerData = m_body;
        m_reset->InitializeTransform();
        m_reset->AddSearchPlacement(node, 2);
        AddChild(m_reset, -1);
    }

    m_trigger = new CollisionObject(1);
    m_trigger->Create(2);
    m_trigger->m_ownerType = WORLDOBJECT_TRIGGER;
    m_trigger->SetSphereCollision(ZeroVector, 10.0f, 0);
    m_trigger->InitializeTransform();
    m_trigger->AddSearchPlacement(node, 2);
    AddChild(m_trigger, -1);

    m_highSpeed->AddToTestList(m_collision);
    m_highSpeed->AddToTestList(m_xA4);
    m_highSpeed->AddToTestList(m_trigger);
    m_collision->AddToTestList(m_highSpeed);
    m_collision->AddToTestList(m_xA4);
    m_collision->AddToTestList(m_trigger);
    m_xA4->AddToTestList(m_highSpeed);
    m_xA4->AddToTestList(m_collision);
    m_xA4->AddToTestList(m_trigger);
    m_trigger->AddToTestList(m_highSpeed);
    m_trigger->AddToTestList(m_collision);
    m_trigger->AddToTestList(m_xA4);

    m_rider->InitializeCollisionData(node);
    if (m_rider->m_wrecker) {
        m_highSpeed->AddToTestList(m_rider->m_wrecker->m_collisionModel->m_collision);
        m_xA4->AddToTestList(m_rider->m_wrecker->m_collisionModel->m_collision);
        m_trigger->AddToTestList(m_rider->m_wrecker->m_collisionModel->m_collision);
    }
    if (reset) {
        m_reset->AddToTestList(m_highSpeed);
        m_reset->AddToTestList(m_collision);
        m_reset->AddToTestList(m_xA4);
        m_reset->AddToTestList(m_trigger);
        m_highSpeed->AddToTestList(m_reset);
        m_collision->AddToTestList(m_reset);
        m_xA4->AddToTestList(m_reset);
        m_trigger->AddToTestList(m_reset);
        if (m_rider->m_wrecker)
            m_reset->AddToTestList(m_rider->m_wrecker->m_collisionModel->m_collision);
    }
    // No return value: retail leaves $v0 as AddToTestList left it.
}


// ---- src/JetskiVehicle/LoadPhysicsParameters.cpp

extern "C" int sprintf(char *buf, const char *fmt, ...);

extern JetskiGame *LpGame;

_D3DMATRIX ZeroMatrix();

int JetskiVehicle::LoadPhysicsParameters(ParameterBlock *params)
{
    float dryWeight;
    float rollScale;
    float pitchScale;
    float yawScale;
    int numWakePoints;
    float wakeScale;
    int numFrothPoints;
    float frothScale;
    int numHullPoints;
    float maxAngleOfAttack;
    float waterSpringSpeed;
    float maxDeckSpeed;
    float maxCompression;
    float spring;
    float damping;
    float impulseSpring;
    float impulseDamping;
    float restitution;
    float friction;
    int i;

    params->ReadParameterBlock("Physical Properties");
    _D3DMATRIX inertia = ZeroMatrix();
    params->GetParameter("Pivot", &_D3DVECTOR(0.0f, 0.0f, 0.0f), &m_x570);
    params->GetParameter("DryWeight", &dryWeight);
    m_body->SetMass(dryWeight / 32.2f);
    params->GetParameter("Gravity", &m_airControl->m_x64);
    params->GetParameter("RollInertiaScale", &rollScale);
    params->GetParameter("PitchInertiaScale", &pitchScale);
    params->GetParameter("YawInertiaScale", &yawScale);
    inertia = ZeroMatrix();
    m_renderable->m_x190_bit6 = 0;
    m_renderable->m_x190_bit4 = 1;
    m_renderable->GetHierarchyBoundingBox(&m_x240, &m_x250);
    // A box's inertia from the hull's bounding box.
    inertia._11 = pitchScale * (m_body->m_mass / 12.0f) * (m_x250.y * m_x250.y + m_x250.z * m_x250.z);
    inertia._22 = yawScale * (m_body->m_mass / 12.0f) * (m_x250.x * m_x250.x + m_x250.z * m_x250.z);
    inertia._33 = rollScale * (m_body->m_mass / 12.0f) * (m_x250.x * m_x250.x + m_x250.y * m_x250.y);
    inertia._44 = 1.0f;
    m_body->SetInertia(inertia);
    params->GetParameter("RiderOffset", &_D3DVECTOR(0.0f, 0.0f, 0.0f), &m_x2D0);
    params->GetParameter("RiderLateralRange", &m_riderLateralRange);
    params->GetParameter("RiderLongitudinalRange", &m_riderLongitudinalRange);

    params->ReadParameterBlock("Wake");
    params->GetParameter("NumberOfWakePoints", &numWakePoints);
    m_hydrodynamics->ResetWakePoints();
    for (i = 0; i < numWakePoints; i++) {
        char name[0x10];
        _D3DVECTOR position;
        sprintf(name, "Wake%d", i + 1);
        params->ReadParameterBlock(name);
        params->GetParameter("Position", &_D3DVECTOR(0.0f, 0.0f, 0.0f), &position);
        params->GetParameter("Scale", &wakeScale);
        m_hydrodynamics->AddWakePoint(position, wakeScale);
    }

    params->ReadParameterBlock("Froth");
    params->GetParameter("NumberOfFrothPoints", &numFrothPoints);
    m_hydrodynamics->ResetFrothPoints();
    for (i = 0; i < numFrothPoints; i++) {
        char name[0x10];
        _D3DVECTOR position;
        sprintf(name, "Froth%d", i + 1);
        params->ReadParameterBlock(name);
        params->GetParameter("Position", &_D3DVECTOR(0.0f, 0.0f, 0.0f), &position);
        params->GetParameter("Scale", &frothScale);
        m_hydrodynamics->AddFrothPoint(position, frothScale);
    }

    m_hydrodynamics->ResetHullPoints();
    params->ReadParameterBlock("Hull");
    params->GetParameter("NumberOfHullPoints", &numHullPoints);
    for (i = 0; i < numHullPoints; i++) {
        char name[0x10];
        _D3DVECTOR position;
        sprintf(name, "Position%d", i + 1);
        params->GetParameter(name, &_D3DVECTOR(0.0f, 0.0f, 0.0f), &position);
        m_hydrodynamics->AddHullPoint(position);
    }

    params->ReadParameterBlock("Engine");
    params->GetParameter("NozzlePosition", &_D3DVECTOR(0.0f, 0.0f, 0.0f), &m_engine->m_x60);
    params->GetParameter("EnginePosition", &_D3DVECTOR(0.0f, 0.0f, 0.0f), &m_engine->m_x70);
    params->GetParameter("HighSpeedEnginePosition", &_D3DVECTOR(0.0f, 0.0f, 0.0f), &m_engine->m_x80);
    params->GetParameter("MaxThrust", &m_engine->m_maxRPM);
    params->GetParameter("MaxThrustBoost", &m_engine->m_xE4);
    params->GetParameter("DryThrustScale", &m_engine->m_xE8);
    params->GetParameter("LowSpeed", &m_engine->m_xEC);
    params->GetParameter("LowSpeedSteer", &m_engine->m_xF0);
    params->GetParameter("HighSpeed", &m_engine->m_xF4);
    params->GetParameter("HighSpeedSteer", &m_engine->m_xF8);
    params->GetParameter("ForeSteerScale", &m_engine->m_x10C);
    params->GetParameter("AftSteerScale", &m_engine->m_x110);
    params->GetParameter("DrySteerScale", &m_engine->m_xFC);
    params->GetParameter("LowSpeedPosition", &m_engine->m_x100);
    params->GetParameter("HighSpeedPosition", &m_engine->m_x104);
    params->GetParameter("ThrottleOffSteering", &m_engine->m_x108);
    params->GetParameter("TopSpeed", &m_engine->m_x50);
    params->GetParameter("TopSpeedBoost", &m_engine->m_x54);
    if (LpGame->m_competition && LpGame->m_competition->m_track && LpGame->m_gameOptions->m_difficulty == 1)
        m_engine->m_maxRPM = m_engine->m_maxRPM * LpGame->m_competition->m_track->m_tuning.z;
    float mphToFps = 1.46666f;
    m_engine->m_x100 = m_engine->m_x100 * mphToFps;
    m_engine->m_x104 = m_engine->m_x104 * mphToFps;
    m_engine->m_x50 = m_engine->m_x50 * mphToFps;
    m_engine->m_x54 = m_engine->m_x54 * mphToFps;

    params->ReadParameterBlock("Hydrodynamics");
    params->GetParameter("HullBottom", &m_hydrodynamics->m_x3C);
    params->GetParameter("MaxDepth", &m_hydrodynamics->m_x44);
    params->GetParameter("MinSurfSpeed", &m_hydrodynamics->m_x48);
    params->GetParameter("MaxSurfSpeed", &m_hydrodynamics->m_x4C);
    params->GetParameter("WaterSpring", &m_hydrodynamics->m_x54);
    params->GetParameter("WaterSpringDepth", &m_hydrodynamics->m_x58);
    params->GetParameter("WaterSpringSpeed", &waterSpringSpeed);
    params->GetParameter("WaterSpringSlope", &m_hydrodynamics->m_x60);
    params->GetParameter("WaterDamping", &m_hydrodynamics->m_x64);
    params->GetParameter("WaterDrag", &m_hydrodynamics->m_x70);
    params->GetParameter("AirDrag", &m_hydrodynamics->m_x74);
    params->GetParameter("WaterTurnDrag", &m_hydrodynamics->m_x78);
    params->GetParameter("AirTurnDrag", &m_hydrodynamics->m_x7C);
    params->GetParameter("WaterFriction", &m_hydrodynamics->m_x68);
    params->GetParameter("MaxAngleOfAttack", &maxAngleOfAttack);
    params->GetParameter("WaterLift", &m_hydrodynamics->m_x84);
    params->GetParameter("AirLift", &m_hydrodynamics->m_x88);
    params->GetParameter("DeckPosition", &_D3DVECTOR(0.0f, 0.0f, 0.0f), &m_hydrodynamics->m_x90);
    params->GetParameter("DeckLift", &m_hydrodynamics->m_xAC);
    params->GetParameter("DeckDrag", &m_hydrodynamics->m_xB4);
    params->GetParameter("DeckPitch", &m_hydrodynamics->m_xB0);
    params->GetParameter("MaxDeckDepth", &m_hydrodynamics->m_xB8);
    params->GetParameter("MaxDeckSpeed", &maxDeckSpeed);
    params->GetParameter("WaterPitch", &m_hydrodynamics->m_xA0);
    params->GetParameter("AligningPitch", &m_hydrodynamics->m_xA8);
    params->GetParameter("AirPitch", &m_hydrodynamics->m_xA4);
    params->GetParameter("AntiSkid", &m_hydrodynamics->m_xC0);
    params->GetParameter("RollDamping", &m_hydrodynamics->m_xC4);
    params->GetParameter("PitchDamping", &m_hydrodynamics->m_xC8);
    params->GetParameter("YawDamping", &m_hydrodynamics->m_xCC);
    params->GetParameter("AligningMomentCoefficient", &m_hydrodynamics->m_aligningMoment);
    m_hydrodynamics->m_xBC = mphToFps * maxDeckSpeed;
    m_hydrodynamics->m_x5C = mphToFps * waterSpringSpeed;
    m_hydrodynamics->m_x6C = m_hydrodynamics->m_x5C * m_hydrodynamics->m_x60;
    m_hydrodynamics->m_x80 = 0.01745329f * maxAngleOfAttack;
    m_hydrodynamics->m_x48 = m_hydrodynamics->m_x48 * mphToFps;
    m_hydrodynamics->m_x4C = m_hydrodynamics->m_x4C * mphToFps;
    bool surfRange = m_hydrodynamics->m_x4C != m_hydrodynamics->m_x48;
    if (surfRange)
        m_hydrodynamics->m_x50 = 1.0f / (m_hydrodynamics->m_x4C - m_hydrodynamics->m_x48);
    params->GetParameter("RollAngleScale", &m_rollControl->m_x44);
    params->GetParameter("RollRateScale", &m_rollControl->m_x48);
    params->GetParameter("RollForceScale", &m_rollControl->m_x4C);
    params->GetParameter("MaxRollAngle", &m_rollControl->m_maxAngle);
    m_rollControl->m_maxAngle = 0.01745329f * m_rollControl->m_maxAngle;

    params->ReadParameterBlock("AirControl");
    params->GetParameter("AeroForce", &m_airControl->m_x58);
    params->GetParameter("YawScale", &m_airControl->m_x70.y);
    params->GetParameter("PitchScale", &m_airControl->m_x70.x);
    params->GetParameter("RollScale", &m_airControl->m_x70.z);
    params->GetParameter("DampingScale", &m_airControl->m_x60);
    params->GetParameter("UnrollScale", &m_airControl->m_x5C);
    params->GetParameter("AirControlTime", &m_airControl->m_x50);

    params->ReadParameterBlock("Collisions");
    params->GetParameter("MaxCompression", &maxCompression);
    params->GetParameter("Spring", &spring);
    params->GetParameter("Damping", &damping);
    params->GetParameter("ImpulseSpring", &impulseSpring);
    params->GetParameter("ImpulseDamping", &impulseDamping);
    params->GetParameter("FrictionTorqueScale", &m_collisionModel->m_xBC);
    params->GetParameter("Restitution", &restitution);
    params->GetParameter("AngularImpulseDamping", &m_collisionModel->m_x130);
    m_collisionModel->SetCollisionParameters(maxCompression, spring, damping, impulseSpring, impulseDamping,
                                               restitution, 0.0f, 0.0f);
    for (i = 0; i < 10; i++) {
        char name[0x40];
        sprintf(name, "Friction%d", i);
        params->GetParameter(name, &friction);
        m_collisionModel->SetFriction(i, friction);
    }

    params->ReadParameterBlock("Wrecks");
    params->GetParameter("VerticalFpsLimit", &m_x510.y);
    params->GetParameter("VerticalHardLandingFps", &m_x520.y);
    params->GetParameter("LateralFpsLimit", &m_x510.x);
    params->GetParameter("LateralHardLandingFps", &m_x520.x);
    params->GetParameter("LongitudinalFpsLimit", &m_x510.z);
    params->GetParameter("LongitudinalHardLandingFps", &m_x520.z);

    params->ReadParameterBlock("Inputs");
    params->GetParameter("XAxisRateLimit", &m_x6C);
    params->GetParameter("YAxisRateLimit", &m_x70);
    params->GetParameter("RampUpThrottleFilterConstant", &m_x78);
    params->GetParameter("RampDownThrottleFilterConstant", &m_x74);
    params->GetParameter("HandlebarTurnAngle", &m_x7C);
    m_x7C *= 0.01745329f;

    params->ReadParameterBlock("Particle EmitPoints");
    params->GetParameter("Nose", &_D3DVECTOR(0.0f, -0.5f, 4.2f), &m_nose);
    params->GetParameter("Tail", &_D3DVECTOR(0.0f, -1.0f, -3.8f), &m_tail);
    params->GetParameter("Head", &_D3DVECTOR(0.0f, 3.2f, 1.0f), &m_head);
    params->GetParameter("LeftSide", &_D3DVECTOR(-1.9f, -0.5f, 1.0f), &m_leftSide);
    params->GetParameter("JetPosition", &_D3DVECTOR(0.0f, -0.2f, -4.3f), &m_jetPosition);
    params->GetParameter("JetDirection", &_D3DVECTOR(0.0f, 0.0f, -1.0f), &m_jetDirection);
    params->GetParameter("JetUp", &_D3DVECTOR(0.0f, 1.0f, 0.0f), &m_jetUp);
    params->GetParameter("JetSide", &_D3DVECTOR(1.0f, 0.0f, 0.0f), &m_jetSide);
    // The other side and the four corners of the stern, from the sides and tail.
    m_rightSide = m_leftSide;
    m_rightSide.x = -m_rightSide.x;
    m_leftTail = m_tail;
    m_leftTail.x = m_leftSide.x;
    m_rightTail = m_tail;
    m_rightTail.x = m_rightSide.x;
    m_x6C0 = m_leftTail;
    m_x6C0.x += 0.5f;
    m_x6D0 = m_rightTail;
    m_x6D0.x -= 0.5f;
    m_hullLength = m_nose.z - m_tail.z;
    m_tailToSide = m_leftSide.z - m_tail.z;
    m_sideToNose = m_hullLength - m_tailToSide;
    m_invTailToSide = 1.0f / m_tailToSide;
    m_invSideToNose = 1.0f / m_sideToNose;
    m_sideRatio = m_sideToNose / m_tailToSide;
}


// ---- src/JetskiVehicle/Render3D.cpp

extern JetskiGame *LpGame;

int JetskiVehicle::Render3D()
{
    static int JSVehicleR3DID = -1; // profiler ID (unused)
    DebugOverlay *overlay = LpGame->m_debugOverlay;
    if (overlay) {
        if (m_x380 < 0)
            m_x380 = overlay->m_numPages++;
        LpGame->m_debugOverlay->PrintfHeading(m_x380, "JetskiVehicle Overlay");
        LpGame->m_debugOverlay->PrintfDetail(m_x380, "Steer Angle %f", m_engine->m_x48);
        LpGame->m_debugOverlay->PrintfDetail(m_x380, "Throttle %f", m_engine->m_throttle);
        LpGame->m_debugOverlay->PrintfDetail(m_x380, "Position (%.1f,%.1f,%.1f)", m_body->m_x60.x, m_body->m_x60.y, m_body->m_x60.z);
        LpGame->m_debugOverlay->PrintfHeading(m_x380, "Terrain Friction");
        LpGame->m_debugOverlay->PrintfDetail(m_x380, "Friction Index %d", m_collisionModel->m_surfaceType);
        LpGame->m_debugOverlay->PrintfDetail(m_x380, "Friction Table %f", m_collisionModel->m_frictions[m_collisionModel->m_surfaceType]);
    }
    return GameObject::Render3D();
}


// ---- src/JetskiVehicle/ResetPosition.cpp

extern JetskiGame *LpGame;


// Put the jetski back on the course: at its start (or reset) point, on top
// of the terrain or water, upright, with the follow camera reset.
void JetskiVehicle::ResetPosition(int reset)
{
    _D3DMATRIX m;   // declared first: retail puts it at the bottom of the frame
    if (m_x544 > -1) {
        int object = LpGame->m_competition->m_xD4;
        if (object > -1)
            m_data->m_animations->m_animations[object].m_automaticSequence = 1;
        m_x790 = 0;
        m_gravity->SetGravity(32.2f);
        m_x548 = 0;
        m_x580 = 1;
        m_x770 = 0;
        m_rider->m_wrecker->m_x160 = 1;
        m_renderable->Unhide();
        m_rider->m_root->Unhide();
        if (object > -1)
            m_data->m_animations->m_animations[object].m_character->m_root->Unhide();
        if (m_edgeMonsterSound && m_edgeMonsterSound->IsPlaying() && m_edgeMonsterSound->m_currentSound == 3)
            m_edgeMonsterSound->Stop();
        if (m_x544 > 1) {
            if (LpGame->m_gameOptions->m_raceType == 5) {
                LpGame->m_competition->m_cameraControllers[0]->m_followCamera->m_x1A0.y = 0.0f;
                LpGame->m_competition->m_cameraControllers[0]->m_followCamera->ChangeToPreset(m_cameraPreset);
                LpGame->m_competition->m_cameraControllers[0]->m_followCamera->m_settings[2] = m_x558;
                LpGame->m_competition->m_cameraControllers[0]->m_followCamera->m_settings[1] = m_x55C;
                LpGame->m_competition->m_cameraControllers[0]->m_followCamera->m_settings[0] = m_x564;
            } else {
                LpGame->m_competition->m_cameraControllers[m_player]->m_followCamera->m_x1A0.y = 0.0f;
                LpGame->m_competition->m_cameraControllers[m_player]->m_followCamera->ChangeToPreset(m_cameraPreset);
                LpGame->m_competition->m_cameraControllers[m_player]->m_followCamera->m_settings[2] = m_x558;
                LpGame->m_competition->m_cameraControllers[m_player]->m_followCamera->m_settings[1] = m_x55C;
                LpGame->m_competition->m_cameraControllers[m_player]->m_followCamera->m_settings[0] = m_x564;
            }
        }
        m_x544 = -1;
    }
    _D3DVECTOR ground;
    _D3DVECTOR water;
    if (LpGame->m_competition && LpGame->m_competition->m_status == 5) {
        if (m_reset && !m_x1DC && !m_x1D0) {
            _D3DVECTOR position = m_x220;
            ground = position;
            water = position;
            if (m_terrain)
                m_terrain->GetGridPointIntersection(&ground, 0, 0, 0);
            if (m_water)
                m_water->GetGridPointIntersection(&water, 0);
            if (water.y > ground.y)
                position.y = water.y;
            else
                position.y = ground.y;
            m_renderable->SetPosition(0, &position);
            m_renderable->SetOrientation(0, &m_x230, &YVector, 0, 1, 0);
            m_renderable->ValidateWorldMatrix();
            m_body->m_x60 = position;
            if (m_player >= 0) {
                if (LpGame->m_competition->m_numPlayers - 1 < m_player)
                    LpGame->m_competition->m_cameraControllers[0]->m_followCamera->m_resetFilters = 1;
                else
                    LpGame->m_competition->m_cameraControllers[m_player]->m_followCamera->m_resetFilters = 1;
            }
        } else {
            if (LpGame->m_gameOptions->m_raceType == 5 && m_x1DC) {
                m_resetPosition = m_data->m_startPosition;
                m_resetOrientation = m_data->m_startDirection;
                m_resetUp = YVector;
            }
            ground = m_resetPosition;
            water = m_resetPosition;
            if (m_terrain)
                m_terrain->GetGridPointIntersection(&ground, 0, 0, 0);
            if (m_water)
                m_water->GetGridPointIntersection(&water, 0);
            if (water.y > ground.y)
                m_resetPosition.y = water.y;
            else
                m_resetPosition.y = ground.y;
            m_ownStatus->m_lastTrackPosition = m_ownStatus->m_x6C0;
            m_renderable->SetPosition(0, &m_resetPosition);
            m_renderable->SetOrientation(0, &m_resetOrientation, &m_resetUp, 0, 1, 0);
            m_renderable->ValidateWorldMatrix();
            m_body->m_x60 = m_resetPosition;
            if (m_player >= 0) {
                if (LpGame->m_competition->m_numPlayers - 1 < m_player)
                    LpGame->m_competition->m_cameraControllers[0]->m_followCamera->m_resetFilters = 1;
                else
                    LpGame->m_competition->m_cameraControllers[m_player]->m_followCamera->m_resetFilters = 1;
            }
        }
    } else {
        _D3DVECTOR ground2 = m_resetPosition;
        _D3DVECTOR water2 = m_resetPosition;
        if (m_terrain)
            m_terrain->GetGridPointIntersection(&ground2, 0, 0, 0);
        if (m_water)
            m_water->GetGridPointIntersection(&water2, 0);
        if (water2.y > ground2.y)
            m_resetPosition.y = water2.y;
        else
            m_resetPosition.y = ground2.y;
        m_renderable->SetPosition(0, &m_resetPosition);
        m_renderable->SetOrientation(0, &m_resetOrientation, &m_resetUp, 0, 1, 0);
        m_renderable->ValidateWorldMatrix();
        m_body->m_x60 = m_resetPosition;
    }
    m_body->Reset();
    InitializeStatus();
    if (m_edgeMonsterSound)
        m_edgeMonsterSound->Stop();
    m_renderable->GetMatrix(0, &m);
    m_rider->m_root->SetMatrix(0, &m);
    m_rider->TickActionByPercentage(m_rider->m_raceMotions[1], 0.5f, 0);
    if (reset)
        MakeInvulnerable();
}


// ---- src/JetskiVehicle/ProcessInputs.cpp

extern JetskiGame *LpGame;

static inline float Fmin(float a, float b)
{
    float r;
    asm("min.s $f8, %1, %2; swc1 $f8, 0(%0)" : : "r"(&r), "f"(a), "f"(b) : "$f8", "memory");
    return r;
}

static inline float Fmax(float a, float b)
{
    float r;
    asm("max.s $f8, %1, %2; swc1 $f8, 0(%0)" : : "r"(&r), "f"(a), "f"(b) : "$f8", "memory");
    return r;
}

void JetskiVehicle::ProcessInputs(float dt)
{
    if (m_x790 || (LpGame->m_competition->m_raceStatus->m_flags & 2)) {
        m_boost = 0.0f;
        m_steer = 0.0f;
        m_throttle = 0.0f;
    } else if (m_rider->m_wobbling) {
        m_steer = 0.0f;
    }
    if (m_rider->m_stuntsKey && m_rider->m_stuntsKey->CheckStuntKeyHolding()) {
        m_stickX = m_steer;
        m_stickY = m_throttle;
        if (m_rider->m_x898) {
            m_x40 = 0;
            m_steer = 0.0f;
            m_throttle *= 0.5f;
        } else {
            m_x40 = 1;
        }
        if (m_body->m_x30) {
            JetskiStuntsKey *key = m_rider->m_stuntsKey;
            unsigned char busy = key->m_numKeys || key->m_x148;
            if (busy) {
                m_steer = 0.0f;
                m_throttle = 0.0f;
            }
        }
    } else {
        m_x40 = 0;
        m_heldSteer = m_steer;
        m_heldThrottle = m_throttle;
    }
    float steerRate = m_x6C * dt;
    float throttleRate = m_x70 * dt;
    m_x60 += Fmin(Fmax(m_steer - m_x60, -steerRate), steerRate);
    m_x64 += Fmin(Fmax(m_throttle - m_x64, -throttleRate), throttleRate);
    if (LpGame->m_gameOptions->m_raceType == 3) {
        m_x300 = m_x60;
        m_x304 = m_x64;
    }
    if (m_x5D4 > 0.0f)
        m_boost = 0.0f;
    m_x5D4 -= dt;
    if (m_x5D4 < 0.0f)
        m_x5D4 = 0.0f;
    if (m_boost > m_boostLevel)
        m_boostLevel = m_boostLevel * m_x78 + m_boost * (1.0f - m_x78);
    else
        m_boostLevel = m_boostLevel * m_x74 + m_boost * (1.0f - m_x74);
}


// ---- src/JetskiVehicle/RunSimulation.cpp

extern JetskiGame *LpGame;


static inline float Fabs(float x)
{
    float r;
    asm("abs.s $f8, %1; swc1 $f8, 0(%0)" : : "r"(&r), "f"(x) : "$f8", "memory");
    return r;
}

// libvu0 sceVu0Normalize.
static inline void Normalize(_D3DVECTOR *out, _D3DVECTOR *in)
{
    asm("lqc2 vf4, 0x0(%1); vmul.xyz vf5, vf4, vf4; vaddy.x vf5, vf5, vf5y; vaddz.x vf5, vf5, vf5z; vrsqrt Q, vf0w, vf5x; vsub.xyzw vf6, vf0, vf0; vaddw.xyzw vf6, vf6, vf4w; vwaitq; vmulq.xyz vf6, vf4, Q; sqc2 vf6, 0x0(%0)"
        : : "r"(out), "r"(in) : "memory");
}

// One physics step: the trigger-sphere search, terrain contact, the rider's
// weight shift and performance into the engine, then at most two sub-steps of
// the force models and the rigid body (recording the peak of two body
// vectors), the airborne bookkeeping, and the reset-point update.
void JetskiVehicle::RunSimulation(float dt)
{
    float maxStep = 0.5f * dt;
    int steps = 0;
    _D3DVECTOR normal;
    _D3DVECTOR unused;                  // constructed, never used (as in retail)
    static int CollisionSearchID = -1;  // unused profiler IDs
    bool aboveGround;
    float radius = m_x250.z;
    m_trigger->UpdateSphereCollision(&m_x240, &radius);
    m_trigger->UpdateCollisionInfo(&m_renderable->m_worldMatrix);
    m_trigger->SearchForCollisions(64, &m_numObjects, m_objects);
    if (m_numObjects > 0)
        m_detection->m_objectCollision = 1;
    else
        m_detection->m_objectCollision = 0;
    if (m_terrain) {
        _D3DVECTOR ground;
        _D3DVECTOR position = m_body->m_x60;
        ground = position;
        m_terrain->GetGridPointIntersection(&ground, &normal, 0, 0);
        m_x614 = ground.y;
        aboveGround = m_x614 - position.y > 0.0f;
        float limit = m_x250.z;         // a named local: its vreg must follow the difference's
        if (position.y - ground.y > limit)
            m_collisionModel->m_groundCollision = 0;
        else
            m_collisionModel->m_groundCollision = 1;
    } else {
        aboveGround = 0;
        m_collisionModel->m_groundCollision = 0;
    }
    m_body->m_centerOfMass.x = m_x570.x + m_x60 * m_riderLateralRange;
    m_body->m_centerOfMass.y = m_x570.y;
    m_body->m_centerOfMass.z = m_x570.z - m_x64 * m_riderLongitudinalRange;
    if (LpGame->m_cheats->fullPerformance && m_player >= 0) {
        m_performance = 1.5f;
    } else if (m_ownStatus && LpGame->m_gameOptions->m_raceType != 5 && LpGame->m_gameOptions->m_raceType != 4) {
        m_performance = 0.01f * (m_ownStatus->m_x6F4 + m_ownStatus->m_x6F8);
    } else {
        m_performance = 1.0f;
    }
    m_engine->m_x44 = m_performance;
    m_engine->m_throttle = m_boostLevel;
    m_engine->m_x48 = m_x60;
    m_engine->m_x4C = m_x64;
    if ((m_collisionModel->m_x70 && !m_x1D8) || m_collisionModel->m_x44)
        m_engine->m_reversed = 1;
    else
        m_engine->m_reversed = 0;
    m_ground->m_onGround = (!m_hydrodynamics->m_inWater && m_collisionModel->m_x3C);
    m_hydrodynamics->m_x38 = m_boostLevel;
    m_rollControl->m_enabled = !m_body->m_x30;
    m_airControl->SetInputs(!m_body->m_x30, (m_rider->m_x484 && m_rider->m_x494 < 28), m_x40, m_steer, m_throttle);
    static int CollisionDetectionID = -1;
    m_detection->SimulationTick(dt);
    m_x1D8 = 0;
    m_engine->m_x3C = 0;
    if (m_collisionModel->m_hitObject && (m_collisionModel->m_hitObject->m_ownerType & WORLDOBJECT_RAMP)) {
        m_x1D8 = 1;
        m_engine->m_x3C = 1;
    }
    static int EngineModelAID = -1;
    m_engine->UpdateForceTick(dt);
    static int HydroAID = -1;
    if (m_x580)
        m_hydrodynamics->ComputeForceTick(dt);
    static int SimLoopID = -1;
    float t = 0.0f;
    float step = Fmin(dt, maxStep);
    m_x4D0 = ZeroVector;
    m_x4E0 = ZeroVector;
    while (t < dt && steps < 2) {
        static int GravityID = -1;
        m_gravity->SetForceTick(step);
        static int EngineModelBID = -1;
        m_engine->SetForceTick(step);
        static int HydroID = -1;
        if (m_x580)
            m_hydrodynamics->SetForceTick(step);
        static int AeroID = -1;
        m_airControl->SimulationTick(step);
        static int RollID = -1;
        m_rollControl->SetForceTick(step);
        static int CollisionModelID = -1;
        m_collisionModel->SimulationTick(step);
        static int GroundID = -1;
        m_ground->SimulationTick(step);
        if (Fabs(m_body->m_x1C0.x) > Fabs(m_x4E0.x))
            m_x4E0.x = m_body->m_x1C0.x;
        if (Fabs(m_body->m_x1C0.y) > Fabs(m_x4E0.y))
            m_x4E0.y = m_body->m_x1C0.y;
        if (Fabs(m_body->m_x1C0.z) > Fabs(m_x4E0.z))
            m_x4E0.z = m_body->m_x1C0.z;
        static int JetskiBodyID = -1;
        m_body->SimulationTick(step);
        if (Fabs(m_body->m_xC0.x) > Fabs(m_x4D0.x))
            m_x4D0.x = m_body->m_xC0.x;
        if (Fabs(m_body->m_xC0.y) > Fabs(m_x4D0.y))
            m_x4D0.y = m_body->m_xC0.y;
        if (Fabs(m_body->m_xC0.z) > Fabs(m_x4D0.z))
            m_x4D0.z = m_body->m_xC0.z;
        t += step;
        step = Fmin(maxStep, dt - t);
        steps++;
    }
    m_x610 = m_hydrodynamics->m_x120;
    m_x5B4 = m_body->m_x30;
    m_body->m_x30 = (!m_hydrodynamics->m_inWater && (!m_collisionModel->m_x3C || m_collisionModel->m_x44));
    m_airborne = m_x48C;
    m_x48C = m_hydrodynamics->m_inWater;
    if (m_body->m_x30 && !m_x5B4) {
        m_x590 = m_body->m_velocity;
        m_x5A0 = m_body->m_x60;
    }
    if (m_reset) {
        static int ResetCollisionsID = -1;
        if (!m_x530 && m_x48C && m_x1F0 > 0.25f && m_engine->m_x34 && !m_collisionModel->m_x3C &&
            !m_collisionModel->m_x6C) {
            _D3DVECTOR dir;
            _D3DVECTOR up;
            m_xBC = 0;
            radius = 20.0f;
            m_trigger->UpdateSphereCollision(&m_x240, &radius);
            m_trigger->UpdateCollisionInfo(&m_renderable->m_worldMatrix);
            m_trigger->SearchForCollisions(64, &m_numObjects, m_objects);
            if (m_numObjects > 0) {
                _D3DVECTOR rays[6];
                _D3DVECTOR forward;
                m_renderable->GetOrientation(&forward, &up);
                if (up.y < 0.0f)
                    forward = _D3DVECTOR(-forward.x, -forward.y, -forward.z);
                forward.y = 0.0f;
                Normalize(&dir, &forward);
                forward = 20.0f * dir;
                rays[0] = m_body->m_x60;
                rays[0].y -= 2.0f;
                rays[1] = rays[0] + forward;
                forward.y += 8.28f;
                rays[2] = rays[0];
                rays[3] = rays[2] + forward;
                rays[4] = rays[0];
                rays[4].y += 8.28f;
                rays[5] = rays[4] + forward;
                m_reset->SetDynamicRays(rays, 0);
                m_reset->CheckForCollision(m_numObjects, m_objects);
            }
            if (!m_xBC && !aboveGround) {
                if (m_terrain) {
                    if (m_engine->m_x90.y - m_x614 > 6.0f && normal.y > 0.966f) {
                        m_x220 = m_x200;
                        m_x200 = m_body->m_x60;
                        m_x230 = m_x210;
                        if (m_numObjects <= 0) {
                            m_renderable->GetOrientation(&dir, &up);
                            if (up.y < 0.0f)
                                dir = _D3DVECTOR(-dir.x, -dir.y, -dir.z);
                            dir.y = 0.0f;
                        }
                        m_x210 = dir;
                        m_x1F0 = 0.0f;
                    }
                } else {
                    m_x220 = m_x200;
                    m_x200 = m_body->m_x60;
                    m_x230 = m_x210;
                    if (m_numObjects <= 0) {
                        m_renderable->GetOrientation(&dir, &up);
                        if (up.y < 0.0f)
                            dir = _D3DVECTOR(-dir.x, -dir.y, -dir.z);
                        dir.y = 0.0f;
                    }
                    m_x210 = dir;
                    m_x1F0 = 0.0f;
                }
            }
        }
        m_x1F0 += dt;
    }
    m_collision->UpdateCollisionInfo(&m_renderable->m_worldMatrix);
}


// ---- src/JetskiVehicle/AnimateJetski.cpp





void JetskiVehicle::AnimateJetski()
{
    // Lean the hull: roll about local z by -m_x7C * m_x60.
    float angle = -m_x7C * m_x60;
    m_object->SetMatrix(m_baseMatrix);
    _D3DMATRIX *m = &m_object->m_localMatrix;
    float c = FastCos(angle);
    float s = FastSin(angle);
    RotateMatrixAxis(m, 0.0f, 0.0f, 1.0f, c, s, angle);
}


// ---- src/JetskiVehicle/UpdateParticles.cpp
// `t` lives in memory: it is the only 4-byte memory local, so the frame layout
// (alignment groups, smallest first) puts it at the top of the frame, above the
// vectors. A local's address that survives to codegen keeps it in memory; `float *pt
// = &t + 1; pt--; *pt = ...;` does that and emits no address arithmetic (IRO folds
// the pointer step after the address-taken scan). The store after the multiply, the
// reload after the GetGridPointIntersection calls, the reload for bowRate and the
// else arm's store of sideRate's f27 are then ordinary memory accesses. Also: the
// nose case's `m_drip->m_particlesEnabled = 0;` is after the inner if/else (the
// head-under arm keeps its own copy), which puts retail's join label before it.
// (inline_depth(6): TU-wide, at the top)



static inline void Subtract(_D3DVECTOR &a, const _D3DVECTOR &b)
{
    a.x -= b.x;
    a.y -= b.y;
    a.z -= b.z;
}


// Places the spray, drip and jet emitters on the hull points (moved down to the
// water line where the hull is partly submerged) and sets their velocities and rates.
void JetskiVehicle::UpdateParticles(float dt)
{
    static int particleCost = -1;       // unused profiler ID
    _D3DVECTOR nose = m_body->m_object->TransformVectorLocalToWorld(&m_nose);
    _D3DVECTOR tail = m_body->m_object->TransformVectorLocalToWorld(&m_tail);
    _D3DVECTOR leftSide = m_body->m_object->TransformVectorLocalToWorld(&m_leftSide);
    _D3DVECTOR rightSide = m_body->m_object->TransformVectorLocalToWorld(&m_rightSide);
    _D3DVECTOR head = m_body->m_object->TransformVectorLocalToWorld(&m_head);
    _D3DVECTOR rightTail = m_body->m_object->TransformVectorLocalToWorld(&m_rightTail);
    _D3DVECTOR leftTailIn = m_body->m_object->TransformVectorLocalToWorld(&m_x6C0);
    _D3DVECTOR rightTailIn = m_body->m_object->TransformVectorLocalToWorld(&m_x6D0);
    _D3DVECTOR leftTail = m_body->m_object->TransformVectorLocalToWorld(&m_leftTail);
    _D3DVECTOR jetPosition = m_body->m_object->TransformVectorLocalToWorld(&m_jetPosition);
    _D3DVECTOR jetUp = m_body->m_object->RotateVectorLocalToWorld(&m_jetUp);
    _D3DVECTOR jetSide = m_body->m_object->RotateVectorLocalToWorld(&m_jetSide);
    _D3DVECTOR jetDirection = m_body->m_object->RotateVectorLocalToWorld(&m_jetDirection);

    m_drip->UpdatePosition(nose, 0.5f * (rightTail + leftTailIn));
    m_jet->UpdatePosition(jetPosition);
    m_jet->m_x210 = jetUp;
    m_jet->m_x220 = jetSide;
    static float jetVelocity = 18.0f;
    m_jet->m_emitRate = 40.0f + 320.0f * m_boostLevel;
    m_jet->UpdateVelocity(m_body->m_velocity + (2.0f + jetVelocity * m_boostLevel) * jetDirection);
    m_jet->m_particlesEnabled = 1;

    _D3DVECTOR noseWater = nose;
    _D3DVECTOR tailWater = tail;
    _D3DVECTOR headWater = head;
    m_water->GetGridPointIntersection(&noseWater, 0);
    m_water->GetGridPointIntersection(&tailWater, 0);
    m_water->GetGridPointIntersection(&headWater, 0);
    float sideRate = 1.0f;
    rightTailIn.y = tailWater.y;
    leftTail.y = tailWater.y;
    float bowRate = m_sideRatio;
    bool tailUnder = tail.y < tailWater.y;
    bool headUnder = head.y < headWater.y;
    bool noseUnder = nose.y < noseWater.y;

    if (noseUnder != 0) {
        if (headUnder) {
            m_bowSpray[0]->UpdatePosition(nose, leftSide);
            m_bowSpray[1]->UpdatePosition(rightSide, nose);
            m_bowSpray[0]->m_particlesEnabled = 0;
            m_bowSpray[1]->m_particlesEnabled = 0;
            m_bowSpray[2]->m_particlesEnabled = 0;
            m_bowSpray[3]->m_particlesEnabled = 0;
            m_bowSpray[4]->m_particlesEnabled = 0;
            m_drip->m_particlesEnabled = 0;
        } else {
            _D3DVECTOR d = nose - head;
            float inv;
            if (d.x == 0.0f)
                inv = 1.0f / d.z;
            else
                inv = 1.0f / d.x;
            float rise = noseWater.y - tailWater.y;
            float t = (head.y - tailWater.y) / (rise * inv - d.y * inv);
            t = 1.0f - t * inv;
            _D3DVECTOR offset = t * d;
            Subtract(nose, offset);
            Subtract(leftSide, offset);
            Subtract(rightSide, offset);
            m_water->GetGridPointIntersection(&nose, 0);
            m_water->GetGridPointIntersection(&leftSide, 0);
            m_water->GetGridPointIntersection(&rightSide, 0);
            m_water->GetGridPointIntersection(&leftTailIn, 0);
            m_water->GetGridPointIntersection(&rightTail, 0);
            m_bowSpray[0]->UpdatePosition(nose, leftSide);
            m_bowSpray[1]->UpdatePosition(nose, rightSide);
            m_bowSpray[2]->UpdatePosition(leftSide, leftTailIn);
            m_bowSpray[3]->UpdatePosition(rightSide, rightTail);
            m_bowSpray[4]->UpdatePosition(leftTail, rightTailIn);
            m_bowSpray[0]->m_particlesEnabled = 1;
            m_bowSpray[1]->m_particlesEnabled = 1;
            m_bowSpray[2]->m_particlesEnabled = 1;
            m_bowSpray[3]->m_particlesEnabled = 1;
            m_bowSpray[4]->m_particlesEnabled = 1;
        }
        m_drip->m_particlesEnabled = 0;
        m_drip->StopDrip();
        if (tailUnder)
            m_jet->SetJet();
    } else if (tailUnder) {
        _D3DVECTOR d = nose - tail;
        float inv;
        if (d.x == 0.0f)
            inv = 1.0f / d.z;
        else
            inv = 1.0f / d.x;
        float rise = noseWater.y - tailWater.y;
        float t;
        float *pt = &t + 1;
        pt--;
        float q = (tail.y - tailWater.y) / (rise * inv - d.y * inv);
        *pt = q * inv;
        m_water->GetGridPointIntersection(&nose, 0);
        m_water->GetGridPointIntersection(&leftSide, 0);
        m_water->GetGridPointIntersection(&rightSide, 0);
        m_water->GetGridPointIntersection(&leftTailIn, 0);
        m_water->GetGridPointIntersection(&rightTail, 0);
        float along = t * m_hullLength;
        if (along > m_tailToSide) {
            _D3DVECTOR left = leftSide + t * (nose - leftSide);
            _D3DVECTOR right = rightSide + t * (nose - rightSide);
            m_bowSpray[0]->UpdatePosition(left, leftSide);
            m_bowSpray[1]->UpdatePosition(right, rightSide);
            m_bowSpray[0]->m_particlesEnabled = 1;
            m_bowSpray[1]->m_particlesEnabled = 1;
            bowRate = m_invSideToNose * (m_sideRatio * (m_sideToNose * t));
        } else {
            t = m_invTailToSide * along;
            sideRate = t;
            leftSide = t * (leftSide - leftTailIn) + leftTailIn;
            rightSide = t * (rightSide - rightTail) + rightTail;
            m_bowSpray[0]->UpdatePosition(leftSide, leftSide);
            m_bowSpray[1]->UpdatePosition(rightSide, rightSide);
            m_bowSpray[0]->m_particlesEnabled = 0;
            m_bowSpray[1]->m_particlesEnabled = 0;
        }
        m_bowSpray[2]->UpdatePosition(leftSide, leftTailIn);
        m_bowSpray[3]->UpdatePosition(rightSide, rightTail);
        m_bowSpray[4]->UpdatePosition(leftTail, rightTailIn);
        m_bowSpray[2]->m_particlesEnabled = 1;
        m_bowSpray[3]->m_particlesEnabled = 1;
        m_bowSpray[4]->m_particlesEnabled = 1;
        m_drip->m_particlesEnabled = 0;
        m_drip->StopDrip();
        m_jet->SetJet();
    } else {
        m_bowSpray[0]->UpdatePosition(nose, leftSide);
        m_bowSpray[1]->UpdatePosition(rightSide, nose);
        m_water->GetGridPointIntersection(&leftTailIn, 0);
        m_water->GetGridPointIntersection(&rightTail, 0);
        m_bowSpray[2]->UpdatePosition(leftSide, leftTailIn);
        m_bowSpray[3]->UpdatePosition(rightSide, rightTail);
        m_bowSpray[4]->UpdatePosition(leftTail, rightTailIn);
        m_bowSpray[0]->m_particlesEnabled = 0;
        m_bowSpray[1]->m_particlesEnabled = 0;
        m_bowSpray[2]->m_particlesEnabled = 0;
        m_bowSpray[3]->m_particlesEnabled = 0;
        m_bowSpray[4]->m_particlesEnabled = 0;
        m_drip->m_particlesEnabled = 1;
        m_drip->SetDrip();
    }

    static _D3DVECTOR forwardVectorLeft(-1.0f, 0.0f, 0.0f);
    static _D3DVECTOR forwardVectorRight(1.0f, 0.0f, 0.0f);
    static _D3DVECTOR rightVector(1.5f, 0.0f, 0.0f);
    static _D3DVECTOR leftVector(-1.5f, 0.0f, 0.0f);
    _D3DVECTOR forwardLeft = m_body->m_object->RotateVectorLocalToWorld(&forwardVectorLeft);
    _D3DVECTOR forwardRight = m_body->m_object->RotateVectorLocalToWorld(&forwardVectorRight);
    _D3DVECTOR right = m_body->m_object->RotateVectorLocalToWorld(&rightVector);
    _D3DVECTOR left = m_body->m_object->RotateVectorLocalToWorld(&leftVector);
    m_bowSpray[0]->UpdateVelocity(forwardLeft, left, m_body->m_velocity, m_body->m_speed, m_body->m_xA0.y);
    m_bowSpray[1]->UpdateVelocity(forwardRight, right, m_body->m_velocity, m_body->m_speed, -m_body->m_xA0.y);
    m_bowSpray[2]->UpdateVelocity(left, ZeroVector, m_body->m_velocity, m_body->m_speed, m_body->m_xA0.y);
    m_bowSpray[3]->UpdateVelocity(right, ZeroVector, m_body->m_velocity, m_body->m_speed, -m_body->m_xA0.y);
    m_bowSpray[2]->m_forward = m_bowSpray[0]->m_side;
    m_bowSpray[3]->m_forward = m_bowSpray[1]->m_side;
    m_bowSpray[2]->m_emitRate = 2.0f * m_bowSpray[0]->m_emitRate * sideRate;
    m_bowSpray[3]->m_emitRate = 2.0f * m_bowSpray[1]->m_emitRate * sideRate;
    if (m_bowSpray[2]->m_emitRate > m_bowSpray[3]->m_emitRate) {
        m_bowSpray[4]->m_emitRate = 0.25f * m_bowSpray[2]->m_emitRate;
        m_bowSpray[4]->m_velocityRange = m_bowSpray[2]->m_velocityRange;
    } else {
        m_bowSpray[4]->m_emitRate = 0.25f * m_bowSpray[3]->m_emitRate;
        m_bowSpray[4]->m_velocityRange = m_bowSpray[3]->m_velocityRange;
    }
    m_bowSpray[0]->m_emitRate *= 0.8f * bowRate;
    m_bowSpray[1]->m_emitRate *= 0.8f * bowRate;
    m_drip->m_x260 = 0.75f * m_body->m_velocity;
    m_drip->m_x270 = forwardLeft;
    if (m_drip->m_x260.y > 0.0f)
        m_drip->m_x260.y = 0.0f;
}


// ---- src/JetskiVehicle/Tick.cpp
#pragma fast_fptosi on
// Compiler session state: retail
// compiled this file after a two-argument runtime helper call, so mwcc
// treats helper calls as reading $a0-$a1 and the prologue copies `this`
// last. This unused function recreates that state.
static void MWCC_SHIM_TEXT MwccSessionState(long *a) { a[0] = a[0] / a[1]; }

extern JetskiGame *LpGame;
void PSXprintf(const char *fmt, ...);


// xyz dot product on VU0.
static inline float DotProduct(_D3DVECTOR *a, _D3DVECTOR *b)
{
    float r;
    asm("lqc2 vf4, 0x0(%1); lqc2 vf5, 0x0(%2); vmul.xyz vf5, vf4, vf5; vaddy.x vf5, vf5, vf5y; vaddz.x vf5, vf5, vf5z; qmfc2.ni $2, vf5; mtc1 $2, %0"
        : "=f"(r) : "r"(a), "r"(b) : "$2");
    return r;
}

// Heap shim: after
// the earlier functions Tick loads a constant late (2 words); a zeroed image
// bigger than any heap block gives its parse a clean head block, as in its
// per-function file. With the session shim above (Tick still 2 words off
// without it). Unused, image object in .mwccshim; not part of the retail file.
struct MwccHeapImageT {
    char tail[0x30000];
};
#pragma section mwccshim begin
inline void MwccHeapImageTShim() { const MwccHeapImageT image = { {0} }; }
#pragma section mwccshim end

int JetskiVehicle::Tick(float dt)
{
    _D3DVECTOR unused;          // never used; retail constructs it
    int result;                 // left unset in playback with no recorded position (retail bug)
    if (LpGame->m_paused)
        return 1;
    static int JSVehicleTickID = -1; // profiler IDs (unused)
    if (m_x1CC) {
        // Playback: put the jetski where the recording says.
        if (!(m_x290 == ZeroVector)) {
            m_body->m_object->SetPosition(0, &m_x290);
            m_body->m_x60 = m_x290;
            m_body->m_object->SetOrientation(0, &m_x2A0, &m_x2B0, 0, 1, 0);
            AnimateJetski();
            if (LpGame->m_gameOptions->m_raceType != 3)
                m_hydrodynamics->UpdateWater(dt);
            if (m_x60 > 1.0f)
                m_x60 = 1.0f;
            else if (m_x60 < -1.0f)
                m_x60 = -1.0f;
            if (m_x64 > 1.0f)
                m_x64 = 1.0f;
            else if (m_x64 < -1.0f)
                m_x64 = -1.0f;
            m_rider->AnimateRider(dt);
            m_rider->m_wrecker->PlaybackRider(dt);
            m_rider->PlaybackRider(dt);
            m_body->m_object->UpdateSearchPlacement();
            m_hydrodynamics->m_inWater = (int)m_x1C8;
            m_body->m_x30 = !m_hydrodynamics->m_inWater;
            UpdateParticles(dt);
            result = GameObject::Tick(dt);
        }
    } else {
        if (m_resetTimer > 0.0f) {
            m_resetTimer -= dt;
            if (m_resetTimer <= 0.0f) {
                ResetPosition(1);
                m_resetTimer = -(float)m_x1F4 / 1000.0f;
            } else {
                return 1;
            }
        } else if (m_resetTimer < 0.0f) {
            if (m_resetTimer == -(float)m_x1F4 / 1000.0f) {
                if (LpGame->m_competition->m_numPlayers == 1)
                    LpGame->m_competition->m_fadeInTime[LpGame->m_competition->m_cameraControllers[0]->m_viewport] = m_x1F4;
                else
                    LpGame->m_competition->m_fadeInTime[LpGame->m_competition->m_cameraControllers[m_player]->m_viewport] = m_x1F4;
            }
            m_resetTimer += dt;
            if (!(m_resetTimer < 0.0f))
                m_resetTimer = 0.0f;
            else
                return 1;
        }
        if (!LpGame->m_competition->m_x118 && LpGame->m_competition->m_cameraControllers[0]->m_x64 == m_joystick)
            m_water->ScrollWakePatch(0, m_body->m_x60);
        if (m_x584 != 3)
            ProcessInputs(dt);
        UpdateStates(dt);
        static int JSTickDID = -1;
        RunSimulation(dt);
        AnimateJetski();
        static int JSTickFID = -1;
        UpdateParticles(dt);
        TestForWreck(dt);
        switch (LpGame->m_gameOptions->m_x834) {
        case 3:
            ShakeJoystick(dt, 0);
            break;
        case 1:
            if (m_player == 0)
                ShakeJoystick(dt, 0);
            break;
        case 2:
            if (m_player == 1)
                ShakeJoystick(dt, 0);
            break;
        }
        TestForReset(dt);
        m_body->m_object->GetPosition(0, &m_x290);
        m_body->m_object->GetOrientation(0, &m_x2A0, &m_x2B0);
        m_x1C8 = m_hydrodynamics->m_inWater;
        RaceStatusStruct_Tag *status = m_ownStatus;
        if (status && !m_x530 && m_resetTimer == 0.0f && LpGame->m_competition->m_track)
            m_x1D0 = status->m_onTrack;
        _D3DVECTOR forward = m_x2A0;
        _D3DVECTOR direction;
        TrackType *track = m_raceStatus ? m_raceStatus->m_x4C : 0;
        if (track)
            track->GetTrackDirection(m_ownStatus->m_trackPosition.m_data, &direction);
        else
            direction = ZeroVector;
        m_wrongWay = DotProduct(&forward, &direction) < -0.5f && (m_ownStatus->m_flags & 2) && !(m_ownStatus->m_flags & 1);
        static int JSTickJ2ID = -1;
        result = GameObject::Tick(dt);
        if (LpGame->m_gameOptions->m_raceType == 3) {
            m_x308 = m_rider->m_stuntF;
            m_x30C = m_rider->m_x898;
            m_x310 = m_rider->m_wrecker->m_wreckType;
            m_x320 = m_body->m_velocity;
            m_x314 = m_body->m_speed;
        }
    }
    if (!(m_x784 < 0.0f)) {
        m_x784 += dt;
        if (m_x784 >= 0.5) {
            if (m_ownStatus->m_place != m_x78C) {
                switch (m_x788) {
                case 0:
                    m_rider->m_sounds->PlayPassSounds();
                    break;
                case 1:
                    m_rider->m_sounds->PlayPassedSounds();
                    break;
                }
            } else {
                PSXprintf("Double Pass, P%d from %d to %d\n", m_joystick, m_x78C, m_ownStatus->m_place);
            }
            m_x784 = -1.0f;
        }
    }
    m_roll += m_body->m_xB0.x * dt;
    if (m_roll <= -3.14f) {
        float scale = m_x7A4 ? 0.5f : 1.0f;
        if (m_ownStatus->m_numStunts > 0)
            scale *= 2.0f;
        if (!(m_ownStatus->m_flags & 0x10))
            LpGame->m_competition->m_raceStatus->BeginStunts(m_ownStatus);
        LpGame->m_competition->m_raceStatus->StuntStarted(m_ownStatus, 60.0f, 0.0f, 0.0f, scale, -1, 60.0f * scale, 12);
        m_rider->DisplayStuntText(m_ownStatus);
        LpGame->m_competition->m_raceStatus->StuntEnded(m_ownStatus, 1, 1.0f);
        m_roll = 3.14f;
        m_x7A4 = 1;
    } else if (!m_body->m_x30) {
        m_roll = 0.0f;
    }
    return result;
}


#pragma fast_fptosi reset

// ---- src/JetskiVehicle/UpdateStates.cpp
// (Its per-function file's session shim is not needed in the TU: Tick's
// helper calls leave the same state.)
// (inline_depth(5) in the per-function file; TU-wide 6 at the top)

extern JetskiGame *LpGame;

void JetskiVehicle::UpdateStates(float dt)
{
    if (!(LpGame->m_competition->m_raceStatus->m_flags & 1)) {
        m_body->SetTranslationDegreesOfFreedom(0, 1, 0);
        m_body->SetOrientationDegreesOfFreedom(1, 0, 1);
    } else {
        m_body->SetTranslationDegreesOfFreedom(1, 1, 1);
        m_body->SetOrientationDegreesOfFreedom(1, 1, 1);
        if (LpGame->m_gameOptions->m_raceType != 2) {
            if (m_x1D0) {
                if (!m_x1D4 && LpGame->m_competition->m_countDown) {
                    LpGame->m_competition->m_countDown->Hide();
                    LpGame->m_competition->m_countDown->DisableAll();
                }
            } else if (m_x1D4 && LpGame->m_competition->m_countDown) {
                LpGame->m_competition->m_countDown->EnableAll();
            }
            m_x1D4 = m_x1D0;
        }
    }
    m_body->ResetBodyCollisions();
    if (m_ownStatus->m_flags & 1)
        m_x5D8 += dt;
    if (m_invulnerable > 0.0f && m_x5D8 <= 0.0f) {
        m_invulnerable -= dt;
        if (m_invulnerable <= 0.0f)
            MakeVulnerable();
    } else if (m_x5D8 > 0.0f && m_player >= 0 && LpGame->m_competition->m_x118 > 1) {
        MakeInvulnerable();
    }
    if (m_x5CC > 0.0f)
        m_x5CC -= dt;
    if (m_x550) {
        m_x554 += dt;
        if (m_x554 > 1.3f) {
            m_detection->m_collision->m_ignoreVegetation = 0;
            m_xA4->m_ignoreVegetation = 0;
            m_highSpeed->m_ignoreVegetation = 0;
            m_collision->m_ignoreVegetation = 0;
            m_trigger->m_ignoreVegetation = 0;
            m_rider->m_wrecker->m_collisionModel->m_collision->m_ignoreVegetation = 0;
        }
    }
    if (m_player >= 0 && LpGame->m_gameOptions->m_raceType == 2 && LpGame->m_competition->m_x118 == 1) {
        if (m_ownStatus && m_ownStatus->m_x6E0 > 5.0) {
            int viewport = LpGame->m_competition->m_cameraControllers[m_player]->m_viewport;
            if (m_ownStatus->m_x6E0 > m_x620) {
                LpGame->m_competition->m_infoText->AddInfoText(LpGame->m_text->GetString(67), viewport, 10.0f, 1, 0.9f);
                m_x620 = 45.0f + m_ownStatus->m_x6E0;
            }
        } else {
            m_x620 = 0.0f;
        }
    }
    UpdateEdgeMonster(dt);
}


// ---- src/JetskiVehicle/UpdateEdgeMonster.cpp
// The edge monster: when the jetski runs off the edge of the world (and no
// terrain blocks the way), the scene's monster animation (Scene animation
// m_xD4) surfaces, swallows the jetski (state 0 -> 1), drags it under with a
// scripted camera (2), throws the rider back (3: the normal wreck path, also
// taken when no monster exists), then resets (4). Also keeps the proximity
// warning level (m_x77C, the competition's m_x254) and the monster sounds.
// VU0/rsqrt asm helpers: optimizer off.
// (inline_depth(6): TU-wide, at the top)
#pragma fast_fptosi on
// (Its per-function file's session shim is not needed in the TU.)

extern JetskiGame *LpGame;
extern "C" int rand();
void PSXprintf(const char *fmt, ...);
float FastACos(float);
float FastCos(float);
float FastSin(float);
_D3DMATRIX IdentityMatrix();
_D3DVECTOR TransformVector(_D3DVECTOR v, _D3DMATRIX m);

static inline float RSqrt(float x)
{
    float r;
    asm("rsqrt.s $f8, %0, %1; swc1 $f8, 0(%2)" : : "f"(1.0f), "f"(x), "r"(&r) : "$f8", "memory");
    return r;
}

static inline void Normalize(_D3DVECTOR *v)
{
    asm("lqc2 vf4, 0x0(%0); vmul.xyz vf5, vf4, vf4; vaddy.x vf5, vf5, vf5y; vaddz.x vf5, vf5, vf5z; vrsqrt Q, vf0w, vf5x; vsub.xyzw vf6, vf0, vf0; vaddw.xyzw vf6, vf6, vf4w; vwaitq; vmulq.xyz vf6, vf4, Q; sqc2 vf6, 0x0(%0)"
        : : "r"(v) : "memory");
}

static inline float DotProduct(const _D3DVECTOR &a, const _D3DVECTOR &b)
{
    float r;
    asm("lqc2 vf4, 0x0(%1); lqc2 vf5, 0x0(%2); vmul.xyz vf5, vf4, vf5; vaddy.x vf5, vf5, vf5y; vaddz.x vf5, vf5, vf5z; qmfc2.ni $2, vf5; mtc1 $2, %0"
        : "=f"(r) : "r"(&a), "r"(&b) : "$2");
    return r;
}


// sceVu0MulMatrix (out = a * b) taking its inputs by value.
static inline void MulMatrix(_D3DMATRIX *out, _D3DMATRIX a, _D3DMATRIX b)
{
    asm("lqc2 vf5, 0x0(%1); lqc2 vf1, 0x0(%2); lqc2 vf2, 0x10(%2); lqc2 vf3, 0x20(%2); lqc2 vf4, 0x30(%2); lqc2 vf6, 0x10(%1); vmulax.xyzw ACC, vf1, vf5x; vmadday.xyzw ACC, vf2, vf5y; vmaddaz.xyzw ACC, vf3, vf5z; vmaddw.xyzw vf9, vf4, vf5w; lqc2 vf7, 0x20(%1); vmulax.xyzw ACC, vf1, vf6x; vmadday.xyzw ACC, vf2, vf6y; vmaddaz.xyzw ACC, vf3, vf6z; vmaddw.xyzw vf10, vf4, vf6w; lqc2 vf8, 0x30(%1); vmulax.xyzw ACC, vf1, vf7x; vmadday.xyzw ACC, vf2, vf7y; vmaddaz.xyzw ACC, vf3, vf7z; vmaddw.xyzw vf11, vf4, vf7w; vmulax.xyzw ACC, vf1, vf8x; vmadday.xyzw ACC, vf2, vf8y; vmaddaz.xyzw ACC, vf3, vf8z; vmaddw.xyzw vf12, vf4, vf8w; sqc2 vf9, 0x0(%0); sqc2 vf10, 0x10(%0); sqc2 vf11, 0x20(%0); sqc2 vf12, 0x30(%0)"
        : : "r"(out), "r"(&b), "r"(&a) : "memory");
}

static inline void SetTranslation(_D3DMATRIX *m, float x, float y, float z)
{
    m->_41 = x;
    m->_42 = y;
    m->_43 = z;
}

static inline float Length(_D3DVECTOR &v)
{
    float sq = v.x * v.x + v.y * v.y + v.z * v.z;
    float length;
    if (sq == 0.0f)
        length = 0.0f;
    else
        length = 1.0f / RSqrt(sq);
    return length;
}

static inline void WaterSurface(PSXWater *water, _D3DVECTOR *point)
{
    if (water)
        water->GetGridPointIntersection(point, 0);
}

static inline void TerrainSurface(Terrain *terrain, _D3DVECTOR *point)
{
    if (terrain)
        terrain->GetGridPointIntersection(point, 0, 0, 0);
}

void JetskiVehicle::UpdateEdgeMonster(float dt)
{
    int index;
    JetskiCompetition *competition = LpGame->m_competition;
    index = competition->m_xD4;
    if (index == -2)
        return;
    float distance = 1000.0f;           // before the vectors: retail's store order
    _D3DVECTOR ground, ahead;
    _D3DMATRIX rotation __attribute__((aligned(16)));
    _D3DMATRIX offset __attribute__((aligned(16)));
    _D3DMATRIX world __attribute__((aligned(16)));
    _D3DMATRIX local __attribute__((aligned(16)));
    int blocked = (unsigned char)(m_collisionModel->m_x6C || (!m_x48C && m_collisionModel->m_x70));
    if (m_x544 == -1 && !m_x530 && CheckForEdgeOfWorld(&distance)) {
        if (!blocked) {
            ground = m_body->m_x60;
            WaterSurface(m_water, &ground);
            float water = ground.y;
            TerrainSurface(m_terrain, &ground);
            if (ground.y > water)
                blocked = 1;
        }
        if (!blocked) {
            ahead = m_body->m_x60 + 2.0f * m_body->m_velocity;
            WaterSurface(m_water, &ahead);
            float water = ahead.y;
            TerrainSurface(m_terrain, &ahead);
            if (ahead.y > water)
                blocked = 1;
        }
        if (index < 0 || blocked) {
            m_x790 = 1;
            m_x548 = 0;
            if (LpGame->m_gameOptions->m_raceType == 5) {
                m_x558 = LpGame->m_competition->m_cameraControllers[0]->m_followCamera->m_settings[2];
                m_x55C = LpGame->m_competition->m_cameraControllers[0]->m_followCamera->m_settings[1];
                m_cameraPreset = LpGame->m_competition->m_cameraControllers[0]->m_followCamera->m_curPreset;
                m_x564 = LpGame->m_competition->m_cameraControllers[0]->m_followCamera->m_settings[0];
            } else {
                m_x558 = LpGame->m_competition->m_cameraControllers[m_player]->m_followCamera->m_settings[2];
                m_x55C = LpGame->m_competition->m_cameraControllers[m_player]->m_followCamera->m_settings[1];
                m_cameraPreset = LpGame->m_competition->m_cameraControllers[m_player]->m_followCamera->m_curPreset;
                m_x564 = LpGame->m_competition->m_cameraControllers[m_player]->m_followCamera->m_settings[0];
            }
            m_x544 = 3;
            if (m_rider->m_sounds)
                m_rider->m_sounds->PlayWreckSounds(&m_x4C0, 1, 1, 1);
        } else {
            m_x544 = 0;
        }
    }
    if (m_player >= 0 && !blocked) {
        if (distance <= 50.0f) {
            if (!m_x76C) {
                m_x76C = 1;
                if (m_edgeMonsterSound)
                    m_edgeMonsterSound->PlayEdgeMonsterSounds(0, 100.0f);
            }
            m_x77C = (int)(100.0f * (distance / 50.0f));
        } else if (distance > 65.0) {
            if (m_x76C && m_x544 < 0) {
                m_x76C = 0;
                if (m_edgeMonsterSound && m_edgeMonsterSound->IsPlaying())
                    m_edgeMonsterSound->Stop();
            }
            m_x77C = 100;
            int i;
            int closer = 0;
            for (i = 0; i < competition->m_x118; i++) {
                if (competition->m_vehicles[i + 5]->m_x77C != 100)
                    closer = 1;
            }
            if (!closer)
                competition->m_x254 = 100;
        }
    }
    if (m_player >= 0 && m_x544 >= 0)
        m_x77C = 0;
    competition->m_x254 = m_x77C > competition->m_x254 ? competition->m_x254 : m_x77C;
    if (m_x544 <= -1)
        return;

    _D3DVECTOR heading;
    heading = m_body->m_velocity;
    heading.y = 0.0f;
    if (0.0f == Length(heading))
        heading.z = 1.0f;
    Normalize(&heading);
    float angle;
    if (heading.x > 0.0f)
        angle = m_edgeAngle + FastACos(DotProduct(_D3DVECTOR(0.0f, 0.0f, 1.0f), heading));
    else
        angle = m_edgeAngle - FastACos(DotProduct(_D3DVECTOR(0.0f, 0.0f, 1.0f), heading));
    float c = FastCos(angle);
    float s = FastSin(angle);
    RotationMatrix(&rotation, 0.0f, 1.0f, 0.0f, c, s, angle);
    offset = IdentityMatrix();
    offset._41 = 0.0f;
    offset._42 = 0.0f;
    offset._43 = 41.434f;
    MulMatrix(&local, rotation, offset);
    world = IdentityMatrix();
    SetTranslation(&world, m_renderable->GetPosition((SoultreeObject *)0).x, m_renderable->GetPosition((SoultreeObject *)0).y,
                   m_renderable->GetPosition((SoultreeObject *)0).z);
    MulMatrix(&world, world, local);
    if (m_rider->m_wrecker->m_springs->m_x6C && m_edgeMonsterSound && m_edgeMonsterSound->IsPlaying() &&
        m_edgeMonsterSound->m_currentSound == 3)
        m_edgeMonsterSound->Stop();

    switch (m_x544) {
    case 0:
        if (!m_data->m_animations->m_animations[index].m_automaticSequence) {
            if (index >= 0 && m_data->m_animations->m_count > 0)
                m_data->m_animations->m_animations[index].m_character->EnableAll();
            m_data->m_animations->m_animations[index].m_automaticSequence = 1;
            float random = 4.656613e-10f * rand();     // named: 0.25 loads before fptodp
            m_edgeAngle = 3.14159274f * (0.25 * random - 0.125);
            m_x544 = 1;
        }
        break;
    case 1: {
        m_data->m_animations->m_animations[index].m_character->m_root->SetMatrix(world);
        _D3DVECTOR up;
        up = TransformVector(up, local);
        m_water->ApplyExternalForce(5.0f, up);
        float remaining = m_data->m_animations->m_animations[index].m_character->m_motion->m_duration -
                          m_data->m_animations->m_animations[index].m_time;
        if (m_data->m_animations->m_animations[index].m_sequence == 1 && remaining < 0.001f) {
            m_data->m_animations->m_animations[index].m_automaticSequence = 0;
            m_x790 = 1;
            m_gravity->SetGravity(322.0f);
            if (LpGame->m_gameOptions->m_raceType == 5) {
                m_x558 = LpGame->m_competition->m_cameraControllers[0]->m_followCamera->m_settings[2];
                m_x55C = LpGame->m_competition->m_cameraControllers[0]->m_followCamera->m_settings[1];
                m_cameraPreset = LpGame->m_competition->m_cameraControllers[0]->m_followCamera->m_curPreset;
                m_x564 = LpGame->m_competition->m_cameraControllers[0]->m_followCamera->m_settings[0];
            } else {
                m_x558 = LpGame->m_competition->m_cameraControllers[m_player]->m_followCamera->m_settings[2];
                m_x55C = LpGame->m_competition->m_cameraControllers[m_player]->m_followCamera->m_settings[1];
                m_cameraPreset = LpGame->m_competition->m_cameraControllers[m_player]->m_followCamera->m_curPreset;
                m_x564 = LpGame->m_competition->m_cameraControllers[m_player]->m_followCamera->m_settings[0];
            }
            if (LpGame->m_gameOptions->m_raceType == 5) {
                LpGame->m_competition->m_cameraControllers[0]->m_followCamera->ChangeToPreset(0);
                LpGame->m_competition->m_cameraControllers[0]->m_followCamera->m_settings[1] = 3.14159274f;
            } else {
                LpGame->m_competition->m_cameraControllers[m_player]->m_followCamera->ChangeToPreset(0);
                LpGame->m_competition->m_cameraControllers[m_player]->m_followCamera->m_settings[1] = 3.14159274f;
            }
            m_x548 = 0;
            m_x544 = 2;
            if (m_edgeMonsterSound)
                m_edgeMonsterSound->PlayEdgeMonsterSounds(1, 100.0f);
        }
        break;
    }
    case 2: {
        if (index >= 0 && index < m_data->m_animations->m_count)
            m_data->m_animations->m_animations[index].m_character->m_root->SetMatrix(world);
        _D3DVECTOR position;
        m_renderable->GetPosition(&position);
        float depth = position.y;
        m_water->GetGridPointIntersection(&position, 0);
        depth = position.y - depth;
        if (LpGame->m_gameOptions->m_raceType == 5)
            LpGame->m_competition->m_cameraControllers[0]->m_followCamera->m_x1A0.y = 3.0f + depth;
        else
            LpGame->m_competition->m_cameraControllers[m_player]->m_followCamera->m_x1A0.y = 3.0f + depth;
        if (depth > 5.0f) {
            m_renderable->Hide();
            m_rider->m_root->Hide();
            m_data->m_animations->m_animations[index].m_character->m_root->Hide();
        }
        m_x548 += dt;
        if (m_x548 <= 2.0f) {
            if (LpGame->m_gameOptions->m_raceType == 5) {
                LpGame->m_competition->m_cameraControllers[0]->m_followCamera->ChangeToPreset(0);
                LpGame->m_competition->m_cameraControllers[0]->m_followCamera->m_settings[2] = -1.06814158f;
                LpGame->m_competition->m_cameraControllers[0]->m_followCamera->m_settings[1] = 3.14159274f * (m_x548 / 2.0f);
                LpGame->m_competition->m_cameraControllers[0]->m_followCamera->m_settings[0] = 20.0f;
            } else {
                LpGame->m_competition->m_cameraControllers[m_player]->m_followCamera->ChangeToPreset(0);
                LpGame->m_competition->m_cameraControllers[m_player]->m_followCamera->m_settings[2] = -1.06814158f;
                LpGame->m_competition->m_cameraControllers[m_player]->m_followCamera->m_settings[1] = 3.14159274f * (m_x548 / 2.0f);
                LpGame->m_competition->m_cameraControllers[m_player]->m_followCamera->m_settings[0] = 20.0f;
            }
        } else {
            if (LpGame->m_gameOptions->m_raceType == 5)
                LpGame->m_competition->m_cameraControllers[0]->m_followCamera->m_settings[0] = 20.0f + 5.0f * ((m_x548 - 2.0f) / 1.5f);
            else
                LpGame->m_competition->m_cameraControllers[m_player]->m_followCamera->m_settings[0] = 20.0f + 5.0f * ((m_x548 - 2.0f) / 1.5f);
        }
        if (m_x548 > 3.5f || m_x1E4 > 0.0f) {
            m_renderable->Unhide();
            m_rider->m_root->Unhide();
            m_data->m_animations->m_animations[index].m_character->m_root->Unhide();
            m_gravity->SetGravity(32.2f);
            m_data->m_animations->m_animations[index].m_automaticSequence = 1;
            m_x548 = 0;
            m_x544 = 3;
            m_x580 = 0;
            if (LpGame->m_gameOptions->m_raceType == 5) {
                LpGame->m_competition->m_cameraControllers[0]->m_followCamera->m_x1A0.y = 0.0f;
                LpGame->m_competition->m_cameraControllers[0]->m_followCamera->ChangeToPreset(m_cameraPreset);
                LpGame->m_competition->m_cameraControllers[0]->m_followCamera->m_settings[2] = m_x558;
                LpGame->m_competition->m_cameraControllers[0]->m_followCamera->m_settings[1] = m_x55C;
                LpGame->m_competition->m_cameraControllers[0]->m_followCamera->m_settings[0] = m_x564;
            } else {
                LpGame->m_competition->m_cameraControllers[m_player]->m_followCamera->m_x1A0.y = 0.0f;
                LpGame->m_competition->m_cameraControllers[m_player]->m_followCamera->ChangeToPreset(m_cameraPreset);
                LpGame->m_competition->m_cameraControllers[m_player]->m_followCamera->m_settings[2] = m_x558;
                LpGame->m_competition->m_cameraControllers[m_player]->m_followCamera->m_settings[1] = m_x55C;
                LpGame->m_competition->m_cameraControllers[m_player]->m_followCamera->m_settings[0] = m_x564;
            }
            if (m_edgeMonsterSound)
                m_edgeMonsterSound->PlayEdgeMonsterSounds(2, 100.0f);
            if (m_rider->m_sounds)
                m_rider->m_sounds->PlayWreckSounds(&m_x4C0, 1, 0, 1);
        }
        break;
    }
    case 3:
        if (m_player >= 0 && !m_x770 && m_x780 >= 0.0f && m_rider->m_wrecker->m_body->m_velocity.y < 0.0f) {
            m_x770 = 1;
            PSXprintf("Falling\n");
            if (m_edgeMonsterSound)
                m_edgeMonsterSound->PlayEdgeMonsterSounds(3, 100.0f);
        }
        if (m_x770 && m_x780 < 0.0f && m_rider->m_wrecker->m_body->m_velocity.y >= 0.0f) {
            if (m_edgeMonsterSound)
                m_edgeMonsterSound->Stop();
        }
        if (!m_x530) {
            Cannonball();
        } else {
            _D3DVECTOR water;
            ground = m_body->m_x60;
            if (m_water)
                m_water->GetGridPointIntersection(&ground, 0);
            if (m_body->m_x60.y <= ground.y && m_body->m_velocity.y < 0.0f) {
                m_gravity->SetGravity(32.2f);
                m_x580 = 1;
            }
            water = m_rider->m_wrecker->m_body->m_x60;
            ground = m_rider->m_wrecker->m_body->m_x60;
            if (m_terrain)
                m_terrain->GetGridPointIntersection(&ground, 0, 0, 0);
            if (m_water)
                m_water->GetGridPointIntersection(&water, 0);
            if (water.y > ground.y)
                ground.y = water.y;
            if (m_rider->m_wrecker->m_body->m_x60.y <= ground.y && m_rider->m_wrecker->m_body->m_velocity.y < 0.0f) {
                m_x544 = 4;
                m_rider->m_wrecker->m_x160 = 1;
                m_gravity->SetGravity(32.2f);
                m_x580 = 1;
            }
        }
        break;
    case 4:
        if (m_edgeMonsterSound && m_edgeMonsterSound->IsPlaying() && m_edgeMonsterSound->m_currentSound == 3)
            m_edgeMonsterSound->Stop();
        if (!m_x550) {
            m_x770 = 0;
            m_x76C = 0;
            m_x774 = 0;
            m_x778 = 0;
            m_x790 = 0;
            m_x544 = -1;
            if (index >= 0 && index < m_data->m_animations->m_count)
                m_data->m_animations->m_animations[index].m_character->DisableAll();
            if (LpGame->m_competition->m_music && !LpGame->m_competition->m_music->m_playing)
                LpGame->m_competition->m_music->Shuffle(1);
        }
        break;
    }
    m_x780 = m_rider->m_wrecker->m_body->m_velocity.y;
}


#pragma fast_fptosi reset

// ---- src/JetskiVehicle/TestForImpact.cpp
#pragma fast_fptosi on

extern JetskiGame *LpGame;



#define PLAY(e)                                                                         \
    (e)->Start();                                                                       \
    LpGame->m_auralScape->AttenuateSound((e)->GetActiveSound(), &m_body->m_x60);

// Hull impact sounds: a slap when the hull lands, a crash response (by what
// was hit) when the jetski collides with something.
void JetskiVehicle::TestForImpact(float dt)
{
    static unsigned int WORLDOBJECT_MATERIAL_ID1 = LpCollisionTypeRegistry->GetType("MATERIALID1");
    static unsigned int WORLDOBJECT_MATERIAL_ID2 = LpCollisionTypeRegistry->GetType("MATERIALID2");
    static unsigned int WORLDOBJECT_MATERIAL_ID3 = LpCollisionTypeRegistry->GetType("MATERIALID3");
    static unsigned int WORLDOBJECT_MATERIAL_ID4 = LpCollisionTypeRegistry->GetType("MATERIALID4");
    static unsigned int WORLDOBJECT_MATERIAL_ID5 = LpCollisionTypeRegistry->GetType("MATERIALID5");
    static unsigned int WORLDOBJECT_MATERIAL_ID6 = LpCollisionTypeRegistry->GetType("MATERIALID6");
    m_x760 = Fmax(0.0f, m_x760 - dt);
    m_x764 = Fmax(0.0f, m_x764 - dt);
    if (m_collisionModel->m_x6C) {
        if (m_x758 && m_x764 == 0.0f) {
            if (m_x748 && m_body->m_velocity.y < -2.0f) {
                PLAY(m_x748)
                PLAY(m_x754)
                m_x764 = 2.0f;
            } else if (m_x750) {
                PLAY(m_x750)
                m_x764 = 2.0f;
            }
        }
        m_x758 = 0;
    } else {
        m_x758 = 1;
        if (m_detection->m_collision->m_collided) {
            if (m_responses && m_x75C && m_collisionModel->m_hitObject && m_x760 == 0.0f) {
                unsigned int type = m_collisionModel->m_hitObject->m_ownerType;
                if (type & WORLDOBJECT_MATERIAL_ID2) {
                    m_responses->TriggerResponse(2, 1, 0);
                    LpGame->m_auralScape->AttenuateSound(m_x754->GetActiveSound(), &m_body->m_x60);
                } else if (type & WORLDOBJECT_MATERIAL_ID3) {
                    m_responses->TriggerResponse(3, 1, 0);
                    PLAY(m_x754)
                } else if (type & WORLDOBJECT_MATERIAL_ID4) {
                    m_responses->TriggerResponse(4, 1, 0);
                    PLAY(m_x754)
                } else if (type & WORLDOBJECT_MATERIAL_ID5) {
                    m_responses->TriggerResponse(5, 1, 0);
                    PLAY(m_x754)
                } else if (type & WORLDOBJECT_MATERIAL_ID6) {
                    m_responses->TriggerResponse(6, 1, 0);
                    PLAY(m_x754)
                } else if (type & WORLDOBJECT_SOLIDFOLIAGE) {
                    m_responses->TriggerResponse(7, 1, 0);
                    PLAY(m_x754)
                } else if (type & WORLDOBJECT_SOFTFOLIAGE) {
                    m_responses->TriggerResponse(8, 1, 0);
                    PLAY(m_x754)
                } else if (type & (WORLDOBJECT_PHYSICSBODY | COLLISION_BIKE)) {
                    m_responses->TriggerResponse(9, 1, 0);
                    PLAY(m_x754)
                } else if (!(type & (WORLDOBJECT_PINKBALLOON | WORLDOBJECT_BLUEBALLOON | WORLDOBJECT_GREENBALLOON))) {
                    m_responses->TriggerResponse(1, 1, 0);
                    PLAY(m_x754)
                }
                int volume = Fmin(100.0f * (m_body->m_speed / 15.0f), 100.0f);
                m_responses->SetBaseVolume(volume);
                m_x754->SetBaseVolume(volume);
                LpGame->m_auralScape->AttenuateSound(((SoundEmitter *)m_responses)->GetActiveSound(), &m_body->m_x60);
                m_x75C = 0;
                m_x760 = 2.0f;
            }
        } else {
            m_x75C = 1;
        }
    }
}


#pragma fast_fptosi reset

// ---- src/JetskiVehicle/TestForWreck.cpp

extern JetskiGame *LpGame;

void JetskiVehicle::TestForWreck(float dt)
{
    static float RiderHeadHeight = 3.0f;
    int hit = 0;
    int hitBike = 0;
    if (m_x544 >= 0)
        return;
    m_x4C0 = m_body->m_object->RotateVectorWorldToLocal(&m_x4E0);
    if (m_x530)
        return;
    JetskiCollisionModel *model = m_collisionModel;
    if (model->m_x3C) {
        // Retail tests the first contact every time.
        ObjectCollisionPoint *contact = model->m_contacts;
        for (int i = 0; i < model->m_numContacts; i++) {
            if (!(contact->m_normal.y <= RiderHeadHeight)) {
                if (contact->m_collision->m_ownerType & COLLISION_BIKE) {
                    if (!((JetskiVehicle *)contact->m_collision->m_owner)->m_hydrodynamics->m_inWater) {
                        hit = 1;
                        hitBike = hit;
                    }
                } else if (!(contact->m_collision->m_ownerType & WORLDOBJECT_BUOY)) {
                    hit = 1;
                }
            }
        }
    }
    if (m_x4C0.z > m_x510.z) {
        m_x540 = 0;
        TriggerWreck((WreckType)3);
    } else if (m_x4C0.z < -m_x510.z) {
        m_x540 = 0;
        TriggerWreck((WreckType)0);
    } else if (m_x4C0.x > m_x510.x) {
        m_x540 = 0;
        TriggerWreck((WreckType)2);
    } else if (m_x4C0.x < -m_x510.x) {
        m_x540 = 0;
        TriggerWreck((WreckType)1);
    } else if (m_x4C0.y > m_x510.y) {
        m_x540 = 0;
        TriggerWreck((WreckType)4);
    } else if (m_x4C0.z < -m_x520.z) {
        m_rider->ActivateWobble(28);
    } else if (m_x4C0.y > m_x520.y) {
        m_rider->ActivateWobble(29);
    } else if (m_x4C0.x > m_x520.x) {
        m_rider->ActivateWobble(30);
    } else if (m_x4C0.x < -m_x520.x) {
        m_rider->ActivateWobble(31);
    } else if (hit) {
        if (m_player < 0 || !hitBike || !LpGame->m_cheats->noJoust) {
            if (m_body->m_lastVelocity.z > 0.0f) {
                m_x540 = 1;
                TriggerWreck((WreckType)3);
            } else {
                m_x540 = 1;
                TriggerWreck((WreckType)0);
            }
        }
    } else if (m_rider->m_x484 && m_rider->m_x494 < 28 && m_x2B0.y < 0.0f) {
        m_x540 = 2;
        TriggerWreck((WreckType)4);
    } else if (m_rider->m_x484 && !m_body->m_x30 && m_rider->m_x494 < 28 &&
               m_rider->m_frame < m_rider->m_stunt4C8 && m_rider->m_stunt7FC) {
        m_x540 = 3;
        TriggerWreck((WreckType)4);
    } else if (m_rider->m_x484 && !m_body->m_x30 && m_rider->m_x494 < 28 &&
               !(m_rider->m_frame < m_rider->m_stunt4C8) && m_rider->m_frame <= m_rider->m_stunt4CC &&
               m_rider->m_stunt7FC) {
        m_x540 = 4;
        TriggerWreck((WreckType)4);
    } else if (m_x5B4 && !m_body->m_x30 && m_x2B0.y < 0.0f) {
        m_x540 = 5;
        TriggerWreck((WreckType)4);
    }
}


// ---- src/JetskiVehicle/Cannonball.cpp
// (inline_depth(6): TU-wide, at the top)


extern "C" int rand();

extern JetskiGame *LpGame;


static inline float Sqrt(float x)
{
    float r;
    asm("sqrt.s $f8, %1; swc1 $f8, 0(%0)" : : "r"(&r), "f"(x) : "$f8", "memory");
    return r;
}

static inline void Normalize(_D3DVECTOR &v)
{
    float len2 = v.x * v.x + v.y * v.y + v.z * v.z;
    if (len2 == 0.0f) {
        v = ZeroVector;
    } else {
        float r = RSqrt(len2);
        v.x *= r;
        v.y *= r;
        v.z *= r;
    }
}

static inline float Magnitude(float x)
{
    return Sqrt(x);
}

static inline float RandomUnit() { return 4.656613e-10f * rand(); }

// Fire the jetski (and its rider) on a ballistic arc towards the middle of
// the course.
void JetskiVehicle::Cannonball()
{
    float height;
    float g;
    float time;
    float speed;
    float originX;
    float originZ;
    float rawMinZ;    // a named local: its register comes after originZ's (see CheckForEdgeOfWorld)
    float minX;
    float maxX;
    float minZ;
    float maxZ;
    minX = 20.0f + m_data->m_manager->m_negXEdgeDist;
    originX = m_data->m_manager->m_posXEdgeDist;
    maxX = originX + 256.0f * m_data->m_manager->m_terrainWidth * m_data->m_manager->m_terrainWidthScale - 20.0f;
    originZ = m_data->m_manager->m_posZEdgeDist;
    rawMinZ = m_data->m_manager->m_negZEdgeDist;
    minZ = 20.0f + rawMinZ;
    maxZ = originZ + 256.0f * m_data->m_manager->m_terrainBreadth * m_data->m_manager->m_terrainBreadthScale - 20.0f;
    _D3DVECTOR pos = m_body->m_x60;
    _D3DVECTOR dir = _D3DVECTOR(minX + 0.5f * (maxX - minX), pos.y, minZ + 0.5f * (maxZ - minZ)) - pos;
    Normalize(dir);
    switch (LpGame->m_gameOptions->m_x2C + 1) {
    case 5:
    case 10:
    case 17:
        height = 92.0f;
        break;
    default:
        height = 115.0f;
        break;
    }
    g = 2.0f;
    time = g * Magnitude(g * height / 32.2f);
    speed = 810.0f / time;
    m_body->m_velocity = speed * dir + _D3DVECTOR(0.0f, 16.1f * time + height / time, 0.0f);
    m_body->m_x27C = 125000.0f;
    m_rider->m_wrecker->m_body->m_x27C = 125000.0f;
    m_body->m_xB0 = _D3DVECTOR(0.7f * RandomUnit(), 0.6f * RandomUnit(), 0.03f) + _D3DVECTOR(1.4f, 1.2f, 0.06f);
    m_detection->m_collision->m_ignoreVegetation = 1;
    m_xA4->m_ignoreVegetation = 1;
    m_highSpeed->m_ignoreVegetation = 1;
    m_collision->m_ignoreVegetation = 1;
    m_trigger->m_ignoreVegetation = 1;
    m_rider->m_wrecker->m_collisionModel->m_collision->m_ignoreVegetation = 1;
    m_x554 = 0;
    m_x538 = 3.0f;
    m_body->m_object->SetPosition(&pos);
    m_x540 = 6;
    TriggerWreck((WreckType)5);
    m_x550 = 1;
    m_rider->m_wrecker->m_x160 = 0;
    if (m_player > -1) {
        JoystickDevice *joystick = LpGame->m_controls->m_joysticks[m_player];
        if (joystick) {
            switch (LpGame->m_gameOptions->m_x834) {
            case 3:
                joystick->PlayEffect(0, 1.7f, 0);
                break;
            case 1:
                if (m_player == 0)
                    LpGame->m_controls->m_joysticks[0]->PlayEffect(0, 1.7f, 0);
                break;
            case 2:
                if (m_player == 1)
                    LpGame->m_controls->m_joysticks[1]->PlayEffect(0, 1.7f, 0);
                break;
            }
        }
    }
    m_x1DC = 1;
}


// ---- src/JetskiVehicle/TestForReset.cpp

extern JetskiGame *LpGame;

// Fade out and reset the player's jetski (the AI resets at once).
#define REQUEST_RESET()                                                                         \
    m_rider->StopSpecialAnimation();                                                            \
    m_rider->m_x8A4 = 1;                                                                        \
    if (m_player >= 0 && LpGame->m_competition) {                                               \
        if (m_resetTimer == 0.0f) {                                                             \
            m_resetTimer = 0.2f;                                                                \
            m_x1F4 = 100;                                                                       \
            if (LpGame->m_competition->m_numPlayers == 1)                                       \
                LpGame->m_competition->m_fadeOutTime[LpGame->m_competition->m_cameraControllers[0]->m_viewport] = 180; \
            else                                                                                \
                LpGame->m_competition->m_fadeOutTime[LpGame->m_competition->m_cameraControllers[m_player]->m_viewport] = 180; \
        }                                                                                       \
    } else {                                                                                    \
        m_resetTimer = 0.0f;                                                                    \
        ResetPosition(1);                                                                       \
    }

void JetskiVehicle::TestForReset(float dt)
{
    if (m_x530) {
        if (m_x530 == 1) {
            m_x534 += dt;
            if (m_x534 > 1.0f) {
                m_x530 = 2;
                m_x534 = 0.0f;
            }
        } else {
            if (m_x534 != 0.0f)
                m_x534 += dt;
            else if (m_rider->m_wrecker->m_x124)
                m_x534 += dt;
            if (m_x534 > m_x538)
                StopWreck(1);
        }
        m_x5D0 = 0;
        return;
    }
    if (m_x544 < 0) {
        if (m_collisionModel->m_x6C || (!m_x48C && m_collisionModel->m_x70)) {
            if (m_x1E4 < 0.0f)
                m_x1E4 = m_x1E8;
            m_x1E4 += dt;
            if (!(m_x1E4 < 3.0f)) {
                m_x1E4 = 0.0f;
                m_x1E8 = 0.0f;
                REQUEST_RESET()
            }
        } else if (m_x1E4 > 0.0f) {
            m_x1E8 = m_x1E4;
            m_x1E4 = -0.5f;
        } else if (m_x1E4 < 0.0f) {
            m_x1E4 += dt;
            if (!(m_x1E4 < 0.0f)) {
                m_x1E4 = 0.0f;
                m_x1E8 = 0.0f;
            }
        }
        if (LpGame->m_gameOptions->m_raceType != 2 && m_ownStatus && m_ownStatus->m_x6E0 > 3.0f) {
            REQUEST_RESET()
        }
    }
}


// ---- src/JetskiVehicle/InitializeStatus.cpp

extern JetskiGame *LpGame;

void JetskiVehicle::InitializeStatus()
{
    if (m_x530 || m_rider->m_x484)
        StopWreck(1);
    m_x550 = 0;
    m_x554 = 0;
    if (m_player < 0)
        m_x538 = 2.0f;
    else if (LpGame->m_gameOptions->m_raceType == 8)
        m_x538 = 2.0f;
    else
        m_x538 = 1.5f;
    m_body->m_x30 = 0;
    m_x5CC = 2.0f;
    m_x5D8 = 0;
    m_x48C = 1;
    m_airborne = 1;
    m_x790 = 0;
    m_x5D0 = 0;
    m_xB4 = 0;
    if (m_x1E0)
        m_x1DC = 1;
    else
        m_x1DC = 0;
    m_rider->ResetVariables();
    m_rider->m_wrecker->m_x160 = 1;
    m_gravity->SetGravity(32.2f);
    m_x580 = 1;
    m_x770 = 0;
    m_x76C = 0;
    m_x544 = -1;
    m_x548 = 0;
    m_x558 = 0.0f;
    m_x55C = 3.14159274f;
    m_cameraPreset = 0;
    m_x564 = 10.0f;
    if (LpGame->m_competition) {
        int index = LpGame->m_competition->m_xD4;
        if (index >= 0 && index < m_data->m_animations->m_count)
            m_data->m_animations->m_animations[index].m_character->DisableAll();
        if (m_player >= 0) {
            if (LpGame->m_gameOptions->m_raceType == 5 || LpGame->m_gameOptions->m_raceType == 8) {
                LpGame->m_competition->m_cameraControllers[0]->m_followCamera->m_resetFilters = 1;
                LpGame->m_competition->m_infoText->FlushQueue(0);
            } else {
                LpGame->m_competition->m_cameraControllers[m_player]->m_followCamera->m_resetFilters = 1;
                LpGame->m_competition->m_infoText->FlushQueue(m_player);
            }
        }
        if (m_joystick < LpGame->m_competition->m_x118) {
            JoystickDevice *joystick = LpGame->m_controls->m_joysticks[m_joystick];
            if (joystick)
                joystick->StopEffect(2);
        }
    }
    m_roll = 0.0f;
}


// ---- src/JetskiVehicle/TriggerWreck.cpp

void PSXprintf(const char *fmt, ...);

extern JetskiGame *LpGame;

void JetskiVehicle::TriggerWreck(WreckType type)
{
    if (m_x548 <= 0.0f) {
        m_x530 = 1;
        if (LpGame->m_gameOptions->m_raceType == 5)
            m_ownStatus->m_timeRemaining = 0.0f;
        if (LpGame->m_gameOptions->m_raceType != 8) {
            // Wrecking costs part of the banked stunt score.
            switch (LpGame->m_gameOptions->m_difficulty) {
            case 1:
                m_ownStatus->m_x6F4 *= 0.75f;
                break;
            case 2:
                m_ownStatus->m_x6F4 *= 0.5f;
                break;
            case 3:
                m_ownStatus->m_x6F4 = 0.0f;
                break;
            }
        }
        m_x790 = 1;
        m_rider->m_wobbling = 0;
        m_x534 = 0;
        if (type == 5)
            m_rider->m_wrecker->TriggerWreck();
        else
            m_rider->m_wrecker->TriggerWreck(type, 0);
        if (m_rider->m_sounds) {
            switch (m_x540) {
            case 0:
            case 1:
            case 5:
                m_rider->m_sounds->PlayWreckSounds(&m_x4C0, 1, 0, 1);
                break;
            case 3:
            case 4:
                m_rider->m_sounds->PlayWreckSounds(&m_x4C0, 1, 1, 1);
                if (LpGame->m_competition->m_crowd)
                    LpGame->m_competition->m_crowd->PlayOhhSound();
                break;
            case 2:
            case 6:
                PSXprintf("\n");
                break;
            }
        }
        if (m_player >= 0 && m_ownStatus->m_place == 1 && LpGame->m_competition->m_crowd)
            LpGame->m_competition->m_crowd->PlayBooSound();
        if ((m_ownStatus->m_flags & 0x20) && !m_x1CC)
            m_raceStatus->StuntEnded(m_ownStatus, 0, 0.0f);
        switch (LpGame->m_gameOptions->m_x834) {
        case 3:
            ShakeJoystick(0.0f, 1);
            break;
        case 1:
            if (m_player == 0)
                ShakeJoystick(0.0f, 1);
            break;
        case 2:
            if (m_player == 1)
                ShakeJoystick(0.0f, 1);
            break;
        }
    }
}


// ---- src/JetskiVehicle/StopWreck.cpp

extern JetskiGame *LpGame;

void JetskiVehicle::StopWreck(int)
{
    if (m_rider->m_wrecker) {
        m_rider->StopSpecialAnimation();
        m_rider->m_wrecker->StopWreck();
        m_x530 = 0;
        m_x550 = 0;
        m_rider->m_wrecker->m_x160 = 1;
        m_x554 = 0;
        JetskiCompetition *competition = LpGame->m_competition;
        if (competition && ((competition->m_raceStatus->m_flags & 2) || LpGame->m_gameOptions->m_raceType == 5)) {
            m_resetTimer = 0.0f;
            ResetPosition(1);
        } else if (m_player >= 0 && competition) {
            if (m_resetTimer == 0.0f) {
                m_resetTimer = 0.2f;
                m_x1F4 = 100;
                if (LpGame->m_competition->m_numPlayers == 1)
                    LpGame->m_competition->m_fadeOutTime[LpGame->m_competition->m_cameraControllers[0]->m_viewport] = 180;
                else
                    LpGame->m_competition->m_fadeOutTime[LpGame->m_competition->m_cameraControllers[m_player]->m_viewport] = 180;
            }
        } else {
            m_resetTimer = 0.0f;
            ResetPosition(1);
        }
        m_body->m_x27C = 22500.0f;
        m_rider->m_wrecker->m_body->m_x27C = 22500.0f;
    }
}


// ---- src/JetskiVehicle/ShakeJoystick.cpp

extern JetskiGame *LpGame;

#define JOYSTICK (LpGame->m_controls->m_joysticks[m_joystick])

// Rumble the player's controller: hard on a crash, a pulse while wrecked,
// otherwise by how far the hull is tilted.
// Heap shim: ShakeJoystick's per-function flags (0x4DC5, 0x84FD) in a 0x30000
// image, so the function parses in a new head block at its per-function
// offsets (22 words off with the zeroed image, 84 without any).
struct MwccHeapImageS {
    char pad0[0x4DC5];
    char flag0;
    char pad1[0x3737];
    char flag1;
    char tail[0x27B02];
};
#pragma section mwccshim begin
inline void MwccHeapImageSShim() { const MwccHeapImageS image = { {0}, 1, {0}, 1, {0} }; }
#pragma section mwccshim end

void JetskiVehicle::ShakeJoystick(float dt, int crash)
{
    JoystickDevice *joystick = JOYSTICK;
    if (joystick && LpGame->m_competition->m_status == 5 && m_player >= 0) {
        if (crash) {
            joystick->PlayEffect(2, 1.1f, 0);
            m_x5C0 = 0.0f;
            m_x5B8 = 0;
            m_x5BC = 0;
        } else if (!m_x530) {
            _D3DVECTOR tilt = m_body->m_object->RotateVectorWorldToLocal(&m_x4E0);
            static float zl = 7.0f;
            static float zh = 20.0f;
            static float xl = 23.0f;
            static float xh = 50.0f;
            static float yl = 12.0f;
            static float yh = 22.0f;
            if (m_x5B4 && m_body->m_x30)
                m_x5C0 += dt;
            if (m_collisionModel->m_x6C) {
                if (!m_x5BC) {
                    JOYSTICK->PlayEffect(1, 0.3f, 0);
                    m_x5BC = 1;
                }
            } else if (tilt.z > zh || tilt.z < -zh) {
                JOYSTICK->PlayEffect(1, 0.1f, 0);
            } else if (tilt.z > zl || tilt.z < -zl) {
                JOYSTICK->PlayEffect(1, 0.2f, 0);
            } else if (tilt.x > xh || tilt.x < -xh) {
                JOYSTICK->PlayEffect(1, 0.1f, 0);
            } else if (tilt.x > xl || tilt.x < -xl) {
                JOYSTICK->PlayEffect(1, 0.2f, 0);
            } else if (tilt.y > yh || tilt.y < -yh) {
                JOYSTICK->PlayEffect(1, 0.1f, 0);
            } else if (tilt.y > yl || tilt.y < -yl) {
                JOYSTICK->PlayEffect(1, 0.2f, 0);
            } else if (m_x5B4 && !m_body->m_x30) {
                JOYSTICK->PlayEffect(0, 0.09f + 0.07f * m_x5C0, 0);
                m_x5C0 = 0.0f;
            } else {
                m_x5BC = 0;
                m_x5B8 = 0;
            }
        } else {
            WaterWrecker *wrecker = m_rider->m_wrecker;
            if (wrecker->m_x124) {
                if (wrecker->m_buoyancy->m_x38) {
                    if (!m_x5B8) {
                        joystick->PlayEffect(0, 0.2f, 0);
                        m_x5B8 = 1;
                    }
                } else {
                    joystick->PlayEffect(0, 0.3f, 0);
                }
            }
        }
    }
}


// ---- src/JetskiVehicle/MakeInvulnerable.cpp


void JetskiVehicle::MakeInvulnerable()
{
    m_invulnerable = 2.0f;
    m_highSpeed->AddToObjectTypeList(COLLISION_BIKE);
    m_collision->AddToObjectTypeList(COLLISION_BIKE);
    m_xA4->AddToObjectTypeList(COLLISION_BIKE);
    m_trigger->AddToObjectTypeList(COLLISION_BIKE);
}


// ---- src/JetskiVehicle/MakeVulnerable.cpp

void JetskiVehicle::MakeVulnerable()
{
    m_highSpeed->ClearObjectTypeList();
    m_collision->ClearObjectTypeList();
    m_xA4->ClearObjectTypeList();
    m_trigger->ClearObjectTypeList();
    m_detection->m_collision->CheckForCollision(0);
    if (m_detection->m_collision->m_collided)
        MakeInvulnerable();
    else
        m_invulnerable = 0;
}


// ---- src/JetskiVehicle/CheckForEdgeOfWorld.cpp

// Returns which edge of the world the jetski is past (1 -x, 2 +x, 3 -z,
// 4 +z) or 0, and in *distance how far inside the nearest edge it is.
int JetskiVehicle::CheckForEdgeOfWorld(float *distance)
{
    float originX;
    float minX;
    float originZ;
    float maxZ;
    float maxX;
    float minZ;
    float rawMinZ;    // its own named local: see below
    minX = 20.0f + m_data->m_manager->m_negXEdgeDist;
    // Reading m_minZ into a named local (coloured after the codegen temps)
    // gives minX's field-load temp $f1 and this one $f2, as in retail.
    rawMinZ = m_data->m_manager->m_negZEdgeDist;
    minZ = 20.0f + rawMinZ;
    originX = m_data->m_manager->m_posXEdgeDist;
    originZ = m_data->m_manager->m_posZEdgeDist;
    _D3DVECTOR pos = m_body->m_x60;
    maxX = originX + 256.0f * m_data->m_manager->m_terrainWidth * m_data->m_manager->m_terrainWidthScale - 20.0f;
    maxZ = originZ + 256.0f * m_data->m_manager->m_terrainBreadth * m_data->m_manager->m_terrainBreadthScale - 20.0f;
    *distance = pos.x - minX;
    *distance = Fmin(*distance, maxX - pos.x);
    *distance = Fmin(*distance, pos.z - minZ);
    *distance = Fmin(*distance, maxZ - pos.z);
    if (pos.x < minX)
        return 1;
    if (pos.x > maxX)
        return 2;
    if (pos.z < minZ)
        return 3;
    if (pos.z > maxZ)
        return 4;
    return 0;
}


// ---- src/JetskiVehicle/KeyDownHandler.cpp

int KEYDOWN_HANDLER_TEST(AssignableActionIndex action, DeviceType device, ProcessNode *node, unsigned int mask);

extern JetskiGame *LpGame;

int JetskiVehicle::KeyDownHandler(ProcessNode *node, KeyInfo *key)
{
    if (KEYDOWN_HANDLER_TEST((AssignableActionIndex)9, (DeviceType)2, node, 0x80000000)) {
        if (LpGame->m_competition->m_status != 10) {
            int type = LpGame->m_gameOptions->m_raceType;
            if (type == 2) {
                if (!LpGame->m_competition->m_faded[0] && !LpGame->m_competition->m_faded[1] && m_player == node->m_controller) {
                    m_rider->StopSpecialAnimation();
                    m_x1DC = LpGame->m_gameOptions->m_raceType != 4;
                    if (m_player >= 0 && LpGame->m_competition) {
                        if (m_resetTimer == 0.0f) {
                            m_resetTimer = 0.2f;
                            m_x1F4 = 100;
                            if (LpGame->m_competition->m_numPlayers == 1)
                                LpGame->m_competition->m_fadeOutTime[LpGame->m_competition->m_cameraControllers[0]->m_viewport] = 180;
                            else
                                LpGame->m_competition->m_fadeOutTime[LpGame->m_competition->m_cameraControllers[m_player]->m_viewport] = 180;
                        }
                    } else {
                        m_resetTimer = 0.0f;
                        ResetPosition(1);
                    }
                }
            } else if (!m_x530 && (type == 1 || type == 6 || type == 7 || type == 3 || type == 4)) {
                if (!LpGame->m_competition->m_faded[0] && !LpGame->m_competition->m_faded[1] && m_player == node->m_controller) {
                    if (m_ownStatus->m_flags & 0x10)
                        m_raceStatus->EndStunts(m_ownStatus, 0, 0.0f);
                    m_rider->StopSpecialAnimation();
                    m_rider->m_x8A4 = 1;
                    m_x1DC = LpGame->m_gameOptions->m_raceType != 4;
                    if (m_player >= 0 && LpGame->m_competition) {
                        if (m_resetTimer == 0.0f) {
                            m_resetTimer = 1.2f;
                            m_x1F4 = 800;
                            if (LpGame->m_competition->m_numPlayers == 1)
                                LpGame->m_competition->m_fadeOutTime[LpGame->m_competition->m_cameraControllers[0]->m_viewport] = 300;
                            else
                                LpGame->m_competition->m_fadeOutTime[LpGame->m_competition->m_cameraControllers[m_player]->m_viewport] = 300;
                        }
                    } else {
                        m_resetTimer = 0.0f;
                        ResetPosition(1);
                    }
                }
            }
        }
    }
    return GameObject::KeyDownHandler(node, key);
}


// ---- src/JetskiVehicle/RSFinishedRaceCallback.cpp

extern JetskiGame *LpGame;

int JetskiVehicle::RSFinishedRaceCallback(RaceStatusClass *, RaceStatusStruct_Tag *)
{
    int finished = 0;
    JetskiCompetition *competition;
    switch (LpGame->m_gameOptions->m_raceType) {
    case 6:
        competition = LpGame->m_competition;
        // +0x40C: JetskiCareerCompetition::m_raceType (the only competition with data there)
        if (((JetskiCareerCompetition *)competition)->m_raceType == 1)
            m_ownStatus->m_place = competition->EndOfRaceRecordKeeping();
    case 1:
    case 7:
        finished = 1;
        break;
    case 0:
    case 2:
    case 3:
    case 4:
    case 5:
    case 8:
        break;
    }
    if (finished) {
        JetskiVCRController *vcr = LpGame->m_competition->m_vcr;
        if (vcr && m_player == 0)
            vcr->m_recording = 0;
    }
    if (m_ownStatus->m_place == 1 && m_rider->m_sounds)
        m_rider->m_sounds->PlayWinSounds();
    return 1;
}


// ---- src/JetskiVehicle/RSGateMissedCallback.cpp

extern JetskiGame *LpGame;

int JetskiVehicle::RSGateMissedCallback(RaceStatusClass *, GateClass *)
{
    if (LpGame->m_gameOptions->m_difficulty == 3) {
        m_ownStatus->m_x6F4 = 0.0f;
    } else {
        m_ownStatus->m_x6F4 -= m_gatePenalty;
        if (m_ownStatus->m_x6F4 < 0.0f)
            m_ownStatus->m_x6F4 = 0.0f;
    }
    if (m_ownStatus->m_x6F4 <= 0.0f || LpGame->m_gameOptions->m_difficulty == 2) {
        m_x5D4 = 1.5f;
        if (m_player >= 0)
            m_engineSound->PlayEngineStallSound();
    }
    return 1;
}


// ---- src/JetskiVehicle/RSGatePassedCallback.cpp

int JetskiVehicle::RSGatePassedCallback(RaceStatusClass *, GateClass *)
{
    m_ownStatus->m_x6F4 += m_gateBonus;
    if (m_ownStatus->m_x6F4 > 120.0f)
        m_ownStatus->m_x6F4 = 120.0f;
    return 1;
}


// ---- src/JetskiVehicle/RSRaceOrderChangeCallback.cpp

extern JetskiGame *LpGame;

int JetskiVehicle::RSRaceOrderChangeCallback(RaceStatusClass *, RaceStatusStruct_Tag *mine, RaceStatusStruct_Tag *other, int passed)
{
    if (!passed) {
        if (mine->m_x6D4 > 30.0f) {
            if (other && m_rider->m_sounds &&
                (m_player >= 0 || LpGame->m_competition->m_vehicles[other->m_id]->m_player >= 0) &&
                mine->m_trackPosition.m_segment == other->m_trackPosition.m_segment) {
                if (m_x784 == -1.0f)
                    m_x78C = m_ownStatus->m_place;
                m_x788 = 0;
                m_x784 = 0.0f;
            }
            if (m_player >= 0 && LpGame->m_competition->m_crowd)
                LpGame->m_competition->m_crowd->PlayCheerSound();
        }
    } else {
        if (mine->m_x6D4 > 30.0f) {
            if (other && m_rider->m_sounds &&
                (m_player >= 0 || LpGame->m_competition->m_vehicles[other->m_id]->m_player >= 0) &&
                mine->m_trackPosition.m_segment == other->m_trackPosition.m_segment) {
                if (m_x784 == -1.0f)
                    m_x78C = m_ownStatus->m_place;
                m_x788 = 1;
                m_x784 = 0.0f;
            }
            if (m_player >= 0 && mine->m_place < 3 && LpGame->m_competition->m_crowd)
                LpGame->m_competition->m_crowd->PlayBooSound();
        }
    }
    return 1;
}


// ---- src/JetskiVehicle/RSVehicleTakesLeadCallback.cpp
int JetskiVehicle::RSVehicleTakesLeadCallback(RaceStatusClass *, RaceStatusStruct_Tag *)
{
    return 1;
}


// ---- src/JetskiVehicle/RSLapUpdateCallback.cpp

extern "C" void *memset(void *dst, int c, unsigned int n);

extern JetskiGame *LpGame;

int JetskiVehicle::RSLapUpdateCallback(RaceStatusClass *race, RaceStatusStruct_Tag *)
{
    JetskiCompetition *competition = LpGame->m_competition;
    if (competition->m_vcr && !competition->m_ghostEnabled && m_player == 0) {
        competition->m_vcr->REC();
        if (m_ownStatus->m_lap < race->m_x64) {
            LpGame->m_competition->m_waterTime = LpGame->m_competition->m_waterDisplacement->m_time;
            LpGame->m_competition->m_baseTime = ((Water *)LpGame->m_competition->m_water)->GetBaseTime();
        }
    }
    if (LpGame->m_gameOptions->m_raceType == 3)
        memset(m_rider->m_x404, 0, 0x80);
    if (m_player >= 0) {
        // Re-read for each test: a local copy becomes a range check.
        m_newLap = LpGame->m_gameOptions->m_raceType == 2 || LpGame->m_gameOptions->m_raceType == 3 ||
                   LpGame->m_gameOptions->m_raceType == 6 || LpGame->m_gameOptions->m_raceType == 1 ||
                   LpGame->m_gameOptions->m_raceType == 7;
    }
    return 1;
}


// ---- src/JetskiVehicle/LoadGenericSounds.cpp

extern JetskiGame *LpGame;

// The vehicle's .aud lists; only the three single-sound lists used here.
struct VehicleAudType {
    ContainerList<char *> pad0[10];     // +0x00 (JetskiAudio.cpp's lists)
    ContainerList<char *> m_x0C8;       // +0xC8
    ContainerList<char *> m_x0DC;       // +0xDC
    ContainerList<char *> m_x0F0;       // +0xF0
};

// JetskiSpraySoundEmitter has no header yet; its Create returns a success flag.
class JetskiSpraySoundEmitter : public SoundEmitter {
public:
    JetskiSpraySoundEmitter(AuralScape *scape, PSXSoundGroup *group, short id, int enabled);
    int Create(JetskiVehicle *vehicle, VehicleAudType *audType, float maxDistance);
    virtual ~JetskiSpraySoundEmitter();
    JetskiVehicle *m_vehicle;           // +0x200
    char pad204[0x210 - 0x204];
};

// Drop an emitter whose Create failed (Release returns the refs left).
#define RELEASE_IF_LAST(p)          \
    if (p) {                        \
        if (!(p)->Release())        \
            (p) = 0;                \
    }

// Heap shim (new; the per-function file needed none): the first 100.0f
// node's hascall byte, recalibrated for the TU with image.py (INLINE=1,
// SIZE=0x30000): without it the 20.0f/100.0f arguments of the three
// SoundEmitter::Create calls load in the other order (12 words).
struct MwccHeapImageL {
    char pad0[0x5F5];
    char flag0;
    char tail[0x2FA0A];
};
#pragma section mwccshim begin
inline void MwccHeapImageLShim() { const MwccHeapImageL image = { {0}, 1, {0} }; }
#pragma section mwccshim end

int JetskiVehicle::LoadGenericSounds(VehicleAudType *audType)
{
    _DS3DBUFFER buffer;
    buffer.flMinDistance = 0.0f;
    buffer.flMaxDistance = 100.0f;
    if (m_player >= 0) {
        m_spraySound = new JetskiSpraySoundEmitter(LpGame->m_auralScape, m_data->m_soundGroup, 3, 1);
        if (m_spraySound) {
            if (((JetskiSpraySoundEmitter *)m_spraySound)->Create(this, audType, 100.0f)) {
                AddChild(m_spraySound, -1);
                LpGame->m_auralScape->AddEmitter(m_spraySound);
            } else {
                RELEASE_IF_LAST(m_spraySound)
            }
        }
        m_edgeMonsterSound = new JetskiEdgeMonsterSoundEmitter(LpGame->m_auralScape->m_soundInterface, m_data->m_soundGroup->m_groupID, 1);
        if (m_edgeMonsterSound) {
            if (m_edgeMonsterSound->Create(audType)) {
                AddChild(m_edgeMonsterSound, -1);
            } else {
                RELEASE_IF_LAST(m_edgeMonsterSound)
            }
        }
    }
    m_splashSound = new JetskiSplashSoundEmitter(LpGame->m_auralScape, m_data->m_soundGroup, 3, 1);
    if (m_splashSound) {
        if (((JetskiSplashSoundEmitter *)m_splashSound)->Create(this, audType, 100.0f)) {
            AddChild(m_splashSound, -1);
            LpGame->m_auralScape->AddEmitter(m_splashSound);
        } else {
            RELEASE_IF_LAST(m_splashSound)
        }
    }
    m_terrainSound = new JetskiTerrainSoundEmitter(LpGame->m_auralScape, m_data->m_soundGroup, 3, 1);
    if (m_terrainSound) {
        if (((JetskiTerrainSoundEmitter *)m_terrainSound)->Create(this, audType, 100.0f)) {
            AddChild(m_terrainSound, -1);
            LpGame->m_auralScape->AddEmitter(m_terrainSound);
        } else {
            RELEASE_IF_LAST(m_terrainSound)
        }
    }
    m_x748 = new SoundEmitter(LpGame->m_auralScape, m_data->m_soundGroup, 10, 1);
    if (m_x748) {
        if (m_x748->Create(buffer, 2, 20.0f, 100.0f, 0, 0)) {
            if (m_x748->LoadSound(audType->m_x0C8.GetItem(0), 0))
                AddChild(m_x748, -1);
            else
                m_x748->Release();
        } else {
            RELEASE_IF_LAST(m_x748)
        }
    }
    m_x750 = new SoundEmitter(LpGame->m_auralScape, m_data->m_soundGroup, 10, 1);
    if (m_x750) {
        if (m_x750->Create(buffer, 2, 20.0f, 100.0f, 0, 0)) {
            if (m_x750->LoadSound(audType->m_x0DC.GetItem(0), 0))
                AddChild(m_x750, -1);
            else
                m_x750->Release();
        } else {
            RELEASE_IF_LAST(m_x750)
        }
    }
    m_x754 = new SoundEmitter(LpGame->m_auralScape, m_data->m_soundGroup, 10, 1);
    if (m_x754) {
        if (m_x754->Create(buffer, 2, 20.0f, 100.0f, 0, 0)) {
            if (m_x754->LoadSound(audType->m_x0F0.GetItem(0), 0))
                AddChild(m_x754, -1);
            else
                m_x754->Release();
        } else {
            RELEASE_IF_LAST(m_x754)
        }
    }
    return 1;
}


// ---- src/sinit/JetskiVehicle.cpp
// Static initializer of JetskiVehicle.cpp: only the axis vectors from the shared header.

