// ColorMapper.cpp  @ 0x002853A0 (text 0x2853A0-0x2858E0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// ColorMapper as one translation unit without a static initializer, in
// PSXRenderTarget.cpp's tu_map row: its .rodata restarts at @1449 after the
// QuadTree RTTI and ends with its BaseObject/ColorMapper RTTI. The file name
// is a guess.

#include "ColorMapper/ColorMapper.h"
#include "DataAccess/DataAccess.h"

// ---- src/ColorMapper/Make15and16BitPalettes.cpp


void ColorMapper::Make15and16BitPalettes()
{
    for (int i = 0; i < 256; i++) {
        m_palette15[i] = ((m_palette[i].r >> 3) << 10) | ((m_palette[i].g >> 3) << 5) | (m_palette[i].b >> 3);
        m_palette16[i] = ((m_palette[i].r >> 3) << 11) | ((m_palette[i].g >> 2) << 5) | (m_palette[i].b >> 3);
    }
}


// ---- src/ColorMapper/ctor.cpp
extern "C" void *malloc(unsigned int size);
extern "C" void free(void *p);

void LZWDecompress(void *dst, void *src, int size);

extern DataAccess *LpDataAccess;


// Each table is stored raw when its size matches, else LZW-compressed.
// Every failed read leaves the mapper as is (goto fail: one shared exit).
ColorMapper::ColorMapper(long handle)
{
    int compressed;
    int size;
    if (LpDataAccess->FRead(handle, &m_firstColor, 4, 1) != 1)
        goto fail;
    if (LpDataAccess->FRead(handle, &m_numColors, 4, 1) != 1)
        goto fail;
    if (LpDataAccess->FRead(handle, &compressed, 4, 1) != 1)
        goto fail;
    if (compressed) {
        void *buffer;
        if (!(buffer = malloc(0x10000)))
            goto fail;
        if (LpDataAccess->FRead(handle, &size, 4, 1) != 1)
            goto fail;
        if (size == sizeof(m_palette)) {
            if (LpDataAccess->FRead(handle, m_palette, size, 1) != 1)
                goto fail;
        } else {
            if (LpDataAccess->FRead(handle, buffer, size, 1) != 1)
                goto fail;
            LZWDecompress(m_palette, buffer, sizeof(m_palette));
        }
        if (LpDataAccess->FRead(handle, &size, 4, 1) != 1)
            goto fail;
        if (size == sizeof(m_lookup15)) {
            if (LpDataAccess->FRead(handle, m_lookup15, size, 1) != 1)
                goto fail;
        } else {
            if (LpDataAccess->FRead(handle, buffer, size, 1) != 1)
                goto fail;
            LZWDecompress(m_lookup15, buffer, sizeof(m_lookup15));
        }
        if (LpDataAccess->FRead(handle, &size, 4, 1) != 1)
            goto fail;
        if (size == sizeof(m_lookup16)) {
            if (LpDataAccess->FRead(handle, m_lookup16, size, 1) != 1)
                goto fail;
        } else {
            if (LpDataAccess->FRead(handle, buffer, size, 1) != 1)
                goto fail;
            LZWDecompress(m_lookup16, buffer, sizeof(m_lookup16));
        }
        free(buffer);
    } else {
        if (LpDataAccess->FRead(handle, m_palette, sizeof(m_palette), 1) != 1)
            goto fail;
        if (LpDataAccess->FRead(handle, m_lookup15, sizeof(m_lookup15), 1) != 1)
            goto fail;
        if (LpDataAccess->FRead(handle, m_lookup16, sizeof(m_lookup16), 1) != 1)
            goto fail;
    }
    // Append magenta as an extra colour, mapped from both magenta keys.
    if (m_numColors != 256) {
        m_palette[m_firstColor + m_numColors].r = 0xFF;
        m_palette[m_firstColor + m_numColors].g = 0;
        m_palette[m_firstColor + m_numColors].b = 0xFF;
        m_lookup15[0x7C1F] = m_firstColor + m_numColors;
        m_lookup16[0xF81F] = m_firstColor + m_numColors;
        m_numColors++;
    }
    Make15and16BitPalettes();
    return;
fail:
    ;
}


// ---- src/ColorMapper/dtor.cpp
ColorMapper::~ColorMapper()
{
    LpDataAccess->DetachObject(this);
}


// ---- src/ColorMapper/GetPalette.cpp

unsigned char *ColorMapper::GetPalette()
{
    return (unsigned char *)m_palette;
}


// ---- src/ColorMapper/GetLookup15.cpp

unsigned char *ColorMapper::GetLookup15()
{
    return m_lookup15;
}


// ---- src/ColorMapper/GetLookup16.cpp

unsigned char *ColorMapper::GetLookup16()
{
    return m_lookup16;
}



// Retail's RTTI block has BaseObject's record before ColorMapper's name
// (@1449, then @1451-@1452): mwcc built BaseObject's RTTI as the base of a
// class whose vtable it wrote first (pushed last on its LIFO list: key
// function defined after ColorMapper's). Nothing constructed that class, so
// retail's linker stripped its vtable, RTTI and functions; link_c leaves
// them out.
class StrippedBaseObject : public BaseObject {
public:
    virtual ~StrippedBaseObject();
};

StrippedBaseObject::~StrippedBaseObject()
{
}
