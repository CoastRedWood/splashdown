// JetskiCareer: one saved career (JetskiGameOptions::m_career), 0x3D0
// bytes: the five racers, then the career path's events. It is the same
// record as JetskiGameOptions::SaveSlot (CareerProgress = CareerRacer,
// CareerRound = CareerEvent); no retail symbol names either.
#ifndef JETSKICAREER_JETSKICAREER_H
#define JETSKICAREER_JETSKICAREER_H

#include "CareerRacer/CareerRacer.h"

// One event (round) of the career path, 0x20 bytes.
struct CareerEvent {
    int m_type;                         // +0x00, 6 = career race, 7 = other
    int m_scene;                        // +0x04, the track (1-based)
    int m_pointsToPass;                 // +0x08
    float m_qualifyTimes[5];            // +0x0C
};

struct JetskiCareer {
    int m_valid;                        // +0x000
    int m_difficulty;                   // +0x004
    int m_currentCircuit;               // +0x008, the current event
    CareerRacer m_racers[5];            // +0x00C
    int m_numEvents;                    // +0x14C
    CareerEvent m_events[20];           // +0x150
};

#endif
