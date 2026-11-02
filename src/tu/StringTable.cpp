// StringTable.cpp  @ 0x00336FD0 (text 0x336FD0-0x337E20)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp)
//
// Retail's StringTable.cpp as one translation unit: a TU without a static
// initializer, the last of WaterWrecker.cpp's tu_map row: StringTable and
// DynamicStringTable (the ASSERTs say "StringTable.cpp"). Its .rodata
// restarts the labels at @409 after MicroProgram.cpp's RTTI. `make link`
// places its data as well as its code.


#include "StringTable/StringTable.h"
#include "TextureMapStringTable/TextureMapStringTable.h"
#include "DataAccess/DataAccess.h"
#include "soultree_assert.h"

extern "C" char *strcpy(char *dst, const char *src);
extern "C" char *strlwr(char *s);
extern "C" void *malloc(unsigned int size);
extern "C" void free(void *p);
extern "C" int tolower(char c);
extern "C" void *memcpy(void *dst, const void *src, unsigned int n);
extern "C" int strcasecmp(const char *a, const char *b);
extern "C" unsigned int strlen(const char *s);
void PSXprintf(const char *fmt, ...);

extern DataAccess *LpDataAccess;

// ---- src/StringTable/ctor.cpp

StringTable::StringTable()
{
    m_totalEntries = 0;
    m_numEntries = 0;
    m_buffer = 0;
    m_entries = 0;
}

// ---- src/StringTable/dtor.cpp

StringTable::~StringTable()
{
    if (m_entries)
        delete[] m_entries;
    free(m_buffer);
}

// ---- src/StringTable/Load.cpp

// Loads a whitespace-separated word list; each word becomes an entry.
int StringTable::Load(char *name)
{
    char path[0x110];
    long size;
    long position;
    int inWord;
    int i;

    if (m_entries)
        delete[] m_entries;
    if (m_buffer)
        free(m_buffer);
    strcpy(path, name);
    for (char *p = path; *p; p++)
        *p = tolower(*p);
    long handle = LpDataAccess->FOpen(path, "rb");
    if (handle == -1) {
        PSXprintf("StringTable '%s' not found\n", path);
        return 0;
    }
    LpDataAccess->GetStatus(handle, &position, &size);
    m_buffer = (char *)malloc(size + 1);
    m_buffer[size] = 0;
    LpDataAccess->FRead(handle, m_buffer, size, 1);
    LpDataAccess->FClose(handle);
    strlwr(m_buffer);

    m_totalEntries = 0;
    inWord = 0;
    for (i = 0; i < size; i++) {
        char c = m_buffer[i];
        if (c == '\r') {
            m_buffer[i] = 0;
            inWord = 0;
        } else if (c == '\n') {
            m_buffer[i] = 0;
            inWord = 0;
        } else if (c == ' ') {
            m_buffer[i] = 0;
            inWord = 0;
        } else if (!inWord) {
            inWord = 1;
            m_totalEntries++;
        }
    }
    m_entries = new StringTableEntry[m_totalEntries];
    int entry = 0;
    inWord = 0;
    for (i = 0; i < size; i++) {
        if (!m_buffer[i]) {
            inWord = 0;
        } else if (!inWord) {
            inWord = 1;
            m_entries[entry].pointer = 0;
            m_entries[entry].string = m_buffer + i;
            m_entries[entry].requested = 0;
            entry++;
        }
    }
    m_numEntries = m_totalEntries;
    return m_totalEntries;
}

// ---- src/StringTable/AddItem.cpp

int StringTable::AddItem(char *string, void *pointer)
{
    if (!m_entries) {
        m_entries = new StringTableEntry[16];
        m_numEntries = 16;
        m_totalEntries = 0;
    } else if (m_totalEntries >= m_numEntries) {
        StringTableEntry *entries = new StringTableEntry[m_numEntries + 16];
        memcpy(entries, m_entries, m_numEntries * sizeof(StringTableEntry));
        delete[] m_entries;
        m_entries = entries;
        m_numEntries += 16;
    }
    m_entries[m_totalEntries].string = string;
    m_entries[m_totalEntries].pointer = pointer;
    m_entries[m_totalEntries].requested = 0;
    m_totalEntries++;
    return m_totalEntries - 1;
}

// ---- src/StringTable/Request.cpp
// ASSERT contains inline asm; retail's label alignment for asm functions
// (through ClearRequested).
#pragma alignlabel off

void StringTable::Request(int index)
{
#line 173 "StringTable.cpp"
    ASSERTF(index >= 0 && index <= m_totalEntries, ("Request: index (%d) < 0 || index > TotalEntries (%d)", index, m_totalEntries));
    m_entries[index].requested++;
}

// ---- src/StringTable/RevokeRequest.cpp

void StringTable::RevokeRequest(int index)
{
#line 181 "StringTable.cpp"
    ASSERTF(index >= 0 && index <= m_totalEntries, ("RevokeRequest: index (%d) < 0 || index > TotalEntries (%d)", index, m_totalEntries));
    ASSERTF(m_entries[index].requested > 0, ("RevokeRequest: index %d revoked too many times", index));
    m_entries[index].requested--;
}

// ---- src/StringTable/GetIndex.cpp

int StringTable::GetIndex(char *string)
{
    for (int i = 0; i < m_totalEntries; i++) {
        if (strcasecmp(string, m_entries[i].string) == 0)
            return i;
    }
#line 200 "StringTable.cpp"
    ASSERTF(0, ("GetIndex: Couldn't find the string '%s'", string));
    return -1;
}

// ---- src/StringTable/GetString.cpp

char *StringTable::GetString(int index)
{
    if (index < 0 || index > m_totalEntries) {
#line 210 "StringTable.cpp"
        ASSERTF(0, ("GetString: index (%d) < 0 || index > TotalEntries (%d)", index, m_totalEntries));
    }
    return m_entries[index].string;
}

// ---- src/StringTable/GetPointer.cpp

void *StringTable::GetPointer(int index)
{
    if (index < 0 || index > m_totalEntries) {
#line 220 "StringTable.cpp"
        ASSERTF(0, ("GetPointer: index (%d) < 0 || index > TotalEntries (%d)", index, m_totalEntries));
    }
    return m_entries[index].pointer;
}

// ---- src/StringTable/GetRequested.cpp

int StringTable::GetRequested(int index)
{
    if (index < 0 || index > m_totalEntries) {
#line 230 "StringTable.cpp"
        ASSERTF(0, ("GetPointer: index (%d) < 0 || index > TotalEntries (%d)", index, m_totalEntries));
    }
    return m_entries[index].requested;
}

// ---- src/StringTable/SetPointer.cpp

void StringTable::SetPointer(int index, void *pointer)
{
    if (index < 0 || index > m_totalEntries) {
#line 241 "StringTable.cpp"
        ASSERTF(0, ("SetPointer: index (%d) < 0 || index > TotalEntries (%d)", index, m_totalEntries));
    }
    m_entries[index].pointer = pointer;
}

// ---- src/StringTable/ClearRequested.cpp

void StringTable::ClearRequested(int index)
{
    if (index < 0 || index > m_totalEntries) {
#line 251 "StringTable.cpp"
        ASSERTF(0, ("GetPointer: index (%d) < 0 || index > TotalEntries (%d)", index, m_totalEntries));
    }
    m_entries[index].requested = 0;
}

#pragma alignlabel reset

// ---- src/DynamicStringTable/ctor.cpp

DynamicStringTable::DynamicStringTable()
{
}

// ---- src/DynamicStringTable/dtor.cpp

DynamicStringTable::~DynamicStringTable()
{
    for (int i = 0; i < m_totalEntries; i++) {
        if (m_entries[i].string)
            free(m_entries[i].string);
    }
}

// ---- src/DynamicStringTable/Load.cpp

#pragma alignlabel off
int DynamicStringTable::Load(char *)
{
#line 276 "StringTable.cpp"
    ASSERT(0, "DynamicStringTable::Load: cannot load into type DynamicStringTable");
    return 0;
}
#pragma alignlabel reset

// ---- src/DynamicStringTable/AddItem.cpp

// A copy of the string is kept; an existing string returns its index.
int DynamicStringTable::AddItem(char *string, void *pointer)
{
    for (int i = 0; i < m_totalEntries; i++) {
        if (strcasecmp(string, m_entries[i].string) == 0)
            return i;
    }
    int index = StringTable::AddItem(string, pointer);
    m_entries[index].string = (char *)malloc(strlen(string) + 1);
    strcpy(m_entries[index].string, string);
    return index;
}
