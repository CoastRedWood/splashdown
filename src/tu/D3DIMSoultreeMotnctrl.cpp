// D3DIMSoultreeMotnctrl.cpp  @ 0x0022BEE0 (text 0x22BEE0-0x22CB20)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp)
//
// Retail's D3DIMSoultreeMotnctrl.cpp as one translation unit: AddToPartsIndex
// and D3DIMSoultreeCharacter, the third TU of Cube.cpp's tu_map row (its
// ASSERTs say "D3DIMSoultreeMotnctrl.cpp"; .rodata 0x411220-0x4113B0 after
// DrawableCube's RTTI; the row's third .ctor slot). `make link` places its
// data as well as its code.


#include "soultree_assert_block.h"
#include "D3DIMSoultreeCharacter/D3DIMSoultreeCharacter.h"
#include "DataAccess/DataAccess.h"
#include "ParameterBlock/ParameterBlock.h"
#include "GameObject/GameObject.h"
#include "PSXRenderableObject/PSXRenderableObject.h"
#include "SoultreeObject/SoultreeObject.h"
#include "sinit/AxisVectors.h"

extern "C" char *strcpy(char *dst, const char *src);
extern "C" char *strcat(char *dst, const char *src);
extern "C" int strcasecmp(const char *a, const char *b);

void SPLIT_PATH(const char *path, char *drive, char *dir, char *fname, char *ext);

extern DataAccess *LpDataAccess;
extern char SltGeometryContentDirectory[0x104];

// One entry of an animated object's part table (0x40 bytes).
struct AnimatedObject {
    char m_name[0x32];                  // +0x00
    char pad32[0x34 - 0x32];
    SoultreeObject *m_object;           // +0x34
    int m_hasPosition;                  // +0x38
    int m_hasRotation;                  // +0x3C
};

// Character::m_bones' element (Character.h forward-declares it): the part
// table LoadParts builds, an AnimatedObject. (Derived from AnimatedObject it
// would no longer be a POD: new[]/delete[] would add an array cookie.)
struct CharacterBone {
    char m_name[0x32];                  // +0x00
    char pad32[0x34 - 0x32];
    SoultreeObject *m_object;           // +0x34
    int m_hasPosition;                  // +0x38
    int m_hasRotation;                  // +0x3C
};

// ---- src/global/AddToPartsIndex.cpp

static inline void Strupr(char *s)
{
    while (*s) {
        if (*s >= 'a' && *s <= 'z')
            *s -= 0x20;
        s++;
    }
}

// Adds object and its siblings, then recursively their children, to the
// part table; returns the next free index.
int AddToPartsIndex(int index, AnimatedObject *parts, SoultreeObject *object)
{
    SoultreeObject *o;
    parts[index].m_object = object;
    parts[index].m_hasPosition = 0;
    parts[index].m_hasRotation = 0;
    Strupr(object->m_name);
#line 35 "D3DIMSoultreeMotnctrl.cpp"
    STRCPY(parts[index].m_name, object->m_name);
    index++;
    for (o = object->m_sibling; o; o = o->m_sibling) {
        parts[index].m_object = o;
        Strupr(o->m_name);
        strcpy(parts[index].m_name, o->m_name);
        parts[index].m_hasPosition = 0;
        parts[index].m_hasRotation = 0;
        index++;
    }
    if (object->m_child)
        index = AddToPartsIndex(index, parts, object->m_child);
    for (o = object->m_sibling; o; o = o->m_sibling) {
        if (o->m_child)
            index = AddToPartsIndex(index, parts, o->m_child);
    }
    return index;
}

// ---- src/D3DIMSoultreeCharacter/ctor.cpp

D3DIMSoultreeCharacter::D3DIMSoultreeCharacter(int construct)
    : GameObject(1), Character(construct)
{
    m_root = 0;
    m_name[0] = 0;
    m_1F8 = 0;
    m_1FC = 0;
    m_200 = 0;
    m_204 = 0;
    m_bones = 0;
}

// ---- src/D3DIMSoultreeCharacter/Create.cpp

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

GameObject *D3DIMSoultreeCharacter::Create(char *name, char *ext, int a, int flags)
{
    char path[0x100];
    char fname[0x100];
    long handle;
    long newHandle;
    Character *data;

    Character::Create(flags);
    m_20C = a;
    handle = -1;
    data = 0;
    if (ext) {
        strcpy(path, name);
        strcat(path, ext);
        if (!LpDataAccess->GetDataOrFileHandle(path, "rb", &handle, (void **)&data, 0)) {
            if (!LpDataAccess->GetDataOrFileHandle(name, "rb", &handle, (void **)&data, 0)) {
                PSXprintf("WARNING!!! Cannot open D3DIMSoultreeCharacter %s\n", name);
                Release();
                return 0;
            }
        }
    } else {
        if (!LpDataAccess->GetDataOrFileHandle(name, "rb", &handle, (void **)&data, 0)) {
            PSXprintf("WARNING!!! Cannot open D3DIMSoultreeCharacter %s\n", name);
            Release();
            return 0;
        }
    }
    if (!data && ext) {
        LpDataAccess->AddFile(path, 0, 0, 0, &newHandle, 0);
        LpDataAccess->AttachObject(newHandle, this);
        LpDataAccess->FClose(handle);
    } else if (!data) {
        LpDataAccess->AttachObject(handle, this);
        LpDataAccess->FClose(handle);
    }
    m_params = new ParameterBlock;
#line 165 "D3DIMSoultreeMotnctrl.cpp"
    STRCPY(m_fileName, name);
    m_params->OpenFile(name, 0);
    m_params->ReadParameterBlock("General info");
    if (ext) {
        strcpy(m_name, ext);
        m_useContentDirectory = 1;
    } else {
        m_params->GetParameter("SLTFile", m_name, -1);
        m_useContentDirectory = 0;
    }
    if (data) {
        SPLIT_PATH(data->m_root->m_name, 0, 0, fname, 0);
        char *dot = FindLast(fname, '.');
        if (dot)
            strcpy(dot, ".slt");
        else
            strcat(fname, (char *)".slt");    // redundant cast: retail loads the literal first
        if (strcasecmp(fname, m_name))
            data = 0;
    }
    if (data) {
        Clone(data);
        PSXprintf("     Cloning\n");
        delete m_params;
        return this;
    } else {
        Init();
        delete m_params;
        return this;
    }
}

// ---- src/D3DIMSoultreeCharacter/dtor.cpp
// (The vtable, and with it the three @528@48@ this-adjusting thunks, is emitted in this file.)

D3DIMSoultreeCharacter::~D3DIMSoultreeCharacter()
{
    if (m_bones)
        delete[] m_bones;
    LpDataAccess->DetachObject(this);
}

// ---- src/D3DIMSoultreeCharacter/LoadParts.cpp

void D3DIMSoultreeCharacter::LoadParts()
{
    char path[0x104];
    char savedDirectory[0x104];
    if (!m_useContentDirectory) {
        strcpy(savedDirectory, SltGeometryContentDirectory);
        SltGeometryContentDirectory[0] = 0;
#line 227 "D3DIMSoultreeMotnctrl.cpp"
        STRCPY(path, m_directory);
        strcat(path, m_name);
    } else {
        savedDirectory[0] = 0;
#line 231 "D3DIMSoultreeMotnctrl.cpp"
        STRCPY(path, m_name);
    }
    m_root = new PSXRenderableObject(m_enabled);
    ((PSXRenderableObject *)m_root)->Create(path, 1, 1);
    AddChild(m_root, 0);
    m_numBones = m_root->NumberOfObjectsInHierarchy() - 1;
    m_bones = new CharacterBone[m_numBones];
    AddToPartsIndex(0, (AnimatedObject *)m_bones, m_root->m_child);
    LoadMirrorList();
    LoadVUT();
    if (!m_useContentDirectory)
        strcpy(SltGeometryContentDirectory, savedDirectory);
}

// ---- src/D3DIMSoultreeCharacter/Clone.cpp

void D3DIMSoultreeCharacter::Clone(Character *other)
{
#line 257 "D3DIMSoultreeMotnctrl.cpp"
    STRCPY(m_name, ((D3DIMSoultreeCharacter *)other)->m_name);   // the other's name, at +0x1A8
    Character::Clone(other);
}

// ---- src/D3DIMSoultreeCharacter/Enable.cpp

void D3DIMSoultreeCharacter::Enable()
{
    GameObject::Enable();
}

// ---- src/D3DIMSoultreeCharacter/Disable.cpp

void D3DIMSoultreeCharacter::Disable()
{
    GameObject::Disable();
    m_1F8 = 0;
    m_1FC = 0;
    m_200 = 0;
    m_204 = 0;
}

// ---- src/sinit/D3DIMSoultreeMotnctrl.cpp
// Static initializer: only the axis vectors (sinit/AxisVectors.h).
