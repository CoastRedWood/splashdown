// ScreenTextFontItemList: a singly linked list of the loaded ScreenTextFonts
// (LpGame->m_fonts). No vtable; 8 bytes. Data from its own methods
// (src/ScreenTextFontItemList/): Clear and GetScreenTextFont walk m_head,
// AddScreenTextFont appends and counts in +0x4.
#ifndef SCREENTEXTFONTITEMLIST_SCREENTEXTFONTITEMLIST_H
#define SCREENTEXTFONTITEMLIST_SCREENTEXTFONTITEMLIST_H

#include "ScreenTextFontItemList/ScreenTextFontItem.h"

class ScreenTextFontItemList {
public:
    ScreenTextFontItemList();
    ~ScreenTextFontItemList();
    void AddScreenTextFont(ScreenTextFont *font);
    void Clear();
    ScreenTextFontItem *GetScreenTextFont(char *name) const;
    ScreenTextFontItem *GetScreenTextFont(int index) const;

    ScreenTextFontItem *m_head;     // +0x0
    int m_count;                    // +0x4
};

#endif
