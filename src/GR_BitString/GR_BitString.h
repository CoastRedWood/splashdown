// GR_BitString: a bit string over an array of big-endian words, with
// shifts and a byte-wise bit writer/reader (PutBits/FlushBits, GetBits; the
// reader keeps its state in globals). 0x28 bytes. Names come from the class's
// own methods.
#ifndef GR_BITSTRING_GR_BITSTRING_H
#define GR_BITSTRING_GR_BITSTRING_H

// GR_PixelString: the abstract base. A 64-bit bit count, then the vptr at
// +0x08; its vtable holds four pure virtuals and no destructor. Its inline
// ctor is retail's (GR_BitString's ctor stores its vtable first).
class GR_PixelString {
public:
    GR_PixelString() {}
    long m_numBits;                     // +0x00 (64-bit)
    virtual void Reverse(int *shift) = 0;   // vptr +0x08
    virtual void Reverse() = 0;
    virtual void Shift(int count) = 0;
    virtual void Invert() = 0;
    int m_x0C;                          // +0x0C, after the vptr
};

class GR_BitString : public GR_PixelString {
public:
    GR_BitString(unsigned int *bits, unsigned int numBits, int fill);
    void _LeftShift(int count);
    void _RightShift(int count);
    void _LeftShiftINTEL(int count);
    void _RightShiftINTEL(int count);
    void PutBits(unsigned int value, int count);
    void FlushBits();
    unsigned int GetBits(int count);

    // --- virtuals in retail slot order ---
    virtual void Reverse(int *shift);
    virtual void Reverse();
    virtual void Shift(int count);
    virtual void Invert();
    virtual ~GR_BitString();

    unsigned int *m_bits;               // +0x10
    unsigned char *m_out;               // +0x14, PutBits' write position (the ctor sets it to m_bits)
    unsigned char m_mask;               // +0x18, next bit to fill (0x80 = byte empty)
    unsigned int m_rack;                // +0x1C, the byte being assembled
    unsigned int m_fill;                // +0x20
    int m_externalBits;                 // +0x24 (m_bits not owned)
};

#endif
