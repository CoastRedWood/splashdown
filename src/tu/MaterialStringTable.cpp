// MaterialStringTable.cpp  @ 0x00396A00 (text 0x396A00-0x396DA0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's MaterialStringTable.cpp as one translation unit: its functions in
// retail order, then its static initializer. `make link` places the object's
// data as well as its code (tools/link_c.py).



#include "StringTable/StringTable.h"
#include "MaterialStringTable/MaterialStringTable.h"
#include "PSXShaderDescriptor/PSXShaderDescriptor.h"
#include "PSXSurfaceShaderList/PSXSurfaceShaderList.h"
#include "PSXSurfaceMaterial/PSXSurfaceMaterial.h"
#include "SurfaceMaterial/SurfaceMaterial.h"

// ---- src/MaterialStringTable/ctor.cpp

MaterialStringTable::MaterialStringTable()
{
}


// ---- src/MaterialStringTable/dtor.cpp


MaterialStringTable::~MaterialStringTable()
{
    ReleaseMaterials();
}


// ---- src/MaterialStringTable/ReleaseMaterials.cpp

void MaterialStringTable::ReleaseMaterials()
{
    for (int i = 0; i < m_totalEntries; i++) {
        SurfaceMaterial *material = (SurfaceMaterial *)GetPointer(i);
        if (material) {
            delete material;
            SetPointer(i, 0);
            m_entries[i].requested = 0;
        }
    }
}


// ---- src/MaterialStringTable/LoadRequested.cpp

extern PSXSurfaceShaderList *LpSurfaceShaderList;


// Loads every requested material and (re)builds its shader instances.
void MaterialStringTable::LoadRequested()
{
    int i;
    PSXSurfaceMaterial *created;
    int j;
    SurfaceMaterial *material;
    if (LpSurfaceShaderList) {
        LpSurfaceShaderList->Purge();
        for (i = 0; i < m_totalEntries; i++) {
            material = (SurfaceMaterial *)GetPointer(i);
            if (material) {
                for (j = 0; j < material->m_numShaders; j++)
                    material->m_instances[j] = 0;
            }
        }
    }
    for (i = 0; i < m_totalEntries; i++) {
        if (m_entries[i].requested > 0 && !GetPointer(i)) {
            created = new PSXSurfaceMaterial;
            created->ReadFromFile(m_entries[i].string);
            SetPointer(i, created);
        }
        if ((material = (SurfaceMaterial *)GetPointer(i)) != 0) {
            for (j = 0; j < material->m_numShaders; j++)
                material->m_instances[j] = LpSurfaceShaderList->AddShaderInstance(material->m_shaders[j], -1);
        }
    }
}


// ---- src/MaterialStringTable/RevokeRequest.cpp


// Once nothing requests a material any more, free it.
void MaterialStringTable::RevokeRequest(int index)
{
    StringTable::RevokeRequest(index);
    if (m_entries[index].requested == 0) {
        SurfaceMaterial *material = (SurfaceMaterial *)GetPointer(index);
        if (material) {
            delete material;
            SetPointer(index, 0);
        }
    }
}


// ---- src/sinit/MaterialStringTable.cpp
// Static initializer of MaterialStringTable.cpp: the global material name table.

MaterialStringTable GMaterialStringTable;
MaterialStringTable *LpMaterialStringTable = &GMaterialStringTable;

