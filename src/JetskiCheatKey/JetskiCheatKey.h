// JetskiCheatKey: the cheat key sequences (LoadKeys reads them from a
// parameter file) and the key-entry state that matches them. Not a
// GameObject: all data first, then the vptr at +0x294; 0x298 bytes. Names
// come from the class's own methods.
#ifndef JETSKICHEATKEY_JETSKICHEATKEY_H
#define JETSKICHEATKEY_JETSKICHEATKEY_H

class JoystickDevice;

// One cheat's key sequence, 0x24 bytes.
struct CheatKey {
    char m_keys[0x20];                  // +0x00, 0 marks an optional key
    int m_length;                       // +0x20
};

class JetskiCheatKey {
public:
    JetskiCheatKey();
    void Initialize(char *name);
    int LoadKeys();
    int GetKeySequence(char *text, char *sequence);
    int CheckHoldKey(char *key);
    int CheckDirectionKey(char *key);
    int CheckCheatActivation(float dt);
    void CheckKeyInputs(float dt);
    int JoystickDirection();

    JoystickDevice *m_joystick;         // +0x000 (Initialize: LpGame->m_system's +0x10)
    char m_name[0x104];                 // +0x004
    int m_numCheats;                    // +0x108
    int m_activated;                    // +0x10C
    int m_cheat;                        // +0x110, -1 = none
    int m_directionHeld;                // +0x114
    CheatKey m_cheats[10];              // +0x118
    char m_keys[16];                    // +0x280, keys entered so far
    int m_numKeys;                      // +0x290

    virtual ~JetskiCheatKey();          // vptr +0x294
};

#endif
