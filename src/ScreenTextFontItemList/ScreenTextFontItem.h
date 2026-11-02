// ScreenTextFontItem: a node of ScreenTextFontItemList (8 bytes: AddScreenTextFont
// news 8 and stores the font, then a null next pointer).
#ifndef SCREENTEXTFONTITEMLIST_SCREENTEXTFONTITEM_H
#define SCREENTEXTFONTITEMLIST_SCREENTEXTFONTITEM_H

class ScreenTextFont;

struct ScreenTextFontItem {
    ScreenTextFont *m_font;         // +0x0
    ScreenTextFontItem *m_next;     // +0x4
};

#endif
