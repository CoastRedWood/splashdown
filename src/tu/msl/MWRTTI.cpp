// MWRTTI.cpp  @ 0x0039F6D0
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// The MSL C++ runtime's RTTI support as one translation unit,
// 0x39F6D0-0x39F830: std::exception's and std::bad_typeid's destructors and
// whats, __get_typeid and its `unknown_type`. Its .rodata
// (0x43D218-0x43D2B0) starts a compiler label run (@47 "???", @192 the
// bad_typeid throw string, @204-@214) after zlib's, and the next object
// (NewHandler.cpp) restarts at @34. .data unknown_type (0x40E828, a local
// in retail's .symtab, hence static); vtables bad_typeid and exception
// (0x44E430, 0x44E440). Retail's .symtab gives the destructors, whats and
// vtables mwcc's inline binding, so the virtuals are defined in the
// classes, as in MSL's <exception> and <typeinfo>. MSL's file is MWRTTI.cp;
// the name is a guess from it.

#pragma exceptions on

namespace std {

class type_info {
public:
    const char *tname;
    const void *tbase;
};

class exception {
public:
    exception() {}
    virtual ~exception() {}
    virtual const char *what() const { return "exception"; }
};

class bad_typeid : public exception {
public:
    bad_typeid() {}
    virtual ~bad_typeid() {}
    virtual const char *what() const { return "bad_typeid"; }
};

}

static std::type_info unknown_type = {"???", 0};  // for objects whose vtable has no RTTI

// mwcc writes the out-of-line copy of an inline function a function needs (a
// thrown type's dtor) after the next top-level declaration (a variable or a
// function body; a prototype doesn't count), or with the vtables at the TU's
// end if none follows. Retail has exception's dtor before __get_typeid and
// bad_typeid's right after it: MSL's file had code throwing before
// __get_typeid (__dynamic_cast's bad_cast) and more after it, which the game
// never called and retail's linker stripped. link_c leaves the stand-in out.

static void StrippedThrow(void *p)
{
    if (!p)
        throw std::exception();
}

extern "C" const std::type_info *__get_typeid(void *object, long offset)
{
    if (!object)
        throw std::bad_typeid();
    // The vtable at `offset` in the object starts with its type_info pointer.
    object = **(void ***)((char *)object + offset);
    if (!object)
        return &unknown_type;
    return (const std::type_info *)object;
}

// (MSL's code after __get_typeid; see above)
static void StrippedTail()
{
}
