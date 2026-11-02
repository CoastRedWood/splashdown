// JetskiEngineSoundEmitter: a jetski's engine sounds. A VehicleEmitter (so a
// SoundEmitter): its vtable's slots 27-29 are SoundEmitter::Stop and its own
// Start/UpdateSettings overrides of VehicleEmitter's.
#ifndef JETSKIENGINESOUNDEMITTER_H
#define JETSKIENGINESOUNDEMITTER_H

#include "VehicleEmitter/VehicleEmitter.h"

class JetskiVehicle;
class JetskiEngineModel;
class PSXSound;

class JetskiEngineSoundEmitter : public VehicleEmitter {
public:
    JetskiEngineSoundEmitter(AuralScape *scape, PSXSoundGroup *group, short type, int flags);
    int Create(JetskiVehicle *vehicle, char *name, float volume);
    int PositionEmitter();
    int StopEngineSounds();
    int PlayEngineResetSound();
    int PlayEngineStallSound();
    int LoadEngineSounds(char *file);
    void ResetState();
    void ChooseState(float dt);
    void ChooseInWaterIdleState(float dt);
    void ChooseOutOfWaterIdleState(float dt);
    void ChooseInWaterAccelState(float dt);
    void ChooseOutOfWaterAccelState(float dt);
    int SwitchSounds(PSXSound *sound);
    int PitchShiftAccel(PSXSound *sound);

    // --- virtuals in retail slot order ---
    virtual ~JetskiEngineSoundEmitter();  // __dt__24JetskiEngineSoundEmitterFv
    virtual int Tick(float dt);           // Tick__24JetskiEngineSoundEmitterFf
    virtual int Start();                  // Start__24JetskiEngineSoundEmitterFv
    virtual int UpdateSettings(float volume, long flags, _D3DVECTOR position, int priority);

    // 0x340 bytes (`new JetskiEngineSoundEmitter` in JetskiVehicle::Create).
    // The sound sets' names come from LoadEngineSounds' parameter blocks.
    char pad218[0x220 - 0x218];
    JetskiVehicle *m_vehicle;           // +0x220 (Create)
    JetskiEngineModel *m_engine;        // +0x224, the vehicle's engine (Create)
    int m_state;                        // +0x228 (9 = stalled)
    float m_stallTime;                  // +0x22C
    float m_x230;                       // +0x230
    int m_idling;                       // +0x234
    float m_idleTimer;                  // +0x238
    int m_x23C;                         // +0x23C (Create sets 1 when the vehicle has no player)
    int m_x240;                         // +0x240
    int m_started;                      // +0x244
    int m_x248;                         // +0x248
    float m_startDelay;                 // +0x24C
    PSXSound *m_resetSound;             // +0x250
    PSXSound *m_previousSound;          // +0x254
    ContainerList<PSXSound *> m_inIdle;     // +0x258
    ContainerList<PSXSound *> m_inAccel;    // +0x26C
    ContainerList<PSXSound *> m_inDecel;    // +0x280
    ContainerList<PSXSound *> m_inDecelHi;  // +0x294
    ContainerList<PSXSound *> m_inPower;    // +0x2A8
    ContainerList<PSXSound *> m_outIdle;    // +0x2BC
    ContainerList<PSXSound *> m_outAccel;   // +0x2D0
    ContainerList<PSXSound *> m_outDecel;   // +0x2E4
    ContainerList<PSXSound *> m_outPower;   // +0x2F8
    ContainerList<PSXSound *> m_submerged;  // +0x30C
    ContainerList<PSXSound *> m_stall;      // +0x320
    int m_lastStallSound;               // +0x334
    char pad338[0x340 - 0x338];
};

#endif  // JETSKIENGINESOUNDEMITTER_H
