// VehicleAIInterface: the AI side of a vehicle (JetskiAIVehicle's second
// base, at +0x7B0). Only a vptr, 4 bytes; no RTTI bases. Its default
// ctor is inline (JetskiAIVehicle's ctor stores the vptr itself).
#ifndef VEHICLEAIINTERFACE_VEHICLEAIINTERFACE_H
#define VEHICLEAIINTERFACE_VEHICLEAIINTERFACE_H

struct _D3DVECTOR;

class VehicleAIInterface {
public:
    VehicleAIInterface() {}

    // --- virtuals in retail slot order ---
    // All inline (retail's copies carry mwcc's inline binding): the class has
    // no key function, so its vtable goes out with JetskiAIVehicle.cpp's in
    // retail's order.
    virtual int GetAIVehicleStatus(_D3DVECTOR *position, _D3DVECTOR *velocity, _D3DVECTOR *a3, _D3DVECTOR *a4,
                                   float *a5, _D3DVECTOR *a6, _D3DVECTOR *a7, float *a8, int *a9) { return 0; }
    virtual int SetAIControllerInputs(_D3DVECTOR *steer, float *throttle, float *brake, int *buttons) { return 0; }
    virtual int ResetAIVehicle() { return 0; }
    virtual int ActivateAIStunt(int stunt) { return 0; }
    virtual int SetAIVehicleStatus(_D3DVECTOR *position, _D3DVECTOR *velocity, _D3DVECTOR *forward, _D3DVECTOR *up,
                                   _D3DVECTOR *right, _D3DVECTOR *angularVelocity) { return 0; }
    virtual int GetAIDataFilename(char *filename) { return 0; }
    virtual int GetAIParameterFilename(char *filename) { return 0; }
    virtual int GenerateRandomStunt(int *stunt, float chance) { return 0; }
};

#endif
