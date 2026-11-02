// PSXSignatureWave: an animated wave texture laid on the water (PSXWater's
// m_waves), with its own VU1 render-state and geometry packets. Not a
// GameObject and no vtable; 0x120 bytes (new in
// PSXWater::LoadWaterFromParameterBlockFile). Names come from the class's
// own methods.
#ifndef PSXSIGNATUREWAVE_PSXSIGNATUREWAVE_H
#define PSXSIGNATUREWAVE_PSXSIGNATUREWAVE_H

#include "d3dtypes.h"
#include "sceGifTag/sceGifTag.h"

class PSXTextureMap;
class PSXWaterDisplacement;
struct _D3DINTVECTOR;

// Render-state packet: a GIF tag and eight A+D register writes.
struct WaveStatePacket {                // 0xB0 bytes
    unsigned int m_vif[4];              // +0x00
    sceGifTag m_gifTag;                 // +0x10
    unsigned long m_ad[16];             // +0x20, data/register pairs
    unsigned int m_vifEnd[4];           // +0xA0
};

struct WaveGeometryPacket {             // 0xF0 bytes
    unsigned int m_vif[4];              // +0x00
    sceGifTag m_gifTag;                 // +0x10
    char pad20[0xE0 - 0x20];
    unsigned int m_vifEnd[4];           // +0xE0
};

class PSXSignatureWave {
public:
    PSXSignatureWave(_D3DVECTOR &position, _D3DVECTOR &direction, unsigned int numWaves, float width, float length,
                     float widthSpeed, float lengthSpeed, float unused, float xC8, float xCC, int xD4, int mode,
                     int x110);
    ~PSXSignatureWave();
    void SetParams(_D3DVECTOR &position, _D3DVECTOR &direction, unsigned int numWaves, float width, float length,
                   float widthSpeed, float lengthSpeed, float unused, float xC8, float xCC, int xD4, int mode,
                   int x110);
    void setOneOverNumWaves(float oneOverNumWaves);
    int loadAnimation(char *name);
    int Tick(float dt, unsigned int *frames);
    void BuildRenderItem(PSXWaterDisplacement *displacement);
    void addVerticesToRenderItem(PSXWaterDisplacement *displacement, _D3DINTVECTOR *color, PSXTextureMap *map,
                                 int viewport, int pass);
    void RenderWaterObjects(PSXWaterDisplacement *displacement);
    // Inline in retail's PSXSignatureWave.cpp (its ASSERT's statics are
    // $localstatics); defined there, src/tu/PSXSignatureWave.cpp.
    void SetTextureMap(PSXTextureMap *texture, unsigned long *tex0);

    PSXTextureMap **m_maps;             // +0x00, one per animation frame
    unsigned int m_numSegments;         // +0x04
    int m_pitch;                        // +0x08
    unsigned int m_bytesPerPixel;       // +0x0C
    float m_width;                      // +0x10
    float m_length;                     // +0x14
    float m_oneOverWidth;               // +0x18
    float m_oneOverLength;              // +0x1C
    float m_oneOverNumWaves;            // +0x20 (RenderWaterObjects: the alpha)
    unsigned int m_numWaves;            // +0x24, animation frames
    unsigned int m_currentWave;         // +0x28
    void *m_surface;                    // +0x2C
    void *m_nextSurface;                // +0x30
    char pad34[0x40 - 0x34];
    _D3DVECTOR m_position;              // +0x40
    _D3DVECTOR m_direction;             // +0x50
    sceGifTag m_gifTag;                 // +0x60
    char pad70[0xB0 - 0x70];
    float m_xB0;                        // +0xB0
    float m_xB4;                        // +0xB4
    float m_xB8;                        // +0xB8
    float m_xBC;                        // +0xBC
    float m_xC0;                        // +0xC0
    float m_xC4;                        // +0xC4
    float m_xC8;                        // +0xC8
    float m_xCC;                        // +0xCC
    int m_xD0;                          // +0xD0
    int m_xD4;                          // +0xD4
    int m_mode;                         // +0xD8
    unsigned int m_wave;                // +0xDC
    unsigned int m_nextWave;            // +0xE0
    char padE4[0xF0 - 0xE4];
    _D3DVECTOR m_scale;                 // +0xF0
    _D3DVECTOR *m_corners[4];           // +0x100
    int m_x110;                         // +0x110
    int m_x114;                         // +0x114
    WaveStatePacket *m_statePackets;    // +0x118
    WaveGeometryPacket *m_geometryPackets;  // +0x11C
};

#endif
