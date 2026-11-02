// ScreenTextFontManager.cpp  @ 0x0036F0D0 (text 0x36F0D0-0x370100)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's ScreenTextFontManager.cpp as one translation unit: its functions in
// retail order, then its static initializer. `make link` places the object's data
// as well as its code (tools/link_c.py).



#include "ScreenTextFont/ScreenTextFont.h"
#include "TextureMap/TextureMap.h"
#include "RenderTarget/RenderTarget.h"
#include "soultree_assert_block.h"
#include "d3dtypes.h"
#include "DataAccess/DataAccess.h"
#include "ScreenTextFontItemList/ScreenTextFontItemList.h"
#include "ScreenTextFontItemList/ScreenTextFontItem.h"
#include "ScreenTextFontManager/ScreenTextFontManager.h"

// The font manager, constructed by the static initializer; retail's .bss
// has it (after its destructor record) before the functions' statics.
ScreenTextFontManager ScreenTextFont::FontManager;

// ---- src/ScreenTextFont/ctor.cpp
extern "C" unsigned int strlen(const char *s);
extern "C" char *strcpy(char *dst, const char *src);
extern "C" int strcmp(const char *, const char *);
extern "C" int sprintf(char *buf, const char *fmt, ...);
extern "C" char *strcat(char *dst, const char *src);
extern char FontContentDirectory[];


ScreenTextFont::ScreenTextFont(const char *name)
{
    m_name = 0;
    for (int i = 0; i < 256; i++)
        m_glyphs[i] = 0;
    m_texture = 0;
    m_numKerning = 0;
    m_kerning = 0;
    m_name = new char[strlen(name) + 1];
    strcpy(m_name, name);
}


// ---- src/ScreenTextFont/Release.cpp

void ScreenTextFont::Release(ScreenTextFont *font)
{
    if (font)
        delete font;
}


// ---- src/ScreenTextFont/ReleaseMemory.cpp

void ScreenTextFont::ReleaseMemory()
{
    for (int i = 0; i < 256; i++) {
        if (m_glyphs[i]) {
            delete m_glyphs[i];
            m_glyphs[i] = 0;
        }
    }
    if (m_kerning) {
        delete[] m_kerning;
        m_kerning = 0;
    }
    if (m_texture) {
        m_texture->Release();
        m_texture = 0;
    }
}


// ---- src/ScreenTextFont/dtor.cpp

extern RenderTarget *LpRenderTarget;


ScreenTextFont::~ScreenTextFont()
{
    if (LpRenderTarget) {
        LpRenderTarget->TrySendRenderQueue(1);
        LpRenderTarget->WaitForRenderingToComplete();
    }
    ReleaseMemory();
    delete[] m_name;
    m_name = 0;
}


// ---- src/ScreenTextFont/GetHeight.cpp

float ScreenTextFont::GetHeight() const
{
    int i;
    for (i = 0; i < 256 && !m_glyphs[i]; i++)
        ;
    if (i < 256) {
        ScreenTextGlyph *glyph = m_glyphs[i];
        float h = glyph->m_bottom - glyph->m_top;
        return m_texture->m_height * h;
    }
    return -1.0f;
}


// ---- src/ScreenTextFont/Create.cpp

ScreenTextFont *ScreenTextFont::Create(char *celName, char *textureName)
{
    Load(celName, textureName, 0);
    return this;
}


// ---- src/ScreenTextFont/Load.cpp

extern DataAccess *LpDataAccess;


struct Palette8;

TextureMap *GetTextureMapFromResourceName(char *name, int flags, Palette8 *palette, unsigned int a,
                                       _D3DBLEND src, _D3DBLEND dst, int alpha, unsigned int colorKey, int b);

// Reads a .cel font description (glyph rectangles, then optional kerning
// pairs) and loads the font texture.
int ScreenTextFont::Load(char *celName, char *textureName, int release)
{
    unsigned int count = 0;
    if (release)
        ReleaseMemory();
    long file = LpDataAccess->FOpen(celName, "rb");
    if (file == -1) {
#line 170 "ScreenTextFontManager.cpp"
        ASSERTF(0, ("cannot open %s.cel file", m_name));
        return 0;
    }
    LpDataAccess->FRead(file, &count, 4, 1);
    int ascii = 0;
    float left = 0.0f;
    float top = 0.0f;
    float right = 0.0f;
    float bottom = 0.0f;
    unsigned int i;
    for (i = 0; i < count; i++) {
        LpDataAccess->FRead(file, &ascii, 4, 1);
#line 189 "ScreenTextFontManager.cpp"
        ASSERTF(ascii >= 0 && ascii < 256, ("incorrect ascii character ascii=%d\n", ascii));
        if (!(LpDataAccess->FRead(file, &left, 4, 1) && LpDataAccess->FRead(file, &top, 4, 1)
              && LpDataAccess->FRead(file, &right, 4, 1) && LpDataAccess->FRead(file, &bottom, 4, 1))) {
#line 197 "ScreenTextFontManager.cpp"
            ASSERT(0, "Invalid .cel file!\n");
            break;
        }
#line 200 "ScreenTextFontManager.cpp"
        ASSERTF(left < right && top < bottom, ("Incorrect uvs : left=%f\n, top=%f\n, right=%f\n, bottom=%f\n", left, top, right, bottom));
        m_glyphs[ascii] = new ScreenTextGlyph(ascii, left, top, right, bottom);
    }
    if (!LpDataAccess->FEof(file)) {
        LpDataAccess->FRead(file, &m_numKerning, 4, 1);
        if (m_numKerning) {
            m_kerning = new ScreenTextKerning[m_numKerning];
            for (i = 0; i < m_numKerning; i++) {
                if (!(LpDataAccess->FRead(file, &m_kerning[i].m_first, 2, 1)
                      && LpDataAccess->FRead(file, &m_kerning[i].m_second, 2, 1)
                      && LpDataAccess->FRead(file, &m_kerning[i].m_amount, 4, 1))) {
#line 219 "ScreenTextFontManager.cpp"
                    ASSERT(0, "Invalid .cel file!\n");
                    break;
                }
            }
        }
    }
    m_texture = GetTextureMapFromResourceName(textureName, 0x22B8, 0, 0, (_D3DBLEND)2, (_D3DBLEND)1, 0x80, 0xFF00FF, 1);
#line 231 "ScreenTextFontManager.cpp"
    ASSERTF(m_texture != 0, ("Could not load font texture %s\n", textureName));
    LpDataAccess->FClose(file);
    return 1;
}


// ---- src/ScreenTextFontItemList/ctor.cpp

ScreenTextFontItemList::ScreenTextFontItemList()
{
    m_head = 0;
    m_count = 0;
}


// ---- src/ScreenTextFontItemList/dtor.cpp

ScreenTextFontItemList::~ScreenTextFontItemList()
{
    Clear();
}


// ---- src/ScreenTextFontItemList/AddScreenTextFont.cpp

// Appends the font at the tail of the list.
void ScreenTextFontItemList::AddScreenTextFont(ScreenTextFont *font)
{
    ScreenTextFontItem *tail = 0;
    ScreenTextFontItem *item = m_head;
    while (item) {
        tail = item;
        item = item->m_next;
    }
    if (!tail) {
        ScreenTextFontItem *added = new ScreenTextFontItem;
        if (added) {
            added->m_font = font;
            added->m_next = 0;
        }
        m_head = added;
    } else {
        ScreenTextFontItem *added = new ScreenTextFontItem;
        if (added) {
            added->m_font = font;
            added->m_next = 0;
        }
        tail->m_next = added;
    }
    m_count++;
}


// ---- src/ScreenTextFontItemList/Clear.cpp


void ScreenTextFontItemList::Clear()
{
    ScreenTextFontItem *item;
    while ((item = m_head) != 0) {
        m_head = item->m_next;
        ScreenTextFont::Release(item->m_font);
        delete item;
        m_count--;
    }
}


// ---- src/ScreenTextFontItemList/GetScreenTextFont.cpp
#pragma global_optimizer off  // retail: unrotated loops, as without the global optimizer

ScreenTextFontItem *ScreenTextFontItemList::GetScreenTextFont(char *name) const
{
    ScreenTextFontItem *item = m_head;
    while (item) {
        if (strcmp(name, item->m_font->m_name) == 0)
            break;
        item = item->m_next;
    }
    return item;
}


// ---- src/ScreenTextFontItemList/GetScreenTextFont_0036FBE0.cpp
ScreenTextFontItem *ScreenTextFontItemList::GetScreenTextFont(int index) const
{
    ScreenTextFontItem *item = m_head;
    int i = 0;
    while (item) {
        if (i == index)
            break;
        item = item->m_next;
        i++;
    }
    return item;
}


// ---- src/ScreenTextFontManager/Create.cpp
#pragma global_optimizer reset

ScreenTextFontManager *ScreenTextFontManager::Create()
{
    return this;
}


// ---- src/ScreenTextFontManager/dtor.cpp
ScreenTextFontManager::~ScreenTextFontManager()
{
    m_fonts.Clear();
}


// ---- src/ScreenTextFontManager/CreateFont.cpp


void ScreenTextFontManager::CreateFont(char *name, int reload)
{
    char celFile[0x110];
    char tgaFile[0x110];
    sprintf(celFile, "%s%s.cel", FontContentDirectory, name);
    sprintf(tgaFile, "%s%s.tga", FontContentDirectory, name);
    ScreenTextFontItem *item = m_fonts.GetScreenTextFont(name);
    ScreenTextFont *font = item ? item->m_font : 0;
    if (!font) {
        font = new ScreenTextFont(name);
        font->Create(celFile, tgaFile);
        m_fonts.AddScreenTextFont(font);
    } else if (reload) {
        font->Load(celFile, tgaFile, 1);
    }
}


// ---- src/ScreenTextFontManager/CreateFonts.cpp
void ScreenTextFontManager::CreateFonts(int count, char **names, int reload)
{
    for (int i = 0; i < count; i++)
        CreateFont(names[i], reload);
}


// ---- src/ScreenTextFontManager/ReloadAllFonts.cpp


void ScreenTextFontManager::ReloadAllFonts()
{
    char celFile[0x50];
    char tgaFile[0x50];
    int i;
    int n;
    int count = m_fonts.m_count;
    celFile[0] = 0;
    tgaFile[0] = 0;
    for (i = 0; i < count; i++) {
        ScreenTextFontItem *item = m_fonts.GetScreenTextFont(i);
        ScreenTextFont *font = item ? item->m_font : 0;
        if (font) {
            n = sizeof(celFile);
            if (n == sizeof(char *)) {
#line 589 "ScreenTextFontManager.cpp"
                ASSERT(0, "STRCPY: len of string is sizeof(char*)");
            }
            {
                int len = strlen(FontContentDirectory);
                n = (n - 1 < len) ? n - 1 : len;
            }
            strncpy(celFile, FontContentDirectory, n);
            celFile[n] = 0;
            strcat(celFile, font->m_name);
            strcat(celFile, ".cel");
            n = sizeof(tgaFile);
            if (n == sizeof(char *)) {
#line 593 "ScreenTextFontManager.cpp"
                ASSERT(0, "STRCPY: len of string is sizeof(char*)");
            }
            {
                int len = strlen(FontContentDirectory);
                n = (n - 1 < len) ? n - 1 : len;
            }
            strncpy(tgaFile, FontContentDirectory, n);
            tgaFile[n] = 0;
            strcat(tgaFile, font->m_name);
            strcat(tgaFile, ".tga");
            font->Load(celFile, tgaFile, 1);
        } else {
            // Retail reads the name through the null font.
#line 601 "ScreenTextFontManager.cpp"
            ASSERTF(0, ("font %s is not currently loaded\n", font->m_name));
        }
    }
}
