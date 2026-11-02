// Delete.cpp  @ 0x0039F670
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// The MSL C++ runtime's operator delete[] and operator delete as one
// translation unit, 0x39F670-0x39F6D0. They reference
// no data; the object after them (MWRTTI.cpp) starts a new .rodata label run,
// and the array helpers before them (ArrayRuntime.cpp) are a separate guess.
// That these two are an object of their own is a guess too, as is the file
// name.

// MSL C++ runtime: built with exceptions on, so the throw() spec gets an
// __unexpected handler.
#pragma exceptions on

extern "C" void free(void *ptr);

void operator delete(void *ptr) throw();

void operator delete[](void *ptr)
{
    ::operator delete(ptr);
}

void operator delete(void *ptr) throw()
{
    free(ptr);
}
