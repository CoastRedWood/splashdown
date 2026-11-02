// JetskiStuntsKey: a rider's stunt key sequences (LoadKeys reads them from
// the rider's parameter file) and the key-entry state that matches them.
// Not a GameObject: all data first, then the vptr at +0xC50; 0xC54 bytes.
// Names come from the class's own methods.
#ifndef JETSKISTUNTSKEY_JETSKISTUNTSKEY_H
#define JETSKISTUNTSKEY_JETSKISTUNTSKEY_H

class JetskiVehicle;
class JoystickDevice;
class Motion;

#define NUM_STUNT_MOTIONS 41

// One stunt's definition, 0x44 bytes (LoadKeys).
struct StuntKeys {
    char m_keys[0x20];                  // +0x00, 0 = optional key follows
    int m_numKeys;                      // +0x20
    int m_inAir;                        // +0x24 (CheckStuntActivation: the stunt type)
    float m_stuntTime;                  // +0x28
    float m_holdStartTime;              // +0x2C
    float m_holdEndTime;                // +0x30
    float m_safeStartTime;              // +0x34
    float m_safeEndTime;                // +0x38
    float m_basePoints;                 // +0x3C
    float m_holdPoints;                 // +0x40
};

class JetskiStuntsKey {
public:
    JetskiStuntsKey();
    void Initialize(char *name, JetskiVehicle *vehicle, int player, Motion **motions);
    int LoadKeys();
    int GetKeySequence(char *text, char *keys);
    void CheckKeyInputs(float dt, int player);
    int CheckStuntActivation(float dt, int type);
    int CheckStuntHolding();
    int CheckStuntKeyHolding();
    int CheckHoldKey(char *key);
    int CheckDirectionKey(char *key);
    int JoystickDirection();
    void GetSpecificInfo(float &p0, float &p1, float &p2, float &p3,
                         float &p4, float &p5, float &p6, int &type);
    void GetSpecificInfo(int i, float &p0, float &p1, float &p2, float &p3,
                         float &p4, float &p5, float &p6, int &type);

    JetskiVehicle *m_vehicle;           // +0x000
    JoystickDevice *m_controller;       // +0x004
    char m_name[0x104];                 // +0x008
    Motion **m_motions;                 // +0x10C
    int m_numStunts;                    // +0x110
    float m_acquisitionTimeOut;         // +0x114
    float m_acquisitionTime;            // +0x118
    float m_x11C;                       // +0x11C, time since the last key
    float m_breakTimeOut;               // +0x120
    float m_holdTime;                   // +0x124
    float m_pointsPerSecond;            // +0x128
    float m_multiStuntBonus[5];         // +0x12C
    int m_x140;                         // +0x140
    int m_holding;                      // +0x144
    int m_x148;                         // +0x148
    int m_currentStunt;                 // +0x14C
    StuntKeys m_stunts[NUM_STUNT_MOTIONS];  // +0x150
    char m_keys[16];                    // +0xC34, keys entered so far
    int m_numKeys;                      // +0xC44
    float m_timer;                      // +0xC48
    char m_buttons;                     // +0xC4C
    char m_direction;                   // +0xC4D

    virtual ~JetskiStuntsKey();         // vptr +0xC50
};

#endif
