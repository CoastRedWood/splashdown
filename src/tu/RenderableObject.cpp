// RenderableObject.cpp  @ 0x003448B0 (text 0x3448B0-0x345880)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's RenderableObject.cpp as one translation unit:
// RenderableObjectData's methods, then RenderableObject's (its @8@ dtor
// thunk at 0x345870 ends the TU), then the static initializer. `make link`
// places the object's data as well as its code (tools/link_c.py).
// UVAnimationData, PSXShaderDescriptor and MaterialAnimationData after it
// open PSXSurfaceMaterial.cpp (the TU map). The old files' views map onto
// the shared headers: UpdateLOD's "LOD data" is m_data (m_numLODs,
// m_lodScale, m_lods[].m_size), its centers/offsets are SoultreeObject's
// m_boundingBoxMin/m_localCenter and m_boundingRadius/m_searchRadius,
// chosen by m_numObjects; Create's PSXRenderableObjectData::m_owner is
// RenderableObjectData::m_owner.



#include "GameObject/GameObject.h"
#include "PSXSurfaceGeometry/PSXSurfaceGeometry.h"
#include "RenderableObjectData/RenderableObjectData.h"
#include "soultree_assert_block.h"
#include "DataAccess/DataAccess.h"
#include "RenderableObject/RenderableObject.h"
#include "d3dtypes.h"
#include "Camera/Camera.h"
#include "RenderTarget/RenderTarget.h"
#include "PSXRenderableObjectData/PSXRenderableObjectData.h"
// The axis vectors every Soultree TU defines (and __sinit constructs).
#include "sinit/AxisVectors.h"

extern "C" void free(void *p);
extern "C" void *malloc(unsigned int size);
extern "C" void *memset(void *dst, int c, unsigned int n);
extern "C" char *strcpy(char *dst, const char *src);
extern "C" char *strcat(char *dst, const char *src);
extern "C" void *memcpy(void *dst, const void *src, unsigned int n);
extern "C" int tolower(int c);
void SPLIT_PATH(const char *path, char *drive, char *dir, char *fname, char *ext);
void operator delete[](void *p);

extern DataAccess *LpDataAccess;
extern RenderTarget *LpRenderTarget;
extern char SltGeometryContentDirectory[0x104];
class InstanceMaster;
// No shared header yet (src/InstanceMasterList/Get.cpp has the class): only
// what Create calls.
class InstanceMasterList {
public:
    InstanceMaster *Get(char *name);
};
extern InstanceMasterList *LpInstanceMasterList;

float SceneGeometryLODBias = 1.0f;
float SceneTextureLODBias = 1.0f;

// ---- src/RenderableObjectData/ctor.cpp
RenderableObjectData::RenderableObjectData()
{
    m_numLODs = 0;
    m_lods = 0;
    m_lodScale = 1.0f;
    m_shader = 0;
    m_shaderChecked = 0;
}


// ---- src/RenderableObjectData/dtor.cpp
RenderableObjectData::~RenderableObjectData()
{
    for (int i = 0; i < m_numLODs; i++)
        delete[] m_lods[i].m_geometries;
    if (m_lods)
        free(m_lods);
}


// ---- src/RenderableObjectData/ReadFromBinaryFile.cpp
void RenderableObjectData::ReadFromBinaryFile(long file, InstanceMaster *master)
{
    int numGeometry;
    LpDataAccess->FRead(file, &m_numLODs, 4, 1);
    m_lods = (RenderableObjectLOD *)malloc(m_numLODs * sizeof(RenderableObjectLOD));
#line 49 "RenderableObject.cpp"
    ASSERT(m_lods, "ReadFromBinaryFile: out of memory");
    memset(m_lods, 0, m_numLODs * sizeof(RenderableObjectLOD));
    for (int i = 0; i < m_numLODs; i++) {
        LpDataAccess->FRead(file, &m_lods[i].m_size, 4, 1);
        LpDataAccess->FRead(file, &numGeometry, 4, 1);
        m_lods[i].m_numGeometries = numGeometry;
        m_lods[i].m_geometries = new PSXSurfaceGeometry[numGeometry];
        for (int j = 0; j < numGeometry; j++)
            m_lods[i].m_geometries[j].ReadFromFile(file, master, j);
    }
}


// ---- src/RenderableObjectData/ChangeMaterial.cpp






int RenderableObjectData::ChangeMaterial(int oldMaterial, int newMaterial)
{
    int i;
    int j;
    int result = oldMaterial;
    for (i = 0; i < m_numLODs; i++) {
        for (j = 0; j < m_lods[i].m_numGeometries; j++) {
            int r = m_lods[i].m_geometries[j].ChangeMaterial(oldMaterial, newMaterial);
            if (r == newMaterial)
                result = newMaterial;
        }
    }
    m_shader = 0;
    m_shaderChecked = 0;
    return result;
}


// ---- src/RenderableObject/ctor.cpp

RenderableObject::RenderableObject(int id) : SoultreeObject(id)
{
    m_searchVisible = 1;
    m_addToList = 1;
    m_data = 0;
    m_lod = 0;
    m_visible = 0;
    m_objects = 0;
    m_x1B1 = 0;
}


// ---- src/RenderableObject/dtor.cpp

RenderableObject::~RenderableObject()
{
    if (m_data) {
        // Retail sign-extends Release()'s result (dsll32/dsra32): it compared
        // an int against 0L. BaseObject::Release returns long, hence the cast.
        if ((int)m_data->Release() == 0L)
            LpDataAccess->DetachObject(m_data);
    }
    if (m_objects)
        delete[] m_objects;
}


// ---- src/RenderableObject/Create.cpp
// strrchr as inlined in retail.
static inline char *FindLast(char *s, int c)
{
    char *start = s;
    while (*s++)
        ;
    do {
        s--;
    } while (s != start && *s != c);
    return *s == c ? s : 0;
}

// Loads (or shares) the object's .psg geometry. With `instance` set the
// hierarchy goes under a new child node.
GameObject *RenderableObject::Create(char *name, int instance, int flags)
{
    SoultreeObject *objects[0x100];
    char path[0x110];
    char fname[0x108];
    long handle;
    int count;
    PSXRenderableObjectData *data;
    char format[4];
    int version;
    SoultreeObject *root;
    InstanceMaster *master;

    GameObject::Create();
    count = 0;
    root = this;
    if (SltGeometryContentDirectory[0]) {
        strcpy(path, SltGeometryContentDirectory);
        if (SltGeometryContentDirectory[strlen(SltGeometryContentDirectory) - 1] != '\\')
            strcat(path, "\\");
        strcat(path, name);
    } else {
        strcpy(path, name);
    }
    for (char *p = path; *p; p++)
        *p = tolower(*p);
    char *ext = FindLast(path, '.');
    if (ext)
        strcpy(ext, ".psg");
    else
        strcat(path, (char *)".psg");    // redundant cast: retail loads the literal first
    strcpy(m_name, path);
    data = 0;
    handle = -1;
    if (instance) {
        m_x191_bit1 = 1;
        root = new SoultreeObject(1);
        AddSoultreeChild(root);
    }
    if (!LpDataAccess->GetDataOrFileHandle(path, "rb", &handle, (void **)&data, 0)) {
        PSXprintf("RenderableObject '%s' not found\n", path);
        Release();
        return 0;
    }
    if (data) {
        m_data = data;
        m_data->AddRef();
        RenderableObject *source = m_data->m_owner;
        if (instance) {
            if (source->m_x191_bit1) {
                SoultreeObject *parent = root->m_parent;
                root->Clone(source->m_child);
                root->m_parent = parent;
            } else {
                root->Clone(source);
            }
            count = 1;
            objects[0] = root;
            root->FillObjectList(&count, objects);
        } else {
            if (source->m_x191_bit1) {
                Clone(source->m_child);
            } else if (!source->m_child) {
                m_boundingBoxMin.x = source->m_boundingBoxMin.x;
                m_boundingBoxMin.y = source->m_boundingBoxMin.y;
                m_boundingBoxMin.z = source->m_boundingBoxMin.z;
                m_boundingBoxMin.w = source->m_boundingBoxMin.w;
                m_boundingBoxMax.x = source->m_boundingBoxMax.x;
                m_boundingBoxMax.y = source->m_boundingBoxMax.y;
                m_boundingBoxMax.z = source->m_boundingBoxMax.z;
                m_boundingBoxMax.w = source->m_boundingBoxMax.w;
                m_boundingRadius = source->m_boundingRadius;
                m_boundingBoxValid = 1;
                m_x190_bit2 = 1;
            } else {
                Clone(source);
            }
            count = 1;
            objects[0] = this;
            root->FillObjectList(&count, objects);
        }
    } else {
        LpDataAccess->FRead(handle, format, 4, 1);
        LpDataAccess->FRead(handle, &version, 4, 1);
        if (!strcasecmp(format, "pgi") || !strcasecmp(format, "dgi"))
            m_x1B1Bits.m_instanced = 1;
        if ((strcasecmp(format, "psg") && strcasecmp(format, "dxg") && !m_x1B1Bits.m_instanced) || version != 2) {
#line 281 "RenderableObject.cpp"
            ASSERTF(0, ("Create: %s invalid file (version (%d) wrong or format (%s) is not psg or dxg)", name, version, format));
        }
        if (instance) {
            count = root->ReadHierarchyFromFile(handle, objects);
            m_numObjects = count;
        } else {
            count = ReadHierarchyFromFile(handle, objects);
        }
        master = 0;
        if (m_x1B1Bits.m_instanced) {
            SPLIT_PATH(path, 0, 0, fname, 0);
            fname[strlen(fname) - 2] = 0;
            master = LpInstanceMasterList->Get(fname);
#line 309 "RenderableObject.cpp"
            ASSERTF(master, ("Instanced geometry '%s' cannot find master '%s%s'", path, fname, ".pgm"));
        }
        m_data = new PSXRenderableObjectData;
        m_data->ReadFromBinaryFile(handle, master);
        m_data->m_owner = this;
        LpDataAccess->AttachObject(handle, m_data);
        LpDataAccess->FClose(handle);
    }
    m_objects = new SoultreeObject *[count];
    memcpy(m_objects, objects, count * sizeof(SoultreeObject *));
    return this;
}


// ---- src/PSXRenderableObject/RenderableObjectMisc.cpp
int RenderableObject::BoundingBoxVisible()
{
    return 1;
}

void RenderableObject::SetRenderableLODBias(float geometry, float texture)
{
    SceneGeometryLODBias = geometry;
    SceneTextureLODBias = texture;
}

// ---- src/RenderableObject/PrepareGeometry.cpp

int RenderableObject::PrepareGeometry()
{
    if (m_hidden) {
        m_visible = 0;
        return 1;
    }
    if (m_visible = (signed char)BoundingBoxVisible())
        UpdateLOD();
    return 1;
}


// ---- src/RenderableObject/UpdateLOD.cpp

static inline float Sqrt(float x)
{
    float r;
    asm("sqrt.s $f8, %1; swc1 $f8, 0(%0)" : : "r"(&r), "f"(x) : "$f8", "memory");
    return r;
}

static inline float Length(const _D3DVECTOR &v)
{
    return Sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

static inline float Fmax(float a, float b)
{
    float r;
    asm("max.s $f8, %1, %2; swc1 $f8, 0(%0)" : : "r"(&r), "f"(a), "f"(b) : "$f8", "memory");
    return r;
}

void RenderableObject::UpdateLOD()
{
    float d;
    float distance;
    float offset = m_numObjects == 1 ? m_boundingRadius : m_searchRadius;
    _D3DVECTOR v;
    v = (m_numObjects == 1 ? m_boundingBoxMin : m_localCenter) - LpRenderTarget->m_camera->m_position;
    v.x += m_localMatrix._41;
    v.y += m_localMatrix._42;
    v.z += m_localMatrix._43;
    distance = Length(v);
    m_distance = distance;
    d = distance - offset;
    d = Fmax(0.1f, d);
    float factor = LpRenderTarget->m_camera->m_focalLength;
    float geometryScale = factor / d;
    float textureScale = 2.0f * (offset * (SceneTextureLODBias * factor)) / d;
    int lod = 0;
    int i;
    for (i = 0; i < m_data->m_numLODs - 1; i++) {
        if (m_data->m_lodScale * (geometryScale * SceneGeometryLODBias * m_data->m_lods[i].m_size) < 1.0f)
            lod = i + 1;
    }
    m_lod = lod;
    RenderableObjectLOD *level = &m_data->m_lods[m_lod];
    for (i = 0; i < level->m_numGeometries; i++)
        level->m_geometries[i].UpdateLOD(textureScale, geometryScale);
}


// ---- src/RenderableObject/ChangeMaterial.cpp
int RenderableObject::ChangeMaterial(int from, int to)
{
    return m_data->ChangeMaterial(from, to);
}


// ---- src/PSXRenderableObject/RenderableObjectMisc.cpp (cont.)
int RenderableObject::ChangeFillColor(int a, int b)
{
    return 0;
}


// ---- src/sinit/RenderableObject.cpp
// Static initializer of RenderableObject.cpp: only the axis vectors from the shared header.

