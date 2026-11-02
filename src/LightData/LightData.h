// LightData: one scene light as the scene file's [LightN] blocks describe it
// (0x100 bytes; Scene::m_lights, `new LightData[n]` in Scene::ReadSceneData,
// filled by Scene::ReadLightData). The ctor only constructs the three vectors.
#ifndef LIGHTDATA_LIGHTDATA_H
#define LIGHTDATA_LIGHTDATA_H

#include "d3dtypes.h"

#ifndef LIGHTEMITTERTYPE_DEFINED
#define LIGHTEMITTERTYPE_DEFINED
enum LightEmitterType {};      // also in LightEmitter.h / LightManager.h
#endif

class LightEmitter;

struct LightData {
    LightData();

    _D3DVECTOR m_position;              // +0x00
    _D3DVECTOR m_infinitePosition;      // +0x10, PlaceLightAtInfinity
    _D3DVECTOR m_direction;             // +0x20
    _D3DVECTOR *m_pPosition;            // +0x30
    int m_initialZone;                  // +0x34
    _D3DVECTOR *m_pDirection;           // +0x38
    LightEmitter *m_emitter;            // +0x3C
    _D3DCOLORVALUE m_color;             // +0x40
    float m_range;                      // +0x50
    union {
        int m_type;                     // +0x54, a LightEmitterType (GetEnumerator fills it as an int)
        LightEmitterType m_emitterType; // +0x54 (JetskiUIManager::LoadScene passes it to LightEmitter::Create)
    };
    unsigned char m_lensFlareLayers[5]; // +0x58
    char pad5D[0x60 - 0x5D];
    int m_lensFlareColor;               // +0x60
    float m_lensFlareBrightness;        // +0x64
    char m_lensFlareTexture[0x40];      // +0x68
    int m_lightFlareColor;              // +0xA8
    float m_lightFlareSize;             // +0xAC
    char m_lightFlareTexture[0x40];     // +0xB0
    unsigned char m_showDebugSphere : 1;    // +0xF0 bit 0
    unsigned char m_atInfinity : 1;         // bit 1
    unsigned char m_emitsLight : 1;         // bit 2
    unsigned char m_castsShadows : 1;       // bit 3
    unsigned char m_hasLensFlare : 1;       // bit 4
    unsigned char m_hasLightFlare : 1;      // bit 5
    char padF1[0x100 - 0xF1];
};

#endif
