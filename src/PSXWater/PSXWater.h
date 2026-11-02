// PSXWater: the PS2 water surface (a Water). 0x4A0 bytes (`new` in
// JetskiCompetition::LoadWater); its own data starts at Water's 0xF0.
// Names follow its own methods (src/PSXWater/). Where they disagree:
//   +0x1F0 and +0x2E0 are two-element arrays (the ctor's __construct_array
//         calls): m_constants (constants1/2) and m_colors (env, froth).
//   +0x320: Render3D's DMA tags and VU constants; LoadWaterFromParameterBlockFile
//         points the geometry's packet at m_dataTag.
//   +0x398 is the file name (strcpy'd by LoadWaterFromParameterBlockFile);
//         the ctor sets a float at +0x498, so it is at most 0x100 bytes.
#ifndef PSXWATER_PSXWATER_H
#define PSXWATER_PSXWATER_H

#include "Water/Water.h"
#include "DmaRenderItem/DmaRenderItem.h"
#include "sceGifTag/sceGifTag.h"
#include "_sceDmaTag/_sceDmaTag.h"
#include "sinit/D3DIntVector.h"

class KeyInfo;
class ProcessNode;

typedef int int128_t __attribute__((mode(TI)));

// The VU constant block (0x40 bytes): Render3D fills one copy and copies it
// to the packet with lq/sq; Create clears its first 13 words as ints.
struct PSXWaterVuConstants {
    union {
        float f[16];
        int i[16];
    };

    void operator=(const PSXWaterVuConstants &o)
    {
#ifdef SPLASHDOWN_PC
        // x86-64: PSXWater's copies are not 16-aligned (movaps faults)
        __builtin_memcpy(this, &o, 0x40);
        return;
#endif
        ((int128_t *)this)[0] = ((const int128_t *)&o)[0];
        ((int128_t *)this)[1] = ((const int128_t *)&o)[1];
        ((int128_t *)this)[2] = ((const int128_t *)&o)[2];
        ((int128_t *)this)[3] = ((const int128_t *)&o)[3];
    }
};

class PSXWater : public Water {
public:
    PSXWater(int enabled, int resolution, int numTiles);
    void ApplyWakeFroth(_D3DVECTOR &position, float strength, _D3DVECTOR &velocity);
    void UpdateTex0();

    // --- virtuals in retail slot order ---
    virtual ~PSXWater();
    virtual int PrepareGeometry();
    virtual int Render3D();
    virtual int KeyDownHandler(ProcessNode *node, KeyInfo *key);
    virtual PSXWater *Create(char *name, LightManager *lights, int a, int b, float f);
    virtual int LoadWaterFromParameterBlockFile(char *name);
    virtual int ReloadWaterFromParameterBlockFile();
    virtual int LoadWaterFromBinaryFile(char *filename);

    DmaRenderItem m_renderItem;         // +0xF0
    sceGifTag m_stripTag;               // +0x120
    sceGifTag m_adTag;                  // +0x130
    void *m_dmaData;                    // +0x140, malloc'd (the dtor frees it)
    char pad144[0x150 - 0x144];
#ifdef SPLASHDOWN_PC
    // x86-64: m_vif..m_x310 and the two tags are DMA'd as they lie (REF
    // m_vif, 0x1D qwords): qword-aligned, as on the PS2
    unsigned int m_vif[4] __attribute__((aligned(16)));
#else
    unsigned int m_vif[4];              // +0x150
#endif
    _D3DMATRIX m_x160;                  // +0x160 (the camera's view-projection)
    PSXWaterVuConstants m_x1A0;         // +0x1A0
    _D3DVECTOR m_x1E0;                  // +0x1E0
    _D3DVECTOR m_constants[2];          // +0x1F0 (an array: the ctor constructs it with __construct_array)
    _D3DVECTOR m_x210;                  // +0x210
    sceGifTag m_stripTagCopy;           // +0x220
    sceGifTag m_adTagCopy;              // +0x230
    long m_texflush;                    // +0x240 (GS register data, then its address)
    long m_texflushReg;                 // +0x248
    long m_tex0a;                       // +0x250
    long m_tex0aReg;                    // +0x258
    long m_tex0b;                       // +0x260
    long m_tex0bReg;                    // +0x268
    long m_alphaA;                      // +0x270
    long m_alphaAReg;                   // +0x278
    long m_alphaB;                      // +0x280
    long m_alphaBReg;                   // +0x288
    long m_testA;                       // +0x290
    long m_testAReg;                    // +0x298
    long m_testB;                       // +0x2A0
    long m_testBReg;                    // +0x2A8
    _D3DVECTOR m_x2B0;                  // +0x2B0
    _D3DVECTOR m_x2C0;                  // +0x2C0
    _D3DVECTOR m_x2D0;                  // +0x2D0
    _D3DINTVECTOR m_colors[2];          // +0x2E0, the env and froth colours (an array, as m_constants)
    _D3DVECTOR m_x300;                  // +0x300
    _D3DVECTOR m_x310;                  // +0x310
    _sceDmaTag m_dataTag;               // +0x320 (the surface geometry's packet)
    _sceDmaTag m_vifTag;                // +0x330
    PSXWaterVuConstants m_vuConstants;  // +0x340
    int m_resolution;                   // +0x380
    int m_numVertices;                  // +0x384
    int m_x388;                         // +0x388
    float m_x38C;                       // +0x38C
    float m_x390;                       // +0x390
    char pad394[0x398 - 0x394];
    char m_fileName[0x100];             // +0x398
    float m_x498;                       // +0x498
    int m_enableBaseDisplacement;       // +0x49C
};

#endif
