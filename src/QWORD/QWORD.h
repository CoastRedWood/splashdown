#ifndef QWORD_H
#define QWORD_H

typedef int int128_t __attribute__((mode(TI)));   // EE 128-bit register (lq/sq)

// One 128-bit VU/DMA quadword (16-aligned through the int128_t).
// Files that view it as float x/y/z + int w keep their own copies.
struct QWORD {
    union {
        int128_t m_value;
        unsigned int ui[4];
        float f[4];
    };
};

#endif
