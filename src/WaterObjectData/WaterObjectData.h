// WaterObjectData: the shared data of a WaterObject (a BaseObject).
// 0x30 bytes (`new` in WaterObject::Create). Names are the most common
// among the copies; +0x28 is the strip groups (some copies call it materials).
#ifndef WATEROBJECTDATA_WATEROBJECTDATA_H
#define WATEROBJECTDATA_WATEROBJECTDATA_H

#include "GameObject/GameObject.h"

class SoultreeMaterial;
struct WaterStripGroup;

class WaterObjectData : public BaseObject {
public:
    WaterObjectData();
    void ReadFromBinaryFile(long handle, int a);
    void ReadMaterialsFromBinaryFile(long handle, int a);

    // --- virtuals in retail slot order ---
    virtual ~WaterObjectData();

    SoultreeMaterial **m_materials;     // +0x10
    int m_numMaterials;                 // +0x14
    int m_18;                           // +0x18
    int m_numGroups;                    // +0x1C
    float *m_20;                        // +0x20
    float m_24;                         // +0x24
    WaterStripGroup *m_groups;          // +0x28
};

#endif
