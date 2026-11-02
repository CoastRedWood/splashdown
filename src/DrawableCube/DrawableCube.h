// DrawableCube: a textured cube (sky box and similar), six faces of 16
// panels that can be animated. A SearchableGameObject (GameObject +
// SearchableObject at +0x30), data from +0x38; 0xAE8 bytes.
// Names follow DrawableCube's own methods (src/DrawableCube/), otherwise the
// most common name among the files that declared it locally.
#ifndef DRAWABLECUBE_DRAWABLECUBE_H
#define DRAWABLECUBE_DRAWABLECUBE_H

#include "GameObject/GameObject.h"
#include "SearchableObject/SearchableObject.h"

struct _D3DVECTOR;
struct _D3DLVERTEX;
class Camera;
class ColorMapper;
class DmaRenderItem;
class SearchPlacement;
class TextureMap;
struct Cube;
struct PanelSequence;
struct VifDataItem;

// DrawableCube's base; no header of its own (src/SearchableGameObject has
// only its ctor and dtor). Declared the same way in Water/Water.h.
#ifndef SEARCHABLEGAMEOBJECT_DEFINED
#define SEARCHABLEGAMEOBJECT_DEFINED
class SearchableGameObject : public GameObject, public SearchableObject {
public:
    SearchableGameObject(int enabled);
    virtual ~SearchableGameObject() {}   // inline: derived dtors inline it (retail's weak copy is at 0x2289F0)
};
#endif

// One face panel (m_panels' element, CreateAnimatedPanels). 0x14 bytes.
struct AnimatedPanel {
    int m_mapping;                      // +0x00, panel id (0 = none)
    int m_numSequences;                 // +0x04
    int m_sequences[3];                 // +0x08
};

class DrawableCube : public SearchableGameObject {
public:
    DrawableCube(int enabled);
    DrawableCube *Create(char *name);
    int CreateGeometry();
    int LoadCube(Cube *cube);
    int FaceVisible(Camera *camera, _D3DVECTOR *normal, _D3DVECTOR *point);
    TextureMap *GetTextureMap(int face, int layer);
    VifDataItem *CreateGeneralDataItem();
    VifDataItem *CreateRestoreDataItem();
    VifDataItem *CreateUniqueDataItem();
    void UpdateUniqueDataItem(VifDataItem *item, TextureMap *texture);
    void BuildRenderList(DmaRenderItem *item, _D3DLVERTEX *verts, int fog, int face);
    void BuildRenderListCPU(DmaRenderItem *item, _D3DLVERTEX *verts, int fog, int face);
    void CreateAnimatedPanels(char *name);
    static void EnableCubeZWrite(int enable);
    static void EnableVUSubdivision(int enable);
    void CalculateVisibility();

    // --- virtuals in retail slot order ---
    virtual ~DrawableCube();
    virtual int Tick(float dt);
    virtual int PrepareGeometry();
    virtual int Render3D();
    virtual int KeyDownHandler(ProcessNode *node, KeyInfo *key);

    Cube *m_cube;                       // +0x38, current
    Cube **m_cubes;                     // +0x3C
    int m_currentCube;                  // +0x40
    int m_animation;                    // +0x44
    int m_numSequences;                 // +0x48
    PanelSequence *m_sequences;         // +0x4C
    AnimatedPanel m_panels[6][16];      // +0x50
    ColorMapper *m_colorMapper;         // +0x7D0
    SearchPlacement *m_searchPlacement; // +0x7D4
    int m_numCubes;                     // +0x7D8
    char pad7DC[0x7E0 - 0x7DC];
    VifDataItem *m_generalItem;         // +0x7E0
    VifDataItem *m_restoreItem;         // +0x7E4
    DmaRenderItem *m_renderItems[96];   // +0x7E8, one per panel
    VifDataItem *m_uniqueItems[96];     // +0x968, one per panel
};

#endif
