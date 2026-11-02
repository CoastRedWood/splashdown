// FabsInline.h — retail's fabs helper: abs.s into $f8, stored through a
// stack temporary.
#ifndef GLOBAL_FABSINLINE_H
#define GLOBAL_FABSINLINE_H

static inline float Fabs(float x)
{
    float r;
    asm("abs.s $f8, %1; swc1 $f8, 0(%0)" : : "r"(&r), "f"(x) : "$f8", "memory");
    return r;
}

#endif
