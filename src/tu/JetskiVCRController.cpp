// JetskiVCRController.cpp  @ 0x0038A860 (text 0x38A860-0x38B6D0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp)
//
// Retail's JetskiVCRController.cpp as one translation unit.
// JetskiMemPages.cpp (the JetskiMP* memory-card pages, the next .ctor
// slot) follows at 0x38B6D0. __sinit_JetskiVCRController.cpp builds only
// the axis vectors. `make link` places its data as well as its code.


#include "d3dtypes.h"
#include "SimpleVCR/SimpleVCR.h"
#include "JetskiVCRController/JetskiVCRController.h"
#include "DataAccess/DataAccess.h"
#include "JetskiGameOptions/JetskiGameOptions.h"
#include "JetskiGame/JetskiGame.h"
#include "JetskiCompetition/JetskiCompetition.h"
#include "JetskiVehicle/JetskiVehicle.h"
#include "PhysicsRigidBody/PhysicsRigidBody.h"
#include "JetskiEngineModel/JetskiEngineModel.h"
#include "JetskiRider/JetskiRider.h"
#include "WaterWrecker/WaterWrecker.h"
#include "sinit/AxisVectors.h"

void PSXprintf(const char *fmt, ...);

extern DataAccess *LpDataAccess;
extern JetskiGame *LpGame;

// ---- src/JetskiVCRController/ctor.cpp

JetskiVCRController::JetskiVCRController(int enabled)
{
    m_loaded = 0;
    m_ghostDirection = 2;
}

// ---- src/JetskiVCRController/dtor.cpp

JetskiVCRController::~JetskiVCRController()
{
}

// ---- src/JetskiVCRController/Create.cpp

SimpleVCR *JetskiVCRController::Create(int a, int b, int c, int d)
{
    m_ghost = d;
    return SimpleVCR::Create(a, b, c);
}

// ---- src/JetskiVCRController/REC.cpp

void JetskiVCRController::REC()
{
    Reset();
    m_recording = 1;
}

// ---- src/JetskiVCRController/STOP.cpp

void JetskiVCRController::STOP()
{
    SaveRecording();
    m_loaded = 1;
}

int JetskiVCRController::TestEOT()
{
    return m_atEnd;
}

// ---- src/JetskiVCRController/REWforPLAY.cpp

void JetskiVCRController::REWforPLAY()
{
    for (int i = 0; i < m_numPointers; i++)
        EnableChannel(i);
    Reset();
    m_playing = 1;
}

// ---- src/JetskiVCRController/REPLAYREC.cpp

void JetskiVCRController::REPLAYREC()
{
    for (int i = 0; i < m_numPointers; i++)
        EnableChannel(i);
    Reset();
    Rewind();
    m_playing = 1;
    m_recording = 1;
}

// ---- src/JetskiVCRController/RESET.cpp

void JetskiVCRController::RESET()
{
    Reset();
}

// ---- src/JetskiVCRController/LoadFromDisk.cpp

int JetskiVCRController::LoadFromDisk(char *filename)
{
    if (!SimpleVCR::LoadFromDisk(filename))
        return 0;
    m_loaded = 1;
    return 1;
}

// ---- src/JetskiVCRController/LoadGhostFromDisk.cpp

// A ghost file is the race time (negated for direction 2) then the
// recording.
int JetskiVCRController::LoadGhostFromDisk(char *name, float *raceTime)
{
    long file = LpDataAccess->FOpen(name, "rb");
    if (file == -1) {
        PSXprintf("recorded vcrfile '%s' not found\n", name);
        return 0;
    }
    float time;
    LpDataAccess->FRead(file, &time, 4, 1);
    if (time > 0.0f) {
        m_ghostDirection = 1;
        *raceTime = time;
    } else {
        m_ghostDirection = 2;
        *raceTime = -time;
    }
    LpDataAccess->FRead(file, m_bufferCopy, m_bufferSize, 1);
    LpDataAccess->FClose(file);
    m_loaded = 1;
    return 1;
}

// ---- src/JetskiVCRController/RegisterVariables.cpp

// Registers the vehicle state the VCR records and plays back; the float is
// each value's quantisation scale.
void JetskiVCRController::RegisterVariables(JetskiVehicle *vehicle)
{
    if (LpGame->m_gameOptions->m_raceType == 3) {
        if (m_ghost) {
            AddPointer(1000.0f, &LpGame->m_competition->m_x260.x, 0, 1, 1);
            AddPointer(1000.0f, &LpGame->m_competition->m_x260.y, 0, 1, 1);
            AddPointer(1000.0f, &LpGame->m_competition->m_x260.z, 0, 1, 1);
            AddPointer(500.0f, &LpGame->m_competition->m_x270.x, 0, 1, 1);
            AddPointer(500.0f, &LpGame->m_competition->m_x270.y, 0, 1, 1);
            AddPointer(500.0f, &LpGame->m_competition->m_x270.z, 0, 1, 1);
            AddPointer(500.0f, &LpGame->m_competition->m_x280.x, 0, 1, 1);
            AddPointer(500.0f, &LpGame->m_competition->m_x280.y, 0, 1, 1);
            AddPointer(500.0f, &LpGame->m_competition->m_x280.z, 0, 1, 1);
        } else {
            AddPointer(1000.0f, &vehicle->m_x290.x, 0, 1, 1);
            AddPointer(1000.0f, &vehicle->m_x290.y, 0, 1, 1);
            AddPointer(1000.0f, &vehicle->m_x290.z, 0, 1, 1);
            AddPointer(500.0f, &vehicle->m_x2A0.x, 0, 1, 1);
            AddPointer(500.0f, &vehicle->m_x2A0.y, 0, 1, 1);
            AddPointer(500.0f, &vehicle->m_x2A0.z, 0, 1, 1);
            AddPointer(500.0f, &vehicle->m_x2B0.x, 0, 1, 1);
            AddPointer(500.0f, &vehicle->m_x2B0.y, 0, 1, 1);
            AddPointer(500.0f, &vehicle->m_x2B0.z, 0, 1, 1);
            AddPointer(500.0f, &vehicle->m_x320.x, 0, 1, 1);
            AddPointer(500.0f, &vehicle->m_x320.y, 0, 1, 1);
            AddPointer(500.0f, &vehicle->m_x320.z, 0, 1, 1);
            AddPointer(100.0f, &vehicle->m_x314, 0, 1, 1);
            AddPointer(100.0f, &vehicle->m_x300, 0, 1, 1);
            AddPointer(100.0f, &vehicle->m_x304, 0, 1, 1);
            AddPointer(1000.0f, &vehicle->m_x2C0.x, 0, 1, 1);
            AddPointer(1000.0f, &vehicle->m_x2C0.y, 0, 1, 1);
            AddPointer(1000.0f, &vehicle->m_x2C0.z, 0, 1, 1);
            AddPointer(500.0f, &vehicle->m_x2E0.x, 0, 1, 1);
            AddPointer(500.0f, &vehicle->m_x2E0.y, 0, 1, 1);
            AddPointer(500.0f, &vehicle->m_x2E0.z, 0, 1, 1);
            AddPointer(500.0f, &vehicle->m_x2F0.x, 0, 1, 1);
            AddPointer(500.0f, &vehicle->m_x2F0.y, 0, 1, 1);
            AddPointer(500.0f, &vehicle->m_x2F0.z, 0, 1, 1);
            AddPointer(1.0f, &vehicle->m_x308, 0, 0, 1);
            AddPointer(1.0f, &vehicle->m_x30C, 0, 0, 1);
            AddPointer(1.0f, &vehicle->m_x310, 0, 0, 1);
        }
    } else {
        AddPointer(1000.0f, &vehicle->m_x290.x, 0, 1, 1);
        AddPointer(1000.0f, &vehicle->m_x290.y, 0, 1, 1);
        AddPointer(1000.0f, &vehicle->m_x290.z, 0, 1, 1);
        AddPointer(500.0f, &vehicle->m_x2A0.x, 0, 1, 1);
        AddPointer(500.0f, &vehicle->m_x2A0.y, 0, 1, 1);
        AddPointer(500.0f, &vehicle->m_x2A0.z, 0, 1, 1);
        AddPointer(500.0f, &vehicle->m_x2B0.x, 0, 1, 1);
        AddPointer(500.0f, &vehicle->m_x2B0.y, 0, 1, 1);
        AddPointer(500.0f, &vehicle->m_x2B0.z, 0, 1, 1);
        AddPointer(500.0f, &vehicle->m_body->m_velocity.x, 0, 1, 1);
        AddPointer(500.0f, &vehicle->m_body->m_velocity.y, 0, 1, 1);
        AddPointer(500.0f, &vehicle->m_body->m_velocity.z, 0, 1, 1);
        AddPointer(500.0f, &vehicle->m_body->m_xA0.x, 0, 1, 1);
        AddPointer(500.0f, &vehicle->m_body->m_xA0.y, 0, 1, 1);
        AddPointer(500.0f, &vehicle->m_body->m_xA0.z, 0, 1, 1);
        if (LpGame->m_gameOptions->m_raceType == 8) {
            AddPointer(500.0f, &vehicle->m_body->m_x60.x, 0, 1, 1);
            AddPointer(500.0f, &vehicle->m_body->m_x60.y, 0, 1, 1);
            AddPointer(500.0f, &vehicle->m_body->m_x60.z, 0, 1, 1);
            AddPointer(500.0f, &vehicle->m_x5A0.x, 0, 1, 1);
            AddPointer(500.0f, &vehicle->m_x5A0.y, 0, 1, 1);
            AddPointer(500.0f, &vehicle->m_x5A0.z, 0, 1, 1);
            AddPointer(100.0f, &vehicle->m_body->m_speed, 0, 1, 1);
        }
        AddPointer(100.0f, &vehicle->m_engine->m_throttle, 0, 1, 1);
        AddPointer(100.0f, &vehicle->m_body->m_speed, 0, 1, 1);
        AddPointer(1.0f, &vehicle->m_x1C8, 0, 1, 1);
        AddPointer(500.0f, &vehicle->m_body->m_lastVelocity.x, 0, 1, 1);
        AddPointer(500.0f, &vehicle->m_body->m_lastVelocity.y, 0, 1, 1);
        AddPointer(500.0f, &vehicle->m_body->m_lastVelocity.z, 0, 1, 1);
        AddPointer(100.0f, &vehicle->m_x60, 0, 1, 1);
        AddPointer(100.0f, &vehicle->m_x64, 0, 1, 1);
        AddPointer(1000.0f, &vehicle->m_x2C0.x, 0, 1, 1);
        AddPointer(1000.0f, &vehicle->m_x2C0.y, 0, 1, 1);
        AddPointer(1000.0f, &vehicle->m_x2C0.z, 0, 1, 1);
        AddPointer(500.0f, &vehicle->m_x2E0.x, 0, 1, 1);
        AddPointer(500.0f, &vehicle->m_x2E0.y, 0, 1, 1);
        AddPointer(500.0f, &vehicle->m_x2E0.z, 0, 1, 1);
        AddPointer(500.0f, &vehicle->m_x2F0.x, 0, 1, 1);
        AddPointer(500.0f, &vehicle->m_x2F0.y, 0, 1, 1);
        AddPointer(500.0f, &vehicle->m_x2F0.z, 0, 1, 1);
        AddPointer(1.0f, &vehicle->m_rider->m_stuntF, 0, 0, 1);
        AddPointer(1.0f, &vehicle->m_rider->m_x898, 0, 0, 1);
        AddPointer(1.0f, &vehicle->m_rider->m_wrecker->m_wreckType, 0, 0, 1);
    }
}
