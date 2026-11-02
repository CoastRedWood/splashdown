// Clut8: 256-entry colour lookup table.
#ifndef CLUT8_H
#define CLUT8_H

#include "Clut4/Clut4.h"        // the Clut base

class Clut8 : public Clut {
public:
    Clut8();
    int Read(long file);

    // --- virtuals in retail slot order ---
    virtual ~Clut8();
};

#endif
