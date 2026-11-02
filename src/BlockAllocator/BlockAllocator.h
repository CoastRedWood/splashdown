// BlockAllocator: hands out fixed-size blocks carved from malloc'ed "big
// blocks", with a free list. A BaseObject, 0x38 bytes. The ctor sets
// +0x10 = (bigBlockSize - blockSize) / blockSize and copies it to +0x20.
#ifndef BLOCKALLOCATOR_BLOCKALLOCATOR_H
#define BLOCKALLOCATOR_BLOCKALLOCATOR_H

#include "GameObject/GameObject.h"

// Big blocks are chained through their first word; the first block-sized
// slot of each big block is that header.
struct BigBlock {
    BigBlock *m_next;
};

class BlockAllocator : public BaseObject {
public:
    BlockAllocator(int blockSize, int bigBlockSize);
    void *AllocBlock();
    int AllocBigBlock();
    void FreeBlock(void *block);
    void FreeAll();
    void Purge();

    virtual ~BlockAllocator();

    int m_blocksPerBigBlock;            // +0x10
    BigBlock *m_firstBigBlock;          // +0x14
    BigBlock *m_currentBigBlock;        // +0x18
    BigBlock *m_spareBigBlocks;         // +0x1C
    int m_usedInBigBlock;               // +0x20, blocks left in the current big block
    int m_blockSize;                    // +0x24
    int m_bigBlockSize;                 // +0x28
    void **m_freeList;                  // +0x2C
    int m_bytesAllocated;               // +0x30
    int m_bytesUsed;                    // +0x34
};

#endif
