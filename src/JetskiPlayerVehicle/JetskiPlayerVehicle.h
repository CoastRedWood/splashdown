// JetskiPlayerVehicle: the human-controlled JetskiVehicle. 0x7E0 bytes
// (`new` in JetskiCompetition); its own data starts at JetskiVehicle's
// 0x7B0. +0x7B4 is the gate/waypoint sound (RSGatePassedCallback),
// +0x7C0 the four AnalogAxis controls SetupControls makes (the dtor deletes
// them); Tick's copy names +0x7C0/+0x7C4 the steer and throttle axes.
#ifndef JETSKIPLAYERVEHICLE_JETSKIPLAYERVEHICLE_H
#define JETSKIPLAYERVEHICLE_JETSKIPLAYERVEHICLE_H

#include "JetskiVehicle/JetskiVehicle.h"

class AnalogAxis;
class InputDevice;
class PSXSound;
class SoundEmitter;

class JetskiPlayerVehicle : public JetskiVehicle {
public:
    JetskiPlayerVehicle(int playerIndex);
    int Create(JetskiCompetition *competition, char *a, char *b, char *c, char *d, char *e, LightManager *lights,
               ParticleManager *particles, _D3DVECTOR f, _D3DVECTOR g, Terrain *terrain, PSXWater *water, int player,
               int i, Scene *scene);
    int LoadAudio();
    int SetCompetition(JetskiCompetition *competition);
    int SetupControls();

    // --- virtuals in retail slot order ---
    virtual ~JetskiPlayerVehicle();
    virtual int Tick(float dt);
    virtual void StopWreck(int);
    virtual void TriggerWreck(WreckType type);
    virtual int RSGatePassedCallback(RaceStatusClass *rs, GateClass *gate);

    JetskiCompetition *m_competition;   // +0x7B0 (SetCompetition)
    SoundEmitter *m_gateSound;          // +0x7B4
    PSXSound *m_wreckSound;             // +0x7B8
    PSXSound *m_stuntSound;             // +0x7BC
    AnalogAxis *m_axis[4];              // +0x7C0
    InputDevice *m_input;               // +0x7D0
    char pad7D4[0x7E0 - 0x7D4];
};

#endif
