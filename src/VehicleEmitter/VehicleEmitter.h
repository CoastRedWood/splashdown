// VehicleEmitter: a SoundEmitter with a list of PSXSounds (the base of the
// jetski engine/spray/splash/terrain emitters). Its data starts at
// SoundEmitter's 0x200: the sound list (+0x200) and +0x214; 0x218 bytes.
//
// ContainerList: VehicleEmitter's ctor/dtor need its destructor inline
// (`delete[] m_items`), which ContainerList/ContainerList.h deliberately leaves
// out. So this header carries the same template plus that dtor, under
// ContainerList.h's own guard: whichever is included first is used, and the
// two never clash. (Include this header first where the dtor matters.)
#ifndef VEHICLEEMITTER_VEHICLEEMITTER_H
#define VEHICLEEMITTER_VEHICLEEMITTER_H

#include "SoundEmitter/SoundEmitter.h"

class AuralScape;
class PSXSoundGroup;
class PSXSound;
struct _DS3DBUFFER;

#include "ContainerList/ContainerList.h"

class VehicleEmitter : public SoundEmitter {
public:
    VehicleEmitter(AuralScape *scape, PSXSoundGroup *group, short type, int flags);
    VehicleEmitter *Create(_DS3DBUFFER buffer, unsigned int flags, float a, float b, int c, int d);

    // --- virtuals in retail slot order ---
    virtual ~VehicleEmitter();
    virtual int Start();
    virtual int UpdateSettings(float volume, long frequency, _D3DVECTOR position, int flags);

    ContainerList<PSXSound *> m_sounds;     // +0x200
    int m_x214;                             // +0x214
};

#endif
