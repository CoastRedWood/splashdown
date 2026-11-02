// PSXRenderStateBlock.cpp  @ 0x002C24C0 (text 0x2C24C0-0x2C3E50)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// PSXRenderStateBlock as one translation unit without a static initializer.
// PSXRenderStateBlock, GsTextureTransfer, IpuTextureTransfer and DmaPacket sit between
// DataAccess.cpp and JetskiGameOptions.cpp; their .rodata (@504-@818) has no label
// restart, but PSXRenderStateBlock's 16-byte constant 0xA (@507) and DmaPacket's 8-byte
// 0xA (@818) are separate copies, which one mwcc TU pools into one. So they are more
// than one TU; one per class is a guess, and so are the file names.




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

// ---- src/PSXRenderStateBlock/ctor.cpp

PSXRenderStateBlock::PSXRenderStateBlock()
{
    SetDefaultRenderStates();
    m_cachedRegs[0] = 0xFFFFFFFFFFFFFFFF;
    m_cachedRegs[1] = 0xFFFFFFFFFFFFFFFF;
    m_cachedRegs[2] = 0xFFFFFFFFFFFFFFFF;
    m_cachedRegs[3] = 0xFFFFFFFFFFFFFFFF;
    m_cachedRegs[4] = 0xFFFFFFFFFFFFFFFF;
    m_cachedRegs[5] = 0xFFFFFFFFFFFFFFFF;
    m_cachedRegs[6] = 0xFFFFFFFFFFFFFFFF;
    m_cachedRegs[7] = 0xFFFFFFFFFFFFFFFF;
}


// ---- src/PSXRenderStateBlock/dtor.cpp

PSXRenderStateBlock::~PSXRenderStateBlock()
{
}


// ---- src/PSXRenderStateBlock/SetDefaultRenderStates.cpp

void PSXRenderStateBlock::SetDefaultRenderStates()
{
    m_alphaTest = 6;
    m_alphaRef = 0;
    m_alphaFail = 0;
    m_wrapMode = 0;
    m_alphaTestEnable = 1;
    m_zTestEnable = 1;
    m_zWriteEnable = 1;
    m_tbp = -1;
    m_tbw = 0xFF;
    m_psm = 0xFF;
    m_tw = 0xFF;
    m_th = 0xFF;
    m_tcc = 1;
    m_tfx = 0;
    m_cbp = -1;
    m_cpsm = 0;
    m_csm = 0;
    m_csa = 0;
    m_cld = 0;
    m_lcm = 0;
    m_mxl = 0;
    m_mmag = 1;
    m_mmin = 5;
    m_mtba = 0;
    m_l = 0;
    m_k = 0xB5;
    m_colorKeyEnable = 1;
    m_colorKeyR = 0xFF;
    m_colorKeyG = 0xFF;
    m_colorKeyB = 1;
    m_alphaBlendEnable = 0;
    m_blendA = 0;
    m_blendB = 1;
    m_blendC = 0;
    m_blendD = 1;
    m_blendFix = 0;
    m_x64_bit4 = 0;
    m_x64_bit5 = 0;
    m_x64_bit6 = 0;
    m_x65_bit0 = 0;
    m_x65_bit1 = 0;
    m_x65_bit2 = 0;
}


// ---- src/PSXRenderStateBlock/SetAlphaBlendMode.cpp
// ASSERT contains inline asm; match retail's label alignment for asm functions.
#pragma alignlabel off



// GS ALPHA register: (A - B) * C >> 7 + D, with 0 = Cs, 1 = Cd, 2 = 0 / FIX.
void PSXRenderStateBlock::SetAlphaBlendMode(int mode, unsigned char fix)
{
    switch (mode) {
    case 0:
        m_alphaBlendEnable = mode;
        m_blendA = 0;
        m_blendB = 1;
        m_blendC = 0;
        m_blendD = 1;
        m_blendFix = fix;
        break;
    case 1:
        m_alphaBlendEnable = mode;
        m_blendA = 0;
        m_blendB = 2;
        m_blendC = 2;
        m_blendD = 1;
        m_blendFix = fix;
        break;
    case 3:
        m_alphaBlendEnable = mode;
        m_blendA = 0;
        m_blendB = 1;
        m_blendC = 2;
        m_blendD = 1;
        m_blendFix = fix;
        break;
    case 4:
        m_alphaBlendEnable = mode;
        m_blendA = 0;
        m_blendB = 2;
        m_blendC = 0;
        m_blendD = 2;
        m_blendFix = fix;
        break;
    case 5:
        m_alphaBlendEnable = mode;
        m_blendA = 0;
        m_blendB = 2;
        m_blendC = 2;
        m_blendD = 2;
        m_blendFix = fix;
        break;
    case 2:
        m_alphaBlendEnable = mode;
        m_blendA = 2;
        m_blendB = 0;
        m_blendC = 2;
        m_blendD = 1;
        m_blendFix = fix;
        break;
    default:
        PSXprintf("AlphaBlendMode not supported in PSXRenderStateBlock::SetAlphaBlendMode()\n");
#line 161 "PSXRenderStateBlock.cpp"
        ASSERTF(0, ("AlphaBlendMode (0x%x not supported in PSXRenderStateBlock::SetAlphaBlendMode()", mode));
    }
}
#pragma alignlabel reset


// ---- src/PSXRenderStateBlock/SetAlphaBlendMode_002C29C0.cpp
#pragma alignlabel off



// GS ALPHA register: (A - B) * C >> 7 + D, with 0 = Cs, 1 = Cd, 2 = 0 / FIX.
void PSXRenderStateBlock::SetAlphaBlendMode(int mode)
{
    switch (mode) {
    case 0:
        m_alphaBlendEnable = mode;
        m_blendA = 0;
        m_blendB = 1;
        m_blendC = 0;
        m_blendD = 1;
        m_blendFix = 0;
        break;
    case 1:
        m_alphaBlendEnable = mode;
        m_blendA = 0;
        m_blendB = 2;
        m_blendC = 2;
        m_blendD = 1;
        m_blendFix = 0x80;
        break;
    case 3:
        m_alphaBlendEnable = mode;
        m_blendA = 0;
        m_blendB = 2;
        m_blendC = 0;
        m_blendD = 1;
        m_blendFix = 0x80;
        break;
    case 4:
        m_alphaBlendEnable = mode;
        m_blendA = 0;
        m_blendB = 2;
        m_blendC = 0;
        m_blendD = 2;
        m_blendFix = 0;
        break;
    case 2:
        m_alphaBlendEnable = mode;
        m_blendA = 2;
        m_blendB = 0;
        m_blendC = 2;
        m_blendD = 1;
        m_blendFix = 0x80;
        break;
    default:
        PSXprintf("AlphaBlendMode not supported in PSXRenderStateBlock::SetAlphaBlendMode()\n");
#line 219 "PSXRenderStateBlock.cpp"
        ASSERTF(0, ("AlphaBlendMode (0x%x not supported in PSXRenderStateBlock::SetAlphaBlendMode()", mode));
    }
}
#pragma alignlabel reset


// ---- src/PSXRenderStateBlock/AlphaTestEnable.cpp

void PSXRenderStateBlock::AlphaTestEnable(int enable)
{
    if (enable)
        m_alphaTestEnable = 1;
    else
        m_alphaTestEnable = 0;
}


// ---- src/PSXRenderStateBlock/ZWriteEnable.cpp

void PSXRenderStateBlock::ZWriteEnable(int enable)
{
    if (enable)
        m_zWriteEnable = 1;
    else
        m_zWriteEnable = 0;
}


// ---- src/PSXRenderStateBlock/ZTestEnable.cpp

void PSXRenderStateBlock::ZTestEnable(int enable)
{
    if (enable)
        m_zTestEnable = 1;
    else
        m_zTestEnable = 0;
}


// ---- src/PSXRenderStateBlock/ColorKeyEnable.cpp

void PSXRenderStateBlock::ColorKeyEnable(int enable)
{
    m_colorKeyEnable = enable;
    if (m_colorKeyEnable) {
        m_colorKeyR = 0xFF;
        m_colorKeyG = 0xFF;
        m_colorKeyB = 1;
    } else {
        m_colorKeyR = 0;
        m_colorKeyG = 0xFF;
        m_colorKeyB = 0;
    }
}


// ---- src/PSXRenderStateBlock/SetTextureAddressMode.cpp
// ASSERT contains inline asm; match retail's label alignment for asm functions.
#pragma alignlabel off



// D3DTADDRESS_WRAP (0) and _MIRROR (1) are supported; _BORDER, _MIRRORONCE
// and the FORCE_DWORD sentinel are listed explicitly but rejected.
void PSXRenderStateBlock::SetTextureAddressMode(int mode)
{
    switch (mode) {
    case 0:
    case 1:
        m_wrapMode = mode;
        break;
    case 5:
    case 6:
    case 0x7FFFFFFF:
    default:
        PSXprintf("TextureAddressMode not supported in PSXRenderStateBlock::SetTextureAddressMode()\n");
#line 277 "PSXRenderStateBlock.cpp"
        ASSERTF(0, ("TextureAddressMode (0x%x) not supported in PSXRenderStateBlock::SetTextureAddressMode()", mode));
    }
}
#pragma alignlabel reset


// ---- src/PSXRenderStateBlock/SetAlphaCompare.cpp
// ASSERT contains inline asm; match retail's label alignment for asm functions.
#pragma alignlabel off



void PSXRenderStateBlock::SetAlphaCompare(int func)
{
    switch (func) {
    case 0: case 1: case 2: case 3:
    case 4: case 5: case 6: case 7:
        m_alphaTest = func;
        break;
    default:
        PSXprintf("AlphaCompareFunction not supported in PSXRenderStateBlock::SetAlphaCompare()\n");
#line 296 "PSXRenderStateBlock.cpp"
        ASSERTF(0, ("SetAlphaCompare: type (0x%x) not supported in PSXRenderStateBlock::SetAlphaCompare()", func));
    }
}
#pragma alignlabel reset


// ---- src/PSXRenderStateBlock/SetAlphaRefValue.cpp
// ASSERT contains inline asm; match retail's label alignment for asm functions.
#pragma alignlabel off



void PSXRenderStateBlock::SetAlphaRefValue(int value)
{
    // Retail tests with || instead of &&, which is true for every int, so
    // the error branch is unreachable. Kept as compiled.
    if (value >= 0 || value < 256) {
        m_alphaRef = value;
    } else {
        PSXprintf("AlphaRefValue out of [0,255] interval in PSXRenderStateBlock::SetAlphaRefValue()\n");
#line 306 "PSXRenderStateBlock.cpp"
        ASSERTF(0, ("AlphaRefValue (%d) out of [0,255] interval in PSXRenderStateBlock::SetAlphaRefValue()", value));
    }
}
#pragma alignlabel reset


// ---- src/PSXRenderStateBlock/SetAlphaFailMethod.cpp
// ASSERT contains inline asm; match retail's label alignment for asm functions.
#pragma alignlabel off



void PSXRenderStateBlock::SetAlphaFailMethod(int type)
{
    switch (type) {
    case 0:
    case 1:
    case 2:
    case 3:
        m_alphaFail = type;
        break;
    default:
        PSXprintf("SetAlphaFailMethod not supported in PSXRenderStateBlock::SetAlphaFailMethod()\n");
#line 321 "PSXRenderStateBlock.cpp"
        ASSERTF(0, ("SetAlphaFailMethod: type (0x%x) not supported in PSXRenderStateBlock::SetAlphaFailMethod()", type));
    }
}
#pragma alignlabel reset


// ---- src/PSXRenderStateBlock/SetTextureFunction.cpp
// ASSERT contains inline asm; match retail's label alignment for asm functions.
#pragma alignlabel off



void PSXRenderStateBlock::SetTextureFunction(int function)
{
    switch (function) {
    case 0:
    case 1:
    case 2:
    case 3:
        m_tfx = function;
        break;
    default:
        PSXprintf("Invalid texture function in PSXRenderStateBlock::SetTextureFunction()\n");
#line 336 "PSXRenderStateBlock.cpp"
        ASSERTF(0, ("SetTextureFunction: Invalid texture function (%d)", function));
    }
}
#pragma alignlabel reset


// ---- src/PSXRenderStateBlock/SetTextureFilter.cpp
// ASSERT contains inline asm; match retail's label alignment for asm functions.
#pragma alignlabel off



void PSXRenderStateBlock::SetTextureFilter(_D3DTEXTUREFILTERTYPE filter)
{
    switch (filter) {
    case D3DFILTER_NEAREST:
    case D3DFILTER_LINEAR:
        m_mmin = 0;
        break;
    case D3DFILTER_MIPNEAREST:
        break;
    default:
#line 353 "PSXRenderStateBlock.cpp"
        ASSERTF(0, ("SetTextureFilter: Invalid texture filter (%d)", filter));
    }
}
#pragma alignlabel reset


// ---- src/PSXRenderStateBlock/SetTextureMap.cpp
// ASSERT contains inline asm; match retail's label alignment for asm functions.
#pragma alignlabel off





void PSXRenderStateBlock::SetTextureMap(PSXTextureMap *texture)
{
#line 361 "PSXRenderStateBlock.cpp"
    ASSERT(texture, "SetTextureMap: lpTextureMap == NULL");
    PSXMipSurface *mip = texture->GetReqdMipLevel();
    m_tbp = texture->m_xEC;
    m_tbw = mip->m_blocks;
    m_tw = mip->m_widthLog2;
    m_th = mip->m_heightLog2;
    m_cbp = 0;
    m_cpsm = 0;
    m_csm = 0;
    m_csa = 0;
    m_cld = 0;
    switch (texture->m_format) {
    case 0x22B8:
    case 0x115C:
        m_psm = 0;
        break;
    case 0x613:
    case 0x22B:
    case 0x235:
        m_psm = 2;
        if (texture->m_compressed)
            ColorKeyEnable(1);
        else
            ColorKeyEnable(0);
        break;
    case 8:
        m_psm = 0x13;
        m_cbp = texture->m_xF0;
        m_cpsm = 0;
        m_csm = 0;
        m_csa = 0;
        m_cld = 1;
        break;
    case 4:
        m_psm = 0x14;
        m_cbp = texture->m_xF0;
        m_cpsm = 0;
        m_csm = 0;
        m_csa = 0;
        m_cld = 1;
        break;
    default:
        // (Retail's messages name SetTextureFunction.)
        PSXprintf("Invalid pixel format (%d) in PSXRenderStateBlock::SetTextureFunction()\n", texture->m_format);
#line 410 "PSXRenderStateBlock.cpp"
        ASSERTF(0, ("Invalid pixel format (%d) in PSXRenderStateBlock::SetTextureFunction()", texture->m_format));
    }
}
#pragma alignlabel reset


// ---- src/DataAccess/PSXRenderStateBlock_GetRenderItemSize.cpp

int PSXRenderStateBlock::GetRenderItemSize()
{
    return 0x34;
}


// ---- src/PSXRenderStateBlock/ComposeRenderItem.cpp

typedef int int128_t __attribute__((mode(TI)));


extern PSXVideoCard *LpVideoCard;



// The texture and blend state as A+D writes in one GIF packet. The byte
// fields are compared with -1 after promotion, so those asserts never fire.
void PSXRenderStateBlock::ComposeRenderItem(DmaRenderItem *item)
{
#line 482 "PSXRenderStateBlock.cpp"
    ASSERT(m_tbp != -1, "ComposeImmediateData: TextureRenderStates.Tex0_BasePointer == -1");
    ASSERT(m_tbw != -1, "ComposeImmediateData: TextureRenderStates.Tex0_BufferWidth == -1");
    ASSERT(m_psm != -1, "ComposeImmediateData: TextureRenderStates.Tex0_PixelFormat == -1");
    ASSERT(m_tw != -1, "ComposeImmediateData: TextureRenderStates.Tex0_TextureWidth == -1");
    ASSERT(m_th != -1, "ComposeImmediateData: TextureRenderStates.Tex0_TextureHeight == -1");
    ASSERT(m_cbp != -1, "ComposeImmediateData: TextureRenderStates.Tex0_ClutBaseAddress == -1");
    int size = GetRenderItemSize();
    if (item->m_dataListCurrentSize + size - item->m_dataListMaxSize >= 0)
        item->IncrementDataList(item->m_dataListCurrentSize + size);
    item->DmaAddVifCode(0x11000000);        // FLUSH
    item->DmaAddVifCode(0x03000000);        // BASE
    item->DmaAddVifCode(0x02000000);        // OFFSET
    if ((item->m_dataListCurrentSize + 5) % 2)
        item->DmaAddVifCode(0);             // NOP, to align the unpack
    item->DmaBeginUnpack(0x320, 0xC);
    sceGifTag tag;
    *(int128_t *)&tag = 0;
    tag.FLG = 0;
    tag.EOP = 1;
    tag.NREG = 8;
    tag.NLOOP = 1;
    tag.REGS0 = 0xE;                        // A+D
    tag.REGS1 = 0xE;
    tag.REGS2 = 0xE;
    tag.REGS3 = 0xE;
    tag.REGS4 = 0xE;
    tag.REGS5 = 0xE;
    tag.REGS6 = 0xE;
    tag.REGS7 = 0xE;
    item->DmaAddGifTagToUnpack(&tag);
    unsigned long *p = (unsigned long *)&item->m_dataList[item->m_dataListCurrentSize];
    p[0] = (unsigned long)m_tbp | ((unsigned long)m_tbw << 14) | ((unsigned long)m_psm << 20)
         | ((unsigned long)m_tw << 26) | ((unsigned long)m_th << 30) | ((unsigned long)m_tcc << 34)
         | ((unsigned long)m_tfx << 35) | ((unsigned long)m_cbp << 37) | ((unsigned long)m_cpsm << 51)
         | ((unsigned long)m_csm << 55) | ((unsigned long)m_csa << 56) | ((unsigned long)m_cld << 61);
    p[1] = 0x06;                            // TEX0_1
    p += 2;
    p[0] = 0;
    p[1] = 0x3F;                            // TEXFLUSH
    p += 2;
    p[0] = (unsigned long)m_lcm | ((unsigned long)m_mxl << 2) | ((unsigned long)m_mmag << 5)
         | ((unsigned long)m_mmin << 6) | ((unsigned long)m_mtba << 9) | ((unsigned long)m_l << 19)
         | ((unsigned long)m_k << 32);
    p[1] = 0x14;                            // TEX1_1
    p += 2;
    p[0] = (unsigned long)m_colorKeyR | ((unsigned long)m_colorKeyB << 15) | ((unsigned long)m_colorKeyG << 32);
    p[1] = 0x3B;                            // TEXA
    int ztst = 1;                           // ALWAYS
    p += 2;
    if (m_zTestEnable)
        ztst = 2;                           // GEQUAL
    p[0] = (unsigned long)m_alphaTestEnable | ((unsigned long)m_alphaTest << 1) | ((unsigned long)m_alphaRef << 4)
         | ((unsigned long)m_alphaFail << 12) | ((unsigned long)1 << 16) | ((unsigned long)ztst << 17);
    p[1] = 0x47;                            // TEST_1
    p += 2;
    p[0] = (unsigned long)LpVideoCard->GetZBufferAddress() | ((unsigned long)0x31 << 24)
         | ((unsigned long)(1 - m_zWriteEnable) << 32);
    p[1] = 0x4E;                            // ZBUF_1
    p += 2;
    p[0] = (unsigned long)m_wrapMode | ((unsigned long)m_wrapMode << 2);
    p[1] = 0x08;                            // CLAMP_1
    p += 2;
    p[0] = (unsigned long)m_blendA | ((unsigned long)m_blendB << 2) | ((unsigned long)m_blendC << 4)
         | ((unsigned long)m_blendD << 6) | ((unsigned long)m_blendFix << 32);
    p[1] = 0x42;                            // ALPHA_1
    DmaRenderItem::UnpackCount += 32;
    item->m_dataListCurrentSize += 32;
    item->DmaEndUnpack();
    item->DmaAddVifCode(0x14000000);        // MSCAL
}


// ---- src/PSXRenderStateBlock/ComposeSimpleRenderItem.cpp
typedef int int128_t __attribute__((mode(TI)));


extern PSXVideoCard *LpVideoCard;



// TEXA, ALPHA, TEST, ZBUF and CLAMP as A+D writes in one GIF packet.
void PSXRenderStateBlock::ComposeSimpleRenderItem(DmaRenderItem *item)
{
    if (item->m_dataListCurrentSize + 40 - item->m_dataListMaxSize >= 0)
        item->IncrementDataList(item->m_dataListCurrentSize + 40);
    item->DmaAddVifCode(0x11000000);        // FLUSH
    item->DmaAddVifCode(0x03000000);        // BASE
    item->DmaAddVifCode(0x02000000);        // OFFSET
    if ((item->m_dataListCurrentSize + 5) % 2)
        item->DmaAddVifCode(0);             // NOP, to align the unpack
    item->DmaBeginUnpack(0x320, 0xC);
    sceGifTag tag;
    *(int128_t *)&tag = 0;
    tag.FLG = 0;
    tag.EOP = 1;
    tag.NREG = 8;
    tag.NLOOP = 1;
    tag.REGS0 = 0xE;                        // A+D
    tag.REGS1 = 0xE;
    tag.REGS2 = 0xE;
    tag.REGS3 = 0xE;
    tag.REGS4 = 0xE;
    item->DmaAddGifTagToUnpack(&tag);
    unsigned long *p = (unsigned long *)&item->m_dataList[item->m_dataListCurrentSize];
    p[0] = (unsigned long)m_colorKeyR | ((unsigned long)m_colorKeyB << 15) | ((unsigned long)m_colorKeyG << 32);
    p[1] = 0x3B;                            // TEXA
    p[2] = (unsigned long)m_blendA | ((unsigned long)m_blendB << 2) | ((unsigned long)m_blendC << 4)
         | ((unsigned long)m_blendD << 6) | ((unsigned long)m_blendFix << 32);
    p[3] = 0x42;                            // ALPHA_1
    int ztst = 1;                           // ALWAYS
    if (m_zTestEnable)
        ztst = 2;                           // GEQUAL
    p[4] = (unsigned long)m_alphaTestEnable | ((unsigned long)m_alphaTest << 1) | ((unsigned long)m_alphaRef << 4)
         | ((unsigned long)m_alphaFail << 12) | ((unsigned long)1 << 16) | ((unsigned long)ztst << 17);
    p[5] = 0x47;                            // TEST_1
    p[6] = (unsigned long)LpVideoCard->GetZBufferAddress() | ((unsigned long)0x31 << 24)
         | ((unsigned long)(1 - m_zWriteEnable) << 32);
    p[7] = 0x4E;                            // ZBUF_1
    p[8] = (unsigned long)m_wrapMode | ((unsigned long)m_wrapMode << 2);
    p[9] = 0x08;                            // CLAMP_1
    DmaRenderItem::UnpackCount += 20;
    item->m_dataListCurrentSize += 20;
    item->DmaEndUnpack();
    item->DmaAddVifCode(0x14000000);        // MSCAL
}


