// _RECT: the Win32 RECT the engine keeps on the PS2 (0x10 bytes).
#ifndef _RECT__RECT_H
#define _RECT__RECT_H

struct _RECT {
    int left;                           // +0x00
    int top;                            // +0x04
    int right;                          // +0x08
    int bottom;                         // +0x0C
};

#endif
