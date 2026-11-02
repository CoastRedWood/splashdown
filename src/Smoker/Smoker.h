// Smoker: a smoke plume on a static model (a GameObject that owns a
// SmogParticleEmitter; `new Smoker` in Scene::LoadStaticModels, 0x50 bytes).
// The Smoker files keep local views for now.
#ifndef SMOKER_SMOKER_H
#define SMOKER_SMOKER_H

#include "GameObject/GameObject.h"
#include "d3dtypes.h"

class ParticleManager;
class SmogParticleEmitter;
class SoultreeObject;

class Smoker : public GameObject {
public:
    Smoker(int enabled);
    GameObject *Create(SoultreeObject *object, _D3DVECTOR offset, _D3DVECTOR velocity, float size, float growth,
                       float variance);
    int SetParticleManager(ParticleManager *particles);

    virtual ~Smoker();
    virtual int Tick(float dt);

    _D3DVECTOR m_offset;                // +0x30
    SoultreeObject *m_object;           // +0x40
    SmogParticleEmitter *m_emitter;     // +0x44
    char pad48[0x50 - 0x48];
};

#endif
