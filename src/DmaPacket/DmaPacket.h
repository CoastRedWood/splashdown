// DmaPacket — builds raw DMA/GIF chains for the PS2 DMA controller.
#ifndef DMAPACKET_H
#define DMAPACKET_H

class DmaPacket {
public:
    DmaPacket(int size, int uncached);
    ~DmaPacket();
    void ResetPacket();
    unsigned int *BuildRawGifData(unsigned int a, unsigned long data);
    void BuildRawGifData(unsigned int *dst, unsigned int a, unsigned long data);
    // Returns BuildRawGifData's start pointer (left in $v0; InitializeTextureDmaPacket
    // keeps it as the BITBLTBUF slot).
    unsigned int *AddGifData(unsigned int a, unsigned long data);
    void AddGifData(unsigned int *dst, unsigned int a, unsigned long data);
    void BuildRawGifTag(unsigned long regs, unsigned int nreg, unsigned int flg,
                        unsigned int prim, unsigned int pre, unsigned int eop,
                        unsigned int nloop);
    void BuildRawDmaTag(unsigned int spr, void *addr, unsigned int irq,
                        unsigned int id, unsigned int pce, unsigned int qwc);
    void AddGifTag(int eop);
    void AddGifTextureTag(unsigned int nloop);
    void SendGifPacket(int wait, int waitForQueue);
    void SendIpuPacket(int wait);
    void AddDmaEndTag();
    void AddSourceDmaTag();
    void AddSourceDmaIpuTag(unsigned char *data, unsigned int qwc);
    void AddSourceDmaTextureTag(unsigned char *data, unsigned int qwc);

    // Ends the open DMA tag: ORs the accumulated QWC into its first word.
    void CloseDmaTag()
    {
        if (m_openDmaTag) {
            *m_openDmaTag |= m_numTags;
            m_numTags = 0;
            m_openDmaTag = 0;
        }
    }

    // Ends the open GIF tag: ORs the accumulated NLOOP count into its first word.
    void CloseGifTag()
    {
        if (m_openTag) {
            *m_openTag |= m_openTagCount;
            m_openTagCount = 0;
            m_openTag = 0;
        }
    }

    unsigned int *m_start;  // +0x00, first word of the packet
    unsigned int *m_cur;     // +0x04, write cursor
    int m_maxQwords;         // +0x08
    unsigned int *m_openDmaTag; // +0x0C, DMA tag whose QWC is still being counted
    unsigned int *m_openTag; // +0x10, GIF tag whose NLOOP is still being counted
    int m_numTags;           // +0x14, also the open DMA tag's QWC
    int m_openTagCount;      // +0x18

    int m_numQwords;         // +0x1C
};

#endif
