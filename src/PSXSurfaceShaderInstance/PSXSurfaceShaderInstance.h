// PSXSurfaceShaderInstance: sends its shader's microprogram, textures and
// render states, then its geometry (0x20 bytes).
#ifndef PSXSURFACESHADERINSTANCE_H
#define PSXSURFACESHADERINSTANCE_H

#include "SurfaceShaderInstance/SurfaceShaderInstance.h"

struct _sceDmaTag;

class PSXSurfaceShaderInstance : public SurfaceShaderInstance {
public:
    PSXSurfaceShaderInstance();
    void AddGeometry(SurfaceGeometry *geometry, _sceDmaTag *start, _sceDmaTag *end);
    void Render();

    // --- virtuals in retail slot order ---
    virtual ~PSXSurfaceShaderInstance();

    union {                             // +0x1C (the ctor clears the word)
        int m_x1C;
        struct {
            signed char m_flush : 1;
        } m_bits;
    };
};

#endif
