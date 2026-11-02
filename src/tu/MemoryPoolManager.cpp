// MemoryPoolManager.cpp  @ 0x002003B0 (text 0x2003B0-0x2005F0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's MemoryPoolManager.cpp as one translation unit: MemoryPoolManager's
// dtor, malloc_align and MemPoolReportToFile, then the static initializer
// (GlobalMemoryPoolManager's destructor registration). Its .rodata is
// @613-@621 (0x40FDB0-0x40FE10); the next TU's labels restart at @548.



extern "C" int sprintf(char *buf, const char *fmt, ...);
extern "C" void *memalign(unsigned int align, unsigned int size);
void PSXprintf(const char *fmt, ...);

struct FILE_HANDLE_STRUCT;
FILE_HANDLE_STRUCT *FOPEN(char *name, const char *mode);
int FCLOSE(FILE_HANDLE_STRUCT *f);
int FPRINTF(FILE_HANDLE_STRUCT *f, const char *fmt, ...);

#include "MemoryPoolManager/MemoryPoolManager.h"


// ---- src/MemoryPoolManager/dtor.cpp

MemoryPoolManager::~MemoryPoolManager()
{
}


// ---- src/global/malloc_align.cpp

// A tail call: retail is `j memalign`.
void *malloc_align(unsigned int align, unsigned int size)
{
    return memalign(align, size);
}


// ---- src/global/MemPoolReportToFile.cpp

// Defined before MemPoolReportToFile: its .bss (and the destructor-chain
// record, @47) precede the function's static in retail.
MemoryPoolManager GlobalMemoryPoolManager;

// GameText::GetString (an inline member; Localization.h in retail) has its
// statics here: retail's copies, the first in link order, sit between
// GlobalMemoryPoolManager and currentFile in .bss (0x44F9E0) and before
// init$590 in .sbss. An inline's statics are emitted where it is defined,
// used or not.
#include "GameText/GameText.h"

// Writes each pool's size to the first unused MemPoolDump<N>.txt.
void MemPoolReportToFile()
{
    static int currentFile = 0;
    char name[260];
    FILE_HANDLE_STRUCT *f;

    sprintf(name, "MemPoolDump%d.txt", currentFile);
    while ((f = FOPEN(name, "r")) != 0) {
        FCLOSE(f);
        currentFile++;
        sprintf(name, "MemPoolDump%d.txt", currentFile);
    }
    f = FOPEN(name, "w");
    if (!f) {
        PSXprintf("Could not open %s for writing!!\n", name);
        return;
    }
    FPRINTF(f, "Memory\r\n");
    FPRINTF(f, "Bytes\tArea\r\n");
    int count;
    int total = 0;
    count = LpMemoryPoolManager->m_numPools;
    for (int i = 0; i < count; i++) {
        FPRINTF(f, "%8d\t%12s\r\n", LpMemoryPoolManager->m_sizes[i], LpMemoryPoolManager->m_names[i]);
        total += LpMemoryPoolManager->m_sizes[i];
    }
    FPRINTF(f, "\r\n");
    FPRINTF(f, "%8d\t%12s\r\n", total, "Total");
    FCLOSE(f);
}


// ---- static initializer (was src/sinit/MemoryPoolManager.cpp)

MemoryPoolManager *LpMemoryPoolManager = &GlobalMemoryPoolManager;
