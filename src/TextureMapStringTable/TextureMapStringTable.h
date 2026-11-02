// TextureMapStringTable: the texture name table (LpTextureMapStringTable);
// each entry's pointer is the TextureMap loaded for it. A
// DynamicStringTable, which adds no data to StringTable (its ctor only sets
// the vptr), so 0x20 bytes. DynamicStringTable has no header of its own; it
// is declared here (retail vtable).
#ifndef TEXTUREMAPSTRINGTABLE_TEXTUREMAPSTRINGTABLE_H
#define TEXTUREMAPSTRINGTABLE_TEXTUREMAPSTRINGTABLE_H

#include "StringTable/StringTable.h"

class DynamicStringTable : public StringTable {
public:
    DynamicStringTable();

    // --- overrides in retail slot order ---
    virtual ~DynamicStringTable();
    virtual int Load(char *name);
    virtual int AddItem(char *string, void *pointer);
};

class TextureMapStringTable : public DynamicStringTable {
public:
    TextureMapStringTable();
    void LoadRequested();
    void ReleaseTextureMaps();
    void RevokeRequest(int index);

    virtual ~TextureMapStringTable();
};

#endif
