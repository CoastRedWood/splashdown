// JetskiAIVehicle with its real bases (JetskiVehicle.h, VehicleAIInterface.h).
// Layout: JetskiVehicle (GameObject at 0, vptr +8; RaceStatusVehicleInterface
// at +0x30 with its vptr at +0x3C), VehicleAIInterface at +0x7B0 (vptr +0),
// JetskiAIVehicle's own members from +0x7B4. Virtual slots follow the retail vtable.
#ifndef JETSKIAIVEHICLE_AIVEHICLE_H
#define JETSKIAIVEHICLE_AIVEHICLE_H

#include "JetskiVehicle/JetskiVehicle.h"
#include "VehicleAIInterface/VehicleAIInterface.h"

#include "AnalogAxis/AnalogAxis.h"

class VehicleAIClass;
class JoystickDevice;
class JetskiCompetition;
class LightManager;
class ParticleManager;
class Terrain;
class PSXWater;
class Scene;
class RaceStatusClass;
class TrackType;

class JetskiAIVehicle : public JetskiVehicle, public VehicleAIInterface {
public:
    JetskiAIVehicle(int);
    JetskiAIVehicle *Create(JetskiCompetition *competition, char *a, char *b, char *c, char *d, char *e,
                            LightManager *lights, ParticleManager *particles, _D3DVECTOR f, _D3DVECTOR g,
                            Terrain *terrain, PSXWater *water, int i, Scene *scene, RaceStatusClass *race,
                            TrackType *track);
    virtual ~JetskiAIVehicle();
    virtual void ResetPosition(int);
    virtual int GetAIVehicleStatus(_D3DVECTOR *, _D3DVECTOR *, _D3DVECTOR *, _D3DVECTOR *, float *,
                                   _D3DVECTOR *, _D3DVECTOR *, float *, int *);
    virtual int SetAIControllerInputs(_D3DVECTOR *, float *, float *, int *);
    virtual int ResetAIVehicle();
    virtual int ActivateAIStunt(int);
    virtual int SetAIVehicleStatus(_D3DVECTOR *, _D3DVECTOR *, _D3DVECTOR *, _D3DVECTOR *,
                                   _D3DVECTOR *, _D3DVECTOR *);
    virtual int GetAIDataFilename(char *);
    virtual int GetAIParameterFilename(char *);
    virtual int GenerateRandomStunt(int *, float);

    VehicleAIClass *m_ai;               // +0x7B4, the AI driver
    AnalogAxis *m_axes[4];              // +0x7B8
    JoystickDevice *m_input;            // +0x7C8, joystick 0 (Create, single-player only)
    char pad7CC[0x7D0 - 0x7CC];         // (`new JetskiAIVehicle` is 0x7D0)
};

#endif
