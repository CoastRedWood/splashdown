// EcoSystem: the vegetation placed over a Terrain (2D sprites, 3D models and
// 2D/3D mixes, rendered through VIF items). A SearchableGameObject (GameObject
// + SearchableObject at +0x30), data from +0x40; 0x2200 bytes.
// Names follow EcoSystem's own methods (src/EcoSystem/), otherwise the most
// common name among the files that declared it locally.
#ifndef ECOSYSTEM_ECOSYSTEM_H
#define ECOSYSTEM_ECOSYSTEM_H

#include "GameObject/GameObject.h"
#include "SearchableObject/SearchableObject.h"
#include "d3dtypes.h"

class ParameterBlock;
class PSXSurfaceGeometry;
class PSXSurfaceShaderInstance;
class Terrain;
class Vegetation;
struct VegetationDescriptor;
struct VifDataItem;

// EcoSystem's base; no header of its own (src/SearchableGameObject has only
// its ctor and dtor). Declared the same way in Water/Water.h.
#ifndef SEARCHABLEGAMEOBJECT_DEFINED
#define SEARCHABLEGAMEOBJECT_DEFINED
class SearchableGameObject : public GameObject, public SearchableObject {
public:
    SearchableGameObject(int enabled);
    virtual ~SearchableGameObject() {}   // inline: derived dtors inline it (retail's weak copy is at 0x2289F0)
};
#endif

// m_slots' element (EcoSystem's ctor zeroes the first two words of each).
struct VegetationSlot {
    int m_0;
    int m_4;
    int m_8;
};

class EcoSystem : public SearchableGameObject {
public:
    EcoSystem(int enabled);
    EcoSystem *Create(Terrain *terrain, char *name, int format, int collision, int, float a, float b, int c);
    int CreateAuthoredEcoSystem();
    int GenerateEcoSystem(int singleTile);
    int WriteBinaryEcoSystemFile(char *name);
    int GetEcosystemParameters(char *fileName);
    int ReadBinaryEcoSystemFile(char *name, long *outFile);
    int ReCreateEcoSystemFromFile(long file, int placeOnTerrain);
    void Calculate2DParameters();
    void AddToBufferOf2DVegetation(Vegetation *vegetation);
    void AddToBufferOf3DVegetation(Vegetation *vegetation);
    void AddToBufferOf2D3DVegetation(Vegetation *vegetation);
    void *Create2DTransformRenderItem();
    void CreateVegetationCollisionObjects(VegetationDescriptor *descriptor);
    void ReadVegetationCollisionDescriptors(ParameterBlock *block, int index, VegetationDescriptor *descriptor);
    void Render2DVegetation(Vegetation **vegetation, int count, PSXSurfaceGeometry *geometry,
                            VifDataItem *start, VifDataItem *end);
    void Render3DVegetation(Vegetation **vegetation, int count);

    // --- virtuals in retail slot order ---
    virtual ~EcoSystem();
    virtual int PrepareGeometry();
    virtual int Render3D();
    virtual int KeyDownHandler(ProcessNode *node, KeyInfo *key);

    char pad38[0x40 - 0x38];
    _D3DVECTOR m_x40;                   // +0x40
    int m_authored;                     // +0x50
    int m_maxVegetation;                // +0x54
    Vegetation *m_vegetation;           // +0x58
    Vegetation **m_visible2D;           // +0x5C
    Vegetation **m_visible3D;           // +0x60
    Vegetation **m_visible2D3D;         // +0x64
    Terrain *m_terrain;                 // +0x68
    VegetationDescriptor *m_descriptors[256]; // +0x6C
    float m_northAngle;                 // +0x46C
    char m_placementBmp[0x80];          // +0x470
    char m_probabilityTga[0x80];        // +0x4F0
    int m_numVegetation;                // +0x570
    int m_x574;                         // +0x574 (0x22B from the ctor)
    float m_x578;                       // +0x578
    float m_x57C;                       // +0x57C
    unsigned char m_typeID;             // +0x580
    char pad581[0x584 - 0x581];
    int m_numVisible2D;                 // +0x584
    int m_numVisible3D;                 // +0x588
    int m_numVisible2D3D;               // +0x58C
    float m_positionScale;              // +0x590
    float m_x594;                       // +0x594
    int m_maxVisible2D;                 // +0x598
    int m_maxVisible3D;                 // +0x59C
    int m_maxVisible2D3D;               // +0x5A0
    float m_depthScale;                 // +0x5A4, 65535 / far clip (PrepareGeometry)
    float m_depthScaleInv;              // +0x5A8, far clip / 65535
    int m_numDescriptors;               // +0x5AC
    unsigned char *m_typeRemap;         // +0x5B0
    int m_x5B4;                         // +0x5B4
    VegetationSlot m_slots[600];        // +0x5B8
    int m_x21D8[2];                     // +0x21D8
    VifDataItem **m_vifItems;           // +0x21E0
    VifDataItem *m_2DItem;              // +0x21E4
    VifDataItem *m_2DTweakItem;         // +0x21E8
    PSXSurfaceGeometry *m_2DGeometry;   // +0x21EC
    PSXSurfaceGeometry *m_2D3DGeometry; // +0x21F0
    PSXSurfaceShaderInstance *m_2DShader; // +0x21F4
    int m_maxVifItems;                  // +0x21F8
};

#endif
