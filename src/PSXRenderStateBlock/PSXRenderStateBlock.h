// PSXRenderStateBlock: the GS state (TEX0/TEX1/ALPHA/TEST fields) a render
// item is built with (0x66 bytes of data, 0x68 with the 64-bit cached
// registers' alignment; ScreenPrimitive::Create news 0x68).
#ifndef PSXRENDERSTATEBLOCK_H
#define PSXRENDERSTATEBLOCK_H

class DmaRenderItem;
class PSXTextureMap;

// Enumerator names and values as SetTextureFilter switches on them.
enum _D3DTEXTUREFILTERTYPE { D3DFILTER_NEAREST, D3DFILTER_LINEAR, D3DFILTER_MIPNEAREST };

class PSXRenderStateBlock {
public:
    PSXRenderStateBlock();
    ~PSXRenderStateBlock();
    void SetDefaultRenderStates();
    void AlphaTestEnable(int enable);
    void ZTestEnable(int enable);
    void ZWriteEnable(int enable);
    void ColorKeyEnable(int enable);
    void SetTextureMap(PSXTextureMap *texture);
    void SetTextureFunction(int function);
    void SetTextureFilter(_D3DTEXTUREFILTERTYPE filter);
    void SetTextureAddressMode(int mode);
    void SetAlphaBlendMode(int mode);
    void SetAlphaBlendMode(int mode, unsigned char fix);
    void SetAlphaCompare(int compare);
    void SetAlphaRefValue(int value);
    void SetAlphaFailMethod(int method);
    int GetRenderItemSize();
    void ComposeRenderItem(DmaRenderItem *item);
    void ComposeSimpleRenderItem(DmaRenderItem *item);

    // TEX0
    short m_tbp;                            // +0x00
    unsigned char m_tbw;                    // +0x02
    unsigned char m_psm;                    // +0x03
    unsigned char m_tw;                     // +0x04
    unsigned char m_th;                     // +0x05
    unsigned char m_tcc;                    // +0x06
    unsigned char m_tfx;                    // +0x07, texture function
    short m_cbp;                            // +0x08
    unsigned char m_cpsm;                   // +0x0A
    unsigned char m_csm;                    // +0x0B
    unsigned char m_csa;                    // +0x0C
    unsigned char m_cld;                    // +0x0D
    // TEX1
    unsigned char m_lcm;                    // +0x0E
    unsigned char m_mxl;                    // +0x0F
    unsigned char m_mmag;                   // +0x10
    unsigned char m_mmin;                   // +0x11, texture filter
    unsigned char m_mtba;                   // +0x12
    unsigned char m_l;                      // +0x13
    unsigned char m_k;                      // +0x14
    unsigned char m_colorKeyR;              // +0x15 (TEXA TA0 in some views)
    unsigned char m_colorKeyG;              // +0x16 (TA1)
    unsigned char m_colorKeyB;              // +0x17 (AEM)
    // ALPHA
    unsigned char m_blendA;                 // +0x18
    unsigned char m_blendB;                 // +0x19
    unsigned char m_blendC;                 // +0x1A
    unsigned char m_blendD;                 // +0x1B
    unsigned char m_blendFix;               // +0x1C
    char pad1D[0x20 - 0x1D];
    unsigned long m_cachedRegs[8];          // +0x20
    // TEST
    unsigned char m_alphaTest;              // +0x60, ATST
    unsigned char m_alphaRef;               // +0x61
    unsigned char m_alphaFail;              // +0x62
    unsigned char m_wrapMode;               // +0x63, texture address mode
    unsigned char m_alphaTestEnable : 1;    // +0x64 bit 0
    unsigned char m_zTestEnable : 1;        //        bit 1
    unsigned char m_colorKeyEnable : 1;     //        bit 2
    unsigned char m_alphaBlendEnable : 1;   //        bit 3
    unsigned char m_x64_bit4 : 1;           //        bit 4
    unsigned char m_x64_bit5 : 1;           //        bit 5
    unsigned char m_x64_bit6 : 1;           //        bit 6
    unsigned char m_zWriteEnable : 1;       //        bit 7
    unsigned char m_x65_bit0 : 1;           // +0x65 bit 0
    unsigned char m_x65_bit1 : 1;           //        bit 1
    unsigned char m_x65_bit2 : 1;           //        bit 2
};

#endif
