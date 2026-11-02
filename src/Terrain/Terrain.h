// Terrain: the heightfield GameObject (a tree of DrawableGridNodes over a
// 17x17-vertex grid per node). 0xE60 bytes (`new Terrain` in
// JetskiCompetition::LoadTerrain): GameObject, GroundFogableObject at +0x30,
// data from +0x40; its virtuals are all GameObject's. Names follow Terrain's own methods (src/Terrain/), then the
// DrawableGridNode files (which reach it through their +0x60 pointer), then
// the most common name among the local copies.
#ifndef TERRAIN_TERRAIN_H
#define TERRAIN_TERRAIN_H

#include "GameObject/GameObject.h"
#include "d3dtypes.h"

class AgeManager;
class Camera;
class DrawableGridNode;
class GridNodeGeometryBase;
class TextureMap;
class PSXTextureMap;
class VifDataItem;
struct _sceDmaTag;
struct FILE_HANDLE_STRUCT;

// Terrain's second base (+0x30, 0x10 bytes as Terrain's ctor.cpp has it);
// its ctor is out of line (src/GroundFogableObject/ctor_00241BD0.cpp).
// Declared here because no shared header exists for it yet.
#ifndef GROUNDFOGABLEOBJECT_DEFINED
#define GROUNDFOGABLEOBJECT_DEFINED
class GroundFogableObject {
public:
    GroundFogableObject();
    int m_groundFog;                    // +0x0
    char pad4[0x10 - 0x4];
};
#endif

class Terrain : public GameObject, public GroundFogableObject {
public:
    Terrain(int enabled);
    int Create(char *name, int a, int b, char *textureFolder, int c, int detailX, int detailZ);
    int GetGridPointIntersection(_D3DVECTOR *point, _D3DVECTOR *normal, int flags, unsigned char *material);
    void GetMinMaxHeight(float *minHeight, float *maxHeight);
    void SetRenderTolerance(int tolerance);
    void SetGraphicQuality(int quality);
    void ConvertToPlain();
    void CalculateKappa();
    int ExportTerrain(char *filename);
    void WriteTexturesToFile(FILE_HANDLE_STRUCT *file, int *offsets);
    VifDataItem *CreateCommonDataItem();
    VifDataItem *CreateScissorDataItem(int alpha);

    // --- virtuals in retail slot order ---
    virtual ~Terrain();
    virtual int PrepareGeometry();
    virtual int Render3D();
    virtual int EventHandler(int event);
    virtual int KeyUpHandler(ProcessNode *node, KeyInfo *key);
    virtual int KeyDownHandler(ProcessNode *node, KeyInfo *key);

    _D3DVECTOR m_x40;                   // +0x40
    _D3DVECTOR m_x50;                   // +0x50
    _D3DVECTOR m_cameraPosition;        // +0x60
    _D3DVECTOR m_cameraGrid;            // +0x70, camera position in grid units
    _D3DVECTOR m_x80;                   // +0x80
    char pad90[0xAC - 0x90];
    BaseObject *m_xAC;                  // +0xAC, released by the dtor
    int m_xB0;                          // +0xB0, texture load flag (Create)
    char padB4[0xB8 - 0xB4];
    long m_file;                        // +0xB8 (64-bit), the .trn file while loading; PrepareGeometry
                                        // passes it to SelectVisibleBlocks as the frame
    float m_gridScale;                  // +0xC0 (1.0 from the ctor)
    DrawableGridNode *m_root;           // +0xC4
    int m_xC8;                          // +0xC8
    int m_xCC;                          // +0xCC
    float m_xD0;                        // +0xD0 (CalculateKappa)
    float m_threshold;                  // +0xD4 (SetRenderTolerance)
    float m_xD8;                        // +0xD8, 1 / m_threshold
    float m_xDC;                        // +0xDC, 1 / m_xD0
    int m_checkDeltas;                  // +0xE0
    int m_xE4;                          // +0xE4
    int m_xE8;                          // +0xE8
    int m_evaluations;                  // +0xEC
    int m_xF0;                          // +0xF0
    int m_xF4;                          // +0xF4
    int m_vertexNotifications;          // +0xF8
    int m_parentNotifications;          // +0xFC
    int m_x100;                         // +0x100
    int m_x104;                         // +0x104, vertices switched on
    int m_x108;                         // +0x108, vertices switched off
    int m_vertexEvaluations;            // +0x10C
    int m_frameCount;                   // +0x110
    int m_x114[17 * 17];                // +0x114
    int m_x598;                         // +0x598, evaluation counter
    int m_numTextures;                  // +0x59C
    PSXTextureMap *m_textures[0x1A9];   // +0x5A0, also the detail textures (Create news PSXTextureMaps)
    int m_isPlain;                      // +0xC44, 1 = flat mode (ConvertToPlain)
    int m_renderTolerance;              // +0xC48 (1000 from the ctor)
    int m_quality;                      // +0xC4C
    int m_dirty;                        // +0xC50
    int m_xC54;                         // +0xC54
    int m_numGeometries;                // +0xC58
    GridNodeGeometryBase **m_geometries;    // +0xC5C
    float m_xC60;                       // +0xC60 (1.0 from the ctor)
    int m_xC64;                         // +0xC64
    TextureMap *m_detailTexture;        // +0xC68, released by the dtor
    char padC6C[0xC70 - 0xC6C];
    _D3DMATRIX m_xC70;                  // +0xC70, lighting matrix
    AgeManager *m_ageManager;           // +0xCB0, texture cache ages
    AgeManager *m_meshAgeManager;       // +0xCB4
    Camera *m_camera;                   // +0xCB8
    unsigned int m_detailColor;         // +0xCBC, the detail texture's color
    _D3DBLEND m_detailSrcBlend;         // +0xCC0
    _D3DBLEND m_detailDestBlend;        // +0xCC4
    unsigned int m_detailInverse;       // +0xCC8
    unsigned int m_detailModulate;      // +0xCCC
    int m_lodDistance;                  // +0xCD0
    int m_xCD4;                         // +0xCD4
    int m_xCD8;                         // +0xCD8
    int m_xCDC;                         // +0xCDC
    int m_xCE0;                         // +0xCE0
    int m_xCE4;                         // +0xCE4
    int m_meshesBuilt;                  // +0xCE8
    int m_numVisibleNodes;              // +0xCEC
    DrawableGridNode **m_visible;       // +0xCF0, visible-node list (malloc 0x3E80 in ConvertToPlain)
    char m_filename[0x104];             // +0xCF4
    int m_debugPage;                    // +0xDF8 (-1 from the ctor)
    int m_xDFC;                         // +0xDFC
    int m_halfRes;                      // +0xE00
    int m_xE04;                         // +0xE04
    _sceDmaTag *m_restoreScissor;       // +0xE08, the scissor-restore DMA chain (Render3D builds it as QWORDs)
    VifDataItem *m_scissorItems[8];     // +0xE0C
    VifDataItem *m_scissorItems2[8];    // +0xE2C
    BaseObject **m_xE4C;                // +0xE4C, released and freed by the dtor
    int m_xE50;                         // +0xE50
    int m_xE54;                         // +0xE54
    int m_xE58;                         // +0xE58
    int m_xE5C;                         // +0xE5C
};

#endif
