// libpad and libmtap on the PC: every connected pad is a DualShock 2 that
// is already stable, in analog mode, with pressure-sensitive buttons and two
// actuators, so PSXJoystickDevice::SetupController's state machine goes
// 0 -> 40 -> 41 -> 42 -> 70 -> 76 -> 77 -> 78 -> 99 -> 100 in as many frames.
// The pads come from ports/pc/input.cpp: pad 0 on port 0; more pads (gamepads)
// on port 1, and with more than two a multitap on port 0 (slots 0-3).
#include "port.h"

#include <cstring>

namespace {

// The pad behind (port, slot), or -1
int PadAt(int port, int slot)
{
    int pads = port_input_pads();
    int pad = -1;
    if (pads > 2)  // multitap in port 0
        pad = port == 0 ? slot : (port == 1 && slot == 0 ? 4 : -1);
    else
        pad = slot == 0 && (port == 0 || port == 1) ? port : -1;
    return pad >= 0 && pad < pads ? pad : -1;
}

} // namespace

extern "C" {

int scePadInit(int mode)
{
    return 1;
}

int scePadPortOpen(int port, int slot, unsigned long *addr)
{
    return 1;
}

// scePadStateStable (6), or scePadStateDiscon (0)
int scePadGetState(int port, int slot)
{
    return PadAt(port, slot) >= 0 ? 6 : 0;
}

// scePadReqStateComplete: every request finishes at once
int scePadGetReqState(int port, int slot)
{
    return 0;
}

// The DualShock 2 report, 32 bytes: status 0, id 0x79 (analog, pressure),
// buttons active low, right stick, left stick, 12 pressures.
int scePadRead(int port, int slot, unsigned char *data)
{
    memset(data, 0, 32);
    int pad = PadAt(port, slot);
    if (pad < 0) {
        data[0] = 0xFF;
        return 0;
    }
    PortPad p;
    port_input_read(pad, &p);
    uint16_t raw = (uint16_t)~p.buttons;
    data[1] = 0x79;
    data[2] = raw >> 8;
    data[3] = raw & 0xFF;
    data[4] = p.rx;
    data[5] = p.ry;
    data[6] = p.lx;
    data[7] = p.ly;
    memcpy(data + 8, p.pressure, 12);
    return 32;
}

// term 1 PAD_MODECURID, 2 PAD_MODECUREXID: 7 (DualShock); 3
// PAD_MODECUROFFS: 1 (analog); 4 PAD_MODETABLE: with offs -1 the number of
// modes (2), else mode offs's id (digital 4, analog 7)
int scePadInfoMode(int port, int slot, int term, int offs)
{
    if (PadAt(port, slot) < 0)
        return 0;
    switch (term) {
    case 1:
    case 2:
        return 7;
    case 3:
        return 1;
    case 4:
        return offs == -1 ? 2 : offs == 0 ? 4 : offs == 1 ? 7 : 0;
    }
    return 0;
}

int scePadSetMainMode(int port, int slot, int offs, int lock)
{
    return PadAt(port, slot) >= 0;
}

// actno -1, term 0: the number of actuators (2)
int scePadInfoAct(int port, int slot, int actno, int term)
{
    if (PadAt(port, slot) < 0)
        return 0;
    if (actno < 0)
        return 2;
    return 0;
}

int scePadSetActAlign(int port, int slot, const unsigned char *data)
{
    return PadAt(port, slot) >= 0;
}

// With scePadSetActAlign's {0, 1}: data[0] drives actuator 0, the small
// motor (on/off), data[1] actuator 1, the large motor (speed 0-255)
int scePadSetActDirect(int port, int slot, const unsigned char *data)
{
    int pad = PadAt(port, slot);
    if (pad < 0)
        return 0;
    port_input_rumble(pad, data[0] ? 0xFF : 0, data[1]);
    return 1;
}

int scePadInfoPressMode(int port, int slot)
{
    return PadAt(port, slot) >= 0;
}

int scePadEnterPressMode(int port, int slot)
{
    return PadAt(port, slot) >= 0;
}

// libmtap: a multitap in port 0 once more than two pads are connected
int sceMtapInit(void)
{
    return 1;
}

int sceMtapPortOpen(int port)
{
    return 1;
}

int sceMtapGetConnection(int port)
{
    return port == 0 && port_input_pads() > 2;
}

} // extern "C"
