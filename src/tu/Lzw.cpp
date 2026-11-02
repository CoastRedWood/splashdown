// Lzw.cpp  @ 0x00259B50 (text 0x259B50-0x25A2B0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp)
//
// The LZW coder (after Mark Nelson's LZW15V.C) as one translation unit, a
// TU without an initializer inside LightEmitter.cpp's tu_map row. The name
// is a guess (LightEmitter.cpp < it < Math3d.cpp keeps the alphabetical
// link order); it has no .rodata, and its .bss (decode_stack ..
// next_bump_code, 0x46B080-0x473964) follows LightEmitter.cpp's with no
// axis vectors between, so it could also be LightEmitter.cpp's tail. `make
// link` places its data as well as its code.


#include "GR_BitString/GR_BitString.h"

extern "C" void *memcpy(void *dst, const void *src, unsigned int n);

#define BITS 15
#define TABLE_SIZE 35023L
#define UNUSED -1
#define FIRST_CODE 259
#define END_OF_STREAM 256
#define BUMP_CODE 257
#define FLUSH_CODE 258
#define MAX_CODE 0x7FFF
#define DICT(i) dict[(i) >> 8][(i) & 0xff]

struct dictionary {
    int code_value;
    int parent_code;
    char character;
};

// The coder's state (.bss, in retail's order).
char decode_stack[TABLE_SIZE];
unsigned int next_code;
int current_code_bits;
unsigned int next_bump_code;

void InitializeDictionary(dictionary (*dict)[256]);
unsigned int find_child_node(int parent_code, int child_character, dictionary (*dict)[256]);

// Writes a code unless that would overflow the output buffer.
#define OUTPUT_CODE(code)                           \
    do {                                            \
        bitsUsed += current_code_bits;              \
        if (bitsUsed > maxBits)                     \
            goto overflow;                          \
        bits.PutBits(code, current_code_bits);      \
    } while (0)

// ---- src/global/InitializeDictionary.cpp



void InitializeDictionary(dictionary (*dict)[256])
{
    unsigned int i;

    for (i = 0; i < TABLE_SIZE; i++)
        DICT(i).code_value = UNUSED;
    next_code = FIRST_CODE;
    current_code_bits = 9;
    next_bump_code = 511;
}


// ---- src/global/find_child_node.cpp


unsigned int find_child_node(int parent_code, int child_character, dictionary (*dict)[256])
{
    // long (64-bit) where LZW15V.C has unsigned int / int.
    long table_index;
    long offset;

    table_index = (child_character << (BITS - 8)) ^ parent_code;
    if (table_index == 0)
        offset = 1;
    else
        offset = TABLE_SIZE - table_index;
    for (;;) {
        if (DICT(table_index).code_value == UNUSED)
            return (unsigned int)table_index;
        if (DICT(table_index).parent_code == parent_code &&
            DICT(table_index).character == (char)child_character)
            return table_index;
        if (table_index >= offset)
            table_index -= offset;
        else
            table_index += TABLE_SIZE - offset;
    }
}


// ---- src/global/LZWCompress.cpp






// Compresses size bytes; the output starts with its own length in bytes.
// If it wouldn't fit in size bytes, the input is copied through unchanged.
int LZWCompress(void *input, void *output, int size)
{
    dictionary dict[(TABLE_SIZE + 255) / 256][256];
    unsigned int maxBits;
    unsigned char *in;
    int i;
    unsigned int bitsUsed;
    int character;
    int string_code;
    unsigned int index;
    maxBits = size * 8;
    GR_BitString bits((unsigned int *)output, maxBits, 0);

    bits.PutBits(0, 32);    // room for the compressed length
    bitsUsed = 32;
    InitializeDictionary(dict);
    string_code = *(unsigned char *)input;
    in = (unsigned char *)input + 1;
    for (i = 0; i < size; i++) {
        character = *in++;
        index = find_child_node(string_code, character, dict);
        if (DICT(index).code_value != UNUSED) {
            string_code = DICT(index).code_value;
        } else {
            DICT(index).code_value = next_code++;
            DICT(index).parent_code = string_code;
            DICT(index).character = (char)character;
            OUTPUT_CODE(string_code);
            string_code = character;
            if (next_code > MAX_CODE) {
                OUTPUT_CODE(FLUSH_CODE);
                InitializeDictionary(dict);
            } else if (next_code > next_bump_code) {
                OUTPUT_CODE(BUMP_CODE);
                current_code_bits++;
                next_bump_code <<= 1;
                next_bump_code |= 1;
            }
        }
    }
    OUTPUT_CODE(string_code);
    OUTPUT_CODE(END_OF_STREAM);
    bits.FlushBits();
    *(unsigned int *)output = (bitsUsed + 7) / 8;
    return (bitsUsed + 7) / 8;

overflow:
    memcpy(output, input, size);
    return size;
}


// ---- src/global/LZWDecompress.cpp





static inline unsigned int decode_string(dictionary (*dict)[256], unsigned int count, unsigned int code)
{
    while (code > 255) {
        decode_stack[count++] = DICT(code).character;
        code = DICT(code).parent_code;
    }
    decode_stack[count++] = (char)code;
    return count;
}

void LZWDecompress(void *output, void *input, int size)
{
    dictionary dict[(TABLE_SIZE + 255) / 256][256];
    unsigned char *out = (unsigned char *)output;
    GR_BitString bits((unsigned int *)input, size * 8, 0);
    unsigned int new_code;
    unsigned int old_code;
    int character;
    unsigned int count;

    bits.GetBits(32);   // (uncompressed size, unused here)
    for (;;) {
        InitializeDictionary(dict);
        old_code = bits.GetBits(current_code_bits);
        if (old_code == END_OF_STREAM)
            return;
        character = old_code;
        *out++ = old_code;
        for (;;) {
            new_code = bits.GetBits(current_code_bits);
            if (new_code == END_OF_STREAM)
                return;
            if (new_code == FLUSH_CODE)
                break;
            if (new_code == BUMP_CODE) {
                current_code_bits++;
                continue;
            }
            if (new_code >= next_code) {
                decode_stack[0] = (char)character;
                count = decode_string(dict, 1, old_code);
            } else {
                count = decode_string(dict, 0, new_code);
            }
            character = decode_stack[count - 1];
            while (count > 0)
                *out++ = decode_stack[--count];
            DICT(next_code).parent_code = old_code;
            DICT(next_code).character = (char)character;
            next_code++;
            old_code = new_code;
        }
    }
}

