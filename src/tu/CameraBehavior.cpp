// CameraBehavior.cpp  @ 0x00303A20 (text 0x303A20-0x303AA0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// CameraBehavior as one translation unit, a TU without an initializer inside
// Portal.cpp's tu_map row, after SimpleFollowCam.cpp: the ctor, dtor and
// SetInputs, so the vtable and RTTI. Its .rodata (0x430708-0x430720) restarts
// the labels at @371 after SimpleFollowCam.cpp's RTTI; the name is a guess.




#include "CameraBehavior/CameraBehavior.h"

// ---- src/CameraBehavior/dtor.cpp

CameraBehavior::CameraBehavior()
{
}

// Retail vtable: slots 0, 1 and 3 are pure virtual (names unknown).

CameraBehavior::~CameraBehavior()
{
}

void CameraBehavior::SetInputs(CameraJoystickInput)
{
}
