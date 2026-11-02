// NewHandler.cpp  @ 0x0039F830
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// The MSL C++ runtime's operator new[] and the default new handler as one
// translation unit, 0x39F830-0x39F920: __nwa__FUi,
// std::default_new_handler, std::bad_alloc's destructor and what(), and
// std::_new_handler_func. Its .rodata (0x43D2B0-0x43D328) is a label run
// @34-@47 (the bad_alloc throw string, RTTI, "bad_alloc") after MWRTTI.cpp's
// @214; New.cpp's throw string is @47 again, so a new object follows.
// default_new_handler is a local in retail's .symtab, so _new_handler_func
// (.data 0x40E830), which points at it, is defined here. vtable bad_alloc
// 0x44E450. The file name is a guess (MSL keeps all of this in new.cp, but
// retail has two objects).
//
// The throw(bad_alloc) specifications (as in MSL) put a "!std::bad_alloc!"
// type list in .rodata that retail doesn't have: only the exception tables
// (.exception) refer to it, and retail's linker dropped those (its exception
// table is empty) and the list with them. link_c leaves it out the same way
// (tudrop). Without a specification default_new_handler is 0x48 bytes,
// retail 0x60.

#pragma exceptions on

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
    virtual ~bad_alloc() {}
    virtual const char *what() const { return "bad_alloc"; }
};

typedef void (*new_handler)();

}

void *operator new(unsigned int size) throw(std::bad_alloc);

void *operator new[](unsigned int size) throw(std::bad_alloc)
{
    return ::operator new(size);
}

namespace std {

static void default_new_handler() throw(bad_alloc)
{
    throw bad_alloc();
}

new_handler _new_handler_func = default_new_handler;

}
