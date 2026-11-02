// SpringCollisionDetectionModel: finds the contacts of a SpringCollisionModel
// against the terrain and search-tree objects (a GameObject). 0x80 bytes
// (`new` in JetskiVehicle/WaterWrecker setup); the ctor sets +0x30..+0x70.
// +0x60 is called m_springs (SpringCollisionData *) by two copies.
#ifndef SPRINGCOLLISIONDETECTIONMODEL_SPRINGCOLLISIONDETECTIONMODEL_H
#define SPRINGCOLLISIONDETECTIONMODEL_SPRINGCOLLISIONDETECTIONMODEL_H

#include "GameObject/GameObject.h"
#include "d3dtypes.h"

class CollisionObject;
class SearchNode;
class SearchableObject;
class SpringCollisionModel;
class Terrain;

class SpringCollisionDetectionModel : public GameObject {
public:
    SpringCollisionDetectionModel(int enabled);
    SpringCollisionDetectionModel *Create(SpringCollisionModel *model, Terrain *terrain, int a, int b,
                                          _D3DVECTOR c, SearchNode *node);
    void SetCollisionObject(CollisionObject *object);

    // --- virtuals in retail slot order ---
    virtual ~SpringCollisionDetectionModel();
    virtual int SimulationTick(float dt);

    SpringCollisionModel *m_model;      // +0x30
    Terrain *m_terrain;                 // +0x34
    CollisionObject *m_collision;       // +0x38
    char pad3C[0x40 - 0x3C];
    _D3DVECTOR m_center;                // +0x40
    _D3DVECTOR m_half;                  // +0x50
    _D3DVECTOR *m_dynamicRays;          // +0x60
    int m_x64;                          // +0x64
    int m_objectCollision;              // +0x68
    int *m_numObjects;                  // +0x6C
    SearchableObject **m_objects;       // +0x70
    char pad74[0x80 - 0x74];
};

#endif
