// ASSERT as compiled into retail (797 _AssertFunc_ call sites, 508
// allowThisAssertion statics). Each site has its own function-local
// static so a failing assertion can be silenced at runtime.
//
// The break is followed by an inline asm statement. Any function containing
// inline asm skips most of mwcc's global optimizer: constants aren't folded,
// bools are built with branches, addresses aren't reused across calls, loops
// aren't rotated. Retail shows exactly those patterns in asserting functions.
// Files using these macros also need `#pragma alignlabel off` (otherwise mwcc
// pads branch-target labels in asm functions with nops; retail doesn't).
#ifndef SOULTREE_ASSERT_H
#define SOULTREE_ASSERT_H

int _AssertFunc_(int cond, const char *file, int line);
void PSXprintf(const char *fmt, ...);

#define ASSERT_BREAK __I_break(0); asm("nop");

#define ASSERT(cond, msg)                                               \
    do {                                                                \
        static int allowThisAssertion = 1;                              \
        if (allowThisAssertion && _AssertFunc_((int)(cond), __FILE__, __LINE__)) { \
            PSXprintf(msg);                                             \
            PSXprintf("\n");                                            \
            ASSERT_BREAK                                                \
        }                                                               \
    } while (0)

// Same, with a printf-style message: ASSERTF(cond, ("fmt %d", x)).
#define ASSERTF(cond, printf_args)                                      \
    do {                                                                \
        static int allowThisAssertion = 1;                              \
        if (allowThisAssertion && _AssertFunc_((int)(cond), __FILE__, __LINE__)) { \
            PSXprintf printf_args;                                      \
            PSXprintf("\n");                                            \
            ASSERT_BREAK                                                \
        }                                                               \
    } while (0)

// Variant that silences itself after firing once (3 retail sites, e.g.
// PSXMultiTapDevice::GetMultiTapStatus).
#define ASSERT_ONCE(cond, msg)                                          \
    do {                                                                \
        static int allowThisAssertion = 1;                              \
        if (allowThisAssertion && _AssertFunc_((int)(cond), __FILE__, __LINE__)) { \
            allowThisAssertion = 0;                                     \
            PSXprintf(msg);                                             \
            PSXprintf("\n");                                            \
            ASSERT_BREAK                                                \
        }                                                               \
    } while (0)

#endif
