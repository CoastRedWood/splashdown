// Clut.cpp  @ 0x00374580 (text 0x374580-0x374730)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// A retail translation unit without a static initializer (the name is a guess):
// Clut's ctor and dtor, Clut4's and Clut8's ctors and Read, fully optimized.
// Split from UIWater.cpp: its .rodata is the Clut RTTI (@467, after
// JetskiStuntsKey.cpp's @1775), which mwcc writes only at a TU's end, and
// UIWater.cpp's literals (@770) come after it.
#include "Clut4/Clut4.h"
#include "Clut8/Clut8.h"
#include "DataAccess/DataAccess.h"

extern "C" void *malloc(unsigned int size);
extern "C" void *memset(void *dst, int c, unsigned int n);
extern "C" void free(void *p);

extern DataAccess *LpDataAccess;


// ---- src/JetskiCheatKey/CheckHoldKey.cpp (Clut's ctor, from that C-mode stub)

Clut::Clut()
{
    m_numColors = 0;
    m_colors = 0;
}


// ---- src/Clut/dtor.cpp

Clut::~Clut()
{
    if (m_colors)
        free(m_colors);
}


// ---- src/Clut4/ctor.cpp




Clut4::Clut4()
{
    m_numColors = 0x10;
    m_colors = (unsigned int *)malloc(m_numColors * sizeof(unsigned int));
    memset(m_colors, 0, m_numColors * sizeof(unsigned int));
}


// ---- src/Clut4/Read.cpp


int Clut4::Read(long handle)
{
    return LpDataAccess->FRead(handle, m_colors, m_numColors * 4, 1);
}


// ---- src/Clut8/ctor.cpp




Clut8::Clut8()
{
    m_numColors = 0x100;
    m_colors = (unsigned int *)malloc(m_numColors * sizeof(unsigned int));
    memset(m_colors, 0, m_numColors * sizeof(unsigned int));
}


// ---- src/Clut8/Read.cpp


int Clut8::Read(long file)
{
    return LpDataAccess->FRead(file, m_colors, m_numColors * 4, 1);
}
