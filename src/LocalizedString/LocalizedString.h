// LocalizedString: one entry of GameText::m_strings (8 bytes, read from a
// .loc index; src/GameText/Create.cpp).
#ifndef LOCALIZEDSTRING_LOCALIZEDSTRING_H
#define LOCALIZEDSTRING_LOCALIZEDSTRING_H

struct LocalizedString {
    char *m_string;                 // +0x0, into GameText::m_data
    short m_maxLength;              // +0x4 (GetMaxStringLength reads it with lh,
                                    // Create stores it with sh)
    char pad6[0x8 - 0x6];
};

#endif
