// GR_BitString.cpp  @ 0x00203810 (text 0x203810-0x204440)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// A retail translation unit without a static initializer (the name is a guess):
// GR_BitString, with the bit tables. Split from BaseObject.cpp: mwcc writes
// RTTI only at the end of a TU, and retail's BaseObject RTTI (@365) comes about
// 300 labels before this file's GR_BitString/GR_PixelString RTTI (@665-@667),
// so BaseObject.cpp ended before GR_BitString's code.
#include "GameObject/GameObject.h"
#include "BaseObject/BaseObject.h"
#include "GR_BitString/GR_BitString.h"

// The bit-string tables and state (.data 0x3F8E50-0x3F9064; the state in .bss).
unsigned char ReverseBits[256] = {      // bit-reversed value of each byte
    0x00, 0x80, 0x40, 0xC0, 0x20, 0xA0, 0x60, 0xE0, 0x10, 0x90, 0x50, 0xD0, 0x30, 0xB0, 0x70, 0xF0,
    0x08, 0x88, 0x48, 0xC8, 0x28, 0xA8, 0x68, 0xE8, 0x18, 0x98, 0x58, 0xD8, 0x38, 0xB8, 0x78, 0xF8,
    0x04, 0x84, 0x44, 0xC4, 0x24, 0xA4, 0x64, 0xE4, 0x14, 0x94, 0x54, 0xD4, 0x34, 0xB4, 0x74, 0xF4,
    0x0C, 0x8C, 0x4C, 0xCC, 0x2C, 0xAC, 0x6C, 0xEC, 0x1C, 0x9C, 0x5C, 0xDC, 0x3C, 0xBC, 0x7C, 0xFC,
    0x02, 0x82, 0x42, 0xC2, 0x22, 0xA2, 0x62, 0xE2, 0x12, 0x92, 0x52, 0xD2, 0x32, 0xB2, 0x72, 0xF2,
    0x0A, 0x8A, 0x4A, 0xCA, 0x2A, 0xAA, 0x6A, 0xEA, 0x1A, 0x9A, 0x5A, 0xDA, 0x3A, 0xBA, 0x7A, 0xFA,
    0x06, 0x86, 0x46, 0xC6, 0x26, 0xA6, 0x66, 0xE6, 0x16, 0x96, 0x56, 0xD6, 0x36, 0xB6, 0x76, 0xF6,
    0x0E, 0x8E, 0x4E, 0xCE, 0x2E, 0xAE, 0x6E, 0xEE, 0x1E, 0x9E, 0x5E, 0xDE, 0x3E, 0xBE, 0x7E, 0xFE,
    0x01, 0x81, 0x41, 0xC1, 0x21, 0xA1, 0x61, 0xE1, 0x11, 0x91, 0x51, 0xD1, 0x31, 0xB1, 0x71, 0xF1,
    0x09, 0x89, 0x49, 0xC9, 0x29, 0xA9, 0x69, 0xE9, 0x19, 0x99, 0x59, 0xD9, 0x39, 0xB9, 0x79, 0xF9,
    0x05, 0x85, 0x45, 0xC5, 0x25, 0xA5, 0x65, 0xE5, 0x15, 0x95, 0x55, 0xD5, 0x35, 0xB5, 0x75, 0xF5,
    0x0D, 0x8D, 0x4D, 0xCD, 0x2D, 0xAD, 0x6D, 0xED, 0x1D, 0x9D, 0x5D, 0xDD, 0x3D, 0xBD, 0x7D, 0xFD,
    0x03, 0x83, 0x43, 0xC3, 0x23, 0xA3, 0x63, 0xE3, 0x13, 0x93, 0x53, 0xD3, 0x33, 0xB3, 0x73, 0xF3,
    0x0B, 0x8B, 0x4B, 0xCB, 0x2B, 0xAB, 0x6B, 0xEB, 0x1B, 0x9B, 0x5B, 0xDB, 0x3B, 0xBB, 0x7B, 0xFB,
    0x07, 0x87, 0x47, 0xC7, 0x27, 0xA7, 0x67, 0xE7, 0x17, 0x97, 0x57, 0xD7, 0x37, 0xB7, 0x77, 0xF7,
    0x0F, 0x8F, 0x4F, 0xCF, 0x2F, 0xAF, 0x6F, 0xEF, 0x1F, 0x9F, 0x5F, 0xDF, 0x3F, 0xBF, 0x7F, 0xFF
};
unsigned int BitFillMask[33] = {        // n bits set, in the INTEL byte order
    0x00000000, 0x00000080, 0x000000C0, 0x000000E0, 0x000000F0, 0x000000F8,
    0x000000FC, 0x000000FE, 0x000000FF, 0x000080FF, 0x0000C0FF, 0x0000E0FF,
    0x0000F0FF, 0x0000F8FF, 0x0000FCFF, 0x0000FEFF, 0x0000FFFF, 0x0080FFFF,
    0x00C0FFFF, 0x00E0FFFF, 0x00F0FFFF, 0x00F8FFFF, 0x00FCFFFF, 0x00FEFFFF,
    0x00FFFFFF, 0x80FFFFFF, 0xC0FFFFFF, 0xE0FFFFFF, 0xF0FFFFFF, 0xF8FFFFFF,
    0xFCFFFFFF, 0xFEFFFFFF, 0xFFFFFFFF
};
unsigned int RightFillMask[33] = {      // RightFillMask[n]: low n bits set
    0x00000000, 0x00000001, 0x00000003, 0x00000007, 0x0000000F, 0x0000001F,
    0x0000003F, 0x0000007F, 0x000000FF, 0x000001FF, 0x000003FF, 0x000007FF,
    0x00000FFF, 0x00001FFF, 0x00003FFF, 0x00007FFF, 0x0000FFFF, 0x0001FFFF,
    0x0003FFFF, 0x0007FFFF, 0x000FFFFF, 0x001FFFFF, 0x003FFFFF, 0x007FFFFF,
    0x00FFFFFF, 0x01FFFFFF, 0x03FFFFFF, 0x07FFFFFF, 0x0FFFFFFF, 0x1FFFFFFF,
    0x3FFFFFFF, 0x7FFFFFFF, 0xFFFFFFFF
};
unsigned int *_DwordValue;
unsigned int rack32;
int _BitIndex;


// ---- src/GR_BitString/LeftShift.cpp
// One counter n is reused for the copy and fill loops; declaration order sets retail registers.

void GR_BitString::_LeftShift(int count)
{
    int words;
    int shift;
    int remain;
    unsigned int *src;
    int n;
    unsigned int *dst;
    if (count >= 32) {
        words = ((int)m_numBits + 31) / 32;
        shift = count / 32;
        count -= shift * 32;
        remain = words - shift;
        if (remain > 0) {
            dst = m_bits;
            src = dst + shift;
            n = remain;
            do {
                *dst++ = *src++;
            } while (--n);
            n = shift;
            do {
                *dst++ = m_fill;
            } while (--n);
        } else {
            dst = m_bits;
            do {
                *dst++ = m_fill;
            } while (--words);
        }
    }
    if (count > 0)
        _LeftShiftINTEL(count);
}



// ---- src/GR_BitString/RightShift.cpp
// One counter n is reused for the copy and fill loops; declaration order sets retail registers.
void GR_BitString::_RightShift(int count)
{
    int words;
    int shift;
    int remain;
    unsigned int *src;
    unsigned int *dst;
    int n;
    if (count >= 32) {
        words = ((int)m_numBits + 31) / 32;
        shift = count / 32;
        count -= shift * 32;
        remain = words - shift;
        if (remain > 0) {
            src = m_bits + remain;
            dst = m_bits + words;
            n = remain;
            do {
                *--dst = *--src;
            } while (--n);
            n = shift;
            do {
                *--dst = m_fill;
            } while (--n);
        } else {
            dst = m_bits;
            do {
                *dst++ = m_fill;
            } while (--words);
        }
    }
    if (count > 0)
        _RightShiftINTEL(count);
}


// ---- src/GR_BitString/LeftShiftINTEL.cpp
// The bit string's words hold big-endian bytes; the INTEL variant swaps each
// word's bytes around the shift.
// Each byte is addressed from &x anew (a byte-pointer local gives different
// code).
#define BYTE_OF(x, n) (((unsigned char *)&(x))[n])
#define SWAP_BYTES(x) \
    { (x) = BYTE_OF(x, 3) | BYTE_OF(x, 2) << 8 | BYTE_OF(x, 1) << 16 | BYTE_OF(x, 0) << 24; }

#define ALL_ZEROS 0x00000000L
#define ALL_ONES 0xFFFFFFFFL



void GR_BitString::_LeftShiftINTEL(int count)
{
    unsigned int *bits;
    unsigned int tmp;
    unsigned int *next;
    int words;
    int rshift;

    words = ((int)m_numBits + 31) / 32;
    bits = m_bits;
    rshift = 32 - count;

    tmp = *bits;
    SWAP_BYTES(tmp);
    tmp <<= count;
    SWAP_BYTES(tmp);
    next = bits + 1;
    *bits = tmp;
    for (words--; words; words--) {
        if (*next == ALL_ZEROS) {
            bits++;
            next++;
        } else if (*next == ALL_ONES) {
            *bits |= ~BitFillMask[rshift];
            *next = BitFillMask[rshift];
            bits++;
            next++;
        } else {
            SWAP_BYTES(*next);
            if ((tmp = *next >> rshift) != ALL_ZEROS) {
                SWAP_BYTES(tmp);
                *bits++ |= tmp;
            } else {
                bits++;
            }
            *next <<= count;
            if (*next != ALL_ZEROS)
                SWAP_BYTES(*next);
            next++;
        }
    }
    if (m_fill == ALL_ZEROS)
        *bits &= BitFillMask[rshift];
    else
        *bits |= ~BitFillMask[rshift];
}


// ---- src/GR_BitString/RightShiftINTEL.cpp
// The bit string's words hold big-endian bytes; the INTEL variant swaps each
// word's bytes around the shift.
// Each byte is addressed from &x anew (a byte-pointer local gives different
// code).
#define BYTE_OF(x, n) (((unsigned char *)&(x))[n])
#define SWAP_BYTES(x) \
    { (x) = BYTE_OF(x, 3) | BYTE_OF(x, 2) << 8 | BYTE_OF(x, 1) << 16 | BYTE_OF(x, 0) << 24; }

#define ALL_ZEROS 0x00000000L
#define ALL_ONES 0xFFFFFFFFL



void GR_BitString::_RightShiftINTEL(int count)
{
    unsigned int *bits;
    unsigned int tmp;
    unsigned int *next;
    int words;
    int lshift;

    words = ((int)m_numBits + 31) / 32;
    bits = m_bits + words - 1;
    lshift = 32 - count;
    tmp = *bits;
    SWAP_BYTES(tmp);
    tmp >>= count;
    SWAP_BYTES(tmp);
    next = bits - 1;
    *bits = tmp;
    // The loop walks both pointers down with pre-decrements.
    bits++;
    next++;
    for (words--; words; words--) {
        if (*--next == ALL_ZEROS) {
            bits--;
        } else if (*next == ALL_ONES) {
            *--bits |= BitFillMask[count];
            *next = ~BitFillMask[count];
        } else {
            SWAP_BYTES(*next);
            if ((tmp = *next << lshift) != ALL_ZEROS) {
                SWAP_BYTES(tmp);
                *--bits |= tmp;
            } else {
                bits--;
            }
            *next >>= count;
            if (*next != ALL_ZEROS)
                SWAP_BYTES(*next);
        }
    }
    if (m_fill == ALL_ZEROS)
        *--bits &= ~BitFillMask[count];
    else
        *--bits |= BitFillMask[count];
}


// ---- src/GR_BitString/ctor.cpp


GR_BitString::GR_BitString(unsigned int *bits, unsigned int numBits, int fill)
{
    m_numBits = numBits;
    m_bits = bits;
    m_fill = fill ? 0xFFFFFFFFL : 0;   // (a long literal: retail builds a 64-bit 0xFFFFFFFF)
    m_externalBits = 1;
    m_out = (unsigned char *)bits;
    m_mask = 0x80;
    m_rack = 0;
    _DwordValue = bits;
    _BitIndex = 0;
    unsigned int x = *bits;
    // Words are stored big-endian.
    rack32 = (x & 0xFF000000) >> 24 | (x & 0xFF0000) >> 8 | (x & 0xFF) << 24 | (x & 0xFF00) << 8;
}


// ---- src/GR_BitString/dtor.cpp

GR_BitString::~GR_BitString()
{
    if (!m_externalBits && m_bits)
        delete m_bits;
}


// ---- src/GR_BitString/Reverse.cpp

// Reverse the bit order of the whole string a byte at a time. The padding
// bits of the last byte end up at the front; *shift says how far to shift
// to drop them.
void GR_BitString::Reverse(int *shift)
{
    if (!m_bits || !m_numBits)
        return;
    int bytes = ((int)m_numBits + 7) / 8;
    int count = bytes / 2;      // (retail bug: an odd middle byte is never bit-reversed)
    unsigned char *lo = (unsigned char *)m_bits;
    unsigned char *hi = lo + bytes;
    do {
        hi--;
        unsigned char a = ReverseBits[*lo];
        unsigned char b = ReverseBits[*hi];
        *lo = b;
        *hi = a;
        lo++;
    } while (--count);
    *shift = -((8 - (int)m_numBits % 8) % 8);
}


// ---- src/GR_BitString/Reverse_00204110.cpp

void GR_BitString::Reverse()
{
    int bits;
    Reverse(&bits);
    Shift(bits);
}


// ---- src/GR_BitString/Shift.cpp

void GR_BitString::Shift(int count)
{
    if (!m_bits || !m_numBits)
        return;
    if (count < 0)
        _LeftShift(-count);
    else if (count > 0)
        _RightShift(count);
}


// ---- src/GR_BitString/Invert.cpp

void GR_BitString::Invert()
{
    if (!m_bits || !m_numBits)
        return;
    int words = ((int)m_numBits + 31) / 32;
    unsigned int *p = m_bits;
    do {
        *p = ~*p;
        p++;
    } while (--words);
}


// ---- src/GR_BitString/PutBits.cpp

// Append the low `count` bits of `value`, most significant first.
void GR_BitString::PutBits(unsigned int value, int count)
{
    if (count == 1) {
        if (value)
            m_rack |= m_mask;
        m_mask >>= 1;
        if (!m_mask) {
            *m_out++ = m_rack;
            m_rack = 0;
            m_mask = 0x80;
        }
    } else {
        unsigned int bit = 1 << (count - 1);
        while (bit) {
            if (bit & value)
                m_rack |= m_mask;
            m_mask >>= 1;
            if (!m_mask) {
                *m_out++ = m_rack;
                m_rack = 0;
                m_mask = 0x80;
            }
            bit >>= 1;
        }
    }
}


// ---- src/GR_BitString/FlushBits.cpp
// Write out a partially filled byte, then start a fresh one.
void GR_BitString::FlushBits()
{
    if (m_mask != 0x80)
        *m_out++ = m_rack;
    m_mask = 0x80;
}


// ---- src/GR_BitString/GetBits.cpp


// Read the next `count` bits from the big-endian word stream.
unsigned int GR_BitString::GetBits(int count)
{
    unsigned int bits;
    _BitIndex += count;
    if (_BitIndex >= 32) {
        _BitIndex -= 32;
        bits = (rack32 << _BitIndex) & RightFillMask[count];
        _DwordValue++;
        unsigned int x = *_DwordValue;
        rack32 = (x & 0xFF000000) >> 24 | (x & 0xFF0000) >> 8 | (x & 0xFF) << 24 | (x & 0xFF00) << 8;
        if (_BitIndex)
            bits |= rack32 >> (32 - _BitIndex);
    } else {
        bits = (rack32 >> (32 - _BitIndex)) & RightFillMask[count];
    }
    return bits;
}

