// RaceStatusStruct_Tag: RaceStatusClass's per-vehicle race state (place,
// laps, track position, stunts), calloc'd 0x710 bytes in AddVehicle and
// chained through m_next. Names come from RaceStatusClass's methods.
#ifndef RACESTATUSSTRUCT_TAG_RACESTATUSSTRUCT_TAG_H
#define RACESTATUSSTRUCT_TAG_RACESTATUSSTRUCT_TAG_H

#include "d3dtypes.h"
#include "TrackPosition_Tag/TrackPosition_Tag.h"

class GateClass;
class RaceStatusVehicleInterface;

struct RaceStatusStunt {                // 0x28 bytes
    unsigned int m_flags;               // +0x00
    int m_type;                         // +0x04
    float m_score;                      // +0x08
    float m_x0C;                        // +0x0C
    float m_x10;                        // +0x10
    float m_x14;                        // +0x14
    float m_x18;                        // +0x18
    float m_x1C;                        // +0x1C
    float m_x20;                        // +0x20
    RaceStatusStunt *m_next;            // +0x24
};

struct RaceStatusStruct_Tag {
    int m_id;                           // +0x000
    RaceStatusVehicleInterface *m_vehicle; // +0x004
    unsigned int m_flags;               // +0x008
    float m_x0C;                        // +0x00C, distance from the last track position to the finish
    float m_x10;                        // +0x010, distance from the track position to the finish
    float m_x14;                        // +0x014
    float m_distance;                   // +0x018
    int m_place;                        // +0x01C
    int m_lap;                          // +0x020
    int m_gates;                        // +0x024, gates passed (waypoint races)
    int m_x28;                          // +0x028, -1 while short of the finish line
    int m_onTrack;                      // +0x02C
    int m_x30;                          // +0x030
    float m_lastLap;                    // +0x034
    float m_x38;                        // +0x038, time the current lap started
    float m_bestLap;                    // +0x03C
    float m_lapTimes[100];              // +0x040
    float m_gateTimes[300];             // +0x1D0, ring of gate-crossing times
    _D3DVECTOR m_position;              // +0x680
    _D3DVECTOR m_lastPosition;          // +0x690
    GateClass *m_objects;               // +0x6A0
    int m_x6A4;                         // +0x6A4
    TrackPosition_Tag m_trackPosition;  // +0x6A8
    TrackPosition_Tag m_lastTrackPosition;  // +0x6B4
    TrackPosition_Tag m_x6C0;           // +0x6C0
    char pad6CC[0x6D4 - 0x6CC];
    float m_x6D4;                       // +0x6D4
    float m_x6D8;                       // +0x6D8, time since the last lap
    float m_x6DC;                       // +0x6DC, time behind the leader
    float m_x6E0;                       // +0x6E0, time off the track
    float m_timeRemaining;              // +0x6E4
    RaceStatusStunt *m_stunts;          // +0x6E8
    int m_numStunts;                    // +0x6EC
    float m_x6F0;                       // +0x6F0, time in the current stunt
    float m_x6F4;                       // +0x6F4, banked stunt score
    float m_x6F8;                       // +0x6F8, sum of the active stunts' scores
    int *m_gatesPassed;                 // +0x6FC

    RaceStatusStruct_Tag *m_next;       // +0x700
    char pad704[0x710 - 0x704];
};

#endif
