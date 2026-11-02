// SurfaceMaterial: a material's shader passes and their instances in the
// SurfaceShaderList (0x6C bytes; the vptr comes last). Read from the .psm
// parameter file named after the material.
#ifndef SURFACEMATERIAL_H
#define SURFACEMATERIAL_H

class ShaderDescriptor;

class SurfaceMaterial {
public:
    SurfaceMaterial();
    int ReadFromFile(char *name);

    char m_name[0x40];                  // +0x00 (the .psm path)
    unsigned int m_index;               // +0x40 (LpMaterialStringTable index, -1; unsigned: ReadFromFile's load order)
    int m_numShaders;                   // +0x44
    ShaderDescriptor *m_shaders[4];     // +0x48 (one per pass, PSXShaderDescriptors; the dtor deletes them)
    unsigned int m_instances[4];        // +0x58 (SurfaceShaderList::AddShaderInstance indices, -1)

    // --- virtuals in retail slot order ---
    virtual ~SurfaceMaterial();         // vptr +0x68
};

#endif
