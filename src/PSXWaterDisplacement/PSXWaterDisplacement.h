// PSXWaterDisplacement: the PS2 water surface renderer (VIF/DMA strips,
// signature waves, wake froth). 0x710 bytes; data from +0x80, after its
// base WaterDisplacement.
// Names follow PSXWaterDisplacement's own methods (src/PSXWaterDisplacement/),
// otherwise the most common name among the files that declared it locally.
// Where copies disagree on overlapping members (+0x590 a saved _D3DVIEWPORT8 overlapping +0x5A0, +0x6CC m_wakeTextures[9]
// overlapping +0x6D0) the single members the ctor writes are kept.
#ifndef PSXWATERDISPLACEMENT_PSXWATERDISPLACEMENT_H
#define PSXWATERDISPLACEMENT_PSXWATERDISPLACEMENT_H

#include "GameObject/GameObject.h"
#include "d3dtypes.h"
#include "DmaRenderItem/DmaRenderItem.h"
#include "sceGifTag/sceGifTag.h"

class LightManager;
class PSXSignatureWave;
class PSXTextureMap;
class PSXWater;
class SoultreeObject;
class WaterObject;
struct SetViewportVif;
struct WakePoint;
// WaterDisplacement::m_params as far as its users read it.
#ifdef SPLASHDOWN_PC
#include "Fog/Fog.h"
#endif
struct WaterDisplacementParams {
#ifdef SPLASHDOWN_PC
    char pad0[__builtin_offsetof(Fog, m_near)];  // x86-64: JetskiCompetition points it at the Fog
#else
    char pad0[0x34];
#endif
    float m_x34;                        // +0x34 (PSXWater::Render3D: the fog's far distance)
    float m_farClip;                    // +0x38
};
struct _D3DINTVECTOR;
struct _sceDmaTag;

// One GS packet (GIF tag and register writes) per textured wake pass.
struct WakePacket {                     // 0xB0 bytes
    char pad0[0x20];
    unsigned long m_tex0;               // +0x20 (64-bit)
    char pad28[0x90 - 0x28];
    unsigned long m_x90;                // +0x90 (64-bit)
    char pad98[0xB0 - 0x98];
};

// The viewport Render3D saves (+0x590). Signed: SetViewport's int + DWORD
// loads the member first, as retail does.
struct ViewportRect {
    int X, Y, Width, Height;
};

// WaterDisplacement has no header of its own: GameObject, +0x30..+0x80 data,
// slot 27 (Create) pure. Its ctor (src/WaterDisplacement) zeroes +0x30..+0x38.
#ifndef WATERDISPLACEMENT_DECLARED
#define WATERDISPLACEMENT_DECLARED
class WaterDisplacement : public GameObject {
public:
    WaterDisplacement();
    virtual ~WaterDisplacement();
    virtual WaterDisplacement *Create(PSXWater *water, char *name, LightManager *lights, char *dir, int players,
                                      int mode) = 0;

    WaterDisplacementParams *m_params;  // +0x30
    SoultreeObject *m_vehicles[2];      // +0x34, per view
    char pad3C[0x80 - 0x3C];
};
#endif

class PSXWaterDisplacement : public WaterDisplacement {
public:
    PSXWaterDisplacement();
    float GetSignatureWaveHeight(float x, float z, int *flag);
    void ApplyWakeFroth(_D3DVECTOR &position, float scale, _D3DVECTOR &velocity);
    void BuildRenderItem(DmaRenderItem *item, float scale, _D3DINTVECTOR &color, float originX, float originZ);
    void ClearViewport();
    void SetViewport(int index);
    void SetViewport(_D3DVIEWPORT8 *viewport, SetViewportVif *vif);
    void RestoreViewport();
    void RenderVifCodes();
    void RenderWaterObjects();
    void RenderWake();
    int FrustumIntersectPlane(_D3DVECTOR *point, _D3DVECTOR *normal);

    // --- virtuals in retail slot order ---
    virtual ~PSXWaterDisplacement();
    virtual int Tick(float dt);
    virtual int PrepareGeometry();
    virtual int Render3D();
    virtual int KeyDownHandler(ProcessNode *node, KeyInfo *key);
    virtual PSXWaterDisplacement *Create(PSXWater *water, char *name, LightManager *lights, char *dir, int players,
                                         int mode);

    DmaRenderItem m_x80;                // +0x80
    DmaRenderItem m_xB0;                // +0xB0
    DmaRenderItem m_xE0[10];            // +0xE0
    DmaRenderItem m_x2C0[2];            // +0x2C0
    DmaRenderItem *m_x320;              // +0x320
    char pad324[0x328 - 0x324];
    PSXSignatureWave **m_waves;         // +0x328
    PSXSignatureWave **m_signatureWaves; // +0x32C
    PSXTextureMap *m_texture;           // +0x330
    char pad334[0x338 - 0x334];
    int m_x338[4];                      // +0x338
    char pad348[0x34C - 0x348];
    float m_frac[5];                    // +0x34C
    float m_invFrac[5];                 // +0x360
    int m_x374;                         // +0x374
    int m_x378;                         // +0x378
    int m_x37C;                         // +0x37C
    char pad380[0x388 - 0x380];
    float m_time;                       // +0x388
    int m_crossings[4];                 // +0x38C, m_x430 indices of the frustum edges that cross the water (FrustumIntersectPlane), -1 if none
    char pad39C[0x3A0 - 0x39C];
    _D3DVECTOR m_x3A0;                  // +0x3A0
    _D3DVECTOR m_x3B0;                  // +0x3B0
    _D3DVECTOR m_x3C0;                  // +0x3C0
    _D3DVECTOR m_x3D0;                  // +0x3D0
    _D3DVECTOR m_x3E0;                  // +0x3E0
    _D3DVECTOR m_x3F0;                  // +0x3F0
    _D3DVECTOR m_x400;                  // +0x400
    _D3DVECTOR m_x410;                  // +0x410
    _D3DVECTOR m_x420;                  // +0x420
    _D3DVECTOR m_x430[12];              // +0x430
    _D3DVECTOR m_corners[6];            // +0x4F0, the water quad's corners (an array: the ctor
                                        // constructs it with one __construct_array call)
    _D3DMATRIX m_savedMatrix;           // +0x550
    ViewportRect m_savedViewport;       // +0x590
    WaterObject **m_waterObjects;       // +0x5A0
    char pad5A4[0x5A8 - 0x5A4];
    float m_savedFov;                   // +0x5A8
    int miViewportIdx;                  // +0x5AC
    int m_currentViewport;              // +0x5B0
    int m_x5B4;                         // +0x5B4
    int m_x5B8;                         // +0x5B8
    PSXTextureMap *m_frothTexture;      // +0x5BC
    WakePoint *m_wakePoints;            // +0x5C0
    DmaRenderItem *m_wakeItems;         // +0x5C4
    unsigned int m_numFroth;            // +0x5C8
    char pad5CC[0x6CC - 0x5CC];
    PSXTextureMap *m_wakeTextures[2];   // +0x6CC, per view
    int m_x6D4;                         // +0x6D4, number of views
    int m_x6D8;                         // +0x6D8
    int m_packetIndex;                  // +0x6DC
    sceGifTag m_gifTag;                 // +0x6E0
    int m_viewIndex;                    // +0x6F0
    int m_lastPlayer;                   // +0x6F4
    SetViewportVif *m_viewportVifs;     // +0x6F8
    _sceDmaTag *m_vifCodes;             // +0x6FC
    void *m_x700;                       // +0x700
    WakePacket *m_packets;              // +0x704
    void *m_tilePackets[2];             // +0x708, per view: the z-buffer tile copy packets
};

#endif
