// JetskiVCRController: Splashdown's race recorder/ghost player (a
// SimpleVCR). 0xE0 bytes (`new` in JetskiCompetition); its own data starts
// at SimpleVCR's 0xD0. +0xD4 is set by LoadFromDisk/LoadGhostFromDisk (other
// copies call it m_playing/m_replaying); +0xD8 is 1 or 2 by the sign of the
// stored ghost time; +0xDC is Create's ghost flag (RegisterVariables).
#ifndef JETSKIVCRCONTROLLER_JETSKIVCRCONTROLLER_H
#define JETSKIVCRCONTROLLER_JETSKIVCRCONTROLLER_H

#include "SimpleVCR/SimpleVCR.h"

class JetskiVehicle;

class JetskiVCRController : public SimpleVCR {
public:
    JetskiVCRController(int enabled);
    SimpleVCR *Create(int a, int b, int c, int ghost);
    int LoadFromDisk(char *filename);
    int LoadGhostFromDisk(char *name, float *raceTime);
    void RegisterVariables(JetskiVehicle *vehicle);
    void REC();
    void REPLAYREC();
    void RESET();
    void REWforPLAY();
    void STOP();
    int TestEOT();

    // --- virtuals in retail slot order ---
    virtual ~JetskiVCRController();

    int m_xD0;                          // +0xD0
    int m_loaded;                       // +0xD4
    int m_ghostDirection;               // +0xD8
    int m_ghost;                        // +0xDC
};

#endif
