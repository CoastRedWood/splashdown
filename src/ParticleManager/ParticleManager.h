// ParticleManager: owns and renders all particles (a GameObject). 0x1F0
// bytes (`new` in Scene loading); data +0x30..+0x1E3. Names are the most
// common among the copies; LoadFromDescriptorFile names +0x110/+0x134/
// +0x13C/+0x140/+0x148; PrepareGeometry +0x11C..+0x124.
#ifndef PARTICLEMANAGER_PARTICLEMANAGER_H
#define PARTICLEMANAGER_PARTICLEMANAGER_H

#include "GameObject/GameObject.h"
#include "d3dtypes.h"
#include "_D3DLVERTEX/_D3DLVERTEX.h"
#include "DmaRenderItem/DmaRenderItem.h"

class ParticleVUPhysics;
class PSXTextureMap;
struct Particle;
struct ParticleFrame;
struct ParticleGroup;
struct ParticleList;             // a depth bin's particle list (Particles.cpp)

class ParticleManager : public GameObject {
public:
    ParticleManager(int enabled);
    void ClearParticles();
    int LoadFromDescriptorFile(char *name);
    void UpdateParticleRenderItem();
    void UpdatePerFrameStates();

    // --- virtuals in retail slot order ---
    virtual ~ParticleManager();
    virtual int Tick(float dt);
    virtual int PrepareGeometry();
    virtual int Render3D();
    virtual ParticleManager *Create(char *name, int count);

    _D3DVECTOR m_30;                    // +0x30
    _D3DVECTOR m_gravity;               // +0x40
    _D3DVECTOR m_50[4];                 // +0x50
    _D3DLVERTEX m_vertices[4];          // +0x90
    ParticleFrame *m_frames;            // +0x110
    int m_114;                          // +0x114
    int m_118;                          // +0x118
    ParticleList *m_lists;              // +0x11C, TotalBins depth bins (PrepareGeometry)
    int m_binMin;                       // +0x120, the lowest and highest bin in use
    int m_binMax;                       // +0x124
    int m_numParticles;                 // +0x128
    int m_12C;                          // +0x12C
    int m_maxParticles;                 // +0x130
    PSXTextureMap *m_texture;           // +0x134
    Particle **m_particles;             // +0x138, m_maxParticles of them
    ParticleGroup *m_groups;            // +0x13C
    int m_numGroups;                    // +0x140
    int m_144;                          // +0x144
    float m_glowScalar;                 // +0x148
    char pad14C[0x150 - 0x14C];
    DmaRenderItem m_renderItem0;        // +0x150
    DmaRenderItem m_renderItem1;        // +0x180
    DmaRenderItem m_renderItem2;        // +0x1B0
    ParticleVUPhysics *m_vuPhysics;     // +0x1E0
    char pad1E4[0x1F0 - 0x1E4];         // (`new ParticleManager` is 0x1F0)
};

#endif
