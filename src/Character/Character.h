// Character: an animated, motion-driven hierarchy (D3DIMSoultreeCharacter
// derives from it). GameObject is a *virtual* base: mwcc puts the
// virtual-base pointer at +0x00, Character's data from +0x04, its vptr at
// +0x1A4 (declared after the data) and the GameObject subobject at +0x1A8,
// so Character is 0x1E0 bytes (as the copies that declare it in full measure).
// Names follow Character's own methods (src/Character/), otherwise the most
// common name among the files that declared it locally.
#ifndef CHARACTER_CHARACTER_H
#define CHARACTER_CHARACTER_H

#include "GameObject/GameObject.h"

struct _D3DMATRIX;
struct CharacterBone;
class Motion;
class MotionFilter;
class ParameterBlock;
class SoultreeObject;

class Character : public virtual GameObject {
public:
    Character(int enabled);
    GameObject *Create(int);
    int LoadMotions();
    void LoadMirrorList();
    void LoadVUT();
    unsigned char GetIndexOf(char *name);
    void ApplyDefaultFrame();
    void PerformActionByMotion(Motion *motion);
    void PerformActionByName(char *name);
    void RestartAction();
    void TickBlendedFrame(float dt, int mirror, MotionFilter *filter);
    void TickActionByPercentage(Motion *motion, float percent, int mirror);
    void TickBlendedActionsByPercentage(Motion *motion1, Motion *motion2, float percent, float blend, int mirror);
    void TransitionToMotion(Motion *motion, float blendTime, int keepPhase);

    SoultreeObject *m_root;             // +0x04
    ParameterBlock *m_params;           // +0x08
    int m_mirrorAxis;                   // +0x0C
    int m_x10;                          // +0x10 (1 from the ctor)
    int m_numMotions;                   // +0x14
    Motion *m_motion;                   // +0x18, current
    Motion *m_previousMotion;           // +0x1C
    int m_interpolatePosition;          // +0x20
    int m_interpolateRotation;          // +0x24
    int m_rotationType;                 // +0x28
    int m_positionType;                 // +0x2C
    int m_blending;                     // +0x30
    float m_frame;                      // +0x34
    float m_blendStart;                 // +0x38
    float m_blendEnd;                   // +0x3C
    float m_previousFrame;              // +0x40
    float m_blendTime;                  // +0x44
    char m_fileName[0x50];              // +0x48
    char m_vutFilename[0x50];           // +0x98
    char m_mirFilename[0x50];           // +0xE8
    char m_directory[0x50];             // +0x138
    int m_keepMotions;                  // +0x188
    Motion **m_motions;                 // +0x18C
    int m_numBones;                     // +0x190
    CharacterBone *m_bones;             // +0x194
    int *m_mirrorBones;                 // +0x198
    _D3DMATRIX *m_defaultFrame;         // +0x19C, malloc'd
    int m_vutLoaded;                    // +0x1A0

    // --- own virtuals in retail slot order (retail vtable);
    // vptr at +0x1A4 ---
    virtual ~Character();
    virtual void LoadParts();
    virtual void Init();
    virtual void SetDefaultFrame();
    virtual void Clone(Character *other);
};

#endif
