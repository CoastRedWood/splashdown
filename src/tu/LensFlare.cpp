// LensFlare.cpp  @ 0x002565D0 (text 0x2565D0-0x256620)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's LensFlare.cpp as one translation unit: a TU without a static
// initializer at the end of JetskiTimeTrialCompetition.cpp's tu_map row (link
// order is alphabetical here: JetskiTimeTrialCompetition < LensFlare <
// LightEmitter). It holds the out-of-line _D3DTLVERTEX ctor and LensFlare's
// two setters, and no data.


#include "d3dtypes.h"
#include "_D3DTLVERTEX/_D3DTLVERTEX.h"
#include "LensFlare/LensFlare.h"

// ---- src/_D3DTLVERTEX/ctor.cpp

_D3DTLVERTEX::_D3DTLVERTEX()
{
}


// ---- src/LensFlare/SetColor.cpp

void LensFlare::SetColor(unsigned int color)
{
    m_color = color & 0xFFFFFF;
}


// ---- src/LensFlare/SetPosition.cpp

void LensFlare::SetPosition(_D3DVECTOR &v)
{
    m_position.x = v.x;
    m_position.y = v.y;
    m_position.z = v.z;
    m_position.w = v.w;
}
