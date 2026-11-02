// CheatFlags: the cheats in effect (JetskiGame::m_cheats, `new`ed by
// JetskiGame::CreateScene; set by the JetskiGameOptions::Cheat* methods).
// 0x10 bytes; its ctor is inline and clears the four flags in order.
#ifndef CHEATFLAGS_CHEATFLAGS_H
#define CHEATFLAGS_CHEATFLAGS_H

struct CheatFlags {
    CheatFlags() { fullPerformance = 0; allOutAI = 0; hardToNormalAI = 0; noJoust = 0; }
    int fullPerformance;                // +0x0
    int allOutAI;                       // +0x4
    int hardToNormalAI;                 // +0x8
    int noJoust;                        // +0xC
};

#endif
