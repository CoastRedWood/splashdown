// CareerRacer: one racer of a saved career (JetskiCareer::m_racers),
// 0x40 bytes. The same record as JetskiGameOptions::CareerProgress.
#ifndef CAREERRACER_CAREERRACER_H
#define CAREERRACER_CAREERRACER_H

struct CareerRacer {
    char m_name[0xC];                   // +0x00
    int m_enabled;                      // +0x0C, 0 = the player
    int m_points;                       // +0x10
    int m_place;                        // +0x14
    int m_numChoices;                   // +0x18
    int m_choices[5];                   // +0x1C
    int m_character;                    // +0x30
    int m_skin;                         // +0x34 (-> JetskiGameOptions::m_racerSkin)
    int m_vehicle;                      // +0x38
    int m_vehicleSkin;                  // +0x3C (-> m_racerVehicleSkin)
};

#endif
