// The JetskiMP* memory-card pages of JetskiMemPages.cpp (0x38B6D0-0x38F120),
// one class each, sized to retail's `new` (JetskiUIManMain::GetPage,
// JetskiUIManCard::GetPage). See JetskiUIPages.h for the page-class header
// convention. Virtuals are the overrides the retail vtable lists for each class.
#ifndef JETSKIUIPAGE_JETSKIMEMPAGES_H
#define JETSKIUIPAGE_JETSKIMEMPAGES_H

#include "JetskiUIPage/JetskiUIPage.h"

class JetskiMPLoadEmptyOpt : public JetskiUIPage {
public:
    JetskiMPLoadEmptyOpt(int enabled);

    virtual ~JetskiMPLoadEmptyOpt();
    virtual int Tick(float dt);
    virtual void ResetPage();

    int m_checked;                          // +0x168
    char pad16C[0x170 - 0x16C];
};

class JetskiMPLoadYes : public JetskiUIPage {
public:
    JetskiMPLoadYes(int enabled);

    virtual ~JetskiMPLoadYes();
    virtual int Tick(float dt);
    virtual void ResetPage();
    virtual int ProcessUserInput(int player, int input);

    int m_loaded;                           // +0x168
    int m_failed;                           // +0x16C
    int m_loading;                          // +0x170
    int m_busy;                             // +0x174
    int m_cancelled;                        // +0x178
    int m_x17C;                             // +0x17C
    int m_x180;                             // +0x180
    int m_cardCheckTimer;                   // +0x184
    int m_loadDelay;                        // +0x188
    char pad18C[0x190 - 0x18C];
};

class JetskiMPLoadNo : public JetskiUIPage {
public:
    JetskiMPLoadNo(int enabled);

    virtual ~JetskiMPLoadNo();
    virtual int Tick(float dt);
    virtual void ResetPage();
    virtual int ProcessUserInput(int player, int input);

    float m_cardCheckTimer;                 // +0x168
    char pad16C[0x170 - 0x16C];
};

class JetskiMPLoadNoGame : public JetskiUIPage {
public:
    JetskiMPLoadNoGame(int enabled);

    virtual ~JetskiMPLoadNoGame();
    virtual int Tick(float dt);
    virtual void ResetPage();
    virtual int ProcessUserInput(int player, int input);

    float m_pollTimer;                      // +0x168
    char pad16C[0x170 - 0x16C];
};

class JetskiMPLoadNoFormat : public JetskiUIPage {
public:
    JetskiMPLoadNoFormat(int enabled);

    virtual ~JetskiMPLoadNoFormat();
    virtual int Tick(float dt);
    virtual void ResetPage();
    virtual int ProcessUserInput(int player, int input);

    float m_timer;                          // +0x168
    char pad16C[0x170 - 0x16C];
};

class JetskiMPSaveEmptyOpt : public JetskiUIPage {
public:
    JetskiMPSaveEmptyOpt(int enabled);

    virtual ~JetskiMPSaveEmptyOpt();
    virtual int Tick(float dt);
    virtual void ResetPage();

    int m_checked;                          // +0x168
    char pad16C[0x170 - 0x16C];
};

class JetskiMPSaveYes : public JetskiUIPage {
public:
    JetskiMPSaveYes(int enabled);
    void MoveToEmpty();
    void Done();

    virtual ~JetskiMPSaveYes();
    virtual int Tick(float dt);
    virtual void ResetPage();
    virtual int ProcessUserInput(int player, int input);

    int m_saved;                            // +0x168
    int m_failed;                           // +0x16C
    int m_saving;                           // +0x170
    int m_busy;                             // +0x174
    int m_cardCheckTimer;                   // +0x178
    int m_saveDelay;                        // +0x17C
    char pad180[0x188 - 0x180];
};

class JetskiMPSaveNo : public JetskiUIPage {
public:
    JetskiMPSaveNo(int enabled);
    void MoveToEmpty();

    virtual ~JetskiMPSaveNo();
    virtual int Tick(float dt);
    virtual void ResetPage();
    virtual int ProcessUserInput(int player, int input);

    float m_checkTimer;                     // +0x168
    char pad16C[0x170 - 0x16C];
};

class JetskiMPSaveNoSpace : public JetskiUIPage {
public:
    JetskiMPSaveNoSpace(int enabled);
    void MoveToEmpty();

    virtual ~JetskiMPSaveNoSpace();
    virtual int Tick(float dt);
    virtual void ResetPage();
    virtual int ProcessUserInput(int player, int input);

    float m_checkTimer;                     // +0x168
    char pad16C[0x170 - 0x16C];
};

class JetskiMPSaveFormat : public JetskiUIPage {
public:
    JetskiMPSaveFormat(int enabled);
    void MoveToEmpty();
    void Done();

    virtual ~JetskiMPSaveFormat();
    virtual int Tick(float dt);
    virtual void ResetPage();
    virtual int ProcessUserInput(int player, int input);

    int m_formatFailed;                     // +0x168
    int m_formatted;                        // +0x16C
    int m_save;                             // +0x170
    int m_saved;                            // +0x174
    int m_saveFailed;                       // +0x178
    int m_startFormat;                      // +0x17C
    int m_x180;                             // +0x180
    char pad184[0x18C - 0x184];
    int m_checkCount;                       // +0x18C
    int m_delay;                            // +0x190
    char pad194[0x198 - 0x194];
};

class JetskiMPSaveEmptyChamp : public JetskiUIPage {
public:
    JetskiMPSaveEmptyChamp(int enabled);

    virtual ~JetskiMPSaveEmptyChamp();
    virtual int Tick(float dt);
    virtual void ResetPage();

    int m_checked;                          // +0x168
    char pad16C[0x170 - 0x16C];
};

class JetskiMPInitNo : public JetskiUIPage {
public:
    JetskiMPInitNo(int enabled);

    virtual ~JetskiMPInitNo();
    virtual int Tick(float dt);
    virtual void ResetPage();
    virtual int ProcessUserInput(int player, int input);

    float m_timer;                          // +0x168
    char pad16C[0x170 - 0x16C];
};

class JetskiMPInitNoSpace : public JetskiUIPage {
public:
    JetskiMPInitNoSpace(int enabled);

    virtual ~JetskiMPInitNoSpace();
    virtual int Tick(float dt);
    virtual void ResetPage();
    virtual int ProcessUserInput(int player, int input);

    float m_timer;                          // +0x168
    char pad16C[0x170 - 0x16C];
};

class JetskiMPInitYes : public JetskiUIPage {
public:
    JetskiMPInitYes(int enabled);

    virtual ~JetskiMPInitYes();
    virtual int Tick(float dt);
    virtual void ResetPage();
    virtual int ProcessUserInput(int player, int input);

    int m_loaded;                           // +0x168
    int m_loadFailed;                       // +0x16C
    int m_startLoad;                        // +0x170
    int m_x174;                             // +0x174
    int m_cancel;                           // +0x178
    int m_x17C;                             // +0x17C
    int m_x180;                             // +0x180
    float m_checkTimer;                     // +0x184
    int m_loadDelay;                        // +0x188
    char pad18C[0x190 - 0x18C];
};

class JetskiMPInitFormat : public JetskiUIPage {
public:
    JetskiMPInitFormat(int enabled);
    void Done();

    virtual ~JetskiMPInitFormat();
    virtual int Tick(float dt);
    virtual void ResetPage();
    virtual int ProcessUserInput(int player, int input);

    int m_formatted;                        // +0x168
    int m_formatFailed;                     // +0x16C
    int m_startFormat;                      // +0x170
    int m_x174;                             // +0x174
    int m_x178;                             // +0x178
    int m_x17C;                             // +0x17C
    float m_checkTimer;                     // +0x180
    int m_formatDelay;                      // +0x184
};

class JetskiMPInitEmpty : public JetskiUIPage {
public:
    JetskiMPInitEmpty(int enabled);

    virtual ~JetskiMPInitEmpty();
    virtual int Tick(float dt);
    virtual int RenderPre3D();
    virtual void ResetPage();

    int m_checked;                          // +0x168
    int m_x16C;                             // +0x16C
    float m_delay;                          // +0x170
    char pad174[0x178 - 0x174];
};

#endif
