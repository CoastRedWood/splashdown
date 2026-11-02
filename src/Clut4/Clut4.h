// Clut4 / Clut: 16-entry colour lookup table. Clut (the base, 0xC bytes)
// lives here too until src/Clut has a header of its own.
#ifndef CLUT4_H
#define CLUT4_H

class Clut {
public:
    Clut();

    // --- virtuals in retail slot order ---
    virtual ~Clut();            // vptr +0x0

    int m_numColors;            // +0x4
    unsigned int *m_colors;     // +0x8 (malloc'd, m_numColors words)
};

class Clut4 : public Clut {
public:
    Clut4();
    int Read(long handle);

    // --- virtuals in retail slot order ---
    virtual ~Clut4();
};

#endif
