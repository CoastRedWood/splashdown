// CollisionCharacter.cpp  @ 0x002185D0 (text 0x2185D0-0x218FA0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's CollisionCharacter.cpp as one translation unit: CollisionCharacter,
// CollisionTypeRegistry and the collision-object Clear* helpers in retail
// order, then the static initializer. `make link` places the object's data as
// well as its code (tools/link_c.py).



#include "d3dtypes.h"
#include "GameObject/GameObject.h"
#include "SearchableObject/SearchableObject.h"
#include "SoultreeObject/SoultreeObject.h"
#include "Character/Character.h"
#include "D3DIMSoultreeCharacter/D3DIMSoultreeCharacter.h"
#include "CollisionCharacter/CollisionCharacter.h"
#include "CollisionObject/CollisionObject.h"
#include "CollisionTypeRegistry/CollisionTypeRegistry.h"
#include "SoultreeCollisionObject/SoultreeCollisionObject.h"
#include "SoultreeHierarchyCollisionObject/SoultreeHierarchyCollisionObject.h"
#include "SoultreeCollisionInstanceData/SoultreeCollisionInstanceData.h"
#include "SoultreeHierarchyCollisionInstanceData/SoultreeHierarchyCollisionInstanceData.h"
#include "BoundingBoxTree/BoundingBoxTree.h"
#include "RayCollisionObject/RayCollisionObject.h"
#include "VertexRayToFaceIntersectionData/VertexRayToFaceIntersectionData.h"
#include "soultree_assert.h"
// The axis vectors every Soultree TU defines (and __sinit constructs).
#include "sinit/AxisVectors.h"

extern "C" int sprintf(char *buf, const char *fmt, ...);
extern "C" char *strcpy(char *dst, const char *src);
extern "C" int strcasecmp(const char *a, const char *b);
extern "C" void free(void *p);
void DeleteBoundingBoxTree(BoundingBoxTree *tree);

extern char AnimContentDirectory[];
extern char CollisionContentDirectory[];
// The character's collision type ID, set by the static initializer (with
// the axis vectors). Defined before the functions: its .bss precedes their
// statics in retail.
unsigned int COLLISION_COLLISIONCHARACTER = LpCollisionTypeRegistry->GetType("CHARACTER");

// ---- src/CollisionCharacter/ctor.cpp

CollisionCharacter::CollisionCharacter(int construct)
    : GameObject(1), D3DIMSoultreeCharacter(construct)
{
    m_collision = 0;
}


// ---- src/CollisionCharacter/dtor.cpp

CollisionCharacter::~CollisionCharacter()
{
}


// ---- src/CollisionCharacter/Create.cpp
#pragma alignlabel off

GameObject *CollisionCharacter::Create(char *name, char *collisionName, int a, int b, int hasCollision)
{
    char path[0x110];
    char savedDirectory[0x110];
    char collisionPath[0x110];

    sprintf(path, "%s%s", AnimContentDirectory, name);
    D3DIMSoultreeCharacter::Create(path, 0, a, b);
    m_x274 = m_root->m_child;
    if (hasCollision) {
        m_collision = new CollisionObject(1);
        m_collision->Create(5);
        m_collision->m_ignoreVegetation = 1;
        GameObject::AddChild(m_collision, -1);
        // The collision file is looked up in the anim directory.
        strcpy(savedDirectory, CollisionContentDirectory);
        strcpy(CollisionContentDirectory, "");
        sprintf(collisionPath, "%s%s", AnimContentDirectory, collisionName);
        // The redundant cast reproduces retail's argument load order.
        if (!m_collision->SetSoultreeCollision((SoultreeObject *)m_root, collisionPath)) {
#line 61 "CollisionCharacter.cpp"
            ASSERTF(0, ("No Collision File for %s!", m_name));
        }
        strcpy(CollisionContentDirectory, savedDirectory);
        m_collision->SetOwner(this, COLLISION_COLLISIONCHARACTER);
        m_collision->InitializeTransform();
    }
    return GameObject::Create();
}
#pragma alignlabel reset


// ---- src/CollisionCharacter/Tick.cpp

int CollisionCharacter::Tick(float dt)
{
    static int colCharMtr = -1;     // unused profiler meter ID
    if (m_collision)
        m_collision->UpdateCollisionInfo();
    _D3DVECTOR position;
    m_x274->GetPosition(0, &position);
    float invDt = 1.0f / dt;
    float dx = position.x - m_lastPosition.x;
    float dy = position.y - m_lastPosition.y;
    float dz = position.z - m_lastPosition.z;
    float vx, vy, vz;
    vx = dx * invDt;
    vz = dz * invDt;
    vy = dy * invDt;
    m_velocity.x = vx;
    m_velocity.y = vy;
    m_velocity.z = vz;
    m_velocity.w = 1.0f;
    m_lastPosition = position;
    if (m_collision) {
        m_collision->CheckForCollision(0);
        if (m_collision->m_collided) {
            VertexRayToFaceIntersectionData *data = m_collision->m_intersectionData;
            m_x250 = data->m_position;
            m_x260 = data->m_normal;
            if (m_collision->m_collisionType == 1) {
                SoultreeHierarchyCollisionObject *hierarchy = (SoultreeHierarchyCollisionObject *)m_collision->m_collisionData;
                for (int i = 0; i < hierarchy->m_count; i++) {
                    SoultreeCollisionObject *object = &hierarchy->m_objects[i];
                    object->m_xD0._41 -= 1.005f * data->m_x00.x;
                    object->m_xD0._42 -= 1.005f * data->m_x00.y;
                    object->m_xD0._43 -= 1.005f * data->m_x00.z;
                }
            } else {
                SoultreeCollisionObject *object = (SoultreeCollisionObject *)m_collision->m_collisionData;
                object->m_xD0._41 -= 1.005f * data->m_x00.x;
                object->m_xD0._42 -= 1.005f * data->m_x00.y;
                object->m_xD0._43 -= 1.005f * data->m_x00.z;
            }
        }
    }
    return GameObject::Tick(dt);
}


// ---- src/CollisionCharacter/Render3D.cpp

int CollisionCharacter::Render3D()
{
    return GameObject::Render3D();
}
