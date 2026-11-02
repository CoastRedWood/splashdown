// IpuTextureTransfer: the DMA packet that feeds compressed texture data to
// the IPU (4 bytes; the ctor news a DmaPacket(0x40, 1)).
#ifndef IPUTEXTURETRANSFER_H
#define IPUTEXTURETRANSFER_H

class DmaPacket;

class IpuTextureTransfer {
public:
    IpuTextureTransfer();
    ~IpuTextureTransfer();
    void TransferTexture(unsigned char *data, int size);
    void SendTexture(int wait);

    DmaPacket *m_packet;    // +0x00
};

#endif
