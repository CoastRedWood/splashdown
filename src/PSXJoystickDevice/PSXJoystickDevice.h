// PSXJoystickDevice: the PS2 game pad (libpad), 0x720 bytes (`new`). Its data
// starts after JoystickDevice's at +0x6B8. Names follow its own methods
// (src/PSXJoystickDevice/), otherwise the most common local name.
#ifndef PSXJOYSTICKDEVICE_PSXJOYSTICKDEVICE_H
#define PSXJOYSTICKDEVICE_PSXJOYSTICKDEVICE_H

#include "JoystickDevice/JoystickDevice.h"

class PSXJoystickDevice : public JoystickDevice {
public:
    PSXJoystickDevice(int port);
    int Create(int port);
    int AssignPortAndSlot();
    int SetJoystickProperties(int id);  // whether libpad's type is known

    // --- virtuals in retail slot order ---
    virtual ~PSXJoystickDevice();
    virtual int GetPOVAngleInRadians(int pov, float *angle);
    virtual int GetPOVData(int pov, float *x, float *y);
    virtual int EnableAutoCenter(int enable);
    virtual int CreateRawAxisForceEffect(int, long, unsigned int);
    virtual int SetRawAxisForceEffect(int, unsigned int *, long);
    virtual int SetEffectDirection(int, unsigned int *);
    virtual int CreateConstantForceEffect(int, unsigned int, unsigned int, long, unsigned int, unsigned int,
                                          unsigned int, unsigned int, long);
    virtual int CreatePeriodicForceEffect(int, unsigned int, unsigned int, long, unsigned int, unsigned int,
                                          unsigned int, unsigned int, long);
    virtual int StopEffect(int motor);
    virtual int PlayEffect(int axis, float magnitude, unsigned int);
    virtual int IsEffectPlaying(int);
    virtual int EnumFFEffects(DIEFFECTINFOA *, int &);
    virtual void ReleaseFFEffects();
    virtual int ResetFFDevice();
    virtual int SendForceFeedbackCommand(unsigned int);
    virtual int PauseFF(int);
    virtual int IsControllerPluggedIn();
    virtual int IsControllerSetup();
    virtual int SetupController();
    virtual int CommErrors();
    virtual int ResetController();
    virtual int PolledUpdate();
    virtual int IsUsingMultiTap();
    virtual void AssignSubType();
    virtual int Update(float dt);

    int m_controller;                   // +0x6B8, pad number (Create's port)
    int m_padPort;                      // +0x6BC
    int m_padSlot;                      // +0x6C0
    int m_ready;                        // +0x6C4
    int m_x6C8;                         // +0x6C8
    float m_motorSmall;                 // +0x6CC, PlayEffect's magnitude, Update counts it
    float m_motorLarge;                 // +0x6D0  down by dt (its m_*RumbleTime)
    int m_commErrors;                   // +0x6D4
    int m_x6D8;                         // +0x6D8, paired with m_motorSmall in Update
    int m_x6DC;                         // +0x6DC, paired with m_motorLarge in Update
    int m_usingMultiTap;                // +0x6E0
    int m_state;                        // +0x6E4, controller setup state
    int m_id;                           // +0x6E8, scePadInfoMode ID
    int m_exId;                         // +0x6EC, extended ID
    int m_controllerId;                 // +0x6F0, 4 from the ctor
    int m_x6F4;                         // +0x6F4
    int m_failedReads;                  // +0x6F8
    int m_axisMap[6];                   // +0x6FC
    unsigned char m_actDirect[6];       // +0x714
    unsigned char m_actAlign[6];        // +0x71A
};

#endif
