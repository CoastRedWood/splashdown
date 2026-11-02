/* Inline asm on the PC port: tools/port/rewrite.py turns
 *     asm("text" : "=f"(r) : "r"(a), "f"(b) : clobbers);
 * into
 *     do { static PortAsmProg _pap; PortAsmArg _paa[] = {
 *         port_out_f(r), port_in_r(a), port_in_f(b) };
 *         port_asm(&_pap, "text", _paa, 3); } while (0);
 * and ports/pc/ee/asm_interp.cpp runs the text against an emulated EE.
 * Operand N of the statement is element N of the array, as %N in the text.
 * Included by game code compiled as C++98. */
#ifndef PORT_ASM_H
#define PORT_ASM_H

#include <stdint.h>

typedef struct PortAsmProg {
    void *prog;
} PortAsmProg;

enum {
    PORT_ASM_IN_R,
    PORT_ASM_IN_F,
    PORT_ASM_OUT_R,
    PORT_ASM_OUT_F,
    PORT_ASM_INOUT_R
};

typedef struct PortAsmArg {
    int kind;
    int size;
    int64_t value;
    float fvalue;
    void *ptr;
} PortAsmArg;

#ifdef __cplusplus
extern "C" {
#endif
void port_asm(PortAsmProg *site, const char *text, PortAsmArg *args, int nargs);
int64_t port_asm_load(PortAsmArg a);
void port_asm_store(PortAsmArg a, int64_t v);
#ifdef __cplusplus
}

inline PortAsmArg port_asm_arg(int kind, int size, int64_t value, float fvalue, void *ptr)
{
    PortAsmArg a;
    a.kind = kind;
    a.size = size;
    a.value = value;
    a.fvalue = fvalue;
    a.ptr = ptr;
    return a;
}

template <class T> inline PortAsmArg port_in_r(T *p)
{
    return port_asm_arg(PORT_ASM_IN_R, 8, (int64_t)(intptr_t)p, 0.0f, 0);
}
template <class T> inline PortAsmArg port_in_r(T v)
{
    return port_asm_arg(PORT_ASM_IN_R, 8, (int64_t)v, 0.0f, 0);
}
inline PortAsmArg port_in_f(float v)
{
    return port_asm_arg(PORT_ASM_IN_F, 4, 0, v, 0);
}
template <class T> inline PortAsmArg port_out_r(T &r)
{
    return port_asm_arg(PORT_ASM_OUT_R, (int)sizeof(T), 0, 0.0f, (void *)&r);
}
template <class T> inline PortAsmArg port_inout_r(T &r)
{
    return port_asm_arg(PORT_ASM_INOUT_R, (int)sizeof(T), 0, 0.0f, (void *)&r);
}
inline PortAsmArg port_out_f(float &r)
{
    return port_asm_arg(PORT_ASM_OUT_F, 4, 0, 0.0f, (void *)&r);
}
#endif

#endif
