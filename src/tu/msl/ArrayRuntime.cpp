// ArrayRuntime.cpp  @ 0x0039F2E0
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// The MSL C++ runtime's array new/delete helpers as one translation unit
//: __destroy_new_array, __destroy_arr,
// __construct_array, __construct_new_array (0x39F2E0-0x39F670). Their text
// is contiguous and they reference no data. MSL's sources keep them in
// NMWException.cp, but retail links __throw_catch_compare and the
// terminate/unexpected handlers (NMWException.cpp here) as a separate object
// after operator new, so this build had them in a file of their own. The
// file name is a guess.

// The C++ runtime (MSL) was built with exceptions on, unlike game code.
#pragma exceptions on

typedef unsigned int size_t;
typedef void *ConstructorDestructor;

#define ARRAY_HEADER_SIZE 16
#define CTORARG_TYPE short
#define CTORCALL_COMPLETE(ctor, objptr) (((void (*)(void *, CTORARG_TYPE))ctor)(objptr, 1))
#define DTORCALL_COMPLETE(dtor, objptr) (((void (*)(void *, CTORARG_TYPE))dtor)(objptr, -1))

void operator delete[](void *p);

class __partial_array_destructor {
private:
    void *p;
    size_t size;
    size_t n;
    ConstructorDestructor dtor;

public:
    size_t i;

    __partial_array_destructor(void *array, size_t elementsize, size_t nelements, ConstructorDestructor destructor)
    {
        p = array;
        size = elementsize;
        n = nelements;
        dtor = destructor;
        i = n;
    }

    ~__partial_array_destructor()
    {
        char *ptr;

        if (i < n && dtor) {
            for (ptr = (char *)p + size * i; i > 0; i--) {
                ptr -= size;
                DTORCALL_COMPLETE(dtor, ptr);
            }
        }
    }
};

extern "C" void __destroy_new_array(void *block, ConstructorDestructor dtor)
{
    if (block) {
        if (dtor) {
            size_t i, objects, objectsize;
            char *p;

            objectsize = *(size_t *)((char *)block - ARRAY_HEADER_SIZE);
            objects = ((size_t *)((char *)block - ARRAY_HEADER_SIZE))[1];
            p = (char *)block + objectsize * objects;
            for (i = 0; i < objects; i++) {
                p -= objectsize;
                DTORCALL_COMPLETE(dtor, p);
            }
        }
        ::operator delete[]((char *)block - ARRAY_HEADER_SIZE);
    }
}

extern "C" void __destroy_arr(void *block, ConstructorDestructor dtor, size_t size, size_t n)
{
    char *p;

    for (p = (char *)block + size * n; n > 0; n--) {
        p -= size;
        DTORCALL_COMPLETE(dtor, p);
    }
}

extern "C" void __construct_array(void *block, ConstructorDestructor ctor, ConstructorDestructor dtor, size_t size, size_t n)
{
    __partial_array_destructor pad(block, size, n, dtor);
    char *p;

    for (pad.i = 0, p = (char *)block; pad.i < n; pad.i++, p += size)
        CTORCALL_COMPLETE(ctor, p);
}

extern "C" void *__construct_new_array(void *block, ConstructorDestructor ctor, ConstructorDestructor dtor, size_t size, size_t n)
{
    char *ptr;

    if ((ptr = (char *)block) != 0) {
        size_t *p = (size_t *)ptr;
        p[0] = size;
        p[1] = n;
        ptr += ARRAY_HEADER_SIZE;
        if (ctor) {
            __partial_array_destructor pad(ptr, size, n, dtor);
            char *p;

            for (pad.i = 0, p = (char *)ptr; pad.i < n; pad.i++, p += size)
                CTORCALL_COMPLETE(ctor, p);
        }
    }
    return ptr;
}
