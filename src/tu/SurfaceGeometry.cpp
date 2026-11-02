// SurfaceGeometry.cpp  @ 0x0034B050 (text 0x34B050-0x34B3D0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's SurfaceGeometry.cpp as one translation unit, a TU without an
// initializer inside PSXSurfaceMaterial.cpp's tu_map row: SurfaceGeometry.
// Its .rodata (0x4368C0-0x436938, labels restarting at @411) is the ASSERT
// strings and the SurfaceGeometry RTTI. One block ASSERT form
// (soultree_assert_block.h), so no alignlabel pragma.



#include "SurfaceGeometry/SurfaceGeometry.h"
#include "MaterialStringTable/MaterialStringTable.h"
#include "soultree_assert_block.h"
#include "StringTable/StringTable.h"
#include "DataAccess/DataAccess.h"

extern "C" char *strcpy(char *dst, const char *src);
extern "C" char *strcat(char *dst, const char *src);
extern "C" unsigned int strlen(const char *s);

class InstanceMaster;
extern char SltTextureContentDirectory[];
extern DataAccess *LpDataAccess;
// Absolute load, so declared const.
extern MaterialStringTable *const LpMaterialStringTable;

// ---- src/SurfaceGeometry/ctor.cpp

SurfaceGeometry::SurfaceGeometry()
{
    m_materialIndex = -1;
}


// ---- src/SurfaceGeometry/dtor.cpp


SurfaceGeometry::~SurfaceGeometry()
{
    LpMaterialStringTable->RevokeRequest(m_materialIndex);
}


// ---- src/SurfaceGeometry/ReadFromFile.cpp


int SurfaceGeometry::ReadFromFile(long file, InstanceMaster *, int)
{
    char name[0x40];
    char path[260];
    LpDataAccess->FRead(file, name, 0x40, 1);
    if (SltTextureContentDirectory[0]) {
        strcpy(path, SltTextureContentDirectory);
        if (SltTextureContentDirectory[strlen(SltTextureContentDirectory) - 1] != '\\')
            strcat(path, "\\");
        strcat(path, name);
    } else {
        strcpy(path, name);
    }
    m_materialIndex = LpMaterialStringTable->GetIndex(path);
#line 51 "SurfaceGeometry.cpp"
    ASSERTF(m_materialIndex != -1, ("%s is not in the material table", path));
    LpMaterialStringTable->Request(m_materialIndex);
    return 1;
}


// ---- src/SurfaceGeometry/Create.cpp


int SurfaceGeometry::Create(char *name, unsigned char flags)
{
    char path[260];
    if (SltTextureContentDirectory[0]) {
        strcpy(path, SltTextureContentDirectory);
        if (SltTextureContentDirectory[strlen(SltTextureContentDirectory) - 1] != '\\')
            strcat(path, "\\");
        strcat(path, name);
    } else {
        strcpy(path, name);
    }
    m_materialIndex = LpMaterialStringTable->GetIndex(path);
#line 80 "SurfaceGeometry.cpp"
    ASSERTF(m_materialIndex != -1, ("%s is not in the material table", path));
    LpMaterialStringTable->Request(m_materialIndex);
    return 1;
}


// ---- src/SurfaceGeometry/ChangeMaterial.cpp


int SurfaceGeometry::ChangeMaterial(int oldMaterial, int newMaterial)
{
    if (m_materialIndex == oldMaterial) {
        m_materialIndex = newMaterial;
        LpMaterialStringTable->Request(newMaterial);
        LpMaterialStringTable->RevokeRequest(oldMaterial);
    }
    return m_materialIndex;
}

