#ifndef VEGETATIONDESCRIPTOR_H
#define VEGETATIONDESCRIPTOR_H

extern "C" int rand();
extern "C" void free(void *p);

#include "DmaRenderItem/DmaRenderItem.h"

#include "GameObject/GameObject.h"

class SoultreeObjectBase {     // first 8 bytes of a SoultreeObject
public:
    int m_x00;
    int m_x04;
};

#include "SoultreeObject/SoultreeObject.h"
#include "PSXRenderableObject/PSXRenderableObject.h"

extern int EcoSystemTextureCount;

class CollisionObject;
class PSXRenderableObject;
class PSXSurfaceShaderInstance;
class SurfaceGeometry;
struct RenderableObjectLOD;
struct VegetationCollisionDescriptor;
struct VegetationVertex;

// VegetationDescriptor: one plant type of an EcoSystem (no vtable), 0x250
// bytes (`new` in EcoSystem). Names are the parameter-file fields its
// loaders read (GetEcosystemParameters, Load).

class VegetationDescriptor {
public:
    VegetationDescriptor();
    ~VegetationDescriptor();
    float GetHeight(char code);
    float GetRadius(char code);
    char GenerateHeight();
    int GenerateRadius(float scale);
    int Load(int index);

    char m_name[0x80];                  // +0x000
    char m_billboard[0x80];             // +0x080
    char m_probabilityTga[0x80];        // +0x100
    float m_meanHeight;                 // +0x180
    float m_minHeight;                  // +0x184
    float m_maxHeight;                  // +0x188
    float m_meanRadius;                 // +0x18C
    float m_minRadius;                  // +0x190
    float m_maxRadius;                  // +0x194
    float m_meanSlope;                  // +0x198
    float m_sdSlope;                    // +0x19C
    float m_meanAspect;                 // +0x1A0
    float m_sdAspect;                   // +0x1A4
    float m_meanDrainage;               // +0x1A8
    float m_sdDrainage;                 // +0x1AC
    float m_meanAltitude;               // +0x1B0
    float m_sdAltitude;                 // +0x1B4
    float m_uLeft;                      // +0x1B8
    float m_uRight;                     // +0x1BC
    float m_vTop;                       // +0x1C0
    float m_vBottom;                    // +0x1C4
    float m_uCenter;                    // +0x1C8
    float m_heightScale;                // +0x1CC
    float m_radiusScale;                // +0x1D0
    float m_x1D4;                       // +0x1D4
    int m_x1D8;                         // +0x1D8
    unsigned int m_keyColor;            // +0x1DC
    char pad1E0[0x1E4 - 0x1E0];
    VegetationVertex *m_vertices;       // +0x1E4
    int m_numVertices;                  // +0x1E8
    char pad1EC[0x1F0 - 0x1EC];
    int m_planarLighting;               // +0x1F0
    int m_blendLODs;                    // +0x1F4
    float m_percent;                    // +0x1F8
    int m_numCollisionObjects;          // +0x1FC
    VegetationCollisionDescriptor *m_collisionObjects; // +0x200
    CollisionObject **m_collision;      // +0x204
    char pad208[0x210 - 0x208];
    DmaRenderItem m_renderItem;         // +0x210
    PSXRenderableObject *m_object;      // +0x240 (Load)
    RenderableObjectLOD *m_lods;        // +0x244, m_object's data's levels (Load)
    PSXSurfaceShaderInstance *m_shader; // +0x248
    SurfaceGeometry *m_geometry;        // +0x24C
};

#endif
