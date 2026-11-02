// SceneFog: a scene's [Fog] block (Scene::m_fog, 0x0C bytes; Scene::ReadFogData
// fills it, JetskiCompetition::LoadFog reads it).
#ifndef SCENEFOG_SCENEFOG_H
#define SCENEFOG_SCENEFOG_H

struct SceneFog {
    unsigned int m_color;               // +0x0, 0xRRGGBB
    float m_visibility;                 // +0x4
    float m_haziness;                   // +0x8
};

#endif
