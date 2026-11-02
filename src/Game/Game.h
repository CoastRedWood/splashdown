// Game: the Soultree application object. PSXGame derives from it and adds no
// data; JetskiGame (the global LpGame) adds members from +0x274.
//
// The data comes first, so the vptr sits after it at +0x270 and Game is
// 0x274 bytes. Member names follow Game's own methods (src/Game/) where they
// use the member, otherwise the most common name among the files that
// declared Game locally (mwcc's layout Game).
#ifndef GAME_GAME_H
#define GAME_GAME_H

class AuralScape;
class Camera;
class ControlInterface;
class DebugOverlay;
class GameObject;
class KeyInfo;
class ProcessNode;
class PSXVideoCard;
class RenderTarget;
enum NET_MODE_TYPE {};

class Game {
public:
    Game();
    void ResetDeltaTimer();
    int Initialize(char *cmdLine);

    AuralScape *m_auralScape;           // +0x000
    ControlInterface *m_controls;       // +0x004
    int m_x08;                          // +0x008
    int m_viewport;                     // +0x00C
    GameObject *m_root;                 // +0x010
    DebugOverlay *m_debugOverlay;       // +0x014, also tested as a show-meter flag
    char m_name[0x104];                 // +0x018
    int m_paused;                       // +0x11C
    int m_x120;                         // +0x120
    char m_contentPath[0x228 - 0x124];  // +0x124, GetFullPathToContentFile's directory
    unsigned char m_flag0 : 1;          // +0x228 bit 0
    unsigned char m_flag1 : 1;          //        bit 1
    unsigned char m_debugKeys : 1;      //        bit 2
    unsigned char m_inputLocked : 1;    //        bit 3
    unsigned char m_terminating : 1;    //        bit 4
    unsigned char m_textureCache : 1;   //        bit 5
    unsigned char m_x228_bit6 : 1;      //        bit 6
    char pad229[0x22C - 0x229];
    int m_renderPhase;                  // +0x22C
    unsigned int m_lastTime;            // +0x230
    unsigned int m_frameTime;           // +0x234
    float m_fps;                        // +0x238, frames per second (PrepareFrame)
    float m_simulationStep;             // +0x23C
    float m_x240;                       // +0x240
    float m_x244;                       // +0x244
    float m_x248;                       // +0x248
    float m_x24C;                       // +0x24C
    float m_x250;                       // +0x250
    Camera *m_cameras[4];               // +0x254 (SetCameras/GetCameras)
    int m_numCameras;                   // +0x264
    int m_currentPlayer;                // +0x268
    GameObject *m_scene;                // +0x26C, receives events, keys and packets

    // --- virtuals in retail slot order (retail vtable); vptr at +0x270.
    // The pure ones are PSXGame's (retail vtable). ---
    virtual ~Game();
    virtual int PreWindowInitialize();
    virtual int PreGameInitialize();
    virtual int LoadResources();
    virtual int CreateScene();
    virtual int Activate();
    virtual int Deactivate();
    virtual int SetBasicRenderStates() = 0;
    virtual int PrepareFrame();
    virtual int UpdateScreen();
    virtual int Tick();
    virtual int PauseGame(int pause);
    virtual int EventHandler(int event);
    virtual int KeyUpHandler(ProcessNode *node, KeyInfo *key);
    virtual int KeyDownHandler(ProcessNode *node, KeyInfo *key);
    virtual int SetCameras(Camera *a, Camera *b, Camera *c, Camera *d);
    virtual void GetCameras(Camera **cameras);
    virtual int Terminate();
    virtual int StartNetInterface(NET_MODE_TYPE mode);
    virtual int PacketHandler(unsigned int type, void *data, unsigned int a, unsigned int b, unsigned int c);
    virtual int GetFullPathToContentFile(char *name, char *path);
    virtual int ChangeVideoMode(int mode) = 0;
    virtual int GetRegistryInt(const char *name, int def) = 0;
    virtual float GetRegistryFloat(const char *name, float def) = 0;
    virtual int GetRegistryBOOL(const char *name, int def) = 0;
    virtual int GetRegistryString(const char *name, const char *def, char *out, unsigned int *size) = 0;
    virtual int GetRegistryData(const char *name, void *out, unsigned int *size) = 0;
    virtual int SetRegistryInt(const char *name, int value) = 0;
    virtual int SetRegistryFloat(const char *name, float value) = 0;
    virtual int SetRegistryBOOL(const char *name, int value) = 0;
    virtual int SetRegistryString(const char *name, const char *value) = 0;
    virtual int SetRegistryData(const char *name, void *data, int size) = 0;
    virtual int GetResourceString(int id, char *out);
    virtual RenderTarget *CreateRenderTarget() = 0;
    virtual int SetDefaultVideoMode() = 0;
    virtual int PostVideoModeChange();  // BOOL: callers test the int (PSXGame::SetDefaultVideoMode)
    virtual int FilterDisplayModes(PSXVideoCard *card) = 0;
};

#endif
