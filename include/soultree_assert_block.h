// ASSERT/ASSERTF/ASSERT_ONCE/STRCPY written as plain `{ ... }` blocks rather
// than `do { ... } while (0)`.
//
// mwcc pads branch-target labels with nops only inside loops, and it treats a
// `do { } while (0)` as a loop. So with include/soultree_assert.h every label
// after an ASSERT gets padded, which is why most asserting files need
// `#pragma alignlabel off` -- and why functions whose retail code pads only
// the labels inside real loops (DataAccess::OpenVirtualFile,
// JetskiUIPageCarRacerSelect::Create) could not match. Retail's macros were
// plain blocks: with these, no pragma is needed and the padding comes out
// exactly as in retail.
//
// Same bodies as include/soultree_assert.h and include/soultree_string.h
// otherwise.
#ifndef SOULTREE_ASSERT_BLOCK_H
#define SOULTREE_ASSERT_BLOCK_H

#include "ps2_port.h"

int _AssertFunc_(int cond, const char *file, int line);
void PSXprintf(const char *fmt, ...);

extern "C" unsigned int strlen(const char *s);
extern "C" char *strncpy(char *dst, const char *src, unsigned int n);

#define ASSERT_BREAK __I_break(0); asm("nop");

#define ASSERT(cond, msg)                                               \
    {                                                                   \
        static int allowThisAssertion = 1;                              \
        if (allowThisAssertion && _AssertFunc_((int)(cond), __FILE__, __LINE__)) { \
            PSXprintf(msg);                                             \
            PSXprintf("\n");                                            \
            ASSERT_BREAK                                                \
        }                                                               \
    }

// ASSERTF(cond, ("fmt %d", x))
#define ASSERTF(cond, printf_args)                                      \
    {                                                                   \
        static int allowThisAssertion = 1;                              \
        if (allowThisAssertion && _AssertFunc_((int)(cond), __FILE__, __LINE__)) { \
            PSXprintf printf_args;                                      \
            PSXprintf("\n");                                            \
            ASSERT_BREAK                                                \
        }                                                               \
    }

#define ASSERT_ONCE(cond, msg)                                          \
    {                                                                   \
        static int allowThisAssertion = 1;                              \
        if (allowThisAssertion && _AssertFunc_((int)(cond), __FILE__, __LINE__)) { \
            allowThisAssertion = 0;                                     \
            PSXprintf(msg);                                             \
            PSXprintf("\n");                                            \
            ASSERT_BREAK                                                \
        }                                                               \
    }

#define STRCPY(dst, src)                                                \
    {                                                                   \
        int _n = sizeof(dst);                                           \
        if (_n == sizeof(char *)) {                                     \
            ASSERT(0, "STRCPY: len of string is sizeof(char*)");        \
        }                                                               \
        int _len = strlen(src);                                         \
        int _c = (_n - 1 < _len) ? _n - 1 : _len;                       \
        strncpy(dst, src, _c);                                          \
        (dst)[_c] = 0;                                                  \
    }

#endif
