// RaceStatusClass.cpp  @ 0x00305F60 (text 0x305F60-0x30A860)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// RaceStatusClass as one translation unit, a TU without an initializer inside
// Portal.cpp's tu_map row, after AnimTextureMap.cpp. Its .rodata is only the
// RaceStatusClass RTTI (0x430950-0x4309C0, labels @2304-2308); the name is a
// guess. UpdateStunts no longer needs its own). fast_fptosi is on from
// EndStunts.




#include "RaceStatusClass/RaceStatusClass.h"
#include "TrackType/TrackType.h"
#include "RaceStatusEventInterface/RaceStatusEventInterface.h"
#include "TrackSegment_Tag/TrackSegment_Tag.h"
#include "GameObject/GameObject.h"
#include "d3dtypes.h"
#include "TrackPosition_Tag/TrackPosition_Tag.h"
#include "TrackData_Tag/TrackData_Tag.h"
#include "RaceStatusVehicleInterface/RaceStatusVehicleInterface.h"
#include "GateClass/GateClass.h"

// ---- src/RaceStatusClass/ctor.cpp

RaceStatusClass::RaceStatusClass(int id) : GameObject(id)
{
    m_vehicles = 0;
    m_flags = 0;
    m_x44 = 0;
    m_raceType = 0;
    m_x4C = 0;
    m_objects = 0;
    m_objectsTail = &m_objects;
    m_blocks = 0;
    m_blocksTail = &m_blocks;
    m_numGates = 0;
    m_x64 = 0;
    m_x68 = 0;
    m_x6C = -1.0f;
    m_leader = 0;
    m_x30 = 0;
    m_x70 = 0;
    m_x78 = 1.0f;
    m_stuntPool = (RaceStatusStunt *)calloc(100, sizeof(RaceStatusStunt));
    m_freeStunts = m_stuntPool;
    RaceStatusStunt *stunt = m_freeStunts;
    for (int i = 0; i < 99; i++, stunt++)
        stunt->m_next = stunt + 1;
    m_x84 = 0;
    m_x88 = 100.0f;
    m_x80 = 0.2f;
}


// ---- src/RaceStatusClass/Create.cpp

RaceStatusClass *RaceStatusClass::Create(RaceStatusEventInterface *events)
{
    m_x30 = events;
    return this;
}


// ---- src/RaceStatusClass/dtor.cpp

RaceStatusClass::~RaceStatusClass()
{
    if (m_vehicles)
        FreeRaceStatusList();
    while (m_objects) {
        GateClass *object = m_objects;
        m_objects = object->m_next;
        object->Release();
    }
    while (m_blocks) {
        RaceStatusBlock *block = m_blocks;
        m_blocks = block->m_next;
        free(block);
    }
    if (m_stuntPool)
        free(m_stuntPool);
}


// ---- src/RaceStatusClass/ResetRaceStatusNode.cpp

extern "C" void *memset(void *dst, int c, unsigned int n);


// Clears a racer's race state for a restart.
int RaceStatusClass::ResetRaceStatusNode(RaceStatusStruct_Tag *status)
{
    if (!status)
        return 0;
    status->m_flags &= 0x20C;
    status->m_x0C = 0;
    status->m_x10 = 0;
    status->m_x14 = 0;
    status->m_distance = 0;
    status->m_place = 0;
    status->m_lap = 0;
    status->m_gates = 0;
    status->m_x28 = 0;
    status->m_lastLap = 0;
    status->m_x38 = 0;
    if (!(status->m_flags & 0x200))
        status->m_bestLap = 0;
    status->m_x6D4 = 0;
    status->m_x6D8 = 0;
    status->m_x6DC = 0;
    status->m_x6E0 = 0;
    status->m_objects = m_objects;
    status->m_numStunts = 0;
    status->m_x6F0 = 0;
    status->m_x6F4 = 0;
    status->m_x6F8 = 0;
    status->m_timeRemaining = m_x70;
    status->m_x30 = 0;
    status->m_x6A4 = 0;
    if (status->m_gatesPassed)
        memset(status->m_gatesPassed, 0, m_numGates * sizeof(int));
    while (status->m_stunts) {
        RaceStatusStunt *stunt = status->m_stunts;
        status->m_stunts = stunt->m_next;
        if (stunt) {
            stunt->m_next = m_freeStunts;
            m_freeStunts = stunt;
        }
    }
    if (m_raceType == 2 || m_raceType == 6) {
        if (m_x4C) {
            status->m_vehicle->RSGetVehicleStatus(&status->m_position, 0, 0, 0);
            status->m_lastPosition = status->m_position;
            m_x4C->FindClosest(status->m_position, &status->m_trackPosition, 0);
            if (m_x4C->m_startGate) {
                status->m_lastTrackPosition.m_segment = m_x4C->m_startTrackPosition.m_segment;
                status->m_lastTrackPosition.m_data = m_x4C->m_startTrackPosition.m_data;
                status->m_lastTrackPosition.m_t = m_x4C->m_startTrackPosition.m_t;
            } else {
                status->m_lastTrackPosition = status->m_trackPosition;
            }
            status->m_x6C0 = status->m_lastTrackPosition;
            status->m_onTrack = m_x4C->OnTrack(status->m_position, status->m_lastTrackPosition, -1.0f, -1.0f, 0, 0, 0);
        }
    }
    return 1;
}


// ---- src/RaceStatusClass/ResetRaceStatus.cpp

class GateClass;


int RaceStatusClass::ResetRaceStatus()
{
    m_flags &= 0x60;
    m_x68 = 0;
    m_leader = 0;
    for (RaceStatusStruct_Tag *status = m_vehicles; status; status = status->m_next)
        ResetRaceStatusNode(status);
    for (GateClass *obj = m_objects; obj; obj = obj->m_next)
        obj->m_flags |= 1;
    if (m_raceType == 5 && m_vehicles) {
        m_vehicles->m_flags |= 0x180;
        m_leader = m_vehicles;
        if (m_x30)
            m_x30->RSSetCurrentVehicle(m_leader);
    }
    return 1;
}


// ---- src/RaceStatusClass/Tick.cpp

int RaceStatusClass::Tick(float dt)
{
    if (m_flags & 1) {
        if (!(m_flags & 8)) {
            switch (m_raceType) {
            case 2:
                UpdateTrackRaceStatus(dt);
                break;
            case 1:
                UpdateWaypointRaceStatus(dt);
                break;
            case 4:
                UpdateTimeEliminationRaceStatus(dt);
                break;
            case 5:
                UpdateCopyCatRaceStatus(dt);
                break;
            case 6:
                UpdateGhostRaceStatus(dt);
                break;
            }
            if (m_raceType != 5)
                FindLeadPlayer();
            UpdateStunts(dt);
        }
    }
    return GameObject::Tick(dt);
}


// ---- src/RaceStatusClass/AddVehicle.cpp

extern "C" void *calloc(unsigned int n, unsigned int size);

struct SegmentList_Tag;



RaceStatusStruct_Tag *RaceStatusClass::AddVehicle(RaceStatusVehicleInterface *vehicle, int flags, int id)
{
    RaceStatusStruct_Tag *status;
    RaceStatusStruct_Tag **link;
    int count;
    if (!vehicle)
        return 0;
    if (!(status = VehicleInList(vehicle))) {
        link = &m_vehicles;
        count = 0;
        while (*link) {
            link = &(*link)->m_next;
            count++;
        }
        if (!(status = (RaceStatusStruct_Tag *)calloc(1, sizeof(RaceStatusStruct_Tag))))
            return 0;
        status->m_flags = flags;
        status->m_vehicle = vehicle;
        if (vehicle)
            vehicle->m_raceStatus = this;
        status->m_objects = m_objects;
        if (id >= 0)
            status->m_id = id;
        else
            status->m_id = count;
        status->m_place = count + 1;
        status->m_x6A4 = 0;
        if (m_raceType == 2 && m_x4C) {
            vehicle->RSGetVehicleStatus(&status->m_position, 0, 0, 0);
            status->m_lastPosition = status->m_position;
            m_x4C->FindClosest(status->m_position, &status->m_trackPosition, 0);
            status->m_lastTrackPosition = status->m_trackPosition;
            status->m_onTrack = m_x4C->OnTrack(status->m_position, status->m_lastTrackPosition, -1.0f, -1.0f, 0, 0, 0);
        } else if (m_raceType == 5 && !m_vehicles) {
            status->m_flags |= 0x180;
            status->m_timeRemaining = m_x70;
            m_leader = status;
            if (m_x30)
                m_x30->RSSetCurrentVehicle(m_leader);
        }
        *link = status;
    }
    return status;
}


// ---- src/RaceStatusClass/AddGhostVehicle.cpp

RaceStatusStruct_Tag *RaceStatusClass::AddGhostVehicle(RaceStatusStruct_Tag *target, RaceStatusVehicleInterface *ghost, int flags)
{
    if (!target || !target->m_vehicle || !ghost)
        return 0;
    return target->m_vehicle->m_status = AddVehicle(ghost, flags, -1);
}


// ---- src/RaceStatusClass/StartRace.cpp

int RaceStatusClass::StartRace()
{
    if (m_raceType != 6)
        EnableAllVehicles();
    m_flags |= 1;
    if (m_flags & 0x40) {
        for (RaceStatusStruct_Tag *v = m_vehicles; v; v = v->m_next) {
            if (v->m_gatesPassed)
                free(v->m_gatesPassed);
            if (!(v->m_gatesPassed = (int *)calloc(m_numGates, sizeof(int))))
                return 0;
        }
    }
    return 1;
}


// ---- src/RaceStatusClass/AddGate.cpp

struct SegmentList_Tag;


GateClass *RaceStatusClass::AddGate(_D3DVECTOR left, _D3DVECTOR right, _D3DVECTOR normal, float width,
                                    float height, float depth, int type)
{
    if (!m_objectsTail)
        return 0;
    GateClass *gate = (new GateClass(1))->Create(type, &left, &right, &normal, &width, &height, &depth);
    *m_objectsTail = gate;
    m_objectsTail = &gate->m_next;
    if (m_x4C) {
        RaceStatusBlock *block;
        if (!(block = (RaceStatusBlock *)calloc(1, sizeof(RaceStatusBlock))))
            return 0;
        if (!m_x4C->FindClosest(left, &block->m_position, 0))
            return 0;
        if (block->m_position.m_segment->m_length < 0.0f)
            return 0;
        *m_blocksTail = block;
        m_blocksTail = &block->m_next;
    }
    m_numGates++;
    return gate;
}


// ---- src/RaceStatusClass/VehicleInList.cpp

RaceStatusStruct_Tag *RaceStatusClass::VehicleInList(RaceStatusVehicleInterface *vehicle)
{
    for (RaceStatusStruct_Tag *status = m_vehicles; status; status = status->m_next) {
        if (status->m_vehicle == vehicle)
            return status;
    }
    return 0;
}


// ---- src/RaceStatusClass/FreeRaceStatusList.cpp

int RaceStatusClass::FreeRaceStatusList()
{
    RaceStatusStruct_Tag *v;
    while ((v = m_vehicles) != 0) {
        m_vehicles = v->m_next;
        if (v->m_gatesPassed)
            free(v->m_gatesPassed);
        free(v);
    }
    return 0;
}


// ---- src/RaceStatusClass/UpdateTrackRaceStatus.cpp

extern "C" void *memset(void *dst, int c, unsigned int n);
extern "C" int rand();


struct Vector {
    float x, y, z, w;
    Vector() { x = y = z = 0.0f; w = 1.0f; }
    Vector(float a, float b, float c) { x = a; y = b; z = c; w = 1.0f; }
};

static inline Vector Delta(const _D3DVECTOR &a, const _D3DVECTOR &b)
{
    return Vector(a.x - b.x, a.y - b.y, a.z - b.z);
}

static inline float RSqrt(float x)
{
    float r;
    asm("rsqrt.s $f8, %0, %1; swc1 $f8, 0(%2)" : : "f"(1.0f), "f"(x), "r"(&r) : "$f8", "memory");
    return r;
}

static inline Vector Scale(float s, const _D3DVECTOR &v)
{
    return Vector(s * v.x, s * v.y, s * v.z);
}

static inline void Sub(_D3DVECTOR &a, const Vector &b)
{
    a.x -= b.x;
    a.y -= b.y;
    a.z -= b.z;
}

static inline float Length(const Vector &v)
{
    float len2 = v.x * v.x + v.y * v.y + v.z * v.z;
    if (len2 == 0.0f)
        return 0.0f;
    return 1.0f / RSqrt(len2);
}

// Crossing the finish line: counts the lap, records its time and checks for
// the end of the race.
inline int TrackLapCompleted(RaceStatusClass *race, RaceStatusStruct_Tag *status)
{
    if (!status)
        return 0;
    int *lap = &status->m_lap;
    float *lastLap = &status->m_lastLap;
    float *lapStart = &status->m_x6D8;
    float *lapTime = &status->m_x38;
    float *now = &status->m_x6D4;
    float *bestLap = &status->m_bestLap;
    float *lapTimes = status->m_lapTimes;
    if (race->m_x4C->m_endGate->Crossed(&status->m_position, &status->m_lastPosition) == 1) {
        (*lap)++;
        *lapStart = 0;
        *lastLap = *now - *lapTime;
        if (*lastLap < 0.0f) {
            *lastLap = 0;
            return 0;
        }
        lapTimes[status->m_lap % 100] = *now;
        if (*bestLap == 0.0f || *lastLap < *bestLap) {
            *bestLap = *lastLap;
            if (status->m_vehicle)
                status->m_vehicle->RSBestLapCallback(race, status);
            if (race->m_x30)
                race->m_x30->RSBestLapCallback(race, status);
        }
        *lapTime = *now;
        if (status->m_vehicle)
            status->m_vehicle->RSLapUpdateCallback(race, status);
        if (race->m_x30)
            race->m_x30->RSLapUpdateCallback(race, status);
        if (race->m_x44 != 2 && *lap >= race->m_x64) {
            status->m_flags |= 1;
            if (status->m_vehicle)
                status->m_vehicle->RSFinishedRaceCallback(race, status);
            if (race->m_x30)
                race->m_x30->RSFinishedRaceCallback(race, status);
        }
        if (status->m_gatesPassed)
            memset(status->m_gatesPassed, 0, race->m_numGates * 4);
        return 1;
    }
    return 0;
}

// Orders the vehicles by lap, then by distance to the finish line, and
// reports place changes.
inline int SortByTrackPosition(RaceStatusClass *race)
{
    RaceStatusStruct_Tag *s;
    if (!race->m_vehicles || !race->m_x4C)
        return 0;
    for (s = race->m_vehicles; s; s = s->m_next) {
        s->m_x10 = race->m_x4C->DistanceBetween(s->m_trackPosition, race->m_x4C->m_finish);
        if (s->m_x10 < 0.0f)
            return 0;
        float d = race->m_x4C->DistanceBetween(s->m_trackPosition, s->m_lastTrackPosition);
        if (!(d <= 0.0f) && (d < s->m_x14 || s->m_x14 < 0.0f) && !(d <= s->m_x10))
            s->m_x28 = -1;
        else
            s->m_x28 = 0;
    }
    RaceStatusStruct_Tag **pp = &race->m_vehicles;
    while (*pp && (*pp)->m_next) {
        if (!((*pp)->m_flags & 1)) {
            RaceStatusStruct_Tag **qq;
            int laps;
            laps = (*pp)->m_lap + (*pp)->m_x28;
            qq = &(*pp)->m_next;
            while (*qq) {
                int l = (*qq)->m_lap + (*qq)->m_x28;
                if (l > laps || (l == laps && (*qq)->m_x10 < (*pp)->m_x10)) {
                    s = *qq;
                    *qq = s->m_next;
                    s->m_next = *pp;
                    *pp = s;
                    laps = (*pp)->m_lap + (*pp)->m_x28;
                }
                if (*qq)
                    qq = &(*qq)->m_next;
            }
        }
        if (*pp)
            pp = &(*pp)->m_next;
    }
    RaceStatusStruct_Tag *prev;
    int place;
    prev = 0;
    place = 1;
    for (s = race->m_vehicles; s; prev = s, s = s->m_next, place++) {
        if (s->m_place < place) {
            if (s->m_vehicle)
                s->m_vehicle->RSRaceOrderChangeCallback(race, s, prev, 1);
            if (race->m_x30)
                race->m_x30->RSRaceOrderChangeCallback(race, s, prev, 1);
        } else if (place < s->m_place) {
            if (s->m_vehicle)
                s->m_vehicle->RSRaceOrderChangeCallback(race, s, s->m_next, 0);
            if (race->m_x30)
                race->m_x30->RSRaceOrderChangeCallback(race, s, s->m_next, 0);
        }
        if (place == 1 && s->m_place != 1) {
            if (s->m_vehicle)
                s->m_vehicle->RSVehicleTakesLeadCallback(race, s);
            if (race->m_x30)
                race->m_x30->RSVehicleTakesLeadCallback(race, s);
        }
        s->m_place = place;
    }
    return 1;
}

// Each vehicle's time behind the leader.
inline int UpdateTimeBehind(RaceStatusClass *race)
{
    RaceStatusStruct_Tag *s;
    if (!race->m_vehicles)
        return 0;
    float inv = race->m_vehicles->m_distance > 0.0f ? 1.0f / race->m_vehicles->m_distance : 0.0f;
    for (s = race->m_vehicles->m_next; s; s = s->m_next) {
        if (!(s->m_flags & 1)) {
            s->m_x6DC = s->m_x6D4 - inv * (race->m_vehicles->m_x6D4 * s->m_distance);
            if (s->m_x6DC < 0.0f)
                s->m_x6DC = 0;
        }
    }
    if (race->m_vehicles->m_next)
        race->m_vehicles->m_x6DC = -race->m_vehicles->m_next->m_x6DC;
    else
        race->m_vehicles->m_x6DC = 0;
    return 1;
}

int RaceStatusClass::UpdateTrackRaceStatus(float dt)
{
    _D3DVECTOR unused;
    _D3DVECTOR position;
    _D3DVECTOR direction;
    RaceStatusStruct_Tag *status;

    if (!m_vehicles || !m_x4C)
        return 0;
    if (m_x44 != 2) {
        m_flags |= 2;
        for (status = m_vehicles; status; status = status->m_next) {
            if (!(status->m_flags & 1) && (status->m_flags & 4))
                m_flags &= ~2;
        }
    }
    for (status = m_vehicles; status; status = status->m_next) {
        if (!(status->m_flags & 1) && (status->m_flags & 2)) {
            if (status->m_vehicle)
                status->m_vehicle->RSGetVehicleStatus(&status->m_position, 0, 0, 0);
            status->m_onTrack = m_x4C->OnTrack(status->m_position, status->m_lastTrackPosition, m_x4C->m_behind,
                                               m_x4C->m_ahead, 0, &status->m_trackPosition, &status->m_x14);
            if (!(status->m_trackPosition.m_data->m_position.y - status->m_position.y <= m_x84))
                status->m_onTrack = 0;
            if ((status->m_flags & 4) && status->m_lastTrackPosition.m_segment &&
                (status->m_lastTrackPosition.m_segment->m_flags & 0x40) && status->m_lastTrackPosition.m_data->m_next &&
                !status->m_lastTrackPosition.m_data->m_next->m_next)
                status->m_lastTrackPosition.m_segment->m_flags &= ~0x40;
            GateClass *gate;
            if ((m_flags & 0x40) && (gate = m_objects)) {
                RaceStatusBlock *block = m_blocks;
                float best = m_x88;
                int i = 0;
                status->m_objects = 0;
                for (; gate; gate = gate->m_next, block = block->m_next, i++) {
                    if (gate->m_flags & 1) {
                        float d = Length(Delta(status->m_position, gate->m_center));
                        if (d < best) {
                            status->m_objects = gate;
                            best = d;
                            status->m_x6A4 = i;
                        }
                    }
                }
                if (status->m_objects && status->m_gatesPassed && !status->m_gatesPassed[status->m_x6A4]) {
                    int crossed = status->m_objects->Crossed(&status->m_position, &status->m_lastPosition);
                    // Retail passes the search cursor, which is null by now.
                    if (crossed == 1) {
                        status->m_vehicle->RSGatePassedCallback(this, gate);
                        status->m_gatesPassed[status->m_x6A4] = 1;
                    } else if (crossed == 2) {
                        status->m_vehicle->RSGateMissedCallback(this, gate);
                        status->m_gatesPassed[status->m_x6A4] = 1;
                    }
                }
            }
            if (status->m_onTrack) {
                status->m_x0C = m_x4C->DistanceBetween(status->m_lastTrackPosition, m_x4C->m_finish);
                status->m_x10 = m_x4C->DistanceBetween(status->m_trackPosition, m_x4C->m_finish);
                if (status->m_x0C != 0.0f && status->m_x0C < m_x4C->m_behind &&
                    !(status->m_x10 - status->m_x0C <= m_x4C->m_ahead)) {
                    if (!(m_x4C->DistanceBetween(m_x4C->m_finish, status->m_trackPosition) < 5.0f))
                        status->m_onTrack = 0;
                }
                if (m_x4C->m_behind < 0.0f ||
                    (!(status->m_x0C < 0.0f) && status->m_x0C < m_x4C->m_behind)) {
                    if (!(status->m_lastPosition.x < 0.0f)) {
                        if (TrackLapCompleted(this, status)) {
                            status->m_lastTrackPosition.m_segment = m_x4C->m_finish.m_segment;
                            status->m_lastTrackPosition.m_data = m_x4C->m_finish.m_data;
                            status->m_lastTrackPosition.m_t = m_x4C->m_finish.m_t + 1e-05f;
                            status->m_trackPosition.m_segment = status->m_lastTrackPosition.m_segment;
                            status->m_trackPosition.m_data = status->m_lastTrackPosition.m_data;
                            status->m_trackPosition.m_t = status->m_lastTrackPosition.m_t;
                            status->m_onTrack = 1;
                            for (gate = m_objects; gate; gate = gate->m_next)
                                gate->m_flags |= 1;
                        }
                        if ((status->m_flags & 1) && (status->m_flags & 4))
                            m_flags |= 4;
                    }
                }
                if ((!(status->m_x14 <= 0.0f) && status->m_x14 < m_x4C->m_behind) || status->m_onTrack == 2) {
                    if (status->m_x14 < status->m_x0C || status->m_x0C == 0.0f) {
                        float lap = status->m_lap;
                        status->m_distance = m_x4C->m_lapLength + lap * m_x4C->m_lapLength - status->m_x10;
                        status->m_lastTrackPosition.m_segment = status->m_trackPosition.m_segment;
                        status->m_lastTrackPosition.m_data = status->m_trackPosition.m_data;
                        status->m_lastTrackPosition.m_t = status->m_trackPosition.m_t;
                    }
                }
                if (status->m_onTrack)
                    status->m_x6E0 = 0;
                else
                    status->m_x6E0 += dt;
            } else {
                status->m_x6E0 += dt;
            }
            status->m_lastPosition.x = status->m_position.x;
            status->m_lastPosition.y = status->m_position.y;
            status->m_lastPosition.z = status->m_position.z;
            if ((m_flags & 2) && !(status->m_flags & 1)) {
                status->m_x6D4 = m_vehicles->m_x6D4 + status->m_x6DC;
                status->m_lastLap = (status->m_x6DC + status->m_x38) / (m_x64 - status->m_lap);
                if (status->m_lastLap < status->m_bestLap)
                    status->m_bestLap = status->m_lastLap;
                status->m_lap = m_x64;
                status->m_flags |= 1;
            } else if (!(status->m_flags & 1)) {
                status->m_x6D4 += dt;
                status->m_x6D8 += dt;
            }
        }
        if (status->m_onTrack && status->m_lastTrackPosition.m_segment && status->m_lastTrackPosition.m_data &&
            status->m_lastTrackPosition.m_data->m_next) {
            TrackPosition_Tag ahead;
            ahead.m_segment = status->m_lastTrackPosition.m_segment;
            ahead.m_data = status->m_lastTrackPosition.m_data;
            ahead.m_t = status->m_lastTrackPosition.m_t;
            while (ahead.m_data && ahead.m_data->m_next && (*(int *)((char *)ahead.m_data + 4) & 1) /* node flags */ &&
                   ahead.m_data != m_x4C->m_finish.m_data) {
                ahead.m_data = ahead.m_data->m_next;
                ahead.m_t = 0;
            }
            if (!ahead.m_data->m_next) {
                ahead.m_data = ahead.m_data->m_prev;
                ahead.m_t = 1.0f;
            }
            status->m_x6C0.m_segment = ahead.m_segment;
            status->m_x6C0.m_data = ahead.m_data;
            status->m_x6C0.m_t = ahead.m_t;
            // The redundant cast reproduces retail's argument load order.
            if (!m_x4C->CoordinatesFromNode(&ahead, (_D3DVECTOR *)&position))
                position = status->m_position;
            if (!m_x4C->GetTrackDirection(ahead.m_data, &direction))
                direction = _D3DVECTOR();
            if (status->m_x0C < 10.0f) {
                float back = 10.0f - status->m_x0C;
                Sub(position, Scale(back, direction));
            }
            if (status->m_vehicle)
                status->m_vehicle->RSSetResetParameters(&position, &direction);
        }
    }
    if ((m_flags & 4) && !(m_flags & 2) && m_x6C > 0.0f) {
        if (!(m_x68 < m_x6C))
            m_flags |= 2;
        else
            m_x68 += dt;
    }
    if (m_flags & 2) {
        for (status = m_vehicles; status; status = status->m_next) {
            if (status->m_next && !(status->m_x6D4 <= status->m_next->m_x6D4))
                status->m_next->m_x6D4 = status->m_x6D4 + 4.656613e-10f * rand();
            if (status->m_vehicle)
                status->m_vehicle->RSRaceOverCallback();
        }
        if (m_x30)
            m_x30->RSRaceOverCallback();
    }
    if (!SortByTrackPosition(this))
        return 0;
    if (!(m_flags & 0x10)) {
        if (!UpdateTimeBehind(this))
            return 0;
    }
    if (m_vehicles->m_flags & 1)
        m_flags |= 0x10;
    return 1;
}


// ---- src/RaceStatusClass/UpdateGhostRaceStatus.cpp

extern "C" void *memset(void *dst, int c, unsigned int n);


// A ghost crossing the finish line: records the lap, hands its best lap to
// the vehicle it races, and resets both for the next lap.
inline int GhostLapCompleted(RaceStatusClass *race, RaceStatusStruct_Tag *status)
{
    if (!status)
        return 0;
    float *lastLap = &status->m_lastLap;
    float *lapTime = &status->m_x6D4;
    float *bestLap = &status->m_bestLap;
    if (race->m_x4C->m_endGate->Crossed(&status->m_position, &status->m_lastPosition) == 1) {
        *lastLap = *lapTime;
        if (*lastLap < 0.0f) {
            *lastLap = 0;
            return 0;
        }
        if (*bestLap == 0.0f || *lastLap < *bestLap) {
            *bestLap = *lastLap;
            if (status->m_vehicle)
                status->m_vehicle->RSBestLapCallback(race, status);
            if (race->m_x30)
                race->m_x30->RSBestLapCallback(race, status);
        }
        if (status->m_vehicle)
            status->m_vehicle->RSLapUpdateCallback(race, status);
        if (race->m_x30)
            race->m_x30->RSLapUpdateCallback(race, status);
        status->m_flags &= ~2;
        RaceStatusStruct_Tag *target;
        if (status->m_vehicle && (target = status->m_vehicle->m_status)) {
            if (race->m_x30)
                race->m_x30->RSStopGhostVehicle(race, target);
            if (*bestLap < target->m_bestLap || target->m_bestLap == 0.0f) {
                target->m_bestLap = *bestLap;
                if (race->m_x30)
                    race->m_x30->RSUpdateBestGhostLap(race, status, target);
            }
            status->m_x6D8 = 0;
            status->m_x38 = 0;
            status->m_x6D4 = 0;
            status->m_distance = 0;
            target->m_lastLap = 0;
            target->m_x6D8 = 0;
            target->m_x38 = 0;
            target->m_x6D4 = 0;
            target->m_distance = 0;
            if (race->m_x4C->m_startGate) {
                target->m_lastTrackPosition.m_segment = race->m_x4C->m_startTrackPosition.m_segment;
                target->m_lastTrackPosition.m_data = race->m_x4C->m_startTrackPosition.m_data;
                target->m_lastTrackPosition.m_t = race->m_x4C->m_startTrackPosition.m_t;
            }
            target->m_flags &= ~2;
        }
        if (status->m_gatesPassed)
            memset(status->m_gatesPassed, 0, race->m_numGates * 4);
        return 1;
    }
    return 0;
}

// How far each ghost-racing vehicle trails its ghost, in seconds.
inline int GhostUpdateTimeBehind(RaceStatusClass *race)
{
    RaceStatusStruct_Tag *status;
    if (!race->m_vehicles)
        return 0;
    for (status = race->m_vehicles; status; status = status->m_next) {
        RaceStatusStruct_Tag *ghost;
        if ((status->m_flags & 4) && status->m_vehicle && (ghost = status->m_vehicle->m_status) &&
            status->m_x6D8 <= ghost->m_bestLap) {
            if (ghost->m_distance > 0.0f) {
                float inv = 1.0f / ghost->m_distance;
                status->m_x6DC = status->m_x6D4 - inv * (ghost->m_x6D4 * status->m_distance);
            } else
                status->m_x6DC = 0;
        }
    }
    return 1;
}

int RaceStatusClass::UpdateGhostRaceStatus(float dt)
{
    _D3DVECTOR unused;
    _D3DVECTOR position;
    _D3DVECTOR direction;
    RaceStatusStruct_Tag *status;

    if (!m_vehicles || !m_x4C)
        return 0;
    for (status = m_vehicles; status; status = status->m_next) {
        if (status->m_flags & 2) {
            if (status->m_vehicle)
                status->m_vehicle->RSGetVehicleStatus(&status->m_position, 0, 0, 0);
            status->m_onTrack = m_x4C->OnTrack(status->m_position, status->m_lastTrackPosition, m_x4C->m_behind,
                                               m_x4C->m_ahead, 0, &status->m_trackPosition, &status->m_x14);
            GateClass *gate;
            if ((m_flags & 0x40) && (gate = m_objects)) {
                RaceStatusBlock *block = m_blocks;
                float best = m_x88;
                int i = 0;
                status->m_objects = 0;
                for (; gate; gate = gate->m_next, block = block->m_next, i++) {
                    if (gate->m_flags & 1) {
                        float d = Length(Delta(status->m_position, gate->m_center));
                        if (d < best) {
                            status->m_objects = gate;
                            best = d;
                            status->m_x6A4 = i;
                        }
                    }
                }
                if (status->m_objects && status->m_gatesPassed && !status->m_gatesPassed[status->m_x6A4]) {
                    int crossed = status->m_objects->Crossed(&status->m_position, &status->m_lastPosition);
                    // Retail passes the search cursor, which is null by now.
                    if (crossed == 1) {
                        status->m_vehicle->RSGatePassedCallback(this, gate);
                        status->m_gatesPassed[status->m_x6A4] = 1;
                    } else if (crossed == 2) {
                        status->m_vehicle->RSGateMissedCallback(this, gate);
                        status->m_gatesPassed[status->m_x6A4] = 1;
                    }
                }
            }
            if (status->m_onTrack) {
                status->m_x6E0 = 0;
                status->m_x0C = m_x4C->DistanceBetween(status->m_lastTrackPosition, m_x4C->m_finish);
                status->m_x10 = m_x4C->DistanceBetween(status->m_trackPosition, m_x4C->m_finish);
                if (status->m_x0C != 0.0f && status->m_x0C < m_x4C->m_behind &&
                    !(status->m_x10 - status->m_x0C <= m_x4C->m_ahead)) {
                    if (!(m_x4C->DistanceBetween(m_x4C->m_finish, status->m_trackPosition) < 5.0f))
                        status->m_onTrack = 0;
                }
                if (status->m_flags & 4) {
                    if (m_x4C->m_behind < 0.0f ||
                        (!(status->m_x0C < 0.0f) && status->m_x0C < m_x4C->m_behind)) {
                        if (!(status->m_lastPosition.x < 0.0f)) {
                            if (GhostLapCompleted(this, status)) {
                                status->m_lastTrackPosition.m_segment = m_x4C->m_finish.m_segment;
                                status->m_lastTrackPosition.m_data = m_x4C->m_finish.m_data;
                                status->m_lastTrackPosition.m_t = m_x4C->m_finish.m_t + 1e-05f;
                                status->m_trackPosition.m_segment = status->m_lastTrackPosition.m_segment;
                                status->m_trackPosition.m_data = status->m_lastTrackPosition.m_data;
                                status->m_trackPosition.m_t = status->m_lastTrackPosition.m_t;
                                status->m_onTrack = 1;
                            }
                        }
                    }
                }
                if ((!(status->m_x14 <= 0.0f) && status->m_x14 < m_x4C->m_behind) || status->m_onTrack == 2) {
                    if (status->m_x14 < status->m_x0C || status->m_x0C == 0.0f) {
                        { float lap = status->m_lap; status->m_distance = m_x4C->m_lapLength + lap * m_x4C->m_lapLength - status->m_x10; }
                        status->m_lastTrackPosition.m_segment = status->m_trackPosition.m_segment;
                        status->m_lastTrackPosition.m_data = status->m_trackPosition.m_data;
                        status->m_lastTrackPosition.m_t = status->m_trackPosition.m_t;
                    }
                }
            } else {
                status->m_x6E0 += dt;
            }
            status->m_lastPosition.x = status->m_position.x;
            status->m_lastPosition.y = status->m_position.y;
            status->m_lastPosition.z = status->m_position.z;
            status->m_x6D4 += dt;
            status->m_x6D8 += dt;
        } else {
            status->m_lastPosition = status->m_position;
            status->m_onTrack = 1;
            if (status->m_vehicle)
                status->m_vehicle->RSGetVehicleStatus(&status->m_position, 0, 0, 0);
            if (m_x4C && m_x4C->m_startGate &&
                m_x4C->m_startGate->Crossed(&status->m_position, &status->m_lastPosition) == 1) {
                status->m_flags |= 2;
                if (status->m_vehicle && status->m_vehicle->m_status) {
                    status->m_vehicle->m_status->m_flags |= 2;
                    if (m_x30)
                        m_x30->RSStartGhostVehicle(this, status->m_vehicle->m_status);
                }
            }
        }
        if (status->m_onTrack && status->m_lastTrackPosition.m_segment && status->m_lastTrackPosition.m_data &&
            status->m_lastTrackPosition.m_data->m_next) {
            TrackPosition_Tag ahead;
            ahead.m_segment = status->m_lastTrackPosition.m_segment;
            ahead.m_data = status->m_lastTrackPosition.m_data;
            ahead.m_t = status->m_lastTrackPosition.m_t;
            while (ahead.m_data && ahead.m_data->m_next && (*(int *)((char *)ahead.m_data + 4) & 1) /* node flags */ &&
                   ahead.m_data != m_x4C->m_finish.m_data) {
                ahead.m_data = ahead.m_data->m_next;
                ahead.m_t = 0;
            }
            if (!ahead.m_data->m_next) {
                ahead.m_data = ahead.m_data->m_prev;
                ahead.m_t = 1.0f;
            }
            status->m_x6C0.m_segment = ahead.m_segment;
            status->m_x6C0.m_data = ahead.m_data;
            status->m_x6C0.m_t = ahead.m_t;
            // The redundant cast reproduces retail's argument load order.
            if (!m_x4C->CoordinatesFromNode(&ahead, (_D3DVECTOR *)&position))
                position = status->m_position;
            if (!m_x4C->GetTrackDirection(ahead.m_data, &direction))
                direction = _D3DVECTOR();
            if (status->m_vehicle)
                status->m_vehicle->RSSetResetParameters(&position, &direction);
        }
    }
    if (!(m_flags & 0x10)) {
        if (!GhostUpdateTimeBehind(this))
            return 0;
    }
    return 1;
}


// ---- src/RaceStatusClass/UpdateWaypointRaceStatus.cpp

extern "C" int rand();


static inline float Sqrt(float x)
{
    float r;
    asm("sqrt.s $f8, %1; swc1 $f8, 0(%0)" : : "r"(&r), "f"(x) : "$f8", "memory");
    return r;
}

static inline float LengthSqrt(const Vector &v)
{
    return Sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

// The vehicle's next gate: on crossing it, moves on (a lap ends after the
// last gate) and checks for the end of the race.
inline int WaypointUpdate(RaceStatusClass *race, RaceStatusStruct_Tag *status)
{
    if (!status)
        return 0;
    float *gateTimes = status->m_gateTimes;
    GateClass **gate = &status->m_objects;
    int *gates = &status->m_gates;
    int *lap = &status->m_lap;
    unsigned int *flags = &status->m_flags;
    float *lastLap = &status->m_lastLap;
    float *bestLap = &status->m_bestLap;
    float *lapTime = &status->m_x38;
    float now = status->m_x6D4;
    float *lapStart = &status->m_x6D8;
    int crossed = status->m_objects->Crossed(&status->m_position, &status->m_lastPosition);
    if (crossed == 1) {
        if (status->m_vehicle)
            status->m_vehicle->RSGatePassedCallback(race, *gate);
        if (race->m_x30)
            race->m_x30->RSGatePassedCallback(race, status, *gate);
        *gate = (*gate)->m_next;
        if (!*gate) {
            *gate = race->m_objects;
            (*lap)++;
            *lapStart = 0;
            *lastLap = now - *lapTime;
            if (*lastLap < 0.0f) {
                *lastLap = 0;
                return 0;
            }
            if (*lastLap < *bestLap || *bestLap <= 0.0f) {
                *bestLap = *lastLap;
                if (status->m_vehicle)
                    status->m_vehicle->RSBestLapCallback(race, status);
                if (race->m_x30)
                    race->m_x30->RSBestLapCallback(race, status);
            }
            *lapTime = now;
            if (status->m_vehicle)
                status->m_vehicle->RSLapUpdateCallback(race, status);
            if (race->m_x30)
                race->m_x30->RSLapUpdateCallback(race, status);
        }
        (*gates)++;
        gateTimes[*gates % 600] = now;  // retail wraps at 600, the array holds 300
        if (status->m_vehicle)
            status->m_vehicle->RSGateUpdateCallback(race, status);
        if (race->m_x30)
            race->m_x30->RSGateUpdateCallback(race, status);
    } else if (crossed == 2) {
        if (status->m_vehicle)
            status->m_vehicle->RSGateMissedCallback(race, *gate);
        if (race->m_x30)
            race->m_x30->RSGateMissedCallback(race, status, *gate);
    }
    if (race->m_x44 != 2 && *lap >= race->m_x64) {
        *flags |= 1;
        if (status->m_vehicle)
            status->m_vehicle->RSFinishedRaceCallback(race, status);
        if (race->m_x30)
            race->m_x30->RSFinishedRaceCallback(race, status);
    }
    return 1;
}

// Orders the vehicles by gates passed, then by distance to their next gate,
// and reports place changes.
inline int SortByGates(RaceStatusClass *race)
{
    RaceStatusStruct_Tag *s;
    if (!race->m_vehicles)
        return 0;
    for (s = race->m_vehicles; s; s = s->m_next) {
        float dz = s->m_objects->m_center.z - s->m_position.z;
        float dx = s->m_objects->m_center.x - s->m_position.x;
        s->m_x10 = dz * dz + dx * dx;
        if (s->m_x10 < 0.0f)
            return 0;
    }
    RaceStatusStruct_Tag **pp = &race->m_vehicles;
    while (*pp && (*pp)->m_next) {
        if (!((*pp)->m_flags & 1)) {
            RaceStatusStruct_Tag **qq;
            int gates;
            gates = (*pp)->m_gates;
            qq = &(*pp)->m_next;
            while (*qq) {
                int g = (*qq)->m_gates;
                if (g > gates || (g == gates && (*qq)->m_x10 < (*pp)->m_x10)) {
                    s = *qq;
                    *qq = s->m_next;
                    s->m_next = *pp;
                    *pp = s;
                    gates = (*pp)->m_gates;
                }
                if (*qq)
                    qq = &(*qq)->m_next;
            }
        }
        if (*pp)
            pp = &(*pp)->m_next;
    }
    RaceStatusStruct_Tag *prev;
    int place;
    prev = 0;
    place = 1;
    for (s = race->m_vehicles; s; prev = s, s = s->m_next, place++) {
        if (s->m_place < place) {
            if (s->m_vehicle)
                s->m_vehicle->RSRaceOrderChangeCallback(race, s, prev, 1);
            if (race->m_x30)
                race->m_x30->RSRaceOrderChangeCallback(race, s, prev, 1);
        } else if (place < s->m_place) {
            if (s->m_vehicle)
                s->m_vehicle->RSRaceOrderChangeCallback(race, s, s->m_next, 0);
            if (race->m_x30)
                race->m_x30->RSRaceOrderChangeCallback(race, s, s->m_next, 0);
        }
        if (place == 1 && s->m_place != 1) {
            if (s->m_vehicle)
                s->m_vehicle->RSVehicleTakesLeadCallback(race, s);
            if (race->m_x30)
                race->m_x30->RSVehicleTakesLeadCallback(race, s);
        }
        s->m_place = place;
    }
    return 1;
}

// Each vehicle's time behind the leader.

int RaceStatusClass::UpdateWaypointRaceStatus(float dt)
{
    RaceStatusStruct_Tag *status;

    if (!m_vehicles || !m_objects)
        return 0;
    m_flags |= 2;
    for (status = m_vehicles; status; status = status->m_next) {
        if (!(status->m_flags & 1) && (status->m_flags & 4))
            m_flags &= ~2;
    }
    for (status = m_vehicles; status; status = status->m_next) {
        if (status->m_flags & 1)
            continue;
        if (!(status->m_lastPosition.x < 0.0f)) {
            if (!WaypointUpdate(this, status))
                return 0;
            if ((status->m_flags & 1) && (status->m_flags & 4))
                m_flags |= 4;
        }
        float before = LengthSqrt(Delta(status->m_objects->m_center, status->m_lastPosition));
        float after = LengthSqrt(Delta(status->m_objects->m_center, status->m_position));
        status->m_distance += before - after;
        status->m_x0C = after;
        status->m_lastPosition.x = status->m_position.x;
        status->m_lastPosition.y = status->m_position.y;
        status->m_lastPosition.z = status->m_position.z;
        if ((m_flags & 2) && !(status->m_flags & 1)) {
            status->m_x6D4 = m_vehicles->m_x6D4 + status->m_x6DC;
            if (status->m_lap < m_x64)
                status->m_lastLap = (status->m_x6D4 - status->m_x38) / (m_x64 - status->m_lap);
            else
                status->m_lastLap = status->m_x6D4 - status->m_x38;
            if (status->m_lastLap < status->m_bestLap)
                status->m_bestLap = status->m_lastLap;
            status->m_lap = m_x64;
            status->m_flags |= 1;
        } else if (!(status->m_flags & 1)) {
            status->m_x6D4 += dt;
            status->m_x6D8 += dt;
        }
    }
    if ((m_flags & 4) && (m_flags & 2) && m_x6C > 0.0f) {
        if (!(m_x68 < m_x6C))
            m_flags |= 2;
        else
            m_x68 += dt;
    }
    if (m_flags & 2) {
        for (status = m_vehicles; status; status = status->m_next) {
            if (status->m_next && !(status->m_x6D4 <= status->m_next->m_x6D4))
                status->m_next->m_x6D4 = status->m_x6D4 + 4.656613e-10f * rand();
        }
    }
    if (!SortByGates(this))
        return 0;
    if (!(m_vehicles->m_flags & 1)) {
        if (!UpdateTimeBehind(this))
            return 0;
    }
    return 1;
}


// ---- src/RaceStatusClass/UpdateTimeEliminationRaceStatus.cpp


// Orders the vehicles by time remaining (most first, finished ones stay put)
// and reports place changes.
inline int SortByTimeRemaining(RaceStatusClass *race)
{
    if (!race->m_vehicles)
        return 0;
    RaceStatusStruct_Tag **pp = &race->m_vehicles;
    while (*pp && (*pp)->m_next) {
        if (!((*pp)->m_flags & 1)) {
            RaceStatusStruct_Tag **qq = &(*pp)->m_next;
            while (*qq) {
                RaceStatusStruct_Tag *q = *qq;
                if (!(q->m_timeRemaining <= (*pp)->m_timeRemaining)) {
                    *qq = q->m_next;
                    q->m_next = *pp;
                    *pp = q;
                }
                if (*qq)
                    qq = &(*qq)->m_next;
            }
        }
        if (*pp)
            pp = &(*pp)->m_next;
    }
    RaceStatusStruct_Tag *s;
    RaceStatusStruct_Tag *prev;
    int place;
    prev = 0;
    place = 1;
    for (s = race->m_vehicles; s; prev = s, s = s->m_next, place++) {
        if (s->m_place < place) {
            if (s->m_vehicle)
                s->m_vehicle->RSRaceOrderChangeCallback(race, s, prev, 1);
            if (race->m_x30)
                race->m_x30->RSRaceOrderChangeCallback(race, s, prev, 1);
        } else if (place < s->m_place) {
            if (s->m_vehicle)
                s->m_vehicle->RSRaceOrderChangeCallback(race, s, s->m_next, 0);
            if (race->m_x30)
                race->m_x30->RSRaceOrderChangeCallback(race, s, s->m_next, 0);
        }
        if (place == 1 && s->m_place != 1) {
            if (s->m_vehicle)
                s->m_vehicle->RSVehicleTakesLeadCallback(race, s);
            if (race->m_x30)
                race->m_x30->RSVehicleTakesLeadCallback(race, s);
        }
        s->m_place = place;
    }
    return 1;
}

// Time-elimination race: tracks the nearest gate and its crossings, counts
// down each vehicle's time, then reorders the field by time remaining.
int RaceStatusClass::UpdateTimeEliminationRaceStatus(float dt)
{
    Vector unused;                      // constructed but never used, as in retail
    RaceStatusStruct_Tag *status;
    if (!m_vehicles)
        return 0;
    m_flags |= 2;
    for (status = m_vehicles; status; status = status->m_next) {
        if (!(status->m_flags & 1) && (status->m_flags & 4))
            m_flags &= ~2;
    }
    for (status = m_vehicles; status; status = status->m_next) {
        if (status->m_vehicle)
            status->m_vehicle->RSGetVehicleStatus(&status->m_position, 0, 0, 0);
        if (!(status->m_flags & 1)) {
            GateClass *gate;
            if ((m_flags & 0x40) && (gate = m_objects)) {
                RaceStatusBlock *block = m_blocks;
                float best = m_x88;
                int i = 0;
                status->m_objects = 0;
                for (; gate; gate = gate->m_next, block = block->m_next, i++) {
                    if (gate->m_flags & 1) {
                        float d = Length(Delta(status->m_position, gate->m_center));
                        if (d < best) {
                            status->m_objects = gate;
                            best = d;
                            status->m_x6A4 = i;
                        }
                    }
                }
                if (status->m_objects && status->m_gatesPassed && !status->m_gatesPassed[status->m_x6A4]) {
                    int crossed = ((GateClass *)status->m_objects)->Crossed(&status->m_position, &status->m_lastPosition);
                    // Retail passes the search cursor, which is null by now.
                    if (crossed == 1) {
                        status->m_vehicle->RSGatePassedCallback(this, (GateClass *)gate);
                        status->m_gatesPassed[status->m_x6A4] = 1;
                    } else if (crossed == 2) {
                        status->m_vehicle->RSGateMissedCallback(this, (GateClass *)gate);
                        status->m_gatesPassed[status->m_x6A4] = 1;
                    }
                }
            }
            status->m_timeRemaining -= dt;
            if (status->m_timeRemaining <= 0.0f) {
                status->m_timeRemaining = 0;
                status->m_flags |= 1;
                if (m_x30)
                    m_x30->RSFinishedRaceCallback(this, status);
                if (status->m_vehicle)
                    status->m_vehicle->RSFinishedRaceCallback(this, status);
            }
            if ((status->m_flags & 1) && (status->m_flags & 4))
                m_flags |= 4;
            status->m_lastPosition = status->m_position;
            if ((m_flags & 2) && !(status->m_flags & 1)) {
                status->m_x6D4 = m_vehicles->m_x6D4 + status->m_x6DC;
                status->m_flags |= 1;
            } else if (!(status->m_flags & 1)) {
                status->m_x6D4 += dt;
            }
        }
        if (status->m_vehicle)
            status->m_vehicle->RSSetResetParameters(&status->m_position, 0);
    }
    if ((m_flags & 4) && !(m_flags & 2) && m_x6C > 0.0f) {
        if (!(m_x68 < m_x6C))
            m_flags |= 2;
        else
            m_x68 += dt;
    }
    if (m_flags & 2) {
        if (m_x30)
            m_x30->RSRaceOverCallback();
    }
    if (!SortByTimeRemaining(this))
        return 0;
    if (!(m_flags & 0x10)) {
        for (status = m_vehicles; status; status = status->m_next) {
            if (status == m_vehicles && status->m_next)
                status->m_x6DC = status->m_next->m_timeRemaining - status->m_timeRemaining;
            else
                status->m_x6DC = m_vehicles->m_timeRemaining - status->m_timeRemaining;
        }
    }
    if (m_vehicles->m_flags & 1)
        m_flags |= 0x10;
    return 1;
}


// ---- src/RaceStatusClass/CopyCatRaceOver.cpp

int RaceStatusClass::CopyCatRaceOver()
{
    RaceStatusStruct_Tag *v;
    int active = 0;
    int finished = 0;
    m_flags |= 2;
    for (v = m_vehicles; v; v = v->m_next) {
        if (!(v->m_flags & 1)) {
            active++;
            if (v->m_flags & 4)
                finished++;
        }
    }
    if (active > 1 && finished > 0)
        m_flags &= ~2;
    return m_flags & 2;
}


// ---- src/RaceStatusClass/CopyCatUpdateTurn.cpp


// Copy-cat races: hands the turn (flag 0x80) to the next vehicle still in the
// game, starting a new round (flag 0x100) when the turn passes the leader.
int RaceStatusClass::CopyCatUpdateTurn(int newRound)
{
    RaceStatusStruct_Tag *current = m_vehicles;
    while (current && !(current->m_flags & 0x80))
        current = current->m_next;
    if (current) {
        if (newRound)
            current->m_flags &= ~0x100;
        RaceStatusStruct_Tag *next;
        if (current->m_next)
            next = current->m_next;
        else
            next = m_vehicles;
        while (next != current && ((next->m_flags & 1) || (next->m_flags & 0x100))) {
            if (next->m_flags & 0x100) {
                next->m_flags &= ~0x100;
                newRound = 1;
                FreeStuntList(next);
            }
            next = next->m_next;
            if (!next)
                next = m_vehicles;
        }
        if (next != current) {
            current->m_flags &= ~0x80;
            next->m_flags |= 0x80;
        }
        next->m_timeRemaining = m_x70;
        if (newRound) {
            next->m_flags |= 0x100;
            m_leader = next;
        }
        if (m_x30)
            m_x30->RSSetCurrentVehicle(next);
    }
    return 1;
}


// ---- src/RaceStatusClass/UpdateCopyCatRaceStatus.cpp


int RaceStatusClass::UpdateCopyCatRaceStatus(float dt)
{
    RaceStatusStruct_Tag *status = m_vehicles;
    if (!status)
        return 0;
    if (m_flags & 2)
        return 1;
    for (; status; status = status->m_next) {
        if (status->m_vehicle)
            status->m_vehicle->RSGetVehicleStatus(&status->m_position, 0, 0, 0);
        if ((status->m_flags & 1) || !(status->m_flags & 0x80))
            continue;
        status->m_timeRemaining -= dt;
        if (status->m_timeRemaining <= 0.0f) {
            status->m_timeRemaining = 0;
            if (!(status->m_flags & 0x100))
                status->m_x30++;
            if (!(status->m_x30 < m_x7C)) {
                status->m_flags |= 1;
                m_flags |= 4;
                if (m_x30)
                    m_x30->RSFinishedRaceCallback(this, status);
                if (status->m_vehicle)
                    status->m_vehicle->RSFinishedRaceCallback(this, status);
            }
            status->m_flags &= ~0x70;
            FreeStuntList(status);
            if (!CopyCatRaceOver())
                CopyCatUpdateTurn(status->m_flags & 0x100);
        }
        status->m_lastPosition = status->m_position;
        if ((m_flags & 2) && !(status->m_flags & 1))
            status->m_flags |= 1;
        else if (!(status->m_flags & 1))
            status->m_x6D4 += dt;
    }
    if ((m_flags & 4) && !(m_flags & 2) && m_x6C > 0.0f) {
        if (!(m_x68 < m_x6C))
            m_flags |= 2;
        else
            m_x68 += dt;
    }
    if (m_flags & 2) {
        if (m_x30)
            m_x30->RSRaceOverCallback();
    }
    return 1;
}


// ---- src/RaceStatusClass/FindLeadPlayer.cpp

int RaceStatusClass::FindLeadPlayer()
{
    m_leader = m_vehicles;
    while (m_leader && !(m_leader->m_flags & 4))
        m_leader = m_leader->m_next;
    return m_leader ? 1 : 0;
}


// ---- src/RaceStatusClass/EnableAllVehicles.cpp

int RaceStatusClass::EnableAllVehicles()
{
    for (RaceStatusStruct_Tag *status = m_vehicles; status; status = status->m_next)
        status->m_flags |= 2;
    return 1;
}


// ---- src/RaceStatusClass/Render3D.cpp

int RaceStatusClass::Render3D()
{
    if (m_flags & 0x20) {
        // Debug drawing of the race objects, compiled out of retail;
        // only the walk over the list survives.
        for (GateClass *obj = m_objects; obj; obj = obj->m_next) {
        }
    }
    return GameObject::Render3D();
}


// ---- src/RaceStatusClass/BeginStunts.cpp

int RaceStatusClass::BeginStunts(RaceStatusStruct_Tag *status)
{
    if (!status)
        return 0;
    if (m_flags & 1) {
        if (!(m_flags & 8)) {
            if (m_raceType != 5 || (status->m_flags & 0x80)) {
                if (status->m_flags & 2) {
                    status->m_flags |= 0x10;
                    status->m_x6F0 = 0;
                    status->m_numStunts = 0;
                }
            }
        }
    }
    return 1;
}


// ---- src/RaceStatusClass/FreeStuntList.cpp

int RaceStatusClass::FreeStuntList(RaceStatusStruct_Tag *status)
{
    if (!status)
        return 0;
    while (status->m_stunts) {
        RaceStatusStunt *stunt = status->m_stunts;
        status->m_stunts = stunt->m_next;
        if (stunt) {
            stunt->m_next = m_freeStunts;
            m_freeStunts = stunt;
        }
    }
    status->m_numStunts = 0;
    return 1;
}


// ---- src/RaceStatusClass/EndStunts.cpp

#pragma fast_fptosi on


static inline float Fabs(float x)
{
    float r;
    asm("abs.s $f8, %1; swc1 $f8, 0(%0)" : : "r"(&r), "f"(x) : "$f8", "memory");
    return r;
}

// Ends a stunt combo: banks its score (rounded to m_x78) and, in a copy-cat
// race, checks it against the leader's combo.
int RaceStatusClass::EndStunts(RaceStatusStruct_Tag *status, int success, float multiplier)
{
    int handled = 0;
    int nextTurn = 0;
    RaceStatusStunt *stunt;
    if (!status || !(status->m_flags & 0x10))
        return 0;
    status->m_flags &= ~0x70;
    if (m_x30)
        handled = m_x30->RSEndStunts(status, success, multiplier);
    if (status->m_vehicle && !handled)
        handled = status->m_vehicle->RSEndStunts(status, success, multiplier);
    if (!handled) {
        status->m_x6F8 = 0;
        for (stunt = status->m_stunts; stunt; stunt = stunt->m_next)
            status->m_x6F8 += stunt->m_score;
        if (success)
            status->m_x6F4 += status->m_x6F8 * multiplier;
        status->m_x6F8 = 0;
        status->m_x6F4 = m_x78 * (int)(status->m_x6F4 / m_x78);
    }
    if (m_raceType == 5) {
        if (status->m_flags & 0x80) {
        if (!(status->m_flags & 0x100)) {
            if (success) {
                if (m_leader) {
                    RaceStatusStunt *target;
                    target = m_leader->m_stunts;
                    stunt = status->m_stunts;
                    while (target && stunt) {
                        if (target->m_type != stunt->m_type ||
                            ((target->m_flags & 8) && !(Fabs(target->m_x1C - stunt->m_x1C) < m_x80))) {
                            stunt = 0;
                            status->m_x30++;
                        } else {
                            target = target->m_next;
                            stunt = stunt->m_next;
                            if (!target && stunt)
                                status->m_x30++;
                            else if (target && !stunt)
                                status->m_x30++;
                        }
                    }
                }
            } else {
                status->m_x30++;
            }
            FreeStuntList(status);
        } else if (!success) {
            nextTurn = 1;
            status->m_flags &= ~0x100;
            FreeStuntList(status);
        }
        if (!(status->m_x30 < m_x7C)) {
            status->m_flags |= 1;
            m_flags |= 4;
            if (m_x30)
                m_x30->RSFinishedRaceCallback(this, status);
            if (status->m_vehicle)
                status->m_vehicle->RSFinishedRaceCallback(this, status);
        }
        if (!CopyCatRaceOver())
            CopyCatUpdateTurn(nextTurn);
        else if (m_x30)
            m_x30->RSRaceOverCallback();
        }
    } else {
        FreeStuntList(status);
    }
    return 1;
}


// ---- src/RaceStatusClass/StuntStarted.cpp

extern "C" void *memset(void *dst, int c, unsigned int n);

// Takes a stunt record off the free list.
static inline RaceStatusStunt *AllocStunt(RaceStatusClass *race)
{
    RaceStatusStunt *free = race->m_freeStunts;
    RaceStatusStunt *stunt = 0;
    if (free) {
        stunt = free;
        race->m_freeStunts = free->m_next;
        memset(free, 0, sizeof(RaceStatusStunt));
        free->m_next = 0;
    }
    return stunt;
}

int RaceStatusClass::StuntStarted(RaceStatusStruct_Tag *status, float a, float b, float c, float d, int type,
                                  float f, int flags)
{
    if (!status || !(status->m_flags & 0x10))
        return 0;
    RaceStatusStunt *stunt = AllocStunt(this);
    if (!stunt)
        return 0;
    stunt->m_type = type;
    stunt->m_x0C = a;
    stunt->m_x18 = b;
    stunt->m_x10 = c;
    stunt->m_x14 = d;
    stunt->m_score = stunt->m_x0C * d;
    stunt->m_x20 = f;
    stunt->m_flags = flags | 1;
    stunt->m_next = status->m_stunts;
    status->m_stunts = stunt;
    status->m_numStunts++;
    status->m_flags |= 0x20;
    if (status->m_vehicle)
        status->m_vehicle->RSStuntScoreCallback(this, status);
    return 1;
}


// ---- src/RaceStatusClass/StuntEnded.cpp
// Compiler session state: retail compiled
// this file in an mwcc session that had already generated a two-argument runtime
// helper call; this unused function recreates that state for StuntEnded.
static void MWCC_SHIM_TEXT MwccSessionState(long *a) { a[0] = a[0] / a[1]; }




// Finishes the current stunt: the event sink or the vehicle may take it over;
// otherwise apply the multiplier and round the score to the stunt's step.
int RaceStatusClass::StuntEnded(RaceStatusStruct_Tag *status, int success, float multiplier)
{
    int handled = 0;
    if (!status || !(status->m_flags & 0x20) || !(status->m_flags & 0x10))
        return 0;
    if (m_x30)
        handled = m_x30->RSStuntEnded(status, success, multiplier);
    if (status->m_vehicle && !handled)
        handled = status->m_vehicle->RSStuntEnded(status, success, multiplier);
    if (status->m_stunts && !handled) {
        if (success) {
            status->m_stunts->m_score *= multiplier;
            RaceStatusStunt *stunt = status->m_stunts;
            float step = stunt->m_x20;
            if (step > 0.0) {
                if (stunt->m_flags & 4)
                    stunt->m_score = step * (int)(stunt->m_score / step);
                else if (stunt->m_flags & 2)
                    stunt->m_score = step * (int)(0.5f + stunt->m_score / step);
            }
        } else {
            status->m_stunts->m_flags &= ~1;
        }
    }
    status->m_flags &= ~0x60;
    return 1;
}


// ---- src/RaceStatusClass/HoldStunt.cpp

int RaceStatusClass::HoldStunt(RaceStatusStruct_Tag *status, int hold)
{
    if (!status)
        return 0;
    if (hold) {
        if (status->m_flags & 0x20)
            status->m_flags |= 0x40;
    } else {
        status->m_flags &= ~0x40;
    }
    return 1;
}


// ---- src/RaceStatusClass/UpdateStunts.cpp




// Scores the stunt in progress for every vehicle (unless the event sink or the
// vehicle does it) and totals the scores of its active stunts.
int RaceStatusClass::UpdateStunts(float dt)
{
    RaceStatusStruct_Tag *status;
    RaceStatusStunt *stunt;
    int handled = 0;
    for (status = m_vehicles; status; status = status->m_next) {
        unsigned int flags = status->m_flags;
        stunt = status->m_stunts;
        if ((flags & 1) || !(flags & 2) || !(flags & 0x10))
            continue;
        status->m_x6F0 += dt;
        if (flags & 0x20) {
            if (flags & 0x40) {
                stunt->m_score = 0;
                if (m_x30)
                    handled = m_x30->RSUpdateStuntScore(this, status);
                if (status->m_vehicle && !handled)
                    handled = status->m_vehicle->RSUpdateStuntScore(this, status);
                if (stunt && !handled) {
                    stunt->m_x1C += dt;
                    stunt->m_score += stunt->m_x0C + stunt->m_x1C * stunt->m_x10;
                    stunt->m_score *= stunt->m_x14;
                    RaceStatusStunt *current = status->m_stunts;
                    float step = current->m_x20;
                    if (step > 0.0) {
                        if (current->m_flags & 4)
                            current->m_score = step * (int)(current->m_score / step);
                        else if (current->m_flags & 2)
                            current->m_score = step * (int)(0.5f + current->m_score / step);
                    }
                }
                if (status->m_vehicle)
                    status->m_vehicle->RSStuntScoreCallback(this, status);
            }
        }
        status->m_x6F8 = 0;
        for (; stunt; stunt = stunt->m_next) {
            if (stunt->m_flags & 1)
                status->m_x6F8 += stunt->m_score;
        }
    }
    return 1;
}


// ---- src/RaceStatusClass/UpdateTimeRemaining.cpp

int RaceStatusClass::UpdateTimeRemaining(RaceStatusStruct_Tag *status, float time, int mode)
{
    if (!status)
        return 0;
    switch (mode) {
    case 0:
        status->m_timeRemaining += time;
        break;
    case 1:
        status->m_timeRemaining -= time;
        break;
    case 2:
        status->m_timeRemaining = time;
        break;
    }
    return 1;
}


// ---- src/RaceStatusClass/SortVehiclesByID.cpp

int RaceStatusClass::SortVehiclesByID()
{
    if (!m_vehicles)
        return 0;
    RaceStatusStruct_Tag **pp = &m_vehicles;
    while (*pp && (*pp)->m_next) {
        RaceStatusStruct_Tag **qq = &(*pp)->m_next;
        while (*qq) {
            RaceStatusStruct_Tag *v = *qq;
            if ((*pp)->m_id > v->m_id) {
                *qq = v->m_next;
                v->m_next = *pp;
                *pp = v;
            }
            if (*qq)
                qq = &(*qq)->m_next;
        }
        if (*pp)
            pp = &(*pp)->m_next;
    }
    return 1;
}

