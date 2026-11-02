// TriggerObject.cpp  @ 0x002A0250 (text 0x2A0250-0x2A0EF0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's TriggerObject.cpp as one translation unit: its functions in
// retail order, then its static initializer. `make link` places the
// object's data as well as its code (tools/link_c.py).



#include "CollisionObject/CollisionObject.h"
#include "TriggerObject/TriggerObject.h"
#include "d3dtypes.h"
#include "GameObject/GameObject.h"
#include "TriggerObject/Vu0Orient.h"
#include "SoultreeObject/SoultreeObject.h"
#include "sinit/AxisVectors.h"

// ---- src/global/TriggerObjectSomethingHitMeCallback.cpp


void TriggerObjectSomethingHitMeCallback(CollisionObject *me, CollisionObject *other)
{
    TriggerObject *trigger = (TriggerObject *)me->m_owner;
    if (trigger->m_filtered) {
        int i;
        int allowed = 0;
        for (i = 0; i < 9; i++) {
            if (trigger->m_filter[i] == other->m_owner)
                allowed = 1;
        }
        if (!allowed)
            return;
    }
    int fire = 1;
    if (trigger->m_onceOnly && trigger->m_triggered)
        fire = 0;
    if (fire) {
        trigger->MakeCallbacks(other, 1);
        trigger->m_triggered = 1;
    }
}


// ---- src/TriggerObject/ctor.cpp


_D3DMATRIX IdentityMatrix();


TriggerObject::TriggerObject(int id) : GameObject(id)
{
    m_x30 = 1;
    m_enabled = 1;
    m_callbackData = 0;
    m_callbacks = 0;
    m_parent = 0;
    m_onceOnly = 0;
    m_triggered = 0;
    m_position = ZeroVector;
    m_xC0 = 0;
    m_matrix = IdentityMatrix();
    m_maxCallbacks = 0;
    m_numCallbacks = 0;
    m_showVisual = 0;
    for (int i = 0; i < 9; i++)
        m_filter[i] = 0;
    m_filtered = 0;
}


// ---- src/TriggerObject/dtor.cpp


TriggerObject::~TriggerObject()
{
    if (m_callbackData)
        delete[] m_callbackData;
    if (m_callbacks)
        delete[] m_callbacks;
}


// ---- src/TriggerObject/Create.cpp


extern const unsigned int WORLDOBJECT_TRIGGER;
void TriggerObjectSomethingHitMeCallback(CollisionObject *me, CollisionObject *other);

// Sets up the trigger volume: a sphere when radius > 0, else a box of the
// given size oriented by forward/up (the header's m_xC0 holds the radius).
TriggerObject *TriggerObject::Create(_D3DVECTOR position, _D3DVECTOR size, _D3DVECTOR forward, _D3DVECTOR up, float radius, int maxCallbacks)
{
    GameObject::Create();
    m_position = position;
    m_size = size;
    *(float *)&m_xC0 = radius;
    m_collision = new CollisionObject(1);
    m_collision->Create(7);
    m_collision->m_ownerType = WORLDOBJECT_TRIGGER;
    if (*(float *)&m_xC0 > 0.0f) {
        m_collision->SetSphereCollision(m_position, *(float *)&m_xC0, 0);
    } else if (!(m_size == ZeroVector)) {
        m_collision->SetBoxCollision(ZeroVector, m_size);
        Vu0OrientMatrix(&m_matrix, &m_position, forward, up, 0, 1);
    }
    m_collision->SetTestListType((CollisionObjectTestListType)1);
    m_collision->m_owner = this;
    m_collision->m_otherCallback = TriggerObjectSomethingHitMeCallback;
    m_collision->InitializeTransform();
    AddChild(m_collision, 2);
    // The self-assignment is optimised away but gives the block optimizer the
    // event that retail's delay-slot fills need.
    if (maxCallbacks <= 0) {
        maxCallbacks = 1;
    } else {
        maxCallbacks = maxCallbacks;
    }
    m_callbackData = new void *[maxCallbacks];
    m_callbacks = new TriggerCallback[maxCallbacks];
    m_maxCallbacks = maxCallbacks;
    m_numCallbacks = 0;
    for (int i = 0; i < 9; i++)
        m_filter[i] = 0;
    return this;
}


// ---- src/TriggerObject/AddCallback.cpp



void TriggerObject::AddCallback(void *data, TriggerCallback callback)
{
    if (m_numCallbacks < m_maxCallbacks) {
        m_callbackData[m_numCallbacks] = data;
        m_callbacks[m_numCallbacks] = callback;
        m_numCallbacks++;
    }
}


// ---- src/TriggerObject/AddSearchPlacement.cpp

_D3DMATRIX MatrixMult(_D3DMATRIX a, _D3DMATRIX b);


void TriggerObject::AddSearchPlacement(SearchNode *node)
{
    if (m_parent) {
        _D3DMATRIX matrix;
        m_parent->GetMatrix(&matrix);
        matrix = MatrixMult(matrix, m_matrix);
        m_collision->UpdateTransform(&matrix);
        m_collision->AddSearchPlacement(node, 2);
    } else {
        m_collision->UpdateTransform(&m_matrix);
        m_collision->AddSearchPlacement(node, 1);
    }
}


// ---- src/TriggerObject/Tick.cpp

// A 16-byte-aligned matrix; copying one by value uses lq/sq.
struct Vu0Matrix {
    _D3DMATRIX m;
} __attribute__((aligned(16)));


// sceVu0MulMatrix (out = a * b) taking its inputs by value, as retail's
// copies of both matrices show; operands listed libvu0-style (out, b, a).
static inline void MulMatrix(_D3DMATRIX *out, Vu0Matrix a, _D3DMATRIX b)
{
    asm("lqc2 vf5, 0x0(%1); lqc2 vf1, 0x0(%2); lqc2 vf2, 0x10(%2); lqc2 vf3, 0x20(%2); lqc2 vf4, 0x30(%2); lqc2 vf6, 0x10(%1); vmulax.xyzw ACC, vf1, vf5x; vmadday.xyzw ACC, vf2, vf5y; vmaddaz.xyzw ACC, vf3, vf5z; vmaddw.xyzw vf9, vf4, vf5w; lqc2 vf7, 0x20(%1); vmulax.xyzw ACC, vf1, vf6x; vmadday.xyzw ACC, vf2, vf6y; vmaddaz.xyzw ACC, vf3, vf6z; vmaddw.xyzw vf10, vf4, vf6w; lqc2 vf8, 0x30(%1); vmulax.xyzw ACC, vf1, vf7x; vmadday.xyzw ACC, vf2, vf7y; vmaddaz.xyzw ACC, vf3, vf7z; vmaddw.xyzw vf11, vf4, vf7w; vmulax.xyzw ACC, vf1, vf8x; vmadday.xyzw ACC, vf2, vf8y; vmaddaz.xyzw ACC, vf3, vf8z; vmaddw.xyzw vf12, vf4, vf8w; sqc2 vf9, 0x0(%0); sqc2 vf10, 0x10(%0); sqc2 vf11, 0x20(%0); sqc2 vf12, 0x30(%0)"
        : : "r"(out), "r"(&b), "r"(&a) : "memory");
}


// Follow the attached object, then report test-list entries that stopped
// overlapping.
int TriggerObject::Tick(float dt)
{
    if (m_parent) {
        Vu0Matrix world;
        m_parent->GetMatrix(&world.m);
        MulMatrix(&world.m, world, m_matrix);
        m_collision->UpdateCollisionInfo(&world.m);
    }
    for (int i = 0; i < m_collision->m_numTestObjects; i++) {
        CollisionObject *candidate = m_collision->m_testList[i];
        if (!candidate)
            continue;
        if (m_collision->CheckForOverlap(candidate))
            continue;
        // No longer overlapping: drop it from the test list and report the exit.
        CollisionObject *other = m_collision->m_testList[i];
        other->RemoveFromTestList(m_collision);
        m_collision->m_testList[i] = 0;
        if (m_x30) {
            for (int j = 0; j < m_numCallbacks; j++)
                m_callbacks[j](other, this, m_callbackData[j], 0);
        }
    }
    return GameObject::Tick(dt);
}


// ---- src/TriggerObject/Render3D.cpp

// The collision box draws itself (CollisionObject's debug display) while
// m_debugMode is set.
int TriggerObject::Render3D()
{
    if (m_showVisual) {
        m_collision->m_debugMode = 1;
        m_collision->Render3D();
    } else {
        m_collision->m_debugMode = 0;
    }
    return GameObject::Render3D();
}


// ---- src/TriggerObject/KeyDownHandler.cpp

enum DeviceType { DEVICE_KEYBOARD };
int KEYDOWN_HANDLER_TEST(unsigned int key, DeviceType device, ProcessNode *node, unsigned int flags);


int TriggerObject::KeyDownHandler(ProcessNode *node, KeyInfo *key)
{
    if (KEYDOWN_HANDLER_TEST(0x17, DEVICE_KEYBOARD, node, 0x80))
        m_showVisual = !m_showVisual;
    return GameObject::KeyDownHandler(node, key);
}


// ---- src/TriggerObject/MakeCallbacks.cpp


void TriggerObject::MakeCallbacks(CollisionObject *other, int flags)
{
    int i;
    for (i = 0; i < m_collision->m_numTestObjects; i++) {
        if (m_collision->m_testList[i] == other)
            return;
    }
    if (m_enabled) {
        for (i = 0; i < m_numCallbacks; i++)
            m_callbacks[i](other, this, m_callbackData[i], flags);
    }
    m_collision->AddToTestList(other);
    other->AddToTestList(m_collision);
}


// ---- src/sinit/TriggerObject.cpp
// Static initializer of TriggerObject.cpp: only the axis vectors from the shared header.

