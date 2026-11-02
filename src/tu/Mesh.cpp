// Mesh.cpp  @ 0x00315930 (text 0x315930-0x315B20)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's Mesh.cpp as one translation unit: a TU without a static
// initializer, at the end of UIWaterEffect.cpp's tu_map row (its .rodata
// repeats the BaseObject/GameObject RTTI records, so it is not part of
// UIWaterEffect.cpp). `make link` places the object's data as well as its
// code (tools/link_c.py).


#include "Mesh/Mesh.h"

// ---- src/Mesh/ctor.cpp
// (Same as MediaControl::MediaControl, different members.)

Mesh::Mesh(int enabled) : GameObject(enabled)
{
    m_count = 0;
    m_values = 0;
    m_flags = 0;
    m_color = 0x7F7F7F;
    m_geometry = 0;
    m_x48 = 0;
}


// ---- src/Mesh/dtor.cpp

Mesh::~Mesh()
{
    if (m_values)
        delete[] m_values;
    if (m_geometry)
        delete m_geometry;
}


// ---- src/Mesh/Create.cpp

Mesh *Mesh::Create(char *name, unsigned char flags, int count, int *values,
                   MeshLightingType lighting, int color)
{
    int i;

    m_flags = flags;
    m_count = count;
    m_values = new int[count];
    m_lighting = lighting;
    m_color = color;
    for (i = 0; i < count; i++)
        m_values[i] = *values++;
    m_geometry = new PSXSurfaceGeometry;
    m_geometry->Create(name, flags);
    return this;
}
