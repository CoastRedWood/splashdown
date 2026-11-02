// PSXGame: the PS2 platform layer over Game. It adds no data (0x274 bytes,
// like Game) and implements Game's pure virtuals: render states, video
// modes and the registry stand-ins.
#ifndef PSXGAME_PSXGAME_H
#define PSXGAME_PSXGAME_H

#include "Game/Game.h"

// The full PSXRenderTarget: CreateRenderTarget() is a covariant override of
// Game's pure virtual, which mwcc only sees with the class complete.
#include "PSXRenderTarget/PSXRenderTarget.h"

class PSXGame : public Game {
public:
    PSXGame();
    int Initialize(char *errorMessage);
    int GetDisplayModes();

    virtual ~PSXGame();
    virtual int PreGameInitialize();
    virtual int SetBasicRenderStates();
    virtual int PrepareFrame();
    virtual int KeyDownHandler(ProcessNode *node, KeyInfo *key);
    virtual int ChangeVideoMode(int mode);
    virtual int GetRegistryInt(const char *name, int def);
    virtual float GetRegistryFloat(const char *name, float def);
    virtual int GetRegistryBOOL(const char *name, int def);
    virtual int GetRegistryString(const char *name, const char *def, char *out, unsigned int *size);
    virtual int GetRegistryData(const char *name, void *out, unsigned int *size);
    virtual int SetRegistryInt(const char *name, int value);
    virtual int SetRegistryFloat(const char *name, float value);
    virtual int SetRegistryBOOL(const char *name, int value);
    virtual int SetRegistryString(const char *name, const char *value);
    virtual int SetRegistryData(const char *name, void *data, int size);
    virtual PSXRenderTarget *CreateRenderTarget();
    virtual int SetDefaultVideoMode();
    virtual int FilterDisplayModes(PSXVideoCard *card);
};

#endif
