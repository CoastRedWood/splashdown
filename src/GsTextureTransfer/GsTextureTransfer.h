// GsTextureTransfer — builds GS image-transfer DMA packets (4 bytes: one
// DmaPacket, new'd by Create).
#ifndef GSTEXTURETRANSFER_H
#define GSTEXTURETRANSFER_H

typedef unsigned long u_long;
typedef unsigned int u_int;

#define SCE_GS_SET_BITBLTBUF(sbp, sbw, spsm, dbp, dbw, dpsm) \
    ((u_long)(sbp) | ((u_long)(sbw) << 16) | ((u_long)(spsm) << 24) | \
     ((u_long)(dbp) << 32) | ((u_long)(dbw) << 48) | ((u_long)(dpsm) << 56))
#define SCE_GS_SET_TRXPOS(ssax, ssay, dsax, dsay, dir) \
    ((u_long)(ssax) | ((u_long)(ssay) << 16) | \
     ((u_long)(dsax) << 32) | ((u_long)(dsay) << 48) | ((u_long)(dir) << 59))
#define SCE_GS_SET_TRXREG(rrw, rrh) ((u_long)(rrw) | ((u_long)(rrh) << 32))
#define SCE_GS_SET_TRXDIR(xdr) ((u_long)(xdr))

#define SCE_GS_BITBLTBUF 0x50
#define SCE_GS_TRXPOS 0x51
#define SCE_GS_TRXREG 0x52
#define SCE_GS_TRXDIR 0x53
#define SCE_GS_TEXFLUSH 0x3F

// libipu macroblock outputs: 16x16 pixels.
struct sceIpuRGB16 {
    unsigned short pix[256];
};
struct sceIpuRGB32 {
    unsigned int pix[256];
};

#include "GsTextureTransfer/GsSyncPath.h"

#include "DmaPacket/DmaPacket.h"

#include "PSXVideoCard/PSXVideoCard.h"
extern PSXVideoCard *LpVideoCard;

class GsTextureTransfer {
public:
    GsTextureTransfer();
    ~GsTextureTransfer();
    GsTextureTransfer *Create(int size);
    void SendTexture(int wait);
    void PrepareTransferOversizedTexture(unsigned char *texture, int dbp, int width, int height,
                                         int x, int y, int psm, int bpp);
    void PrepareTransferTextureToFrameBuffer(unsigned char *texture, int width, int height, int x, int y, int stride);
    void PrepareTransferTextureToDisplayBuffer(unsigned char *texture, int width, int height, int x, int y, int stride);
    void GetTextureFromVideoMemory(unsigned char *dest, int sbp, int sbw, int width, int height, int x, int y,
                                   int psm, int bpp);
    void PrepareTransferTextureAndClut(unsigned char *texture, unsigned char *clut, int dbp, int clutDbp,
                                       int width, int height, int x, int y, int psm, int clutPsm, int bpp, int dbw);
    void PrepareTransferTexture(unsigned char *texture, int dbp, int width, int height, int x, int y, int psm,
                                int bpp, int stride);
    void PrepareTransferTiledTexture(sceIpuRGB16 *rgb, int dbp, int width, int height, int x, int y, int stride);
    void PrepareTransferTiledTexture(sceIpuRGB32 *rgb, int dbp, int width, int height, int x, int y, int stride);
    void UpdateTransferTexture(unsigned char *texture, int dbp, int width, int height, int psm, int bpp);
    void UpdateTransferTextureAndClut(unsigned char *texture, unsigned char *clut, int dbp, int clutDbp,
                                      int width, int height, int psm, int clutPsm, int bpp, int dbw);
    void TransferVideoFrameToFrameBuffer(sceIpuRGB32 *rgb, int width, int height, int x, int y, int stride,
                                         DmaPacket *zPacket, unsigned int *zData, DmaPacket *packet,
                                         unsigned int *alphaData, unsigned int *xyz0Data, unsigned int *xyz1Data,
                                         unsigned char a, unsigned char b, unsigned char c, unsigned char d,
                                         unsigned char fix);

    DmaPacket *m_packet;    // +0x00
};

#endif
