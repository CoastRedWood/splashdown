// JoystickDevice: a game pad (PSXJoystickDevice is the PS2 one). The chain is
// InputDevice -> PSXInputDevice -> JoystickDevice -> PSXJoystickDevice; the
// first two have no header of their own yet, so they are declared here.
// InputDevice's data comes first, so the vptr sits after it at +0x18.
// JoystickDevice is 0x6B8 bytes; PSXJoystickDevice (0x720, `new`) adds from
// +0x6B8. Slots 6-30 are pure in JoystickDevice's vtable; their signatures
// are PSXJoystickDevice's overrides (retail vtable).
#ifndef JOYSTICKDEVICE_JOYSTICKDEVICE_H
#define JOYSTICKDEVICE_JOYSTICKDEVICE_H

#include "ContainerList/ContainerList.h"
#include "ControlInterface/ControlTypes.h"
#include "KeyInfo/KeyInfo.h"

class AnalogAxis;
class KeyInfo;
struct PSXButton;
struct DIEFFECTINFOA;

class InputDevice {
public:
    InputDevice(DeviceType type);

    int m_x00;                          // +0x00
    int m_numButtons;                   // +0x04, entries of m_buttons
    DeviceType m_type;                  // +0x08
    int m_subType;                      // +0x0C, -1 from InputDevice's ctor
    unsigned char m_axisPresent0 : 1;   // +0x10 bit 0
    unsigned char m_axisPresent1 : 1;   //        bit 1
    unsigned char m_axisPresent2 : 1;   //        bit 2
    unsigned char m_axisPresent3 : 1;   //        bit 3
    unsigned char m_axisPresent4 : 1;   //        bit 4
    unsigned char m_axisPresent5 : 1;   //        bit 5
    PSXButton *m_buttons;               // +0x14, per-key table (bit 1 of m_flags: analog)

    // vptr at +0x18. Slot 0 is pure here; JoystickDevice's RemoveAxis fills it.
    virtual void RemoveAxis(int id) = 0;
    virtual ~InputDevice();
};

class PSXInputDevice : public InputDevice {
public:
    PSXInputDevice(DeviceType type);
    virtual ~PSXInputDevice();
};

typedef ContainerList<AnalogAxis *> AnalogAxisList;

// One button binding per (axis, key) slot.
struct JoystickButton {
    int m_x0;
    int m_x4;
};

class JoystickDevice : public PSXInputDevice {
public:
    JoystickDevice(int port);
    int AttachAxis(AnalogAxis *axis, float a, float b);
    void UpdateAttachedAxes(AxisType type, float a, float b);

    // --- virtuals in retail slot order ---
    virtual void RemoveAxis(int id);
    virtual ~JoystickDevice();
    virtual int IsKeyDown(unsigned int key, unsigned int mods, KeyInfo *info);
    virtual int IsKeyDown(AssignableActionIndex action, unsigned int mods, KeyInfo *info);
    virtual int IsKeyAnalog(unsigned int key);
    virtual int IsKeyAnalog(AssignableActionIndex action);
    virtual int GetPOVAngleInRadians(int pov, float *angle) = 0;
    virtual int GetPOVData(int pov, float *x, float *y) = 0;
    virtual int EnableAutoCenter(int enable) = 0;
    virtual int CreateRawAxisForceEffect(int, long, unsigned int) = 0;
    virtual int SetRawAxisForceEffect(int, unsigned int *, long) = 0;
    virtual int SetEffectDirection(int, unsigned int *) = 0;
    virtual int CreateConstantForceEffect(int, unsigned int, unsigned int, long, unsigned int, unsigned int,
                                          unsigned int, unsigned int, long) = 0;
    virtual int CreatePeriodicForceEffect(int, unsigned int, unsigned int, long, unsigned int, unsigned int,
                                          unsigned int, unsigned int, long) = 0;
    virtual int StopEffect(int motor) = 0;
    virtual int PlayEffect(int axis, float magnitude, unsigned int) = 0;
    virtual int IsEffectPlaying(int) = 0;
    virtual int EnumFFEffects(DIEFFECTINFOA *, int &) = 0;
    virtual void ReleaseFFEffects() = 0;
    virtual int ResetFFDevice() = 0;
    virtual int SendForceFeedbackCommand(unsigned int) = 0;
    virtual int PauseFF(int) = 0;
    virtual int IsControllerPluggedIn() = 0;
    virtual int IsControllerSetup() = 0;
    virtual int SetupController() = 0;
    virtual int CommErrors() = 0;
    virtual int ResetController() = 0;
    virtual int PolledUpdate() = 0;
    virtual int IsUsingMultiTap() = 0;
    virtual void AssignSubType() = 0;
    virtual int Update(float dt) = 0;

    JoystickButton m_buttonMap[6][16];      // +0x01C
    int m_x31C;                             // +0x31C
    int m_port;                             // +0x320
    KeyInfo m_keys[32];                     // +0x324
    float m_axis[6];                        // +0x624
    AnalogAxisList m_axes[6];               // +0x63C, axes attached per axis type
    unsigned char m_directionFlipped : 1;   // +0x6B4 bit 0, registry "JoyDirectionFlipped"
    unsigned char m_actuators : 2;          //        bits 1-2 (PSXJoystickDevice: 2 from its
                                            //        ctor, SetupController sets bit 0)
};

#endif
