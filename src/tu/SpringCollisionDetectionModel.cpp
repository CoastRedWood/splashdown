// SpringCollisionDetectionModel.cpp  @ 0x002ED780 (text 0x2ED780-0x2EE7F0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's SpringCollisionDetectionModel.cpp as one translation unit: its functions in
// retail order, then its static initializer. `make link` places the object's data as well
// as its code (tools/link_c.py). It opens with
// SpringCollisionDetectionIHitSomethingCallback, the collision callback Create installs
// (it was at the end of PSXWaterDisplacement's tu_map row; the TU map_FREE).




// inline_depth is TU-wide: set once, before the callback (its inlined
// GetSurfaceType/FindChar need it) and SimulationTick.
#pragma inline_depth(4)

#include "d3dtypes.h"
#include "GameObject/GameObject.h"
#include "SpringCollisionDetectionModel/SpringCollisionDetectionModel.h"
#include "SpringCollisionModel/SpringCollisionModel.h"
#include "SearchableObject/SearchableObject.h"
#include "SoultreeObject/SoultreeObject.h"
#include "Terrain/Terrain.h"
#include "CollisionObject/CollisionObject.h"
#include "global/CollisionTree.h"
#include "global/Vu0More.h"
#include "PhysicsBody/PhysicsBody.h"
// The axis vectors every Soultree TU defines (and __sinit constructs).
#include "sinit/AxisVectors.h"

extern const unsigned int WORLDOBJECT_PHYSICSBODY;

extern "C" unsigned int strlen(const char *s);

extern const unsigned int WORLDOBJECT_TRIGGER;
extern const unsigned int WORLDOBJECT_STATICOBJECT;

// Direct3D's componentwise product (D3D_OVERLOADS).
inline _D3DVECTOR operator*(const _D3DVECTOR &a, const _D3DVECTOR &b)
{
    return _D3DVECTOR(a.x * b.x, a.y * b.y, a.z * b.z);
}

// An inlined strchr.
static inline char *FindChar(char *s, char c)
{
    while (*s && *s != c)
        s++;
    if (*s == c)
        return s;
    return 0;
}

// The surface type a soultree's name ends in ("...fr<digit>", before any
// extension), or id unchanged.
static inline void GetSurfaceType(SoultreeObject *object, int &id)
{
    char *end = FindChar(object->m_name, '.');
    if (!end)
        end = object->m_name + strlen(object->m_name);
    char *suffix = end - 3;
    if (suffix[0] == 'f' && suffix[1] == 'r')
        id = suffix[2] - '0';
}

// Collision callback of a spring-modelled body: adds the contacts of the
// collision just found to the body's SpringCollisionModel.
void SpringCollisionDetectionIHitSomethingCallback(CollisionObject *self, CollisionObject *other)
{
    // Declared in this order for retail's registers.
    int i;
    SpringCollisionModel *model;
    PhysicsBody *body;
    PhysicsBody *otherBody;
    SoultreeObject *object;
    int id;
    DynamicRayCollisionObject *dynRays;
    int collide;
    RayToFaceIntersectionData *rayData;
    RayToFaceIntersectionData *dynData;
    SphereIntersectionData *sphereData;
    SphereCollisionObject *sphere;
    RayCollisionObject *rays;
    unsigned int type = other->m_ownerType;
    if (type & WORLDOBJECT_TRIGGER)
        return;
    model = (SpringCollisionModel *)self->m_owner;
    body = (PhysicsBody *)self->m_ownerData;
    otherBody = 0;
    if (!model->m_hitObject || (model->m_hitObject->m_ownerType & WORLDOBJECT_PHYSICSBODY))
        model->m_hitObject = other;
    collide = 1;
    id = 0;
    if (type & WORLDOBJECT_PHYSICSBODY) {
        otherBody = (PhysicsBody *)other->m_ownerData;
        if (body->OtherBodyAlreadyCollided(otherBody)) {
            collide = 0;
        } else {
            otherBody->AddOtherBodyCollision(body);
            body->AddBodyCollision(otherBody);
            otherBody->AddBodyCollision(body);
        }
        object = otherBody->m_object;
        if (object)
            GetSurfaceType(object, id);
    } else if (type & WORLDOBJECT_STATICOBJECT) {
        object = (SoultreeObject *)other->m_owner;
        if (object)
            GetSurfaceType(object, id);
    }
    if (!collide)
        return;
    switch (self->m_collisionType) {
    case COLLISION_RAYS: {
        rayData = (RayToFaceIntersectionData *)self->m_intersectionData;
        rays = (RayCollisionObject *)self->m_collisionData;
        for (i = 0; i < rayData->m_numRays; i++)
            model->AddObjectCollisionPoint(rayData->m_rays[i]->m_start, rayData->m_rays[i]->m_end, rayData->m_rayNormals[i],
                                           rayData->m_rayPoints[i], otherBody, rays->m_object, other, id);
        break;
    }
    case COLLISION_DYNAMIC_RAYS: {
        _D3DVECTOR start, end, point;
        dynRays = (DynamicRayCollisionObject *)self->m_collisionData;
        dynData = (RayToFaceIntersectionData *)self->m_intersectionData;
        // Bounded by the model's spring count, not the ray count (as retail).
        for (i = 0; i < model->m_numSprings; i++) {
            if (!dynData->m_rayHit[i])
                continue;
            point = dynRays->m_starts[i] + dynRays->m_directions[i] * _D3DVECTOR(dynRays->m_lengths);
            Vu0InverseTransformPoint(&end, point, &body->m_object->m_worldMatrix);
            Vu0InverseTransformPoint(&start, dynRays->m_starts[i], &body->m_object->m_worldMatrix);
            model->AddObjectCollisionPoint(start, end, dynData->m_rayNormals[i], dynData->m_rayPoints[i], otherBody,
                                           dynRays->m_objects[i], other, id);
        }
        break;
    }
    case COLLISION_SPHERE: {
        sphere = (SphereCollisionObject *)self->m_collisionData;
        sphereData = (SphereIntersectionData *)self->m_intersectionData;
        _D3DVECTOR center, point;
        Vu0InverseTransformPoint(&center, sphereData->m_center, &body->m_object->m_worldMatrix);
        for (i = 0; i < sphereData->m_count; i++) {
            _D3DVECTOR p = sphereData->m_center - sphere->radius * sphereData->m_normals[i];
            Vu0InverseTransformPoint(&point, p, &body->m_object->m_worldMatrix);
            model->AddObjectCollisionPoint(center, point, sphereData->m_normals[i], sphereData->m_positions[i], otherBody,
                                           sphere->m_object, other, id);
        }
        break;
    }
    }
}

// ---- src/SpringCollisionDetectionModel/ctor.cpp

SpringCollisionDetectionModel::SpringCollisionDetectionModel(int id) : GameObject(id)
{
    m_x64 = 0;
    m_objectCollision = 1;
    m_model = 0;
    m_terrain = 0;
    m_collision = 0;
    m_dynamicRays = 0;
    m_numObjects = 0;
    m_objects = 0;
    m_center = ZeroVector;
    m_half = ZeroVector;
}


// ---- src/SpringCollisionDetectionModel/dtor.cpp
SpringCollisionDetectionModel::~SpringCollisionDetectionModel()
{
    if (m_dynamicRays)
        delete[] m_dynamicRays;
}


// ---- src/SpringCollisionDetectionModel/Create.cpp

SpringCollisionDetectionModel *SpringCollisionDetectionModel::Create(SpringCollisionModel *model, Terrain *terrain,
                                                                     int dynamicRays, int rays, _D3DVECTOR origin, SearchNode *node)
{
    m_model = model;
    m_terrain = terrain;
    if (!m_terrain)
        m_model->m_groundCollision = 0;
    if (dynamicRays || rays) {
        m_collision = new CollisionObject(1);
        m_collision->Create(2);
        AddChild(m_collision, 2);
        m_collision->m_callback = SpringCollisionDetectionIHitSomethingCallback;
        m_collision->m_ownerType = WORLDOBJECT_PHYSICSBODY;
        m_collision->m_owner = m_model;
        m_collision->m_ownerData = m_model->m_body;
        m_collision->SetTestListType((CollisionObjectTestListType)1);
        if (dynamicRays) {
            m_dynamicRays = new _D3DVECTOR[m_model->m_numSprings * 2];
            m_collision->SetDynamicRayCollision(m_model->m_numSprings, 0, 0);
            m_collision->AddSearchPlacement(node, 2);
        } else if (rays) {
            _D3DVECTOR *v = new _D3DVECTOR[m_model->m_numSprings * 2];
            for (int i = 0; i < m_model->m_numSprings; i++) {
                v[i * 2] = origin;
                v[i * 2 + 1] = m_model->m_springs[i].m_point;
            }
            m_collision->SetRayCollision(m_model->m_numSprings, v, m_model->m_springs->m_object);
            m_collision->AddSearchPlacement(node, 2);
            delete[] v;
        }
    }
    return (SpringCollisionDetectionModel *)GameObject::Create();
}


// ---- src/SpringCollisionDetectionModel/SetCollisionObject.cpp
void SpringCollisionDetectionModel::SetCollisionObject(CollisionObject *object)
{
    m_collision = object;
    m_collision->m_owner = m_model;
    m_collision->m_ownerData = m_model->m_body;
    m_collision->m_callback = SpringCollisionDetectionIHitSomethingCallback;
    m_collision->SetTestListType((CollisionObjectTestListType)1);
}


// ---- src/SpringCollisionDetectionModel/SimulationTick.cpp


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


// Places the springs in the world: ground contact for each spring, and the
// bounding box of the springs, whose center starts every dynamic ray; then
// tests the rays against objects.
int SpringCollisionDetectionModel::SimulationTick(float dt)
{
    int i;
    if (m_model) {
        static int GroundCollisionID = -1;  // unused profiler ID
        if (m_dynamicRays || (m_model->m_groundCollision && m_terrain)) {
            i = 0;
            SpringCollisionPoint *spring = m_model->m_springs;
            _D3DVECTOR pos;
            _D3DVECTOR min = _D3DVECTOR(3.40282347e+38f, 3.40282347e+38f, 3.40282347e+38f);
            _D3DVECTOR max = _D3DVECTOR(-3.40282347e+38f, -3.40282347e+38f, -3.40282347e+38f);
            for (; i < m_model->m_numSprings; i++) {
                pos = spring->m_object->TransformVectorLocalToWorld(&spring->m_point);
                if (m_terrain && m_model->m_groundCollision) {
                    unsigned char type;
                    spring->m_worldPosition = pos;
                    m_terrain->GetGridPointIntersection(&spring->m_worldPosition, &spring->m_groundNormal, 0, &type);
                    spring->m_surfaceType = type & 7;
                    m_model->m_surfaceType = spring->m_surfaceType;
                }
                if (m_dynamicRays) {
                    m_dynamicRays[i * 2 + 1] = pos;
                    min.x = Fmin(pos.x, min.x);
                    min.y = Fmin(pos.y, min.y);
                    min.z = Fmin(pos.z, min.z);
                    max.x = Fmax(pos.x, max.x);
                    max.y = Fmax(pos.y, max.y);
                    max.z = Fmax(pos.z, max.z);
                }
                spring++;
            }
            if (m_dynamicRays) {
                m_center = 0.5f * (max + min);
                m_half = 0.5f * (max - min);
            }
        }
        static int ObjectCollisionID = -1;  // unused profiler ID
        m_model->ResetObjectCollisions();
        if (m_collision && m_objectCollision) {
            if (m_dynamicRays) {
                for (i = 0; i < m_model->m_numSprings; i++)
                    m_dynamicRays[i * 2] = m_center;
                m_collision->SetDynamicRays(m_dynamicRays, 0);
            } else {
                m_collision->UpdateCollisionInfo();
            }
            if (m_objects)
                m_collision->CheckForCollision(*m_numObjects, m_objects);
            else
                m_collision->CheckForCollision(0);
        }
    }
    return 1;
}


// ---- src/sinit/SpringCollisionDetectionModel.cpp
// Static initializer of SpringCollisionDetectionModel.cpp: only the axis vectors from the shared header.

