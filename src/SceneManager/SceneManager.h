// SceneManager: the current track's scene and resource file names (no
// vtable; JetskiGame::m_sceneManager). 0x2CC bytes (`new` in
// JetskiGame::CreateScene). +0x40 is a path buffer (GetSceneDescriptor's copy
// declares it 0x110 long, which would overlap +0x144). The +0x144.. file
// names are what Scene (its m_info) reads and writes back.
#ifndef SCENEMANAGER_SCENEMANAGER_H
#define SCENEMANAGER_SCENEMANAGER_H

class SceneManager {
public:
    SceneManager();
    ~SceneManager();
    void GetSceneDescriptor(char *descriptor, char *sceneFile, int waypointSet);
    void SetCurrentSceneFile(char *name, char *extension, int waypointSet);
    void SetCurrentTerrainFile(char *name);

    char m_sceneName[0x40];             // +0x000
    char m_scenePath[0x104];            // +0x040
    char m_sceneFile[0x40];             // +0x144
    char m_estFile[0x40];               // +0x184
    char m_terrainFile[0x40];           // +0x1C4
    char m_cubeFile[0x40];              // +0x204, Scene::ReadEnvironmentData's cube file (empty here)
    char m_waypointFile[0x40];          // +0x244
    char m_wttFile[0x40];               // +0x284
    int m_sceneValid;                   // +0x2C4 (SetCurrentSceneFile)
    int m_numWaypointRaces;             // +0x2C8
};

#endif
