// GateClass.cpp  @ 0x0030A860 (text 0x30A860-0x30AC90)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// GateClass as one translation unit, a TU without an initializer inside
// Portal.cpp's tu_map row, after RaceStatusClass.cpp. Its .rodata is only the
// GateClass RTTI (0x4309C0-0x430A30, labels restart at @436).



#include "d3dtypes.h"
#include "GameObject/GameObject.h"
#include "GateClass/GateClass.h"

// ---- src/GateClass/ctor.cpp


GateClass::GateClass(int id) : GameObject(id)
{
    m_flags = 0;
    m_center.x = 0.0f;
    m_center.y = 0.0f;
    m_center.z = 0.0f;
    m_center.w = 1.0f;
    m_normal.x = 0.0f;
    m_normal.y = 0.0f;
    m_normal.z = 0.0f;
    m_normal.w = 1.0f;
    m_halfSize.x = 0.0f;
    m_halfSize.y = 0.0f;
    m_halfSize.z = 0.0f;
    m_halfSize.w = 1.0f;
    m_x70 = 0;
    m_height = 0;
    m_depth = 0;
    m_next = 0;
}


// ---- src/GateClass/Create.cpp

GateClass *GateClass::Create(int type, _D3DVECTOR *center, _D3DVECTOR *normal, _D3DVECTOR *halfSize, float *width,
                             float *height, float *depth)
{
    m_flags = type;
    if (center)
        m_center = *center;
    if (normal)
        m_normal = *normal;
    if (halfSize)
        m_halfSize = *halfSize;
    if (width)
        m_x70 = *width;
    if (height)
        m_height = *height;
    if (depth)
        m_depth = *depth;
    GameObject::Create();
    return this;
}


// ---- src/GateClass/Crossed.cpp

int Parametric2DLineIntersection(float x1, float y1, float dx1, float dy1,
                                 float x2, float y2, float dx2, float dy2, float *t1, float *t2);


// 0: not crossed, 1: crossed, 2: crossed the gate's line outside the gate,
// 4: the gate is inactive.
int GateClass::Crossed(_D3DVECTOR *from, _D3DVECTOR *to)
{
    float x, z, dx, dz;
    float gx, gz, gdx, gdz;
    float t, u;

    if (!from || !to)
        return 0;
    if (!(m_flags & 1))
        return 4;
    if (to->x == 0.0f && to->y == 0.0f && to->z == 0.0f)
        return 0;
    x = to->x;
    z = to->z;
    dx = from->x - x;
    dz = from->z - z;
    if (m_flags & 8) {
        gx = m_center.x;
        gz = m_center.z;
        gdx = m_halfSize.x;
        gdz = m_halfSize.z;
    } else {
        gx = m_center.x - m_halfSize.x;
        gz = m_center.z - m_halfSize.z;
        gdx = 2.0f * m_halfSize.x;
        gdz = 2.0f * m_halfSize.z;
    }
    if (Parametric2DLineIntersection(x, z, dx, dz, gx, gz, gdx, gdz, &t, &u)) {
        if (t >= 0.0f && t <= 1.0f) {
            if (u >= 0.0f && u <= 1.0f) {
                if (m_height < 0.0f || to->y + t * (from->y - to->y) - m_center.y <= m_height) {
                    if ((m_flags & 4) || m_normal.x * (from->x - to->x) + m_normal.y * (from->y - to->y) + m_normal.z * (from->z - to->z) >= 0.0f)
                        return 1;
                }
            } else {
                return 2;
            }
        }
    }
    return 0;
}


// ---- src/GateClass/dtor.cpp


GateClass::~GateClass()
{
}

