// ScreenTextFontManager: the loaded fonts (ScreenTextFont::FontManager, the
// one instance): a ScreenTextFontItemList of fonts loaded by name from
// FontContentDirectory. 8 bytes, no vtable; the list is a member (the dtor
// destroys it as one). Methods in
// src/tu/ScreenTextFontManager.cpp.
#ifndef SCREENTEXTFONTMANAGER_SCREENTEXTFONTMANAGER_H
#define SCREENTEXTFONTMANAGER_SCREENTEXTFONTMANAGER_H

#include "ScreenTextFontItemList/ScreenTextFontItemList.h"

class ScreenTextFontManager {
public:
    ~ScreenTextFontManager();
    ScreenTextFontManager *Create();
    void CreateFont(char *name, int reload);
    void CreateFonts(int count, char **names, int reload);
    void ReloadAllFonts();

    ScreenTextFontItemList m_fonts;     // +0x0
};

#endif
