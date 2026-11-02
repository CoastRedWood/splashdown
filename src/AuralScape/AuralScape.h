// AuralScape: the sound manager GameObject (LpGame->m_auralScape). Holds
// the listeners, three emitter lists and the PSXSoundInterface. 0xA8 bytes
// (8-aligned GameObject; data ends at +0xA4). Names come from the class's
// own methods.
#ifndef AURALSCAPE_AURALSCAPE_H
#define AURALSCAPE_AURALSCAPE_H

#include "GameObject/GameObject.h"

struct _D3DVECTOR;
struct _DS3DBUFFER;
class PSXSound;
class PSXSoundGroup;
class PSXSoundInterface;
class SoultreeObject;
class SoultreeSoundEmitter;
class SoundEmitter;
class SoundListener;

#define MAX_NUM_LISTENERS 4

class AuralScape : public GameObject {
public:
    AuralScape(int id);
    AuralScape *Create(int x38, unsigned int flags);
    int AddListener(int listenerID, float a, float b);
    int RemoveListener(int index);
    void UpdateListener(int listener, _D3DVECTOR *position, _D3DVECTOR *front,
                        _D3DVECTOR *top, _D3DVECTOR *velocity);
    SoundEmitter *CreateSoundEmitter(PSXSoundGroup *group, char *sound, _DS3DBUFFER buffer, int enabled,
                                     short type, unsigned int flags, float a, float b, int c, int d);
    SoultreeSoundEmitter *CreateSoultreeEmitter(char *name, SoultreeObject *object, PSXSoundGroup *group,
                                                char *sound, _DS3DBUFFER buffer, int enabled, short type,
                                                unsigned int flags, float a, float b, int c, int d);
    int AddEmitter(SoundEmitter *emitter);
    int RemoveEmitter(SoundEmitter *emitter);
    int DestroySoundEmitter(SoundEmitter *emitter);
    int DestroyAllSoundEmitters();
    int AttenuateSound(PSXSound *sound, _D3DVECTOR *position);
    void StopAllAuralScapeSounds();
    int CreateMusic(int a, int b, int c);
    void Evaluate();
    void SetDolby(int on);
    void SetMono(int on);

    // --- virtuals in retail slot order ---
    virtual ~AuralScape();
    virtual int Tick(float dt);
    virtual int KeyDownHandler(ProcessNode *node, KeyInfo *key);

    int m_x30;                                  // +0x30
    int m_x34;                                  // +0x34
    int m_x38;                                  // +0x38
    int m_num3DVoices;                          // +0x3C
    int m_x40;                                  // +0x40
    int m_x44;                                  // +0x44
    int m_numListeners;                         // +0x48
    SoundListener *m_listeners[MAX_NUM_LISTENERS];  // +0x4C
    // Three ContainerList<SoundEmitter *>. Typed only when the file has
    // included ContainerList/ContainerList.h first: most users carry their
    // own ContainerList template copy, so otherwise they are bytes.
#ifdef CONTAINERLIST_H
    ContainerList<SoundEmitter *> m_emitters;   // +0x5C
    ContainerList<SoundEmitter *> m_audible;    // +0x70
    ContainerList<SoundEmitter *> m_playing;    // +0x84
#else
    char m_lists[0x98 - 0x5C];
#endif
    int m_debugPage;                            // +0x98
    unsigned char m_x9C_bit0 : 1;               // +0x9C
    PSXSoundInterface *m_soundInterface;        // +0xA0
};

#endif
