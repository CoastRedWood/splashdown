// WaterDisplacement.cpp  @ 0x002D0160 (text 0x2D0160-0x2D0210)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's WaterDisplacement.cpp as one translation unit: the ctor and dtor,
// then its static initializer. `make link` places the object's data as well as
// its code (tools/link_c.py). The JetskiMenu functions that followed in its
// old tu_map row open JetskiInGameMenu.cpp (their ASSERTs say so; the TU map).



#include "GameObject/GameObject.h"
#include "ViewPortCode/ViewPortCode.h"
#include "PSXWaterDisplacement/PSXWaterDisplacement.h"
// The axis vectors every Soultree TU defines (and __sinit constructs).
#include "sinit/AxisVectors.h"

// ---- src/WaterDisplacement/ctor.cpp

WaterDisplacement::WaterDisplacement() : GameObject(1)
{
    m_params = 0;
    m_vehicles[0] = 0;
    m_vehicles[1] = 0;
}


// ---- src/WaterDisplacement/dtor.cpp

WaterDisplacement::~WaterDisplacement()
{
}


// ---- src/sinit/WaterDisplacement.cpp
// Static initializer of WaterDisplacement.cpp: only the axis vectors from the shared header.
