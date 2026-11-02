// UIWaterEffect.cpp  @ 0x00315010 (text 0x315010-0x315930)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's UIWaterEffect.cpp as one translation unit: its functions in
// retail order, then its static initializer. `make link` places the
// object's data as well as its code (tools/link_c.py). The three Mesh
// functions at the end of its tu_map row are Mesh.cpp, a TU of its own
// without an initializer (its .rodata repeats the BaseObject and GameObject
// RTTI records); see src/tu/Mesh.cpp.



#include "UIWaterEffect/UIWaterEffect.h"
#include "UIWaterPatch/UIWaterPatch.h"
#include "d3dtypes.h"
#include "sinit/AxisVectors.h"

// ---- src/UIWaterEffect/ctor.cpp

UIWaterEffect::UIWaterEffect(int enabled) : GameObject(enabled)
{
    m_patch = 0;
    m_vertices = 0;
    m_normals = 0;
    m_uvs = 0;
    m_normalDirty = 0;
    m_heightChanged = 0;
    m_width = 0;
    m_height = 0;
    m_x60 = 0;
    m_x50 = -1;
    m_x58 = 0;
    m_x54 = -1;
    m_x5C = 0;
}


// ---- src/UIWaterEffect/dtor.cpp

UIWaterEffect::~UIWaterEffect()
{
    if (m_patch)
        m_patch->Release();
    if (m_vertices)
        delete[] m_vertices;
    if (m_normals)
        delete[] m_normals;
    if (m_uvs)
        delete[] m_uvs;
    if (m_normalDirty)
        delete[] m_normalDirty;
    if (m_heightChanged)
        delete[] m_heightChanged;
}


// ---- src/UIWaterEffect/Create.cpp

UIWaterEffect *UIWaterEffect::Create(int size)
{
    GameObject::Create();
    InternalCreate(size);
    return this;
}


// ---- src/UIWaterEffect/InternalCreate.cpp


// Builds a size x size grid at y = 975 over WIDTH x HEIGHT.
void UIWaterEffect::InternalCreate(int size)
{
    _D3DVECTOR origin(OriginX, 975.0f, OriginZ);
    int z, x;
    float cellWidth, cellHeight;

    m_patch = new UIWaterPatch(1);
    m_width = size;
    m_height = size;
    m_patch->Create(origin, size, WIDTH / (m_width - 1), HEIGHT / (m_height - 1));
    m_vertices = new _D3DVECTOR[m_height * m_width];
    m_normals = new _D3DVECTOR[m_height * m_width];
    m_uvs = new WaterUV[m_height * m_width];
    m_heightChanged = new int[m_height * m_width];
    m_normalDirty = new int[m_height * m_width];
    cellWidth = WIDTH / (m_width - 1);
    cellHeight = HEIGHT / (m_height - 1);
    for (z = 0; z < m_height; z++) {
        float offsetZ = z * cellHeight;
        float v = 1.0f - (float)z / (float)(m_height - 1);
        for (x = 0; x < m_width; x++) {
            int index = x + z * m_width;
            float u = (float)x / (float)(m_width - 1);
            m_vertices[index].x = OriginX + x * cellWidth;
            m_vertices[index].y = 975.0f;
            m_vertices[index].z = OriginZ + offsetZ;
            m_normals[index].x = 0.0f;
            m_normals[index].y = 1.0f;
            m_normals[index].z = 0.0f;
            m_uvs[index].u = u;
            m_uvs[index].v = v;
            m_normalDirty[index] = 0;
        }
    }
}


// ---- src/UIWaterEffect/ModifyMesh.cpp


void UIWaterEffect::ModifyMesh()
{
    for (int z = 0; z < m_height; z++) {
        for (int x = 0; x < m_width; x++) {
            int index = x + z * m_width;
            m_vertices[index].y = 975.0f + m_patch->GetHeight(x, z);
        }
    }
}


// The class's statics. Retail's .data has them after VehicleAI.cpp's, which
// fits this TU or PSXUIWaterEffect.cpp; this one reads all three.
// Defined before UpdateNormals: the origin's .bss precedes its static.
int UIWaterEffect::EnableUIWaterEffectLighting = 1;
float UIWaterEffect::HEIGHT = 38.461536f;
float UIWaterEffect::WIDTH = 51.28205f;
float UIWaterEffect::OriginZ = 256.0f - UIWaterEffect::HEIGHT / 2.0f;
float UIWaterEffect::OriginX = 384.0f - UIWaterEffect::WIDTH / 2.0f;

// ---- src/UIWaterEffect/UpdateNormals.cpp
#pragma inline_depth(6)  // CrossProduct/Normalize/operator- nest


// libvu0 sceVu0OuterProduct: out = a x b.
static inline _D3DVECTOR CrossProduct(_D3DVECTOR &a, _D3DVECTOR &b)
{
    _D3DVECTOR out;
    asm("lqc2 vf4, 0x0(%1); lqc2 vf5, 0x0(%2); vopmula.xyz ACC, vf4, vf5; vopmsub.xyz vf6, vf5, vf4; vsub.w vf6, vf6, vf6; sqc2 vf6, 0x0(%0)"
        : : "r"(&out), "r"(&a), "r"(&b) : "memory");
    return out;
}

// libvu0 sceVu0Normalize.
static inline _D3DVECTOR Normalize(const _D3DVECTOR &in)
{
    _D3DVECTOR out;
    asm("lqc2 vf4, 0x0(%1); vmul.xyz vf5, vf4, vf4; vaddy.x vf5, vf5, vf5y; vaddz.x vf5, vf5, vf5z; vrsqrt Q, vf0w, vf5x; vsub.xyzw vf6, vf0, vf0; vaddw.xyzw vf6, vf6, vf4w; vwaitq; vmulq.xyz vf6, vf4, Q; sqc2 vf6, 0x0(%0)"
        : : "r"(&out), "r"(&in) : "memory");
    return out;
}

// Recomputes the normal of every interior grid point whose neighbours moved,
// flagging it for PSXUIWaterEffect::UpdateNormals to copy into the mesh.
// Retail's pointers start at the row's first vertex while x starts at 1, so
// each normal lands one point to the left of the one it describes; kept.
void UIWaterEffect::UpdateNormals()
{
    static int UIWaterEffect_UpdateNormals = -1;  // profiling timer id (unused)
    if (EnableUIWaterEffectLighting) {
        _D3DVECTOR across;
        _D3DVECTOR down;
        int row;
        int x;
        _D3DVECTOR *vertex;
        _D3DVECTOR *normal;
        int *dirty;
        int *changed;
        int end = (m_height - 1) * m_width;
        for (row = m_width; row < end; row += m_width) {
            vertex = &m_vertices[row];
            normal = &m_normals[row];
            dirty = &m_normalDirty[row];
            changed = &m_heightChanged[row];
            for (x = 1; x < m_width - 1; x++) {
                if (changed[1] || changed[-1] || *(changed + m_width) || *(changed - m_width)) {
                    across = vertex[1] - vertex[-1];
                    down = *(vertex + m_width) - *(vertex - m_width);
                    *normal = Normalize(CrossProduct(down, across));
                    *dirty = 1;
                } else {
                    *dirty = 0;
                }
                vertex++;
                normal++;
                dirty++;
                changed++;
            }
        }
    }
}


// ---- src/UIWaterEffect/Tick.cpp
// (Was `#pragma inline_depth(reset)`, which mwcc rejects as an illegal
// pragma and ignores: inline_depth is effectively TU-wide)

int UIWaterEffect::Tick(float dt)
{
    GameObject::Tick(dt);
    ModifyMesh();
    return 1;
}


// ---- src/UIWaterEffect/ApplyExternalForce.cpp


// Screen coordinates (640x448) to water-patch coordinates.
void UIWaterEffect::ApplyExternalForce(float force, float x, float y)
{
    x = OriginX + (x * WIDTH) / 640.0f;
    y = OriginZ + ((448.0f - y) * HEIGHT) / 448.0f;
    m_patch->ApplyExternalForce(force, _D3DVECTOR(x, 1025.0f, y));
}


// ---- src/PSXUIWaterEffect/UIWaterEffectRender3D.cpp

int UIWaterEffect::Render3D() { return 0; }


// ---- src/sinit/UIWaterEffect.cpp
// Static initializer of UIWaterEffect.cpp: the axis vectors and the water
// origin, centred on the 768x512 screen (the statics are defined before
// UpdateNormals).
