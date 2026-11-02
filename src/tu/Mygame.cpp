// Mygame.cpp  @ 0x00441E00 (no text; static initializer only)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's Mygame.cpp as one translation unit: it has no functions of its
// own in .text (Motnctrl.cpp ends where NormalDistribution.cpp starts,
// 0x25EF80), only its data and its static initializer: the global game
// object, then the game's collision type IDs.


#include "JetskiGame/JetskiGame.h"
#include "CollisionTypeRegistry/CollisionTypeRegistry.h"

// ---- src/sinit/Mygame.cpp
JetskiGame GlobalGame;
Game *LpGame = &GlobalGame;

unsigned int COLLISION_BIKE = LpCollisionTypeRegistry->GetType("BIKE");
unsigned int COLLISION_RIDER = LpCollisionTypeRegistry->GetType("RIDER");
unsigned int COLLISION_WALL = LpCollisionTypeRegistry->GetType("WALL");
unsigned int COLLISION_TERRAIN = LpCollisionTypeRegistry->GetType("TERRAIN");
unsigned int COLLISION_MOVABLE_OBJECT = LpCollisionTypeRegistry->GetType("MOVABLE_OBJECT");
unsigned int COLLISION_PROCEDURAL = LpCollisionTypeRegistry->GetType("PROCEDURAL");
unsigned int COLLISION_SOLID_OBJECT = LpCollisionTypeRegistry->GetType("SOLID_OBJECT");
