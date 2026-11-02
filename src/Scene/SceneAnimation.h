// A scene's animated objects (Scene::ReadAnimationData builds them; Scene::Tick
// and Scene::RewindAnimation run them). Scene +0x1AC points at a
// SceneAnimations: the animation array and the random sets drawn from it.
// Each animation is a character, a car or a boat (bits 3..5 of +0x45).
#ifndef SCENE_SCENEANIMATION_H
#define SCENE_SCENEANIMATION_H

#include "d3dtypes.h"

class BoatProcedural;
class CarProcedural;
class CollisionCharacter;
class Motion;

// A wake point of an animated character: when motion m_motion reaches frame
// m_frame, the water gets a push of m_force at m_offset (character space).
// PSXWaterDisplacement has its own, unrelated WakePoint.
struct SceneWakePoint {                 // 0x30 bytes
    int m_motion;                       // +0x00
    int m_frame;                        // +0x04
    char pad8[0x10 - 0x8];
    _D3DVECTOR m_offset;                // +0x10
    float m_force;                      // +0x20
    char pad24[0x30 - 0x24];
};

struct SceneAnimation {                 // 0x50 bytes
    _D3DVECTOR m_position;              // +0x00
    CollisionCharacter *m_character;    // +0x10
    CarProcedural *m_car;               // +0x14
    BoatProcedural *m_boat;             // +0x18
    float m_time;                       // +0x1C
    int m_numMotions;                   // +0x20
    Motion **m_motions;                 // +0x24
    signed char m_motion;               // +0x28
    signed char m_numSequences;         // +0x29
    signed char m_sequence;             // +0x2A
    signed char *m_sequences;           // +0x2C, 1-based motion numbers
    char m_name[0x45 - 0x30];           // +0x30
    unsigned char m_inRandomSet : 1;    // +0x45 bit 0
    unsigned char m_automaticSequence : 1;
    unsigned char m_loopingSequence : 1;
    unsigned char m_isCharacter : 1;    // +0x45 bit 3
    unsigned char m_isCar : 1;          // +0x45 bit 4
    unsigned char m_isBoat : 1;         // +0x45 bit 5
    unsigned char m_excludeFromCamera : 1;
    unsigned char m_x45_bit7 : 1;
    char pad46[0x48 - 0x46];
    int m_numWakePoints;                // +0x48
    SceneWakePoint *m_wakePoints;       // +0x4C
};

struct RandomSet {                      // 0x10 bytes
    signed char m_count;                // +0x00
    SceneAnimation *m_animations;       // +0x04
    signed char *m_anims;               // +0x08
    signed char *m_motions;             // +0x0C
};

struct SceneAnimations {
    int m_count;                        // +0x00
    SceneAnimation *m_animations;       // +0x04
    int m_numRandomSets;                // +0x08
    RandomSet *m_randomSets;            // +0x0C
};

#endif
