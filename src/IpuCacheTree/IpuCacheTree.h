// IpuCacheTree: a quadtree splitting 0x20000 bytes of IPU decompression
// cache into blocks of 0x100..0x10 (0x24 bytes).
#ifndef IPUCACHETREE_H
#define IPUCACHETREE_H

// One block; its children are the four quarters (0x18 bytes).
struct _CacheTreeNode {
    unsigned char *m_data;              // +0x00 (the root's is malloc'd)
    unsigned short m_size;              // +0x04
    unsigned char m_free : 1;           // +0x06 bit 0
    unsigned char m_used : 4;           //        bits 1-4, one per child
    _CacheTreeNode *m_children[4];      // +0x08
};

class IpuCacheTree {
public:
    IpuCacheTree();
    ~IpuCacheTree();
    void Create();
    void Release();
    void BuildCacheTree(int size, _CacheTreeNode *node, unsigned char *data, int depth);
    unsigned char *RetrieveCacheAddress(_CacheTreeNode *node, int size, unsigned short *freeCounts,
                                        unsigned char level);
    void RemoveFromCacheTree(_CacheTreeNode *node, unsigned char *data, int size,
                             unsigned short *freeCounts, unsigned char level);

    _CacheTreeNode m_root;              // +0x00
    unsigned short m_freeCounts[5];     // +0x18 (free blocks of 0x100, 0x80, 0x40, 0x20, 0x10; ctor 1, 4, 0x10, 0x40, 0x100)
    char pad22[0x24 - 0x22];
};

#endif
