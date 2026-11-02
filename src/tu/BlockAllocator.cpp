// BlockAllocator.cpp  @ 0x00204440 (text 0x204440-0x204800)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// A retail translation unit without a static initializer (the name is a guess):
// BlockAllocator. Split from BoundingBoxTree.cpp: its .rodata is the
// BlockAllocator RTTI (@421-@423, after BaseObject.cpp's), which mwcc writes
// only at a TU's end, and BoundingBoxTree.cpp's literals follow.
#include "BlockAllocator/BlockAllocator.h"


// ---- src/BlockAllocator/ctor.cpp

BlockAllocator::BlockAllocator(int blockSize, int bigBlockSize)
{
    m_blocksPerBigBlock = (bigBlockSize - blockSize) / blockSize;
    m_firstBigBlock = 0;
    m_currentBigBlock = 0;
    m_usedInBigBlock = m_blocksPerBigBlock;
    m_blockSize = blockSize;
    m_bigBlockSize = bigBlockSize;
    m_freeList = 0;
    m_bytesAllocated = 0;
    m_spareBigBlocks = 0;
}


// ---- src/BlockAllocator/dtor.cpp

BlockAllocator::~BlockAllocator()
{
    FreeAll();
}


// ---- src/BlockAllocator/AllocBlock.cpp
// Big blocks are chained through their first word; the first block-sized
// slot of each big block is that header.

void *BlockAllocator::AllocBlock()
{
    void *block;
    if (m_usedInBigBlock < m_blocksPerBigBlock) {
        block = (char *)m_currentBigBlock + m_blockSize + m_usedInBigBlock * m_blockSize;
        m_usedInBigBlock++;
    } else if (m_freeList) {
        block = m_freeList;
        m_freeList = (void **)*m_freeList;
    } else if (m_spareBigBlocks) {
        if (!m_firstBigBlock) {
            m_firstBigBlock = m_currentBigBlock = m_spareBigBlocks;
        } else {
            m_currentBigBlock->m_next = m_spareBigBlocks;
            m_currentBigBlock = m_spareBigBlocks;
        }
        m_spareBigBlocks = m_spareBigBlocks->m_next;
        m_currentBigBlock->m_next = 0;
        block = (char *)m_currentBigBlock + m_blockSize;
        m_usedInBigBlock = 1;
    } else if (!AllocBigBlock()) {
        block = 0;
    } else {
        block = (char *)m_currentBigBlock + m_blockSize;
        m_usedInBigBlock = 1;
    }
    if (block)
        m_bytesUsed += m_blockSize;
    return block;
}


// ---- src/BlockAllocator/FreeBlock.cpp

void BlockAllocator::FreeBlock(void *block)
{
    if ((unsigned int)m_blockSize > 4) {
        *(void **)block = m_freeList;
        m_freeList = (void **)block;
        m_bytesUsed -= m_blockSize;
    }
}


// ---- src/BlockAllocator/AllocBigBlock.cpp
extern "C" void *malloc(unsigned int size);


int BlockAllocator::AllocBigBlock()
{
    BigBlock *block = (BigBlock *)malloc(m_bigBlockSize);
    if (!block)
        return 0;
    if (!m_firstBigBlock) {
        m_firstBigBlock = m_currentBigBlock = block;
    } else {
        m_currentBigBlock->m_next = block;
        m_currentBigBlock = block;
    }
    m_currentBigBlock->m_next = 0;
    m_bytesAllocated += m_bigBlockSize;
    return 1;
}


// ---- src/BlockAllocator/FreeAll.cpp
extern "C" void free(void *p);


void BlockAllocator::FreeAll()
{
    BigBlock *block = m_firstBigBlock;
    while (block) {
        BigBlock *dead = block;
        block = block->m_next;
        free(dead);
    }
    block = m_spareBigBlocks;
    while (block) {
        BigBlock *dead = block;
        block = block->m_next;
        free(dead);
    }
    m_firstBigBlock = 0;
    m_currentBigBlock = 0;
    m_usedInBigBlock = m_blocksPerBigBlock;
    m_freeList = 0;
    m_bytesAllocated = 0;
    m_bytesUsed = 0;
    m_spareBigBlocks = 0;
}


// ---- src/BlockAllocator/Purge.cpp

void BlockAllocator::Purge()
{
    if (m_firstBigBlock) {
        if (m_spareBigBlocks && m_currentBigBlock)
            m_currentBigBlock->m_next = m_spareBigBlocks;
        m_spareBigBlocks = m_firstBigBlock;
    }
    m_firstBigBlock = 0;
    m_currentBigBlock = 0;
    m_bytesUsed = 0;
    m_usedInBigBlock = m_blocksPerBigBlock;
    m_freeList = 0;
}
