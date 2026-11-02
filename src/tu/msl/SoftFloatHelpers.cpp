// SoftFloatHelpers.cpp  @ 0x003A19A0
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// The soft-float helpers at the end of the MSL runtime as one translation
// unit, 0x3A19A0-0x3A1AD8: _f_ulltof, _d_ulltod,
// _d_utod, _dpflt, _dpfle, _dpfgt, _dpfge. They reference no data. They
// follow ExceptionHandling.cpp's text with no data to mark a boundary, but
// unlike it they were built with exceptions off: with `#pragma exceptions
// on` mwcc doesn't make _d_utod's call to _d_ulltod a tail call (0x20 bytes,
// retail 0xC). So they are a separate object; the file name is a guess.

extern "C" float __floatdisf(long value);
extern "C" double __floatdidf(long value);
// The soft-float double compare: the operands arrive in registers and are
// passed straight through, so neither it nor its callers name them.
extern "C" int dpcmp(void);

// Unsigned 64-bit integer to float. Values with the top bit set are halved
// (keeping the sticky low bit) and doubled back.
extern "C" float _f_ulltof(unsigned long value)
{
    if ((long)value > 0)
        return __floatdisf(value);
    float f = __floatdisf((value >> 1) | (value & 1));
    return f + f;
}

// Unsigned 64-bit integer to double, the same way.
extern "C" double _d_ulltod(unsigned long value)
{
    if ((long)value > 0)
        return __floatdidf(value);
    double f = __floatdidf((value >> 1) | (value & 1));
    return f + f;
}

// Unsigned 32-bit to double: a tail call into the 64-bit conversion
// (zero-extended with dsll32/dsrl32).
extern "C" double _d_utod(unsigned int x)
{
    return _d_ulltod(x);
}

extern "C" int _dpflt(void)
{
    return dpcmp() < 0;
}

extern "C" int _dpfle(void)
{
    return dpcmp() <= 0;
}

extern "C" int _dpfgt(void)
{
    return dpcmp() > 0;
}

extern "C" int _dpfge(void)
{
    return dpcmp() >= 0;
}
