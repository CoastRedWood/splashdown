// TriggerObject: an invisible box that calls back when something collides
// with it (a GameObject). 0xF0 bytes (`new` in JetskiTraining::Create and
// the track loaders). AddCallback stores its data pointer in +0xB4[] and the
// function in +0xB8[] (the ctor/dtor copies have them the other way round).
// TriggerCallback is the type retail mangles into AddCallback.
#ifndef TRIGGEROBJECT_TRIGGEROBJECT_H
#define TRIGGEROBJECT_TRIGGEROBJECT_H

#include "GameObject/GameObject.h"
#include "d3dtypes.h"

class CollisionObject;
class SearchNode;
class SoultreeObject;

typedef void (*TriggerCallback)(CollisionObject *, void *, void *, int);

class TriggerObject : public GameObject {
public:
    TriggerObject(int enabled);
    TriggerObject *Create(_D3DVECTOR position, _D3DVECTOR size, _D3DVECTOR a, _D3DVECTOR b, float f, int i);
    void AddCallback(void *data, TriggerCallback callback);
    void AddSearchPlacement(SearchNode *node);
    void MakeCallbacks(CollisionObject *other, int flags);

    // --- virtuals in retail slot order ---
    virtual ~TriggerObject();
    virtual int Tick(float dt);
    virtual int Render3D();
    virtual int KeyDownHandler(ProcessNode *node, KeyInfo *key);

    int m_x30;                          // +0x30
    int m_enabled;                      // +0x34
    char pad38[0x40 - 0x38];
    _D3DVECTOR m_position;              // +0x40
    _D3DVECTOR m_size;                  // +0x50
    _D3DMATRIX m_matrix;                // +0x60
    int m_onceOnly;                     // +0xA0
    int m_triggered;                    // +0xA4
    int m_showVisual;                   // +0xA8
    int m_maxCallbacks;                 // +0xAC
    int m_numCallbacks;                 // +0xB0
    void **m_callbackData;              // +0xB4
    TriggerCallback *m_callbacks;       // +0xB8
    SoultreeObject *m_parent;           // +0xBC
    int m_xC0;                          // +0xC0
    CollisionObject *m_collision;       // +0xC4
    int m_filtered;                     // +0xC8
    void *m_filter[9];                  // +0xCC, owners allowed to trigger
};

#endif
