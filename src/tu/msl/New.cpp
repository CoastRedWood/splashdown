// New.cpp  @ 0x0039F920
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// The MSL C++ runtime's operator new as one translation unit,
// 0x39F920-0x39FA20: __copy and __nw__FUi, and std::__throws_bad_alloc. Its
// .rodata is one item, the bad_alloc throw string at 0x43D330, label @47
// (NewHandler.cpp's run ends at @47, so this is a new object). .data
// __throws_bad_alloc (0x40E838; it could equally be NewHandler.cpp's, the
// last item of its .data: it is here with its only reader). __copy
// references no data, so it could be NewHandler.cpp's last function
// instead; it is put here, at the head. The file name is a guess.

#pragma exceptions on

extern "C" void *malloc(unsigned int size);

// Metrowerks C++ runtime: byte copy used by the array constructors.
extern "C" void *__copy(char *to, char *from, unsigned int size)
{
    char *p;

    if (to && size) {
        p = to;
        do {
            *p = *from;
            p++;
            from++;
            size--;
        } while (size);
    }
    return to;
}

namespace std {

class exception {
public:
    exception() {}
    virtual ~exception() {}
    virtual const char *what() const;
};

class bad_alloc : public exception {
public:
    bad_alloc() {}
    virtual ~bad_alloc();
    virtual const char *what() const;
};

typedef void (*new_handler)();
extern new_handler _new_handler_func;
// In .data, not $gp small data (MSL's build put 1-byte globals there).
char __throws_bad_alloc __attribute__((section(".data"))) = 1;

}

// optimize_for_size: without it mwcc copies the malloc test in front of the
// loop.
#pragma optimize_for_size on

void *operator new(unsigned int size) throw(std::bad_alloc)
{
    void *ptr;

    if (!size)
        size = 1;
    while ((ptr = malloc(size)) == 0) {
        if (std::_new_handler_func)
            std::_new_handler_func();
        else if (std::__throws_bad_alloc)
            throw std::bad_alloc();
        else
            return 0;
    }
    return ptr;
}

#pragma optimize_for_size reset
