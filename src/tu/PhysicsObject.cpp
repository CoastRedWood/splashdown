// PhysicsObject.cpp  @ 0x0029F020 (text 0x29F020-0x2A0250)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's PhysicsObject.cpp as one translation unit: its functions in
// retail order, then its static initializer. `make link` places the
// object's data as well as its code (tools/link_c.py).



#include "GameObject/GameObject.h"
#include "PhysicsBody/PhysicsBody.h"
#include "d3dtypes.h"
#include "PhysicsObject/PhysicsObject.h"
#include "SearchableObject/SearchableObject.h"
#include "SoultreeObject/SoultreeObject.h"
#include "PSXRenderableObject/PSXRenderableObject.h"
#include "PhysicsRigidBody/PhysicsRigidBody.h"
#include "SelectiveGravityModel/SelectiveGravityModel.h"
#include "SpringCollisionDetectionModel/SpringCollisionDetectionModel.h"
#include "SpringCollisionModel/SpringCollisionModel.h"
#include "CollisionObject/CollisionObject.h"
#include "sinit/AxisVectors.h"

// ---- src/global/PhysicsObjectSomethingHitMeCallback.cpp

void PhysicsObjectSomethingHitMeCallback(CollisionObject *me, CollisionObject *)
{
    PhysicsBody *body = (PhysicsBody *)me->m_ownerData;
    if (((PhysicsObject *)me->m_owner)->m_unk30)
        body->SetEnabled(1);
}


// ---- src/global/PhysicsObjectTriggerCallBack.cpp

void PhysicsObjectTriggerCallBack(CollisionObject *, void *, void *userData, int entering)
{
    if (entering)
        ((PhysicsObject *)userData)->m_body->SetEnabled(1);
}


// ---- src/PhysicsObject/ctor.cpp


PhysicsObject::PhysicsObject(int id) : GameObject(id)
{
    m_initialPosition = ZeroVector;
    m_initialForward = ZeroVector;
    m_initialUp = ZeroVector;
    m_object = 0;
    m_unkB4 = 0;
    m_unk30 = 1;
    m_xA8 = 0;
    m_xAC = 0;
}


// ---- src/PhysicsObject/dtor.cpp


PhysicsObject::~PhysicsObject()
{
}


// ---- src/PhysicsObject/Create.cpp
extern "C" char *strcpy(char *dst, const char *src);


extern const unsigned int WORLDOBJECT_PHYSICSBODY;


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

// Builds the physics object for model `name`: its renderable, rigid body,
// gravity, collision rays ("m.col") with spring collision against the
// terrain, and face collision ("f.col").
PhysicsObject *PhysicsObject::Create(_D3DVECTOR position, _D3DVECTOR forward, _D3DVECTOR up, Terrain *terrain,
                                     char *name, SearchNode *node, SearchNode *node2)
{
    char rayFile[0x110];
    char faceFile[0x110];
    // Retail's m_object was a PSXRenderableObject *: passing it as a
    // SoultreeObject * converts it, and the conversion loads it first (the
    // casts below; PhysicsObject.h types it SoultreeObject *).

    GameObject::Create();
    m_initialPosition = position;
    m_initialForward = forward;
    m_initialUp = up;
    m_objectSearchNode = node;
    m_collisionSearchNode = node2;
    m_object = new PSXRenderableObject(1);
    ((PSXRenderableObject *)m_object)->Create(name, 0, 0);
    AddChild(m_object, 0);
    if (m_initialPosition == ZeroVector)
        m_object->GetPosition(&m_initialPosition);
    m_object->SetPosition(ZeroVector);

    m_body = new PhysicsRigidBody(1);
    m_body->Create();
    m_body->SetSoultreeObject(m_object);
    m_body->m_x320 = 1;
    m_body->SetEnabled(0);
    AddChild(m_body, -1);

    m_gravity = new SelectiveGravityModel(1);
    m_gravity->Create(1);
    m_gravity->AddBody(m_body);
    AddChild(m_gravity, 0);

    strcpy(rayFile, name);
    strcpy(FindExtension(rayFile), "m.col");
    m_rayCollision = new CollisionObject(1);
    m_rayCollision->Create(3);
    m_rayCollision->m_ownerType = WORLDOBJECT_PHYSICSBODY;
    m_rayCollision->m_owner = this;
    m_rayCollision->m_ownerData = m_body;
    m_rayCollision->SetTestListType((CollisionObjectTestListType)1);
    m_rayCollision->SetSoultreeCollisionRays((PSXRenderableObject *)m_object, rayFile, 0);
    m_rayCollision->InitializeTransform();
    AddChild(m_rayCollision, 0);

    m_springs = new SpringCollisionModel(1);
    m_springs->Create(0x32);
    m_springs->SetBody(m_body);
    m_springs->AddSpringCollisionPoint(m_rayCollision, 1, (PSXRenderableObject *)m_object);
    m_springs->m_x34 = 1;
    m_springs->m_x5C = 0;
    m_springs->m_xAC = 0;
    AddChild(m_springs, 0);

    m_springDetection = new SpringCollisionDetectionModel(1);
    m_springDetection->Create(m_springs, terrain, 0, 0, ZeroVector, 0);
    m_springDetection->m_x64 = 1;
    m_springDetection->SetCollisionObject(m_rayCollision);
    AddChild(m_springDetection, 0);

    strcpy(faceFile, name);
    strcpy(FindExtension(faceFile), "f.col");
    m_faceCollision = new CollisionObject(1);
    m_faceCollision->Create(5);
    m_faceCollision->m_ownerType = WORLDOBJECT_PHYSICSBODY;
    m_faceCollision->SetSoultreeCollision((PSXRenderableObject *)m_object, faceFile);
    m_faceCollision->m_otherCallback = PhysicsObjectSomethingHitMeCallback;
    m_faceCollision->m_owner = this;
    m_faceCollision->m_ownerData = m_body;
    m_faceCollision->InitializeTransform();
    AddChild(m_faceCollision, 0);
    return this;
}


// ---- src/PhysicsObject/SetPhysicalProperties.cpp


_D3DMATRIX ZeroMatrix();


// Mass from weight (pounds / 32.2), box inertia from the object's bounding
// box, then the collision and friction parameters.
void PhysicsObject::SetPhysicalProperties(float mass, float inertiaScale, float a, float b, float c, float d,
                                          float e, float friction, float g, float h, float i)
{
    _D3DVECTOR min;
    _D3DVECTOR max;
    _D3DMATRIX inertia;

    inertia = ZeroMatrix();
    m_object->m_x190_bit4 = 0;
    m_object->m_x190_bit6 = 0;
    m_object->GetHierarchyBoundingBox(&min, &max);
    m_object->m_x190_bit4 = 1;
    m_body->SetMass(mass / 32.2f);
    inertia = ZeroMatrix();
    inertia._11 = inertiaScale * (m_body->m_mass / 12.0f) * (max.y * max.y + max.z * max.z);
    inertia._22 = inertiaScale * (m_body->m_mass / 12.0f) * (max.x * max.x + max.z * max.z);
    inertia._33 = inertiaScale * (m_body->m_mass / 12.0f) * (max.x * max.x + max.y * max.y);
    inertia._44 = 1.0f;
    m_body->SetInertia(inertia);
    m_springs->SetCollisionParameters(a, b, c, d, e, g, g, g);
    m_springs->SetFriction(0, friction);
    m_xA8 = h;
    m_xAC = i;
}


// ---- src/PhysicsObject/ResetPositionAndOrientation.cpp


void PhysicsObject::ResetPositionAndOrientation()
{
    m_faceCollision->RemoveFromSearch();
    m_rayCollision->RemoveFromSearch();
    m_object->RemoveFromSearch();
    m_object->SetPosition(0, &m_initialPosition);
    m_object->SetOrientation(0, &m_initialForward, &m_initialUp, 0, 1, 0);
    m_faceCollision->UpdateTransform();
    m_rayCollision->UpdateTransform();
    m_faceCollision->AddSearchPlacement(m_collisionSearchNode, 2);
    m_rayCollision->AddSearchPlacement(m_collisionSearchNode, 2);
    m_object->AddSearchPlacement(m_objectSearchNode, 2);
    m_body->Reset();
}


// ---- src/PhysicsObject/Tick.cpp
#pragma inline_depth(6)  // Length -> Magnitude -> Sqrt inside Fmax


static inline float Sqrt(float x)
{
    float r;
    asm("sqrt.s $f8, %1; swc1 $f8, 0(%0)" : : "r"(&r), "f"(x) : "$f8", "memory");
    return r;
}

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

static inline float Fmax(float a, float b)
{
    float r;
    asm("max.s $f8, %1, %2; swc1 $f8, 0(%0)" : : "r"(&r), "f"(a), "f"(b) : "$f8", "memory");
    return r;
}

static inline float Magnitude(const _D3DVECTOR &v)
{
    return Sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

// A second level of inlining: it sets where the Sqrt temporaries sit on
// the stack (after every first-level helper's).
static inline float Length(const _D3DVECTOR &v)
{
    return Magnitude(v);
}

// libvu0 sceVu0Normalize.
static inline void Normalize(_D3DVECTOR *out, _D3DVECTOR *in)
{
    asm("lqc2 vf4, 0x0(%1); vmul.xyz vf5, vf4, vf4; vaddy.x vf5, vf5, vf5y; vaddz.x vf5, vf5, vf5z; vrsqrt Q, vf0w, vf5x; vsub.xyzw vf6, vf0, vf0; vaddw.xyzw vf6, vf6, vf4w; vwaitq; vmulq.xyz vf6, vf4, Q; sqc2 vf6, 0x0(%0)"
        : : "r"(out), "r"(in) : "memory");
}

// Steps the body in fixed sub-steps with drag, gravity and springs,
// tracking the largest force components and accelerations; a body that
// has come to rest is disabled.
int PhysicsObject::Tick(float dt)
{
    static float SimulationRate = 1.0f / 60.0f;
    _D3DVECTOR drag;
    _D3DVECTOR spinDrag;
    float maxAcceleration = 0.0f;
    float maxAngularAcceleration = maxAcceleration;
    if (m_body->m_enabled) {
        m_springDetection->SimulationTick(dt);
        float t = maxAcceleration;
        float step = Fmin(dt, SimulationRate);
        m_unk80 = ZeroVector;
        while (t < dt) {
            m_gravity->SetForceTick(step);
            m_springs->SimulationTick(step);
            Normalize(&drag, &m_body->m_velocity);
            m_body->m_speed = Length(m_body->m_velocity);
            drag = m_body->m_speed * (-m_xA8 * m_body->m_speed) * drag;
            m_body->AddForce(drag);
            spinDrag = -m_xAC * m_body->m_xB0;
            m_body->AddTorque(spinDrag);
            if (Fabs(m_body->m_x1C0.x) > Fabs(m_unk80.x))
                m_unk80.x = m_body->m_x1C0.x;
            if (Fabs(m_body->m_x1C0.y) > Fabs(m_unk80.y))
                m_unk80.y = m_body->m_x1C0.y;
            if (Fabs(m_body->m_x1C0.z) > Fabs(m_unk80.z))
                m_unk80.z = m_body->m_x1C0.z;
            m_body->SimulationTick(step);
            maxAcceleration = Fmax(maxAcceleration, Length(m_body->m_xC0));
            maxAngularAcceleration = Fmax(maxAngularAcceleration, Length(m_body->m_xD0));
            t += step;
            step = Fmin(SimulationRate, dt - t);
        }
        m_rayCollision->UpdateCollisionInfo();
        m_faceCollision->UpdateCollisionInfo();
        static float SpeedThreshold = 1.0f;
        static float AngularRateThreshold = 0.2f;
        static float AccelerationThreshold = 5.0f;
        static float AngularAccelerationThreshold = 0.5f;
        float angularRate = Length(m_body->m_xB0);
        if (m_body->m_speed < SpeedThreshold && angularRate < AngularRateThreshold &&
            maxAcceleration < AccelerationThreshold && maxAngularAcceleration < AngularAccelerationThreshold)
            m_body->SetEnabled(0);
    }
    return GameObject::Tick(dt);
}


// ---- src/sinit/PhysicsObject.cpp
// Static initializer of PhysicsObject.cpp: only the axis vectors from the shared header.

