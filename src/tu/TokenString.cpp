// TokenString.cpp  @ 0x0029D010 (text 0x29D010-0x29D160)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's TokenString.cpp as one translation unit: a TU without an
// initializer or data at the end of Terrain.cpp's tu_map row, just before
// TypeRegistry.cpp (link order is alphabetical, ignoring case).



#include "TokenString/TokenString.h"

extern "C" unsigned int strlen(const char *s);
extern "C" char *strpbrk(const char *s, const char *accept);

// ---- src/TokenString/ctor.cpp

TokenString::TokenString(char *s)
{
    if (strlen(s))
        m_next = s;
    else
        m_next = 0;
}


// ---- src/TokenString/NextToken.cpp

// strchr as retail inlined it.
inline char *InlineStrchr(const char *s, int c)
{
    while (*s && *s != (char)c)
        s++;
    return (*s == (char)c) ? (char *)s : 0;
}

char *TokenString::NextToken(const char *delims)
{
    if (!m_next)
        return 0;
    m_token = m_next;
    m_next = strpbrk(m_token, delims);
    if (m_next) {
        *m_next++ = 0;
        if (*m_next == 0) {
            m_next = 0;
        } else {
            while (InlineStrchr(delims, *m_next))
                m_next++;
        }
    }
    return m_token;
}
