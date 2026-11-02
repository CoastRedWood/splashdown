// MemoryPoolManager: the memory pool table (0x1C bytes; its constructor is
// trivial). GlobalMemoryPoolManager is defined in MemoryPoolManager.cpp;
// LpMemoryPoolManager points at it.
#ifndef MEMORYPOOLMANAGER_MEMORYPOOLMANAGER_H
#define MEMORYPOOLMANAGER_MEMORYPOOLMANAGER_H

class MemoryPoolManager {
public:
    ~MemoryPoolManager();

    int m_numPools;             // +0x00
    char (*m_names)[0x80];      // +0x04, pool names
    int *m_sizes;               // +0x08, pool sizes in bytes
    char m_pad[0x1C - 0xC];
};

extern MemoryPoolManager *LpMemoryPoolManager;

#endif
