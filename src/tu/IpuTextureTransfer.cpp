// IpuTextureTransfer.cpp  @ 0x002C5480 (text 0x2C5480-0x2C55D0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// IpuTextureTransfer as one translation unit without a static initializer.
// PSXRenderStateBlock, GsTextureTransfer, IpuTextureTransfer and DmaPacket sit between
// DataAccess.cpp and JetskiGameOptions.cpp; their .rodata (@504-@818) has no label
// restart, but PSXRenderStateBlock's 16-byte constant 0xA (@507) and DmaPacket's
// 8-byte 0xA (@818) are separate copies, which one mwcc TU pools into one. So they are
// more than one TU; one per class is a guess, and so are the file names.




#include "PSXRenderStateBlock/PSXRenderStateBlock.h"
#include "sceGifTag/sceGifTag.h"
#include "PSXVideoCard/PSXVideoCard.h"
#include "DmaRenderItem/DmaRenderItem.h"
#include "GsTextureTransfer/GsTextureTransfer.h"
#include "DmaPacket/DmaPacket.h"
#include "_RECT/_RECT.h"
#include "Camera/Camera.h"
#include "IpuTextureTransfer/IpuTextureTransfer.h"

#include "PSXTextureMap/PSXTextureMap.h"
#include "PSXRenderTarget/PSXRenderTarget.h"
#include "GsTextureTransfer/GsTextureTransfer.h"
#include "DmaPacket/DmaPacket.h"
#include "soultree_assert_block.h"

// A texture transfer packet as GsTextureTransfer builds it (DmaPacket data).
struct TransferPacket {
    char pad0[0x20];
    long m_bitbltbuf;               // +0x20
    char pad28[0x30 - 0x28];
    long m_trxreg;                  // +0x30
    char pad38[0x70 - 0x38];
    unsigned short m_nloop : 15;    // +0x70 (image GIFtag)
    unsigned short m_eop : 1;
    char pad72[0x80 - 0x72];
    unsigned short m_qwc;           // +0x80 (DMA tag)
    short pad82;
    unsigned int m_addr;            // +0x84
    char pad88[0xB0 - 0x88];
    long m_clutBitbltbuf;           // +0xB0
    char padB8[0x104 - 0xB8];
    unsigned int m_clutAddr;        // +0x104
};

// ---- src/IpuTextureTransfer/ctor.cpp

IpuTextureTransfer::IpuTextureTransfer()
{
    m_packet = new DmaPacket(0x40, 1);
}


// ---- src/IpuTextureTransfer/dtor.cpp


IpuTextureTransfer::~IpuTextureTransfer()
{
    if (m_packet)
        delete m_packet;
}


// ---- src/IpuTextureTransfer/TransferTexture.cpp


void IpuTextureTransfer::TransferTexture(unsigned char *data, int size)
{
    m_packet->ResetPacket();
    size = (size < 0x40000) ? size : 0x40000;
    m_packet->AddSourceDmaIpuTag(data, (size + 16 - size % 16) >> 4);
    m_packet->AddDmaEndTag();
}


// ---- src/IpuTextureTransfer/SendTexture.cpp


void IpuTextureTransfer::SendTexture(int wait)
{
    m_packet->SendIpuPacket(wait);
}


