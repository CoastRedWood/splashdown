// GlobalDestructorChain.cpp  @ 0x0039FD40
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// The C++ runtime's global destructor chain and static-initializer runner
// as one translation unit, 0x39FD40-0x39FE20:
// __destroy_global_chain, __register_global_object, __initialize_cpp_rts,
// and .bss __global_destructor_chain (0x4E43C8). No .rodata marks its ends;
// it could be part of NMWException.cpp's or ExceptionHandling.cpp's object.
// The file name is a guess (MSL's global_destructor_chain.c).

#pragma exceptions on

typedef void (*DestructorFunc)(void *, int);

struct DestructorChain {
    DestructorChain *next;
    DestructorFunc destructor;
    void *object;
};

extern "C" DestructorChain *__global_destructor_chain;
DestructorChain *__global_destructor_chain;

// Run the destructors registered by __register_global_object, newest first.
extern "C" void __destroy_global_chain(void)
{
    DestructorChain *gdc;

    while ((gdc = __global_destructor_chain) != 0) {
        __global_destructor_chain = gdc->next;
        gdc->destructor(gdc->object, -1);
    }
}

// Global objects with destructors are pushed on a singly linked chain that
// __destroy_global_chain walks at exit.
extern "C" void *__register_global_object(void *object, DestructorFunc destructor,
                                          DestructorChain *regmem)
{
    regmem->next = __global_destructor_chain;
    regmem->destructor = destructor;
    regmem->object = object;
    __global_destructor_chain = regmem;
    return object;
}

// Run the static initializers in [start, end).
typedef void (*StaticInitializer)(void);

extern "C" void __initialize_cpp_rts(StaticInitializer *start, StaticInitializer *end)
{
    for (StaticInitializer *p = start; p < end; p++)
        (*p)();
}
