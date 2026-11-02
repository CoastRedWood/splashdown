// JetskiCompetition: one race (LpGame->m_competition), the base of the
// circuit, career, challenge, copycat, time-trial, time-elimination,
// practice and training competitions. 0x3E0 bytes (`new
// JetskiCopycatCompetition`, which adds no data; JetskiTraining's members
// start at +0x3E0). Bases: GameObject (vptr +0x8) and
// RaceStatusEventInterface (vptr +0x30); its own data starts at +0x34.
// Member names come from the class's own methods where they use the member,
// otherwise the most common name among the local copies
// (mwcc's layout).
#ifndef JETSKICOMPETITION_JETSKICOMPETITION_H
#define JETSKICOMPETITION_JETSKICOMPETITION_H

#include "d3dtypes.h"
#include "GameObject/GameObject.h"
#include "RaceStatusEventInterface/RaceStatusEventInterface.h"

// Argument types of RestartRace and SetPlayback (mangled 14GamePlayStatus,
// 12PlaybackType); no other header defines them.
enum GamePlayStatus {};
enum PlaybackType {};

class EcoSystem;
class Fog;
class HeightFilter;
class JetskiCameraController;
class JetskiCountDown;
class JetskiGhostVehicle;
class JetskiHeadingArrows;
class JetskiInGameStats;
class JetskiInfoText;
class JetskiNameMenu;
class JetskiPerformanceMeter;
class JetskiPromptMenu;
class JetskiRadar;
class JetskiVCRController;
class JetskiVehicle;
class LightManager;
class ParticleManager;
class Camera;
class PSXMusic;
class PSXSound;
class PSXSoundGroup;
class PSXRenderableObject;
class PSXWaterDisplacement;
class JetskiGameOptions;
class ResponseEmitter;
class Scene;
class ScreenIconManager;
class ScreenTextManager;
class SkyCube;
class SplitScreenSeparator;
class StaticObject;
class Terrain;
class JetskiCrowdSoundEmitter;
class TrackType;
class VideoFXManager;
class PSXWater;

// A player's saved camera in copycat mode (CopyCatSwitchTurn).
struct CopyCatCamera {              // 0x3C bytes
    int m_x00;
    float m_x04;
    float m_x08;
    float m_x0C;
    float m_x10;
    char m_presets[0x28];           // +0x14
};

class JetskiCompetition : public GameObject, public RaceStatusEventInterface {
public:
    JetskiCompetition(int enabled);
    int AttachScene();
    void SetSceneFolder();
    int LoadScene();
    int LoadTrackDefinition();
    int SetTrackHeights();
    int LoadTerrain();
    int LoadWater();
    int LoadWaterDisplacement();
    int LoadSkyCube();
    int LoadFog(int underwater);
    int LoadEcosystem();
    int LoadLightManager();
    int LoadGhost();
    int LoadCameras(int numPlayers, int player);
    int CreateBuoys();
    void SetUpAnimations();
    void SetupCamerasAndViewports();
    void SetupViewport(int player);
    void ResetCameraControllers();
    int FadeInCameras();
    void PickACamera(float dt);
    void AllocateIpuCache();
    void FreeIpuCache();
    void CreateListeners(int count);
    void DestroyListeners(int count);
    void UpdateListeners();
    void ResetAllEngineSounds();
    int LoadRaceVehicles(int join);
    int LoadGenericVehicleSounds(char *fileName);
    int CheckForNewHighScores(int player);
    void ResetTimeElimBuoys(int resetVehicles);
    int DoingJoinAnimation();
    int InGamePause(int pause, int player);
    int RestartRace(GamePlayStatus status);
    int CopyCatSwitchTurn(RaceStatusStruct_Tag *status);
    void UpdateGhost(float dt);
    void ResetVCR();
    void SetPlayback(PlaybackType type);
    void LoadVCRAndRun(char *filename);
    void SetPointersForVCR(JetskiVehicle *vehicle);
    void checkSecretSkinObjectsName(char *name, StaticObject *object);


    GameObject *m_x34;                          // +0x34, attached to the overlay container
    char m_sceneFolder[0x40];                   // +0x38
    int m_x78;                                  // +0x78
    PSXWaterDisplacement *m_waterDisplacement;  // +0x7C
    LightManager *m_lightManager;               // +0x80
    ParticleManager *m_particles;               // +0x84
    Fog *m_fog;                                 // +0x88
    SkyCube *m_skyCube;                         // +0x8C
    Terrain *m_terrain;                         // +0x90
    EcoSystem *m_ecoSystem;                     // +0x94
    PSXWater *m_water;                          // +0x98
    Scene *m_scene;                             // +0x9C
    int m_xA0;                                  // +0xA0
    int m_xA4;                                  // +0xA4
    int m_xA8[5];                               // +0xA8
    int m_xBC[5];                               // +0xBC
    ScreenTextManager *m_textManager;           // +0xD0
    int m_xD4;                                  // +0xD4
    int m_xD8;                                  // +0xD8
    int m_faded[2];                             // +0xDC, per player
    unsigned int m_joinTime;                    // +0xE4
    JetskiVehicle *m_vehicles[7];               // +0xE8
    Camera *m_cameras[2];                       // +0x104, per player
    JetskiCameraController *m_cameraControllers[2]; // +0x10C
    int m_numVehicles;                          // +0x114
    int m_x118;                                 // +0x118
    int m_numPlayers;                           // +0x11C
    int m_fadeOutTime[2];                       // +0x120
    int m_fadeInTime[2];                        // +0x128
    VideoFXManager *m_videoFX;                  // +0x130
    int m_x134;                                 // +0x134, viewport mode
    float m_x138;                               // +0x138
    float m_joinTimer;                          // +0x13C
    int m_status;                               // +0x140
    int m_x144;                                 // +0x144
    int m_x148;                                 // +0x148
    char *m_trackNames2[12];                    // +0x14C
    char *m_trackNames[12];                     // +0x17C
    int m_joinState;                            // +0x1AC
    JetskiGameOptions *m_options;               // +0x1B0, LpGame->m_gameOptions
    TrackType *m_track;                         // +0x1B4
    RaceStatusClass *m_raceStatus;              // +0x1B8
    JetskiCrowdSoundEmitter *m_crowd;           // +0x1BC (Create news it)
    PSXSound *m_readySound;                     // +0x1C0
    PSXSound *m_goSound;                        // +0x1C4
    PSXSound *m_balloonSounds[6];               // +0x1C8, by balloon color
    PSXSound *m_skinSound;                      // +0x1E0, "skin001.wav" (SecretSkinCallback plays it)
    PSXSound *m_rainSound;                      // +0x1E4, "rain.vag" (Everglades rain loop)
    PSXSound *m_x1E8;                           // +0x1E8 (the in-game menu plays it)
    PSXSoundGroup *m_x1EC;                      // +0x1EC (JetskiRider::Create reads its +0x40 flags)
    PSXSoundGroup *m_x1F0;                      // +0x1F0, music volume follows the options
    ResponseEmitter *m_responses;               // +0x1F4
    float m_raceTime;                           // +0x1F8
    int m_x1FC;                                 // +0x1FC
    int m_x200;                                 // +0x200
    int m_x204;                                 // +0x204
    int m_x208;                                 // +0x208
    JetskiRadar *m_radar;                       // +0x20C
    JetskiHeadingArrows *m_headingArrows;       // +0x210
    JetskiInGameStats *m_inGameStats;           // +0x214
    JetskiInfoText *m_infoText;                 // +0x218
    JetskiCountDown *m_countDown;               // +0x21C
    SplitScreenSeparator *m_separator;          // +0x220
    JetskiPerformanceMeter *m_performanceMeter; // +0x224
    int m_timeElimTime;                         // +0x228
    int m_timeElimStartTime;                    // +0x22C
    ScreenIconManager *m_iconManager;           // +0x230
    GameObject *m_inGameMenu;                   // +0x234
    GameObject *m_raceStartMenu;                // +0x238
    GameObject *m_raceOverMenu;                 // +0x23C
    JetskiNameMenu *m_nameMenu;                 // +0x240
    JetskiPromptMenu *m_promptMenu;             // +0x244
    char pad248[0x24C - 0x248];
    int m_x24C;                                 // +0x24C
    PSXMusic *m_music;                          // +0x250
    int m_x254;                                 // +0x254
    int m_x258;                                 // +0x258
    JetskiVCRController *m_vcr;                 // +0x25C
    _D3DVECTOR m_x260;                          // +0x260
    _D3DVECTOR m_x270;                          // +0x270
    _D3DVECTOR m_x280;                          // +0x280
    int m_x290;                                 // +0x290
    int m_x294;                                 // +0x294
    float m_x298;                               // +0x298
    int m_ghostEnabled;                         // +0x29C
    int m_ghostVisible;                         // +0x2A0
    float m_bestTime;                           // +0x2A4, the ghost's best time (JetskiTimeTrialCompetition::InitializeRace)
    JetskiGhostVehicle *m_ghost;                // +0x2A8
    HeightFilter *m_ghostHeight;                // +0x2AC
    int m_playbackType;                         // +0x2B0, a PlaybackType
    PSXRenderableObject *m_ghostObject;         // +0x2B4
    int m_cycleIndex;                           // +0x2B8
    int m_cameraType;                           // +0x2BC
    int m_x2C0;                                 // +0x2C0
    int m_x2C4;                                 // +0x2C4
    int m_x2C8;                                 // +0x2C8
    float m_timer;                              // +0x2CC
    float m_waterTime;                          // +0x2D0
    float m_baseTime;                           // +0x2D4
    int m_x2D8;                                 // +0x2D8
    int m_x2DC;                                 // +0x2DC
    char m_x2E0;                                // +0x2E0
    char pad2E1[0x330 - 0x2E1];
    int m_x330;                                 // +0x330
    int m_x334;                                 // +0x334
    int m_x338;                                 // +0x338
    int m_x33C;                                 // +0x33C
    int m_x340;                                 // +0x340
    int m_x344;                                 // +0x344
    int m_x348;                                 // +0x348
    int m_x34C;                                 // +0x34C
    float m_x350;                               // +0x350
    RaceStatusStruct_Tag *m_currentStatus;      // +0x354
    float m_x358;                               // +0x358
    CopyCatCamera m_copyCatCameras[2];          // +0x35C, per player
    char pad3D4[0x3E0 - 0x3D4];

    // --- virtuals in retail slot order ---
    // The dtor is declared first, so it is the key function (defined before
    // LoadGhost: retail's vtable order).
    // Overrides keep their base slots.
    virtual ~JetskiCompetition();
    virtual int Pause(int pause);
    virtual JetskiCompetition *Create();
    virtual int Tick(float dt);
    virtual int PrepareGeometry();
    virtual int RenderPre3D();
    virtual int KeyDownHandler(ProcessNode *node, KeyInfo *key);
    virtual void LoadOverlays();
    virtual void EventComplete();
    virtual void ModifyLocalGameOptions();
    virtual int InitializeRace();
    virtual int EndOfRaceRecordKeeping() { return 1; }   // inline in retail
    virtual int RSSetCurrentVehicle(RaceStatusStruct_Tag *status);
    virtual int RSStartGhostVehicle(RaceStatusClass *race, RaceStatusStruct_Tag *status);
    virtual int RSStopGhostVehicle(RaceStatusClass *race, RaceStatusStruct_Tag *status);
    virtual int RSUpdateBestGhostLap(RaceStatusClass *race, RaceStatusStruct_Tag *status, RaceStatusStruct_Tag *best);
    virtual void EnableOverlays() = 0;          // slot 54 is 0 in retail
};

#endif
