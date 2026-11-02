// NMWException.cpp  @ 0x0039FA20
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// The MSL C++ runtime's NMWException.cp as one translation unit,
// 0x39FA20-0x39FD40: __throw_catch_compare, std::unexpected, std::terminate
// and the default handlers duhandler and dthandler, with .data thandler and
// uhandler (0x40E840, 0x40E848). Retail's .symtab has dthandler, duhandler,
// thandler and uhandler as locals, after New.cpp's @47 and before
// ExceptionHandling.cpp's locals. There is no .rodata to mark its ends:
// New.cpp before it and GlobalDestructorChain.cpp after it could belong to
// the same object. The name follows MSL (which also keeps the array helpers,
// ArrayRuntime.cpp here, in it; retail has those elsewhere).


// MSL C++ runtime (NMWException.cp), built with exceptions on.
#pragma exceptions on

extern "C" void abort(void);

namespace std {

typedef void (*terminate_handler)();
typedef void (*unexpected_handler)();

static void dthandler();
static void duhandler();

static terminate_handler thandler = dthandler;
static unexpected_handler uhandler = duhandler;

}

// MSL C++ runtime (NMWException.cp): does a thrown type match a catch type?
// Types are encoded as strings: "!A!0!B!8!!" lists a class and its public
// bases with their offsets, "*" the same for a pointer to class, otherwise a
// mangled type ('P' pointer, 'R' reference, 'C'/'V' cv-qualifiers).
extern "C" int __throw_catch_compare(const char *throwtype, const char *catchtype, long *offset_result)
{
    const char *cptr1, *cptr2;

    *offset_result = 0;
    if ((cptr2 = catchtype) == 0) {
        // catch(...)
        return 1;
    }
    cptr1 = throwtype;

    if (*cptr2 == 'P') {
        // catch(cv T *)
        cptr2++;
        if (*cptr2 == 'C')
            cptr2++;
        if (*cptr2 == 'V')
            cptr2++;
        if (*cptr2 == 'v') {
            // catch(cv void *)
            if (*cptr1 == 'P' || *cptr1 == '*') {
                // throw T *
                return 1;
            }
        }
        cptr2 = catchtype;
    }

    switch (*cptr1) {
    case '*':   // pointer to class
    case '!':   // class
        if (*cptr1++ != *cptr2++)
            return 0;
        for (;;) {
            // class type compare loop
            if (*cptr1 == *cptr2++) {
                if (*cptr1++ == '!') {
                    // class type match
                    long offset;

                    for (offset = 0; *cptr1 != '!';)
                        offset = offset * 10 + *cptr1++ - '0';
                    *offset_result = offset;
                    return 1;
                }
            } else {
                while (*cptr1++ != '!')
                    ;   // skip class name
                while (*cptr1++ != '!')
                    ;   // skip offset
                if (*cptr1 == 0)
                    return 0;   // no more class names => no match
                cptr2 = catchtype + 1;
            }
        }
        return 0;
    }

    while ((*cptr1 == 'P' || *cptr1 == 'R') && *cptr1 == *cptr2) {
        // pointer/reference catch => match cv-qualifiers
        cptr1++;
        cptr2++;

        if (*cptr2 == 'C') {
            if (*cptr1 == 'C')
                cptr1++;
            cptr2++;
        }
        if (*cptr1 == 'C')
            return 0;

        if (*cptr2 == 'V') {
            if (*cptr1 == 'V')
                cptr1++;
            cptr2++;
        }
        if (*cptr1 == 'V')
            return 0;
    }

    // compare pointer type
    for (; *cptr1 == *cptr2; cptr1++, cptr2++)
        if (*cptr1 == 0)
            return 1;
    return 0;
}

namespace std {

void unexpected()
{
    uhandler();
}

void terminate()
{
    thandler();
}

static void duhandler()
{
    thandler();
}

static void dthandler()
{
    abort();
}

}
