// SceneManagerData: a scene's [Environment] block (Scene::m_manager, 0x53C
// bytes, `new` in Scene::ReadEnvironmentData): the terrain, ecosystem, water
// and cube files, terrain metrics and the per-zone surface properties.
// Names from ReadEnvironmentData's parameter names. ReadSceneInfo reads the
// terrain metrics as the visibility quadtree's extent.
#ifndef SCENEMANAGERDATA_SCENEMANAGERDATA_H
#define SCENEMANAGERDATA_SCENEMANAGERDATA_H

struct SceneManagerData {               // 0x53C bytes
    char m_ecosystemFile[0x104];        // +0x000
    char m_terrainFile[0x104];          // +0x104
    char m_waterFile[0x104];            // +0x208
    char m_cubeFile[0x104];             // +0x30C
    char m_particleTexture[0x40];       // +0x410
    char m_detailTexture[0x40];         // +0x450
    float m_terrainWidthScale;          // +0x490
    float m_terrainBreadthScale;        // +0x494
    float m_terrainWidth;               // +0x498
    float m_terrainBreadth;             // +0x49C
    float m_terrainBorderWidth;         // +0x4A0
    float m_posXEdgeDist;               // +0x4A4
    float m_negXEdgeDist;               // +0x4A8
    float m_posZEdgeDist;               // +0x4AC
    float m_negZEdgeDist;               // +0x4B0
    float m_startGateScale;             // +0x4B4
    float m_friction[8];                // +0x4B8, per terrain zone
    float m_drag[8];                    // +0x4D8
    float m_traction[8];                // +0x4F8
    unsigned char m_dust[8];            // +0x518
    unsigned char m_dirt[8];            // +0x520
    int m_reverb[4];                    // +0x528, ReverbZoneN's EnviroID
    unsigned char m_lowRes;             // +0x538
    unsigned char m_placeEco;           // +0x539
    char pad53A[0x53C - 0x53A];
};

#endif
