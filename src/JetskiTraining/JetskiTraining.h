// JetskiTraining: the training mode competition (tests with checkpoints
// and instructor dialogue). Members start at +0x3E0 after JetskiCompetition;
// 0x6B0 bytes. Names come from the class's own methods.
#ifndef JETSKITRAINING_JETSKITRAINING_H
#define JETSKITRAINING_JETSKITRAINING_H

#include "JetskiCompetition/JetskiCompetition.h"

class JetskiDialogueStreamer;
class ScreenText;

class JetskiTraining : public JetskiCompetition {
public:
    // One test's dialogue ranges (0x48 bytes).
    struct TrainingTest {
        int m_next;                     // +0x00
        int m_last;                     // +0x04
        int m_start[8];                 // +0x08
        int m_end[8];                   // +0x28
    };
    struct IntroDialogs {
        int m_next;                     // +0x00
        int m_last;                     // +0x04
        int m_start;                    // +0x08
        int m_end;                      // +0x0C
    };
    struct RandomDialogs {
        int m_first;                    // +0x00
        int m_count;                    // +0x04
        int m_played;                   // +0x08
        int m_last;                     // +0x0C
    };

    JetskiTraining(int enabled);
    void SetupOverlays();
    int LoadCheckpoints();
    int LoadIntroDialog();
    void UnloadIntroDialog();
    int PlayTrainingDialog(float dt);
    void ResetFlags();
    int ResetTest(int test, int restart);
    void RelocateJetski(int test);
    void ChangeCamera(int jetski);
    int IsOutOfBounds(int player);
    int NextTestForInGameMenu();
    void WriteTestInfo();

    // --- virtuals in retail slot order ---
    // Overrides keep their base slots. The dtor is the key function:
    // src/tu/JetskiTraining.cpp emits the vtable and the @48@ thunks.
    virtual ~JetskiTraining();
    virtual int Tick(float dt);
    virtual JetskiTraining *Create();
    virtual int KeyDownHandler(ProcessNode *node, KeyInfo *key);
    virtual void EventComplete();
    virtual void ModifyLocalGameOptions();
    virtual int InitializeRace();
    virtual void EnableOverlays();

    int m_section;                      // +0x3E0, current section / stage
    int m_x3E4;                         // +0x3E4
    int m_x3E8;                         // +0x3E8
    int m_x3EC;                         // +0x3EC
    int m_x3F0;                         // +0x3F0
    int m_x3F4;                         // +0x3F4
    int m_x3F8;                         // +0x3F8
    int m_x3FC;                         // +0x3FC
    int m_x400;                         // +0x400
    int m_x404;                         // +0x404
    float m_x408;                       // +0x408
    float m_x40C;                       // +0x40C
    float m_x410;                       // +0x410
    int m_test;                         // +0x414
    int m_checkpointHit[20];            // +0x418
    float m_x468;                       // +0x468
    float m_x46C;                       // +0x46C
    int m_x470;                         // +0x470
    int m_x474;                         // +0x474
    ScreenText *m_titleText;            // +0x478
    ScreenText *m_instructionText;      // +0x47C
    ScreenText *m_statusText;           // +0x480
    JetskiDialogueStreamer *m_dialogue; // +0x484
    TrainingTest m_tests[3];            // +0x488
    IntroDialogs m_intro;               // +0x560
    int m_x570;                         // +0x570
    RandomDialogs m_random;             // +0x574
    int m_numDialogues;                 // +0x584
    char pad588[0x590 - 0x588];
    _D3DVECTOR m_x590;                  // +0x590
    int m_hasDemo[42];                  // +0x5A0
    int m_x648;                         // +0x648
    int m_x64C;                         // +0x64C
    int m_trainingComplete;             // +0x650
    int m_x654;                         // +0x654
    float m_savedFloat0;                // +0x658
    float m_savedFloat1;                // +0x65C
    int m_savedFlags;                   // +0x660
    int m_savedControllerPreset;        // +0x664
    int m_x668;                         // +0x668
    int m_x66C;                         // +0x66C
    int m_x670;                         // +0x670
    float m_x674;                       // +0x674
    char pad678[0x680 - 0x678];
    _D3DVECTOR m_x680;                  // +0x680
    _D3DVECTOR m_x690;                  // +0x690
    int m_x6A0;                         // +0x6A0
    float m_x6A4;                       // +0x6A4
    float m_x6A8;                       // +0x6A8
    float m_dialogTimer;                // +0x6AC
};

#endif
