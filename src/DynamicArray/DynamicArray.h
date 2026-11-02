// DynamicArray<T>: a growable array (DynamicArray.h, per the ASSERTs at its
// lines 31 and 63). operator[] grows it to twice the index; Resize copies the
// elements into a new T[size] up to m_maxSize. 0x14 bytes. The template
// members are emitted where they are used (InstanceMasterList.cpp).
#ifndef DYNAMICARRAY_DYNAMICARRAY_H
#define DYNAMICARRAY_DYNAMICARRAY_H

#include "soultree_assert_block.h"

#define DYNAMICARRAY_MIN(a, b) ((a) < (b) ? (a) : (b))

template <class T>
class DynamicArray {
public:
    T &operator[](int i);
    int Resize(int size);

    T *m_data;                          // +0x00
    int m_size;                         // +0x04
    int m_maxSize;                      // +0x08
    int m_used;                         // +0x0C, highest index used + 1
    int m_count;                        // +0x10
};

template <class T>
T &DynamicArray<T>::operator[](int i)
{
#line 31 "DynamicArray.h"
    ASSERT(i >= 0, "negative index into DynamicArray");
    if (i >= m_size)
        Resize((i + 1) * 2);
    if (i + 1 > m_used)
        m_used = i + 1;
    return m_data[i];
}

template <class T>
int DynamicArray<T>::Resize(int size)
{
#line 63 "DynamicArray.h"
    ASSERT(m_maxSize >= size, "DynamicArray has reached max size");
    if (size <= 0)
        return 0;
    if (size < m_size)
        m_used = size - 1;
    T *old = m_data;
    m_data = new T[size];
    if (!m_data)
        return 0;
    int n = DYNAMICARRAY_MIN(m_size, size);
    for (int i = 0; i < n; i++)
        m_data[i] = old[i];
    delete[] old;
    m_size = size;
    return 1;
}

#endif
