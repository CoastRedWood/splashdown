// KeyInfo: the state of one key or button as the input devices report it
// (IsKeyDown fills one in), 0x18 bytes.
#ifndef KEYINFO_KEYINFO_H
#define KEYINFO_KEYINFO_H

class KeyInfo {
public:
    int m_down;                         // +0x00
    float m_value;                      // +0x04
    int m_x08;                          // +0x08
    int m_x0C;                          // +0x0C
    int m_x10;                          // +0x10
    int m_x14;                          // +0x14
};

#endif
