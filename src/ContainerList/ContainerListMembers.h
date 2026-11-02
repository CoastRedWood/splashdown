// ContainerList<T>'s out-of-line members (ContainerList.h declares them). Kept
// out of ContainerList.h: with the definitions visible, every file that calls
// a member would emit its own copy. A file includes this header to own
// instances: a whole TU defines CONTAINERLIST_MEMBERS_ALL and gets the members
// it uses instantiated implicitly (retail's InputDeviceType.cpp and
// SearchMethod.cpp have them at their ends). The ASSERTs use whichever assert
// header the includer chose.


#ifndef CONTAINERLIST_CONTAINERLISTMEMBERS_H
#define CONTAINERLIST_CONTAINERLISTMEMBERS_H

#include "ContainerList/ContainerList.h"

#if defined(CONTAINERLIST_MEMBERS_ALL) || defined(CONTAINERLIST_CTOR)
template <class T>
ContainerList<T>::ContainerList()
{
    m_items = 0;
    m_numItems = 0;
    m_growBy = 0;
    m_maxItems = 0;
    m_created = 0;
}
#endif

#if defined(CONTAINERLIST_MEMBERS_ALL) || defined(CONTAINERLIST_CREATE)
template <class T>
int ContainerList<T>::Create(int size, int growBy)
{
// The asserts report ContainerList.h:60-61, where this was defined inline.
#line 60 "ContainerList.h"
    ASSERT(size > 0, "ContainerList::Create size must be greater than 0");
    ASSERT(m_maxItems == 0, "ContainerList::Trying to create the ItemList array twice");
    m_items = new T[size];
    m_growBy = growBy;
    m_maxItems = size;
    m_created = 1;
    return m_items ? 1 : 0;
}
#endif

#if defined(CONTAINERLIST_MEMBERS_ALL) || defined(CONTAINERLIST_ADDITEM)
template <class T>
int ContainerList<T>::AddItem(T item)
{
// The asserts report ContainerList.h:90-91, where this was defined inline.
#line 90 "ContainerList.h"
    ASSERT(m_created, "AddItem: Item not created");
    ASSERT(item, "AddItem: item==0");
    if (m_numItems >= m_maxItems && !Grow(m_numItems + m_growBy))
        return 0;
    m_items[m_numItems] = item;
    m_numItems++;
    return 1;
}
#endif

#if defined(CONTAINERLIST_MEMBERS_ALL) || defined(CONTAINERLIST_REMOVEITEM)
// Swaps the last item into the removed slot.
template <class T>
int ContainerList<T>::RemoveItem(T item)
{
    if (m_numItems <= 0)
        return 0;
    for (int i = 0; i < m_numItems; i++) {
        if (m_items[i] == item) {
            m_items[i] = m_items[m_numItems - 1];
            m_numItems--;
            return 1;
        }
    }
    return 0;
}
#endif

#if defined(CONTAINERLIST_MEMBERS_ALL) || defined(CONTAINERLIST_REMOVEITEMBYINDEX)
// Swaps the last item into slot i.
template <class T>
int ContainerList<T>::RemoveItemByIndex(int i)
{
    if (m_numItems <= 0 || !m_items || i >= m_numItems)
        return 0;
    m_items[i] = m_items[m_numItems - 1];
    m_numItems--;
    return 1;
}
#endif

#if defined(CONTAINERLIST_MEMBERS_ALL) || defined(CONTAINERLIST_GETITEM)
template <class T>
T ContainerList<T>::GetItem(int i)
{
    if (m_numItems <= 0 || !m_items || i >= m_numItems)
        return 0;
    return m_items[i];
}
#endif

#if defined(CONTAINERLIST_MEMBERS_ALL) || defined(CONTAINERLIST_CLEAR)
template <class T>
void ContainerList<T>::Clear()
{
    memset(m_items, 0, m_numItems * sizeof(T));
    m_numItems = 0;
}
#endif

#if defined(CONTAINERLIST_MEMBERS_ALL) || defined(CONTAINERLIST_GROW)
template <class T>
int ContainerList<T>::Grow(int newMax)
{
    if (newMax <= m_maxItems)  // not "m_maxItems >= newMax": changes slt operands
        return 0;
    T *items = new T[newMax];
    if (!items)
        return 0;
    memcpy(items, m_items, m_maxItems * sizeof(T));
    T *old = m_items;
    m_items = items;
    delete[] old;
    m_maxItems = newMax;
    return 1;
}
#endif

#if defined(CONTAINERLIST_MEMBERS_ALL) || defined(CONTAINERLIST_SORT)
// Only <SoundEmitter *> has it (AuralScape.cpp).
extern "C" void qsort(void *base, unsigned int n, unsigned int size,
                      int (*cmp)(const void *, const void *));

template <class T>
void ContainerList<T>::Sort(int (*cmp)(const void *, const void *))
{
    qsort(m_items, m_numItems, sizeof(T), cmp);
}
#endif

#endif
