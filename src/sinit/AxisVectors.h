#ifndef SINIT_AXISVECTORS_H
#define SINIT_AXISVECTORS_H

#include "d3dtypes.h"

// The four unit/zero vectors that a common Soultree header defined in every
// translation unit that included it: local ZeroVector/XVector/YVector/ZVector
// symbols, 0x10 bytes each, in that order, in every TU's .bss; their
// constructors are the first thing each TU's __sinit_<File> runs (zero stores
// first, then the 1.0f ones). `static`, not `const`: mwcc puts a const object
// with a constructor in .rodata, retail has them in .bss.

static _D3DVECTOR ZeroVector(0.0f, 0.0f, 0.0f);
static _D3DVECTOR XVector(1.0f, 0.0f, 0.0f);
static _D3DVECTOR YVector(0.0f, 1.0f, 0.0f);
static _D3DVECTOR ZVector(0.0f, 0.0f, 1.0f);

#endif
