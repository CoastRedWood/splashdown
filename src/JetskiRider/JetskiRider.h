// JetskiRider as far as its matched code needs it. Character virtually
// inherits GameObject; JetskiRider's virtual-base GameObject sits at +0x8A8.
#ifndef JETSKIRIDER_H
#define JETSKIRIDER_H

#include "D3DIMSoultreeCharacter/D3DIMSoultreeCharacter.h"

#include "JetskiStuntsKey/JetskiStuntsKey.h"

#ifdef SPLASHDOWN_PC
// x86-64: the same views, their offsets taken from the real classes
// (PhysicsBody, JetskiVehicle, JetskiPlayerVehicle/JetskiAIVehicle, VehicleAIClass).
#include "JetskiPlayerVehicle/JetskiPlayerVehicle.h"
#include "PhysicsRigidBody/PhysicsRigidBody.h"
#include "VehicleAIClass/VehicleAIClass.h"
#define JR_OFF(c, m) __builtin_offsetof(c, m)
#define JR_GAP(c, m, prev, size) char pad_##m[JR_OFF(c, m) - JR_OFF(c, prev) - (size)]

struct JetskiInput {                // PhysicsRigidBody
    char pad0[JR_OFF(PhysicsBody, m_x30)];
    int m_player;
    JR_GAP(PhysicsBody, m_x60, m_x30, 4);
    float m_x60[4];
    JR_GAP(PhysicsBody, m_object, m_x60, 16);
    SoultreeObject *m_model;
};

struct JetskiKeys {                 // VehicleAIClass
    char pad0[JR_OFF(VehicleAIClass, m_flags)];
    int m_buttons;
};

struct Jetski {                     // JetskiVehicle
    char pad0[JR_OFF(JetskiVehicle, m_ownStatus)];
    struct RaceStatusStruct_Tag *m_status;
    class RaceStatusClass *m_raceStatus;
    JR_GAP(JetskiVehicle, m_x60, m_raceStatus, sizeof(void *));
    float m_x60;
    float m_x64;
    JR_GAP(JetskiVehicle, m_body, m_x64, 4);
    JetskiInput *m_input;
    JR_GAP(JetskiVehicle, m_x1CC, m_body, sizeof(void *));
    int m_replay;
    JR_GAP(JetskiVehicle, m_x2C0, m_x1CC, 4);
    float m_x2C0[4];
    JR_GAP(JetskiVehicle, m_x530, m_x2C0, 16);
    int m_x530;
    JR_GAP(JetskiVehicle, m_x5CC, m_x530, 4);
    float m_x5CC;
    JR_GAP(JetskiVehicle, m_player, m_x5CC, 4);
    int m_player;
    // JetskiPlayerVehicle's m_gateSound, JetskiAIVehicle's m_ai: one pointer
    // past JetskiVehicle (m_competition, VehicleAIInterface's vptr)
    char pad_keys[JR_OFF(JetskiPlayerVehicle, m_gateSound) - JR_OFF(JetskiVehicle, m_player) - 4];
    JetskiKeys *m_keys;
};
static_assert(JR_OFF(Jetski, m_keys) == JR_OFF(JetskiPlayerVehicle, m_gateSound), "Jetski::m_keys");
static_assert(JR_OFF(Jetski, m_player) == JR_OFF(JetskiVehicle, m_player), "Jetski::m_player");
static_assert(JR_OFF(JetskiInput, m_model) == JR_OFF(PhysicsBody, m_object), "JetskiInput::m_model");
#else
struct JetskiInput {
    char pad0[0x30];
    int m_player;                   // +0x30
    char pad34[0x60 - 0x34];
    float m_x60[4];                 // +0x60 (JetskiRider::Tick copies it to the Jetski's +0x2C0)
    char pad70[0x288 - 0x70];
    SoultreeObject *m_model;        // +0x288
};

struct JetskiKeys {
    char pad0[0x38];
    int m_buttons;                  // +0x38
};

struct Jetski {
    char pad0[0x30];
    struct RaceStatusStruct_Tag *m_status; // +0x30
    class RaceStatusClass *m_raceStatus;  // +0x34
    char pad38[0x60 - 0x38];
    float m_x60;                    // +0x60 (AnimateRider)
    float m_x64;                    // +0x64
    char pad68[0x80 - 0x68];
    JetskiInput *m_input;           // +0x80
    char pad84[0x1CC - 0x84];
    int m_replay;                   // +0x1CC
    char pad1D0[0x2C0 - 0x1D0];
    // +0x2C0..+0x2FF: position, ?, forward and up vectors (PlaybackRider);
    // typed as floats, since including d3dtypes.h here moves DisplayStuntText.
    float m_x2C0[4];                // +0x2C0, the rider position (Tick)
    char pad2D0[0x530 - 0x2D0];
    int m_x530;                     // +0x530, JetskiVehicle::m_x530 (wrecked)
    char pad534[0x5CC - 0x534];
    float m_x5CC;                   // +0x5CC
    char pad5D0[0x5E4 - 0x5D0];
    int m_player;                   // +0x5E4
    char pad5E8[0x7B4 - 0x5E8];
    JetskiKeys *m_keys;             // +0x7B4
};

#endif

class JetskiCompetition;
class JetskiRiderSoundEmitter;
class JetskiVehicle;
class LightManager;
class Motion;
class ParameterBlock;
class ParticleManager;
class Scene;
class SearchNode;
class SplashParticleEmitter;
class PSXWater;
class Terrain;
class WaterWrecker;

#define NUM_STUNTS 28

class JetskiRider : public D3DIMSoultreeCharacter {
public:
    JetskiRider(int construct);
    GameObject *Create(JetskiVehicle *vehicle, JetskiCompetition *competition, char *file, char *keysFile,
                       char *soundFile, LightManager *lights, Terrain *terrain, PSXWater *water, int player,
                       Scene *scene, ParticleManager *particles);
    void LoadPhysicsParameters(ParameterBlock *block);
    void CreateWrecker(ParticleManager *particles, SearchNode *node, char *collisionFile, char *suffix);
    void InitializeCollisionData(SearchNode *node);
    void ResetVariables();
    virtual ~JetskiRider();
    int Tick(float);
    int Render3D();
    int KeyDownHandler(ProcessNode *, KeyInfo *);
    void ProcessInputs(float dt);
    void ActivateStunt(int stunt);
    void DisplayStuntText(struct RaceStatusStruct_Tag *status);
    void ActivateWobble(int amount);
    int ActivateWinningAnimation(int place);
    void AnimateRider(float dt);
    void PlaybackRider(float dt);
    void StopSpecialAnimation();

    Terrain *m_terrain;                     // +0x210
    Jetski *m_jetski;                       // +0x214 (retail: the JetskiVehicle Create gets)
    WaterWrecker *m_wrecker;                // +0x218
    JetskiCompetition *m_competition;       // +0x21C
    int m_x220;                             // +0x220
    PSXWater *m_water;                      // +0x224
    char m_file[0x104];                     // +0x228, the .PHY file (the ctor clears it)
    char m_riderName[0x10];                 // +0x32C, from the motion control file name
    int m_player;                           // +0x33C
    float m_x340;                           // +0x340
    SplashParticleEmitter *m_splash;        // +0x344
    JetskiRiderSoundEmitter *m_sounds;      // +0x348
    int m_x34C;                             // +0x34C
    int m_x350;                             // +0x350
    int m_x354;                             // +0x354
    float m_x358;                           // +0x358
    float m_x35C;                           // +0x35C
    float m_x360;                           // +0x360
    float m_x364;                           // +0x364
    float m_x368;                           // +0x368
    Motion *m_raceMotions[3];               // +0x36C, lean forward, neutral, lean back
    Motion *m_placeMotions[3];              // +0x378, 1st, 2nd, 3rd place
    Motion *m_stunts[NUM_STUNTS];           // +0x384, the last is the signature stunt
    Motion *m_wobbles[4];                   // +0x3F4, front, rear, left, right
    int m_x404[32];                         // +0x404
    int m_x484;                             // +0x484
    int m_x488;                             // +0x488
    int m_x48C;                             // +0x48C
    int m_wobbling;                         // +0x490 (ActivateWobble)
    int m_x494;                             // +0x494
    int m_winAnimation;                     // +0x498
    int m_x49C;                             // +0x49C
    int m_x4A0;                             // +0x4A0
    int m_x4A4;                             // +0x4A4
    int m_x4A8;                             // +0x4A8
    int m_x4AC;                             // +0x4AC
    float m_x4B0;                           // +0x4B0
    float m_x4B4;                           // +0x4B4
    JetskiStuntsKey *m_stuntsKey;           // +0x4B8
    float m_stunt4BC;                       // +0x4BC
    float m_stunt4C0;                       // +0x4C0
    float m_stunt4C4;                       // +0x4C4
    float m_stunt4C8;                       // +0x4C8
    float m_stunt4CC;                       // +0x4CC
    float m_stunt4D0;                       // +0x4D0
    float m_stunt4D4;                       // +0x4D4
    float m_table4D8[NUM_STUNTS];           // +0x4D8
    float m_table548[NUM_STUNTS];           // +0x548
    float m_table5B8[NUM_STUNTS];           // +0x5B8
    float m_table628[NUM_STUNTS];           // +0x628
    float m_table698[NUM_STUNTS];           // +0x698
    float m_table708[NUM_STUNTS];           // +0x708
    float m_table778[NUM_STUNTS];           // +0x778
    float m_x7E8[5];                        // +0x7E8, JetskiStuntsKey::m_multiStuntBonus
    int m_stunt7FC;                         // +0x7FC
    int m_table800[NUM_STUNTS];             // +0x800
    int m_x870;                             // +0x870
    float m_x874;                           // +0x874
    float m_blendRate;                      // +0x878
    float m_blendTime;                      // +0x87C
    float m_x880;                           // +0x880
    float m_x884;                           // +0x884
    int m_x888;                             // +0x888
    float m_stuntF;                         // +0x88C
    int m_stunt;                            // +0x890
    float m_x894;                           // +0x894
    float m_x898;                           // +0x898
    int m_x89C;                             // +0x89C
    int m_x8A0;                             // +0x8A0 (ResetVariables)
    int m_x8A4;                             // +0x8A4
};

#endif
