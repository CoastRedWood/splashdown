// PSXSurfaceShaderList: the PS2 list wraps the shaders in the VU1 preamble
// and a PRMODE restore item (0x4018 bytes).
#ifndef PSXSURFACESHADERLIST_H
#define PSXSURFACESHADERLIST_H

#include "SurfaceShaderList/SurfaceShaderList.h"

struct VifDataItem;

class PSXSurfaceShaderList : public SurfaceShaderList {
public:
    PSXSurfaceShaderList();
    int AddShaderInstance(ShaderDescriptor *descriptor, int index);
    void Render();

    // --- virtuals in retail slot order ---
    virtual ~PSXSurfaceShaderList();

    VifDataItem *m_preamble;                // +0x4010 (BuildPreambleRenderItem, memalign'd)
    VifDataItem *m_restorePRMODE;           // +0x4014 (BuildRestorePRMODERenderItem, memalign'd)
};

#endif
