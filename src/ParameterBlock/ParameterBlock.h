// ParameterBlock: reads "[block]" sections of name=value text files (or
// their binary form) and answers GetParameter queries. No vtable, 0x4D0
// bytes (8-aligned by m_file). Names come from the class's own methods.
#ifndef PARAMETERBLOCK_PARAMETERBLOCK_H
#define PARAMETERBLOCK_PARAMETERBLOCK_H

struct _D3DVECTOR;

struct _RECT;
struct ParameterDataBlock;      // one [block] of the random-access table
struct ParameterBinaryBlock;

// One value of a data line (ReadDataLine / GetData).
struct DataItem {
    int m_type;                         // 0 int, 1 float, 2 string
    union {
        int i;
        float f;
        char *s;
    } m_value;
};

class ParameterBlock {
public:
    ParameterBlock();
    ~ParameterBlock();
    int OpenFile(char *name, int writable);
    void CreateRandomAccessTable(long file);
    int FindParameterBlock(char *name);
    int ReadParameterBlock(char *name);
    int GotoDataBlock(char *name);
    int ReadDataLine(char *rawLine);
    void AllocParameterStrings(int count);
    void DeleteParameterStrings();
    void RemoveWhiteSpace(const char *src, char *dst);
    int GetParameter(char *name, int *value);
    int GetParameter(char *name, float *value);
    int GetParameter(char *name, float *values, int count);
    int GetParameter(char *name, char *value, int size);
    int GetParameter(char *name, int defaultValue, int *value);
    int GetParameter(char *name, float defaultValue, float *value);
    int GetParameter(char *name, char *defaultValue, char *value, int size);
    int GetParameter(char *name, _D3DVECTOR *defaultValue, _D3DVECTOR *value);
    int GetParameter(char *name, _RECT *defaultValue, _RECT *value);
    int GetData(int index, float *out);
    int GetData(int index, char *out);

    long m_file;                        // +0x000 (64-bit)
    int m_binary;                       // +0x008
    int m_open;                         // +0x00C
    int m_writable;                     // +0x010
    int m_numBlocks;                    // +0x014
    int m_maxBlocks;                    // +0x018
    ParameterDataBlock *m_blocks;       // +0x01C
    int m_numStrings;                   // +0x020
    char **m_strings;                   // +0x024
    char m_currentBlock[0x400];         // +0x028
    int m_inDataBlock;                  // +0x428
    int m_x42C;                         // +0x42C
    int m_dataSize;                     // +0x430
    int m_dataRead;                     // +0x434
    int m_numItems;                     // +0x438
    DataItem m_items[16];               // +0x43C
    ParameterBinaryBlock *m_binaryBlocks; // +0x4BC
    unsigned int m_binaryBlocksSize;    // +0x4C0
    char *m_binaryData;                 // +0x4C4
    unsigned int m_binaryDataSize;      // +0x4C8
    char *m_blockNames;                 // +0x4CC
};

#endif
