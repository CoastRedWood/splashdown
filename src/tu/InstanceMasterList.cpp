// InstanceMasterList.cpp  @ 0x00397050 (text 0x397050-0x397340)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// InstanceMasterList::Get and the DynamicArray<InstanceMasterType> members it
// instantiates, as one translation unit without a static initializer, in
// TextureMapStringTable.cpp's tu_map row: its .rodata (@410-@463,
// DynamicArray.h's ASSERT strings) restarts after the TextureMapStringTable
// RTTI, and its .bss is LpInstanceMasterList and the two ASSERT statics. The
// file name is a guess (it is out of alphabetical order).



#include "InstanceMasterList/InstanceMasterList.h"

extern "C" int strcmp(const char *a, const char *b);

InstanceMasterList *LpInstanceMasterList;

void *InstanceMasterList::Get(char *name)
{
    for (int i = 0; i < m_count; i++) {
        if (strcmp((*this)[i].m_name, name) == 0)
            return (*this)[i].m_master;
    }
    return 0;
}
