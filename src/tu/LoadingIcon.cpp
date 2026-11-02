// LoadingIcon.cpp  @ 0x00395CA0 (text 0x395CA0-0x396A00)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's LoadingIcon.cpp as one translation unit: the vblank handler
// LoadingIconInterrupt and LoadingIcon's methods (Create's ASSERTs say
// "LoadingIcon.cpp"), a TU without a static initializer at the end of
// ParticleVUPhysics.cpp's tu_map row.


#include "soultree_assert_block.h"
#include "d3dtypes.h"
#include "PSXTextureMap/PSXTextureMap.h"
#include "PSXVideoCard/PSXVideoCard.h"
#include "RenderTarget/RenderTarget.h"
#include "DataAccess/DataAccess.h"
#include "DmaPacket/DmaPacket.h"
#include "_RECT/_RECT.h"
#include "LoadingIcon/LoadingIcon.h"

// Defined here for its retail .bss slot.
DmaPacket *LpSendPacket;
unsigned int *Tex0TagAddress;
int StartAddress;
int Increment;
int ClutIncrement;
int Iterate;
int MaxIterate;
int SpeedCount;
u_long Tex0Setting;
int NewAddress;
int NewClutAddress;
u_long NewTex0;


typedef unsigned long u_long;   // 64-bit under mwccps2

// libgraph's register macros.
#define SCE_GS_SET_BITBLTBUF(sbp, sbw, spsm, dbp, dbw, dpsm) \
    ((u_long)(sbp) | ((u_long)(sbw) << 16) | ((u_long)(spsm) << 24) | \
     ((u_long)(dbp) << 32) | ((u_long)(dbw) << 48) | ((u_long)(dpsm) << 56))
#define SCE_GS_SET_TRXREG(rrw, rrh) ((u_long)(rrw) | ((u_long)(rrh) << 32))
#define SCE_GS_SET_FRAME(fbp, fbw, psm, fbmsk) \
    ((u_long)(fbp) | ((u_long)(fbw) << 16) | ((u_long)(psm) << 24) | ((u_long)(fbmsk) << 32))
#define SCE_GS_SET_TEST(ate, atst, aref, afail, date, datm, zte, ztst) \
    ((u_long)(ate) | ((u_long)(atst) << 1) | ((u_long)(aref) << 4) | ((u_long)(afail) << 12) | \
     ((u_long)(date) << 14) | ((u_long)(datm) << 15) | ((u_long)(zte) << 16) | ((u_long)(ztst) << 17))
#define SCE_GS_SET_PRIM(prim, iip, tme, fge, abe, aa1, fst, ctxt, fix) \
    ((u_long)(prim) | ((u_long)(iip) << 3) | ((u_long)(tme) << 4) | ((u_long)(fge) << 5) | \
     ((u_long)(abe) << 6) | ((u_long)(aa1) << 7) | ((u_long)(fst) << 8) | ((u_long)(ctxt) << 9) | \
     ((u_long)(fix) << 10))
#define SCE_GS_SET_ALPHA(a, b, c, d, fix) \
    ((u_long)(a) | ((u_long)(b) << 2) | ((u_long)(c) << 4) | ((u_long)(d) << 6) | ((u_long)(fix) << 32))
#define SCE_GS_SET_TEX0(tbp, tbw, psm, tw, th, tcc, tfx, cbp, cpsm, csm, csa, cld) \
    ((u_long)(tbp) | ((u_long)(tbw) << 14) | ((u_long)(psm) << 20) | ((u_long)(tw) << 26) | \
     ((u_long)(th) << 30) | ((u_long)(tcc) << 34) | ((u_long)(tfx) << 35) | ((u_long)(cbp) << 37) | \
     ((u_long)(cpsm) << 51) | ((u_long)(csm) << 55) | ((u_long)(csa) << 56) | ((u_long)(cld) << 61))
#define SCE_GS_SET_TEX1(lcm, mxl, mmag, mmin, mtba, l, k) \
    ((u_long)(lcm) | ((u_long)(mxl) << 2) | ((u_long)(mmag) << 5) | ((u_long)(mmin) << 6) | \
     ((u_long)(mtba) << 9) | ((u_long)(l) << 19) | ((u_long)(k) << 32))
#define SCE_GS_SET_UV(u, v) ((u_long)(u) | ((u_long)(v) << 16))
#define SCE_GS_SET_RGBAQ(r, g, b, a, q) \
    ((u_long)(r) | ((u_long)(g) << 8) | ((u_long)(b) << 16) | ((u_long)(a) << 24) | ((u_long)(q) << 32))
#define SCE_GS_SET_XYZ(x, y, z) ((u_long)(x) | ((u_long)(y) << 16) | ((u_long)(z) << 32))

#define SCE_GS_PRIM 0x00
#define SCE_GS_RGBAQ 0x01
#define SCE_GS_UV 0x03
#define SCE_GS_XYZ2 0x05
#define SCE_GS_TEX0_1 0x06
#define SCE_GS_TEX1_1 0x14
#define SCE_GS_ALPHA_1 0x42
#define SCE_GS_TEST_1 0x47
#define SCE_GS_FRAME_1 0x4C
#define SCE_GS_BITBLTBUF 0x50
#define SCE_GS_TRXPOS 0x51
#define SCE_GS_TRXREG 0x52
#define SCE_GS_TRXDIR 0x53
#define SCE_GS_TEXFLUSH 0x3F

extern "C" int sprintf(char *buf, const char *fmt, ...);
extern "C" int AddIntcHandler(int cause, int (*handler)(int), int next);
extern "C" int EnableIntc(int cause);
extern "C" int DisableIntc(int cause);
extern "C" int RemoveIntcHandler(int cause, int id);
extern "C" void FlushCache(int mode);
extern "C" double log(double x);

extern PSXVideoCard *LpVideoCard;
extern DataAccess *LpDataAccess;
extern RenderTarget *LpRenderTarget;

class Palette8;
PSXTextureMap *GetTextureMapFromResourceName(char *name, int flags, Palette8 *palette, unsigned int a,
                                             _D3DBLEND src, _D3DBLEND dst, int alpha, unsigned int colorKey, int b);

extern char UIContentDirectory[0x104];
extern DmaPacket *LpSendPacket;
extern int SpeedCount;
int Speed = 1;
extern u_long Tex0Setting;      // sceGsTex0
extern u_long NewTex0;          // sceGsTex0
extern int Iterate;
extern int MaxIterate;
extern int Increment;
extern int StartAddress;
extern int ClutIncrement;
extern int NewAddress;
extern int NewClutAddress;
extern unsigned int *Tex0TagAddress;

#define GIF_CHCR (*(volatile unsigned int *)0x1000A000)

// Interrupt handler: every Speed ticks, point the loading icon's TEX0 at the
// next animation frame and send it.
int LoadingIconInterrupt(int cause)
{
    if (LpSendPacket && !(GIF_CHCR & 0x100)) {
        SpeedCount++;
        if (SpeedCount >= Speed) {
            unsigned int *upper;
            upper = (unsigned int *)&NewTex0;
            NewTex0 = Tex0Setting;
            upper++;
            NewAddress = StartAddress + Iterate * Increment;
            NewClutAddress = NewAddress + ClutIncrement;
            // TBP0 (bits 0-13) set on the whole word, CBP (bits 37-50) on
            // the upper 32-bit word.
            NewTex0 = (NewTex0 & ~(u_long)0x3FFF) | (long)(NewAddress & 0x3FFF);
            *upper = (*upper & 0xFFF8001F) | ((NewClutAddress & 0x3FFF) << 5);
            LpSendPacket->AddGifData(Tex0TagAddress, 6, NewTex0);
            LpSendPacket->SendGifPacket(0, 1);
            Iterate++;
            if (Iterate >= MaxIterate)
                Iterate = 0;
            SpeedCount = 0;
        }
    }
    asm("sync; ei");
    return -1;
}


LoadingIcon::LoadingIcon()
{
    m_numFrames = 0;
    m_packet = new DmaPacket(0x17, 1);
    m_xyz0 = 0;
    m_xyz1 = 0;
    LpSendPacket = 0;
    Tex0TagAddress = 0;
    SpeedCount = 0;
    MaxIterate = 0;
    Iterate = 0;
    Increment = 0;
    StartAddress = 0x3800;
    Speed = 1;
    m_psm = 0;
}

LoadingIcon::~LoadingIcon()
{
    LpSendPacket = 0;
    if (m_packet)
        delete m_packet;
    DisableIntc(3);
    RemoveIntcHandler(3, 0);
}

// Loads the icon's animation frames into the top of video memory and builds
// the packet LoadingIconInterrupt sends each vblank: a textured sprite at rect.
void LoadingIcon::Create(char *name, int numFrames, _RECT rect, unsigned char alpha, int copyDisplay)
{
    char path[0x110];
    char frameName[0x110];
    unsigned char a;
    int tbw;
    int i;
    int total;
    PSXTextureMap **frames;
    int available;
    int abe;
    int tw;
    int th;
    available = (0x4000 - StartAddress) << 8;
    total = 0;
    abe = 0;

    if (alpha == 0xFF)
        a = 0x40;
    else
        a = alpha >> 2;
    AddIntcHandler(3, LoadingIconInterrupt, 0);
    m_numFrames = numFrames;
    frames = new PSXTextureMap *[m_numFrames];
    sprintf(path, "%s%s.lsi", UIContentDirectory, name);
    LpDataAccess->LoadResourceFile(path, 0, 0, 0, 0, 0, 0, 0);
    for (i = 0; i < m_numFrames; i++) {
        sprintf(frameName, "frame%02d.pst", i + 1);
        frames[i] = GetTextureMapFromResourceName(frameName, 0x613, 0, 0, (_D3DBLEND)5, (_D3DBLEND)6, 0, 0xFF00FF, 1);
        FlushCache(0);
        total += (frames[i]->m_width * frames[i]->m_height * frames[i]->m_bitsPerPixel) >> 3;
    }
    LpDataAccess->DropDevice(path, 1);
#line 122 "LoadingIcon.cpp"
    ASSERT(total <= available, "All the icons will not fit in video memory!\n");
    LpRenderTarget->WaitForRenderingToComplete();
    Increment = ((frames[0]->m_width * frames[0]->m_height * frames[0]->m_bitsPerPixel) >> 3) / 256;
    ClutIncrement = Increment;
    switch (frames[0]->m_bitsPerPixel) {
    case 32:
        abe = 1;
        m_psm = 0;
        break;
    case 16:
        m_psm = 2;
        abe = 0;
        break;
    case 8:
        m_psm = 0x13;
        abe = 1;
        Increment += 4;
        break;
    case 4:
        m_psm = 0x14;
        abe = 1;
        Increment += 4;
        break;
    default:
#line 149 "LoadingIcon.cpp"
        ASSERT(0, "Invalid texture format.");
        break;
    }
    for (i = 0; i < m_numFrames; i++) {
        PSXMipSurface *mip = frames[i]->GetTopMipLevel();
        // m_surface read as an unsigned char * (retail's type for it here):
        // a cast of the void * at the call moves the load.
        frames[i]->PrepareTransferTexture(0, StartAddress + i * Increment, ClutIncrement + (StartAddress + i * Increment),
                                          mip, *(unsigned char **)&mip->m_surface);
        frames[i]->m_transfer.SendTexture(1);
        LpRenderTarget->WaitForRenderingToComplete();
    }
    MaxIterate = m_numFrames;
    tbw = frames[0]->m_width / 64;
    tbw = tbw > 1 ? tbw : 1;   // MAX-style: retail's branch-likely clamp
    tw = log(frames[0]->m_width) / 0.69314718055994529;
    th = log(frames[0]->m_height) / 0.69314718055994529;
    rect.left = rect.left * 16 + LpVideoCard->GetXOffset();
    rect.top = rect.top * 16 + LpVideoCard->GetYOffset();
    rect.right = rect.right * 16 + LpVideoCard->GetXOffset();
    rect.bottom = rect.bottom * 16 + LpVideoCard->GetYOffset();
    m_packet->ResetPacket();
    m_packet->AddSourceDmaTag();
    m_packet->AddGifTag(1);
    if (copyDisplay) {
        m_packet->AddGifData(SCE_GS_BITBLTBUF,
                             SCE_GS_SET_BITBLTBUF(LpVideoCard->GetCurrentFrameBufferAddress() << 5, LpVideoCard->m_width >> 6,
                                                  LpVideoCard->m_psm, LpVideoCard->GetCurrentDisplayBufferAddress() << 5,
                                                  LpVideoCard->m_width >> 6, LpVideoCard->m_psm));
        m_packet->AddGifData(SCE_GS_TRXPOS, 0);
        m_packet->AddGifData(SCE_GS_TRXREG, SCE_GS_SET_TRXREG(LpVideoCard->m_width, LpVideoCard->m_height));
        m_packet->AddGifData(SCE_GS_TRXDIR, 2);
        m_packet->AddGifData(SCE_GS_FRAME_1, SCE_GS_SET_FRAME(LpVideoCard->GetCurrentDisplayBufferAddress(),
                                                              LpVideoCard->m_width >> 6, LpVideoCard->m_psm, 0));
    }
    m_packet->AddGifData(SCE_GS_TEST_1, SCE_GS_SET_TEST(1, 6, 0, 0, 0, 0, 1, 1));
    m_packet->AddGifData(SCE_GS_PRIM, SCE_GS_SET_PRIM(6, 0, 1, 0, abe, 0, 1, 0, 0));
    m_packet->AddGifData(SCE_GS_ALPHA_1, SCE_GS_SET_ALPHA(0, 1, 0, 1, 0x80));
    Tex0TagAddress = m_packet->m_cur;
    if (m_psm == 0x13 || m_psm == 0x14)
        Tex0Setting = SCE_GS_SET_TEX0(StartAddress, tbw, m_psm, tw, th, abe, 0, StartAddress + ClutIncrement, 0, 0, 0, 1);
    else
        Tex0Setting = SCE_GS_SET_TEX0(StartAddress, tbw, m_psm, tw, th, abe, 0, 0, 0, 0, 0, 0);
    m_packet->AddGifData(SCE_GS_TEX0_1, Tex0Setting);
    m_packet->AddGifData(SCE_GS_TEXFLUSH, 0);
    m_packet->AddGifData(SCE_GS_TEX1_1, SCE_GS_SET_TEX1(0, 0, 1, 5, 0, 0, -75));
    m_packet->AddGifData(SCE_GS_UV, SCE_GS_SET_UV(16, 16));
    m_packet->AddGifData(SCE_GS_RGBAQ, SCE_GS_SET_RGBAQ(0x80, 0x80, 0x80, a, 0x3F800000));
    m_xyz0 = m_packet->m_cur;
    m_packet->AddGifData(SCE_GS_XYZ2, SCE_GS_SET_XYZ(rect.left, rect.top, 0));
    m_packet->AddGifData(SCE_GS_UV, SCE_GS_SET_UV(frames[0]->m_width << 4, frames[0]->m_height << 4));
    m_packet->AddGifData(SCE_GS_RGBAQ, SCE_GS_SET_RGBAQ(0x80, 0x80, 0x80, 0x40, 0x3F800000));
    m_xyz1 = m_packet->m_cur;
    m_packet->AddGifData(SCE_GS_XYZ2, SCE_GS_SET_XYZ(rect.right, rect.bottom, 0));
    m_packet->AddGifData(SCE_GS_TEST_1, SCE_GS_SET_TEST(1, 6, 0, 0, 0, 0, 1, 3));
    m_packet->AddGifData(SCE_GS_FRAME_1, SCE_GS_SET_FRAME(LpVideoCard->GetCurrentFrameBufferAddress(),
                                                          LpVideoCard->m_width >> 6, LpVideoCard->m_psm, 0));
    if (copyDisplay)
        m_packet->AddGifData(SCE_GS_FRAME_1, SCE_GS_SET_FRAME(LpVideoCard->GetCurrentFrameBufferAddress(),
                                                              LpVideoCard->m_width >> 6, LpVideoCard->m_psm, 0));
    m_packet->AddDmaEndTag();
    LpSendPacket = m_packet;
    for (i = 0; i < m_numFrames; i++)
        frames[i]->Release();
    delete[] frames;
}


// INTC_VBLANK_S (3)
void LoadingIcon::Enable()
{
    EnableIntc(3);
}

// INTC_VBLANK_S (3)
void LoadingIcon::Disable()
{
    DisableIntc(3);
}

void LoadingIcon::SetSpeed(int speed)
{
    if (speed > 100)
        speed = 100;
    if (speed <= 0)
        speed = 1;
    Speed = 101 - speed;
}
