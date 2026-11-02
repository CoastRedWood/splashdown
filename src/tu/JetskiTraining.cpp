// JetskiTraining.cpp  @ 0x0038F120 (text 0x38F120-0x3933B0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's JetskiTraining.cpp as one translation unit: TrainingTriggerCallBack (its
// checkpoint trigger callback, at 0x38F120 before the ctor), JetskiTraining's methods,
// CheckpointSpacial's ctor (for the CheckpointTrigger array) and the static
// initializer. `make link` places the object's data as well as its code
// (tools/link_c.py). The rest of the tu_map row is two TUs without an initializer, each
// with its own BaseObject RTTI: ManagedTextureGroup.cpp and ScreenObject.cpp (src/tu/).
// The old files' views map onto the shared headers: the vehicles are
// JetskiCompetition::m_vehicles[0] (the player), [1] (the instructor) and [5] (the
// jetski the HUD follows); the test/intro dialogue ranges are m_tests[] and m_intro;
// the saved options are JetskiGameOptions' m_racerCharacter[0..1], m_racerVehicle[],
// m_racerSkin[], m_racerVehicleSkin[].




#include "GameObject/GameObject.h"
#include "RaceStatusEventInterface/RaceStatusEventInterface.h"
#include "d3dtypes.h"
#include "JetskiTraining/JetskiTraining.h"
#include "ScreenText/ScreenText.h"
#include "TextureMapManager/TextureMapManager.h"
#include "JetskiGameOptions/JetskiGameOptions.h"
#include "JetskiGame/JetskiGame.h"
#include "TriggerObject/TriggerObject.h"
#include "CheckpointSpacial/CheckpointSpacial.h"
#include "CollisionObject/CollisionObject.h"
#include "JetskiEngineSoundEmitter/JetskiEngineSoundEmitter.h"
#include "JetskiVehicle/JetskiVehicle.h"
#include "JetskiInGameStats/JetskiInGameStats.h"
#include "JetskiPerformanceMeter/JetskiPerformanceMeter.h"
#include "JetskiLoadManager/JetskiLoadManager.h"
#include "VideoFXManager/VideoFXManager.h"
#include "PSXSoundGroup/PSXSoundGroup.h"
#include "Scene/Scene.h"
#include "AuralScape/AuralScape.h"
#include "JetskiRider/JetskiRider.h"
#include "PhysicsRigidBody/PhysicsRigidBody.h"
#include "JetskiTrainingInGameMenu/JetskiTrainingInGameMenu.h"
#include "JetskiAirControlModel/JetskiAirControlModel.h"
#include "soultree_assert_block.h"
#include "GameText/GameText.h"
#include "ScreenTextManager/ScreenTextManager.h"
#include "ScreenTextFontItemList/ScreenTextFontItem.h"
#include "ScreenTextFontItemList/ScreenTextFontItemList.h"
#include "JetskiDialogueStreamer/JetskiDialogueStreamer.h"
#include "JetskiVCRController/JetskiVCRController.h"
#include "ParameterBlock/ParameterBlock.h"
#include "JetskiFollowCamera/JetskiFollowCamera.h"
#include "JetskiCameraController/JetskiCameraController.h"
#include "ViewPortCode/ViewPortCode.h"
#include "TextureMap/TextureMap.h"
#include "RaceStatusClass/RaceStatusClass.h"
#include "RaceStatusStruct_Tag/RaceStatusStruct_Tag.h"
#include "ControlInterface/ControlTypes.h"
// The axis vectors every Soultree TU defines (and __sinit constructs).
#include "sinit/AxisVectors.h"

// Retail never calls fptosi here: float->int is an inline cvt.w.s.
#pragma fast_fptosi on
// inline_depth is read when the deferred functions are generated, at the
// end of the TU: WriteTestInfo's 5 applies to every function.
#pragma inline_depth(5)

extern "C" int sprintf(char *buf, const char *fmt, ...);
extern "C" int rand();
void PSXprintf(const char *fmt, ...);
int KEYDOWN_HANDLER_TEST(AssignableActionIndex action, DeviceType device, ProcessNode *node, unsigned int flags);
void TrainingTriggerCallBack(CollisionObject *object, void *triggerData, void *userData, int entering);

extern TextureMapManager *LpTextureMapManager;
extern JetskiGame *LpGame;

// Per-test reset positions and headings: x, y, z, dir x, dir y, dir z. Not
// const: retail's is a global in .data.
float JetskiRelocation[8][6] = {
    { 1372.4f, 30.0f, 2011.4f,  0.9996f, 0.0f, -0.0292f },
    { 1772.9f, 30.0f, 2006.4f,  0.9999f, 0.0f,  0.0092f },
    { 2074.6f, 30.0f, 1846.3f, -0.9995f, 0.0f, -0.0327f },
    { 2009.2f, 30.0f, 1627.1f, -0.9999f, 0.0f, -0.0017f },
    { 1439.7f, 30.0f, 1420.9f,  0.9999f, 0.0f,  0.0083f },
    { 1514.4f, 30.0f, 1165.4f,  0.9985f, 0.0f,  0.0541f },
    { 1514.4f, 30.0f, 1165.4f,  0.9985f, 0.0f,  0.0541f },
    { 2074.6f, 30.0f, 1846.3f, -0.9995f, 0.0f, -0.0327f },
};

// training.aud's parameter block per test (in .data in retail, like
// JetskiRelocation).
char TestTitles[8][20] = {
    "BuoyTest", "JumpPitchTest", "HydroTest", "SubTest",
    "HopTest", "JumpStuntTest", "StuntFillTest", "BackflipTest",
};

// ---- src/global/TrainingTriggerCallBack.cpp

extern unsigned int WORLDOBJECT_PHYSICSBODY;
extern unsigned int COLLISION_BIKE;

// CollisionObject::IsAt as this file inlined it ((x, z, y) declaration order
// reproduces retail's register choice).
static inline int CollisionObjectIsAt(CollisionObject *object, const _D3DVECTOR &v)
{
    float x, z, y;
    x = v.x;
    y = v.y;
    z = v.z;
    bool result = false;
    bool xy = object->m_center.x == x && object->m_center.y == y;
    if (xy && object->m_center.z == z)
        result = true;
    return result;
}

void TrainingTriggerCallBack(CollisionObject *object, void *triggerData, void *userData, int entering)
{
    if ((object->m_ownerType & WORLDOBJECT_PHYSICSBODY) || (object->m_ownerType & COLLISION_BIKE)) {
        CollisionObject *trigger = (CollisionObject *)triggerData;
        JetskiTraining *training = (JetskiTraining *)userData;
        int i = 0;
        int found = 0;
        while (!found && i < 20) {
            if (CollisionObjectIsAt(trigger, CheckpointTrigger[i].m_position))
                found = 1;
            else
                i++;
        }
        if (found && entering && training->m_section == 2)
            training->m_checkpointHit[i] = 1;
    }
}


// ---- src/JetskiTraining/ctor.cpp

JetskiTraining::JetskiTraining(int enabled) : JetskiCompetition(enabled)
{
    m_x3EC = 0;
    m_x3F0 = 0;
    m_x404 = 0;
    m_x408 = 0;
    m_x40C = 0.25f;
    m_x3F4 = 0;
    m_x3F8 = 0;
    m_x3FC = 0;
    m_x400 = 0;
    m_test = 0;
    m_section = 0;
    m_x3E4 = 1;
    m_x468 = 1.0f;
    m_x3E8 = 0;
    m_x46C = 0;
    m_titleText = 0;
    m_instructionText = 0;
    m_statusText = 0;
    m_tests[0].m_next = 0;
    m_tests[1].m_next = 0;
    m_tests[2].m_next = 0;
    m_tests[0].m_last = 0;
    m_tests[1].m_last = 0;
    m_tests[2].m_last = 0;
    m_dialogue = 0;
    m_x590.x = 0.0f;
    m_x590.y = 0.0f;
    m_x590.z = 0.0f;
    m_x680 = ZeroVector;
    m_x690 = ZeroVector;
    m_x648 = 1;
    m_x64C = 1;
    m_trainingComplete = 0;
    m_x654 = 1;
    m_x6A0 = 0;
    m_x6A4 = 0;
    m_x670 = 1;
    m_x668 = 0;
    m_intro.m_next = 0;
    m_intro.m_last = 0;
    m_intro.m_start = 0;
    m_intro.m_end = 0;
    m_x570 = 0;
    m_random.m_count = 0;
    m_random.m_first = 0;
    m_x410 = 0;
    m_x66C = 0;
    m_savedControllerPreset = 1;
    m_x6A8 = 0;
    m_dialogTimer = 0;
    m_savedFloat0 = 0;
    m_savedFloat1 = 0;
    m_x470 = 0;
    m_numDialogues = 0;
    m_random.m_played = 0;
    m_random.m_last = 0;
    m_x674 = 0;
    m_x474 = 0;
    int i;
    for (i = 0; i < 20; i++)
        m_checkpointHit[i] = 0;
    for (i = 0; i < 8; i++) {
        m_tests[0].m_start[i] = 0;
        m_tests[0].m_end[i] = 0;
        m_tests[1].m_start[i] = 0;
        m_tests[1].m_end[i] = 0;
        m_tests[2].m_start[i] = 0;
        m_tests[2].m_end[i] = 0;
    }
    // `i <= 41` (not `i < 42`) gives retail's slti into $at.
    for (i = 0; i <= 41; i++)
        m_hasDemo[i] = 0;
}



// ---- src/JetskiTraining/dtor.cpp
JetskiTraining::~JetskiTraining()
{
    delete m_titleText;
    delete m_instructionText;
    delete m_statusText;
}



// ---- src/JetskiTraining/Create.cpp
JetskiTraining *JetskiTraining::Create()
{
    int ok = 1;
    if (!JetskiCompetition::Create())
        ok = 0;
    if (ok && !AttachScene())
        ok = 0;
    if (LpGame->m_textureCache)
        LpTextureMapManager->SetupCache();
    if (ok)
        SetupOverlays();
    m_titleText = new ScreenText(40, 0);
    m_instructionText = new ScreenText(40, 0);
    m_statusText = new ScreenText(40, 0);
    m_status = 5;
    RelocateJetski(m_test);
    m_raceStatus->EnableAllVehicles();
    m_raceStatus->m_flags |= 1;
    ChangeCamera(1);
    m_vehicles[1]->m_engineSound->StopEngineSounds();
    m_vehicles[1]->DisableAll();
    LoadIntroDialog();
    m_tests[0].m_next = m_tests[0].m_start[m_test];
    m_tests[0].m_last = m_tests[0].m_next;
    m_tests[1].m_next = m_tests[1].m_start[m_test];
    m_tests[1].m_last = m_tests[1].m_next;
    m_tests[2].m_next = m_tests[2].m_start[m_test];
    m_tests[2].m_last = m_tests[2].m_next;
    m_intro.m_next = m_intro.m_start;
    m_intro.m_last = m_intro.m_next;
    if (!LoadCheckpoints())
        ok = 0;
    for (int i = 0; i < 20; i++) {
        TriggerObject *trigger = new TriggerObject(1);
        trigger->Create(CheckpointTrigger[i].m_position, CheckpointTrigger[i].m_box,
                        CheckpointTrigger[i].m_look, CheckpointTrigger[i].m_up,
                        CheckpointTrigger[i].m_radius, 1);
        AddChild(trigger, 2);
        trigger->AddCallback(this, TrainingTriggerCallBack);
        trigger->AddSearchPlacement(*m_scene->m_dynamicNodes);
    }
    switch (LpGame->m_gameOptions->m_x4C) {
    case 1:
        m_test = 0;
        m_trainingComplete = 1;
        break;
    case 2:
        m_test = 0;
        break;
    case 3:
        m_test = 2;
        break;
    case 4:
        m_test = 3;
        break;
    case 5:
        m_test = 4;
        break;
    case 6:
        m_test = 5;
        break;
    case 7:
        m_test = 7;
        break;
    default:
        PSXprintf("Bad Selection, using test $d\n", m_test);   // "$d" sic
        m_trainingComplete = 1;
        break;
    }
    if (ok) {
        PSXprintf("SUCCESSFUL CREATION OF TRAINING\n");
        return this;
    }
    PSXprintf("FAILED CREATION OF TRAINING\n");
    return 0;
}



// ---- src/JetskiTraining/InitializeRace.cpp


int JetskiTraining::InitializeRace()
{
    if (!JetskiCompetition::InitializeRace()) {
        PSXprintf("JetskiTraining::InitializeRace FAILED!  Aborting...\n");
        return 0;
    }
    SetupOverlays();
    return 1;
}


// ---- src/JetskiTraining/SetupOverlays.cpp
void JetskiTraining::SetupOverlays()
{
    m_inGameStats->SetCompetition(this);
    EnableOverlays();
    if (m_performanceMeter)
        m_performanceMeter->SetCompetition(this);
}


// ---- src/JetskiTraining/EnableOverlays.cpp


void JetskiTraining::EnableOverlays()
{
    m_inGameStats->SetTotalRaceTime(1);
    m_inGameStats->SetBestLap(1);
    m_inGameStats->SetLapTime(1);
    m_inGameStats->SetTimeBehindLeader(1);
    m_inGameStats->SetPosition(1);
    m_inGameStats->SetLap(1);
}


// ---- src/JetskiTraining/EventComplete.cpp
void JetskiTraining::EventComplete()
{
    UnloadIntroDialog();
    LpGame->m_gameOptions->m_racerCharacter[0] = m_savedFlags;
    LpGame->m_gameOptions->m_x828 = (int)m_savedFloat0;
    LpGame->m_gameOptions->m_x82C = (int)m_savedFloat1;
    LpGame->m_gameOptions->ChangeControllerPreset(m_savedControllerPreset, 0);
    LpGame->m_gameOptions->RestoreSplitScreenMode();
    JetskiCompetition::EventComplete();
    LpGame->m_gameOptions->SetUI(1, 4);
    LpGame->m_loadManager->SetStatus(3, 0, 0);
}


// ---- src/JetskiTraining/Tick.cpp
static inline _D3DVECTOR Difference(const _D3DVECTOR &a, const _D3DVECTOR &b)
{
    return _D3DVECTOR(a.x - b.x, a.y - b.y, a.z - b.z);
}

// d3dtypes.h's operator!= returns bool; this file's comparison returned int.
static inline int NotEqual(const _D3DVECTOR &a, const _D3DVECTOR &b)
{
    return a.x != b.x || a.y != b.y || a.z != b.z;
}

// JetskiTraining.h types +0x408, +0x674, +0x6A4 and +0x6A8 as int; this
// function uses them as floats (timers and a saved performance value).
int JetskiTraining::Tick(float dt)
{
    if (m_x654) {
        LpGame->m_competition->m_scene->m_soundGroup->SetVolume(LpGame->m_gameOptions->m_x828);
        LpGame->m_auralScape->m_soundInterface->SetStreamVolume(100);
        m_x654 = 0;
        ResetTest(m_test, 1);
        m_x668 = 1;
        m_inGameStats->SetShowing(0, 0);
        m_performanceMeter->SetShowing(1, 1);
        m_performanceMeter->SetSpeedometer(1);
        m_performanceMeter->SetPerformance(1);
    }
    if (m_status == 6)
        return JetskiCompetition::Tick(dt);
    if (m_section == 0 && m_x470) {
        if (m_videoFX->FadeIn(250, 0) || (m_videoFX->m_state[0] != 3 && m_x474)) {
            m_x470 = 0;
            if (m_x46C == 0.25f) {
                PSXprintf("IsFaded\n");
                m_x46C = 0;
                if (m_x3E4 == 1)
                    ChangeCamera(1);
                m_vehicles[5]->m_x790 = 1;
                RelocateJetski(m_test);
                m_x6A0 = 1;
                m_x474 = 0;
                m_x6A4 = 0.25f;
            }
        } else {
            return JetskiCompetition::Tick(dt);
        }
    }
    if (m_x6A0) {
        if (m_x6A4 <= 0.0f) {
            m_x6A0 = 0;
            m_section = m_x3E4;
            PSXprintf("End of fade in timer\n");
        }
        m_x6A4 -= dt;
    }
    if (m_section)
        PlayTrainingDialog(dt);
    if (m_section == 2) {
        m_vehicles[5]->m_x790 = 0;
        if (m_vehicles[5]->m_x530) {
            m_section = 4;
            m_x3F4 = 1;
        }
        if (m_checkpointHit[13] || m_checkpointHit[14] || m_checkpointHit[15] || m_checkpointHit[16] ||
            m_checkpointHit[17] || m_checkpointHit[18] || m_checkpointHit[19])
            m_section = 4;
    }
    if (m_x3F4 && !m_vehicles[5]->m_x530)
        ResetTest(8, 0);
    if (m_section == 1) {
        m_vehicles[5]->m_x790 = 1;
        if (!(m_x410 < 0.0f)) {
            WriteTestInfo();
            m_x410 -= dt;
        }
        if ((m_tests[0].m_next > m_tests[0].m_end[m_test] && m_x648 && !(m_dialogTimer < 0.75f) && m_x64C) ||
            m_x670) {
            m_x670 = 1;
            m_status = 5;
            m_vehicles[0]->m_x1CC = 0;
            m_vehicles[1]->m_x1CC = 0;
            ChangeCamera(0);
            m_vehicles[5]->m_x790 = 0;
            m_section = 2;
            PSXprintf("Out of intro and into running\n");
        }
        if (m_test == 5) {
            switch (m_x3FC) {
            case 0:
            case 1:
                if (!m_x3F0 && m_vehicles[1]->m_body->m_x30) {
                    m_x3F0 = 1;
                    m_x400 = 0;
                }
                if (m_x3F0 && !m_vehicles[1]->m_body->m_x30)
                    m_x3F0 = 0;
                if (!m_x400 && m_vehicles[1]->m_rider->m_stuntF != -1.0f) {
                    m_vehicles[1]->m_performance = 0.4f + 0.4f * m_x3FC;
                    m_x3FC++;
                    m_x400 = 1;
                }
                break;
            case 2:
                if (!m_x3F0 && m_vehicles[1]->m_body->m_x30)
                    m_x3F0 = 1;
                if (m_x3F0 && !m_vehicles[1]->m_body->m_x30) {
                    m_vehicles[1]->m_performance = 0;
                    m_x3F0 = 0;
                }
                break;
            }
        } else if (m_test == 6) {
            switch (m_x3FC) {
            case 0:
                m_vehicles[1]->m_performance = 0.7f;
                m_x3FC++;
                break;
            case 1:
                if (!m_x3F0 && m_vehicles[1]->m_body->m_x30) {
                    m_x3F0 = 1;
                    m_x400 = 0;
                }
                if (m_x3F0 && !m_vehicles[1]->m_body->m_x30)
                    m_x3F0 = 0;
                if (!m_x400 && m_vehicles[1]->m_rider->m_stuntF != -1.0f) {
                    m_vehicles[1]->m_performance = 1.1f;
                    m_x3FC++;
                    m_x400 = 1;
                }
                break;
            }
        }
        int result = JetskiCompetition::Tick(dt);
        if (NotEqual(m_x680, ZeroVector) && m_x690 == ZeroVector) {
            m_x690 = Difference(m_x680, m_vehicles[1]->m_x290);
        }
        if (NotEqual(m_x690, ZeroVector)) {
            m_vehicles[1]->m_x290 += m_x690;
            m_vehicles[1]->m_x2C0 += m_x690;
        }
        return result;
    }
    switch (m_test) {
    case 0:
        if (m_section == 2) {
            if (m_checkpointHit[0])
                m_section = 3;
            if (m_checkpointHit[1])
                m_section = 4;
            if (m_checkpointHit[2])
                m_section = 4;
            if (m_checkpointHit[3])
                m_section = 4;
        }
        m_x66C = 0;
        break;
    case 1:
        if (m_section == 2) {
            if (m_vehicles[5]->m_body->m_x30 && m_vehicles[5]->m_throttle)
                m_x3E8 = 1;
            if (!m_x3EC && m_vehicles[5]->m_body->m_x30)
                m_x3EC = 1;
            if (m_x3EC && !m_vehicles[5]->m_body->m_x30) {
                if (m_checkpointHit[8]) {
                    if (m_x3E8) {
                        m_section = 3;
                        PSXprintf("Pitched in air\n");
                    }
                } else {
                    m_x3E8 = 0;
                    m_x3EC = 0;
                }
            }
        }
        if (!m_vehicles[5]->m_body->m_x30 && m_checkpointHit[8] && m_section != 3)
            m_section = 4;
        if (!m_trainingComplete)
            m_x66C = 1;
        else
            m_x66C = 0;
        break;
    case 2:
        if (m_section == 2) {
            float speed;
            m_vehicles[5]->RSGetVehicleStatus(0, 0, &speed, 0);
            if (!(speed <= 70.0f) && !(m_vehicles[5]->m_throttle <= 0.0f)) {
                m_x408 += dt;
                if (!(m_x408 < 3.0f)) {
                    m_section = 3;
                    PSXprintf("HydroSpeed %g\n", speed);
                }
            } else {
                m_x408 = 0;
            }
        }
        if (!m_trainingComplete)
            m_x66C = 1;
        else
            m_x66C = 0;
        break;
    case 3:
        if (m_section == 2) {
            if (m_checkpointHit[6])
                PSXprintf("Entered sub area");
            if (m_checkpointHit[7])
                PSXprintf("Fin sub area");
            if (m_checkpointHit[6] && m_checkpointHit[7])
                m_section = 3;
        }
        if (!m_trainingComplete)
            m_x66C = 1;
        else
            m_x66C = 0;
        break;
    case 4:
        if (m_section == 2 && !m_vehicles[5]->m_body->m_x30 && m_checkpointHit[10])
            m_section = 3;
        if (!m_trainingComplete)
            m_x66C = 1;
        else
            m_x66C = 0;
        break;
    case 5:
        if (m_section == 2) {
            if (m_vehicles[5]->m_body->m_x30 && m_vehicles[5]->m_airControl->m_x3C)
                m_x3E8 = 1;
            if (!m_x3EC && m_vehicles[5]->m_body->m_x30)
                m_x3EC = 1;
            if (m_x3EC && !m_vehicles[5]->m_body->m_x30) {
                if (m_x3E8) {
                    m_section = 3;
                    PSXprintf("Stunt in air\n");
                } else {
                    m_x3E8 = 0;
                    m_x3EC = 0;
                }
            }
            if (IsOutOfBounds(0))
                m_section = 4;
        }
        if (!m_vehicles[5]->m_body->m_x30 && m_checkpointHit[12] && m_section != 3)
            m_section = 4;
        m_x66C = 0;
        break;
    case 6:
        if (!m_vehicles[5]->m_body->m_x30)
            m_x674 = m_vehicles[5]->m_performance;
        if (!m_vehicles[5]->m_body->m_x30)
            m_x408 += dt;
        if (!m_x3EC && m_vehicles[5]->m_body->m_x30) {
            m_x3EC = 1;
            m_x408 = 0;
        }
        if (m_x3EC && !m_vehicles[5]->m_body->m_x30) {
            m_x3EC = 0;
            m_x408 = 0;
        }
        if (!(m_x408 < 1.0f)) {
            if (!(m_vehicles[5]->m_performance <= 1.0f))
                m_section = 3;
        }
        if (IsOutOfBounds(0))
            m_section = 4;
        if (!m_trainingComplete)
            m_x66C = 1;
        else
            m_x66C = 0;
        break;
    case 7:
        if (m_section == 2) {
            if (m_vehicles[0]->m_x7A4)
                m_section = 3;
        }
        m_x66C = 1;
        break;
    }
    if (m_section == 2 && m_test != 6) {
        if (!(m_x6A8 < 20.0f))
            m_section = 4;
        m_x6A8 += dt;
    }
    if (m_section == 3 || m_section == 4) {
        m_vehicles[5]->m_x790 = 1;
        if (m_x648 && !(m_dialogTimer < 0.75f))
            m_x468 -= dt;
    }
    if (m_x468 <= 0.0f) {
        m_x468 = 1.0f;
        if (m_section == 3) {
            if (m_x66C) {
                Pause(1);
                m_status = 6;
                LpGame->m_paused = 1;
                ((JetskiTrainingInGameMenu *)m_inGameMenu)->m_selection = NextTestForInGameMenu();
                ((JetskiTrainingInGameMenu *)m_inGameMenu)->EnableMenu();
            } else {
                ResetTest(m_test + 1, 1);
            }
        } else {
            ResetTest(8, 0);
        }
    }
    return JetskiCompetition::Tick(dt);
}


// ---- src/JetskiTraining/RelocateJetski.cpp
void JetskiTraining::RelocateJetski(int test)
{
    _D3DVECTOR position;
    _D3DVECTOR direction;
    position.x = JetskiRelocation[test][0];
    position.y = JetskiRelocation[test][1];
    position.z = JetskiRelocation[test][2];
    direction.x = JetskiRelocation[test][3];
    direction.y = JetskiRelocation[test][4];
    direction.z = JetskiRelocation[test][5];
    m_vehicles[5]->RSSetResetParameters(&position, &direction);
    m_vehicles[5]->ResetPosition(1);
    m_vehicles[1]->RSSetResetParameters(&position, &direction);
    m_vehicles[1]->ResetPosition(1);
}


// ---- src/JetskiTraining/WriteTestInfo.cpp
// Shows the current test's title and instructions.
void JetskiTraining::WriteTestInfo()
{
    ScreenTextFontItem *item;
    ScreenTextFont *infoFont = (item = LpGame->m_fonts->GetScreenTextFont("info_c")) ? item->m_font : 0;
    ScreenTextFont *titleFont = (item = LpGame->m_fonts->GetScreenTextFont("title_b")) ? item->m_font : 0;
    m_statusText->SetPosition(320.0f, 50.0f);
    m_statusText->SetScale(1.2860000f);
    m_instructionText->SetPosition(320.0f, 70.0f);
    switch (m_test) {
    case 0:
        m_statusText->SetString(LpGame->m_text->GetString(159));
        m_instructionText->SetString(LpGame->m_text->GetString(165));
        m_textManager->PositionText(m_statusText, ScreenTextManager::CenterHorizontalJustification, infoFont);
        m_textManager->PositionText(m_instructionText, ScreenTextManager::CenterHorizontalJustification, titleFont);
        m_textManager->AddText(m_statusText, infoFont);
        m_textManager->AddText(m_instructionText, titleFont);
        break;
    case 1:
        m_statusText->SetString(LpGame->m_text->GetString(159));
        m_instructionText->SetString(LpGame->m_text->GetString(165));
        break;
    case 2:
        m_statusText->SetString(LpGame->m_text->GetString(160));
        m_instructionText->SetString(LpGame->m_text->GetString(166));
        m_textManager->PositionText(m_statusText, ScreenTextManager::CenterHorizontalJustification, infoFont);
        m_textManager->PositionText(m_instructionText, ScreenTextManager::CenterHorizontalJustification, titleFont);
        m_textManager->AddText(m_statusText, infoFont);
        m_textManager->AddText(m_instructionText, titleFont);
        break;
    case 3:
        m_statusText->SetString(LpGame->m_text->GetString(161));
        m_instructionText->SetString(LpGame->m_text->GetString(167));
        m_textManager->PositionText(m_statusText, ScreenTextManager::CenterHorizontalJustification, infoFont);
        m_textManager->PositionText(m_instructionText, ScreenTextManager::CenterHorizontalJustification, titleFont);
        m_textManager->AddText(m_statusText, infoFont);
        m_textManager->AddText(m_instructionText, titleFont);
        break;
    case 4:
        m_statusText->SetString(LpGame->m_text->GetString(162));
        m_instructionText->SetString(LpGame->m_text->GetString(168));
        m_textManager->PositionText(m_statusText, ScreenTextManager::CenterHorizontalJustification, infoFont);
        m_textManager->PositionText(m_instructionText, ScreenTextManager::CenterHorizontalJustification, titleFont);
        m_textManager->AddText(m_statusText, infoFont);
        m_textManager->AddText(m_instructionText, titleFont);
        break;
    case 5:
        m_statusText->SetString(LpGame->m_text->GetString(163));
        m_instructionText->SetString(LpGame->m_text->GetString(169));
        m_textManager->PositionText(m_statusText, ScreenTextManager::CenterHorizontalJustification, infoFont);
        m_textManager->PositionText(m_instructionText, ScreenTextManager::CenterHorizontalJustification, titleFont);
        m_textManager->AddText(m_statusText, infoFont);
        m_textManager->AddText(m_instructionText, titleFont);
        break;
    case 6:
        m_statusText->SetString(LpGame->m_text->GetString(163));
        m_instructionText->SetString(LpGame->m_text->GetString(169));
        break;
    case 7:
        m_statusText->SetString(LpGame->m_text->GetString(164));
        m_instructionText->SetString(LpGame->m_text->GetString(170));
        m_textManager->PositionText(m_statusText, ScreenTextManager::CenterHorizontalJustification, infoFont);
        m_textManager->PositionText(m_instructionText, ScreenTextManager::CenterHorizontalJustification, titleFont);
        m_textManager->AddText(m_statusText, infoFont);
        m_textManager->AddText(m_instructionText, titleFont);
        break;
    }
}


// ---- src/JetskiTraining/ResetTest.cpp
int JetskiTraining::ResetTest(int test, int restart)
{
    m_status = 5;
    m_x670 = 1;
    if (m_dialogue->m_playing) {
        PSXprintf("DIALOGUE CUT OFF BY RESET");
        m_x648 = 1;
        m_dialogue->Stop();
    }
    if (!m_vcr->TestEOT()) {
        m_x64C = 1;
        m_vcr->RESET();
        m_vehicles[1]->m_x1CC = 0;
        m_x680 = ZeroVector;
        m_x690 = ZeroVector;
    }
    if (test >= 0 && test < 8) {
        if (restart) {
            m_x40C = 0.25f;
            m_x410 = 3.0f;
            m_x3E4 = 1;
            m_x670 = 0;
        } else {
            m_x3E4 = 2;
        }
        ResetFlags();
        m_test = test;
        m_section = 0;
        m_x46C = 0.25f;
        m_tests[0].m_next = m_tests[0].m_start[m_test];
        m_tests[0].m_last = m_tests[0].m_next;
        m_tests[1].m_next = m_tests[1].m_start[m_test];
        m_tests[1].m_last = m_tests[1].m_next;
        m_tests[2].m_next = m_tests[2].m_start[m_test];
        m_tests[2].m_last = m_tests[2].m_next;
        m_vehicles[1]->m_ownStatus->m_x6F4 = 0;
        m_vehicles[0]->m_ownStatus->m_x6F4 = 0;
        m_videoFX->FadeOut(250, 0);
        m_x470 = 1;
    } else if (test == 8) {
        if (restart) {
            m_x40C = 0.25f;
            m_x410 = 3.0f;
            m_x3E4 = 1;
            m_x670 = 0;
        } else {
            m_x3E4 = 2;
        }
        m_section = 0;
        m_x46C = 0.25f;
        m_videoFX->FadeOut(250, 0);
        m_x470 = 1;
        ResetFlags();
        m_tests[1].m_next = m_tests[1].m_start[m_test];
        m_tests[1].m_last = m_tests[1].m_next;
        m_tests[2].m_next = m_tests[2].m_start[m_test];
        m_tests[2].m_last = m_tests[2].m_next;
    } else {
        return 0;
    }
    return 1;
}


// ---- src/JetskiTraining/LoadIntroDialog.cpp
// Reads training\<lang>\training.aud: which dialogue streams belong to each
// test's intro/pass/fail, the welcome speech and the wreck remarks.
int JetskiTraining::LoadIntroDialog()
{
    ParameterBlock block;
    char directory[0x110];
    char path[0x110];
    char key[0x14];
    int demo;
    int sound;
    int num;
    int j;
    int i;

    switch (LpGame->m_gameOptions->m_language) {
    case 1:
        sprintf(directory, "%s%s", "training\\", "fre\\");
        m_numDialogues = 41;
        break;
    case 2:
        sprintf(directory, "%s%s", "training\\", "ger\\");
        m_numDialogues = 41;
        break;
    case 3:
        sprintf(directory, "%s%s", "training\\", "ita\\");
        m_numDialogues = 41;
        break;
    case 4:
        sprintf(directory, "%s%s", "training\\", "spa\\");
        m_numDialogues = 41;
        break;
    default:
        sprintf(directory, "%s%s", "training\\", "eng\\");
        m_numDialogues = 50;
        break;
    }
    m_dialogue = new JetskiDialogueStreamer(LpGame->m_auralScape->m_soundInterface, 1);
    m_dialogue->Create(m_numDialogues, directory);
    AddChild(m_dialogue, 2);
    switch (LpGame->m_gameOptions->m_language) {
    case 1:
        sprintf(directory, "%s%s", "Audio\\training\\", "fre\\");
        break;
    case 2:
        sprintf(directory, "%s%s", "Audio\\training\\", "ger\\");
        break;
    case 3:
        sprintf(directory, "%s%s", "Audio\\training\\", "ita\\");
        break;
    case 4:
        sprintf(directory, "%s%s", "Audio\\training\\", "spa\\");
        break;
    default:
        sprintf(directory, "%s%s", "Audio\\training\\", "eng\\");
        break;
    }
    sprintf(path, "%s%s", directory, "training.aud");
    if (!block.OpenFile(path, 0)) {
        PSXprintf("PB PROB:Can't load %s\n", path);
        return 0;
    }
    for (i = 0; i < 8; i++) {
        if (!block.ReadParameterBlock((char *)TestTitles[i]))
            continue;
        if (block.GetParameter("NumSounds", 0, &num)) {
            for (j = 1; j <= num; j++) {
                sprintf(key, "Sound_%d", j);
                block.GetParameter(key, &sound);
                if (j == 1)
                    m_tests[0].m_start[i] = sound;
                sprintf(key, "Demo_%d", j);
                block.GetParameter(key, &demo);
                if (demo)
                    m_hasDemo[sound] = 1;
            }
            m_tests[0].m_end[i] = sound;
        }
        if (block.GetParameter("NumPassSounds", 0, &num)) {
            for (j = 1; j <= num; j++) {
                sprintf(key, "Pass_Sound_%d", j);
                block.GetParameter(key, &sound);
                if (j == 1)
                    m_tests[1].m_start[i] = sound;
            }
            m_tests[1].m_end[i] = sound;
        }
        if (block.GetParameter("NumFailSounds", 0, &num)) {
            for (j = 1; j <= num; j++) {
                sprintf(key, "Fail_Sound_%d", j);
                block.GetParameter(key, &sound);
                if (j == 1)
                    m_tests[2].m_start[i] = sound;
            }
            m_tests[2].m_end[i] = sound;
        }
    }
    if (block.ReadParameterBlock("Welcome") && block.GetParameter("NumSounds", 0, &num)) {
        for (j = 1; j <= num; j++) {
            sprintf(key, "Sound_%d", j);
            block.GetParameter(key, &sound);
            if (j == 1)
                m_intro.m_start = sound;
        }
        m_intro.m_end = sound;
    }
    if (block.ReadParameterBlock("WreckSounds") && block.GetParameter("NumSounds", 0, &num)) {
        m_random.m_count = num;
        for (j = 1; j <= num; j++) {
            sprintf(key, "Sound_%d", j);
            block.GetParameter(key, &sound);
            if (j == 1)
                m_random.m_first = sound;
        }
    }
    PSXprintf("Loaded Dialogues\n");
    return 1;
}


// ---- src/JetskiTraining/UnloadIntroDialog_00392750.cpp


// Only its GameObject base matters here.

void JetskiTraining::UnloadIntroDialog()
{
    if (m_dialogue)
        m_dialogue->Release();
}


// ---- src/JetskiTraining/PlayTrainingDialog.cpp
// Dialogs picked at random (without repeating the last one).

#define RAND_SCALE 4.656613e-10f


int JetskiTraining::PlayTrainingDialog(float dt)
{
    if (m_x648)
        m_dialogTimer += dt;
    if (m_section == 2)
        return 1;
    if (!m_x648 && !m_dialogue->m_playing) {
        m_x648 = 1;
        m_dialogTimer = 0;
    }
    if (m_x668) {
        if (!(m_intro.m_next > m_intro.m_end)) {
            if (m_x648 && !(m_dialogTimer < 0.75f)) {
                m_dialogue->Play(m_intro.m_next, 0);
                m_x648 = 0;
                m_intro.m_last = m_intro.m_next;
                m_intro.m_next++;
            }
            return 1;
        }
        m_x668 = 0;
        return 1;
    }
    switch (m_section) {
    case 1:
        if (!m_x64C && m_vcr->TestEOT()) {
            m_x64C = 1;
            m_vcr->RESET();
            m_vehicles[1]->m_x1CC = 0;
            m_x680 = ZeroVector;
            m_x690 = ZeroVector;
        }
        if (!(m_tests[0].m_next > m_tests[0].m_end[m_test]) &&
            m_x64C && m_x648 && !(m_dialogTimer < 0.75f) && !m_x670) {
            PSXprintf("Play next dialog: %d\n", m_tests[0].m_next);
            if (m_hasDemo[m_tests[0].m_next]) {
                char name[0x110];
                sprintf(name, "%sdemo%d.vcr", "Training\\", m_tests[0].m_next);
                m_x680 = m_vehicles[1]->m_x290;
                LoadVCRAndRun(name);
                m_x64C = 0;
            } else if (m_tests[0].m_next == m_tests[0].m_end[m_test]) {
                m_vehicles[0]->m_x1CC = 0;
                m_vehicles[1]->m_x1CC = 0;
                ChangeCamera(0);
            }
            m_dialogue->Play(m_tests[0].m_next, 0);
            m_x648 = 0;
            m_tests[0].m_last = m_tests[0].m_next;
            m_tests[0].m_next++;
        }
        break;
    case 3:
        if (!(m_tests[1].m_next > m_tests[1].m_end[m_test]) && m_x648 && !(m_dialogTimer < 0.75f)) {
            m_dialogue->Play(m_tests[1].m_next, 0);
            m_tests[1].m_last = m_tests[1].m_next;
            m_tests[1].m_next++;
            m_x648 = 0;
        }
        break;
    case 4:
        if (m_x3F4 && m_random.m_first) {
            if (!m_random.m_played && m_x648 && !(m_dialogTimer < 0.75f)) {
                int pick = (int)(m_random.m_count * (RAND_SCALE * rand()));
                if (pick == m_random.m_last) {
                    pick++;
                    pick %= m_random.m_count;
                }
                if (m_dialogue->Play(pick + m_random.m_first, 0)) {
                    m_x648 = 0;
                    m_random.m_played = 1;
                    m_random.m_last = pick;
                }
            }
        } else if (!(m_tests[2].m_next > m_tests[2].m_end[m_test]) && m_x648 && !(m_dialogTimer < 0.75f)) {
            m_dialogue->Play(m_tests[2].m_next, 0);
            m_tests[2].m_last = m_tests[2].m_next;
            m_tests[2].m_next++;
            m_x648 = 0;
        }
        break;
    default:
        PSXprintf("Invalid eSectionState in PlayTrainingDialog");
        return 0;
    }
    return 1;
}


// ---- src/JetskiTraining/ResetFlags.cpp


void JetskiTraining::ResetFlags()
{
    m_x3EC = 0;
    m_x3F0 = 0;
    m_x404 = 0;
    m_x408 = 0;
    m_x3F4 = 0;
    m_x3F8 = 0;
    m_random.m_played = 0;
    m_x3E8 = 0;
    m_x6A8 = 0;
    m_x3FC = 0;
    m_x400 = 0;
    m_vehicles[0]->m_x7A4 = 0;
    m_tests[0].m_next = 0;
    m_tests[1].m_next = 0;
    m_tests[2].m_next = 0;
    m_tests[0].m_last = 0;
    m_tests[1].m_last = 0;
    m_tests[2].m_last = 0;
    m_x590.x = 0;
    m_x590.y = 0;
    m_x590.z = 0;
    for (int i = 0; i < 20; i++)
        m_checkpointHit[i] = 0;
}


// ---- src/JetskiTraining/ChangeCamera.cpp
void JetskiTraining::ChangeCamera(int jetski)
{
    if (jetski == 0) {
        m_vehicles[1]->m_engineSound->StopEngineSounds();
        m_vehicles[1]->DisableAll();
        m_vehicles[0]->EnableAll();
        m_vehicles[0]->ResetPosition(1);
        m_cameraControllers[0]->SetJetski(m_vehicles[0], 0);
        m_vehicles[5]->m_x790 = 1;
        m_cameraControllers[0]->m_followCamera->SwitchFilterValueForPlayback(0);
    } else if (jetski == 1) {
        m_vehicles[0]->m_engineSound->StopEngineSounds();
        m_vehicles[0]->DisableAll();
        m_vehicles[1]->EnableAll();
        m_vehicles[1]->ResetPosition(1);
        m_cameraControllers[0]->SetJetski(m_vehicles[1], 1);
        m_cameraControllers[0]->m_followCamera->SwitchFilterValueForPlayback(1);
    }
}


// ---- src/JetskiTraining/KeyDownHandler.cpp
int JetskiTraining::KeyDownHandler(ProcessNode *node, KeyInfo *key)
{
    if (m_section == 1 && KEYDOWN_HANDLER_TEST((AssignableActionIndex)4, (DeviceType)2, node, 0x80000000)) {
        // nothing (retail tests the key and ignores it)
    }
    if (KEYDOWN_HANDLER_TEST((AssignableActionIndex)0x11, (DeviceType)2, node, 0x80000000))
        return 1;
    if (KEYDOWN_HANDLER_TEST((AssignableActionIndex)8, (DeviceType)2, node, 0x80000000) && m_section != 2 && m_section != 1)
        return 1;
    return JetskiCompetition::KeyDownHandler(node, key);
}


// ---- src/JetskiTraining/ModifyLocalGameOptions.cpp
void JetskiTraining::ModifyLocalGameOptions()
{
    JetskiGameOptions *options = LpGame->m_gameOptions;
    options->SaveCurrentSplitScreenMode(0, (ViewPortCode)0);
    m_numPlayers = options->m_numHumans;
    options->m_x2C = 99;
    m_savedFlags = options->m_racerCharacter[0];
    options->m_racerCharacter[0] = 0;
    options->m_racerVehicle[0] = options->GetVehicleID(0);
    options->m_racerSkin[0] = 0;
    options->m_racerVehicleSkin[0] = 0;
    switch (LpGame->m_gameOptions->m_language) {
    case 3:
    case 4:
        options->m_racerCharacter[1] = 7;
        options->m_racerVehicle[1] = options->GetVehicleID(7);
        options->m_racerSkin[1] = 0;
        options->m_racerVehicleSkin[1] = 0;
        break;
    default:
        options->m_racerCharacter[1] = 1;
        options->m_racerVehicle[1] = options->GetVehicleID(1);
        options->m_racerSkin[1] = 0;
        options->m_racerVehicleSkin[1] = 0;
        break;
    }
    m_savedFloat0 = options->m_x828;
    options->m_x828 = 25;
    m_savedFloat1 = options->m_x82C;
    options->m_x82C = 100;
    m_savedControllerPreset = options->m_controllerPreset[0];
    options->ChangeControllerPreset(1, 0);
}


// ---- src/JetskiTraining/LoadCheckpoints.cpp
// Reads the 20 training checkpoints from training\\checkpts.pb.
int JetskiTraining::LoadCheckpoints()
{
    char name[0x110];
    ParameterBlock params;
    if (!params.OpenFile("training\\checkpts.pb", 0)) {
        PSXprintf("Can't load training\\checkpts.pb\n");
        return 0;
    }
    for (int i = 0; i < 20; i++) {
        sprintf(name, "Checkpoint%d", i + 1);
        if (params.ReadParameterBlock(name)) {
            if (!params.GetParameter("Position", &ZeroVector, &CheckpointTrigger[i].m_position))
                return 0;
            if (!params.GetParameter("Box", &ZeroVector, &CheckpointTrigger[i].m_box))
                return 0;
            if (!params.GetParameter("Look", &ZeroVector, &CheckpointTrigger[i].m_look))
                return 0;
            if (!params.GetParameter("Up", &ZeroVector, &CheckpointTrigger[i].m_up))
                return 0;
            if (!params.GetParameter("Radius", &CheckpointTrigger[i].m_radius))
                return 0;
        } else {
            return 0;
        }
    }
    return 1;
}


// ---- src/JetskiTraining/NextTestForInGameMenu.cpp
int JetskiTraining::NextTestForInGameMenu()
{
    int next;
    switch (m_test) {
    case 0:
    case 1:
        ((JetskiTrainingInGameMenu *)m_inGameMenu)->m_x68 = 1;
        next = 1;
        break;
    case 2:
        ((JetskiTrainingInGameMenu *)m_inGameMenu)->m_x68 = 1;
        next = 2;
        break;
    case 3:
        ((JetskiTrainingInGameMenu *)m_inGameMenu)->m_x68 = 1;
        next = 3;
        break;
    case 4:
        ((JetskiTrainingInGameMenu *)m_inGameMenu)->m_x68 = 1;
        next = 4;
        break;
    case 5:
    case 6:
        ((JetskiTrainingInGameMenu *)m_inGameMenu)->m_x68 = 1;
        next = 5;
        break;
    case 7:
        ((JetskiTrainingInGameMenu *)m_inGameMenu)->m_x68 = 0;
        next = 3;
        break;
    default:
        ((JetskiTrainingInGameMenu *)m_inGameMenu)->m_x68 = 1;
        next = 0;
        break;
    }
    if (m_trainingComplete) {
        next = 3;
        ((JetskiTrainingInGameMenu *)m_inGameMenu)->m_x68 = 0;
    }
    return next;
}


// ---- src/JetskiTraining/IsOutOfBounds.cpp
int JetskiTraining::IsOutOfBounds(int player)
{
    float x = m_vehicles[player]->m_body->m_x60.x;
    float z = m_vehicles[player]->m_body->m_x60.z;
    if (x > 2470.0f || x < 1070.0f || z > 2430.0f || z < 1080.0f)
        return 1;
    return 0;
}


// ---- src/CheckpointSpacial/ctor.cpp

CheckpointSpacial::CheckpointSpacial()
{
}


// ---- src/sinit/JetskiTraining.cpp
CheckpointSpacial CheckpointTrigger[20];
