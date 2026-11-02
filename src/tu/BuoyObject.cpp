// BuoyObject.cpp  @ 0x0030C1A0 (text 0x30C1A0-0x30D860)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's BuoyObject.cpp as one translation unit, the first of five TUs in
// its tu_map row: BuoyObjectSomethingHitMeCallback, then BuoyObject's ten
// functions, then the static initializer (the axis vectors only). Its .rodata
// (0x430AC8-0x430C60) ends with the SearchableGameObject RTTI records;
// JetskiThrillCam.cpp's follow. `make link` places the object's data as well
// as its code (tools/link_c.py).



// Tick's and SimulationTick's per-function files set it (MapRange -> Clamp);
// TU-wide (read at the end of the TU).
#pragma inline_depth(4)

#include "d3dtypes.h"
#include "GameObject/GameObject.h"
#include "SearchableObject/SearchableObject.h"
#include "SearchableGameObject/SearchableGameObject.h"
#include "BuoyObject/BuoyObject.h"
#include "PhysicsBody/PhysicsBody.h"
#include "PhysicsRigidBody/PhysicsRigidBody.h"
#include "SoultreeObject/SoultreeObject.h"
#include "PSXRenderableObject/PSXRenderableObject.h"
#include "CollisionObject/CollisionObject.h"
#include "CollisionTypeRegistry/CollisionTypeRegistry.h"
#include "SearchPlacement/SearchPlacement.h"
#include "Water/Water.h"
#include "PSXWater/PSXWater.h"
#include "Camera/Camera.h"
#include "RenderTarget/RenderTarget.h"
#include "Game/Game.h"
#include "soultree_assert.h"

#include "sinit/AxisVectors.h"

extern "C" int rand();
extern "C" int sprintf(char *buf, const char *fmt, ...);
extern "C" char *strcpy(char *dst, const char *src);

extern char SltGeometryContentDirectory[];
extern unsigned int WORLDOBJECT_PHYSICSBODY;
extern RenderTarget *LpRenderTarget;
extern Game *LpGame;

float gBuoyPositionFilterConstant = 0.2f;
float gBuoyMinOrientationScale = 0.1f;
float gBuoyMaxOrientationScale = 1.5f;
float gBuoyOrientationTimeScale = 0.5f;

_D3DMATRIX ZeroMatrix();
float noise1(float x);

// A buoy group's entry (BuoyObject +0x40): Create records the buoy's model.
#ifdef SPLASHDOWN_PC
#include "SoultreeSoundEmitter/SoultreeSoundEmitter.h"
#endif
struct BuoyGroup {
#ifdef SPLASHDOWN_PC
    // The buoy's SoultreeSoundEmitter: m_object is at +0x220 on the PC (the
    // store at +0x204 overwrote its PSXSound pointer's high half)
    char pad0[__builtin_offsetof(SoultreeSoundEmitter, m_object)];
#else
    char pad0[0x204];
#endif
    PSXRenderableObject *m_object;      // +0x204
};

// The buoy's marker (BuoyObject +0xE4): Tick keeps it 50 below the body.
struct BuoyMarker {
    char pad0[0x40];
    float m_x, m_y, m_z;                // +0x40
};

// Retail's fabs/sqrt/fmax: abs.s/sqrt.s/max.s into $f8, stored through a stack temporary.
static inline float Fabs(float x)
{
    float r;
    asm("abs.s $f8, %1; swc1 $f8, 0(%0)" : : "r"(&r), "f"(x) : "$f8", "memory");
    return r;
}

static inline float Sqrt(float x)
{
    float r;
    asm("sqrt.s $f8, %1; swc1 $f8, 0(%0)" : : "r"(&r), "f"(x) : "$f8", "memory");
    return r;
}

static inline float Fmax(float a, float b)
{
    float r;
    asm("max.s $f8, %1, %2; swc1 $f8, 0(%0)" : : "r"(&r), "f"(a), "f"(b) : "$f8", "memory");
    return r;
}

// ---- src/global/BuoyObjectSomethingHitMeCallback.cpp
// The buoy's collision callback (Create installs it): wake the body and flag
// the hit in the owner's event mask.
void BuoyObjectSomethingHitMeCallback(CollisionObject *me, CollisionObject *)
{
    PhysicsBody *body = (PhysicsBody *)me->m_ownerData;
    ((BuoyObject *)me->m_owner)->m_wantedEvents |= 2;
    body->SetEnabled(1);
}

// ---- src/BuoyObject/ctor.cpp

BuoyObject::BuoyObject(int id) : SearchableGameObject(id)
{
    m_activeDistance = 250.0f;
    m_type = 0;
    m_x44 = 0.0f;
    m_x48 = 0.0f;
    m_x4C = 0.0f;
    m_x50 = 0.0f;
    m_spring = 0.0f;
    m_damping = 0.0f;
    m_height = 0.0f;
    m_x60 = 0.2f;
    m_time = 0.0f;
    m_x68 = 0.0f;
    m_up = YVector;
    m_position = ZeroVector;
    m_forward = ZeroVector;
    m_xC0 = ZeroVector;
    m_object = 0;
    m_collisionType = 0;
    m_group = 0;
    m_placement = 0;
    // Random phases in [0, 5).
    m_noiseOffsetX = 5.0f * (4.656613e-10f * rand());
    m_noiseOffsetZ = 5.0f * (4.656613e-10f * rand());
    m_maxDisplacement = 0.0f;
    m_restHeight = 0.0f;
    m_tiltScale = gBuoyMinOrientationScale;
    m_marker = 0;
}

// ---- src/BuoyObject/dtor.cpp

BuoyObject::~BuoyObject()
{
    if (m_placement) {
        m_placement->Remove(this);
        delete m_placement;
    }
}

// ---- src/BuoyObject/Create.cpp

// An inlined strrchr(name, '.').
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

BuoyObject *BuoyObject::Create(SearchNode *node, SearchNode *collisionNode, LightManager *lights, PSXWater *water,
                               char *name)
{
    char path[0x110];
    GameObject::Create();
    m_water = water;
    m_water->GetGridPointIntersection(&m_position, 0);
    m_object = new PSXRenderableObject(1);
    m_object->Create(name, 0, 0);
    AddChild(m_object, 0);
    if (m_group)
        m_group->m_object = m_object;
    m_body = new PhysicsRigidBody(1);
    m_body->Create();
    m_body->SetSoultreeObject(m_object);
    m_body->m_x320 = 1;
    m_body->SetEnabled(0);
    AddChild(m_body, 0);
    ResetPositionAndOrientation();
    sprintf(path, "%s%s", SltGeometryContentDirectory, name);
    strcpy(FindExtension(path), ".col");
    unsigned int type = LpCollisionTypeRegistry->GetType("BUOY");
    m_collision = new CollisionObject(1);
    m_collision->Create(5);
    m_collision->m_ownerType = m_collisionType | (WORLDOBJECT_PHYSICSBODY | type);
    m_collision->SetSoultreeCollision(m_object, path);
    m_collision->m_owner = this;
    m_collision->m_ownerData = m_body;
    m_collision->m_otherCallback = BuoyObjectSomethingHitMeCallback;
    m_collision->InitializeTransform();
    AddChild(m_collision, 0);
    m_object->AddSearchPlacement(node, 1);
    m_collision->AddSearchPlacement(collisionNode, 1);
    AddSearchPlacement(node, 1);
    m_searchVisible = 1;
    m_addToList = 1;
    SearchableObject::m_hidden = 0;
    return this;
}

// ---- src/BuoyObject/AddSearchPlacement.cpp
#pragma alignlabel off

void BuoyObject::AddSearchPlacement(SearchNode *node, unsigned int flags)
{
#line 176 "BuoyObject.cpp"
    ASSERT(m_placement == 0, "AddSearchPlacement: LpSearchPlacement already exists");
    m_placement = new SearchPlacement;
    _D3DVECTOR center;
    _D3DVECTOR half;
    m_object->GetCenterHalf(&center, &half);
    m_placement->PerformInitialPlacement(this, node, flags, &center, &half, 0.0f);
#line 189 "BuoyObject.cpp"
    ASSERT(m_placement->FirstPlacementItem.LpSearchNode, "AddSearchPlacement: FirstPlacementItem.LpSearchNode == 0");
}

#pragma alignlabel reset

// ---- src/BuoyObject/UpdateSearchPlacement.cpp
void BuoyObject::UpdateSearchPlacement()
{
    m_object->UpdateSearchPlacement();
    m_collision->UpdateCollisionInfo();
}

// ---- src/BuoyObject/SetPhysicalProperties.cpp

void BuoyObject::SetPhysicalProperties(int type, float a, float b, float c, float d,
                                       _D3DVECTOR v0, _D3DVECTOR v1, _D3DVECTOR v2)
{
    m_type = type;
    m_x44 = a;
    m_x48 = b;
    m_x4C = c;
    m_x50 = d;
    m_position = v0;
    m_forward = v1;
    m_xC0 = v2;
}

// ---- src/BuoyObject/ResetPositionAndOrientation.cpp

void BuoyObject::ResetPositionAndOrientation()
{
    m_object->SetPosition(0, &m_position);
    m_object->SetOrientation(0, &m_forward, &m_xC0, 0, 1, 0);
    _D3DVECTOR min;
    _D3DVECTOR max;
    _D3DMATRIX inertia = ZeroMatrix();     // unused in retail too
    m_object->GetHierarchyBoundingBox(&min, &max);
    m_body->SetMass(m_x44 / 32.2f);
    m_body->m_inertia = ZeroMatrix();
    m_body->m_inverseInertia = ZeroMatrix();
    m_spring = Fabs(32.2f * (m_x4C * m_body->m_mass));
    m_damping = 2.0f * m_x50 * Sqrt(m_body->m_mass * m_spring);
    m_body->Reset();
}

// ---- src/BuoyObject/Tick.cpp

static inline float Fmin(float a, float b)
{
    float r;
    asm("min.s $f8, %1, %2; swc1 $f8, 0(%0)" : : "r"(&r), "f"(a), "f"(b) : "$f8", "memory");
    return r;
}

// Profiling IDs are compiled out in retail.
static inline int GetProfileID() { return -1; }

// Pulls the buoy towards its anchor on the water surface with a damped
// spring, and puts it to sleep once it has settled.
int BuoyObject::Tick(float dt)
{
    static int BuoyObjectID = GetProfileID();
    _D3DVECTOR position;
    _D3DVECTOR water;
    if (m_body->m_enabled) {
        m_object->GetPosition(0, &position);
        water = position;
        m_water->GetGridPointIntersection(&water, 0);
        _D3DVECTOR target;
        target.x = m_position.x;
        target.y = water.y;
        target.z = m_position.z;
        _D3DVECTOR delta = target - position;
        m_body->m_force += m_spring * delta - m_damping * m_body->m_velocity;
        m_body->m_xC0 = m_body->m_inverseMass * m_body->m_force;
        m_body->m_velocity += dt * m_body->m_xC0;
        position += dt * m_body->m_velocity;
        position.y = Fmin(position.y, water.y);
        m_object->SetPosition(0, &position);
        m_body->m_lastVelocity = m_body->m_velocity;
        m_body->m_xB0 = ZeroVector;
        m_body->m_xA0 = ZeroVector;
        m_body->m_force = ZeroVector;
        m_body->m_torque = ZeroVector;
        m_body->m_x1C0 = ZeroVector;
        m_body->m_x1D0 = ZeroVector;
        static float SpeedThresholdSquared = 0.1f * 0.1f;
        static float DistanceThresholdSquared = 0.1f * 0.1f;
        PhysicsRigidBody *body = m_body;
        float dx = delta.x;
        float dz = delta.z;
        float speedSquared = body->m_velocity.z * body->m_velocity.z + body->m_velocity.x * body->m_velocity.x;
        float distanceSquared = dz * dz + dx * dx;
        if (speedSquared < SpeedThresholdSquared && distanceSquared < DistanceThresholdSquared) {
            body->Reset();
            m_body->SetEnabled(0);
            m_wantedEvents = 0;
        }
        UpdateSearchPlacement();
        if (m_marker) {
            m_marker->m_x = m_body->m_object->m_worldMatrix._41;
            m_marker->m_y = m_body->m_object->m_worldMatrix._42 - 50.0f;
            m_marker->m_z = m_body->m_object->m_worldMatrix._43;
        }
    }
    return GameObject::Tick(dt);
}

// ---- src/BuoyObject/PrepareGeometry.cpp

// Buoys near the camera keep simulating while they're drawn.
int BuoyObject::PrepareGeometry()
{
    if (LpGame->m_paused)
        return GameObject::PrepareGeometry();
    static int BuoyObjectPrepGeomID = -1;
    _D3DVECTOR position;
    _D3DVECTOR offset;
    m_object->GetPosition(0, &position);
    offset = position - LpRenderTarget->m_camera->m_position;
    float distance = Sqrt(offset.x * offset.x + offset.y * offset.y + offset.z * offset.z);
    if (distance < m_activeDistance)
        SimulationTick(LpGame->m_simulationStep);
    return GameObject::PrepareGeometry();
}

// ---- src/BuoyObject/SimulationTick.cpp
// Nested inlines (MapRange -> Clamp) need a deeper inline depth than
// mwcc's default.

// Clamp x between a and b, in either order.
static inline float Clamp(float x, float a, float b)
{
    if (b > a) {
        if (x > b)
            x = b;
        else if (x < a)
            x = a;
    } else {
        if (x < b)
            x = b;
        else if (x > a)
            x = a;
    }
    return x;
}

// Map x from [x0, x1] to [y0, y1], clamped.
static inline float MapRange(float x, float x0, float x1, float y0, float y1)
{
    return Clamp(y0 + (x - x0) * (y1 - y0) / (x1 - x0), y0, y1);
}

// libvu0 sceVu0Normalize.
static inline void Vu0Normalize(_D3DVECTOR *out, _D3DVECTOR *in)
{
    asm("lqc2 vf4, 0x0(%1); vmul.xyz vf5, vf4, vf4; vaddy.x vf5, vf5, vf5y; vaddz.x vf5, vf5, vf5z; vrsqrt Q, vf0w, vf5x; vsub.xyzw vf6, vf0, vf0; vaddw.xyzw vf6, vf6, vf4w; vwaitq; vmulq.xyz vf6, vf4, Q; sqc2 vf6, 0x0(%0)"
        : : "r"(out), "r"(in) : "memory");
}

// VU0 cross product, out = a x b.
static inline void CrossProduct(_D3DVECTOR &out, const _D3DVECTOR &a, const _D3DVECTOR &b)
{
    asm("lqc2 vf4, 0x0(%1); lqc2 vf5, 0x0(%2); vopmula.xyz ACC, vf4, vf5; vopmsub.xyz vf6, vf5, vf4; vsub.w vf6, vf6, vf6; sqc2 vf6, 0x0(%0)"
        : : "r"(&out), "r"(&a), "r"(&b) : "memory");
}

// Float the buoy on the water, and for the first five seconds tilt it by
// noise scaled to how far it has been displaced.
int BuoyObject::SimulationTick(float dt)
{
    _D3DVECTOR position;
    _D3DVECTOR water;
    _D3DVECTOR forward;
    _D3DVECTOR up;
    _D3DVECTOR side;
    m_object->GetPosition(0, &position);
    water = position;
    m_water->GetGridPointIntersection(&water, 0);
    if (m_height == 0.0f) {
        m_height = water.y;
        m_restHeight = water.y;
    }
    float k = gBuoyPositionFilterConstant;
    m_height = (1.0f - k) * m_height + k * water.y;
    if (m_type) {
        if ((m_time += dt) < 5.0f) {
            m_maxDisplacement = Fmax(m_maxDisplacement, Fabs(water.y - m_restHeight));
            m_tiltScale = MapRange(m_maxDisplacement, 0.0f, 5.0f, gBuoyMinOrientationScale, gBuoyMaxOrientationScale);
        }
        up.x = m_tiltScale * noise1(m_noiseOffsetX + gBuoyOrientationTimeScale * m_time);
        up.y = 1.0f;
        up.z = m_tiltScale * noise1(m_noiseOffsetZ + gBuoyOrientationTimeScale * m_time);
        Vu0Normalize(&up, &up);
        CrossProduct(side, up, m_forward);
        CrossProduct(forward, side, up);
        Vu0Normalize(&forward, &forward);
        m_object->SetOrientation(0, &forward, &up, 0, 1, 0);
    }
    if (!m_body->m_enabled) {
        water.y = m_height;
        m_object->SetPosition(0, &water);
    }
    return 1;
}

// ---- src/sinit/BuoyObject.cpp
// Static initializer of BuoyObject.cpp: only the axis vectors from the shared header.
