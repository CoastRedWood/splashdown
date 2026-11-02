// LightManager: the scene's light list and per-node ambient/light colours
// (the global LpLightManager). A GameObject, data from +0x30, 0xB0 bytes.
// Names follow LightManager's own methods (src/LightManager/), otherwise
// the most common name among the local copies.
//
// +0x50 (the ambient colour) and +0x70 (the saved ambient colour) are
// quadwords held by value: copied as one lq/sq (ChangeAmbientColor,
// RestoreAmbientColor, CreateLightGraph) and read as four floats (Tick,
// KeyDownHandler, AddLight, LightVertices). QWORD/QWORD.h can't be included
// here (two TUs that include this header keep their own unaligned QWORD for
// GetThreeLights' buffers), so they are anonymous unions of the same two
// views. +0x60/+0x80 are one colour per light-graph node.
#ifndef LIGHTMANAGER_LIGHTMANAGER_H
#define LIGHTMANAGER_LIGHTMANAGER_H

#include "GameObject/GameObject.h"
#include "d3dtypes.h"

typedef int int128_t __attribute__((mode(TI)));   // EE 128-bit register (lq/sq)

class LightEmitter;
class LightSearchCache;
class SearchList;
class SearchNode;
class SearchPlacement;
struct QWORD;
struct _D3DLVERTEX;
#ifndef LIGHTEMITTERTYPE_DEFINED
#define LIGHTEMITTERTYPE_DEFINED
enum LightEmitterType {};      // also in LightEmitter.h / LightManager.h
#endif

class LightManager : public GameObject {
public:
    LightManager();
    void AddLight(LightEmitter *light, int index);
    void RemoveLight(LightEmitter *light);
    LightEmitter *FindLight(LightEmitterType type);
    void CreateLightGraph(int numNodes);
    void ReleaseLightGraph();
    void LightingChanged();
    void ChangeAmbientColor(QWORD &color);
    void RestoreAmbientColor();
    QWORD BlendedAmbientColor(SearchPlacement *placement, _D3DVECTOR *position, float radius);
    void GetDefaultLights(QWORD *directions, QWORD *colors, QWORD *speculars);
    void SimpleLightSearch(int ambient, _D3DVECTOR point, QWORD *directions, QWORD *colors, QWORD *speculars);
    void GetThreeLights(SearchPlacement *placement, _D3DVECTOR position, float maxDistance, QWORD *light0,
                        QWORD *light1, QWORD *ambient);
    void LightVertices(_D3DMATRIX *m, int count, _D3DVECTOR *normals, _D3DLVERTEX *vertices, int stride);

    // --- virtuals in retail slot order ---
    virtual ~LightManager();
    virtual LightManager *Create();
    virtual int Tick(float dt);
    virtual int KeyDownHandler(ProcessNode *node, KeyInfo *key);

    _D3DVECTOR m_x30;                   // +0x30, ZeroVector from the ctor
    LightEmitter *m_sun;                // +0x40, the directional light (AddLight)
    char pad44[0x50 - 0x44];
    union {                             // +0x50 (see above)
        int128_t m_ambientValue;
        float m_ambientColor[4];
    };
    QWORD *m_lightColors;               // +0x60, one per light-graph node
    char pad64[0x70 - 0x64];
    union {                             // +0x70
        int128_t m_savedAmbientValue;
        float m_savedAmbientColor[4];
    };
    QWORD *m_savedLightColors;          // +0x80
    int m_lightingVersion;              // +0x84 (LightingChanged)
    SearchNode **m_nodes;               // +0x88, light-graph nodes
    SearchList **m_lists;               // +0x8C
    int m_numLightColors;               // +0x90, light-graph node count
    int m_numLights;                    // +0x94
    int m_maxLights;                    // +0x98
    LightEmitter **m_lights;            // +0x9C (malloc'd)
    int m_haveType2;                    // +0xA0, a type-2 light was added
    LightSearchCache *m_searchCache;    // +0xA4 (deleted by the dtor)
    int m_overlayPage;                  // +0xA8, -1 from the ctor
};

extern LightManager *LpLightManager;

#endif
