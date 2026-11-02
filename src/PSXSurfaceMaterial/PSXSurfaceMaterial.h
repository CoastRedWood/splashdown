// PSXSurfaceMaterial: queues geometry on its passes' shader instances
// (0x6C bytes, MaterialStringTable::LoadRequested news it).
#ifndef PSXSURFACEMATERIAL_H
#define PSXSURFACEMATERIAL_H

#include "SurfaceMaterial/SurfaceMaterial.h"

class SurfaceGeometry;
struct _sceDmaTag;

class PSXSurfaceMaterial : public SurfaceMaterial {
public:
    PSXSurfaceMaterial();
    void AddGeometry(SurfaceGeometry *geometry, _sceDmaTag *head, _sceDmaTag *tail, int single);
    void UpdateLOD(float *uvScales, float numTexCoords, float maxSize, float scale);

    // --- virtuals in retail slot order ---
    virtual ~PSXSurfaceMaterial();
};

#endif
