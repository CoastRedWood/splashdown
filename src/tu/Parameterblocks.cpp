// Parameterblocks.cpp  @ 0x00260390 (text 0x260390-0x262670)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's Parameterblocks.cpp (its ASSERTs' file name) as one translation
// unit, a TU without a static initializer in NormalDistribution.cpp's tu_map
// row: the DataItem helpers, then ParameterBlock's 22 functions. Its .rodata
// (0x416040-0x416290) follows Overlay.cpp's RTTI records with no BaseObject
// restart (ParameterBlock has no vtable). `make link` places the object's data
// as well as its code (tools/link_c.py).




#include "d3dtypes.h"
#include "_RECT/_RECT.h"
#include "ParameterBlock/ParameterBlock.h"
#include "DataAccess/DataAccess.h"
#include "TokenString/TokenString.h"
#include "soultree_assert_block.h"

extern "C" unsigned int strlen(const char *s);
extern "C" int atoi(const char *s);
extern "C" double atof(const char *s);
extern "C" long strtol(const char *s, char **end, int base);
extern "C" void *malloc(unsigned int size);
extern "C" void *realloc(void *p, unsigned int size);
extern "C" void free(void *p);
extern "C" void *memset(void *dst, int c, unsigned int n);
extern "C" void *memcpy(void *dst, const void *src, unsigned int n);
extern "C" char *strcpy(char *dst, const char *src);
extern "C" char *strncpy(char *dst, const char *src, unsigned int n);
extern "C" int strcmp(const char *a, const char *b);
extern "C" int strcasecmp(const char *a, const char *b);
extern "C" int strncasecmp(const char *a, const char *b, unsigned int n);
extern "C" unsigned int strcspn(const char *s, const char *reject);
extern "C" char *strtok(char *s, const char *delim);
extern "C" int sprintf(char *buf, const char *fmt, ...);
extern "C" int tolower(char c);    // (char parameter: retail passes the lb value unextended)
extern "C" const char _ctype_[];

#define _U 01
#define _L 02
#define isalpha(c) ((_ctype_ + 1)[(int)(c)] & (_U | _L))

extern DataAccess *LpDataAccess;

// One [block] of a text file's random-access table.
struct ParameterDataBlock {
    long m_offset;                      // +0x0 (64-bit)
    int m_size;                         // +0x8, in lines
    char m_name[0x418 - 0xC];           // +0xC
};

// A block's place in a binary parameter file.
struct ParameterBinaryBlock {
    unsigned int m_offset;              // +0x0
    unsigned int m_size;                // +0x4
};

int DataItemTypeOfString(char *s);
void StringToDataItem(char *s, DataItem *item);
void ConvertDataItem(DataItem item, float *out);
void ConvertDataItem(DataItem item, char *out);

// ---- src/global/DataItemTypeOfString.cpp

// 2 if the string has a letter, 1 if it has a '.', else 0 (the DataItem
// types string, float and int).
int DataItemTypeOfString(char *s)
{
    int len = strlen(s);
    int hasDot = 0;
    int isString = 0;

    for (int i = 0; i < len; i++) {
        int c = s[i];
        if (isalpha(c)) {
            isString = 1;
            i = len;
        } else if ((char)c == '.') {
            hasDot = 1;
        }
    }
    if (isString)
        return 2;
    return hasDot != 0;
}

// ---- src/global/StringToDataItem.cpp

void StringToDataItem(char *s, DataItem *item)
{
    item->m_type = DataItemTypeOfString(s);
    switch (item->m_type) {
    case 0:
        item->m_value.i = atoi(s);
        break;
    case 1:
        item->m_value.f = atof(s);
        break;
    case 2:
        item->m_value.s = (char *)malloc(strlen(s) + 1);
        strcpy(item->m_value.s, s);
        break;
    }
}

// ---- src/global/ConvertDataItem_audit.cpp

void ConvertDataItem(DataItem item, float *out)
{
    switch (item.m_type) {
    case 0:
        // Retail copies the int's bits rather than converting it.
        *out = *(float *)&item.m_value.i;
        break;
    case 1:
        *out = item.m_value.f;
        break;
    case 2:
        *out = atof(item.m_value.s);
        break;
    }
}

// ---- src/global/ConvertDataItem.cpp
// Compiler session state: retail compiled
// this file in an mwcc session that had already generated a two-argument runtime
// helper call (64-bit / or %, double arithmetic or compare). mwcc then models
// every later helper call (fptodp for the "%f" case here) as reading $a0-$a1.
// This unused function recreates that state; it is not part of the retail file.
static void MWCC_SHIM_TEXT MwccSessionState(long *a) { a[0] = a[0] / a[1]; }

// Prints a data line's value into buf as text.
void ConvertDataItem(DataItem item, char *buf)
{
    switch (item.m_type) {
    case 0:
        sprintf(buf, "%i", item.m_value.i);
        break;
    case 1:
        sprintf(buf, "%f", item.m_value.f);
        break;
    case 2:
        strcpy(buf, item.m_value.s);
        break;
    }
}

// ---- src/ParameterBlock/ctor.cpp

ParameterBlock::ParameterBlock()
{
    m_file = -1;
    m_binary = 0;
    m_open = 0;
    m_writable = 0;
    m_numBlocks = 0;
    m_maxBlocks = 0;
    m_blocks = 0;
    m_numStrings = 0;
    m_strings = 0;
    m_currentBlock[0] = 0;
    m_inDataBlock = 0;
    m_x42C = 0;
    m_dataSize = 0;
    m_dataRead = 0;
    m_numItems = 0;
    memset(m_items, 0, sizeof(m_items));
    m_blockNames = 0;
    m_binaryBlocks = 0;
    m_binaryBlocksSize = 0;
    m_binaryData = 0;
    m_binaryDataSize = 0;
}

// ---- src/ParameterBlock/dtor.cpp

ParameterBlock::~ParameterBlock()
{
    if (m_file != -1) {
        LpDataAccess->FFlush(m_file);
        LpDataAccess->FClose(m_file);
        m_file = -1;
    }
    DeleteParameterStrings();
    if (m_blocks) {
        free(m_blocks);
        m_blocks = 0;
        m_numBlocks = 0;
    }
    for (int i = 0; i < m_numItems; i++) {
        if (m_items[i].m_type == 2)
            free(m_items[i].m_value.s);
        m_items[i].m_value.s = 0;
    }
    m_numItems = 0;
    if (m_blockNames) {
        free(m_blockNames);
        m_blockNames = 0;
    }
    if (m_binaryBlocks) {
        free(m_binaryBlocks);
        m_binaryBlocks = 0;
    }
    if (m_binaryData)
        free(m_binaryData);
}

// ---- src/ParameterBlock/CreateRandomAccessTable.cpp

// Indexes the "[name]" headers of a text parameter file: each block's file
// position and length in lines.
void ParameterBlock::CreateRandomAccessTable(long file)
{
    char stripped[0x400];
    char line[0x400];
    char name[0x400];
    int lineNumber = 0;

    while (LpDataAccess->FGets(file, line, 0x400)) {
        RemoveWhiteSpace(line, stripped);
        if (stripped[0] == '[') {
            int length = strcspn(stripped + 1, "]\n\r");
            strncpy(name, stripped + 1, length);
            name[length] = 0;
            if (FindParameterBlock(name) == -1) {
                if (m_blocks) {
                    if (m_numBlocks >= m_maxBlocks) {
                        m_maxBlocks += 10;
                        m_blocks = (ParameterDataBlock *)realloc(m_blocks, m_maxBlocks * sizeof(ParameterDataBlock));
                    }
                } else {
                    m_blocks = (ParameterDataBlock *)malloc(10 * sizeof(ParameterDataBlock));
                    m_maxBlocks = 10;
                }
                strcpy(m_blocks[m_numBlocks].m_name, name);
                m_blocks[m_numBlocks].m_offset = LpDataAccess->FTell(file);
                m_blocks[m_numBlocks].m_size = lineNumber;
                if (m_numBlocks > 0)
                    m_blocks[m_numBlocks - 1].m_size = lineNumber - m_blocks[m_numBlocks - 1].m_size - 1;
                m_numBlocks++;
            } else {
                PSXprintf("ParameterBlock Warning: Duplicate header [%s] found in file!\n", name);
#line 273 "Parameterblocks.cpp"
                ASSERT(0, "");
            }
        }
        lineNumber++;
    }
    if (m_numBlocks > 0)
        m_blocks[m_numBlocks - 1].m_size = lineNumber - m_blocks[m_numBlocks - 1].m_size - 1;
}

// ---- src/ParameterBlock/OpenFile.cpp

int ParameterBlock::OpenFile(char *name, int writable)
{
    char header[8];
    int version;
    int count;
    unsigned int length;
    int i;

    if (m_blocks) {
        free(m_blocks);
        m_blocks = 0;
        m_numBlocks = 0;
    }
    if (m_file != -1) {
        LpDataAccess->FClose(m_file);
        m_file = -1;
    }
    if (m_binaryBlocks) {
        free(m_binaryBlocks);
        m_binaryBlocks = 0;
    }
    if (m_binaryData) {
        free(m_binaryData);
        m_binaryData = 0;
    }
    if (writable) {
        m_file = LpDataAccess->FOpen(name, "r+b");
        if (m_file == -1)
            return 0;
        m_open = 1;
        m_writable = 1;
        m_x42C = 0;
        m_binary = 0;
        CreateRandomAccessTable(m_file);
        m_inDataBlock = 0;
    } else {
        m_file = LpDataAccess->FOpen(name, "rb");
        if (m_file == -1)
            return 0;
        m_x42C = 0;
        m_open = 1;
        m_writable = 0;
        LpDataAccess->FRead(m_file, header, 7, 1);
        header[6] = 0;
        if (!strcmp(header, "BINARY")) {
            m_binary = 1;
            LpDataAccess->FRead(m_file, &version, 4, 1);
            if (version != 1) {
                LpDataAccess->FClose(m_file);
                m_file = -1;
                return 0;
            }
            LpDataAccess->FRead(m_file, &count, 4, 1);
            if (!count) {
                LpDataAccess->FClose(m_file);
                m_file = -1;
                return 0;
            }
            m_numBlocks = count;
            m_blockNames = (char *)malloc((long)m_numBlocks * 0x400);
            char *p = m_blockNames;
            if (!p) {
                LpDataAccess->FClose(m_file);
                m_file = -1;
                return 0;
            }
            i = 0;
            do {
                length = 0;
                LpDataAccess->FRead(m_file, &length, 4, 1);
                LpDataAccess->FRead(m_file, p, length, 1);
                i++;
                p += 0x400;
            } while (i != m_numBlocks);
            m_binaryBlocksSize = m_numBlocks * 8;
            m_binaryBlocks = (ParameterBinaryBlock *)malloc(m_binaryBlocksSize);
            LpDataAccess->FRead(m_file, m_binaryBlocks, m_binaryBlocksSize, 1);
            return 1;
        }
        m_binary = 0;
        LpDataAccess->FSeek(m_file, 0, 0);
        CreateRandomAccessTable(m_file);
        m_inDataBlock = 0;
    }
    return 1;
}

// ---- src/ParameterBlock/FindParameterBlock.cpp

// Index of the named block: a binary search over the sorted, lower-case
// names of a binary file, else a linear search.
int ParameterBlock::FindParameterBlock(char *name)
{
    char lower[0x400];

    if (m_binary) {
        if (!m_blockNames)
            return -1;
        strcpy(lower, name);
        for (char *p = lower; *p; p++)
            *p = tolower(*p);
        int lo = 0;
        int hi = m_numBlocks - 1;
        int mid = hi >> 1;
        do {
            int cmp = strcmp(lower, m_blockNames + (long)mid * 0x400);
            if (cmp == 0)
                return mid;
            if (cmp < 0)
                hi = mid;
            else
                lo = mid;
            mid = lo + ((hi - lo) >> 1);
        } while (hi - lo >= 2);
        if (!strcmp(lower, m_blockNames + (long)lo * 0x400))
            return lo;
        if (strcmp(lower, m_blockNames + (long)hi * 0x400) == 0)
            return hi;
        return -1;
    }
    for (int i = 0; i < m_numBlocks; i++) {
        if (!strcasecmp(name, m_blocks[i].m_name))
            return i;
    }
    return -1;
}

// ---- src/ParameterBlock/DeleteParameterStrings.cpp

void ParameterBlock::DeleteParameterStrings()
{
    if (m_strings) {
        if (m_strings[0])
            free(m_strings[0]);
        free(m_strings);
    }
    m_numStrings = 0;
    m_strings = 0;
}

// ---- src/ParameterBlock/AllocParameterStrings.cpp

// One malloc'd block of 1024-byte strings.
void ParameterBlock::AllocParameterStrings(int count)
{
    m_numStrings = count;
    m_strings = (char **)malloc(m_numStrings * sizeof(char *));
    m_strings[0] = (char *)malloc(m_numStrings * 1024);
    for (int i = 1; i < m_numStrings; i++)
        m_strings[i] = m_strings[i - 1] + 1024;
}

// Retail creates STRCPY's message (@741) here, before ReadParameterBlock's
// own ASSERT message (@825): a function with a STRCPY came here that nothing
// called, which retail's linker stripped ("Section
// order"). link_c leaves its sections out.
static void StrippedStrcpy(char *name)
{
    char block[0x40];
    STRCPY(block, name);
}

// ---- src/ParameterBlock/ReadParameterBlock.cpp
// ReadParameterBlock__14ParameterBlockFPc  @ 0x00261080
// Block-form ASSERT/STRCPY (no alignlabel pragma): retail pads only the loop labels.

// Makes the named block current: loads its data (binary file) or its
// non-empty, non-comment lines (text file).
int ParameterBlock::ReadParameterBlock(char *name)
{
    char line[0x400];
    char stripped[0x400];

    if (!m_open) {
#line 582 "Parameterblocks.cpp"
        ASSERT(0, "ParameterBlock Error : Attempting to read a parameterblock when file is not open for input");
        return 0;
    }
    if (m_writable)
        LpDataAccess->FFlush(m_file);
    int index = FindParameterBlock(name);
    if (index == -1)
        return 0;
    if (m_binary) {
#line 605 "Parameterblocks.cpp"
        STRCPY(m_currentBlock, name);
        unsigned int offset = m_binaryBlocks[index].m_offset;
        unsigned int size = m_binaryBlocks[index].m_size;
        if (m_binaryData) {
            if (m_binaryDataSize < size) {
                m_binaryDataSize = size;
                m_binaryData = (char *)realloc(m_binaryData, m_binaryDataSize);
            }
        } else {
            m_binaryDataSize = size;
            m_binaryData = (char *)malloc(m_binaryDataSize);
        }
        LpDataAccess->FSeek(m_file, offset, 0);
        int count = 0;
        LpDataAccess->FRead(m_file, &count, 4, 1);
        m_numStrings = count;
        LpDataAccess->FRead(m_file, m_binaryData, m_binaryDataSize, 1);
    } else {
        LpDataAccess->FSeek(m_file, m_blocks[index].m_offset, 0);
        if (m_strings)
            DeleteParameterStrings();
        int numLines = m_blocks[index].m_size;
#line 651 "Parameterblocks.cpp"
        STRCPY(m_currentBlock, name);
        for (int i = 0; i < numLines; i++) {
            LpDataAccess->FGets(m_file, line, 0x400);
            RemoveWhiteSpace(line, stripped);
            if (stripped[0] && stripped[0] != ';')
                m_numStrings++;
        }
        LpDataAccess->FSeek(m_file, m_blocks[index].m_offset, 0);
        AllocParameterStrings(m_numStrings);
        int n = 0;
        for (int i = 0; i < numLines; i++) {
            LpDataAccess->FGets(m_file, line, 0x400);
            RemoveWhiteSpace(line, stripped);
            if (stripped[0] && stripped[0] != ';') {
                strcpy(m_strings[n], stripped);
                n++;
            }
        }
        m_inDataBlock = 0;
    }
    return 1;
}

// ---- src/ParameterBlock/GetParameter_00261550.cpp

#define RECORD_SIZE 43

// Copies the value of a binary record into value, up to size - 1 chars (size -1: no limit).
#define COPY_RECORD_VALUE(rec)                                          \
    {                                                                   \
        int offset = 0;                                                 \
        memcpy(&offset, (rec) + 0x23, 4);                               \
        char *s = m_binaryData + offset;                                   \
        unsigned long len = strlen(s);                                  \
        if (size != -1)                                                 \
            len = ((int)len < size - 1) ? (int)len : size - 1;          \
        memcpy(value, s, (int)len);                                     \
        value[(int)len] = 0;                                            \
        return 1;                                                       \
    }

// Looks name up: a binary search of the sorted records of a binary file, else a
// scan of the text lines ("name = value", quotes stripped).
int ParameterBlock::GetParameter(char *name, char *value, int size)
{
    if (m_binary) {
        if (m_numStrings == 1) {
            char lower[0x400];
            strcpy(lower, name);
            char *p;
            for (p = lower; *p; p++)
                *p = tolower(*p);
            char *rec = m_binaryData;
            if (!strcmp(lower, rec))
                COPY_RECORD_VALUE(rec);
            return 0;
        }
        char lower[0x400];
        strcpy(lower, name);
        char *p;
        for (p = lower; *p; p++)
            *p = tolower(*p);
        int lo = 0;
        int hi = m_numStrings - 1;
        int mid = lo + ((hi - lo) >> 1);
        do {
            char *rec = m_binaryData + (long)mid * RECORD_SIZE;
            int cmp = strcmp(lower, rec);
            if (cmp == 0)
                COPY_RECORD_VALUE(rec);
            if (cmp < 0)
                hi = mid;
            else
                lo = mid;
            mid = lo + ((hi - lo) >> 1);
        } while (hi - lo >= 2);
        char *rec = m_binaryData + (long)lo * RECORD_SIZE;
        if (!strcmp(lower, rec))
            COPY_RECORD_VALUE(rec);
        rec = m_binaryData + (long)hi * RECORD_SIZE;
        if (!strcmp(lower, rec))
            COPY_RECORD_VALUE(rec);
        return 0;
    }
    int count = m_numStrings;
    char **lines = m_strings;
    for (int i = 0; i < count; i++) {
        char line[0x400];
#line 873 "Parameterblocks.cpp"
        STRCPY(line, lines[i]);
        TokenString tokens(line);
        char *token = tokens.NextToken("=\n");
        char key[0x400];
#line 877 "Parameterblocks.cpp"
        STRCPY(key, token);
        token = tokens.NextToken("\n");
        if (token && !strcasecmp(key, name)) {
            token = (*token == '"') ? token + 1 : token;
            int last = strlen(token) - 1;
            if (last >= 0 && token[last] == '"')
                token[last] = 0;
            if (size != -1) {
                strncpy(value, token, size);
                int len = strlen(token);
                value[(size - 1 < len) ? size - 1 : len] = 0;
            } else {
                strcpy(value, token);
            }
            return 1;
        }
    }
    strcpy(value, "");
    return 0;
}

// ---- src/ParameterBlock/GetParameter.cpp

// Accepts T/F, ON/OFF, Y/N, or a number.
int ParameterBlock::GetParameter(char *name, int *value)
{
    char buffer[0x40C];
    char *end;
    if (GetParameter(name, buffer, -1)) {
        if (!strncasecmp(buffer, "T", 1)) {
            *value = 1;
            return 1;
        }
        if (!strncasecmp(buffer, "F", 1)) {
            *value = 0;
            return 1;
        }
        if (!strcasecmp(buffer, "ON")) {
            *value = 1;
            return 1;
        }
        if (!strcasecmp(buffer, "OFF")) {
            *value = 0;
            return 1;
        }
        if (!strncasecmp(buffer, "Y", 1)) {
            *value = 1;
            return 1;
        }
        if (!strncasecmp(buffer, "N", 1)) {
            *value = 0;
            return 1;
        }
        *value = strtol(buffer, &end, 0);
        return 1;
    }
    return 0;
}

// ---- src/ParameterBlock/GetParameter_00261D60.cpp
int ParameterBlock::GetParameter(char *name, float *out)
{
    char buf[0x400];
    if (GetParameter(name, buf, -1)) {
        *out = atof(buf);
        return 1;
    }
    return 0;
}

// ---- src/ParameterBlock/GetParameter_00261DC0.cpp
int ParameterBlock::GetParameter(char *name, char *defaultValue, char *value, int size)
{
    if (GetParameter(name, value, size))
        return 1;
    strcpy(value, defaultValue);
    return 0;
}

// ---- src/ParameterBlock/GetParameter_00261E20.cpp
int ParameterBlock::GetParameter(char *name, int defaultValue, int *value)
{
    if (GetParameter(name, value))
        return 1;
    *value = defaultValue;
    return 0;
}

// ---- src/ParameterBlock/GetParameter_00261E70.cpp
int ParameterBlock::GetParameter(char *name, float defaultValue, float *value)
{
    if (GetParameter(name, value))
        return 1;
    *value = defaultValue;
    return 0;
}

// ---- src/ParameterBlock/GetParameter_00261EC0.cpp

int ParameterBlock::GetParameter(char *name, _D3DVECTOR *defaultValue, _D3DVECTOR *value)
{
    char buffer[0x184];     // retail passes 0x80 as its size
    if (GetParameter(name, "", buffer, 0x80)) {
        value->x = atof(strtok(buffer, ","));
        value->y = atof(strtok(0, ","));
        value->z = atof(strtok(0, "\n"));
        return 1;
    }
    value->x = defaultValue->x;
    value->y = defaultValue->y;
    value->z = defaultValue->z;
    return 0;
}

// ---- src/ParameterBlock/GetParameter_00261FA0.cpp

// Reads "v0,v1,...,vN-1" into values[].
int ParameterBlock::GetParameter(char *name, float *values, int count)
{
    char buffer[0x184];     // retail passes 0x80 as its size
    if (GetParameter(name, "", buffer, 0x80)) {
        if (count > 1) {
            values[0] = atof(strtok(buffer, ","));
            for (int i = 1; i < count - 1; i++)
                values[i] = atof(strtok(0, ","));
            values[count - 1] = atof(strtok(0, "\n"));
        } else {
            values[0] = atof(strtok(buffer, "\n"));
        }
        return 1;
    }
    return 0;
}

// ---- src/ParameterBlock/GetParameter_002620E0.cpp

// The text is "left,right,top,bottom".
int ParameterBlock::GetParameter(char *name, _RECT *defaultValue, _RECT *value)
{
    char buffer[0x184];     // retail passes 0x80 as its size
    if (GetParameter(name, "", buffer, 0x80)) {
        value->left = atoi(strtok(buffer, ","));
        value->right = atoi(strtok(0, ","));
        value->top = atoi(strtok(0, ","));
        value->bottom = atoi(strtok(0, "\n"));
        return 1;
    }
    value->left = defaultValue->left;
    value->right = defaultValue->right;
    value->top = defaultValue->top;
    value->bottom = defaultValue->bottom;
    return 0;
}

// ---- src/ParameterBlock/GotoDataBlock.cpp

int ParameterBlock::GotoDataBlock(char *name)
{
    int index = FindParameterBlock(name);
    if (FindParameterBlock(name) != -1) {   // retail searches twice
        if (m_binary) {
            LpDataAccess->FSeek(m_file, m_binaryBlocks[index].m_offset, 0);
            int size = 0;
            LpDataAccess->FRead(m_file, &size, 4, 1);
            m_inDataBlock = 1;
            m_dataRead = 0;
            m_dataSize = size;
            return m_dataSize;
        } else {
            LpDataAccess->FSeek(m_file, m_blocks[index].m_offset, 0);
            m_inDataBlock = 1;
            m_dataRead = 0;
            m_dataSize = m_blocks[index].m_size;
            return m_dataSize;
        }
    }
    return 0;
}

// ---- src/ParameterBlock/ReadDataLine.cpp

// Read and parse the next comma-separated line of the current data block.
int ParameterBlock::ReadDataLine(char *rawLine)
{
    char line[0x400];
    char clean[0x408];

    if (!m_open) {
        PSXprintf("ParameterBlock Error : Attempting to ReadDataLine when file is not open for input\n");
        return 0;
    }
    if (!m_inDataBlock) {
        PSXprintf("ParameterBlock Error : Trying to ReadDataLine while not in DataBlock\n");
        return 0;
    }
    if (m_dataRead > m_dataSize - 1) {
        PSXprintf("ParameterBlock Error : Attempting to read past the end of a DataBlock\n");
        return 0;
    }
    for (int i = 0; i < m_numItems; i++) {
        if (m_items[i].m_type == 2)
            free(m_items[i].m_value.s);
        m_items[i].m_value.s = 0;
    }
    LpDataAccess->FGets(m_file, line, 0x400);
    if (rawLine)
        strcpy(rawLine, line);
    RemoveWhiteSpace(line, clean);
    TokenString tokens(clean);
    char *token = tokens.NextToken(",\n");
    m_numItems = 0;
    while (token) {
        StringToDataItem(token, &m_items[m_numItems]);
        token = tokens.NextToken(",\n");
        m_numItems++;
    }
    m_dataRead++;
    return m_numItems;
}

// ---- src/ParameterBlock/GetData.cpp

int ParameterBlock::GetData(int index, float *out)
{
    if (index > m_numItems)  // sic: retail lets index == m_numItems through
        return 0;
    ConvertDataItem(m_items[index], out);
    return 1;
}

// ---- src/ParameterBlock/GetData_002624E0.cpp

int ParameterBlock::GetData(int index, char *out)
{
    if (index >= m_numItems)
        return 0;
    ConvertDataItem(m_items[index], out);
    return 1;
}

// ---- src/ParameterBlock/RemoveWhiteSpace.cpp

// Copies src to dst, dropping whitespace outside quotes and [brackets].
void ParameterBlock::RemoveWhiteSpace(const char *src, char *dst)
{
    int inQuote = 0;
    int depth = 0;
    int len = strlen(src);
    dst[0] = 0;
    int j = 0;
    if (len) {
        for (int i = 0; i < len; i++) {
            int keep = 1;
            char c = src[i];
            switch (c) {
            case '[':
                depth++;
                break;
            case '"':
                inQuote = 1 - inQuote;
                break;
            case ']':
                depth--;
                break;
            case ' ':
            case '\t':
            case '\n':
            case '\r':
                if (depth <= 0 && !inQuote)
                    keep = 0;
                break;
            }
            if (keep)
                dst[j++] = c;
        }
    }
    dst[j] = 0;
}
