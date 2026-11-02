// ParticleEmitter: a GameObject that feeds particles to a ParticleManager.
// 0x1E8 bytes (data from +0x30 to +0x1E4, rounded to 8); SmogParticleEmitter,
// BowSprayParticleEmitter and the other emitters derive from it. Names follow
// its own methods; +0x180.. are what LoadFromFile(ParameterBlock *) reads
// ("ParticleLife", "Growth", "InitialSize", "EmitRate", "RandomMirrorAboutX/Y",
// "AgeAtBirth", "ParticleGroup", "EmitMode", "DelayTime", "RandomMin/MaxTimeOn/Off").
// The ctor stores 0 to several float members as ints in its copy.
#ifndef PARTICLEEMITTER_PARTICLEEMITTER_H
#define PARTICLEEMITTER_PARTICLEEMITTER_H

#include "GameObject/GameObject.h"
#include "d3dtypes.h"

class ParameterBlock;
class ParticleManager;
class SoultreeObject;

class ParticleEmitter : public GameObject {
public:
    ParticleEmitter(int enabled);
    void UpdatePosition(_D3DVECTOR position);

    // --- virtuals in retail slot order ---
    virtual ~ParticleEmitter();
    virtual int Tick(float dt);
    virtual int KeyDownHandler(ProcessNode *node, KeyInfo *key);
    virtual ParticleEmitter *Create(ParticleManager *manager, SoultreeObject *owner, _D3DVECTOR *offset);
    virtual void SetParticleManager(ParticleManager *manager);
    virtual void EnableParticles();
    virtual void DisableParticles();
    virtual void ResetParticles();
    virtual int LoadFromFile(char *name);
    virtual int LoadFromFile(ParameterBlock *block);
    virtual int ReloadFromFile();

    // Tick's "the emitter stays put this tick": no particles in flight.
    int Idle(float dt)
    {
        m_x1B0 += dt;
        m_lastPosition = m_position;
        m_x1A4 = 0;
        return 1;
    }

    _D3DVECTOR m_position;              // +0x30
    _D3DVECTOR m_lastPosition;          // +0x40
    _D3DVECTOR m_offset;                // +0x50, from the owner
    _D3DVECTOR m_velocity;              // +0x60 "Velocity"
    int m_reset;                        // +0x70, 1 from the ctor
    ParticleManager *m_particleManager; // +0x74
    SoultreeObject *m_owner;            // +0x78
    char m_fileName[0x104];             // +0x7C
    float m_particleLife;               // +0x180, 4000 from the ctor
    float m_growth;                     // +0x184
    float m_initialSize;                // +0x188
    float m_emitRate;                   // +0x18C
    float m_x190;                       // +0x190 (SmogParticleEmitter::SetSize stores the size here too)
    int m_particlesEnabled;             // +0x194
    int m_randomMirrorX;                // +0x198
    int m_randomMirrorY;                // +0x19C
    int m_x1A0;                         // +0x1A0
    int m_x1A4;                         // +0x1A4
    unsigned int m_color;               // +0x1A8, 0xFFFFFF from the ctor
    unsigned int m_flags;               // +0x1AC
    float m_x1B0;                       // +0x1B0 (Tick adds dt)
    float m_ageAtBirth;                 // +0x1B4
    int m_firstIndex;                   // +0x1B8
    int m_lastIndex;                    // +0x1BC
    int m_group;                        // +0x1C0, -1 from the ctor
    int m_mode;                         // +0x1C4, 1 one-shot, 2 random, 3 timed
    float m_startDelay;                 // +0x1C8
    float m_delayTime;                  // +0x1CC
    float m_timer;                      // +0x1D0
    float m_onTimeMin;                  // +0x1D4
    float m_onTimeMax;                  // +0x1D8
    float m_offTimeMin;                 // +0x1DC
    float m_offTimeMax;                 // +0x1E0
};

#endif
