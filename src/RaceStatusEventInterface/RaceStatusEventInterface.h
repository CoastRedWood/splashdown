// RaceStatusEventInterface: the race status callbacks. Only a vptr
// (4 bytes); every handler is a do-nothing `return 0` that the
// competitions and race overlays override. JetskiCompetition takes it as
// its second base, after GameObject.
#ifndef RACESTATUSEVENTINTERFACE_RACESTATUSEVENTINTERFACE_H
#define RACESTATUSEVENTINTERFACE_RACESTATUSEVENTINTERFACE_H

class GateClass;
class RaceStatusClass;
struct RaceStatusStruct_Tag;

class RaceStatusEventInterface {
public:
    // Every handler is inline, as retail's copies (binding 13) show: the class
    // has no key function, so its vtable goes out wherever it is first used,
    // in retail's order.
    // --- virtuals in retail slot order ---
    virtual int RSLapUpdateCallback(RaceStatusClass *race, RaceStatusStruct_Tag *racer) { return 0; }
    virtual int RSBestLapCallback(RaceStatusClass *race, RaceStatusStruct_Tag *racer) { return 0; }
    virtual int RSGateUpdateCallback(RaceStatusClass *race, RaceStatusStruct_Tag *racer) { return 0; }
    virtual int RSFinishedRaceCallback(RaceStatusClass *race, RaceStatusStruct_Tag *racer) { return 0; }
    virtual int RSRaceOverCallback() { return 0; }
    virtual int RSGateMissedCallback(RaceStatusClass *race, RaceStatusStruct_Tag *racer, GateClass *gate) { return 0; }
    virtual int RSGatePassedCallback(RaceStatusClass *race, RaceStatusStruct_Tag *racer, GateClass *gate) { return 0; }
    virtual int RSRaceOrderChangeCallback(RaceStatusClass *race, RaceStatusStruct_Tag *racer, RaceStatusStruct_Tag *other, int place) { return 0; }
    virtual int RSVehicleTakesLeadCallback(RaceStatusClass *race, RaceStatusStruct_Tag *racer) { return 0; }
    virtual int RSUpdateStuntScore(RaceStatusClass *race, RaceStatusStruct_Tag *racer) { return 0; }
    virtual int RSEndStunts(RaceStatusStruct_Tag *racer, int stunt, float score) { return 0; }
    virtual int RSStuntEnded(RaceStatusStruct_Tag *racer, int stunt, float score) { return 0; }
    virtual int RSSetCurrentVehicle(RaceStatusStruct_Tag *racer) { return 0; }
    virtual int RSStartGhostVehicle(RaceStatusClass *race, RaceStatusStruct_Tag *racer) { return 0; }
    virtual int RSStopGhostVehicle(RaceStatusClass *race, RaceStatusStruct_Tag *racer) { return 0; }
    virtual int RSUpdateBestGhostLap(RaceStatusClass *race, RaceStatusStruct_Tag *racer, RaceStatusStruct_Tag *ghost) { return 0; }
};

#endif
