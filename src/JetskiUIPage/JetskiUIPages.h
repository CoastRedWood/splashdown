// The JetskiUIPage* pages of JetskiUIPages.cpp (0x3551C0-0x3690F0), one class
// each, sized to retail's `new` (JetskiUIManMain::GetPage). Page-class header
// convention: each page TU gets one header in
// src/JetskiUIPage/ named after the TU (JetskiUIPages.h, JetskiMemPages.h)
// that declares all its page classes; members start at +0x168 (JetskiUIPage
// is 0x168). Virtuals are the overrides the retail vtable lists for each class.
// The +0x168 ScreenTextManager copies are m_pageTextManager, so they don't
// hide UIPage::m_textManager (+0x134).
#ifndef JETSKIUIPAGE_JETSKIUIPAGES_H
#define JETSKIUIPAGE_JETSKIUIPAGES_H

#include "JetskiUIPage/JetskiUIPage.h"
#include "_RECT/_RECT.h"

class ProcessNode;
class KeyInfo;
class ScreenIcon;
class ScreenPrimitive;
class ScreenPrimitiveManager;
class ScreenPrimitiveVertex;
class ScreenText;
class ScreenTextFont;
class TextureMap;
struct ModeSelection;

class JetskiUIPageCareer : public JetskiUIPage {
public:
    JetskiUIPageCareer(int enabled);
    void ProcessInputSounds(int player, int input);
    void SetAIRacers();
    void StartNewVideo();
    void EndVideo();

    virtual ~JetskiUIPageCareer();
    virtual int PrepareGeometry();
    virtual int Render3D();
    virtual void ResetPage();
    virtual int ProcessUserInput(int player, int input);
    virtual int Transition(int direction);
    virtual int ProcessTransition(float dt);
    virtual JetskiUIPageCareer *Create(char *name, char *fileName, UIManager *manager,
                                       ScreenTextManager *textManager);

    ScreenTextManager *m_pageTextManager;   // +0x168
    ScreenText *m_name;                     // +0x16C
    ScreenText *m_character;                // +0x170
    ScreenText *m_racesLeft;                // +0x174
    ScreenText *m_event;                    // +0x178
    ScreenText *m_difficulty;               // +0x17C
    ScreenTextFont *m_font180;              // +0x180
    ScreenTextFont *m_font184;              // +0x184
    ScreenTextFont *m_font188;              // +0x188
    ScreenTextFont *m_font18C;              // +0x18C
    char pad190[0x194 - 0x190];
    _RECT m_rects[4];                       // +0x194, where the corners go
    TextureMap *m_corners[4];               // +0x1D4, UL UR BL BR
    int m_fadeMusic;                        // +0x1E4
};

class JetskiUIPageCarRacerSelect : public JetskiUIPage {
public:
    JetskiUIPageCarRacerSelect(int enabled);
    void TeamHeadShots();
    void SetRing(int ring);
    void ProcessInputSounds(int player, int input);
    void LoadCharacter();
    void DeleteCharacter(int selected);
    void BioLocked();
    void BioRequested();
    int ProcessNewInput(int input);
    void CharacterSkinNext();
    void CharacterSkinPrevious();
    void SetUpAITeams(int playerCharacter, int playerTeam);
    int ProcessContinueInput(int input);
    int ProcessChallengerNewInput(int input);
    int ProcessChallengerTeamInput(int input);

    virtual ~JetskiUIPageCarRacerSelect();
    virtual int Tick(float dt);
    virtual int PrepareGeometry();
    virtual int Render3D();
    virtual void ResetPage();
    virtual int ProcessUserInput(int player, int input);
    virtual int Transition(int direction);
    virtual int ProcessTransition(float dt);
    virtual JetskiUIPageCarRacerSelect *Create(char *name, char *fileName, UIManager *manager,
                                               ScreenTextManager *textManager);

    ScreenIcon **m_icons;                   // +0x168
    ScreenIcon *m_ring;                     // +0x16C
    ScreenIcon **m_headShots;               // +0x170
    ScreenIcon **m_portraits;               // +0x174
    int m_teamIndex;                        // +0x178
    int m_bioState;                         // +0x17C
    int m_x180;                             // +0x180
    int m_fadeMusic;                        // +0x184
    int m_riderChosen;                      // +0x188, 1 from the ctor
    char m_deviceName[16];                  // +0x18C
    char pad19C[0x1C0 - 0x19C];
};

class JetskiUIPageCharSelect : public JetskiUIPage {
public:
    JetskiUIPageCharSelect(int enabled);
    void SetRing(int ring);
    void ChangeHeadShots(int player);
    void ProcessInputSounds(int player, int input);
    void LoadCharacter();
    void DeleteCharacter();
    void BioLocked();
    void BioRequested();
    void SwitchSkinNext(int player, int slot);
    void SwitchSkinPrevious(int player, int slot);

    virtual ~JetskiUIPageCharSelect();
    virtual int Tick(float dt);
    virtual int PrepareGeometry();
    virtual int Render3D();
    virtual void ResetPage();
    virtual int ProcessUserInput(int player, int input);
    virtual int Transition(int direction);
    virtual int ProcessTransition(float dt);
    virtual JetskiUIPageCharSelect *Create(char *name, char *fileName, UIManager *manager,
                                           ScreenTextManager *textManager);

    ScreenIcon **m_headShots;               // +0x168, 8 clones
    ScreenIcon *m_ring;                     // +0x16C
    ScreenIcon **m_headShotIcons;           // +0x170, the manager's ([9] locked, [10] unlocked)
    ScreenIcon **m_portraits;               // +0x174, the manager's
    int m_x178;                             // +0x178, 1 from the ctor
    int m_bioState;                         // +0x17C
    int *m_character;                       // +0x180
    int *m_skin;                            // +0x184
    char m_deviceName[16];                  // +0x188
    char pad198[0x1C0 - 0x198];
};

class JetskiUIPageDiff : public JetskiUIPage {
public:
    JetskiUIPageDiff(int enabled);
    void ProcessInputSounds(int player, int input);

    virtual ~JetskiUIPageDiff();
    virtual void ResetPage();
    virtual int ProcessUserInput(int player, int input);
    virtual int Transition(int direction);
    virtual int ProcessTransition(float dt);
};

class JetskiUIPageGOptions : public JetskiUIPage {
public:
    JetskiUIPageGOptions(int enabled);
    void ProcessInputSounds(int player, int input);

    virtual ~JetskiUIPageGOptions();
    virtual void ResetPage();
    virtual int ProcessUserInput(int player, int input);
    virtual int Transition(int direction);
    virtual int ProcessTransition(float dt);
};

class JetskiUIPageLoadSave : public JetskiUIPage {
public:
    JetskiUIPageLoadSave(int enabled);
    void SetSavedCareer(int slot);

    virtual ~JetskiUIPageLoadSave();
    virtual int PrepareGeometry();
    virtual int Render3D();
    virtual void ResetPage();
    virtual int ProcessUserInput(int player, int input);
    virtual int Transition(int direction);
    virtual int ProcessTransition(float dt);
    virtual JetskiUIPageLoadSave *Create(char *name, char *fileName, UIManager *manager,
                                         ScreenTextManager *textManager);

    ScreenTextManager *m_pageTextManager;   // +0x168
    ScreenText **m_slotTexts;               // +0x16C, 5 (malloc'd)
    ScreenText *m_titleText;                // +0x170
    ScreenText **m_infoTexts;               // +0x174, 6 (malloc'd)
    ScreenText **m_promptTexts;             // +0x178, 3 (malloc'd): [1] yes, [2] no
    ScreenTextFont *m_font;                 // +0x17C
    int m_controlsMoving;                   // +0x180
    int m_groupMoving;                      // +0x184
    int m_confirming;                       // +0x188
    int m_confirmNo;                        // +0x18C
    int m_selectedCareer;                   // +0x190
    char pad194[0x198 - 0x194];
};

class JetskiUIPageMain : public JetskiUIPage {
public:
    JetskiUIPageMain(int enabled);
    void ProcessInputSounds(int player, int input);

    virtual ~JetskiUIPageMain();
    virtual int Tick(float dt);
    virtual void ResetPage();
    virtual int ProcessUserInput(int player, int input);
    virtual int Transition(int direction);
    virtual int ProcessTransition(float dt);

    int m_transitionB;                      // +0x168, group 1 still moving
    int m_transitionA;                      // +0x16C, group 0 still moving
    int m_cardPresent;                      // +0x170, 1 from the ctor
    int m_x174;                             // +0x174
    float m_idleTime;                       // +0x178, 30 from the ctor
    float m_musicFade;                      // +0x17C
};

class JetskiUIPageName : public JetskiUIPage {
public:
    JetskiUIPageName(int enabled);
    void ProcessInputSounds(int player, int input);
    void SetupKeyBoard();
    void SetSelectionYellow();
    void SetSelectionWhite();
    void ProcessLetter();

    virtual ~JetskiUIPageName();
    virtual int Tick(float dt);
    virtual int Render3D();
    virtual int KeyDownHandler(ProcessNode *node, KeyInfo *key);
    virtual void ResetPage();
    virtual int ProcessUserInput(int player, int input);
    virtual int Transition(int direction);
    virtual int ProcessTransition(float dt);
    virtual JetskiUIPageName *Create(char *name, char *fileName, UIManager *manager,
                                     ScreenTextManager *textManager);

    ScreenTextManager *m_pageTextManager;   // +0x168
    ScreenText *m_name;                     // +0x16C
    ScreenText **m_letters;                 // +0x170, the keys
    ScreenText *m_cursor;                   // +0x174
    ScreenTextFont *m_font;                 // +0x178
    int m_showCursor;                       // +0x17C
    float m_cursorTimer;                    // +0x180
    int m_selection;                        // +0x184
    int m_column;                           // +0x188
    char pad18C[0x190 - 0x18C];
};

class JetskiUIPageOptions : public JetskiUIPage {
public:
    JetskiUIPageOptions(int enabled);
    void ProcessInputSounds(int player, int input);

    virtual ~JetskiUIPageOptions();
    virtual int Tick(float dt);
    virtual void ResetPage();
    virtual int ProcessUserInput(int player, int input);
    virtual int Transition(int direction);
    virtual int ProcessTransition(float dt);

    int m_x168;                             // +0x168
    int m_x16C;                             // +0x16C
    int m_pad2Plugged;                      // +0x170, 1 from the ctor
    int m_refresh;                          // +0x174
    int m_selectHeld;                       // +0x178
    char pad17C[0x180 - 0x17C];
};

class JetskiUIPageRecords : public JetskiUIPage {
public:
    JetskiUIPageRecords(int enabled);
    void RenderCircuitStats();
    void RenderTrialStats();
    void RenderCountDownStats();
    void SetupEventGroups();
    void ProcessInputSounds(int player, int input);

    virtual ~JetskiUIPageRecords();
    virtual int PrepareGeometry();
    virtual int Render3D();
    virtual void ResetPage();
    virtual int ProcessUserInput(int player, int input);
    virtual int Transition(int direction);
    virtual int ProcessTransition(float dt);
    virtual JetskiUIPageRecords *Create(char *name, char *fileName, UIManager *manager,
                                        ScreenTextManager *textManager);

    ScreenTextManager *m_pageTextManager;   // +0x168
    ScreenText **m_labels;                  // +0x16C, 6 (malloc'd)
    ScreenText **m_values;                  // +0x170, 6 (malloc'd)
    ScreenText **m_labels2;                 // +0x174, 6 (malloc'd)
    ScreenText **m_values2;                 // +0x178, 6 (malloc'd)
    ScreenTextFont *m_font;                 // +0x17C
};

class JetskiUIPageSetup : public JetskiUIPage {
public:
    JetskiUIPageSetup(int enabled);
    void SetupEventGroups();
    void SetupMap();
    void CourseLocked();
    void ProcessInputSounds(int player, int input);
    void EndVideo();
    void SetupGameOptions();
    void StartNewVideo();

    virtual ~JetskiUIPageSetup();
    virtual int Tick(float dt);
    virtual int Render3D();
    virtual void ResetPage();
    virtual int ProcessUserInput(int player, int input);
    virtual int Transition(int direction);
    virtual int ProcessTransition(float dt);
    virtual JetskiUIPageSetup *Create(char *name, char *fileName, UIManager *manager,
                                      ScreenTextManager *textManager);

    int m_courseUnlocked;                   // +0x168 (CourseLocked)
    int m_transitioning;                    // +0x16C
    int m_upGroup[5][5];                    // +0x170, [group - 1][control - 1]
    int m_downGroup[5][5];                  // +0x1D4
    ModeSelection *m_selections;            // +0x238, indexed by mode - 1
    int *m_mode;                            // +0x23C
    int m_currentVideo;                     // +0x240, -1 from the ctor
    float m_videoDelay;                     // +0x244, -1 from the ctor
    _RECT m_alphaRects[4];                  // +0x248
    TextureMap *m_alphaMaps[4];             // +0x288
};

class JetskiUIPageSLevels : public JetskiUIPage {
public:
    JetskiUIPageSLevels(int enabled);
    void ProcessInputSounds(int player, int input);

    virtual ~JetskiUIPageSLevels();
    virtual int Tick(float dt);
    virtual void ResetPage();
    virtual int ProcessUserInput(int player, int input);
    virtual int Transition(int direction);
    virtual int ProcessTransition(float dt);

    int m_levelsTransitioning;              // +0x168
    int m_pageTransitioning;                // +0x16C
    int m_soundMode;                        // +0x170
    float m_pan;                            // +0x174
};

class JetskiUIPageStandings : public JetskiUIPage {
public:
    JetskiUIPageStandings(int enabled);
    JetskiUIPageStandings *Create(char *name, char *fileName, UIManager *manager, ScreenTextManager *textManager,
                                  ScreenPrimitiveManager *primitives);
    void ProcessInputSounds(int player, int input);

    virtual ~JetskiUIPageStandings();
    virtual int PrepareGeometry();
    virtual int Render3D();
    virtual void ResetPage();
    virtual int ProcessUserInput(int player, int input);
    virtual int Transition(int direction);
    virtual int ProcessTransition(float dt);

    ScreenTextManager *m_pageTextManager;   // +0x168
    ScreenText **m_texts;                   // +0x16C, 10
    ScreenTextFont *m_font;                 // +0x170
    ScreenPrimitiveManager *m_primitiveManager; // +0x174
    ScreenPrimitiveVertex *m_barVertices[5];    // +0x178
    ScreenPrimitive *m_bars[5];             // +0x18C
    ScreenPrimitiveVertex *m_frameVertices[5];  // +0x1A0
    ScreenPrimitive *m_frames[5];           // +0x1B4
    int m_numVertices;                      // +0x1C8, 4 from the ctor
    char pad1CC[0x1D0 - 0x1CC];
};

class JetskiUIPageController : public JetskiUIPage {
public:
    JetskiUIPageController(int enabled);
    void ProcessInputSounds(int player, int input);

    virtual ~JetskiUIPageController();
    virtual void ResetPage();
    virtual int ProcessUserInput(int player, int input);
    virtual int Transition(int direction);
    virtual int ProcessTransition(float dt);
};

class JetskiUIPageChallenge : public JetskiUIPage {
public:
    JetskiUIPageChallenge(int enabled);
    void ProcessInputSounds(int player, int input);

    virtual ~JetskiUIPageChallenge();
    virtual void ResetPage();
    virtual int ProcessUserInput(int player, int input);
    virtual int Transition(int direction);
    virtual int ProcessTransition(float dt);
};

class JetskiUIPageTraining : public JetskiUIPage {
public:
    JetskiUIPageTraining(int enabled);
    void ProcessInputSounds(int player, int input);

    virtual ~JetskiUIPageTraining();
    virtual void ResetPage();
    virtual int ProcessUserInput(int player, int input);
    virtual int Transition(int direction);
    virtual int ProcessTransition(float dt);

    int m_transitioning;                    // +0x168
    char pad16C[0x170 - 0x16C];
};

class JetskiUIPageVideos : public JetskiUIPage {
public:
    JetskiUIPageVideos(int enabled);
    void ProcessInputSounds(int player, int input);

    virtual ~JetskiUIPageVideos();
    virtual void ResetPage();
    virtual int ProcessUserInput(int player, int input);
    virtual int Transition(int direction);
    virtual int ProcessTransition(float dt);
};

class JetskiUIPageCredits : public JetskiUIPage {
public:
    JetskiUIPageCredits(int enabled);
    void EndVideo();

    virtual ~JetskiUIPageCredits();
    virtual int Tick(float dt);
    virtual void ResetPage();
    virtual int ProcessUserInput(int player, int input);
};

class JetskiUIPageCovers : public JetskiUIPage {
public:
    JetskiUIPageCovers(int enabled);

    virtual ~JetskiUIPageCovers();
    virtual int Tick(float dt);
    virtual void ResetPage();

    float m_timer;                          // +0x168, 2.2 from the ctor
    char pad16C[0x170 - 0x16C];
};

class JetskiUIPageCenterScreen : public JetskiUIPage {
public:
    JetskiUIPageCenterScreen(int enabled);
    void ProcessInputSounds(int player, int input);

    virtual ~JetskiUIPageCenterScreen();
    virtual int Tick(float dt);
    virtual void ResetPage();
    virtual int ProcessUserInput(int player, int input);
    virtual JetskiUIPageCenterScreen *Create(char *name, char *fileName, UIManager *manager,
                                             ScreenTextManager *textManager);

    float m_repeatTimer;                    // +0x168
    int m_direction;                        // +0x16C, 1 left, 2 right, 3 up, 4 down
    int m_lastDirection;                    // +0x170
    char pad174[0x178 - 0x174];
};

#endif
