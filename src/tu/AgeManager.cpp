// AgeManager.cpp  @ 0x0029EB50 (text 0x29EB50-0x29F020)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's AgeManager.cpp as one translation unit: a TU without a static
// initializer at the end of VisibilityQuadTree.cpp's tu_map row (its strings,
// "AgeManager.cpp" first, follow VisibilityQuadTree.cpp's RTTI at 0x41B538;
// visible.cpp has no .rodata). `make link` places the object's data as well
// as its code (tools/link_c.py).


#include "AgeManager/AgeManager.h"
#include "AgeItem/AgeItem.h"
#include "soultree_assert_block.h"

extern "C" void free(void *p);
extern "C" void *realloc(void *p, unsigned int size);
extern "C" void qsort(void *base, unsigned int count, unsigned int size, int (*compare)(const void *, const void *));

// ---- src/AgeManager/dtor.cpp

AgeManager::~AgeManager()
{
    if (m_items)
        free(m_items);
}


// ---- src/AgeManager/Tick.cpp
void AgeManager::Tick()
{
    m_clock++;
}


// ---- src/AgeManager/AddItem.cpp

void AgeManager::AddItem(AgeItem *item, int (*callback)(void *, void *), void *context, void *data, int age)
{
    if (m_numItems == m_maxItems) {
        AgeItem **items = (AgeItem **)realloc(m_items, (m_maxItems + 1000) * sizeof(AgeItem *));
        if (!items)
            return;
        m_items = items;
        m_maxItems += 1000;
    }
    m_items[m_numItems] = item;
    item->m_callback = callback;
    item->m_data = data;
    item->m_context = context;
    item->m_birthTime = m_clock;
    item->m_size = age;
    m_numItems++;
    m_totalSize += age;
}


// ---- src/AgeManager/RemoveItem.cpp

void AgeManager::RemoveItem(AgeItem *item)
{
    if (m_items && m_numItems) {
        for (int i = 0; i < m_numItems; i++) {
            if (m_items[i] == item) {
                m_numItems--;
                if (m_numItems) {
                    m_items[i] = m_items[m_numItems];
                    m_items[m_numItems] = 0;
                } else {
                    m_items[0] = 0;
                }
                return;
            }
        }
    }
}


// ---- src/AgeManager/GetSize.cpp


int AgeManager::GetSize(int *size)
{
    if (size) {
        *size = 0;
        for (int i = 0; i < m_numItems; i++) {
            if (m_items[i]->m_birthTime < m_clock)
                *size += m_items[i]->m_size;
        }
    }
    return m_totalSize;
}


// ---- src/global/AgeItemTimeCompare.cpp

// qsort comparator over AgeItem pointers: newest first.
int AgeItemTimeCompare(const void *a, const void *b)
{
    int diff = (*(AgeItem **)b)->m_birthTime - (*(AgeItem **)a)->m_birthTime;
    if (diff < 0)
        return -1;
    else
        return diff == 0 ? 0 : 1;
}


// ---- src/AgeManager/Purge.cpp

// Frees the oldest items (via their callbacks) until the total age is down to
// target; returns how much was freed.
int AgeManager::Purge(int target)
{
    if (m_totalSize < target)
        return 0;
    qsort(m_items, m_numItems, sizeof(AgeItem *), AgeItemTimeCompare);
    int failed = 0;
    int removed = 0;
    int freed = 0;
    for (int i = m_numItems - 1; i >= 0; i--) {
        AgeItem *item = m_items[i];
        if (item->m_birthTime < m_clock) {
            int age = item->m_size;
            if (item->m_callback(item->m_context, item->m_data)) {
                freed += age;
                removed++;
                if (m_totalSize - freed <= target)
                    break;
            } else {
#line 141 "AgeManager.cpp"
                ASSERT(0, "Purge: we couldn't remove an item");
                failed = 1;
            }
        }
    }
    if (failed) {
#line 150 "AgeManager.cpp"
        ASSERT(0, "Purge: we couldn't remove an item");
    }
    m_numItems -= removed;
    m_totalSize -= freed;
    return freed;
}


// ---- src/AgeManager/TouchItem.cpp
void AgeManager::TouchItem(AgeItem *item)
{
    item->m_birthTime = m_clock;
}

