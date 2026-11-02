// Game.cpp  @ 0x002005F0 (text 0x2005F0-0x201DE0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// A retail translation unit without a static initializer: Game, then
// GameObject, the weak BaseObject::GetRefCount, do_global_dtors and mwInit. Its
// .rodata labels run from @548 (after MemoryPoolManager.cpp's @621) through the
// Game RTTI (@996) and the GameObject/BaseObject RTTI (@1119-@1121);
// SoundGroup.cpp's restart at @574.


#include "DataAccess/DataAccess.h"
#include "Game/Game.h"
#include "PSXControlInterface/PSXControlInterface.h"
#include "GameObject/GameObject.h"
#include "AuralScape/AuralScape.h"
#include "LightManager/LightManager.h"
#include "PSXTextureMapManager/PSXTextureMapManager.h"
#include "RenderTarget/RenderTarget.h"
#include "PSXRenderTarget/PSXRenderTarget.h"
#include "PSXVideoCard/PSXVideoCard.h"
#include "Camera/Camera.h"
#include "DebugOverlay/DebugOverlay.h"
#include "ControlInterface/ControlInterface.h"
#include "PSXSoundInterface/PSXSoundInterface.h"
#include "GameObject/GameObjectGlobals.h"
#include "BaseObject/BaseObject.h"
#include "MemoryPoolManager/MemoryPoolManager.h"
#include "ControlInterface/ControlTypes.h"

// Defined here for its retail .bss slot.
static int ForceDeltaSecondsTo30Hz;
FILE_HANDLE_STRUCT *GameLogFile;


// ---- src/Game/ctor.cpp
extern "C" char *strcpy(char *dst, const char *src);

struct FILE_HANDLE_STRUCT;
extern FILE_HANDLE_STRUCT *GameLogFile;

class ProcessNode;
class KeyInfo;
class Camera;

extern DataAccess *LpDataAccess;

class RenderTarget;
extern RenderTarget *LpRenderTarget;
class VideoCard;

void FastMathInitialize();

// Obfuscated registry key names (XOR 0x5B).
extern char baseKeyName[];
extern char subKeyName[];


Game::Game()
{
    LpDataAccess = new DataAccess;
    FastMathInitialize();
    m_auralScape = 0;
    LpRenderTarget = 0;
    LpVideoCard = 0;
    m_controls = 0;
    m_root = 0;
    m_scene = 0;
    m_x08 = 1;
    m_viewport = 0;
    m_x228_bit6 = 1;
    m_flag1 = 1;
    m_debugKeys = 0;
    m_debugOverlay = 0;
    GameLogFile = 0;
    strcpy(m_name, "");
    m_inputLocked = 0;
    m_terminating = 0;
    m_frameTime = 0;
    m_lastTime = 0;
    m_x240 = 1.0f;
    m_paused = 0;
    m_x120 = 0;
    m_textureCache = 1;
    SetCameras(0, 0, 0, 0);
    m_numCameras = 1;
    m_currentPlayer = 0;
    m_x244 = 1.0f;
    m_x248 = 0.0f;
    m_x24C = 0.05f;
    m_x250 = 0.0f;
    m_simulationStep = 0;
    for (char *p = baseKeyName; *p; p++)
        *p ^= 0x5B;
    for (char *p = subKeyName; *p; p++)
        *p ^= 0x5B;
}


// ---- src/Game/PreWindowInitialize.cpp

int Game::PreWindowInitialize()
{
    return 1;
}


// ---- src/Game/PreGameInitialize.cpp


int Game::PreGameInitialize()
{
    m_controls = new PSXControlInterface;
    if (!((PSXControlInterface *)m_controls)->Create(4))
        return 0;
    return 1;
}


// ---- src/Game/Initialize.cpp
// Compiler session state: an unused
// helper that recreates the mwcc session retail compiled this file in; it is
// not part of the retail file.
static void MWCC_SHIM_TEXT MwccSessionState(long *a) { a[0] = a[0] / a[1]; }

extern "C" void srand(unsigned int seed);
extern "C" int rand(void);

extern int TimerOverflowCounterT0;

inline unsigned int timeGetTime()
{
    static long Time = 0;
    static int lastCount = 0;
    int count, elapsed, overflows;
    count = *(volatile int *)0x10000000;
    overflows = TimerOverflowCounterT0;
    TimerOverflowCounterT0 = 0;
    if (lastCount <= count)
        elapsed = count - lastCount;
    else
        elapsed = 0x10000 - lastCount + count;
    if (lastCount >= count && overflows > 0)
        overflows--;
    lastCount = count;
    Time += elapsed + (overflows << 16);
    return 0.0017066667f * Time;
}


extern PSXTextureMapManager *LpTextureMapManager;



int Game::Initialize(char *cmdLine)
{
    m_scene = new GameObject(1);
    m_root = new GameObject(1);
    m_scene->AddChild(m_root, -1);
    m_auralScape = new AuralScape(0);
    m_auralScape->Create(0, 6);
    m_auralScape->m_x9C_bit0 = 0;
    srand(timeGetTime());
    int n = (unsigned char)timeGetTime();     // warm up rand() 1..256 times
    do {
        rand();
    } while (n--);
    m_debugKeys = 0;
    if (!SetDefaultVideoMode())
        return 0;
    LpTextureMapManager = new PSXTextureMapManager;
    LpTextureMapManager->Create();
    m_root->AddChild(LpTextureMapManager, -1);
    m_root->AddChild(new LightManager, 0x402);
    m_flag0 = 1;
    if (!LoadResources())
        return 0;
    if (!CreateScene())
        return 0;
    m_root->AddChild(m_auralScape, -1);
    return 1;
}


// ---- src/Game/CreateScene.cpp

int Game::CreateScene()
{
    return 1;
}


// ---- src/Game/LoadResources.cpp

int Game::LoadResources()
{
    return 1;
}


// ---- src/Game/Activate.cpp

int Game::Activate()
{
    return 1;
}


// ---- src/Game/Deactivate.cpp

int Game::Deactivate()
{
    return 1;
}


// ---- src/Game/PostVideoModeChange.cpp
class RenderTarget;
class Camera;
class KeyInfo;
class ProcessNode;
extern RenderTarget *LpRenderTarget;

// Slots per retail's vtable; unnamed slots are placeholders.


int Game::PostVideoModeChange()
{
    LpRenderTarget = CreateRenderTarget();
    // Game.h declares it int (BOOL): PSXGame::SetDefaultVideoMode tests the
    // int result. This form gives the bool-style xor/sltiu/xori.
    if (LpRenderTarget == 0)
        return 0;
    return 1;
}


// ---- src/Game/SetCameras.cpp
class Camera;


int Game::SetCameras(Camera *a, Camera *b, Camera *c, Camera *d)
{
    m_cameras[0] = a;
    m_cameras[1] = b;
    m_cameras[2] = c;
    m_cameras[3] = d;
    m_numCameras = 0;
    for (int i = 0; i < 4; i++) {
        if (m_cameras[i])
            m_numCameras++;
    }
    return 1;
}


// ---- src/Game/GetCameras.cpp

void Game::GetCameras(Camera **cameras)
{
    cameras[0] = m_cameras[0];
    cameras[1] = m_cameras[1];
    cameras[2] = m_cameras[2];
    cameras[3] = m_cameras[3];
}


// ---- src/Game/PrepareFrame.cpp

extern "C" int strcmp(const char *a, const char *b);
void PSXprintf(const char *fmt, ...);

extern int TimerOverflowCounterT0;




static int OverlayId = -1;
static int MemOverlayId = -1;

// The registry key names, XORed with 0x5B (the ctor decodes them in place):
// "SOFTWARE\\Rainbow Studios\\" and "TestKey".
char baseKeyName[] = {0x08, 0x14, 0x1D, 0x0F, 0x0C, 0x1A, 0x09, 0x1E, 0x07, 0x09, 0x3A, 0x32, 0x35, 0x39, 0x34, 0x2C, 0x7B, 0x08, 0x2F, 0x2E, 0x3F, 0x32, 0x34, 0x28, 0x07, 0x00};
char subKeyName[] = {0x0F, 0x3E, 0x28, 0x2F, 0x10, 0x3E, 0x22, 0x00};



static inline int GetDebugPage(DebugOverlay *o) { return o->m_numPages++; }

int Game::PrepareFrame()
{
    static int PrepareGeometryID = -1;  // unused profiler IDs
    static int BeginSceneID = -1;
    static int EndSceneID = -1;
    static int ClearID = -1;
    static int RenderPre3DID = -1;
    static int Render3DID = -1;
    static int RenderPost3DID = -1;
    static int WaitForGPUID = -1;
    static unsigned int baseTime = 0;
    static unsigned int lastTime = 0;
    static unsigned int lastFrame = 0;
    static int FrameRendered = 0;
    int n;
    int total;
    int i;
    int debugSize;

    for (i = 0; i < 4; i++)
        if (m_cameras[i])
            m_cameras[i]->Disable();
    for (m_viewport = 0; m_viewport < 4; m_viewport++) {
        if (m_cameras[m_viewport]) {
            m_currentPlayer = m_viewport;
            if (m_viewport > 0)
                m_cameras[m_viewport - 1]->Disable();
            LpRenderTarget->SetCamera(m_cameras[m_viewport]);
            m_cameras[m_viewport]->Enable();
            if (((PSXRenderTarget *)LpRenderTarget)->IsRenderingComplete() == 1)
                LpRenderTarget->TrySendRenderQueue(1);
            m_renderPhase = 2;
            m_scene->PrepareGeometry();
            m_renderPhase = 0;
            LpRenderTarget->WaitForRenderingToComplete();
            LpRenderTarget->TrySendRenderQueue(1);
            LpRenderTarget->WaitForRenderingToComplete();
            if (m_viewport == 0 && FrameRendered) {
                if (m_debugOverlay)
                    m_debugOverlay->RenderDelayed();
                LpVideoCard->DisplayToScreen();
                unsigned int now = timeGetTime();
                unsigned int elapsed = now - lastTime;
                if (elapsed > 1000) {
                    unsigned int frame = LpVideoCard->m_frameCount;
                    m_fps = 1000.0f * (frame - lastFrame) / elapsed;
                    lastFrame = frame;
                    lastTime = now;
                    PSXprintf("%f FPS\n", m_fps);
                }
            }
            LpRenderTarget->m_camera->SetUpViewport();
            m_renderPhase = 3;
            m_scene->RenderPre3D();
            m_renderPhase = 0;
            if (!LpRenderTarget->BeginScene())
                goto fail;
            SetBasicRenderStates();
            m_renderPhase = 4;
            m_scene->Render3D();
            m_renderPhase = 0;
            FrameRendered = 1;
            if (!LpRenderTarget->EndScene())
                goto fail;
        }
    }
    for (i = 0; i < 4; i++)
        if (m_cameras[i])
            m_cameras[i]->Enable();
    m_renderPhase = 5;
    m_scene->RenderPost3D();
    m_renderPhase = 0;
    if (m_debugKeys) {
        if (m_debugOverlay) {
            if (m_debugOverlay->m_enabled && OverlayId == m_debugOverlay->m_page) {
                PSXVideoCard *card = LpVideoCard;
                // The cast (VideoCard::m_depth was probably unsigned) puts its load first.
                m_debugOverlay->PrintfHeading(OverlayId, "%c %d x %d %d bit(x%d)", 'R', card->m_width,
                                              card->m_height, (unsigned int)card->m_depth, card->m_x14);
                m_debugOverlay->PrintfDetail(OverlayId, "FPS:   %2.1f", m_fps);
            }
            if (MemOverlayId < 0)
                MemOverlayId = GetDebugPage(m_debugOverlay);
            if (m_debugOverlay->m_enabled && MemOverlayId == m_debugOverlay->m_page) {
                m_debugOverlay->PrintfHeading(MemOverlayId, "Memory");
                m_debugOverlay->PrintfDetail(MemOverlayId, "  Size       Area");
                total = 0;
                debugSize = 0;
                n = LpMemoryPoolManager->m_numPools;
                for (i = 0; i < n; i++) {
                    m_debugOverlay->PrintfDetail(MemOverlayId, "%8d %12s", LpMemoryPoolManager->m_sizes[i],
                                                 LpMemoryPoolManager->m_names[i]);
                    total += LpMemoryPoolManager->m_sizes[i];
                    if (!strcmp(LpMemoryPoolManager->m_names[i], "DebugOverlay"))
                        debugSize = LpMemoryPoolManager->m_sizes[i];
                }
                m_debugOverlay->PrintfDetail(MemOverlayId, "Total - DebugOverlay %08d", total - debugSize);
            }
        }
    }
    return 1;
fail:
    return 0;
}


// ---- src/Game/UpdateScreen.cpp


extern PSXVideoCard *LpVideoCard;


int Game::UpdateScreen()
{
    LpRenderTarget->WaitForRenderingToComplete();
    LpVideoCard->DisplayToScreen();
    return 1;
}


// ---- src/Game/Tick.cpp

extern int TimerOverflowCounterT0;



int Game::Tick()
{
    static int NetID = -1;              // unused profiler IDs
    static int TickID = -1;
    if (!m_frameTime)
        ResetDeltaTimer();
    else
        m_lastTime = timeGetTime();
    float dt = 0.001f * (m_lastTime - m_frameTime);
    m_x250 = dt;
    if (dt > 0.06f)
        dt = 0.06f;
    if (ForceDeltaSecondsTo30Hz && dt > 0.03333f)
        dt = 0.03333f;
    if (m_controls)
        m_controls->Update(dt);
    if (!m_inputLocked) {
        if (((PSXRenderTarget *)LpRenderTarget)->IsRenderingComplete() == 1)
            LpRenderTarget->TrySendRenderQueue(1);
        m_renderPhase = 1;
        m_simulationStep = dt;
        m_scene->Tick(dt);
        m_renderPhase = 0;
        PrepareFrame();
        LpRenderTarget->NextCyclingBufferId();
    }
    m_auralScape->m_soundInterface->m_snd.SendBatch();
    m_frameTime = m_lastTime;
    if (m_x120) {
        m_paused = 1;
        m_x120 = 0;
    }
    return 1;
}


// ---- src/Game/ResetDeltaTimer.cpp

extern int TimerOverflowCounterT0;

// Win32-style timeGetTime: EE timer 0 (T0_COUNT at 0x10000000) ticks plus the
// overflows counted by TimerHandlerT0, accumulated and scaled to milliseconds.


void Game::ResetDeltaTimer()
{
    m_lastTime = m_frameTime = timeGetTime();
}


// ---- src/Game/PauseGame_00201960.cpp

int Game::PauseGame(int pause)
{
    return 0;
}


// ---- src/Game/EventHandler.cpp


int Game::EventHandler(int event)
{
    return m_scene->EventHandler(event) != 0;
}


// ---- src/Game/KeyUpHandler.cpp
class KeyInfo;
class ProcessNode;

// GameObject's vtable up to KeyUpHandler (slots per the retail vtable).


int Game::KeyUpHandler(ProcessNode *node, KeyInfo *key)
{
    if (m_inputLocked)
        return 1;
    return m_scene->KeyUpHandler(node, key) != 0;
}


// ---- src/Game/KeyDownHandler.cpp
class ProcessNode;
class KeyInfo;

int KEYDOWN_HANDLER_TEST(unsigned int key, DeviceType device, ProcessNode *node, unsigned int mods);
void MemPoolReportToFile();




int Game::KeyDownHandler(ProcessNode *node, KeyInfo *key)
{
    if (m_inputLocked)
        return 1;
    if (m_debugKeys) {
        if (KEYDOWN_HANDLER_TEST(0x10, DEVICE_KEYBOARD, node, 0x80))
            MemPoolReportToFile();
        if (KEYDOWN_HANDLER_TEST(0x3C, DEVICE_KEYBOARD, node, 0x80))
            ForceDeltaSecondsTo30Hz = 1 - ForceDeltaSecondsTo30Hz;
    }
    return m_scene->KeyDownHandler(node, key) != 0;
}


// ---- src/Game/dtor.cpp
struct FILE_HANDLE_STRUCT;
void FCLOSE(FILE_HANDLE_STRUCT *file);

extern FILE_HANDLE_STRUCT *GameLogFile;


extern RenderTarget *LpRenderTarget;




extern DataAccess *LpDataAccess;


Game::~Game()
{
    if (LpRenderTarget) {
        delete LpRenderTarget;
        LpRenderTarget = 0;
    }
    if (LpVideoCard) {
        LpVideoCard->Release();
        delete LpVideoCard;
        LpVideoCard = 0;
    }
    if (GameLogFile)
        FCLOSE(GameLogFile);
    LpDataAccess->Release();
}


// ---- src/Game/Terminate.cpp
void PSXprintf(const char *fmt, ...);

extern RenderTarget *LpRenderTarget;





class HiResMeter : public GameObject {
public:
    virtual ~HiResMeter();
    unsigned char m_inChain;        // +0x30
};
extern HiResMeter *LpHiResMeter;



int Game::Terminate()
{
    PSXprintf("Game terminating...\n");
    if (LpRenderTarget) {
        delete LpRenderTarget;
        LpRenderTarget = 0;
    }
    if (LpVideoCard) {
        LpVideoCard->Release();
        delete LpVideoCard;
        LpVideoCard = 0;
    }
    m_terminating = 1;
    if (LpHiResMeter && m_debugOverlay && LpHiResMeter->m_inChain)
        LpHiResMeter->RemoveFromChain();
    if (m_scene) {
        m_scene->Release();
        m_scene = 0;
    }
    if (m_controls) {
        delete m_controls;
        m_controls = 0;
    }
    return 1;
}


// ---- src/Game/StartNetInterface.cpp


int Game::StartNetInterface(NET_MODE_TYPE)
{
    return 0;
}


// ---- src/Game/PacketHandler.cpp
// GameObject's vtable up to PacketHandler (slots per the retail vtable), declared
// locally because GameObject.h gives PacketHandler a void return.


int Game::PacketHandler(unsigned int type, void *data, unsigned int a, unsigned int b, unsigned int c)
{
    return m_scene->PacketHandler(type, data, a, b, c) != 0;
}


// ---- src/Game/GetFullPathToContentFile.cpp
extern "C" char *strcat(char *dst, const char *src);

int Game::GetFullPathToContentFile(char *name, char *path)
{
    strcpy(path, m_contentPath);
    strcat(path, "\\");
    strcat(path, name);
    return 1;
}


// ---- src/Game/GetResourceString.cpp

int Game::GetResourceString(int id, char *out)
{
    strcpy(out, "No Strings Available");
    return 0;
}
