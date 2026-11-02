#ifndef RACESTATUSCLASS_RACESTATUSCLASS_H
#define RACESTATUSCLASS_RACESTATUSCLASS_H

#include "GameObject/GameObject.h"

extern "C" void free(void *p);
extern "C" void *calloc(unsigned int n, unsigned int size);

struct RaceStatusStruct_Tag;
class RaceStatusClass;

class GateClass;

#include "d3dtypes.h"
#include "RaceStatusVehicleInterface/RaceStatusVehicleInterface.h"

#include "RaceStatusStruct_Tag/RaceStatusStruct_Tag.h"

#include "GateClass/GateClass.h"

// A gate's position on the track.
struct RaceStatusBlock {
    TrackPosition_Tag m_position;       // +0x00
    RaceStatusBlock *m_next;            // +0x0C
};

class RaceStatusEventInterface;
class TrackType;

class RaceStatusClass : public GameObject {
public:
    RaceStatusClass(int id);
    virtual ~RaceStatusClass();
    virtual int Tick(float dt);
    virtual int Render3D();
    RaceStatusClass *Create(RaceStatusEventInterface *events);
    RaceStatusStruct_Tag *AddVehicle(RaceStatusVehicleInterface *vehicle, int flags, int id);
    RaceStatusStruct_Tag *AddGhostVehicle(RaceStatusStruct_Tag *target, RaceStatusVehicleInterface *ghost, int flags);
    int FreeRaceStatusList();
    int UpdateTrackRaceStatus(float dt);
    int UpdateWaypointRaceStatus(float dt);
    int UpdateTimeEliminationRaceStatus(float dt);
    int UpdateCopyCatRaceStatus(float dt);
    int UpdateGhostRaceStatus(float dt);
    int UpdateStunts(float dt);
    int FindLeadPlayer();
    int FreeStuntList(RaceStatusStruct_Tag *status);
    int HoldStunt(RaceStatusStruct_Tag *status, int hold);
    int UpdateTimeRemaining(RaceStatusStruct_Tag *status, float time, int mode);
    int BeginStunts(RaceStatusStruct_Tag *status);
    int CopyCatRaceOver();
    int SortVehiclesByID();
    int StartRace();
    int EnableAllVehicles();
    int ResetRaceStatus();
    int StuntStarted(RaceStatusStruct_Tag *status, float a, float b, float c, float d, int type, float f, int flags);
    int StuntEnded(RaceStatusStruct_Tag *status, int success, float multiplier);
    int EndStunts(RaceStatusStruct_Tag *status, int success, float multiplier);
    int CopyCatUpdateTurn(int newRound);
    GateClass *AddGate(_D3DVECTOR left, _D3DVECTOR right, _D3DVECTOR normal, float width, float height,
                       float depth, int type);
    RaceStatusStruct_Tag *VehicleInList(RaceStatusVehicleInterface *vehicle);
    int ResetRaceStatusNode(RaceStatusStruct_Tag *status);

    RaceStatusEventInterface *m_x30;    // +0x30, event sink set by Create()
    RaceStatusStruct_Tag *m_vehicles;   // +0x34
    RaceStatusStunt *m_stuntPool;       // +0x38
    RaceStatusStunt *m_freeStunts;      // +0x3C
    unsigned int m_flags;               // +0x40
    int m_x44;                          // +0x44
    int m_raceType;                     // +0x48
    TrackType *m_x4C;                   // +0x4C, the track
    GateClass *m_objects;               // +0x50
    GateClass **m_objectsTail;          // +0x54
    RaceStatusBlock *m_blocks;          // +0x58
    RaceStatusBlock **m_blocksTail;     // +0x5C
    int m_numGates;                     // +0x60
    int m_x64;                          // +0x64
    float m_x68;                        // +0x68
    float m_x6C;                        // +0x6C
    float m_x70;                        // +0x70
    RaceStatusStruct_Tag *m_leader;     // +0x74
    float m_x78;                        // +0x78
    float m_x7C;                        // +0x7C
    float m_x80;                        // +0x80
    float m_x84;                        // +0x84, how far below the track counts as off it
    float m_x88;                        // +0x88
};

#endif

