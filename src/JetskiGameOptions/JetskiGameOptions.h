// JetskiGameOptions: race setup, unlocks, profile and hall-of-fame
// persistence (LpGame->m_gameOptions). A BaseObject, 0x6448 bytes
// (`new` in JetskiGame::Initialize). Names come from the class's own
// methods where they use the member, otherwise the most common name among
// the files that declared it locally (mwcc's layout).
#ifndef JETSKIGAMEOPTIONS_JETSKIGAMEOPTIONS_H
#define JETSKIGAMEOPTIONS_JETSKIGAMEOPTIONS_H

#include "GameObject/GameObject.h"
#include "ViewPortCode/ViewPortCode.h"
#include "HallOfFameListStruct/HallOfFameListStruct.h"
#include "PSXMemoryCard/PSXMemoryCard.h"

class JetskiCareer;
class JetskiPageSettings;
struct CareerRacer;

class JetskiGameOptions : public BaseObject {
public:
    JetskiGameOptions();
    void Create();
    int IsGameInstalled();
    int CheckForMemoryCard(int loadProfile);
    int FormatMemoryCard();
    int GetFreeMemoryCardSpace();
    int LoadGameProfileCardNo();
    int LoadGameProfileCardYes();
    int SaveAll();
    int SaveHallOfFame();
    int SaveStore_GameProfile(int save);
    void PopulateNewHallOfFame(HallOfFameListStruct *list);
    int DefaultSaveSlot(int slot);
    void SetCareerPath(int difficulty, int player);
    void SetPlayerTypes(int allComputer);
    void SetupAIRacers(int numHumans);
    void SetUpSkinIndices();
    void SetUI(int a, int b);
    int ChangeControllerPreset(int preset, int pad);
    int GetVehicleID(int vehicle);
    void GetCharacterName(int character, char *name);
    void RestoreSplitScreenMode();
    void SaveCurrentSplitScreenMode(int mode, ViewPortCode code);
    void CheatUnlockAllCharacters();
    void CheatUnlockAllMovies();
    void CheatUnlockAllTracks();
    void CheatUnlockEverything();
    void CheatAllOutAI();
    void CheatFullPerformance();
    void CheatHardToNormalAI();
    void CheatNoJoust();
    // Inlined into SetupAIRacers.
    void SetRacer(int racer, int character, int skin)
    {
        m_racerActive[racer] = 1;
        m_racerCharacter[racer] = character;
        m_racerSkin[racer] = skin;
        m_racerVehicle[racer] = GetVehicleID(character);
        m_racerVehicleSkin[racer] = skin;
    }

    // One difficulty's career progress (0x40 bytes).
    struct CareerProgress {
        char m_name[9];                 // +0x00 (retail copies 9 bytes)
        int m_unlocked;                 // +0x0C
        int m_x10;                      // +0x10
        int m_x14;                      // +0x14
        int m_x18;                      // +0x18
        int m_x1C[5];                   // +0x1C
        int m_x30;                      // +0x30
        int m_x34;                      // +0x34
        int m_x38;                      // +0x38
        int m_x3C;                      // +0x3C
    };
    struct CareerRound {
        int m_type;                     // +0x00, 6 = career race, 7 = other
        int m_scene;                    // +0x04
        int m_pointsToPass;             // +0x08
        float m_qualifyTimes[5];        // +0x0C
    };
    struct CareerPath {                 // 0x284 bytes
        int m_numRounds;                // +0x000
        CareerRound m_rounds[20];       // +0x004
    };
    // One saved career (0x3D0 bytes).
    struct SaveSlot {
        int m_x00;                      // +0x00
        int m_x04;                      // +0x04
        int m_x08;                      // +0x08
        CareerProgress m_careers[5];    // +0x00C
        CareerPath m_careerPath;        // +0x14C
    };
    // The profile image saved as "BASLUS-20223RBSplash" (0x1554 bytes):
    // the save slots, then copies of the options SaveStore_GameProfile keeps.
    struct GameProfileSave {
        SaveSlot m_slots[5];            // +0x0000
        int m_difficulty;               // +0x1310
        int m_musicVolume;              // +0x1314
        int m_x828;                     // +0x1318
        int m_x82C;                     // +0x131C
        int m_x830;                     // +0x1320
        int m_controllerPreset[2];      // +0x1324
        int m_x834;                     // +0x132C
        int m_screenShiftX;             // +0x1330
        int m_screenShiftY;             // +0x1334
        int m_x50;                      // +0x1338
        int m_trackUnlocked[2][21];     // +0x133C
        int m_characterUnlocked[9];     // +0x13E4
        int m_x4A4C;                    // +0x1408
        int m_secretSkinUnlocked[9][8]; // +0x140C
        int m_movieUnlocked[10];        // +0x152C
    };

    virtual ~JetskiGameOptions();

    int m_language;                     // +0x10
    int m_difficulty;                   // +0x14
    int m_x18;                          // +0x18
    int m_x1C;                          // +0x1C
    int m_raceType;                     // +0x20 (SetPlayerTypes: m_gameMode)
    int m_uiMode;                       // +0x24
    int m_x28;                          // +0x28
    int m_x2C;                          // +0x2C, track/scene index (JetskiCompetition::LoadScene)
    int m_x30;                          // +0x30 (m_numLaps or m_track in some copies)
    int m_numRacers;                    // +0x34
    int m_numHumans;                    // +0x38
    int m_x3C;                          // +0x3C
    int m_x40;                          // +0x40
    int m_x44;                          // +0x44
    int m_x48;                          // +0x48
    int m_x4C;                          // +0x4C
    int m_x50;                          // +0x50
    int m_x54;                          // +0x54 (SetPlayerTypes: m_numHumans; ghost/camera mode in JetskiCompetition)
    int m_x58;                          // +0x58
    // Some copies split this as [20] + m_xAC + [20] + m_x100, or as
    // m_trackLocked[21] at +0x58 (1-based) and m_hardTrackLocked at +0xAC.
    int m_trackUnlocked[2][21];         // +0x5C, per difficulty (ctor)
    int m_characterUnlocked[9];         // +0x104 (some copies: m_x124 = [8])
    int m_movieUnlocked[10];            // +0x128
    int m_x150[2][21];                  // +0x150 (copies: m_eventUnlocked[21] at +0x1A4)
    int m_racerActive[5];               // +0x1F8, player types: 0 human, 1 computer, 2 second human
    int m_racerCharacter[5];            // +0x20C
    int m_racerVehicle[5];              // +0x220
    int m_racerSkin[5];                 // +0x234
    int m_racerVehicleSkin[5];          // +0x248
    int m_jetskiSkins[9][2][8];         // +0x25C
    int m_riderSkins[9][2][8];          // +0x49C
    int m_secretSkinUnlocked[9][8];     // +0x6DC (ctor: first two of each 1)
    int m_skins[9];                     // +0x7FC
    int m_splitScreen;                  // +0x820
    int m_musicVolume;                  // +0x824
    int m_x828;                         // +0x828
    int m_x82C;                         // +0x82C
    int m_x830;                         // +0x830
    int m_x834;                         // +0x834
    char pad838[0x858 - 0x838];
    int m_x858;                         // +0x858
    int m_hudFlags[2];                  // +0x85C, per player: bit 0 the in-game stats, bit 1 the performance meter
    int m_controllerPreset[2];          // +0x864
    int m_x86C;                         // +0x86C
    int m_savedSplitScreen;             // +0x870, m_splitScreen saved by SaveCurrentSplitScreenMode
    int m_savedViewport;                // +0x874, m_x50 saved by SaveCurrentSplitScreenMode
    int m_x878;                         // +0x878
    char m_creditsVideo[0x20];          // +0x87C, video to play
    int m_x89C;                         // +0x89C
    int m_screenShiftX;                 // +0x8A0
    int m_screenShiftY;                 // +0x8A4
    HallOfFameListStruct m_hallOfFame;  // +0x8A8, saved as "fame.dat"
    int m_careerMode;                   // +0x4A48
    int m_x4A4C;                        // +0x4A4C
    int m_challengerRider;              // +0x4A50
    int m_challengerSkin;               // +0x4A54
    int m_challengerVehicle;            // +0x4A58
    int m_challengerVehicleSkin;        // +0x4A5C
    GameProfileSave m_profile;          // +0x4A60, written to the memory card
    // The player's racer in the current career: &m_career->m_racers[m_teamIndex]
    // (JetskiUIPageCarRacerSelect::ProcessNewInput), the record that
    // JetskiCareerCompetition::CircuitRecordKeeping awards points to. The
    // racer-select page calls it the team (its choices are the riders).
    CareerRacer *m_team;                // +0x5FB4
    int m_teamIndex;                    // +0x5FB8
    JetskiCareer *m_career;             // +0x5FBC
    int m_x5FC0;                        // +0x5FC0
    int m_cardPresent;                  // +0x5FC4
    int m_x5FC8;                        // +0x5FC8
    char pad5FCC[0x5FD0 - 0x5FCC];
    int m_x5FD0;                        // +0x5FD0
    int m_x5FD4;                        // +0x5FD4
    JetskiPageSettings *m_pageSettings; // +0x5FD8
    int m_x5FDC;                        // +0x5FDC
    char pad5FE0[0x5FE4 - 0x5FE0];
    PSXMemoryCard m_memoryCard;         // +0x5FE4 (0x460 bytes)
    char pad6444[0x6448 - 0x6444];
};

#endif
