// Motnctrl.cpp  @ 0x0025CC40 (text 0x25CC40-0x25EF80)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp)
//
// Retail's Motnctrl.cpp as one translation unit: the Character methods,
// unanchored at the end of LightEmitter.cpp's tu_map row (their ASSERTs say
// "Motnctrl.cpp"; its strings restart at @927 after Math3d.cpp's and end
// with Character's RTTI at 0x415F58). __sinit_Motnctrl.cpp (the .ctor slot
// after MathQuaternion.cpp's) builds only the axis vectors. `make link`
// places its data as well as its code.


// The vector operators and the mirror helpers nest inlines.
#pragma inline_depth(5)

#include "d3dtypes.h"
#include "soultree_assert_block.h"
#include "DataAccess/DataAccess.h"
#include "Character/Character.h"
#include "Motion/Motion.h"
#include "PositionController/PositionController.h"
#include "RotationController/RotationController.h"
#include "MotionLibrary/MotionLibrary.h"
#include "GameObject/GameObject.h"
#include "ParameterBlock/ParameterBlock.h"
#include "Quaternion/Quaternion.h"
#include "SoultreeObject/SoultreeObject.h"
#include "sinit/AxisVectors.h"

extern "C" char *strcpy(char *dst, const char *src);
extern "C" char *strcat(char *dst, const char *src);
extern "C" char *strncpy(char *dst, const char *src, unsigned int n);
extern "C" char *strupr(char *s);
extern "C" int sprintf(char *buf, const char *fmt, ...);
extern "C" int strcasecmp(const char *a, const char *b);
extern "C" unsigned int strlen(const char *s);
extern "C" char *strtok(char *s, const char *delim);
extern "C" void *malloc(unsigned int size);
extern "C" void free(void *p);
void PSXprintf(const char *fmt, ...);

extern DataAccess *LpDataAccess;
extern MotionLibrary *LpMotionLibrary;

// One bone of a Character (Character::m_bones, 0x40 bytes; Character.h only
// forward-declares it). The same layout as D3DIMSoultreeMotnctrl.cpp's.
struct CharacterBone {
    char m_name[0x32];                  // +0x00, the VUT part name (LoadVUT copies sizeof = 0x32)
    char pad32[0x34 - 0x32];
    SoultreeObject *m_object;           // +0x34
    int m_x38;                          // +0x38
    int m_x3C;                          // +0x3C, 1 from LoadVUT
};

// Per-bone skip flags for TickBlendedFrame.
class MotionFilter {
public:
    int m_skip[1];
};

// The vector part (x, y, z) of a quaternion, as TickActionByPercentage's
// local Quaternion::GetVector returned it.
static inline _D3DVECTOR QuaternionVector(Quaternion *q)
{
    return _D3DVECTOR(q->x, q->y, q->z);
}

// ---- src/Character/LoadVUT.cpp

// Reads the VUT file: "<bone>=<name>[,T]" per line.
void Character::LoadVUT()
{
    // Retail works through a copy of `this` (it takes $s0, ahead of the
    // locals; mwcc colours parameters after locals).
    Character *self = this;
    char *token;
    long file;
    int index;
    int n2;
    int len2;
    char line[0x100];
    int n1;
    int len1;

    if (self->m_vutLoaded)
        return;
    n1 = sizeof(line);
    if (n1 == sizeof(char *)) {
#line 65 "Motnctrl.cpp"
        ASSERT(0, "STRCPY: len of string is sizeof(char*)");
    }
    len1 = strlen(self->m_directory);
    n1 = (n1 - 1 < len1) ? n1 - 1 : len1;
    strncpy(line, self->m_directory, n1);
    line[n1] = 0;
    strcat(line, self->m_vutFilename);
    file = LpDataAccess->FOpen(line, "r");
    if (file == -1) {
        PSXprintf("Unable to open VUT file -");
        PSXprintf(line);
        PSXprintf("\n");
        return;
    }
    while (LpDataAccess->FGets(file, line, 0xFF)) {
        strupr(line);
        token = strtok(line, "=,");
        index = self->GetIndexOf(token);
        if (index == 0xFF) {
            PSXprintf("Hmmm, VUT string invalid...\n");
            continue;
        }
        // Retail's first delimiter literal is 6 bytes ("=, \n" and an extra NUL,
        // @934), a separate object from the second one's (@935).
        token = strtok(0, "=, \n\0");
        n2 = sizeof(self->m_bones[index].m_name);
        if (n2 == sizeof(char *)) {
#line 85 "Motnctrl.cpp"
            ASSERT(0, "STRCPY: len of string is sizeof(char*)");
        }
        len2 = strlen(token);
        n2 = (n2 - 1 < len2) ? n2 - 1 : len2;
        strncpy(self->m_bones[index].m_name, token, n2);
        self->m_bones[index].m_name[n2] = 0;
        self->m_bones[index].m_x38 = 0;
        self->m_bones[index].m_x3C = 1;
        token = strtok(0, "=, \n");
        if (token) {
            char *p = token;
            while (*p) {
                if (*p >= 'a' && *p <= 'z')
                    *p -= 0x20;
                p++;
            }
            if (token[0] == 'T')
                self->m_bones[index].m_x38 = 1;
        }
    }
    LpDataAccess->FClose(file);
    self->m_vutLoaded = 1;
}

// ---- src/Character/LoadMirrorList.cpp

// Reads "<bone>=<bone>" pairs naming each bone's left/right mirror.
#pragma alignlabel off
void Character::LoadMirrorList()
{
    char line[0x100];
    long file;
    int bone;
    int mirror;
#line 115 "Motnctrl.cpp"
    STRCPY(line, m_directory);
    strcat(line, m_mirFilename);
    file = LpDataAccess->FOpen(line, "r");
    if (file != -1) {
        m_mirrorBones = new int[m_numBones];
        while (LpDataAccess->FGets(file, line, 0xFF)) {
            strupr(line);
            bone = GetIndexOf(strtok(line, "=,"));
            mirror = GetIndexOf(strtok(0, "=, \n"));
            m_mirrorBones[bone] = mirror;
            m_mirrorBones[mirror] = bone;
        }
        LpDataAccess->FClose(file);
    }
}
#pragma alignlabel reset

// ---- src/Character/LoadMotions.cpp

int Character::LoadMotions()
{
    char name[80];
    m_motion = 0;
    if (!m_numMotions)
        return 0;
    if (m_keepMotions)
        m_motions = (Motion **)malloc(m_numMotions * sizeof(Motion *));
    for (int i = 0; i < m_numMotions; i++) {
        sprintf(name, "MOTION %i", i + 1);
        Motion *motion = LpMotionLibrary->LoadMotion(this, name);
        if (motion) {
            motion->m_refCount++;
            if (m_keepMotions)
                m_motions[i] = motion;
        }
    }
    return 1;
}

// ---- src/Character/ctor.cpp

Character::Character(int enabled) : GameObject(enabled)
{
    m_x10 = 1;
    m_mirrorAxis = 0;
    m_numMotions = 0;
    m_motion = 0;
    m_frame = 0;
    m_interpolatePosition = 1;
    m_interpolateRotation = 1;
    m_rotationType = 0;
    m_positionType = 1;
    m_previousMotion = 0;
    m_blendStart = 0;
    m_blending = 0;
    strcpy(m_fileName, "");
    strcpy(m_vutFilename, "");
    strcpy(m_mirFilename, "");
    strcpy(m_directory, "");
    m_defaultFrame = 0;
    m_vutLoaded = 0;
    m_keepMotions = 1;
    m_motions = 0;
    m_mirrorBones = 0;
}

// ---- src/Character/Create.cpp

GameObject *Character::Create(int)
{
    return GameObject::Create();
}

// ---- src/Character/Init.cpp

extern char AnimContentDirectory[];

// Reads the "General info" block of the character's parameter file, then
// loads its parts and motions.
void Character::Init()
{
    char type[0x40];
    char axis[0x40];

    m_params->ReadParameterBlock("General info");
    m_params->GetParameter("ContentDirectory", "", m_directory, -1);
    if (!strlen(m_directory))
        strcpy(m_directory, AnimContentDirectory);
    m_params->GetParameter("VUTFilename", "\\", m_vutFilename, -1);
    m_params->GetParameter("MIRFilename", "\\", m_mirFilename, -1);
    m_params->GetParameter("NumberOfMotions", 0, &m_numMotions);
    m_params->GetParameter("InterpolatePosition", 1, &m_interpolatePosition);
    m_params->GetParameter("InterpolateRotation", 1, &m_interpolateRotation);
    if (m_params->GetParameter("RotationType", type, -1)) {
        if (!strcasecmp(type, "BYTEAXIS")) {
            m_rotationType = 0;
        } else if (!strcasecmp(type, "SHORTAXIS")) {
            m_rotationType = 1;
        } else {
            PSXprintf("UNKNOWN ROTATION TYPE %s\n", type);
            m_rotationType = 0;
        }
    } else {
        m_rotationType = 0;
    }
    if (m_params->GetParameter("PositionType", type, -1)) {
        if (!strcasecmp(type, "LINEAR")) {
            m_positionType = 0;
        } else if (!strcasecmp(type, "BEZIER")) {
            m_positionType = 1;
        } else {
            PSXprintf("UNKNOWN POSITION TYPE %s\n", type);
            m_positionType = 1;
        }
    }
    // Retail bug: the axis is read into its own buffer but the comparisons
    // still look at the position type string.
    if (m_params->GetParameter("MirrorAxis", axis, -1)) {
        if (!strcasecmp(type, "XAXIS"))
            m_mirrorAxis = 0;
        else if (!strcasecmp(type, "YAXIS"))
            m_mirrorAxis = 1;
        else if (!strcasecmp(type, "ZAXIS"))
            m_mirrorAxis = 2;
    } else {
        m_mirrorAxis = 0;
    }
    LoadParts();
    SetDefaultFrame();
    LoadMotions();
    ApplyDefaultFrame();
}

// ---- src/Character/dtor.cpp

Character::~Character()
{
    m_motion = 0;
    if (m_mirrorBones)
        delete[] m_mirrorBones;
    if (m_defaultFrame)
        free(m_defaultFrame);
    if (m_motions) {
        for (int i = 0; i < m_numMotions; i++)
            LpMotionLibrary->ReleaseMotion(m_motions[i]);
        free(m_motions);
    }
}

// ---- src/Character/LoadParts.cpp

void Character::LoadParts()
{
    // Base characters have no parts to load.
    PSXprintf("Loading WRONG part");
}

// ---- src/Character/GetIndexOf.cpp

unsigned char Character::GetIndexOf(char *name)
{
    for (int i = 0; i != m_numBones; i++) {
        if (!strcasecmp(m_bones[i].m_name, name))
            return i;
    }
    PSXprintf("Couldn't find index for ");
    PSXprintf(name);
    PSXprintf("============\n");
    return 0xFF;
}

// ---- src/Character/TickBlendedFrame.cpp
// The vector operators and the mirror helpers nest inlines.

void FastInterpolateQuaternion(Quaternion *out, Quaternion *a, Quaternion *b, float t);

// Character as in src/Character/Character.h, plus this method.

// Mirrors a bone position across the root's axis. As an inline helper its
// TransformVector temporaries get retail's (reversed) stack slots.
static inline void MirrorPosition(_D3DVECTOR *p, SoultreeObject *root, SoultreeObject *parent, int axis)
{
    *p = parent->TransformVectorLocalToWorld(p);
    *p = root->TransformVectorWorldToLocal(p);
    if (axis == 0)
        p->x = -p->x;
    else if (axis == 1)
        p->y = -p->y;
    else if (axis == 2)
        p->z = -p->z;
    *p = root->TransformVectorLocalToWorld(p);
    *p = parent->TransformVectorWorldToLocal(p);
}

// Mirrors a rotation: negates the vector part, then the mirrored axis back.
static inline void MirrorRotation(Quaternion *q, int axis)
{
    _D3DVECTOR v(q->x, q->y, q->z);
    if (axis == 0)
        v.x = -v.x;
    else if (axis == 1)
        v.y = -v.y;
    else if (axis == 2)
        v.z = -v.z;
    q->x = -v.x;
    q->y = -v.y;
    q->z = -v.z;
}

// Hermite ease between lo and hi.
static inline float SmoothStep(float x, float lo, float hi)
{
    if (x < lo)
        return lo;
    if (!(x < hi))
        return hi;
    float s = (x - lo) / (hi - lo);
    return (3.0f - 2.0f * s) * (s * s);
}

// Advances the current motion by dt and poses the bones; while blending,
// mixes in the previous motion over m_blendTime.
void Character::TickBlendedFrame(float dt, int mirror, MotionFilter *filter)
{
    static int TickBlendedID = -1;      // unused profiler ID
    int finished = 0;
    int i;
    int j;
    if (m_x10)
        return;
    m_frame += dt;
    // A switch, not an if: its empty case gives the block optimizer the event
    // that makes retail's delay-slot fills.
    switch (m_motion->PastEndOfMotion(m_frame)) {
    case 0:
        break;
    default:
        if (m_motion->m_looping)
            m_frame -= m_motion->m_duration;
        else
            finished = 1;
        break;
    }
    if (!m_blending) {
        for (i = 0; i < m_motion->m_numRotations; i++) {
            if (filter && filter->m_skip[m_motion->m_rotations[i]->m_bone])
                continue;
            Quaternion q;
            m_motion->m_rotations[i]->GetQuaternion(m_frame, &q);
            if (mirror) {
                MirrorRotation(&q, m_mirrorAxis);
                m_bones[m_mirrorBones[m_motion->m_rotations[i]->m_bone]].m_object->SetOrientation(&q);
            } else {
                m_bones[m_motion->m_rotations[i]->m_bone].m_object->SetOrientation(&q);
            }
        }
        for (j = 0; j < m_motion->m_numPositions; j++) {
            if (filter && filter->m_skip[m_motion->m_positions[j]->m_bone])
                continue;
            _D3DVECTOR p;
            m_motion->m_positions[j]->GetPosition(m_frame, &p);
            if (mirror) {
                MirrorPosition(&p, m_root, m_bones[m_mirrorBones[m_motion->m_positions[j]->m_bone]].m_object->m_parent, m_mirrorAxis);
                m_bones[m_mirrorBones[m_motion->m_positions[j]->m_bone]].m_object->SetPosition(&p);
            } else {
                m_bones[m_motion->m_positions[j]->m_bone].m_object->SetPosition(&p);
            }
        }
    } else {
        m_blendStart += dt;
        float w = SmoothStep((m_blendStart - m_previousFrame - dt) / m_blendTime, 0.0f, 1.0f);
        // A loop-scoped counter here gives retail's $s3/$s4 split.
        for (int k = 0; k < m_motion->m_numRotations; k++) {
            if (filter && filter->m_skip[m_motion->m_rotations[k]->m_bone])
                continue;
            Quaternion q1;
            Quaternion q2;
            m_motion->m_rotations[k]->GetQuaternion(m_frame, &q1);
            m_previousMotion->m_rotations[k]->GetQuaternion(m_blendStart, &q2);
            FastInterpolateQuaternion(&q2, &q1, &q1, w);
            if (mirror) {
                MirrorRotation(&q1, m_mirrorAxis);
                m_bones[m_mirrorBones[m_motion->m_rotations[k]->m_bone]].m_object->SetOrientation(&q1);
            } else {
                m_bones[m_motion->m_rotations[k]->m_bone].m_object->SetOrientation(&q1);
            }
        }
        w = 1.0f - w;
        for (j = 0; j < m_motion->m_numPositions; j++) {
            if (filter && filter->m_skip[m_motion->m_positions[j]->m_bone])
                continue;
            _D3DVECTOR p1;
            m_motion->m_positions[j]->GetPosition(m_frame, &p1);
            _D3DVECTOR p2;
            m_previousMotion->m_positions[j]->GetPosition(m_blendStart, &p2);
            p1 = p1 + w * (p2 - p1);
            if (mirror) {
                MirrorPosition(&p1, m_root, m_bones[m_mirrorBones[m_motion->m_positions[j]->m_bone]].m_object->m_parent, m_mirrorAxis);
                m_bones[m_mirrorBones[m_motion->m_positions[j]->m_bone]].m_object->SetPosition(&p1);
            } else {
                m_bones[m_motion->m_positions[j]->m_bone].m_object->SetPosition(&p1);
            }
        }
    }
    if (!(m_blendStart < m_blendEnd)) {
        m_blending = 0;
        m_blendStart = 0.0f;
    }
    if (finished) {
        m_x10 = 1;
        m_frame = 0.0f;
        m_blendStart = 0.0f;
    }
}

// ---- src/Character/ApplyDefaultFrame.cpp

void Character::ApplyDefaultFrame()
{
    for (int i = 0; i < m_numBones; i++)
        m_bones[i].m_object->SetMatrix(&m_defaultFrame[i]);
}

// ---- src/Character/SetDefaultFrame.cpp

void Character::SetDefaultFrame()
{
    if (!m_defaultFrame)
        m_defaultFrame = (_D3DMATRIX *)malloc(m_numBones * sizeof(_D3DMATRIX));
    for (int i = 0; i < m_numBones; i++)
        m_bones[i].m_object->GetMatrix(&m_defaultFrame[i]);
}

// ---- src/Character/PerformActionByName.cpp

void Character::PerformActionByName(char *name)
{
    PerformActionByMotion(LpMotionLibrary->MotionOfName(name, 1));
}

// ---- src/Character/PerformActionByMotion.cpp

void Character::PerformActionByMotion(Motion *motion)
{
    m_x10 = 0;
    m_motion = motion;
    m_blending = 0;
    m_frame = 0;
}

// ---- src/Character/RestartAction.cpp

void Character::RestartAction()
{
    m_x10 = 0;
    m_blending = 0;
    m_frame = 0;
    m_blendStart = 0;
}

// ---- src/Character/TransitionToMotion.cpp
#pragma fast_fptosi on

void Character::TransitionToMotion(Motion *motion, float blendTime, int keepPhase)
{
    int frame;  // only set (and used) when keepPhase
    if (keepPhase)
        frame = (int)(motion->m_duration * (m_frame / m_motion->m_duration));
    if (blendTime != 0.0f) {
        m_previousFrame = m_frame;
        m_blendStart = m_frame;
        m_blendEnd = m_frame + blendTime;
        m_blendTime = blendTime;
        m_previousMotion = m_motion;
        PerformActionByMotion(motion);
        m_blending = 1;
    } else {
        PerformActionByMotion(motion);
    }
    if (keepPhase)
        m_frame = frame;
}

// ---- src/Character/TickActionByPercentage.cpp
// The vector operators and the mirror helpers nest inlines.

// Mirrors a bone position across the root's axis. As an inline helper its
// TransformVector temporaries get retail's (reversed) stack slots.

// Mirrors a rotation: negates the vector part, then the mirrored axis back.
static inline void MirrorRotationV(Quaternion *q, int axis)
{
    _D3DVECTOR v = QuaternionVector(q);
    if (axis == 0)
        v.x = -v.x;
    else if (axis == 1)
        v.y = -v.y;
    else if (axis == 2)
        v.z = -v.z;
    q->x = -v.x;
    q->y = -v.y;
    q->z = -v.z;
}

void Character::TickActionByPercentage(Motion *motion, float percent, int mirror)
{
    int i;
    if (percent < 0.0f || percent > 1.0f) {
#line 542 "Motnctrl.cpp"
        ASSERTF(0, ("Invalid Action percentage(%f) in Character\n", percent));
    }
    percent *= motion->m_duration;
    for (i = 0; i < motion->m_numRotations; i++) {
        Quaternion q;
        motion->m_rotations[i]->GetQuaternion(percent, &q);
        if (mirror) {
            MirrorRotationV(&q, m_mirrorAxis);
            m_bones[m_mirrorBones[motion->m_rotations[i]->m_bone]].m_object->SetOrientation(&q);
        } else {
            m_bones[motion->m_rotations[i]->m_bone].m_object->SetOrientation(&q);
        }
    }
    for (i = 0; i < motion->m_numPositions; i++) {
        _D3DVECTOR p;
        motion->m_positions[i]->GetPosition(percent, &p);
        if (mirror) {
            MirrorPosition(&p, m_root, m_bones[m_mirrorBones[motion->m_positions[i]->m_bone]].m_object->m_parent, m_mirrorAxis);
            m_bones[m_mirrorBones[motion->m_positions[i]->m_bone]].m_object->SetPosition(&p);
        } else {
            m_bones[motion->m_positions[i]->m_bone].m_object->SetPosition(&p);
        }
    }
}

// ---- src/Character/TickBlendedActionsByPercentage.cpp
// The vector operators and the mirror helpers nest inlines.

void FastInterpolateQuaternion(Quaternion *out, Quaternion *a, Quaternion *b, float t);

// Mirrors a bone position across the root's axis. As an inline helper its
// TransformVector temporaries get retail's (reversed) stack slots.

// Blends two motions at the same point (percent of motion1's length).
void Character::TickBlendedActionsByPercentage(Motion *motion1, Motion *motion2, float percent, float blend, int mirror)
{
    static int TickBlendedID = -1;      // unused profiler ID
    int i;
    float time = motion1->m_duration * percent;
    for (i = 0; i < motion1->m_numRotations; i++) {
        Quaternion q1;
        Quaternion q2;
        motion1->m_rotations[i]->GetQuaternion(time, &q1);
        motion2->m_rotations[i]->GetQuaternion(time, &q2);
        FastInterpolateQuaternion(&q2, &q1, &q2, blend);
        if (mirror) {
            MirrorRotation(&q2, m_mirrorAxis);
            m_bones[m_mirrorBones[motion1->m_rotations[i]->m_bone]].m_object->SetOrientation(&q2);
        } else {
            m_bones[motion1->m_rotations[i]->m_bone].m_object->SetOrientation(&q2);
        }
    }
    for (i = 0; i < motion1->m_numPositions; i++) {
        _D3DVECTOR p1;
        motion1->m_positions[i]->GetPosition(time, &p1);
        _D3DVECTOR p2;
        motion2->m_positions[i]->GetPosition(time, &p2);
        p1 = p2 - blend * (p2 - p1);
        if (mirror) {
            MirrorPosition(&p1, m_root, m_bones[m_mirrorBones[motion1->m_positions[i]->m_bone]].m_object->m_parent, m_mirrorAxis);
            m_bones[m_mirrorBones[motion1->m_positions[i]->m_bone]].m_object->SetPosition(&p1);
        } else {
            m_bones[motion1->m_positions[i]->m_bone].m_object->SetPosition(&p1);
        }
    }
}

// ---- src/Character/Clone.cpp

void Character::Clone(Character *other)
{
    m_x10 = other->m_x10;
    m_numMotions = other->m_numMotions;
    m_motion = other->m_motion;
    m_blendStart = other->m_blendStart;
    m_blending = other->m_blending;
    strcpy(m_fileName, other->m_fileName);
    strcpy(m_vutFilename, other->m_vutFilename);
    strcpy(m_mirFilename, other->m_mirFilename);
    strcpy(m_directory, other->m_directory);
    LoadParts();
    if (!m_defaultFrame)
        m_defaultFrame = (_D3DMATRIX *)malloc(m_numBones * sizeof(_D3DMATRIX));
    for (int i = 0; i < m_numBones; i++)
        m_defaultFrame[i] = other->m_defaultFrame[i];
    m_vutLoaded = 1;
    ApplyDefaultFrame();
}

// ---- src/sinit/Motnctrl.cpp
// __sinit_Motnctrl.cpp builds only the axis vectors (sinit/AxisVectors.h, included above).
