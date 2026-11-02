// JetskiEdgeMonsterSoundEmitter: the edge monster's roar streams. A
// GameObject, 0x60 bytes (data ends at +0x5C). Names come from the class's
// own methods.
#ifndef JETSKIEDGEMONSTERSOUNDEMITTER_JETSKIEDGEMONSTERSOUNDEMITTER_H
#define JETSKIEDGEMONSTERSOUNDEMITTER_JETSKIEDGEMONSTERSOUNDEMITTER_H

#include "GameObject/GameObject.h"
// PSXSoundInterface.h brings SoundInterface.h's ContainerList (inline
// destructor, no AddItem/GetItem/...), so m_streams has that type; the
// files that call the list's methods keep their own copy of the class.
#include "PSXSoundInterface/PSXSoundInterface.h"

struct Rider_Stream_Info;
struct VehicleAudType;

class JetskiEdgeMonsterSoundEmitter : public GameObject {
public:
    JetskiEdgeMonsterSoundEmitter(PSXSoundInterface *sound, unsigned int soundGroup, int enabled);
    JetskiEdgeMonsterSoundEmitter *Create(VehicleAudType *audType);
    int LoadSounds(VehicleAudType *audType);
    int UnloadSounds();
    int PlayEdgeMonsterSounds(int sound, float speed);
    int SwitchSounds(Rider_Stream_Info *info, float volume);
    int Stop();
    int IsPlaying();

    // --- virtuals in retail slot order ---
    virtual ~JetskiEdgeMonsterSoundEmitter();
    virtual int Tick(float dt);

    int m_voice;                        // +0x30
    int m_playing;                      // +0x34
    unsigned int m_soundGroup;          // +0x38
    unsigned int m_x3C;                 // +0x3C
    float m_x40;                        // +0x40, time since the sound started (Tick)
    PSXSoundInterface *m_soundSystem;   // +0x44
    ContainerList<Rider_Stream_Info *> m_streams;   // +0x48
    int m_currentSound;                 // +0x5C
};

#endif
