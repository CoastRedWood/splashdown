// JetskiStuntsKey.cpp  @ 0x003724D0 (text 0x3724D0-0x374580)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's JetskiStuntsKey.cpp as one translation unit: JetskiStuntsKey's
// and JetskiCheatKey's functions, then the static initializer. `make link`
// places the object's data as well as its code (tools/link_c.py). The rest
// of the tu_map row (0x374580-0x375590: Clut, Clut4, Clut8, UIWaterPatch) is
// UIWater.cpp (src/tu/UIWater.cpp): its compiler labels restart at the Clut
// RTTI string (@467 after this TU's @1775) and __sinit_UIWater.cpp is the
// next .ctor slot.

#include "JetskiStuntsKey/JetskiStuntsKey.h"
#include "ParameterBlock/ParameterBlock.h"
#include "MotionLibrary/MotionLibrary.h"
#include "JoystickDevice/JoystickDevice.h"
#include "JetskiVehicle/JetskiVehicle.h"
#include "JetskiCheatKey/JetskiCheatKey.h"
#include "KeyInfo/KeyInfo.h"
#include "Game/Game.h"
#include "ControlInterface/ControlInterface.h"
#include "sinit/AxisVectors.h"

extern "C" char *strcpy(char *dst, const char *src);
extern "C" char *strncpy(char *dst, const char *src, unsigned int n);
extern "C" int sprintf(char *buf, const char *fmt, ...);
extern "C" int strcasecmp(const char *a, const char *b);

extern Game *LpGame;

// ---- src/JetskiStuntsKey/ctor.cpp

JetskiStuntsKey::JetskiStuntsKey()
{
    m_numStunts = 0;
    m_acquisitionTimeOut = 0;
    m_acquisitionTime = 0;
    m_x11C = 0;
    m_breakTimeOut = 0;
    m_holdTime = 0;
    m_numKeys = 0;
    m_x140 = 0;
    m_holding = 0;
    m_currentStunt = -1;
    m_buttons = 0;
    m_direction = 0;
    int i;
    for (i = 0; i < 16; i++)
        m_keys[i] = 0;
    for (i = 0; i < 28; i++) {
        m_stunts[i].m_keys[0] = 0;
        m_stunts[i].m_numKeys = 0;
        m_stunts[i].m_inAir = 0;
        m_stunts[i].m_stuntTime = 0;
        m_stunts[i].m_holdStartTime = 0;
        m_stunts[i].m_holdEndTime = 0;
        m_stunts[i].m_safeStartTime = 0;
        m_stunts[i].m_safeEndTime = 0;
        m_stunts[i].m_basePoints = 0;
        m_stunts[i].m_holdPoints = 0;
    }
}


// ---- src/JetskiStuntsKey/dtor.cpp

JetskiStuntsKey::~JetskiStuntsKey()
{
}


// ---- src/JetskiStuntsKey/Initialize.cpp



void JetskiStuntsKey::Initialize(char *name, JetskiVehicle *vehicle, int player, Motion **motions)
{
    m_vehicle = vehicle;
    m_controller = LpGame->m_controls->m_joysticks[player];
    m_motions = motions;
    strncpy(m_name, name, sizeof(m_name));
    LoadKeys();
}


// ---- src/JetskiStuntsKey/LoadKeys.cpp
void PSXprintf(const char *fmt, ...);



extern MotionLibrary *LpMotionLibrary;



// Reads the stunt definitions (key sequences, timings, points) for this rider.
int JetskiStuntsKey::LoadKeys()
{
    char inAir[0x40];
    char motionName[0x40];
    char fileName[0x104];
    ParameterBlock block;
    char keySequence[0x80];
    char blockName[0x50];
    int i;
    int j;
    int index;

    strncpy(fileName, m_name, sizeof(fileName));
    if (!block.OpenFile(m_name, 0)) {
        PSXprintf("WARNING!!! Cannot open Stunt key definition file %s - JetskiStuntsKey::LoadKeys\n", m_name);
        return 0;
    }
    block.ReadParameterBlock("General info");
    if (!block.GetParameter("NumberOfStunts", 0, &m_numStunts)) {
        PSXprintf("WARNING!!! Missing NumberOfStunts in %s - JetskiStuntsKey::LoadKeys\n", m_name);
        return 0;
    }
    if (!block.GetParameter("AcquisitionTime", 0.0f, &m_acquisitionTime)) {
        PSXprintf("WARNING!!! Missing AcquisitionTime in %s - JetskiStuntsKey::LoadKeys\n", m_name);
        return 0;
    }
    if (!block.GetParameter("AcquisitionTimeOut", 0.0f, &m_acquisitionTimeOut)) {
        PSXprintf("WARNING!!! Missing AcquisitionTimeOut in %s - JetskiStuntsKey::LoadKeys\n", m_name);
        return 0;
    }
    if (!block.GetParameter("BreakTimeOut", 0.0f, &m_breakTimeOut)) {
        PSXprintf("WARNING!!! Missing BreakTimeOut in %s - JetskiStuntsKey::LoadKeys\n", m_name);
        return 0;
    }
    if (!block.GetParameter("PointsPerSecond", 0.0f, &m_pointsPerSecond)) {
        PSXprintf("WARNING!!! Missing PointsPerSecond in %s - JetskiStuntsKey::LoadKeys\n", m_name);
        return 0;
    }
    // (retail reports every missing bonus as MultiStuntBonus1)
    if (!block.GetParameter("MultiStuntBonus1", 0.0f, &m_multiStuntBonus[0])) {
        PSXprintf("WARNING!!! Missing MultiStuntBonus1 in %s - JetskiStuntsKey::LoadKeys\n", m_name);
        return 0;
    }
    if (!block.GetParameter("MultiStuntBonus2", 0.0f, &m_multiStuntBonus[1])) {
        PSXprintf("WARNING!!! Missing MultiStuntBonus1 in %s - JetskiStuntsKey::LoadKeys\n", m_name);
        return 0;
    }
    if (!block.GetParameter("MultiStuntBonus3", 0.0f, &m_multiStuntBonus[2])) {
        PSXprintf("WARNING!!! Missing MultiStuntBonus1 in %s - JetskiStuntsKey::LoadKeys\n", m_name);
        return 0;
    }
    if (!block.GetParameter("MultiStuntBonus4", 0.0f, &m_multiStuntBonus[3])) {
        PSXprintf("WARNING!!! Missing MultiStuntBonus1 in %s - JetskiStuntsKey::LoadKeys\n", m_name);
        return 0;
    }
    if (!block.GetParameter("MultiStuntBonus5", 0.0f, &m_multiStuntBonus[4])) {
        PSXprintf("WARNING!!! Missing MultiStuntBonus1 in %s - JetskiStuntsKey::LoadKeys\n", m_name);
        return 0;
    }
    if (!(m_breakTimeOut < m_acquisitionTime)) {
        PSXprintf("WARNING!!! BreakTimeOut must be smaller than AcquisitionTime - LoadKeys::Initialize\n");
        return 0;
    }
    for (i = 0; i < m_numStunts; i++) {
        sprintf(blockName, "MOTION %d", i + 1);
        block.ReadParameterBlock(blockName);
        if (!block.GetParameter("MotionName", "", motionName, sizeof(motionName))) {
            PSXprintf("WARNING!!! Missing MotionName in block %s - JetskiStuntsKey::LoadKeys\n", blockName);
            return 0;
        }
        Motion *motion = LpMotionLibrary->MotionOfName(motionName, 1);
        if (!motion) {
            PSXprintf("WARNING!!! Wrong MotionName %s - JetskiStuntsKey::LoadKeys\n", motionName);
            return 0;
        }
        for (j = 0; j < NUM_STUNT_MOTIONS; j++) {
            if (motion == m_motions[j]) {
                index = j;
                break;
            }
        }
        if (!block.GetParameter("KeySequence", "", keySequence, sizeof(keySequence))) {
            PSXprintf("WARNING!!! Missing KeySequence in block %s - JetskiStuntsKey::LoadKeys\n", blockName);
            return 0;
        }
        int numKeys = GetKeySequence(keySequence, m_stunts[index].m_keys);
        if (!numKeys) {
            PSXprintf("WARNING!!! Key Sequence error in block %s - JetskiStuntsKey::LoadKeys\n", blockName);
            return 0;
        }
        m_stunts[index].m_numKeys = numKeys;
        block.GetParameter("InAir", "FALSE", inAir, -1);
        if (!strcasecmp(inAir, "TRUE"))
            m_stunts[index].m_inAir = 1;
        else
            m_stunts[index].m_inAir = 0;
        block.GetParameter("StuntTime", 0.0f, &m_stunts[index].m_stuntTime);
        block.GetParameter("HoldStartTime", 0.0f, &m_stunts[index].m_holdStartTime);
        block.GetParameter("HoldEndTime", m_stunts[index].m_stuntTime, &m_stunts[index].m_holdEndTime);
        block.GetParameter("SafeStartTime", 1.0f, &m_stunts[index].m_safeStartTime);
        block.GetParameter("SafeEndTime", m_stunts[index].m_stuntTime, &m_stunts[index].m_safeEndTime);
        block.GetParameter("StuntBasePoint", 3000.0f, &m_stunts[index].m_basePoints);
        block.GetParameter("StuntHoldPoint", 500.0f, &m_stunts[index].m_holdPoints);
    }
    return 1;
}


// ---- src/JetskiStuntsKey/GetKeySequence.cpp
void PSXprintf(const char *fmt, ...);


// Parses a stunt key string such as "R2;UP,DOWN:..." into key bytes:
// ';' ends a hold key, ',' a key in a sequence, ':' a separate key.
// START_C/END_C (0xF0/0xFF) bracket a "circle" group. Returns the number of
// bytes written (plus the terminator), or 0 on a malformed string.
int JetskiStuntsKey::GetKeySequence(char *text, char *sequence)
{
    char buffer[128];
    char *token;
    char *p;
    char hold;
    int n;
    int circle;
    char key;

    // Unlike JetskiCheatKey::GetKeySequence, retail never terminates buffer.
    strncpy(buffer, text, sizeof(buffer));
    token = buffer;
    p = token;
    n = 0;
    hold = 0;
    circle = 0;
    for (;;) {
        while (*p != ';' && *p != ':' && *p != ',' && *p != 0)
            p++;
        switch (*p) {
        case 0:
            *p = 0;
            if (!(char)CheckHoldKey(token)) {
                int dir = (char)CheckDirectionKey(token);
                if (dir) {
                    if (circle) {
                        if (dir != -1) {
                            PSXprintf("WARNING!!! missing END_C (end of string) - JetskiStuntsKey::Initialize\n");
                            return 0;
                        }
                        char last = sequence[n - 1];
                        sequence[n - 1] = 0;
                        sequence[n] = last;
                    } else {
                        sequence[n] |= dir;
                    }
                }
            } else {
                PSXprintf("WARNING!!! you can't put a HOLD key here (end of string) - JetskiStuntsKey::Initialize\n");
                return 0;
            }
            n++;
            sequence[n] = 0;
            return n;
        case ';':
            *p = 0;
            key = CheckHoldKey(token);
            if (!key) {
                PSXprintf("WARNING!!! no Hold key (hold) - JetskiStuntsKey::Initialize\n");
                return 0;
            }
            hold |= key;
            sequence[n] = hold;
            p++;
            token = p;
            break;
        case ',':
            *p = 0;
            key = CheckDirectionKey(token);
            if (!key) {
                PSXprintf("WARNING!!! no Direction key (sequence) - JetskiStuntsKey::Initialize\n");
                return 0;
            }
            if (key == (char)0xF0) {
                sequence[n] = 0;
                n++;
                circle = 1;
            } else {
                sequence[n] = hold + key;
                n++;
                sequence[n] = hold;
            }
            p++;
            token = p;
            break;
        case ':':
            *p = 0;
            key = CheckDirectionKey(token);
            if (!key) {
                PSXprintf("WARNING!!! no Direction key (separate) - JetskiStuntsKey::Initialize\n");
                return 0;
            }
            if (key == (char)0xF0) {
                PSXprintf("WARNING!!! Can't put a CIRCLE here (separate) - JetskiStuntsKey::Initialize\n");
                return 0;
            }
            if (circle) {
                if (key != (char)0xFF) {
                    PSXprintf("WARNING!!! you didn't close a CIRCLE (separate) - JetskiStuntsKey::Initialize\n");
                    return 0;
                }
                char last = sequence[n - 1];
                sequence[n - 1] = 0;
                sequence[n] = last;
                n++;
                circle = 0;
            } else {
                sequence[n] = hold + key;
                n++;
            }
            p++;
            hold = 0;
            token = p;
            break;
        }
    }
}


// ---- src/JetskiStuntsKey/CheckHoldKey.cpp


int JetskiStuntsKey::CheckHoldKey(char *key)
{
    if (!strcasecmp(key, "S1"))
        return 1;
    if (!strcasecmp(key, "S2"))
        return 2;
    return strcasecmp(key, "S3") == 0 ? 4 : 0;
}


// ---- src/JetskiStuntsKey/CheckDirectionKey.cpp


// Direction bits; returned as a int (RIGHT is 0x80, so -0x80).
int JetskiStuntsKey::CheckDirectionKey(char *key)
{
    if (!strcasecmp(key, "UP"))
        return 0x10;
    if (!strcasecmp(key, "LEFT"))
        return 0x20;
    if (!strcasecmp(key, "DOWN"))
        return 0x40;
    if (!strcasecmp(key, "RIGHT"))
        return -0x80;
    if (!strcasecmp(key, "START_C"))
        return -0x10;
    return strcasecmp(key, "END_C") == 0 ? -1 : 0;
}


// ---- src/JetskiStuntsKey/CheckStuntActivation.cpp
// Matches the keys entered so far against each stunt's key sequence.
int JetskiStuntsKey::CheckStuntActivation(float dt, int type)
{
    if (m_numKeys) {
        for (int s = 0; s < 41; s++) {
            int k = 0;
            int j = 0;
            int skipped = 0;
            int ok = 1;
            while (j < m_stunts[s].m_numKeys) {
                signed char c = m_stunts[s].m_keys[j];
                if (c == 0 && m_stunts[s].m_keys[j + 1] == m_keys[k]) {
                    j += 2;
                    skipped += 1;
                } else if (c == 0 && m_stunts[s].m_keys[j + 2] == m_keys[k]) {
                    j += 3;
                    skipped += 2;
                } else if (c == 0 && m_numKeys == k) {
                    skipped += 2;
                    break;
                } else if (c == m_keys[k]) {
                    j++;
                } else {
                    ok = 0;
                    break;
                }
                k++;
            }
            if (!ok)
                continue;
            if (m_stunts[s].m_numKeys != m_numKeys + skipped)
                continue;
            if (!(m_x11C < m_acquisitionTime) && m_stunts[s].m_inAir == type) {
                m_holdTime = 0;
                m_x140 = 1;
                m_holding = 1;
                m_numKeys = 0;
                m_x11C = 0.0f;
                m_timer = 0;
                m_currentStunt = s;
                return s;
            }
            m_x11C += dt;
        }
    }
    return -1;
}


// ---- src/JetskiStuntsKey/CheckKeyInputs.cpp


void JetskiStuntsKey::CheckKeyInputs(float dt, int)
{
    char buttons = 0;
    int i;
    if ((m_numKeys && !m_x140 && !(m_timer <= m_acquisitionTimeOut)) || m_numKeys > 16) {
        m_numKeys = 0;
        for (i = 0; i < 16; i++)
            m_keys[i] = 0;
        m_x11C = 0;
        m_timer = 0.0f;
        m_holdTime = 0.0f;
        m_buttons = 0;
        m_direction = 0;
        return;
    }
    if (m_controller->IsKeyDown((AssignableActionIndex)0x14, 0x3F, 0))
        buttons |= 1;
    if (m_controller->IsKeyDown((AssignableActionIndex)0x15, 0x3F, 0))
        buttons |= 2;
    if (m_controller->IsKeyDown((AssignableActionIndex)0x16, 0x3F, 0))
        buttons |= 4;
    if (!buttons) {
        if (!(m_holdTime <= m_breakTimeOut)) {
            m_numKeys = 0;
            for (i = 0; i < 16; i++)
                m_keys[i] = 0;
            m_timer = 0.0f;
            m_x11C = 0;
            m_x140 = 0;
            m_holding = 0;
            m_holdTime = 0.0f;
            m_buttons = 0;
            m_direction = 0;
        } else {
            m_holdTime += dt;
            m_direction = 0;
        }
        return;
    }
    m_holdTime = 0.0f;
    m_buttons = buttons;
    char direction = JoystickDirection();
    if (direction == m_direction) {
        m_timer += dt;
    } else if (!direction) {
        m_direction = 0;
        m_timer += dt;
    } else {
        m_keys[m_numKeys++] = buttons | direction;
        m_direction = direction;
        m_timer = 0.0f;
    }
}


// ---- src/JetskiStuntsKey/CheckStuntHolding.cpp




int JetskiStuntsKey::CheckStuntHolding()
{
    char held = 0;
    if (m_holding) {
        if (m_controller->IsKeyDown((AssignableActionIndex)0x14, 0x3F, 0))
            held |= 1;
        if (m_controller->IsKeyDown((AssignableActionIndex)0x15, 0x3F, 0))
            held |= 2;
        if (m_controller->IsKeyDown((AssignableActionIndex)0x16, 0x3F, 0))
            held |= 4;
        if (!held)
            m_holding = 0;
        m_x148 = 0;
    }
    return held;
}


// ---- src/JetskiStuntsKey/CheckStuntKeyHolding.cpp




int JetskiStuntsKey::CheckStuntKeyHolding()
{
    signed char held = 0;
    if (m_controller->IsKeyDown((AssignableActionIndex)0x14, 0x3F, 0))
        held |= 1;
    if (m_controller->IsKeyDown((AssignableActionIndex)0x15, 0x3F, 0))
        held |= 2;
    if (m_controller->IsKeyDown((AssignableActionIndex)0x16, 0x3F, 0))
        held |= 4;
    return held;
}


// ---- src/JetskiStuntsKey/GetSpecificInfo.cpp
// Same as GetSpecificInfo(int, ...) for the current stunt; m_currentStunt is
// re-read for every field (the stores may alias it).
void JetskiStuntsKey::GetSpecificInfo(float &p0, float &p1, float &p2, float &p3,
                                      float &p4, float &p5, float &p6, int &type)
{
    p0 = m_stunts[m_currentStunt].m_stuntTime;
    p1 = m_stunts[m_currentStunt].m_holdStartTime;
    p2 = m_stunts[m_currentStunt].m_holdEndTime;
    p3 = m_stunts[m_currentStunt].m_safeStartTime;
    p4 = m_stunts[m_currentStunt].m_safeEndTime;
    p5 = m_stunts[m_currentStunt].m_basePoints;
    p6 = m_stunts[m_currentStunt].m_holdPoints;
    type = m_stunts[m_currentStunt].m_inAir;
}


// ---- src/JetskiStuntsKey/GetSpecificInfo_00373740.cpp
void JetskiStuntsKey::GetSpecificInfo(int i, float &p0, float &p1, float &p2, float &p3,
                                      float &p4, float &p5, float &p6, int &type)
{
    p0 = m_stunts[i].m_stuntTime;
    p1 = m_stunts[i].m_holdStartTime;
    p2 = m_stunts[i].m_holdEndTime;
    p3 = m_stunts[i].m_safeStartTime;
    p4 = m_stunts[i].m_safeEndTime;
    p5 = m_stunts[i].m_basePoints;
    p6 = m_stunts[i].m_holdPoints;
    type = m_stunts[i].m_inAir;
}


// ---- src/JetskiStuntsKey/JoystickDirection.cpp


// Returns the stick's direction bit (UP 0x10, LEFT 0x20, DOWN 0x40,
// RIGHT 0x80 as a signed byte), or 0 inside the dead zone.
int JetskiStuntsKey::JoystickDirection()
{
    float x = m_vehicle->m_stickX;
    float y = m_vehicle->m_stickY;
    if (y * y + x * x > 0.36f) {
        if (y < 0.0f) {
            if (x < y)
                return 0x20;
            return (x > -y) ? -0x80 : 0x10;
        } else {
            if (!(x <= y))
                return -0x80;
            return (x < -y) ? 0x20 : 0x40;
        }
    }
    return 0;
}


// ---- src/JetskiCheatKey/ctor.cpp
JetskiCheatKey::JetskiCheatKey()
{
    int i;
    m_directionHeld = 0;
    m_numCheats = 0;
    m_numKeys = 0;
    m_activated = 0;
    m_cheat = -1;
    for (i = 0; i < 16; i++)
        m_keys[i] = 0;
    for (i = 0; i < 10; i++) {
        m_cheats[i].m_keys[0] = 0;
        m_cheats[i].m_length = 0;
    }
}


// ---- src/JetskiCheatKey/dtor.cpp
JetskiCheatKey::~JetskiCheatKey()
{
}


// ---- src/JetskiCheatKey/Initialize.cpp


void JetskiCheatKey::Initialize(char *name)
{
    m_joystick = LpGame->m_controls->m_joysticks[0];
    strncpy(m_name, name, sizeof(m_name));
    LoadKeys();
}


// ---- src/JetskiCheatKey/LoadKeys.cpp
void PSXprintf(const char *fmt, ...);


// An inlined strrchr.
static inline char *FindLast(char *s, char c)
{
    char *start = s;
    while (*s++)
        ;
    do {
        s--;
    } while (s != start && *s != c);
    if (*s == c)
        return s;
    return 0;
}

int JetskiCheatKey::LoadKeys()
{
    char fileName[0x110];
    ParameterBlock block;
    char sequence[0x80];
    char section[0x50];

    strncpy(fileName, m_name, 0x104);
    // Retail doesn't check for a missing extension.
    strcpy(FindLast(fileName, '.'), ".bpb");
    if (!block.OpenFile(fileName, 0)) {
        PSXprintf("WARNING!!! Cannot open cheat key definition file %s - JetskiCheatKey::LoadKeys\n", fileName);
        if (!block.OpenFile(m_name, 0)) {
            PSXprintf("WARNING!!! Cannot open cheat key definition file %s - JetskiCheatKey::LoadKeys\n", m_name);
            return 0;
        }
    }
    block.ReadParameterBlock("General info");
    if (!block.GetParameter("NumberOfCheats", 0, &m_numCheats)) {
        PSXprintf("WARNING!!! Missing NumberOfCheats in %s - JetskiCheatKey::LoadKeys\n", m_name);
        return 0;
    }
    if (m_numCheats > 10) {
        PSXprintf("WARNING!!! NumberOfCheats exceeds limit (%d) in %s - JetskiCheatKey::LoadKeys\n", 10, m_name);
        m_numCheats = 10;
    }
    for (int i = 0; i < m_numCheats; i++) {
        sprintf(section, "ACTION %d", i + 1);
        block.ReadParameterBlock(section);
        if (!block.GetParameter("KeySequence", "", sequence, 0x80)) {
            PSXprintf("WARNING!!! Missing KeySequence in block %s - JetskiCheatKey::LoadKeys\n", section);
            return 0;
        }
        int length = GetKeySequence(sequence, m_cheats[i].m_keys);
        if (!length) {
            PSXprintf("WARNING!!! Key Sequence error in block %s - JetskiCheatKey::LoadKeys\n", section);
            return 0;
        }
        m_cheats[i].m_length = length;
    }
    PSXprintf("Cheat key definition file %s loaded - JetskiCheatKey::LoadKeys\n", m_name);
    return 1;
}


// ---- src/JetskiCheatKey/GetKeySequence.cpp
void PSXprintf(const char *fmt, ...);


// Parses a cheat string such as "R2;UP,DOWN:..." into key bytes:
// ';' ends a hold key, ',' a key in a sequence, ':' a separate key.
// START_C/END_C (0xF0/0xFF) bracket a "circle" group. Returns the number of
// bytes written (plus the terminator), or 0 on a malformed string.
int JetskiCheatKey::GetKeySequence(char *text, char *sequence)
{
    char buffer[128];
    char *token;
    char *p;
    char hold;
    int n;
    int circle;
    char key;

    strncpy(buffer, text, sizeof(buffer));
    buffer[sizeof(buffer) - 1] = 0;
    token = buffer;
    p = token;
    n = 0;
    hold = 0;
    circle = 0;
    for (;;) {
        while (*p != ';' && *p != ':' && *p != ',' && *p != 0)
            p++;
        switch (*p) {
        case 0:
            *p = 0;
            if (!(char)CheckHoldKey(token)) {
                int dir = (char)CheckDirectionKey(token);
                if (dir) {
                    if (circle) {
                        if (dir != -1) {
                            PSXprintf("WARNING!!! missing END_C (end of string) - JetskiCheatKey::Initialize\n");
                            return 0;
                        }
                        char last = sequence[n - 1];
                        sequence[n - 1] = 0;
                        sequence[n] = last;
                    } else {
                        sequence[n] |= dir;
                    }
                }
            } else {
                PSXprintf("WARNING!!! you can't put a HOLD key here (end of string) - JetskiCheatKey::Initialize\n");
                return 0;
            }
            n++;
            sequence[n] = 0;
            return n;
        case ';':
            *p = 0;
            key = CheckHoldKey(token);
            if (!key) {
                PSXprintf("WARNING!!! no Hold key (hold) - JetskiCheatKey::Initialize\n");
                return 0;
            }
            hold |= key;
            sequence[n] = hold;
            p++;
            token = p;
            break;
        case ',':
            *p = 0;
            key = CheckDirectionKey(token);
            if (!key) {
                PSXprintf("WARNING!!! no Direction key (sequence) - JetskiCheatKey::Initialize\n");
                return 0;
            }
            if (key == (char)0xF0) {
                sequence[n] = 0;
                n++;
                circle = 1;
            } else {
                sequence[n] = hold + key;
                n++;
                sequence[n] = hold;
            }
            p++;
            token = p;
            break;
        case ':':
            *p = 0;
            key = CheckDirectionKey(token);
            if (!key) {
                PSXprintf("WARNING!!! no Direction key (separate) - JetskiCheatKey::Initialize\n");
                return 0;
            }
            if (key == (char)0xF0) {
                PSXprintf("WARNING!!! Can't put a CIRCLE here (separate) - JetskiCheatKey::Initialize\n");
                return 0;
            }
            if (circle) {
                if (key != (char)0xFF) {
                    PSXprintf("WARNING!!! you didn't close a CIRCLE (separate) - JetskiCheatKey::Initialize\n");
                    return 0;
                }
                char last = sequence[n - 1];
                sequence[n - 1] = 0;
                sequence[n] = last;
                n++;
                circle = 0;
            } else {
                sequence[n] = hold + key;
                n++;
            }
            p++;
            hold = 0;
            token = p;
            break;
        }
    }
}


// ---- src/JetskiCheatKey/CheckHoldKey_00373F50.cpp


// The "hold" key for cheat entry is R2.
int JetskiCheatKey::CheckHoldKey(char *key)
{
    return strcasecmp(key, "R2") == 0;
}


// ---- src/JetskiCheatKey/CheckDirectionKey.cpp


// Key bits as signed chars: RIGHT, START_C and END_C come out negative.
int JetskiCheatKey::CheckDirectionKey(char *key)
{
    if (!strcasecmp(key, "UP"))
        return 0x10;
    if (!strcasecmp(key, "LEFT"))
        return 0x20;
    if (!strcasecmp(key, "DOWN"))
        return 0x40;
    if (!strcasecmp(key, "RIGHT"))
        return (char)0x80;
    if (!strcasecmp(key, "START_C"))
        return (char)0xF0;
    if (!strcasecmp(key, "END_C"))
        return (char)0xFF;
    if (!strcasecmp(key, "SQ"))
        return 2;
    return strcasecmp(key, "CI") == 0 ? 4 : 0;
}


// ---- src/JetskiCheatKey/CheckCheatActivation.cpp
// Compares the keys entered so far with each cheat code. A 0 in a code
// lets the next one or two keys be skipped.
int JetskiCheatKey::CheckCheatActivation(float)
{
    if (m_numKeys) {
        for (int i = 0; i < 10; i++) {
            int k = 0;
            int j = 0;
            int skipped = 0;
            int match = 1;
            while (j < m_cheats[i].m_length) {
                char key = m_cheats[i].m_keys[j];
                if (key == 0 && m_cheats[i].m_keys[j + 1] == m_keys[k]) {
                    j += 2;
                    skipped += 1;
                } else if (key == 0 && m_cheats[i].m_keys[j + 2] == m_keys[k]) {
                    j += 3;
                    skipped += 2;
                } else if (key == 0 && m_numKeys == k) {
                    skipped += 2;
                    break;
                } else if (key == m_keys[k]) {
                    j++;
                } else {
                    match = 0;
                    break;
                }
                k++;
            }
            if (match && m_numKeys + skipped == m_cheats[i].m_length) {
                m_activated = 1;
                m_directionHeld = 0;
                m_numKeys = 0;
                m_cheat = i;
                return i;
            }
        }
    }
    return -1;
}


// ---- src/JetskiCheatKey/CheckKeyInputs.cpp
// Retail keeps `key` (always 1 here) as a live value: constant propagation was off.
#pragma opt_propagation off




// While the hold key is down, appends each new direction/button (tagged with bit 0)
// to the key buffer; releasing it clears the buffer.
void JetskiCheatKey::CheckKeyInputs(float dt)
{
    char key = 0;
    if (m_joystick->IsKeyDown((AssignableActionIndex)0xD, 0x3F, 0)) {
        key |= 1;
    } else {
        m_directionHeld = 0;
        m_activated = 0;
        m_numKeys = 0;
        int i = 0;
        do {
            m_keys[i] = 0;
            m_keys[i + 1] = 0;
            i += 2;
        } while (i < 16);
        return;
    }
    char direction = JoystickDirection();
    if (direction)
        m_keys[m_numKeys++] = key | direction;
}



#pragma opt_propagation reset

// ---- src/JetskiCheatKey/JoystickDirection.cpp



// Returns the cheat key bit for a newly pressed direction or button, or 0.
int JetskiCheatKey::JoystickDirection()
{
    KeyInfo info;
    int pressed = 0;
    int direction;
    // The first test passes no KeyInfo (as in retail).
    if (m_joystick->IsKeyDown((AssignableActionIndex)0, 0x3F, 0)) {
        pressed = 1;
        direction = 0x20;
    } else if (m_joystick->IsKeyDown((AssignableActionIndex)1, 0x3F, &info)) {
        pressed = 1;
        direction = 0x80;
    } else if (m_joystick->IsKeyDown((AssignableActionIndex)2, 0x3F, &info)) {
        pressed = 1;
        direction = 0x10;
    } else if (m_joystick->IsKeyDown((AssignableActionIndex)3, 0x3F, &info)) {
        pressed = 1;
        direction = 0x40;
    } else if (m_joystick->IsKeyDown((AssignableActionIndex)7, 0x3F, &info)) {
        pressed = 1;
        direction = 4;
    } else if (m_joystick->IsKeyDown((AssignableActionIndex)5, 0x3F, &info)) {
        pressed = 1;
        direction = 2;
    } else if (m_joystick->IsKeyDown((AssignableActionIndex)4, 0x3F, &info)) {
        pressed = 1;
        direction = 8;
    } else if (m_joystick->IsKeyDown((AssignableActionIndex)6, 0x3F, &info)) {
        pressed = 1;
        direction = 8;
    } else if (m_joystick->IsKeyDown((AssignableActionIndex)0xB, 0x3F, &info)) {
        pressed = 1;
        direction = 8;
    } else if (m_joystick->IsKeyDown((AssignableActionIndex)0xA, 0x3F, &info)) {
        pressed = 1;
        direction = 8;
    } else if (m_joystick->IsKeyDown((AssignableActionIndex)0xC, 0x3F, &info)) {
        pressed = 1;
        direction = 8;
    } else if (m_joystick->IsKeyDown((AssignableActionIndex)0xE, 0x3F, &info)) {
        pressed = 1;
        direction = 8;
    } else if (m_joystick->IsKeyDown((AssignableActionIndex)0xF, 0x3F, &info)) {
        pressed = 1;
        direction = 8;
    }
    if (pressed && !m_directionHeld) {
        m_directionHeld = 1;
        return direction;
    }
    if (!pressed)
        m_directionHeld = 0;
    return 0;
}


// ---- src/sinit/JetskiStuntsKey.cpp
// Static initializer of JetskiStuntsKey.cpp: only the axis vectors from the shared header.
