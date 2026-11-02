#ifndef SURFACEGEOMETRY_H
#define SURFACEGEOMETRY_H

class InstanceMaster;

// One piece of geometry drawn with one material (index into
// LpMaterialStringTable). 8 bytes: the index, then the vptr.
class SurfaceGeometry {
public:
    SurfaceGeometry();
    int ReadFromFile(long file, InstanceMaster *master, int instance);

    unsigned int m_materialIndex;       // +0x00 (-1 = none; unsigned: the conversion to GetPointer's int orders the loads like retail)

    // --- virtuals in retail slot order ---
    virtual ~SurfaceGeometry();         // vptr +0x04
    virtual int Create(char *name, unsigned char flags);
    virtual int ChangeMaterial(int oldMaterial, int newMaterial);
};

#endif
