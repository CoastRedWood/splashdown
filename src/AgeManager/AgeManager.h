// AgeManager: tracks AgeItems by last use and purges the oldest to free
// space (Terrain's texture/mesh caches); 0x14 bytes. Names come from the
// class's own methods.
#ifndef AGEMANAGER_AGEMANAGER_H
#define AGEMANAGER_AGEMANAGER_H

#include "AgeItem/AgeItem.h"

class AgeManager {
public:
    ~AgeManager();
    void Tick();
    void AddItem(AgeItem *item, int (*callback)(void *, void *), void *context, void *data, int size);
    void RemoveItem(AgeItem *item);
    void TouchItem(AgeItem *item);
    int GetSize(int *total);
    int Purge(int target);

    unsigned int m_clock;               // +0x00
    int m_numItems;                     // +0x04
    int m_maxItems;                     // +0x08
    AgeItem **m_items;                  // +0x0C
    int m_totalSize;                    // +0x10
};

#endif
