// MotionLibrary.cpp  @ 0x003515D0 (text 0x3515D0-0x3535F0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's MotionLibrary.cpp as one translation unit: Motion's five functions
// and MotionLibrary's nine, then the static initializer. `make link` places
// the object's data as well as its code (tools/link_c.py). The rest of the
// tu_map row (0x3535F0-0x353BE0, SinCosTableCalculations and the angle-axis
// functions) is AngleAxis.cpp, src/tu/AngleAxis.cpp.




// LoadMotion's per-function file set it (FindExtension is inlined inside
// SetExtension); TU-wide.
#pragma inline_depth(5)

#include "Motion/Motion.h"
#include "PositionController/PositionController.h"
#include "LinearPositionController/LinearPositionController.h"
#include "BezierPositionController/BezierPositionController.h"
#include "RotationController/RotationController.h"
#include "ByteAxisRotationController/ByteAxisRotationController.h"
#include "ShortAxisRotationController/ShortAxisRotationController.h"
#include "MotionLibrary/MotionLibrary.h"
#include "soultree_assert_block.h"
#include "ParameterBlock/ParameterBlock.h"
#include "DataAccess/DataAccess.h"
#include "d3dtypes.h"
#include "Quaternion/Quaternion.h"
#include "SoultreeObject/SoultreeObject.h"
#include "TriggerObject/Vu0Orient.h"
#include "Character/Character.h"
#include "ByteVector/ByteVector.h"
#include "sinit/AxisVectors.h"

extern "C" void free(void *p);
extern "C" void *realloc(void *p, unsigned int size);
extern "C" void *malloc(unsigned int size);
extern "C" void *memset(void *dst, int c, unsigned int n);
extern "C" void *memcpy(void *dst, const void *src, unsigned int n);
extern "C" char *strcpy(char *dst, const char *src);
extern "C" char *strcat(char *dst, const char *src);
extern "C" char *strupr(char *s);
extern "C" char *strtok(char *s, const char *delim);
extern "C" int strcasecmp(const char *a, const char *b);
extern "C" double atof(const char *s);
void PSXprintf(const char *fmt, ...);

extern DataAccess *LpDataAccess;

// One bone of a Character (0x40 bytes).
struct CharacterBone {
    char pad0[0x34];
    SoultreeObject *m_object;           // +0x34
    int m_hasPosition;                  // +0x38
    int m_hasRotation;                  // +0x3C
};


// The global motion library, allocated by the static initializer (with
// the axis vectors). Defined before the functions: its .bss precedes their
// statics in retail.
MotionLibrary *LpMotionLibrary = new MotionLibrary;

// Motion's functions (0x3515D0-0x351970) open this TU: retail's .rodata
// starts with the "" (@826) that Motion's ctor and LoadMotion share, so the
// ctor is in this TU, not after KeyFrameController.cpp's RTTI (one literal,
// no pooling across TUs).

// ---- src/Motion/ctor.cpp



Motion::Motion()
{
    strcpy(m_vueName, "");
    strcpy(m_name, "");
    m_transitionTime = 0;
    m_fps = 0;
    m_looping = 0;
    m_positions = 0;
    m_rotations = 0;
    m_numPositions = 0;
    m_numRotations = 0;
    m_rotationType = 0;
    m_positionType = 1;
    m_refCount = 0;
    m_duration = 0;
    m_frameCount = 0;
}


// ---- src/Motion/dtor.cpp

Motion::~Motion()
{
    if (m_rotations) {
        for (int i = 0; i < m_numRotations; i++) {
            delete m_rotations[i];
        }
        free(m_rotations);
    }
    if (m_positions) {
        for (int i = 0; i < m_numPositions; i++) {
            delete m_positions[i];
        }
        free(m_positions);
    }
}


// ---- src/Motion/PastEndOfMotion.cpp

int Motion::PastEndOfMotion(float time)
{
    if (time > m_duration)
        return 1;
    return 0;
}


// ---- src/Motion/AllocateRotations.cpp

void Motion::AllocateRotations(int count)
{
    m_rotations = (RotationController **)malloc(count * sizeof(RotationController *));
    for (int i = 0; i < count; i++) {
        if (m_rotationType == 0)
            m_rotations[i] = new ByteAxisRotationController;
        else if (m_rotationType == 1)
            m_rotations[i] = new ShortAxisRotationController;
    }
}


// ---- src/Motion/AllocatePositions.cpp

void Motion::AllocatePositions(int count)
{
    m_positions = (PositionController **)malloc(count * sizeof(PositionController *));
    for (int i = 0; i < count; i++) {
        if (m_positionType == 0)
            m_positions[i] = new LinearPositionController;
        else if (m_positionType == 1)
            m_positions[i] = new BezierPositionController;
    }
}


// ---- src/MotionLibrary/ctor.cpp

MotionLibrary::MotionLibrary()
{
    m_numMotions = 0;
    m_motions = 0;
    m_memoryUsed = 0;
    m_x08 = 0;
    m_x0C = 0;
    m_keepMotions = 0;
    m_exportEnabled = 1;
}


// ---- src/MotionLibrary/dtor.cpp
MotionLibrary::~MotionLibrary()
{
    if (m_motions) {
        for (int i = 0; i < m_numMotions; i++)
            delete m_motions[i];
        free(m_motions);
    }
    m_numMotions = 0;
}


// ---- src/MotionLibrary/ReleaseMotion.cpp
void MotionLibrary::ReleaseMotion(Motion *motion)
{
    int index = 0;
    for (int i = 0; i < m_numMotions; i++) {
        if (m_motions[i] == motion)
            index = i;
    }
    motion->m_refCount--;
    if (motion->m_refCount <= 0 && !m_keepMotions) {
        delete motion;
        m_motions[index] = 0;
        for (; index < m_numMotions - 1; index++)
            m_motions[index] = m_motions[index + 1];
        m_numMotions--;
        m_motions = (Motion **)realloc(m_motions, m_numMotions * sizeof(Motion *));
    }
}


// ---- src/MotionLibrary/LoadMotion.cpp
// An inlined strrchr(name, '.').
static inline char *FindExtension(char *s)
{
    char *start = s;
    while (*s++)
        ;
    do {
        s--;
    } while (s != start && *s != '.');
    if (*s == '.')
        return s;
    return 0;
}

// Replaces name's extension, or appends one.
static inline void SetExtension(char *name, char *extension)
{
    char *ext;
    if ((ext = FindExtension(name)))
        strcpy(ext, extension);
    else
        strcat(name, extension);
}

// Loads (or finds) the motion described by a parameter block: from its .MOT
// file, else from the .VUE source, which is then exported as a .MOT.
Motion *MotionLibrary::LoadMotion(Character *character, char *block)
{
    char name[0x110];
    char vueName[0x110];
    char path[0x110];
    char type[0x40];
    Motion *motion;
    int loadedMot;

    character->m_params->ReadParameterBlock(block);
    character->m_params->GetParameter("Name", "NONE", name, -1);
    motion = MotionOfName(name, 0);
    if (motion)
        return motion;
    motion = new Motion;
    motion->m_refCount = 0;
    m_memoryUsed += sizeof(Motion);
#line 250 "MotionLibrary.cpp"
    STRCPY(motion->m_name, name);
    character->m_params->GetParameter("VUE", "NONE", vueName, -1);
#line 254 "MotionLibrary.cpp"
    STRCPY(motion->m_vueName, vueName);
    strcpy(path, "");
    strcat(path, character->m_directory);
    strcat(path, motion->m_vueName);
    character->m_params->GetParameter("FrameCount", 0, &motion->m_frameCount);
    character->m_params->GetParameter("Looping", 0, &motion->m_looping);
    character->m_params->GetParameter("FPS", 15, &motion->m_fps);
    character->m_params->GetParameter("InterpolatePosition", character->m_interpolatePosition, &motion->m_interpolatePosition);
    character->m_params->GetParameter("InterpolateRotation", character->m_interpolateRotation, &motion->m_interpolateRotation);
    if (character->m_params->GetParameter("RotationType", type, -1)) {
        if (!strcasecmp(type, "BYTEAXIS")) {
            motion->m_rotationType = 0;
        } else if (!strcasecmp(type, "SHORTAXIS")) {
            motion->m_rotationType = 1;
        } else {
            PSXprintf("UNKNOWN ROTATION TYPE %s\n", type);
            motion->m_rotationType = character->m_rotationType;
        }
    } else {
        motion->m_rotationType = character->m_rotationType;
    }
    if (character->m_params->GetParameter("PositionType", type, -1)) {
        if (!strcasecmp(type, "LINEAR")) {
            motion->m_positionType = 0;
        } else if (!strcasecmp(type, "BEZIER")) {
            motion->m_positionType = 1;
        } else {
            PSXprintf("UNKNOWN POSITION TYPE %s\n", type);
            motion->m_positionType = character->m_positionType;
        }
    } else {
        motion->m_positionType = character->m_positionType;
    }
    SetExtension(path, ".MOT");
    int loadedVue;
    if (!(loadedMot = LoadMotionFromMOT(path, motion))) {
        SetExtension(path, ".VUE");
        loadedVue = LoadMotionFromVUE(character, path, motion);
    }
    if (!loadedMot) {
        if (!loadedVue) {
#line 304 "MotionLibrary.cpp"
            ASSERTF(0, ("Can't find motion file %s", path));
        } else {
            SetExtension(path, ".MOT");
            ExportMotion(path, motion);
        }
    }
    if (!character->m_params->GetParameter("TransitionTime", 0.0f, &motion->m_transitionTime)) {
        float t = 0.25f * motion->m_duration;
        motion->m_transitionTime = t;
        if (t > 0.5f)
            motion->m_transitionTime = 0.5f;
    }
    AddMotion(motion);
    return m_motions[m_numMotions - 1];
}


// ---- src/MotionLibrary/AddMotion.cpp
void MotionLibrary::AddMotion(Motion *motion)
{
    m_motions = (Motion **)realloc(m_motions, (m_numMotions + 1) * sizeof(Motion *));
    m_motions[m_numMotions] = motion;
    m_numMotions++;
}


// ---- src/MotionLibrary/LoadMotionFromVUE.cpp
// libvu0 sceVu0Normalize, in place.
static inline void Vu0Normalize(_D3DVECTOR *v)
{
    asm("lqc2 vf4, 0x0(%0); vmul.xyz vf5, vf4, vf4; vaddy.x vf5, vf5, vf5y; vaddz.x vf5, vf5, vf5z; vrsqrt Q, vf0w, vf5x; vsub.xyzw vf6, vf0, vf0; vaddw.xyzw vf6, vf6, vf4w; vwaitq; vmulq.xyz vf6, vf4, Q; sqc2 vf6, 0x0(%0)"
        : : "r"(v) : "memory");
}

class BezierPositionController;
void GenerateTangents(BezierPositionController *controller);
void SetMatrixRow(_D3DMATRIX *m, int row, _D3DVECTOR v);

// Reads a .vue animation export: one FRAME block per frame, a TRANSFORM line
// per bone with its three axes and position, which are posed on the
// character's bones and sampled into the motion's tracks.
int MotionLibrary::LoadMotionFromVUE(Character *character, char *name, Motion *motion)
{
// Declaration order chosen for retail's register assignment.
    int frames;
    float time;
    char *token;
    long file;
    _D3DMATRIX *matrices;
    int numTransforms;
    char line[0x100];
    int j;
    int rotation;
    int frame;
    int used[0x100];
    int i;
    int bone;
    int position;
    file = LpDataAccess->FOpen(name, "r");
    if (file == -1) {
        PSXprintf("Unable to open %s\n", name);
        return 0;
    }
    numTransforms = 0;
    frames = 0;
    memset(used, 0, 0x100);   // retail clears only the first 64 entries
    while (LpDataAccess->FGets(file, line, 0xFF)) {
        strupr(line);
        token = strtok(line, " ");
        if (!strcasecmp(token, "FRAME")) {
            frames++;
        } else if (!strcasecmp(token, "TRANSFORM")) {
            if (frames == 1)
                numTransforms++;
            token = strtok(0, " \"");
            int index = character->GetIndexOf(token);
            if (index != 0xFF && frames == 1) {
                used[index] = 1;
                if (character->m_bones[index].m_hasRotation)
                    motion->m_numRotations++;
                if (character->m_bones[index].m_hasPosition)
                    motion->m_numPositions++;
            }
        }
    }
    if (frames < motion->m_frameCount || !motion->m_frameCount)
        motion->m_frameCount = frames;
    motion->m_duration = (float)(motion->m_frameCount - 1) / (float)motion->m_fps;
    matrices = (_D3DMATRIX *)malloc(character->m_numBones << 6);
    for (i = 0; i < character->m_numBones; i++) {
        memset(&matrices[i], 0, 0x40);
        matrices[i]._11 = matrices[i]._22 = matrices[i]._33 = matrices[i]._44 = 1.0f;
    }
    motion->AllocateRotations(motion->m_numRotations);
    motion->AllocatePositions(motion->m_numPositions);
    for (i = 0; i < motion->m_numPositions; i++) {
        motion->m_positions[i]->AllocateKeys(motion->m_frameCount);
        motion->m_positions[i]->m_interpolate = motion->m_interpolatePosition;
    }
    for (i = 0; i < motion->m_numRotations; i++) {
        motion->m_rotations[i]->AllocateKeys(motion->m_frameCount);
        motion->m_rotations[i]->m_4 = motion->m_interpolateRotation;
    }
    rotation = 0;
    position = 0;
    for (i = 0; i < character->m_numBones; i++) {
        if (used[i]) {
            if (character->m_bones[i].m_hasRotation)
                motion->m_rotations[rotation++]->m_bone = i;
            if (character->m_bones[i].m_hasPosition)
                motion->m_positions[position++]->m_bone = i;
        }
    }
    LpDataAccess->FSeek(file, 0, 0);
    _D3DVECTOR front;
    _D3DVECTOR up;
    _D3DVECTOR pos;
    frame = 0;
    while (LpDataAccess->FGets(file, line, 0xFF) && frame < motion->m_frameCount) {
        strupr(line);
        strtok(line, " ");
        for (j = 0; j < numTransforms; j++) {
            LpDataAccess->FGets(file, line, 0xFF);
            strtok(line, " ");
            token = strtok(0, " \"");
            bone = character->GetIndexOf(token);
            if (bone == 0xFF)
                continue;
            strtok(0, " ");
            strtok(0, " ");
            strtok(0, " ");
            token = strtok(0, " ");
            front.x = atof(token);
            token = strtok(0, " ");
            front.z = atof(token);
            token = strtok(0, " ");
            front.y = atof(token);
            Vu0Normalize(&front);
            token = strtok(0, " ");
            up.x = atof(token);
            token = strtok(0, " ");
            up.z = atof(token);
            token = strtok(0, " ");
            up.y = atof(token);
            Vu0Normalize(&up);
            token = strtok(0, " ");
            pos.x = atof(token);
            token = strtok(0, " ");
            pos.z = atof(token);
            token = strtok(0, " ");
            pos.y = atof(token);
            Vu0OrientMatrix(&matrices[bone], &pos, front, up, 1, 1);
        }
        for (i = 0; i < character->m_numBones; i++) {
            if (!character->m_bones[i].m_hasRotation && !character->m_bones[i].m_hasPosition)
                continue;
            if (!character->m_bones[i].m_hasPosition) {
                pos = character->m_bones[i].m_object->GetPosition(character->m_root);
                SetMatrixRow(&matrices[i], 3, pos);
            }
            character->m_bones[i].m_object->SetMatrix(character->m_root, &matrices[i]);
        }
        time = (float)frame / (float)motion->m_fps;
        for (i = 0; i < motion->m_numPositions; i++) {
            character->m_bones[motion->m_positions[i]->m_bone].m_object->GetPosition(&pos);
            motion->m_positions[i]->SetPosition(&pos, time);
        }
        for (i = 0; i < motion->m_numRotations; i++) {
            Quaternion q;
            character->m_bones[motion->m_rotations[i]->m_bone].m_object->GetOrientation(&q);
            motion->m_rotations[i]->SetQuaternion(&q, time);
        }
        frame++;
    }
    LpDataAccess->FClose(file);
    // Emits nothing, but gives mwcc's block optimizer the event retail's
    // delay-slot fills need.
    if (frame) frame = frame; else frame = frame;
    if (motion->m_positionType == 1) {
        for (i = 0; i < motion->m_numPositions; i++)
            GenerateTangents((BezierPositionController *)motion->m_positions[i]);
    }
    return 1;
}


// ---- src/MotionLibrary/MotionOfName.cpp
Motion *MotionLibrary::MotionOfName(char *name, int warn)
{
    for (int i = 0; i < m_numMotions; i++) {
        if (strcasecmp(m_motions[i]->m_name, name) == 0)
            return m_motions[i];
    }
    if (warn)
        PSXprintf("Unable to find motion %s\n", name);
    return 0;
}


// ---- src/MotionLibrary/LoadMotionFromMOT.cpp
// Reads a .mot file (see ExportMotion). A negative first word is the format
// version, followed by the real one; version 2 stores rotation keys in bulk,
// older files key by key. (The redundant (void *) casts on the key buffers
// give retail's load order.)
int MotionLibrary::LoadMotionFromMOT(char *name, Motion *motion)
{
    long file = LpDataAccess->FOpen(name, "rb");
    if (file == -1) {
        PSXprintf("Unable to open %s\n", name);
        return 0;
    }
    int version = 1;
    LpDataAccess->FRead(file, &motion->m_frameCount, 4);
    if (motion->m_frameCount < 0) {
        version = -motion->m_frameCount;
        LpDataAccess->FRead(file, &motion->m_frameCount, 4);
    }
    LpDataAccess->FRead(file, &motion->m_duration, 4);
    LpDataAccess->FRead(file, &motion->m_rotationType, 4);
    LpDataAccess->FRead(file, &motion->m_positionType, 4);
    LpDataAccess->FRead(file, &motion->m_numPositions, 4);
    LpDataAccess->FRead(file, &motion->m_numRotations, 4);
    motion->AllocateRotations(motion->m_numRotations);
    motion->AllocatePositions(motion->m_numPositions);
    int i;
    for (i = 0; i < motion->m_numPositions; i++) {
        LpDataAccess->FRead(file, &motion->m_positions[i]->m_numKeys, 4);
        LpDataAccess->FRead(file, &motion->m_positions[i]->m_bone, 4, 1);
        motion->m_positions[i]->AllocateKeys(motion->m_positions[i]->m_numKeys);
        if (motion->m_positionType == 0)
            LpDataAccess->FRead(file, (void *)((LinearPositionController *)motion->m_positions[i])->m_keys, motion->m_positions[i]->m_numKeys * 16, 1);
        else if (motion->m_positionType == 1)
            LpDataAccess->FRead(file, (void *)((BezierPositionController *)motion->m_positions[i])->m_keys, motion->m_positions[i]->m_numKeys * 40, 1);
    }
    float time;
    Quaternion rotation;
    for (i = 0; i < motion->m_numRotations; i++) {
        LpDataAccess->FRead(file, &motion->m_rotations[i]->m_numKeys, 4, 1);
        LpDataAccess->FRead(file, &motion->m_rotations[i]->m_bone, 4, 1);
        motion->m_rotations[i]->AllocateKeys(motion->m_rotations[i]->m_numKeys);
        if (version == 2) {
            if (motion->m_rotationType == 0)
                LpDataAccess->FRead(file, (void *)((ByteAxisRotationController *)motion->m_rotations[i])->m_keys, motion->m_rotations[i]->m_numKeys * 8, 1);
            else if (motion->m_rotationType == 1)
                LpDataAccess->FRead(file, (void *)((ShortAxisRotationController *)motion->m_rotations[i])->m_keys, motion->m_rotations[i]->m_numKeys * 12, 1);
        } else {
            for (int k = 0; k < motion->m_rotations[i]->m_numKeys; k++) {
                LpDataAccess->FRead(file, &time, 4);
                LpDataAccess->FRead(file, &rotation.x, 4);
                LpDataAccess->FRead(file, &rotation.y, 4);
                LpDataAccess->FRead(file, &rotation.z, 4);
                LpDataAccess->FRead(file, &rotation.w, 4);
                motion->m_rotations[i]->SetKey(k, &time, &rotation);
            }
        }
    }
    LpDataAccess->FClose(file);
    return 1;
}


// ---- src/MotionLibrary/ExportMotion.cpp
extern "C" void *memcpy(void *dst, const void *src, unsigned int n);

// Writes a motion out as a .mot-style blob: a 7-word header, then each
// rotation and position track as (key count, bone, keys).
void MotionLibrary::ExportMotion(char *name, Motion *motion)
{
    if (!m_exportEnabled)
        return;
    int size = 16;
    size += 12;
    int i;
    for (i = 0; i < motion->m_numPositions; i++) {
        size += 8;
        if (motion->m_positionType == 0)
            size += motion->m_positions[i]->m_numKeys * 16;
        else if (motion->m_positionType == 1)
            size += motion->m_positions[i]->m_numKeys * 40;
    }
    for (i = 0; i < motion->m_numRotations; i++) {
        size += 8;
        if (motion->m_rotationType == 0)
            size += motion->m_rotations[i]->m_numKeys * 8;
        else if (motion->m_rotationType == 1)
            size += motion->m_rotations[i]->m_numKeys * 12;
    }
    char *buffer = new char[size];
    char *p = buffer;
    int version = -2;
    memcpy(p, &version, 4);
    memcpy(p + 4, &motion->m_frameCount, 4);
    memcpy(p + 8, &motion->m_duration, 4);
    memcpy(p + 12, &motion->m_rotationType, 4);
    memcpy(p + 16, &motion->m_positionType, 4);
    memcpy(p + 20, &motion->m_numPositions, 4);
    memcpy(p + 24, &motion->m_numRotations, 4);
    p += 28;
    for (i = 0; i < motion->m_numPositions; i++) {
        memcpy(p, &motion->m_positions[i]->m_numKeys, 4);
        memcpy(p + 4, motion->m_positions[i], 4);
        p += 8;
        if (motion->m_positionType == 0) {
            int n = motion->m_positions[i]->m_numKeys * 16;
            memcpy(p, ((LinearPositionController *)motion->m_positions[i])->m_keys, n);
            p += n;
        } else if (motion->m_positionType == 1) {
            int n = motion->m_positions[i]->m_numKeys * 40;
            memcpy(p, ((BezierPositionController *)motion->m_positions[i])->m_keys, n);
            p += n;
        }
    }
    for (i = 0; i < motion->m_numRotations; i++) {
        memcpy(p, &motion->m_rotations[i]->m_numKeys, 4);
        memcpy(p + 4, motion->m_rotations[i], 4);
        p += 8;
        if (motion->m_rotationType == 0) {
            int n = motion->m_rotations[i]->m_numKeys * 8;
            memcpy(p, ((ByteAxisRotationController *)motion->m_rotations[i])->m_keys, n);
            p += n;
        } else if (motion->m_rotationType == 1) {
            int n = motion->m_rotations[i]->m_numKeys * 12;
            memcpy(p, ((ShortAxisRotationController *)motion->m_rotations[i])->m_keys, n);
            p += n;
        }
    }
    long file = LpDataAccess->FOpen(name, "wb");
    LpDataAccess->FWrite(file, buffer, size, 1);
    LpDataAccess->FClose(file);
    if (buffer)
        delete[] buffer;
}
