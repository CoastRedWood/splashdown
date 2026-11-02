// SkyCube.cpp  @ 0x00291270 (text 0x291270-0x291460)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp)
//
// Retail's SkyCube.cpp as one translation unit: a TU without a static
// initializer inside SelectiveGravityModel.cpp's tu_map row. Its .rodata
// (0x41A6A0-0x41A7D8: "%s%s" @479, then the RTTI chain ending with SkyCube's)
// restarts the labels after SelectiveGravityModel.cpp's RTTI, and
// soultree.cpp's strings (@813..) follow it. It has no .bss (no axis vectors).
// `make link` places its data as well as its code.


#include "d3dtypes.h"
#include "Cube/Cube.h"
#include "Camera/Camera.h"
#include "RenderTarget/RenderTarget.h"
#include "DrawableCube/DrawableCube.h"
#include "SkyCube/SkyCube.h"

extern "C" int sprintf(char *buf, const char *fmt, ...);
extern char SkyContentDirectory[];

// ---- src/SkyCube/ctor.cpp

SkyCube::SkyCube(int enabled) : DrawableCube(enabled)
{
}

// ---- src/SkyCube/Create.cpp

SkyCube *SkyCube::Create(char *name, float scale)
{
    char path[260];
    sprintf(path, "%s%s", SkyContentDirectory, name);
    if (DrawableCube::Create(path)) {
        m_heightOffset = scale;
        return this;
    }
    return 0;
}

// ---- src/SkyCube/Tick.cpp

extern RenderTarget *LpRenderTarget;

// Keep the sky centred on the camera.
int SkyCube::Tick(float dt)
{
    _D3DVECTOR eye;
    eye.x = LpRenderTarget->m_camera->m_position.x;
    eye.y = LpRenderTarget->m_camera->m_position.y + m_heightOffset;
    eye.z = LpRenderTarget->m_camera->m_position.z;
    m_cube->SetCamera(&eye, 0, 0);
    return DrawableCube::Tick(dt);
}

// ---- src/SkyCube/Render3D.cpp

int SkyCube::Render3D()
{
    if (m_searchPlacement)
        LpRenderTarget->m_camera->ResetClipPlanes();
    return DrawableCube::Render3D();
}

// ---- src/SkyCube/dtor.cpp

SkyCube::~SkyCube()
{
}
