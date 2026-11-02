// adler32.c  @ 0x00397C10
// MATCHED (mwccps2 2.4.1.01, -O4,p -enum min -Cpp_exceptions off -sdatathreshold 4 -str readonly, mode: c) — byte-identical to retail.

// compiler: mwcps2-2.3-zlib
// Retail's zlib adler32.c as one translation unit: zlib 0.99
// adler32.c, unmodified, with the functions retail doesn't have #if 0'd out (the
// linker dropped them): adler32.
// Retail's zlib is 0.99 (ZLIB_VERSION "1.0"); 1.0.4's headers serve. It was
// built by an mwcc 2.3.x with -enum min; toolchain/mwcps2-2.3-zlib
// reconstructs it.

/* adler32.c -- compute the Adler-32 checksum of a data stream
 * Copyright (C) 1995-1996 Mark Adler
 * For conditions of distribution and use, see copyright notice in zlib.h 
 */

/* $Id: adler32.c,v 1.6 1995/05/03 17:27:08 jloup Exp $ */

#include "zlib.h"

#define BASE 65521L /* largest prime smaller than 65536 */
#define NMAX 5552
/* NMAX is the largest n such that 255n(n+1)/2 + (n+1)(BASE-1) <= 2^32-1 */

#define DO1(buf)  {s1 += *buf++; s2 += s1;}
#define DO2(buf)  DO1(buf); DO1(buf);
#define DO4(buf)  DO2(buf); DO2(buf);
#define DO8(buf)  DO4(buf); DO4(buf);
#define DO16(buf) DO8(buf); DO8(buf);

/* ========================================================================= */
#if 1 /* adler32 */
uLong adler32(adler, buf, len)
    uLong adler;
    const Bytef *buf;
    uInt len;
{
    unsigned long s1 = adler & 0xffff;
    unsigned long s2 = (adler >> 16) & 0xffff;
    int k;

    if (buf == Z_NULL) return 1L;

    while (len > 0) {
        k = len < NMAX ? len : NMAX;
        len -= k;
        while (k >= 16) {
            DO16(buf);
            k -= 16;
        }
        if (k != 0) do {
            DO1(buf);
        } while (--k);
        s1 %= BASE;
        s2 %= BASE;
    }
    return (s2 << 16) | s1;
}
#endif
