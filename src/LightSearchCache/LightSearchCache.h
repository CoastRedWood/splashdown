// LightSearchCache — caches the light set found for a search placement.
// A GameObject (data from +0x30): LightManager's dtor deletes it through
// the virtual dtor (LightManager::m_searchCache, +0xA4).
#ifndef LIGHTSEARCHCACHE_H
#define LIGHTSEARCHCACHE_H

#include "GameObject/GameObject.h"

class SearchPlacement;

typedef int int128_t __attribute__((mode(TI)));
#include "QWORD/QWORD.h"

struct LightSearchCacheEntry {
    SearchPlacement *placement;     // +0x00
    unsigned int m_lastUsed;        // +0x04 (0 = free)
    char pad8[0x10 - 0x8];
    QWORD m_a[4];                   // +0x10
    QWORD m_b[4];                   // +0x50
    QWORD m_c[4];                   // +0x90
};

class LightSearchCache : public GameObject {
public:
    virtual ~LightSearchCache();
    int Hit(SearchPlacement *placement);
    int LRU();
    int Retrieve(SearchPlacement *placement, QWORD *a, QWORD *b, QWORD *c);
    int Cache(SearchPlacement *placement, QWORD *a, QWORD *b, QWORD *c);

    LightSearchCacheEntry *m_entries;   // +0x30
    int m_numEntries;                   // +0x34
    unsigned int m_time;                // +0x38
};

#endif
