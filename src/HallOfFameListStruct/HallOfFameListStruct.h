// HallOfFameListStruct: JetskiGameOptions::m_hallOfFame (0x41A0 bytes,
// saved as "fame.dat"): per table, a record for each track/event of the six
// best names and times. JetskiGameOptions::PopulateNewHallOfFame fills it.
#ifndef HALLOFFAMELISTSTRUCT_HALLOFFAMELISTSTRUCT_H
#define HALLOFFAMELISTSTRUCT_HALLOFFAMELISTSTRUCT_H

// One track's or event's records (0x50 bytes).
struct HallOfFameEntry {
    char m_names[6][9];                 // +0x00
    float m_times[6];                   // +0x38
};

struct HallOfFameTable42 {
    HallOfFameEntry m_entries[42];
};

struct HallOfFameTable21 {
    HallOfFameEntry m_entries[21];
};

struct HallOfFameListStruct {
    HallOfFameTable42 m_table0;         // +0x0000
    HallOfFameTable42 m_table1;         // +0x0D20
    HallOfFameTable21 m_table2;         // +0x1A40
    HallOfFameTable21 m_table3;         // +0x20D0
    HallOfFameTable42 m_table4;         // +0x2760
    HallOfFameTable42 m_table5;         // +0x3480
};

#endif
