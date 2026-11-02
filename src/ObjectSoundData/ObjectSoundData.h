// ObjectSoundData: one [DynamicSoundN] "ObjectSound" of a scene (0x198 bytes,
// `new` in Scene::ReadSoundData): the emitter settings and the names of the
// static models that get it (Scene::LoadStaticModels). Scene::m_objectSounds
// holds them; Scene::RemoveAllObjectSounds deletes them.
#ifndef OBJECTSOUNDDATA_OBJECTSOUNDDATA_H
#define OBJECTSOUNDDATA_OBJECTSOUNDDATA_H

#include "_DS3DBUFFER/_DS3DBUFFER.h"
#include "ContainerList/ContainerList.h"

struct ObjectSoundData {
    _DS3DBUFFER m_buffer;               // +0x00
    unsigned int m_flags;               // +0x70
    float m_oneShotDistance;            // +0x74
    float m_randomTriggerPercent;       // +0x78
    int m_force2D;                      // +0x7C
    char m_name[0x104];                 // +0x80
    ContainerList<char *> m_objects;    // +0x184, model names (malloc'd)
};

#endif
