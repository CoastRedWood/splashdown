/* PS2_PTR(T) on the PC (include/ps2_port.h): a pointer stored in 32 bits,
 * as DMA tags and the game's files hold them. The game's data lies below
 * 4 GB (ports/pc/hw.cpp), so every pointer it stores fits. No constructors, so
 * structs holding one stay aggregates (C++98). */
#ifndef PORT_PTR32_H
#define PORT_PTR32_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" void port_ptr32_overflow(const void *p);
extern "C" void port_ptr32_unaligned(const void *p, const void *caller);

inline uint32_t port_ptr32(const void *p)
{
    if ((uintptr_t)p >> 32)
        port_ptr32_overflow(p);
    return (uint32_t)(uintptr_t)p;
}

// Every PS2_PTR but a string's is a DMA address the DMAC reads qword-aligned.
template <class T> struct PortPtr32Qword { enum { value = 1 }; };
template <> struct PortPtr32Qword<char> { enum { value = 0 }; };

template <class T> struct Ptr32 {
    uint32_t m_addr;

    operator T *() const { return (T *)(uintptr_t)m_addr; }
    T *operator->() const { return (T *)(uintptr_t)m_addr; }
    Ptr32 &operator=(T *p)
    {
        if (PortPtr32Qword<T>::value && ((uintptr_t)p & 0xF))
            port_ptr32_unaligned(p, __builtin_return_address(0));
        m_addr = port_ptr32(p);
        return *this;
    }
};
#endif

#endif
