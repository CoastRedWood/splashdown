// IpuCache: where IPU-decompressed textures live (0x1822 bytes of data, 0x1824
// in all): 256 cache handles of 0x18 bytes, then a block of cache memory
// split over IpuCacheTrees.
#ifndef IPUCACHE_H
#define IPUCACHE_H

class PSXTextureMap;
class IpuCacheTree;
struct PSXMipSurface;

struct IpuCacheHandle {
    IpuCacheHandle();

    unsigned char *m_data;              // +0x00, in the cache memory
    IpuCacheTree *m_tree;               // +0x04
    PSXMipSurface *m_mip;               // +0x08
    PSXTextureMap *m_texture;           // +0x0C
    int m_x10;                          // +0x10
    unsigned char m_x14_bit0 : 1;       // +0x14 bit 0
    unsigned char m_used : 1;           // +0x14 bit 1
    unsigned char m_decompress : 1;     // +0x14 bit 2
    unsigned char m_x14_bits3 : 4;      // +0x14 bits 3-6
};

class IpuCache {
public:
    IpuCache(int numTrees);
    ~IpuCache();
    void AllocateCacheHandle(int slot);
    int CheckForTextureInCache(IpuCacheHandle *&handle, PSXTextureMap *texture);
    IpuCacheHandle *CheckMipSurfaceForDecompression(PSXMipSurface *surface);
    IpuCacheHandle *CheckTextureForDecompression(PSXTextureMap *texture);
    void ClearUnusedTexturesFromCache(int all);
    int GetAvailableIpuCacheHandle(IpuCacheHandle *&handle, int width);
    int GetAvailableSpaceFromCache(int size);
    int GetOccupiedSpaceFromCache(int size);
    int PlaceDecompressedTextureIntoCache(IpuCacheHandle *handle);
    void PrepareCacheForNewFrame(int frame);
    void RemoveCacheHandle(IpuCacheHandle *handle);
    void RemoveTextureFromCache(PSXTextureMap *texture);

    IpuCacheHandle m_handles[256];      // +0x0000
    void *m_cacheMem;                   // +0x1800, malloc'd
    IpuCacheTree *m_trees;              // +0x1804
    int m_numTrees;                     // +0x1808
    int m_lastHandle;                   // +0x180C, highest handle index in use
    int m_cacheSize;                    // +0x1810
    void *m_entries;                    // +0x1814, malloc'd (LpCacheMem)
    unsigned short m_used[5];           // +0x1818, blocks of 0x100, 0x80, 0x40, 0x20, 0x10 in use
    unsigned char m_outOfSpace;         // +0x1822
};

#endif
