// GsTextureTransfer.cpp  @ 0x002C3E50 (text 0x2C3E50-0x2C5480)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// GsTextureTransfer as one translation unit without a static initializer.
// PSXRenderStateBlock, GsTextureTransfer, IpuTextureTransfer and DmaPacket sit
// between DataAccess.cpp and JetskiGameOptions.cpp; their .rodata (@504-@818) has no
// label restart, but PSXRenderStateBlock's 16-byte constant 0xA (@507) and
// DmaPacket's 8-byte 0xA (@818) are separate copies, which one mwcc TU pools into
// one. So they are more than one TU; one per class is a guess, and so are the file
// names.



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

// ---- src/GsTextureTransfer/ctor.cpp

GsTextureTransfer::GsTextureTransfer()
{
    m_packet = 0;
}


// ---- src/GsTextureTransfer/Create.cpp


GsTextureTransfer *GsTextureTransfer::Create(int size)
{
    m_packet = new DmaPacket(size, 1);
    return this;
}


// ---- src/GsTextureTransfer/dtor.cpp


GsTextureTransfer::~GsTextureTransfer()
{
    if (m_packet)
        delete m_packet;
}


// ---- src/GsTextureTransfer/PrepareTransferTexture.cpp
// Compiled with the global optimizer ON (no asm) but `#pragma opt_propagation off`:
// the named parameter copies (dbp, psm, xpos, ...) survive as their own variables,
// which is what gives retail's spill choice (dbp/dbw/psm spilled, x kept in $fp) and
// frame layout. The loop uses xpos directly; mwcc hoists `(u_long)xpos << 32` out of
// the loop into xpos's register, and `stride` becomes a vreg spilled to its home slot,
// which is why retail reloads it from 0xD8 right before the add.

#pragma opt_propagation off

// Uploads a texture in one IMAGE transfer, or one row at a time when the
// source rows are `stride` bytes apart.
void GsTextureTransfer::PrepareTransferTexture(unsigned char *texture, int dbp_, int width, int height, int x, int y,
                                               int psm_, int bpp, int stride)
{
    int dbp = dbp_;
    u_int dbw = (width + 63) >> 6;
    int psm = psm_;
    int qwc = (bpp * (width * height)) >> 7;
    int xpos = x;
    int ypos = y;
    int w = width;
    m_packet->ResetPacket();
    if (!stride) {
        m_packet->AddSourceDmaTag();
        m_packet->AddGifTag(0);
        m_packet->AddGifData(SCE_GS_BITBLTBUF, ((u_long)(u_int)psm << 56) | SCE_GS_SET_BITBLTBUF(0, 0, 0, (u_int)dbp, dbw, 0));
        m_packet->AddGifData(SCE_GS_TRXREG, SCE_GS_SET_TRXREG((u_int)w, (u_int)height));
        m_packet->AddGifData(SCE_GS_TRXPOS, SCE_GS_SET_TRXPOS(0, 0, (u_int)xpos, (u_int)ypos, 0));
        m_packet->AddGifData(SCE_GS_TRXDIR, SCE_GS_SET_TRXDIR(0));
        m_packet->AddGifData(SCE_GS_TEXFLUSH, 0);
        m_packet->AddGifTextureTag(qwc);
        m_packet->AddSourceDmaTextureTag(texture, qwc);
    } else {
        int row = 0;
        int i = 0;
        int offset;
        u_int rowQwc = (u_int)(bpp * w) >> 7;
        m_packet->AddSourceDmaTag();
        m_packet->AddGifTag(0);
        m_packet->AddGifData(SCE_GS_BITBLTBUF, ((u_long)(u_int)psm << 56) | SCE_GS_SET_BITBLTBUF(0, 0, 0, (u_int)dbp, dbw, 0));
        m_packet->AddGifData(SCE_GS_TRXREG, SCE_GS_SET_TRXREG((u_int)w, 1));
        if (0 < height) {
            offset = 0;
            do {
                m_packet->AddSourceDmaTag();
                m_packet->AddGifTag(0);
                m_packet->AddGifData(SCE_GS_TRXPOS, SCE_GS_SET_TRXPOS(0, 0, (u_int)xpos, (u_int)(ypos + row), 0));
                m_packet->AddGifData(SCE_GS_TRXDIR, SCE_GS_SET_TRXDIR(0));
                m_packet->AddGifTextureTag(rowQwc);
                m_packet->AddSourceDmaTextureTag(texture + offset, rowQwc);
                i++;
                offset += stride;
                row++;
            } while (i < height);
        }
    }
    m_packet->AddDmaEndTag();
}
#pragma opt_propagation reset


// ---- src/GsTextureTransfer/PrepareTransferTextureAndClut.cpp


// Uploads a palettized texture and then its CLUT (16x16 for PSMT8, 8x2 for
// PSMT4).
void GsTextureTransfer::PrepareTransferTextureAndClut(unsigned char *texture, unsigned char *clut, int dbp_, int clutDbp,
                                                      int width, int height, int x, int y, int psm, int clutPsm,
                                                      int bpp, int dbw)
{
    // A dead inline asm, as an asm statement in retail's source must have been:
    // it turns mwcc's global optimizer off without the cross-block reuse of
    // loaded values that `#pragma global_optimizer off` keeps (retail reloads
    // bpp for the multiply).
    if (0) asm("nop");
    // A local copy of dbp: as a named local it is the value mwcc spills
    // (retail keeps x in $fp and stores dbp to its home slot).
    int dbp = dbp_;
    u_int bufWidth = dbw;
    u_int format = psm;
    if (bpp == 32)
        format = 0;
    int qwc = (bpp * (width * height)) >> 7;
    m_packet->ResetPacket();
    m_packet->AddSourceDmaTag();
    m_packet->AddGifTag(0);
    m_packet->AddGifData(SCE_GS_BITBLTBUF, SCE_GS_SET_BITBLTBUF(0, 0, 0, (u_int)dbp, bufWidth, format));
    m_packet->AddGifData(SCE_GS_TRXREG, SCE_GS_SET_TRXREG((u_int)width, (u_int)height));
    m_packet->AddGifData(SCE_GS_TRXPOS, SCE_GS_SET_TRXPOS(0, 0, (u_int)x, (u_int)y, 0));
    m_packet->AddGifData(SCE_GS_TRXDIR, SCE_GS_SET_TRXDIR(0));
    m_packet->AddGifData(SCE_GS_TEXFLUSH, 0);
    m_packet->AddGifTextureTag(qwc);
    m_packet->AddSourceDmaTextureTag(texture, qwc);

    int clutWidth;
    int clutHeight;
    if (psm == 0x13) {          // PSMT8
        clutWidth = clutHeight = 16;
    } else {                    // PSMT4
        clutWidth = 8;
        clutHeight = 2;
    }
    u_int clutQwc = (u_int)(clutWidth * clutHeight * 4) >> 4;
    m_packet->AddSourceDmaTag();
    m_packet->AddGifTag(0);
    // The CLUT format ORed in front of the rest (retail's operand order).
    m_packet->AddGifData(SCE_GS_BITBLTBUF, ((u_long)(u_int)clutPsm << 56) | SCE_GS_SET_BITBLTBUF(0, 0, 0, (u_int)clutDbp, 1, 0));
    m_packet->AddGifData(SCE_GS_TRXREG, SCE_GS_SET_TRXREG((u_int)clutWidth, (u_int)clutHeight));
    m_packet->AddGifData(SCE_GS_TRXPOS, SCE_GS_SET_TRXPOS(0, 0, 0, 0, 0));
    m_packet->AddGifData(SCE_GS_TRXDIR, SCE_GS_SET_TRXDIR(0));
    m_packet->AddGifTextureTag(clutQwc);
    m_packet->AddSourceDmaTextureTag(clut, clutQwc);
    m_packet->AddDmaEndTag();
}


// ---- src/GsTextureTransfer/UpdateTransferTexture_audit.cpp
// UpdateTransferTexture__17GsTextureTransferFPUciiiii — graphics register setup (audit)  @ 0x002C4460
typedef unsigned long u_long;
typedef unsigned int u_int;
#define SCE_GS_SET_BITBLTBUF(sbp, sbw, spsm, dbp, dbw, dpsm) \
    ((u_long)(sbp) | ((u_long)(sbw) << 16) | ((u_long)(spsm) << 24) | \
     ((u_long)(dbp) << 32) | ((u_long)(dbw) << 48) | ((u_long)(dpsm) << 56))
#define SCE_GS_SET_TRXREG(rrw, rrh) ((u_long)(rrw) | ((u_long)(rrh) << 32))




// Rewrites the BITBLTBUF/TRXREG registers, image GIFtag NLOOP and DMA tag of a
// transfer packet built earlier, for a new texture of the same layout.
void GsTextureTransfer::UpdateTransferTexture(unsigned char *texture, int dbp, int width, int height, int psm, int bpp)
{
    // Retail compiled this without the global optimizer (as an ASSERT would).
    if (0) asm("nop");
    TransferPacket *packet = (TransferPacket *)m_packet->m_start;
    int size = width * height;
    packet->m_bitbltbuf = SCE_GS_SET_BITBLTBUF(0, 0, 0, dbp, (u_int)((width + 63) >> 6), psm);
    packet->m_trxreg = SCE_GS_SET_TRXREG(width, height);
    // The quadword count reuses the bpp parameter (retail keeps it in $t2).
    bpp *= size;
    bpp >>= 7;
    packet->m_nloop = bpp;
    packet->m_addr = (u_int)texture & ~0xF;
    packet->m_qwc = bpp;
}


// ---- src/GsTextureTransfer/UpdateTransferTextureAndClut.cpp
// UpdateTransferTextureAndClut__17GsTextureTransferFPUcPUciiiiiiii  @ 0x002C4500

typedef unsigned long u_long;
typedef unsigned int u_int;
#define SCE_GS_SET_BITBLTBUF(sbp, sbw, spsm, dbp, dbw, dpsm) \
    ((u_long)(sbp) | ((u_long)(sbw) << 16) | ((u_long)(spsm) << 24) | \
     ((u_long)(dbp) << 32) | ((u_long)(dbw) << 48) | ((u_long)(dpsm) << 56))
#define SCE_GS_SET_TRXREG(rrw, rrh) ((u_long)(rrw) | ((u_long)(rrh) << 32))




// The image quadword count. Computed through an inline helper: its value then
// gets the last-coloured register ($t4), which a plain local doesn't.
static inline int Qwc(int size, int bpp)
{
    return (bpp * size) >> 7;
}

// Rewrites a texture-plus-CLUT transfer packet built earlier for a new image of
// the same layout.
void GsTextureTransfer::UpdateTransferTextureAndClut(unsigned char *texture, unsigned char *clut, int dbp, int clutDbp,
                                                     int width, int height, int psm, int clutPsm, int bpp, int dbw)
{
    int qwc = Qwc(width * height, bpp);
    u_int bufWidth = dbw;
    u_int format = psm;
    if (bpp == 32)
        format = 0;
    TransferPacket *packet = (TransferPacket *)m_packet->m_start;
    packet->m_bitbltbuf = SCE_GS_SET_BITBLTBUF(0, 0, 0, dbp, bufWidth, format);
    packet->m_trxreg = SCE_GS_SET_TRXREG(width, height);
    packet->m_nloop = qwc;
    packet->m_addr = (u_int)texture & ~0xF;
    packet->m_qwc = qwc;
    packet->m_clutBitbltbuf = ((u_long)clutPsm << 56) | SCE_GS_SET_BITBLTBUF(0, 0, 0, clutDbp, 1, 0);
    packet->m_clutAddr = (u_int)clut & ~0xF;
}


// ---- src/GsTextureTransfer/PrepareTransferOversizedTexture.cpp
// Retail compiled this function without the global optimizer (unrotated loop,
// the clamp's empty arm kept as a branch).
#pragma global_optimizer off


// Uploads a texture too big for one GIF IMAGE transfer, in chunks of at most
// 0x7FC0 quadwords.
void GsTextureTransfer::PrepareTransferOversizedTexture(unsigned char *texture, int dbp, int width, int height,
                                                        int x, int y, int psm, int bpp)
{
    u_int total = (bpp * (width * height)) >> 7;
    u_int done = 0;
    m_packet->ResetPacket();
    m_packet->AddSourceDmaTag();
    m_packet->AddGifTag(0);
    m_packet->AddGifData(SCE_GS_BITBLTBUF, SCE_GS_SET_BITBLTBUF(0, 0, 0, (u_int)dbp, (u_int)(width >> 6), (u_int)psm));
    m_packet->AddGifData(SCE_GS_TRXREG, SCE_GS_SET_TRXREG((u_int)width, (u_int)height));
    m_packet->AddGifData(SCE_GS_TRXPOS, SCE_GS_SET_TRXPOS(0, 0, (u_int)x, (u_int)y, 0));
    m_packet->AddGifData(SCE_GS_TRXDIR, SCE_GS_SET_TRXDIR(0));
    m_packet->AddGifData(SCE_GS_TEXFLUSH, 0);
    while (done < total) {
        u_int qwc = total - done;
        qwc = (qwc > 0x7FC0) ? 0x7FC0 : qwc;
        m_packet->AddSourceDmaTag();
        m_packet->AddGifTextureTag(qwc);
        m_packet->AddSourceDmaTextureTag(texture + done * 16, qwc);
        done += qwc;
    }
    m_packet->AddDmaEndTag();
}
#pragma global_optimizer reset


// ---- src/GsTextureTransfer/PrepareTransferTiledTexture.cpp
// PrepareTransferTiledTexture__17GsTextureTransferFP11sceIpuRGB16iiiiii  @ 0x002C4790



// Uploads IPU output (16-pixel-wide macroblock columns) one column at a time.
void GsTextureTransfer::PrepareTransferTiledTexture(sceIpuRGB16 *rgb, int dbp_, int width, int height, int x, int y, int stride)
{
    // A dead inline asm turns mwcc's global optimizer off, as an asm statement
    // in retail's source must have (see PrepareTransferTextureAndClut).
    if (0) asm("nop");
    // Named copies of the parameters, in this order, give retail's prologue
    // copy order and its register map (dbp is the one spilled).
    int dbp = dbp_;
    int xpos = x;
    int dbw = width >> 6;
    int ypos = y;
    int h = height;
    u_int qwc = (u_int)(h * 32) >> 4;
    int columns = width >> 4;
    int i;
    int step;
    u_long yField;
    // The parameter, not the copy: retail reads $t0 here.
    if (!stride)
        stride = height;
    m_packet->ResetPacket();
    m_packet->AddSourceDmaTag();
    m_packet->AddGifTag(0);
    m_packet->AddGifData(SCE_GS_BITBLTBUF, SCE_GS_SET_BITBLTBUF(0, 0, 0, (u_int)dbp, (u_int)dbw, 2));
    m_packet->AddGifData(SCE_GS_TRXREG, SCE_GS_SET_TRXREG(16, (u_int)h));
    step = (stride >> 4) * sizeof(sceIpuRGB16);
    yField = (u_long)(u_int)ypos << 48;
    columns *= 16;
    for (i = 0; i < columns; i += 16, rgb = (sceIpuRGB16 *)((unsigned char *)rgb + step)) {
        m_packet->AddSourceDmaTag();
        m_packet->AddGifTag(0);
        // Named, for retail's `or` operand order.
        u_long xField = (u_long)(u_int)(xpos + i) << 32;
        m_packet->AddGifData(SCE_GS_TRXPOS, xField | yField);
        m_packet->AddGifData(SCE_GS_TRXDIR, SCE_GS_SET_TRXDIR(0));
        m_packet->AddGifTextureTag(qwc);
        m_packet->AddSourceDmaTextureTag((unsigned char *)rgb, qwc);
    }
    m_packet->AddSourceDmaTag();
    m_packet->AddGifTag(1);
    m_packet->AddGifData(SCE_GS_TEXFLUSH, 0);
    m_packet->AddDmaEndTag();
}


// ---- src/GsTextureTransfer/PrepareTransferTiledTexture_002C4950.cpp
// PrepareTransferTiledTexture__17GsTextureTransferFP11sceIpuRGB32iiiiii  @ 0x002C4950



// Uploads IPU output (16-pixel-wide macroblock columns) one column at a time.
void GsTextureTransfer::PrepareTransferTiledTexture(sceIpuRGB32 *rgb, int dbp_, int width, int height, int x, int y, int stride)
{
    // A dead inline asm turns mwcc's global optimizer off, as an asm statement
    // in retail's source must have (see PrepareTransferTextureAndClut).
    if (0) asm("nop");
    static int TransferTiledTexture32ID = -1;   // unused profiler ID
    // Named copies of the parameters, in this order, give retail's prologue
    // copy order and its register map (dbp is the one spilled).
    int dbp = dbp_;
    int xpos = x;
    int dbw = width >> 6;
    int ypos = y;
    int h = height;
    u_int qwc = (u_int)(h * 64) >> 4;
    int columns = width >> 4;
    int i;
    int step;
    u_long yField;
    // The parameter, not the copy: retail reads $t0 here.
    if (!stride)
        stride = height;
    m_packet->ResetPacket();
    m_packet->AddSourceDmaTag();
    m_packet->AddGifTag(0);
    m_packet->AddGifData(SCE_GS_BITBLTBUF, SCE_GS_SET_BITBLTBUF(0, 0, 0, (u_int)dbp, (u_int)dbw, 0));
    m_packet->AddGifData(SCE_GS_TRXREG, SCE_GS_SET_TRXREG(16, (u_int)h));
    step = (stride >> 4) * sizeof(sceIpuRGB32);
    yField = (u_long)(u_int)ypos << 48;
    columns *= 16;
    for (i = 0; i < columns; i += 16, rgb = (sceIpuRGB32 *)((unsigned char *)rgb + step)) {
        m_packet->AddSourceDmaTag();
        m_packet->AddGifTag(0);
        // Named, for retail's `or` operand order.
        u_long xField = (u_long)(u_int)(xpos + i) << 32;
        m_packet->AddGifData(SCE_GS_TRXPOS, xField | yField);
        m_packet->AddGifData(SCE_GS_TRXDIR, SCE_GS_SET_TRXDIR(0));
        m_packet->AddGifTextureTag(qwc);
        m_packet->AddSourceDmaTextureTag((unsigned char *)rgb, qwc);
    }
    m_packet->AddSourceDmaTag();
    m_packet->AddGifTag(1);
    m_packet->AddGifData(SCE_GS_TEXFLUSH, 0);
    m_packet->AddDmaEndTag();
}


// ---- src/GsTextureTransfer/PrepareTransferTextureToFrameBuffer.cpp

// Retail compiled this function without the global optimizer (unrotated loops,
// the clamp's empty arm kept as a branch).
#pragma global_optimizer off

// Uploads a 32-bit image into the current frame buffer, either in one run of
// 0x7FC0-quadword chunks or, with a source stride, one row at a time.
void GsTextureTransfer::PrepareTransferTextureToFrameBuffer(unsigned char *texture, int width, int height, int x, int y, int stride)
{
    int dbp = LpVideoCard->GetCurrentFrameBufferAddress() << 5;
    int dbw = LpVideoCard->m_width >> 6;
    u_int total = (width * height * 4) >> 4;
    u_int done = 0;
    m_packet->ResetPacket();
    m_packet->AddSourceDmaTag();
    m_packet->AddGifTag(0);
    m_packet->AddGifData(SCE_GS_BITBLTBUF, SCE_GS_SET_BITBLTBUF(0, 0, 0, (u_int)dbp, (u_int)dbw, 0));
    m_packet->AddGifData(SCE_GS_TRXREG, SCE_GS_SET_TRXREG((u_int)width, (u_int)height));
    m_packet->AddGifData(SCE_GS_TRXPOS, SCE_GS_SET_TRXPOS(0, 0, (u_int)x, (u_int)y, 0));
    m_packet->AddGifData(SCE_GS_TRXDIR, SCE_GS_SET_TRXDIR(0));
    if (!stride) {
        while (done < total) {
            u_int qwc = total - done;
            qwc = (qwc > 0x7FC0) ? 0x7FC0 : qwc;
            m_packet->AddSourceDmaTag();
            m_packet->AddGifTextureTag(qwc);
            m_packet->AddSourceDmaTextureTag(texture + done * 16, qwc);
            done += qwc;
        }
    } else {
        int offset;
        int rowQwc = width >> 4;
        for (offset = 0; done < total; done += rowQwc, offset += stride) {
            m_packet->AddSourceDmaTag();
            m_packet->AddGifTextureTag(rowQwc);
            m_packet->AddSourceDmaTextureTag(texture + offset, rowQwc);
        }
    }
    m_packet->AddSourceDmaTag();
    m_packet->AddGifTag(1);
    m_packet->AddGifData(SCE_GS_TEXFLUSH, 0);
    m_packet->AddDmaEndTag();
}
#pragma global_optimizer reset


// ---- src/GsTextureTransfer/PrepareTransferTextureToDisplayBuffer.cpp

// Retail compiled this function without the global optimizer (unrotated loops,
// the clamp's empty arm kept as a branch).
#pragma global_optimizer off

// Uploads a 32-bit image into the current display buffer, either in one run of
// 0x7FC0-quadword chunks or, with a source stride, one row at a time.
void GsTextureTransfer::PrepareTransferTextureToDisplayBuffer(unsigned char *texture, int width, int height, int x, int y, int stride)
{
    int dbp = LpVideoCard->GetCurrentDisplayBufferAddress() << 5;
    int dbw = LpVideoCard->m_width >> 6;
    u_int total = (width * height * 4) >> 4;
    u_int done = 0;
    m_packet->ResetPacket();
    m_packet->AddSourceDmaTag();
    m_packet->AddGifTag(0);
    m_packet->AddGifData(SCE_GS_BITBLTBUF, SCE_GS_SET_BITBLTBUF(0, 0, 0, (u_int)dbp, (u_int)dbw, 0));
    m_packet->AddGifData(SCE_GS_TRXREG, SCE_GS_SET_TRXREG((u_int)width, (u_int)height));
    m_packet->AddGifData(SCE_GS_TRXPOS, SCE_GS_SET_TRXPOS(0, 0, (u_int)x, (u_int)y, 0));
    m_packet->AddGifData(SCE_GS_TRXDIR, SCE_GS_SET_TRXDIR(0));
    if (!stride) {
        while (done < total) {
            u_int qwc = total - done;
            qwc = (qwc > 0x7FC0) ? 0x7FC0 : qwc;
            m_packet->AddSourceDmaTag();
            m_packet->AddGifTextureTag(qwc);
            m_packet->AddSourceDmaTextureTag(texture + done * 16, qwc);
            done += qwc;
        }
    } else {
        int offset;
        int rowQwc = width >> 4;
        for (offset = 0; done < total; done += rowQwc, offset += stride) {
            m_packet->AddSourceDmaTag();
            m_packet->AddGifTextureTag(rowQwc);
            m_packet->AddSourceDmaTextureTag(texture + offset, rowQwc);
        }
    }
    m_packet->AddSourceDmaTag();
    m_packet->AddGifTag(1);
    m_packet->AddGifData(SCE_GS_TEXFLUSH, 0);
    m_packet->AddDmaEndTag();
}
#pragma global_optimizer reset


// ---- src/GsTextureTransfer/TransferVideoFrameToFrameBuffer.cpp
#pragma fast_fptosi on

typedef unsigned long u_long;
typedef unsigned int u_int;
#define SCE_GS_SET_BITBLTBUF(sbp, sbw, spsm, dbp, dbw, dpsm) \
    ((u_long)(sbp) | ((u_long)(sbw) << 16) | ((u_long)(spsm) << 24) | \
     ((u_long)(dbp) << 32) | ((u_long)(dbw) << 48) | ((u_long)(dpsm) << 56))







extern PSXVideoCard *LpVideoCard;



void GsTextureTransfer::TransferVideoFrameToFrameBuffer(sceIpuRGB32 *rgb, int width, int height, int x, int y,
                                                        int stride, DmaPacket *zPacket, unsigned int *zData,
                                                        DmaPacket *packet, unsigned int *alphaData,
                                                        unsigned int *xyz0Data, unsigned int *xyz1Data,
                                                        unsigned char a, unsigned char b, unsigned char c,
                                                        unsigned char d, unsigned char fix)
{
    int offsetX;
    int offsetY;
    _RECT rect;

    ((PSXRenderTarget *)LpRenderTarget)->WaitForRenderingToComplete();
    u_int zbp = LpVideoCard->GetZBufferAddress() << 5;
    zPacket->AddGifData(zData, 0x50, SCE_GS_SET_BITBLTBUF(0, 0, 0, zbp, (u_int)(LpVideoCard->m_width >> 6), 0));
    zPacket->SendGifPacket(1, 1);
    ((PSXRenderTarget *)LpRenderTarget)->WaitForRenderingToComplete();
    if (((PSXRenderTarget *)LpRenderTarget)->m_camera) {
        ((PSXRenderTarget *)LpRenderTarget)->m_camera->GetViewportBoundsPhysical(&rect);
        offsetX = (int)(2048.0f - 0.5f * (rect.right - rect.left) - rect.left) << 4;
        offsetY = (int)(2048.0f - 0.5f * (rect.bottom - rect.top) - rect.top) << 4;
    } else {
        offsetX = 0x6C00;
        offsetY = 0x7200;
    }
    if (width != 640 && height != 448) {
        // A no-op float round trip, as in retail (a scale factor of 1 compiled out?).
        width = (int)(float)width;
        height = (int)(float)height;
    }
    packet->AddGifData(alphaData, 0x42,
                       (u_long)a | ((u_long)b << 2) | ((u_long)c << 4) | ((u_long)d << 6) | ((u_long)fix << 32));
    packet->AddGifData(xyz0Data, 5, (u_long)(offsetX + x * 16) | ((u_long)(offsetY + y * 16) << 16));
    packet->AddGifData(xyz1Data, 5,
                       (u_long)(offsetX + (x + width) * 16) | ((u_long)(offsetY + (y + height) * 16) << 16));
    packet->SendGifPacket(1, 1);
    if (((PSXRenderTarget *)LpRenderTarget)->m_camera)
        LpVideoCard->ClearFrameBuffer(0, 0, 2);
}
#pragma fast_fptosi reset


// ---- src/GsTextureTransfer/GetTextureFromVideoMemory.cpp

#define GS_CSR      (*(volatile u_long *)0x12001000)
#define GS_BUSDIR   (*(volatile u_long *)0x12001040)
#define VIF1_STAT   (*(volatile u_int *)0x10003C00)
#define D1_CHCR     (*(volatile u_int *)0x10009000)
#define D1_MADR     (*(volatile u_int *)0x10009010)
#define D1_QWC      (*(volatile u_int *)0x10009020)

// Reads a rectangle of GS memory back into main memory (local -> host
// transfer over PATH1/VIF1).
void GsTextureTransfer::GetTextureFromVideoMemory(unsigned char *dest, int sbp, int sbw, int width, int height,
                                                  int x, int y, int psm, int bpp)
{
    int qwc = (bpp * (width * height)) >> 4;
    GsSyncPath(1, (GS_PATH)3);
    m_packet->ResetPacket();
    m_packet->AddSourceDmaTag();
    m_packet->AddGifTag(1);
    m_packet->AddGifData(SCE_GS_BITBLTBUF, SCE_GS_SET_BITBLTBUF(sbp, (u_int)(sbw >> 6), (u_int)psm, 0, 0, 0));
    m_packet->AddGifData(SCE_GS_TRXREG, SCE_GS_SET_TRXREG(width, height));
    m_packet->AddGifData(SCE_GS_TRXPOS, SCE_GS_SET_TRXPOS(x, y, 0, 0, 0));
    m_packet->AddGifData(0x61, 0);     // FINISH
    m_packet->AddGifData(SCE_GS_TRXDIR, SCE_GS_SET_TRXDIR(1));
    m_packet->AddDmaEndTag();
    GS_CSR = 2;                         // clear the FINISH event
    m_packet->SendGifPacket(1, 1);
    while (!(GS_CSR & 2))
        ;
    GsSyncPath(1, (GS_PATH)1);
    VIF1_STAT = 0x800000;               // FDR: VIF1 FIFO direction GS -> memory
    GS_BUSDIR = 1;
    D1_QWC = qwc;
    D1_MADR = (u_int)dest & 0x0FFFFFFF;
    D1_CHCR = 0x100;
    GsSyncPath(1, (GS_PATH)1);
    VIF1_STAT = 0;
    GS_BUSDIR = 0;
}


// ---- src/GsTextureTransfer/SendTexture.cpp


void GsTextureTransfer::SendTexture(int wait)
{
    m_packet->SendGifPacket(wait, 1);
}


