// WaterWrecker.cpp  @ 0x0032D3B0 (text 0x32D3B0-0x3308A0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp)
//
// Retail's WaterWrecker.cpp as one translation unit. RainbowPS2Snd's code
// (a TU without an initializer) follows at 0x3308A0, then
// MicroProgram.cpp's string tables. __sinit_WaterWrecker.cpp builds only
// the axis vectors. `make link` places its data as well as its code.


#pragma inline_depth(5)
#pragma fast_fptosi on

#include "GameObject/GameObject.h"
#include "d3dtypes.h"
#include "D3DIMSoultreeCharacter/D3DIMSoultreeCharacter.h"
#include "SelectiveGravityModel/SelectiveGravityModel.h"
#include "SpringCollisionModel/SpringCollisionModel.h"
#include "BuoyancyModel/BuoyancyModel.h"
#include "WaterWrecker/WaterWrecker.h"
#include "CollisionObject/CollisionObject.h"
#include "SpringCollisionDetectionModel/SpringCollisionDetectionModel.h"
#include "SoultreeObject/SoultreeObject.h"
#include "PhysicsRigidBody/PhysicsRigidBody.h"
#include "Motion/Motion.h"
#include "JetskiRider/JetskiRider.h"
#include "JetskiRiderSoundEmitter/JetskiRiderSoundEmitter.h"
#include "Water/Water.h"
#include "PSXWater/PSXWater.h"
#include "PhysicsBody/PhysicsBody.h"
#include "global/Vu0Inline.h"
#include "Character/Character.h"
#include "SplashParticleEmitter/SplashParticleEmitter.h"
#include "sinit/AxisVectors.h"

// ---- src/WaterWrecker/ctor.cpp


WaterWrecker::WaterWrecker(int id) : GameObject(id)
{
    m_water = 0;
    m_terrain = 0;
    m_separations = 0;
    m_numSeparations = 0;
    // retail clears the first four counts here and all six again below
    m_numByType[0] = 0;
    m_numByType[1] = 0;
    m_numByType[2] = 0;
    m_numByType[3] = 0;
    m_inAirMotions = 0;
    m_numInAirMotions = 0;
    m_bounceMotions = 0;
    m_numBounceMotions = 0;
    m_treadMotions = 0;
    m_numTreadMotions = 0;
    m_xF8 = 0;
    m_xFC = 0;
    m_xB0 = 0;
    m_wreckType = 0.0f;
    m_lastWreckType = 0.0f;
    m_x14C = 0;
    m_x120 = 0;
    m_x15C = 0;
    m_x12C = 0;
    m_x130 = 0;
    m_x13C = 0;
    m_x140 = 0x1D;
    m_particles = 0;
    m_x134 = 0;
    m_x150 = 0;
    m_x154 = 1;
    m_wreckIndex = 0;
    m_transitionTime = 0.5f;
    m_x104 = 0.03f;
    m_x108 = 0;
    m_x10C = 0;
    m_x110 = ZeroVector;
    for (int i = 0; i < 6; i++)
        m_numByType[i] = 0;
    m_numParticlePoints = 0;
    m_particlePoints = 0;
    m_x60 = 0;
    m_x64 = 0;
    m_x160 = 1;
    m_x68 = 0;
}


// ---- src/WaterWrecker/dtor.cpp

WaterWrecker::~WaterWrecker()
{
    if (m_bounceMotions)
        free(m_bounceMotions);
    if (m_treadMotions)
        free(m_treadMotions);
    if (m_inAirMotions)
        free(m_inAirMotions);
    if (m_separations)
        free(m_separations);
    if (m_xF8)
        free(m_xF8);
    if (m_particlePoints)
        free(m_particlePoints);
    m_numParticlePoints = 0;
}


// ---- src/WaterWrecker/Create.cpp



struct _D3DMATRIX;
class Terrain;
class PSXWater;
class SearchNode;












// Some constants reach the calls as default arguments of these inline helpers;
// literals at the call (or passed explicitly) order the argument loads
// differently from retail.
// Heap shim (not part of the retail file, image object in .mwccshim): a 0x30000 parser
// heap image with the "has call" bytes of the helper's 0.2f and 0.3f nodes
// set (epoch offsets 0x10/0x70 in the per-function Create.cpp), as the
// per-function file's heap residue left them. Without it Create loads 1.0f
// before 0.3f (1 word).
struct MwccHeapImage {
    char pad0[0x5];
    char flag0;
    char pad1[0x5F];
    char flag1;
    char tail[0x2FF9A];
};
#pragma section mwccshim begin
inline void MwccHeapImageShim() { const MwccHeapImage image = { {0}, 1, {0}, 1, {0} }; }
#pragma section mwccshim end

static inline void SetSpringParameters(SpringCollisionModel *springs, float k = 0.2f, float z = 0.0f, float d = 0.3f)
{
    springs->SetCollisionParameters(1.0f, k, d, k, d, z, z, z);
}

static inline void SetBuoyancyParameters(BuoyancyModel *buoyancy, float density = 58.6664009f, float scale = 1.0f)
{
    buoyancy->SetCollisionParameters(scale, 5.0f, 0.1f, 0.4f, 0.3f, density, -5.0f);
}

WaterWrecker *WaterWrecker::Create(D3DIMSoultreeCharacter *rider, Terrain *terrain, PSXWater *water,
                                   PhysicsRigidBody *vehicle, char *name, SearchNode *node)
{
    GameObject::Create();
    m_rider = (JetskiRider *)rider;
    m_rider->m_root->SetPosition(0, _D3DVECTOR(0.0f, 120.0f, 0.0f));
    m_terrain = terrain;
    m_water = water;
    m_vehicle = vehicle;
    m_gravity = new SelectiveGravityModel(1);
    m_gravity->Create(1);
    AddChild(m_gravity, 2);
    m_body = new PhysicsRigidBody(1);
    m_body->Create();
    AddChild(m_body, 2);
    m_springs = new SpringCollisionModel(1);
    m_springs->Create(0x3C);
    AddChild(m_springs, 2);
    m_body->SetSoultreeObject(m_rider->m_root);
    m_gravity->AddBody(m_body);
    m_body->SetMass(150.0f / 32.2f);    // 150 lb in slugs
    m_body->m_x280 = 5.0f;
    CalculateInertia();
    m_body->m_enabled = 0;
    m_body->SetInitialBodyAngularRate(ZeroVector);
    m_body->SetInitialVelocity(ZeroVector);
    m_body->Reset();
    m_springs->SetBody(m_body);
    SetSpringParameters(m_springs);
    m_vehicleObject = m_rider->m_root->m_child;
    m_springs->m_x34 = 1;
    m_body->m_x320 = 1;
    m_buoyancy = new BuoyancyModel(1);
    m_buoyancy->Create(m_body, water);
    SetBuoyancyParameters(m_buoyancy);
    AddChild(m_buoyancy, -1);
    return this;
}


// ---- src/WaterWrecker/CreateCollisionObject.cpp


extern unsigned int WORLDOBJECT_PHYSICSBODY;
extern unsigned int COLLISION_RIDER;

class SearchNode;
class Terrain;
class SpringCollisionModel;





void WaterWrecker::CreateCollisionObject(SearchNode *node)
{
    m_collisionModel = new SpringCollisionDetectionModel(1);
    m_collisionModel->Create(m_springs, m_terrain, 1, 0, ZeroVector, node);
    m_collisionModel->m_collision->m_ownerType = WORLDOBJECT_PHYSICSBODY | COLLISION_RIDER;
    AddChild(m_collisionModel, 2);
}


// ---- src/WaterWrecker/AddTreadMotion.cpp
extern "C" void *realloc(void *p, unsigned int size);

class Motion;


int WaterWrecker::AddTreadMotion(Motion *motion, int x)
{
    m_treadMotions = (WreckMotion *)realloc(m_treadMotions, (m_numTreadMotions + 1) * sizeof(WreckMotion));
    m_treadMotions[m_numTreadMotions].m_motion = motion;
    m_treadMotions[m_numTreadMotions].m_x4 = x;
    m_numTreadMotions++;
    return m_numTreadMotions - 1;
}


// ---- src/WaterWrecker/AddBounceMotion.cpp
extern "C" void *realloc(void *p, unsigned int size);

class Motion;


int WaterWrecker::AddBounceMotion(Motion *motion, int x)
{
    m_bounceMotions = (WreckMotion *)realloc(m_bounceMotions, (m_numBounceMotions + 1) * sizeof(WreckMotion));
    m_bounceMotions[m_numBounceMotions].m_motion = motion;
    m_bounceMotions[m_numBounceMotions].m_x4 = x;
    m_numBounceMotions++;
    return m_numBounceMotions - 1;
}


// ---- src/WaterWrecker/AddInAirMotion.cpp
extern "C" void *realloc(void *p, unsigned int size);

class Motion;


int WaterWrecker::AddInAirMotion(Motion *motion, int x)
{
    m_inAirMotions = (WreckMotion *)realloc(m_inAirMotions, (m_numInAirMotions + 1) * sizeof(WreckMotion));
    m_inAirMotions[m_numInAirMotions].m_motion = motion;
    m_inAirMotions[m_numInAirMotions].m_x4 = x;
    m_numInAirMotions++;
    return m_numInAirMotions - 1;
}


// ---- src/WaterWrecker/AddSeparationMotion.cpp

int WaterWrecker::AddSeparationMotion(WreckType type, Motion *motion, int x)
{
    m_separations = (WreckSeparation *)realloc(m_separations, (m_numSeparations + 1) * sizeof(WreckSeparation));
    m_separations[m_numSeparations].m_type = type;
    m_separations[m_numSeparations].m_motion = motion;
    m_separations[m_numSeparations].m_x8 = x;
    m_numByType[type]++;
    m_numSeparations++;
    return m_numSeparations - 1;
}


// ---- src/WaterWrecker/AddVehicleCollisionObject.cpp

void WaterWrecker::AddVehicleCollisionObject(CollisionObject *object)
{
    m_x130 = object;
}

// ---- src/WaterWrecker/AnimateRider.cpp
// Compiler session state: retail compiled
// this file in an mwcc session that had already generated a two-argument
// runtime helper call; this unused function recreates that state. It is not
// part of the retail file.
static void MWCC_SHIM_TEXT MwccSessionState(long *a) { a[0] = a[0] / a[1]; }

extern "C" int rand(void);
extern "C" void srand(unsigned int seed);
extern int TimerOverflowCounterT0;

inline unsigned int timeGetTime()
{
    static long Time = 0;
    static int lastCount = 0;
    int count;
    int elapsed;
    int overflows;
    count = *(volatile int *)0x10000000;
    overflows = TimerOverflowCounterT0;
    TimerOverflowCounterT0 = 0;
    if (lastCount <= count)
        elapsed = count - lastCount;
    else
        elapsed = 0x10000 - lastCount + count;
    if (lastCount >= count && overflows > 0)
        overflows--;
    lastCount = count;
    Time += elapsed + (overflows << 16);
    return 0.0017066667f * Time;
}

inline int GetRandomNumber(int lo, int hi)
{
    static int seeder = 0;
    if (!seeder) {
        seeder = timeGetTime();
        srand(seeder);
    }
    int r = (int)((hi - lo + 1) * (rand() * 4.656613e-10f));
    r = r > hi ? hi : r;
    r = lo > r ? lo : r;
    return r;
}





// Runs the wreck state machine: 0 idle, 1 separating from the jetski,
// 2 in the air, 3 bouncing, 4 treading water.
void WaterWrecker::AnimateRider(float dt)
{
    JetskiRider *rider = (JetskiRider *)m_rider;
    _D3DVECTOR position;
    GameObject::Tick(dt);
    m_x124 = 0;
    switch (m_xB0) {
    case 0:
        m_x120 = 0;
        m_x150 = 0;
        m_body->m_enabled = 0;
        m_wreckType = 0.0f;
        PlaybackRider(dt);
        break;
    case 1:
        m_wreckType = 1.0f;
        RunSimulation(dt);
        m_vehicleObject->GetMatrix(0, &m_baseTransform);
        PlaybackRider(dt);
        if (m_x15C && m_rider->m_motion->m_duration - m_rider->m_frame <= 0.1f) {
            CalculateVelocityAndAngularRate(dt, 1);
            m_body->m_enabled = 1;
            if (m_separations[(int)m_wreckIndex].m_x8 >= 0)
                m_wreckIndex = m_separations[(int)m_wreckIndex].m_x8;
            else
                m_wreckIndex = GetRandomNumber(0, m_numInAirMotions - 1);
            m_xB0 = 2;
            m_transitionTime = 0.1f;
        } else {
            m_body->m_velocity = m_vehicle->m_velocity;
            m_body->m_xA0 = m_vehicle->m_xA0;
        }
        if (m_buoyancy->m_x38) {
            CalculateVelocityAndAngularRate(dt, 0);
            m_body->m_enabled = 1;
            m_wreckIndex = GetRandomNumber(0, m_numTreadMotions - 1);
            m_xB0 = 4;
            m_x14C = 0;
            m_transitionTime = 0.4f;
        } else if (m_springs->m_x3C) {
            CalculateVelocityAndAngularRate(dt, 0);
            m_body->m_enabled = 1;
            m_wreckIndex = GetRandomNumber(0, m_numBounceMotions - 1);
            m_xB0 = 3;
            rider->m_sounds->PlayWreckSounds(&position, 1, 1, 0);
            m_x14C = 0;
            m_transitionTime = 0.4f;
        }
        m_x148 = m_rider->m_blendStart;
        break;
    case 2:
    case 3:
    case 4:
        if (m_xB0 == 2)
            m_wreckType = 2.0f;
        else if (m_xB0 == 3)
            m_wreckType = 3.0f;
        else
            m_wreckType = 4.0f;
        RunSimulation(dt);
        if (m_xB0 == 2) {
            if (m_springs->m_x3C) {
                if (m_inAirMotions[(int)m_wreckIndex].m_x4 >= 0)
                    m_wreckIndex = m_inAirMotions[(int)m_wreckIndex].m_x4;
                else
                    m_wreckIndex = GetRandomNumber(0, m_numBounceMotions - 1);
                m_xB0 = 3;
                rider->m_sounds->PlayWreckSounds(&position, 1, 1, 0);
                m_x14C = 0;
                m_transitionTime = 0.5f;
            } else if (m_buoyancy->m_x38) {
                if (m_inAirMotions[(int)m_wreckIndex].m_x4 >= 0)
                    m_wreckIndex = m_inAirMotions[(int)m_wreckIndex].m_x4;
                else
                    m_wreckIndex = GetRandomNumber(0, m_numTreadMotions - 1);
                m_xB0 = 4;
                m_x14C = 0;
                m_transitionTime = 0.2f;
            }
        } else if (m_xB0 == 3) {
            if (m_buoyancy->m_x38) {
                m_x14C += dt;
                if (m_x14C > 1.0f) {
                    if (m_bounceMotions[(int)m_wreckIndex].m_x4 >= 0)
                        m_wreckIndex = m_bounceMotions[(int)m_wreckIndex].m_x4;
                    else
                        m_wreckIndex = GetRandomNumber(0, m_numTreadMotions - 1);
                    m_xB0 = 4;
                    m_x14C = 0;
                    m_transitionTime = 1.1f;
                }
            }
        } else if (m_xB0 == 4) {
            if (m_springs->m_x3C) {
                m_x14C += dt;
                if (m_x14C > 1.0f) {
                    if (m_treadMotions[(int)m_wreckIndex].m_x4 >= 0)
                        m_wreckIndex = m_treadMotions[(int)m_wreckIndex].m_x4;
                    else
                        m_wreckIndex = GetRandomNumber(0, m_numBounceMotions - 1);
                    m_xB0 = 3;
                    rider->m_sounds->PlayWreckSounds(&position, 1, 1, 0);
                    m_x14C = 0;
                    m_transitionTime = 0.8f;
                }
            }
        }
        m_vehicleObject->GetMatrix(0, &m_baseTransform);
        PlaybackRider(dt);
        if (m_x150 || m_rider->Character::m_blendTime - m_rider->m_blendStart <= dt || m_rider->m_blendStart < m_x148) {
            FixBaseObjectTransform();
            m_x150 = 1;
        }
        m_x148 = m_rider->m_blendStart;
        if (m_x154 && m_x130 && !m_collisionModel->m_collision->CheckForOverlap(m_x130)) {
            m_collisionModel->m_collision->RemoveFromTestList(m_x130);
            m_x154 = 0;
        }
        break;
    }
    if (m_xB0 != 0 && m_xB0 != 1)
        GenerateParticles(dt);
}


// ---- src/WaterWrecker/PlaybackRider.cpp


class Motion;
class MotionFilter;















// The rider's blend state after a motion step (retail inlines it).
static inline void ResetBlend(JetskiRider *rider)
{
    rider->m_x484 = 0;
    rider->m_x488 = 1;
    rider->m_x48C = 0;
}

static inline int Clamp(int i, int max)
{
    if (0 > i)
        i = 0;
    return i < max ? i : max;
}

// Replays a recorded wreck: starts the recorded motion when the wreck type
// changes, then steps the rider's animation.
void WaterWrecker::PlaybackRider(float dt)
{
    // Retail calls through m_rider but reads the sound emitter through this copy.
    JetskiRider *rider = m_rider;
    if (m_wreckType != 0.0f) {
        if (m_wreckType != m_lastWreckType) {
            int index = 0.5f + m_wreckIndex;
            if (m_wreckType == 1.0f) {
                m_rider->PerformActionByMotion(m_separations[Clamp(index, m_numSeparations - 1)].m_motion);
                m_x15C = 1;
            } else if (m_wreckType == 2.0f) {
                m_rider->TransitionToMotion(m_inAirMotions[Clamp(index, m_numInAirMotions - 1)].m_motion, m_transitionTime, 0);
                m_x15C = 1;
            } else if (m_wreckType == 3.0f) {
                m_rider->TransitionToMotion(m_bounceMotions[Clamp(index, m_numBounceMotions - 1)].m_motion, m_transitionTime, 0);
                m_x15C = 1;
            } else {
                m_rider->TransitionToMotion(m_treadMotions[Clamp(index, m_numTreadMotions - 1)].m_motion, m_transitionTime, 0);
                m_x15C = 1;
                if (m_water)
                    m_water->ApplyExternalForce(m_body->m_velocity.y / 10.0f, m_body->m_x60);
                if (rider->m_sounds)
                    rider->m_sounds->PlaySplashSounds(m_x110.y);
            }
        }
        m_rider->TickBlendedFrame(dt, 0, 0);
        m_rider->m_root->UpdateSearchPlacement();
        ResetBlend(m_rider);
    }
    m_lastWreckType = m_wreckType;
}


// ---- src/WaterWrecker/RunSimulation.cpp


// Retail's fabs/fmin/sqrt: the result goes to $f8 and is stored through a
// stack temporary.
static inline float Fabs(float x)
{
    float r;
    asm("abs.s $f8, %1; swc1 $f8, 0(%0)" : : "r"(&r), "f"(x) : "$f8", "memory");
    return r;
}

static inline float Fmin(float a, float b)
{
    float r;
    asm("min.s $f8, %1, %2; swc1 $f8, 0(%0)" : : "r"(&r), "f"(a), "f"(b) : "$f8", "memory");
    return r;
}


static inline float Magnitude(_D3DVECTOR *v)
{
    return Fsqrt(v->x * v->x + v->y * v->y + v->z * v->z);
}


class PhysicsBody;








// Steps the wreck simulation in ticks of at most 1/90 s, tracking the peak
// of the vehicle's m_xC0 on each axis.
void WaterWrecker::RunSimulation(float dt)
{
    float startSpeed = m_body->m_speed;
    m_x110 = ZeroVector;
    m_collisionModel->SimulationTick(dt);
    m_body->m_centerOfMass = m_body->m_object->TransformVectorWorldToLocal(&m_collisionModel->m_center);
    float t = 0.0f;
    float step = Fmin(dt, 0.011111111f);
    while (t < dt) {
        if (m_xB0 != 1)
            m_gravity->SetForceTick(step);
        if (m_x160 == 1)
            m_buoyancy->SimulationTick(step);
        m_springs->SimulationTick(step);
        if (m_springs->m_x3C || m_buoyancy->m_x38) {
            m_gravity->SetGravity(32.2f);
            m_body->m_enabled = 1;
            m_x124 = 1;
            m_x120 = 1;
        } else {
            m_gravity->SetGravity(32.2f * 1.2f);
        }
        m_body->SimulationTick(step);
        t += step;
        step = Fmin(0.011111111f, dt - t);
        if (Fabs(m_body->m_xC0.x) > Fabs(m_x110.x))
            m_x110.x = m_body->m_xC0.x;
        if (Fabs(m_body->m_xC0.y) > Fabs(m_x110.y))
            m_x110.y = m_body->m_xC0.y;
        if (Fabs(m_body->m_xC0.z) > Fabs(m_x110.z))
            m_x110.z = m_body->m_xC0.z;
    }
    if (m_springs->m_x3C) {
        float damping = 1.0f - m_x108;
        _D3DVECTOR *v = &m_body->m_xB0;
        v->x *= damping;
        v->y *= damping;
        v->z *= damping;
        m_body->m_xA0 = m_body->m_object->RotateVectorLocalToWorld(&m_body->m_xB0);
        damping = 1.0f - m_x104;
        v = &m_body->m_velocity;
        v->x *= damping;
        v->y *= damping;
        v->z *= damping;
    }
    m_body->m_speed = Magnitude(&m_body->m_velocity);
    m_x10C = Fabs(m_body->m_speed - startSpeed);
}


// ---- src/WaterWrecker/AddParticleCollisionPoint.cpp
extern "C" void *realloc(void *p, unsigned int size);



class SoultreeObject;

// A pair of points (one on each object) that particles collide between.

// Adds the point pair unless it is already registered.
void WaterWrecker::AddParticleCollisionPoint(_D3DVECTOR point1, SoultreeObject *object1, _D3DVECTOR point2,
                                             SoultreeObject *object2, int flags)
{
    int found = 0;
    for (int i = 0; i < m_numParticlePoints; i++) {
        if (point1 == m_particlePoints[i].m_point1 && object1 == m_particlePoints[i].m_object1 &&
            point2 == m_particlePoints[i].m_point2 && object2 == m_particlePoints[i].m_object2)
            found = 1;
    }
    if (!found) {
        m_particlePoints = (ParticleCollisionPoint *)realloc(
            m_particlePoints, (m_numParticlePoints + 1) * sizeof(ParticleCollisionPoint));
        m_particlePoints[m_numParticlePoints].m_object1 = object1;
        m_particlePoints[m_numParticlePoints].m_point1 = point1;
        m_particlePoints[m_numParticlePoints].m_object2 = object2;
        m_particlePoints[m_numParticlePoints].m_point2 = point2;
        m_particlePoints[m_numParticlePoints].m_velocity = ZeroVector;
        m_particlePoints[m_numParticlePoints].m_x30 = 0;
        m_particlePoints[m_numParticlePoints].m_flags = flags;
        m_numParticlePoints++;
    }
}


// ---- src/WaterWrecker/GenerateParticles.cpp

void WaterWrecker::GenerateParticles(float dt)
{
    if (m_water) {
        JetskiRider *rider = m_rider;
        _D3DVECTOR position;
        m_rider->m_root->m_child->GetPosition(0, &position);
        m_water->GetGridPointIntersection(&position, 0);
        if (m_buoyancy->m_x38 && !m_x68)
            rider->m_splash->InitiateSplash(position, m_body->m_velocity);
        m_x68 = m_buoyancy->m_x38;
        rider->m_splash->m_splashPosition = position;
    }
}


// ---- src/WaterWrecker/CalculateInertia.cpp

struct _D3DVECTOR;
class SoultreeObject;


_D3DMATRIX ZeroMatrix();




// Inertia of a solid box: diagonal 10 * mass / 12.
void WaterWrecker::CalculateInertia()
{
    _D3DMATRIX inertia;
    inertia = ZeroMatrix();
    float i = 10.0f * (m_body->m_mass / 12.0f);
    inertia._11 = i;
    inertia._22 = i;
    inertia._33 = i;
    inertia._44 = 1.0f;
    m_body->SetInertia(inertia);
}








// ---- src/WaterWrecker/TriggerWreck.cpp

void WaterWrecker::TriggerWreck()
{
    TriggerWreck((WreckType)GetRandomNumber(0, 5), 0);
}












// ---- src/WaterWrecker/TriggerWreck_0032F940.cpp

// Picks a random separation motion of the given type and starts the wreck.
void WaterWrecker::TriggerWreck(WreckType type, int x)
{
    m_collisionModel->m_collision->UpdateCollisionInfo();
    m_collisionModel->m_collision->Enable();
    m_collisionModel->m_collision->m_searchable = 1;
    m_collisionModel->m_collision->m_collidable = 0;
    m_x154 = 1;
    m_collisionModel->m_collision->AddToTestList((CollisionObject *)m_x130);
    int which = GetRandomNumber(0, m_numByType[type] - 1);
    int count = -1;
    m_wreckIndex = 0.0f;
    while (m_wreckIndex < m_numSeparations && count != which) {
        if (m_separations[(int)m_wreckIndex].m_type == type)
            count++;
        m_wreckIndex += 1.0f;
    }
    m_wreckIndex -= 1.0f;
    m_xB0 = 1;
    m_body->Reset();
    m_body->m_xB0 = ZeroVector;
    m_body->m_velocity = ZeroVector;
    m_body->m_xA0 = ZeroVector;
    m_rider->m_splash->m_particlesEnabled = 1;
}


// ---- src/WaterWrecker/CalculateVelocityAndAngularRate.cpp

extern "C" int rand();
_D3DVECTOR GetMatrixRow(_D3DMATRIX *matrix, int row);

// VU0 cross product, out = a x b.
static inline void CrossProduct(_D3DVECTOR &out, const _D3DVECTOR &a, const _D3DVECTOR &b)
{
    asm("lqc2 vf4, 0x0(%1); lqc2 vf5, 0x0(%2); vopmula.xyz ACC, vf4, vf5; vopmsub.xyz vf6, vf5, vf4; vsub.w vf6, vf6, vf6; sqc2 vf6, 0x0(%0)"
        : : "r"(&out), "r"(&a), "r"(&b) : "memory");
}

static inline float RandomSigned()
{
    return 4.0f * (4.656613e-10f * rand()) - 2.0f;
}

// Give the rider's body the vehicle's velocity and angular rate, plus (when
// it has separated) how far its root moved off the vehicle this tick, then
// a random tumble.
void WaterWrecker::CalculateVelocityAndAngularRate(float dt, int separated)
{
    if (separated) {
        _D3DMATRIX m;
        m_vehicleObject->GetMatrix(0, &m);
        _D3DVECTOR offset = GetMatrixRow(&m_baseTransform, 3);
        offset = GetMatrixRow(&m, 3) - offset;
        _D3DVECTOR baseUp = GetMatrixRow(&m_baseTransform, 1);
        _D3DVECTOR up = GetMatrixRow(&m, 1);
        _D3DVECTOR baseForward = GetMatrixRow(&m_baseTransform, 2);
        _D3DVECTOR forward = GetMatrixRow(&m, 2);
        _D3DVECTOR axis;
        if (baseUp.x != up.x || baseUp.y != up.y || baseUp.z != up.z)
            CrossProduct(axis, baseUp, up);
        else if (baseForward.x != forward.x || baseForward.y != forward.y || baseForward.z != forward.z)
            CrossProduct(axis, baseForward, forward);
        else
            axis = _D3DVECTOR(0.0f, 0.0f, 0.0f);
        float inv = 1.0f / dt;
        offset = inv * offset;
        axis = inv * axis;
        _D3DVECTOR rate = m_body->m_object->RotateVectorWorldToLocal(&axis);
        m_body->m_velocity = m_vehicle->m_velocity + offset;
        m_body->m_xB0 = m_vehicle->m_xB0 + rate;
    } else {
        m_body->m_velocity = m_vehicle->m_velocity;
        m_body->m_xB0 = m_vehicle->m_xB0;
    }
    m_body->m_xB0 += _D3DVECTOR(RandomSigned(), RandomSigned(), RandomSigned());
}


// ---- src/WaterWrecker/FixBaseObjectTransform.cpp
// Orthonormalize's VU0 helpers sit two inlines deep, past mwcc's default.



_D3DVECTOR GetMatrixRow(_D3DMATRIX *m, int row);

// libvu0 sceVu0Normalize.
static inline void Vu0Normalize(_D3DVECTOR *out, _D3DVECTOR *in)
{
    asm("lqc2 vf4, 0x0(%1); vmul.xyz vf5, vf4, vf4; vaddy.x vf5, vf5, vf5y; vaddz.x vf5, vf5, vf5z; vrsqrt Q, vf0w, vf5x; vsub.xyzw vf6, vf0, vf0; vaddw.xyzw vf6, vf6, vf4w; vwaitq; vmulq.xyz vf6, vf4, Q; sqc2 vf6, 0x0(%0)"
        : : "r"(out), "r"(in) : "memory");
}



// sceVu0MulMatrix (out = a * b) taking its inputs by value.
static inline void MulMatrix(_D3DMATRIX *out, _D3DMATRIX a, _D3DMATRIX b)
{
    asm("lqc2 vf5, 0x0(%1); lqc2 vf1, 0x0(%2); lqc2 vf2, 0x10(%2); lqc2 vf3, 0x20(%2); lqc2 vf4, 0x30(%2); lqc2 vf6, 0x10(%1); vmulax.xyzw ACC, vf1, vf5x; vmadday.xyzw ACC, vf2, vf5y; vmaddaz.xyzw ACC, vf3, vf5z; vmaddw.xyzw vf9, vf4, vf5w; lqc2 vf7, 0x20(%1); vmulax.xyzw ACC, vf1, vf6x; vmadday.xyzw ACC, vf2, vf6y; vmaddaz.xyzw ACC, vf3, vf6z; vmaddw.xyzw vf10, vf4, vf6w; lqc2 vf8, 0x30(%1); vmulax.xyzw ACC, vf1, vf7x; vmadday.xyzw ACC, vf2, vf7y; vmaddaz.xyzw ACC, vf3, vf7z; vmaddw.xyzw vf11, vf4, vf7w; vmulax.xyzw ACC, vf1, vf8x; vmadday.xyzw ACC, vf2, vf8y; vmaddaz.xyzw ACC, vf3, vf8z; vmaddw.xyzw vf12, vf4, vf8w; sqc2 vf9, 0x0(%0); sqc2 vf10, 0x10(%0); sqc2 vf11, 0x20(%0); sqc2 vf12, 0x30(%0)"
        : : "r"(out), "r"(&b), "r"(&a) : "memory");
}

// Rebuild m's rotation rows from its forward (row 3) and up (row 2) axes,
// zeroing their w.
static inline void Orthonormalize(_D3DMATRIX &m)
{
    _D3DVECTOR up = _D3DVECTOR(m._21, m._22, m._23);
    _D3DVECTOR forward = _D3DVECTOR(m._31, m._32, m._33);
    _D3DVECTOR side;
    Vu0Normalize(&forward, &forward);
    CrossProduct(side, up, forward);
    Vu0Normalize(&side, &side);
    CrossProduct(up, forward, side);
    m._11 = side.x;
    m._12 = side.y;
    m._13 = side.z;
    m._14 = 0.0f;
    m._21 = up.x;
    m._22 = up.y;
    m._23 = up.z;
    m._24 = 0.0f;
    m._31 = forward.x;
    m._32 = forward.y;
    m._33 = forward.z;
    m._34 = 0.0f;
}





// Put the rider's root back where m_baseTransform (relative to the vehicle)
// says, keeping it upright.
void WaterWrecker::FixBaseObjectTransform()
{
    _D3DMATRIX m __attribute__((aligned(16)));
    m_vehicleObject->GetMatrix(0, &m);
    _D3DVECTOR position = GetMatrixRow(&m_baseTransform, 3);
    Vu0InversMatrix(&m);
    _D3DMATRIX relative __attribute__((aligned(16)));
    MulMatrix(&relative, m_baseTransform, m);
    m_rider->m_root->GetMatrix(0, &m);
    MulMatrix(&m, relative, m);
    Orthonormalize(m);
    m_rider->m_root->SetMatrix(0, &m);
    _D3DVECTOR current;
    m_vehicleObject->GetPosition(0, &current);
    _D3DVECTOR offset = position - current;
    m_rider->m_root->Translate(0, &offset);
}


// ---- src/WaterWrecker/StopWreck.cpp

void WaterWrecker::StopWreck()
{
    if (m_xB0 || m_wreckType != 0.0f) {
        m_xB0 = 0;
        m_x12C = 0;
        m_rider->m_root->ResetMatrices();
        m_rider->ApplyDefaultFrame();
        m_collisionModel->m_collision->Disable();
        m_x120 = 0;
        m_x150 = 0;
        m_transitionTime = 0.0f;
        m_body->m_enabled = 0;
        m_wreckType = 0.0f;
        m_lastWreckType = 0.0f;
        m_x15C = 0;
    }
    m_buoyancy->m_x38 = 0;
    m_springs->m_x3C = 0;
    for (int i = 0; i < m_numParticlePoints; i++)
        m_particlePoints[i].m_x30 = 0;
    m_x68 = 0;
    m_rider->m_splash->m_particlesEnabled = 0;
}


// ---- src/WaterWrecker/KeyDownHandler.cpp


int WaterWrecker::KeyDownHandler(ProcessNode *node, KeyInfo *key)
{
    return GameObject::KeyDownHandler(node, key);
}


// ---- src/WaterWrecker/AddCollisionPoint.cpp

class SoultreeObject;




void WaterWrecker::AddCollisionPoint(_D3DVECTOR point, SoultreeObject *object)
{
    m_springs->AddSpringCollisionPoint(point, 1, object, 0.0f);
    m_buoyancy->AddHardCollisionPoint(point, object);
}


// ---- src/WaterWrecker/AddBuoyancyPoint.cpp

class SoultreeObject;



void WaterWrecker::AddBuoyancyPoint(_D3DVECTOR point, SoultreeObject *object)
{
    m_buoyancy->AddSoftCollisionPoint(point, object);
}

