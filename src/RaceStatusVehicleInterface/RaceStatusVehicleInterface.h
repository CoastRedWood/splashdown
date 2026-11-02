// RaceStatusVehicleInterface: the vehicle side of the race status link
// (JetskiVehicle's second base). Three words of data, then the vptr at +0x0C;
// 0x10 bytes. Every callback's default returns 0.
#ifndef RACESTATUSVEHICLEINTERFACE_RACESTATUSVEHICLEINTERFACE_H
#define RACESTATUSVEHICLEINTERFACE_RACESTATUSVEHICLEINTERFACE_H

struct _D3DVECTOR;
struct RaceStatusStruct_Tag;
class RaceStatusClass;
class GateClass;

class RaceStatusVehicleInterface {
public:
    // Retail's inline ctor (JetskiVehicle's) clears the three words.
    RaceStatusVehicleInterface() : m_ownStatus(0), m_raceStatus(0), m_status(0) {}

    RaceStatusStruct_Tag *m_ownStatus;  // +0x00, this vehicle's entry: what RaceStatusClass::AddVehicle returned
    RaceStatusClass *m_raceStatus;      // +0x04, set by RaceStatusClass::AddVehicle
    RaceStatusStruct_Tag *m_status;     // +0x08

    // Every handler is inline, as retail's copies (binding 13) show: the class
    // has no key function, so its vtable goes out wherever it is first used,
    // in retail's order.
    // --- virtuals in retail slot order, vptr at +0x0C ---
    virtual int RSGetVehicleStatus(_D3DVECTOR *position, _D3DVECTOR *velocity, float *speed, int *status) { return 0; }
    virtual int RSLapUpdateCallback(RaceStatusClass *, RaceStatusStruct_Tag *) { return 0; }
    virtual int RSBestLapCallback(RaceStatusClass *, RaceStatusStruct_Tag *) { return 0; }
    virtual int RSGateUpdateCallback(RaceStatusClass *, RaceStatusStruct_Tag *) { return 0; }
    virtual int RSFinishedRaceCallback(RaceStatusClass *, RaceStatusStruct_Tag *) { return 0; }
    virtual int RSRaceOverCallback() { return 0; }
    virtual int RSSetResetParameters(_D3DVECTOR *, _D3DVECTOR *) { return 0; }
    virtual int RSGateMissedCallback(RaceStatusClass *, GateClass *) { return 0; }
    virtual int RSGatePassedCallback(RaceStatusClass *, GateClass *) { return 0; }
    virtual int RSRaceOrderChangeCallback(RaceStatusClass *, RaceStatusStruct_Tag *, RaceStatusStruct_Tag *, int) { return 0; }
    virtual int RSVehicleTakesLeadCallback(RaceStatusClass *, RaceStatusStruct_Tag *) { return 0; }
    virtual int RSUpdateStuntScore(RaceStatusClass *, RaceStatusStruct_Tag *) { return 0; }
    virtual int RSStuntScoreCallback(RaceStatusClass *, RaceStatusStruct_Tag *) { return 0; }
    virtual int RSEndStunts(RaceStatusStruct_Tag *, int, float) { return 0; }
    virtual int RSStuntEnded(RaceStatusStruct_Tag *, int, float) { return 0; }
};

#endif
