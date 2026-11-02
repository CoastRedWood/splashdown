// STRCPY as compiled into retail: a bounded copy into a fixed char array that
// asserts the destination isn't just a pointer (sizeof == 4). The assert can
// never fire for an array, but the ASSERT's inline asm turns off the global
// optimizer, so the sizeof test stays in the code (see soultree_assert.h).
// Files using it need `#pragma alignlabel off`.
#ifndef SOULTREE_STRING_H
#define SOULTREE_STRING_H

#include "soultree_assert.h"

extern "C" unsigned int strlen(const char *s);
extern "C" char *strncpy(char *dst, const char *src, unsigned int n);

#define STRCPY(dst, src)                                                \
    do {                                                                \
        int _n = sizeof(dst);                                           \
        if (_n == sizeof(char *)) {                                     \
            ASSERT(0, "STRCPY: len of string is sizeof(char*)");        \
        }                                                               \
        int _len = strlen(src);                                         \
        int _c = (_n - 1 < _len) ? _n - 1 : _len;                       \
        strncpy(dst, src, _c);                                          \
        (dst)[_c] = 0;                                                  \
    } while (0)

#endif
