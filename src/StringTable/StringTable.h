// StringTable — indexed table of strings with per-entry user pointer and
// request count. A BaseObject, 0x20 bytes (the ctor clears +0x10..+0x1C);
// Material/TextureMap/Dynamic... string tables derive from it.
#ifndef STRINGTABLE_H
#define STRINGTABLE_H

#include "GameObject/GameObject.h"

struct StringTableEntry {
    char *string;       // +0x0
    void *pointer;      // +0x4
    int requested;      // +0x8
};

class StringTable : public BaseObject {
public:
    StringTable();
    void Request(int index);
    void RevokeRequest(int index);
    char *GetString(int index);
    int GetIndex(char *string);
    void *GetPointer(int index);
    int GetRequested(int index);
    void SetPointer(int index, void *pointer);
    void ClearRequested(int index);

    // --- virtuals in retail slot order ---
    virtual ~StringTable();
    virtual int Load(char *name);
    virtual int AddItem(char *string, void *pointer);

    int m_totalEntries;           // +0x10
    int m_numEntries;             // +0x14 (AddItem: m_maxEntries)
    char *m_buffer;               // +0x18, string data
    StringTableEntry *m_entries;  // +0x1C
};

#endif
