/* Variable arguments. mwcc's MIPS va_start: __builtin_args_info(2) is the
 * number of integer argument registers the named parameters use; the unnamed
 * ones were spilled to the 8-register save area that ends at
 * __builtin_next_arg(last). Each argument takes 8 bytes there, which
 * MWCC_VA_NEXT walks as retail's code did. The PC port uses <stdarg.h>. */
#ifndef MWCC_VA_H
#define MWCC_VA_H

#ifdef SPLASHDOWN_PC
#include <stdarg.h>
#define MWCC_VA_NEXT(ap, type, out) out = va_arg(ap, type)
#else
typedef char *va_list;
#define va_start(ap, last)                                              \
    ((ap) = (char *)__builtin_next_arg(last) -                          \
            (__builtin_args_info(2) >= 8 ? 0 : (8 - __builtin_args_info(2)) * 8))
#define MWCC_VA_NEXT(ap, type, out) ap += 8; out = *(type *)(ap - 8)
#endif

#endif
