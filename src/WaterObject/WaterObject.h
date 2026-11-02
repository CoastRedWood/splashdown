// WaterObject: a static water surface model (a SoultreeObject, GameObject
// at +8). 0x1D0 bytes (`new` in PSXWater::LoadWaterFromParameterBlockFile);
// its own data starts at SoultreeObject's 0x1B0. PrepareGeometry/Render3D
// override GameObject's (the second base), so they also get primary slots
// (39, 40) after SoultreeObject's own.
#ifndef WATEROBJECT_WATEROBJECT_H
#define WATEROBJECT_WATEROBJECT_H

#include "SoultreeObject/SoultreeObject.h"

class DmaRenderItem;
class PSXTextureMap;
class WaterObjectData;
struct _D3DINTVECTOR;

class WaterObject : public SoultreeObject {
public:
    WaterObject(int id);
    void BuildRenderLists();
    void BuildUniqueScissorItem(DmaRenderItem *item, int a, _D3DINTVECTOR *color, int b, _D3DVECTOR *c,
                                _D3DVECTOR *d, float e, float f, int g, int h, _D3DVECTOR *i);
    void CalculateMaterialTexelSizes();
    GameObject *Create(char *name, int a, int b);   // returns the GameObject subobject
    int Render3D(int blendMode, _D3DINTVECTOR *color, PSXTextureMap *texture, _D3DVECTOR *a, _D3DVECTOR *b,
                 float c, float d, int e, int f, float g, _D3DVECTOR *h);

    // --- virtuals in retail slot order ---
    virtual ~WaterObject();
    virtual void CalculateBoundingBox();
    virtual int PrepareGeometry();
    virtual int Render3D();

    int m_material;                     // +0x1B0
    WaterObjectData *m_data;            // +0x1B4
    int m_x1B8;                         // +0x1B8
    DmaRenderItem *m_renderItems;       // +0x1BC
    int m_numRenderItems;               // +0x1C0
    char pad1C4[0x1D0 - 0x1C4];
};

#endif
