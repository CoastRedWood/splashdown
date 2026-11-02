// MemoryCard.cpp  @ 0x002F9D10 (text 0x2F9D10-0x2F9D70)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's MemoryCard.cpp as one translation unit: a TU without a static
// initializer or data, inside CutSceneCamera.cpp's tu_map row: the
// out-of-line ctor and dtor of PSXMemoryCard's base class. It sits between
// CutSceneCamera.cpp and JetskiParticleEmitters.cpp, which is out of
// alphabetical link order (as PSXMemoryCard.cpp is in ParticleEmitter.cpp's
// row), so these two functions might also be the tail of CutSceneCamera.cpp;
// nothing in the data ties them to either neighbour (no data, no strings), so
// they are taken as their own file.


#include "PSXMemoryCard/PSXMemoryCard.h"

// ---- src/MemoryCard/ctor.cpp

MemoryCard::MemoryCard()
{
}


// ---- src/MemoryCard/dtor.cpp

MemoryCard::~MemoryCard()
{
}
