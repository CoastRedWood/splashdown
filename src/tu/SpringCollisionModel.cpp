// SpringCollisionModel.cpp  @ 0x002EE7F0 (text 0x2EE7F0-0x2F2110)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's SpringCollisionModel.cpp as one translation unit: its functions in
// retail order, then its static initializer. `make link` places the object's data
// as well as its code (tools/link_c.py).



#include "d3dtypes.h"
#include "GameObject/GameObject.h"
#include "SpringCollisionModel/SpringCollisionModel.h"
#include "PhysicsBody/PhysicsBody.h"
#include "SoultreeCollisionObject/SoultreeCollisionObject.h"
#include "CollisionObject/CollisionObject.h"
#include "SoultreeObject/SoultreeObject.h"
#include "BoundingBoxTree/BoundingBoxTree.h"
#include "sinit/AxisVectors.h"

// ---- src/SpringCollisionModel/ctor.cpp


SpringCollisionModel::SpringCollisionModel(int enabled) : GameObject(enabled)
{
    m_numSprings = 0;
    m_numContacts = 0;
    m_maxContacts = 0;
    m_body = 0;
    m_springs = 0;
    m_contacts = 0;
    m_hitObject = 0;
    m_x34 = 0;
    m_x3C = 0;
    m_x38 = 0;
    m_useCenter = 0;
    m_groundCollision = 1;
    m_x4C = 1;
    m_x50 = 0;
    m_x54 = 1;
    m_x58 = 0;
    m_x5C = 1;
    m_x60 = 0;
    m_yawOnly = 0;
    m_x6C = 0;
    m_x70 = 0;
    m_x40 = 0;
    m_x44 = 0;
    m_friction = 0;
    m_x8C = 10000.0f;
    m_x90 = 0.15f;
    m_stiffness = 0;
    m_damping = 0;
    m_stiffness2 = 0;
    m_damping2 = 0;
    m_xA4 = 1.0f;
    m_xA8 = 1.0f;
    m_xAC = 1.0f;
    m_xBC = 1.0f;
    m_xC4 = 0.07f;
    m_xC8 = 0.07f;
    m_center = ZeroVector;
    for (int i = 0; i < 10; i++)
        m_frictions[i] = 0;
    m_surfaceType = 0;
    m_xB0 = 0;
    m_xB8 = 0;
    m_xB4 = 0;
}


// ---- src/SpringCollisionModel/dtor.cpp
extern "C" void free(void *p);


SpringCollisionModel::~SpringCollisionModel()
{
    if (m_springs)
        free(m_springs);
    if (m_contacts)
        free(m_contacts);
}


// ---- src/SpringCollisionModel/Create.cpp
extern "C" void *malloc(unsigned int size);


void SpringCollisionModel::Create(int maxContacts)
{
    m_maxContacts = maxContacts;
    m_springs = (SpringCollisionPoint *)malloc(m_maxContacts * sizeof(SpringCollisionPoint));
    m_contacts = (ObjectCollisionPoint *)malloc(m_maxContacts * sizeof(ObjectCollisionPoint));
    GameObject::Create();
}


// ---- src/SpringCollisionModel/ComputeStiffness.cpp
// Retail's fabs is an inline-asm helper: abs.s into $f8, stored through a
// pointer to a stack temporary (the shape of nearly all 274 abs.s in retail).
inline float Fabs(float x)
{
    float r;
    asm("abs.s $f8, %1; swc1 $f8, 0(%0)" : : "r"(&r), "f"(x) : "$f8", "memory");
    return r;
}


float SpringCollisionModel::ComputeStiffness(float a, float b)
{
    return Fabs(32.2f * (a * b));
}


// ---- src/SpringCollisionModel/ComputeDamping.cpp
// Retail's sqrt is an inline-asm helper (same shape as its fabs): sqrt.s into
// $f8, stored through a pointer to a stack temporary.
inline float Sqrt(float x)
{
    float r;
    asm("sqrt.s $f8, %1; swc1 $f8, 0(%0)" : : "r"(&r), "f"(x) : "$f8", "memory");
    return r;
}


float SpringCollisionModel::ComputeDamping(float ratio, float stiffness, float mass)
{
    if (stiffness > 0.0f && mass > 0.0f)
        return 2.0f * ratio * Sqrt(stiffness * mass);
    return 0.0f;
}


// ---- src/SpringCollisionModel/SetBody_002EEAE0.cpp


void SpringCollisionModel::SetBody(PhysicsBody *body)
{
    m_body = body;
}


// ---- src/SpringCollisionModel/SetCollisionParameters.cpp


void SpringCollisionModel::SetCollisionParameters(float friction, float stiffness, float damping, float stiffness2,
                                                  float damping2, float b0, float b4, float b8)
{
    if (m_body) {
        m_friction = friction;
        m_stiffness = ComputeStiffness(stiffness, m_body->m_mass);
        m_damping = ComputeDamping(damping, m_body->m_mass, m_stiffness);
        m_stiffness2 = ComputeStiffness(stiffness2, m_body->m_mass);
        m_damping2 = ComputeDamping(damping2, m_body->m_mass, m_stiffness2);
        m_xB0 = b0;
        m_xB4 = b4;
        m_xB8 = b8;
    }
}


// ---- src/SpringCollisionModel/SetFriction.cpp

void SpringCollisionModel::SetFriction(int i, float friction)
{
    if (i >= 0 && i < 10)
        m_frictions[i] = friction;
}


// ---- src/SpringCollisionModel/AddSpringCollisionPoint_002EEC10.cpp


// Adds a spring contact at point on object unless it is already registered.
int SpringCollisionModel::AddSpringCollisionPoint(_D3DVECTOR point, int id, SoultreeObject *object, float stiffness)
{
    if (m_numSprings < m_maxContacts) {
        int found = 0;
        for (int i = 0; i < m_numSprings; i++) {
            if (point == m_springs[i].m_point && object == m_springs[i].m_object)
                found = 1;
        }
        if (!found) {
            m_springs[m_numSprings].m_object = object;
            m_springs[m_numSprings].m_point = point;
            m_springs[m_numSprings].m_id = id;
            m_springs[m_numSprings].m_stiffness = stiffness;
            m_springs[m_numSprings].m_groundNormal = ZeroVector;
            m_springs[m_numSprings].m_worldPosition = ZeroVector;
            m_numSprings++;
        }
    }
}


// ---- src/SpringCollisionModel/AddSpringCollisionPoint.cpp
int SpringCollisionModel::AddSpringCollisionPoint(BoundingBoxTree *tree, int flags, SoultreeObject *obj)
{
    if (tree->m_size < 0.0f)
        return AddSpringCollisionPoint(tree->m_max, flags, obj, 0.0f);
    AddSpringCollisionPoint(tree->m_left, flags, obj);
    return AddSpringCollisionPoint(tree->m_right, flags, obj);
}


// ---- src/SpringCollisionModel/AddSpringCollisionPoint_002EEED0.cpp


// m_collisionData of a COLLISION_RAYS object. Declared here: the shared
// declarations (global/CollisionTree.h, CollisionObject/vu0col.h) clash with
// this TU's SoultreeCollisionObject.h and CollisionObject.h.
struct RayCollisionObject {
    int m_x00;
    BoundingBoxTree *m_tree;    // +0x04
};

void SpringCollisionModel::AddSpringCollisionPoint(CollisionObject *object, int flags, SoultreeObject *obj)
{
    if (object) {
        switch (object->m_collisionType) {
        case COLLISION_SOULTREE: {
            int i;
            SoultreeCollisionObject *soultree = (SoultreeCollisionObject *)object->m_collisionData;
            for (i = 0; i < soultree->m_numVertices; i++)
                AddSpringCollisionPoint(soultree->m_vertices[i], flags, obj, 0.0f);
            break;
        }
        case COLLISION_RAYS:
            AddSpringCollisionPoint(((RayCollisionObject *)object->m_collisionData)->m_tree, flags, obj);
            break;
        case COLLISION_SPHERE: {
            SphereCollisionObject *sphere = (SphereCollisionObject *)object->m_collisionData;
            AddSpringCollisionPoint(sphere->center, flags, obj, sphere->radius);
            break;
        }
        }
    }
}


// ---- src/SpringCollisionModel/AddObjectCollisionPoint.cpp


void SpringCollisionModel::AddObjectCollisionPoint(_D3DVECTOR point, _D3DVECTOR normal, _D3DVECTOR a, _D3DVECTOR b,
                                                   PhysicsBody *body, SoultreeObject *object,
                                                   CollisionObject *collision, int id)
{
    if (m_numContacts < m_maxContacts) {
        ObjectCollisionPoint *contact = &m_contacts[m_numContacts];
        contact->m_point = point;
        contact->m_normal = normal;
        contact->m_x30 = a;
        contact->m_x40 = b;
        contact->m_body = body;
        contact->m_object = object;
        contact->m_collision = collision;
        contact->m_x64 = id;
        m_numContacts++;
        m_surfaceType = id;
    }
}


// ---- src/SpringCollisionModel/ResetObjectCollisions.cpp

void SpringCollisionModel::ResetObjectCollisions()
{
    m_numContacts = 0;
    m_x38 = 1;
    m_hitObject = 0;
    m_x3C = 0;
    m_x6C = 0;
    m_x70 = 0;
    m_x40 = 0;
    m_x44 = 0;
}


// ---- src/SpringCollisionModel/SimulationTick.cpp

// out = m * in (point: w = 1), libvu0 sceVu0ApplyMatrix plus the translation row.
static inline void TransformPoint(_D3DVECTOR *out, _D3DMATRIX *m, _D3DVECTOR *in)
{
    asm("lqc2 vf4, 0x0(%1); lqc2 vf5, 0x10(%1); lqc2 vf6, 0x20(%1); lqc2 vf7, 0x30(%1); lqc2 vf8, 0x0(%2); vmulw.w vf9, vf0, vf0w; vmulax.xyz ACC, vf4, vf8x; vmadday.xyz ACC, vf5, vf8y; vmaddz.xyz vf9, vf6, vf8z; vadd.xyz vf9, vf9, vf7; sqc2 vf9, 0x0(%0)"
        : : "r"(out), "r"(m), "r"(in) : "memory");
}

// xyz dot product on VU0.
static inline float DotProduct(_D3DVECTOR *a, _D3DVECTOR *b)
{
    float r;
    asm("lqc2 vf4, 0x0(%1); lqc2 vf5, 0x0(%2); vmul.xyz vf5, vf4, vf5; vaddy.x vf5, vf5, vf5y; vaddz.x vf5, vf5, vf5z; qmfc2.ni $2, vf5; mtc1 $2, %0"
        : "=f"(r) : "r"(a), "r"(b) : "$2");
    return r;
}

int SpringCollisionModel::SimulationTick(float dt)
{
    _D3DVECTOR world, offset, velocity;
    int i;
    if (m_groundCollision || m_numContacts > 0) {
        if (m_useCenter)
            TransformPoint(&m_x110, &m_body->m_object->m_worldMatrix, &m_center);
        else
        {
            PhysicsBody *body = m_body;
            TransformPoint(&m_x110, &body->m_object->m_worldMatrix, &body->m_centerOfMass);
        }
    }
    static int GroundCollisionID = -1;  // unused profiler ID
    if (m_groundCollision) {
        SpringCollisionPoint *spring = m_springs;
        for (i = 0; i < m_numSprings; i++) {
            TransformPoint(&world, &spring->m_object->m_worldMatrix, &spring->m_point);
            offset = spring->m_worldPosition - world;
            velocity = spring->m_worldPosition;
            float depth = spring->m_stiffness + DotProduct(&offset, &spring->m_groundNormal);
            if (depth > 0.0f) {
                m_x3C = 1;
                m_x6C = 1;
                spring->m_colliding = 1;
                if (m_x54)
                    CalculateImpulseResponse(depth, dt, spring->m_groundNormal, world, 0);
                if (m_x60)
                    RemoveNormalVelocity(spring->m_groundNormal, velocity);
                if (m_x4C) {
                    if (m_x58 && !(depth < m_friction) && !m_x54)
                        RemoveNormalVelocity(spring->m_groundNormal, velocity);
                    CalculateCollisionResponse(spring->m_id, dt, m_frictions[spring->m_surfaceType], depth,
                                               spring->m_groundNormal, world, 0);
                }
            } else {
                spring->m_colliding = 0;
            }
            spring++;
        }
    }
    static int ObjectCollisionID = -1;  // unused profiler ID
    ObjectCollisionPoint *contact = m_contacts;
    int type = 1;
    for (i = 0; i < m_numContacts; i++) {
        if (contact->m_object)
            TransformPoint(&world, &contact->m_object->m_worldMatrix, &contact->m_normal);
        else
            TransformPoint(&world, &m_body->m_object->m_worldMatrix, &contact->m_normal);
        offset = contact->m_x40 - world;
        velocity = world + offset;
        float depth = DotProduct(&offset, &contact->m_x30);
        if (depth > 0.0f) {
            m_x3C = 1;
            m_x70 = 1;
            contact->m_colliding = 1;
            if (contact->m_body && m_x50) {
                CalculateCollisionResponse(type, m_frictions[contact->m_x64], dt, depth, contact->m_x30, world,
                                           contact->m_body);
            } else {
                if (m_x54)
                    CalculateImpulseResponse(depth, dt, contact->m_x30, world, contact->m_body);
                if (m_x60)
                    RemoveNormalVelocity(contact->m_x30, velocity);
                if (m_x4C) {
                    if (m_x58 && !(depth < m_friction) && !m_x54)
                        RemoveNormalVelocity(contact->m_x30, velocity);
                    CalculateCollisionResponse(type, dt, m_frictions[contact->m_x64], depth, contact->m_x30, world,
                                               contact->m_body);
                }
            }
        } else {
            contact->m_colliding = 0;
        }
        contact++;
    }
    m_x38 = 0;
    return 1;
}


// ---- src/SpringCollisionModel/ApplyForceToBody.cpp


// VU0 cross product, out = a x b.
static inline void CrossProduct(_D3DVECTOR &out, const _D3DVECTOR &a, const _D3DVECTOR &b)
{
    asm("lqc2 vf4, 0x0(%1); lqc2 vf5, 0x0(%2); vopmula.xyz ACC, vf4, vf5; vopmsub.xyz vf6, vf5, vf4; vsub.w vf6, vf6, vf6; sqc2 vf6, 0x0(%0)"
        : : "r"(&out), "r"(&a), "r"(&b) : "memory");
}

// out = transpose(rotation of m) * v: a world direction into m's space.
static inline void RotateWorldToLocal(_D3DVECTOR *out, _D3DMATRIX *m, _D3DVECTOR v)
{
    asm("lqc2 vf1, 0x0(%1); lqc2 vf2, 0x10(%1); lqc2 vf3, 0x20(%1); lqc2 vf4, 0x30(%1); vmulw.w vf21, vf0, vf0w; vsub.xyzw vf30, vf30, vf30; vadd.xyzw vf5, vf1, vf30; vadd.xyzw vf6, vf2, vf30; vadd.xyzw vf7, vf3, vf30; vadd.xyzw vf8, vf4, vf30; vaddy.x vf6, vf0, vf1y; vaddz.x vf7, vf0, vf1z; vaddz.y vf7, vf0, vf2z; vaddx.y vf5, vf0, vf2x; vaddx.z vf5, vf0, vf3x; vaddy.z vf6, vf0, vf3y; lqc2 vf20, 0x0(%2); vmulax.xyz ACC, vf5, vf20x; vmadday.xyz ACC, vf6, vf20y; vmaddz.xyz vf21, vf7, vf20z; sqc2 vf21, 0x0(%0)"
        : : "r"(out), "r"(m), "r"(&v) : "memory");
}

// out = inverse(m) * v for a rigid m: a world point into m's space.
static inline void TransformWorldToLocal(_D3DVECTOR *out, _D3DMATRIX *m, _D3DVECTOR v)
{
    asm("lqc2 vf1, 0x0(%1); lqc2 vf2, 0x10(%1); lqc2 vf3, 0x20(%1); lqc2 vf4, 0x30(%1); vmulw.w vf21, vf0, vf0w; vsub.xyzw vf30, vf30, vf30; vadd.xyzw vf5, vf1, vf30; vadd.xyzw vf6, vf2, vf30; vadd.xyzw vf7, vf3, vf30; vadd.xyzw vf8, vf4, vf30; vaddy.x vf6, vf0, vf1y; vaddz.x vf7, vf0, vf1z; vaddz.y vf7, vf0, vf2z; vaddx.y vf5, vf0, vf2x; vaddx.z vf5, vf0, vf3x; vaddy.z vf6, vf0, vf3y; lqc2 vf20, 0x0(%2); vsub.xyz vf20, vf20, vf8; vmulax.xyz ACC, vf5, vf20x; vmadday.xyz ACC, vf6, vf20y; vmaddz.xyz vf21, vf7, vf20z; sqc2 vf21, 0x0(%0)"
        : : "r"(out), "r"(m), "r"(&v) : "memory");
}


// Push body with force applied at point (both world space).
void SpringCollisionModel::ApplyForceToBody(PhysicsBody *body, _D3DVECTOR force, _D3DVECTOR point, float scale)
{
    body->AddForce(force);
    if (point == ZeroVector)
        return;
    _D3DVECTOR localForce;
    _D3DVECTOR localPoint;
    RotateWorldToLocal(&localForce, &body->m_object->m_worldMatrix, force);
    TransformWorldToLocal(&localPoint, &body->m_object->m_worldMatrix, point);
    if (m_useCenter)
        localPoint = localPoint - m_center;
    else
        localPoint = localPoint - m_body->m_centerOfMass;
    _D3DVECTOR torque;
    CrossProduct(torque, localPoint, localForce);
    if (m_yawOnly) {
        torque.x = 0.0f;
        torque.z = 0.0f;
    }
    body->AddTorque(scale * torque);
}


// ---- src/SpringCollisionModel/CalculateCollisionResponse.cpp


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

inline float RSqrt(float x)
{
    float r;
    asm("rsqrt.s $f8, %0, %1; swc1 $f8, 0(%2)" : : "f"(1.0f), "f"(x), "r"(&r) : "$f8", "memory");
    return r;
}

inline float Magnitude(const _D3DVECTOR &v)
{
    float len2 = v.x * v.x + v.y * v.y + v.z * v.z;
    if (len2 == 0.0f)
        return 0.0f;
    return 1.0f / RSqrt(len2);
}

inline _D3DVECTOR operator-(const _D3DVECTOR &v)
{
    return _D3DVECTOR(-v.x, -v.y, -v.z);
}

// out = m * in (point: w = 1), libvu0 sceVu0ApplyMatrix plus the translation row.

// xyz dot product on VU0.

// libvu0 sceVu0Normalize.
static inline void Normalize(_D3DVECTOR *out, _D3DVECTOR *in)
{
    asm("lqc2 vf4, 0x0(%1); vmul.xyz vf5, vf4, vf4; vaddy.x vf5, vf5, vf5y; vaddz.x vf5, vf5, vf5z; vrsqrt Q, vf0w, vf5x; vsub.xyzw vf6, vf0, vf0; vaddw.xyzw vf6, vf6, vf4w; vwaitq; vmulq.xyz vf6, vf4, Q; sqc2 vf6, 0x0(%0)"
        : : "r"(out), "r"(in) : "memory");
}

// libvu0 sceVu0OuterProduct: out = a x b.
static inline void CrossProduct(_D3DVECTOR *out, const _D3DVECTOR &a, _D3DVECTOR *b)
{
    asm("lqc2 vf4, 0x0(%1); lqc2 vf5, 0x0(%2); vopmula.xyz ACC, vf4, vf5; vopmsub.xyz vf6, vf5, vf4; vsub.w vf6, vf6, vf6; sqc2 vf6, 0x0(%0)"
        : : "r"(out), "r"(&a), "r"(b) : "memory");
}

void SpringCollisionModel::CalculateCollisionResponse(int type, float dt, float friction, float depth,
                                                      _D3DVECTOR normal, _D3DVECTOR point, PhysicsBody *other)
{
    _D3DVECTOR offset, spin, velocity, otherVelocity, otherCenter, otherOffset, relative, force, normalVelocity,
        tangentVelocity, tangent, frictionForce;
    if (depth <= 0.0f)
        return;
    depth = Fmin(depth, m_friction);
    float massScale;
    float stiffness, damping;
    if (m_x54 && (!other || !m_x50)) {
        stiffness = m_stiffness2;
        damping = m_damping2;
    } else {
        stiffness = m_stiffness;
        damping = m_damping;
    }
    if (other) {
        if (!(m_body->m_mass <= other->m_mass))
            massScale = m_body->m_inverseMass * other->m_mass;
        else
            massScale = m_body->m_mass * other->m_inverseMass;
    } else {
        massScale = 1.0f;
    }
    float spring = stiffness * depth;
    offset = point - m_x110;
    CrossProduct(&spin, m_body->m_object->RotateVectorLocalToWorld(&m_body->m_xB0), &offset);
    velocity = m_body->m_velocity + spin;
    if (!other) {
        otherVelocity = ZeroVector;
        if (!(normal.y <= 0.8f))
            m_x40 = 1;
    } else {
        TransformPoint(&otherCenter, &other->m_object->m_worldMatrix, &other->m_centerOfMass);
        otherOffset = point - otherCenter;
        CrossProduct(&spin, other->m_object->RotateVectorLocalToWorld(&other->m_xB0), &otherOffset);
        otherVelocity = other->m_velocity + spin;
        m_x44 = 1;
    }
    relative = velocity - otherVelocity;
    float speed = DotProduct(&relative, &normal);
    speed = Fmin(Fmax(speed, -m_x8C), m_x8C);
    spring = Fmax(spring - damping * speed, 0.0f);
    force = massScale * spring * normal;
    ApplyForceToBody(m_body, force, point, m_xAC);
    if (other)
        ApplyForceToBody(other, -force, point, m_xAC);
    if (type) {
        normalVelocity = speed * normal;
        tangentVelocity = relative - normalVelocity;
        Normalize(&tangent, &tangentVelocity);
        tangent = -tangent;
        float scale = Fmin(0.1f * Magnitude(tangentVelocity), 1.0f);
        frictionForce = 32.2f * (m_body->m_mass * (scale * friction)) * tangent;
        ApplyForceToBody(m_body, frictionForce, point, m_xBC);
        if (other)
            ApplyForceToBody(other, -frictionForce, point, m_xBC);
    }
}


// ---- src/SpringCollisionModel/CalculateImpulseResponse.cpp
// CalculateImpulseResponse__20SpringCollisionModelFff10_D3DVECTOR10_D3DVECTORP11PhysicsBody  @ 0x002F03A0
// Contact impulse between the model's body and an optional other body: a restitution impulse along
// the normal when the bodies close, a force cancelling the closing acceleration, and (m_x5C) a
// positional correction on the bodies' +0xE0/+0xF0 vectors. VU0 asm helpers: optimizer off.
// inline_depth(6) keeps the file-local operator- and the _D3DVECTOR constructor inline. All 43
// vectors are declared up front: retail default-constructs them in the prologue, in address order.
#pragma inline_depth(6)


// out = transpose(rotation of m) * v: a world direction into m's space.

// out = m * in (point: w = 1), libvu0 sceVu0ApplyMatrix plus the translation row.

// libvu0 sceVu0OuterProduct: out = a x b.
static inline void CrossProduct(_D3DVECTOR *out, _D3DVECTOR *a, _D3DVECTOR *b)
{
    asm("lqc2 vf4, 0x0(%1); lqc2 vf5, 0x0(%2); vopmula.xyz ACC, vf4, vf5; vopmsub.xyz vf6, vf5, vf4; vsub.w vf6, vf6, vf6; sqc2 vf6, 0x0(%0)"
        : : "r"(out), "r"(a), "r"(b) : "memory");
}

// xyz dot product on VU0.
static inline float DotProduct(const _D3DVECTOR &a, const _D3DVECTOR &b)
{
    float r;
    asm("lqc2 vf4, 0x0(%1); lqc2 vf5, 0x0(%2); vmul.xyz vf5, vf4, vf5; vaddy.x vf5, vf5, vf5y; vaddz.x vf5, vf5, vf5z; qmfc2.ni $2, vf5; mtc1 $2, %0"
        : "=f"(r) : "r"(&a), "r"(&b) : "$2");
    return r;
}

// m (3x3) * v, w = 1: an inverse inertia applied to a torque. The vector comes first, in the
// parameters and the asm operands: that is the evaluation order retail has for both a loaded
// (m_body) and a register (other) body pointer.
static inline _D3DVECTOR ApplyInertia(_D3DVECTOR *v, _D3DMATRIX *m)
{
    _D3DVECTOR r;
    asm("lqc2 vf4, 0x0(%2); lqc2 vf5, 0x10(%2); lqc2 vf6, 0x20(%2); lqc2 vf8, 0x0(%1); vmulax.xyz ACC, vf4, vf8x; vmadday.xyz ACC, vf5, vf8y; vmaddz.xyz vf9, vf6, vf8z; vadd.xyzw vf9, vf9, vf0; sqc2 vf9, 0x0(%0)"
        : : "r"(&r), "r"(v), "r"(m) : "memory");
    return r;
}


static inline void AddLinearImpulse(PhysicsBody *body, _D3DVECTOR dv)
{
    body->m_x1C0 += dv;
    body->m_velocity += dv;
}

static inline void AddAngularImpulse(PhysicsBody *body, _D3DVECTOR dw)
{
    body->m_x1D0 += dw;
    body->m_xB0 += dw;
}

static inline void AddLinearCorrection(PhysicsBody *body, _D3DVECTOR dv)
{
    body->m_xE0 += dv;
}

static inline void AddAngularCorrection(PhysicsBody *body, _D3DVECTOR dw)
{
    body->m_xF0 += dw;
}

int SpringCollisionModel::CalculateImpulseResponse(float depth, float dt, _D3DVECTOR normal, _D3DVECTOR point,
                                                   PhysicsBody *other)
{
    _D3DVECTOR angular, cross, angularCross, otherAngular, otherCross, offset, localOffset, otherOffset,
        otherLocalOffset, otherCenter, linearAccel, accel, otherAccel, unused130, angularAccel, unused150,
        spinAxis, spin, force, spinVelocity, velocity, otherVelocity, rate, otherRate, relative, localNormal,
        otherLocalNormal, impulse, deltaV, localImpulse, torque, deltaW, correctionRate, correctionVelocity,
        otherCorrectionVelocity, correctionSpin, otherCorrectionSpin, error, correction, deltaV2, localCorrection,
        torque2, deltaW2;
    float k;
    float restitution;
    if (other) {
        if (other->m_mass > 1000.0f)
            restitution = 0.25f;
        else
            restitution = m_xB8;
    } else {
        if (normal.y > 0.8f)
            restitution = m_xB4;
        else
            restitution = m_xB0;
    }

    offset = point - m_x110;
    RotateWorldToLocal(&localOffset, &m_body->m_object->m_worldMatrix, offset);
    if (other) {
        TransformPoint(&otherCenter, &other->m_object->m_worldMatrix, &other->m_centerOfMass);
        otherOffset = point - otherCenter;
        RotateWorldToLocal(&otherLocalOffset, &other->m_object->m_worldMatrix, otherOffset);
    }

    float inverseMass = m_body->m_inverseMass;
    RotateWorldToLocal(&localNormal, &m_body->m_object->m_worldMatrix, normal);
    CrossProduct(&cross, &localOffset, &localNormal);
    angular = ApplyInertia(&cross, &m_body->m_inverseInertia);
    CrossProduct(&angularCross, &angular, &localOffset);
    float angularTerm = DotProduct(localNormal, angularCross);
    float otherInverseMass, otherAngularTerm;
    if (other) {
        otherInverseMass = other->m_inverseMass;
        RotateWorldToLocal(&otherLocalNormal, &other->m_object->m_worldMatrix, normal);
        CrossProduct(&otherCross, &otherLocalOffset, &otherLocalNormal);
        otherAngular = ApplyInertia(&otherCross, &other->m_inverseInertia);
        CrossProduct(&angularCross, &otherAngular, &otherLocalOffset);
        otherAngularTerm = DotProduct(otherLocalNormal, angularCross);
    } else {
        otherLocalNormal = ZeroVector;
        otherAngularTerm = otherInverseMass = 0.0f;
    }
    k = 1.0f / (inverseMass + angularTerm + otherInverseMass + otherAngularTerm);

    CrossProduct(&rate, &m_body->m_xB0, &localOffset);
    spinVelocity = m_body->m_object->RotateVectorLocalToWorld(&rate);
    velocity = m_body->m_velocity + spinVelocity;
    if (other) {
        CrossProduct(&otherRate, &other->m_xB0, &otherLocalOffset);
        spinVelocity = other->m_object->RotateVectorLocalToWorld(&otherRate);
        otherVelocity = other->m_velocity + spinVelocity;
    } else {
        otherVelocity = ZeroVector;
    }
    relative = velocity - otherVelocity;
    float speed = DotProduct(relative, normal);
    if (speed < 0.0f) {
        float j = k * (-(1.0f + restitution) * speed);
        impulse = j * normal;
        deltaV = m_body->m_inverseMass * impulse;
        AddLinearImpulse(m_body, deltaV);
        localImpulse = m_body->m_object->RotateVectorWorldToLocal(&impulse);
        CrossProduct(&torque, &localOffset, &localImpulse);
        deltaW = ApplyInertia(&torque, &m_body->m_inverseInertia);
        if (m_yawOnly) {
            deltaW.x = 0.0f;
            deltaW.z = 0.0f;
        }
        AddAngularImpulse(m_body, m_xA8 * deltaW);
        if (other) {
            deltaV = -impulse * other->m_inverseMass;  // (vector first: the mass loads before the negation)
            AddLinearImpulse(other, deltaV);
            localImpulse = other->m_object->RotateVectorWorldToLocal(&impulse);
            CrossProduct(&torque, &otherLocalOffset, &localImpulse);
            deltaW = ApplyInertia(&torque, &other->m_inverseInertia);
            if (m_yawOnly) {
                deltaW.x = 0.0f;
                deltaW.z = 0.0f;
            }
            AddAngularImpulse(other, -m_xA8 * deltaW);
        }
    }

    angularAccel = ApplyInertia(&m_body->m_torque, &m_body->m_inverseInertia);
    CrossProduct(&spinAxis, &angularAccel, &localOffset);
    spin = m_body->m_object->RotateVectorLocalToWorld(&spinAxis);
    linearAccel = m_body->m_inverseMass * m_body->m_force;
    accel = linearAccel + spin;
    if (other) {
        angularAccel = ApplyInertia(&other->m_torque, &other->m_inverseInertia);
        CrossProduct(&spinAxis, &angularAccel, &otherLocalOffset);
        spin = other->m_object->RotateVectorLocalToWorld(&spinAxis);
        linearAccel = other->m_inverseMass * other->m_force;
        otherAccel = linearAccel + spin;
    } else {
        otherAccel = ZeroVector;
    }
    float closing = DotProduct(accel - otherAccel, normal);
    if (closing < 0.0f) {
        force = (-closing * k) * normal;
        ApplyForceToBody(m_body, force, point, m_xAC);
        if (other)
            ApplyForceToBody(other, -force, point, m_xAC);
    }

    if (m_x5C) {
        CrossProduct(&correctionSpin, &m_body->m_xF0, &localOffset);
        correctionRate = m_body->m_object->RotateVectorLocalToWorld(&correctionSpin);
        correctionVelocity = m_body->m_xE0 + correctionRate;
        if (other) {
            CrossProduct(&otherCorrectionSpin, &other->m_xF0, &otherLocalOffset);
            correctionRate = other->m_object->RotateVectorLocalToWorld(&otherCorrectionSpin);
            otherCorrectionVelocity = other->m_xE0 + correctionRate;
        } else {
            otherCorrectionVelocity = ZeroVector;
        }
        error = (-depth + 0.1) * normal + correctionVelocity - otherCorrectionVelocity;
        float approach = DotProduct(error, normal);
        if (approach < 0.0f) {
            float j = -approach * k;
            correction = j * normal;
            deltaV2 = m_body->m_inverseMass * correction;
            AddLinearCorrection(m_body, m_xC4 * deltaV2);
            localCorrection = m_body->m_object->RotateVectorWorldToLocal(&correction);
            CrossProduct(&torque2, &localOffset, &localCorrection);
            deltaW2 = ApplyInertia(&torque2, &m_body->m_inverseInertia);
            if (m_yawOnly) {
                deltaW2.x = 0.0f;
                deltaW2.z = 0.0f;
            }
            AddAngularCorrection(m_body, m_xC8 * deltaW2);
            if (other) {
                deltaV2 = -correction * other->m_inverseMass;
                AddLinearCorrection(other, m_xC4 * deltaV2);
                localCorrection = other->m_object->RotateVectorWorldToLocal(&correction);
                CrossProduct(&torque2, &otherLocalOffset, &localCorrection);
                deltaW2 = ApplyInertia(&torque2, &other->m_inverseInertia);
                if (m_yawOnly) {
                    deltaW2.x = 0.0f;
                    deltaW2.z = 0.0f;
                }
                AddAngularCorrection(other, -m_xC8 * deltaW2);
            }
        }
    }
    return 1;
}


// ---- src/SpringCollisionModel/RemoveNormalVelocity.cpp
// VU0 dot product (xyz), as retail's inline-asm helper.
static inline float DotProduct2(_D3DVECTOR *a, _D3DVECTOR *b)
{
    float r;
    asm("lqc2 vf4, 0x0(%1); lqc2 vf5, 0x0(%2); vmul.xyz vf5, vf4, vf5; vaddy.x vf5, vf5, vf5y; vaddz.x vf5, vf5, vf5z; qmfc2 $2, vf5; mtc1 $2, %0"
        : "=f"(r) : "r"(a), "r"(b) : "$2");
    return r;
}

// Retail's fmin is an inline-asm helper: min.s into $f8, stored through a
// pointer to a stack temporary.

// Cancel the part of the body's velocity heading into the surface.
void SpringCollisionModel::RemoveNormalVelocity(_D3DVECTOR &normal, _D3DVECTOR &)
{
    float into = -Fmin(DotProduct2(&m_body->m_velocity, &normal), 0.0f);
    // Copy-initialised (the old file declared it first with a local
    // _D3DVECTOR whose default ctor stored nothing).
    _D3DVECTOR correction = _D3DVECTOR(into * normal.x, into * normal.y, into * normal.z);
    m_body->m_velocity += correction;
}


// ---- src/SpringCollisionModel/Render3D.cpp


int SpringCollisionModel::Render3D()
{
    return GameObject::Render3D();
}


// ---- src/sinit/SpringCollisionModel.cpp
// Static initializer of SpringCollisionModel.cpp: only the axis vectors from the shared header.

