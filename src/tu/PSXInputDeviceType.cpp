// PSXInputDeviceType.cpp  @ 0x002708F0 (text 0x2708F0-0x2729C0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp)
//
// Retail's PSXInputDeviceType.cpp as one translation unit: PSXMultiTapDevice
// and PSXJoystickDevice, unanchored inside PSXCamera.cpp's tu_map row
// (GetMultiTapStatus's ASSERT says "PSXInputDeviceType.cpp"). Its .rodata
// (0x416CC0-0x416FD0) starts at @544 after PSXInputDevice.cpp's RTTI and
// ends with its own copies of the
// InputDevice/PSXInputDevice/JoystickDevice/PSXJoystickDevice RTTI; main's
// strings follow with a label restart (@584). __sinit_PSXInputDeviceType.cpp
// (the .ctor slot after PSXCamera.cpp's) constructs only MultiTapDevice.
// `make link` places its data as well as its code.


#include "ControlInterface/ControlTypes.h"
#include "ControlInterface/ControlInterface.h"
#include "KeyInfo/KeyInfo.h"
#include "PSXButton/PSXButton.h"
#include "JoystickDevice/JoystickDevice.h"
#include "PSXJoystickDevice/PSXJoystickDevice.h"
#include "Game/Game.h"
#include "DebugOverlay/DebugOverlay.h"
#include "soultree_assert_block.h"
#include "mwcc_shim.h"

extern "C" int sceMtapGetConnection(int port);
extern "C" int scePadRead(int port, int slot, unsigned char *data);
extern "C" int scePadSetActDirect(int port, int slot, const unsigned char *data);
extern "C" int scePadGetState(int port, int slot);
extern "C" int scePadInfoMode(int port, int slot, int term, int offs);
extern "C" int scePadSetMainMode(int port, int slot, int offs, int lock);
extern "C" int scePadGetReqState(int port, int slot);
extern "C" int scePadInfoPressMode(int port, int slot);
extern "C" int scePadEnterPressMode(int port, int slot);
extern "C" int scePadInfoAct(int port, int slot, int actno, int term);
extern "C" int scePadSetActAlign(int port, int slot, const unsigned char *data);
extern "C" void *malloc(unsigned int size);
void PSXprintf(const char *fmt, ...);

extern Game *LpGame;
extern int TimerOverflowCounterT0;

// The libmtap multitap state, polled once a second (no header: only this TU
// uses it).
class PSXMultiTapDevice {
public:
    PSXMultiTapDevice();
    int GetMultiTapStatus(int id);
    void Update(float dt);

    int m_connected[2];
    float m_timer;
};

// The globals, in retail's order: .data DebugOverlayID (-1),
// LpMultiTapDevice; .bss MultiTapDevice (the static initializer constructs
// it; __sinit_PSXInputDeviceType.cpp, no axis vectors), then
// GetMultiTapStatus's assertion flag.
int DebugOverlayID = -1;
PSXMultiTapDevice MultiTapDevice;
PSXMultiTapDevice *LpMultiTapDevice = &MultiTapDevice;

#define T0_COUNT ((volatile int *)0x10000000)

// Milliseconds from EE timer 0 (BUSCLK / 256 = 585.9375 ticks per ms).
inline unsigned int timeGetTime()
{
    static long Time = 0;
    static int lastCount = 0;
    int count;
    int elapsed;
    int overflows;
    count = *T0_COUNT;
    overflows = TimerOverflowCounterT0;
    TimerOverflowCounterT0 = 0;
    if (lastCount <= count)
        elapsed = count - lastCount;
    else
        elapsed = 0x10000 - lastCount + count;
    if (lastCount >= count && overflows > 0)
        overflows--;
    lastCount = count;
    Time += elapsed + (overflows << 16);
    return 0.0017066667f * Time;
}

// The digital buttons, active high (libpad reports them active low).
static inline unsigned short ButtonBits(unsigned char *data)
{
    return (data[2] << 8 | data[3]) ^ 0xFFFF;
}

// ---- src/PSXMultiTapDevice/ctor.cpp
PSXMultiTapDevice::PSXMultiTapDevice()
{
    m_connected[0] = sceMtapGetConnection(0);
    m_connected[1] = sceMtapGetConnection(1);
    m_timer = 0;
}

// ---- src/PSXMultiTapDevice/GetMultiTapStatus.cpp

#pragma alignlabel off
int PSXMultiTapDevice::GetMultiTapStatus(int id)
{
#line 28 "PSXInputDeviceType.cpp"
    ASSERT_ONCE(id < 1 && id >= 0, "Invalid MultiTap ID");
    return m_connected[id];
}
#pragma alignlabel reset

// ---- src/PSXMultiTapDevice/Update.cpp
void PSXMultiTapDevice::Update(float dt)
{
    m_timer += dt;
    if (m_timer >= 1.0f) {
        m_connected[0] = sceMtapGetConnection(0);
        m_connected[1] = sceMtapGetConnection(1);
        m_timer = 0;
    }
}

// ---- src/PSXJoystickDevice/ctor.cpp

PSXJoystickDevice::PSXJoystickDevice(int port) : JoystickDevice(port)
{
    m_ready = 0;
    m_x6C8 = 0;
    m_actuators = 2;
    m_usingMultiTap = 0;
    m_controllerId = 4;
    m_x6F4 = 0;
    m_failedReads = 0;
}

// ---- src/PSXJoystickDevice/dtor.cpp

PSXJoystickDevice::~PSXJoystickDevice()
{
}

// ---- src/PSXJoystickDevice/Create.cpp

int PSXJoystickDevice::Create(int port)
{
    m_controller = port;
    m_padPort = 0;
    m_padSlot = 0;
    m_numButtons = 16;
    m_buttons = (PSXButton *)malloc(m_numButtons * sizeof(PSXButton));
    for (int i = 0; i < m_numButtons; i++) {
        m_buttons[i].m_flags = 0;
        m_buttons[i].m_pressureIndex = 0;
        m_buttons[i].m_mask = 0;
    }
    m_motorSmall = 0;
    m_motorLarge = 0;
    m_state = 0;
    m_id = 0;
    m_exId = 0;
    m_x6D8 = 0;
    m_x6DC = 0;
    m_commErrors = 0;
    return 1;
}

// ---- src/PSXJoystickDevice/ResetController.cpp

int PSXJoystickDevice::ResetController()
{
    m_state = 0;
    m_x6F4 = 0;
    return 1;
}

// ---- src/PSXJoystickDevice/Update.cpp
// Update__17PSXJoystickDeviceFf  @ 0x00270BF0

// A key's state, also handed out to callers as a KeyInfo.

// Reads the pad: digital and pressure buttons into the key states (queuing a
// process node per change), the analog sticks into the attached axes, then
// runs the two rumble motors' timers.
// Heap shim (not part of the retail file, image object in .mwccshim): a 0x30000 image
// with the has-call bytes of the first 128.0 and 255.0 nodes set (epoch.sh
// 0x3F8, 0x678), as Update.cpp had them; the float constants load in
// another order without it (13 words).
struct MwccHeapImage {
    char pad0[0x3ED];
    char flag0;
    char pad1[0x27F];
    char flag1;
    char tail[0x2F992];
};
#pragma section mwccshim begin
inline void MwccHeapImageShim() { const MwccHeapImage image = { {0}, 1, {0}, 1 }; }
#pragma section mwccshim end
int PSXJoystickDevice::Update(float dt)
{
    int i;
    int tries;
    // The read-failure loop's constants as function-scope locals: retail
    // loads them in this order (the unready loop above uses literals).
    float center2 = 128.0f;
    float center = 127.0f;
    float range = 255.0f;
    if (LpGame->m_debugOverlay && DebugOverlayID < 0)
        DebugOverlayID = LpGame->m_debugOverlay->m_numPages++;
    static int joySetup = -1;
    if (m_failedReads > 2) {
        tries = 0;
        while (tries++ < 50 && !IsControllerSetup())
            SetupController();
        m_failedReads = 0;
    }
    if (!m_controller)
        LpMultiTapDevice->Update(dt);
    if (!SetupController()) {
        if (m_ready) {
            for (i = 0; i < 6; i++) {
                if (i == 2)
                    UpdateAttachedAxes((AxisType)i, 128.0f, 255.0f);
                else
                    UpdateAttachedAxes((AxisType)i, 127.0f, 255.0f);
            }
            m_failedReads++;
        }
        return 0;
    }
    int wasDown[32];
    unsigned char data[32];
    for (i = 0; i < m_numButtons; i++) {
        wasDown[i] = m_keys[i].m_down;
        m_keys[i].m_down = 0;
        m_keys[i].m_value = 0;
    }
    unsigned short raw;
    if (scePadRead(m_padPort, m_padSlot, data) > 0 && data[0] == 0) {
        raw = ButtonBits(data);
    } else {
        for (i = 0; i < 6; i++) {
            if (i == 2)
                UpdateAttachedAxes((AxisType)i, center2, range);
            else
                UpdateAttachedAxes((AxisType)i, center, range);
        }
        m_failedReads++;
        return 1;
    }
    unsigned int buttons = raw;
    m_failedReads = 0;
    for (i = 0; i < 6; i++) {
        if (m_axisMap[i])
            m_axis[i] = (unsigned int)data[m_axisMap[i]];
        else
            m_axis[i] = 127.0f;
        if (i == 2)
            UpdateAttachedAxes((AxisType)i, 255.0f - m_axis[2], 255.0f);
        else
            UpdateAttachedAxes((AxisType)i, m_axis[i], 255.0f);
    }
    for (i = 0; i < m_numButtons; i++) {
        if (m_buttons[i].m_flags & 2) {
            int pressure = data[m_buttons[i].m_pressureIndex];
            if (pressure > 25) {
                m_keys[i].m_value = 0.00390625f * (pressure + 1);
                m_keys[i].m_down = 1;
            }
        } else if (buttons & m_buttons[i].m_mask) {
            m_keys[i].m_down = 1;
            m_keys[i].m_value = 1.0f;
        } else {
            m_keys[i].m_value = 0;
        }
    }
    for (i = 0; i < m_numButtons; i++) {
        if (m_keys[i].m_down != wasDown[i]) {
            if (m_keys[i].m_down == 0) {
                m_keys[i].m_x10 = 0;
                m_keys[i].m_x08 = 0;
                LpGame->m_controls->AddProcessNode(i, (DeviceType)2, (KeyState)0, m_port);
            } else {
                m_keys[i].m_x14 = 0;
                m_keys[i].m_x0C = 0;
                LpGame->m_controls->AddProcessNode(i, (DeviceType)2, (KeyState)1, m_port);
            }
        }
    }
    if (m_motorSmall > 0.0f && m_x6D8 <= 0) {
        m_actDirect[1] = 200;
        scePadSetActDirect(m_padPort, m_padSlot, m_actDirect);
        m_x6D8 = 2;
    }
    if (m_x6D8 > 0) {
        m_motorSmall -= dt;
        if (m_motorSmall <= 0.0f) {
            m_actDirect[1] = 0;
            scePadSetActDirect(m_padPort, m_padSlot, m_actDirect);
            m_x6D8--;
        }
    }
    if (m_motorLarge > 0.0f && m_x6DC <= 0) {
        m_actDirect[0] = 1;
        scePadSetActDirect(m_padPort, m_padSlot, m_actDirect);
        m_x6DC = 2;
    }
    if (m_x6DC > 0) {
        m_motorLarge -= dt;
        if (m_motorLarge <= 0.0f) {
            m_actDirect[0] = 0;
            scePadSetActDirect(m_padPort, m_padSlot, m_actDirect);
            m_x6DC--;
        }
    }
    return 1;
}

// ---- src/PSXJoystickDevice/IsControllerSetup.cpp

int PSXJoystickDevice::IsControllerSetup()
{
    return m_state == 100;
}

// ---- src/PSXJoystickDevice/PolledUpdate.cpp
// mwcc session state: retail was compiled in one long compiler run in which
// an earlier 64-bit divide raised the helper-argument high-water mark; this
// unused shim recreates it.
static void MWCC_SHIM_TEXT MwccSessionState(long *a) { a[0] = a[0] / a[1]; }

#define SAMPLE_VALUE(axis) (*(float *)&m_buttonMap[axis][m_x31C].m_x0)
#define SAMPLE_TIME(axis) (m_buttonMap[axis][m_x31C].m_x4)
// The d-pad overrides of axis 1 go through this helper: written in place,
// mwcc adds the slot offset to `this` in the other operand order.
static inline void SetSample(JoystickDevice *d, int axis, float v)
{
    *(float *)&d->m_buttonMap[axis][d->m_x31C].m_x0 = v;
}

// Maps a 0..255 stick reading to -1..1. Retail computes Normalize(127.0f) at
// run time for an unready pad: mwcc propagates the stored constant into the
// inlined body but does not fold it.
static inline float Normalize(float v) { return -1.0f + 2.0f * (v / 255.0f); }

// Retail returns without setting $v0 on every path.
int PSXJoystickDevice::PolledUpdate()
{
    unsigned int time = timeGetTime();
    int i;
    if (!m_ready) {
        for (i = 0; i < 6; i++) {
            SAMPLE_VALUE(i) = Normalize(m_axis[i] = 127.0f);
            SAMPLE_TIME(i) = time;
        }
        SetSample(this, 1, -1.0f);
        m_x31C++;
        if (m_x31C >= 16)
            m_x31C = 0;
        return;
    }
    unsigned char data[32];
    KeyInfo keys[32];
    unsigned short buttons;
    if (scePadRead(m_padPort, m_padSlot, data) > 0) {
        buttons = ButtonBits(data);
    } else {
        for (i = 0; i < 6; i++) {
            if (i == 2)
                UpdateAttachedAxes((AxisType)i, 128.0f, 255.0f);
            else
                UpdateAttachedAxes((AxisType)i, 127.0f, 255.0f);
        }
        return;
    }
    unsigned int bits = buttons;
    for (i = 0; i < 32; i++) {
        keys[i].m_down = 0;
        keys[i].m_value = 0;
    }
    for (i = 0; i < 6; i++) {
        if (m_axisMap[i])
            m_axis[i] = (unsigned int)data[m_axisMap[i]];
        else
            m_axis[i] = 127.0f;
        SAMPLE_VALUE(i) = Normalize(m_axis[i]);
        SAMPLE_TIME(i) = time;
    }
    // i is 6 here (retail tests button 6's flags).
    if (m_buttons[i].m_flags & 2) {
        if (data[m_buttons[14].m_pressureIndex])
            SetSample(this, 1, 1.0f);
        if (data[m_buttons[12].m_pressureIndex])
            SetSample(this, 1, -1.0f);
    } else {
        if (bits & m_buttons[14].m_mask)
            SetSample(this, 1, 1.0f);
        if (bits & m_buttons[12].m_mask)
            SetSample(this, 1, -1.0f);
    }
    m_x31C++;
    if (m_x31C >= 16)
        m_x31C = 0;
}

// ---- src/PSXJoystickDevice/AssignSubType.cpp

void PSXJoystickDevice::AssignSubType()
{
    switch (m_controllerId) {
    case 4:
        m_subType = 4;
        break;
    case 7:
        m_subType = 10;
        break;
    case 2:
        m_subType = 6;
        break;
    case 5:
        m_subType = 3;
        break;
    case 3:
    case 6:
        m_subType = 8;
        break;
    case 0x100:
    case 0x300:
        m_subType = 9;
        break;
    default:
        m_subType = 4;
        break;
    }
}

// ---- src/PSXJoystickDevice/GetPOVAngleInRadians.cpp

int PSXJoystickDevice::GetPOVAngleInRadians(int, float *angle)
{
    *angle = -1.0f;
    return 1;
}

// ---- src/PSXJoystickDevice/GetPOVData.cpp

// Retail writes *x twice and never touches *y (a bug in the original).
int PSXJoystickDevice::GetPOVData(int, float *x, float *y)
{
    *x = 0.0f;
    *x = 0.0f;
    return 0;
}

// ---- src/PSXJoystickDevice/EnableAutoCenter.cpp

int PSXJoystickDevice::EnableAutoCenter(int)
{
    return 0;
}

// ---- src/PSXJoystickDevice/CreateRawAxisForceEffect.cpp

int PSXJoystickDevice::CreateRawAxisForceEffect(int, long, unsigned int)
{
    return 0;
}

// ---- src/PSXJoystickDevice/SetRawAxisForceEffect.cpp

int PSXJoystickDevice::SetRawAxisForceEffect(int, unsigned int *, long)
{
    return 0;
}

// ---- src/PSXJoystickDevice/SetEffectDirection.cpp

int PSXJoystickDevice::SetEffectDirection(int, unsigned int *)
{
    return 0;
}

// ---- src/PSXJoystickDevice/CreateConstantForceEffect.cpp

int PSXJoystickDevice::CreateConstantForceEffect(int, unsigned int, unsigned int, long, unsigned int, unsigned int, unsigned int, unsigned int, long)
{
    return 0;
}

// ---- src/PSXJoystickDevice/CreatePeriodicForceEffect.cpp

int PSXJoystickDevice::CreatePeriodicForceEffect(int, unsigned int, unsigned int, long, unsigned int, unsigned int, unsigned int, unsigned int, long)
{
    return 0;
}

// ---- src/PSXJoystickDevice/StopEffect.cpp
// motor: 0 = small, 1 = large, 2 = both.
int PSXJoystickDevice::StopEffect(int motor)
{
    if (motor == 0 || motor == 2)
        m_motorSmall = 0;
    if (motor == 1 || motor == 2)
        m_motorLarge = 0;
    return 0;
}

// ---- src/PSXJoystickDevice/PlayEffect.cpp

int PSXJoystickDevice::PlayEffect(int axis, float magnitude, unsigned int)
{
    if (m_actuators != 3)
        return 0;
    if (magnitude > 5.0f)
        magnitude = 5.0f;
    if (magnitude < 0.0f)
        magnitude = 0.0f;
    if (axis == 0 || axis == 2)
        m_motorSmall = magnitude;
    if (axis == 1 || axis == 2)
        m_motorLarge = magnitude;
    return 0;
}

// ---- src/PSXJoystickDevice/IsUsingMultiTap.cpp

int PSXJoystickDevice::IsUsingMultiTap()
{
    return m_usingMultiTap;
}

// ---- src/PSXJoystickDevice/IsEffectPlaying.cpp

int PSXJoystickDevice::IsEffectPlaying(int)
{
    return 0;
}

// ---- src/PSXJoystickDevice/EnumFFEffects.cpp

int PSXJoystickDevice::EnumFFEffects(DIEFFECTINFOA *, int &)
{
    return 0;
}

// ---- src/PSXJoystickDevice/ReleaseFFEffects.cpp

void PSXJoystickDevice::ReleaseFFEffects() {}

// ---- src/PSXJoystickDevice/ResetFFDevice.cpp

int PSXJoystickDevice::ResetFFDevice()
{
    return 0;
}

// ---- src/PSXJoystickDevice/SendForceFeedbackCommand.cpp

int PSXJoystickDevice::SendForceFeedbackCommand(unsigned int)
{
    return 0;
}

// ---- src/PSXJoystickDevice/PauseFF.cpp

int PSXJoystickDevice::PauseFF(int)
{
    return 0;
}

// ---- src/PSXJoystickDevice/AssignPortAndSlot.cpp

// Works out which pad ports/pc/slot this controller reads, depending on whether a
// multitap is plugged in.
int PSXJoystickDevice::AssignPortAndSlot()
{
    if (m_controller < 4)
        m_usingMultiTap = LpMultiTapDevice->GetMultiTapStatus(0);
    else
        m_usingMultiTap = LpMultiTapDevice->GetMultiTapStatus(1);
    if (m_usingMultiTap) {
        m_padPort = 0;
        m_padSlot = m_controller;
        if (m_controller == 1) {
            if (!scePadGetState(0, 1)) {
                m_padPort = 1;
                m_padSlot = 0;
            }
            int tap = 1;
            int port1;
            int state = scePadGetState(0, tap);
            if (!state)
                tap = 0;
            port1 = 1;
            if (!scePadGetState(port1, 0))
                port1 = 0;
            if (tap && port1 && state != 6 && state != 2 && m_ready && !m_commErrors) {
                m_ready = 0;
                m_state = 0;
                m_commErrors = 1;
            }
        }
        if (m_controller > 1) {
            int tap = 1;
            if (!scePadGetState(0, tap))
                tap = 0;
            int port1 = 1;
            if (!scePadGetState(port1, 0))
                port1 = 0;
            m_padPort = 0;
            m_padSlot = m_controller;
            if (!tap && port1 == 1) {
                m_ready = 0;
                return 0;
            }
        }
    } else {
        m_padPort = 0;
        m_padSlot = m_controller;
        if (m_controller == 1) {
            m_padPort = 1;
            m_padSlot = 0;
        }
    }
    return 1;
}

// ---- src/PSXJoystickDevice/SetupController.cpp

// The libpad setup state machine (as in Sony's pad samples): identifies the
// controller, locks it in analog mode, enables pressure sensitivity and the
// actuators. Returns 1 once the controller is usable.
int PSXJoystickDevice::SetupController()
{
    if (!AssignPortAndSlot())
        return 0;
    int state = scePadGetState(m_padPort, m_padSlot);
    if (m_state == 100 && state != 1 && state != 7 && state != 0)
        return 1;
    if (state == 0) {
        m_state = 0;
        m_ready = 0;
    } else if (state == 1 || state == 7) {
        m_state = 0;
        m_commErrors = 1;
    }
    switch (m_state) {
    case 0:
        if (state == 6) {
            m_ready = 1;
            m_id = scePadInfoMode(m_padPort, m_padSlot, 1, 0);
            if (m_id) {
                m_exId = scePadInfoMode(m_padPort, m_padSlot, 2, 0);
                if (m_exId > 0)
                    m_id = m_exId;
                if (m_id == 4 || m_id == 7)
                    m_state = 40;
                else
                    m_state = 99;
            }
        } else if (state == 2) {
            m_id = 4;
            m_ready = 1;
            SetJoystickProperties(m_id);
            m_state = 100;
        }
        break;
    case 40:
        if (!scePadInfoMode(m_padPort, m_padSlot, 4, -1)) {
            m_state = 99;
            break;
        }
        m_state++;
    case 41:
        if (scePadSetMainMode(m_padPort, m_padSlot, 1, 3) == 1)
            m_state++;
        break;
    case 42:
        if (scePadGetReqState(m_padPort, m_padSlot) == 1)
            m_state--;
        if (scePadGetReqState(m_padPort, m_padSlot) == 0 && state == 6) {
            m_id = scePadInfoMode(m_padPort, m_padSlot, 1, 0);
            if (m_id) {
                m_exId = scePadInfoMode(m_padPort, m_padSlot, 2, 0);
                if (m_exId > 0)
                    m_id = m_exId;
                SetJoystickProperties(m_id);
                m_state = 70;
            }
        }
        break;
    case 70:
        if (scePadInfoPressMode(m_padPort, m_padSlot) == 1) {
            PSXprintf("Joystick device buttons are pressure sensitive.\n");
            m_state = 76;
        } else {
            PSXprintf("Joystick device buttons are not pressure sensitive.\n");
            for (int i = 0; i < m_numButtons; i++) {
                // Clears bit 1, spelled so it compiles to retail's ori/xori.
                m_buttons[i].m_flags = (m_buttons[i].m_flags | 2) ^ 2;
                m_buttons[i].m_pressureIndex = 0;
            }
            m_state = 78;
        }
        break;
    case 76:
        if (scePadEnterPressMode(m_padPort, m_padSlot) == 1)
            m_state++;
        break;
    case 77:
        if (scePadGetReqState(m_padPort, m_padSlot) == 1) {
            m_state--;
            PSXprintf("Could not switch to pressure sensitive mode.\n");
        }
        if (scePadGetReqState(m_padPort, m_padSlot) == 0)
            m_state++;
        break;
    case 78: {
        int count = scePadInfoAct(m_padPort, m_padSlot, -1, 0);
        PSXprintf("%d \"rumble\" actuators detected.\n", count);
        if (count) {
            if (count == 2) {
                m_actuators |= 1;
                m_actAlign[0] = 0;
                m_actAlign[1] = 1;
                for (int i = 2; i < 6; i++)
                    m_actAlign[i] = 0xFF;
                if (!scePadSetActAlign(m_padPort, m_padSlot, m_actAlign)) {
                    PSXprintf("scePadSetActAlign failed.\n");
                    m_actuators &= ~1;
                }
            }
        }
        m_state = 99;
        break;
    }
    case 99:
        m_ready = !(scePadGetState(m_padPort, m_padSlot) == 0);
        m_state++;
        break;
    }
    state = scePadGetState(m_padPort, m_padSlot);
    if (state == 1 || state == 7) {
        m_state = 0;
        m_commErrors = 1;
    } else {
        m_commErrors = 0;
    }
    if (m_commErrors || !m_ready)
        return 0;
    return 1;
}

// ---- src/PSXJoystickDevice/SetJoystickProperties.cpp

// Sets the button and axis tables for the controller type libpad reported.
int PSXJoystickDevice::SetJoystickProperties(int id)
{
    int i;
    int known = 1;
    m_controllerId = id;
    m_x00 = 0;
        m_axisPresent0 = 0;
        m_axisPresent1 = 0;
        m_axisPresent2 = 0;
        m_axisPresent3 = 0;
        m_axisPresent4 = 0;
        m_axisPresent5 = 0;
    m_x6C8 = 0;
    for (i = 0; i < 6; i++)
        m_axisMap[i] = 0;
    for (i = 0; i < m_numButtons; i++) {
        m_buttons[i].m_flags = 0;
        m_buttons[i].m_pressureIndex = 0;
        m_buttons[i].m_mask = 0;
    }
    switch (id) {
    case 4:
        PSXprintf("Digital joypad compatible controller detected.\n");
        m_x00 = 0;
        m_axisPresent0 = 0;
        m_axisPresent1 = 0;
        m_axisPresent2 = 0;
        m_axisPresent3 = 0;
        m_axisPresent4 = 0;
        m_axisPresent5 = 0;
        m_x6C8 = 0;
        m_buttons[0].m_mask = 16;
        m_buttons[1].m_mask = 32;
        m_buttons[2].m_mask = 64;
        m_buttons[3].m_mask = 128;
        m_buttons[4].m_mask = 1;
        m_buttons[5].m_mask = 2;
        m_buttons[7].m_mask = 8;
        m_buttons[6].m_mask = 4;
        m_buttons[8].m_mask = 256;
        m_buttons[9].m_mask = 2048;
        m_buttons[12].m_mask = 4096;
        m_buttons[13].m_mask = 8192;
        m_buttons[14].m_mask = 16384;
        m_buttons[15].m_mask = 32768;
        m_buttons[0].m_flags = 1;
        m_buttons[1].m_flags = 1;
        m_buttons[2].m_flags = 1;
        m_buttons[3].m_flags = 1;
        m_buttons[4].m_flags = 1;
        m_buttons[5].m_flags = 1;
        m_buttons[7].m_flags = 1;
        m_buttons[6].m_flags = 1;
        m_buttons[8].m_flags = 1;
        m_buttons[9].m_flags = 1;
        m_buttons[12].m_flags = 1;
        m_buttons[13].m_flags = 1;
        m_buttons[14].m_flags = 1;
        m_buttons[15].m_flags = 1;
        break;
    case 7:
        PSXprintf("DualShock compatible controller detected.\n");
        m_x00 = 4;
        m_axisPresent0 = 1;
        m_axisPresent1 = 1;
        m_axisPresent2 = 1;
        m_axisPresent3 = 0;
        m_axisPresent4 = 0;
        m_axisPresent5 = 1;
        m_x6C8 = 2;
        m_buttons[0].m_pressureIndex = 12;
        m_buttons[1].m_pressureIndex = 13;
        m_buttons[2].m_pressureIndex = 14;
        m_buttons[3].m_pressureIndex = 15;
        m_buttons[4].m_pressureIndex = 18;
        m_buttons[5].m_pressureIndex = 19;
        m_buttons[6].m_pressureIndex = 16;
        m_buttons[7].m_pressureIndex = 17;
        m_buttons[12].m_pressureIndex = 10;
        m_buttons[13].m_pressureIndex = 8;
        m_buttons[14].m_pressureIndex = 11;
        m_buttons[15].m_pressureIndex = 9;
        m_buttons[0].m_mask = 16;
        m_buttons[1].m_mask = 32;
        m_buttons[2].m_mask = 64;
        m_buttons[3].m_mask = 128;
        m_buttons[4].m_mask = 1;
        m_buttons[5].m_mask = 2;
        m_buttons[7].m_mask = 8;
        m_buttons[6].m_mask = 4;
        m_buttons[8].m_mask = 256;
        m_buttons[9].m_mask = 2048;
        m_buttons[10].m_mask = 1024;
        m_buttons[11].m_mask = 512;
        m_buttons[12].m_mask = 4096;
        m_buttons[13].m_mask = 8192;
        m_buttons[14].m_mask = 16384;
        m_buttons[15].m_mask = 32768;
        m_buttons[0].m_flags = 3;
        m_buttons[1].m_flags = 3;
        m_buttons[2].m_flags = 3;
        m_buttons[3].m_flags = 3;
        m_buttons[4].m_flags = 3;
        m_buttons[5].m_flags = 3;
        m_buttons[7].m_flags = 3;
        m_buttons[6].m_flags = 3;
        m_buttons[8].m_flags = 1;
        m_buttons[9].m_flags = 1;
        m_buttons[10].m_flags = 1;
        m_buttons[11].m_flags = 1;
        m_buttons[12].m_flags = 3;
        m_buttons[13].m_flags = 3;
        m_buttons[14].m_flags = 3;
        m_buttons[15].m_flags = 3;
        m_axisMap[0] = 6;
        m_axisMap[1] = 7;
        m_axisMap[2] = 4;
        m_axisMap[5] = 5;
        break;
    case 2:
        PSXprintf("NeGcon compatible controller detected.\n");
        m_x00 = 1;
        m_axisPresent0 = 1;
        m_axisPresent1 = 0;
        m_axisPresent2 = 0;
        m_axisPresent3 = 0;
        m_axisPresent4 = 0;
        m_axisPresent5 = 0;
        m_x6C8 = 1;
        m_buttons[3].m_pressureIndex = 6;
        m_buttons[2].m_pressureIndex = 5;
        m_buttons[6].m_pressureIndex = 7;
        m_buttons[0].m_mask = 16;
        m_buttons[1].m_mask = 32;
        m_buttons[7].m_mask = 8;
        m_buttons[9].m_mask = 2048;
        m_buttons[12].m_mask = 4096;
        m_buttons[13].m_mask = 8192;
        m_buttons[14].m_mask = 16384;
        m_buttons[15].m_mask = 32768;
        m_buttons[0].m_flags = 1;
        m_buttons[1].m_flags = 1;
        m_buttons[2].m_flags = 3;
        m_buttons[3].m_flags = 3;
        m_buttons[7].m_flags = 1;
        m_buttons[6].m_flags = 3;
        m_buttons[9].m_flags = 1;
        m_buttons[12].m_flags = 1;
        m_buttons[13].m_flags = 1;
        m_buttons[14].m_flags = 1;
        m_buttons[15].m_flags = 1;
        m_axisMap[0] = 4;
        break;
    case 5:
        PSXprintf("Analog joystick or dual flightstick compatible controller detected.\n");
        m_x00 = 4;
        m_axisPresent0 = 1;
        m_axisPresent1 = 1;
        m_axisPresent2 = 1;
        m_axisPresent3 = 0;
        m_axisPresent4 = 0;
        m_axisPresent5 = 1;
        m_x6C8 = 0;
        m_buttons[0].m_mask = 16;
        m_buttons[1].m_mask = 32;
        m_buttons[2].m_mask = 64;
        m_buttons[3].m_mask = 128;
        m_buttons[4].m_mask = 1;
        m_buttons[5].m_mask = 2;
        m_buttons[7].m_mask = 8;
        m_buttons[6].m_mask = 4;
        m_buttons[8].m_mask = 256;
        m_buttons[9].m_mask = 2048;
        m_buttons[12].m_mask = 4096;
        m_buttons[13].m_mask = 8192;
        m_buttons[14].m_mask = 16384;
        m_buttons[15].m_mask = 32768;
        m_buttons[0].m_flags = 1;
        m_buttons[1].m_flags = 1;
        m_buttons[2].m_flags = 1;
        m_buttons[3].m_flags = 1;
        m_buttons[4].m_flags = 1;
        m_buttons[5].m_flags = 1;
        m_buttons[7].m_flags = 1;
        m_buttons[6].m_flags = 1;
        m_buttons[8].m_flags = 1;
        m_buttons[9].m_flags = 1;
        m_buttons[12].m_flags = 1;
        m_buttons[13].m_flags = 1;
        m_buttons[14].m_flags = 1;
        m_buttons[15].m_flags = 1;
        m_axisMap[0] = 6;
        m_axisMap[1] = 7;
        m_axisMap[2] = 4;
        m_axisMap[5] = 5;
        break;
    case 3:
    case 6:
    case 0x100:
    case 0x300:
    default:
        PSXprintf("Unsupported controller detected  Id %d.\n", id);
        known = 0;
        break;
    }
    return known;
}

// ---- src/PSXJoystickDevice/IsControllerPluggedIn.cpp

int PSXJoystickDevice::IsControllerPluggedIn()
{
    return m_ready;
}

// ---- src/PSXJoystickDevice/CommErrors.cpp

int PSXJoystickDevice::CommErrors()
{
    return m_commErrors;
}


// Retail's RTTI block has InputDevice's and PSXInputDevice's records before
// PSXJoystickDevice's name (@1315-@1317, then @1320): mwcc built them as
// the bases of a class whose vtable it wrote first, pushed last on its LIFO
// list (key function defined after PSXJoystickDevice's). Nothing constructed that
// class, so retail's linker stripped its vtable, RTTI and functions; link_c
// leaves them out ("Section
// order").
class StrippedPSXInputDevice : public PSXInputDevice {
public:
    virtual ~StrippedPSXInputDevice();
};

StrippedPSXInputDevice::~StrippedPSXInputDevice()
{
}
