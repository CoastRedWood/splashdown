// TextureMapStringTable.cpp  @ 0x00396DA0 (text 0x396DA0-0x397050)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// TextureMapStringTable as one translation unit, the first of its tu_map
// row: its .rodata is @479-@485 and the TextureMapStringTable RTTI, its
// .data LpTextureMapStringTable, its .bss GTextureMapStringTable.




#include "d3dtypes.h"
#include "TextureMapStringTable/TextureMapStringTable.h"
#include "TextureMap/TextureMap.h"

extern "C" void free(void *p);

class Palette8;

TextureMap *GetTextureMapFromResourceName(char *name, int flags, Palette8 *palette, unsigned int a,
                                          _D3DBLEND src, _D3DBLEND dst, int alpha, unsigned int colorKey, int b);

TextureMapStringTable GTextureMapStringTable;
TextureMapStringTable *LpTextureMapStringTable = &GTextureMapStringTable;

TextureMapStringTable::TextureMapStringTable()
{
}

TextureMapStringTable::~TextureMapStringTable()
{
    ReleaseTextureMaps();
}

void TextureMapStringTable::ReleaseTextureMaps()
{
    for (int i = 0; i < m_totalEntries; i++) {
        if (m_entries[i].pointer) {
            ((TextureMap *)m_entries[i].pointer)->Release();
            m_entries[i].pointer = 0;
        }
        if (m_entries[i].string) {
            free(m_entries[i].string);
            m_entries[i].string = 0;
        }
        m_entries[i].requested = 0;
    }
    m_totalEntries = 0;
}

void TextureMapStringTable::LoadRequested()
{
    for (int i = 0; i < m_totalEntries; i++) {
        if (m_entries[i].requested > 0 && !m_entries[i].pointer)
            m_entries[i].pointer = GetTextureMapFromResourceName(m_entries[i].string, 0x613, 0, 2, (_D3DBLEND)5,
                                                                 (_D3DBLEND)6, 0x80, 0xFF00FF, 1);
    }
}

void TextureMapStringTable::RevokeRequest(int index)
{
    StringTable::RevokeRequest(index);
    if (!m_entries[index].requested && m_entries[index].pointer) {
        ((TextureMap *)m_entries[index].pointer)->Release();
        m_entries[index].pointer = 0;
    }
}
