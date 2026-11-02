// TypeRegistry.cpp  @ 0x0029D160 (text 0x29D160-0x29D320)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's TypeRegistry.cpp as one translation unit: its functions in retail
// order, then its static initializer. `make link` places the object's data as
// well as its code (tools/link_c.py). The VideoCard functions that follow in
// its tu_map row (0x29D320-0x29D5F0, with their
// ContainerList<VideoPreFlipObject *> instances) are VideoCard.cpp, a TU of
// its own with no static initializer (link order is alphabetical here):
// src/tu/VideoCard.cpp.



#include "TypeRegistry/TypeRegistry.h"
#include "soultree_assert.h"

// ---- src/TypeRegistry/ctor.cpp
TypeRegistry::TypeRegistry()
{
    m_numTypes = 0;
    m_types = 0;
}


// ---- src/TypeRegistry/dtor.cpp
extern "C" void free(void *ptr);

TypeRegistry::~TypeRegistry()
{
    if (m_types)
        free(m_types);
}


// The global type registry, constructed by the static initializer; its
// .bss (after its destructor record) precedes GetTypeID's static in retail.
TypeRegistry GlobalTypeRegistry;

// ---- src/TypeRegistry/GetTypeID.cpp
#pragma alignlabel off

extern "C" int strncmp(const char *a, const char *b, unsigned int n);
extern "C" char *strncpy(char *dst, const char *src, unsigned int n);
extern "C" void *memset(void *dst, int c, unsigned int n);
extern "C" void *realloc(void *p, unsigned int size);




// Returns the ID of a named type, registering it first if it is new.
unsigned char TypeRegistry::GetTypeID(char *name)
{
    int i;
    for (i = 0; i < m_numTypes; i++) {
        if (!strncmp(name, m_types[i].m_name, sizeof(TypeName)))
            return i;
    }
#line 38 "TypeRegistry.cpp"
    ASSERT(m_numTypes != 0xFF, "GetTypeID: out of TypeIDs!");
    m_numTypes++;
    m_types = (TypeName *)realloc(m_types, m_numTypes * sizeof(TypeName));
    memset(&m_types[i], 0, sizeof(TypeName));
    strncpy(m_types[i].m_name, name, sizeof(TypeName));
    return m_numTypes - 1;
}


#pragma alignlabel reset

// ---- src/sinit/TypeRegistry.cpp
// Static initializer of TypeRegistry.cpp: the global type registry (defined
// above GetTypeID).

TypeRegistry *LpTypeRegistry = &GlobalTypeRegistry;

