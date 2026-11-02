// Scene: one loaded race scene (its parameter file, zones and search nodes,
// buoys, lights, sounds, stadium and the ambient colour flash). A GameObject,
// data from +0x30, 0xBC0 bytes (`new Scene`, JetskiCompetition); nothing
// past +0xAF0 is known yet. Names follow Scene's own methods (src/Scene/),
// otherwise the most common name among the files that declared it locally.
//
// m_info is the SceneManager that Create was given (its file names and
// m_sceneValid); src/tu/SceneManager.cpp defines both classes.
#ifndef SCENE_SCENE_H
#define SCENE_SCENE_H

#include "GameObject/GameObject.h"
#include "d3dtypes.h"
#include "LightEmitter/_D3DCOLORVALUE.h"
#include "ContainerList/ContainerList.h"

class AuralScape;
class BuoyObject;
class CollisionObject;
class ParameterBlock;
class ParticleManager;
class Portal;
class PSXSound;
class PSXSoundGroup;
class SceneFog;
class SceneManager;
class SearchList;
class SearchNode;
struct _DS3DBUFFER;
struct FILE_HANDLE_STRUCT;
struct LightData;
struct ObjectSoundData;
struct SceneManagerData;
struct SceneAnimations;             // Scene/SceneAnimation.h
class PhysicsObject;
class SoultreeObject;
class TriggerObject;

// One static model of a zone (0x50 bytes).
struct StaticModel {
    SoultreeObject *m_object;           // +0x00
    PhysicsObject *m_physics;           // +0x04
    BuoyObject *m_buoy;                 // +0x08
    CollisionObject *m_collision;       // +0x0C
    char m_name[0x40];                  // +0x10
};

// A zone's static models (+0x844, one per zone).
struct ZoneModels {
    int m_numModels;                    // +0x00
    StaticModel *m_models;              // +0x04
};

// The [Triggers] block's actions (ReadTriggerData).
struct ScoringTrigger {                 // 0x0C bytes
    TriggerObject *m_trigger;           // +0x00
    float m_points;                     // +0x04
    float m_bonusMultiplier;            // +0x08
};

struct AnimationTrigger {               // 0x10 bytes
    TriggerObject *m_trigger;           // +0x00
    float m_timeTillReset;              // +0x04
    int m_oneTimeOnly;                  // +0x08
    int m_x0C;                          // +0x0C
};

// EnableTriggerCallBack's data.
struct EnableTrigger {                  // 0x18 bytes
    TriggerObject *m_trigger;           // +0x00
    TriggerObject *m_target;            // +0x04, an animation trigger
    int m_enable;                       // +0x08
    int m_excludeAI;                    // +0x0C
    float m_timeTillReset;              // +0x10
    int m_oneTimeOnly;                  // +0x14
};

#define SCENE_MAX_BUOYS 20

class Scene : public GameObject {
public:
    Scene(int enabled);
    Scene *Create(SceneManager *manager, AuralScape *auralScape, unsigned char flag);
    void CleanUpScene();
    int ReadSceneData(ParticleManager **particles, void (*progress)(int *), int a, int b, ParameterBlock *params);
    int ReadSceneInfo();
    int ReadResourceFiles();
    int ReadEnvironmentData(char *dir, char *cubeDir);
    int ReadFogData();
    int ReadLightData(int index);
    void ReadAmbientColorFlashData();
    void ReadBuoyNames();
    void ReadStadiumData();
    int ReadTriggerData();
    int ReadAnimationData(void (*progress)(int *), int a);
    int ReadStaticModelData(ParticleManager *particles, char *path, void (*progress)(int *), int a, int b);
    int LoadStaticModels(ParticleManager *particles, char *path, void (*progress)(int *), int a, int b,
                         ParameterBlock *params, int index, GameObject *object);  // 0 on errors, 1 at the end
    int ReadSoundData(GameObject *object);
    void ReadSoundEmitterParams(char *block, char *fileName, int is3D, _DS3DBUFFER *buffer, unsigned int *flags,
                                float *oneShotDistance, float *randomTriggerPercent, int *oneShot, int *force2D);
    int ReadColorRGB(int *color, char *block, char *name, char *defaultValue, int arg, ParameterBlock *params);
    int ReadD3DVector(_D3DVECTOR *v, char *block, char *name, char *defaultValue, int arg, ParameterBlock *params);
    int ReadSeriesOfBOOLs(unsigned char *values, int count, char *block, char *name, char *defaultValue, int arg,
                          ParameterBlock *params);
    void AddObjectSound(ObjectSoundData *data);
    void RemoveAllObjectSounds();
    void RemoveAllObjectSoundModels(ObjectSoundData *data);
    void RemoveAllImpactSoundModels();
    int RewindAnimation(int index, float time, int force, int sequence);
    void ReportError(char *);
    void ReportError(char *, char *, char *);
    void ReportSuccess(char *);
    void ReportSuccess(char *, char *, char *);
    void ReportSuccess(char *, char *, int);
    void ReportSuccess(char *, char *, float);
    void ReportSuccess(char *, char *, _D3DVECTOR *);
    void ReportSuccessBOOL(char *, char *, int);
    // ReadSoundData's inline helpers (defined in src/tu/SceneManager.cpp).
    inline void ReadObjectNames(char *key, char *objectName, ContainerList<char *> *list, int *flag);
    inline void ReadImpactNames(char *key, char *objectName, int index);

    // --- virtuals in retail slot order ---
    virtual ~Scene();
    virtual void Enable();
    virtual void AncestorsEnabled();
    virtual int Tick(float dt);
    virtual int Render3D();
    virtual int KeyUpHandler(ProcessNode *node, KeyInfo *key);
    virtual int KeyDownHandler(ProcessNode *node, KeyInfo *key);

    _D3DVECTOR m_startPosition;         // +0x30
    _D3DVECTOR m_startDirection;        // +0x40
    char m_preRaceVue[0x80];            // +0x50
    _D3DVECTOR m_podiumPosition;        // +0xD0
    _D3DVECTOR m_podiumDirection;       // +0xE0
    SceneManager *m_info;               // +0xF0, from Create
    SceneManagerData *m_manager;        // +0xF4
    char padF8[0x100 - 0xF8];
    float m_flashTime;                  // +0x100, ambient colour flash (Tick)
    float m_soundDelay;                 // +0x104, until m_sound plays
    float m_alternateSoundDelay;        // +0x108, until m_alternateSound plays
    float m_idleTime;                   // +0x10C
    float m_minIdleTime;                // +0x110
    float m_x114;                       // +0x114
    float m_duration;                   // +0x118
    float m_minDuration;                // +0x11C
    float m_x120;                       // +0x120
    float m_volume;                     // +0x124
    float m_minVolume;                  // +0x128
    float m_x12C;                       // +0x12C
    float m_soundVolume;                // +0x130
    float m_alternateSoundVolume;       // +0x134
    float m_soundMinDelay;              // +0x138
    float m_soundMaxDelay;              // +0x13C
    PSXSound *m_sound;                  // +0x140
    PSXSound *m_alternateSound;         // +0x144
    PSXSound *m_flashSound;             // +0x148, the one picked for this flash
    char pad14C[0x150 - 0x14C];
    _D3DCOLORVALUE m_currentColor;      // +0x150
    _D3DCOLORVALUE m_minColor;          // +0x160
    _D3DCOLORVALUE m_color;             // +0x170
    unsigned char m_flashEnabled : 1;   // +0x180
    char pad181[0x190 - 0x181];
    int m_numLights;                    // +0x190
    LightData *m_lights;                // +0x194
    float m_ghostOpacity;               // +0x198
    float m_ghostReset;                 // +0x19C
    int m_numCameras;                   // +0x1A0
    int *m_cameraZones;                 // +0x1A4
    SceneFog *m_fog;                    // +0x1A8
    SceneAnimations *m_animations;      // +0x1AC
    int m_numBuoys;                     // +0x1B0
    int m_buoyOrient[SCENE_MAX_BUOYS];  // +0x1B4
    float m_buoyWeight[SCENE_MAX_BUOYS];    // +0x204
    float m_buoyInertia[SCENE_MAX_BUOYS];   // +0x254
    float m_buoySpring[SCENE_MAX_BUOYS];    // +0x2A4
    float m_buoyDamping[SCENE_MAX_BUOYS];   // +0x2F4
    char m_buoyNames[SCENE_MAX_BUOYS][0x40];    // +0x344
    ZoneModels **m_zoneModels;          // +0x844, per zone
    int m_numScoringTriggers;           // +0x848
    ScoringTrigger *m_scoringTriggers;  // +0x84C
    int m_numAnimationTriggers;         // +0x850
    AnimationTrigger *m_animationTriggers;  // +0x854
    int m_numEnableTriggers;            // +0x858
    EnableTrigger *m_enableTriggers;    // +0x85C
    int m_crowdPresent;                 // +0x860
    PSXSoundGroup *m_soundGroup;        // +0x864
    ContainerList<PSXSound *> m_ambientSounds;  // +0x868
    int m_haveImpactSounds;             // +0x87C
    ContainerList<char *> m_impactSoundModels[6];   // +0x880, per impact type
    int m_haveObjectSounds;             // +0x8F8
    ContainerList<ObjectSoundData *> m_objectSounds;    // +0x8FC
    ParameterBlock *m_params;           // +0x910
    char pad914[0x920 - 0x914];
    _D3DVECTOR m_stadiumPosition;       // +0x920
    float m_stadiumTop;                 // +0x930
    float m_stadiumBottom;              // +0x934
    float m_stadiumScale;               // +0x938
    char m_stadiumFileName[0xFF];       // +0x93C
    char padA3B[0xA3C - 0xA3B];
    unsigned char m_hasStadium : 1;     // +0xA3C bit 0
    char padA3D[0xA40 - 0xA3D];
    int m_visible;                      // +0xA40, 1 from the ctor
    int m_xA44;                         // +0xA44
    char m_sceneName[0x40];             // +0xA48
    char m_xA88[0x4];                   // +0xA88
    SearchList **m_staticLists;         // +0xA8C
    SearchList **m_dynamicLists;        // +0xA90
    SearchNode **m_staticNodes;         // +0xA94
    SearchNode **m_dynamicNodes;        // +0xA98
    Portal *m_portals;                  // +0xA9C
    GameObject **m_zones;               // +0xAA0 ("NumZones")
    int m_numZones;                     // +0xAA4
    int m_numPortals;                   // +0xAA8
    FILE_HANDLE_STRUCT *m_file;         // +0xAAC
    char m_fileName[0x40];              // +0xAB0
    char padAF0[0xBC0 - 0xAF0];
};

#endif
