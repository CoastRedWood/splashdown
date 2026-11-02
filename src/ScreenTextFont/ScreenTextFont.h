// ScreenTextFont: a bitmap font, 256 glyph rectangles plus kerning pairs read
// from a .cel file (Load) and the font texture. Own vptr at +0 (the vtable is
// just the dtor). 0x414 bytes (new in ScreenTextFontManager::CreateFont).
// +0x408 is the TextureMap GetTextureMapFromResourceName returns: Load stores
// it, ReleaseMemory calls its Release (slot 2), GetHeight reads its m_height
// (+0x18), JetskiNameMenu::PrepareGeometry clamps its m_usage (+0x10).
#ifndef SCREENTEXTFONT_SCREENTEXTFONT_H
#define SCREENTEXTFONT_SCREENTEXTFONT_H

class TextureMap;
class ScreenTextFontManager;

struct ScreenTextGlyph {
    ScreenTextGlyph(int ascii, float left, float top, float right, float bottom)
    {
        m_ascii = ascii;
        m_left = left;
        m_right = right;
        m_top = top;
        m_bottom = bottom;
    }
    ~ScreenTextGlyph() {}
    int m_ascii;                     // +0x00
    float m_left;                    // +0x04
    float m_right;                   // +0x08
    float m_top;                     // +0x0C
    float m_bottom;                  // +0x10
};

struct ScreenTextKerning {
    short m_first;                   // +0x0
    short m_second;                  // +0x2
    int m_amount;                    // +0x4
};

class ScreenTextFont {
public:
    ScreenTextFont(const char *name);
    virtual ~ScreenTextFont();          // vptr at +0x0
    ScreenTextFont *Create(char *celName, char *textureName);
    int Load(char *celName, char *textureName, int release);
    void ReleaseMemory();
    float GetHeight() const;
    // Inline in retail's ScreenTextFontManager.h (its ASSERT's static is a
    // $localstatic); defined where used, src/tu/ScreenTextManager.cpp.
    ScreenTextGlyph *GetCell(int ascii) const;
    static void Release(ScreenTextFont *font);

    static ScreenTextFontManager FontManager;  // (ScreenTextFontManager.cpp)

    char *m_name;                       // +0x4
    ScreenTextGlyph *m_glyphs[256];     // +0x8
    TextureMap *m_texture;              // +0x408
    unsigned int m_numKerning;          // +0x40C
    ScreenTextKerning *m_kerning;       // +0x410
};

#endif
