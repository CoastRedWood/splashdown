// MaterialStringTable: the material names, each entry's pointer a
// PSXSurfaceMaterial loaded on request (0x20 bytes, no data of its own).
#ifndef MATERIALSTRINGTABLE_H
#define MATERIALSTRINGTABLE_H

#include "StringTable/StringTable.h"

class MaterialStringTable : public StringTable {
public:
    MaterialStringTable();
    void LoadRequested();
    void ReleaseMaterials();
    void RevokeRequest(int index);  // frees the material once nothing requests it

    // --- virtuals in retail slot order ---
    virtual ~MaterialStringTable();
};

#endif
