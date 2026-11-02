// Water: the water surface (PSXWater derives from it). A SearchableGameObject
// (GameObject + SearchableObject at +0x30), data from +0x38. Slots 28-30 are
// pure in Water's vtable; their signatures are PSXWater's overrides.
// Names follow Water's and PSXWater's own methods, otherwise the most common
// name among the local copies. PSXWater's members start at +0xF0.
#ifndef WATER_WATER_H
#define WATER_WATER_H

#include "GameObject/GameObject.h"
#include "SearchableObject/SearchableObject.h"
#include "d3dtypes.h"

class LightManager;
class PSXSignatureWave;
class PSXSurfaceGeometry;
class PSXSurfaceShaderInstance;
class PSXTextureMap;
class PSXWaterDisplacement;
class SoultreeObject;
struct WaterWakePatch;

// Water's base; no header of its own yet (src/SearchableGameObject has only
// its ctor and dtor). Its ctor sets SearchableObject's m_typeID and bits.
#ifndef SEARCHABLEGAMEOBJECT_DEFINED
#define SEARCHABLEGAMEOBJECT_DEFINED
class SearchableGameObject : public GameObject, public SearchableObject {
public:
    SearchableGameObject(int enabled);
    virtual ~SearchableGameObject() {}   // inline: derived dtors inline it (retail's weak copy is at 0x2289F0)
};
#endif

class Water : public SearchableGameObject {
public:
    Water(int enabled);
    void GetGridPointIntersection(_D3DVECTOR *point, int *index);
    void ApplyExternalForce(float force, _D3DVECTOR position);
    void ResetWakes();
    void ResetWakePatch(WaterWakePatch *patch, _D3DVECTOR position, float scale);
    void ScrollWakePatch(int index, _D3DVECTOR position);
    void setWaterDisplacement(PSXWaterDisplacement *displacement);
    float GetBaseTime();
    void SetBaseTime(float time);

    // --- virtuals in retail slot order ---
    virtual ~Water();
    virtual int Tick(float dt);
    virtual Water *Create(char *name, LightManager *lights, int numWakePatches, int patchSize, float scale);
    virtual int LoadWaterFromParameterBlockFile(char *name) = 0;
    virtual int ReloadWaterFromParameterBlockFile() = 0;
    virtual int LoadWaterFromBinaryFile(char *filename) = 0;

    LightManager *m_lights;             // +0x38
    float m_baseHeight;                 // +0x3C
    WaterWakePatch *m_wakePatches;      // +0x40
    WaterWakePatch *m_firstPatch;       // +0x44
    WaterWakePatch *m_lastPatch;        // +0x48
    int m_numWakePatches;               // +0x4C
    GameObject *m_x50;                  // +0x50
    PSXTextureMap *m_texture;           // +0x54 (PSXWater::PrepareGeometry)
    void *m_wakeBits;                   // +0x58, the first patch's texture, locked
    unsigned char *m_baseMap;           // +0x5C, displacement bits
    float m_baseOpacity;                // +0x60
    int m_numPools;                     // +0x64
    SoultreeObject **m_objects;         // +0x68
    int m_numWaves;                     // +0x6C
    int m_x70[5];                       // +0x70
    float m_scrollRate[5];              // +0x84, per signature-wave mode (PSXWaterDisplacement::Tick)
    float m_weights[4];                 // +0x98, per mode (GetSignatureWaveHeight; PSXWater::Render3D)
    int m_wakePitch;                    // +0xA8, in pixels (bytes / 4)
    int m_baseMapSize;                  // +0xAC
    int m_patchWidth;                   // +0xB0
    int m_patchHeight;                  // +0xB4
    float m_wakeScroll;                 // +0xB8
    float m_baseMapScale;               // +0xBC
    float m_heightScale;                // +0xC0
    PSXWaterDisplacement *m_displacement;   // +0xC4
    PSXSignatureWave **m_waves;         // +0xC8
    int m_baseMapStride;                // +0xCC
    PSXSurfaceGeometry *m_geometry;     // +0xD0 (deleted virtually by ~PSXWater)
    PSXSurfaceShaderInstance *m_shader; // +0xD4
    int m_numTiles;                     // +0xD8
    int m_gridSize;                     // +0xDC
    int m_xE0;                          // +0xE0
    char padE4[0xF0 - 0xE4];            // PSXWater's first member is at +0xF0
};

#endif
