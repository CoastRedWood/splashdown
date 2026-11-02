// SurfaceMaterial.cpp  @ 0x0034A5D0 (text 0x34A5D0-0x34AA90)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's SurfaceMaterial.cpp as one translation unit, a TU without an
// initializer inside PSXSurfaceMaterial.cpp's tu_map row: ShaderDescriptor's
// ctor/dtor and SurfaceMaterial. Its .rodata (0x4367B0-0x43681C, labels
// restarting at @527 after PSXSurfaceGeometry.cpp's RTTI) is ".psm",
// "General", "TotalShaderPasses" and the SurfaceMaterial and ShaderDescriptor
// RTTI.


#include "ShaderDescriptor/ShaderDescriptor.h"
#include "SurfaceMaterial/SurfaceMaterial.h"
#include "PSXShaderDescriptor/PSXShaderDescriptor.h"
#include "StringTable/StringTable.h"
#include "ParameterBlock/ParameterBlock.h"

// ---- src/ShaderDescriptor/ctor.cpp

ShaderDescriptor::ShaderDescriptor()
{
}


// ---- src/ShaderDescriptor/dtor.cpp

ShaderDescriptor::~ShaderDescriptor()
{
}


// ---- src/SurfaceMaterial/ctor.cpp

SurfaceMaterial::SurfaceMaterial()
{
    m_index = -1;
    m_numShaders = 0;
    for (int i = 0; i < 4; i++) {
        m_shaders[i] = 0;
        m_instances[i] = -1;
    }
}


// ---- src/SurfaceMaterial/dtor.cpp


SurfaceMaterial::~SurfaceMaterial()
{
    for (int i = 0; i < 4; i++) {
        if (m_shaders[i])
            delete m_shaders[i];
    }
}


// ---- src/SurfaceMaterial/ReadFromFile.cpp
extern "C" char *strcpy(char *dst, const char *src);
extern "C" char *strcat(char *dst, const char *src);
extern "C" unsigned int strlen(const char *s);
extern "C" void *memset(void *dst, int c, unsigned int n);
extern "C" int tolower(char c);

extern StringTable *LpMaterialStringTable;


// An inlined strrchr.
static inline char *FindLast(char *s, char c)
{
    char *start = s;
    while (*s++)
        ;
    do {
        s--;
    } while (s != start && *s != c);
    if (*s == c)
        return s;
    return 0;
}



int SurfaceMaterial::ReadFromFile(char *name)
{
    char path[0x110];
    char directory[0x80];
    strcpy(path, name);
    for (char *p = path; *p; p++)
        *p = tolower(*p);
    m_index = LpMaterialStringTable->GetIndex(path);
    char *ext = FindLast(path, '.');
    if (ext)
        strcpy(ext, ".psm");
    else
        strcat(path, (const char *)".psm");  // the cast loads the literal first, as in retail
    strcpy(m_name, path);
    ParameterBlock *params = new ParameterBlock;
    params->OpenFile(path, 0);
    if (params->m_file == -1) {
        // No material file: one shader named after the texture.
        ext = FindLast(path, '.');
        if (ext)
            memset(ext, 0, strlen(ext));
        m_numShaders = 1;
        m_shaders[0] = new PSXShaderDescriptor;
        m_shaders[0]->ReadFromFile(path);
    } else {
        params->ReadParameterBlock("General");
        params->GetParameter("TotalShaderPasses", 0, &m_numShaders);
        strcpy(directory, path);
        char *slash = FindLast(directory, '\\');
        if (slash)
            slash[1] = 0;
        else
            directory[0] = 0;
        for (int i = 0; i < m_numShaders; i++) {
            m_shaders[i] = new PSXShaderDescriptor;
            ((PSXShaderDescriptor *)m_shaders[i])->ReadFromFile(directory, params, i, m_numShaders);
        }
    }
    LpMaterialStringTable->SetPointer(m_index, this);
    if (params)
        delete params;
    return 1;
}

